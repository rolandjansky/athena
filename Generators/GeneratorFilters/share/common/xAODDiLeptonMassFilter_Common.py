# common fragment for xAODDiLeptonMass filter
# conversion to XAOD, 
# creation of slimmed container containing electrons
# connecting the filter

include ("GeneratorFilters/CreatexAODSlimContainers.py")
createxAODSlimmedContainer("TruthLightLeptons",prefiltSeq)
prefiltSeq.xAODCnv.AODContainerName = 'GEN_EVENT'

from GeneratorFilters.GeneratorFiltersConf import xAODDiLeptonMassFilter
xAODSDiLeptonMassFilter = xAODDiLeptonMassFilter("xAODDiLeptonMassFilter")
filtSeq += xAODDiLeptonMassFilter

# to modiify cuts put into JOs e.g.:
#filtSeq.xAODDiLeptonMassFilter.Ptcut = 12000.0
#filtSeq.xAODDiLeptonMassFilter.MaxEta = 2.7
#filtSeq.xAODDiLeptonMassFilter.MinMass = 1000
#filtSeq.xAODDiLeptonMassFilter.MaxMass = 45000
#filtSeq.xAODDiLeptonMassFilter.MinDilepPt = -1
#filtSeq.xAODDiLeptonMassFilter.AllowElecMu =  False
#filtSeq.xAODDiLeptonMassFilter.AllowSameCharge = False


