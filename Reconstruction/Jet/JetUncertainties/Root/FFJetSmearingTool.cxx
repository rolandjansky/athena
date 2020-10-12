/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/* ***********************************************************************************\
 *                                                                                   *
 *      Name: FFJetSmearingTool                                                      *
 *      Purpose: Perform a jet mass smearing using Forward Folding                   *
 *      This tool allow the user to stimate the JMS & JMR systematics uncertainities *
 *                                                                                   *
\*************************************************************************************/

/// For information, see the Twiki:
/// https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/FFJetSmearingTool
///
/// If you find any bug, please, contact <alberto.prades.ibanez@cern.ch>


// Local includes
#include "JetUncertainties/FFJetSmearingTool.h"

namespace CP {

// Constructor
FFJetSmearingTool::FFJetSmearingTool(const std::string name)
    : asg::AsgTool(name) 
    , m_isInit(false)
    , m_release("")
    , m_truth_jetColl("")
    , m_EtaRange(0)
    , m_calibArea("CalibArea-08")
    , m_histFileName("")
    , m_InfoWarnings(0)
    {
    declareProperty( "MassDef", m_MassDef_string = ""                             );
    declareProperty( "ConfigFile", m_configFile = ""        );//Path to the config file. By default it points to XXX
    declareProperty("Path",m_path);
}

// Destructor
FFJetSmearingTool::~FFJetSmearingTool()
{ }



// Initialize
StatusCode FFJetSmearingTool::initialize()
{

    // Ensure it hasn't been initialized already
    if (m_isInit)
    {
        ATH_MSG_FATAL(Form("Blocking re-initialization of tool named %s",AsgTool::name().c_str())); //AsgTool::name() calls the name
        return StatusCode::FAILURE;
    }

    ATH_MSG_INFO(Form("Preparing to initialize the FFJetSmearingTool named %s",AsgTool::name().c_str()));

    if (AsgTool::name()=="")
    {
        ATH_MSG_FATAL("No name specified.  Aborting.");
        return StatusCode::FAILURE;
    }
    if (m_MassDef_string.empty())
    {
        ATH_MSG_FATAL("No kind of jet mass specified.  Aborting.");
        return StatusCode::FAILURE;
    }

    // Make sure we have a valid mass definition
    ATH_CHECK(JetTools::stringToEnum(m_MassDef_string, m_MassDef)); //If it fails it means that there is No Systematic Uncertainties derived for to the given mass definition.
                                                                     //The mass definition should be 'Calo', 'TA' or 'Comb'. Show an error and exits.

    //reading the config file as in JetUncertaintiesTool
    TEnv settings;

    const TString configFilePath = jet::utils::findFilePath(m_configFile.c_str(),m_path.c_str(),m_calibArea.c_str());

    if (settings.ReadFile( configFilePath.Data(),kEnvGlobal))
    {
        ATH_MSG_ERROR("Cannot read config file: " << configFilePath.Data());
        return StatusCode::FAILURE;
    }
    // We can read it - start printing
    ATH_MSG_INFO("================================================");
    ATH_MSG_INFO(Form("  Initializing the FFJetSmearingTool named %s",AsgTool::name().c_str()));
    ATH_MSG_INFO("  Configuration file: " << m_configFile);
    ATH_MSG_INFO("    Location: " << configFilePath.Data());


    m_release = settings.GetValue("UncertaintyRelease","UNKNOWN");
    ATH_MSG_INFO("  Uncertainty release: " << m_release.c_str());

    // Check the jet definition
    m_truth_jetColl = settings.GetValue("TruthJetColl","");
    if (m_truth_jetColl.empty())
    {
        ATH_MSG_ERROR("Cannot find the truth jet collection to use in config");
        return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("  Truth Jet Collection: " << m_truth_jetColl);
    // Check the name of the truth label accessor for BoostjetTaggers
    m_truthlabelaccessor = settings.GetValue("TruthLabelAccessor","");
    if (m_truthlabelaccessor.empty())
    {
        ATH_MSG_ERROR("Cannot find the TruthLabelAccessor to use in config");
        return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("  Truth Label Accessor: " << m_truthlabelaccessor);
    //eta range of the tool  
    m_EtaRange = settings.GetValue("EtaRange",0);
    if (m_EtaRange == 0)
    {
        ATH_MSG_ERROR("Cannot find the EtaRange parameter in the config file");
        return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("  EtaRange : Abs(eta) < " << m_EtaRange);
    //mass range of the tool  
    m_MaxMass = settings.GetValue("MaxMass",0);
    if (m_MaxMass == 0)
    {
        ATH_MSG_ERROR("Cannot find the MaxMass parameter in the config file");
        return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("  MaxMass : jet_mass < " << m_MaxMass);
    //pt range of the tool 
    m_MaxPt = settings.GetValue("MaxPt",0);
    if (m_MaxPt == 0)
    {
        ATH_MSG_ERROR("Cannot find the MaxPt parameter in the config file");
        return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("  MaxPt : jet_pt < " << m_MaxPt);
    // Get the file to read uncertainties in from
    m_histFileName = settings.GetValue("UncertaintyRootFile","");
    m_HistogramsFilePath = jet::utils::findFilePath(m_histFileName.c_str(),m_path.c_str(),m_calibArea.c_str());
    if (m_histFileName.empty())
    {
        ATH_MSG_ERROR("Cannot find uncertainty histogram file in the config file");
        return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("  UncertaintyFile: " << m_histFileName);
    ATH_MSG_INFO("    Location: " << m_HistogramsFilePath);



    //Read all the histogram files where the jms jmr variations are saved
    ATH_CHECK(readFFJetSmearingToolSimplifiedData(settings));//If fail, it shows an Error message and exits

    // Add the affecting systematics to the global registry
    CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
    if(registry.registerSystematics(*this) != CP::SystematicCode::Ok){
      ATH_MSG_ERROR("Unable to register systematics!");
      return StatusCode::FAILURE;
    }

    m_isInit = true;

   // ANA_CHECK (applySystematicVariation (CP::SystematicSet()));

    return StatusCode::SUCCESS;
}



//-----------------------------------------------------------------------------
// Declare affecting systematics
//-----------------------------------------------------------------------------
bool FFJetSmearingTool::isAffectedBySystematic
(const CP::SystematicVariation& systematic) const
{
    // Using 'find' is sufficient until this tool supports continuous
    //   // variations, at which point I'll need to use the 'match' method.
    CP::SystematicSet sys = affectingSystematics();
    return sys.find(systematic) != sys.end();
}
//-----------------------------------------------------------------------------

CP::SystematicSet FFJetSmearingTool::affectingSystematics() const
{
    CP::SystematicSet result;
    result.insert(m_SysList);

    return result;
}
//-----------------------------------------------------------------------------
CP::SystematicSet FFJetSmearingTool::recommendedSystematics() const
{
    return m_SysList;
}

//-----------------------------------------------------------------------------
// Apply systematic configuration
//-----------------------------------------------------------------------------

CP::SystematicCode FFJetSmearingTool::applySystematicVariation
(const CP::SystematicSet& systematics)
{
    // First check if we already know this systematic configuration.
    // Look for it in our sysData map.
    auto iter = m_sysData.find (systematics);


    // If this is a new input set, we need to filter it.
    if(iter == m_sysData.end()){

        // Filter the input systematics with my affecting systematics.
        const CP::SystematicSet affectingSysts = affectingSystematics();
        CP::SystematicSet filteredSysts;
        if( CP::SystematicSet::
            filterForAffectingSystematics(systematics, affectingSysts, filteredSysts) !=
            CP::SystematicCode::Ok ) 
        {
            ATH_MSG_ERROR("Received unsupported systematics: " << systematics.name());
            return CP::SystematicCode::Unsupported;
        }

        // At this point, we can do some additional checks for consistency
        // with the JMS/JMR functionality. For example, if the tool can only handle
        // one type of systematic at a time, we return an error if the filtered
        // set has more than one item:
        if(filteredSysts.size() > 1){
            ATH_MSG_ERROR("No support for more than one JMS/JMR sys at a time: " <<
                    filteredSysts.name());
            return CP::SystematicCode::Unsupported;
        }



        // Insert the new systematic data onto our map
        SysData myData;

        const CP::SystematicVariation& sys = *filteredSysts.begin();

        myData.SysParameter = sys.parameter(); //Up (+1) and Down (-1) systematic variation
        myData.SysBaseName = sys.basename(); //Name of the systematic variation

        iter = m_sysData.emplace (systematics, myData).first;
    }

    // Apply the filtered systematics
    m_currentSysData = &iter->second;

    return CP::SystematicCode::Ok;
}



//-----------------------------------------------------------------------------
// Read the external file that conatins the JSS recomendations for FatJets
//-----------------------------------------------------------------------------

StatusCode FFJetSmearingTool::readFFJetSmearingToolSimplifiedData(TEnv& settings){

    std::unique_ptr<TFile> data_file ( TFile::Open(m_HistogramsFilePath.c_str()));
    if(!data_file || data_file->IsZombie()){
        ATH_MSG_FATAL( "Could not open the first input file: " << m_HistogramsFilePath );
        return StatusCode::FAILURE;
    }




    TString CaloResponseMap_path = settings.GetValue("CaloResponseMap","");

    if(CaloResponseMap_path.IsNull())
    {
        ATH_MSG_ERROR("Cannot find the CaloResponseMap in the config file");
        return StatusCode::FAILURE;
    }

    m_CALO_ResponseMap  = std::make_unique<TH2D>(*dynamic_cast<TH2D*>(data_file->Get( CaloResponseMap_path )));
    m_CALO_ResponseMap->SetDirectory(0);


    if(m_MassDef==JetTools::FFJetAllowedMassDefEnum::Comb || m_MassDef==JetTools::FFJetAllowedMassDefEnum::TA){
        TString TAResponseMap_path = settings.GetValue("TAResponseMap","");

        if(TAResponseMap_path.IsNull())
        {
            ATH_MSG_ERROR("Cannot find the TAResponseMap in the config file");
            return StatusCode::FAILURE;
        }

        m_TA_ResponseMap  = std::make_unique<TH2D>(*dynamic_cast<TH2D*>(data_file->Get( TAResponseMap_path )));
        m_TA_ResponseMap->SetDirectory(0);//To keep it open when we close the .root file
    }



    //JMS systematics
    for (size_t iComp = 0; iComp < 999; ++iComp)
    {
        const TString prefix = Form("JMSComponent.%zu.",iComp);

        std::string Syst_Name = settings.GetValue(prefix+"Name","");

        if( !Syst_Name.empty()){
            m_SysList.insert( CP::SystematicVariation(Syst_Name, 1) );
            m_SysList.insert( CP::SystematicVariation(Syst_Name, -1) );

            m_Syst_HistPath_map[Syst_Name] = settings.GetValue(prefix+"Hist","");
            m_Syst_MassDefAffected_map[Syst_Name] = settings.GetValue(prefix+"MassDef","");
            m_Syst_TopologyAffected_map[Syst_Name] = settings.GetValue(prefix+"Topology","");
            m_Syst_Affects_JMSorJMR[Syst_Name] = "JMS";

            m_Syst_Hist_map[Syst_Name] = std::make_unique<TH2D>(*dynamic_cast<TH2D*>(data_file->Get(m_Syst_HistPath_map[Syst_Name].c_str())));
            m_Syst_Hist_map[Syst_Name]->SetDirectory(0);

        }
    }
    //JMR Systematics
    for (size_t iComp = 0; iComp < 999; ++iComp)
    {
        const TString prefix = Form("JMRComponent.%zu.",iComp);

        std::string Syst_Name = settings.GetValue(prefix+"Name","");


        if( !Syst_Name.empty()){
            m_SysList.insert( CP::SystematicVariation(Syst_Name, 1) );
            m_SysList.insert( CP::SystematicVariation(Syst_Name, -1) );

            m_Syst_HistPath_map[Syst_Name] = settings.GetValue(prefix+"Hist","");
            m_Syst_MassDefAffected_map[Syst_Name] = settings.GetValue(prefix+"MassDef","");
            m_Syst_TopologyAffected_map[Syst_Name] = settings.GetValue(prefix+"Topology","");
            m_Syst_Affects_JMSorJMR[Syst_Name] = "JMR";

            m_Syst_Hist_map[Syst_Name] = std::make_unique<TH2D>(*dynamic_cast<TH2D*>(data_file->Get(m_Syst_HistPath_map[Syst_Name].c_str())));
            m_Syst_Hist_map[Syst_Name]->SetDirectory(0);

        }
    }

    data_file->Close();


    //Read the Calo and TA mass weight histograms from the same file that JetUncertainties uses


    TString Calo_TA_weight_file_name = settings.GetValue("JetUncertainties_UncertaintyRootFile","");
    const TString Calo_TA_weight_file_path = jet::utils::findFilePath(Calo_TA_weight_file_name.Data(),m_path.c_str(),m_calibArea.c_str());

    if (Calo_TA_weight_file_path.IsNull())
    {
        ATH_MSG_ERROR("Cannot find the file with the Calo and TA weights");
        return StatusCode::FAILURE;
    }

    TString Calo_weight_hist_name = settings.GetValue("CombMassWeightCaloHist","");
    if (Calo_weight_hist_name.IsNull())
    {
        ATH_MSG_ERROR("Cannot find the histogram name that contains the Calo weights in the config file");
        return StatusCode::FAILURE;
    }

    TString TA_weight_hist_name = settings.GetValue("CombMassWeightTAHist","");
    if (TA_weight_hist_name.IsNull())
    {
        ATH_MSG_ERROR("Cannot find the histogram name that contains the TA weights in the config file");
        return StatusCode::FAILURE;
    }

    ATH_MSG_INFO(Form("  Calo weights hist: \"%s\"",Calo_weight_hist_name.Data()));
    ATH_MSG_INFO(Form("  TA weights hist: \"%s\"",TA_weight_hist_name.Data()));
    ATH_MSG_INFO(Form("    Location: %s",Calo_TA_weight_file_path.Data()));


    std::unique_ptr<TFile> Calo_TA_weight_file ( TFile::Open(Calo_TA_weight_file_path));
    if(!Calo_TA_weight_file || Calo_TA_weight_file->IsZombie()){
        ATH_MSG_FATAL( "Could not open the first input file: " << Calo_TA_weight_file_path );
        return StatusCode::FAILURE;
    }

    m_caloMassWeight = std::make_unique<TH3F>(*dynamic_cast<TH3F*>(Calo_TA_weight_file->Get(Calo_weight_hist_name)));
    m_TAMassWeight = std::make_unique<TH3F>(*dynamic_cast<TH3F*>(Calo_TA_weight_file->Get(TA_weight_hist_name)));

    m_caloMassWeight->SetDirectory(0);
    m_TAMassWeight->SetDirectory(0);//To keep it open when we close the .root file


    Calo_TA_weight_file->Close();

    return StatusCode::SUCCESS;
}





//-----------------------------------------------------------------------------
// The function "getMatchedTruthJet" finds the truth jet that match with the given jet_reco and it save it in the given jet_truth_matched jet.
//-----------------------------------------------------------------------------

StatusCode FFJetSmearingTool::getMatchedTruthJet(xAOD::Jet jet_reco, xAOD::Jet& jet_truth_matched){

    // Get the truth jets of the event
    const xAOD::JetContainer* jets_truth = nullptr;

    ATH_CHECK(evtStore()->retrieve( jets_truth, m_truth_jetColl));//If fail, it means that we are "Unable to retrieve jetColl Info". It shows an Error message and exits

    double dRmax_truthJet = 0.75;// matching condition
    double dRmin=9999; //we will take the closest jet reco-truth

    //Loop over the truth jets in the event to match
    const xAOD::Jet* close_jet = nullptr;
    for (const auto& jet_truth : *jets_truth) {
        float dR_Test = jet_reco.p4().DeltaR(jet_truth->p4());
        if ( dR_Test < dRmax_truthJet){
            if(dR_Test < dRmin){
                close_jet = jet_truth;
                dRmin = dR_Test;
            }
        }
    }
    if(dRmin > 999){ return StatusCode::FAILURE;}

    jet_truth_matched.setJetP4(close_jet->jetP4());
    return StatusCode::SUCCESS;
} 


//-----------------------------------------------------------------------------
// The function "getJetTopology" gets the topology of the given jet. "QCD" jets have a extra source of uncertainties called "MODELLINGUNCERTAINTIESQCDJETS".
//-----------------------------------------------------------------------------

StatusCode FFJetSmearingTool::getJetTopology( xAOD::Jet& jet_reco, std::string& jetTopology) const
{

    const SG::AuxElement::ConstAccessor<int> accTruthLabel(m_truthlabelaccessor);
    if (!accTruthLabel.isAvailable(jet_reco) )
    {
        ATH_MSG_ERROR("Unable to retrieve the FatjetTruthLabel from the jet.  Please call the BoostedJetTaggers decorateTruthLabel() function before calling this function.");
        return StatusCode::FAILURE;
    }


LargeRJetTruthLabel::TypeEnum jetTruthLabel = LargeRJetTruthLabel::intToEnum(accTruthLabel(jet_reco));

    if(jetTruthLabel == LargeRJetTruthLabel::tqqb || jetTruthLabel == LargeRJetTruthLabel::other_From_t)
    {
        jetTopology="Top";
    }

    else if(jetTruthLabel == LargeRJetTruthLabel::Wqq || jetTruthLabel == LargeRJetTruthLabel::Zqq || jetTruthLabel == LargeRJetTruthLabel::Wqq_From_t || jetTruthLabel == LargeRJetTruthLabel::other_From_V)
    {
        jetTopology="V";
    }

    else if(jetTruthLabel == LargeRJetTruthLabel::qcd)   
    {
        jetTopology="QCD";
    }

    else if(jetTruthLabel == LargeRJetTruthLabel::Hbb || jetTruthLabel == LargeRJetTruthLabel::other_From_H)
    {
        jetTopology="H";
    }

    else if(jetTruthLabel == LargeRJetTruthLabel::notruth)    
    {
        jetTopology="no_match";
        ATH_MSG_DEBUG("No truth jet match with this reco jet. The jet will not be smeared.");
    }
  
    else jetTopology="QCD"; //We should never arrive here 

    ATH_MSG_VERBOSE("The topology of this jet correspond to a " << jetTopology << " large-R jet");

    return StatusCode::SUCCESS;
}




//-----------------------------------------------------------------------------
// The function "getJMSJMR" read the JMS and JMR uncertainties associated with the systematic 
//-----------------------------------------------------------------------------

StatusCode FFJetSmearingTool::getJMSJMR( xAOD::Jet& jet_reco, double jet_mass_value, JetTools::FFJetAllowedMassDefEnum MassDef_of_syst, std::string jetTopology, double& JMS_err,  double& JMR_err){

    //JMS/JMR systematic variations
    JMS_err=0;
    JMR_err=0;

    if(m_Syst_MassDefAffected_map[m_currentSysData->SysBaseName] ==  JetTools::enumToString(MassDef_of_syst)){
        ATH_MSG_VERBOSE("This uncertainty affects to the " << JetTools::enumToString(MassDef_of_syst) << " mass");
    } //Only apply the systematic to the proper mass definition
    else{return StatusCode::SUCCESS;}



    if(m_Syst_TopologyAffected_map[m_currentSysData->SysBaseName] != "All" && m_Syst_TopologyAffected_map[m_currentSysData->SysBaseName] != jetTopology){
        ATH_MSG_VERBOSE("The systematic do not affects to this jet topology");
        return StatusCode::SUCCESS;
    }


    float jet_mass = jet_mass_value*m_MeVtoGeV;//jet_reco->m()*m_MeVtoGeV; The TA mass can not be extracted this way
    float jet_pT = jet_reco.pt()*m_MeVtoGeV;


    if(m_Syst_Affects_JMSorJMR[m_currentSysData->SysBaseName] == "JMS"){

        JMS_err=m_Syst_Hist_map[m_currentSysData->SysBaseName]->GetBinContent(m_Syst_Hist_map[m_currentSysData->SysBaseName]->GetXaxis()->FindBin(jet_pT),m_Syst_Hist_map[m_currentSysData->SysBaseName]->GetYaxis()->FindBin(jet_mass)) * m_currentSysData->SysParameter;

        JMR_err= 0;
    }

    if(m_Syst_Affects_JMSorJMR[m_currentSysData->SysBaseName] == "JMR"){

        JMS_err=0;
        JMR_err= m_Syst_Hist_map[m_currentSysData->SysBaseName]->GetBinContent(m_Syst_Hist_map[m_currentSysData->SysBaseName]->GetXaxis()->FindBin(jet_pT),m_Syst_Hist_map[m_currentSysData->SysBaseName]->GetYaxis()->FindBin(jet_mass)) * m_currentSysData->SysParameter;
    }





    ATH_MSG_DEBUG("Systematic applied: " << m_currentSysData->SysBaseName);

    ATH_MSG_VERBOSE("JMS_err: " << JMS_err);
    ATH_MSG_VERBOSE("JMR_err: " << JMR_err);

    return StatusCode::SUCCESS;
}



//-----------------------------------------------------------------------------
// Once the tool is initialized. The user can call the function "SmearJetMass" to perform the jet smearing using FF (for the current systematic in his loop)
//-----------------------------------------------------------------------------

CP::CorrectionCode FFJetSmearingTool::applyCorrection( xAOD::Jet& jet_reco){

    ATH_MSG_VERBOSE("//---------------------------------------------------------------//");
    ATH_MSG_VERBOSE("Reco Jet to Smear: pt = " << jet_reco.pt() << ", mass = " << jet_reco.m() << ", eta = " << jet_reco.eta());

    if(std::abs(jet_reco.eta()) > m_EtaRange){//JetCalibTools do not properly for jets with |eta|>2
        ATH_MSG_DEBUG("This jet exceeds the eta range that the tool allows (|eta|<" << m_EtaRange << ")");
        return CP::CorrectionCode::OutOfValidityRange; 
    }
    if(jet_reco.m() > m_MaxMass){
        ATH_MSG_DEBUG("This jet exceeds the mass range that the tool allows jet_mass <" << m_MaxMass << " MeV)");
        return CP::CorrectionCode::OutOfValidityRange;
    }
    if(jet_reco.pt() > m_MaxPt){
        ATH_MSG_DEBUG("This jet exceeds the maximum pt that the tool allows jet_pt <" << m_MaxPt << " MeV)");
        return CP::CorrectionCode::OutOfValidityRange;
    }

    //Find matched truth jet
    xAOD::Jet jet_truth_matched;
    jet_truth_matched.makePrivateStore();

    if(!(getMatchedTruthJet(jet_reco, jet_truth_matched).isSuccess())){

        m_InfoWarnings = m_InfoWarnings + 1; 
        if(m_InfoWarnings < 5) ATH_MSG_VERBOSE("No truth jet match with this reco jet. The jet will not be smeared.");
        else if(m_InfoWarnings == 5) ATH_MSG_VERBOSE("No truth jet match with this reco jet. The jet will not be smeared. This is the last time this message is shown.");

        return CP::CorrectionCode::OutOfValidityRange; 
    }

    ATH_MSG_VERBOSE("Matched truth Jet: pt = " << jet_truth_matched.pt() << ", mass = " << jet_truth_matched.m() << ", eta = " << jet_truth_matched.eta());

    //Get the jet topology
    std::string jetTopology;

    if(!(getJetTopology( jet_reco, jetTopology)).isSuccess()){
        ATH_MSG_ERROR("Imposible to obtain the jetTopology");
        return CP::CorrectionCode::Error;
    }
    if(jetTopology == "no_match"){
        return CP::CorrectionCode::OutOfValidityRange;
    }


    //The TA mass is saved in an attribute so you can not access to it using ->m(). (if calibrated as Calo mass and not as Combined Mass), The Calo mass is not set as an attribute so you can not access it using ->getAttribute . 

    double jet_mass_CALO = 0;
    double jet_mass_TA = 0;
    double calo_mass_weight=1; // m_comb = Weight*m_Calo + (1-Weight)*m_TA

    float JetTrackAssistedMassCalibrated_from_JetCalibTools;

    if(m_MassDef==JetTools::FFJetAllowedMassDefEnum::Comb){

        xAOD::JetFourMom_t jet_reco_CALO;
        xAOD::JetFourMom_t jet_reco_TA;
        xAOD::JetFourMom_t jet_reco_Comb;

        jet_reco.getAttribute<xAOD::JetFourMom_t>("JetJMSScaleMomentumCalo",jet_reco_CALO);
        jet_reco.getAttribute<xAOD::JetFourMom_t>("JetJMSScaleMomentumTA",jet_reco_TA);
        jet_reco.getAttribute<xAOD::JetFourMom_t>("JetJMSScaleMomentumCombQCD",jet_reco_Comb);

        ATH_MSG_VERBOSE("CALO jet mass " << jet_reco_CALO.mass());
        ATH_MSG_VERBOSE("TA jet mass " << jet_reco_TA.mass() );
        ATH_MSG_VERBOSE("Comb jet mass " << jet_reco_Comb.mass() );

        jet_mass_CALO = jet_reco_CALO.mass();
        jet_mass_TA = jet_reco_TA.mass();

        jet_reco.getAttribute<float>("JetTrackAssistedMassCalibrated", JetTrackAssistedMassCalibrated_from_JetCalibTools);
    }
    if(m_MassDef==JetTools::FFJetAllowedMassDefEnum::Calo){
        jet_mass_CALO = jet_reco.m();
        calo_mass_weight = 1;
    }
    else if(m_MassDef==JetTools::FFJetAllowedMassDefEnum::TA){
        jet_mass_TA = jet_reco.m();
        calo_mass_weight = 0;
    }




    //Obtain the average mass response of the jet. The response will depend in the chosed topology (top,W or QCD) and also in the mass definition (CALO, TA, Combined). By default the map used correspond to QCD jets

    double avg_response_CALO=1;
    double avg_response_TA=1;

    if(m_MassDef==JetTools::FFJetAllowedMassDefEnum::Comb || m_MassDef==JetTools::FFJetAllowedMassDefEnum::Calo){

        if(m_CALO_ResponseMap->GetBinContent(m_CALO_ResponseMap->GetXaxis()->FindBin(jet_reco.pt()*m_MeVtoGeV),m_CALO_ResponseMap->GetYaxis()->FindBin(jet_truth_matched.m()*m_MeVtoGeV)) == 0){//If we look outside the Th2 histogram, we would obtain a 0 so we apply the nominal response (1)
        avg_response_CALO=1;
        }
        else{
            avg_response_CALO = m_CALO_ResponseMap->GetBinContent(m_CALO_ResponseMap->GetXaxis()->FindBin(jet_reco.pt()*m_MeVtoGeV),m_CALO_ResponseMap->GetYaxis()->FindBin(jet_truth_matched.m()*m_MeVtoGeV));
        }
    }

    if(m_MassDef==JetTools::FFJetAllowedMassDefEnum::Comb || m_MassDef==JetTools::FFJetAllowedMassDefEnum::TA){

        if(m_TA_ResponseMap->GetBinContent(m_TA_ResponseMap->GetXaxis()->FindBin(jet_reco.pt()*m_MeVtoGeV),m_TA_ResponseMap->GetYaxis()->FindBin(jet_truth_matched.m()*m_MeVtoGeV))==0){

            avg_response_TA=1;
        }
        else{           
            avg_response_TA = m_TA_ResponseMap->GetBinContent(m_TA_ResponseMap->GetXaxis()->FindBin(jet_reco.pt()*m_MeVtoGeV),m_TA_ResponseMap->GetYaxis()->FindBin(jet_truth_matched.m()*m_MeVtoGeV));
        }
    }


    //Obtain the jet mass scale (JMS) and the jet mass resolution (JMR) nominal values and variation that correspond to the jet_reco

    double JMS (1), JMS_err(0), JMR(1), JMR_err(0);
    double scale;
    double resolution;

    double smeared_CALO_mass = jet_mass_CALO;
    double smeared_TA_mass = jet_mass_TA;

    bool is_CALO_mass_smeared = false;
    bool is_TA_mass_smeared = false;

    if(m_MassDef==JetTools::FFJetAllowedMassDefEnum::Comb || m_MassDef==JetTools::FFJetAllowedMassDefEnum::Calo){

        if(!(getJMSJMR( jet_reco, jet_mass_CALO, JetTools::FFJetAllowedMassDefEnum::Calo,jetTopology, JMS_err, JMR_err)).isSuccess()){
            return CP::CorrectionCode::Ok;
        }	

        scale = JMS + JMS_err;
        resolution = JMR + JMR_err;

        if(TMath::Abs(scale-1) > 0.0001 || TMath::Abs(resolution-1)  > 0.0001){
            is_CALO_mass_smeared = true;

            ATH_MSG_VERBOSE("Forward Folding CALO procedure will use scale=" << scale << ", resolution=" << resolution << " and average respose=" << avg_response_CALO);

            //FF procedure
            smeared_CALO_mass = jet_mass_CALO * scale + (jet_mass_CALO - avg_response_CALO*jet_truth_matched.m())*(resolution-scale);//FF formula
        }

    }

    if(m_MassDef==JetTools::FFJetAllowedMassDefEnum::Comb || m_MassDef==JetTools::FFJetAllowedMassDefEnum::TA){
        if(!(getJMSJMR( jet_reco, jet_mass_TA, JetTools::FFJetAllowedMassDefEnum::TA,jetTopology, JMS_err, JMR_err))){
            return CP::CorrectionCode::Ok;
        }

        scale = JMS + JMS_err;
        resolution = JMR + JMR_err;

        if(TMath::Abs(scale-1) > 0.0001 || TMath::Abs(resolution-1)  > 0.0001){

            is_TA_mass_smeared = true;

            ATH_MSG_VERBOSE("Forward Folding TA procedure will use scale=" << scale << ", resolution=" << resolution << " and average respose=" << avg_response_TA);

            //FF procedure
            smeared_TA_mass = jet_mass_TA * scale + (jet_mass_TA - avg_response_TA*jet_truth_matched.m())*(resolution-scale);//FF formula
        }

    }

    if(is_CALO_mass_smeared==false && is_TA_mass_smeared == false){//We only smear the jet if we have to. If not, avoid doing extra calculations

        ATH_MSG_VERBOSE("This jet is not affected by the systematic. The jet won't be modified");
        ATH_MSG_VERBOSE("//---------------------------------------------------------------//");

        return CP::CorrectionCode::Ok;
    }


    //Recalculate the weights after the smearing

    if(m_MassDef==JetTools::FFJetAllowedMassDefEnum::Comb && JetTrackAssistedMassCalibrated_from_JetCalibTools != 0 && jet_mass_CALO != 0){
                                        //we check that JetTrackAssistedMassCalibrated_from_JetCalibTools != 0 instead of jet_mass_TA != 0 becuase
                                        //there is a problem in the conversion between the mass itself and the four-vector representation (due to a
                                        //limitation of floating). This makes the value of jet_mass_TA!=0 in situations where it should be 0.
                                        //In order to work arround it we check JetTrackAssistedMassCalibrated_from_JetCalibTools != 0 insead.
        double caloRes;
        double TARes;

        xAOD::JetFourMom_t jet_reco_CALO;
        xAOD::JetFourMom_t jet_reco_TA;

        jet_reco.getAttribute<xAOD::JetFourMom_t>("JetJMSScaleMomentumCalo",jet_reco_CALO);
        jet_reco.getAttribute<xAOD::JetFourMom_t>("JetJMSScaleMomentumTA",jet_reco_TA);


        xAOD::JetFourMom_t p4_aux;

        //The smearing do not change the pt but it changes the mass (so the energy too) so, itf we want to perform the smearing properly, we have to change the Calo and TA foru m	//omenta before looking at the weights map
        //omenta before looking at the weights map
        p4_aux = xAOD::JetFourMom_t(jet_reco_CALO.pt(),jet_reco_CALO.eta(),jet_reco_CALO.phi(),smeared_CALO_mass);//The smearing do not change the pt but it changes the Energy 
        jet_reco_CALO = p4_aux;

        p4_aux = xAOD::JetFourMom_t(jet_reco_TA.pt(),jet_reco_TA.eta(),jet_reco_TA.phi(),smeared_TA_mass);
        jet_reco_TA = p4_aux;

        caloRes=FFJetSmearingTool::Read3DHistogram(m_caloMassWeight.get() ,jet_reco_CALO.e()*m_MeVtoGeV,TMath::Log(jet_reco_CALO.M()/jet_reco_CALO.e()),std::abs(jet_reco_CALO.eta()));

        TARes=FFJetSmearingTool::Read3DHistogram(m_TAMassWeight.get() ,jet_reco_TA.e()*m_MeVtoGeV,TMath::Log(jet_reco_TA.M()/jet_reco_TA.e()),std::abs(jet_reco_TA.eta()));

        //The histograms with the weights that we are reading was deffined with the code "e_LOGmOe_eta" what means that each axis correspond to:
        //-X: Jet Energy
        //-Y: Log(Jet_Energy/Jet_mass)
        //-Z:Eta
        //Domain is [200-6000],[-6,0],[0,2] but, the ReadHistogram function put the value of the extream of the histogram to the values outside the domain.
        //We have to use a custom "My_Interpolate" because the Z axis has just one bin (and this makes the Root Interpolate function fail) 

        double caloFactor;
        double TAFactor;

        if (caloRes == 0 ) { caloFactor = 0; TAFactor = 1;}
        else if( TARes == 0) { caloFactor = 1; TAFactor = 0;}
        else{
            caloFactor = 1./(caloRes*caloRes);
            TAFactor   = 1./(TARes*TARes);
        }


        calo_mass_weight = caloFactor /(caloFactor + TAFactor);

        ATH_MSG_VERBOSE(" Map Calo weight = " << calo_mass_weight  );
        ATH_MSG_VERBOSE(" Map TA weight = " << 1 - calo_mass_weight  );
    }
    else if( JetTrackAssistedMassCalibrated_from_JetCalibTools == 0){calo_mass_weight = 1;}
    else if(jet_mass_CALO == 0){calo_mass_weight = 0;}

    double smeared_mass = calo_mass_weight*smeared_CALO_mass + (1 - calo_mass_weight)*smeared_TA_mass;

    ATH_MSG_VERBOSE("Smeared CALO mass " << smeared_CALO_mass);
    ATH_MSG_VERBOSE("Smeared TA mass " << smeared_TA_mass);

    xAOD::JetFourMom_t p4 = jet_reco.jetP4();


    p4 = xAOD::JetFourMom_t(jet_reco.pt(),jet_reco.eta(),jet_reco.phi(),smeared_mass);
    jet_reco.setJetP4(p4);


    ATH_MSG_VERBOSE("Smeared Reco Jet: pt = " << jet_reco.pt() << ", mass = " << jet_reco.m() << ", eta = " << jet_reco.eta());
	



    ATH_MSG_VERBOSE("//---------------------------------------------------------------//");

    return CP::CorrectionCode::Ok;
}







//Functions from JetUncertainties. We copy them in order to read the map exactly as it is done in JetUncertainties and get EXACTLY the same result

double FFJetSmearingTool::Read3DHistogram(const TH3* histo, double x, double y, double z) const
{

  double aux_x = x;
  double aux_y = y;
  double aux_z = z;


  // Asymptotic values
  //If the value is outside the histogram region, we take the closest value to that one

  double xMax = histo->GetXaxis()->GetBinLowEdge(histo->GetNbinsX()+1);
  double xMin = histo->GetXaxis()->GetBinLowEdge(1);
  double yMax = histo->GetYaxis()->GetBinLowEdge(histo->GetNbinsY()+1);
  double yMin = histo->GetYaxis()->GetBinLowEdge(1);
  double zMax = histo->GetZaxis()->GetBinLowEdge(histo->GetNbinsZ()+1);
  double zMin = histo->GetZaxis()->GetBinLowEdge(1);

  if(x >= xMax) aux_x = xMax-1e-6 ; //so it fits the up-most x-bin
  if(x <= xMin) aux_x = xMin+1e-6 ; //so it fits the low-most x-bin
  if ( std::isnan(y)) return 0; // no weight if the input is NaN, can happen for log(X)
  if(y >= yMax) aux_y = yMax-1e-6 ; //so it fits the up-most y-bin
  if(y <= yMin) aux_y = yMin+1e-6 ; //so it fits the low-most y-bin
  if(z >= zMax) aux_z = zMax-1e-6 ; //so it fits the up-most z-bin
  if(z <= zMin) aux_z = zMin+1e-6 ; //so it fits the low-most z-bin

  //Use the interpolate function from JetHelpers.cxx
  double weight = JetHelpers::Interpolate(histo, aux_x, aux_y, aux_z);


  return weight;
}

} // namespace CP
