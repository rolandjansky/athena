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
    TheTruthCollectionKey = 'TruthEvent'
    BTaggingFlags.RetagJets = BTaggingFlags.Jets
    if BTaggingFlags.AutoInspectInputFile:
      from AthenaCommon.GlobalFlags import globalflags
      if globalflags.InputFormat == 'pool':
        try:
          from RecExConfig.InputFilePeeker import inputFileSummary
          BTaggingFlags.RetagJets = []
          for i in inputFileSummary['eventdata_items']:
            if i[0] == 'McEventCollection':
              TheTruthCollectionKey = i[1]
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

    # -- to label jets with truth information
    from RecExConfig.RecFlags import rec
    if rec.doTruth:
      # Jet Label
      if BTaggingFlags.jetTruthMatching == 'matchQuark' or BTaggingFlags.jetTruthMatching == 'jetTrackTruthMatching':
        BTagJetTruthMatching = addTool("BTagJetTruthMatching", 
                                       ToolSvc = ToolSvc, 
                                       Verbose = BTaggingFlags.OutputLevel < 3,
                                       options = {'subtype' : BTaggingFlags.jetTruthMatching},
                                       SuppressNonCloneWarning=True)
      BTagJetTruthMatching.McEventCollection = TheTruthCollectionKey
      # Lepton Truth Match
      #BTagLeptonTruthTool = addTool('BTagLeptonTruthTool', ToolSvc = ToolSvc, Verbose = BTaggingFlags.OutputLevel < 3)
      #BTagLeptonTruthTool.McEventCollection = TheTruthCollectionKey
      BTagLeptonTruthTool = None # TEMPORARY: BTagLeptonTruthTool not implemented yet?
      # Labeling tool
      thisBTagLabeling = addTool("thisBTagLabeling", ToolSvc = ToolSvc, Verbose = BTaggingFlags.OutputLevel < 3,
                                            options = {'JetTruthMatchTool' : BTagJetTruthMatching,
                                                       #'LeptonTruthTool'   : BTagLeptonTruthTool}, TEMPORARY: BTagLeptonTruthTool not implemented yet?
                                                       },
                                            SuppressNonCloneWarning=True)
    else:
      BTagJetTruthMatching = None
      BTagLeptonTruthTool  = None
      thisBTagLabeling = None
      # NOTE: These variables no longer serve any purpose, since any tool can be retrieved using getTool

    # -- for reference mode:
    if BTaggingFlags.Runmodus == 'reference':
      
      for key in BTaggingFlags.Jets:
        for tagger in ['JetProb','IP1D','IP2D','IP2DSpc','IP3D','IP3DSpc','SV1','SV2','SoftMu','SoftEl','JetFitterTagNN','JetFitterCOMBNN']:
          if BTaggingFlags.IsEnabled(tagger):
            RefileName = "BTaggingRef"+tagger+key+".root"
            svcMgr.THistSvc.Output += ["RefFile"+tagger+key+" DATAFILE='"+RefileName+"' OPT='RECREATE'"]

    Initialized = True
    return True
  else:
    print '#BTAG - WARNING - Tool initialization requested but B-Tagging is not possible for the current dataset.'
    return False

