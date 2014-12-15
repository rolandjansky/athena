# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

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

# The Initiate() function is called if any default setup is requested for any jet collection; but only once per run (it sets up the global tools).
Initialized = False

# Global list to hold settings of BTaggingFlags during scheme setup functions so users can use different taggers for different jet collections
# easily. This list is used after setup to restore BTaggingFlags.
BTaggingFlagsTaggers = []

def Initiate():
  """Sets up the basic global tools required for B-Tagging. This function is idempotent; it will not run again if it has run once. It is
  typically called by other functions in this file to make sure the basic global tools exist.

  The function returns True if B-Tagging is initialized or has been before. It returns False if it B-Tagging has been switched off for various
  reasons (these are checked in the checkFlagsUsingBTaggingFlags function).

  If the B-tagging calibration broker has been registered we assume initiation has already been performed; this allows users to setup their own initiation code."""
  from BTagging.BTaggingConfiguration import checkFlagsUsingBTaggingFlags, checkToolCollectionStructure, registerTool, addTool, jetCollectionExists, addJetCollectionTool, getTool, setupTrackAssociator, setupMuonAssociator, setupElectronAssociator, printAllTools
  global Initialized
  if Initialized:
    return True

  if getTool("BTagCalibrationBrokerTool"):
    print '#BTAG# - INFO - BTagCalibrationBrokerTool already exists prior to default initialization; assuming user set up entire initialization him/herself. Note however that if parts of the initalization were not set up, and a later tool requires them, they will be set up at that point automatically with default settings.'
    Initialized = True
    return True

  print '#BTAG# - INFO - Initializing default basic tools'

  if checkFlagsUsingBTaggingFlags():

    #Print the flags
    BTaggingFlags.Print()

    #If debugging do a check of the tool collection structure
    if(BTaggingFlags.OutputLevel < 3):
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
          print '#BTAG# - WARNING - Automatic inspection of input file failed (file too old?)'

    print '#BTAG# - Using ', TheTruthCollectionKey, ' as truth key'
    print '#BTAG# - Re-tagging these jet collections: ', BTaggingFlags.RetagJets

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
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon.Resilience import treatException,protectedInclude
    protectedInclude("BTagging/BTagCalibBroker_jobOptions.py") # New file which includes the file from JetCalibration and also registers it via registerTool() so it will be recognized by the various addTool() functions.
    BTagCalibrationBrokerTool = getTool("BTagCalibrationBrokerTool") # In case this variable is needed

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

    Initialized = True
    return True
  else:
    print '#BTAG# - WARNING - Tool initialization requested but B-Tagging is not possible for the current dataset.'
    return False

def BTaggingFlagsLoadTaggers(TaggerList):
  """Modifies the BTaggingFlags instance to set only those taggers listed in TaggerList. Keeps track of previous settings which can
  be restored using BTaggingFlagsReset(). This is a wrap around setup scheme functions so users can govern which taggers they want.
  Although technically a hack, it does allow us to support this without worrying about it ourselves during development (you can just
  keep on using BTaggingFlags). A RuntimeError is raised if this function is called twice in a row without a BTaggingFlagsReset in between."""
  global BTaggingFlagsTaggers
  if len(BTaggingFlagsTaggers) > 0:
    print("#BTAG# - ERROR - BTaggingFlagsLoadTaggers() called twice in a row without a BTaggingFlagsReset() call in between.")
    print("#BTAG# - ERROR - This has a high potential of undesired behaviour and is difficult to debug. Developers: please fix!")
    raise RuntimeError
  from BTagging.BTaggingConfiguration import getActiveTaggers
  # Store BTaggingFlags tagger list
  BTaggingFlagsTaggers = getActiveTaggers()
  # Print warning
  for tagger in TaggerList:
    if not tagger in BTaggingFlags._tags:
      print("#BTAG# - WARNING - Tagger flag '"+tagger+"' does not exist in BTaggingFlags. Setting will be ignored.")
      print("#BTAG# - WARNING - Valid options are found in BTagging/python/BTaggingFlags.py file in the _tags list.")
  # Set taggers correctly
  from BTagging.BTaggingConfiguration import taggerIsPossible
  for tagger in BTaggingFlags._tags:
    if tagger in TaggerList:
      if taggerIsPossible(tagger):
        BTaggingFlags.Enable(tagger)
      else:
        BTaggingFlags.Disable(tagger)
    else:
      BTaggingFlags.Disable(tagger)
    
def BTaggingFlagsReset():
  """Companion function to BTaggingFlagsLoadTaggers(). Resets the BTaggingFlags active tagger list."""
  global BTaggingFlagsTaggers
  if len(BTaggingFlagsTaggers) == 0:
    print("#BTAG# - WARNING - BTaggingFlagsReset() called without need. Might indicate code structure problems.")
  for tagger in BTaggingFlags._tags:
    if tagger in BTaggingFlagsTaggers:
      BTaggingFlags.Enable(tagger)
    else:
      BTaggingFlags.Disable(tagger)
  BTaggingFlagsTaggers = []

