
#
# This macro generates PhysicsNtuple data from AOD files
#
# Example command:
#
#  $ athena -c 'testMuon=True;EvtMax=200;setOption="minbias"' $TestArea/PhysicsNtuple/PhysicsAthena/share/savePhysicsData.py
#
#  -- testMuon, testData or testElec are used to select an input test file at CERN.
#
#  -- setOption string is passed to PhysicsAthenaConfig.preparePhysicsData function
#

from AthenaCommon.Logging import logging
spd_log = logging.getLogger('RunAOD.py')

#----------------------------------------------------------------------------------
# Read list of input files from local directory
#
def readInput():

    import os, string, sys
    filelist = []
    
    try:
        mylist = open("input_files.txt")
        
        for line in mylist.readlines():
            if line.count('ESD') > 0 or line.count('AOD') > 0:
                filelist.append(line.rstrip())
                print 'Added file: ',line
        mylist.close()
    except:
        pass

    return filelist
            
#----------------------------------------------------------------------------------
# Start script and configure input files
#
data_files = []
if 'InputFiles' not in dir() :

    pennww = 'root://eosatlas//eos/atlas/atlascerngroupdisk/penn-ww/test/'

    if   ('testData10' in dir()):
        data_files = [pennww+'data10_7TeV.00166466.physics_JetTauEtmiss.merge.AOD.r1774_p327_p333_tid207114_00/AOD.207114._000137.pool.root.1']
    elif ('testMuon10' in dir()):
        data_files = [pennww+'data10_7TeV.00166466.physics_Muons.merge.AOD.r1774_p327_p333_tid207199_00/AOD.207199._000026.pool.root.1']
    elif ('testElec10' in dir()):
        data_files = [pennww+'data10_7TeV.00166466.physics_Egamma.merge.AOD.r1774_p327_p333_tid207275_00/AOD.207275._000048.pool.root.4']                
    elif ('testData11' in dir() or 'testData' in dir()):
        data_files = [pennww+'data11_7TeV.00180636.physics_JetTauEtmiss.merge.AOD.f371_m824/data11_7TeV.00180636.physics_JetTauEtmiss.merge.AOD.f371_m824._lb0095-lb0096._0001.1']
    elif ('testMuon11' in dir() or 'testMuon' in dir()):
        data_files = [pennww+'data11_7TeV.00180636.physics_Muons.merge.AOD.f371_m824/data11_7TeV.00180636.physics_Muons.merge.AOD.f371_m824._lb0149-lb0151._0001.1']
    elif ('testElec11' in dir() or 'testElec' in dir()):
        data_files = [pennww+'data11_7TeV.00180636.physics_Egamma.merge.AOD.f371_m824/data11_7TeV.00180636.physics_Egamma.merge.AOD.f371_m824._lb0737-lb0739._0001.1']
    elif ('testMuon12' in dir()):
        data_files = [pennww+'data12_8TeV.00201555.physics_Muons.merge.AOD.f437_m1126/data12_8TeV.00201555.physics_Muons.merge.AOD.f437_m1126._lb0095._0001.1']
    elif ('testElec12' in dir()):
        data_files = [pennww+'data12_8TeV.00201555.physics_Egamma.merge.AOD.f437_m1126/data12_8TeV.00201555.physics_Egamma.merge.AOD.f437_m1126._lb0126._0001.1']    
    elif ('testMC10' in dir()):
        data_files = [pennww+'mc10_7TeV.105925.McAtNlo_JIMMY_WpWm_munuenu.merge.AOD.e598_s933_s946_r1831_r2040_tid266759_00/AOD.266759._000008.pool.root.1']
    elif ('testMC11' in dir()):
        data_files = [pennww+'mc11_7TeV.106047.PythiaZmumu_no_filter.merge.AOD.e815_s1272_s1274_r3043_r2993/AOD.645509._001019.pool.root.1']
    elif ('testMC12' in dir()):
        data_files = [pennww+'mc12_8TeV.147807.PowhegPythia8_AU2CT10_Zmumu.merge.AOD.e1169_s1469_s1470_r3542_r3549_tid779181_00/AOD.779181._000485.pool.root.1']
    elif ('testFile' in dir()):
        data_files = [testFile]
    
    if ('data_files' not in dir()) or len(data_files) < 1:
        data_files = readInput()
    
    if len(data_files) < 1:
        spd_log.error('Missing input files...')
        import sys
        sys.exit(1)
    
else :
    data_files = InputFiles

if not ('EvtMax' in dir()):
    EvtMax = -1

if ('setEvent' in dir()):
    EvtMax = setEvent
    
#----------------------------------------------------------------------------------
# Do auto configuration
#
spd_log.info('Do auto configuration...')

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput.set_Value_and_Lock( data_files )
athenaCommonFlags.EvtMax.set_Value_and_Lock( EvtMax )

spd_log.info('Update EvtMax to: %d' %athenaCommonFlags.EvtMax())
del EvtMax

