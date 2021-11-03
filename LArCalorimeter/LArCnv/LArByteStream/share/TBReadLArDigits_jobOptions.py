# Location & number of raw data file:
runnumber=1000066
RawDataDir="/castor/cern.ch/atlas/testbeam/combined/2004/"
RawDataFilePrefix="daq_SFI-51_calo"
#Gain, set HIGH,MEDIUM,LOW or FREE
GainKey="FREE" 
# Number of events to be processed
theApp.EvtMax = 100
#Result location (full path name, no ~ and trailing / please!)
ResultDir=""


# ***************************************************************
include( "ByteStreamCnvSvc/TBEventSelector_jobOptions.py" )

ByteStreamAddressProviderSvc = Service( "ByteStreamAddressProviderSvc" )
ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/HIGH"] 
ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/MEDIUM"] 
ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/LOW"] 
ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/FREE"] 
include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_H8_joboptions.py" )

if GainKey=="FREE":
  ToolSvc.LArRodDecoder.FirstSample=2

# Directory & Prefix & Runnumber 
ByteStreamInputSvc=Service("ByteStreamInputSvc")
ByteStreamInputSvc.InputDirectory += [RawDataDir]
ByteStreamInputSvc.FilePrefix += [RawDataFilePrefix]
ByteStreamInputSvc.RunNumber = [runnumber]


# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel =3
AthenaEventLoopMgr = Service ("AthenaEventLoopMgr")
AthenaEventLoopMgr.OutputLevel=4

# The only TopAlg: ReadLArDigits 
theApp.TopAlg += ["ReadLArDigits"]
ReadLArDigits = Algorithm( "ReadLArDigits" )
ReadLArDigits.ContainerKey=GainKey;
#compose file name:
dumpFileName='%(dir)sLArDigits_%(No)d_%(key)s.txt' % {"dir" : ResultDir, "No" : runnumber, "key" : GainKey}
print "**** Output file Name: ****"
print dumpFileName

ReadLArDigits.DumpFile=dumpFileName
ReadLArDigits.PrintCellLocation=FALSE
#StoreGateSvc.dump=true;

#----- JobOptions for Ntuple-output
NtupleFileName='%(dir)sLArDigits_%(No)d_%(key)s.root' % {"dir" : ResultDir, "No" : runnumber, "key" : GainKey}
print "**** Ntuple file Name: ****"
print NtupleFileName
NtupleString="FILE1 DATAFILE='" + NtupleFileName + "' TYP='ROOT' OPT='NEW'" 
theApp.Dlls += [ "RootHistCnv" ]; 
theApp.HistogramPersistency = "ROOT" 
NtupleSvc = Service( "NtupleSvc" )
#NtupleSvc.Output=["FILE1 DATAFILE='~/scratch0/LArDigits_66_free.root' TYP='ROOT' OPT='NEW'"]
NtupleSvc.Output=[NtupleString]
