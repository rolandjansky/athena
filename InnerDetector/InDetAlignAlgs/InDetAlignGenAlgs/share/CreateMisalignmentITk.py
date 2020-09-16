#####################################################################
# CreateMisalignment.py
#####################################################################
# JobOptions for CreateMisalignAlg which creates a misaligned
# copy of the geometry database to introduce misalignments at reconstruction
# level
# This is configured to run with ITk geometries
#
#####################################################################
## parameters
#####################################################################

MisalignmentOnTopOfExistingSet = False
ReadDBPoolFile = False
#ReadDBPoolFile = True
WriteDBPoolFile = True
ROOToutput = True

createFreshDB = not(ReadDBPoolFile or MisalignmentOnTopOfExistingSet)

if not 'MisalignmentMode' in dir():
    MisalignmentMode = 11 # Radial

MaximumShift = 100*micrometer
if MisalignmentMode in [11, 12, 31]:
    MaximumShift = 500*micrometer

InFile = 'NominalAlignment'
OutFiles = 'MisalignmentSet%s' % (MisalignmentMode)

#####################################################################
theApp.EvtMax = 1
#####################################################################

MisalignModeMap = {0:'no Misalignment',
                    1: 'misalignment by 6 parameters',
                    2: 'random misalignment',
                    11: 'R deltaR (radial expansion)', 12: 'Phi deltaR (ellipse)',13: 'Z deltaR (funnel)',
                    21: 'R deltaPhi (curl)', 22: 'Phi deltaPhi (clamshell) ',23:'Z deltaPhi (twist)',
                    31: 'R deltaZ (telescope)',32:'Phi deltaZ (skew)',33:'Z deltaZ (z-expansion)'}

# construct Alignmentparameter Input filename
AlignmentInFilename = InFile + '.pool.root'

# construct Alignment- and Monitoring NTuple filenames and Alignmentparameter Output filename
NtupleOutFilename    = OutFiles + '.ntuple.root'
AlignmentOutFilename = OutFiles + '.pool.root'
DatabaseFilename     = OutFiles + '.db'
ASCIIFilename        = OutFiles

# Setup geometry
from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource='geant4'
globalflags.DetDescrVersion = 'ATLAS-P2-ITK-22-02-00'

include("InDetSLHC_Example/preInclude.SLHC.py")
include("InDetSLHC_Example/preInclude.SiliconOnly.py")
include("InDetSLHC_Example/preInclude.SLHC_Setup.py")
include("InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py")

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AtlasGeoModel import SetGeometryVersion 
from AtlasGeoModel import GeoModelInit 

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
ServiceMgr += GeoModelSvc
GeoModelSvc.AtlasVersion = globalflags.DetDescrVersion()

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('OFLCOND-MC15c-SDR-14-03')

import AthenaCommon.AtlasUnixGeneratorJob
ServiceMgr.EventSelector.RunNumber         = 222222
ServiceMgr.MessageSvc.defaultLimit = 9999999  # all messages

from InDetAlignGenTools.InDetAlignGenToolsConf import InDetAlignDBTool
InDetDBTool = InDetAlignDBTool()
ToolSvc += InDetDBTool
print InDetDBTool

print "CreateMisalignAlg: Creation of misalignment mode %s: %s" % (MisalignmentMode,MisalignModeMap.get(MisalignmentMode,'unknown'))

from InDetAlignGenAlgs.InDetAlignGenAlgsConf import InDetAlignment__CreateMisalignAlg
myMisalignAlg = InDetAlignment__CreateMisalignAlg( name = "MyMisalignmentAlg",
                                                    IDAlignDBTool = InDetDBTool,
                                                    CreateFreshDB = createFreshDB,
                                                    ASCIIFilenameBase = ASCIIFilename,
                                                    SQLiteTag = 'MisalignmentMode_'+str(MisalignmentMode),
                                                    MisalignMode = MisalignmentMode,
                                                    MisalignmentX = 0.0,
                                                    MisalignmentY = 0.0,
                                                    MisalignmentZ = 0.,
                                                    MisalignmentAlpha = 0.,
                                                    MisalignmentBeta = 0.,
                                                    MisalignmentGamma = 0.,
                                                    IBLBowingTshift = 1,
                                                    ScalePixelIBL = 0.1,
                                                    ScalePixelBarrel=0.1,
                                                    ScalePixelEndcap=0.1,
                                                    ScaleSCTBarrel=0.1,
                                                    ScaleSCTEndcap=0.,
                                                    ScaleTRTBarrel=0.,
                                                    ScaleTRTEndcap=0.,
                                                    MaxShift = MaximumShift, 
                                                    doPixel = True,
                                                    doSCT = False,
                                                    doTRT = False,
                                                    OutputLevel = VERBOSE)

topSequence += myMisalignAlg
print          myMisalignAlg

# Needed for database-, Pool- and AlignableTransforms-Handling
include ( "DetDescrCondAthenaPool/DetDescrCondAthenaPool_joboptions.py" )
PoolSvc=Service("PoolSvc")
    
if ROOToutput:
#rel 14 stuff
    theApp.HistogramPersistency = "ROOT"

    NTupleSvc        = Service( "NTupleSvc" )
    NTupleSvc.Output += [ "CREATEMISALIGN DATAFILE='"+NtupleOutFilename+"' TYP='ROOT' OPT='NEW'" ]
    NTupleSvc.Output += [ "IDENTIFIERTREE DATAFILE='ID_Tree.root' TYP='ROOT' OPT='RECREATE'"]
    
if ReadDBPoolFile:
    from IOVDbSvc.CondDB import conddb
    # block folders that you want to override
    conddb.blockFolder("/Indet/Align")
    from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import CondProxyProvider
    from AthenaCommon.AppMgr import ServiceMgr
    ServiceMgr += CondProxyProvider()
    ServiceMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
    ServiceMgr.CondProxyProvider.InputCollections = [ AlignmentInFilename ]
    
    ServiceMgr.CondProxyProvider.OutputLevel=INFO
    print ServiceMgr.CondProxyProvider
    ServiceMgr.IOVSvc.preLoadData = True
else:
    pass
    
if WriteDBPoolFile:
    from OutputStreamAthenaPool.OutputStreamAthenaPoolConf import AthenaPoolOutputStreamTool
    InDetCondStream=AthenaPoolOutputStreamTool(name="CondStream1", OutputFile = AlignmentOutFilename)
    ToolSvc += InDetCondStream
    print InDetCondStream
    
    include( "RegistrationServices/IOVRegistrationSvc_jobOptions.py" )
    regSvc = Service( "IOVRegistrationSvc" )
    regSvc.RecreateFolders = False

    IOVDbSvc = Service( "IOVDbSvc" )
    IOVDbSvc.dbConnection = "sqlite://;schema=%s;dbname=COMP200" % (DatabaseFilename)



# Disable StatusCodeSvc 
from AthenaCommon.AppMgr import ServiceMgr as svcMgr 
svcMgr.StatusCodeSvc.SuppressCheck = True
svcMgr.StatusCodeSvc.AbortOnError = False

include("InDetSLHC_Example/postInclude.SLHC_Setup_ITK.py")
