/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaMonitoring/AthMonitorAlgorithm.h"

AthMonitorAlgorithm::AthMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
:AthReentrantAlgorithm(name,pSvcLocator)
,m_environment(Environment_t::user)
,m_dataType(DataType_t::userDefined)
,m_vTrigChainNames({})
{}


AthMonitorAlgorithm::~AthMonitorAlgorithm() {}


StatusCode AthMonitorAlgorithm::initialize() {
    StatusCode sc;

    // ROOT global histogram flag: off (not thread-safe to have it on)
    TH1::AddDirectory(kFALSE);

    // Retrieve the generic monitoring tools (a ToolHandleArray)
    if ( !m_tools.empty() ) {
        ATH_CHECK( m_tools.retrieve() );
        for (size_t idx = 0; idx < m_tools.size(); ++idx) {
	    std::string name(m_tools[idx].name());
	    m_toolLookupMap[ name ] = idx;
        }
    }

    // Retrieve the trigger decision tool if requested
    if ( !m_trigDecTool.empty() ) {
        ATH_CHECK( m_trigDecTool.retrieve() );
        ATH_MSG_DEBUG( "TDT retrieved" );

        // If the trigger chain is specified, parse it into a list.
        if ( m_triggerChainString!="" ) {
            sc = parseList(m_triggerChainString,m_vTrigChainNames);
            if ( !sc.isSuccess() ) {
                ATH_MSG_WARNING("Error parsing trigger chain list, using empty list instead.");
                m_vTrigChainNames.clear();
            }

            // Then, retrieve the trigger translator if requested. Finally, convert 
            // into a usable format using the unpackTriggerCategories function.
            if (!m_trigTranslator.empty()) {
                ATH_CHECK( m_trigTranslator.retrieve() );
                unpackTriggerCategories(m_vTrigChainNames);
            }
        }
    }

    // Retrieve filters
    ATH_CHECK( m_DQFilterTools.retrieve() );

    // Convert the data type and environment strings from the python configuration into the
    // enum class types DataType_t and Environment_t
    m_dataType = dataTypeStringToEnum(m_dataTypeStr);
    m_environment = envStringToEnum(m_environmentStr);

    ATH_CHECK( m_lumiDataKey.initialize (m_useLumi && m_dataType != DataType_t::monteCarlo) );
    ATH_CHECK( m_lbDurationDataKey.initialize (m_useLumi && m_dataType != DataType_t::monteCarlo) );
    ATH_CHECK( m_trigLiveFractionDataKey.initialize (m_useLumi && m_dataType != DataType_t::monteCarlo) );

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
            ATH_MSG_DEBUG("Event rejected due to filter tool.");
            return StatusCode::SUCCESS;
        }
    }

    // Trigger: If there is a decision tool and the chains fail, skip the event.
    if ( !m_trigDecTool.empty() && !trigChainsArePassed(m_vTrigChainNames) ) {
        ATH_MSG_DEBUG("Event rejected due to trigger filter.");
        return StatusCode::SUCCESS;
    }

    ATH_MSG_DEBUG("Event accepted!");
    return fillHistograms(ctx);
}


void AthMonitorAlgorithm::fill( const ToolHandle<GenericMonitoringTool>& groupHandle,
                                MonVarVec_t&& variables ) const {
    Monitored::Group(groupHandle,std::move(variables)).fill();
}


void AthMonitorAlgorithm::fill( const std::string& groupName,
                                MonVarVec_t&& variables ) const {
   this->fill(getGroup(groupName),std::move(variables));
}


SG::ReadHandle<xAOD::EventInfo> AthMonitorAlgorithm::GetEventInfo( const EventContext& ctx ) const {
    return SG::ReadHandle<xAOD::EventInfo>(m_EventInfoKey, ctx);
}


AthMonitorAlgorithm::Environment_t AthMonitorAlgorithm::envStringToEnum( const std::string& str ) const {
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
            <<str<<", returning user.");
        return Environment_t::user;
    }
}


AthMonitorAlgorithm::DataType_t AthMonitorAlgorithm::dataTypeStringToEnum( const std::string& str ) const {
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
            <<str<<", returning userDefined.");
        return DataType_t::userDefined;
    }
}


ToolHandle<GenericMonitoringTool> AthMonitorAlgorithm::getGroup( const std::string& name ) const {
    // get the pointer to the tool, and check that it exists
    const ToolHandle<GenericMonitoringTool>* toolPtr{nullptr};
    auto idx = m_toolLookupMap.find(name);
    if (ATH_LIKELY(idx != m_toolLookupMap.end())) {
        toolPtr = &m_tools[idx->second];
    }
    if ( ATH_UNLIKELY(!toolPtr) ) {
	if ( ! isInitialized() ) {
	  ATH_MSG_FATAL("It seems that the AthMonitorAlgorithm::initialize was not called in derived class initialize method");
	} else {
	  std::string available = std::accumulate( m_toolLookupMap.begin(), m_toolLookupMap.end(),
						   std::string(""), [](const std::string& s, auto h){return s + "," + h.first;} );
	  ATH_MSG_FATAL( "The tool " << name << " could not be found in the tool array of the " <<
			 "monitoring algorithm " << m_name << ". This probably reflects a discrepancy between " <<
			 "your python configuration and c++ filling code. Note: your available groups are {" <<
			 available << "}." );
	}
        return ToolHandle<GenericMonitoringTool>();
    }
    const ToolHandle<GenericMonitoringTool> toolHandle = *toolPtr;
    if ( ATH_UNLIKELY(toolHandle.empty()) ) {
        ATH_MSG_FATAL("The tool "<<name<<" could not be found because of an empty tool handle." );
    }
    // return the tool handle
    return toolHandle;
}


