# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# The python script contains functions which will set up default b-tagging configurations for a certain jet-collection.
# Author: Wouter van den Wollenberg (wwollenb@nikhef.nl, wouter.van.den.wollenberg@cern.ch)

# The BTagging_LoadTools found in the ./share directory will call these functions to set up the default. The reason for these functions
# is that it is possible to set up the defaults per jets from the JetRec area.
# Note that there is no reason for ./share/BTagging_LoadTools.py to be used anymore (it just exists
# as a form of backwards compatibility).

# The functions will do nothing if a configuration already exists for a given jet collection, so it can be safely called by JetRec; only if
# there is no configuration it will setup the default one.

# IMPORTANT NOTE: If checkFlagsUsingBTaggingFlags() deems that BTagging should not run (for example if crucial detector components are not working), none of the functions will do anything and they will return False.

# =================================================
# Load the flags
from BTagging.BTaggingFlags import BTaggingFlags
# =================================================

# list of taggers that use MultivariateTagManager
mvtm_taggers = ['MV2c00','MV2c10','MV2c20','MV2c100','MV2cl100','MV2c10mu','MV2c10rnn','MV2m','MV2c10hp','DL1','DL1mu','DL1rnn']
mvtm_flip_taggers = [x+'Flip' for x in mvtm_taggers]
mvtm_trighybrid_taggers = ['MV2c10TrigHybrid']


def Initiate(ConfInstance=None):
  """Sets up the basic global tools required for B-Tagging. This function is idempotent; it will not run again if it has run once. It is
  typically called by other functions in this file to make sure the basic global tools exist.

  The function returns True if B-Tagging is initialized or has been before. It returns False if it B-Tagging has been switched off for various
  reasons (these are checked in the checkFlagsUsingBTaggingFlags function).

  If the B-tagging calibration broker has been registered we assume initiation has already been performed; this allows users to setup their own initiation code."""

  if ConfInstance is None:
    from BTagging.BTaggingConfiguration import getConfiguration
    ConfInstance = getConfiguration()

  if ConfInstance._Initialized:
    return True

  from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags as commonGeoFlags
  from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags as geoFlags
  from IOVDbSvc.CondDB import conddb
  btagrun1=False
  if conddb.dbdata == 'COMP200':
    btagrun1=True
  elif conddb.isMC:
    # The Run() parameter only exists for ATLAS-R1(...) and ATLAS-R2(...) geo tags,
    # not for ATLAS-GEO(...) and ATLAS-IBL(...) ones. Hence if Run() is undefined,
    # presence of IBL is used to switch between Run1/Run2
    btagrun1 = (commonGeoFlags.Run() == "RUN1" or (commonGeoFlags.Run() == "UNDEFINED" and geoFlags.isIBL() == False))
  if (btagrun1):
    print ConfInstance.BTagTag()+' - INFO - Setting up Run 1 configuration'
    BTaggingFlags.JetFitterNN=True
    BTaggingFlags.SV2    =True
    BTaggingFlags.JetVertexCharge=False
    BTaggingFlags.SoftMu=False
    BTaggingFlags.MV2c10mu=False
    BTaggingFlags.MV2c10rnn=False
    BTaggingFlags.MV2cl100=False
    BTaggingFlags.RNNIP=False
    BTaggingFlags.DL1=False
    BTaggingFlags.DL1mu=False
    BTaggingFlags.DL1rnn=False
  else:
    print ConfInstance.BTagTag()+' - INFO - Setting up Run 2 configuration'

  if ConfInstance._name == "Trig":
    BTaggingFlags.MV2c20=True
  
  if ConfInstance.getTool("BTagCalibrationBrokerTool"):
    print ConfInstance.BTagTag()+' - INFO - BTagCalibrationBrokerTool already exists prior to default initialization; assuming user set up entire initialization him/herself. Note however that if parts of the initalization were not set up, and a later tool requires them, they will be set up at that point automatically with default settings.'
    ConfInstance._Initialized = True
    return True

  print ConfInstance.BTagTag()+' - INFO - Initializing default basic tools'

  if ConfInstance.checkFlagsUsingBTaggingFlags():

    #Print the flags
    BTaggingFlags.Print()

    #If debugging do a check of the tool collection structure
    if(BTaggingFlags.OutputLevel < 3):
      from BTagging.BTaggingConfiguration import checkToolCollectionStructure
      checkToolCollectionStructure()

    #Get TheTruthCollectionKey from input
    TheTruthCollectionKey = 'TruthEvents'
    BTaggingFlags.RetagJets = BTaggingFlags.Jets
    if BTaggingFlags.AutoInspectInputFile:
      from AthenaCommon.GlobalFlags import globalflags
      if globalflags.InputFormat == 'pool':
        try:
          from RecExConfig.InputFilePeeker import inputFileSummary
          BTaggingFlags.RetagJets = []
          for i in inputFileSummary['eventdata_items']:
            if i[0] == 'McEventCollection':
              # TheTruthCollectionKey = i[1] # disable taking the name from the input file?
              pass
            elif i[0] == 'JetCollection':
              jetC1 = (i[1]).replace('AODJets','')
              jetC = jetC1.replace('Jets','')
              if jetC in BTaggingFlags.Jets:
                BTaggingFlags.RetagJets += [ jetC ]
        except Exception, err:
          print ConfInstance.BTagTag()+' - WARNING - Automatic inspection of input file failed (file too old?)'

    print ConfInstance.BTagTag()+' - Using ', TheTruthCollectionKey, ' as truth key'