def SetupJetCollection(JetCollection, TaggerList=[], SetupScheme="Default"):
  """General function which can setup up a default B-Tagging configuration. Returns True if successful. Returns False if B-Tagging has
  been disabled for some reason. Also calls Initiate if needed. The function does nothing (and returns True) if a configuration for a given
  jet collection already exists; therefore the function is idempotent.

  The TaggerList holds a list of BTaggingFlags tagger names. Only these will be set up. If left empty the default taggers will be used instead.
  Note that if a tagger as a prerequisite which is not listed it will be set up nonetheless.

  The specific setup used depends on SetupScheme. This allows us to support multiple different default setup which can be easily set by users."""
  from BTagging.BTaggingConfiguration import jetCollectionExists
  print("#BTAG# - INFO - Setting up b-tagging for jet collection '"+JetCollection+"' using scheme: "+SetupScheme)
  if len(TaggerList) == 0:
    print("#BTAG# - INFO - Using default taggers.")
  else:
    print("#BTAG# - INFO - Using the following taggers:")
    for tagger in TaggerList:
      print("#BTAG# - INFO - "+tagger)
  if not Initiate():
    # If initiation fails; return False
    print("#BTAG# - ERROR - Basic initialization failed; disabling b-tagging!")
    return False
  if jetCollectionExists(JetCollection):
    if SetupScheme != "Default":
      print("#BTAG# - WARNING - Requesting setup of jet collection '"+JetCollection+"' with non-default scheme '"+SetupScheme+"' but")
      print("#BTAG# - WARNING - there is already a configuration for this jet collection. This scheme will therefore be ignored.")
    elif len(TaggerList) > 0:
      print("#BTAG# - WARNING - Requesting setup of jet collection '"+JetCollection+"' with non-default list of taggers, but")
      print("#BTAG# - WARNING - there is already a configuration for this jet collection. This list will therefore be ignored.")
    else:
      if BTaggingFlags.OutputLevel < 3:
        print("#BTAG# - DEBUG - SetupJetCollection called on jet collection which already exists. This is no problem, but any settings")
        print("#BTAG# - DEBUG - you pass to it using this function will be ignored.")
    return True
  ReturnValue = True
  if len(TaggerList) > 0:
    BTaggingFlagsLoadTaggers(TaggerList)
  try:
    exec 'ReturnValue = SetupJetCollection'+SetupScheme+'(JetCollection)'
  except:
    print("#BTAG# - ERROR - Attempted setup for scheme '"+SetupScheme+"' failed! Possibly this scheme does not exist or is improperly implemented!")
    raise
  if len(TaggerList) > 0:
    BTaggingFlagsReset()
  if not ReturnValue:
    print("#BTAG# - WARNING - A problem has occurred during the execution of SetupJetCollection"+SetupScheme)
    print("#BTAG# - WARNING - B-tagging might not function properly!")
  return ReturnValue

