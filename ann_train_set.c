#include "ann_train_set.h"

#include <assert.h>
#include <stdlib.h>
#include "genann.h"

struct Set {
    genann          **nets;
    char*           models;
    int             model_size;
    int             num;
};

struct ann_topology {
    int inputs_num, hidden_layers_num, hidden_num, outputs_num;
};

typedef void (*ModelInitFunc)(void *);

struct Set *set_new(
    int agents_num, struct ann_topology topology, 
    int model_size, ModelInitFunc func
) {
    struct Set *s = malloc(sizeof(*s));
    s->num = agents_num;
    s->nets = calloc(sizeof(s->nets[0]), agents_num);
    s->models = calloc(model_size, agents_num);
    s->model_size = model_size;
    for (int i = 0; i < s->num; i++) {
        s->nets[i] = genann_init(
            topology.inputs_num,
            topology.hidden_layers_num,
            topology.hidden_num,
            topology.outputs_num
        );
        func(&s->models[i * model_size]);
        /*modelbox_init(&s->models[i]);*/
    }
    return s;
}

void set_shutdown(struct Set *s) {
    assert(s);
    for (int i = 0; i < s->num; ++i) {
        genann_free(s->nets[i]);
    }
    free(s->models);
    free(s->nets);
}

void set_remove_one(struct Set *s, int index) {
    assert(s);
    assert(index >= 0);
    assert(index < s->num);

    genann_free(s->nets[index]);
    s->nets[index] = s->nets[s->num - 1];
    s->models[index] = s->models[s->num - 1];
    --s->num;
}

struct Set *set_select(struct Set *s) {
    assert(s);
    int num = s->num - 1;
    printf("set_select: num %d\n", num);
    assert(num >= 1);

    const int remove_num = 4;
    for (int i = 0; i < remove_num; i++) {
        int min_index = set_find_min_index(s);
        assert(min_index != -1);
        set_remove_one(s, min_index);
    }

    struct Set *new = malloc(sizeof(*new));
    new->num = s->num;
    new->nets = calloc(sizeof(new->nets[0]), s->num);
    new->models = calloc(sizeof(new->models[0]), s->num);

    for (int i = 0; i < s->num; ++i) {
        new->nets[i] = genann_copy(s->nets[i]);
        new->models[i] = s->models[i];
    }

    return new;
}

/*
static int set_find_min_index(struct Set *s) {
    int min_scores = 3000;
    int min_index = -1;
    for (int k = 0; k < s->num; k++) {
        //printf("k %d\n", k);
        if (s->models[k].scores < min_scores) {
            min_scores = s->models[k].scores;
            min_index = k;
        }
    }
    return min_index;
}
*/

void set_train(struct Set *s) {
}

void set_write(struct Set *s) {
    assert(s);
    printf("set_write: num %d\n", s->num);
    for (int j = 0; j < s->num; ++j) {
        char fname[100] = {0};
        sprintf(fname, "bin/trained_%.3d.binary", j);
        FILE *file = fopen(fname, "w");
        if (file) {
            genann_write(s->nets[j], file);
            fclose(file);
        }
    }
}


