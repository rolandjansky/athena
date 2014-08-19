# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# testing exception handline
# to use it :
# athena -i testResilience.py
from RecExConfig.RecConfFlags import recConfFlags
# if true catch exception and proceed
# if false rethrow exception and stop


for flag in [ True , False ]:
  recConfFlags.AllowIgnoreConfigError=flag

  print "*"*80
  # if all that is needed is to protect an include 
  from RecExConfig.Resilience import protectedInclude
  protectedInclude ("truc/pouet.py")

  print "*"*80


  # if more than an include or if want to do sthg special in case of an exception
  from RecExConfig.Resilience import treatException
  try:
      something=wrong
  except Exception:
      print "doSomethingIntelligent about this exception"
      treatException(" failed to something=wrong. Did something intelligent about it")



  print "*"*80


stop
