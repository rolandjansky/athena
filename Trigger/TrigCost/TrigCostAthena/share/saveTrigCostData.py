#athenaCommonFlags.FilesInput= ["/tmp/aidan/q122.ESD.root"]

#----------------------------------------------------------------------------------
# Try to read list of input files from local directory
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
# Start script...
#
from AthenaCommon.Logging import logging
logger = logging.getLogger('saveTrigCostData')

#----------------------------------------------------------------------------------
# Define input files
#
logger.info('Define input files')

data_files = []
if 'InputFiles' not in dir() :
    InputFiles = []
    
    if   ('testAOD' in dir()):
        InputFiles = ["/exports/project/data_d02_2/rustem/rates/data09_900GeV.00142383.physics_MinBias.merge.AOD.r988_p62_tid102217_00/AOD.102217._000017.pool.root.1"]
    elif ('testESD' in dir()):
        InputFiles = ["/exports/project/data_d02_2/rustem/rates/data09_900GeV.00142383.physics_MinBias.recon.ESD.r988_tid101673_00/ESD.101673._000290.pool.root.1"]
    elif ('testData' in dir()):
        InputFiles = ["root://castoratlas//castor/cern.ch/grid/atlas/atlaslocalgroupdisk/data10_7TeV/AOD/f236_m427/data10_7TeV.00152166.physics_MinBias.merge.AOD.f236_m427/data10_7TeV.00152166.physics_MinBias.merge.AOD.f236_m427._lb0197-lb0209._0001.1"]
    elif ('testFile' in dir()):
        InputFiles = [testFile]
    
    if len(InputFiles) < 1:
        InputFiles = readInput()
    
    data_files = InputFiles

else :
    data_files = InputFiles

print runArgs

if runArgs.tmpESD:
    data_files = [runArgs.tmpESD]

if not ('EvtMax' in dir()):
    EvtMax = -1

#----------------------------------------------------------------------------------
# Do autoconfiguration
#
logger.info('Do autoconfiguration')

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput.set_Value_and_Lock( data_files )
athenaCommonFlags.EvtMax.set_Value_and_Lock( EvtMax )

from RecExConfig.RecFlags import rec
rec.AutoConfiguration = [ "everything" ]

rec.doCBNT.set_Value_and_Lock( False )
rec.doWriteESD.set_Value_and_Lock( False )
rec.doWriteAOD.set_Value_and_Lock( False )
rec.doWriteTAG.set_Value_and_Lock( False )
rec.doWriteTAGCOM.set_Value_and_Lock( False )
rec.doESD.set_Value_and_Lock( False )
rec.doAOD.set_Value_and_Lock( False )
rec.doDPD.set_Value_and_Lock( False )
rec.doHist.set_Value_and_Lock( False )
rec.doPerfMon.set_Value_and_Lock( False )
rec.doForwardDet.set_Value_and_Lock( False )
#rec.RootNtupleOutput.set_Value_and_Lock( '' )
rec.RootHistoOutput.set_Value_and_Lock( '' )

include('RecExCommon/RecExCommon_topOptions.py')

#----------------------------------------------------------------------------------
# Setup cost algorithms and tools
#
logger.info('Setup cost algorithms and tools')

import TrigCostAthena.TrigCostAthenaConfig as costAthena

if not ('setOption' in dir()) or type(setOption) != str:
    setOption = ''

setOption = setOption.replace('.root','')
alg = costAthena.prepareCostAlg(setOption)

from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()
topSeq += [alg]

#----------------------------------------------------------------------------------
# Set up the trigger configuration
#
logger.info('Set up the trigger configuration')

from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
TriggerConfigGetter( "ReadPool" )

ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 10000000
ServiceMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"

if not hasattr(ServiceMgr, "AthenaEventLoopMgr"):
    from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
    ServiceMgr += AthenaEventLoopMgr()

ServiceMgr.AthenaEventLoopMgr.EventPrintoutInterval = 100
