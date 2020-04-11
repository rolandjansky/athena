/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/* ***********************************************************************************\
 *                                                                      	     *
 *      Name: FFJetSmearingTool_MyExample                                     	     *
 *      Purpose: Example code for using the FFJetSmearingTool                  	     *
 *                                                                      	     *
\*************************************************************************************/

//In order to simplify the usage of this example we recomend to set up a run directory as shown in:
// https://gitlab.cern.ch/alprades/ffjetsmearingtool/tree/master/FFJetSmearingTool
// If a user want to use the FFJetSmearingTool, he has to calibrate and tagg his jets.
// This will be different for each  ntuple and is not easy to generalise so, to make the
// example simple, plese, copy the run directoy.

//For more information about the tool, please, check TWiki of the tool: https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/FFJetSmearingTool

// System include(s):
#include <memory>

#include "AsgTools/ToolHandle.h"
#include "AsgTools/AsgTool.h"

// ROOT
#include "TFile.h"
#include "TCanvas.h"


//xAOD EDM classes
#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"

#include "xAODEventShape/EventShape.h"
#include "xAODRootAccess/tools/Message.h"
#include "xAODRootAccess/tools/ReturnCheck.h"
#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"
#include "xAODCore/ShallowCopy.h"

#include "JetCalibTools/JetCalibrationTool.h"

//Other includes
#include "TH1F.h"
#include "TTree.h"


//truh label
//#include "BoostedJetTaggers/SmoothedWZTagger.h"

#include "ParticleJetTools/JetTruthLabelingTool.h"

//My includes
#include "JetUncertainties/FFJetSmearingTool.h"

