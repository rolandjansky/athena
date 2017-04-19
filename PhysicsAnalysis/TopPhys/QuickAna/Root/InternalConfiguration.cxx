/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//        Copyright Iowa State University 2015.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <QuickAna/InternalConfiguration.h>

#include <QuickAna/Configuration.h>
#include <RootCoreUtils/Assert.h>

//
// method implementations
//

namespace ana
{
  void InternalConfiguration ::
  fetchConfiguration (const Configuration& conf)
  {
    m_isData = conf.isDataFlag;
    m_isAFII = conf.isAFIIFlag;
    m_doingMet = conf.metDef != "none";
    m_muDataFiles = conf.muDataFiles;
    m_msgLevel = MSG::Level (conf.msgLevel);
    if (!conf.selectionNameMET.empty())
      m_selectionName[unsigned (SelectionStep::MET)] = conf.selectionNameMET;
    else
      m_selectionName[unsigned (SelectionStep::MET)] = "ana_select_met";
    if (!conf.selectionNameOR.empty())
      m_selectionName[unsigned (SelectionStep::OR)] = conf.selectionNameOR;
    else
      m_selectionName[unsigned (SelectionStep::OR)] = "ana_select_or";
    if (!conf.selectionName.empty())
      m_selectionName[unsigned (SelectionStep::ANALYSIS)] = conf.selectionName;
    else
      m_selectionName[unsigned (SelectionStep::ANALYSIS)] = "ana_select";
  }



  bool InternalConfiguration ::
  isData () const
  {
    return m_isData;
  }


  bool InternalConfiguration ::
  isAFII () const
  {
    return m_isAFII;
  }


  MSG::Level InternalConfiguration ::
  msgLevel () const
  {
    return m_msgLevel;
  }


  std::string InternalConfiguration ::
  muonWP () const
  {
    return m_muonWP;
  }


  std::string InternalConfiguration ::
  muonIsolationWP () const
  {
    return m_muonIsoWP;
  }


  void InternalConfiguration ::
  setMuonWP (std::string WP)
  {
    m_muonWP = WP;
  }


  void InternalConfiguration ::
  setMuonIsolationWP (std::string WP)
  {
    m_muonIsoWP=WP;
  }



  std::string InternalConfiguration ::
  electronWP () const
  {
    return m_eleWP;
  }


  std::string InternalConfiguration ::
  electronIsolationWP () const
  {
    return m_eleIsoWP;
  }


  void InternalConfiguration ::
  setElectronWP (std::string WP)
  {
    m_eleWP=WP;
  }


  void InternalConfiguration ::
  setElectronIsolationWP (std::string WP)
  {
    m_eleIsoWP=WP;
  }


  const std::string& InternalConfiguration ::
  selectionName (SelectionStep step) const
  {
    if (unsigned (step) >= EnumSize<SelectionStep>::dataSize)
    {
      static const std::string empty;
      return empty;
    }
    return m_selectionName[unsigned (step)];
  }



  const std::string& InternalConfiguration ::
  inputName (ObjectType type) const
  {
    return m_inputName[unsigned(type)];
  }



  void InternalConfiguration ::
  setInputName (ObjectType type, const std::string& val_inputName)
  {
    RCU_ASSERT_SOFT (m_inputName[unsigned(type)].empty());
    m_inputName[unsigned(type)] = val_inputName;
  }
}
