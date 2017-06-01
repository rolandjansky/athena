/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTruthBase_TruthStrategyManager_H
#define MCTruthBase_TruthStrategyManager_H

#include <string>
#include <map>
#include <vector>

#include "HepMC/GenEvent.h"
#include "AthenaKernel/MsgStreamMember.h"

// ISF include
#include "ISF_Interfaces/ITruthSvc.h"
#include "ISF_Interfaces/IGeoIDSvc.h"

// Truth-related includes
#include "SimHelpers/SecondaryTracksHelper.h"

/// Forward declarations
class TruthStrategy;
class G4Step;
class G4Track;
class G4StepPoint;
class EventInformation;

class G4LogicalVolume;
class G4VPhysicalVolume;


/// @brief Singleton class for some truth stuff (???)
/// NEEDS DOCUMENTATION
///
class TruthStrategyManager
{

public:

  /// Retrieve the singleton instance
  static TruthStrategyManager* GetStrategyManager();
  bool IsApplicable() {return !isEmpty;}

  /// Returns true if any of the truth strategies return true
  bool AnalyzeVertex(const G4Step*);

  /// Get/set strategies
  void RegisterStrategy(TruthStrategy *);
  TruthStrategy* GetStrategy(const std::string&);
  void ListStrategies(); // Non-const as logger may need to be grabbed

  /// Get/set some parameters
  void SetNrOfSecondaries(int i) {nSecondaries=i;}
  int GetNrOfSecondaries() {return nSecondaries;}
  void SetSecondarySavingLevel(int i) {secondarySavingLevel=i;}
  int GetSecondarySavingLevel() const {return secondarySavingLevel;}

  /// Get and set truth parameters.
  void SetTruthParameter(const std::string&, double);
  double GetTruthParameter(const std::string&);
  void PrintParameterList(); // Non-const as logger may need to be grabbed

  /// Retrieves EventInformation from G4 EventManager.
  EventInformation* GetEventInformation() const;

  /// Retrieves secondaries from a static SecondaryTracksHelper.
  std::vector<G4Track*> GetSecondaries();

  /// Construct and return a GenVertex from the space-time 4-vector of the step point.
  HepMC::GenVertex* StepPoint2Vertex(G4StepPoint*) const;
  /// Construct and return a GenParticle from the track kinematics and pdg ID.
  HepMC::GenParticle* Track2Particle(G4Track*) const;

  /// Construct GenVertex, fill with GenParticles, and save it to GenEvent.
  /// N.B. using this isn't thread-safe. Migrate to TruthStrategyUtils instead.
  void SaveSecondaryVertex(G4Track*, G4StepPoint*, const std::vector<G4Track*>&) const;

  /// Define which ISF TruthService to use
  void SetISFTruthSvc(ISF::ITruthSvc *truthSvc);

  /// Define which ISF GeoIDSvc to use
  void SetISFGeoIDSvc(ISF::IGeoIDSvc *geoIDSvc);

  StatusCode InitializeWorldVolume();

private:
  TruthStrategyManager();
  TruthStrategyManager(const TruthStrategyManager&) = delete;
  TruthStrategyManager& operator=(const TruthStrategyManager&) = delete;

  /// Map of truth strategies keyed by name.
  std::map<std::string, TruthStrategy*> theStrategies;
  bool isEmpty;

  int secondarySavingLevel;

  /// cache the nr. of secondaries produced in an interaction here, on
  /// request, to avoid too many tours in the G4 meanders...
  int nSecondaries;
  //std::vector<G4Track*> theSecondaries; // not used..

  /// Here go the parameters needed by all various strategies
  std::map<std::string, double> truthParams;

  /// Log a message using the Athena controlled logging system
  MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
  /// Check whether the logging system is active at the provided verbosity level
  bool msgLvl( MSG::Level lvl ) const { return m_msg.get().level() <= lvl; }
  /// Private message stream member
  mutable Athena::MsgStreamMember m_msg;

  /// ISF Services the TruthStrategyManager talks to
  ISF::ITruthSvc* m_truthSvc;
  ISF::IGeoIDSvc* m_geoIDSvc;
  SecondaryTracksHelper m_sHelper; // needed for the Geant4TruthIncident

  /// The level in the G4 volume hierarchy at which can we find the sub-detector name
  int m_subDetVolLevel;
};

#endif
