# Author: Ketevi A. Assamagan
# BNL, June 12, 2004

# get a handle on the service manager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

# get a handle on the sequence of top-level algorithms
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# Event selector
import AthenaPoolCnvSvc.ReadAthenaPool
#svcMgr.EventSelector.BackNavigation = True

# the POOL converters: loaded on demand from now on (release 13)
## include( "ParticleBuilderOptions/ESD_PoolCnv_jobOptions.py" )
## include( "ParticleBuilderOptions/AOD_PoolCnv_jobOptions.py")
## include( "ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py")
## include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

# Read in the AOD from POOL - AOD.pool.root
# this file is produced by running the AOD builders of the RDO raw data

#-------- to convert "GEN_EVENT" to TruthParticles on the fly - Ketevi A. Assamagan
#from ParticleBuilderOptions.AODFlags import AODFlags
#AODFlags.McEventKey = "GEN_EVENT"
#from McParticleAlgs.JobOptCfg import createTruthParticlesBuilder
#topSequence += createTruthParticlesBuilder()
#from JetRec.JetGetters import *
#from JetRec.JetRecFlags import jetFlags
#jetFlags.inputFileType = 'GEN'
#make_StandardJetGetter('Cone',0.4,'Truth').jetAlgorithmHandle()

#------------

include ( "AnalysisExamples/ReadTruthParticles_jobOptions.py" )
ReadTruthParticles = topSequence.ReadTruthParticles
ReadTruthParticles.ConvertFromESD = True

if ReadTruthParticles.ConvertFromESD:
    #ESD case
    svcMgr.EventSelector.InputCollections = [ "ESD.pool.root" ]
    ReadTruthParticles.CnvTool.McEvents = "TruthEvent"
    ReadTruthParticles.CnvTool.TruthParticlesOutput = "SpclMC_ESD"
    ReadTruthParticles.TruthParticles = "SpclMC_ESD"
else:
    # The AOD case
    svcMgr.EventSelector.InputCollections = [ "AOD.pool.root" ]

# Number of Events to process
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 500



