#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


#***************************************************************************
#                           makeplots.py  -  
#                              -------------------
#     begin                : 28 03 2020
#     email                : tong.qiu@cern.ch
#  **************************************************************************
from __future__ import print_function
from __future__ import division
import numpy as np
from eFEXNTuple import *
from plotlib import *
import sys

def rebin(binning, data):
    hist, edge = np.histogram(data, binning)
    newbin = [edge[0]]
    total_i = 0
    for i in range(len(hist)):
        total_i += hist[i]
        if total_i > 0:
            newbin.append(edge[i+1])
            total_i = 0
    newbin[-1] = binning[-1]
    return newbin

def rebin2(binning, data1, data2):
    hist1, edge = np.histogram(data1, binning)
    hist2, edge = np.histogram(data2, binning)
    newbin = [edge[0]]
    total_1 = 0
    total_2 = 0
    for i in range(len(hist1)):
        total_1 += hist1[i]
        total_2 += hist2[i]
        if total_1 > 0 and total_2 > 0:
            newbin.append(edge[i+1])
            total_1 = 0
            total_2 = 0
    newbin[-1] = binning[-1]
    return newbin

def turn_on_curve(all_data, selected_data, bins):
    bins = rebin(bins, all_data)
    hist_all, edges = np.histogram(all_data, bins)
    hist_selected, edges = np.histogram(selected_data, bins)
    bin_centre = [(edges[i] + edges[i+1])/2. for i in range(len(edges)-1)]
    hist_height = hist_selected / hist_all
    hist_error = ((hist_selected**0.5 / hist_all)**2. + (hist_all**0.5 * hist_selected / hist_all**2.)**2.)**0.5
    return (hist_height, bin_centre, hist_error)

def main(path):
    myntuple = eFEXNTupleLoader(path)
    all_e_ET = []
    selected_e_ET = []
    for each in myntuple.entries():
        each.set_class_type("e")
        if each.is_in_crack(each.leading_l_id()):
            continue
        if abs(each.truth_e_eta[each.leading_l_id()]) > 2.3:
            continue
        all_e_ET.append(each.truth_e_ET[each.leading_l_id()])
        matched_tobs = each.get_eg_matchedtob_id()[each.leading_l_id()]
        selected = False
        for i in matched_tobs:
            if not each.eg_haveseed[i]:
                continue
            if each.eg_ET[i] <= 22:
                continue
            #if 0.025*(each.eg_wstotnum[i]/each.eg_wstotden[i])**0.5 < 0.045:
            selected = True
            break
        if selected:
            selected_e_ET.append(each.truth_e_ET[each.leading_l_id()])

    hist_height, bin_centre, hist_error = turn_on_curve(all_e_ET, selected_e_ET, range(0,100,5))
    histplot([bin_centre], [hist_height], [hist_error], errorbar_limit=1, filename="egammatrunon")


if __name__ == "__main__":
    if len(sys.argv) > 2:
        print("Error: too many arguments")
        print("Example: python makeplots.py PATH/TO/INPUT")
        exit(1)
    if len(sys.argv) == 1:
        print("Error: need the path to the input file")
        print("Example: python makeplots.py PATH/TO/INPUT")
        print("Info: Using default input file path")
        inputfile = "myfile.root"
    else:
        inputfile = sys.argv[1]
    main(inputfile)