#    print ConfInstance.BTagTag()+' - Re-tagging these jet collections: ', BTaggingFlags.RetagJets

    #
    # ============ Setup basic services
    #
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    if not hasattr(svcMgr, 'THistSvc'):
      from GaudiSvc.GaudiSvcConf import THistSvc
      svcMgr += THistSvc()
    if not 'topSequence' in dir():
      from AthenaCommon.AlgSequence import AlgSequence
      topSequence = AlgSequence()

    #
    # ========== Add tools now
    #

    # -------------- Calibration Broker --------------
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon.Resilience import treatException,protectedInclude
    if ConfInstance._name == "":
      protectedInclude("BTagging/BTagCalibBroker_jobOptions.py") # New file which includes the file from JetCalibration and also registers it via registerTool() so it will be recognized by the various addTool() functions.
      BTagCalibrationBrokerTool = ConfInstance.getTool("BTagCalibrationBrokerTool") # In case this variable is needed
    elif ConfInstance._name == "Trig":
      protectedInclude("BTagging/BTagCalibBroker_Trig_jobOptions.py")
      BTagCalibrationBrokerTool = ConfInstance.getTool("BTagCalibrationBrokerTool")
    elif ConfInstance._name == "AODFix":
      protectedInclude("BTagging/BTagCalibBroker_AODFix_jobOptions.py")
      BTagCalibrationBrokerTool = ConfInstance.getTool("BTagCalibrationBrokerTool")
    else:
      print ConfInstance.BTagTag()+' - ERROR - Configuration instance "'+ConfInstance._name+'" has no calibration broker setup specified!'
      raise RuntimeError
    # -------------- \Calibration Broker --------------


    global BTagJetTruthMatching
    global BTagLeptonTruthTool
    global thisBTagLabeling
    # FF: disable cone-based labelling as this is now being done as a part of jet reconstruction
    BTagJetTruthMatching = None
    BTagLeptonTruthTool = None
    thisBTagLabeling = None

    # -- to label jets with truth information
    #from JetRec.JetRecFlags import jetFlags
    #if jetFlags.useTruth():
    #  # Jet Label
    #  if BTaggingFlags.jetTruthMatching == 'matchQuark' or BTaggingFlags.jetTruthMatching == 'jetTrackTruthMatching':
    #    BTagJetTruthMatching = addTool("BTagJetTruthMatching",
    #                                   ToolSvc = ToolSvc,
    #                                   Verbose = BTaggingFlags.OutputLevel < 3,
    #                                   options = {'subtype' : BTaggingFlags.jetTruthMatching},
    #                                   SuppressNonCloneWarning=True)
    #    BTagJetTruthMatching.McEventCollection = TheTruthCollectionKey
    #  else:
    #    BTagJetTruthMatching = None
    #    print("#BTAG# - WARNING - BTaggingFlags.jetTruthMatching has unsupported value!")
    #  # Lepton Truth Match
    #  #BTagLeptonTruthTool = addTool('BTagLeptonTruthTool', ToolSvc = ToolSvc, Verbose = BTaggingFlags.OutputLevel < 3)
    #  #BTagLeptonTruthTool.McEventCollection = TheTruthCollectionKey
    #  BTagLeptonTruthTool = None # TEMPORARY: BTagLeptonTruthTool not implemented yet?
    #  # Labeling tool
    #  thisBTagLabeling = addTool("thisBTagLabeling", ToolSvc = ToolSvc, Verbose = BTaggingFlags.OutputLevel < 3,
    #                                        options = {'JetTruthMatchTool' : BTagJetTruthMatching,
    #                                                   #'LeptonTruthTool'   : BTagLeptonTruthTool}, TEMPORARY: BTagLeptonTruthTool not implemented yet?
    #                                                   },
    #                                        SuppressNonCloneWarning=True)
    #else:
    #  BTagJetTruthMatching = None
    #  BTagLeptonTruthTool  = None
    #  thisBTagLabeling = None
      # NOTE: These variables no longer serve any purpose, since any tool can be retrieved using getTool

    # -- for reference mode:
    if BTaggingFlags.Runmodus == 'reference':

      svcMgr.THistSvc.Output += ["RefFile DATAFILE='BTagCalibALL.root' OPT='RECREATE'"]
      #for key in BTaggingFlags.Jets:
      #  for tagger in ['JetProb','IP1D','IP2D','IP2DSpc','IP3D','IP3DSpc','SV1','SV2','SoftMu','SoftEl','JetFitterTagNN','JetFitterCOMBNN']:
      #    if BTaggingFlags.IsEnabled(tagger):
      #      RefileName = "BTaggingRef"+tagger+key+".root"
      #      svcMgr.THistSvc.Output += ["RefFile"+tagger+key+" DATAFILE='"+RefileName+"' OPT='RECREATE'"]

    ConfInstance._Initialized = True
    return True
  else:
    print ConfInstance.BTagTag()+' - WARNING - Tool initialization requested but B-Tagging is not possible for the current dataset.'
    return False

