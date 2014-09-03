# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##===============================================================================
## Name:        TrigEgammaElectronIsEMCutDefs_medium1.py
##
## Author:      Ryan Mackenzie White
## Created:     June 2013
##
## Description: Medium1 trigger electron cut definitions for 2012. 
##              Migrated from TrigEgammaElectronCutIDTool_medium1.py LowLumi_2012
##===============================================================================

import PyCintex
PyCintex.loadDictionary('ElectronPhotonSelectorToolsDict')
# PyCintex.loadDictionary('egammaEvent') Is this required?
from ROOT import egammaPID

# Import a needed helper
from PATCore.HelperUtils import *

# Define GeV
GeV = 1000.0
def TrigEgammaElectronIsEMSelectorConfig2012(theTool) :
    '''
    This is for the Loose isEM definitions for the Trigger.
    '''

    theTool = GetTool(theTool)

    theTool.PIDName = egammaPID.IsEMLoose


    #########################################################################################
    # TO BE USED FOR LOOSE ELECTRON ONY (2nd EM samplling and HadLeakage ONLY)
    # NB: MEDIUM AND TIGHT ELECTRONS ARE NO MORE SUPPORTED - REPLACED BY MEDIUM1,TIGHT1 MENU
    #########################################################################################
    # range of eta bins for e-ID
    theTool.CutBinEta = [0.1, 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47]

    # range of ET bins for e-ID
    theTool.CutBinET = [5.0*GeV, 10.0*GeV, 15.0*GeV, 20.0*GeV, 30.0*GeV, 40.0*GeV, 50.0*GeV, 60.0*GeV, 70.0*GeV, 80.0*GeV]

    # cut on fraction of energy deposited in 1st sampling
    theTool.CutF1 = [0.005]

    # cut on hadronic energy
    theTool.CutHadLeakage  = [0.12800, 0.15600, 0.15200, 0.11600, 0.15600, 0.04275, 0.13200, 0.15200, 0.15600, 0.14000,# < 5
                               0.12800, 0.15600, 0.15200, 0.11600, 0.15600, 0.04275, 0.13200, 0.15200, 0.15600, 0.14000, # 5-10
                               0.03225, 0.03225, 0.03075, 0.03575, 0.02575, 0.04275, 0.04325, 0.04525, 0.04325, 0.03675,  # 10-15
                               0.02925, 0.02925, 0.02775, 0.03175, 0.02375, 0.03875, 0.04025, 0.03425, 0.03825, 0.02975,  # 15-20
                               0.02425, 0.02425, 0.02275, 0.02575, 0.01975, 0.01975, 0.02725, 0.02725, 0.02725, 0.01975,  # 20-30
                               0.02275, 0.02275, 0.02125, 0.01975, 0.01825, 0.01825, 0.02425, 0.02575, 0.02425, 0.01675,  # 30-40
                               0.01825, 0.01825, 0.01975, 0.01525, 0.01675, 0.01675, 0.02125, 0.02275, 0.01975, 0.01675,  # 40-50
                               0.01825, 0.01825, 0.01975, 0.01525, 0.01675, 0.01675, 0.02125, 0.02275, 0.01975, 0.01675,  # 50-60
                               0.01825, 0.01825, 0.01975, 0.01525, 0.01675, 0.01675, 0.02125, 0.02275, 0.01975, 0.01675,  # 60-70
                               0.01825, 0.01825, 0.01975, 0.01525, 0.01675, 0.01675, 0.02125, 0.02275, 0.01975, 0.01675,  # 70-80
                               0.01825, 0.01825, 0.01975, 0.01525, 0.01675, 0.01675, 0.02125, 0.02275, 0.01975, 0.01675 # >80
                               ]  
     
                 
    # cut on ratio e237/e277
    theTool.CutReta37 = [0.6800, 0.5600, 0.6000, 0.6800, 0.7200, 0.440, 0.7600, 0.7200, 0.7600, 0.7475,  # < 5
                          0.6800, 0.5600, 0.6000, 0.6800, 0.7200, 0.440, 0.7600, 0.7200, 0.7600, 0.7475,  # 5-10
                          0.8475, 0.8475, 0.8425, 0.8175, 0.8475, 0.740, 0.8275, 0.8675, 0.8675, 0.7475, # 10-15
                          0.8675, 0.8675, 0.8675, 0.8475, 0.8725, 0.740, 0.8525, 0.8775, 0.8775, 0.7575, # 15-20
                          0.8825, 0.8825, 0.8825, 0.8575, 0.8875, 0.750, 0.8725, 0.9025, 0.8875, 0.7575, # 20-30
                          0.9075, 0.9075, 0.8975, 0.8875, 0.8875, 0.790, 0.8925, 0.9075, 0.8975, 0.8075, # 30-40
                          0.9175, 0.9175, 0.9125, 0.9075, 0.9025, 0.790, 0.8975, 0.9125, 0.9025, 0.8625, # 40-50
                          0.9175, 0.9175, 0.9125, 0.9075, 0.9025, 0.790, 0.8975, 0.9125, 0.9025, 0.8625, # 50-60
                          0.9175, 0.9175, 0.9125, 0.9075, 0.9025, 0.790, 0.8975, 0.9125, 0.9025, 0.8625, # 60-70
                          0.9175, 0.9175, 0.9125, 0.9075, 0.9025, 0.790, 0.8975, 0.9125, 0.9025, 0.8625, # 70-80
                          0.9175, 0.9175, 0.9125, 0.9075, 0.9025, 0.790, 0.8975, 0.9125, 0.9025, 0.8625 # >80          
                          ]
                          

    # cut on shower width in 2nd sampling
    theTool.CutWeta2c = [0.017, 0.016, 0.018, 0.016, 0.019, 0.031, 0.017, 0.016, 0.015, 0.0145,    # < 5
                          0.017, 0.016, 0.018, 0.016, 0.019, 0.031, 0.017, 0.016, 0.015, 0.0145,  # 5-10
                          0.014 ,0.014 ,0.015 ,0.016 ,0.017 ,0.025 ,0.017 ,0.015 ,0.015 ,0.0145,  #10-15
                          0.0135 ,0.0135 ,0.0145 ,0.016 ,0.017 ,0.025 ,0.017 ,0.015 ,0.015 ,0.0145,  #15-20
                          0.013 ,0.013 ,0.014 ,0.015 ,0.015 ,0.025 ,0.016 ,0.015 ,0.015 ,0.014 ,  #20-30
                          0.012 ,0.012 ,0.013 ,0.013 ,0.013 ,0.025 ,0.015 ,0.014 ,0.014 ,0.013 ,  #30-40
                          0.011 ,0.011 ,0.012 ,0.013 ,0.013 ,0.025 ,0.015 ,0.014 ,0.014 ,0.013 ,  #40-50
                          0.011 ,0.011 ,0.012 ,0.013 ,0.013 ,0.025 ,0.015 ,0.014 ,0.014 ,0.013, # 50-60
                          0.011 ,0.011 ,0.012 ,0.013 ,0.013 ,0.025 ,0.015 ,0.014 ,0.014 ,0.013, # 60-70
                          0.011 ,0.011 ,0.012 ,0.013 ,0.013 ,0.025 ,0.015 ,0.014 ,0.014 ,0.013, # 70-80
                          0.011 ,0.011 ,0.012 ,0.013 ,0.013 ,0.025 ,0.015 ,0.014 ,0.014 ,0.013 # 80<  
                          ]           

    
    # cut on total width in 1st sampling
    theTool.CutWtot  = []
            
    # cut on (Emax - Emax2)/(Emax + Emax2) in 1st sampling 
    theTool.CutDEmaxs1 = []
 
    # cut on Delta Emax2 in 1st sampling 
    theTool.CutDeltaEmax2  = []
        
    # cut on Emax2 - Emin in 1st sampling 
    theTool.CutDeltaE  = []
        
    # cut on width in 1st sampling
    theTool.CutWeta1c  = []
        
    # cut on Fside in 1st sampling
    theTool.CutFracm = []
        
    # cut on Energy in cone of 0.20
    # theTool.CutClusterIsolation = []
        
    # cut on Track quality cut
        
    # cut on pixel-layer hits
    theTool.CutPi  = []
        
    # cut on precision hits
    theTool.CutSi  = []
        
    # cut on transverse impact parameter
    theTool.CutA0  = []
        
    # cut on transverse impact parameter for tight selection   	 	 
    theTool.CutA0Tight  = []
        
    # cut on delta eta
    theTool.CutDeltaEta = []
        
    # cut on delta eta for tight selection
    theTool.CutDeltaEtaTight = []
        
    theTool.useTRTOutliers  = True
    #use outliers for BL, Pix and SCT tracks
    theTool.useBLOutliers  = True
    theTool.usePIXOutliers  = True
    theTool.useSCTOutliers  = True
 

    # cut on b-layer hits
    theTool.CutBL  = []

    # cut on min delta phi
    theTool.CutminDeltaPhi  = []

    # cut on max delta phi
    theTool.CutmaxDeltaPhi  = []

    # cut min on E/P
    theTool.CutminEp  = []
        
    # cut max on E/P
    theTool.CutmaxEp  = []
        
    # cuts on TRT
    # range of eta bins for e-ID for TRT
    theTool.CutBinEta_TRT = []
    # cuts on Number of TRT hits with Outliers
    theTool.CutNumTRT  = []
    # cuts on TRT ratio with Outliers
    theTool.CutTRTRatio  = []
    # cuts on TRT ratio with Outliers for 90% efficiency
    theTool.CutTRTRatio90  = []
