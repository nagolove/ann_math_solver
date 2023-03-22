#!/usr/bin/env python3

import re
import matplotlib.pyplot as plt
# import numpy as np

# fig = plt.figure()
# ax = plt.axes(projection='3d')
# ax = plt.axes(projection='3d')
# plt.show()

ax = plt.axes(projection='3d')

# zline = np.linspace(10, 150, 1000)
# xline = np.sin(zline)
# yline = np.cos(zline)
# ax.plot3D(xline, yline, zline, 'gray')
# plt.show()

fname_st = "/home/nagolove/ann_math_solver/sum_study.txt"
fname_res = "/home/nagolove/ann_math_solver/sum_res.txt"


def load(fname, color):
    zdata = []
    xdata = []
    ydata = []
    for line in open(fname).readlines():
        rez = re.match("(\\S+) (\\S+) (\\S+)", line)
        xdata.append(float(rez.groups()[0]))
        ydata.append(float(rez.groups()[1]))
        zdata.append(float(rez.groups()[2]))

    # zdata = 15 * np.random.random(100)
    # xdata = np.sin(zdata) + 0.1 * np.random.randn(100)
    # ydata = np.cos(zdata) + 0.1 * np.random.randn(100)
    # ax.scatter3D(xdata, ydata, zdata, c=zdata, cmap='Greens');
    # ax.scatter3D(xdata, ydata, zdata, color = 'red', c=zdata, cmap='Greens');
    ax.scatter3D(xdata, ydata, zdata, color=color)


# ax.scatter3D(xdata, ydata, zdata, color = 'blue');
load(fname_st, 'blue')
load(fname_res, 'red')
plt.show()
