/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file Psc.h
 * @author Frank Winklmeier, Ricardo Abreu
 *
 * @brief HLT Pesa Steering Controller.
 *
 */

#ifndef TRIGPSC_PSC_H
#define TRIGPSC_PSC_H

// Package includes
#include "hltinterface/HLTInterface.h"
#include "hltinterface/EventId.h"

// TDAQ includes
#include "threads/ThreadData.h"

// Gaudi Includes
#include "GaudiKernel/IEventProcessor.h"
// STL includes
#include <map>
#include <string>
#include <vector>

// Boost includes
#include <boost/function.hpp>
#include <boost/property_tree/ptree.hpp>

class IAppMgrUI;
class ITrigEventLoopMgr;

namespace psc {

  // Fwd decl
  class Config;
  
  /**
   * Common base class for HLT Pesa Steering Controller.
   */
  class Psc: public hltinterface::HLTInterface
  {
  public:    
    /**
     * C'tor. (Nothing happens here...)
     */
    Psc() ;

    /**
     * D'tor virtualisation
     */
    virtual ~Psc() ;

    /**
     * Configures the framework
     */
    virtual bool configure (const boost::property_tree::ptree& config);

    /**
     * Connects the framework
     */
    virtual bool connect (const boost::property_tree::ptree& args);

    /**
     * prepares the HLT framework for a run
     */
    virtual bool prepareForRun (const boost::property_tree::ptree& args);

    /**
     * stops the HLT framework without re-configuring
     */
    virtual bool stopRun (const boost::property_tree::ptree& args);

    /**
     * Disconnects the framework
     */
    virtual bool disconnect (const boost::property_tree::ptree& args);

    /**
     * Unconfigures the framework, releasing all acquired resources.
     */
    virtual bool unconfigure (const boost::property_tree::ptree& args);

    /**
     * Calls the HLT framework to publish statistics, after the run has
     * finished.
     */
    virtual bool publishStatistics (const boost::property_tree::ptree& args);

    /**
     * Calls the HLT framework to notify it that a time out is imminent
     */
    virtual void timeOutReached (const boost::property_tree::ptree& args);

    /**
     * Calls the HLT framework to notify it that a user command has arrived
     */
    virtual bool hltUserCommand (const boost::property_tree::ptree& args);

    /**
     * Process one event, taking as input, the LVL1 Result and producing as
     * output, the HLT Result.
     *
     * @param l1r.  LVL1 result ROBs (input)
     * @param hltr. HLT result (output)
     * @param evId. EventId structure containing global event number l1id and lb
     *  number (input)
     */
    virtual bool
    process(const std::vector<eformat::ROBFragment<const uint32_t*>>& l1r,
            hltinterface::HLTResult& hltr,
            const hltinterface::EventId& evId);

    /**
     * Method which can be called for a worker to perform the necessary steps to set
     * unique worker IDs and adapt histogram paths
     */
    virtual bool prepareWorker (const boost::property_tree::ptree& args);

    /**
     * Method which can be called for a worker to perform a cleanup before the
     * worker gets killed
     */
    virtual bool finalizeWorker (const boost::property_tree::ptree& args);


  private:
    bool setDFProperties(std::map<std::string, std::string> name_tr_table);

    /**
     * Return pointer to (thread-specific) event processor
     */
    virtual IEventProcessor* eventProcessor();

    /**
     * Returns the thread ID suffix
     *
     * @return Thread ID suffix (empty string if nworker = 0)
     */
    std::string getThreadSuffix(int id);

    /**
     * Call the given function for each EventLoopMgr
     *
     * @return The "AND" of all StatusCodes
     */
    template <class T>
    StatusCode foreachEventLoopMgr(boost::function<StatusCode (T*)> func,
                                   const std::string& msg);
    
    /**
     * Initialize the application manager
     */
    bool doAppMgrInit();

  private:
    uint32_t            m_run_number;         ///< (initial) run number to be used for this run
    IAppMgrUI*          m_pesaAppMgr;         ///< Application Manager
    std::string         m_nameEventLoopMgr;   ///< name of the event loop manager
    bool                m_interactive;        ///< Running in interactive mode (athenaMT/PT)
    
    // for handling thread specific data
    ThreadDataPtr<IEventProcessor>           m_eventProcessor ;
    std::map<unsigned int, IEventProcessor*> m_availableEventProcessors ;
    
    // User command handling
    bool           m_failNextUsrCmd;
    unsigned int   m_sleepNextUsrCmd;
    psc::Config * m_config;
  };
}
  
#endif /* TRIGPSC_PSC_H */
