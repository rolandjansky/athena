###########################################################################
#
# Example Job options running the RPC LVL1 simulation from ZebraTDR events
# and from G4 data. It produces the ntuple for efficiency computation.
#
#==========================================================================


#--------------------------------------------------------
# configuration flags
#--------------------------------------------------------
UseRoot = False   # set to True for using Root
G4data  = True    # set to False for using ZEBRA data

MuonLayout = "P03-DC1"   # set the Muon Layout Type
                         # Available types:
			 # P03-DC1, P03-DC2, Q02_initial, Q02, CTB2004

EffMonitor = True # set to True for producing the efficiency ntuple
DetailedTW = True # set to True for producing the Trigger Windows ntuple


PoolRDOInput = [ "rfio:/castor/cern.ch/user/d/drebuzzi/preprod9.0.2/p.03-mu+eta0.4-0.9.root" ]


Events = 20
#SkipEvents = 319
SkipEvents = 0


if G4data:
    #-----------------------------------------------------------
    # Configure the job to access G4 data
    #-----------------------------------------------------------
    include( "AthenaCommon/Atlas.UnixStandardJob.py" )

    # include generic pool converters
    include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
    include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

    # include specific fragment for the Muon Converters
    include( "MuonEventAthenaPool/MuonEventAthenaPool_joboptions.py" )

    # include converter for the Montecarlo Truth
    include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )


    #-----------------------------------------------------------
    # Cofigure the detector description
    #-----------------------------------------------------------
    if (MuonLayout == "P03-DC1" or MuonLayout == "P03-DC2"):
        DetDescrVersion = "DC2"
    elif (MuonLayout == "Q02_initial"):
        DetDescrVersion = "Rome-Initial"
	
    include( "AtlasGeoModel/GeoModelCommon.py" )
    include( "AtlasGeoModel/MuonGeoModel.py" )

    DetDescrCnvSvc.DecodeIdDict = TRUE


    #-----------------------------------------------------------
    # Cofigure the RPC digitization only for G4 data
    #-----------------------------------------------------------
    # configure Pileup
    theApp.Dlls += [ "PileUpTools" ]

    # RPC Digitization 
    include( "RPC_Digitization/RPC_Digitization_jobOptions.py" )

else:
    #-----------------------------------------------------------
    # Configure the job to access G3 data
    #-----------------------------------------------------------
    include ("AthenaCommon/Atlas_ZebraTDR.UnixStandardJob.py")
    theApp.Dlls += [ "MuonZebraCnvTest", "MuonDigitZebraCnv" ]
    

    #-----------------------------------------------------------
    # Cofigure the detector description
    #-----------------------------------------------------------
    include ("MuonDetMgrDetDescrCnv/MuonDetMgrDetDescrCnv_joboptions.py")
    
    #setup the IdHelper for RPC and MDT
    include ( "DetDescrCnvSvc/DetStore_joboptions.py" )
    
    # Turn on the reading of the dictionary
    DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
    DetDescrCnvSvc.DecodeIdDict = TRUE    
    
    if (MuonLayout == "P03-DC1" or MuonLayout == "P03-DC2"):
        DetDescrCnvSvc.MuonIDFileName  = "IdDictMuonSpectrometer_P.03.xml" 

    # Genzkine->HepMc converter
    theApp.Dlls += [ "GenzModule" ]
    theApp.TopAlg += [ "GenzModule" ]
    

include ("PartPropSvc/PartPropSvc.py")

# include the ANDC stuff
include ("AmdcAth/AmdcAth_jobOptions.py")

    
# Configure the LVL1 RPC simulation
theApp.Dlls += [ "RPCgeometry" ]
theApp.Dlls += [ "RPCcabling" ]
theApp.Dlls += [ "TrigT1RPCsteering" ]

theApp.ExtSvc += [ "RPCgeometrySvc" ]
theApp.ExtSvc += [ "RPCcablingSvc" ]



