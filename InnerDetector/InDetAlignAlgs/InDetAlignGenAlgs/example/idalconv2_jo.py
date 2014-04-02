# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# minimal test of CTB data migrated to COOL
# do not use this file
#use McEventSelector
theApp.setup( MONTECARLO )
include( "AthenaPoolCnvSvc/AthenaPool_jobOptions.py" )
include ( "IOVDbSvc/IOVDbSvc_jobOptions.py" )
IOVDbSvc=Service("IOVDbSvc")
#IOVDbSvc.dbConnection="impl=conddb;techno=mysql;atlobk02.cern.ch:conditions_ctb_2004:conditions:conditions"
#IOVDbSvc.dbConnection="impl=cool;techno=sqlite;schema=mysqlfile.db;X:TBPROD"

PoolSvc = Service( "PoolSvc" )
#PoolSvc.ReadCatalog+= [ "file:PoolFileCatalog.xml","file:poolcond/poolcond.xml"]
Service("PoolSvc").ReadCatalog= ["file:PoolFileCatalog.xml",
 "xmlcatalog_http://atlas.web.cern.ch/Atlas/GROUPS/DATABASE/project/catrep/condcat.xml"]

#ctbcon="<dbConnection>impl=conddb;techno=mysql;atlobk02.cern.ch:conditions_ctb_2004:conditions:conditions</dbConnection>"
#ctbcon="<dbConnection>impl=cool;techno=oracle;schema=ATLAS_COOL_INDET;ATLAS_COOLPROD:TBMCPROD:ATLAS_COOL_READER:COOLRED4PRO</dbConnection>"
ctbcon="<dbConnection>impl=cool;techno=sqlite;schema=sqlite/TBMCPROD.db;X:TBMCPROD</dbConnection>"
IOVDbSvc.Folders+=[ ctbcon+" /Indet/Align"]

include ( "RecAthenaPool/RecAthenaPool_joboptions.py" )
include ( "DetDescrCondAthenaPool/DetDescrCondAthenaPool_joboptions.py" )

from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DataSource.set_data()
GlobalFlags.InputFormat.set_bytestream() 
GlobalFlags.DetGeo.set_ctbh8()

include( "InDetTBRecExample/InDetTBFlags_jobOptions.py" )
include("InDetTBRecExample/InDetTBSetFlags_jobOptions.py")
InDetTBFlags.TRT=False
include ("AtlasGeoModel/SetGeometryVersion.py")
include ("AtlasGeoModel/GeoModelInit.py")
#include ("InDetTBRecExample/InDetTB2004DetDescr_jobOptions.py")
GeoModelSvc.PixelTBDetectorTool.Alignable=TRUE
GeoModelSvc.SCT_TBDetectorTool.Alignable=TRUE
NovaCnvSvc.Host = "atlasdbdev.cern.ch"

theApp.TopAlg += [ "InDetAlignNt" ]
theApp.Dlls   += [ "InDetAlignGenAlgs","InDetAlignGenTools" ]
# set to 1 for TRT hits
InDetAlignNt = Service( "InDetAlignNt" )
InDetAlignNt.TRT=1
# set to true to read loaded alignment info from TDS
InDetAlignNt.ReadAl=True
InDetAlignNt.Truth=False
# enable this to get histograms of overlap residual distributions
InDetAlignNt.overlap=False
InDetAlignNt.OutputLevel=3
InDetAlignNt.NumDeriv=0

# uses new Trk:Tracks (perhaps create with TrkLegacyCnvAlg)
InDetAlignNt.newtrk=False
#InDetAlignNt.newtrkcol="ConvertedXKalmanTracks"
InDetAlignNt.newtrkcol="ConvertedIPatTracks"
# this to make truth for xkalman tracks available
InDetTrackTruthMaker = Algorithm( "InDetTrackTruthMaker" )
#InDetTrackTruthMaker.TrackName = "ConvertedXKalmanTracks"
InDetTrackTruthMaker.TrackName = "ConvertedIPatTracks"


# this for running with hbook ntuples
theApp.Dlls += [ "HbookCnv" ]
NtupleSvc = Service( "NtupleSvc" )
NtupleSvc.Output = [ "FILE1 DATAFILE='idalign.hb' OPT='NEW'" ]
theApp.HistogramPersistency="HBOOK"
HbookHistSvc = Service ( "HbookHistSvc")
HbookHistSvc.NPAWC = 1500000 
HistogramPersistencySvc = Service( "HistogramPersistencySvc" )
HistogramPersistencySvc.OutputFile  = "ipathis.hbook"

InDetAlignWrt = Service( "InDetAlignWrt" )
theApp.TopAlg += [ "InDetAlignWrt" ]
InDetAlignWrt.WFile="migrate.check2"
InDetAlignWrt.RWNtuple=FALSE
InDetAlignWrt.Write=TRUE


EventSelector = Service( "EventSelector" )
EventSelector.RunNumber         = 2102355
EventSelector.EventsPerRun      = 1
EventSelector.FirstEvent        = 0
EventSelector.InitialTimeStamp  = 0
EventSelector.TimeStampInterval = 5
theApp.EvtMax                   = 1

#IOVDbSvc.GlobalTag="SiTBAlign03"
IOVDbSvc.GlobalTag="nominal"
IOVDbSvc.OutputLevel=1

Service("IOVSvc").OutputLevel=1

