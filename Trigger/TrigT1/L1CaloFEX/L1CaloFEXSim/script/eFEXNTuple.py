#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


#***************************************************************************
#                           eFEXNTuple.py  -  
#                              -------------------
#     begin                : 28 03 2020
#     email                : tong.qiu@cern.ch
#  **************************************************************************
from __future__ import division
import ROOT
import numpy as np
import math

def delta_phi(phi1, phi2):
    output = abs(phi1 - phi2)
    if output > math.pi:
        output = math.pi * 2. - output
    return output

def delta_R(eta1, phi1, eta2, phi2):
    return (delta_phi(phi1, phi2)**2. + (eta1 - eta2)**2.)**0.5

class eFEXNTupleLoader():
    def __init__(self, filename):
        self.tfile = ROOT.TFile(filename)
        self.tree = self.tfile.Get("data;1")

    def entries(self):
        for each_entry in self.tree:
            entry_dict = {}
            entry_dict["truth_e_ET"] = np.asarray(each_entry.truth_e_ET)/1000.
            entry_dict["truth_e_phi"] = np.asarray(each_entry.truth_e_phi)
            entry_dict["truth_e_eta"] = np.asarray(each_entry.truth_e_eta)
            entry_dict["eg_eta"] = np.asarray(each_entry.eg_eta) * 0.1 #+ 0.05
            entry_dict["eg_phi"] = np.asarray(each_entry.eg_phi) * math.pi/32. #+ math.pi / 64# - math.pi)
            entry_dict["eg_ET"] = np.asarray(each_entry.eg_ET)/1000.
            entry_dict["eg_wstotnum"] = np.asarray(each_entry.eg_wstotnum)
            entry_dict["eg_wstotden"] = np.asarray(each_entry.eg_wstotden)
            entry_dict["eg_rhadnum"] = np.asarray(each_entry.eg_wstotnum)
            entry_dict["eg_rhadden"] = np.asarray(each_entry.eg_wstotden)
            entry_dict["eg_retanum"] = np.asarray(each_entry.eg_wstotnum)
            entry_dict["eg_retaden"] = np.asarray(each_entry.eg_wstotden)
            entry_dict["eg_haveseed"] = np.asarray(each_entry.eg_haveseed).astype(dtype=bool)
            entry_dict["eg_nTOBs"] = int(each_entry.eg_nTOBs)
            
            entry_dict["truth_tauvisible_ET"] = np.asarray(each_entry.truth_tauvisible_ET)/1000.
            entry_dict["truth_tauvisible_phi"] = np.asarray(each_entry.truth_tauvisible_phi)
            entry_dict["truth_tauvisible_eta"] = np.asarray(each_entry.truth_tauvisible_eta)

            entry_dict["eg_tauOregon_Et"] = np.asarray(each_entry.eg_tauOregon_Et)/1000.
            entry_dict["eg_tauOregon_Iso"] = np.asarray(each_entry.eg_tauOregon_Iso)
            entry_dict["eg_tauTLV_Et"] = np.asarray(each_entry.eg_tauTLV_Et)/1000.
            entry_dict["eg_tauTLV_Iso"] = np.asarray(each_entry.eg_tauTLV_Iso)
            # print(entry_dict["truth_e_phi"][0], entry_dict["truth_e_eta"][0])
            # i = np.argmax(entry_dict["eg_ET"])
            # print(entry_dict["eg_phi"][i], entry_dict["eg_eta"][i])
            yield eFEXAnalyzer(entry_dict)

    def __len__(self):
        return self.tree.GetEntries() - 1

class eFEXAnalyzer():
    def __init__(self, inputs):
        self.classtype = "e"
        for key, value in inputs.items():
            setattr(self, key, value)

    def set_class_type(self, inputs):
        self.classtype = inputs

    def get_eg_matchedtob_id(self):
        output = []
        if self.classtype == "e":
            for i_e in range(len(self.truth_e_ET)):
                tob_id_tem = []
                for i_tob in range(self.eg_nTOBs):
                    # TODO et selection
                    if self.eg_ET[i_tob] < 15.:
                        continue
                    if delta_R(self.truth_e_eta[i_e], self.truth_e_phi[i_e], self.eg_eta[i_tob], self.eg_phi[i_tob]) > 0.12:
                        continue
                    tob_id_tem.append(i_tob)
                output.append(tob_id_tem)
        if self.classtype == "tau":
            for i_e in range(len(self.truth_tauvisible_ET)):
                tob_id_tem = []
                for i_tob in range(self.eg_nTOBs):
                    # TODO et selection
                    # if self.truth_tauvisible_ET[i_tob] < 12.:
                    #     continue
                    if delta_R(self.truth_tauvisible_eta[i_e], self.truth_tauvisible_phi[i_e], self.eg_eta[i_tob], self.eg_phi[i_tob]) > 0.12:
                        continue
                    tob_id_tem.append(i_tob)
                output.append(tob_id_tem)
        return output

    def leading_l_id(self):
        if self.classtype == "e":
            return np.argmax(self.truth_e_ET)
        if self.classtype == "tau":
            if len(self.truth_tauvisible_ET) == 0:
                return None
            return np.argmax(self.truth_tauvisible_ET)


    def is_in_crack(self, i_e):
        if self.classtype == "e":
            return (abs(self.truth_e_eta[i_e]) > 1.375 and abs(self.truth_e_eta[i_e]) < 1.52)
        if self.classtype == "tau":
            return (abs(self.truth_tauvisible_eta[i_e]) > 1.375 and abs(self.truth_tauvisible_eta[i_e]) < 1.52)
