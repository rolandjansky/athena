# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Include import include
from AthenaCommon.AppMgr import theApp

# almost all the contents of this file is stolen
# from G4AtlasApps/PyG4Atlas.py

class ISF_Input:
  """
   Place to handle the persistency.
  """

  print "in ISF_Input"

  """ Place to read evgen events.
  """
  from G4AtlasApps.SimFlags import simFlags
  from AthenaCommon.AppMgr import ServiceMgr as svcMgr
  #G4AtlasEngine.log.verbose('SimSkeleton._do_readevgen :: starting')

  from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

  if athenaCommonFlags.PoolEvgenInput.statusOn:
      ## Tell the event selector about the evgen input files and event skipping
      if not hasattr(svcMgr, 'EventSelector'):
          import AthenaPoolCnvSvc.ReadAthenaPool
      svcMgr.EventSelector.InputCollections = athenaCommonFlags.PoolEvgenInput()
      if athenaCommonFlags.SkipEvents.statusOn:
          svcMgr.EventSelector.SkipEvents = athenaCommonFlags.SkipEvents()

      ## Read input file metadata
      import PyUtils.AthFile as af
      hitfile = athenaCommonFlags.PoolEvgenInput.get_Value()[0]
      f = af.fopen(hitfile)

      ## Check that event type is SIMULATION (as it must be)
      if "evt_type" in f.infos.keys():
          evttypes = f.infos["evt_type"]
          evttype0 = str(evttypes[0])
          if not evttype0.startswith("IS_SIMULATION"):
              msg = "ERROR: ISF_Input :: This input file has incorrect evt_type: %s\n" % str(evttypes)
              msg += "Please make sure you have set input file metadata correctly - "
              msg += "consider using the job transforms for earlier steps if you aren't already doing so."
              print msg
              raise SystemExit("Input file evt_type is incorrect: please check your evgen jobs.")
      else:
        print "WARNING: ISF_Input :: Could not find 'evt_type' key in athfile.infos. Unable to that check evt_type is correct."

  else:
      ## No input file so assume that we are running a Generator in the same job
      if not hasattr(svcMgr, 'EventSelector'):
          import AthenaCommon.AtlasUnixGeneratorJob
      # TODO: Check that there is at least one algorithm already in the AlgSequence?
      ## Warn if attempting to skip events in a generator job
      if athenaCommonFlags.SkipEvents.statusOn:
          msg = "WARNING: ISF_Input :: athenaCommonFlags.SkipEvents set in a job without an active "
          msg += "athenaCommonFlags.PoolEvgenInput flag: ignoring event skip request"
          print msg

  #G4AtlasEngine.log.verbose('SimSkeleton._do_readevgen :: done')

