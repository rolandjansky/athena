# -*- coding: utf-8 -*-
#####################################################################
# CreateMisalignment.py
#####################################################################
# JobOptions for CreateMisalignAlg which creates a misaligned
# copy of the geometry database to introduce misalignments at reconstruction
# level
#
# NB: Mode1:Misalignment by 6 parameters currently moves whole InnerDetector
#
# Author: Roland Haertel    rhaertel@mppmu.mpg.de
#         Tobias Goettfert  tobias.goettfert@cern.ch
#
# Date: 18-03-2008
#
#####################################################################
## parameters
#####################################################################

MisalignmentOnTopOfExistingSet = True
ReadDBPoolFile = False
#ReadDBPoolFile = True
WriteDBPoolFile = True
ROOToutput = True

createFreshDB = not(ReadDBPoolFile or MisalignmentOnTopOfExistingSet)

if not 'MisalignmentMode' in dir():
    MisalignmentMode = 11

MaximumShift = 200*micrometer
if MisalignmentMode in [11, 12,31]:
    MaximumShift = 500*micrometer

InFile = 'NominalAlignment'
#InFile = 'MisalignmentSet11'
OutFiles = 'MisalignmentSet%s' % MisalignmentMode

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

from InDetAlignGenTools.InDetAlignGenToolsConf import InDetAlignDBTool
InDetDBTool = InDetAlignDBTool()
ToolSvc += InDetDBTool
print InDetDBTool

# from TRT_ConditionsAlgs.TRT_ConditionsAlgsConf import TRTAlignDbTool
#from TRT_ConditionsTools.TRT_ConditionsToolsConf import TRTAlignDbTool
#TRTDBTool = TRTAlignDbTool()
#ToolSvc += TRTDBTool
#print TRTDBTool
#FIXME this has moved to the TRTAlignDBService

print "CreateMisalignAlg: Creation of misalignment mode %s: %s" % (MisalignmentMode,MisalignModeMap.get(MisalignmentMode,'unknown'))

from InDetAlignGenAlgs.InDetAlignGenAlgsConf import InDetAlignment__CreateMisalignAlg
myMisalignAlg = InDetAlignment__CreateMisalignAlg( name = "MyMisalignmentAlg",
                                                    IDAlignDBTool = InDetDBTool,
                                                    #TRTAlignDBService = ,
                                                    CreateFreshDB = createFreshDB,
                                                    ASCIIFilenameBase = ASCIIFilename,
                                                    SQLiteTag = 'MisalignmentMode_',
                                                    MisalignMode = MisalignmentMode,
                                                    MisalignmentX = 0.0,
                                                    MisalignmentY = 0.0,
                                                    MisalignmentZ = 0.,
                                                    MisalignmentAlpha = 0.,
                                                    MisalignmentBeta = 0.,
                                                    MisalignmentGamma = 0.,
                                                    ScalePixelBarrel=1.,
                                                    ScalePixelEndcap=1.,
                                                    ScaleSCTBarrel=1.,
                                                    ScaleSCTEndcap=1.,
                                                    ScaleTRTBarrel=5.,
                                                    ScaleTRTEndcap=5.,
                                                    MaxShift = MaximumShift
                                                    #, OutputLevel = DEBUG
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
    conddb.blockFolder("/TRT/Align")
    from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import CondProxyProvider
    from AthenaCommon.AppMgr import ServiceMgr
    ServiceMgr += CondProxyProvider()
    ServiceMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
    ServiceMgr.CondProxyProvider.InputCollections = [ AlignmentInFilename ]
    
    ServiceMgr.CondProxyProvider.OutputLevel=INFO
    print ServiceMgr.CondProxyProvider
    ServiceMgr.IOVSvc.preLoadData = True
else:
    ##day1
    #siOverride = 'InDetSi_CSC_Day1-01'
    #trtOverride = 'InDetTRT_CSC_Day1-01'

    #from IOVDbSvc.CondDB import conddb
    #conddb.addOverride('/Indet/Align', siOverride)
    #conddb.addOverride('/TRT/Align', trtOverride)
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
    #IOVDbSvc.dbConnection = "impl=cool;techno=sqlite;schema=%s;X:COMP200" % DatabaseFilename
    IOVDbSvc.dbConnection = "sqlite://;schema=%s;dbname=COMP200" % (DatabaseFilename)
