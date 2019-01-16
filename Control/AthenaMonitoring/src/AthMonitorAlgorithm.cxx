/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaMonitoring/AthMonitorAlgorithm.h"

AthMonitorAlgorithm::AthMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
:AthReentrantAlgorithm(name,pSvcLocator)
,m_tools(this)
,m_trigDecTool("")
,m_trigTranslator("")
,m_DQFilterTools(this)
,m_lumiTool("LuminosityTool")
,m_liveTool("TrigLivefractionTool")
,m_environment(Environment_t::user)
,m_dataType(DataType_t::userDefined)
,m_environmentStr("user")
,m_dataTypeStr("userDefined")
,m_triggerChainString("")
,m_vTrigChainNames({})
,m_fileKey("")
,m_hasRetrievedLumiTool(false)
,m_useLumi(false)
,m_defaultLBDuration(60.)
,m_detailLevel(1)
,m_EventInfoKey("EventInfo")
{
    // The following properties are set in the python configuration and
    // picked up here to be converted into the method variables. For an
    // explanation of each variable, see the header.
    declareProperty("GMTools",m_tools);
    declareProperty("TrigDecisionTool",m_trigDecTool);
    declareProperty("TriggerTranslatorTool",m_trigTranslator);
    declareProperty("FilterTools",m_DQFilterTools);
    declareProperty("Environment",m_environmentStr);
    declareProperty("DataType",m_dataTypeStr);
    declareProperty("TriggerChain",m_triggerChainString);
    declareProperty("FileKey",m_fileKey);
    declareProperty("EnableLumi",m_useLumi); 
    declareProperty("DefaultLBDuration",m_defaultLBDuration);
    declareProperty("DetailLevel",m_detailLevel);
    declareProperty("EventInfoKey",m_EventInfoKey);
}


AthMonitorAlgorithm::~AthMonitorAlgorithm() {}


StatusCode AthMonitorAlgorithm::initialize() {
    StatusCode sc;

    // Retrieve the generic monitoring tools (a ToolHandleArray)
    if ( !m_tools.empty() ) {
        sc = m_tools.retrieve();
        if ( !sc.isSuccess() ) {
            ATH_MSG_ERROR("Unable to retrieve the generic monitoring tools." << endmsg);
        }
    }

    // Retrieve the trigger decision tool if requested
    if ( !m_trigDecTool.empty() ) {
        sc = m_trigDecTool.retrieve();
        if( !sc.isSuccess() ) {
            ATH_MSG_ERROR("Unable to retrieve the TrigDecisionTool." << endmsg);
            return sc;
        }

        // If the trigger chain is specified, parse it into a list.
        if ( m_triggerChainString!="" ) {
            sc = parseList(m_triggerChainString,m_vTrigChainNames);
            if ( !sc.isSuccess() ) {
                ATH_MSG_WARNING("Error parsing trigger chain list, using empty list instead." << endmsg);
                m_vTrigChainNames.clear();
            }

            // Then, retrieve the trigger translator if requested. Finally, convert 
            // into a usable format using the unpackTriggerCategories function.
            if (!m_trigTranslator.empty()) {
                sc = m_trigTranslator.retrieve();
                if ( !sc.isSuccess() ) {
                    ATH_MSG_ERROR("Unable to retrieve the TrigTranslatorTool." << endmsg);
                    return sc;
                }
                unpackTriggerCategories(m_vTrigChainNames);
            }
        }
    }

    // Convert the data type and environment strings from the python configuration into the
    // enum class types DataType_t and Environment_t
    m_dataType = dataTypeStringToEnum(m_dataTypeStr);
    m_environment = envStringToEnum(m_environmentStr);

    // Retrieve the luminosity tool if requested and whenever not using Monte Carlo
    if (m_useLumi) {
        if (m_dataType == DataType_t::monteCarlo) {
            ATH_MSG_WARNING("Lumi tool requested, but AthMonitorAlgorithm is configured for MC. Disabling lumi tool.");
        } else {
            // Retrieve the luminosity and live fraction tools
            StatusCode sc_lumiTool = m_lumiTool.retrieve();
            StatusCode sc_liveTool = m_liveTool.retrieve();

            // Set m_hasRetrievedLumiTool to true if both tools are retrieved successfully
            if ( sc_lumiTool.isSuccess() && sc_liveTool.isSuccess() ) {
               m_hasRetrievedLumiTool = true;
            }
        }
    }

    // get event info key
    ATH_CHECK( m_EventInfoKey.initialize() );

    // end of initialization
    ATH_MSG_DEBUG("Exiting AthMonitorAlgorithm::initialize() successfully.");
    return sc;
}


