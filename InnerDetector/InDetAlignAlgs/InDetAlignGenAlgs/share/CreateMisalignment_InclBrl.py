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

if not 'MisalignmentMode' in dir():
    MisalignmentMode = 2

MaximumShift = 200*micrometer
if MisalignmentMode in [11, 12, 31]:
    MaximumShift = 500*micrometer

InFile = 'alignment_nominal'
#InFile = 'NominalAlignment'
#InFile = 'MisalignmentSet11'
OutFiles = 'MisalignmentSet%s' % MisalignmentMode

#####################################################################

#from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#athenaCommonFlags.EvtMax     =  1
theApp.EvtMax = 2
#####################################################################

MisalignModeMap = {  0: 'no Misalignment',
                     1: 'misalignment by 6 parameters',
                     2: 'random misalignment',
                    11: 'R deltaR (radial expansion)', 12: 'Phi deltaR (ellipse)'    , 13: 'Z deltaR (funnel)'      ,
                    21: 'R deltaPhi (curl)'          , 22: 'Phi deltaPhi (clamshell)', 23: 'Z deltaPhi (twist)'     ,
                    31: 'R deltaZ (telescope)'       , 32: 'Phi deltaZ (skew)'       , 33: 'Z deltaZ (z-expansion)' }

# construct Alignmentparameter Input filename
AlignmentInFilename = InFile + '.pool.root'

# construct Alignment- and Monitoring NTuple filenames and Alignmentparameter Output filename
NtupleOutFilename    = OutFiles + '.ntuple.root'
AlignmentOutFilename = OutFiles + '.pool.root'
DatabaseFilename     = OutFiles + '.db'
ASCIIFilename        = OutFiles


# Setup geometry
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.TRT_setOff()


from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource='geant4'

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion = 'ATLAS-P2-ITK-10-00-01'

include('InDetSLHC_Example/preInclude.SLHC_Setup_InclBrl_4.py')
include('InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py')

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('OFLCOND-SIM-00-00-00')
#conddb.addOverride('/Indet/Align','InDetSi_CSCMisaligned_RDeltaPhi_03')


import AthenaCommon.AtlasUnixGeneratorJob
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr.EventSelector.RunNumber         = 222222
svcMgr.MessageSvc.defaultLimit = 9999999  # all messages

from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetGeometryVersion

from IOVDbSvc.CondDB import conddb
# block folders that you want to override
conddb.blockFolder("/Indet/Align")

from InDetAlignGenTools.InDetAlignGenToolsConf import InDetAlignDBTool
InDetDBTool = InDetAlignDBTool(name              = "InDetDBTool",
			       DBRoot            = "/Indet/Align",
			       DBKey             = "/Indet/Align")
InDetDBTool.OutputLevel=VERBOSE
ToolSvc += InDetDBTool
print InDetDBTool

## get a handle on the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

print "CreateMisalignAlg: Creation of misalignment mode %s: %s" % (MisalignmentMode,MisalignModeMap.get(MisalignmentMode,'unknown'))

from InDetAlignGenAlgs.InDetAlignGenAlgsConf import InDetAlignment__CreateMisalignAlg_ITk
myMisalignAlg = InDetAlignment__CreateMisalignAlg_ITk( name              = "MyMisalignmentAlg",
                                                       IDAlignDBTool     = InDetDBTool,
                                                       CreateFreshDB     = True,
                                                       ASCIIFilenameBase = ASCIIFilename,
                                                       SQLiteTag         = 'MisalignmentMode_'+str(MisalignmentMode),
                                                       MisalignMode      = MisalignmentMode,
                                                       MisalignmentX     = 0.001,
                                                       MisalignmentY     = 0.001,
                                                       MisalignmentZ     = 0.001,
                                                       MisalignmentAlpha = 0.0,
                                                       MisalignmentBeta  = 0.0,
                                                       MisalignmentGamma = 0.0,
                                                       ScalePixelBarrel  = 1.,
                                                       ScalePixelEndcap  = 1.,
                                                       ScaleSCTBarrel    = 0.1,
                                                       ScaleSCTEndcap    = 0.0,
                                                       MaxShift          = MaximumShift,
                                                       OutputLevel       = VERBOSE )

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
    #regSvc.RecreateFolders = True

    IOVDbSvc = Service( "IOVDbSvc" )
    #IOVDbSvc.dbConnection = "impl=cool;techno=sqlite;schema=%s;X:COMP200" % DatabaseFilename
    IOVDbSvc.dbConnection = "sqlite://;schema=%s;dbname=OFLP200" % (DatabaseFilename)



# Disable StatusCodeSvc 
from AthenaCommon.AppMgr import ServiceMgr as svcMgr 
svcMgr.StatusCodeSvc.SuppressCheck = True
svcMgr.StatusCodeSvc.AbortOnError = False

include('InDetSLHC_Example/postInclude.SLHC_Setup_InclBrl_4.py')
