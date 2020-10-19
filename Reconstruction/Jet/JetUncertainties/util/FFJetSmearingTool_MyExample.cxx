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
#include "ParticleJetTools/JetTruthLabelingTool.h"

//My includes
#include "JetUncertainties/FFJetSmearingTool.h"

// Error checking macro
#define CHECK( ARG )                                 \
  do {                                               \
    const bool result = ARG;                         \
    if(!result) {                                    \
      printf("Failed to execute: %s \n", #ARG);\
      return 1;                                      \
    }                                                \
  } while( false )



// Help message if the  --help option is given by the user
void usage() {
    printf("Running options:\n");
    printf("YOU HAVE TO ADAPT THE OPTIONS TO FFJETSMEARINGCORRECTION\n");
    printf("        --help : To get the help you're reading\n");
    printf("        --jetColl= : Specify the jet collection (TrimmedLargeR)\n");
    printf("        --MassDef= : Specify the kind of jet mass used (Calo, TA, Comb)\n");
    printf("        --sample= : Specify input xAOD\n");
    printf("        Example: FFJetSmearingTool_MyExample  --truth_jetColl=AntiKt10TruthTrimmedPtFrac5SmallR20Jets --reco_jetColl=AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets --MassDef=Comb  --sample=DAOD_JETM6.16317590._000003.pool.root.1  --output=file_name.root --ConfigFile=rel21/Spring2020/FFJetSmearingTool_TestOnly_JMR.config   --DebugTool=true \n");
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
        printf("No input xAOD file specified, exiting\n");
        return 1;
    }
    if(truth_jetColl==""){
        printf("No truth jet collection specified, exiting\n");
        return 1;
    }
    if(reco_jetColl==""){
        printf("No truth jet collection specified, exiting\n");
        return 1;
    }
    if(string_kindofmass==""){
        printf("No kind of jet mass specified, exiting\n");
        return 1;
    }
    if(string_configfile_path==""){
        printf("No ConfigFile specified, exiting\n");
        return 1;
    }
    if(string_debugtool==""){
        printf("No debugtool specified, exiting\n");
        return 1;
    }
    if(output==""){
        printf("output not specified, exiting\n");
        return 1;
    }
    printf("%s %s %s %s %s \n", sample.c_str(), truth_jetColl.c_str(), reco_jetColl.c_str(), output.c_str(), string_debugtool.c_str());


    TString kindofmass = string_kindofmass;

    bool want_to_debug = false;

    if(string_debugtool == "true"){want_to_debug = true;}
    if(string_debugtool == "false"){want_to_debug = false;}

//*************************************************************************************************************************************


    //--------------------
    // Opening input file
    //--------------------

    std::unique_ptr< TFile > ifile( TFile::Open( sample.c_str(), "READ" ) );


    // Create a TEvent object
    xAOD::TEvent event(xAOD::TEvent::kClassAccess);

    CHECK( event.readFrom( ifile.get() ) );

  
    //----------------------------------
    // Initialization of FFJetSmearingTool
    //----------------------------------
        
    const std::string name_FFJetSmearingTool = "FFJetSmearing_Example";
    // Call the constructor

    CP::FFJetSmearingTool ffjetsmearingtool(name_FFJetSmearingTool.c_str());

/*
asg::AsgToolConfig config (("FFJetSmearingTool/" + name_FFJetSmearingTool).c_str());
// configure `config` object
std::shared_ptr<void> cleanup;
ToolHandle<FFJetSmearingTool> ffjetsmearingtool;


//ASSERT_SUCCESS (config.makeTool (fjetsmearingtool, cleanup));
config.makeTool (ffjetsmearingtool, cleanup);
*/






    (ffjetsmearingtool.setProperty( "MassDef", kindofmass )).ignore();

    (ffjetsmearingtool.setProperty( "ConfigFile", string_configfile_path  )).ignore();

    // Initialize the tool
    if(!(ffjetsmearingtool.initialize().isSuccess())){
        printf("Initialization of FFJetSmearingTool failed, exiting\n");
        return 0;
    }


    //Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL, 7=ALWAYS)
    ffjetsmearingtool.setProperty ("OutputLevel", MSG::ERROR);

    if(want_to_debug){
        ffjetsmearingtool.setProperty ("OutputLevel", MSG::VERBOSE);
    }
//***************************************************************** TESTINGGGG**********************
    // Print the recommended systematics

    const CP::SystematicSet& recommendedSysts = ffjetsmearingtool.recommendedSystematics();//take the systematics of the FFJETSmearing Tool
    printf("Recommended systematics: \n");
    for(auto sysItr = recommendedSysts.begin();
        sysItr != recommendedSysts.end(); ++sysItr){
        printf("%s \n", sysItr->name().c_str());
    }


    std::vector<CP::SystematicSet> sysList;



    //---------------------------------------------------------
    //////Initialize the tool to set the truth tagging

    JetTruthLabelingTool m_JetTruthLabelingTool("JetTruthLabelingTool");

    CHECK(m_JetTruthLabelingTool.setProperty("TruthLabelName",  "R10TruthLabel_R21Consolidated"));
    CHECK(m_JetTruthLabelingTool.setProperty("UseTRUTH3",  false)); // Set this to false only if you have the FULL !TruthParticles container in your input file
    CHECK(m_JetTruthLabelingTool.setProperty("TruthParticleContainerName",  "TruthParticles")); // Set this if you have the FULL !TruthParticles container but have named it something else
    CHECK(m_JetTruthLabelingTool.initialize());





    //--------------------
    // Calibrate jets (ir is necessary to extract the Calo and TA from a Combined jet). The user have to calibrate the jets before using the SmearingTool
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
        printf("Initialization of JetCalibTools failed, exiting \n");
        return 0;
    }
