/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TruthStrategyManager_H
#define TruthStrategyManager_H

#include <string>
#include <map>
#include <vector>

#include "HepMC/GenEvent.h"
#include "GaudiKernel/MsgStream.h"

class TruthStrategy;
class G4Step;
class G4Track;
class G4StepPoint;
class EventInformation;
class MCTruthSteppingAction;

/// TODO: why is this global? Move into the class.
typedef std::map<std::string,TruthStrategy*, std::less<std::string> > strategyMap;

/// @brief Singleton class for some truth stuff (???)
/// NEEDS DOCUMENTATION
///
/// Some of these methods are poorly motivated, because all they do is
/// forward arguments onto the underlying MCTruthSteppingAction AlgTool.
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
  TruthStrategy * GetStrategy(std::string);
  void ListStrategies(); // Non-const as logger may need to be grabbed

  /// Get/set some parameters
  void SetNrOfSecondaries(int i) {nSecondaries=i;}
  int GetNrOfSecondaries() {return nSecondaries;}
  void SetSecondarySavingLevel(int i) {secondarySavingLevel=i;}
  int GetSecondarySavingLevel() const {return secondarySavingLevel;}

  /// Retrieves the MCTruthSteppingAction from the ToolSvc
  void EnableRecordingEnvelopes();

  /// Forwards the arguments to MCTruthSteppingAction to assign a volume
  /// envelope to a sensitive detector for truth hits writing. This is
  /// a very poorly designed way to configure a tool...
  void AddRecordingEnvelope(std::string,int,std::string sdName="");

  /// Calls ListRecordingEnvelopes on the MCTruthSteppingAction,
  /// which dumps the list of assigned recording envelopes.
  void ListRecordingEnvelopes();

  /// Get and set truth parameters (whatever those are...).
  void SetTruthParameter(const std::string, double);
  double GetTruthParameter(const std::string);
  void PrintParameterList(); // Non-const as logger may need to be grabbed

  /// Retrieves EventInformation from G4 EventManager.
  EventInformation* GetEventInformation() const;

  /// Retrieves secondaries from a static SecondaryTracksHelper.
  std::vector<G4Track*> GetSecondaries();

  /// Constructs and returns a GenVertex from the space-time 4-vector of the step point.
  HepMC::GenVertex* StepPoint2Vertex(G4StepPoint*) const;
  /// Constructs and returns a GenParticle from the track kinematics and pdg ID.
  HepMC::GenParticle* Track2Particle(G4Track*) const;

  /// Construct GenVertex, fill with GenParticles, and save it to GenEvent.
  void SaveSecondaryVertex(G4Track*, G4StepPoint*, std::vector<G4Track*>) const;

private:
  TruthStrategyManager();
  TruthStrategyManager(const TruthStrategyManager&) : m_log(0) {}
  ~TruthStrategyManager() { if (m_log) delete m_log; }
  static TruthStrategyManager *thePointer;

  strategyMap theStrategies;
  bool isEmpty;

  int secondarySavingLevel;

  // cache the nr. of secondaries produced in an interaction here, on
  // request, to avoid too many tours in the G4 meanders...

  int nSecondaries;
  std::vector<G4Track*> theSecondaries;

  // here go the parameters needed by all various strategies

  std::map< std::string, double, std::less<std::string> > truthParams;

  MCTruthSteppingAction *theSteppingAction;

  /// TODO: Don't do this manually! Use a MsgStreamMember instead!
  MsgStream * m_log;
  MsgStream log();

};

#endif
