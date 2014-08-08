from JetTagTools.JetTagToolsConf import Analysis__CombinerToolTag
NewJetFitterCOMB = Analysis__CombinerToolTag( name = "NewJetFitterCOMB",
                                              OutputLevel = BTaggingFlags.OutputLevel,
                                              combinedTagString = "JetFitterCOMB",
                                              listTaggers = [ "JetFitterTag","IP3D" ] )
                                           
ToolSvc += NewJetFitterCOMB
if BTaggingFlags.OutputLevel < 3:
  print NewJetFitterCOMB
