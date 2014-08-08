from JetTagTools.JetTagToolsConf import Analysis__CombinerToolTag
NewJetFitterFlipCOMB = Analysis__CombinerToolTag( name = "NewJetFitterFlipCOMB",
                                                  OutputLevel = BTaggingFlags.OutputLevel,
                                                  combinedTagString = "JetFitterCOMBFlip",
                                                  listTaggers = [ "JetFitterTagFlip","IP3DNeg" ] )
                                           
ToolSvc += NewJetFitterFlipCOMB
if BTaggingFlags.OutputLevel < 3:
  print NewJetFitterFlipCOMB
