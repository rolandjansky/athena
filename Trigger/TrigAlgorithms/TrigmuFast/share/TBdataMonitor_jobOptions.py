################################################################
#
#  jopOptions file for reading test beam ByteStream 
#
################################################################

# Input 

# ByteStreamCnvSvc options
#-------------------------
include ( "ByteStreamCnvSvc/TBEventSelector_jobOptions.py" )
include ( "ByteStreamCnvSvcBase/BSAddProvSvc_RDO_jobOptions.py" )
include ( "ByteStreamCnvSvcBase/BSAddProvSvc_RIO_jobOptions.py" )



# Specify input file
#-------------------
ByteStreamInputSvc = Service ( "ByteStreamInputSvc" )

# ==> Directory and Prefix
if ( (not CombinedRun) and (not Local) ):
	ByteStreamInputSvc.InputDirectory += [ "/castor/cern.ch/atlas/testbeam/tbmuon/2004" ]
	ByteStreamInputSvc.FilePrefix += [ "daq_SFI-61_muons" ] 

elif ( CombinedRun and (not Local) ):
	ByteStreamInputSvc.InputDirectory += [ "/castor/cern.ch/atlas/testbeam/combined/2004" ]
	ByteStreamInputSvc.FilePrefix += [ "daq_SFI-51_combined" ]

elif ( Local ):
        ByteStreamInputSvc.InputDirectory += [ LocalRunDirectory ]
        ByteStreamInputSvc.FilePrefix += [ LocalFilePrefix ]

# ==> Run number 
# ByteStreamInputSvc.RunNumber = [ 600610 ] 
# file used in July for the pattern recognition studies
# configuration:   TB_2_RODmap.data   &   MuonTBData-00-00-52
ByteStreamInputSvc.RunNumber = [ RunNumber ]

ByteStreamInputSvc.MaxBadEvents = 224


# Specify the Converters
#-----------------------

ByteStreamCnvSvc = Service ( "ByteStreamCnvSvc" )

theApp.ExtSvc +=  [ "ByteStreamCnvSvc" ]
ByteStreamCnvSvc.InitCnvs +=  [ "MdtCsm" ]
# ByteStreamCnvSvc.InitCnvs += { "RpcPad" };



ToolSvc = Service ( "ToolSvc" )

# setup the standard Testbeam services (DetDescr, Cabling, Calib, Converters)
include ( "TrigmuFast/jobOptions_MuonTestBeam.py" )
ToolSvc.RpcColROD_Decoder.DecodeTDC = FALSE


# setup the L2 environment and the Data Monitor algor
#----------------------------------------------------
include ( "TrigmuFast/jobOptions_DataMonitor.py" )


# Output level & events
#----------------------
MessageSvc.OutputLevel = FATAL
theApp.EvtMax = EventNumber

#---------------------------------------------------------------
# Ntuple service output
#---------------------------------------------------------------
NTupleSvc                = Service ( "NTupleSvc " )
HistogramPersinstencySvc = Service ( "HistogramPersinstencySvc" )
HbookHistSvc             = Service ( "HbookHistSvc" )
#RootHistSvc              = Service ( "RootHistSvc" )

theApp.Dlls += [ "RootHistCnv" ]
#theApp.HistogramPersistency = "ROOT"
theApp.HistogramPersistency = "HBOOK"
NTupleSvc.Output    =  "FILE1 DATAFILE='trigger.ntup' OPT='NEW'" 
HistogramPersinstencySvc.OutputFile = "trigger.hbook"
HbookHistSvc.OutputFile = "trigger.his"
#RootHistSvc.OutputFile = "trigger.root"