//--------------------------------------------------------


    for (auto sys : recommendedSysts) { sysList.push_back(CP::SystematicSet({sys})); }

    printf("\n=============SYSTEMATICS CHECK NOW \n");
    for (auto sys : sysList)	 {
        // Tell the calibration tool which variation to apply
        if (ffjetsmearingtool.applySystematicVariation(sys) != CP::SystematicCode::Ok) {
            printf("Error, Cannot configure calibration tool for systematics \n");
        }

        printf("\nWe are using the systematic %s \n", sys.name().c_str());

	


//**************************************************************************************

        static constexpr float MeVtoGeV = 1.e-3;

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


        std::unique_ptr<TH3F> hist_jet_mass_scale_change_3D;
        hist_jet_mass_scale_change_3D =  std::make_unique<TH3F>("hist_jet_mass_scale_change_3D","hist_jet_mass_scale_change_3D",numBinsPt,0,upperlimit2/*pt*/,numBinsMass,0,upperlimit1/*mass*/,numBinsMass,0,upperlimit1/*mass*/);


        float lowerlimit3 = -0.5;
        float upperlimit3 = 0.5;
        int numBinsDiff = 100;


        std::unique_ptr<TH3F> hist_jet_mass_resolution_change_3D;
        hist_jet_mass_resolution_change_3D =  std::make_unique<TH3F>("hist_jet_mass_resolution_change_3D","hist_jet_mass_resolution_change_3D",numBinsPt,0,upperlimit2/*pt*/,numBinsMass,0,upperlimit1/*mass*/,numBinsDiff,lowerlimit3,upperlimit3/*mass*/);



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
                printf("Failed to load entry %lld \n", ievent);
                return 1;
            }


            // Show status
            if(ievent % 100==0) printf("Event %lld of %lld \n", ievent, nevents);

            //************************************************************//
            // Print some event information for fun
            if(want_to_debug){
                const xAOD::EventInfo* ei = nullptr;
                CHECK( event.retrieve(ei, "EventInfo") );

                printf("===>>>  start processing event #%llu, run #%i, %lld events processed so far  <<<=== \n", ei->eventNumber(), ei->runNumber(), ievent);


                // Get the truth jets from the event
                const xAOD::JetContainer* jets_truth = nullptr;
                CHECK( event.retrieve(jets_truth, "AntiKt10TruthTrimmedPtFrac5SmallR20Jets") );

                printf("Number of truth jets: %lu \n", jets_truth->size());

                //Loop over the truth jets in the event
                for(const xAOD::Jet* jet_truth : *jets_truth){
                    // Print basic info about this jet
                    printf("Truth Jet: pt = %f, mass = %f, eta = %f \n", jet_truth->pt()*MeVtoGeV, jet_truth->m()*MeVtoGeV, jet_truth->eta());
                }



                // Get the reco jets from the event
                const xAOD::JetContainer* jets_reco = nullptr;
                CHECK( event.retrieve(jets_reco, reco_jetColl) ); 
 
                printf("Number of reco jets: %lu \n", jets_reco->size());

                //Loop over the reco jets in the event
                for(const xAOD::Jet* jet_reco : *jets_reco){

                    // Print basic info about this jet
                    printf("Reco Jet: pt = %f, mass = %f, eta = %f \n", jet_reco->pt()*MeVtoGeV, jet_reco->m()*MeVtoGeV, jet_reco->eta());
                }
            }
            //************************************************************//



            xAOD::Jet jet_truth_matched;
            jet_truth_matched.makePrivateStore();
 
            // Retrieve jet container
            const xAOD::JetContainer* jets = nullptr;
            CHECK( event.retrieve( jets, reco_jetColl ) );

            // Shallow copy 
            auto jets_shallowCopy = xAOD::shallowCopyContainer( *jets );

            //Give a TruthLabel to the jets. We will need it in the FFSmearingTool to apply the uncertainties of one jet topology or another
            m_JetTruthLabelingTool.modify(*(jets_shallowCopy.first));
	
            if(want_to_debug) printf("Start the loop over the jets \n");

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

                if(want_to_debug && kindofmass == "Comb"){   
                    printf("Comb jet mass = %f \n", jet_reco_Comb_FourMom.mass());
                    printf("Calo jet mass = %f \n", jet_reco_CALO_FourMom.mass());
                    printf("TA jet mass = %f \n", jet_reco_TA_FourMom.mass());
                }

                std::unique_ptr<xAOD::Jet> jet_reco_Comb = std::make_unique<xAOD::Jet>(); //You have to initialize the jet object this way. 
                                                                                            //If not, oyu will encounter breaks when running 
                jet_reco_Comb->makePrivateStore(); //And after that we have to give it a private store to avoid a brak when using setJetP4
                jet_reco_Comb->setJetP4(jet_reco_Comb_FourMom);
                std::unique_ptr<xAOD::Jet> jet_reco_CALO = std::make_unique<xAOD::Jet>();     
                jet_reco_CALO->makePrivateStore();
                jet_reco_CALO->setJetP4(jet_reco_CALO_FourMom);
                std::unique_ptr<xAOD::Jet> jet_reco_TA = std::make_unique<xAOD::Jet>();  
                jet_reco_TA->makePrivateStore();
                jet_reco_TA->setJetP4(jet_reco_TA_FourMom);

                if(want_to_debug && kindofmass == "Comb"){
                    printf("NEW Comb jet mass = %f \n", jet_reco_Comb->m());
                    printf("NEW CALO jet mass = %f \n", jet_reco_CALO->m());
                    printf("NEW TA jet mass = %f \n", jet_reco_TA->m());
                }

