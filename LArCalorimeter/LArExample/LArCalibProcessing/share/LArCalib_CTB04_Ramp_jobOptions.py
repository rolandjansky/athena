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

theApp.Dlls += [ "LArByteStream"]

ByteStreamInputSvc = Service( "ByteStreamInputSvc" )

ByteStreamAddressProviderSvc = Service( "ByteStreamAddressProviderSvc" )
ByteStreamAddressProviderSvc.TypeNames += ["LArCalibDigitContainer/MEDIUM"]
ByteStreamAddressProviderSvc.TypeNames += ["LArCalibDigitContainer/HIGH"]

# Three input files (for H8): accordion HG, accordion MG, PS (HG only)
EventSelector.Input += ["/castor/cern.ch/atlas/testbeam/lar/2004/daq_ROS-41_LargCalib_0005134_file01.data"]
EventSelector.Input += ["/castor/cern.ch/atlas/testbeam/lar/2004/daq_ROS-41_LargCalib_0005135_file01.data"]
EventSelector.Input += ["/castor/cern.ch/atlas/testbeam/lar/2004/daq_ROS-41_LargCalib_0005136_file01.data"]


include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_H8_joboptions.py" )

DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DoIdChecks

ToolSvc = Service( "ToolSvc" )
ToolSvc.LArRoI_Map.Print=FALSE

include("LArCondCnv/LArCondCnv_TB04_jobOptions.py")
IOVDbSvc.Folders += ["/lar/LArElecCalibTB04/LArCalibParams"]
IOVDbSvc.Folders += ["/lar/LArElecCalibTB04/LArPedestal"]
IOVDbSvc.Folders += ["/lar/LArElecCalibTB04/LArCaliWaves/LArCaliWaveHigh"]
IOVDbSvc.Folders += ["/lar/LArElecCalibTB04/LArCaliWaves/LArCaliWaveMedium"]


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

theApp.topAlg+=["LArRampBuilder"]
LArRampBuilder=Algorithm("LArRampBuilder")
LArRampBuilder.KeyList+=["HIGH"]
LArRampBuilder.KeyList+=["MEDIUM"]
LArRampBuilder.SubtractDac0=TRUE
LArRampBuilder.DAC0=0
LArRampBuilder.StoreRawRamp=TRUE
LArRampBuilder.StoreRecRamp=TRUE
LArRampBuilder.Polynom=2
# Set to true if want to write Ramps into DB (use with great care !)
LArRampBuilder.WriteDB=FALSE
LArRampBuilder.RampRange = 3000;
# Reco can by Parabola or Shape. If Parabola, use correctBias=True (H8 only)
#LArRampBuilder.RecoType="Shape"
LArRampBuilder.RecoType="Parabola"
LArRampBuilder.correctBias=True

# output ntuples
theApp.topAlg+=["LArAverages2Ntuple/averageshigh"]
averageshigh=Algorithm("averageshigh")
averageshigh.containerKey="HIGH"

theApp.topAlg+=["LArAverages2Ntuple/averagesmedium"]
averagesmedium=Algorithm("averagesmedium")
averagesmedium.containerKey="MEDIUM"

theApp.topAlg+=["LArRamps2Ntuple/rampshigh"]
rampshigh=Algorithm("rampshigh")
rampshigh.containerKey="HIGH"

theApp.topAlg+=["LArRamps2Ntuple/rampsmedium"]
rampsmedium=Algorithm("rampsmedium")
rampsmedium.containerKey="MEDIUM"


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

