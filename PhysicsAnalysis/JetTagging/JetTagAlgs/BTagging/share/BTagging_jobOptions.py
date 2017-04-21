########################################
# BTagging_jobOptions.py
# author: andreas.wildauer@cern.ch
#         devivie@lal.in2p3.fr
#         vacavant@in2p3.fr 
#
# Main jobO for b-tagging:
# - load all the necessary tools
# - configure the b-tagging algorithms
########################################

# <================= IMPORTANT ==============================================>
  # !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  # NOTE: The inclusion of the LoadTools is no longer needed with the
  # new configuration; the default configuration is automatically set up
  # for any unconfigured jet collection where which setupJetBTaggerTool
  # is called. In fact the only thing the LoadTools does now is just call
  # this default setup on all jet collections in BTaggingFlags.Jets.
  #
  # If you need to modify the default setup permanently can modify
  # BTaggingConfiguration_LoadTools.py in the ./python/ directory.
  #
  # If you want different settings not obtainable via the BTaggingFlags,
  # you need to use the new configuration scheme before any call to
  # setupJetBTaggerTools is made for the jet collection in question.
  # You can start by calling BTaggingConfiguration_LoadTools.py's
  # Initiate() function.
  # !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# <================= IMPORTANT ==============================================>

# <================= IMPORTANT ==============================================>
  # !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  # NOTE: Consider adding some of the stuff found here to the Initiate()
  # function in BTaggingConfiguration_LoadTools.py. The code there is
  # run exactly once; if B-tagging is enabled.
  #
  # DOING SO WILL MAKE THE CODE COMPATIBLE WITH A FUTURE CHANGE IN JETREC
  # WHERE THEY WILL RETRIEVE OUR BTAGGING FUNCTION IF REQUESTED BY A USER.
  # !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# <================= IMPORTANT ==============================================>

import re

if not BTaggingFlags.DoNotSetupBTagging: # Temporary measure so the JetRec people can test setting this all up from their side.
  #
  # ========== Load and configure everything
  #
  
  from BTagging.BTaggingConfiguration import getConfiguration
  ConfInstance = getConfiguration()

  if ConfInstance.checkFlagsUsingBTaggingFlags():

    #Jet collections
    #JetCollectionList = ['AntiKt4LCTopoJets', 'AntiKt4EMTopoJets', 'AntiKt4TrackJets', 'AntiKt4EMPFlowJets', 'AntiKt2TrackJets']
    JetCollectionList = ['AntiKt4EMTopoJets']
    from JetRec.JetRecFlags import jetFlags
    #if jetFlags.useTruth():
    #  JetCollectionList += [ 'AntiKt10TruthWZJets', 'AntiKt4TruthWZJets' ]

    #WOUTER: Moved these into the BTaggingsFlags.py file.
    #BTaggingFlags.CalibrationChannelAliases += [ "AntiKt4EMTopo->AntiKt4TopoEM" ]
    #BTaggingFlags.CalibrationChannelAliases += [ "AntiKt4LCTopo->AntiKt4LCTopo,AntiKt4TopoEM" ]
    #BTaggingFlags.CalibrationChannelAliases += [ "AntiKt10LCTopo->AntiKt6LCTopo,AntiKt6TopoEM,AntiKt4TopoEM" ]
    #BTaggingFlags.CalibrationChannelAliases += [ "AntiKt10Truth->AntiKt6TopoEM,AntiKt4TopoEM" ]
    #BTaggingFlags.CalibrationChannelAliases += [ "AntiKt10TruthWZ->AntiKt6TopoEM,AntiKt4TopoEM" ]
    #BTaggingFlags.CalibrationChannelAliases += [ "AntiKt4Truth->AntiKt4TopoEM" ]
    #BTaggingFlags.CalibrationChannelAliases += [ "AntiKt4TruthWZ->AntiKt4TopoEM" ]
    #BTaggingFlags.CalibrationChannelAliases += [ "AntiKt4Track->AntiKt4TopoEM" ]
    #BTaggingFlags.CalibrationChannelAliases += [ "AntiKt3Track->AntiKt4TopoEM" ]

    BTaggingFlags.Jets = [ name[:-4] for name in JetCollectionList]

    #BTagging list
    btag = ConfInstance.getOutputFilesPrefix() #BTaggingFlags.OutputFilesBTag #"BTagging_"
    #tmpSVname = "SecVtx"
    #tmpJFVxname = "JFVtx"   

    #TODO define name author (now BTagging_AntiKt4LCTopo)
    AuthorSubString = [ btag+name[:-4] for name in JetCollectionList] 

    #include( "BTagging/BTagging_Rel19_LoadTools.py" )

    #from BTagging.BTaggingConf import Analysis__JetBTaggerTool as JetBTaggerTool
    #from BTagging.BTaggingConfiguration import setupJetBTaggerTool, getJetCollectionTool
    #### should be unique per collection
    #### replaces the jet rec JetBTaggerTool config or the BJetBuilder config in case of retagging
   

    NotInJetToolManager = [] # For jet collections
    from JetRec.JetRecStandard import jtm
    for i, jet in enumerate(JetCollectionList):
        # try: 
