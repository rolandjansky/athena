# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration


def KeepMonitoring(chainName, ChainsToKeepMonitoring, strictComparison = False):
  retval = False
  for chainToKeep in ChainsToKeepMonitoring:
    if strictComparison:
       retval = (chainName==chainToKeep)
    else:
       retval = ( chainName.find(chainToKeep) > -1 )
    if ( retval ) : break
  return retval

def DisableMonitoringButValAndTime(list_in):
   list_output=[]
   for item in list_in:
      target = item.target()
      if ( isinstance (target, str) ) :
        if ( (target == "Validation") or (target == "Time") ):
          list_output+=[item]
      elif ( isinstance (target, list) ):
        for t in target:
            if ( (t == "Validation") or (t == "Time") ):
               list_output+=[item]
   return list_output
