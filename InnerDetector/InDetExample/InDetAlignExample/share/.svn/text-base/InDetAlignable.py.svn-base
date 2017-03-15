
#Job options needed to enable alignment for Inner Detector

# load converters for AlignableTransforms
include ( "DetDescrCondAthenaPool/DetDescrCondAthenaPool_joboptions.py" )

# Alignment folders
IOVDbSvc = Service( "IOVDbSvc" )
IOVDbSvc.Folders+=[ CondDBCool.INDET + "/Indet/Align"]
IOVDbSvc.Folders+=[ CondDBCool.TRT   + "/TRT/Align"]

# Eventually there will be a central mechanism to override tags, but in the meantime
# just replace the above lines with a specific tag like in the following lines:
#IOVDbSvc.Folders+=[ CondDBCool.INDET + "/Indet/Align" + "<tag>InDetAlign_CSC_02</tag>"]
#IOVDbSvc.Folders+=[ CondDBCool.TRT   + "/TRT/Align"   + "<tag>TRTAlign_CSC_01</tag>"]
#
# Or alternatively include InDetAlignExample/InDetAlignableOveride.py before the geometry initialization
# and then specify the folders in your job options.



