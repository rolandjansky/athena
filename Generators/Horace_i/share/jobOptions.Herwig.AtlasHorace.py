###############################################################
#
# Job options file Horace_i/jobOptions.Herwig.AtlasHorace.py
#
#==============================================================
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixGeneratorJob

# Private Application Configuration option
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.MessageSvc.OutputLevel = INFO

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 100
OUTPUT="/tmp/mc13.005143.HoraceZmumu.evgen.EVNT.v13004000._00001.pool.root"
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
from AthenaServices.AthenaServicesConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc()
ServiceMgr.AtRndmGenSvc.Seeds = ["HERWIG 390020611 821000366", "HERWIG_INIT 820021 2347532"]

# Horace core jobOption
include ( "Horace_i/CSC.ExtProcess.HerwigHorace.py")

# nospac = true switch off initial-state radiation. Only forced splitting of non-valence partons is generated.
# noshower = true switch off the parton shower

#from TruthExamples.TruthExamplesConf import PrintMC
#job += PrintMC()
#job.PrintMC.McEventKey = "GEN_EVENT" #the storegate key for reading
#job.PrintMC.VerboseOutput = TRUE     #dumps the event to stout (TRUE/FALSE)
#job.PrintMC.PrintStyle = "Barcode"   #Event print style (Vertex/Barcode)
#job.PrintMC.FirstEvent = 1           #First event to print
#job.PrintMC.LastEvent = 10           #Last event to print or all after FirstEvent are printed

#---------------------------------------------------------------
# Pool Persistency
#---------------------------------------------------------------
#from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
#Stream1 = AthenaPoolOutputStream( "Stream1" )
# 2101 = EventInfo
# 133273 = MCTruth (HepMC)
#Stream1.ItemList += [ "2101#*", "133273#*" ]
#Stream1.OutputFile = "Wmunu-herwig-horace.pool.root"


#---------------------------------------------------------------
# On the fly conversion from GEN_EVENT to SpclMC
#---------------------------------------------------------------

##########################
# Configure McAod options

#from McParticleAlgs.McAodFlags import jobproperties as jp
#jp.McAodFlags.doTruthEtIsolations = False

########
# Create the AOD McEventCollection
#
#from McParticleAlgs.JobOptCfg import McAodBuilder
#from McParticleTools.McParticleToolsConf import NoopFilterTool
#from McParticleTools.McParticleToolsConf import TruthParticleCnvTool
#topAlg += McAodBuilder(
#    "McAodBuilder",
#    OutputLevel = INFO,
#    FilterTool = NoopFilterTool(
#                   McEvents = "GEN_EVENT",
#                   DoEtIsolations = jp.McAodFlags.doTruthEtIsolations()
#                   ),
#    CnvTool = TruthParticleCnvTool(
#                   McEvents = "GEN_AOD",
#                   TruthParticlesOutput = "SpclMC",
#                   DoEtIsolations = jp.McAodFlags.doTruthEtIsolations()
#                   )
#    )

#---------------------------------------------------------------
# Pool Persistency
#---------------------------------------------------------------
#include( "ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py")

#from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
#outStream = AthenaPoolOutputStream("OutStream")
#outStream.ItemList  = [ "EventInfo#McEventInfo" ]
#outStream.ItemList += [ "McEventCollection#GEN_EVENT" ]
#outStream.ItemList += [ "McEventCollection#GEN_AOD" ]
#outStream.ItemList += [ "TruthParticleContainer#SpclMC" ]
##outStream.ItemList += [ "TruthEtIsolationsContainer#TruthEtIsol_GEN_EVENT" ]
#outStream.OutputFile = OUTPUT

#----------------------------------------------------------------
# Ntuple service output
#----------------------------------------------------------------

#theApp.Dlls += [ 'RootHistCnv' ]                               
#theApp.HistogramPersistency = 'ROOT'
#HistogramPersistencySvc = Service( "HistogramPersistencySvc" )
#HistogramPersistencySvc.OutputFile  = "herwig_horace_zmumu_hist.root"
#THistSvc = Service( 'THistSvc' )
#THistSvc.Output = [ "AANT DATAFILE='herwig_horace_zmumu_cbnt.root' OPT='RECREATE'" ]

#theApp.Dlls   += [ "AnalysisTools" ]
#AANTupleStream = Algorithm( "AANTupleStream" )
#AANTupleStream.ExistDataHeader = False
#AANTupleStream.OutputName = "herwig_horace_zmumu_cbnt.root"
#theApp.TopAlg += [ "AANTupleStream" ]

#include( 'CBNT_Athena/CBNT_AthenaAware_jobOptions.py' )             
#include( 'CBNT_Athena/CBNT_EventInfo_jobOptions.py' )          
#include( 'RecExCommon/CBNT_Truth_jobOptions.py' )

#
#  Atlfast
#
#---------------------------------------------------------------
# Uncomment only one of the next two files at a time (ie.. make either CBNT or AOD).
#include( "DC3_joboptions/GentoAtlfasttoCBNT.py" )
#include( "DC3_joboptions/GentoAtlfasttoAOD.py" )

#CBNT_Atlfast.FillHistory = True
#CBNT_Atlfast.NtupleLocID="/FILE1/CBNT/t3333" 

#==============================================================
#
# End of job options file
#
###############################################################
