# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# default configuration of the PhotonIsEMSelectorCutDefs
# This one is used for stadard photons cuts menus


# Import a needed helper
from PATCore.HelperUtils import *


def PhotonIsEMMediumSelectorConfigDC14(theTool) :
    '''
    These are the photon isEM definitions from *DC14*
    '''
    
    theTool = GetTool(theTool)

    # MEDIUM (20/11/2013 -- Ludovica@cern.ch)
    #  Coming from Fer
    #
    theTool.ConfigFile = "ElectronPhotonSelectorTools/offline/mc15_20150712/PhotonIsEMMediumSelectorCutDefs.conf"


def PhotonIsEMMediumSelectorConfig2012(theTool) :
    '''
    These are the photon isEM definitions
    '''
    
    theTool = GetTool(theTool)

    # MEDIUM (25/3/2012 -- Jovan.Mitrevski@cern.ch)
    #  Coming from the trigger
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
    theTool.weta2_photonsLoose  += [ 0.011,  0.012,  0.012, 0.0125, 0.025, 0.013, 0.012, 0.0128, 0.0128 ]

    # TIGHT UNCONVERTED 
    
    # range of eta and ET for Tight selection for non converted photons
    theTool.CutBinEta_photonsNonConverted += [0.6, 0.8,1.15, 1.37,1.52, 1.81, 2.01, 2.37,2.47]
    theTool.CutBinEnergy_photonsNonConverted += []
        
    # E277
    theTool.e277_photonsNonConverted  += [0.1]

    # eta binning in strips
    theTool.CutBinEtaStrips_photonsNonConverted += [0.6, 0.8,1.15, 1.37,1.52, 1.81, 2.01, 2.37, 2.47]
    # ET binning in strips
    theTool.CutBinEnergyStrips_photonsNonConverted += []
        
    # fraction of energy in strips for non converted photons
    theTool.f1_photonsNonConverted += [0.005]

    # cut on (Emax - Emax2)/(Emax + Emax2) in 1st sampling
    theTool.DEmaxs1_photonsNonConverted += [0.60,  0.76,  0.70, 0.67,  0.68  ,  0.65,  0.68 ,  0.75,  0.78]

    #
    # TIGHT CONVERTED 


    # range of eta and ET for Tight selection for converted photons
    theTool.CutBinEta_photonsConverted    += [0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47]
    theTool.CutBinEnergy_photonsConverted += []

    # E277
    theTool.e277_photonsConverted  += [0.1]

    # eta binning in strips
    theTool.CutBinEtaStrips_photonsConverted    += [0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47]
    # ET binning in strips
    theTool.CutBinEnergyStrips_photonsConverted += []

    # cuts for converted photon in strips
    theTool.f1_photonsConverted  += [0.005]

    # cut on (Emax - Emax2)/(Emax + Emax2) in 1st sampling
    theTool.DEmaxs1_photonsConverted += [0.60,  0.76,  0.70, 0.67,  0.68  ,  0.65,  0.68 ,  0.75,  0.78]

    # end of LowLumi_2012()
    #



