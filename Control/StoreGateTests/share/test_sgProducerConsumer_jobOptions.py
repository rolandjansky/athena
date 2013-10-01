###############################################################
#
# Job options file
#
#==============================================================

from AthenaCommon.AppMgr import theApp

###############################
# Load perf service
###############################
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
if not 'doMonitoring' in dir():
    doMonitoring = True
    pass
from PerfMonComps.PerfMonFlags import jobproperties as jp
jp.PerfMonFlags.doMonitoring = doMonitoring

#--------------------------------------------------------------
# General Application Configuration options
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixStandardJob

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
if not 'EVTMAX' in dir():
    EVTMAX = 1000
    pass
theApp.EvtMax = EVTMAX

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr.MessageSvc.OutputLevel = ERROR

class Options:
    DataName     = "MyData"
    NbrOfObjects = 10000
    ObjectsSize  = 100
    pass

## enable or not production of data using DataPools
if 'SGDATAPOOL' not in dir(): SGDATAPOOL = False

## enable various ways of running algorithms (Cpp/Py)
if 'ALGMODE' not in dir(): ALGMODE = 'cpp_cpp'

## schedule StoreGate stress tests
###################################

##############
## Producers
##############
if ALGMODE.startswith('cpp_'):
    topSequence += CfgMgr.SgStressProducer(
        'SgStressProducer',
        DataName     = Options.DataName,
        NbrOfObjects = Options.NbrOfObjects,
        ObjectsSize  = Options.ObjectsSize,
        UseDataPool  = SGDATAPOOL
        )
if ALGMODE.startswith('py_'):
    from StoreGateTests.Lib import PySgStressProducer
    topSequence += PySgStressProducer(
        'SgStressProducer',
        DataName     = Options.DataName,
        NbrOfObjects = Options.NbrOfObjects,
        ObjectsSize  = Options.ObjectsSize,
        UseDataPool  = SGDATAPOOL
        )

##############
## Consumers
##############
if ALGMODE.endswith('_cpp'):
    topSequence += CfgMgr.SgStressConsumer(
        'SgStressConsumer',
        DataName     = Options.DataName,
        NbrOfObjects = Options.NbrOfObjects
        )

if ALGMODE.endswith('_py'): 
    from StoreGateTests.Lib import PySgStressConsumer
    topSequence += PySgStressConsumer(
        'SgStressConsumer',
        DataName     = Options.DataName,
        NbrOfObjects = Options.NbrOfObjects
        )

## example of a full py/cpp comparison...
"""
athena -c'EVTMAX=1000;ALGMODE="cpp_py"' StoreGateTests/test_sgProducerConsumer_jobOptions.py
/bin/mv ntuple.pmon.gz cpp_py.ntuple.pmon.gz
athena -c'EVTMAX=1000;ALGMODE="cpp_cpp"' StoreGateTests/test_sgProducerConsumer_jobOptions.py
/bin/mv ntuple.pmon.gz cpp_cpp.ntuple.pmon.gz
athena -c'EVTMAX=1000;ALGMODE="py_py"' StoreGateTests/test_sgProducerConsumer_jobOptions.py
/bin/mv ntuple.pmon.gz py_py.ntuple.pmon.gz
athena -c'EVTMAX=1000;ALGMODE="py_cpp"' StoreGateTests/test_sgProducerConsumer_jobOptions.py
/bin/mv ntuple.pmon.gz py_cpp.ntuple.pmon.gz
athena -c'EVTMAX=1000;ALGMODE="cpp_cpp";SGDATAPOOL=True' StoreGateTests/test_sgProducerConsumer_jobOptions.py
/bin/mv ntuple.pmon.gz cpp_cpp.dp.ntuple.pmon.gz
athena -c'EVTMAX=1000;ALGMODE="cpp_py";SGDATAPOOL=True' StoreGateTests/test_sgProducerConsumer_jobOptions.py
/bin/mv ntuple.pmon.gz cpp_py.dp.ntuple.pmon.gz

perfmon.py cpp_cpp.ntuple.pmon.gz cpp_py.ntuple.pmon.gz py_cpp.ntuple.pmon.gz py_py.ntuple.pmon.gz -o out.root -l cpp,cpp_py,py_cpp,py -s "m.name.count('SgStre')>0"
"""

