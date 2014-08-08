from JetTagTools.JetTagToolsConf import Analysis__CombinerToolTag
JetFitterCOMB = Analysis__CombinerToolTag( name = "JetFitterCOMB",
                                           OutputLevel = BTaggingFlags.OutputLevel,
                                           combinedTagString = "JetFitterCOMB",
                                           listTaggers = [ "JetFitterTag","IP3D" ] )
                                           
ToolSvc += JetFitterCOMB
if BTaggingFlags.OutputLevel < 3:
  print JetFitterCOMB
