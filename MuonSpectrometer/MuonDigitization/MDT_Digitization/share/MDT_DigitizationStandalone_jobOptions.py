# the Athena Pool converters
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )

include( "MuonEventAthenaPool/MuonEventAthenaPool_joboptions.py" )

include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )

PoolSvc = Service( "PoolSvc" )
PoolSvc.DbType     = "root";  # to define ROOT file resident  collection
EventSelector = Service( "EventSelector" )
EventSelector.InputCollection = "NewPoolTry"
theApp.Dlls += [ "MDT_Digitization"]
theApp.Dlls += [ "GaudiAlg" ,"GaudiAud"]
#ApplicationMgr.Dlls += { "EventHdrAthenaRoot", "MuonSimHitAthenaRoot"};
#ApplicationMgr.Dlls   += { "RootSvcModules" , "AthenaRootCnvSvc"  };
#ApplicationMgr.ExtSvc += { "RootSvc" };
include( "MuonDetMgrDetDescrCnv/MuonDetMgrDetDescrCnv_joboptions.py" )

theApp.Dlls += ["MuonDetMgrDetDescrCnv"]
DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DecodeIdDict = TRUE
#DetDescrCnvSvc.ReadFromROOT = true;
# set the appropriate xml file for the Muon ID Dict
#DetDescrCnvSvc.MuonIDFileName  = "IdDictMuonSpectrometer_P.03.xml";
theApp.Dlls += [ "AmdbAth", "MuonAsciiDetDescrDecoder" ]
theApp.ExtSvc += [ "AmdbsimrecAthenaSvc"]
theApp.ExtSvc += [ "MuonAsciiDetDescrSvc" ]
theApp.TopAlg += ["MDT_Digitizer"]
# MDT_Digitizer
# ------------
MDT_Digitizer = Algorithm( "MDT_Digitizer" )
MDT_Digitizer.InnerTubeRadius  = 14.6275
MDT_Digitizer.EffectiveRadius  = 14.4275
MDT_Digitizer.OffsetTDC        =       0
MDT_Digitizer.BunchCountOffset = -200.
MDT_Digitizer.UseTof = FALSE
MDT_Digitizer.UseProp = FALSE
# uncomment the following line if you want to run with 
# the external Garfield rt relation
#MDT_Digitizer.DigitizationTool = "RT_Relation_DigiTool"

#DumpSimHits
ReadMuonSimHits.DumpMDTSimHits =  TRUE
ReadMuonSimHits = Algorithm( "ReadMuonSimHits" )
ReadMuonSimHits.DumpRPCSimHits =  FALSE
ReadMuonSimHits.DumpTGCSimHits =  FALSE
ReadMuonSimHits.DumpCSCSimHits =  FALSE
# DumpDigits
ReadMuonDigits = Algorithm( "ReadMuonDigits" )
ReadMuonDigits.DumpMdtDigits = TRUE
ReadMuonDigits.DumpRpcDigits = TRUE
ReadMuonDigits.DumpTgcDigits = TRUE
ReadMuonDigits.DumpCscDigits = FALSE
