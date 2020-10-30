# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloSwEtamod_v4.py
# Created: Nov 2006, sss
# Purpose: Eta modulation corrections, v4.
#
# These corrections were derived using 12.0.3 simulation and reconstruction,
# for cluster sizes of 5x5, 3x5, and 3x7, for both electrons
# and photons.
#
# This was added in CaloClusterCorrection-00-02-64, in 12.0.4.
#


from CaloClusterCorrection.constants import CALOCORR_COMBINED2
from AthenaCommon.SystemOfUnits import GeV


# Energies at which corrections were tabulated.
etamod_ele_sample_energies = [10*GeV, 25*GeV, 50*GeV, 75*GeV,
                              100*GeV, 200*GeV, 500*GeV, 1000*GeV]
etamod_gam_sample_energies = [5*GeV, 10*GeV, 25*GeV, 50*GeV, 75*GeV,
                              100*GeV, 200*GeV, 500*GeV, 1000*GeV]


etamod_v4_ele5_55 = [0.983806, 3.750856, -154.635676]
etamod_v4_ele5_35 = [0.999497, 0.188522, -8.791095]
etamod_v4_ele5_37 = [1.001259, -0.474412, 9.681194]
etamod_v4_ele10_55 = [0.993712, 1.209621, -49.526417]
etamod_v4_ele10_35 = [0.991334, 1.695083, -60.905850]
etamod_v4_ele10_37 = [0.989896, 2.063848, -73.906376]
etamod_v4_ele25_55 = [0.995942, 0.555437, -15.500041]
etamod_v4_ele25_35 = [0.992321, 1.538730, -52.050354]
etamod_v4_ele25_37 = [0.994053, 1.392923, -54.094068]
etamod_v4_ele50_55 = [0.996982, 0.528204, -17.990771]
etamod_v4_ele50_35 = [0.993117, 1.512039, -56.901355]
etamod_v4_ele50_37 = [0.993183, 1.473668, -54.724100]
etamod_v4_ele75_55 = [0.997754, 0.377698, -16.414560]
etamod_v4_ele75_35 = [0.993308, 1.318025, -46.984286]
etamod_v4_ele75_37 = [0.992406, 1.573704, -57.762698]
etamod_v4_ele100_55 = [0.998076, 0.308318, -9.139902]
etamod_v4_ele100_35 = [0.992809, 1.425346, -51.864830]
etamod_v4_ele100_37 = [0.992503, 1.586192, -58.698449]
etamod_v4_ele200_55 = [0.997283, 0.478227, -16.154217]
etamod_v4_ele200_35 = [0.993058, 1.496153, -56.248150]
etamod_v4_ele200_37 = [0.992762, 1.558639, -58.741619]
etamod_v4_ele500_55 = [0.997707, 0.421512, -14.367443]
etamod_v4_ele500_35 = [0.992741, 1.499762, -55.405371]
etamod_v4_ele500_37 = [0.992597, 1.570775, -58.663627]
etamod_v4_ele1000_55 = [0.997539, 0.412089, -12.911872]
etamod_v4_ele1000_35 = [0.992670, 1.557133, -57.989423]
etamod_v4_ele1000_37 = [0.992992, 1.484024, -55.313061]

etamod_v4_gam5_55 = [0.990666, 1.975982, -93.145708]
etamod_v4_gam5_35 = [0.979576, 3.897743, -146.429330]
etamod_v4_gam5_37 = [0.990993, 1.612493, -68.488132]
etamod_v4_gam10_55 = [0.990695, 1.738726, -61.737552]
etamod_v4_gam10_35 = [0.997539, 1.247956, -56.066169]
etamod_v4_gam10_37 = [0.991926, 2.244305, -91.768508]
etamod_v4_gam25_55 = [0.996626, 0.767360, -29.799398]
etamod_v4_gam25_35 = [0.989307, 2.261203, -85.400993]
etamod_v4_gam25_37 = [0.993203, 1.603192, -64.949502]
etamod_v4_gam50_55 = [0.997944, 0.265441, -5.366243]
etamod_v4_gam50_35 = [0.991916, 1.677999, -60.542668]
etamod_v4_gam50_37 = [0.991638, 1.689945, -61.928597]
etamod_v4_gam75_55 = [0.997735, 0.386117, -13.184488]
etamod_v4_gam75_35 = [0.992081, 1.694850, -63.205380]
etamod_v4_gam75_37 = [0.992619, 1.604587, -60.797382]
etamod_v4_gam100_55 = [0.996571, 0.666099, -24.579692]
etamod_v4_gam100_35 = [0.991244, 1.791867, -67.731569]
etamod_v4_gam100_37 = [0.991843, 1.740992, -64.891654]
etamod_v4_gam200_55 = [0.996892, 0.582237, -19.943738]
etamod_v4_gam200_55 = [0.996909, 0.573404, -19.243274]
etamod_v4_gam200_35 = [0.992165, 1.597763, -58.265427]
etamod_v4_gam200_37 = [0.992049, 1.642700, -60.023122]
etamod_v4_gam500_55 = [0.997435, 0.456722, -15.115561]
etamod_v4_gam500_35 = [0.992351, 1.594982, -58.915334]
etamod_v4_gam500_37 = [0.992352, 1.592001, -59.154386]
etamod_v4_gam1000_55 = [0.997663, 0.399569, -12.693380]
etamod_v4_gam1000_35 = [0.993040, 1.504923, -56.415771]
etamod_v4_gam1000_37 = [0.993451, 1.568644, -58.555394]


