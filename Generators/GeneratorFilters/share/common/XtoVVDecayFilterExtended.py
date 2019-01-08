if not hasattr(filtSeq, "XtoVVDecayFilterExtended"):
    from GeneratorFilters.GeneratorFiltersConf import XtoVVDecayFilterExtended
    filtSeq += XtoVVDecayFilterExtended()

## Default cut params
filtSeq.XtoVVDecayFilterExtended.PDGGrandParent = 25                  
filtSeq.XtoVVDecayFilterExtended.PDGParent = 24                       
filtSeq.XtoVVDecayFilterExtended.StatusParent = 22                    
filtSeq.XtoVVDecayFilterExtended.PDGChild1 = [11,12,13,14,15,16]      
filtSeq.XtoVVDecayFilterExtended.PDGChild2 = [1,2,3,4,5,6]            
