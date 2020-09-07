#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


#***************************************************************************
#                           plotlib.py  -  
#                              -------------------
#     begin                : 28 03 2020
#     email                : tong.qiu@cern.ch
#  **************************************************************************
from __future__ import division
import matplotlib
matplotlib.use('Agg')
import math
import matplotlib.pyplot as plt
import numpy as np
import copy

def histplot(data_x, data_y, error=None, label=None, **kwargs):
    settings = {
        "xlabel" : r"$p_T\:[GeV]$",
        "ylabel": 'Efficiency',
        "title1": r"ATLAS",
        "title1_1": r"Simulation",
        "title2": r"$\sqrt{s}=13\:TeV$",
        "filename": "trunontest",
        "log_y":False,
        "norm":False,
        "scale":1,
        "upper_y": 1.7,
        "errorbar_limit": None,
        }
    for each_key in kwargs.items():
        settings[each_key[0]] = kwargs[each_key[0]]
    plt.figure(figsize=(8, 6))
    remove_label = False
    if label is None:
        remove_label = True
        label = ["a"] * len(data_x)
    for i in range(len(data_x)):
        if error is not None:
            if settings["errorbar_limit"] is not None:
                allerror = []
                for each_error, each_yi in zip(error[i], data_y[i]):
                    uperror = each_error
                    if each_yi + each_error > settings["errorbar_limit"]:
                        uperror = settings["errorbar_limit"] - each_yi
                    allerror.append([each_error, uperror])
            else:
                allerror = error[i]
            style = '.-'
            if len(data_x) == 1:
                style = 'k.-'
            plt.errorbar(data_x[i], data_y[i], yerr=np.array(allerror).T, label=label[i], fmt=style)
        else:
            plt.plot(data_x[i], data_y[i], label=label[i])
    ax1 = plt.gca()

    plt.legend(loc='upper right', prop={'size': 25}, frameon=False)
    if remove_label:
        ax1.get_legend().remove()

    ax1.set_ylim([0, max([y for x in data_y for y in x])* settings["upper_y"]])
    ax1.text(0.05, 1.55 / 1.7, settings['title1'], fontsize=25, transform=ax1.transAxes, style='italic', fontweight='bold')
    ax1.text(0.28, 1.55/ 1.7, settings['title1_1'], fontsize=25, transform=ax1.transAxes)
    ax1.text(0.05, 1.40 / 1.7, settings['title2'], fontsize=23, transform=ax1.transAxes)

    plt.tick_params(labelsize=16)
    plt.tick_params(labelsize=16)
    plt.ylabel(settings['ylabel'], fontsize=20)
    plt.xlabel(settings['xlabel'], fontsize=20)
    plt.savefig(settings['filename'] + '.pdf', bbox_inches='tight')
    plt.close()
