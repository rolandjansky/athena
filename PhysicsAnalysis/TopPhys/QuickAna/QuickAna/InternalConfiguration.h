/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef QUICK_ANA__INTERNAL_CONFIGURATION_H
#define QUICK_ANA__INTERNAL_CONFIGURATION_H

#include <QuickAna/Global.h>

#include <QuickAna/SelectionStep.h>
#include <AsgTools/MsgLevel.h>
#include <string>
#include <vector>

namespace ana
{
  /// \brief This structure holds internal configuration information,
  /// based on the overall QuickAna configuration as well as the
  /// configuration of the individual tools/objects.

  class InternalConfiguration
  {
    //
    // public interface
    //

    /// \brief fetch the configuration options from the global
    /// configuration
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   out of memory II
  public:
    void fetchConfiguration (const Configuration& conf);


    /// \brief whether we are running on data
    /// \par Guarantee
    ///   no-fail
  public:
    bool isData () const;


    /// \brief whether we are running on AFII MC
    /// \par Guarantee
    ///   no-fail
  public:
    bool isAFII () const;


    /// \brief is MET being configured?
  public:
    bool doingMET() const
    { return m_doingMet; }

    /// \brief get the list of LumiCalc files
  public:
    const std::vector<std::string>& muDataFiles() const
    { return m_muDataFiles; }

    /// \brief global message level for QuickAna internal tools
    /// \par Guarantee
    ///   no-fail
  public:
    MSG::Level msgLevel() const;


    /// \brief what electron WP is the key one (for SFs)
    /// \par Guarantee
    ///   no-fail
  public:
    std::string electronWP () const;
    std::string electronIsolationWP () const;
    void setElectronWP(std::string WP);
    void setElectronIsolationWP(std::string WP);

    /// \brief what muon WP is the key one (for SFs)
    /// \par Guarantee
    ///   no-fail
  public:
    std::string muonWP () const;
    std::string muonIsolationWP () const;
    void setMuonWP(std::string WP);
    void setMuonIsolationWP(std::string WP);


    /// \brief the names of the decorations for the different
    /// selection stages.
    ///
    /// this is set from the global configuration object.
    /// \par Guarantee
    ///    no-fail
  public:
    const std::string& selectionName (SelectionStep step) const;


    /// \brief the input container name for the given object type
    ///
    /// this is used for the output tool, as well as to communicate
    /// the jet collection used to the MET tool
    /// \par Guarantee
    ///    no-fail
  public:
    const std::string& inputName (ObjectType type) const;

    /// \brief set the value of \ref inputName
    /// \par Guarantee
    ///    strong
    /// \par Failures
    ///    out of memory II\n
    ///    inputName already set\n
    ///    object type does not correspond to actual objects
  public:
    void setInputName (ObjectType type, const std::string& val_inputName);


  private:
    //
    // private interface
    //

    /// \brief the value of \ref isData
    bool m_isData;

    /// \brief the value of \ref isAFII
    bool m_isAFII;

    /// \brief whether MET is configured in current job
    bool m_doingMet;

    /// \brief the LumicCalc muDataFiles for luminosity fractions.
    /// Currently used by the PileupReweightTool in the muon SF tools.
    std::vector<std::string> m_muDataFiles;

    /// \brief global tool output level
    MSG::Level m_msgLevel;

    /// \brief the value of \ref ElectronWP
    std::string m_eleWP;

    /// \brief the value of \ref ElectronIsolationWP
    std::string m_eleIsoWP;

    /// \brief the value of \ref MuonWP
    std::string m_muonWP;

    /// \brief the value of \ref MuonIsolationWP
    std::string m_muonIsoWP;

    /// \brief the value of \ref selectionName
    std::string m_selectionName [EnumSize<SelectionStep>::dataSize];

    /// \brief the value of \ref inputName
    std::string m_inputName [EnumSize<ObjectType>::size];
  };
}

#endif