def SetupJetCollection(JetCollection, TaggerList=[], SetupScheme="Default", ConfInstance=None):
  """General function which can setup up a default B-Tagging configuration. Returns True if successful. Returns False if B-Tagging has
  been disabled for some reason. Also calls Initiate if needed. The function does nothing (and returns True) if a configuration for a given
  jet collection already exists; therefore the function is idempotent.

  The TaggerList holds a list of BTaggingFlags tagger names. Only these will be set up. If left empty the default taggers will be used instead.
  Note that if an unlisted tagger is a prerequisite of another tagger which is listed it will be set up nonetheless.

  The specific setup used depends on SetupScheme. This allows us to support multiple different default setup which can be easily set by users."""
  from BTagging.BTaggingConfiguration import taggerIsPossible
  if ConfInstance is None:
    from BTagging.BTaggingConfiguration import getConfiguration
    ConfInstance = getConfiguration()

  print(ConfInstance.BTagTag()+" - INFO - Setting up b-tagging for jet collection '"+JetCollection+"' using scheme: "+SetupScheme)
  if len(TaggerList) == 0:
    print(ConfInstance.BTagTag()+" - INFO - Using default taggers.")
  else:
    print(ConfInstance.BTagTag()+" - INFO - Using the following taggers: "+str(TaggerList))
  if not Initiate(ConfInstance):
    # If initiation fails; return False
    print(ConfInstance.BTagTag()+" - ERROR - Basic initialization failed; disabling b-tagging!")
    return False
  if ConfInstance.jetCollectionExists(JetCollection):
    if SetupScheme != "Default":
      print(ConfInstance.BTagTag()+" - WARNING - Requesting setup of jet collection '"+JetCollection+"' with non-default scheme '"+SetupScheme+"' but")
      print(ConfInstance.BTagTag()+" - WARNING - there is already a configuration for this jet collection. This scheme will therefore be ignored.")
    elif len(TaggerList) > 0:
      print(ConfInstance.BTagTag()+" - WARNING - Requesting setup of jet collection '"+JetCollection+"' with non-default list of taggers, but")
      print(ConfInstance.BTagTag()+" - WARNING - there is already a configuration for this jet collection. This list will therefore be ignored.")
    else:
      if BTaggingFlags.OutputLevel < 3:
        print(ConfInstance.BTagTag()+" - DEBUG - SetupJetCollection called on jet collection which already exists. This is no problem, but any settings")
        print(ConfInstance.BTagTag()+" - DEBUG - you pass to it using this function will be ignored.")
    return True
  ReturnValue = True
  TaggerList = list(TaggerList) # Just in case we might break something
  if len(TaggerList) == 0:
    # Load the default taggers (for now we use BTaggingFlags for this)
    for tagger in BTaggingFlags._tags:
      if BTaggingFlags.IsEnabled(tagger):
        TaggerList.append(tagger)
  ApprovedTaggerList = []
  # Check whether the flags actually this tagger to run
  for tagger in TaggerList:
    if ConfInstance._IgnoreTaggerObstacles:
      ApprovedTaggerList.append(tagger)
    elif taggerIsPossible(tagger):
      ApprovedTaggerList.append(tagger)
  if len(ApprovedTaggerList) == 0:
    print(ConfInstance.BTagTag()+' - WARNING - No taggers specified, or none of them can run with the present settings. Opting out of b-tagging for '+JetCollection)
    print(ConfInstance.BTagTag()+' - WARNING - B-tagging might not function properly!')
    return False
  try:
    exec 'ReturnValue = SetupJetCollection'+SetupScheme+'(JetCollection, ApprovedTaggerList, ConfInstance)'
  except:
    print(ConfInstance.BTagTag()+" - ERROR - Attempted setup for scheme '"+SetupScheme+"' failed! Possibly this scheme does not exist or is improperly implemented!")
    raise
  if not ReturnValue:
    print(ConfInstance.BTagTag()+" - WARNING - A problem has occurred during the execution of SetupJetCollection"+SetupScheme)
    print(ConfInstance.BTagTag()+" - WARNING - B-tagging might not function properly!")
  return ReturnValue

