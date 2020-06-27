# *******************************************************************
# JobOptions file to produce ramps
#
# 15/08/2005 S. Laplace (laplace@lapp.in2p3.fr)
# *******************************************************************

include( "ByteStreamCnvSvc/TBEventSelector_jobOptions.py" )
from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOn()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

from AthenaCommon.GlobalFlags  import GlobalFlags
GlobalFlags.DetGeo.set_atlas()
GlobalFlags.DataSource.set_data()
include( "AthenaCommon/Atlas.UnixStandardJob.py" )



theApp.Dlls += [ "LArByteStream"]

ByteStreamInputSvc = Service( "ByteStreamInputSvc" )

ByteStreamAddressProviderSvc = Service( "ByteStreamAddressProviderSvc" )
ByteStreamAddressProviderSvc.TypeNames += ["LArCalibDigitContainer/MEDIUM"]
ByteStreamAddressProviderSvc.TypeNames += ["LArCalibDigitContainer/HIGH"]

# Three input files (for H8): accordion HG, accordion MG, PS (HG only)
#EventSelector.Input += ["/castor/cern.ch/atlas/testbeam/lar/2004/daq_ROS-41_LargCalib_0005134_file01.data"]
EventSelector.Input += ["/castor/cern.ch/atlas/testbeam/lar/2004/daq_ROS-41_LargCalib_0005135_file01.data"]
#EventSelector.Input += ["/castor/cern.ch/atlas/testbeam/lar/2004/daq_ROS-41_LargCalib_0005136_file01.data"]


include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_H8_joboptions.py" )

DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DoIdChecks

ToolSvc = Service( "ToolSvc" )
ToolSvc.LArRoI_Map.Print=FALSE

include ("LArConditionsCommon/LArConditionsCommon_H8_jobOptions.py")
IOVDbSvc.Folders += ["/LAR/LArElecCalibH8/LArCalibParams<tag>LArCalibParams-"+LArTB04FolderTag_Calib+"</tag>"+LArH8DBConnection]

theApp.Dlls += ["LArRawUtils"]
theApp.Dlls += ["LArTools"]
theApp.Dlls += ["LArCalibUtils"]
theApp.Dlls += ["LArCalibTools"]
theApp.Dlls += ["LArRecUtils"]
theApp.Dlls += ["LArROD"]

theApp.topAlg+=["LArCalibDigitsAccumulator"]
LArCalibDigitsAccumulator=Algorithm("LArCalibDigitsAccumulator")
LArCalibDigitsAccumulator.KeyList+=["HIGH"]
LArCalibDigitsAccumulator.KeyList+=["MEDIUM"]

# output ntuples
theApp.topAlg+=["LArAverages2Ntuple/averageshigh"]
averageshigh=Algorithm("averageshigh")
averageshigh.containerKey="HIGH"

theApp.topAlg+=["LArAverages2Ntuple/averagesmedium"]
averagesmedium=Algorithm("averagesmedium")
averagesmedium.containerKey="MEDIUM"

theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"
NTupleSvc = Service( "NTupleSvc" )

NTupleSvc.Output    = [ "FILE1 DATAFILE='LArAverages.root' OPT='NEW'" ]


AthenaEventLoopMgr=Service("AthenaEventLoopMgr")
AthenaEventLoopMgr.OutputLevel=ERROR

MessageSvc.OutputLevel=INFO
#MessageSvc.OutputLevel=DEBUG
MessageSvc.defaultLimit=1000000;

#StoreGateSvc=Service("StoreGateSvc");
#StoreGateSvc.dump=TRUE

