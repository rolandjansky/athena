#-----------------------------------------------------------------------------
# Athena imports
#-----------------------------------------------------------------------------
from AthenaCommon.Constants import *
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr
#import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()


#-----------------------------------------------------------------------------
# Message Service
#-----------------------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
ServiceMgr.MessageSvc.OutputLevel = WARNING
#import AthenaServices
#AthenaServices.AthenaServicesConf.AthenaEventLoopMgr.OutputLevel = WARNING


#-----------------------------------------------------------------------------
# Input Datasets
#-----------------------------------------------------------------------------
#ServiceMgr.EventSelector.InputCollections = [ ]
theApp.EvtMax = 0 # -1 means all events


#-----------------------------------------------------------------------------
# Algorithms
#-----------------------------------------------------------------------------
from CalibrationDataInterface.CalibrationDataInterfaceConf import Analysis__CalibrationDataUpdater as updater

tagname = 'MyCalibV1'
flavours = [ 'B', 'C', 'Light' ]
folder = 'Cuts'
jets = [ 'Antikt4H1Tower', 'Antikt4H1Topo', 'Antikt4LCTopo' ]
OPs = [ 'Loose', 'Medium', 'Tight' ]
taggers = [ 'SV0' ]
inputcalibration = 'dummy'
outputcalibration = 'mydummy'
names = [ ]
for jet in jets:
    for tagger in taggers:
        for OP in OPs:
            for flavour in flavours:
                inn = jet + '/' + tagger + '/' + OP + '/' + inputcalibration + '_Eff'
                out = tagger + '/' + jet + '/' + OP + '/' + flavour + '/' + outputcalibration + '_Eff'
                name = inn + '->' + out
                names.append(name)
                inn = jet + '/' + tagger + '/' + OP + '/' + inputcalibration + '_SF'
                out = tagger + '/' + jet + '/' + OP + '/' + flavour + '/' + outputcalibration + '_SF'
                name = inn + '->' + out
                names.append(name)

theJob += updater('CalibUpdater',
                  inputRootFile = 'perfcalibrations.root',
                  DbRootFile = 'BTaggingPerformance'+tagname+'_'+folder+'.root',
                  paramNames = names,
                  overrideDefaults = False,
                  OutputLevel = INFO)


print theJob

