# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# default configuration of the PhotonIsEMSelectorCutDefs
# This one is used for stadard photons cuts menus

import PyCintex
PyCintex.loadDictionary('ElectronPhotonSelectorToolsDict')

from ROOT import egammaPID

# Import a needed helper
from PATCore.HelperUtils import *

# Define GeV
GeV = 1000.0

def TrigEgammaPhotonIsEMSelectorConfig_tight(theTool) :
    '''
    These are the photon isEM definitions
    '''
    
    theTool = GetTool(theTool)

    # the isEM name
    theTool.PIDName = egammaPID.IsEM

    #
    # PHOTON cuts, with updated for 2012.
    #
    
    #
    # LOOSE (25/3/2012 -- Jovan.Mitrevski@cern.ch)
    #  Modified from 2011 menu for the trigger
    #

    # range of eta and ET for Loose selection
    theTool.CutBinEta_photonsLoose      += [0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47]
    theTool.CutBinEnergy_photonsLoose   += []
        
    # cut on hadronic energy
    theTool.CutHadLeakage_photonsLoose  += [ 0.02425, 0.02275, 0.02575, 0.01975, 0.01975, 0.02725, 0.02725, 0.02725, 0.01975 ]

    # WARNING: format is: first full eta (number of bins taken from size of empid.CutHadLeakage_photons, then energy 
    theTool.e277_photonsLoose   += [ 0.1 ]
    # Reta37 for loose photons
    theTool.Reta37_photonsLoose += [ 0.8825, 0.8825, 0.8575, 0.8875, 0.750, 0.8725, 0.9025, 0.8875, 0.7575 ]
    # width in 2nd sampling for loose photons
    theTool.weta2_photonsLoose  += [ 0.013 ,0.014 ,0.015 ,0.015 ,0.025 ,0.016 ,0.015 ,0.015 ,0.014 ]

    #
    # TIGHT UNCONVERTED (2012 menu, copied by J. Mitrevski from PhotonIDTool "2012" menu by J. Saxon)
    #
        
    # range of eta and ET for Tight selection for non converted photons
    theTool.CutBinEta_photonsNonConverted += [0.6, 0.8,1.15, 1.37,1.52, 1.81, 2.01, 2.37,2.47]
    theTool.CutBinEnergy_photonsNonConverted += [80.*GeV]
    # cut on hadronic energy
    # WARNING: format is: first full eta (number of bins taken from size of empid.CutHadLeakage_photons, then energy
    theTool.CutHadLeakage_photonsNonConverted += [0.020,   0.020,   0.01975, 0.01825, -9999., 0.02425, 0.02575, 0.02325, -9999.,
                                                 0.01825, 0.01975, 0.01525, 0.01675, -9999., 0.02125, 0.02275, 0.01975, -9999. ]        
    # E277
    theTool.e277_photonsNonConverted  += [0.1]
    # Reta37 for non converted photons
    theTool.Reta37_photonsNonConverted += [ 0.92,    0.92,    0.93,   0.925, 9999.,   0.925,   0.925,   0.910, 9999.,
                                           0.92,    0.92,    0.93,   0.925, 9999.,   0.925,   0.925,   0.910, 9999.] 
        
    # Rphi33 for non converted photons
    theTool.Rphi33_photonsNonConverted += [  0.93,    0.93,    0.93,    0.92, 9999.,    0.93,    0.93,    0.93, 9999.,
                                            0.93,    0.93,    0.93,    0.92, 9999.,    0.93,    0.93,    0.93, 9999.]

    # Width in 2nd sampling for non converted photons
    theTool.weta2_photonsNonConverted += [  0.011,  0.0115,  0.0115,  0.0115, -9999.,   0.012,   0.012,  0.0128, -9999.,
                                           0.011,  0.0115,  0.0115,  0.0115, -9999.,   0.012,   0.012,  0.0128, -9999.]
        
    # eta binning in strips
    theTool.CutBinEtaStrips_photonsNonConverted += [0.6, 0.8,1.15, 1.37,1.52, 1.81, 2.01, 2.37, 2.47]
    # ET binning in strips
    theTool.CutBinEnergyStrips_photonsNonConverted += []
    
    # fraction of energy in strips for non converted photons
    theTool.f1_photonsNonConverted += [0.005]
    # Delta E for non converted photons
    theTool.deltae_photonsNonConverted += [ 180.,     170.,     165.,     160., -9999.,     425.,     500.,     560., -9999. ]
    # cut on (Emax - Emax2)/(Emax + Emax2) in 1st sampling
    theTool.DEmaxs1_photonsNonConverted += [ 0.80,    0.80,    0.76,    0.82, 9999.,    0.78,    0.80,    0.80, 9999. ]
    # Total width in strips for non converted photons
    theTool.wtot_photonsNonConverted += [ 3.0,     3.0,     3.3,     3.5, -9999.,     3.3,     2.3,     2.0, -9999.]
    # Fracm for non converted photons
    theTool.fracm_photonsNonConverted += [ 0.28,    0.33,    0.38,   0.425, -9999.,    0.42,   0.255,    0.24, -9999. ]
    # Width in three strips for non converted photons
    theTool.w1_photonsNonConverted += [ 0.67,    0.69,    0.69,   0.715, -9999.,    0.72,    0.66,   0.645, -9999.]
    
    #
    # TIGHT CONVERTED (2012 menu, copied by J. Mitrevski from PhotonIDTool "2012" menu by J. Saxon)
    #
    
    # range of eta and ET for Tight selection for converted photons
    theTool.CutBinEta_photonsConverted    += [0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47]
    theTool.CutBinEnergy_photonsConverted += [80.*GeV]

    # cut on hadronic energy
    # WARNING: format is: first full eta (number of bins taken from size of empid.CutHadLeakage_photons, then energy
    theTool.CutHadLeakage_photonsConverted += [ 0.020,   0.018,   0.01975,   0.018, -9999., 0.02425,   0.024,   0.024, -9999.,
                                               0.01825, 0.01975, 0.01525, 0.01675, -9999., 0.02125, 0.02275, 0.01975, -9999.]
    
    theTool.e277_photonsConverted  += [0.1]
    # Reta37 for converted photons
    theTool.Reta37_photonsConverted += [ 0.92,  0.9125,   0.915,    0.91, 9999.,   0.908,    0.917,   0.903, 9999., 
                                        0.92,  0.9125,   0.915,    0.91, 9999.,   0.908,    0.917,   0.903, 9999. ] 
            
        # Rphi33 for converted photons
    theTool.Rphi33_photonsConverted += [ 0.57,    0.60,    0.60,    0.64, 9999.,    0.68,    0.72,    0.72, 9999.,
                                        0.57,    0.60,    0.60,    0.64, 9999.,    0.68,    0.72,    0.72, 9999. ]

        # width in 2nd sampling for converted photons
    theTool.weta2_photonsConverted += [ 0.011,  0.0117,   0.012,  0.0120, -9999.,  0.0130,   0.012,  0.0127, -9999.,
                                       0.011,  0.0117,   0.012,  0.0120, -9999.,  0.0130,   0.012,  0.0127, -9999. ]
        
        # eta binning in strips
    theTool.CutBinEtaStrips_photonsConverted    += [0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47]
        # ET binning in strips
    theTool.CutBinEnergyStrips_photonsConverted += []

        # cuts for converted photon in strips
    theTool.f1_photonsConverted  += [0.005]

        # Delta E for converted photons
    theTool.deltae_photonsConverted += [ 160.,     160.,     120.,     125., -9999.,     350.,     520.,     525., -9999.]

        # cut on (Emax - Emax2)/(Emax + Emax2) in 1st sampling
    theTool.DEmaxs1_photonsConverted += [ 0.85,    0.85,    0.80,    0.78, 9999.,    0.82,    0.86,    0.88, 9999.]

        # total width in strips for converted photons
    theTool.wtot_photonsConverted += [ 2.8,     2.9,     3.1,     3.3, -9999.,     3.5,     2.2,     1.8, -9999.]

        # Fracm for converted photons
    theTool.fracm_photonsConverted += [ 0.33,    0.38,    0.46,    0.52, -9999.,    0.52,    0.31,    0.25, -9999.]

        # width in three strips for converted photon
    theTool.w1_photonsConverted += [ 0.73,   0.715,    0.74,    0.75, -9999.,    0.75,    0.69,    0.66, -9999.] 

        # E/p min for converted photons
    theTool.CutminEp_photonsConverted += [0.,0.,0.,0.,0.,0.,0.,0.,0.,
                                         0.,0.,0.,0.,0.,0.,0.,0.,0.]

        # E/p max for converted photons
    theTool.CutmaxEp_photonsConverted += [100.,100.,100.,100.,100.,100.,100.,100.,100.,
                                         100.,100.,100.,100.,100.,100.,100.,100.,100.]


