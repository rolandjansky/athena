from JetTagTools.JetTagToolsConf import Analysis__CombinerToolTag
BaselineTaggerTool = Analysis__CombinerToolTag( name = "BaselineTagger",
                                                OutputLevel = BTaggingFlags.OutputLevel,
                                                combinedTagString = BTaggingFlags.BaselineTagger,
                                                listTaggers = [ "SV1","IP3D" ] )
                                           
ToolSvc += BaselineTaggerTool
if BTaggingFlags.OutputLevel < 3:
  print BaselineTaggerTool

myBTagTool.TagToolList += [ BaselineTaggerTool ]