StatusCode AthMonitorAlgorithm::execute( const EventContext& ctx ) const {

    // Checks that all of the  DQ filters are passed. If any one of the filters
    // fails, return SUCCESS code and do not fill the histograms with the event.
    for ( const auto& filterItr : m_DQFilterTools ) {
        if (!filterItr->accept()) {
            return StatusCode::SUCCESS;
        }
    }

    // Trigger: If there is a decision tool and the chains fail, skip the event.
    if ( !m_trigDecTool.empty() && !trigChainsArePassed(m_vTrigChainNames) ) {
        return StatusCode::SUCCESS;
    }

    return fillHistograms(ctx);
}

SG::ReadHandle<xAOD::EventInfo> AthMonitorAlgorithm::GetEventInfo( const EventContext& ctx ) const {
  return SG::ReadHandle<xAOD::EventInfo>(m_EventInfoKey, ctx);
}


AthMonitorAlgorithm::Environment_t AthMonitorAlgorithm::environment() const {
    return m_environment;
}


AthMonitorAlgorithm::Environment_t AthMonitorAlgorithm::envStringToEnum( const std::string& str ) {
    // convert the string to all lowercase
    std::string lowerCaseStr = str;
    std::transform(lowerCaseStr.begin(), lowerCaseStr.end(), lowerCaseStr.begin(), ::tolower);

    // check if it matches one of the enum choices
    if( lowerCaseStr == "user" ) {
        return Environment_t::user;
    } else if( lowerCaseStr == "online" ) {
        return Environment_t::online;
    } else if( lowerCaseStr == "tier0" ) {
        return Environment_t::tier0;
    } else if( lowerCaseStr == "tier0raw" ) {
        return Environment_t::tier0Raw;
    } else if( lowerCaseStr == "tier0esd" ) {
        return Environment_t::tier0ESD;
    } else if( lowerCaseStr == "aod" ) {
        return Environment_t::AOD;
    } else if( lowerCaseStr == "altprod" ) {
        return Environment_t::altprod;
    } else { // otherwise, warn the user and return "user"
        ATH_MSG_WARNING("AthMonitorAlgorithm::envStringToEnum(): Unknown environment "
            <<str<<", returning user."<<endmsg);
        return Environment_t::user;
    }
}


AthMonitorAlgorithm::DataType_t AthMonitorAlgorithm::dataType() const {
    return m_dataType;
}


AthMonitorAlgorithm::DataType_t AthMonitorAlgorithm::dataTypeStringToEnum( const std::string& str ) {
    // convert the string to all lowercase
    std::string lowerCaseStr = str;
    std::transform(lowerCaseStr.begin(), lowerCaseStr.end(), lowerCaseStr.begin(), ::tolower);

    // check if it matches one of the enum choices
    if( lowerCaseStr == "userdefined" ) {
        return DataType_t::userDefined;
    } else if( lowerCaseStr == "montecarlo" ) {
        return DataType_t::monteCarlo;
    } else if( lowerCaseStr == "collisions" ) {
        return DataType_t::collisions;
    } else if( lowerCaseStr == "cosmics" ) {
        return DataType_t::cosmics;
    } else if( lowerCaseStr == "heavyioncollisions" ) {
        return DataType_t::heavyIonCollisions;
    } else { // otherwise, warn the user and return "userDefined"
        ATH_MSG_WARNING("AthMonitorAlgorithm::dataTypeStringToEnum(): Unknown data type "
            <<str<<", returning userDefined."<<endmsg);
        return DataType_t::userDefined;
    }
}


GenericMonitoringTool& AthMonitorAlgorithm::getGroup( const std::string& name ) const {
    // get the pointer to the tool, check that it exists, and return
    GenericMonitoringTool* tool = &(*(*m_tools[name]));
    if (tool == nullptr) {
        ATH_MSG_FATAL("The tool "<<name<<" could not be found in the monitoring algorithm's tool array."<<endmsg);
    }
    return *tool;
}


const ToolHandle<Trig::ITrigDecisionTool>& AthMonitorAlgorithm::getTrigDecisionTool() {
    return m_trigDecTool;
}


bool AthMonitorAlgorithm::trigChainsArePassed( const std::vector<std::string>& vTrigNames ) const {
    // Check whether ANY of the triggers in the list are passed
    for ( auto& trigName : vTrigNames ) {
        if ( m_trigDecTool->isPassed(trigName) ) {
            return true;
        }
    }
    // If no triggers were given, return true. Otherwise, the trigger requirement failed
    return vTrigNames.size()==0;
}


