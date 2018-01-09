/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "PileupReweighting/PileupReweightingTool.h"
#include "TTree.h"

void testValue(double v1, double v2) {
   if( (!v1 && v2) || (!v2 && v1) ) throw std::runtime_error("zero-non-zero mismatch");
   if( fabs(v1-v2)/v1 > 0.001 ) throw std::runtime_error("mismatch > 0.1% in values");
}


int main() {
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

   //make some trigger lumicalcs... only the prescale numbers are important!
   TFile l2("dummy.TriggerA.lumicalc.root","RECREATE");
   t = new TTree("LumiMetaData","LumiMetaData");
   t->Branch("RunNbr",&runNbr);t->Branch("L1Presc",&ps1);t->Branch("L2Presc",&ps2);t->Branch("L3Presc",&ps3);t->Branch("LBStart",&lbn);t->Branch("IntLumi",&intLumi);t->Branch("AvergeInteractionPerXing",&mu);

   runNbr=1;lbn=1;ps1=2;mu=0.5;t->Fill();
   runNbr=1;lbn=2;ps1=3;mu=1.5;t->Fill();
   runNbr=2;lbn=1;ps1=3;mu=1.5;t->Fill();
   runNbr=3;lbn=1;ps1=4;mu=1.5;t->Fill();
   runNbr=3;lbn=2;ps1=2;mu=2.5;t->Fill(); 
 
   t->Write();delete t;
   l2.Close();

   TFile l3("dummy.TriggerB.lumicalc.root","RECREATE");
   t = new TTree("LumiMetaData","LumiMetaData");
   t->Branch("RunNbr",&runNbr);t->Branch("L1Presc",&ps1);t->Branch("L2Presc",&ps2);t->Branch("L3Presc",&ps3);t->Branch("LBStart",&lbn);t->Branch("IntLumi",&intLumi);t->Branch("AvergeInteractionPerXing",&mu);

   runNbr=1;lbn=1;ps1=4;mu=0.5;t->Fill();
   runNbr=1;lbn=2;ps1=6;mu=1.5;t->Fill();
   runNbr=2;lbn=1;ps1=6;mu=1.5;t->Fill();
   runNbr=3;lbn=1;ps1=8;mu=1.5;t->Fill();
   runNbr=3;lbn=2;ps1=4;mu=2.5;t->Fill(); 
 
   t->Write();delete t;
   l3.Close();

   //now create a config file ... this test config file generating 

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

   CP::TPileupReweighting* genConfig = new CP::TPileupReweighting("genConfig");
   genConfig->AddPeriod(100,1,2);
   genConfig->AddPeriod(101,3,3);
   genConfig->SetUniformBinning(10,0,10);
   genConfig->Initialize();

   genConfig->Fill(100,2000/*channel number*/,1/*evt_weight*/,0.5);
   genConfig->Fill(100,2000,1,1.5);
   genConfig->Fill(101,2000,1,1.5);genConfig->Fill(101,2000,1,1.5);genConfig->Fill(101,2000,1,1.5); //three times stats in this bin 

   genConfig->Fill(100,2001,0.5,1.5);
   genConfig->Fill(101,2001,2,1.5);
   genConfig->Fill(101,2001,1,2.5); 

   genConfig->WriteToFile("dummy2.prw.root");


   std::vector<std::string> configFiles = {"dummy2.prw.root"};
   std::vector<std::string> lumicalcFiles = {"dummy.None.lumicalc.root"};

   //check multi period failure
   CP::IPileupReweightingTool* prw_bad = new CP::PileupReweightingTool("prw_bad");
   asg::setProperty(prw_bad,"ConfigFiles",configFiles);
   asg::setProperty(prw_bad,"LumiCalcFiles",lumicalcFiles);
   
   try {
    prw_bad->initialize();
   } catch(const std::runtime_error& e) {
    std::cout << "correctly caught:" << e.what() << std::endl;
   }
   
   

   //repeat with action=3 ... 
   CP::IPileupReweightingTool* prw = new CP::PileupReweightingTool("prw");
   asg::setProperty(prw,"ConfigFiles",configFiles);
   asg::setProperty(prw,"LumiCalcFiles",lumicalcFiles);
   asg::setProperty(prw,"UseMultiPeriods",true); //channel 2000 has periods 100 and 101
   prw->initialize();



   prw->expert()->SetUnrepresentedDataAction(3,1);

   std::cout << "prw channel2000 combined weights: " << prw->expert()->GetCombinedWeight(100,2000,0.5) << " (expected 0.0) " << prw->expert()->GetCombinedWeight(100,2000,1.5) << " (expected 3.33333) " << prw->expert()->GetCombinedWeight(101,2000,1.5) << " (expected=0.555555) " << std::endl;
   std::cout << "prw channel2000 'sum of weights': " << prw->expert()->GetCombinedWeight(100,2000,0.5)+prw->expert()->GetCombinedWeight(100,2000,1.5)+3*prw->expert()->GetCombinedWeight(101,2000,1.5) << " from 5 events (sum of evtweights=5)" << std::endl;

   std::cout << "prw channel2001 combined weights: " << prw->expert()->GetCombinedWeight(100,2001,1.5) << " (expected 4.66666) " << prw->expert()->GetCombinedWeight(101,2001,1.5) << " (expected 0.58333) " << prw->expert()->GetCombinedWeight(101,2001,2.5) << " (expected=0.259259) " << std::endl;
   std::cout << "prw channel2001 'sum of weights': " << 0.5*prw->expert()->GetCombinedWeight(100,2001,1.5)+2.*prw->expert()->GetCombinedWeight(101,2001,1.5)+1.*prw->expert()->GetCombinedWeight(101,2001,2.5) << " from 3 events (of various weights, sum of evtweights=3.5)" << std::endl;




   CP::IPileupReweightingTool* prw1 = new CP::PileupReweightingTool("prw1");
   std::vector<std::string> configFiles1 = {"dummy1.prw.root"};
   std::vector<std::string> lumicalcFiles1 = {"dummy.None.lumicalc.root","dummy.TriggerA.lumicalc.root:TriggerA","dummy.TriggerB.lumicalc.root:TriggerB"};
   asg::setProperty(prw1, "ConfigFiles",configFiles1);
   asg::setProperty(prw1, "LumiCalcFiles",lumicalcFiles1);
   asg::setProperty(prw1,"UseMultiPeriods",true); //channel 2000 has periods 100 and 101
   prw1->initialize();

   std::cout << "prw1 Integrated lumi = " << prw1->GetIntegratedLumi() << " (expected=2.7e-5) " << std::endl;  testValue(prw1->GetIntegratedLumi(),2.7e-5);
   std::cout << "prw1 periodWeights : " << prw1->expert()->GetPeriodWeight(100,2002) << " (expected=1.6666) " << prw1->expert()->GetPeriodWeight(101,2002) << " (expected=0.5555)" << std::endl; testValue(prw1->expert()->GetPeriodWeight(100,2002),1.6666);
   std::cout << "prw1 primaryWeights : " << prw1->expert()->GetPrimaryWeight(100,2002,0.5) << " (expected=0.5555) " << prw1->expert()->GetPrimaryWeight(100,2002,1.5) << " (expected=1.4444) " << prw1->expert()->GetPrimaryWeight(101,2002,0.5) << " (expected=inf .. no MC) " <<  prw1->expert()->GetPrimaryWeight(101,2002,1.5) << " (expected=2.3333) " << prw1->expert()->GetPrimaryWeight(101,2002,2.5) << " (expected=0.6666) " << prw1->expert()->GetPrimaryWeight(101,2002,3.5) << " (expected=0.) " << std::endl;
   testValue(prw1->expert()->GetPrimaryWeight(100,2002,0.5),0.5555);
   testValue(prw1->expert()->GetPrimaryWeight(100,2002,1.5),1.4444);
   testValue(prw1->expert()->GetPrimaryWeight(101,2002,1.5),2.3333);
   testValue(prw1->expert()->GetPrimaryWeight(101,2002,2.5),0.6666);
   testValue(prw1->expert()->GetPrimaryWeight(101,2002,3.5),0);

   //test the random run number and lumiblock number generating 
   int num1(0); int lnum1(0);
   for(int i=0;i<1000000;i++) {
      if(prw1->expert()->GetRandomRunNumber(100)==1) { //returns 1 or 2 in this test case .. should give 1 about 15/18ths of the time
         num1++; 
         if(prw1->expert()->GetRandomLumiBlockNumber(1)==1) lnum1++;
      }
   }
   std::cout << "prw1 fraction of run1 = " << float(num1)/1000000 << " (expected=0.833333) " << " fraction of these with lbn1 = " << float(lnum1)/num1 << " (expected=0.333333) " << std::endl;
   testValue(float(num1)/1000000,0.8333333);
   testValue(float(lnum1)/num1,0.333333);

   //test the DataWeights functionality .. first test the IntegratedLumi ...
   std::cout << "prw1 TriggerA lumi = " << prw1->expert()->GetIntegratedLumi("TriggerA") << " (expected=9.58333e-6) " << std::endl; testValue(prw1->expert()->GetIntegratedLumi("TriggerA"),9.58333e-6);
   std::cout << "prw1 TriggerB lumi = " << prw1->expert()->GetIntegratedLumi("TriggerB") << " (expected=4.79166e-6) " << std::endl; testValue(prw1->expert()->GetIntegratedLumi("TriggerB"),4.79166e-6);
   std::cout << "prw1 TriggerA&&TriggerB lumi = " << prw1->expert()->GetIntegratedLumi("TriggerA&&TriggerB") << " (expected=1.81597e-6) " << std::endl;testValue(prw1->expert()->GetIntegratedLumi("TriggerA&&TriggerB"),1.81597e-6);

   //now check the dataweight .. remember the runNumber is converted into a periodNumber (i.e. the weights aren't unique to a runNumber, they are unique to the periodNumber)
   std::cout << "prw1 TriggerA DataWeights (mu independent): " << prw1->expert()->GetDataWeight(1,"TriggerA") << " (expected=2.63415) " << prw1->expert()->GetDataWeight(2,"TriggerA") << " (expected=2.63415) " << prw1->expert()->GetDataWeight(3,"TriggerA") << " (expected=3.2727)" << std::endl;
   std::cout << "prw1 TriggerA DataWeights (mu dependent): " << prw1->expert()->GetDataWeight(1,"TriggerA",0.5) << " (expected=2) " << prw1->expert()->GetDataWeight(1,"TriggerA",1.5) << " (expected=3) " << prw1->expert()->GetDataWeight(2,"TriggerA",1.5) << " (expected=3.0000) " << prw1->expert()->GetDataWeight(3,"TriggerA",1.5) << " (expected=4) " << prw1->expert()->GetDataWeight(3,"TriggerA",2.5) << " (expected=2) " << std::endl;
   
   try {
    prw1->expert()->GetDataWeight(3,"TriggerA",3.5);
   } catch(...) {
    std::cout << "correctly threw exception for out of range mu" << std::endl;
   }
   
   testValue(prw1->expert()->GetDataWeight(1,"TriggerA"),2.63415);
   testValue(prw1->expert()->GetDataWeight(2,"TriggerA"),2.63415);
   testValue(prw1->expert()->GetDataWeight(3,"TriggerA"),3.2727);
   testValue(prw1->expert()->GetDataWeight(1,"TriggerA",0.5),2.);
   testValue(prw1->expert()->GetDataWeight(2,"TriggerA",1.5),3.);
   testValue(prw1->expert()->GetDataWeight(3,"TriggerA",1.5),4);
   testValue(prw1->expert()->GetDataWeight(3,"TriggerA",2.5),2);
   //testValue(prw1->expert()->GetDataWeight(3,"TriggerA",3.5),nan);


   //CP::TPileupReweighting t;

   //t.AddLumiCalcFile("/usera/will/testareas/ZZ8TeV/ZZAnalysis/share/GRL_v61.None.lumicalc.root");




   return 0;
}
