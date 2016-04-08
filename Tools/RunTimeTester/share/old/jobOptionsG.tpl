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

ApplicationMgr.DLLs += { "TestAtlfast"};
//--------------------------------------------------------------
// Event related parameters
//--------------------------------------------------------------
// Number of events to be processed (default is 10)
ApplicationMgr.EvtMax = 1000;

// Execution of algorithms given in sequencer
//
ApplicationMgr.TopAlg = {"Sequencer/TopSequence"};
TopSequence.Members = {"Sequencer/Generator","Sequencer/Atlfast"};

//GeneratorsTest
Atlfast.members += {"TestAtlfast::GeneratorsTest/SimpleTest1"};
SimpleTest1.writeRef                     = true;
SimpleTest1.outFile                      = "./outFile_Generators_!$FLAGS$!.txt";
SimpleTest1.refFile                      = "./refFile_Generators_!$FLAGS$!.txt";
SimpleTest1.cmpFile                      = "./cmpFile_Generators_!$FLAGS$!.txt";

Isajet.IsajetCommand = {"isalis !$FLAGS$!.lis"};


MessageSvc.OutputLevel               = 4;

//==============================================================
//
// End of job options file
//
//##############################################################










