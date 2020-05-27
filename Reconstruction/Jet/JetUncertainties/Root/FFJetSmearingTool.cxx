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


// Constructor
FFJetSmearingTool::FFJetSmearingTool(const std::string name)
    : asg::AsgTool(name) 
    , m_isInit(false)
    , m_name(name)
    , m_release("")
    , m_truth_jetColl("")
    , m_EtaRange(0)
    , m_calibArea("CalibArea-08")
    , m_histFileName("")
    , m_InfoWarnings(0)
    {
    declareProperty( "MassDef", m_MassDef = ""                             );
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
        ATH_MSG_FATAL(Form("Blocking re-initialization of tool named %s",m_name.c_str()));
        return StatusCode::FAILURE;
    }

    ATH_MSG_INFO(Form("Preparing to initialize the FFJetSmearingTool named %s",m_name.c_str()));

    if (m_name=="")
    {
        ATH_MSG_FATAL("No name specified.  Aborting.");
        return StatusCode::FAILURE;
    }
    if (m_MassDef == "")
    {
        ATH_MSG_FATAL("No kind of jet mass specified.  Aborting.");
        return StatusCode::FAILURE;
    }


    //reading the config file as in JetUncertaintiesTool
    TEnv settings;


    const TString configFilePath = jet::utils::findFilePath(m_configFile.c_str(),m_path.c_str(),m_calibArea.c_str());

    if (settings.ReadFile( configFilePath.Data(),kEnvGlobal))
    {
        ATH_MSG_ERROR("Cannot read config file: " << configFilePath.Data());
        return StatusCode::FAILURE;
    }
    // We can read it - start printing
    ATH_MSG_INFO(Form("================================================"));
    ATH_MSG_INFO(Form("  Initializing the FFJetSmearingTool named %s",m_name.c_str()));
    ATH_MSG_INFO(Form("  Configuration file: \"%s\"",m_configFile.c_str()));
    ATH_MSG_INFO(Form("    Location: %s",configFilePath.Data()));


    m_release = settings.GetValue("UncertaintyRelease","UNKNOWN");
    ATH_MSG_INFO(Form("  Uncertainty release: %s",m_release.c_str()));

    // Check the jet definition
    m_truth_jetColl = settings.GetValue("TruthJetColl","");
    if (m_truth_jetColl == "")
    {
        ATH_MSG_ERROR("Cannot find the truth jet collection to use in config");
        return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("  Truth Jet Collection: " << m_truth_jetColl);
    // Check the name of the truth label accessor for BoostjetTaggers
    m_truthlabelaccessor = settings.GetValue("TruthLabelAccessor","");
    if (m_truthlabelaccessor == "")
    {
        ATH_MSG_ERROR("Cannot find the TruthLabelAccessor to use in config");
        return StatusCode::FAILURE;
    }
    ATH_MSG_INFO(" Truth Label Accessor: " << m_truthlabelaccessor);
    //eta range of the tool  
    m_EtaRange = settings.GetValue("EtaRange",0);
    if (m_EtaRange == 0)
    {
        ATH_MSG_ERROR("Cannot find the EtaRange  in the config file");
        return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("  EtaRange : Abs(eta) < " << m_EtaRange);
    //mass range of the tool  
    m_MassRange = settings.GetValue("MassRange",0);
    if (m_MassRange == 0)
    {
        ATH_MSG_ERROR("Cannot find the MassRange  in the config file");
        return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("  MassRange : jet_mass < " << m_MassRange);
    //pt range of the tool 
    m_PtRange = settings.GetValue("PtRange",0);
    if (m_PtRange == 0)
    {
        ATH_MSG_ERROR("Cannot find the PtRange  in the config file");
        return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("  PtRange : jet_pt < " << m_PtRange);
    // Get the file to read uncertainties in from
    m_histFileName = settings.GetValue("UncertaintyRootFile","");
    m_HistogramsFilePath = jet::utils::findFilePath(m_histFileName.Data(),m_path.c_str(),m_calibArea.c_str());
    if (m_histFileName == "")
    {
        ATH_MSG_ERROR("Cannot find uncertainty histogram file in the config file");
        return StatusCode::FAILURE;
    }
    ATH_MSG_INFO(Form("  UncertaintyFile: \"%s\"",m_histFileName.Data()));
    ATH_MSG_INFO(Form("    Location: %s",m_HistogramsFilePath.Data()));



    //Read all the histogram files where the jms jmr variations are saved
    if(!(readFFJetSmearingToolSimplifiedData(settings).isSuccess())){
        ATH_MSG_WARNING("Error reading " << m_HistogramsFilePath);
        return StatusCode::FAILURE;
    }



    // Make sure we have a valid systematic mode
    if(m_MassDef != "Calo" && m_MassDef != "TA" && m_MassDef != "Comb"){
      ATH_MSG_ERROR("No Systematics associated to this mass definition: " << m_MassDef);
      ATH_MSG_ERROR("Should be 'Calo', 'TA' or 'Comb'");
      return StatusCode::FAILURE;
    }

    // Add the affecting systematics to the global registry
    CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
    if(registry.registerSystematics(*this) != CP::SystematicCode::Ok){
      ATH_MSG_ERROR("Unable to register systematics!");
      return StatusCode::FAILURE;
    }

    m_isInit = true;

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
    static CP::SystematicSet sys = affectingSystematics();
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
    // Look for it in our filter map.
    auto sysMapItr = m_sysFilterMap.find(systematics);

    // If this is a new input set, we need to filter it.
    if(sysMapItr == m_sysFilterMap.end()){

        // Filter the input systematics with my affecting systematics.
        // Should be ok to use static here, as the affecting systematics should be
        // the same across any instance of this tool.
        static const CP::SystematicSet affectingSysts = affectingSystematics();
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

        // Insert the new filtered set onto our filter map
        sysMapItr = m_sysFilterMap.insert
        (std::make_pair(systematics, filteredSysts)).first;

    }

    // Apply the filtered systematics
    m_sysConfig = &sysMapItr->second;

    return CP::SystematicCode::Ok;
}



//-----------------------------------------------------------------------------
// Read the external file that conatins the JSS recomendations for FatJets
//-----------------------------------------------------------------------------

StatusCode FFJetSmearingTool::readFFJetSmearingToolSimplifiedData(TEnv& settings){


    TString data_file_location =  m_HistogramsFilePath;//m_ConfigFile;

    TFile data_file(data_file_location,"READ");
    if(data_file.IsOpen()==false){
        ATH_MSG_FATAL( "Unable to open " << data_file_location );
        return StatusCode::FAILURE;
    }




    TString CaloResponseMap_path = settings.GetValue("CaloResponseMap","");

    if(CaloResponseMap_path == "")
    {
        ATH_MSG_ERROR("Cannot find the CaloResponseMap in the config file");
        return StatusCode::FAILURE;
    }

    m_CALO_ResponseMap  = (TH2D*)data_file.Get( CaloResponseMap_path  );
    m_CALO_ResponseMap->SetDirectory(0);


    if(m_MassDef=="Comb" || m_MassDef=="TA"){
        TString TAResponseMap_path = settings.GetValue("TAResponseMap","");

        if(TAResponseMap_path == "")
        {
            ATH_MSG_ERROR("Cannot find the TAResponseMap in the config file");
            return StatusCode::FAILURE;
        }

        m_TA_ResponseMap  = (TH2D*)data_file.Get( TAResponseMap_path   );
        m_TA_ResponseMap->SetDirectory(0);//To keep it open when we close the .root file
    }



    //JMS systematics
    for (size_t iComp = 0; iComp < 999; ++iComp)
    {
        const TString prefix = Form("JMSComponent.%zu.",iComp);

        std::string Syst_Name = settings.GetValue(prefix+"Name","");

        if( Syst_Name != ""){
            m_SysList.insert( CP::SystematicVariation(Syst_Name, 1) );
            m_SysList.insert( CP::SystematicVariation(Syst_Name, -1) );

            m_Syst_HistPath_map[Syst_Name] = settings.GetValue(prefix+"Hist","");
            m_Syst_MassDefAffected_map[Syst_Name] = settings.GetValue(prefix+"MassDef","");
            m_Syst_TopologyAffected_map[Syst_Name] = settings.GetValue(prefix+"Topology","");
            m_Syst_Affects_JMSorJMR[Syst_Name] = "JMS";


            m_Syst_Hist_map[Syst_Name] = (TH2D*)data_file.Get(m_Syst_HistPath_map[Syst_Name].c_str());
            m_Syst_Hist_map[Syst_Name]->SetDirectory(0);


        }
    }
    //JMR Systematics
    for (size_t iComp = 0; iComp < 999; ++iComp)
    {
        const TString prefix = Form("JMRComponent.%zu.",iComp);

        std::string Syst_Name = settings.GetValue(prefix+"Name","");


        if( Syst_Name != ""){
            m_SysList.insert( CP::SystematicVariation(Syst_Name, 1) );
            m_SysList.insert( CP::SystematicVariation(Syst_Name, -1) );

            m_Syst_HistPath_map[Syst_Name] = settings.GetValue(prefix+"Hist","");
            m_Syst_MassDefAffected_map[Syst_Name] = settings.GetValue(prefix+"MassDef","");
            m_Syst_TopologyAffected_map[Syst_Name] = settings.GetValue(prefix+"Topology","");
            m_Syst_Affects_JMSorJMR[Syst_Name] = "JMR";


            m_Syst_Hist_map[Syst_Name] = (TH2D*)data_file.Get(m_Syst_HistPath_map[Syst_Name].c_str());
            m_Syst_Hist_map[Syst_Name]->SetDirectory(0);


        }
    }

    data_file.Close();


    //Read the Calo and TA mass weight histograms from the same file that JetUncertainties uses


    TString Calo_TA_weight_file_name = settings.GetValue("JetUncertainties_UncertaintyRootFile","");
    const TString Calo_TA_weight_file_path = jet::utils::findFilePath(Calo_TA_weight_file_name.Data(),m_path.c_str(),m_calibArea.c_str());

    if (Calo_TA_weight_file_path == "")
    {
        ATH_MSG_ERROR("Cannot find the file with the Calo and TA weights");
        return StatusCode::FAILURE;
    }

    TString Calo_weight_hist_name = settings.GetValue("CombMassWeightCaloHist","");
    if (Calo_weight_hist_name == "")
    {
        ATH_MSG_ERROR("Cannot find the histogram name that contains the Calo weights in the config file");
        return StatusCode::FAILURE;
    }

    TString TA_weight_hist_name = settings.GetValue("CombMassWeightTAHist","");
    if (TA_weight_hist_name == "")
    {
        ATH_MSG_ERROR("Cannot find the histogram name that contains the TA weights in the config file");
        return StatusCode::FAILURE;
    }

    ATH_MSG_INFO(Form("  Calo weights hist: \"%s\"",Calo_weight_hist_name.Data()));
    ATH_MSG_INFO(Form("  TA weights hist: \"%s\"",TA_weight_hist_name.Data()));
    ATH_MSG_INFO(Form("    Location: %s",Calo_TA_weight_file_path.Data()));



    TFile Calo_TA_weight_file(Calo_TA_weight_file_path,"READ");
    if(Calo_TA_weight_file.IsOpen()==false){
        ATH_MSG_FATAL( "Unable to open " << Calo_TA_weight_file_path );
        return StatusCode::FAILURE;
    }

    m_caloMassWeight = (TH3F*)Calo_TA_weight_file.Get(Calo_weight_hist_name);
    m_TAMassWeight = (TH3F*)Calo_TA_weight_file.Get(TA_weight_hist_name);

    m_caloMassWeight->SetDirectory(0);
    m_TAMassWeight->SetDirectory(0);//To keep it open when we close the .root file


    Calo_TA_weight_file.Close();

    return StatusCode::SUCCESS;
}





//-----------------------------------------------------------------------------
// The function "getMatchedTruthJet" finds the truth jet that match with the given jet_reco and it save it in the given jet_truth_matched jet.
//-----------------------------------------------------------------------------

StatusCode FFJetSmearingTool::getMatchedTruthJet(xAOD::Jet* jet_reco, xAOD::Jet& jet_truth_matched){

    // Get the truth jets of the event
    const xAOD::JetContainer* jets_truth = 0;

    if (evtStore()->retrieve( jets_truth, m_truth_jetColl).isFailure()){
        ATH_MSG_FATAL( "Unable to retrieve jetcoll Info" );
        return StatusCode::FAILURE;
    }

    double dRmax_truthJet = 1.00;// matching condition
    double dRmin=9999; //we will take the closest jet reco-truth

    //Loop over the truth jets in the event to match
    xAOD::JetContainer::const_iterator jetItr;
    for(jetItr = jets_truth->begin(); jetItr != jets_truth->end(); ++jetItr){

        const xAOD::Jet* jet_truth = *jetItr;

        double dR=jet_reco->p4().DeltaR(jet_truth->p4());

        if(dR < dRmax_truthJet){
            if( dR < dRmin ){
                dRmin=dR;
                xAOD::JetFourMom_t p4 = jet_truth->jetP4();
                jet_truth_matched.setJetP4(p4);
            }
        }
    }

    if(dRmin < 9999)  return StatusCode::SUCCESS; //A matching truth jet has been found

    return StatusCode::FAILURE; //No asociated truth jet has been found in the event
} 


//-----------------------------------------------------------------------------
// The function "getJetTopology" gets the topology of the given jet. "QCD" jets have a extra source of uncertainties called "MODELLINGUNCERTAINTIESQCDJETS".
//-----------------------------------------------------------------------------

StatusCode FFJetSmearingTool::getJetTopology( xAOD::Jet* jet_reco, std::string& jetTopology){

    static const SG::AuxElement::ConstAccessor<int> accTruthLabel(m_truthlabelaccessor);
    if (!accTruthLabel.isAvailable(*jet_reco) )
    {
        ATH_MSG_ERROR("Unable to retrieve the FatjetTruthLabel from the jet.  Please call the BoostedJetTaggers decorateTruthLabel() function before calling this function.");
        return StatusCode::FAILURE;
    }


    int jetTruthLabel = accTruthLabel(*jet_reco);


    if(jetTruthLabel == 1 || jetTruthLabel == 5)
    {
        jetTopology="Top";
    }

    else if(jetTruthLabel == 2 || jetTruthLabel == 3 || jetTruthLabel == 4 || jetTruthLabel == 6)
    {
        jetTopology="V";
    }

    else if(jetTruthLabel == 8)   
    {
        jetTopology="QCD";
    }

    else if(jetTruthLabel == 9 || jetTruthLabel == 10)
    {
        jetTopology="H";
    }

    else if(jetTruthLabel == 7)    
    {
        jetTopology="no_match";
        ATH_MSG_WARNING("No truth jet match with this reco jet");//applyCorrention should fail for the same reason before arriving here. This message should never be shown
    }

    else if(jetTruthLabel == 0)
    {
        return StatusCode::FAILURE;
    }
  
    else jetTopology="QCD"; //We should never arrive here 

    ATH_MSG_VERBOSE("The topology of this jet correspond to a " << jetTopology << " large-R jet");

    return StatusCode::SUCCESS;
}




//-----------------------------------------------------------------------------
// The function "getJMSJMR" read the JMS and JMR uncertainties associated with the systematic 
//-----------------------------------------------------------------------------

StatusCode FFJetSmearingTool::getJMSJMR( xAOD::Jet* jet_reco, double jet_mass_value, std::string CALO_or_TA, std::string jetTopology, double& JMS_err,  double& JMR_err){

    //JMS/JMR systematic variations
    JMS_err=0;
    JMR_err=0;

    // For now, assuming there is only one (affecting) systematic at a time.	
    if(m_sysConfig->size() != 1){
        throw std::logic_error("More than one affecting systematic received");
    }
    const CP::SystematicVariation& sys = *m_sysConfig->begin();


    if(m_Syst_MassDefAffected_map[sys.basename()] == CALO_or_TA){
        if(CALO_or_TA=="Calo")ATH_MSG_VERBOSE("This uncertainty affects to the Calo mass");
        if(CALO_or_TA=="TA")ATH_MSG_VERBOSE("This uncertainty affects to the TA mass");
    } //Only apply the systematic to the proper mass definition
    else{return StatusCode::SUCCESS;}



    if(m_Syst_TopologyAffected_map[sys.basename()] != "All" && m_Syst_TopologyAffected_map[sys.basename()] != jetTopology){
        ATH_MSG_VERBOSE("The systematic do not affects to this jet topology");
        return StatusCode::SUCCESS;
    }


    float jet_mass = jet_mass_value/1000.;//jet_reco->m()/1000.; The TA mass can not be extracted this way
    float jet_pT = jet_reco->pt()/1000.;


    if(m_Syst_Affects_JMSorJMR[sys.basename()] == "JMS"){
        float sysVar = sys.parameter();

        JMS_err=m_Syst_Hist_map[sys.basename()]->GetBinContent(m_Syst_Hist_map[sys.basename()]->GetXaxis()->FindBin(jet_pT),m_Syst_Hist_map[sys.basename()]->GetYaxis()->FindBin(jet_mass)) * sysVar;

        JMR_err= 0;
    }

    if(m_Syst_Affects_JMSorJMR[sys.basename()] == "JMR"){
        float sysVar = sys.parameter();

        JMS_err=0;
        JMR_err= m_Syst_Hist_map[sys.basename()]->GetBinContent(m_Syst_Hist_map[sys.basename()]->GetXaxis()->FindBin(jet_pT),m_Syst_Hist_map[sys.basename()]->GetYaxis()->FindBin(jet_mass)) * sysVar;
    }





    ATH_MSG_DEBUG("Systematic applied: " << sys.basename());

    ATH_MSG_VERBOSE("JMS_err: " << JMS_err);
    ATH_MSG_VERBOSE("JMR_err: " << JMR_err);

    return StatusCode::SUCCESS;
}



//-----------------------------------------------------------------------------
// Once the tool is initialized. The user can call the function "SmearJetMass" to perform the jet smearing using FF (for the current systematic in his loop)
//-----------------------------------------------------------------------------

CP::CorrectionCode FFJetSmearingTool::applyCorrection( xAOD::Jet* jet_reco){

    ATH_MSG_VERBOSE("//---------------------------------------------------------------//");
    ATH_MSG_VERBOSE("Reco Jet to Smear: pt = " << jet_reco->pt()/1000. << ", mass = " << jet_reco->m()/1000. << ", eta = " << jet_reco->eta());

    if(std::abs(jet_reco->eta()) > m_EtaRange){//JetCalibTools do not properly for jets with |eta|>2
      ATH_MSG_VERBOSE("This jet exceeds the eta range that the tool allows (|eta|<" << m_EtaRange << ")");
      return CP::CorrectionCode::OutOfValidityRange; 
    }
    if(jet_reco->m()/1000. > m_MassRange){
        ATH_MSG_VERBOSE("This jet exceeds the mass range that the tool allows jet_mass <" << m_MassRange << " GeV)");
        return CP::CorrectionCode::OutOfValidityRange;
    }
    if(jet_reco->pt()/1000. > m_PtRange){
        ATH_MSG_VERBOSE("This jet exceeds the pt range that the tool allows jet_pt <" << m_PtRange << " GeV)");
        return CP::CorrectionCode::OutOfValidityRange;
    }

    //Find matched truth jet
    xAOD::Jet jet_truth_matched;
    jet_truth_matched.makePrivateStore();

    if(!(getMatchedTruthJet(jet_reco, jet_truth_matched).isSuccess())){

        m_InfoWarnings = m_InfoWarnings + 1; 
        if(m_InfoWarnings < 5) ATH_MSG_WARNING("No truth jet match with this reco jet. The jet will not be smeared.");
        else if(m_InfoWarnings == 5) ATH_MSG_WARNING("No truth jet match with this reco jet. The jet will not be smeared. This is the last time this message is shown.");

        return CP::CorrectionCode::Ok; 
    }

    ATH_MSG_VERBOSE("Matched truth Jet: pt = " << jet_truth_matched.pt()/1000. << ", mass = " << jet_truth_matched.m()/1000. << ", eta = " << jet_truth_matched.eta());

    //Get the jet topology
    std::string jetTopology;

    if(!(getJetTopology( jet_reco, jetTopology)).isSuccess()){
        ATH_MSG_WARNING("Imposible to obtain the jetTopology");
        return CP::CorrectionCode::Ok;
    }
    if(jetTopology == "no_match"){
        return CP::CorrectionCode::Ok;
    }


    //The TA mass is saved in an attribute so you can not access to it using ->m(). (if calibrated as Calo mass and not as Combined Mass), The Calo mass is not set as an attribute so you can not access it using ->getAttribute . 

    double jet_mass_CALO = 0;
    double jet_mass_TA = 0;
    double calo_mass_weight=1; // m_comb = Weight*m_Calo + (1-Weight)*m_TA

    bool use_jetuncertaintiesweight = true;

    if(m_MassDef=="Comb"){

        xAOD::JetFourMom_t jet_reco_CALO;
        xAOD::JetFourMom_t jet_reco_TA;
        xAOD::JetFourMom_t jet_reco_Comb;

        jet_reco->getAttribute<xAOD::JetFourMom_t>("JetJMSScaleMomentumCalo",jet_reco_CALO);
        jet_reco->getAttribute<xAOD::JetFourMom_t>("JetJMSScaleMomentumTA",jet_reco_TA);
        jet_reco->getAttribute<xAOD::JetFourMom_t>("JetJMSScaleMomentumCombQCD",jet_reco_Comb);

        ATH_MSG_VERBOSE("CALO jet mass " << jet_reco_CALO.mass());
        ATH_MSG_VERBOSE("TA jet mass " << jet_reco_TA.mass() );
        ATH_MSG_VERBOSE("Comb jet mass " << jet_reco_Comb.mass() );

        jet_mass_CALO = jet_reco_CALO.mass();
        jet_mass_TA = jet_reco_TA.mass();


        float JetTrackAssistedMassCalibrated_from_JetCalibTools;
        jet_reco->getAttribute<float>("JetTrackAssistedMassCalibrated", JetTrackAssistedMassCalibrated_from_JetCalibTools);

        if( JetTrackAssistedMassCalibrated_from_JetCalibTools == 0){ //Sometimes JetCalibTools set a TA mass of 0 or close to 0 to some Calo or TA jets. 
					//The mass should be equal to 0 (it is a problem of JetCalibTools). In order to solve it, we extract again the TA mass
					//geting the attribute "JetTrackAssistedMassCalibrated" instead of the four momenta mass (jet_reco_TA.mass())
					//the attribute "JetTrackAssistedMassCalibrated" do not suffer of the non clousure and it is 0 when it has to be 0

					//Later, when we rejoin the jets, we will use the JetUncertainty weights if the TA mass is not 0. If it is 0, we will use
					// the weight 1 for the Calo mass (to avoid the non-clousure problem of JetCalibTools)
             calo_mass_weight = 1;
             use_jetuncertaintiesweight = false;		
        } 
	
        else if(jet_mass_CALO == 0){
            calo_mass_weight = 0;
            use_jetuncertaintiesweight = false;
        }

        else{
            use_jetuncertaintiesweight = true;
        }


    }
    if(m_MassDef=="Calo"){
        jet_mass_CALO = jet_reco->m();
        calo_mass_weight = 1;
    }
    else if(m_MassDef=="TA"){
        jet_mass_TA = jet_reco->m();
        calo_mass_weight = 0;
    }




    //Obtain the average mass response of the jet. The response will depend in the chosed topology (top,W or QCD) and also in the mass definition (CALO, TA, Combined). By default the map used correspond to QCD jets

    double avg_response_CALO;
    double avg_response_TA;

    if(m_MassDef=="Comb" || m_MassDef=="Calo"){

        if(m_CALO_ResponseMap->GetBinContent(m_CALO_ResponseMap->GetXaxis()->FindBin(jet_reco->pt()/1000.),m_CALO_ResponseMap->GetYaxis()->FindBin(jet_truth_matched.m()/1000.)) == 0){//If we look outside the Th2 histogram, we would obtain a 0 so we apply the nominal response (1)
        avg_response_CALO=1;
        }
        else{
            avg_response_CALO = m_CALO_ResponseMap->GetBinContent(m_CALO_ResponseMap->GetXaxis()->FindBin(jet_reco->pt()/1000.),m_CALO_ResponseMap->GetYaxis()->FindBin(jet_truth_matched.m()/1000.));
        }
    }

    if(m_MassDef=="Comb" || m_MassDef=="TA"){

        if(m_TA_ResponseMap->GetBinContent(m_TA_ResponseMap->GetXaxis()->FindBin(jet_reco->pt()/1000.),m_TA_ResponseMap->GetYaxis()->FindBin(jet_truth_matched.m()/1000.))==0){

            avg_response_TA=1;
        }
        else{           
            avg_response_TA = m_TA_ResponseMap->GetBinContent(m_TA_ResponseMap->GetXaxis()->FindBin(jet_reco->pt()/1000.),m_TA_ResponseMap->GetYaxis()->FindBin(jet_truth_matched.m()/1000.));
        }
    }


    //Obtain the jet mass scale (JMS) and the jet mass resolution (JMR) nominal values and variation that correspond to the jet_reco

    double JMS=1; double JMS_err; double JMR=1; double JMR_err;	
    double scale;
    double resolution;

    double smeared_CALO_mass = jet_mass_CALO;
    double smeared_TA_mass = jet_mass_TA;

    bool is_CALO_mass_smeared = false;
    bool is_TA_mass_smeared = false;

    if(m_MassDef=="Comb" || m_MassDef=="Calo"){
        getJMSJMR( jet_reco, jet_mass_CALO, "Calo",jetTopology, JMS_err, JMR_err);	

        scale = JMS + JMS_err;
        resolution = JMR + JMR_err;

        if(scale != 1 || resolution != 1){

            is_CALO_mass_smeared = true;

            ATH_MSG_VERBOSE("Forward Folding CALO procedure will use scale=" << scale << ", resolution=" << resolution << " and average respose=" << avg_response_CALO);

            //FF procedure
            smeared_CALO_mass = jet_mass_CALO * scale + (jet_mass_CALO - avg_response_CALO*jet_truth_matched.m())*(resolution-scale);//FF formula
        }

    }

    if(m_MassDef=="Comb" || m_MassDef=="TA"){
        getJMSJMR( jet_reco, jet_mass_TA, "TA",jetTopology, JMS_err, JMR_err);

        scale = JMS + JMS_err;
        resolution = JMR + JMR_err;

        if(scale != 1 || resolution != 1){

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



    if(m_MassDef=="Comb" && use_jetuncertaintiesweight == true){

        double caloRes;
        double TARes;

        xAOD::JetFourMom_t jet_reco_CALO;
        xAOD::JetFourMom_t jet_reco_TA;

        jet_reco->getAttribute<xAOD::JetFourMom_t>("JetJMSScaleMomentumCalo",jet_reco_CALO);
        jet_reco->getAttribute<xAOD::JetFourMom_t>("JetJMSScaleMomentumTA",jet_reco_TA);


        xAOD::JetFourMom_t p4_aux;

        //The smearing do not change the pt but it changes the mass (so the energy too) so, itf we want to perform the smearing properly, we have to change the Calo and TA foru m	//omenta before looking at the weights map
        //omenta before looking at the weights map
        p4_aux = xAOD::JetFourMom_t(jet_reco_CALO.pt(),jet_reco_CALO.eta(),jet_reco_CALO.phi(),smeared_CALO_mass);//The smearing do not change the pt but it changes the Energy 
        jet_reco_CALO = p4_aux;

        p4_aux = xAOD::JetFourMom_t(jet_reco_TA.pt(),jet_reco_TA.eta(),jet_reco_TA.phi(),smeared_TA_mass);
        jet_reco_TA = p4_aux;

        caloRes=FFJetSmearingTool::Read3DHistogram(m_caloMassWeight,jet_reco_CALO.e()/1000.,TMath::Log(jet_reco_CALO.M()/jet_reco_CALO.e()),std::abs(jet_reco_CALO.eta()));

        TARes=FFJetSmearingTool::Read3DHistogram(m_TAMassWeight,jet_reco_TA.e()/1000.,TMath::Log(jet_reco_TA.M()/jet_reco_TA.e()),std::abs(jet_reco_TA.eta()));

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

    double smeared_mass = calo_mass_weight*smeared_CALO_mass + (1 - calo_mass_weight)*smeared_TA_mass;

    ATH_MSG_VERBOSE("Smeared CALO mass " << smeared_CALO_mass);
    ATH_MSG_VERBOSE("Smeared TA mass " << smeared_TA_mass);

    xAOD::JetFourMom_t p4 = jet_reco->jetP4();


    p4 = xAOD::JetFourMom_t(jet_reco->pt(),jet_reco->eta(),jet_reco->phi(),smeared_mass);
    jet_reco->setJetP4(p4);


    ATH_MSG_VERBOSE("Smeared Reco Jet: pt = " << jet_reco->pt()/1000. << ", mass = " << jet_reco->m()/1000. << ", eta = " << jet_reco->eta());
	



    ATH_MSG_VERBOSE("//---------------------------------------------------------------//");

    return CP::CorrectionCode::Ok;
}







//Functions from JetUncertainties. We copy them in order to read the map exactly as it is done in JetUncertainties and get EXACTLY the same result

double FFJetSmearingTool::Read3DHistogram(TH3* histo, double x, double y, double z)
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

  //Use the interpolate function from Heleprs.cxx
  RootHelpers help_interpolate;

  double weight = help_interpolate.Interpolate(histo, aux_x, aux_y, aux_z);


  return weight;
}


