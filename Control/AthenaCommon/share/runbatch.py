# File: runbatch.py
# Author: Wim Lavrijsen <WLavrijsen@lbl.gov>

# Code for running a batch job with athena.py, after all other
# job options are set.
#
# Usage:
#   athena.py <myJobOptions.py> runbatch.py

try:
   #first check if command-line evtMax or skipEvents options were provided
   if opts.evtMax != None:
      theApp.EvtMax = jps.AthenaCommonFlags.EvtMax()
   if opts.skipEvents != None:
      if hasattr(svcMgr,"EventSelector"):
         svcMgr.EventSelector.SkipEvents = jps.AthenaCommonFlags.SkipEvents()

   theApp.run()     # runs until theApp.EvtMax events reached
   from AthenaCommon.Debugging import hookDebugger,DbgStage
   if DbgStage.value == "fini":
      hookDebugger()
except:
   # print a stack trace to know something bad happened !
   import traceback, sys
   traceback.print_exc(file=sys.stdout)
   # update the exit-code
   import AthenaCommon.ExitCodes as ath_codes
   if theApp._exitstate == ath_codes.ALL_OK:
      theApp._exitstate = ath_codes.UNKNOWN_EXCEPTION
   # re-raise, in case somebody wants to do something about it
   raise
finally:
   import AthenaCommon.ExitCodes as ath_codes
   if theApp._exitstate != ath_codes.ALL_OK:
      # trigger the AthenaSummarySvc
      try:
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        if hasattr( svcMgr, 'AthenaSummarySvc' ):
          from AthenaPython import PyAthena
          aths = PyAthena.py_svc('AthenaSummarySvc', iface='IAthenaSummarySvc')
          aths.setStatus( theApp._exitstate )
          aths.createSummary()
      except ImportError:
         print "import of PyAthena failed: unable to trigger AthenaSummarySvc"
   theApp.exit()    # exits program, yields theApp._exitstate on shell,
                    # ok if re-thrown C++ exception: try to exit clean
