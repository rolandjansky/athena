/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MYPACKAGE_MYAPP_H
#define MYPACKAGE_MYAPP_H
#ifndef ROOTCORE

#include "AthAnalysisBaseComps/AthAnalysisHelper.h" //tool creation and configuration


#include "POOLRootAccess/TEvent.h" //event looping
#include "GaudiKernel/ToolHandle.h" //for better working with tools

// Tool include(s):
#include "AsgTools/AnaToolHandle.h"

#include "AsgTools/MessageCheck.h" //messaging
using namespace asg::msgUserCode;  //messaging

// added for truth studies
#include "xAODJet/JetContainer.h"

#include "PATInterfaces/IWeightTool.h"
#include "../Root/PMGSherpa22VJetsWeightTool.cxx"
//#PMGSherpa22VJetsWeightTooll

//ROOT includes
#include "TString.h"
#include "TSystem.h"
#include "TH1F.h"
#include "TFile.h"

int main( int argc, char* argv[] ) {


  // for access to everything including EVNT
  IAppMgrUI* app = POOL::Init(); //important to do this first!
  // just for xAOD file type access - improved performance due to 
  // fast xAOD reading mechanism rather than generic POOL reading mechanism
  //IAppMgrUI* app = POOL::Init("POOLRootAccess/basixAOD.opts");

         
   ANA_MSG_INFO("Making the tool");

   // test the interface
   asg::AnaToolHandle< IWeightTool > pmgTool( "PMGSherpa22VJetsWeightTool" );
   ANA_CHECK( pmgTool.setProperty( "TruthJetContainer", "AntiKt4TruthWZJets" ) );  // default
   ANA_CHECK( pmgTool.initialize() ); 

   // can make the subtool directly or via this cast                                                                       
   ANA_MSG_INFO("Making the sub tool");
   //   PMGTools::PMGSherpa22VJetsWeightTool sherpatool;
   //sherpatool.initialize();        
   PMGTools::PMGSherpa22VJetsWeightTool* sherpatool = dynamic_cast<PMGTools::PMGSherpa22VJetsWeightTool*>(&*pmgTool);
   // PMGCrossSectionTool*        xsectool   = dynamic_cast<PMGCrossSectionTool*>(&*pmgXsecTool);

   // Open the input file:
   TString fileName = "$ASG_TEST_FILE_MC";
   if( argc < 2 ) {
      ANA_MSG_WARNING( "No file name received, using $ASG_TEST_FILE_MC" );
   } else {
      fileName = argv[1]; //use the user provided file
   }

   ANA_MSG_INFO("Opening file: " << gSystem->ExpandPathName(fileName.Data()) );


   //loop over input file with POOL 
   POOL::TEvent evt;
   evt.readFrom( fileName );

   ANA_MSG_INFO("Opening file: " << evt.getEntries() );

   // book some histograms
   TH1F* h_njetTruth = new TH1F("jetmult_AntiKt4TruthJets","jetmult_AntiKt4TruthJets",10,-0.5,9.5);
   TH1F* h_njetTruthWZ = new TH1F("jetmult_AntiKt4TruthWZJets","jetmult_AntiKt4TruthWZJets",10,-0.5,9.5);

   //
   TH1F* h_njetTruth_Tool = new TH1F("jetmult_AntiKt4TruthJets_Tool","jetmult_AntiKt4TruthJets_Tool",10,-0.5,9.5);
   TH1F* h_njetTruthWZ_Tool = new TH1F("jetmult_AntiKt4TruthWZJets_Tool","jetmult_AntiKt4TruthWZJets_Tool",10,-0.5,9.5);


   TFile* histfile=TFile::Open("hists.root","RECREATE");

   bool debug=false;

   for(int i=0;i < evt.getEntries(); i++) {
   //   for(int i=0;i < 200; i++) {
      if( evt.getEntry(i) < 0) { ANA_MSG_ERROR("Failed to read event " << i); continue; }

      const xAOD::JetContainer* truthWZJets=0;
      evt.retrieve(truthWZJets, "AntiKt4TruthWZJets" );

      int nTruthWZJets20 = 0;
      int nTruthWZJets30 = 0;
      //loop over all AntiKtTruthWZ jets
      for (unsigned i=0; i!=truthWZJets->size(); i++) {
	const xAOD::Jet* truthjet = (*truthWZJets)[i];
	//if(m_debug) {std::cout<<"Index "<<i<<" truthjet pointer = "<<truthjet<<std::endl;}
	double Pt=truthjet->pt();
	double AbsEta=fabs(truthjet->eta());
	if (Pt>20000.0 && AbsEta<4.5) {nTruthWZJets20++;}
	if (Pt>30000.0 && AbsEta<4.5) {nTruthWZJets30++;}
      }//end loop over truth jets
      if (debug) {
	std::cout<<"nTruthWZJets20="<<nTruthWZJets20<<std::endl;
	std::cout<<"nTruthWZJets30="<<nTruthWZJets30<<std::endl;
      }
      h_njetTruthWZ->Fill(nTruthWZJets20);

      // same for AntiKt4TruthJets
      const xAOD::JetContainer* truthJets=0;
      evt.retrieve(truthJets, "AntiKt4TruthJets" );

      int nTruthJets20 = 0;
      //loop over all truth jets
      for (unsigned i=0; i!=truthJets->size(); i++) {
	const xAOD::Jet* truthjet = (*truthJets)[i];
	double Pt=truthjet->pt();
	double AbsEta=fabs(truthjet->eta());
	if (Pt>20000.0 && AbsEta<4.5) {nTruthJets20++;}
      }//end loop over truth jets
      h_njetTruth->Fill(nTruthJets20);

      // Now use the PMG tool directly over AntiKt4TruthWZJets
      int njet=sherpatool->getSherpa22VJets_NJet("AntiKt4TruthWZJets"); 

      if(debug)
	std::cout<<"njets TruthWZJets "<< njet << std::endl;

      h_njetTruthWZ_Tool->Fill(njet);

      double reweight(0);
 
      int nuse=njet;
      reweight=sherpatool->getSherpa22VJets_NJetCorrection(nuse) ;
      if (debug)
	std::cout<<"correction with njet input(check) "<< nuse << " " << reweight  << std::endl;

      // check
      reweight=sherpatool->getSherpa22VJets_NJetCorrection("AntiKt4TruthWZJets"); 
      if (debug)
	std::cout<<"correction with string "<< reweight << std::endl;


      // and do again for TruthJets
      njet=sherpatool->getSherpa22VJets_NJet("AntiKt4TruthJets"); 
      if (debug)
	std::cout<<"njets (TruthJets) "<< njet << std::endl;

      h_njetTruth_Tool->Fill(njet);

      nuse=njet;
      reweight=sherpatool->getSherpa22VJets_NJetCorrection(nuse) ;
      if(debug)
	std::cout<<"correction with njet input (TruthJets) (check) "<< nuse << " " << reweight  << std::endl;

      // check
      reweight=sherpatool->getSherpa22VJets_NJetCorrection("AntiKt4TruthJets"); 
      if(debug)
	std::cout<<"correction with string (TruthJets) "<< reweight << std::endl;

      // and test the interface itself
      reweight=sherpatool->getWeight(); 
      if(debug)
	std::cout<<"correction from Interface" << reweight << std::endl;

   }

   app->finalize(); //trigger finalization of all services and tools created by the Gaudi Application

   histfile->cd();
   h_njetTruthWZ->Write(); 
   h_njetTruthWZ_Tool->Write(); 
   h_njetTruth->Write(); 
   h_njetTruth_Tool->Write(); 
   histfile->Close();

   return 0;
}

#endif // ROOTCORE  
#endif //> !MYPACKAGE_MYAPP_H