float AthMonitorAlgorithm::lbAverageInteractionsPerCrossing() const {
    if ( m_hasRetrievedLumiTool ) {
        return m_lumiTool->lbAverageInteractionsPerCrossing();
    } else {
        ATH_MSG_DEBUG("AthMonitorAlgorithm::lbAverageInteractionsPerCrossing() - luminosity tools are not retrieved.");
        return -1.0;
    }
}


float AthMonitorAlgorithm::lbInteractionsPerCrossing() const {
    if ( m_hasRetrievedLumiTool ) {
        float instmu = 0.;
        if (m_lumiTool->muToLumi() > 0.) {
            instmu = m_lumiTool->lbLuminosityPerBCID()/m_lumiTool->muToLumi();
        }
        return instmu;
    } else {
        ATH_MSG_DEBUG("AthMonitorAlgorithm::lbInteractionsPerCrossing() - luminosity tools are not retrieved.");
        return -1.0;
    }
}


float AthMonitorAlgorithm::lbAverageLuminosity() const {
    if ( m_hasRetrievedLumiTool ) {
        return m_lumiTool->lbAverageLuminosity();
    } else {
        ATH_MSG_DEBUG("AthMonitorAlgorithm::lbAverageLuminosity() - luminosity tools are not retrieved.");
        return -1.0;
    }
}


float AthMonitorAlgorithm::lbLuminosityPerBCID() const {
    if ( m_hasRetrievedLumiTool ) {
        return m_lumiTool->lbLuminosityPerBCID();
    } else {
        ATH_MSG_DEBUG("AthMonitorAlgorithm::lbLuminosityPerBCID() - luminosity tools are not retrieved.");
        return -1.0;
    }
}


float AthMonitorAlgorithm::lbAverageLivefraction() const {
    if (m_environment == Environment_t::online) {
        return 1.0;
    }

    if ( m_hasRetrievedLumiTool ) {
        return m_liveTool->lbAverageLivefraction();
    } else {
        ATH_MSG_DEBUG("AthMonitorAlgorithm::lbAverageLivefraction() - luminosity tools are not retrieved.");
        return -1.0;
    }
}


float AthMonitorAlgorithm::livefractionPerBCID() const {
    if (m_environment == Environment_t::online) {
        return 1.0;
    }

    if ( m_hasRetrievedLumiTool ) {
        return m_liveTool->livefractionPerBCID();
    } else {
        ATH_MSG_DEBUG("AthMonitorAlgorithm::livefractionPerBCID() - luminosity tools are not retrieved.");
        return -1.0;
    }
}


double AthMonitorAlgorithm::lbLumiWeight() const {
    if ( m_hasRetrievedLumiTool ) {
        return (lbAverageLuminosity()*lbDuration())*lbAverageLivefraction();
    } else {
        ATH_MSG_DEBUG("AthMonitorAlgorithm::lbLumiWeight() - luminosity tools are not retrieved.");
        return -1.0;
    }
}


double AthMonitorAlgorithm::lbDuration() const {
    if ( m_environment == Environment_t::online ) {
        return m_defaultLBDuration;
    }

    if ( m_hasRetrievedLumiTool ) {
        return m_lumiTool->lbDuration();
    } else {
        ATH_MSG_DEBUG("AthMonitorAlgorithm::lbDuration() - luminosity tools are not retrieved.");
        return m_defaultLBDuration;
    }
}


StatusCode AthMonitorAlgorithm::parseList(const std::string& line, std::vector<std::string>& result) {
    std::string item;
    std::stringstream ss(line);

    ATH_MSG_DEBUG("AthMonitorAlgorithm::parseList()" << endmsg);

    while ( std::getline(ss, item, ',') ) {
        std::stringstream iss(item); // remove whitespace
        iss >> item;
        result.push_back(item);
    }

    return StatusCode::SUCCESS;
}


void AthMonitorAlgorithm::unpackTriggerCategories(std::vector<std::string>& vTrigChainNames) {
    for (size_t i = 0; i < vTrigChainNames.size(); ++i) {
        std::string& thisName = vTrigChainNames[i];

        if (thisName.substr(0,9) == "CATEGORY_") {
            ATH_MSG_DEBUG("Found a trigger category: " << thisName << ". Unpacking.");
            std::vector<std::string> triggers = m_trigTranslator->translate(thisName.substr(9,std::string::npos));
            std::ostringstream oss;
            oss << "(";
            for (size_t itrig = 0; itrig < triggers.size(); ++itrig) {
                if (itrig != 0) { 
                    oss << "|";
                }
                oss << triggers[itrig];
            }
            oss << ")";
            // replace with new value
            std::string newval = oss.str();
            ATH_MSG_DEBUG("Replaced with " << newval);
            vTrigChainNames[i] = newval;
        }
    }
}
