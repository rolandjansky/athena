# ReadPool.py
# Read AlignableTransforms directly from file without using IOV database
# Use CondProxyProvider instead of IOVSvc to provide objects
# joboption FRAGMENT to be run e.g. in framework of InDetRecExample
#
# NOTE: Alignmemt now read by default from InDetAlignExample/InDetAlignable.py
# to use this, this default must be overridden, so put
# include.block("InDetAlignExample/InDetAlignable.py")
# at the top of your joboptions
#
ProxyProviderSvc = Service( "ProxyProviderSvc" )
ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
CondProxyProvider = Service( "CondProxyProvider" )
# set this to the file containing AlignableTransform objects
CondProxyProvider.InputCollections=["alignobjs1.root"]
CondProxyProvider.OutputLevel=2

#convertors for AlignableTransform objects
include ( "DetDescrCondAthenaPool/DetDescrCondAthenaPool_joboptions.py" )

# this preload causes callbacks for read in objects to be activated,
# allowing GeoModel to pick up the transforms
IOVSvc=Service("IOVSvc")
IOVSvc.PreLoadData=True
IOVSvc.OutputLevel=2

# tell SCT/pixel GeoModel to look for alignment constants
GeoModelSvc = Service( "GeoModelSvc" )
GeoModelSvc.PixelDetectorTool.Alignable=TRUE
GeoModelSvc.SCT_DetectorTool.Alignable=TRUE

