# the G4 digitization
#include( "MuonDigiExample/G4MuonDigit_jobOptions.py" )

# specify input file here.
EventSelector = Service( "EventSelector" )
EventSelector.InputCollection="rfio:/castor/cern.ch/atlas/project/dc2/preprod/g4sim803/dc2.002881.mu_pt10tev_eta120.g4sim803/data/dc2.002881.mu_pt10tev_eta120.g4sim803._0001.pool.root"
theApp.Dlls += [ "GaudiAlg","GaudiAud" ]
theAuditorSvc = AuditorSvc()
theAuditorSvc.Auditors  += [ "ChronoAuditor" ]
include( "RpcClusterization/RpcClusterJobOption.py" )

# MuonGeoModel Initialization
include( "MuonGeoModel/MuonGeoModel_sa_fragmentOptions.py" )

# Turn on the reading of the dictionary
DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DecodeIdDict = TRUE
# set the appropriate xml file for the Muon ID Dict
DetDescrCnvSvc.MuonIDFileName  = "IdDictMuonSpectrometer_P.03.xml"
include( "PartPropSvc/PartPropSvc.py" )

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = 2
# Number of events to be processed (default is 10)
theApp.EvtMax = 500
EventSelector.SkipEvents=4500
