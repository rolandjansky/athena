/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
 

#ifndef AFPSILAYERSUMMARYPROFILEBASE_H
#define AFPSILAYERSUMMARYPROFILEBASE_H

#include <AthenaMonitoring/ManagedMonitorToolBase.h>

#include <TProfile.h>

#include <string>

// forward declarations
class AFPHitsMonitorTool;

/// Base class for summary distributions (showing mean value for each plane) 
class AFPSiLayerSummaryProfileBase
{
public:
  /// Creates a TPofile object #m_profile and sets bin labels according to monitors in #m_parentTool.
  AFPSiLayerSummaryProfileBase (AFPHitsMonitorTool* parentTool, const ManagedMonitorToolBase::MonGroup& monGrp, const std::string name, const std::string title);
  
  /// Does nothing. The ownership of the #m_profile object is taken by monitoring service.
  virtual ~AFPSiLayerSummaryProfileBase() {}

  /// Registers the #m_profile as a histogram in #m_parentTool.
  void book ();

  /// Returns the profile object #m_profile.
  TProfile* profile () const {return m_profile;}

protected:
  /// The summary distribution. TProfile ensures proper merging of mean values.
  TProfile* m_profile;

  /// Pointer to the parent tool in which the distribution is registered.
  AFPHitsMonitorTool* m_parentTool;

  /// @brief Configuration of the monitoring options.
  ///
  /// Defines, among others, how often the distribution should be
  /// renewed e.g. every luminosity block or every run and sets where
  /// the distribution should be saved i.e. in which ROOT file directory.
  ManagedMonitorToolBase::MonGroup m_monGrp;
};

#endif
