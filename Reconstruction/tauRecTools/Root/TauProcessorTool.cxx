/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "tauRecTools/TauProcessorTool.h"

#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauTrackContainer.h"
#include "xAODTau/TauTrackAuxContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODPFlow/PFOContainer.h"
#include "xAODPFlow/PFOAuxContainer.h"

// Used for configuration
#include "PathResolver/PathResolver.h"
#include "TEnv.h"
#include "THashList.h"
#include "TClass.h"
#include "TROOT.h"

// // Tools we can configure
// #include "tauRecTools/TauRecToolBase.h"
// #include "tauRecTools/TauCalibrateLC.h"
// #include "tauRecTools/TauCommonCalcVars.h"
// #include "tauRecTools/TauTrackFilter.h"
// #include "tauRecTools/TauGenericPi0Cone.h"
// #include "tauRecTools/TauIDPileupCorrection.h"

//________________________________________
TauProcessorTool::TauProcessorTool(const std::string& type) :
  asg::AsgTool(type),
  m_tauContainerName("TauJets"),
  m_tauAuxContainerName("TauJetsAux."),
  m_AODmode(false)
{
  declareProperty("ConfigPath", m_ConfigPath="tauRecTools/TauProcessorTool.conf");
  declareProperty("TauContainer", m_tauContainerName);
  declareProperty("TauAuxContainer", m_tauAuxContainerName);
  declareProperty("Tools", m_tools, "List of ITauToolBase tools");
  declareProperty("runOnAOD", m_AODmode); //AODS are input file
  declareProperty("deepCopyChargedPFOContainer", m_deep_copy_chargedPFOContainer=true);
  declareProperty("deepCopyHadronicPFOContainer", m_deep_copy_hadronicPFOContainer=true);
  declareProperty("deepCopyNeutralPFOContainer", m_deep_copy_neutralPFOContainer=true);
  declareProperty("deepCopySecVtxContainer", m_deep_copy_SecVtxContainer=false);  
  declareProperty("deepCopyTauTrackContainer", m_deep_copy_TauTrackContainer=true);
}

//________________________________________
TauProcessorTool::~TauProcessorTool(){}

//________________________________________
StatusCode TauProcessorTool::initialize(){


  //ATH_MSG_INFO("FF::TauProcessor :: initialize()");

#ifdef XAOD_ANALYSIS

  if (!m_configured) {
    if (!readConfig()) {
      // TODO output some kind of error message before returning
      return StatusCode::FAILURE;
    }
    for (unsigned i = 0 ; i < m_tools.size() ; ++i) {
      ITauToolBase* tool = dynamic_cast<ITauToolBase*>(&*m_tools.at(i));
      if (!tool->readConfig().isSuccess()) {
        // TODO output some kind of error message before returning
        return StatusCode::FAILURE;
      }
    }
  }

  for (unsigned i = 0 ; i < m_tools.size() ; ++i) {
    // Tools are (normally) not already initialized when running in analysis mode
    if (!m_tools.at(i)->initialize()) {
      // TODO output some kind of error message before returning
      return StatusCode::FAILURE;
    }
  }

#endif //XAOD_ANALYSIS

  //-------------------------------------------------------------------------
  // No tools allocated!
  //-------------------------------------------------------------------------
  if (m_tools.size() == 0) {
    ATH_MSG_ERROR("no tools given!");
    return StatusCode::FAILURE;
  }

  StatusCode sc;
    
  //-------------------------------------------------------------------------
  // Allocate tools
  //-------------------------------------------------------------------------
  ToolHandleArray<ITauToolBase> ::iterator itT = m_tools.begin();
  ToolHandleArray<ITauToolBase> ::iterator itTE = m_tools.end();
  ATH_MSG_INFO("List of tools in execution sequence:");
  ATH_MSG_INFO("------------------------------------");

  unsigned int tool_count = 0;

  for (; itT != itTE; ++itT) {
    sc = itT->retrieve();
    if (sc.isFailure()) {
      ATH_MSG_WARNING("Cannot find tool named <" << *itT << ">");
    } else {
      ++tool_count;
      ATH_MSG_INFO((*itT)->name());
      //If you want to utlize TauCandidate In Tools, 
      //decalre TauCandidate in your class, and pass its address
      //to function below
      (*itT)->setTauEventData(&m_data);
    }
  }
  ATH_MSG_INFO(" ");
  ATH_MSG_INFO("------------------------------------");

  if (tool_count == 0) {
    ATH_MSG_ERROR("could not allocate any tool!");
    return StatusCode::FAILURE;
  }

  ///////////////////////////////////////////////////////////////////////////

  return StatusCode::SUCCESS;
}

