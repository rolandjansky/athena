/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class AthMonitorAlgorithm
 * @author C. Burton <burton@utexas.edu>
 * @author P. Onyisi <ponyisi@utexas.edu>
 * @date 2018/12/31
 * @brief Base class for Athena Monitoring Algorithms
 * 
 * A class in the AthenaMonitoring package which is subclassed to implement algorithms for
 * the data monitoring system. An example subclass is given in ExampleMonitorAlgorithm. In
 * the subclass, the user is required to implement the fillHistograms() function.
 */

#ifndef ATHMONITORALGORITHM_H
#define ATHMONITORALGORITHM_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "AthenaMonitoring/GenericMonitoringTool.h"
#include "AthenaMonitoring/IDQFilterTool.h"
#include "AthenaMonitoring/IMonitorToolBase.h"
#include "AthenaMonitoring/ITriggerTranslatorTool.h"

#include "LumiBlockComps/ILuminosityTool.h"
#include "TrigDecisionInterface/ITrigDecisionTool.h"
#include "LumiBlockComps/ITrigLivefractionTool.h"

#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"

class AthMonitorAlgorithm : public AthReentrantAlgorithm {
public:


    /** 
     * Constructor
     */
    AthMonitorAlgorithm(const std::string& name, ISvcLocator* pSvcLocator );


    /**
     * Destructor
     */
    virtual ~AthMonitorAlgorithm();


    /** 
     * initialize
     * 
     * @return StatusCode
     */
    virtual StatusCode initialize() override;


    /** 
     * Applies filters and trigger requirements. Then, calls fillHistograms().
     * 
     * @param ctx event context for reentrant Athena call
     * @return StatusCode
     */
    virtual StatusCode execute(const EventContext& ctx) const override;


    /** 
     * adds event to the monitoring histograms
     * 
     * User will overwrite this function. Histogram booking is no longer done in C++. 
     * This function is called in execute once the filters are all passed.
     *
     * @param ctx forwarded from execute
     * @return StatusCode
     */
    virtual StatusCode fillHistograms(const EventContext& ctx) const = 0;


    /**
     * Specifies the processing environment.
     * 
     * The running environment may be used to select which histograms are produced, and 
     * where they are located in the output. For example, the output paths of the 
     * histograms are different for the "user", "online" and the various offline flags. 
     * Strings of the same names may be given as jobOptions.
     */
    enum class Environment_t {
        user = 0, ///< 
        online, ///< 
        tier0, ///< 
        tier0Raw, ///< 
        tier0ESD, ///< 
        AOD, ///< 
        altprod, ///< 
    };


    /**
     * Specifies what type of input data is being monitored.
     * 
     * An enumeration of the different types of data the monitoring application may be 
     * running over. This can be used to select which histograms to produce, e.g. to 
     * prevent the production of colliding-beam histograms when running on cosmic-ray data.
     * Strings of the same names may be given as jobOptions.
     */
    enum class DataType_t {
        userDefined = 0, ///< 
        monteCarlo, ///< 
        collisions, ///< 
        cosmics, ///< 
        heavyIonCollisions, ///< 
    };


    /** 
     * Accessor functions for the environment.
     *
     * @return the current value of the class's Environment_t instance.
     */
    Environment_t environment() const;


    /**
     * Convert the environment string from the python configuration to an enum object.
     *
     * @return a value in the Environment_t enumeration which matches the input string.
     */
    Environment_t envStringToEnum( const std::string& str ) const;


    /** 
     * Accessor functions for the data type.
     *
     * @return the current value of the class's DataType_t instance.
     */
    DataType_t dataType() const;


    /** 
     * Convert the data type string from the python configuration to an enum object.
     * 
     * @return a value in the DataType_t enumeration which matches the input string.
     */
    DataType_t dataTypeStringToEnum( const std::string& str ) const;


    /** 
     * Get a specific monitoring tool from the tool handle array.
     * 
     * Finds a specific GenericMonitoringTool instance from the list of monitoring
     * tools (a ToolHandleArray). Throws a FATAL warning if the object found is a
     * null pointer.
     * 
     * @param name string name of the desired tool
     * @return reference to the desired monitoring tool
     */
    GenericMonitoringTool& getGroup( const std::string& name ) const;


    /** 
     * Get the trigger decision tool member.
     * 
     * The trigger decision tool is used to check whether a specific trigger is 
     * passed by an event.
     * 
     * @return m_trigDecTool
     */
    const ToolHandle<Trig::ITrigDecisionTool>& getTrigDecisionTool();


    /** 
     * Check whether triggers are passed
     * 
     * For the event, use the trigger decision tool to check that at least one 
     * of the triggers listed in the supplied vector is passed.
     * 
     * @param vTrigNames List of trigger names.
     * @return If empty input, default to true. If at least one trigger is 
     *  specified, returns whether at least one trigger was passed.
     */
    bool trigChainsArePassed( const std::vector<std::string>& vTrigNames ) const;

