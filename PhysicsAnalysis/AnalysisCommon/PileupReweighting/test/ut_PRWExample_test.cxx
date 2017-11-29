/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTree.h"
#include "PileupReweighting/TPileupReweighting.h"

void generateTestFiles() {
   //create a 'dummy' lumicalc file, with only a few runs, with a few lumiblocks 
   TFile l("dummy.None.lumicalc.root","RECREATE");
   TTree *t = new TTree("LumiMetaData","LumiMetaData");
   UInt_t runNbr=0;Float_t ps1=1;Float_t ps2=1; Float_t ps3=1;UInt_t lbn=0;Float_t intLumi=0;Float_t mu=0;
   t->Branch("RunNbr",&runNbr);t->Branch("L1Presc",&ps1);t->Branch("L2Presc",&ps2);t->Branch("L3Presc",&ps3);t->Branch("LBStart",&lbn);t->Branch("IntLumi",&intLumi);t->Branch("AvergeInteractionPerXing",&mu);

   runNbr=1;lbn=1;intLumi=5;mu=0.5;t->Fill();
   runNbr=1;lbn=2;intLumi=10;mu=1.5;t->Fill();
   runNbr=2;lbn=1;intLumi=3;mu=1.5;t->Fill();
   runNbr=3;lbn=1;intLumi=7;mu=1.5;t->Fill();
   runNbr=3;lbn=2;intLumi=2;mu=2.5;t->Fill(); //will be unrepresented data in the setup in channel 2000
 
   t->Write();delete t;
   l.Close();

   CP::TPileupReweighting g;
   g.AddPeriod(100,1,2);
   g.AddPeriod(101,3,3);
   g.SetUniformBinning(10,0,10);
   g.Initialize();

   g.Fill(100,2002,1,0.5);
   g.Fill(100,2002,1,1.5);
   g.Fill(101,2002,1,1.5);
   g.Fill(101,2002,1,2.5);
   g.Fill(101,2002,1,3.5);

   g.WriteToFile("dummy1.prw.root");

}

void testValue(double v1, double v2) {
   if( (!v1 && v2) || (!v2 && v1) ) throw std::runtime_error("zero-non-zero mismatch");
   if( fabs(v1-v2)/v1 > 0.001 ) throw std::runtime_error("mismatch > 0.1% in values");
}

//example program for how to use PRW (can be used in standalone executables or athena)

#ifdef XAOD_ANALYSIS

#include "AsgTools/AnaToolHandle.h"
#include "AsgAnalysisInterfaces/IPileupReweightingTool.h"

#include "TROOT.h"


using namespace asg::msgUserCode;

int main() {
  ANA_CHECK_SET_TYPE (int); //makes ANA_CHECK return ints if exiting function

   generateTestFiles();
   xAOD::EventInfo evt;
   evt.makePrivateStore();
   evt.setMCChannelNumber(2002);evt.setRunNumber(100);evt.setAverageInteractionsPerCrossing(1.6);

   //for the example we have to generate a test file
   asg::AnaToolHandle<CP::IPileupReweightingTool> prwTool("CP::PileupReweightingTool/prw");
   ANA_CHECK( prwTool.setProperty( "ConfigFiles", std::vector<std::string>({"dummy1.prw.root"}) ) );
   ANA_CHECK( prwTool.setProperty( "LumiCalcFiles",  std::vector<std::string>({"dummy.None.lumicalc.root"}) ) );
   ANA_CHECK( prwTool.setProperty( "UseMultiPeriods",true) ); //channel 2000 has periods 100 and 101
   ANA_CHECK( prwTool.initialize() );

   ///This is an example of retrieving the pileup weight
   float prwWeight = prwTool->getCombinedWeight( evt );


   testValue(prwWeight, 2.407); //this is just to check the result


   return 0;
}

#else //AnaToolHandle not yet in AtlasDerivation

int main() {
  return 0;
}

#endif
