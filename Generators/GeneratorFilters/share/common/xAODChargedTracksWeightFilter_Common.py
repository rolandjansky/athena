# common fragment for xAODChargedTracksWeight filter
# conversion to XAOD, 
# creation of slimmed container containing truth events
# connecting the filter

include ("GeneratorFilters/CreatexAODSlimContainers.py")

from GeneratorFilters.GeneratorFiltersConf import xAODChargedTracksWeightFilter
xAODChargedTracksWeightFilter = xAODChargedTracksWeightFilter("xAODChargedTracksWeightFilter")  
filtSeq += xAODChargedTracksWeightFilter

# to modiify cuts put into JOs e.g.:
#filtSeq.xAODChargedTracksWeightFilter.NchMin = 7
#filtSeq.xAODChargedTracksWeightFilter.NchMax = 20
#filtSeq.xAODChargedTracksWeightFilter.Ptcut = 12000.0
#filtSeq.xAODChargedTracksWeightFilter.Etacut= 2.5
#filtSeq.xAODChargedTracksWeightFilter.SplineX =[ 0, 2, 5, 10, 20, 30, 35 ]
#filtSeq.xAODChargedTracksWeightFilter.SplineY =[ 4, 4, 9, 13, 17, 18, 18 ]