#          jet = jet.replace("Track", "PV0Track")
#          jetname = getattr(jtm, jet)
  #          btagger = JetBTaggerTool(AuthorSubString[i].lower(), 
  #                                   BTagTool=myBTagTool, 
  #                                   BTagName = AuthorSubString[i], 
  #                                   BTagTrackAssocTool = myBTagTrackAssociation, 
  #                                   BTagSVName = tmpSVname, 
  #                                   BTagJFVtxName = tmpJFVxname, 
  #                                   BTagSecVertexing=myBTagSecVtx)
  #
  #          ToolSvc += btagger
          btagger = ConfInstance.setupJetBTaggerTool(ToolSvc, jet) #The [:-4] is not needed here; this function automatically removes trailing 'jets' or 'Jets'.
          if btagger is None:
            continue
          jet = jet.replace("Track", "PV0Track")
          jetname = getattr(jtm, jet)
          jetname.unlock()
          jetname.JetModifiers += [ btagger ]
          jetname.lock()
          if BTaggingFlags.OutputLevel < 3:
            print ConfInstance.getJetCollectionTool(jet[:-4])
        # except AttributeError as error:
        #   print '#BTAG# --> ' + str(error)
        #   NotInJetToolManager.append(AuthorSubString[i])

    if len(NotInJetToolManager) > 0:
        AuthorSubString = list(set(AuthorSubString) - set(NotInJetToolManager))


    # WOUTER: THESE LISTS ARE NOW FILLED DYNAMICALLY AND GOVERNED BY BTAGGINGFLAGS (see ./python/BTaggingsFlags.py and ./python/BTaggingConfiguration.py (function called RegisterOutputContainersForJetCollection)
    # Both standard and aux container must be listed explicitly.
    # For release 19, the container version must be explicit.
    #BaseName = "xAOD::BTaggingContainer_v1#"
    #BaseAuxName = "xAOD::BTaggingAuxContainer_v1#"
    #AOD list
    #BTaggingFlags.btaggingAODList += [ BaseName + author for author in AuthorSubString]
    #BTaggingFlags.btaggingAODList += [ BaseAuxName + author + 'Aux.' for author in AuthorSubString]
    #ESD list
    #BTaggingFlags.btaggingESDList += [ BaseName + author for author in AuthorSubString]
    #BTaggingFlags.btaggingESDList += [ BaseAuxName + author + 'Aux.' for author in AuthorSubString]

    #AOD list SeCVert
    #BaseNameSecVtx = "xAOD::VertexContainer_v1#"
    #BaseAuxNameSecVtx = "xAOD::VertexAuxContainer_v1#"
    #BTaggingFlags.btaggingAODList += [ BaseNameSecVtx + author + tmpSVname for author in AuthorSubString]
    #BTaggingFlags.btaggingAODList += [ BaseAuxNameSecVtx + author + tmpSVname + 'Aux.-vxTrackAtVertex' for author in AuthorSubString]
    #ESD list
    #BTaggingFlags.btaggingESDList += [ BaseNameSecVtx + author + tmpSVname for author in AuthorSubString]
    #BTaggingFlags.btaggingESDList += [ BaseAuxNameSecVtx + author + tmpSVname + 'Aux.-vxTrackAtVertex' for author in AuthorSubString]

    #AOD list JFSeCVert
    #BaseNameJFSecVtx = "xAOD::BTagVertexContainer_v1#"
    #BaseAuxNameJFSecVtx = "xAOD::BTagVertexAuxContainer_v1#"
    #BTaggingFlags.btaggingAODList += [ BaseNameJFSecVtx + author + tmpJFVxname for author in AuthorSubString]
    #BTaggingFlags.btaggingAODList += [ BaseAuxNameJFSecVtx + author + tmpJFVxname + 'Aux.' for author in AuthorSubString]
    #ESD list
    #BTaggingFlags.btaggingESDList += [ BaseNameJFSecVtx + author + tmpJFVxname for author in AuthorSubString]
    #BTaggingFlags.btaggingESDList += [ BaseAuxNameJFSecVtx + author + tmpJFVxname + 'Aux.' for author in AuthorSubString]

    # 
    # ========== Create and configure main b-tagging algorithms (one for each jet collection)
    #
  #  from BTagging.BTaggingConf import Analysis__BJetBuilder
  #  for key in BTaggingFlags.Jets:
  #    if key in BTaggingFlags.RetagJets:
  #      buildername = "BJetBuilder"+key
  #      jetBasis = "Cells"
  #      if key.find('Track') >= 0:
  #        jetBasis = "Tracks"
  #      if BTaggingFlags.OutputLevel < 3:
  #        print '### creating ',buildername,' ',jetBasis
  #      thisAlg = Analysis__BJetBuilder(
  #        name                       = buildername,
  #        Runmodus                   = BTaggingFlags.Runmodus,
  ##        TrackParticleContainerName = BTaggingFlags.TrackParticleCollectionName,
  #        OutputLevel                = BTaggingFlags.OutputLevel,
  #        BTagTool                   = myBTagTool,
  #        KeepTruthInfo              = not BTaggingFlags.ConvertParticleJets,
  #        jetFinderBasedOn           = jetBasis,
  #        BTagTrackAssocTool         = thisBTagTrackAssociation,
  #        BTagAssociation            = BTaggingFlags.doStandardAssoc,
  #        MuonToJetAssociator       = BTagMuonToJetAssociator,     # OK: always the same
  #        ElectronToJetAssociator   = BTagElectronToJetAssociator, # will also not be duplicated in memory
  #        JetCollectionName          = key+"Jets")
  #  # -- association of tracks to jet (for the time being same one for all jet collections):
  #      if jetBasis == "Tracks" and not BTaggingFlags.doStandardAssoc:
  #        if BTaggingFlags.OutputLevel < 3:
  #          print '### jetBasis tracks, and use the track constituents for tagging'
  #        thisAlg.ParticleToJetAssociator = None
  #      else:
  #        if BTaggingFlags.OutputLevel < 3:
  #          print '### jetBasis cells or tracks but using standard btagging track-jet association'
  #        thisAlg.ParticleToJetAssociator =  CfgMgr.Analysis__ParticleToJetAssociator("BTagTrackToJetAssociator")
  #  # -- configure leptons:
  #      if BTaggingFlags.SoftMu:
  #        thisAlg.MuonContainerName       = BTaggingFlags.MuonCollectionName
  #      if BTaggingFlags.SoftMuChi2:
  #        thisAlg.MuonContainerName       = BTaggingFlags.MuonCollectionName
  #      if BTaggingFlags.SoftEl:
  #        thisAlg.ElectronContainerName   = BTaggingFlags.ElectronCollectionName
  #        thisAlg.PhotonContainerName     = BTaggingFlags.PhotonCollectionName
  #      if BTaggingFlags.MultiSV:
  #        thisAlg.AdditionalTrackToJetAssocName = [ "TracksBB" ]
  #        thisAlg.AdditionalTrackToJetAssociator = [ToolSvc.BTagTrackToJetAssociatorBB ] 
          
    # -- add the algo:
  #      topSequence += thisAlg
  #      if BTaggingFlags.OutputLevel < 3:
         # print thisAlg
