
# ==============================================================================
# Change the input file
#InputFile = "/afs/cern.ch/work/h/hristova/public/M7/248370/data14_cos.00248370.express_express.merge.RAW._lb0163._SFO-ALL._0001.1"
#InputFile = "/afs/cern.ch/user/k/korcyl/public/ZDC/data/data_test.1438344841.calibration_.daq.RAW._lb0000._ROS-FWD-ZDC-00._0001.data"
InputFile = "root://eosatlas//eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/data11_hi.00193403.physics_HardProbes.merge.RAW._lb0012._SFO-9._0001.1"
# ==============================================================================
#from TrigT1CaloByteStream.TrigT1CaloByteStreamConf import LVL1BS__TrigT1CaloDataAccessV2
from ZdcByteStream.ZdcByteStreamConf import ZdcDataAccessV2

from ByteStreamCnvSvc import ReadByteStream
import struct


svcMgr.ByteStreamInputSvc.FullFileName = [InputFile]


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
        self.tool = PyAthena.py_tool('ZdcDataAccessV2/ZdcDataAccessV2', 
                                     iface="IZdcDataAccessV2");        
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
ToolSvc += ZdcDataAccessV2("ZdcDataAccessV2")

topSequence += PyTriggerTowerRef("PyTriggerTowerRef")
theApp.EvtMax = -1
# ==============================================================================
