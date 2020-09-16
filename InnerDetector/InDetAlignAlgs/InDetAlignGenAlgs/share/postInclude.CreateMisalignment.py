#####################################################################
# CreateMisalignment.py
#####################################################################
# JobOptions for CreateMisalignAlg which creates a misaligned
# copy of the geometry database to introduce misalignments at reconstruction
# level
# To be used as postInclude to reconstruction jobs, or any job where the geometry is loaded
#
#####################################################################
## parameters
#####################################################################

MisalignmentOnTopOfExistingSet = False
ReadDBPoolFile = False
WriteDBPoolFile = True
ROOToutput = True

createFreshDB = not(ReadDBPoolFile or MisalignmentOnTopOfExistingSet)

if not 'MisalignmentMode' in dir():
    MisalignmentMode = 2 # Random

MaximumShift = 100*micrometer
if MisalignmentMode in [11, 12,31]:
    MaximumShift = 500*micrometer

InFile = 'NominalAlignment'
userSuffix = "_p01"
OutFiles = 'MisalignmentSet%s%s' % (MisalignmentMode, userSuffix)

#####################################################################
theApp.EvtMax = 2
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


#import AthenaCommon.AtlasUnixGeneratorJob
from AthenaCommon.AppMgr import ServiceMgr as svcMgr


from InDetAlignGenTools.InDetAlignGenToolsConf import InDetAlignDBTool
InDetDBTool = InDetAlignDBTool()
InDetDBTool.DBRoot = "/Indet/AlignITk"
ToolSvc += InDetDBTool
print InDetDBTool


## get a handle on the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


print "CreateMisalignAlg: Creation of misalignment mode %s: %s" % (MisalignmentMode,MisalignModeMap.get(MisalignmentMode,'unknown'))

from InDetAlignGenAlgs.InDetAlignGenAlgsConf import InDetAlignment__CreateMisalignAlg
myMisalignAlg = InDetAlignment__CreateMisalignAlg( name = "MyMisalignmentAlg",
                                                   IDAlignDBTool = InDetDBTool,
                                                   CreateFreshDB = True,
                                                   ASCIIFilenameBase = ASCIIFilename,
                                                   SQLiteTag = 'MisalignmentMode_',
                                                   MisalignMode = MisalignmentMode,
                                                   MisalignmentX = 0.1,
                                                   MisalignmentY = 0.1,
                                                   MisalignmentZ = 0.1,
                                                   MisalignmentAlpha = 0.,
                                                   MisalignmentBeta = 0.,
                                                   MisalignmentGamma = 0.,
                                                   IBLBowingTshift = 1,
                                                   ScalePixelIBL = 0.1,
                                                   ScalePixelBarrel=0.1,
                                                   ScalePixelEndcap=0.1,
                                                   ScaleSCTBarrel=0.1,
                                                   ScaleSCTEndcap=0.1,
                                                   ScaleTRTBarrel=0.,
                                                   ScaleTRTEndcap=0.,
                                                   MaxShift = MaximumShift,
                                                   OutputLevel = DEBUG,
                                                   doTRT=False,
                                                   doPixel=True
                                                 )

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
    #conddb.blockFolder("/TRT/Align")
    from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import CondProxyProvider
    from AthenaCommon.AppMgr import ServiceMgr
    ServiceMgr += CondProxyProvider()
    ServiceMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
    ServiceMgr.CondProxyProvider.InputCollections = [ AlignmentInFilename ]
    
    ServiceMgr.CondProxyProvider.OutputLevel=INFO
    print ServiceMgr.CondProxyProvider
    ServiceMgr.IOVSvc.preLoadData = True
    
if WriteDBPoolFile:
    print "Writing DB Pool File"
    from OutputStreamAthenaPool.OutputStreamAthenaPoolConf import AthenaPoolOutputStreamTool
    InDetCondStream=AthenaPoolOutputStreamTool(name="CondStream1", OutputFile = AlignmentOutFilename)
    InDetCondStream.OutputLevel = VERBOSE
    ToolSvc += InDetCondStream
    print InDetCondStream
    
    include( "RegistrationServices/IOVRegistrationSvc_jobOptions.py" )
    regSvc = Service( "IOVRegistrationSvc" )
    regSvc.RecreateFolders = False

    IOVDbSvc = Service( "IOVDbSvc" )
    IOVDbSvc.OutputLevel = VERBOSE
    IOVDbSvc.dbConnection = "sqlite://;schema=%s;dbname=OFLP200" % (DatabaseFilename)



# Disable StatusCodeSvc 
from AthenaCommon.AppMgr import ServiceMgr as svcMgr 
svcMgr.StatusCodeSvc.SuppressCheck = True
svcMgr.StatusCodeSvc.AbortOnError = False
