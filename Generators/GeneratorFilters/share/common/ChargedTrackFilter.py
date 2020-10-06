## Default truth ncharged filter setup
      
from GeneratorFilters.GeneratorFiltersConf import ChargedTracksFilter             
if "ChargedTracksFilter" not in filtSeq:
   filtSeq += ChargedTracksFilter()
 
filtSeq.ChargedTracksFilter.Ptcut = 100.
filtSeq.ChargedTracksFilter.Etacut = 2.5

  