from RecExConfig.InputFilePeeker import inputFileSummary
from RecExConfig.RecFlags import rec

rec.AutoConfiguration = [ 'everything' ]
rec.readRDO                 .set_Value_and_Lock( False )
rec.doCBNT                  .set_Value_and_Lock( False )
rec.doWriteESD              .set_Value_and_Lock( False )
rec.doWriteAOD              .set_Value_and_Lock( False )
rec.doWriteTAG              .set_Value_and_Lock( False )
rec.doWriteTAGCOM           .set_Value_and_Lock( False )
rec.doESD                   .set_Value_and_Lock( False )
rec.doAOD                   .set_Value_and_Lock( False )
rec.doDPD                   .set_Value_and_Lock( False )
rec.doHist                  .set_Value_and_Lock( False )
rec.doPerfMon               .set_Value_and_Lock( False )
rec.doForwardDet            .set_Value_and_Lock( False )
rec.doFloatingPointException.set_Value_and_Lock( False )
rec.doTrigger               .set_Value_and_Lock( True )
rec.RootNtupleOutput        .set_Value_and_Lock( 'test.root' )
rec.RootHistoOutput         .set_Value_and_Lock( 'test.root' )

#from RecExConfig.RecAlgsFlags  import recAlgs
#recAlgs.doMissingET.set_Value_and_Lock(True)


from PerfMonComps.PerfMonFlags import jobproperties as pmon_properties

pmon_properties.PerfMonFlags.doSemiDetailedMonitoring.set_Value_and_Lock(False)

# To Re-run the Electron ID
#from egammaRec.egammaRecFlags import jobproperties
#jobproperties.egammaRecFlags.doAODRender.set_Value_and_Lock(True)

include('RecExCommon/RecExCommon_topOptions.py')

spd_log.info('Finished setting up RecExConfig...')

if ('printAuto' in dir()):
    spd_log.info('Print inputFileSummary with %d element(s)' %len(inputFileSummary))
    
    for k, v in inputFileSummary.iteritems():
        print k, v
        
#----------------------------------------------------------------------------------
# Set up trigger
#
from TriggerJobOpts.TriggerFlags import TriggerFlags
#setMenu = 'Physics_pp_v1'
#TriggerFlags.triggerMenuSetup = setMenu
TriggerFlags.readLVL1configFromXML=False
TriggerFlags.readHLTconfigFromXML=False
TriggerFlags.inputLVL1configFile=''
TriggerFlags.inputHLTconfigFile=''

TriggerFlags.readBS   = False
TriggerFlags.doLVL1   = True
TriggerFlags.doLVL2   = True
TriggerFlags.doEF     = True
TriggerFlags.doNtuple = True

#TriggerFlags.L1PrescaleSet  = 'L1Prescales100_' +TriggerFlags.triggerMenuSetup()
#TriggerFlags.HLTPrescaleSet = 'HLTPrescales100_'+TriggerFlags.triggerMenuSetup()
TriggerFlags.configurationSourceList=['xml']
TriggerFlags.enableMonitoring = [ 'Time', 'CostAthena' ]
TriggerFlags.disableRandomPrescale=False

from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
TriggerConfigGetter('ReadPool')

#----------------------------------------------------------------------------------
# Useful formatting options and debugging information
#
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 10000000
ServiceMgr.MessageSvc.Format = '% F%50W%S%7W%R%T %0W%M'

if ('dumpSG' in dir()) and dumpSG:
    StoreGateSvc = Service('StoreGateSvc')
    StoreGateSvc.Dump = True

if ('dumpMC' in dir()) and dumpMC:
    from TruthExamples.TruthExamplesConf import DumpMC
    DumpMC.McEventKey = 'GEN_AOD'
    topSeq += DumpMC()

if not hasattr(ServiceMgr, 'AthenaEventLoopMgr'):
    from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
    ServiceMgr += AthenaEventLoopMgr()

ServiceMgr.AthenaEventLoopMgr.EventPrintoutInterval = 100

if ('allowAbort' in dir()) and allowAbort:
    spd_log.info('Enable job abort on exception')
    from AthenaCommon.AthenaCommonFlags import jobproperties
    jobproperties.AthenaCommonFlags.RuntimeStrictness.set_Value_and_Lock('abort')


from TrigCostMonitor.TrigCostMonitorConfig import readInputFiles
inputfilelist = readInputFiles('input_files.txt','AOD')

from TrigCostMonitor.TrigCostMonitorConfig import setupCostAlg, setupCostJob, setupCostExtras
setupCostAlg()
setupCostJob()
setupCostExtras()

#from TrigCostMonitor.TrigCostMonitorConfig import prepareCostAlg
#runAlg = prepareCostAlg('TrigCostAlg', 'rate', 'read')

#from AthenaCommon.AlgSequence import AlgSequence
#topSeq = AlgSequence()
#topSeq += [runAlg]