//------------------------------------------------------------------------------------------------------------------------------------------//

                if(want_to_debug && kindofmass=="Calo"){ printf("CALO jet mass = %f \n", jet_reco->m()); }
                if(want_to_debug && kindofmass=="TA"){ printf("TA jet mass = %f \n", jet_reco_TA->m()); }

                if(kindofmass=="TA"){m_JetTruthLabelingTool.modifyJet(*(jet_reco_TA)); };//To tag the TA jet with the jet topology


                if(!(ffjetsmearingtool.getMatchedTruthJet(*jet_reco, jet_truth_matched).isSuccess())){ continue;}

                if(lead_jet == true){reco_jet_mass_hist->Fill(jet_reco->m()*MeVtoGeV); matched_truth_jet_mass_hist->Fill(jet_truth_matched.m()*MeVtoGeV);  }

                if(kindofmass=="TA"){

                    Double_t aux_original_jet_mass = jet_reco->m()*MeVtoGeV;

                    ffjetsmearingtool.applyCorrection(*jet_reco_TA);
                    if(lead_jet == true && aux_original_jet_mass > 0){
                    smeared_reco_jet_mass_hist->Fill(jet_reco_TA->m()*MeVtoGeV); lead_jet=false;

                    hist_jet_mass_scale_change_3D->Fill(jet_reco->pt()*MeVtoGeV, aux_original_jet_mass, (jet_reco_TA->m()*MeVtoGeV)/*/(aux_original_jet_mass)*/);
                    }
                }  
                else{
                    Double_t aux_original_jet_mass = jet_reco->m()*MeVtoGeV;

                    ffjetsmearingtool.applyCorrection(*jet_reco);

                    if(lead_jet == true && aux_original_jet_mass > 0){
                        smeared_reco_jet_mass_hist->Fill(jet_reco->m()*MeVtoGeV); lead_jet=false; 

                        hist_jet_mass_scale_change_3D->Fill(jet_reco->pt()*MeVtoGeV, aux_original_jet_mass, (jet_reco->m()*MeVtoGeV)/*/(aux_original_jet_mass)*/);

                        hist_jet_mass_resolution_change_3D->Fill(jet_reco->pt()*MeVtoGeV, aux_original_jet_mass, TMath::Abs((jet_reco->m()*MeVtoGeV) - (aux_original_jet_mass))/ aux_original_jet_mass);

                    }
                }

                if(want_to_debug){
                    if(kindofmass=="Comb"){ printf("Comb jet mass after smearing = %f \n", jet_reco->m()); }
                    if(kindofmass=="Calo"){ printf("CALO jet mass after smearing = %f \n", jet_reco->m()); }
                    if(kindofmass=="TA"){ printf("TA jet mass after smearing = %f \n", jet_reco_TA->m()); }
                }

                //-----------------------------------------------------//
                //To test corrected copy function
                xAOD::Jet* jet_reco_copy = nullptr;
                if( ! ffjetsmearingtool.correctedCopy(*jet_reco, jet_reco_copy) ){//The copy is filled only if the jet passes the conditions to be used inside the Tool
                    if(want_to_debug){ printf("Failed to fill the corrected copy of the jet \n"); }
                }
                else{
                    if(want_to_debug){ printf("Corrected Copy Comb jet mass after smearing = %f \n", jet_reco_copy->m()); }
                }                   
                // Delete the corrected copy:
                delete jet_reco_copy; 
                //-----------------------------------------------------//



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
                    hold->SetBinContent(k,hist_jet_mass_scale_change_3D->GetBinContent(i,j,k));                    //A 2D projection of the TH3
                }
                hist_jet_mass_scale_change_2D->SetBinContent(i,j,hold->GetMean()/hist_jet_mass_scale_change_3D->GetYaxis()->GetBinCenter(j));
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
                    hold->SetBinContent(k,hist_jet_mass_resolution_change_3D->GetBinContent(i,j,k));                    //A 2D projection of the TH3
                }
                hist_jet_mass_resolution_change_2D->SetBinContent(i,j,hold->GetMean()/hist_jet_mass_resolution_change_3D->GetYaxis()->GetBinCenter(j));
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

        delete hist_jet_mass_scale_change_2D;

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