//________________________________________
StatusCode TauProcessorTool::execute(){
  
  StatusCode sc;
  m_data.clear();

  if(m_AODmode){
    //-------------------------------------------------------------------------
    // In AODMode, deep copy all PFOs BEFORE reading in tau
    //-------------------------------------------------------------------------
    if(m_deep_copy_SecVtxContainer){
      xAOD::VertexContainer* pSecVtxContainer(0);
      xAOD::VertexAuxContainer* pSecVtxAuxContainer(0);
      xAOD::Vertex* v(0);
      ATH_CHECK(deepCopy(pSecVtxContainer, pSecVtxAuxContainer, v, "TauSecondaryVertices"));
    }
    if(m_deep_copy_TauTrackContainer){
      xAOD::TauTrackContainer* pTrackContainer(0);
      xAOD::TauTrackAuxContainer* pTauTrackAuxContainer(0);
      xAOD::TauTrack* v(0);
      ATH_CHECK(deepCopy(pTrackContainer, pTauTrackAuxContainer, v, "TauTracks"));
    }

    if(m_deep_copy_neutralPFOContainer){
      xAOD::PFOContainer* neutralPFOContainer(0);
      xAOD::PFOAuxContainer* neutralPFOAuxStore(0);
      xAOD::PFO* p(0);
      //container name hard-coded, but configurable in tool where objects are created in core reco
      ATH_CHECK(deepCopy(neutralPFOContainer, neutralPFOAuxStore, p, "TauNeutralParticleFlowObjects"));
    }

    if(m_deep_copy_hadronicPFOContainer){
      xAOD::PFOContainer* hadronicPFOContainer(0);
      xAOD::PFOAuxContainer* hadronicPFOAuxStore(0);
      xAOD::PFO* p(0);
      //container name hard-coded, but configurable in tool where objects are created in core reco
      ATH_CHECK(deepCopy(hadronicPFOContainer, hadronicPFOAuxStore, p, "TauHadronicParticleFlowObjects"));
    }

    if(m_deep_copy_chargedPFOContainer){
      xAOD::PFOContainer* chargedPFOContainer(0);
      xAOD::PFOAuxContainer* chargedPFOAuxStore(0);
      xAOD::PFO* p(0);
      //container name hard-coded, but configurable in tool where objects are created in core reco
      ATH_CHECK(deepCopy(chargedPFOContainer, chargedPFOAuxStore, p, "TauChargedParticleFlowObjects"));
    }
    //-------------------------------------------------------------------------
    // End pre-tau reading operations
    //-------------------------------------------------------------------------
  }


  const xAOD::TauJetContainer*     pContainerOriginal(0);
  const xAOD::TauJetAuxContainer*     pAuxContainerOriginal(0);

  //-------------------------------------------------------------------------
  // retrieve Tau Containers from StoreGate
  //-------------------------------------------------------------------------
  sc = evtStore()->retrieve(pContainerOriginal, m_tauContainerName);
  if (sc.isFailure()) {
    if (m_AODmode) {
      // don't exit Athena if there is no Tau Container in (D)AODs when running in AOD mode
      // just exit TauProcessor
      // reason: somebody might use slimmed (D)AODs, where not needed containers are not present
      ATH_MSG_WARNING("Failed to retrieve " << m_tauContainerName << "! Will exit TauProcessor now!!");
      return StatusCode::SUCCESS;
    }
    else {
      ATH_MSG_FATAL("Failed to retrieve " << m_tauContainerName);
      return StatusCode::FAILURE;
    }
  } 

  sc = evtStore()->retrieve(pAuxContainerOriginal, m_tauAuxContainerName);
  if (sc.isFailure()) {
    if (m_AODmode) {
      // don't exit Athena if there is no Tau AuxContainer in (D)AODs when running in AOD mode
      // just exit TauProcessor
      // reason: somebody might use slimmed (D)AODs, where not needed containers are not present
      ATH_MSG_WARNING("Failed to retrieve " << m_tauAuxContainerName << "! Will exit TauProcessor now!!");
      return StatusCode::SUCCESS;
    }
    else {
      ATH_MSG_FATAL("Failed to retrieve " << m_tauAuxContainerName);
      return StatusCode::FAILURE;
    }
  } 

  xAOD::TauJetContainer* pContainer = const_cast<xAOD::TauJetContainer*> (pContainerOriginal);
  xAOD::TauJetAuxContainer* pAuxContainer = const_cast<xAOD::TauJetAuxContainer*> (pAuxContainerOriginal);

  if(m_AODmode){
    pContainer=0;
    pAuxContainer=0;
    xAOD::TauJet* tau(0);
    ATH_CHECK(deepCopy(pContainer, pAuxContainer, tau, m_tauContainerName, m_tauAuxContainerName));
  }

  m_data.xAODTauContainer = pContainer;
  m_data.tauAuxContainer = pAuxContainer;

  //-------------------------------------------------------------------------
  // Initialize tools for this event
  //-------------------------------------------------------------------------
  ToolHandleArray<ITauToolBase> ::iterator itT = m_tools.begin();
  ToolHandleArray<ITauToolBase> ::iterator itTE = m_tools.end();
  for (; itT != itTE; ++itT) {
    sc = (*itT)->eventInitialize();
    if (sc != StatusCode::SUCCESS)
      return StatusCode::FAILURE;
  }

  ////////////////////////////////////////////////////////

  //loop over taus
  xAOD::TauJetContainer::iterator tau_it  = pContainer->begin();
  xAOD::TauJetContainer::iterator tau_end = pContainer->end();
    
  for(; tau_it != tau_end; ++tau_it) {
        
    //-----------------------------------------------------------------
    // set tau candidate data for easy handling
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    // Process the candidate
    //-----------------------------------------------------------------
    ToolHandleArray<ITauToolBase>::iterator itT = m_tools.begin();
    ToolHandleArray<ITauToolBase>::iterator itTE = m_tools.end();

    //-----------------------------------------------------------------
    // Loop stops when Failure indicated by one of the tools
    //-----------------------------------------------------------------
    for (; itT != itTE; ++itT) {
      ATH_MSG_VERBOSE("Invoking tool " << (*itT)->name());

      sc = (*itT)->execute(**tau_it);

      if (sc.isFailure())
	break;
    }

    if (sc.isSuccess()) {
          
      ATH_MSG_VERBOSE("The tau candidate has been modified");

    } else if (!sc.isSuccess()) {
      //TODO:cleanup of EndTools not necessary??
      //keep this here for future use (in case more than one seeding algo exist)
      /*
	ToolHandleArray<ITauToolBase> ::iterator p_itT1 = m_tools.begin();
	for (; p_itT1 != p_itT; ++p_itT1)
	(*p_itT1)->cleanup(&m_data);
	(*p_itT1)->cleanup(&m_data);
      */
      //delete m_data.tau;
    } else  {
      //delete m_data.tau;
    }
  }



  //-------------------------------------------------------------------------
  // Finalize tools for this event
  //-------------------------------------------------------------------------

  itT = m_tools.begin();
  itTE = m_tools.end();
  for (; itT != itTE; ++itT) {
    sc = (*itT)->eventFinalize();
    if (sc != StatusCode::SUCCESS)
      return StatusCode::FAILURE;
  }


  ///////////////////////////////////////////////////////
  // locking of containers is moved to separate tau tool
 

  return StatusCode::SUCCESS;
}

