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

#define MAX_VIEVERS_NUM 4
static genann_view *net_viewers[MAX_VIEVERS_NUM] = {0};
static int net_viewers_num = 0;

static genann *ann_xor = NULL;
static genann *ann_div = NULL;

static const int screen_width = 1920;
static const int screen_height = 1080;

static Camera2D camera = { 0 };

void input() {
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

static bool xor_input_mode = true;
static char xor_input[50] = {0};
static char xor_result[50] = {0};

void xor_input_process() {
    Rectangle xor_bounds = {
        100, 100, 400, 80,
    };
    if (GuiTextBox(xor_bounds, xor_input, sizeof(xor_input), xor_input_mode)) {
        printf("enter\n");
        int a = 0;
        int b = 0;
        assert(ann_xor);
        if (sscanf(xor_input, "%d %d", &a, &b) == 2) {
            double inputs[2] = { a, b };
            const double *outputs = genann_run(ann_xor, inputs);
            printf("outputs[0] %f\n", outputs[0]);
            printf("outputs[1] %f\n", outputs[1]);
            /*
               if (outputs[0] > 0.5) {
               strcpy(xor_result, "0");
               } else {
               strcpy(xor_result, "1");
               }
               */
            sprintf(xor_result, "%.1f", outputs[0]);
        } else {
            strcpy(xor_result, "0");
        }
    }

    DrawText(
        xor_result, xor_bounds.x + xor_bounds.width * 1.5, 100, 20, BLACK
    );
}

void update() {
    camera_process();

    BeginDrawing();
    BeginMode2D(camera);
    ClearBackground(RAYWHITE);

    Vector2 mouse_point = GetScreenToWorld2D(GetMousePosition(), camera);

    for (int i = 0; i < net_viewers_num; i++) {
        genann_view_draw(net_viewers[i]);
        genann_view_update(net_viewers[i], mouse_point);
    }

    xor_input_process();

    /*printf("xor_input: %s\n", xor_input);*/

    EndMode2D();
    EndDrawing();

    //usleep(10000);
}

void viewers_free() {
    for (int j = 0; j < net_viewers_num; ++j) 
        genann_view_free(net_viewers[j]);
}

int main(void) {
    camera.zoom = 1.0f;
    srand(time(NULL));
    InitWindow(screen_width, screen_height, "2048");

    /*
    net_viewers[net_viewers_num] = genann_view_new("divider");
    genann_view_position_set(
        net_viewers[net_viewers_num], (Vector2) { 0., -1000. }
    );
    net_viewers_num++;

    net_viewers[net_viewers_num] = printing_test();
    genann_view_position_set(
        net_viewers[net_viewers_num], (Vector2) { 0., -2000. }
    );
    net_viewers_num++;
    */

    net_viewers[net_viewers_num] = genann_view_new("xor");
    ann_xor = ann_get_xor();
    genann_view_prepare(net_viewers[net_viewers_num], ann_xor);
    genann_view_position_set(
        net_viewers[net_viewers_num], (Vector2) { 0., -2000. }
    );
    net_viewers_num++;

    net_viewers[net_viewers_num] = genann_view_new("div");
    ann_div = ann_get_div();
    genann_view_prepare(net_viewers[net_viewers_num], ann_div);
    genann_view_position_set(
        net_viewers[net_viewers_num], (Vector2) { 0., -3000. }
    );
    net_viewers_num++;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        update();
    }

    CloseWindow();
    genann_view_free(net_viewers[0]);

    viewers_free();
    genann_free(ann_xor);
    genann_free(ann_div);
    return 0;
}
