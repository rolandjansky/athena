
# ==============================================================================
# Change the input file
InputFile = "/afs/cern.ch/work/h/hristova/public/M7/248370/data14_cos.00248370.express_express.merge.RAW._lb0163._SFO-ALL._0001.1"
# ==============================================================================
from TrigT1CaloByteStream.TrigT1CaloByteStreamConf import LVL1BS__TrigT1CaloDataAccessV2

from ByteStreamCnvSvc import ReadByteStream
import struct


svcMgr.EventSelector.Input = [InputFile]


from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'atlas'
globalflags.DataSource = 'data'

from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.all_setOff()
include("RecExCond/AllDet_detDescr.py")

class PyTriggerTowerRef(PyAthena.Alg):

    def __init__(self, name='PyTriggerTowerRef', **kw):
        # init base class
        kw['name'] = name
        super(PyTriggerTowerRef, self).__init__(**kw)

    def initialize(self):
        self.tool = PyAthena.py_tool('LVL1BS::TrigT1CaloDataAccessV2/TrigT1CaloDataAccessV2', 
                                     iface="LVL1BS::ITrigT1CaloDataAccessV2");        
        return PyAthena.StatusCode.Success

    def execute(self):

        self.tool.PrintTriggerTowers();
        
        return PyAthena.StatusCode.Success

    def finalize(self):
        return PyAthena.StatusCode.Success
    pass

# ==============================================================================
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
ToolSvc += LVL1BS__TrigT1CaloDataAccessV2("TrigT1CaloDataAccessV2")

topSequence += PyTriggerTowerRef("PyTriggerTowerRef")
theApp.EvtMax = -1
# ==============================================================================