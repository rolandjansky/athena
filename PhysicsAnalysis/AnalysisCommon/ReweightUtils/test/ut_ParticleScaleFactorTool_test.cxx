/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "AsgTools/AnaToolHandle.h"
#include "AsgAnalysisInterfaces/IObservableTool.h"
#include "PATInterfaces/ISystematicsTool.h"

#include "xAODEgamma/Electron.h"
#include "CxxUtils/ubsan_suppress.h"

#include "TFile.h"
#include "TH1D.h"
#include "TInterpreter.h"

using namespace asg::msgUserCode;

int main() {
   CxxUtils::ubsan_suppress ([]() { TInterpreter::Instance(); });
   ANA_CHECK_SET_TYPE (int); //makes ANA_CHECK return ints if exiting function

   //make test files
   TFile f1("test.root","RECREATE");
   TH1D* h = new TH1D("h","Electron",10,0,10);
   h->GetXaxis()->SetTitle("blah");
   h->Sumw2();h->SetBinContent(5,3);h->Write();
   f1.Close();

   TFile f2("test2.root","RECREATE");
   h = new TH1D("h","Electron",10,0,100);
   h->GetXaxis()->SetTitle("pt / GeV");
   h->Sumw2();h->SetBinContent(5,3);
   h->SetBinContent(3,4);
   h->Write();
   f2.Close();

   TFile f3("test3.root","RECREATE");
   h = new TH1D("h","Electron",10,0,100);
   h->GetXaxis()->SetTitle("pt / GeV");
   h->Sumw2();h->SetBinContent(5,3);
   h->SetBinContent(3,4);
   h->Write();
   h = new TH1D("h2","Electron",2,-5,5);
   h->GetXaxis()->SetTitle("eta");
   h->Sumw2();h->SetBinContent(2,5);
   h->SetBinContent(1,6);
   h->Write();
   f3.Close();

   //example systematic
   //the nominal value is from the main hist
   //the systematic variation values are in the variation hist. 
   //   +1sigma is taken as content+error
   //   -1sigma from content-error
   //by choosing content and error appropriately, you can do asymmetric errors this way

   TFile f4("test4.root","RECREATE");
   h = new TH1D("h","Electron",10,0,100);
   h->GetXaxis()->SetTitle("pt / GeV");
   h->Sumw2();h->SetBinContent(3,4); //nominal=4
   h->Write();
   f4.mkdir("mySyst__1up");
   f4.cd("mySyst__1up");
   h = new TH1D("h","Electron",10,0,100);
   h->GetXaxis()->SetTitle("pt / GeV");
   h->Sumw2();h->SetBinContent(3,5);//up 1sigma = 5;
   h->Write();
   /*f4.cd();
   f4.mkdir("mySyst__1down");
   f4.cd("mySyst__1down");
   h = new TH1D("h","Electron",10,0,100);
   h->GetXaxis()->SetTitle("pt / GeV");
   h->Sumw2();h->SetBinContent(3,1);//down 1sigma = 1;
   h->Write();*/
   f4.Close();

   // Verbose level setting:
#ifdef XAOD_STANDALONE
   static const MSG::Level verboseLevel = MSG::INFO;
#else
   static const int verboseLevel = 3;
#endif // XAOD_STANDALONE

   asg::AnaToolHandle<IObservableTool> myTool("ParticleScaleFactorTool/tool1");
   ANA_CHECK( myTool.setProperty( "File", "test.root" ) );
   ANA_CHECK( myTool.setProperty( "OutputLevel", verboseLevel ) );
   ANA_CHECK( myTool.initialize() );

   asg::AnaToolHandle<IObservableTool> myTool2("ParticleScaleFactorTool/tool2");
   ANA_CHECK( myTool2.setProperty( "File", "test2.root" ) );
   ANA_CHECK( myTool2.setProperty( "OutputLevel", verboseLevel ) ); //debugging
   ANA_CHECK( myTool2.initialize() );

   asg::AnaToolHandle<IObservableTool> myTool3("ParticleScaleFactorTool/tool3");
   ANA_CHECK( myTool3.setProperty( "File", "test3.root" ) );
   ANA_CHECK( myTool3.setProperty( "OutputLevel", verboseLevel ) ); //debugging
   ANA_CHECK( myTool3.initialize() );

   asg::AnaToolHandle<IObservableTool> myTool4("ParticleScaleFactorTool/tool4");
   ANA_CHECK( myTool4.setProperty( "File", "test4.root" ) );
   ANA_CHECK( myTool4.setProperty( "OutputLevel", verboseLevel ) ); //debugging
   ANA_CHECK( myTool4.initialize() );

   //a test electron
   xAOD::Electron* e = new xAOD::Electron;
   e->makePrivateStore();
   e->setP4(25000.,2,3,4000.);
   e->auxdata<double>("blah") = 4;

   std::cout << myTool->evaluate(e) << std::endl; //should print 3.000
   std::cout << myTool2->evaluate(e) << std::endl; //should print 4.000
   std::cout << myTool3->evaluate(e) << std::endl; //should print 4.444
   std::cout << myTool4->evaluate(e) << std::endl; //should print 4.000

   CP::SystematicSet s; s.insert(CP::SystematicVariation("mySyst",1));

   dynamic_cast<CP::ISystematicsTool*>(&*myTool4)->applySystematicVariation(s).ignore();
   std::cout << myTool4->evaluate(e) << std::endl; //should print 5.0

   s.clear(); s.insert(CP::SystematicVariation("mySyst",-1));

   dynamic_cast<CP::ISystematicsTool*>(&*myTool4)->applySystematicVariation(s).ignore();
   std::cout << myTool4->evaluate(e) << std::endl; //should print 1.0

   return 0; //zero = success
}
