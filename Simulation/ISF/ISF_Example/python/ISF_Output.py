# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Include import include
from AthenaCommon.AppMgr import theApp

class ISF_HITSStream:
  """
   Place to handle the persistency.
  """

  print "in ISF_HITSStream"

  from G4AtlasApps.SimFlags import simFlags
  from ISF_Config.ISF_jobProperties import ISF_Flags
  from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
  from AthenaCommon.AppMgr import ServiceMgr
  if athenaCommonFlags.PoolHitsOutput.statusOn or (hasattr(simFlags, "WriteTR") and simFlags.WriteTR.statusOn):
      ## Write hits in POOL
      print "ISF_HITSStream starting"

      ## The following used to be in G4AtlasApps/HitAthenaPoolWriteOptions
      from AthenaCommon.DetFlags import DetFlags
      from AthenaCommon.Configurable import Configurable
      from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream

      ## Default setting for one output stream
      ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += ["TREE_BRANCH_OFFSETTAB_LEN = '100'"]
      ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += ["DEFAULT_BUFFERSIZE = '2048'"]

      ## Write geometry tag info
      import EventInfoMgt.EventInfoMgtInit

      ## Instantiate StreamHITS
      ## NB. Two-arg constructor is needed, since otherwise metadata writing fails!
      stream1 = None
      if athenaCommonFlags.PoolHitsOutput.statusOn:
          stream1 = AthenaPoolOutputStream("StreamHITS", athenaCommonFlags.PoolHitsOutput())

          ## EventInfo & TruthEvent always written by default
          stream1.ForceRead = True
          stream1.ItemList = ["EventInfo#*",
                              "McEventCollection#TruthEvent",
                              "JetCollection#*"]

          if simFlags.SimulationFlavour() is not None and 'ongLived' in simFlags.SimulationFlavour(): # to catch LongLived and longLived
              stream1.ItemList += ["McEventCollection#GEN_EVENT"]

          from PyJobTransforms.trfUtils import releaseIsOlderThan
          if releaseIsOlderThan(20,0):
            #Hack to maintain compatibility of G4AtlasApps trunk with
            #19.2.X.Y after EDM changes in release 20.0.0.
            stream1.ItemList += ["xAOD::JetContainer_v1#*",
                                 "xAOD::JetAuxContainer_v1#*"]
          else:
            stream1.ItemList += ["xAOD::JetContainer#*",
                                 "xAOD::JetAuxContainer#*"]

          ## Make stream aware of aborted events
          stream1.AcceptAlgs = ['ISF_Kernel_'+ISF_Flags.Simulator()]#,"G4AtlasAlg"]

          ## Detectors

          ## Inner Detector
          if DetFlags.ID_on():
              stream1.ItemList += ["SiHitCollection#*",
                                   "TRTUncompressedHitCollection#*",
                                   "TrackRecordCollection#CaloEntryLayer"]
          ## Calo
          if DetFlags.Calo_on():
              stream1.ItemList += ["CaloCalibrationHitContainer#*",
                                   "LArHitContainer#*",
                                   "TileHitVector#*",
                                   #"SimpleScintillatorHitCollection#*",
                                   "TrackRecordCollection#MuonEntryLayer"]
          ## Muon
          if DetFlags.Muon_on():
              stream1.ItemList += ["RPCSimHitCollection#*",
                                   "TGCSimHitCollection#*",
                                   "CSCSimHitCollection#*",
                                   "MDTSimHitCollection#*",
                                   "TrackRecordCollection#MuonExitLayer"]
          ## FwdRegion
          checkFwdRegion = getattr(DetFlags, 'FwdRegion_on', None) #back-compatibility
          if checkFwdRegion is not None: #back-compatibility
              if checkFwdRegion():
                  stream1.ItemList += ["SimulationHitCollection#*"]

          ## Lucid
          if DetFlags.Lucid_on():
              stream1.ItemList += ["LUCID_SimHitCollection#*"]

          ## ZDC
          if DetFlags.ZDC_on():
              stream1.ItemList += ["ZDC_SimPixelHit_Collection#*",
                                   "ZDC_SimStripHit_Collection#*"]
          ## ALFA
          if DetFlags.ALFA_on():
              stream1.ItemList += ["ALFA_HitCollection#*","ALFA_ODHitCollection#*"]

          ## AFP
          checkAFP = getattr(DetFlags, 'AFP_on', None) #back-compatibility
          if checkAFP is not None: #back-compatibility
              if checkAFP():
                 stream1.ItemList += ["AFP_TDSimHitCollection#*","AFP_SIDSimHitCollection#*"]

          ## Ancillary scintillators
          #stream1.ItemList += ["ScintillatorHitCollection#*"]

          ## TimingAlg
          stream1.ItemList +=["RecoTimingObj#EVNTtoHITS_timings"]

          ## Add cosmics and test beam configuration hit persistency if required cf. geom tag
          layout = simFlags.SimLayout.get_Value()

          ## add xAOD::TrackParticles output collection Parametric Simulation
          if simFlags.SimulationFlavour == "ParametricSimulation":
              stream1.ItemList +=["xAOD::TrackParticleContainer#*",
                                  "xAOD::TrackParticleAuxContainer#*"]
          #from AthenaCommon.GlobalFlags import globalflags
          #layout = globalflags.DetDescrVersion.get_Value()
          if "tb" not in layout:
              from AthenaCommon.BeamFlags import jobproperties
              if jobproperties.Beam.beamType() == 'cosmics' or \
                      (hasattr(simFlags, "WriteTR") and simFlags.WriteTR.statusOn) or \
                      (hasattr(simFlags, "ReadTR") and simFlags.ReadTR.statusOn):
                  stream1.ItemList += ["TrackRecordCollection#CosmicRecord", "TrackRecordCollection#CosmicPerigee"]
          else:
              ## CTB-specific
              if layout.startswith("ctb"):
                  if simFlags.LArFarUpstreamMaterial.statusOn and simFlags.LArFarUpstreamMaterial.get_Value():
                      stream1.ItemList.append("TrackRecordCollection#LArFarUpstreamMaterialExitLayer")
              ## Persistency of test-beam layout
              if layout.startswith('ctb') or layout.startswith('tb_Tile2000_'):
                  stream1.ItemList += ["TBElementContainer#*"]

      ## StreamEVGEN: needed for cosmic simulations and cavern BG
      ## Separate stream of track record (TR) info -- it does not apply to the CTB simulations.
      # TODO: Can this be merged into the cosmics sec above, or do the AthenaPool includes *need* to be in-between?
      layout = simFlags.SimLayout.get_Value()
      #from AthenaCommon.GlobalFlags import globalflags
      #layout = globalflags.DetDescrVersion.get_Value()
      if "tb" not in layout:
          if hasattr(simFlags, "WriteTR") and simFlags.WriteTR.statusOn:
              stream2 = AthenaPoolOutputStream("StreamEVGEN", simFlags.WriteTR.get_Value())
              if simFlags.CavernBG.statusOn and 'Write' in simFlags.CavernBG.get_Value():
                  stream2.ItemList += ["TrackRecordCollection#NeutronBG"]
              else:
                  stream2.ItemList += ["TrackRecordCollection#CosmicRecord"]
              stream2.AcceptAlgs = ['ISF_Kernel_'+ISF_Flags.Simulator()]#,"G4AtlasAlg"]


      #
      #  Setup and add metadata to the HITS file.
      #  ( heavily based on G4AtlasApps/python/SimAtlasKernel.py )
      #
      #  causes problems with cosmics currently:
      #    "IOVDbSvc ERROR Duplicate request for folder /Simulation/Parameters associated to already"
      from AthenaServices.AthenaServicesConf import AthenaOutputStream
      stream1_SimMetaData = AthenaOutputStream("StreamHITS_SimMetaData")
      stream1_SimMetaData.ItemList += [ "IOVMetaDataContainer#*" ]

      from IOVDbMetaDataTools import ParameterDbFiller
      dbFiller = ParameterDbFiller.ParameterDbFiller()

      ## Set run numbers
      minrunnum = 0
      maxrunnum = 2147483647 # MAX
      from G4AtlasApps.SimFlags import simFlags
      if hasattr(simFlags, 'RunNumber') and simFlags.RunNumber.statusOn:
         minrunnum = simFlags.RunNumber()
         ## FIXME need to use maxrunnum = 2147483647 for now to keep overlay working but in the future this should be set properly.
        #maxrunnum = minrunnum + 1
      elif athenaCommonFlags.PoolEvgenInput.statusOn:
         import PyUtils.AthFile as af
         f = af.fopen(athenaCommonFlags.PoolEvgenInput()[0])
         if len(f.run_numbers) > 0:
             minrunnum = f.run_numbers[0]
             maxrunnum = minrunnum + 1
         else:
             raise Exception('IllegalRunNumber')
      dbFiller.setBeginRun(minrunnum)
      dbFiller.setEndRun(maxrunnum)

      ### Read in special simulation job option fragments based on metadata passed by the evgen stage
      #if athenaCommonFlags.PoolEvgenInput.statusOn:
      #    import PyUtils.AthFile as af
      #    f = af.fopen(athenaCommonFlags.PoolEvgenInput()[0])
      #    #for k, v in f.infos.iteritems():
      #    #    print k, v
      #    if "specialConfiguration" in f.infos["tag_info"]:
      #        item = f.infos["tag_info"]["specialConfiguration"]
      #        ## Parse the specialConfiguration string
      #        ## Format is 'key1=value1;key2=value2;...'. or just '
      #        spcitems = item.split(";")
      #        params = {}
      #        for spcitem in spcitems:
      #            #print spcitem
      #            ## Ignore empty or "NONE" substrings, e.g. from consecutive or trailing semicolons
      #            if not spcitem or spcitem.upper() == "NONE":
      #                continue
      #            ## If not in key=value format, treat as v, with k="preInclude"
      #            if "=" not in spcitem:
      #                spcitem = "preInclude=" + spcitem
      #            ## Handle k=v directives
      #            k, v = spcitem.split("=")
      #            if k == "preInclude":
      #                include(v)
      #            else:
      #                params[k] = v

      ## Write sim parameters as metadata keys
      from AthenaCommon.JobProperties import JobProperty
      simParams = [sf for sf in dir(simFlags) if isinstance(getattr(simFlags, sf), JobProperty)]
      for sp in simParams:
          ## Don't write out random number seeds or RunDict as metadata
          if sp in ("RandomSeedList", "RandomSeedOffset", "RunDict"):
              continue
          ## Only store InitFunction names
          if sp in ("InitFunctions"):
              initfuncdict = dict()
              for level in getattr(simFlags, sp).get_Value():
                  initfuncdict.setdefault(level, [])
                  for element in getattr(simFlags, sp).get_Value()[level]:
                      initfuncdict[level].append(element.__name__)
              testValue = str( initfuncdict ) if getattr(simFlags, sp).statusOn else 'default'
              dbFiller.addSimParam(sp, testValue)
              continue
          testValue = str( getattr(simFlags, sp).get_Value() ) if getattr(simFlags, sp).statusOn else 'default'
          dbFiller.addSimParam(sp, testValue)
      import os
      dbFiller.addSimParam('G4Version', str(os.environ['G4VERS']))
      dbFiller.addSimParam('RunType', 'atlas')
      from AthenaCommon.BeamFlags import jobproperties
      dbFiller.addSimParam('beamType', jobproperties.Beam.beamType.get_Value())

      ## Simulated detector flags: add each enabled detector to the simulatedDetectors list
      simDets = []
      for det in ['pixel','SCT','TRT','BCM','DBM','Lucid','FwdRegion','ZDC','ALFA','AFP','LAr','Tile','MDT','CSC','TGC','RPC','Micromegas','sTGC','Truth']:
          attrname = det+"_on"
          checkfn = getattr(DetFlags.geometry, attrname, None)
          if checkfn is None:
              continue
          if checkfn():
              simDets.append(det)
      dbFiller.addSimParam('SimulatedDetectors', repr(simDets))

      ## Hard-coded simulation hit file magic number (for major changes)
      dbFiller.addSimParam('hitFileMagicNumber', '0') ##FIXME Remove this?

      dbFiller.addSimParam('Simulator', ISF_Flags.Simulator())
      dbFiller.addSimParam('TruthStrategy', ISF_Flags.TruthStrategy())

      ## Write the db info
      dbFiller.genSimDb()
      folder = "/Simulation/Parameters"
      dbConnection = "sqlite://;schema=SimParams.db;dbname=SIMPARAM"
      import IOVDbSvc.IOVDb
      ServiceMgr.IOVDbSvc.Folders += [ folder + "<dbConnection>" + dbConnection + "</dbConnection>" ]
      ServiceMgr.IOVDbSvc.FoldersToMetaData += [folder]
      ServiceMgr.IOVSvc.partialPreLoadData = True

      print "ISF_HITSStream done"
  else:
      print "ISF_HITSStream nothing done"

  # Check on run numbers and update them if necessary
  # copied from do_run_number_modifications() in PyG4Atlas.py
  from G4AtlasApps.SimFlags import simFlags
  from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

  myRunNumber = 1
  myFirstLB = 1
  myInitialTimeStamp = 0
  if hasattr(simFlags, "RunNumber") and simFlags.RunNumber.statusOn:
      myRunNumber = simFlags.RunNumber.get_Value()
      print 'ISF_HITSStream.do_run_number_modifications INFO :: Found run number %d in sim flags.' % myRunNumber
      ## Set event selector details based on evgen metadata

      ######update the run/event info for each event
      if not hasattr(ServiceMgr,'EvtIdModifierSvc'):
          import AthenaServices.Configurables as asc
          ServiceMgr +=asc.EvtIdModifierSvc(EvtStoreName="StoreGateSvc")
          from AthenaCommon.AppMgr import theApp
          theApp.CreateSvc += ["EvtIdModifierSvc"]
      else:
           print 'ISF_HITSStream.do_run_number_modifications WARNING :: Will override the settings of the EvtIdModifierSvc that was previously set up!'
      #fix iov metadata
      if not hasattr(ServiceMgr.ToolSvc, 'IOVDbMetaDataTool'):
          from AthenaCommon import CfgMgr
          ServiceMgr.ToolSvc += CfgMgr.IOVDbMetaDataTool()
      ServiceMgr.ToolSvc.IOVDbMetaDataTool.MinMaxRunNumbers = [myRunNumber, 2147483647]
      ## FIXME need to use maxRunNumber = 2147483647 for now to keep overlay working but in the future this should be set properly.
      # Using event numbers to avoid "some very large number" setting
      from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
      totalNumber = 1000000 # TODO possibly get this from AthFile??
      if athenaCommonFlags.EvtMax() is not None and athenaCommonFlags.EvtMax()>0: totalNumber = athenaCommonFlags.EvtMax()+1
      if athenaCommonFlags.SkipEvents() is not None and athenaCommonFlags.SkipEvents()>0: totalNumber += athenaCommonFlags.SkipEvents()
      try:
        from RunDependentSimComps.RunDMCFlags import runDMCFlags
        myInitialTimeStamp = runDMCFlags.RunToTimestampDict.getTimestampForRun(myRunNumber)
        #print "FOUND TIMESTAMP ", str(myInitialTimeStamp)
      except:
        myInitialTimeStamp = 1
      ServiceMgr.EvtIdModifierSvc.add_modifier(run_nbr=myRunNumber, lbk_nbr=myFirstLB, time_stamp=myInitialTimeStamp, nevts=totalNumber)
      if hasattr(ServiceMgr.EventSelector,'OverrideRunNumberFromInput'): ServiceMgr.EventSelector.OverrideRunNumberFromInput = True
  elif athenaCommonFlags.PoolEvgenInput.statusOn:
      ## Read input file metadata
      import PyUtils.AthFile as af
      hitfile = athenaCommonFlags.PoolEvgenInput.get_Value()[0]
      f = af.fopen(hitfile)

      ## Get evgen run number and lumi block
      if len(f.run_numbers) > 0:
          myRunNumber = f.run_numbers[0]
          print 'ISF_HITSStream.do_run_number_modifications INFO :: Found run number %d in hits file metadata.'% myRunNumber
      else:
          print 'ISF_HITSStream.do_run_number_modifications WARNING :: Failed to find run number in hits file metadata.'
      if f.lumi_block:
          myFirstLB = f.lumi_block[0]

  ServiceMgr.EventSelector.OverrideRunNumber = True
  ServiceMgr.EventSelector.RunNumber = myRunNumber
  ServiceMgr.EventSelector.FirstLB = myFirstLB
  # Necessary to avoid a crash
  ServiceMgr.EventSelector.InitialTimeStamp = myInitialTimeStamp