def DefaultSetupForJetCollection(jetcol):
  """Sets up the default B-Tagging configuration for jet collection called jetcol. Returns True if successful. Returns False if B-Tagging has
  been disabled for some reason. Also calls Initiate() if needed. The function does nothing (and returns True) if a configuration for the given
  jet collection already exists; therefore the function is idempotent."""
  # IMPORTANT NOTE: Many tools here exist globally, but this is okay; the addTool() function is idempotent; no additional tools will be set up.
  # They will just be registered to the specified jet collection (if not already the case).

  # !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  # NOTE: Many things are now commented out; these correspond to tools/taggers which
  # are not yet ported to release 19. Maybe the never will. The code is there to be used
  # if a porting attempt is made. Typically the code here should be fine; only the corresponding
  # configuration scripts require an update (BTaggingConfiguration_SomeTagger.py).
  # The associator tools (specifically the lepton one and the one for MultiSV) has not been
  # comment out; their code should still be up to date.
  # !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  from BTagging.BTaggingConfiguration import checkFlagsUsingBTaggingFlags, checkToolCollectionStructure, registerTool, addTool, jetCollectionExists, addJetCollectionTool, getTool, setupTrackAssociator, setupMuonAssociator, setupElectronAssociator, printAllTools

  if not Initiate():
    # If initiation fails; return False
    return False

  if jetCollectionExists(jetcol):
    return True

  print '#BTAG# - INFO - Using default settings for tagging jet collection: '+jetcol

  from AthenaCommon.AppMgr import ToolSvc

  # -- setup basic b-tagging tool for this jet collection
  # Note that we are not storing this tool on a variable on purpose; there is now a different one per jet collection and therefore
  # subsequent scripts cannot depend on this variable (since it would just hold the last one initialized).
  addJetCollectionTool(jetcol, ToolSvc, Verbose = BTaggingFlags.OutputLevel < 3,
                                  options={'BTagLabelingTool'            : getTool("thisBTagLabeling", JetCollection=jetcol),
                                           'storeSecondaryVerticesInJet' : BTaggingFlags.writeSecondaryVertices})
  
  # Setup associators
  BTagTrackToJetAssociator = setupTrackAssociator('BTagTrackToJetAssociator', jetcol, ToolSvc, Verbose = BTaggingFlags.OutputLevel < 3)
  if ( BTaggingFlags.SoftMu | BTaggingFlags.SoftMuChi2):
    BTagMuonToJetAssociator = setupMuonAssociator('Muons', jetcol, ToolSvc, Verbose = BTaggingFlags.OutputLevel < 3)
  else:
    BTagMuonToJetAssociator = None
  if BTaggingFlags.SoftEl:
    BTagElectronToJetAssociator = setupElectronAssociator('Electrons', jetcol, ToolSvc, 
                                                                     Verbose = BTaggingFlags.OutputLevel < 3,
                                                                     PhotonCollectionName = 'Photons')
  else:
    BTagElectronToJetAssociator = None
  if BTaggingFlags.MultiSVbb1 or BTaggingFlags.MultiSVbb2:
    setupTrackAssociator('BTagTrackToJetAssociatorBB', jetcol, ToolSvc,
                         Verbose = BTaggingFlags.OutputLevel < 3,
                         options={'shareTracks': False,
                                  'useVariableSizedTrackCone' : True,
                                  'coneSizeFitPar1' : 3.15265e-01,
                                  'coneSizeFitPar2' : -3.66502e-01,
                                  'coneSizeFitPar3' : -1.56387e-05})
    
  # -- extrapolation to propagate tracks to primary vertex:
  AtlasExtrapolator = addTool('AtlasExtrapolator', ToolSvc, 'BTagTrackToJetAssociator', jetcol,
                              Verbose = BTaggingFlags.OutputLevel < 3)
  BTagTrackToVertexTool = addTool('BTagTrackToVertexTool', ToolSvc, 'BTagTrackToJetAssociator', jetcol,
                                  Verbose = BTaggingFlags.OutputLevel < 3,
                                  options={'Extrapolator' : AtlasExtrapolator},
                                  SuppressNonCloneWarning=True)
  
  # -- tracking tools to sign the impact parameter:
  if (BTaggingFlags.IP1D | BTaggingFlags.IP2D | BTaggingFlags.IP3D |
      BTaggingFlags.JetProb |
      BTaggingFlags.TrackCounting |
      BTaggingFlags.SoftMu | BTaggingFlags.SoftMuChi2 | BTaggingFlags.SoftEl):
    BTagLinTrkFactory = addTool('BTagFullLinearizedTrackFactory', ToolSvc, 'BTagTrackToJetAssociator', jetcol,
                                           Verbose = BTaggingFlags.OutputLevel < 3,
                                           options={'Extrapolator' : AtlasExtrapolator},
                                           SuppressNonCloneWarning=True)
    BTagTrackToVertexIPEstimator = addTool('BTagTrackToVertexIPEstimator', ToolSvc, 'BTagTrackToJetAssociator', jetcol,
                                           Verbose = BTaggingFlags.OutputLevel < 3,
                                           options={'Extrapolator'           : AtlasExtrapolator,
                                                    'LinearizedTrackFactory' : BTagLinTrkFactory},
                                           SuppressNonCloneWarning=True)

  # -- load secondary vertex tools:
  if (BTaggingFlags.SV1 | BTaggingFlags.SV2 |
        BTaggingFlags.IP1D | BTaggingFlags.IP2D | BTaggingFlags.IP3D |
        BTaggingFlags.JetProb |
        BTaggingFlags.TrackCounting):
    addTool('InDetVKalVxInJetTool', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
      
  if BTaggingFlags.SV0:
    addTool('InDetVKalVxNegInJetTool', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
            
    #          if BTaggingFlags.MultiSV:
#            addTool('InDetVKalMultiVxInJetTool', ToolSvc, 'BTagTrackToJetAssociatorBB', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
        
#          if (BTaggingFlags.SV1Flip | BTaggingFlags.SV2Flip):
#            addTool('InDetVKalVxNegativeTagInJetTool', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)

#          if BTaggingFlags.BasicJetFitter:
#            addTool('JetFitterVxFinder', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)

  if BTaggingFlags.JetFitterTag | BTaggingFlags.JetFitterNN:
    addTool('NewJetFitterVxFinder', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)

#          if BTaggingFlags.JetFitterTagFlip | BTaggingFlags.JetFitterNNFlip:
#            addTool('NewJetFitterVxFinderFlip', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)

  # -- load all the taggers ! :
      
#          if BTaggingFlags.IP1D:
#            addTool('IP1DTag', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
  if BTaggingFlags.IP2D:
    addTool('IP2DTag', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.IP2DFlip:
#            addTool('IP2DFlipTag', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.IP2DPos:
#            addTool('IP2DPosTag', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.IP2DNeg:
#            addTool('IP2DNegTag', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.IP2DSpc:
#            addTool('IP2DSpcTag', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.IP2DSpcFlip:
#            addTool('IP2DSpcFlipTag', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.IP2DSpcPos:
#            addTool('IP2DSpcPosTag', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.IP2DSpcNeg:
#            addTool('IP2DSpcNegTag', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
  if BTaggingFlags.IP3D:
    addTool('IP3DTag', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.IP3DFlip:
#            addTool('IP3DFlipTag', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.IP3DPos:
#            addTool('IP3DPosTag', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.IP3DNeg:
#            addTool('IP3DNegTag', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.IP3DSpc:
#            addTool('IP3DSpcTag', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.IP3DSpcFlip:
#            addTool('IP3DSpcFlipTag', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.IP3DSpcPos:
#            addTool('IP3DSpcPosTag', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.IP3DSpcNeg:
#            addTool('IP3DSpcNegTag', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)

  if BTaggingFlags.SV1:
    addTool('SV1Tag', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.SV1Flip:
#            addTool('SV1FlipTag', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.SV2:
#            addTool('SV2Tag', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.SV2Flip:
#            addTool('SV2FlipTag', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
  if BTaggingFlags.SV0:
    addTool('SV0Tag', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)

  if BTaggingFlags.MultiSVbb1:
    addTool('MultiSVbb1Tag', ToolSvc, 'BTagTrackToJetAssociatorBB', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
  if BTaggingFlags.MultiSVbb2:
    addTool('MultiSVbb2Tag', ToolSvc, 'BTagTrackToJetAssociatorBB', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)

#          if BTaggingFlags.MultiSV:
#            # The name of this tagger was misspelled in the old code; I fixed it (Wouter)
#            addTool('MultiSVTag', ToolSvc, 'BTagTrackToJetAssociatorBB', jetcol, Verbose =  BTaggingFlags.OutputLevel < 3)

#          if BTaggingFlags.JetProb:
#            addTool('JetProbTag', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
#            if BTaggingFlags.JetProbFlip:
#              addTool('JetProbFlipTag', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)

#          if BTaggingFlags.SoftEl:
#            addTool('SoftElectronTag', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.SoftMu:
#            addTool('SoftMuonTag', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.SoftMuChi2:
#            addTool('SoftMuonTagChi2', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)

#          if BTaggingFlags.BasicJetFitter:
#            addTool('JetFitterTag', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
#            if BTaggingFlags.IP3D:
#              addTool('JetFitterCOMB', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.JetFitterTag:
#            addTool('NewJetFitterTag', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
#            if BTaggingFlags.IP3D:
#              addTool('NewJetFitterCOMB', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
  if BTaggingFlags.JetFitterNN:
    addTool('JetFitterTagNN', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
    if BTaggingFlags.IP3D:
      addTool('JetFitterTagCOMBNN', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)

#          if BTaggingFlags.JetFitterTagFlip:
#            addTool('NewJetFitterTagFlip', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
#            if BTaggingFlags.IP3DNeg:
#              addTool('NewJetFitterFlipCOMB', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.JetFitterNNFlip:
#            addTool('JetFitterTagNNFlip', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
#            if BTaggingFlags.IP3DPos:
#              addTool('JetFitterTagCOMBNNIP3DPos', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
#            if BTaggingFlags.IP3DNeg:
#              addTool('JetFitterTagCOMBNNIP3DNeg', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)

#          if BTaggingFlags.TrackCounting:
#            addTool('TrackCounting', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.TrackCountingFlip:
#            addTool('TrackCountingFlip', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
       
#          if BTaggingFlags.GbbNNTag:
#            addTool('GbbNNTag', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
       
  if BTaggingFlags.MV1:
    addTool('MV1Tag', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
  if BTaggingFlags.MVb:
    addTool('MVbTag', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
  if BTaggingFlags.MV1c:
    addTool('MV1cTag', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.MV1Flip:
#            addTool('MV1FlipTag', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.MV2:
#            addTool('MV2Tag', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
  if BTaggingFlags.MV2c00:
    addTool('MV2c00Tag', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
  if BTaggingFlags.MV2c10:
    addTool('MV2c10Tag', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
  if BTaggingFlags.MV2c20:
    addTool('MV2c20Tag', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)
#          if BTaggingFlags.MV2Flip:
#            addTool('MV2FlipTag', ToolSvc, 'BTagTrackToJetAssociator', jetcol, Verbose = BTaggingFlags.OutputLevel < 3)

  # -- b-tagging tool is now fully configured.

  if BTaggingFlags.OutputLevel < 3:
    printAllTools(PrintBJetBuilders = False, PrintOtherTools = True)
      # NOTE: Printing all tools might be excessive; however the adding of future tools might affect the earlier ones; so printing
      # only really makes sense at the end, so we either have to do it each time; or get people to add the print command in their
      # jobOptions. Let us go for the excessive printing in this case; which is only done if debugging anyway.