# top algorithms to be run
theApp.TopAlg += [ "TrigT1RPC" ]


RPCgeometrySvc = Service ("RPCgeometrySvc")
RPCcablingSvc  = Service ("RPCcablingSvc")
StoreGateSvc   = Service ("StoreGateSvc")
TrigT1RPC      = Algorithm ("TrigT1RPC")


#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------

MessageSvc.OutputLevel      = 4
TrigT1RPC.OutputLevel       = 4

# dump the StoreGate contents for debugging
#StoreGateSvc.Dump = True  #true will dump data store contents
#StoreGateSvc.OutputLevel = 1


#--------------------------------------------------------------
# Define Printout,Debug and Dump flags for RPC geometry service
#--------------------------------------------------------------
RPCgeometrySvc.DumpRPCGeometry = False
RPCgeometrySvc.DumpRPCStations = False
RPCgeometrySvc.Debug           = False
RPCgeometrySvc.DumpLVL1conf    = False
RPCgeometrySvc.DumpFileName    = "LVL1conf.dump" 


#--------------------------------------------------------------
# Define configuration file for cabling service
#--------------------------------------------------------------
if (MuonLayout == "P03-DC1" or MuonLayout == "P03-DC2"):
    RPCcablingSvc.ConfFileName    = "P03conf.data"
    RPCcablingSvc.CorrFileName    = "P03conf.corr"
else:
    RPCcablingSvc.ConfFileName    = "LVL1conf.data"
    RPCcablingSvc.CorrFileName    = "LVL1conf.corr"


#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

# Number of events to be processed
theApp.EvtMax = Events


EventSelector = Service ("EventSelector")
EventSelector.SkipEvents = SkipEvents

if G4data:
    theApp.EvtSel = "EventSelector"
    EventSelector.InputCollections = PoolRDOInput
else:
    #EventSelector.EventNumbers = " 1806 " ;

    EventSelector.firstRun=1
    EventSelector.lastRun=10000
    EventSelector.readHits = False                        
    EventSelector.directConversion = True

    # Switch off detectors not needed
    EventSelector.readHits = False                        
    EventSelector.readDigits = False                        
    EventSelector.calos = False                        
    EventSelector.muons = False                        
    EventSelector.mdt   = False                        
    EventSelector.rpc   = False                        
    EventSelector.tgc   = True                        
    EventSelector.trt   = False
    EventSelector.sct   = False
    EventSelector.pixel = False



#--------------------------------------------------------------
# Define configuration file for Level-1 Muon Barrel Trigger
#--------------------------------------------------------------
TrigT1RPC.Hardware          = True
TrigT1RPC.DataDetail        = False
TrigT1RPC.RPCbytestream     = False
TrigT1RPC.RPCbytestreamFile = ""
TrigT1RPC.MaxMuon = 100
TrigT1RPC.MaxRoI = 100

TrigT1RPC.EffMonitor = EffMonitor
TrigT1RPC.DetailedTW = DetailedTW

if not G4data:
    TrigT1RPC.KeyForTruth = "GEN_EVENT"


#---------------------------------------------------------------
# Ntuple service output
#---------------------------------------------------------------

theApp.Dlls += [ "HbookCnv" ]
theApp.Dlls += [ "RootHistCnv" ]

NTupleSvc = Service( "NTupleSvc" )
# ntuple output name
NTupleSvc.Output = [ "FILE1 DATAFILE='trigger.ntup' OPT='NEW'" ]

if UseRoot:
    theApp.HistogramPersistency = "ROOT"

    RootHistSvc            = Service ( "RootHistSvc" )
    RootHistSvc.OutputFile = "trigger.root"
else:
    theApp.HistogramPersistency = "HBOOK"
    
    HbookHistSvc            = Service ( "HbookHistSvc" )
    HbookHistSvc.OutputFile = "trigger.his"   
