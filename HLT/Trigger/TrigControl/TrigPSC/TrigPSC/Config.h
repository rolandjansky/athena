/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TrigPSC/Config.h
 * @author Frank Winklmeier, Ricardo Abreu
 *
 * @brief PSC Configuration type
 */

#ifndef TRIGPSC_CONFIG_H
#define TRIGPSC_CONFIG_H

#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <boost/property_tree/ptree.hpp>

namespace TrigConf
{
  struct TrigDBConnectionConfig;
}

namespace psc {
  
  /**
   * PSC configuration object
   */
  class Config
  {
  public:
    Config(const boost::property_tree::ptree& config);

    void prepareWorker(const boost::property_tree::ptree& args);

    /**
     * Returns a string with all options which are held in the options cache
     */
    std::string dumpOptions() const;

    /**
     * Gets an option in a "safer" way
     */
    std::string getOption(const std::string& key) const;

    /**
     * Incredibly simple minded way to access this object from python.
     * Create a python dictionary with key/value pairs.
     */
    std::string toPython(const std::string& dictName) const;

    /**
     * Get a string representing the actual number value of the first LOGLEVEL
     * in this configuration.
     */
    std::string getLogLevelAsNumStr() const;

    /**
     * Returns whether the user specified an explicit log level
     */
    bool didUserSetLogLevel() const;

  public:

    /**
     * Map with name/value pairs of parameters
     */
    std::map<std::string, std::string>  optmap;

    /**
     * Lists of statically enabled ROBs and sub-detectors  
     */
    std::vector<uint32_t> enabled_robs;
    std::vector<uint32_t> enabled_SubDets;

  private:

    // setup needed optmap defaults for options that may not always be filled
    void setup_optmap_defaults();

    // fill in options for HLTImplementationJobOptions
    void fillopt_jo(const boost::property_tree::ptree& hlt);

    // fill in options for HLTImplementationDB
    void fillopt_db(const boost::property_tree::ptree& hlt);

    // fill in options for HLTImplementationDBPython
    void fillopt_dbpy(const boost::property_tree::ptree& hlt);

    // fill in what is common to jo and dbpy
    void fillopt_py(const boost::property_tree::ptree& hlt);

    // fill in common options
    void fillopt_common(const boost::property_tree::ptree& hlt);

    // fill in athenaHLT specific options
    void fillopt_athenaHLT();

    // sets fields with info on this process
    void setPInfo();

    // fill the vector of enabled ROBs and sub-detectors and the vectors of MET ROBs for
    // LAr and Tile
    void fill_enabled_robs(const boost::property_tree::ptree& ros2robs);
    void fill_enabled_dets(const boost::property_tree::ptree& ros2robs);

    // fill in the TrigDBConnectionConfig appropriately
    void fill_dbCon(const boost::property_tree::ptree& hlt,
                    TrigConf::TrigDBConnectionConfig& dbcon);

    void updatePids();
    void updateSeed();

  private:
    const boost::property_tree::ptree& m_config;

  };
}

#endif /* TRIGPSC_CONFIG_H */
