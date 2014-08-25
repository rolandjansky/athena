PoolAODInput=["/scratch/ayurkewi/Zmumu_files/mc08.106051.PythiaZmumu_1Lepton.recon.AOD.e347_s462_r635_tid047165_sub06186680/AOD.047165._20527.pool.root.1"]

if not 'RootNtupleOutput' in dir():
    RootNtupleOutput="ntuple_fromAOD.root"
EvtMax=10
doCBNT=False
readAOD=True
doWriteAOD=True
doWriteESD=False
doAOD=False
doWriteTAG=False
doMissingET=True
# main jobOption
include ("RecExCommon/RecExCommon_flags.py")
include ("RecExCommon/RecExCommon_topOptions.py")
GeoModelSvc = Service( "GeoModelSvc" )
GeoModelSvc.IgnoreTagDifference = True

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion = "ATLAS-GEO-02-01-00"
include( "RecExCond/AllDet_detDescr.py")

############################################################################################
#configure MET muon term re-reconstruction since it is off by default
############################################################################################
from MissingET.MissingETConf import METAlg
theMETAlg=METAlg("METAlg")

from MissingET.MissingETConf import METMuonTool 	 
theMETMuonBoyTrackTool = METMuonTool("MuonBoyTrack",			 
                                     MuonContainer	       ="StacoMuonCollection",   
                                     outKey  	       ="MET_MuonBoy_Track",		       
                                     ApplyChi2OverDoFCut    ="Off",  		       
                                     cut1Chi2OverDoF        = 0. ,			       
                                     cut2Chi2OverDoF        = 10.,
                                     CaloMuonCollection     = "CaloMuonCollection",
                                     MuTagCollection        = "StacoMuonCollection",
                                     JetContainerName       = "AntiKt6LCTopoJets",
                                     Muon_Selected_Type     = "Isol",    # Isol = use muon tracks
                                     Muon_Add_Crack         = "On"	    # add muon tracks from cracks
                                     )		    
theMETMuonBoySpectroTool = METMuonTool("MuonBoySpectro",		 
                                       MuonContainer	       ="StacoMuonCollection",   
                                       outKey  	       ="MET_MuonBoy_Spectro",  	       
                                       ApplyChi2OverDoFCut    ="Off",  		       
                                       cut1Chi2OverDoF        = 0. ,			       
                                       cut2Chi2OverDoF        = 10.,	    
                                       CaloMuonCollection     = "CaloMuonCollection",
                                       MuTagCollection        = "StacoMuonCollection",
                                       JetContainerName       = "AntiKt6LCTopoJets",
                                       Muon_Selected_Type     = "NonIsol", # NonIsol = use spectro muons
                                       Muon_Add_Crack         = "Off" 
                                       )
theMETMuonTool    = METMuonTool("Muon",      
                                MuonContainer	       ="StacoMuonCollection",
                                outKey  	       ="MET_Muon",			   
                                ApplyChi2OverDoFCut    ="Off",  		   
                                cut1Chi2OverDoF        = 0.,			   
                                cut2Chi2OverDoF        = 10.,
                                CaloMuonCollection     = "CaloMuonCollection",
                                MuTagCollection        = "StacoMuonCollection",
                                JetContainerName       = "AntiKt6LCTopoJets",
                                Muon_Selected_Type     = "All",     # All = use spectro muons
                                Muon_Add_Crack         = "Off"	    # add muon tracks from cracks
                                )
# add the METMuonTool to list of tools 
theMETAlg.AlgTools+= [ theMETMuonBoyTrackTool.getFullName() ]
theMETAlg.AlgTools+= [ theMETMuonBoySpectroTool.getFullName() ] 
theMETAlg.AlgTools+= [ theMETMuonTool.getFullName() ]

# add tool to alg							  
theMETAlg += theMETMuonBoyTrackTool
theMETAlg += theMETMuonBoySpectroTool
theMETAlg += theMETMuonTool

from MissingET.MissingETConf import METFinalTool
theMETMuonFinal      = METFinalTool("MuFinal",
                                    WhichCalc     = "MuonCalc",           # calculate final muon term
                                    outKey        = "MET_MuonBoy",        # output key
                                    muonBOYTrack  = "MET_MuonBoy_Track",  # key for track muons
                                    muonBOYSpectro= "MET_MuonBoy_Spectro" # key for spectro muons 
                                    )
theMETAlg.AlgTools+= [ theMETMuonFinal.getFullName() ]
# add tools to alg
theMETAlg += theMETMuonFinal
############################################################################################

#suffix to be added to all the storegate keys for the reprocessing
#have to set it to be the same in share/ContainerComparatorTool_jobOptions.py
newrelease = '_15_2_0'

