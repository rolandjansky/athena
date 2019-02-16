# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# default configuration of the PhotonIsEMSelectorCutDefs
# This one is used for stadard photons cuts menus

import cppyy
try :
    cppyy.loadDictionary('ElectronPhotonSelectorToolsDict')
except :
    pass

from ROOT import egammaPID

# Import a needed helper
from PATCore.HelperUtils import *

## obsolete the old cut-def should be updated with the new selectors.
def PhotonIsEMSelectorConfig2011(theTool) :
    '''
    These are the photon isEM definitions
    '''
    
    theTool = GetTool(theTool)

    #
    # PHOTON cuts (new menu optimized menus for 16.6.X.Y ( 2011 ) )
    # is even "more robust" for the Tight and Loose part
    #

    #
    # LOOSE (14/6/2009 -- Marco.Delmastro@cern.ch)
    # + changes to mitiage large systematic at high eta and to recover efficiency lost at eta(0.8-1.115) LOOSE (27/01/2011 -- habreu@cern.ch)
    #

    # range of eta and ET for Loose selection
    theTool.CutBinEta_photonsLoose      += [0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47]
    theTool.CutBinEnergy_photonsLoose   += []
        
    # cut on hadronic energy
    theTool.CutHadLeakage_photonsLoose  += [ 0.010, 0.010, 0.010, 0.008, 0.010, 0.025, 0.015, 0.014, 0.019 ]

    # WARNING: format is: first full eta (number of bins taken from size of empid.CutHadLeakage_photons, then energy 
    theTool.e277_photonsLoose   += [ 0.1 ]
    # Reta37 for loose photons
    theTool.Reta37_photonsLoose += [ 0.927, 0.912, 0.925, 0.916, 0.750, 0.906, 0.920, 0.908, 0.915 ]
    # width in 2nd sampling for loose photons
    theTool.weta2_photonsLoose  += [ 0.012, 0.012, 0.013, 0.013, 0.025, 0.015, 0.013, 0.013, 0.012 ]

    #
    # TIGHT UNCONVERTED (29/1/2010 -- Valeria Perez.Real@cern.ch)
    # + changes to make menu "robust" (relax reta, weta) (9/9/2010 -- Marco.Delmastro@cern.ch)
    # + changes to mitigate the large systematics observed in the high eta region (27/01/2011 -- habreu@cern.ch)
    #
        
    # range of eta and ET for Tight selection for non converted photons
    theTool.CutBinEta_photonsNonConverted += [0.6, 0.8,1.15, 1.37,1.52, 1.81, 2.01, 2.37,2.47]
    theTool.CutBinEnergy_photonsNonConverted += []
    # cut on hadronic energy
    # WARNING: format is: first full eta (number of bins taken from size of empid.CutHadLeakage_photons, then energy
    theTool.CutHadLeakage_photonsNonConverted += [0.0089,0.007,0.006,0.008,-9999.,0.019,0.015,0.0137,-9999.]
    # E277
    theTool.e277_photonsNonConverted  += [0.1]
    # Reta37 for non converted photons
    ### theTool.Reta37_photonsNonConverted += [0.955,0.946,0.948,0.952,9999.,0.941,0.948,0.935,9999.] # OLD
    theTool.Reta37_photonsNonConverted += [ 0.950784, 0.9398, 0.9418, 0.9458, 9999., 0.932066, 0.928, 0.924, 9999. ] # "Robust"
        
    # Rphi33 for non converted photons
    theTool.Rphi33_photonsNonConverted += [0.954,0.95,0.59,0.82,9999.,0.93,0.947,0.935,9999.]
    # Width in 2nd sampling for non converted photons
    ### theTool.weta2_photonsNonConverted += [0.0105,0.0112,0.0105,0.0111,-9999.,0.011,0.0111,0.0125,-9999.] # OLD
    theTool.weta2_photonsNonConverted += [ 0.0107194, 0.011459, 0.010759, 0.011359, -9999., 0.0114125, 0.0110, 0.0125, -9999.] # "Robust" 
        
    # eta binning in strips
    theTool.CutBinEtaStrips_photonsNonConverted += [0.6, 0.8,1.15, 1.37,1.52, 1.81, 2.01, 2.37, 2.47]
    # ET binning in strips
    theTool.CutBinEnergyStrips_photonsNonConverted += []
        
    # fraction of energy in strips for non converted photons
    theTool.f1_photonsNonConverted += [0.005]
    # Delta E for non converted photons
    theTool.deltae_photonsNonConverted += [92,92,99,111,-9999.,92,110,148,-9999.]
    # cut on (Emax - Emax2)/(Emax + Emax2) in 1st sampling
    theTool.DEmaxs1_photonsNonConverted += [0.63,0.84,0.823,0.887,9999.,0.88,0.710,0.780,9999.]
    # Total width in strips for non converted photons
    theTool.wtot_photonsNonConverted += [2.95,4.4,3.26,3.4,-9999.,3.8,2.4,1.64,-9999.]
    # Fracm for non converted photons
    theTool.fracm_photonsNonConverted += [0.284,0.36,0.36,0.514,-9999.,0.67,0.211,0.181,-9999.]
    # Width in three strips for non converted photons
    theTool.w1_photonsNonConverted += [0.66,0.69,0.697,0.81,-9999.,0.73,0.651,0.610,-9999.]

    #
    # TIGHT CONVERTED (28/1/2010 -- Giovanni.Marchiori@lpnhe.in2p3.fr)
    # + changes to make menu "robust" (relax reta, weta) (9/9/2010 -- Marco.Delmastro@cern.ch)
    # + changes to mitigate the large systematics observed in the high eta region (27/01/2011 -- habreu@cern.ch)
    #

    # range of eta and ET for Tight selection for converted photons
    theTool.CutBinEta_photonsConverted    += [0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47]
    theTool.CutBinEnergy_photonsConverted += []

    # cut on hadronic energy
    # WARNING: format is: first full eta (number of bins taken from size of empid.CutHadLeakage_photons, then energy
    theTool.CutHadLeakage_photonsConverted += [0.00748,0.00700,0.00489,0.00800,-9999.,0.01490,0.015,0.011,-9999.]

    theTool.e277_photonsConverted  += [0.1]
    # Reta37 for converted photons
    ### theTool.Reta37_photonsConverted += [0.945,0.933,0.936,0.937,9999.,0.927,0.938,0.922,9999.] # OLD
    theTool.Reta37_photonsConverted += [0.940784, 0.9268, 0.9298, 0.9308, 9999., 0.918066, 0.924, 0.913, 9999.] # "Robust"
            
    # Rphi33 for converted photons
    theTool.Rphi33_photonsConverted += [0.400,0.426,0.493,0.437,9999.,0.535,0.479,0.692,9999.]

    # width in 2nd sampling for converted photons
    ### theTool.weta2_photonsConverted += [0.0114,0.0111,0.0126,0.0124,-9999.,0.0134,0.0118,0.0125,-9999.] # OLD
    theTool.weta2_photonsConverted += [0.0116194, 0.011359, 0.012859, 0.012659, -9999., 0.0138125, 0.012, 0.0129, -9999.] # "Robust"
        
    # eta binning in strips
    theTool.CutBinEtaStrips_photonsConverted    += [0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47]
    # ET binning in strips
    theTool.CutBinEnergyStrips_photonsConverted += []

    # cuts for converted photon in strips
    theTool.f1_photonsConverted  += [0.005]

    # Delta E for converted photons
    theTool.deltae_photonsConverted += [200,200,122,86,-9999.,123,80,132,-9999.]

    # cut on (Emax - Emax2)/(Emax + Emax2) in 1st sampling
    theTool.DEmaxs1_photonsConverted += [0.908,0.911,0.808,0.803,9999.,0.67,0.915,0.962,9999.]

    # total width in strips for converted photons
    theTool.wtot_photonsConverted += [2.8,2.95,2.89,3.14,-9999.,3.7,2.0,1.48,-9999.]

    # Fracm for converted photons
    theTool.fracm_photonsConverted += [0.32,0.428,0.483,0.51,-9999.,0.508,0.252,0.215,-9999.]

    # width in three strips for converted photon
    theTool.w1_photonsConverted += [0.697,0.709,0.749,0.78,-9999.,0.773,0.672,0.644,-9999.] 

    # E/p min for converted photons
    theTool.CutminEp_photonsConverted += [0.,0.,0.,0.,0.,0.,0.,0.,0.]

    # E/p max for converted photons
    theTool.CutmaxEp_photonsConverted += [100.,100.,100.,100.,100.,100.,100.,100.,100.]



