#Include all the basic stuff and the Event Selector to read TB files 
include( "ByteStreamCnvSvc/TBEventSelector_jobOptions.py" )

#Register Containers/Keys to be converted from ByteStream
ByteStreamAddressProviderSvc = Service( "ByteStreamAddressProviderSvc" )
ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/HIGH"]
ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/MEDIUM"]
ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/LOW"]
#Get DetectorDescription
include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_H8_joboptions.py" )

#Raw Data Location:
ByteStreamInputSvc = Service( "ByteStreamInputSvc" )
ByteStreamInputSvc.InputDirectory += ["/castor/cern.ch/atlas/testbeam/combined/2004/"]
ByteStreamInputSvc.FilePrefix += ["daq_SFI-51_calo"]
ByteStreamInputSvc.RunNumber = [1000919]
#DetDescrCnvSvc.DoIdChecks;
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel =DEBUG
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 100
#Get necessary DLLs
theApp.Dlls += [ "LArRawUtils","LArCalibUtils", "LArTools"]
theApp.Dlls += [ "LArByteStream"]
#Define Top Algoithm to analyze Pedestals and make a ntuple
theApp.TopAlg = ["LArPedestalMaker/Pedestal","LArPedestals2Ntuple"]

Pedestal = Algorithm( "Pedestal" )
Pedestal.events_ref =50
Pedestal.nsigma =5

LArPedestals2Ntuple=Algorithm("LArPedestals2Ntuple")
LArPedestals2Ntuple.ContainerKey = "Pedestal"
#StoreGateSvc = Service( "StoreGateSvc" )
#StoreGateSvc.dump=TRUE

#----- JobOptions for Ntuple-output
theApp.Dlls += [ "RootHistCnv" ]; 
theApp.HistogramPersistency = "ROOT" 
#HistogramPersistencySvc = Service( "HistogramPersistencySvc" )
#HistogramPersistencySvc.OutputFile="histo.root"

NtupleSvc = Service( "NtupleSvc" )
NtupleSvc.Output=["FILE1 DATAFILE='ped_1000919.root' TYP='ROOT' OPT='NEW'"]

