# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ElectronPhotonSelectorTools.TrigEGammaPIDdefs import BitDefElectron, CutDefElectron




class SelectionDefElectron:
    """ @brief Loose definition for HI """
    ElectronLooseHI = \
        1 << BitDefElectron.ClusterEtaRange_Electron | \
        1 << BitDefElectron.ClusterHadronicLeakage_Electron | \
        1 << BitDefElectron.ClusterMiddleEnergy_Electron | \
        1 << BitDefElectron.ClusterMiddleEratio37_Electron | \
        1 << BitDefElectron.ClusterMiddleWidth_Electron | \
        1 << BitDefElectron.ClusterStripsWtot_Electron 
    
    ElectronMediumHI = ElectronLooseHI |\
        1 << BitDefElectron.ClusterMiddleEratio33_Electron | \
        1 << BitDefElectron.ClusterBackEnergyFraction_Electron | \
        1 << BitDefElectron.ClusterStripsEratio_Electron | \
        1 << BitDefElectron.ClusterStripsDeltaEmax2_Electron | \
        1 << BitDefElectron.ClusterStripsDeltaE_Electron | \
        1 << BitDefElectron.ClusterStripsFracm_Electron | \
        1 << BitDefElectron.ClusterStripsWeta1c_Electron 


ElectronIsEMBits = {'loose': SelectionDefElectron.ElectronLooseHI, 
                    'loose1': SelectionDefElectron.ElectronLooseHI, 
                    'medium': SelectionDefElectron.ElectronMediumHI,
                    'medium1': SelectionDefElectron.ElectronMediumHI}

print ""
print "============================================================================"
print "====                       TrigEgamma isEM bit masks HI                ====="
print "====  ElectronLoose:                              0x%08x               =====" % SelectionDefElectron.ElectronLooseHI
print "====  ElectronMedium:                             0x%08x               =====" % SelectionDefElectron.ElectronMediumHI
print "==============================================================================="
print ""
