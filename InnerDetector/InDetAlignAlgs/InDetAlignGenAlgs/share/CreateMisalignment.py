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

MisalignmentOnTopOfExistingSet = False
ReadDBPoolFile = False
#ReadDBPoolFile = True
WriteDBPoolFile = True
ROOToutput = True

createFreshDB = not(ReadDBPoolFile or MisalignmentOnTopOfExistingSet)

MisalignmentMode = 11

if not 'MisalignmentMode' in dir():
    MisalignmentMode = 3

shiftInMicrons = 200 # this shift is given at a the 3rd SCT layer (r=0.5 m = 500 mm)
if MisalignmentMode in [11, 12,31]:
    shiftInMicrons = 250
    
MaximumShift = shiftInMicrons*micrometer

InFile = 'alignment_nominal'
signKey = 'p'
if(shiftInMicrons > 0): signKey = 'n' 
OutFiles = 'MisalignmentSet%s_%s%d' % (MisalignmentMode, signKey, shiftInMicrons) 

#####################################################################

#from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#athenaCommonFlags.EvtMax     =  1
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

# 

# construct Alignment- and Monitoring NTuple filenames and Alignmentparameter Output filename
NtupleOutFilename    = OutFiles + '.ntuple.root'
AlignmentOutFilename = OutFiles + '.pool.root'
DatabaseFilename     = OutFiles + '.db'
ASCIIFilename        = OutFiles



from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource='geant4'
from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag("OFLCOND-RUN12-SDR-14")
#conddb.addOverride('/Indet/Align','InDetSi_CSCMisaligned_RDeltaPhi_03')
#conddb.addOverride('/TRT/Align','InDetTRT_CSCMisaligned_RDeltaPhi_03')

# Setup geometry
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()

# The geometry version does not really matter. Any will do.
globalflags.DetDescrVersion = 'ATLAS-R2-2015-03-00-00'


import AthenaCommon.AtlasUnixGeneratorJob
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr.EventSelector.RunNumber         = 222222
svcMgr.MessageSvc.defaultLimit = 9999999  # all messages

from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetGeometryVersion

from InDetAlignGenTools.InDetAlignGenToolsConf import InDetAlignDBTool
InDetDBTool = InDetAlignDBTool()
ToolSvc += InDetDBTool
print (InDetDBTool)

# from TRT_ConditionsAlgs.TRT_ConditionsAlgsConf import TRTAlignDbTool
#from TRT_ConditionsTools.TRT_ConditionsToolsConf import TRTAlignDbTool
#TRTDBTool = TRTAlignDbTool()
#ToolSvc += TRTDBTool
#print TRTDBTool
#FIXME this has moved to the TRTAlignDBService

from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_AlignDbSvc
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr += TRT_AlignDbSvc(
    name = "TRT_AlignDbSvc",
    #StreamTool = TRTCondStream,
    alignTextFile = "")
    #alignString = "ALold",


## get a handle on the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


print ("\n CreateMisalignAlg: Creation of misalignment mode %s: %s \n" % (MisalignmentMode,MisalignModeMap.get(MisalignmentMode,'unknown')))

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
                                                    IBLBowingTshift = 1,
                                                    ScalePixelIBL = 0.1,
                                                    ScalePixelBarrel=0.1,
                                                    ScalePixelEndcap=0.1,
                                                    ScaleSCTBarrel=0.1,
                                                    ScaleSCTEndcap=0.,
                                                    ScaleTRTBarrel=0.,
                                                    ScaleTRTEndcap=0.,
                                                    MaxShift = MaximumShift,
                                                    OutputLevel = DEBUG
                                                 )

topSequence += myMisalignAlg
print (myMisalignAlg)

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
    print (ServiceMgr.CondProxyProvider)
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
    from AthenaServices.AthenaServicesConf import AthenaOutputStreamTool
    InDetCondStream=AthenaOutputStreamTool(name="CondStream1", OutputFile = AlignmentOutFilename)
    ToolSvc += InDetCondStream
    print (InDetCondStream)
    
    include( "RegistrationServices/IOVRegistrationSvc_jobOptions.py" )
    regSvc = Service( "IOVRegistrationSvc" )
    regSvc.RecreateFolders = False

    IOVDbSvc = Service( "IOVDbSvc" )
    #IOVDbSvc.dbConnection = "impl=cool;techno=sqlite;schema=%s;X:COMP200" % DatabaseFilename
    IOVDbSvc.dbConnection = "sqlite://;schema=%s;dbname=COMP200" % (DatabaseFilename)