def SetupJetCollectionDefault(JetCollection):
  """Sets up the default B-Tagging configuration for jet collection called JetCollection. Returns True if successful. The function does nothing 
  (and returns True) if a configuration for the given jet collection already exists; therefore the function is idempotent."""
  # IMPORTANT NOTE: Many tools here exist globally, but this is okay; the addTool() function is idempotent; no additional tools will be set up.
  # They will just be registered to the specified jet collection (if not already the case).

  # !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  # NOTE: If there is any need to have different setup scheme then there is support for this.
  # See the SetupJetCollection() function. Your new function should have the following name:
  # SetupJetCollectionX(JetCollection, TaggerList=[])
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

  from BTagging.BTaggingConfiguration import checkFlagsUsingBTaggingFlags, checkToolCollectionStructure, registerTool, addTool, jetCollectionExists, addJetCollectionTool, getTool, setupTrackAssociator, setupMuonAssociator, setupElectronAssociator, printAllTools

  from AthenaCommon.AppMgr import ToolSvc

  # -- setup basic b-tagging tool for this jet collection
  # Note that we are not storing this tool on a variable on purpose; there is now a different one per jet collection and therefore
  # subsequent scripts cannot depend on this variable (since it would just hold the last one initialized).
  addJetCollectionTool(JetCollection, ToolSvc, Verbose = BTaggingFlags.OutputLevel < 3,
                                  options={'BTagLabelingTool'            : getTool("thisBTagLabeling", JetCollection=JetCollection),
                                           'storeSecondaryVerticesInJet' : BTaggingFlags.writeSecondaryVertices})
  
  # Setup associators
  BTagTrackToJetAssociator = setupTrackAssociator('BTagTrackToJetAssociator', JetCollection, ToolSvc, Verbose = BTaggingFlags.OutputLevel < 3)
  if ( BTaggingFlags.SoftMu | BTaggingFlags.SoftMuChi2):
    BTagMuonToJetAssociator = setupMuonAssociator('Muons', JetCollection, ToolSvc, Verbose = BTaggingFlags.OutputLevel < 3)
  else:
    BTagMuonToJetAssociator = None
  if BTaggingFlags.SoftEl:
    BTagElectronToJetAssociator = setupElectronAssociator('Electrons', JetCollection, ToolSvc, 
                                                                     Verbose = BTaggingFlags.OutputLevel < 3,
                                                                     PhotonCollectionName = 'Photons')
  else:
    BTagElectronToJetAssociator = None
  if BTaggingFlags.MultiSVbb1 or BTaggingFlags.MultiSVbb2:
    setupTrackAssociator('BTagTrackToJetAssociatorBB', JetCollection, ToolSvc,
                         Verbose = BTaggingFlags.OutputLevel < 3,
                         options={'shareTracks': False,
                                  'useVariableSizedTrackCone' : True,
                                  'coneSizeFitPar1' : 3.15265e-01,
                                  'coneSizeFitPar2' : -3.66502e-01,
                                  'coneSizeFitPar3' : -1.56387e-05})
    
  # -- extrapolation to propagate tracks to primary vertex:
  AtlasExtrapolator = addTool('AtlasExtrapolator', ToolSvc, 'BTagTrackToJetAssociator', JetCollection,
                              Verbose = BTaggingFlags.OutputLevel < 3)
  BTagTrackToVertexTool = addTool('BTagTrackToVertexTool', ToolSvc, 'BTagTrackToJetAssociator', JetCollection,
                                  Verbose = BTaggingFlags.OutputLevel < 3,
                                  options={'Extrapolator' : AtlasExtrapolator},
                                  SuppressNonCloneWarning=True)
  
  # -- tracking tools to sign the impact parameter:
  if (BTaggingFlags.IP1D | BTaggingFlags.IP2D | BTaggingFlags.IP3D |
      BTaggingFlags.JetProb |
      BTaggingFlags.TrackCounting |
      BTaggingFlags.SoftMu | BTaggingFlags.SoftMuChi2 | BTaggingFlags.SoftEl):
    BTagLinTrkFactory = addTool('BTagFullLinearizedTrackFactory', ToolSvc, 'BTagTrackToJetAssociator', JetCollection,
                                           Verbose = BTaggingFlags.OutputLevel < 3,
                                           options={'Extrapolator' : AtlasExtrapolator},
                                           SuppressNonCloneWarning=True)
    BTagTrackToVertexIPEstimator = addTool('BTagTrackToVertexIPEstimator', ToolSvc, 'BTagTrackToJetAssociator', JetCollection,
                                           Verbose = BTaggingFlags.OutputLevel < 3,
                                           options={'Extrapolator'           : AtlasExtrapolator,
                                                    'LinearizedTrackFactory' : BTagLinTrkFactory},
                                           SuppressNonCloneWarning=True)

  # -- load secondary vertex tools:
  if (BTaggingFlags.SV1 | BTaggingFlags.SV2 |
        BTaggingFlags.IP1D | BTaggingFlags.IP2D | BTaggingFlags.IP3D |
        BTaggingFlags.JetProb |
        BTaggingFlags.TrackCounting):
    addTool('InDetVKalVxInJetTool', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
      
  if BTaggingFlags.SV0:
    addTool('InDetVKalVxNegInJetTool', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
            
    #          if BTaggingFlags.MultiSV:
#            addTool('InDetVKalMultiVxInJetTool', ToolSvc, 'BTagTrackToJetAssociatorBB', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
        
  if (BTaggingFlags.SV1Flip | BTaggingFlags.SV2Flip):
    addTool('InDetVKalVxNegativeTagInJetTool', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)

#          if BTaggingFlags.BasicJetFitter:
#            addTool('JetFitterVxFinder', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)

  if BTaggingFlags.JetFitterTag | BTaggingFlags.JetFitterNN:
    addTool('NewJetFitterVxFinder', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)

#          if BTaggingFlags.JetFitterTagFlip | BTaggingFlags.JetFitterNNFlip:
#            addTool('NewJetFitterVxFinderFlip', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)

  # -- load all the taggers ! :
      
#          if BTaggingFlags.IP1D:
#            addTool('IP1DTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if BTaggingFlags.IP2D:
    addTool('IP2DTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.IP2DFlip:
#            addTool('IP2DFlipTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.IP2DPos:
#            addTool('IP2DPosTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if BTaggingFlags.IP2DNeg:
    addTool('IP2DNegTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.IP2DSpc:
#            addTool('IP2DSpcTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.IP2DSpcFlip:
#            addTool('IP2DSpcFlipTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.IP2DSpcPos:
#            addTool('IP2DSpcPosTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.IP2DSpcNeg:
#            addTool('IP2DSpcNegTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if BTaggingFlags.IP3D:
    addTool('IP3DTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.IP3DFlip:
#            addTool('IP3DFlipTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.IP3DPos:
#            addTool('IP3DPosTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if BTaggingFlags.IP3DNeg:
    addTool('IP3DNegTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.IP3DSpc:
#            addTool('IP3DSpcTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.IP3DSpcFlip:
#            addTool('IP3DSpcFlipTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.IP3DSpcPos:
#            addTool('IP3DSpcPosTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.IP3DSpcNeg:
#            addTool('IP3DSpcNegTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)

  if BTaggingFlags.SV1:
    addTool('SV1Tag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if BTaggingFlags.SV1Flip:
    addTool('SV1FlipTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.SV2:
#            addTool('SV2Tag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.SV2Flip:
#            addTool('SV2FlipTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if BTaggingFlags.SV0:
    addTool('SV0Tag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)

  if BTaggingFlags.MultiSVbb1:
    addTool('MultiSVbb1Tag', ToolSvc, 'BTagTrackToJetAssociatorBB', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if BTaggingFlags.MultiSVbb2:
    addTool('MultiSVbb2Tag', ToolSvc, 'BTagTrackToJetAssociatorBB', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)

#          if BTaggingFlags.MultiSV:
#            # The name of this tagger was misspelled in the old code; I fixed it (Wouter)
#            addTool('MultiSVTag', ToolSvc, 'BTagTrackToJetAssociatorBB', JetCollection, Verbose =  BTaggingFlags.OutputLevel < 3)

#          if BTaggingFlags.JetProb:
#            addTool('JetProbTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#            if BTaggingFlags.JetProbFlip:
#              addTool('JetProbFlipTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)

#          if BTaggingFlags.SoftEl:
#            addTool('SoftElectronTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.SoftMu:
#            addTool('SoftMuonTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
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
  if BTaggingFlags.JetFitterNN:
    addTool('JetFitterTagNN', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
    if BTaggingFlags.IP3D:
      addTool('JetFitterTagCOMBNN', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)

#          if BTaggingFlags.JetFitterTagFlip:
#            addTool('NewJetFitterTagFlip', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#            if BTaggingFlags.IP3DNeg:
#              addTool('NewJetFitterFlipCOMB', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if BTaggingFlags.JetFitterNNFlip:
    addTool('JetFitterTagNNFlip', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#            if BTaggingFlags.IP3DPos:
#              addTool('JetFitterTagCOMBNNIP3DPos', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
    if BTaggingFlags.IP3DNeg:
      addTool('JetFitterTagCOMBNNIP3DNeg', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)

#          if BTaggingFlags.TrackCounting:
#            addTool('TrackCounting', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.TrackCountingFlip:
#            addTool('TrackCountingFlip', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
       
#          if BTaggingFlags.GbbNNTag:
#            addTool('GbbNNTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
       
  if BTaggingFlags.MV1:
    addTool('MV1Tag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if BTaggingFlags.MVb:
    addTool('MVbTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if BTaggingFlags.MVbFlip:
    addTool('MVbFlipTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if BTaggingFlags.MV1c:
    addTool('MV1cTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if BTaggingFlags.MV1cFlip:
    addTool('MV1cFlipTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if BTaggingFlags.MV1Flip:
    addTool('MV1FlipTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#  if BTaggingFlags.MV2:
#    addTool('MV2Tag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if BTaggingFlags.MV2c00:
    addTool('MV2c00Tag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if BTaggingFlags.MV2c00Flip:
    addTool('MV2c00FlipTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if BTaggingFlags.MV2c10:
    addTool('MV2c10Tag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if BTaggingFlags.MV2c10Flip:
    addTool('MV2c10FlipTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if BTaggingFlags.MV2c20:
    addTool('MV2c20Tag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
  if BTaggingFlags.MV2c20Flip:
    addTool('MV2c20FlipTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.MV2Flip:
#            addTool('MV2FlipTag', ToolSvc, 'BTagTrackToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)

  # -- b-tagging tool is now fully configured.

  if BTaggingFlags.OutputLevel < 3:
    printAllTools(PrintOtherTools = True)
      # NOTE: Printing all tools might be excessive; however the adding of future tools might affect the earlier ones; so printing
      # only really makes sense at the end, so we either have to do it each time; or get people to add the print command in their
      # jobOptions. Let us go for the excessive printing in this case; which is only done if debugging anyway.

  # Return True to denote all was successful
  return True
