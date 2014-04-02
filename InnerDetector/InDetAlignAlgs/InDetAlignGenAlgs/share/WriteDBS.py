# setup for writing SCT+pixel alignment data to COOL or Lisbon CondDB
# job option FRAGMENT to be run in conjunction with e.g. InDetRecExample
#
include("IOVDbSvc/IOVRecExCommon.py")
# setup for COOL (write password deleted!)
IOVDbSvc=Service("IOVDbSvc")
IOVDbSvc.dbConnection = "impl=cool;techno=oracle;schema=ATLAS_COOL_INDET;ATLAS_COOLPROD:OFLPROD:ATLAS_COOL_INDET:xxxx"

# setup for Lisbon CondDBMySQL (write password deleted!)
#IOVDbSvc.dbConnection = "impl=conddb;techno=mysql;atlasdbdev.cern.ch:conditions_dc2:rdrCnDC2sDB:"
# for Lisbon, must also set newDB to FALSE to use old scheme where each
# AlignableTransform objects goes to a separate folder
# ToolSvc.InDetAlignDBTool.NewDB=FALSE

include( "RegistrationServices/IOVRegistrationSvc_jobOptions.py" )
regSvc = Service( "IOVRegistrationSvc" )
regSvc.RecreateFolders = False

# include convertors for AlignableTransform objects, and DB write tool
include ( "DetDescrCondAthenaPool/DetDescrCondAthenaPool_joboptions.py" )

ToolSvc=Service("ToolSvc")
ToolSvc.CondStream1.OutputFile="alignobjs.root"

theApp.Dlls    += [ "InDetAlignGenAlgs","InDetAlignGenTools" ]
theApp.TopAlg += [ "InDetAlignWrt" ]

InDetAlignWrt = Service( "InDetAlignWrt" )

InDetAlignWrt.create=TRUE
InDetAlignWrt.write=TRUE
# set to false to only write objects, not register in IOV database
InDetAlignWrt.WriteIOV=True
# set these to record a specific IOV, otherwise valid for all times
#InDetAlignWrt.ValidRun1=0
#InDetAlignWrt.ValidRun2=4020
#InDetAlignWrt.ValidEvent1=0
#InDetAlignWrt.ValidEvent2=10004
# set this to write data with a specific tag (otherwise HEAD tag)
InDetAlignWrt.IOVTag="test9"

# parameters affecting the displacmenets applied to SCT/pixel alignment
# before writing out
# DispMod 1 affects level1 transforms, 2 level 2, 3 level 3
InDetAlignWrt.DispMode=3
InDetAlignWrt.DispRphi=0.1
InDetAlignWrt.DispR=0.2
InDetAlignWrt.DispZ=0.5
InDetAlignWrt.DispDet=-1
InDetAlignWrt.DispBec=-1
InDetAlignWrt.DispLayer=-1
# 4 is random, 3 systematic
InDetAlignWrt.DispSyst=2
