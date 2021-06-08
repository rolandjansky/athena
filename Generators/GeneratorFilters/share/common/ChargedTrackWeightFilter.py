## Default truth ncharged filter setup
      
from GeneratorFilters.GeneratorFiltersConf import ChargedTracksWeightFilter             
if "ChargedTracksWeightFilter" not in filtSeq:
   filtSeq += ChargedTracksWeightFilter()
 
filtSeq.ChargedTracksWeightFilter.Ptcut = 100.
filtSeq.ChargedTracksWeightFilter.Etacut = 2.5

  
