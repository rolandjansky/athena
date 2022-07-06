# common fragment for xAODLeptonPair filter
# conversion to XAOD, 
# creation of slimmed container containing truth events
# connecting the filter

include ("GeneratorFilters/CreatexAODSlimContainers.py")

from GeneratorFilters.GeneratorFiltersConf import xAODLeptonPairFilter
xAODLeptonPairFilter = xAODLeptonPairFilter("xAODLeptonPairFilter")
filtSeq += xAODLeptonPairFilter

# to modiify cuts put into JOs e.g.:
#filtSeq.xAODLeptonPairFilter.Ptcut = 12000.0
#filtSeq.xAODLeptonPairFilter.Etacut = 10.0
#filtSeq.xAODLeptonPairFilter.NLeptons_Max=-1 # if max < 0 it is ignored
#filtSeq.xAODLeptonPairFilter.NLeptons_Min=0
#for more settable parameters see: 
#https://gitlab.cern.ch/atlas/athena/-/blob/master/Generators/GeneratorFilters/GeneratorFilters/xAODLeptonPairFilter.h

