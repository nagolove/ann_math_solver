#include <assert.h>
#include <stddef.h>
#include <raylib.h>
#include "raymath.h"

#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "mathops.h"
#include "genann_view.h"
#include "genann.h"
#include "xor.h"
#include "logger.h"

#define MAX_VIEVERS_NUM 4

static genann_view *viewers[MAX_VIEVERS_NUM] = {0};
static int viewers_num = 0;

struct AnnHolder;

typedef const char *(*AnnRun)(struct AnnHolder *ah, float a, float b);

struct AnnHolder {
    genann  *ann;
    char    name[40];
    AnnRun  run;
};

static struct AnnHolder holders[MAX_VIEVERS_NUM] = {0};

//static genann *ann_xor = NULL;
//static genann *ann_div = NULL;
//static genann *ann_test = NULL;

static const int screen_width = 1920;
static const int screen_height = 1080;

static Camera2D camera = { 0 };

void holders_shutdown() {
    for (int i = 0; i < viewers_num; ++i) {
        genann_free(holders[i].ann);
    }
}

struct AnnHolder *holders_search(const char *name) {
    assert(name);
    for (int i = 0; i < viewers_num; ++i) {
        if (!strcmp(holders[i].name, name)) {
            return &holders[i];
        }
    }
    return NULL;
}

Vector2 place_center(const char *text, int fontsize) {
    float width = MeasureText(text, fontsize);
    return (Vector2) {
        .x = (screen_width - width) / 2.,
        .y = (screen_height - fontsize) / 2.,
    };
}

void print_inputs(const double *inputs, int inputs_num) {
    printf("print_inputs: ");
    for (int i = 0; i < inputs_num; i++) {
        printf("%f ", inputs[i]);
    }
    printf("\n");
}

/*
void write_normalized_inputs(struct ModelBox *mb, double *inputs) {
    assert(mb);
    assert(inputs);

    int inputs_num = FIELD_SIZE * FIELD_SIZE;
    int idx = 0;
    for (int i = 0; i < FIELD_SIZE; i++)
        for (int j = 0; j < FIELD_SIZE; j++) 
            inputs[idx++] = mb->field[j][i];

    for (int i = 0; i < inputs_num; i++) {
        inputs[i] = inputs[i] / MAX_VALUE;
    }

    //print_inputs(inputs, inputs_num);
}
*/

// Как протестировать функцию?
int out2dir(const double *outputs) {
    assert(outputs);
    double max = 0.;
    int max_index = 0;
    for (int j = 0; j < 4; j++)
        if (outputs[j] > max) {
            max = outputs[j];
            max_index = j;
        }
    return max_index;
}

void ann_shake(genann *net) {
    /* Take a random guess at the ANN weights. */
    for (int i = 0; i < net->total_weights; ++i) {
        net->weight[i] += ((double)rand())/RAND_MAX-0.5;
    }
}

/*
void train() {
    printf("training\n");
    // Стратегия обучения?
    struct Set *s = set_new(10005);
    for (int cycles = 0; cycles < 2500; cycles++) {
        printf("cycles %d\n", cycles);
        set_train(s);
        struct Set *new = set_select(s);
        set_shutdown(s);
        s = new;
    }
    set_write(s);

    genann_print(s->nets[0]);

    set_shutdown(s);
}
*/

void camera_process() {
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
    {
        Vector2 delta = GetMouseDelta();
        delta = Vector2Scale(delta, -1.0f/camera.zoom);

        camera.target = Vector2Add(camera.target, delta);
    }

    // Zoom based on mouse wheel
    float wheel = GetMouseWheelMove();
    if (wheel != 0)
    {
        // Get the world point that is under the mouse
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

        // Set the offset to where the mouse is
        camera.offset = GetMousePosition();

        // Set the target to match, so that the camera maps the world space point 
        // under the cursor to the screen space point under the cursor at any zoom
        camera.target = mouseWorldPos;

        // Zoom increment
        const float zoomIncrement = 0.125f;

        camera.zoom += (wheel*zoomIncrement);
        if (camera.zoom < zoomIncrement) camera.zoom = zoomIncrement;
    }
}

genann_view *printing_test() {
    genann *ann = genann_init(5, 3, 5, 4);
    genann_randomize(ann);
    //genann *ann = genann_init(4, 2, 7, 3);
    /*genann_print(ann);*/
    /*genann_print_run(ann);*/
    genann_view *view = genann_view_new("randomized[5, 3, 5, 4]");
    genann_view_prepare(view, ann);
    genann_free(ann);
    return view;
}

static bool input_mode = true;
static char input[50] = {0};
static char result[50] = {0};

