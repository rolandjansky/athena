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
    all_l_ET = []
    selected_l_ET_TLV = []
    selected_l_eta_TLV = []
    selected_l_ET_Oregon = []
    selected_l_eta_Oregon = []
    all_l_eta = []
    for each in myntuple.entries():
        each.set_class_type("tau")
        if each.leading_l_id() is None:
            continue
        # if len(each.truth_l_ET)==0:
        #     continue
        if each.is_in_crack(each.leading_l_id()):
            continue
        if abs(each.truth_tauvisible_eta[each.leading_l_id()]) > 2.3:
            continue
        # if abs(each.truth_tauvisible_ET[each.leading_l_id()] <= 40):
        #     continue
        all_l_ET.append(each.truth_tauvisible_ET[each.leading_l_id()])
        all_l_eta.append(each.truth_tauvisible_eta[each.leading_l_id()])
        matched_tobs = each.get_eg_matchedtob_id()[each.leading_l_id()]
        tlv_selected = False
        oregon_selected = False
        for i in matched_tobs:
            if each.eg_tauTLV_Et[i] <= 12:
                continue
            if each.eg_tauTLV_Iso[i] < 0.66 and each.eg_tauTLV_Et[i] <= 15:
                continue
            tlv_selected = True
            break
        for i in matched_tobs:
            if each.eg_tauOregon_Et[i] <= 12:
                continue
            if each.eg_tauOregon_Iso[i] < 0.66 and each.eg_tauOregon_Et[i] <= 15:
                continue
            oregon_selected = True
            break
        if tlv_selected:
            selected_l_ET_TLV.append(each.truth_tauvisible_ET[each.leading_l_id()])
            #selected_l_eta_TLV.append(each.truth_tauvisible_eta[each.leading_l_id()])
        if oregon_selected:
            selected_l_ET_Oregon.append(each.truth_tauvisible_ET[each.leading_l_id()])
            #selected_l_eta_Oregon.append(each.truth_tauvisible_eta[each.leading_l_id()])
    hist_height, bin_centre, hist_error = turn_on_curve(all_l_ET, selected_l_ET_TLV, range(0,100,5))
    histplot([bin_centre], [hist_height], [hist_error], errorbar_limit=1, filename="tautlvtrunon")
    hist_height, bin_centre, hist_error = turn_on_curve(all_l_ET, selected_l_ET_Oregon, range(0,100,5))
    histplot([bin_centre], [hist_height], [hist_error], errorbar_limit=1, filename="tauoregontrunon")
    # hist_height, bin_centre, hist_error = turn_on_curve(all_l_eta, selected_l_eta, np.linspace(-2.3,2.3,10))
    # histplot([bin_centre], [hist_height], [hist_error], errorbar_limit=1, filename="eta", xlabel="eta")


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