def SetupJetCollectionDefault(JetCollection, TaggerList, ConfInstance = None):
  """Sets up the default B-Tagging configuration for jet collection called JetCollection. Returns True if successful. The function does nothing
  (and returns True) if a configuration for the given jet collection already exists; therefore the function is idempotent.

  TaggerList is a list of taggers (the names coming from BTaggingFlags._tags (see SetupJetCollection() for how the default is filled))."""
  if ConfInstance is None:
    from BTagging.BTaggingConfiguration import getConfiguration
    ConfInstance = getConfiguration()

  # IMPORTANT NOTE: Many tools here exist globally, but this is okay; the addTool() function is idempotent; no additional tools will be set up.
  # They will just be registered to the specified jet collection (if not already the case).

  # !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  # NOTE: If there is any need to have different setup scheme then there is support for this.
  # See the SetupJetCollection() function. Your new function should have the following name:
  # SetupJetCollectionX(JetCollection, TaggerList)
  # where X is the name for your scheme. These functions should return True if all went well
  # and False if something went wrong.
  # !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  # !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  # NOTE: Many things are now commented out; these correspond to tools/taggers which
  # are not yet ported to release 19. Maybe they never will. The code is there to be used
  # if a porting attempt is made. Typically the code here should be fine; only the corresponding
  # configuration scripts require an update (BTaggingConfiguration_SomeTagger.py).
  # The associator tools (specifically the lepton one and the one for MultiSV) has not been
  # comment out; their code should still be up to date.
  # !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  from AthenaCommon.AppMgr import ToolSvc

  # -- setup basic b-tagging tool for this jet collection
  # Note that we are not storing this tool on a variable on purpose; there is now a different one per jet collection and therefore
  # subsequent scripts cannot depend on this variable (since it would just hold the last one initialized).
  ConfInstance.addJetCollectionTool(JetCollection, ToolSvc, Verbose = BTaggingFlags.OutputLevel < 3,
                                  options={'BTagLabelingTool'            : ConfInstance.getTool("thisBTagLabeling", JetCollection=JetCollection),
                                           'storeSecondaryVerticesInJet' : BTaggingFlags.writeSecondaryVertices})

  # Setup associators
  BTagTrackToJetAssociator = ConfInstance.setupTrackAssociator(
                                  'BTagTrackToJetAssociator'
                                , JetCollection
                                , ToolSvc
                                , Verbose = BTaggingFlags.OutputLevel < 3
                                )


  if 'SoftMu' in TaggerList or 'SoftMuChi2' in TaggerList:
    BTagMuonToJetAssociator = ConfInstance.setupMuonAssociator('Muons', JetCollection, ToolSvc, Verbose = BTaggingFlags.OutputLevel < 3)
  else:
    BTagMuonToJetAssociator = None
  if 'SoftEl' in TaggerList:
    BTagElectronToJetAssociator = ConfInstance.setupElectronAssociator('Electrons', JetCollection, ToolSvc,
                                                                     Verbose = BTaggingFlags.OutputLevel < 3,
                                                                     PhotonCollectionName = 'Photons')
  else:
    BTagElectronToJetAssociator = None
  if 'MultiSVbb1' in TaggerList or 'MultiSVbb2' in TaggerList:
    ConfInstance.setupTrackAssociator('BTagTrackToJetAssociatorBB', JetCollection, ToolSvc,
                                    Verbose = BTaggingFlags.OutputLevel < 3,
                                    options={'shareTracks': False,
                                             'useVariableSizedTrackCone' : True,
                                             'coneSizeFitPar1' : 3.15265e-01,
                                             'coneSizeFitPar2' : -3.66502e-01,
                                             'coneSizeFitPar3' : -1.56387e-05})

  # NOTE: The secondary vertex finders etc. don't need to be set up here depending on the flags; they are set up when needed by the
  # taggers below.

  # Setup all taggers