void input_text() {
    Rectangle bounds = {
        100, 400, 400, 80,
    };
    const int font_size = 20;

    DrawText(
        "enter ann name and two arguments",
        bounds.x, bounds.y - font_size,
        font_size,
        BLACK
    );

    if (GuiTextBox(bounds, input, sizeof(input), input_mode)) {
        printf("enter\n");
        int a = 0;
        int b = 0;
        char ann_name[32] = {0};

        if (sscanf(input, "%30s %d %d", (char*)ann_name, &a, &b) == 3) {
            struct AnnHolder *ah = holders_search(ann_name);
            if (!ah)
                return;

            const char *str_result = ah->run(ah, a, b);

            //double inputs[2] = { a, b };
            //const double *outputs = genann_run(ann_xor, inputs);
            //printf("outputs[0] %f\n", outputs[0]);
            //printf("outputs[1] %f\n", outputs[1]);

            /*
               if (outputs[0] > 0.5) {
               strcpy(result, "0");
               } else {
               strcpy(result, "1");
               }
               */

            //sprintf(result, "%.1f", outputs[0]);
            strcpy(result, str_result);
        } else {
            strcpy(result, "undefined");
        }
    }

    DrawText(
        result, bounds.x + bounds.width * 1.1, bounds.y, font_size, BLACK
    );
}

void update() {
    camera_process();

    BeginDrawing();
    BeginMode2D(camera);
    ClearBackground(RAYWHITE);

    Vector2 mouse_point = GetScreenToWorld2D(GetMousePosition(), camera);

    for (int i = 0; i < viewers_num; i++) {
        genann_view_draw(viewers[i]);
        genann_view_update(viewers[i], mouse_point);
    }

    input_text();

    /*printf("xor_input: %s\n", xor_input);*/

    EndMode2D();
    EndDrawing();

    //usleep(10000);
}

void viewers_free() {
    for (int j = 0; j < viewers_num; ++j) 
        genann_view_free(viewers[j]);
}

void ann_add(const char *name, genann *ann, Vector2 pos, AnnRun run) {
    assert(name);
    assert(ann);
    assert(viewers_num < MAX_VIEVERS_NUM);

    viewers[viewers_num] = genann_view_new(name);
    genann_view_position_set(
        viewers[viewers_num], pos
    );
    genann_view_prepare(viewers[viewers_num], ann);
    holders[viewers_num].ann = ann;
    holders[viewers_num].run = run;
    strcpy(holders[viewers_num].name, name);
    viewers_num++;
}

const char * ann_run_test(struct AnnHolder *ah, float a, float b) {
    const double inputs[2] = { a, b};
    const double *outputs = genann_run(ah->ann, inputs);
    static char buf[256] = {0};
    char *buf_ptr = buf;
    for (int i = 0; i < ah->ann->outputs; i++) {
        buf_ptr += sprintf(buf_ptr, "%f ", outputs[i]);
    }
    return buf;
}

const char * ann_run_xor(struct AnnHolder *ah, float a, float b) {
    const double inputs[2] = { a, b};
    const double *outputs = genann_run(ah->ann, inputs);
    static char buf[256] = {0};
    char *buf_ptr = buf;
    for (int i = 0; i < ah->ann->outputs; i++) {
        buf_ptr += sprintf(buf_ptr, "%f ", outputs[i]);
    }
    return buf;
}

const char * ann_run_div(struct AnnHolder *ah, float a, float b) {
    const double inputs[2] = { a, b};
    const double *outputs = genann_run(ah->ann, inputs);
    static char buf[256] = {0};
    char *buf_ptr = buf;
    for (int i = 0; i < ah->ann->outputs; i++) {
        buf_ptr += sprintf(buf_ptr, "%f ", outputs[i]);
    }
    return buf;
}

void anns_init() {
    ann_add(
        "test",
        genann_init(5, 3, 5, 4),
        (Vector2) { 0., 0. },
        ann_run_test
    );

    ann_add(
        "xor",
        ann_get_xor(),
        (Vector2) { 300., -300. },
        ann_run_xor
    );

    ann_add(
        "div",
        ann_get_div(),
        (Vector2) { -200., -500. },
        ann_run_div
    );
}

int main(void) {
    camera.zoom = 1.0f;
    srand(time(NULL));
    logger_init();
    InitWindow(screen_width, screen_height, "Персетрончики как лимончики");

    anns_init();

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        update();
    }

    CloseWindow();
    genann_view_free(viewers[0]);

    viewers_free();
    holders_shutdown();
    logger_shutdown();
    return 0;
}
