/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

// System include(s):
#include <memory>
#include <cstdlib>
#include <iomanip>
#include <string>
#include <numeric>
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

    /*
     * Parse the input from the command line
     */
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

    // Initialize the xAOD application 
    const char* APP_NAME = argv[0];
    MSG::Level mylevel=static_cast<MSG::Level>(inputlevel);
    MSGHELPERS::getMsgStream().msg().setName(APP_NAME);     
    MSGHELPERS::getMsgStream().msg().setLevel(mylevel); 
    CHECK( xAOD::Init( APP_NAME ) );

    /*
     * create and then retrieve the dummy electrons (in this case is 1)
     * This also setups the store,so for done before the tool init
     * it really is a pseudo "reconstruction"
     */

    std::vector< std::pair<double,double>> pt_eta{{pt,eta}}; 
    xAOD::TStore* store=getElectrons(pt_eta,runno);
    if(!store){
        MSG_ERROR("could not create the electrons ");
    } 
    const xAOD::ElectronContainer* electrons(nullptr);
    CHECK(store->retrieve(electrons,"MyElectrons").isSuccess());

    //Configure the tool based on the inputs
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
    CHECK( ElEffCorrectionTool.setProperty("CorrelationModel", model )); 
    CHECK( ElEffCorrectionTool.initialize());  
    dumbProperties(ElEffCorrectionTool);

    /*
     * Set up the systematic variations
     * 2 cases one for "continuous" one for "toys"
     */
    bool isToys = model.find("TOY") != std::string::npos ? true :false;
    double nominalSF{};
    double totalNeg{};
    double totalPos{}; 
    CP::SystematicSet systs = ElEffCorrectionTool.recommendedSystematics();  
    /*
     * Potentiallly we can make this part more clever, for now since it is an
     * util I did not tried to optimise too much.
     */
    if(!isToys){
        /*
         * Split the variation in up and down
         * Do it before any loop
         * This is obviously more of a sanity check
         * as they are currently symmetric so is mainly
         * about inspecting them both 
         */
        std::vector<CP::SystematicVariation> positiveVar{};
        std::vector<CP::SystematicVariation> negativeVar{};
        for(const auto& sys : systs){
            float param =sys.parameter() ;    
            if(param<0) {
                negativeVar.push_back(sys);  
            }
            else{
                positiveVar.push_back(sys) ;  
            }   
        }
        //Helper function as a lamda
        auto totalSyst = [&ElEffCorrectionTool] (xAOD::Electron el, const std::vector<CP::SystematicVariation>& variations, 
                const double nominal, double&result ){
            double total2{} ; 
            double systematic{}; 
            for(const auto& sys : variations){
                if(ElEffCorrectionTool.applySystematicVariation({sys})!=CP::SystematicCode::Ok ||
                        ElEffCorrectionTool.getEfficiencyScaleFactor(el,systematic) != CP::CorrectionCode::Ok){   
                    MSG_ERROR("Error in setting/getting " << sys.name());
                    return CP::CorrectionCode::Error;    
                }
                MSG_INFO( ElEffCorrectionTool.appliedSystematics().name()
                        << " Result : " << systematic<< " Value :  "<<systematic-nominal );  
                total2 += (nominal-systematic)*(nominal-systematic) ;  
            }
            result=std::sqrt(total2);
            return CP::CorrectionCode::Ok;
        };
        //Loop over electrons , here it is one    
        xAOD::Electron el= *(electrons->at(0));
        MSG_INFO( "Electron pt/eta index : " << ElEffCorrectionTool.systUncorrVariationIndex(el) );
        //Do the work 
        CHECK(ElEffCorrectionTool.getEfficiencyScaleFactor(el,nominalSF) == CP::CorrectionCode::Ok);
        CHECK(totalSyst(el,negativeVar,nominalSF,totalNeg));
        CHECK(totalSyst(el,positiveVar,nominalSF,totalPos));
   }
    else{
        CP::MakeSystematicsVector sysVec;
        sysVec.addGroup("toys");
        sysVec.setToys(ElEffCorrectionTool.getNumberOfToys());
        sysVec.calc(systs);
        std::vector<CP::SystematicSet> toys=sysVec.result("toys");
        std::vector<double> toysVal{};
        toysVal.reserve(toys.size());

        //Do the work
        xAOD::Electron el= *(electrons->at(0));
        MSG_INFO( "Electron pt/eta index : " << ElEffCorrectionTool.systUncorrVariationIndex(el) );
        for (const auto& sys : toys){
            double systematic{};
            CHECK(ElEffCorrectionTool.applySystematicVariation(sys)==CP::SystematicCode::Ok &&
                ElEffCorrectionTool.getEfficiencyScaleFactor(el,systematic) == CP::CorrectionCode::Ok); 
            MSG_INFO( ElEffCorrectionTool.appliedSystematics().name() << " toy Result : " <<systematic)
            toysVal.push_back(systematic);
        }
         /*
         *  B. P. Welford 1962  
         *  Donald KnutArt of Computer Programming, Vol 2, page
         *  232, 3rd edition
         */
        //1st element,initilize 
        double meanK{toysVal[0]}; //current mean
        double meanK_1{toysVal[0]};//next mean
        double s{0};
        size_t k{1};
        const size_t N=toysVal.size();
        //subsequent ones 
        for (size_t i=1; i!=N;++i){            
            const double x{toysVal[i]};
            const double invk {(1.0/(++k))};   
            meanK_1= meanK + (x-meanK)*invk;
            s+=(x-meanK_1)*(x-meanK);
            meanK=meanK_1;
        }
        const double variance= s/(N-1);
        nominalSF=meanK;
        totalNeg=sqrt(variance);
        totalPos=sqrt(variance);
    }
    MSG_INFO("===> " <<model << " : SF = "<< nominalSF << " + " << totalPos << " - " <<totalNeg );
    return 0;
}