etamod_v4_ele55_correction = [
    #etamod_v4_ele5_55,
    etamod_v4_ele10_55,
    etamod_v4_ele25_55,
    etamod_v4_ele50_55,
    etamod_v4_ele75_55,
    etamod_v4_ele100_55,
    etamod_v4_ele200_55,
    etamod_v4_ele500_55,
    etamod_v4_ele1000_55,
    ]


etamod_v4_ele35_correction = [
    #etamod_v4_ele5_35,
    etamod_v4_ele10_35,
    etamod_v4_ele25_35,
    etamod_v4_ele50_35,
    etamod_v4_ele75_35,
    etamod_v4_ele100_35,
    etamod_v4_ele200_35,
    etamod_v4_ele500_35,
    etamod_v4_ele1000_35,
    ]


etamod_v4_ele37_correction = [
    #etamod_v4_ele5_37,
    etamod_v4_ele10_37,
    etamod_v4_ele25_37,
    etamod_v4_ele50_37,
    etamod_v4_ele75_37,
    etamod_v4_ele100_37,
    etamod_v4_ele200_37,
    etamod_v4_ele500_37,
    etamod_v4_ele1000_37,
    ]


etamod_v4_gam55_correction = [
    etamod_v4_gam5_55,
    etamod_v4_gam10_55,
    etamod_v4_gam25_55,
    etamod_v4_gam50_55,
    etamod_v4_gam75_55,
    etamod_v4_gam100_55,
    etamod_v4_gam200_55,
    etamod_v4_gam500_55,
    etamod_v4_gam1000_55,
    ]


etamod_v4_gam35_correction = [
    etamod_v4_gam5_35,
    etamod_v4_gam10_35,
    etamod_v4_gam25_35,
    etamod_v4_gam50_35,
    etamod_v4_gam75_35,
    etamod_v4_gam100_35,
    etamod_v4_gam200_35,
    etamod_v4_gam500_35,
    etamod_v4_gam1000_35,
    ]


etamod_v4_gam37_correction = [
    etamod_v4_gam5_37,
    etamod_v4_gam10_37,
    etamod_v4_gam25_37,
    etamod_v4_gam50_37,
    etamod_v4_gam75_37,
    etamod_v4_gam100_37,
    etamod_v4_gam200_37,
    etamod_v4_gam500_37,
    etamod_v4_gam1000_37,
    ]


class CaloSwEtamod_v4_parms:
    region = CALOCORR_COMBINED2
    energy_degree = 2
    rfac = [ [0, 1], [3, 1] ]
    rfac_degree = 1

    energies = {'ele55' : etamod_ele_sample_energies,
                'ele35' : etamod_ele_sample_energies,
                'ele37' : etamod_ele_sample_energies,
                'gam55' : etamod_gam_sample_energies,
                'gam35' : etamod_gam_sample_energies,
                'gam37' : etamod_gam_sample_energies,
                }

    correction = {'ele55' : etamod_v4_ele55_correction,
                  'ele35' : etamod_v4_ele35_correction,
                  'ele37' : etamod_v4_ele37_correction,
                  'gam55' : etamod_v4_gam55_correction,
                  'gam35' : etamod_v4_gam35_correction,
                  'gam37' : etamod_v4_gam37_correction,
                  }


# Rescale all the fits so that the integral over a cell is exactly 1.
def _etamod_rescale (pars, cellwidth = 0.025):
    scale = pars[0] + pars[1]*cellwidth/2 + pars[2]*cellwidth**2/3
    pars[0] = pars[0] /scale
    pars[1] = pars[1] /scale
    pars[2] = pars[2] /scale
    return
def _etamod_rescale_all(corr):
    for v in corr.values():
        for p in v:
            _etamod_rescale (p)
    return
_etamod_rescale_all (CaloSwEtamod_v4_parms.correction)
