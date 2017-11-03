/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTruthBase_TruthStrategyManager_H
#define MCTruthBase_TruthStrategyManager_H

#include "AthenaKernel/MsgStreamMember.h"

// ISF include
#include "ISF_Interfaces/ITruthSvc.h"
#include "ISF_Interfaces/IGeoIDSvc.h"

/// Forward declarations
class G4Step;


/// @brief Singleton class for some truth stuff (???)
/// NEEDS DOCUMENTATION
///
class TruthStrategyManager
{

public:

  /// Retrieve the singleton instance
  static TruthStrategyManager* GetStrategyManager();

  /// Returns true if any of the truth strategies return true
  bool CreateTruthIncident(const G4Step*, int numSecondaries);

  /// Define which ISF TruthService to use
  void SetISFTruthSvc(ISF::ITruthSvc *truthSvc);

  /// Define which ISF GeoIDSvc to use
  void SetISFGeoIDSvc(ISF::IGeoIDSvc *geoIDSvc);

  StatusCode InitializeWorldVolume();

private:
  TruthStrategyManager();
  TruthStrategyManager(const TruthStrategyManager&) = delete;
  TruthStrategyManager& operator=(const TruthStrategyManager&) = delete;

  /// Log a message using the Athena controlled logging system
  MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
  /// Check whether the logging system is active at the provided verbosity level
  bool msgLvl( MSG::Level lvl ) const { return m_msg.get().level() <= lvl; }
  /// Private message stream member
  mutable Athena::MsgStreamMember m_msg;

  /// ISF Services the TruthStrategyManager talks to
  ISF::ITruthSvc* m_truthSvc;
  ISF::IGeoIDSvc* m_geoIDSvc;

  /// The level in the G4 volume hierarchy at which can we find the sub-detector name
  int m_subDetVolLevel;
};

#endif