#these are the keys that are used within the MET algs
topSequence.METAlg.MuFinal.muonBOYTrack    += newrelease
topSequence.METAlg.MuFinal.muonBOYSpectro  += newrelease
topSequence.METAlg.RefFinal.EleRefKey      += newrelease
topSequence.METAlg.RefFinal.GammaRefKey    += newrelease
#off because these aren't recalculated yet on AOD
#topSequence.METAlg.RefFinal.MuoRefKey     += newrelease
#topSequence.METAlg.RefFinal.TauRefKey     += newrelease
topSequence.METAlg.RefFinal.JetRefKey      += newrelease
topSequence.METAlg.RefFinal.CellOutKey     += newrelease
topSequence.METAlg.RefFinal.muonKey        += newrelease
topSequence.METAlg.RefFinal.cryoKey        += newrelease

#these are the keys that are going to be written out
topSequence.METAlg.MuFinal.outKey                           += newrelease
topSequence.METAlg.Muon.outKey                              += newrelease
topSequence.METAlg.MuonBoySpectro.outKey                    += newrelease
topSequence.METAlg.MuonBoyTrack.outKey                      += newrelease
topSequence.METAlg.RefFinal.outKey                          += newrelease
topSequence.METRefAlg.METRefEle.MissingETOutKey             += newrelease
topSequence.METRefAlg.METRefGamma.MissingETOutKey           += newrelease
topSequence.METRefAlg.METRefJet.MissingETOutKey             += newrelease
#topSequence.METRefAlg.METRefTau.MissingETOutKey            += newrelease
topSequence.METRefAlg.METRefCluster.MissingETOutKey         += newrelease

print 'printing again topSequence.METRefAlg'
print topSequence.METRefAlg

#here is where we specify the keys that are going to be written out by adding them to the list
#needs doWriteAOD=True
StreamAOD.ItemList += ["MissingET#" + topSequence.METAlg.MuFinal.outKey]
StreamAOD.ItemList += ["MissingET#" + topSequence.METAlg.Muon.outKey]
StreamAOD.ItemList += ["MissingET#" + topSequence.METAlg.MuonBoySpectro.outKey]
StreamAOD.ItemList += ["MissingET#" + topSequence.METAlg.MuonBoyTrack.outKey]
StreamAOD.ItemList += ["MissingET#" + topSequence.METAlg.RefFinal.outKey]
StreamAOD.ItemList += ["MissingET#" + topSequence.METRefAlg.METRefEle.MissingETOutKey]
StreamAOD.ItemList += ["MissingET#" + topSequence.METRefAlg.METRefGamma.MissingETOutKey]
#StreamAOD.ItemList += ["MissingET#" + topSequence.METRefAlg.METRefTau.MissingETOutKey]
StreamAOD.ItemList += ["MissingET#" + topSequence.METRefAlg.METRefJet.MissingETOutKey]
StreamAOD.ItemList += ["MissingET#" + topSequence.METRefAlg.METRefCluster.MissingETOutKey]

from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AppMgr import ServiceMgr
import AthenaPoolCnvSvc.ReadAthenaPool
from PartPropSvc.PartPropSvcConf import PartPropSvc

# the POOL converters
include( "ParticleBuilderOptions/ESD_PoolCnv_jobOptions.py" )
include( "ParticleBuilderOptions/AOD_PoolCnv_jobOptions.py")
include( "ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py")
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

include ( "MissingETPerformance/MissingETData_jobOptions.py" )
include ( "MissingETPerformance/ContainerComparatorTool_jobOptions.py" )
MissingETData.IgnoreMissingContainers = True

# Athena-Aware NTuple making Tools
CBNTAthenaAware = True
include ("CBNT_Athena/CBNT_AthenaAware_jobOptions.py")
include ("CBNT_Athena/CBNT_EventInfo_jobOptions.py")

from MissingETPerformance.MissingETPerformanceConf import MissingETPerformance
topSequence.CBNT_AthenaAware += MissingETPerformance() 
MissingETPerformance = MissingETPerformance()

#only want ContainerComparatorTool on
MissingETPerformance.doContainerComparatorTool          = True
MissingETPerformance.FilterOnTrigger                    = False
MissingETPerformance.doBasicPlotsTool                   = False
MissingETPerformance.doResolutionTool                   = False
MissingETPerformance.doLinearityTool                    = False
MissingETPerformance.doEtaRingsTool                     = False
MissingETPerformance.doTrigMissingETTool                = False
MissingETPerformance.doTrigVsOfflineMissingETTool       = False
MissingETPerformance.doFakeMissingETTool                = False
MissingETPerformance.doMuonTool                         = False

##########################################
# setup TTree registration Service
# save ROOT histograms and Tuple
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = [ "AANT DATAFILE='METPerformance.compareRELS.root' OPT='RECREATE'" ]
from AnalysisTools.AthAnalysisToolsConf import AANTupleStream
topSequence += AANTupleStream()
AANTupleStream = AANTupleStream()
AANTupleStream.OutputName = 'METPerformance.compareRELS.root'
AANTupleStream.WriteInputDataHeader = True

AthenaEventLoopMgr = Service ( "AthenaEventLoopMgr")
try:
    AthenaEventLoopMgr.EventPrintoutInterval = 1
except Exception:
    print 'EventPrintoutInterval exception, defaults to 1'