const ToolHandle<Trig::TrigDecisionTool>& AthMonitorAlgorithm::getTrigDecisionTool() const {
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


float AthMonitorAlgorithm::lbAverageInteractionsPerCrossing (const EventContext& ctx /*= Gaudi::Hive::currentContext()*/) const
{
    if (!m_lumiDataKey.empty()) {
        SG::ReadCondHandle<LuminosityCondData> lumi (m_lumiDataKey, ctx);
        return lumi->lbAverageInteractionsPerCrossing();
    } else {
        ATH_MSG_DEBUG("AthMonitorAlgorithm::lbAverageInteractionsPerCrossing() - luminosity tools are not retrieved.");
        return -1.0;
    }
}


float AthMonitorAlgorithm::lbInteractionsPerCrossing (const EventContext& ctx /*= Gaudi::Hive::currentContext()*/) const
{
    if (!m_lumiDataKey.empty()) {
        SG::ReadCondHandle<LuminosityCondData> lumi (m_lumiDataKey, ctx);
        float muToLumi = lumi->muToLumi();
        if (muToLumi > 0) {
          return lumi->lbLuminosityPerBCIDVector().at (ctx.eventID().bunch_crossing_id()) / muToLumi;
        }
        return 0;
    } else {
        ATH_MSG_DEBUG("AthMonitorAlgorithm::lbInteractionsPerCrossing() - luminosity tools are not retrieved.");
        return -1.0;
    }
}


float AthMonitorAlgorithm::lbAverageLuminosity (const EventContext& ctx /*= Gaudi::Hive::currentContext()*/) const
{
    if (!m_lumiDataKey.empty()) {
        SG::ReadCondHandle<LuminosityCondData> lumi (m_lumiDataKey, ctx);
        return lumi->lbAverageLuminosity();
    } else {
        ATH_MSG_DEBUG("AthMonitorAlgorithm::lbAverageLuminosity() - luminosity tools are not retrieved.");
        return -1.0;
    }
}


float AthMonitorAlgorithm::lbLuminosityPerBCID (const EventContext& ctx /*= Gaudi::Hive::currentContext()*/) const
{
    if (!m_lumiDataKey.empty()) {
        SG::ReadCondHandle<LuminosityCondData> lumi (m_lumiDataKey, ctx);
        return lumi->lbLuminosityPerBCIDVector().at (ctx.eventID().bunch_crossing_id());
    } else {
        ATH_MSG_DEBUG("AthMonitorAlgorithm::lbLuminosityPerBCID() - luminosity tools are not retrieved.");
        return -1.0;
    }
}


float AthMonitorAlgorithm::lbAverageLivefraction (const EventContext& ctx /*= Gaudi::Hive::currentContext()*/) const
{
    if (m_environment == Environment_t::online) {
        return 1.0;
    }

    if (!m_trigLiveFractionDataKey.empty()) {
        SG::ReadCondHandle<TrigLiveFractionCondData> live (m_trigLiveFractionDataKey, ctx);
        return live->lbAverageLiveFraction();
    } else {
        ATH_MSG_DEBUG("AthMonitorAlgorithm::lbAverageLivefraction() - luminosity not available.");
        return -1.0;
    }
}


float AthMonitorAlgorithm::livefractionPerBCID (const EventContext& ctx /*= Gaudi::Hive::currentContext()*/) const
{
    if (m_environment == Environment_t::online) {
        return 1.0;
    }

    if (!m_trigLiveFractionDataKey.empty()) {
        SG::ReadCondHandle<TrigLiveFractionCondData> live (m_trigLiveFractionDataKey, ctx);
        return live->l1LiveFractionVector().at (ctx.eventID().bunch_crossing_id());
    } else {
        ATH_MSG_DEBUG("AthMonitorAlgorithm::livefractionPerBCID() - luminosity not available.");
        return -1.0;
    }
}


double AthMonitorAlgorithm::lbLumiWeight (const EventContext& ctx /*= Gaudi::Hive::currentContext()*/) const
{
    if (!m_lumiDataKey.empty()) {
        return (lbAverageLuminosity(ctx)*lbDuration(ctx))*lbAverageLivefraction(ctx);
    } else {
        ATH_MSG_DEBUG("AthMonitorAlgorithm::lbLumiWeight() - luminosity tools are not retrieved.");
        return -1.0;
    }
}


double AthMonitorAlgorithm::lbDuration (const EventContext& ctx /*= Gaudi::Hive::currentContext()*/) const
{
    if ( m_environment == Environment_t::online ) {
        return m_defaultLBDuration;
    }

    if (!m_lbDurationDataKey.empty()) {
        SG::ReadCondHandle<LBDurationCondData> dur (m_lbDurationDataKey, ctx);
        return dur->lbDuration();
    } else {
        ATH_MSG_DEBUG("AthMonitorAlgorithm::lbDuration() - luminosity tools are not retrieved.");
        return m_defaultLBDuration;
    }
}


StatusCode AthMonitorAlgorithm::parseList(const std::string& line, std::vector<std::string>& result) const {
    std::string item;
    std::stringstream ss(line);

    ATH_MSG_DEBUG( "AthMonitorAlgorithm::parseList()" );

    while ( std::getline(ss, item, ',') ) {
        std::stringstream iss(item); // remove whitespace
        iss >> item;
        result.push_back(item);
    }

    return StatusCode::SUCCESS;
}


void AthMonitorAlgorithm::unpackTriggerCategories(std::vector<std::string>& vTrigChainNames) const {
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