// Error checking macro
#define CHECK( ARG )                                 \
  do {                                               \
    const bool result = ARG;                         \
    if(!result) {                                    \
      ::Error(APP_NAME, "Failed to execute: \"%s\"", \
              #ARG );                                \
      return 1;                                      \
    }                                                \
  } while( false )



// Help message if the  --help option is given by the user
void usage() {
  std::cout << "Running options:" << std::endl;
  std::cout << "YOU HAVE TO ADAPT THE OPTIONS TO FFJETSMEARINGCORRECTION" << std::endl;
  std::cout << "        --help : To get the help you're reading" << std::endl;
  std::cout << "        --jetColl= : Specify the jet collection (TrimmedLargeR)" << std::endl;
  std::cout << "        --MassDef= : Specify the kind of jet mass used (CALO, TA, Comb)" << std::endl;
  std::cout << "        --sample= : Specify input xAOD" << std::endl;
  std::cout << "        Example: FFJetSmearingTool_MyExample  --truth_jetColl=AntiKt10TruthTrimmedPtFrac5SmallR20Jets --reco_jetColl=AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets --MassDef=Comb  --sample=DAOD_JETM6.16317590._000003.pool.root.1  --output=file_name.root --ConfigFile=rel21/Spring2020/FFJetSmearingTool_TestOnly_JMS_JMR.config   --DebugTool=true" << std::endl;
}





//---------------
// Main Function
//---------------

int main(int argc, char* argv[]){






//Reading user instructions of the example ***********************************************************************************************


  //---------------------------------------------
  // Declaring input variables with default values
  //---------------------------------------------
  std::string sample = "";
  std::string truth_jetColl = "";
  std::string reco_jetColl = "";
  std::string string_kindofmass = "";
  std::string output = "";
  std::string string_configfile_path = "";
  std::string string_debugtool = "";
  //---------------------------
  // Decoding the user settings
  //---------------------------

  for (int i=1; i< argc; i++){

    std::string opt(argv[i]); std::vector< std::string > v;

    std::istringstream iss(opt);

    std::string item;
    char delim = '=';

    while (std::getline(iss, item, delim)){
        v.push_back(item);
    }

    if ( opt.find("--help") != std::string::npos ) {
      usage(); return 0;
    }

    if ( opt.find("--sample=")   != std::string::npos ) sample = v[1];

    if ( opt.find("--truth_jetColl=")   != std::string::npos ) truth_jetColl = v[1];

    if ( opt.find("--reco_jetColl=")   != std::string::npos ) reco_jetColl = v[1];

    if ( opt.find("--MassDef=")   != std::string::npos ) string_kindofmass = v[1];

    if ( opt.find("--ConfigFile=")   != std::string::npos ) string_configfile_path = v[1];

    if ( opt.find("--DebugTool=")   != std::string::npos ) string_debugtool = v[1];

    if ( opt.find("--output=")   != std::string::npos ) output = v[1];
  }//End: Loop over input options


  if(sample==""){
    std::cout << "No input xAOD file specified, exiting" << std::endl;
    return 1;
  }
  if(truth_jetColl==""){
    std::cout << "No truth jet collection specified, exiting" << std::endl;
    return 1;
  }
  if(reco_jetColl==""){
    std::cout << "No truth jet collection specified, exiting" << std::endl;
    return 1;
  }
  if(string_kindofmass==""){
    std::cout << "No kind of jet mass specified, exiting" << std::endl;
    return 1;
  }
  if(string_configfile_path==""){
    std::cout << "No ConfigFile specified, exiting" << std::endl;
    return 1;
  }
  if(string_debugtool==""){
    std::cout << "No debugtool specified, exiting" << std::endl;
    return 1;
  }
  if(output==""){
    std::cout << "output not specified, exiting" << std::endl;
    return 1;
  }
std::cout << sample << " " << truth_jetColl << " " << reco_jetColl  << " " << output << " " << string_debugtool  << std::endl;



TString kindofmass = string_kindofmass;

bool want_to_debug = false;

if(string_debugtool == "true"){want_to_debug = true;}
if(string_debugtool == "false"){want_to_debug = false;}

//*************************************************************************************************************************************



  // Set up the job for xAOD access:
  static const char* APP_NAME = "FFJetSmearingTool_Example";



 
  //--------------------
  // Opening input file
  //--------------------

  std::unique_ptr< TFile > ifile( TFile::Open( sample.c_str(), "READ" ) );





  // Create a TEvent object
  xAOD::TEvent event(xAOD::TEvent::kClassAccess);

  RETURN_CHECK( APP_NAME, event.readFrom( ifile.get() ) );

  


  //----------------------------------
  // Initialization of FFJetSmearingTool
  //----------------------------------
        
  const std::string name_FFJetSmearingTool = "FFJetSmearing_Example";
  // Call the constructor
  FFJetSmearingTool ffjetsmearingtool(name_FFJetSmearingTool.c_str());

  (ffjetsmearingtool.setProperty( "MassDef", kindofmass )).ignore();

  (ffjetsmearingtool.setProperty( "ConfigFile", string_configfile_path  )).ignore();

  // Initialize the tool
  if(!(ffjetsmearingtool.initialize().isSuccess())){
     std::cout << "Initialization of FFJetSmearingTool failed, exiting" << std::endl;
     return 0;
  }



//Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL, 7=ALWAYS)

ffjetsmearingtool.setProperty ("OutputLevel", MSG::FATAL);

if(want_to_debug==true){
ffjetsmearingtool.setProperty ("OutputLevel", MSG::VERBOSE);
}
//***************************************************************** TESTINGGGG**********************
   // Print the recommended systematics

const CP::SystematicSet& recommendedSysts = ffjetsmearingtool.recommendedSystematics();//take the systematics of the FFJETSmearing Tool
   Info( APP_NAME, "Recommended systematics:" );
   for(auto sysItr = recommendedSysts.begin();
       sysItr != recommendedSysts.end(); ++sysItr){
      Info( APP_NAME, "  %s", sysItr->name().c_str() );
   }


std::vector<CP::SystematicSet> sysList;







//---------------------------------------------------------
//////Initialize the tool to set the truth tagging
//           std::unique_ptr<JSSTaggerBase> m_TaggerForJES;
//
//  m_TaggerForJES = nullptr;

//m_TaggerForJES = std::unique_ptr<SmoothedWZTagger>( new SmoothedWZTagger( "TaggerTruthLabelling" ) );

//m_TaggerForJES->setProperty("CalibArea",  "SmoothedWZTaggers/Rel21");

//m_TaggerForJES->setProperty("ConfigFile",  "SmoothedContainedWTagger_AntiKt10LCTopoTrimmed_FixedSignalEfficiency50_MC16d_20190410.dat");

//m_TaggerForJES->setProperty("DSID", 304308/*m_config->getDSID()*/);

//m_TaggerForJES->initialize();

//--------------------------------------------------------

  //---------------------------------------------------------
  //////Initialize the tool to set the truth tagging

JetTruthLabelingTool m_JetTruthLabelingTool("JetTruthLabelingTool");

  CHECK(m_JetTruthLabelingTool.setProperty("TruthLabelName",  "R10TruthLabel_R21Consolidated"));
  CHECK(m_JetTruthLabelingTool.setProperty("UseTRUTH3",  false)); // Set this to false only if you have the FULL !TruthParticles container in your input file
  CHECK(m_JetTruthLabelingTool.setProperty("TruthParticleContainerName",  "TruthParticles")); // Set this if you have the FULL !TruthParticles container but have named it something else
  CHECK(m_JetTruthLabelingTool.initialize());





  //--------------------
  // Calibrate jets (ir is necessary to extract the CALO and TA from a Combined jet). The user have to calibrate the jets before using the SmearingTool
  //--------------------
    const std::string name_JetCalibTools = "JetCalibTool_LC";

  // Call the constructor
  JetCalibrationTool jetCalibrationTool(name_JetCalibTools.c_str());

 jetCalibrationTool.setProperty("JetCollection", "AntiKt10LCTopoTrimmedPtFrac5SmallR20");

 if(kindofmass=="Comb"){ // To test like if the user has different jet mass definitions in his analysis
 jetCalibrationTool.setProperty("ConfigFile", "JES_MC16recommendation_FatJet_Trimmed_JMS_comb_17Oct2018.config");
 }
 if(kindofmass=="Calo"){
 jetCalibrationTool.setProperty("ConfigFile", "JES_MC16recommendation_FatJet_Trimmed_JMS_calo_12Oct2018.config");
 }
 if(kindofmass=="TA"){
 jetCalibrationTool.setProperty("ConfigFile", "JES_MC16recommendation_FatJet_Trimmed_JMS_TA_12Oct2018.config");
 }


 jetCalibrationTool.setProperty("CalibSequence", "EtaJES_JMS");
 jetCalibrationTool.setProperty("IsData", false);
 jetCalibrationTool.setProperty("CalibArea", "00-04-82");
 jetCalibrationTool.setProperty("DEVmode", false);


  if(!(jetCalibrationTool.initialize().isSuccess())){
    std::cout << "Initialization of JetCalibTools failed, exiting" << std::endl;
    return 0;
  }
//--------------------------------------------------------


for (auto sys : recommendedSysts) { sysList.push_back(CP::SystematicSet({sys})); }

       std::cout << "\n=============SYSTEMATICS CHECK NOW" << std::endl;
       for (auto sys : sysList)	 {
	 // Tell the calibration tool which variation to apply
	 if (ffjetsmearingtool.applySystematicVariation(sys) != CP::SystematicCode::Ok) {
	   Error(APP_NAME, "Cannot configure calibration tool for systematics");
	 }

	std::cout << "\nWe are using the systematic " << sys.name()  << std::endl;

	


//**************************************************************************************



  //----------------------------------
  // Define histograms
  //----------------------------------

    TH1F* reco_jet_mass_hist=new TH1F("reco_jet_mass_hist","reco_jet_mass_hist", 100, 0, 200);
    TH1F* matched_truth_jet_mass_hist=new TH1F("matched_truth_jet_mass_hist","matched_truth_jet_mass_hist", 100, 0, 200);
    TH1F* smeared_reco_jet_mass_hist=new TH1F("smeared_reco_jet_mass_hist","smeared_reco_jet_mass_hist", 100, 0, 200);

    reco_jet_mass_hist->Sumw2();
    matched_truth_jet_mass_hist->Sumw2();
    smeared_reco_jet_mass_hist->Sumw2();






	int upperlimit1 = 600;
	int upperlimit2 = 1000;

	int numBinsMass = 120;
	int numBinsPt = 100;

//numBinsMass = 12;
//numBinsPt = 10;

    TH3F* hist_jet_mass_scale_change_3D    = new TH3F ("hist_jet_mass_scale_change_3D","hist_jet_mass_scale_change_3D",numBinsPt,0,upperlimit2/*pt*/,numBinsMass,0,upperlimit1/*mass*/,numBinsMass,0,upperlimit1/*mass*/);


float lowerlimit3 = -0.5;
float upperlimit3 = 0.5;
int numBinsDiff = 100;



    TH3F* hist_jet_mass_resolution_change_3D    = new TH3F ("hist_jet_mass_resolution_change_3D","hist_jet_mass_resolution_change_3D",numBinsPt,0,upperlimit2/*pt*/,numBinsMass,0,upperlimit1/*mass*/,numBinsDiff,lowerlimit3,upperlimit3/*mass*/);



  //------------------
  // Loop over events
  //------------------

  // Get the number of events
  const Long64_t nevents = event.getEntries();

  // Decide how many events to run over
  //if(nevents>10000){nevents = 10000; }



  for(Long64_t ievent = 0;  ievent < nevents; ++ievent){

     // Load the event:
     if( event.getEntry( ievent ) < 0 ) {
        std::cerr << "Failed to load entry " << ievent << std::endl;
        return 1;
     }


    // Show status
    if(ievent % 100==0) std::cout << "Event " << ievent << " of " << nevents << std::endl;


    // Print some event information for fun
    const xAOD::EventInfo* ei = 0;
    CHECK( event.retrieve(ei, "EventInfo") );

if(want_to_debug==true){
    Info(APP_NAME,
         "===>>>  start processing event #%i, "
         "run #%i %i events processed so far  <<<===",
         static_cast<int>(ei->eventNumber()),
         static_cast<int>(ei->runNumber()),
         static_cast<int>(ievent));
}


    // Get the truth jets from the event
    const xAOD::JetContainer* jets_truth = 0;
    CHECK( event.retrieve(jets_truth, "AntiKt10TruthTrimmedPtFrac5SmallR20Jets") );
if(want_to_debug==true){
    Info(APP_NAME, "Number of truth jets: %i",
         static_cast<int>(jets_truth->size()));
}
    //Loop over the truth jets in the event
    xAOD::JetContainer::const_iterator jetItr;
    for(jetItr = jets_truth->begin(); jetItr != jets_truth->end(); ++jetItr){

      const xAOD::Jet* jet_truth = *jetItr;

if(want_to_debug==true){
      // Print basic info about this jet
      Info(APP_NAME, "Truth Jet: pt = %g, mass = %g, eta = %g", jet_truth->pt()/1000., jet_truth->m()/1000., jet_truth->eta());
}


    }



    // Get the reco jets from the event
    const xAOD::JetContainer* jets_reco = 0;
    CHECK( event.retrieve(jets_reco, reco_jetColl) ); 
 
if(want_to_debug==true){
    Info(APP_NAME, "Number of reco jets: %i",
         static_cast<int>(jets_reco->size()));
}
    //Loop over the reco jets in the event
    //xAOD::JetContainer::const_iterator jetItr;
    for(jetItr = jets_reco->begin(); jetItr != jets_reco->end(); ++jetItr){

      const xAOD::Jet* jet_reco = *jetItr;

if(want_to_debug==true){
      // Print basic info about this jet
      Info(APP_NAME, "Reco Jet: pt = %g, mass = %g, eta = %g", jet_reco->pt()/1000., jet_reco->m()/1000., jet_reco->eta());
}
      //Give a TruthLabel to the LargeRJets. ou will need it in the FFSmearingTool (you need to know the jet topology) (JetUncertainties also used this)
      //
//     CHECK( m_TaggerForJES->decorateTruthLabel(*jet_reco) );// , "Failed to do truth labeling for large-R jet" 

    }




      xAOD::Jet jet_truth_matched;
      jet_truth_matched.makePrivateStore();

      // Retrieve jet container
      const xAOD::JetContainer* jets = 0;
      RETURN_CHECK( APP_NAME, event.retrieve( jets, reco_jetColl ) );

      // Shallow copy 
      auto jets_shallowCopy = xAOD::shallowCopyContainer( *jets );

     m_JetTruthLabelingTool.modify(*(jets_shallowCopy.first));
	
    std::cout << "I am passed the modify function " << std::endl;

      bool lead_jet = true;//to fill the histogram

      // Iterate over the shallow copy
      for( xAOD::Jet* jet_reco : *( jets_shallowCopy.first ) ) {






	//Calibrate the jet. This step is necessary to extract the CALO and TA mass inside the tool later (if we are talking about a combined mass jet)
	jetCalibrationTool.applyCalibration( *jet_reco );		
//------------------------------------------------------------------------------------------------------------------------------------------//
//Test how the Calo and TA mass are set in the Combined mass.

	    	xAOD::JetFourMom_t jet_reco_Comb_FourMom;
    		xAOD::JetFourMom_t jet_reco_CALO_FourMom;
    		xAOD::JetFourMom_t jet_reco_TA_FourMom;

        	jet_reco->getAttribute<xAOD::JetFourMom_t>("JetJMSScaleMomentumCombQCD",jet_reco_Comb_FourMom);
        	jet_reco->getAttribute<xAOD::JetFourMom_t>("JetJMSScaleMomentumCalo",jet_reco_CALO_FourMom);
        	jet_reco->getAttribute<xAOD::JetFourMom_t>("JetJMSScaleMomentumTA",jet_reco_TA_FourMom);

if(want_to_debug==true && kindofmass == "Comb"){   
        	Info(APP_NAME,"Comb jet mass = %g", jet_reco_Comb_FourMom.mass() );
        	Info(APP_NAME,"CALO jet mass = %g", jet_reco_CALO_FourMom.mass() );
        	Info(APP_NAME, "TA jet mass = %g", jet_reco_TA_FourMom.mass() );
}

		xAOD::Jet* jet_reco_Comb = new xAOD::Jet(); //You have to initialize the jet object this way. If not, oyu will encounter breaks when running 
     		jet_reco_Comb->makePrivateStore(); //And after that we have to give it a private store toa void a brak when using setJetP4
            	jet_reco_Comb->setJetP4(jet_reco_Comb_FourMom);
                xAOD::Jet* jet_reco_CALO = new xAOD::Jet();
		jet_reco_CALO->makePrivateStore();
                jet_reco_CALO->setJetP4(jet_reco_CALO_FourMom);
                xAOD::Jet* jet_reco_TA = new xAOD::Jet();
		jet_reco_TA->makePrivateStore();
                jet_reco_TA->setJetP4(jet_reco_TA_FourMom);
//		jet_reco_Comb.setJetP4( xAOD::JetFourMom_t(jet_reco_Comb_FourMom.pt(), jet_reco_Comb_FourMom.eta(), jet_reco_Comb_FourMom.phi(), jet_reco_Comb_FourMom.mass() ) );

if(want_to_debug==true){
                Info(APP_NAME,"NEW Comb jet mass = %g", jet_reco_Comb->m() );
                Info(APP_NAME,"NEW CALO jet mass = %g", jet_reco_CALO->m() );
                Info(APP_NAME,"NEW TA jet mass = %g", jet_reco_TA->m() );
}


		delete jet_reco_Comb;
		delete jet_reco_CALO;
//		delete jet_reco_TA;

//------------------------------------------------------------------------------------------------------------------------------------------//



	      if(!(ffjetsmearingtool.getMatchedTruthJet(jet_reco, jet_truth_matched).isSuccess())){ 
	      //Warning(APP_NAME, "No truth jet match with this reco jet");
		   delete jet_reco_TA;
	           continue;            
	      }

      
				if(lead_jet == true){reco_jet_mass_hist->Fill(jet_reco->m()/1000.); matched_truth_jet_mass_hist->Fill(jet_truth_matched.m()/1000.);  }

if(kindofmass=="TA"){
//    	CHECK( m_TaggerForJES->decorateTruthLabel(*jet_reco_TA) );// , "Failed to do truth labeling for large-R jet" 

        Double_t aux_original_jet_mass = jet_reco->m()/1000.;

	ffjetsmearingtool.applyCorrection(jet_reco_TA);
        if(lead_jet == true && aux_original_jet_mass > 0){
		smeared_reco_jet_mass_hist->Fill(jet_reco_TA->m()/1000.); lead_jet=false;

		hist_jet_mass_scale_change_3D->Fill(jet_reco->pt()/1000., aux_original_jet_mass, (jet_reco_TA->m()/1000.)/*/(aux_original_jet_mass)*/);

		hist_jet_mass_resolution_change_3D->Fill(jet_reco->pt()/1000., aux_original_jet_mass, (jet_reco_TA->m()/1000.) - (aux_original_jet_mass));
	}


}
else{
	Double_t aux_original_jet_mass = jet_reco->m()/1000.;
 
	ffjetsmearingtool.applyCorrection(jet_reco);
       	if(lead_jet == true && aux_original_jet_mass > 0){
		smeared_reco_jet_mass_hist->Fill(jet_reco->m()/1000.); lead_jet=false; 

		hist_jet_mass_scale_change_3D->Fill(jet_reco->pt()/1000., aux_original_jet_mass, (jet_reco->m()/1000.)/*/(aux_original_jet_mass)*/);

                hist_jet_mass_resolution_change_3D->Fill(jet_reco->pt()/1000., aux_original_jet_mass, TMath::Abs((jet_reco->m()/1000.) - (aux_original_jet_mass))/ aux_original_jet_mass);

	}



}
delete jet_reco_TA;

      }
      delete jets_shallowCopy.first;
      delete jets_shallowCopy.second;


   




  }//END: Loop over events

TString output_path = "output/" + sys.name() + "_" + output  ;

TFile *jetmass_histograms = new TFile(output_path,"recreate"/*"UPDATE"*/);


        reco_jet_mass_hist->Write();
        matched_truth_jet_mass_hist->Write();
        smeared_reco_jet_mass_hist->Write();



//------------------------------------------------------------------------------------------------------------//
// 3D plots for testing the JetMassScale and JetMassResolution smearing for each systematic

    TH2F* hist_jet_mass_scale_change_2D = new TH2F("hist_jet_mass_scale_change_2D","hist_jet_mass_scale_change_2D",numBinsPt,0,upperlimit2,numBinsMass,0,upperlimit1);

    for (int i=1; i<=hist_jet_mass_scale_change_3D->GetNbinsX(); i++)
    {
      for (int j=1; j<= hist_jet_mass_scale_change_3D->GetNbinsY(); j++)
      {
        TH1F* hold = new TH1F("","",numBinsMass,0,upperlimit1);
        for (int k=1; k<= hist_jet_mass_scale_change_3D->GetNbinsZ(); k++)
        {
          hold->SetBinContent(k,hist_jet_mass_scale_change_3D->GetBinContent(i,j,k));                    //Es basicamente una proyeccion en 2D del TH3
        }
          hist_jet_mass_scale_change_2D->SetBinContent(i,j,hold->GetMean()/hist_jet_mass_scale_change_3D->GetYaxis()->GetBinCenter(j));
//                Info(APP_NAME,"meannnnn = %g", hold->GetMean() );
//                Info(APP_NAME,"binCenter = %g", hist_jet_mass_scale_change_3D->GetYaxis()->GetBinCenter(j) );
	delete hold;
      }
    }

    //hist_jet_mass_scale_change_2D->Smooth(1,"");

    TH2F* hist_jet_mass_resolution_change_2D = new TH2F("hist_jet_mass_resolution_change_2D","hist_jet_mass_resolution_change_2D",numBinsPt,0,upperlimit2,numBinsMass,0,upperlimit1);

    for (int i=1; i<=hist_jet_mass_resolution_change_3D->GetNbinsX(); i++)
    {
      for (int j=1; j<= hist_jet_mass_resolution_change_3D->GetNbinsY(); j++)
      {
        TH1F* hold = new TH1F("","",numBinsDiff,lowerlimit3,upperlimit3);
        for (int k=1; k<= hist_jet_mass_resolution_change_3D->GetNbinsZ(); k++)
        {
          hold->SetBinContent(k,hist_jet_mass_resolution_change_3D->GetBinContent(i,j,k));                    //Es basicamente una proyeccion en 2D del TH3
        }
          hist_jet_mass_resolution_change_2D->SetBinContent(i,j,hold->GetMean()/*/hist_jet_mass_resolution_change_3D->GetYaxis()->GetBinCenter(j)*/);
//                Info(APP_NAME,"meannnnn = %g", hold->GetMean() );
//                Info(APP_NAME,"binCenter = %g", hist_jet_mass_scale_change_3D->GetYaxis()->GetBinCenter(j) );
        delete hold;
      }
    }





    TCanvas *c1 = new TCanvas("c1","c1",500,500);


    hist_jet_mass_scale_change_2D->GetXaxis()->SetTitleOffset(1.5);
    hist_jet_mass_scale_change_2D->GetXaxis()->SetNdivisions(5);
    hist_jet_mass_scale_change_2D->GetYaxis()->SetTitleOffset(1.5);
    hist_jet_mass_scale_change_2D->GetZaxis()->SetTitleOffset(1.5);
    hist_jet_mass_scale_change_2D->GetYaxis()->SetTitle("Initial Reco Mass [GeV]");
    hist_jet_mass_scale_change_2D->GetXaxis()->SetTitle("Reco p_{T} [GeV]");
    hist_jet_mass_scale_change_2D->GetZaxis()->SetTitle("Average Mass smearing (Initial_reco_mass/smeared_reco_mass)");

    hist_jet_mass_scale_change_2D->GetZaxis()->SetRangeUser(0.95,1.05);

    //hist_jet_mass_scale_change_2D->GetXaxis()->SetRangeUser(pt_low_bound,pt_up_bound);
    hist_jet_mass_scale_change_2D->Draw("colz");

	TString output_path_scale_debug = "output/debug_plots/scale_variations/" + sys.name() + "_scaleDebug.pdf"  ;

    c1->Print(output_path_scale_debug);

    TCanvas *c2 = new TCanvas("c2","c2",500,500);

    hist_jet_mass_resolution_change_2D->GetXaxis()->SetTitleOffset(1.5);
    hist_jet_mass_resolution_change_2D->GetXaxis()->SetNdivisions(5);
    hist_jet_mass_resolution_change_2D->GetYaxis()->SetTitleOffset(1.5);
    hist_jet_mass_resolution_change_2D->GetZaxis()->SetTitleOffset(1.5);
    hist_jet_mass_resolution_change_2D->GetYaxis()->SetTitle("Initial Reco Mass [GeV]");
    hist_jet_mass_resolution_change_2D->GetXaxis()->SetTitle("Reco p_{T} [GeV]");
    hist_jet_mass_resolution_change_2D->GetZaxis()->SetTitle("Average Mass smearing (Initial_reco_mass/smeared_reco_mass)");

    hist_jet_mass_resolution_change_2D->GetZaxis()->SetRangeUser(-0.1,0.1);

    hist_jet_mass_resolution_change_2D->Draw("colz");

    TString output_path_resolution_debug = "output/debug_plots/resolution_variations/" + sys.name() + "_resolutionDebug.pdf"  ;

    c2->Print(output_path_resolution_debug);

delete hist_jet_mass_scale_change_3D;
delete hist_jet_mass_scale_change_2D;

delete hist_jet_mass_resolution_change_3D;
delete hist_jet_mass_resolution_change_2D;

delete c1;
delete c2;
//------------------------------------------------------------------------------------------------------------//






jetmass_histograms->Close();

delete jetmass_histograms;


delete reco_jet_mass_hist;
delete matched_truth_jet_mass_hist;
delete smeared_reco_jet_mass_hist;


	}//Close the systematics loop




  return 0;
}