//________________________________________
StatusCode TauProcessorTool::finalize(){

  return StatusCode::SUCCESS;
}

//________________________________________
//TODO: Inherit this, don't reimplement it
std::string TauProcessorTool::find_file(const std::string& fname) const {
  static const std::string m_tauRecToolsTag="tauRecTools/00-00-00/";
  std::string full_path = PathResolverFindCalibFile(m_tauRecToolsTag+fname);
  if(full_path=="") full_path = PathResolverFindCalibFile(fname);
  return full_path;
}

//________________________________________
StatusCode TauProcessorTool::readConfig() {
  // Sanity check to see if property ConfigPath is declared for a tool. Might be
  // removed once all tools are updated to have a config path declared.
  // in athena getProperties returns std::vector<Property*>
  // in rc     getProperties returns std::map<std::string,Property*>
#ifdef ASGTOOL_ATHENA
  bool configPathDeclared = false;
  for (Property* property : getProperties())
  {
    if (property->name() == "ConfigPath")
    {
      configPathDeclared = true;
      break;
    }
  }
  if (!configPathDeclared)
#elif defined(ASGTOOL_STANDALONE)
  PropertyMgr::PropMap_t property_map = getPropertyMgr()->getProperties();
  if (property_map.find("ConfigPath") == property_map.end())
#else
#   error "What environment are we in?!?"
#endif // ASGTOOL_ATHENA
  {
    ATH_MSG_INFO("No config file path property declared yet, this is not recommended");
    return StatusCode::SUCCESS;
  }
  
  // get configured config path and load file via TEnv
  const std::string* config_file_path_property;
  // if (getProperty("ConfigPath", config_file_path).isFailure())
  //   return StatusCode::FAILURE;
  config_file_path_property = getProperty<std::string>("ConfigPath");
  std::string config_file_path = find_file(*config_file_path_property);
  TEnv env;
  env.ReadFile(PathResolverFindCalibFile(config_file_path).c_str(),kEnvAll);

  THashList* lList = env.GetTable();
  std::vector<std::pair<std::string, std::string>> toolList;
  for( Int_t i = 0; lList && i < lList->GetEntries(); ++i )
  {
    std::string name = lList->At( i )->GetName();
    if (std::string::npos != name.rfind('.')) // FIXME this condition is a hack
    {
      std::pair<std::string, std::string> toolInfo;
      toolInfo.first  = lList->At( i )->GetName();
      toolInfo.second = env.GetValue(lList->At( i )->GetName(),"");
      toolList.push_back(toolInfo);
    }
    else {
      StatusCode sc;
#ifdef ASGTOOL_ATHENA
      // get type of variable with the entry name
      const std::type_info* type = getProperty(lList->At( i )->GetName()).type_info();

      // search for type is needed by env.GetValue function (needs a variable of the correct type as 2nd argument)
      if (*type == typeid(bool))
        sc = this->setProperty(lList->At( i )->GetName(),
          bool(env.GetValue(lList->At( i )->GetName(),bool(true))));
      else if (*type == typeid(int))
        sc = this->setProperty(lList->At( i )->GetName(),
          env.GetValue(lList->At( i )->GetName(),int(0)));
      else if (*type == typeid(float))
        sc = this->setProperty(lList->At( i )->GetName(),
          env.GetValue(lList->At( i )->GetName(),float(0)));
      else if (*type == typeid(double))
        sc = this->setProperty(lList->At( i )->GetName(),
          env.GetValue(lList->At( i )->GetName(),double(0)));
      else if (*type == typeid(std::string))
        sc = this->setProperty(lList->At( i )->GetName(),
          env.GetValue(lList->At( i )->GetName(),""));
#else
    // get type of variable with the entry name
      Property::Type type = getPropertyMgr()->getProperty(lList->At( i )->GetName())->type();

      if (type == Property::BOOL)
        sc = this->setProperty(lList->At( i )->GetName(),
          bool(env.GetValue(lList->At( i )->GetName(),bool(true))));
      else if (type == Property::INT)
        sc = this->setProperty(lList->At( i )->GetName(),
          env.GetValue(lList->At( i )->GetName(),int(0)));
      else if (type == Property::FLOAT)
        sc = this->setProperty(lList->At( i )->GetName(),
          env.GetValue(lList->At( i )->GetName(),float(0)));
      else if (type == Property::DOUBLE)
        sc = this->setProperty(lList->At( i )->GetName(),
          env.GetValue(lList->At( i )->GetName(),double(0)));
      else if (type == Property::STRING)
        sc = this->setProperty(lList->At( i )->GetName(),
          env.GetValue(lList->At( i )->GetName(),""));
#endif // ASGTOOL_ATHENA
      else {
        sc = StatusCode::FAILURE;
      }
      if (!sc.isSuccess()) {
#ifdef ASGTOOL_ATHENA
        ATH_MSG_FATAL("there was a problem to find the correct type enum: "<<type->name());
#else
        ATH_MSG_FATAL("there was a problem to find the correct type enum: "<<type);
#endif // ASGTOOL_ATHENA
        return StatusCode::FAILURE;
      }
    }
  }

  // At this point, the config file is read. Now we need to instantiate the tools.

  //ToolHandleArray<ITauToolBase> tools;

  for (unsigned i = 0 ; i < toolList.size() ; ++i)
  {
    std::pair<std::string, std::string> toolInfo = toolList.at(i);
    std::string toolType = toolInfo.second;
    std::string toolConfigPath = toolInfo.first;

    // Going to get name from the basename of the config file
    std::string toolName = toolConfigPath; 

    // Remove directory if present.
    // Do this before extension removal incase directory has a period character.
    const size_t last_slash_idx = toolName.find_last_of("\\/");
    if (std::string::npos != last_slash_idx) { toolName.erase(0, last_slash_idx + 1); }

    // Remove extension if present.
    const size_t period_idx = toolName.rfind('.');
    if (std::string::npos != period_idx) { toolName.erase(period_idx); }

    // Instantiate the tool
    // TauRecToolBase* tool;
    // if      (toolType == "TauCalibrateLC")        { tool = new TauCalibrateLC(toolName); }
    // else if (toolType == "TauCommonCalcVars")     { tool = new TauCommonCalcVars(toolName); }
    // else if (toolType == "TauTrackFilter")        { tool = new TauTrackFilter(toolName); }
    // else if (toolType == "TauGenericPi0Cone")     { tool = new TauGenericPi0Cone(toolName); }
    // else if (toolType == "TauIDPileupCorrection") { tool = new TauIDPileupCorrection(toolName); }
    // else {
    //   // TODO output some kind of error message before returning
    //   return StatusCode::FAILURE;
    // }

    ITauToolBase* tool(0);
    TClass* cl = gROOT->GetClass(toolType.c_str());
    if(!cl) {
      ATH_MSG_FATAL("No class " << toolType << " Found (is there a dictionary?)");
      return StatusCode::FAILURE;
    }
    if( !cl->InheritsFrom("ITauToolBase") ){
      ATH_MSG_FATAL("Class " << toolType << " Does not inherit from ITauToolBase");
      return StatusCode::FAILURE;
    }
    tool = static_cast<ITauToolBase*> (cl->New());
    if(tool==0){
      ATH_MSG_FATAL("Couldn't allocate " << toolType << " Is there a default constructor?");
      return StatusCode::FAILURE;      
    }
#ifdef XAOD_ANALYSIS
    asg::ToolStore::remove(toolType);// name of tool is name of class, in case we want multiple instances in store, 
    //remove instance, rename tool, and put tool back in store
    tool->setName(toolName);
    asg::ToolStore::put(tool);
#endif


    // Set the configuration path for the tool
    asg::AsgTool* asg_tool = dynamic_cast<asg::AsgTool*> (tool);
    if (!asg_tool->setProperty("ConfigPath", toolConfigPath).isSuccess()) {
      // TODO output some kind of error message before returning
      ATH_MSG_FATAL("Tool should have ConfigPath defined");
      return StatusCode::FAILURE;
    }

    // Schedule the tool
    ToolHandle<ITauToolBase> handle(tool);
    m_tools.push_back(handle);

  }

  // Configure the TauProcessorTool
  m_configured = true;

  return StatusCode::SUCCESS;
}
