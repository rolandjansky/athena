# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

print "WARNING RecExConfig.Resilience obsolete. Please use AthenaCommon.Resilience !"
# usage treatException(" problem with bla ")
# catch exception if recConfFlags.AllowIgnoreConfigError otherwise rethrow
#  from RecExConfig.Resilience import treatException
#  try:
#      something=wrong
#  except Exception:
#      print "doSomethingIntelligent about this exception"
#      treatException(" failed to something=wrong. Did something intelligent about it")

def treatException (desc):
  from RecExConfig.RecConfFlags import recConfFlags
  if recConfFlags.AllowIgnoreConfigError():
      import traceback
      print traceback.format_exc()
      print "ERROR: "+desc
      return None
  else:
    raise

# do comand with optional catch of exception
# usage protect ("Could not load MakeAODCellsFromCluster!", include, "CaloRec/MakeAODCellsFromCluster_jobOptions.py")
# be careful that this will not catch exception if an argument is invalid
def protect (desc, fn, *args, **kw):
  try:
    return fn (*args, **kw)      
  except Exception:
    treatException(desc)
    return None

  

# protectedInclude ("Could not load MakeAODCellsFromCluster!", "CaloRec/MakeAODCellsFromCluster_jobOptions.py")
# exception is caught optionally
def protectedInclude (jo):
  from AthenaCommon.Include import include
  return protect ("Could not load " + jo, include, jo)




