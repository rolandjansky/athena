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

import re
#
# ========== Load and configure everything
#
if BTaggingFlags.Active:

  #Jet collections
  JetCollectionList = ['AntiKt4LCTopoJets', 'AntiKt10LCTopoJets', 'AntiKt4EMTopoJets']
  from JetRec.JetRecFlags import jetFlags
  if jetFlags.useTruth():
    JetCollectionList += [ 'AntiKt10TruthJets', 'AntiKt10TruthWZJets', 'AntiKt4TruthJets', 'AntiKt4TruthWZJets' ]


  BTaggingFlags.CalibrationChannelAliases += [ "AntiKt4EMTopo->AntiKt4TopoEM,AntiKt4H1Topo" ]
  BTaggingFlags.CalibrationChannelAliases += [ "AntiKt4LCTopo->AntiKt4LCTopo,AntiKt4TopoEM,AntiKt4H1Topo" ]
  BTaggingFlags.CalibrationChannelAliases += [ "AntiKt10LCTopo->AntiKt6LCTopo,AntiKt6TopoEM,AntiKt4TopoEM,AntiKt4H1Topo" ]
  BTaggingFlags.CalibrationChannelAliases += [ "AntiKt10Truth->AntiKt6TopoEM,AntiKt4TopoEM,AntiKt4H1Topo" ]
  BTaggingFlags.CalibrationChannelAliases += [ "AntiKt10TruthWZ->AntiKt6TopoEM,AntiKt4TopoEM,AntiKt4H1Topo" ]
  BTaggingFlags.CalibrationChannelAliases += [ "AntiKt4Truth->AntiKt4TopoEM,AntiKt4H1Topo" ]
  BTaggingFlags.CalibrationChannelAliases += [ "AntiKt4TruthWZ->AntiKt4TopoEM,AntiKt4H1Topo" ]

  BTaggingFlags.Jets = [ name[:-4] for name in JetCollectionList]

  #BTagging list
  btag = "BTagging_"
  #TODO define name author (now BTagging_AntiKt4LCTopo)
  AuthorSubString = [ btag+name[:-4] for name in JetCollectionList] 

  tmpSVname = "SecVtx"
  tmpJFVxname = "JFVtx"

  include( "BTagging/BTagging_Rel19_LoadTools.py" )
  # -- b-tagging tool is now fully configured:
  if BTaggingFlags.OutputLevel < 3:
    print myBTagTool


  from BTagging.BTaggingConf import Analysis__JetBTaggerTool as JetBTaggerTool
  #### should be unique per collection
  #### replaces the jet rec JetBTaggerTool config or the BJetBuilder config in case of retagging
 

  NotInJetToolManager = [] # For jet collections
  from JetRec.JetRecStandard import jtm
  for i, jet in enumerate(JetCollectionList):
      try: 
          jetname = getattr(jtm, jet)
          btagger = JetBTaggerTool(AuthorSubString[i].lower(), 
                                   BTagTool=myBTagTool, 
                                   BTagName = AuthorSubString[i], 
                                   BTagTrackAssocTool = myBTagTrackAssociation, 
                                   BTagSVName = tmpSVname, 
                                   BTagJFVtxName = tmpJFVxname, 
                                   BTagSecVertexing=myBTagSecVtx)

          ToolSvc += btagger
          jetname.unlock()
          jetname.JetModifiers += [ btagger ]
          jetname.lock()
      except AttributeError as error:
          print '#BTAG# --> ' + str(error)
          NotInJetToolManager.append(AuthorSubString[i])

  if len(NotInJetToolManager) > 0:
      AuthorSubString = list(set(AuthorSubString) - set(NotInJetToolManager))

  # Both standard and aux container must be listed explicitly.
  # For release 19, the container version must be explicit.
  BaseName = "xAOD::BTaggingContainer_v1#"
  BaseAuxName = "xAOD::BTaggingAuxContainer_v1#"
  #AOD list
  BTaggingFlags.btaggingAODList += [ BaseName + author for author in AuthorSubString]
  BTaggingFlags.btaggingAODList += [ BaseAuxName + author + 'Aux.' for author in AuthorSubString]
  #ESD list
  BTaggingFlags.btaggingESDList += [ BaseName + author for author in AuthorSubString]
  BTaggingFlags.btaggingESDList += [ BaseAuxName + author + 'Aux.' for author in AuthorSubString]

  #AOD list SeCVert
  BaseNameSecVtx = "xAOD::VertexContainer_v1#"
  BaseAuxNameSecVtx = "xAOD::VertexAuxContainer_v1#"
  BTaggingFlags.btaggingAODList += [ BaseNameSecVtx + author + tmpSVname for author in AuthorSubString]
  BTaggingFlags.btaggingAODList += [ BaseAuxNameSecVtx + author + tmpSVname + 'Aux.-vxTrackAtVertex' for author in AuthorSubString]
  #ESD list
  BTaggingFlags.btaggingESDList += [ BaseNameSecVtx + author + tmpSVname for author in AuthorSubString]
  BTaggingFlags.btaggingESDList += [ BaseAuxNameSecVtx + author + tmpSVname + 'Aux.-vxTrackAtVertex' for author in AuthorSubString]


  # #AOD list JFSeCVert
  # BaseNameJFSecVtx = "xAOD::BTagVertexContainer_v1#"
  # BaseAuxNameJFSecVtx = "xAOD::BTagVertexAuxContainer_v1#"
  # BTaggingFlags.btaggingAODList += [ BaseNameJFSecVtx + author + tmpJFVxname for author in AuthorSubString]
  # BTaggingFlags.btaggingAODList += [ BaseAuxNameJFSecVtx + author + tmpJFVxname + 'Aux.' for author in AuthorSubString]
  # #ESD list
  # BTaggingFlags.btaggingESDList += [ BaseNameJFSecVtx + author + tmpJFVxname for author in AuthorSubString]
  # BTaggingFlags.btaggingESDList += [ BaseAuxNameJFSecVtx + author + tmpJFVxname + 'Aux.' for author in AuthorSubString]

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