    /** 
     * Return a ReadHandle for an EventInfo object (get run/event numbers, etc.)
     *
     * @param ctx EventContext for the event
     * @return a SG::ReadHandle<xAOD::EventInfo>
     */
    SG::ReadHandle<xAOD::EventInfo> GetEventInfo(const EventContext&) const;

    /** @defgroup lumi Luminosity Functions
     *  A group of functions which all deal with calculating luminosity.
     *  @{
     */

    /**
     * Calculate the average mu, i.e. <mu>.
     */
    virtual float lbAverageInteractionsPerCrossing() const;

    /**
     * Calculate instantaneous number of interactions, i.e. mu.
     */
    virtual float lbInteractionsPerCrossing() const;

    /**
     * Calculate average luminosity (in ub-1 s-1 => 10^30 cm-2 s-1).
     */
    virtual float lbAverageLuminosity() const;

    /**
     * Calculate the instantaneous luminosity per bunch crossing.
     */
    virtual float lbLuminosityPerBCID() const;

    /**
     *  Calculate the duration of the luminosity block (in seconds)
     */
    virtual double lbDuration() const;

    /**
     * Calculate the average luminosity livefraction
     */
    virtual float lbAverageLivefraction() const;

    /**
     * Calculate the live fraction per bunch crossing ID.
     */
    virtual float livefractionPerBCID() const;

    /**
     * Calculate the average integrated luminosity multiplied by the live fraction.
     */
    virtual double lbLumiWeight() const;

    /** @} */ // end of lumi group


    /**
     * Parse a string into a vector
     * 
     * The input string is a single long string of all of the trigger names. It parses this string
     * and turns it into a vector, where each element is one trigger or trigger category.
     * 
     * @param line The input string.
     * @param result The parsed output vector of strings.
     * @return StatusCode
     */
    virtual StatusCode parseList( const std::string& line, std::vector<std::string>& result );


    /**
     * Expands trigger categories.
     * 
     * Searches through the vector of trigger names. If a trigger category is found, it finds the 
     * constituent triggers in that category and expands the vector to include each one.
     * 
     * @param vTrigChainNames A vector of triggers which is modified.
     */
    virtual void unpackTriggerCategories( std::vector<std::string>& vTrigChainNames );


protected:
    // Using the new way to declare JO properties: Gaudi::Property<int> m_myProperty {this,"MyProperty",0};

    ToolHandleArray<GenericMonitoringTool> m_tools {this,"GMTools",{}}; ///< Array of Generic Monitoring Tools
    ToolHandle<Trig::ITrigDecisionTool> m_trigDecTool {this,"TrigDecisionTool",""}; ///< Tool to tell whether a specific trigger is passed
    ToolHandle<ITriggerTranslatorTool> m_trigTranslator {this,"TriggerTranslatorTool",""}; ///< Tool to unpack trigger categories into a trigger list
    ToolHandleArray<IDQFilterTool> m_DQFilterTools {this,"FilterTools",{}}; ///< Array of Data Quality filter tools

    ToolHandle<ILuminosityTool> m_lumiTool {this,"lumiTool","LuminosityTool"}; ///< Tool for calculating various luminosity quantities
    ToolHandle<ITrigLivefractionTool> m_liveTool {this,"liveTool","TrigLivefractionTool"}; ///< Tool for calculating various live luminosity quantities

    AthMonitorAlgorithm::Environment_t m_environment; ///< Instance of the Environment_t enum
    AthMonitorAlgorithm::DataType_t m_dataType; ///< Instance of the DataType_t enum
    Gaudi::Property<std::string> m_environmentStr {this,"Environment","user"}; ///< Environment string pulled from the job option and converted to enum
    Gaudi::Property<std::string> m_dataTypeStr {this,"DataType","userDefined"}; ///< DataType string pulled from the job option and converted to enum

    Gaudi::Property<std::string> m_triggerChainString {this,"TriggerChain",""}; ///< Trigger chain string pulled from the job option and parsed into a vector
    std::vector<std::string> m_vTrigChainNames; ///< Vector of trigger chain names parsed from trigger chain string

    Gaudi::Property<std::string> m_fileKey {this,"FileKey",""}; ///< Internal Athena name for file
    bool m_hasRetrievedLumiTool; ///< Allows use of various luminosity functions
    Gaudi::Property<bool> m_useLumi {this,"EnableLumi",false}; ///< Allows use of various luminosity functions
    Gaudi::Property<float> m_defaultLBDuration {this,"DefaultLBDuration",60.}; ///< Default duration of one lumi block
    Gaudi::Property<int> m_detailLevel {this,"DetailLevel",0}; ///< Sets the level of detail used in the monitoring
    SG::ReadHandleKey<xAOD::EventInfo> m_EventInfoKey {this,"EventInfoKey","EventInfo"}; ///< Key for retrieving EventInfo from StoreGate
};

#endif
