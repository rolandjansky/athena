/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "TrigPSC/Config.h"

// TDAQ includes
#include "hltinterface/HLTInterface.h"

// Gaudi Includes
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/ISvcLocator.h"

// STL includes
#include <map>
#include <string>
#include <functional>
#include <memory>

// Boost includes
#include <boost/property_tree/ptree.hpp>

class IAppMgrUI;

namespace psc {

  /**
   * Common base class for HLT Pesa Steering Controller.
   */
  class Psc: public hltinterface::HLTInterface
  {
  public:
    /**
     * Default constructor
     */
    Psc() = default;

    /**
     * Virtual desctuctor
     */
    virtual ~Psc();

    /**
     * Configures the framework
     */
    virtual bool configure (const boost::property_tree::ptree& config) override;

    /**
     * Connects the framework
     */
    virtual bool connect (const boost::property_tree::ptree& args) override;

    /**
     * prepares the HLT framework for a run
     */
    virtual bool prepareForRun (const boost::property_tree::ptree& args) override;

    /**
     * stops the HLT framework without re-configuring
     */
    virtual bool stopRun (const boost::property_tree::ptree& args) override;

    /**
     * Disconnects the framework
     */
    virtual bool disconnect (const boost::property_tree::ptree& args) override;

    /**
     * Unconfigures the framework, releasing all acquired resources.
     */
    virtual bool unconfigure (const boost::property_tree::ptree& args) override;

    /**
     * Calls the HLT framework to publish statistics, after the run has
     * finished.
     */
    virtual bool publishStatistics (const boost::property_tree::ptree& args) override;

    /**
     * Calls the HLT framework to notify it that a user command has arrived
     */
    virtual bool hltUserCommand (const boost::property_tree::ptree& args) override;

    /**
     * Starts the HLT event loop. The HLT framework will start requesting and processing events.
     */
    virtual bool doEventLoop () override;

    /**
     * Method which can be called for a worker to perform the necessary steps to set
     * unique worker IDs and adapt histogram paths
     */
    virtual bool prepareWorker (const boost::property_tree::ptree& args) override;

    /**
     * Method which can be called for a worker to perform a cleanup before the
     * worker gets killed
     */
    virtual bool finalizeWorker (const boost::property_tree::ptree& args) override;

  private:
    bool setDFProperties(std::map<std::string, std::string> name_tr_table);
    bool setAthenaProperties();

    /// Initialize the application manager
    bool doAppMgrInit();
    /// Finalize the application manager
    bool doAppMgrFinalize();

    /**
     * Utility method to call a method on the event loop manager
     * @param func the function to call
     * @param name the name of the function func
     * @return The "AND" of all StatusCodes
     */
    template <typename T>
    StatusCode callOnEventLoopMgr(std::function<StatusCode (T*)> func,
                                  const std::string& name) const;

  private:
    uint32_t                      m_run_number{0};       ///< (initial) run number to be used for this run
    IAppMgrUI*                    m_pesaAppMgr{nullptr}; ///< Application Manager
    std::string                   m_nameEventLoopMgr;    ///< name of the event loop manager
    bool                          m_interactive{false};  ///< Running in interactive mode (athenaHLT)
    std::unique_ptr<psc::Config>  m_config{nullptr};     ///< Config derived from ptree
    int                           m_workerID{0};         ///< worker ID (0=mother)

    SmartIF<ISvcLocator> m_svcLoc;     ///< Service locator handle
  };
}

#endif /* TRIGPSC_PSC_H */
