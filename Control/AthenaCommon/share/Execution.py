import builtins
printfunc = getattr(builtins,'print')

### execution of user script and drop into batch or interactive mode ---------
for script in opts.scripts:
   try:
      include( script )
   except Exception as e:
      if isinstance(e,SystemExit):
         raise

    # "sanitize" the traceback under the assumption that it is the user's fault
      import traceback

      if not _msg.isEnabledFor( logging.DEBUG ):
    # "sanitize" the traceback under the assumption that it is the user's fault
         exc_info = sys.exc_info()
         tb = traceback.extract_tb( exc_info[2] )

         short_tb = []
         for frame_info in tb:
            if not 'AthenaCommon' in frame_info[0]:
               short_tb.append( frame_info )

         printfunc ('Shortened traceback (most recent user call last):')
         printfunc (''.join( traceback.format_list( short_tb ) ),)
         printfunc (''.join( traceback.format_exception_only( exc_info[0], exc_info[1] ) ),)
      else:
         traceback.print_exc()

      if opts.run_batch:
       # additional processing to get right error codes
         import AthenaCommon.ExitCodes as ExitCodes
         if isinstance( e, IncludeError ):
            theApp._exitstate = ExitCodes.INCLUDE_ERROR
         elif isinstance( e, ImportError ):
            theApp._exitstate = ExitCodes.IMPORT_ERROR
         else:
            theApp._exitstate = ExitCodes.UNKNOWN_EXCEPTION

         del ExitCodes
         theApp.exit()

    # for interactive: drop into prompt
      break

else:
 ## only get here if all scripts successfully included
   if "-h" in opts.user_opts or "--help" in opts.user_opts:
      # The user script was displaying help messages stop here
      # In most cases, the user script should kill the process already (argparse
      # does this for you) but just in case print our help message and stop
      from AthenaCommon.AthOptionsParser import _error_msg
      print(_error_msg)
      theApp.exit()

 ## load from .pkl file if given
   if opts.fromdb:
      _msg.info( 'now loading %s  ... ', opts.fromdb )
      import AthenaCommon.ConfigurationShelve
      AthenaCommon.ConfigurationShelve.loadJobOptionsCatalogue( opts.fromdb )
      del AthenaCommon.ConfigurationShelve
      if opts.fromdb.startswith( 'TempAthenaConfig.' ):
         try:
            os.remove( opts.fromdb )
         except OSError:
            pass
      opts.fromdb = None

 ## let command line decide if one wants to enable PerfMon
   if opts.do_pmon:
      import PerfMonComps.PerfMonFlags as _pmf
      _pmf._decode_pmon_opts(opts.do_pmon)

 ## now is a good time to tweak the event selector according to command line opt
   if opts.nbr_repeat_evts:
      try:
         from PerfMonComps.PerfMonEventSelector import repeatEvents
         repeatEvents(nbrReplays=opts.nbr_repeat_evts)
      except ImportError as err:
         # not enough karma (probably b/c of runtime/compiletime deps. mix-up)
         from AthenaCommon.Logging import logging
         _msg.info( "Could not install the stammer eventloop selector" )
      
 ## all symbols have been loaded, but nothing has been run, good time for debug
   if DbgStage.value == "init":
      hookDebugger()

 ## setup multi-process running and debugging
   if not opts.minimal and opts.nprocs and (opts.nprocs >= 1 or opts.nprocs==-1):
      from AthenaCommon.AppMgr import ServiceMgr as svcMgr
      import AthenaMP.PyComps as _amppy
      svcMgr += _amppy.MpEvtLoopMgr(NWorkers=opts.nprocs)

   #if EvtMax and SkipEvents are set, use them
   from AthenaCommon.AthenaCommonFlags import jobproperties as jps
   if jps.AthenaCommonFlags.EvtMax.statusOn:
      theApp.EvtMax = jps.AthenaCommonFlags.EvtMax()
   if jps.AthenaCommonFlags.SkipEvents.statusOn:
      if hasattr(svcMgr,"EventSelector"):
         svcMgr.EventSelector.SkipEvents = jps.AthenaCommonFlags.SkipEvents()
      else:
         _msg.warning('No EventSelector in svcMgr, not skipping events')


 ## in batch, run as many events as requested, otherwise explain
   if opts.run_batch:
      ## enable or not athena-mp
      include( "AthenaCommon/runbatch.py" )
   else:
      #pass
      # done, back to the user


      if opts.nprocs and (opts.nprocs >= 1 or opts.nprocs==-1):
         _msg.warning('interactive mode not validated with AthenaMP')

      _msg.info( 'entered interactive mode - have fun!' )
      _msg.info( 'you can call theApp.run() to execute and use ctrl-d to exit' )
      os.environ['PYTHONINSPECT'] = '1'
