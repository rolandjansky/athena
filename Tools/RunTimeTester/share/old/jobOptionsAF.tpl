//##############################################################
//
// Job options file for running simpleTest
//
//==============================================================
//

#include "RunTimeTester/GeneratorOptions_!$PROCESS$!.txt"
#include "PartPropSvc/PartPropSvc.txt"
ApplicationMgr.DLLs += { "GaudiAlg" };	
ApplicationMgr.DLLs += { "GaudiAud" };
AuditorSvc.Auditors  = { "ChronoAuditor" };


ApplicationMgr.ExtSvc += {"AtRndmGenSvc"};

ApplicationMgr.DLLs += { "AtlfastAlgs","TestAtlfast"};
//--------------------------------------------------------------
// Event related parameters
//--------------------------------------------------------------
// Number of events to be processed (default is 10)
ApplicationMgr.EvtMax = 1000;

//--------------------------------------------------------------
// Private Application Configuration options
//--------------------------------------------------------------


// Execution of algorithms given in sequencer
//
ApplicationMgr.TopAlg = {"Sequencer/TopSequence"};
TopSequence.Members = {"Sequencer/Generator","Sequencer/Atlfast"};

#include "RunTimeTester/TestAtlfastOptions.txt"

//set up individual jobOptions
ElectronMaker.DoSmearing		 = !$SMEARING$!;
MuonMaker.DoSmearing                     = !$SMEARING$!;
PhotonMaker.DoSmearing			 = !$SMEARING$!;
JetMaker.DoSmearing       	         = !$SMEARING$!;
TrackMaker.DoSmearing       	         = !$SMEARING$!;

GlobalEventDataMaker.Luminosity		=  !$LUMINOSITY$!;	


ClusterMaker.ProcessCells       = !$CLUSTERCELLS$!;
ClusterMaker.ProcessTracks      = !$CLUSTERTRACKS$!;

ClusterMaker.Strategy           = "!$CLUSTERSTRATEGY$!";

//SimpleTest1
Atlfast.members += {"TestAtlfast::SimpleTest/SimpleTest1"};
SimpleTest1.writeRef                     = true;
SimpleTest1.outFile                      = "./outFile_simpleTest.txt";
SimpleTest1.refFile                      = "./refFile_outFile_simpleTest.txt";
SimpleTest1.cmpFile                      = "./cmpFile_simpleTest.txt";


Isajet.IsajetCommand = {"isalis !$FLAGS$!.lis"};
MessageSvc.OutputLevel               = 3;

//==============================================================
//
// End of job options file
//
//##############################################################










