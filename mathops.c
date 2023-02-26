#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <math.h>
#include "genann.h"

double *gen_outputs(int a, int b) {
    static double outputs[4] = {0};
    int res = floor(((float)a / b) * 10.);

    outputs[0] = res % 10;
    outputs[1] = (res - (res % 10)) / 10.;
    printf("out[0] %f\n", outputs[0]);
    printf("out[1] %f\n", outputs[1]);
    /*outputs[2] = res % 10;*/
    /*outputs[3] = (res - (res % 10)) / 10.;*/
    return outputs;
}


double *gen_inputs(int *_a, int *_b) {
    assert(_a);
    assert(_b);

    int a = rand() % 99;
    int b = 1 + rand() % 98;
    /*float res = (float)a / (float)b;*/

    *_a = a;
    *_b = b;

    printf("a %d\n", a);
    printf("b %d\n", b);

    static double inputs[4] = {0};

    inputs[0] = a % 10;
    inputs[1] = (a - (a % 10)) / 10.;
    inputs[2] = b % 10;
    inputs[3] = (b - (b % 10)) / 10.;

    printf("inputs[0] %f\n", inputs[0]);
    printf("inputs[1] %f\n", inputs[1]);
    printf("inputs[2] %f\n", inputs[2]);
    printf("inputs[3] %f\n", inputs[3]);
    return (double*)inputs;
}

genann *ann_get_div() {
    /*printf("Train a small ANN to the XOR function using backpropagation.\n");*/

    /* This will make the neural network initialize differently each run. */
    /* If you don't get a good result, try again for a different result. */
    srand(time(0));

    /* Input and expected out data for the XOR function. */
    /*const double input[4][2] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};*/
    /*const double output[4] = {0, 1, 1, 0};*/
    int i;

    /* New network with 2 inputs,
     * 1 hidden layer of 2 neurons,
     * and 1 output. */
    genann *ann = genann_init(4, 1, 10, 4);

    /* Train on the four labeled data points many times. */
    for (i = 0; i < 1; ++i) {
        int a, b;
        double *inputs = gen_inputs(&a, &b);
        double *outputs = gen_outputs(a, b);

        genann_train(ann, inputs, outputs, 0.1);

        /*genann_train(ann, input[0], output + 0, 3);*/
        /*genann_train(ann, input[1], output + 1, 3);*/
        /*genann_train(ann, input[2], output + 2, 3);*/
        /*genann_train(ann, input[3], output + 3, 3);*/
    }

    /*genann_free(ann);*/
    return ann;
}

