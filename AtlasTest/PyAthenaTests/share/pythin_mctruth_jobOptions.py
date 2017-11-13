## @file PyAthenaTests/share/pythin_mctruth_jobOptions.py
## @brief read an AOD POOL file and slim the truth

###################################
if not 'EVTMAX' in dir():
    EVTMAX=-1
if not 'INPUT' in dir():
    INPUT = [
        'root://eosatlas.cern.ch//eos/atlas/user/b/binet/reffiles/14.1.0.x'\
        '/AllBasicSamples.AOD.pool.root'
        ]
if not 'OUTPUT' in dir():
    OUTPUT='slimmed_truth.aod.pool'
###################################

## import the SI units
import AthenaCommon.SystemOfUnits as Units

## setup the Athena application to read-in POOL files
import AthenaPoolCnvSvc.ReadAthenaPool
## now the appropriate EventSelector has been added and
## configured to the list of services
##  --> tell this service which file to read-in
svcMgr.EventSelector.InputCollections = INPUT

## get a handle on the sequence of algorithms to be run
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

## slim the truth
from McParticleAlgs.JobOptCfg import createMcAodBuilder as McDpdBuilder
job += McDpdBuilder(
    name = 'McDpdBuilder',
    inMcEvtCollection='GEN_AOD',
    outMcEvtCollection='GEN_DPD',
    outTruthParticles ='SpclMC_DPD',
    doTruthEtIsolations=True,
    filterTool=CfgMgr.OldSpclMcFilterTool(IncludeSimul=False,
                                          IncludePartonShowers=False,
                                          ptGammaMin = 5.*Units.GeV)
    )


# add a few mc-dumpers
from PyAthenaTests.Lib import McDump
job += McDump('mc_aod_dumper',mcName='GEN_AOD')
job += McDump('mc_dpd_dumper',mcName='GEN_DPD')

## get a handle on the application manager
from AthenaCommon.AppMgr import theApp

## read over some events from the input file(s)
theApp.EvtMax = EVTMAX

## configure the job to write out POOL files
import AthenaPoolCnvSvc.WriteAthenaPool
## create an output stream
job += CfgMgr.AthenaOutputStream(
    'OutStream',
    WritingTool = "AthenaPoolOutputStreamTool"
    )
job.OutStream.TakeItemsFromInput = False
job.OutStream.ForceRead          = True
job.OutStream.OutputFile         = OUTPUT
job.OutStream.ItemList = [ 'EventInfo#*',
                           'McEventCollection#GEN_DPD',
                           'TruthParticleContainer#SpclMC_DPD', 
                           "TruthEtIsolationsContainer#TruthEtIsol_GEN_DPD" ]
## EOF ##