def PhotonIsEMSelectorConfig2012(theTool) :
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


def PhotonIsEMSelectorConfig2012OLD(theTool) :
    '''
    These are the photon isEM definitions
    '''
    
    theTool = GetTool(theTool)

    #
    # PHOTON cuts, with loose updated for 2012. (Tight remains the same as 2011 for now)
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
    # TIGHT UNCONVERTED (29/1/2010 -- Valeria Perez.Real@cern.ch)
    # + changes to make menu "robust" (relax reta, weta) (9/9/2010 -- Marco.Delmastro@cern.ch)
    # + changes to mitigate the large systematics observed in the high eta region (27/01/2011 -- habreu@cern.ch)
    #
        
    # range of eta and ET for Tight selection for non converted photons
    theTool.CutBinEta_photonsNonConverted += [0.6, 0.8,1.15, 1.37,1.52, 1.81, 2.01, 2.37,2.47]
    theTool.CutBinEnergy_photonsNonConverted += []
    # cut on hadronic energy
    # WARNING: format is: first full eta (number of bins taken from size of empid.CutHadLeakage_photons, then energy
    theTool.CutHadLeakage_photonsNonConverted += [0.0089,0.007,0.006,0.008,-9999.,0.019,0.015,0.0137,-9999.]
    # E277
    theTool.e277_photonsNonConverted  += [0.1]
    # Reta37 for non converted photons
    ### self.Reta37_photonsNonConverted += [0.955,0.946,0.948,0.952,9999.,0.941,0.948,0.935,9999.] # OLD
    theTool.Reta37_photonsNonConverted += [ 0.950784, 0.9398, 0.9418, 0.9458, 9999., 0.932066, 0.928, 0.924, 9999. ] # "Robust"
        
    # Rphi33 for non converted photons
    theTool.Rphi33_photonsNonConverted += [0.954,0.95,0.59,0.82,9999.,0.93,0.947,0.935,9999.]
    # Width in 2nd sampling for non converted photons
    ### self.weta2_photonsNonConverted += [0.0105,0.0112,0.0105,0.0111,-9999.,0.011,0.0111,0.0125,-9999.] # OLD
    theTool.weta2_photonsNonConverted += [ 0.0107194, 0.011459, 0.010759, 0.011359, -9999., 0.0114125, 0.0110, 0.0125, -9999.] # "Robust" 
        
    # eta binning in strips
    theTool.CutBinEtaStrips_photonsNonConverted += [0.6, 0.8,1.15, 1.37,1.52, 1.81, 2.01, 2.37, 2.47]
    # ET binning in strips
    theTool.CutBinEnergyStrips_photonsNonConverted += []
        
    # fraction of energy in strips for non converted photons
    theTool.f1_photonsNonConverted += [0.005]
    # Delta E for non converted photons
    theTool.deltae_photonsNonConverted += [92,92,99,111,-9999.,92,110,148,-9999.]
    # cut on (Emax - Emax2)/(Emax + Emax2) in 1st sampling
    theTool.DEmaxs1_photonsNonConverted += [0.63,0.84,0.823,0.887,9999.,0.88,0.710,0.780,9999.]
    # Total width in strips for non converted photons
    theTool.wtot_photonsNonConverted += [2.95,4.4,3.26,3.4,-9999.,3.8,2.4,1.64,-9999.]
    # Fracm for non converted photons
    theTool.fracm_photonsNonConverted += [0.284,0.36,0.36,0.514,-9999.,0.67,0.211,0.181,-9999.]
    # Width in three strips for non converted photons
    theTool.w1_photonsNonConverted += [0.66,0.69,0.697,0.81,-9999.,0.73,0.651,0.610,-9999.]

    #
    # TIGHT CONVERTED (28/1/2010 -- Giovanni.Marchiori@lpnhe.in2p3.fr)
    # + changes to make menu "robust" (relax reta, weta) (9/9/2010 -- Marco.Delmastro@cern.ch)
    # + changes to mitigate the large systematics observed in the high eta region (27/01/2011 -- habreu@cern.ch)
    #

    # range of eta and ET for Tight selection for converted photons
    theTool.CutBinEta_photonsConverted    += [0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47]
    theTool.CutBinEnergy_photonsConverted += []

    # cut on hadronic energy
    # WARNING: format is: first full eta (number of bins taken from size of empid.CutHadLeakage_photons, then energy
    theTool.CutHadLeakage_photonsConverted += [0.00748,0.00700,0.00489,0.00800,-9999.,0.01490,0.015,0.011,-9999.]

    theTool.e277_photonsConverted  += [0.1]
    # Reta37 for converted photons
    ### self.Reta37_photonsConverted += [0.945,0.933,0.936,0.937,9999.,0.927,0.938,0.922,9999.] # OLD
    theTool.Reta37_photonsConverted += [0.940784, 0.9268, 0.9298, 0.9308, 9999., 0.918066, 0.924, 0.913, 9999.] # "Robust"
            
    # Rphi33 for converted photons
    theTool.Rphi33_photonsConverted += [0.400,0.426,0.493,0.437,9999.,0.535,0.479,0.692,9999.]
    
    # width in 2nd sampling for converted photons
    ### self.weta2_photonsConverted += [0.0114,0.0111,0.0126,0.0124,-9999.,0.0134,0.0118,0.0125,-9999.] # OLD
    theTool.weta2_photonsConverted += [0.0116194, 0.011359, 0.012859, 0.012659, -9999., 0.0138125, 0.012, 0.0129, -9999.] # "Robust"
        
    # eta binning in strips
    theTool.CutBinEtaStrips_photonsConverted    += [0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47]
    # ET binning in strips
    theTool.CutBinEnergyStrips_photonsConverted += []

    # cuts for converted photon in strips
    theTool.f1_photonsConverted  += [0.005]

    # Delta E for converted photons
    theTool.deltae_photonsConverted += [200,200,122,86,-9999.,123,80,132,-9999.]

    # cut on (Emax - Emax2)/(Emax + Emax2) in 1st sampling
    theTool.DEmaxs1_photonsConverted += [0.908,0.911,0.808,0.803,9999.,0.67,0.915,0.962,9999.]

    # total width in strips for converted photons
    theTool.wtot_photonsConverted += [2.8,2.95,2.89,3.14,-9999.,3.7,2.0,1.48,-9999.]

    # Fracm for converted photons
    theTool.fracm_photonsConverted += [0.32,0.428,0.483,0.51,-9999.,0.508,0.252,0.215,-9999.]

    # width in three strips for converted photon
    theTool.w1_photonsConverted += [0.697,0.709,0.749,0.78,-9999.,0.773,0.672,0.644,-9999.] 

    # E/p min for converted photons
    theTool.CutminEp_photonsConverted += [0.,0.,0.,0.,0.,0.,0.,0.,0.]

    # E/p max for converted photons
    theTool.CutmaxEp_photonsConverted += [100.,100.,100.,100.,100.,100.,100.,100.,100.]

    #
    # end of LowLumi_2012()
    #

