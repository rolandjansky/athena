/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

// System include(s):
#include <memory>
#include <cstdlib>
#include <iomanip>
#include <string>

// ROOT include(s):
#include <TFile.h>
#include <TString.h>

// Infrastructure include(s):
#ifdef ROOTCORE
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"
#endif // ROOTCORE

//Asg includes
#include "PATInterfaces/SystematicsUtil.h"
#include "PATInterfaces/MakeSystematicsVector.h"
#include "ElectronEfficiencyCorrection/AsgElectronEfficiencyCorrectionTool.h"

//Local includes
#include "EGToolProperties.h"
#include "Messaging.h"
#include "CreateDummyEl.h"
//
#include <boost/program_options.hpp>


int main( int argc, char* argv[] ) {
    xAOD::TReturnCode::enableFailure();
    MSGHELPERS::getMsgStream().msg().setLevel(MSG::INFO); 

    //Parse input from Command line 
    std::string fileName{};
    std::string mapfileName{};
    std::string idkey{};
    std::string recokey{};
    std::string isokey{};
    std::string triggerkey{};
    std::string model{};
    int runno{};
    float eta{};
    float pt{};
    std::string type{};
    int inputlevel{};

    using namespace boost::program_options;
    options_description desc{"Options"};
    desc.add_options()
        ("help,h", "Help screen")
        ("msgLevel,l", value<int>(&inputlevel)->default_value(static_cast<int>(MSG::INFO)), "message level")
        ("file,f", value<std::string>(&fileName)->default_value(""), "scale factor file: Required a SF file or a map file")
        ("runno,r", value<int>(&runno)->required(), "run number: Required")
        ("eta,e", value<float>(&eta)->required(), "eta: Required") 
        ("pt,p", value<float>(&pt)->required(), "pt: Required")
        ("type,t", value<std::string>(&type)->required(), "Simulation type: Required") 
        ("correlation,c", value<std::string>(&model)->default_value("FULL"), "Correlation Model FULL (default),SIMPLIFIED,TOTAL,COMBMCTOYS")
        ("keyreco,k", value<std::string>(&recokey)->default_value(""), "Reco working point Key")
        ("mapfile,m", value<std::string>(&mapfileName)->default_value(""), "Map file name: Required a SF file or a map file")
        ("keyid,d", value<std::string>(&idkey)->default_value(""), "Identification working point Key")
        ("keyiso,i", value<std::string>(&isokey)->default_value(""), "Isolation working point Key")
        ("keytrigger,g", value<std::string>(&triggerkey)->default_value(""), "Trigger working point Key")
        ;   
    variables_map vm;
    try{
        store(parse_command_line(argc, argv, desc), vm);
        if (vm.count("help")){
            MSG_INFO(desc << '\n');
            return 0;
        }
        notify(vm);
    }       
    catch (const boost::program_options::error &ex){
        MSG_ERROR(ex.what() << '\n');   
        MSG_ERROR(desc << '\n');
        return 0;
    }

    if(fileName=="" && mapfileName==""){
        MSG_ERROR("Neither map file name nor file name given");
        MSG_ERROR(desc << '\n');
        return 0;
    }
    if (!(type=="FullSim"||type=="AtlFast2")){
        MSG_ERROR("No valid type given (FullSim or AtlFast2)");
        MSG_ERROR(desc << '\n');
        return 0;
    }
    PATCore::ParticleDataType::DataType SimType= (
            type=="FullSim" ? PATCore::ParticleDataType::Full : 
            PATCore::ParticleDataType::Fast);
    if (!(model=="COMBMCTOYS"
                ||model=="FULL"
                ||model=="SIMPLIFIED"
                ||model=="TOTAL"
         )){
        MSG_ERROR("No valid correlation model");
        MSG_ERROR(desc << '\n');
        return 0;
    }

    // Initialise the analysis application
    const char* APP_NAME = argv[0];
    MSG::Level mylevel=static_cast<MSG::Level>(inputlevel);
    MSGHELPERS::getMsgStream().msg().setName(APP_NAME);     
    MSGHELPERS::getMsgStream().msg().setLevel(mylevel); 
    CHECK( xAOD::Init( APP_NAME ) );

    //create and then retrieve the dummy electrons   
    std::pair<double,double> pt_eta{pt,eta};
    xAOD::TStore* store=getElectrons({pt_eta},runno);
    if(!store){
        MSG_ERROR("could not create the electrons ");
    } 
    const xAOD::ElectronContainer* electrons(nullptr);
    std::cout<<store->contains<xAOD::ElectronContainer>("MyElectrons");
    CHECK(store->retrieve(electrons,"MyElectrons").isSuccess());
    xAOD::Electron el= *(electrons->at(0));

    //Do the test 
    AsgElectronEfficiencyCorrectionTool ElEffCorrectionTool ("ElEffCorrectionTool");   
    if (fileName!=""){ 
        std::vector<std::string> inputFiles{fileName} ;
        CHECK( ElEffCorrectionTool.setProperty("CorrectionFileNameList",inputFiles) );
    }
    if (mapfileName!="") {
        CHECK( ElEffCorrectionTool.setProperty("MapFilePath", mapfileName));
    }
    if ( recokey!="")  {
        CHECK( ElEffCorrectionTool.setProperty("RecoKey", recokey));
    }
    if ( idkey!="") {
        CHECK( ElEffCorrectionTool.setProperty("IdKey", idkey));
    }
    if ( isokey!="") {
        CHECK( ElEffCorrectionTool.setProperty("IsoKey", isokey));
    }
    if ( triggerkey!="") {
        CHECK( ElEffCorrectionTool.setProperty("TriggerKey", triggerkey));
    }
    CHECK( ElEffCorrectionTool.setProperty("ForceDataType",(int)SimType) );
    CHECK( ElEffCorrectionTool.setProperty("OutputLevel", mylevel ));
    CHECK( ElEffCorrectionTool.setProperty("CorrelationModel", "FULL" )); 
    CHECK( ElEffCorrectionTool.initialize());  
    dumbProperties(ElEffCorrectionTool);
    double SF = 0; 
    std::vector<double> unc;
    // Get a list of systematics
    CP::SystematicSet recSysts = ElEffCorrectionTool.recommendedSystematics();

    if(ElEffCorrectionTool.getEfficiencyScaleFactor(el,SF) != CP::CorrectionCode::Ok){
        MSG_ERROR( APP_NAME << "Problem in getEfficiencyScaleFactor");
    }
    MSG_INFO("SF  "<< SF );

    // Loop over systematics
    double total2_up =0 ; 
    double total2_down =0 ;
    for(const auto& sys : recSysts){
        double systematic = 0; 

        // Configure the tool for this systematic
        CHECK( ElEffCorrectionTool.applySystematicVariation({sys}) );

        if(ElEffCorrectionTool.getEfficiencyScaleFactor(el,systematic) == CP::CorrectionCode::Ok){
            MSG_INFO( ElEffCorrectionTool.appliedSystematics().name().c_str()<< " Result " << systematic<< " Systematic value  "<<systematic-SF );  
            unc.push_back(systematic);

            float param =sys.parameter() ;    
            if(param>0) {
                total2_up += (SF-systematic)*(SF-systematic) ;  
            }
            else{
                total2_down+= (SF-systematic)*(SF-systematic) ;  
            }
        }else{
            MSG_ERROR( APP_NAME << "Problem in getEfficiencyScaleFactor");
        }
    }
    double total_up = sqrt(total2_up);
    double total_down = -sqrt(total2_down); 

    MSG_INFO( "total up " << total_up  <<" total down " << total_down  );
    MSG_INFO( "Index " << ElEffCorrectionTool.systUncorrVariationIndex(el) );

    /*
    //==================================================================================
    //Test the TOYS
    //TOYS
    MSG::Level mylevelToy=MSG::INFO;//MSG::FATAL;
    AsgElectronEfficiencyCorrectionTool ElEffCorrectionToolToys ("ElEffCorrectionToolToys");
    if (fileName!="")   CHECK( ElEffCorrectionToolToys.setProperty("CorrectionFileNameList",inputFiles) );
    if (mapfileName!="") CHECK( ElEffCorrectionToolToys.setProperty("MapFilePath", mapfileName));

    // set the keys of interest for correction files
    if ( recokey!="")  CHECK( ElEffCorrectionToolToys.setProperty("RecoKey", recokey));
    MSG_INFO("using mapfile :" << recokey);
    if ( idkey!="")   CHECK( ElEffCorrectionToolToys.setProperty("IdKey", idkey));
    if ( isokey!="")   CHECK( ElEffCorrectionToolToys.setProperty("IsoKey", isokey));
    if ( triggerkey!="")   CHECK( ElEffCorrectionToolToys.setProperty("TriggerKey", triggerkey));

    CHECK( ElEffCorrectionToolToys.setProperty("ForceDataType",(int)SimType) );
    CHECK( ElEffCorrectionToolToys.setProperty("CorrelationModel", "MCTOYS" ));
    ElEffCorrectionToolToys.msg().setLevel(mylevelToy);
    CHECK( ElEffCorrectionToolToys.initialize() );
    dumbProperties(ElEffCorrectionToolToys);
    double SFToys = 0; 
    if(ElEffCorrectionToolToys.getEfficiencyScaleFactor(*el,SFToys) == CP::CorrectionCode::Ok){
    MSG_INFO("SF  Toys central "<< SF );
    }
    std::vector<double> uncToys;
    CP::SystematicSet recSystsToys = ElEffCorrectionToolToys.recommendedSystematics();
    //std::vector<CP::SystematicSet> sysListToys;
    CP::MakeSystematicsVector sysListToys;
    sysListToys.addGroup("toys");
    sysListToys.setToys(  ElEffCorrectionToolToys.getNumberOfToys( )   );
    sysListToys.calc(recSystsToys);
    std::vector<CP::SystematicSet> sysListToys2=sysListToys.result("toys");

    /// DO TOY LOOP
    for(const auto& sysToys : sysListToys2){
    double systematicToys = 0; 
    CHECK( ElEffCorrectionToolToys.applySystematicVariation(sysToys) );    
    if(ElEffCorrectionToolToys.getEfficiencyScaleFactor(*el,systematicToys) == CP::CorrectionCode::Ok){
    MSG_INFO("SFToys value "<< systematicToys );
    }
    uncToys.push_back(systematicToys);
    }

*/
    return 0;
}


