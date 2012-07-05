from AthenaCommon.AppMgr import ServiceMgr
from MdtCalibIOSvc.MdtCalibIOSvcConf import MdtCalibInputSvc
from MdtCalibIOSvc.MdtCalibIOSvcConf import MdtCalibOutputDbSvc
from MdtCalibIOSvc.MdtCalibIOSvcConf import MuonCalib__CalibrationFileIOTool
from MdtCalibIOSvc.MdtCalibIOSvcConf import MuonCalib__CalibrationOracleFileIOTool
MdtCalibInputSvc = MdtCalibInputSvc()
MdtCalibOutputDbSvc = MdtCalibOutputDbSvc()
CalibrationFileIOTool  = MuonCalib__CalibrationFileIOTool()
CalibrationOracleFileIOTool = MuonCalib__CalibrationOracleFileIOTool()
from AthenaCommon.AppMgr import ToolSvc
ToolSvc += CalibrationFileIOTool
ToolSvc += CalibrationOracleFileIOTool
ServiceMgr += MdtCalibInputSvc
ServiceMgr += MdtCalibOutputDbSvc
