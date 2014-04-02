# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

###############################################################
# IDAlignWrite.py
# Richard Hawkings, 14/2/06
# A standalone Athena joboption to read ID alignment constants from
# an existing POOL file, and rewrite/append them to a production POOL file
#  - optionally (p_migrate=true) changing format from old multifolder version
#     (used with Libson MySQL) to single folder/multichannel (used with COOL)
#    with this option, the full GeoModel geometry will be initialised.
#    if doing a straight new->new format copy, geometry is NOT initialised
#  - optionally also register the constants in COOL IOV database
#     (can be set to local SQlite file or Oracle (p_dbconn option)
#
# usage:  1) Copy to run directory, setup the  parameters you want below
#         2) execute athena.py IDAlignWrite.py
#         3) You will get an output POOL file of alignment constansts, a
#            corresponding mywrite.xml POOL file catalogue fragment, and
#            optionally a local SQLIte database file
#               
# Set parameters here
p_inputFile ="alignobjs1.root"
p_geom=1   # 1 for full ATLAS, 2 for combined testbeam
p_migrate=False
# detdescr only needed if migration is enabled (Rome-Initial, DC2, etc)
# using Rome-Initial will produce a two-layer pixel detector
p_detdescr="DC2"
p_outputFile="myalign.root"
p_registerIOV=False

# extra parameters for IOV registration
# register in a COOL database in a local SQlite file
p_dbconn="impl=cool;techno=sqlite;schema=mycool.db;X:TESTDB"
# for oracle, use something like (with xxxx replaced by write p/w):
# p_dbconn="impl=cool;techno=oracle;schema=ATLAS_COOL_INDET;ATLAS_COOLPROD:OFLPROD:ATLAS_COOL_INDET_W:xxxx"
p_tag="test1"
p_beginRun=0
p_beginEvent=0
p_endRun=99999999
p_endEvent=99999999

###############################################################

# this output will appear near beginning of your job
print 70*"*"
print "Input filename:",p_inputFile
if (p_geom==1):
    print "Assuming full ATLAS geometry"
elif (p_geom==2):
    print "Assuming combined testbeam geometry"
else:
    print "Invalid geometry definition"
    exit
if (p_migrate):
    print "Data will be migrated from old format to COOL format assuming geometry",p_detdescr
print "Output filename:",p_outputFile
print "If file exists, output will be overwritten"
if (p_registerIOV):
    print "Output will be registered in IOV database with tag:", p_tag
    print "Interval of validity from run",p_beginRun,"event",p_beginEvent,\
          "to run",p_endRun,"event",p_endEvent
else:
    print "Output will NOT be registered in IOV database, only POOL file\n"
print 70*"*"

#use McEventSelector
theApp.setup( MONTECARLO )
theApp.EvtMax=1

include ( "IOVDbSvc/IOVRecExCommon.py" )
IOVDbSvc=Service("IOVDbSvc")
IOVDbSvc.dbConnection=p_dbconn
include( "RegistrationServices/IOVRegistrationSvc_jobOptions.py" )
regSvc = Service( "IOVRegistrationSvc" )
regSvc.RecreateFolders = False

theApp.Dlls    += [ "InDetAlignGenAlgs","InDetAlignGenTools" ]
theApp.TopAlg += [ "InDetAlignWrt" ]
InDetAlignWrt = Algorithm( "InDetAlignWrt" )
ToolSvc=Service("ToolSvc")
ToolSvc.CondStream1.OutputFile=p_outputFile
InDetAlignWrt.create=False
InDetAlignWrt.Write=True
InDetAlignWrt.DispMode=0
ToolSvc.InDetAlignDBTool.NewDB=True
ToolSvc.InDetAlignDBTool.FakeDB=p_geom

# need to read in full geometry in migration mode
if (p_migrate):
    ToolSvc.IndetAlignDBTool.FakeDB=0
    InDetAlignWrt.Write=False
    InDetAlignWrt.Migrate=True
    ToolSvc.InDetAlignMigrate.NewDB=True
    ToolSvc.InDetAlignDBTool.NewDB=False
    if (p_geom==1):
        # set of options/flags to setup GeoModel for full geometry
        DetDescrVersion = p_detdescr
        from AthenaCommon.DetFlags import DetFlags 
        DetFlags.ID_setOn()
        include("AtlasGeoModel/SetGeometryVersion.py" )
        include("AtlasGeoModel/GeoModelInit.py" )
    if (p_geom==2):
        # set of options/flags to setup GeoModel for CTB (a bit fragile)
        DetDescrVersion = "CTB"
        include("InDetTBRecExample/InDetTBFlags_jobOptions.py")
        from AthenaCommon.GlobalFlags import GlobalFlags
        GlobalFlags.DetGeo.set_ctbh8()
        from AthenaCommon.DetFlags import DetFlags
        DetFlags.detdescr.ID_setOn()
        include("AtlasGeoModel/SetGeometryVersion.py" )
        include("AtlasGeoModel/GeoModelInit.py" )
        include("InDetTBRecExample/InDetTB2004DetDescr_jobOptions.py")
        IOVDbSvc.Folders=[]
        

# options for AlignWrt
InDetAlignWrt.WriteIOV=p_registerIOV
# set these to record a specific IOV, otherwise valid for all times
InDetAlignWrt.ValidRun1=p_beginRun
InDetAlignWrt.ValidRun2=p_endRun
InDetAlignWrt.ValidEvent1=p_beginEvent
InDetAlignWrt.ValidEvent2=p_endEvent
InDetAlignWrt.IOVTag=p_tag

# include POOL support and convertors for AlignableTransform objects
include("AthenaPoolCnvSvc/AthenaPool_jobOptions.py")
theApp.Dlls+=["EventSelectorAthenaPool"]
theApp.ExtSvc+=["PoolSvc","AthenaPoolCnvSvc"]
include ("RecAthenaPool/RecAthenaPool_joboptions.py")
include("DetDescrCondAthenaPool/DetDescrCondAthenaPool_joboptions.py" )
PoolSvc=Service("PoolSvc")
PoolSvc.WriteCatalog="file:mywrite.xml"
# uncomment below to allow objects to be appended to existing files
#PoolSvc.FileOpen="update"

# CondProxyProviderSvc to read in AlignableTransform objects
ProxyProviderSvc = Service( "ProxyProviderSvc" )
ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
CondProxyProvider = Service( "CondProxyProvider" )
# set this to the file containing AlignableTransform objects
CondProxyProvider.InputCollections=[ p_inputFile ]
