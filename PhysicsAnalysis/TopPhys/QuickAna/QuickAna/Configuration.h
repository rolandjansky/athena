/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUICK_ANA__CONFIGURATION_H
#define QUICK_ANA__CONFIGURATION_H

//        Copyright Iowa State University 2014.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.



#include <QuickAna/Global.h>

#include <AsgTools/MsgLevel.h>

#include <string>
#include <vector>

namespace ana
{
  /// \brief the overall configuration options for QuickAna
  struct Configuration
  {
    /// \brief whether we intent to run on data
    ///
    /// this option may go away at some point if we figure out how to
    /// obtain this information from the input file.
  public:
    bool isDataFlag = false;
    bool isAFIIFlag = false;

    /// global message level to apply to all tools created by QuickAna.
  public:
    unsigned msgLevel = MSG::INFO;

    /// \brief the EventInfo definition to use
  public:
    std::string eventinfoDef;

    /// \brief the EventSelect definition to use
  public:
    std::string eventSelectDef;


    /// \brief the electron definition
    ///
    /// e.g. tight, loose, default, none, etc.
  public:
    std::string electronDef;


    /// \brief the electron kinematic selection
    ///
    /// e.g. pt>50e3
  public:
    std::string electronKine;


    /// \brief the photon definition
    ///
    /// e.g. tight, loose, default, none, etc.
  public:
    std::string photonDef;


    /// \brief the photon kinematic selection
    ///
    /// e.g. pt>50e3
  public:
    std::string photonKine;


    /// \brief the muon definition
    ///
    /// e.g. tight, loose, default, none, etc.
  public:
    std::string muonDef;


    /// \brief the muon kinematic selection
    ///
    /// e.g. pt>50e3
  public:
    std::string muonKine;


    /// \brief the tau definition
    ///
    /// e.g. tight, loose, default, none, etc.
  public:
    std::string tauDef;


    /// \brief the tau kinematic selection
    ///
    /// e.g. pt>50e3
  public:
    std::string tauKine;


    /// \brief the jet definition
    ///
    /// e.g. antikt04, default, none, etc.
  public:
    std::string jetDef;


    /// \brief the jet kinematic selection
    ///
    /// e.g. pt>50e3
  public:
    std::string jetKine;


    /// \brief the fat jet definition
    ///
    /// e.g. antikt10, default, none, etc.
  public:
    std::string fatJetDef;


    /// \brief the fat jet kinematic selection
    ///
    /// e.g. pt>50e3
  public:
    std::string fatJetKine;


    /// \brief the met definition
    ///
    /// e.g. default, none, etc.
  public:
    std::string metDef;


    /// \brief the second met definition
    ///
    /// This is so you can evaluate regular and track MET at the same
    /// time.
  public:
    std::string met2Def;


    /// \brief the overlap removal definition
    ///
    /// e.g. default, none, etc.
  public:
    std::string orDef;


    /// \brief string that contains triggers used seperated by spaces
    ///
    /// e.g. "", "HLT_mu20_iloose_L1MU15", "HLT_mu20_iloose_L1MU15 HLT_e24_tight1_iloose"
  public:
    std::string triggerDef;


    /// \brief the files with the vertex distribution in data
  public:
    std::vector<std::string> muDataFiles;


    /// \brief the file with the vertex distribution for the current MC
  public:
    std::vector<std::string> muMcFiles;


    /// \brief the scheduler to use
  public:
    std::string schedulerDef;


    /// \brief the name of the selection decoration for the MET tool
  public:
    std::string selectionNameMET;


    /// \brief the name of the selection decoration for the OR tool
  public:
    std::string selectionNameOR;


    /// \brief the name of the selection decoration for the final
    /// analysis objects
  public:
    std::string selectionName;


    /// effects: standard default constructor
    /// guarantee: strong
    /// failures: out of memory I
  public:
    Configuration ();


    /// effects: standard destructor
    /// guarantee: no-fail
    /// rationale: virtual destructor for base class
  public:
    virtual ~Configuration () {};


    /// effects: set the configuration to the given one
    /// guarantee: basic
    /// failures: out of memory II
    /// rationale: this is essentially the standard assignment
    ///   operator, but I introduced it as an explicit function to
    ///   make it more clear what is happening and to avoid awkward
    ///   cast or using statements
  public:
    void setConfig (const Configuration& conf);


    /// \brief declare the properties for the given object
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   property declaration failures
  public:
    template<class T> void declareConfigProperties (T& obj);
  };
}

#include <QuickAna/Configuration.icc>

#endif
