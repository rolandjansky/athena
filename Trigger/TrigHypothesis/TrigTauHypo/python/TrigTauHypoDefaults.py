# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.SystemOfUnits import GeV
from AthenaCommon.SystemOfUnits import mm

EFTau_defaults_loose = {
    'NTrackMin'       : 1,
    'EMRadiusMax_1P'  : 0.18,
    'EMRadiusMax_MP'  : 0.134,
    'DrTrkAvgMax_1P'  : 0.151,
    'DrTrkAvgMax_MP'  : 0.105,
    'EtovPtLeadMax_1P': 999.,
    'EtovPtLeadMax_MP': 5.0,
    'emrad_OP_a0'     : 0.190,
    'emrad_OP_a1'     : -8.5E-04,
    'emrad_OP_a2'     : 0.365,
    'emrad_MP_a0'     : 0.203,
    'emrad_MP_a1'     : -1.15E-03,
    'emrad_MP_a2'     : -1.94E-03,
    'trkrad_OP_a0'    : 0.140,
    'trkrad_OP_a1'    : -2.13E-04,
    'trkrad_OP_a2'    : 0.827,
    'trkrad_MP_a0'    : 0.111,
    'trkrad_MP_a1'    : -2.22E-04,
    'trkrad_MP_a2'    : 0.441,
    }

EFTau_defaults_medium = {
    'NTrackMin'       : 1,
    'EMRadiusMax_1P'  : 0.095,
    'EMRadiusMax_MP'  : 0.109,
    'DrTrkAvgMax_1P'  : 0.158,
    'DrTrkAvgMax_MP'  : 0.069,
    'EtovPtLeadMax_1P': 8.33,
    'EtovPtLeadMax_MP': 3.33,
    'emrad_OP_a0'     : 0.20,
    'emrad_OP_a1'     : -3.76E-04,
    'emrad_OP_a2'     : 0.516,
    'emrad_MP_a0'     : 0.157,
    'emrad_MP_a1'     : -8.5E-04,
    'emrad_MP_a2'     : 0.197,
    'trkrad_OP_a0'    : 0.27,
    'trkrad_OP_a1'    : -3.92E-04,
    'trkrad_OP_a2'    : 0.626,
    'trkrad_MP_a0'    : 0.0894,
    'trkrad_MP_a1'    : -4.92E-04,
    'trkrad_MP_a2'    : 0.563,
    }

EFTau_defaults_tight = {
    'NTrackMin'       : 1,
    'EMRadiusMax_1P'  : 0.055,
    'EMRadiusMax_MP'  : 0.0781,
    'DrTrkAvgMax_1P'  : 0.0515,
    'DrTrkAvgMax_MP'  : 0.03,
    'EtovPtLeadMax_1P': 6.25,
    'EtovPtLeadMax_MP': 2.78,
    'emrad_OP_a0'     : 0.0584,
    'emrad_OP_a1'     : -2.27E-04,
    'emrad_OP_a2'     : 0.616,
    'emrad_MP_a0'     : 0.1,
    'emrad_MP_a1'     : -4.94E-04,
    'emrad_MP_a2'     : 0.436,
    'trkrad_OP_a0'    : 0.0643,
    'trkrad_OP_a1'    : -3.18E-04,
    'trkrad_OP_a2'    : 0.0381,
    'trkrad_MP_a0'    : 0.0455,
    'trkrad_MP_a1'    : -3.57E-04,
    'trkrad_MP_a2'    : 0.356,
    }

EFTau_defaults = {
    'NTrackMin'          : 1,
    'NTrackMax'          : 1,
    'IsoFracMax'         : 10.,
    'EMFracMin'          : -10.,
    'EtCalibMin'         : 0.*GeV,
    'EMEtMin'            : -10.*GeV,
    'EMRadiusMax_1P'     : 0.11,
    'EMRadiusMax_MP'     : 0.11,
    'CalRadiusMax_1P'    : 100.,
    'CalRadiusMax_MP'    : 100.,
    'DrTrkAvgMax_1P'     : 100.,
    'DrTrkAvgMax_MP'     : 100.,
    'PtMaxTrackMin_1P'   : 6.*GeV,
    'PtMaxTrackMin_MP'   : 0.*GeV,
    'MassMax_2P'         : 0.*GeV,
    'MassMax_MP'         : 0.*GeV,
}

EFTau_defaults_calo = {
    'NTrackMin'          : 0,
    'NTrackMax'          : 100,
    'EtCalibMin'         : 0.*GeV,
    'EMRadiusMax_1P'     : 0.055,
    'EMRadiusMax_MP'     : 0.142,
    'DrTrkAvgMax_1P'     : 1.,
    'DrTrkAvgMax_MP'     : 1.,
    'EtovPtLeadMax_1P'   : 999.,
    'EtovPtLeadMax_MP'   : 999.,
    'emrad_OP_a0'        : 0.063,
    'emrad_OP_a1'        : -2.04E-04,
    'emrad_OP_a2'        : 0.663,
    'emrad_MP_a0'        : 0.179,
    'emrad_MP_a1'        : -5.17E-04,
    'emrad_MP_a2'        : 0.339,
    'trkrad_OP_a0'       : 1.,
    'trkrad_OP_a1'       : 0.,
    'trkrad_OP_a2'       : 0.,
    'trkrad_MP_a0'       : 1.,
    'trkrad_MP_a1'       : 0.,
    'trkrad_MP_a2'       : 0.,
}

T2Calo_defaults = {
    'EtRawMin'        : 0. *GeV,
    'EmRadius3SMax'   : 9999.9,
    'CoreFractionMin' : -9999.9
    }

T2ID_defaults = {  
    'NTrkMin'             : 1,
    'NTrkMax'             : -1,#make sure you always set it!
    'SumPtRatioMax'       : 9999.9,
    'SumPtRatioMax1P'     : 9999.9,
    'SumPtRatioMaxMP'     : 9999.9,
    'LdTrkPtMin'          : -9999.9 *GeV
    }

T2Tau_defaults = {
    'EmRadius3S1pMax'      : 9999.9,
    'EtOverPtLeadTrkMax'   : 9999.9,
    'EtOverPtLeadTrkMax1P' : 9999.9,
    'EtOverPtLeadTrkMaxMP' : 9999.9,
    'TrkAvgDistMax'        : 9999.9,
    'TrkAvgDistMax1P'      : 9999.9,
    'TrkAvgDistMaxMP'      : 9999.9
    }

#EFHadCalib_defaults = {
#    "CutOnPtHT"             :  9.*GeV,
#    "CutOnIsolationDeltaR"  :  0.4,
#    "CutnTracksOnDeltaR"    :  0,
#    #"CutOnLeadingTrackEta"  :  10.,
#    "MaxTrackPtInIso"       :  0,
#    "CutminSCTHitsHT"       :  -1,
#    "CutminPixHitsHT"       :  -1,
#    "CutmaxD0HT"            : 9999.9*mm,
#    "CutmaxZ0HT"            : 9999.9*mm
#    }
EFHadCalib_defaults = {
    "CutOnPtHT"             :  0.*GeV,
    "CutOnIsolationDeltaR"  :  0.0,
    "CutnTracksOnDeltaR"    :  99,
    "CutOnEtaHT"            :  10.,
    "MaxTrackPtInIso"       :  99,
    "CutminSCTHitsHT"       :  -1,
    "CutminPixHitsHT"       :  -1,
    "CutmaxD0HT"            : 9999.9*mm,
    "CutmaxZ0HT"            : 9999.9*mm
    }


def applyDefaults( obj, dict):
    for k, v in dict.iteritems():
        obj.__setattr__( k, v)
