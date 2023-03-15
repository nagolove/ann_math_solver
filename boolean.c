#include "boolean.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "genann.h"

genann *ann_get_xor() {
    /*printf("Train a small ANN to the XOR function using backpropagation.\n");*/

    /* This will make the neural network initialize differently each run. */
    /* If you don't get a good result, try again for a different result. */

    /* Input and expected out data for the XOR function. */
    const double input[4][2] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    const double output[4] = {0, 1, 1, 0};
    int i;

    /* New network with 2 inputs,
     * 1 hidden layer of 2 neurons,
     * and 1 output. */
    genann *ann = genann_init(2, 1, 2, 1);

    /* Train on the four labeled data points many times. */
    for (i = 0; i < 1500; ++i) {
        genann_train(ann, input[0], output + 0, 3);
        genann_train(ann, input[1], output + 1, 3);
        genann_train(ann, input[2], output + 2, 3);
        genann_train(ann, input[3], output + 3, 3);
    }

    /*genann_free(ann);*/
    return ann;
}

genann *ann_get_and() {

    /* Input and expected out data for the AND function. */
    const double input[4][2] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    const double output[4] = {0, 0, 0, 1};
    int i;

    /* New network with 2 inputs,
     * 1 hidden layer of 2 neurons,
     * and 1 output. */
    genann *ann = genann_init(2, 1, 2, 1);

    /* Train on the four labeled data points many times. */
    for (i = 0; i < 1500; ++i) {
        genann_train(ann, input[0], output + 0, 3);
        genann_train(ann, input[1], output + 1, 3);
        genann_train(ann, input[2], output + 2, 3);
        genann_train(ann, input[3], output + 3, 3);
    }

    return ann;
}

genann *ann_get_or() {

    /* Input and expected out data for the OR function. */
    const double input[4][2] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    const double output[4] = {0, 1, 1, 1};
    int i;

    /* New network with 2 inputs,
     * 1 hidden layer of 2 neurons,
     * and 1 output. */
    genann *ann = genann_init(2, 1, 2, 1);

    /* Train on the four labeled data points many times. */
    for (i = 0; i < 1500; ++i) {
        genann_train(ann, input[0], output + 0, 3);
        genann_train(ann, input[1], output + 1, 3);
        genann_train(ann, input[2], output + 2, 3);
        genann_train(ann, input[3], output + 3, 3);
    }

    return ann;
}