#          if BTaggingFlags.IP1D:
#            addTool('IP1DTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if 'IP2D' in TaggerList:
    ConfInstance.addTool('IP2DTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.IP2DFlip:
#            addTool('IP2DFlipTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.IP2DPos:
#            addTool('IP2DPosTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if 'IP2DNeg' in TaggerList:
    ConfInstance.addTool('IP2DNegTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.IP2DSpc:
#            addTool('IP2DSpcTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.IP2DSpcFlip:
#            addTool('IP2DSpcFlipTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.IP2DSpcPos:
#            addTool('IP2DSpcPosTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.IP2DSpcNeg:
#            addTool('IP2DSpcNegTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if 'IP3D' in TaggerList:
    ConfInstance.addTool('IP3DTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if 'RNNIP' in TaggerList:
    ConfInstance.addTool('RNNIPTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel)
  if 'RNNIPNeg' in TaggerList:
    ConfInstance.addTool('RNNIPNegTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel)

#          if BTaggingFlags.IP3DFlip:
#            addTool('IP3DFlipTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.IP3DPos:
#            addTool('IP3DPosTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if 'IP3DNeg' in TaggerList:
    ConfInstance.addTool('IP3DNegTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.IP3DSpc:
#            addTool('IP3DSpcTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.IP3DSpcFlip:
#            addTool('IP3DSpcFlipTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.IP3DSpcPos:
#            addTool('IP3DSpcPosTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.IP3DSpcNeg:
#            addTool('IP3DSpcNegTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)

  if 'SV1' in TaggerList:
    ConfInstance.addTool('SV1Tag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if 'SV2' in TaggerList:
    ConfInstance.addTool('SV2Tag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if 'SV1Flip' in TaggerList:
    ConfInstance.addTool('SV1FlipTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.SV2:
#            addTool('SV2Tag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.SV2Flip:
#            addTool('SV2FlipTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if 'SV0' in TaggerList:
    ConfInstance.addTool('SV0Tag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)

  if 'MultiSVbb1' in TaggerList:
    ConfInstance.addTool('MultiSVbb1Tag', ToolSvc, 'BTagTrackToJetAssociatorBB', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if 'MultiSVbb2' in TaggerList:
    ConfInstance.addTool('MultiSVbb2Tag', ToolSvc, 'BTagTrackToJetAssociatorBB', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)

#          if BTaggingFlags.MultiSV:
#            # The name of this tagger was misspelled in the old code; I fixed it (Wouter)
#            addTool('MultiSVTag', ToolSvc, 'BTagTrackToJetAssociatorBB', JetCollection, Verbose =  BTaggingFlags.OutputLevel < 3)

#          if BTaggingFlags.JetProb:
#            addTool('JetProbTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#            if BTaggingFlags.JetProbFlip:
#              addTool('JetProbFlipTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)

#          if BTaggingFlags.SoftEl:
#            addTool('SoftElectronTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if 'SoftMu' in TaggerList:
    ConfInstance.addTool('SoftMuonTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.SoftMuChi2:
#            addTool('SoftMuonTagChi2', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)

#          if BTaggingFlags.BasicJetFitter:
#            addTool('JetFitterTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#            if BTaggingFlags.IP3D:
#              addTool('JetFitterCOMB', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.JetFitterTag:
#            addTool('NewJetFitterTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#            if BTaggingFlags.IP3D:
#              addTool('NewJetFitterCOMB', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if 'JetFitterNN' in TaggerList:
    ConfInstance.addTool('JetFitterTagNN', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)

  if 'JetFitterCOMBNN' in TaggerList:
    ConfInstance.addTool('JetFitterTagCOMBNN', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)

#          if BTaggingFlags.JetFitterTagFlip:
#            addTool('NewJetFitterTagFlip', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#            if BTaggingFlags.IP3DNeg:
#              addTool('NewJetFitterFlipCOMB', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if 'JetFitterNNFlip' in TaggerList:
    ConfInstance.addTool('JetFitterTagNNFlip', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#            if BTaggingFlags.IP3DPos:
#              addTool('JetFitterTagCOMBNNIP3DPos', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
    if 'IP3DNeg' in TaggerList:
      ConfInstance.addTool('JetFitterTagCOMBNNIP3DNeg', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)

#          if BTaggingFlags.TrackCounting:
#            addTool('TrackCounting', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.TrackCountingFlip:
#            addTool('TrackCountingFlip', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)


  if 'MV1' in TaggerList:
    ConfInstance.addTool('MV1Tag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if 'MV1c' in TaggerList:
    ConfInstance.addTool('MV1cTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if 'MV1cFlip' in TaggerList:
    ConfInstance.addTool('MV1cFlipTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if 'MV1Flip' in TaggerList:
    ConfInstance.addTool('MV1FlipTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if 'JetVertexCharge' in TaggerList:
    ConfInstance.addTool('JetVertexCharge', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if 'ExKtbb_Hbb_MV2Only' in TaggerList:
    ConfInstance.addTool('ExKtbb_Hbb_MV2OnlyTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if 'ExKtbb_Hbb_MV2andJFDRSig' in TaggerList:
    ConfInstance.addTool('ExKtbb_Hbb_MV2andJFDRSigTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if 'ExKtbb_Hbb_MV2andTopos' in TaggerList:
    ConfInstance.addTool('ExKtbb_Hbb_MV2andToposTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)

  #MultivariateTagManager
  #list of mvtm taggers that are also in TaggerList
  mvtm_active_taggers = list(set(mvtm_taggers) & set(TaggerList))
  mvtm_active_flip_taggers = list(set(mvtm_flip_taggers) & set(TaggerList))

  #set up MVTM if any of its taggers are active
  if (mvtm_active_taggers):
    MVTM = ConfInstance.addTool('MultivariateTagManager', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
    if 'RNNIP' in TaggerList:
      MVTM.arbitraryAuxData = BTaggingFlags.MultivariateTagManagerAuxBranches
    else:
      MVTM.arbitraryAuxData=[]

  #set up MVTMFlip
  if (mvtm_active_flip_taggers):
    MVTMFlip = ConfInstance.addTool('MultivariateFlipTagManager', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
    MVTMFlip.arbitraryAuxData = BTaggingFlags.MultivariateFlipTagManagerAuxBranches
    MVTMFlip.auxDataNameMap = BTaggingFlags.MultivariateTagManagerAuxNameMap

  if 'TagNtupleDumper' in TaggerList:
    tag = ConfInstance.addTool('TagNtupleDumper', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
    MVTM.MVTagToolList.append(tag)

  #add all the taggers that use MVTM
  for mvtm_tagger in mvtm_active_taggers:
    tag = ConfInstance.addTool(mvtm_tagger+'Tag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)

    if tag not in MVTM.MVTagToolList:
      MVTM.MVTagToolList.append(tag)

  #add all the flip taggers that use MVTM
  for mvtm_tagger in mvtm_active_flip_taggers:
    tag = ConfInstance.addTool(mvtm_tagger+'Tag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
    if tag not in MVTMFlip.MVTagToolList:
      MVTMFlip.MVTagToolList.append(tag)

  # -- b-tagging tool is now fully configured.

  if BTaggingFlags.OutputLevel < 3:
    ConfInstance.printAllTools()
      # NOTE: Printing all tools might be excessive; however the adding of future tools might affect the earlier ones; so printing
      # only really makes sense at the end, so we either have to do it each time; or get people to add the print command in their
      # jobOptions. Let us go for the excessive printing in this case; which is only done if debugging anyway.

  # Return True to denote all was successful
  return True

def SetupJetCollectionRetag(JetCollection, TaggerList, ConfInstance = None):
  """Sets up the B-Tagging configuration for jet collection called JetCollection. Returns True if successful. The function does nothing
  (and returns True) if a configuration for the given jet collection already exists; therefore the function is idempotent.

  This function is meant to be used for retagging, many tools will not be set up, and those that are, will not check for prerequisites.

  TaggerList is a list of taggers (the names coming from BTaggingFlags._tags (see SetupJetCollection() for how the default is filled))."""
  if ConfInstance is None:
    from BTagging.BTaggingConfiguration import getConfiguration
    ConfInstance = getConfiguration()

  from AthenaCommon.AppMgr import ToolSvc

  # -- setup basic b-tagging tool for this jet collection
  # Note that we are not storing this tool on a variable on purpose; there is now a different one per jet collection and therefore
  # subsequent scripts cannot depend on this variable (since it would just hold the last one initialized).
  ConfInstance.addJetCollectionTool(JetCollection, ToolSvc, Verbose = BTaggingFlags.OutputLevel < 3,
                                  options={'BTagLabelingTool'            : ConfInstance.getTool("thisBTagLabeling", JetCollection=JetCollection),
                                           'storeSecondaryVerticesInJet' : BTaggingFlags.writeSecondaryVertices})

  # Setup all taggers

  # if 'IP2D' in TaggerList:
  #   ConfInstance.addTool('IP2DTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3,
  #                        CheckOnlyInsideToolCollection=True, DoNotSetUpParticleAssociators=True)
  # if 'IP2DNeg' in TaggerList:
  #   ConfInstance.addTool('IP2DNegTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3,
  #                        CheckOnlyInsideToolCollection=True, DoNotSetUpParticleAssociators=True)
  if 'IP3D' in TaggerList:
    # We need to setup both BTagTrackToVertexTool and BTagTrackToVertexIPEstimator here since the IP3D setup line
    # has CheckOnlyInsideToolCollection set to True, but these two tools are outside the IP3D tool collection and are needed anyway for IP3D.
    ConfInstance.addTool('BTagTrackToVertexTool', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3,
                         CheckOnlyInsideToolCollection=False, DoNotSetUpParticleAssociators=True)
    ConfInstance.addTool('BTagTrackToVertexIPEstimator', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3,
                         CheckOnlyInsideToolCollection=False, DoNotSetUpParticleAssociators=True)
    ConfInstance.addTool('IP3DTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3,
                         CheckOnlyInsideToolCollection=True, DoNotSetUpParticleAssociators=True)

  if 'RNNIP' in TaggerList:
    ConfInstance.addTool('RNNIPTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  # if 'IP3DNeg' in TaggerList:
  #   ConfInstance.addTool('IP3DNegTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3,
  #                        CheckOnlyInsideToolCollection=True, DoNotSetUpParticleAssociators=True)
  # if 'SV1' in TaggerList:
  #   ConfInstance.addTool('SV1Tag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3,
  #                        CheckOnlyInsideToolCollection=True, DoNotSetUpParticleAssociators=True)
  # if 'SV1Flip' in TaggerList:
  #   ConfInstance.addTool('SV1FlipTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3,
  #                        CheckOnlyInsideToolCollection=True, DoNotSetUpParticleAssociators=True)
  # if 'SV0' in TaggerList:
  #   ConfInstance.addTool('SV0Tag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3,
  #                        CheckOnlyInsideToolCollection=True, DoNotSetUpParticleAssociators=True)

  # if 'MultiSVbb1' in TaggerList:
  #   ConfInstance.addTool('MultiSVbb1Tag', ToolSvc, 'BTagTrackToJetAssociatorBB', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3,
  #                        CheckOnlyInsideToolCollection=True, DoNotSetUpParticleAssociators=True)
  # if 'MultiSVbb2' in TaggerList:
  #   ConfInstance.addTool('MultiSVbb2Tag', ToolSvc, 'BTagTrackToJetAssociatorBB', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3,
  #                        CheckOnlyInsideToolCollection=True, DoNotSetUpParticleAssociators=True)

  # if 'JetFitterNN' in TaggerList:
  #   ConfInstance.addTool('JetFitterTagNN', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3,
  #                        CheckOnlyInsideToolCollection=True, DoNotSetUpParticleAssociators=True)
  #   if 'IP3D' in TaggerList:
  #     ConfInstance.addTool('JetFitterTagCOMBNN', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3,
  #                          CheckOnlyInsideToolCollection=True, DoNotSetUpParticleAssociators=True)

  # if 'JetFitterNNFlip' in TaggerList:
  #   ConfInstance.addTool('JetFitterTagNNFlip', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3,
  #                        CheckOnlyInsideToolCollection=True, DoNotSetUpParticleAssociators=True)
  #   if 'IP3DNeg' in TaggerList:
  #     ConfInstance.addTool('JetFitterTagCOMBNNIP3DNeg', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3,
  #                          CheckOnlyInsideToolCollection=True, DoNotSetUpParticleAssociators=True)

  if 'MV1' in TaggerList:
    ConfInstance.addTool('MV1Tag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3,
                         CheckOnlyInsideToolCollection=True, DoNotSetUpParticleAssociators=True)

  if 'MV1c' in TaggerList:
    ConfInstance.addTool('MV1cTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3,
                         CheckOnlyInsideToolCollection=True, DoNotSetUpParticleAssociators=True)
  if 'MV1cFlip' in TaggerList:
    ConfInstance.addTool('MV1cFlipTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3,
                         CheckOnlyInsideToolCollection=True, DoNotSetUpParticleAssociators=True)
  if 'MV1Flip' in TaggerList:
    ConfInstance.addTool('MV1FlipTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3,
                         CheckOnlyInsideToolCollection=True, DoNotSetUpParticleAssociators=True)
  if 'JetVertexCharge' in TaggerList:   #LC FIXME
    #ConfInstance.setupMuonAssociator('Muons', JetCollection, ToolSvc, Verbose = BTaggingFlags.OutputLevel < 3)
    ConfInstance.addTool('JetVertexCharge', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3,
                         CheckOnlyInsideToolCollection=True, DoNotSetUpParticleAssociators=True)

  #MultivariateTagManager
  #list of mvtm taggers that are also in TaggerList
  mvtm_active_taggers = list(set(mvtm_taggers) & set(TaggerList))
  mvtm_active_flip_taggers = list(set(mvtm_flip_taggers) & set(TaggerList))

  #set up MVTM if any of its taggers are active
  if (mvtm_active_taggers):
    MVTM = ConfInstance.addTool('MultivariateTagManager', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
    if 'RNNIP' in TaggerList:
      MVTM.arbitraryAuxData = BTaggingFlags.MultivariateTagManagerAuxBranches
    else:
      MVTM.arbitraryAuxData=[]

  #set up MVTMFlip
  if (mvtm_active_flip_taggers):
    MVTMFlip = ConfInstance.addTool('MultivariateFlipTagManager', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)

  if 'TagNtupleDumper' in TaggerList:
    tag = ConfInstance.addTool('TagNtupleDumper', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
    MVTM.MVTagToolList.append(tag)

  #add all the taggers that use MVTM
  for mvtm_tagger in mvtm_active_taggers:
    tag = ConfInstance.addTool(mvtm_tagger+'Tag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
    if tag not in MVTM.MVTagToolList:
      MVTM.MVTagToolList.append(tag)

  #add all the flip taggers that use MVTM
  for mvtm_tagger in mvtm_active_flip_taggers:
    tag = ConfInstance.addTool(mvtm_tagger+'Tag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
    if tag not in MVTMFlip.MVTagToolList:
      MVTMFlip.MVTagToolList.append(tag)

  if BTaggingFlags.OutputLevel < 3:
    ConfInstance.printAllTools()
      # NOTE: Printing all tools might be excessive; however the adding of future tools might affect the earlier ones; so printing
      # only really makes sense at the end, so we either have to do it each time; or get people to add the print command in their
      # jobOptions. Let us go for the excessive printing in this case; which is only done if debugging anyway.

  # Return True to denote all was successful
  return True

def SetupJetCollectionTrig(JetCollection, TaggerList, ConfInstance = None):
  """Sets up the trigger B-Tagging configuration for jet collection called JetCollection. Returns True if successful. The function does nothing
  (and returns True) if a configuration for the given jet collection already exists; therefore the function is idempotent.

  TaggerList is a list of taggers (the names coming from BTaggingFlags._tags (see SetupJetCollection() for how the default is filled))."""
  if ConfInstance is None:
    from BTagging.BTaggingConfiguration import getConfiguration
    ConfInstance = getConfiguration()

  from AthenaCommon.AppMgr import ToolSvc

  # -- setup basic b-tagging tool for this jet collection
  # Note that we are not storing this tool on a variable on purpose; there is now a different one per jet collection and therefore
  # subsequent scripts cannot depend on this variable (since it would just hold the last one initialized).
  ConfInstance.addJetCollectionTool(JetCollection, ToolSvc, Verbose = BTaggingFlags.OutputLevel < 3,
                                  options={'BTagLabelingTool'            : ConfInstance.getTool("thisBTagLabeling", JetCollection=JetCollection),
                                           'storeSecondaryVerticesInJet' : BTaggingFlags.writeSecondaryVertices})

  # Setup associators
  BTagTrackToJetAssociator = ConfInstance.setupTrackAssociator('BTagTrackToJetAssociator', JetCollection, ToolSvc, Verbose = BTaggingFlags.OutputLevel < 3)
  if 'SoftMu' in TaggerList or 'SoftMuChi2' in TaggerList:
    BTagMuonToJetAssociator = ConfInstance.setupMuonAssociator('Muons', JetCollection, ToolSvc, Verbose = BTaggingFlags.OutputLevel < 3)
  else:
    BTagMuonToJetAssociator = None
  if 'SoftEl' in TaggerList:
    BTagElectronToJetAssociator = ConfInstance.setupElectronAssociator('Electrons', JetCollection, ToolSvc,
                                                                     Verbose = BTaggingFlags.OutputLevel < 3,
                                                                     PhotonCollectionName = 'Photons')
  else:
    BTagElectronToJetAssociator = None
  if 'MultiSVbb1' in TaggerList or 'MultiSVbb2' in TaggerList:
    ConfInstance.setupTrackAssociator('BTagTrackToJetAssociatorBB', JetCollection, ToolSvc,
                                    Verbose = BTaggingFlags.OutputLevel < 3,
                                    options={'shareTracks': False,
                                             'useVariableSizedTrackCone' : True,
                                             'coneSizeFitPar1' : 3.15265e-01,
                                             'coneSizeFitPar2' : -3.66502e-01,
                                             'coneSizeFitPar3' : -1.56387e-05})

  # NOTE: The secondary vertex finders etc. don't need to be set up here depending on the flags; they are set up when needed by the
  # taggers below.

  # Setup all taggers

  #Now the basic taggers:
  if 'IP2D' in TaggerList:
    ConfInstance.addTool('IP2DTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if 'IP2DNeg' in TaggerList:
    ConfInstance.addTool('IP2DNegTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if 'IP2DTrigHybrid' in TaggerList:
    ConfInstance.addTool('IP2DTrigHybridTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose=BTaggingFlags.OutputLevel < 3)
  if 'IP3D' in TaggerList:
    ConfInstance.addTool('IP3DTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if 'IP3DTrigHybrid' in TaggerList:
    ConfInstance.addTool('IP3DTrigHybridTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose=BTaggingFlags.OutputLevel < 3)
  if 'IP3DNeg' in TaggerList:
    ConfInstance.addTool('IP3DNegTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if 'SV1' in TaggerList:
    ConfInstance.addTool('SV1Tag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if 'SV1Flip' in TaggerList:
    ConfInstance.addTool('SV1FlipTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if 'SV0' in TaggerList:
    ConfInstance.addTool('SV0Tag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)

  if 'MultiSVbb1' in TaggerList:
    ConfInstance.addTool('MultiSVbb1Tag', ToolSvc, 'BTagTrackToJetAssociatorBB', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if 'MultiSVbb2' in TaggerList:
    ConfInstance.addTool('MultiSVbb2Tag', ToolSvc, 'BTagTrackToJetAssociatorBB', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)

  if 'JetFitterNN' in TaggerList:
    ConfInstance.addTool('JetFitterTagNN', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)

  if 'JetFitterCOMBNN' in TaggerList:
    ConfInstance.addTool('JetFitterTagCOMBNN', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)

  if 'JetFitterNNFlip' in TaggerList:
    ConfInstance.addTool('JetFitterTagNNFlip', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
    if 'IP3DNeg' in TaggerList:
      ConfInstance.addTool('JetFitterTagCOMBNNIP3DNeg', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)

  if 'MV1' in TaggerList:
    ConfInstance.addTool('MV1Tag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if 'MV1c' in TaggerList:
    ConfInstance.addTool('MV1cTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if 'MV1cFlip' in TaggerList:
    ConfInstance.addTool('MV1cFlipTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if 'MV1Flip' in TaggerList:
    ConfInstance.addTool('MV1FlipTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if 'JetVertexCharge' in TaggerList:   #LC FIXME
    ConfInstance.addTool('JetVertexCharge', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if 'MV2c10TrigHybrid' in TaggerList:
    ConfInstance.addTool('MV2c10TrigHybridTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose=BTaggingFlags.OutputLevel < 3)

  # Activate MultivariateTagManager if any of its taggers are in TaggerList
  #list of mvtm taggers that are also in TaggerList
  mvtm_active_taggers = list(set(mvtm_taggers) & set(TaggerList))
  mvtm_active_flip_taggers = list(set(mvtm_flip_taggers) & set(TaggerList))
  mvtm_active_trighybrid_taggers = list(set(mvtm_trighybrid_taggers) & set(TaggerList))

  #set up MVTM if any of its taggers are active
  if (mvtm_active_taggers):
    MVTM = ConfInstance.addTool('MultivariateTagManager', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
    if 'RNNIP' in TaggerList:
      MVTM.arbitraryAuxData = BTaggingFlags.MultivariateTagManagerAuxBranches
    else:
      MVTM.arbitraryAuxData=[]

  #set up MVTMFlip
  if (mvtm_active_flip_taggers):
    MVTMFlip = ConfInstance.addTool('MultivariateFlipTagManager', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)

  # set up MVTMHybrid
  if (mvtm_active_trighybrid_taggers):
    MVTMTrigHybrid = ConfInstance.addTool('MultivariateTrigHybridTagManager', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose=BTaggingFlags.OutputLevel < 3)

  if 'TagNtupleDumper' in TaggerList:
    tag = ConfInstance.addTool('TagNtupleDumper', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
    MVTM.MVTagToolList.append(tag)

  #add all the taggers that use MVTM
  for mvtm_tagger in mvtm_active_taggers:
    tag = ConfInstance.addTool(mvtm_tagger+'Tag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
    if tag not in MVTM.MVTagToolList:
      MVTM.MVTagToolList.append(tag)

  #add all the flip taggers that use MVTM
  for mvtm_tagger in mvtm_active_flip_taggers:
    tag = ConfInstance.addTool(mvtm_tagger+'Tag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
    if tag not in MVTMFlip.MVTagToolList:
      MVTMFlip.MVTagToolList.append(tag)

  # add all the hybrid tuning taggers that use MVTM
  for mvtm_tagger in mvtm_active_trighybrid_taggers:
    tag = ConfInstance.addTool(mvtm_tagger + 'Tag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose=BTaggingFlags.OutputLevel < 3)
    if tag not in MVTMTrigHybrid.MVTagToolList:
      MVTMTrigHybrid.MVTagToolList.append(tag)

  if BTaggingFlags.OutputLevel < 3:
    ConfInstance.printAllTools()
      # NOTE: Printing all tools might be excessive; however the adding of future tools might affect the earlier ones; so printing
      # only really makes sense at the end, so we either have to do it each time; or get people to add the print command in their
      # jobOptions. Let us go for the excessive printing in this case; which is only done if debugging anyway.

  # Return True to denote all was successful
  return True
