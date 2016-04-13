/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTruthBase_TruthStrategy_H
#define MCTruthBase_TruthStrategy_H

#include "MCTruthBase/TruthStrategyManager.h"
#include "AthenaKernel/MsgStreamMember.h"
#include <string>
#include <map>

/// Forward declarations
class G4Step;
class G4LogicalVolume;

/// @brief Base class for the truth strategies
///
/// TODO: NEEDS DOCUMENTATION
///
class TruthStrategy {
public:
  TruthStrategy(const std::string& name);
  virtual ~TruthStrategy(){};

  std::string StrategyName() {return strategyName;}
  //std::string VolumeName() {return volumeName;}
  //int VolumeLevel() {return volumeLevel;}
  virtual bool IsApplicable(const G4Step*);
  virtual bool AnalyzeVertex(const G4Step*) = 0;
  bool Activate(const std::string, int);
  void Disactivate() {MCActiveArea.clear();activated=false;}
  bool IsActivated() {return activated;}

protected:
  TruthStrategyManager* theTruthManager;

  /// Log a message using the Athena controlled logging system
  MsgStream& msg(MSG::Level lvl) const { return m_msg << lvl; }
  /// Check whether the logging system is active at the provided verbosity level
  bool msgLvl(MSG::Level lvl) const { return m_msg.get().level() <= lvl; }
  /// Private message stream member
  mutable Athena::MsgStreamMember m_msg;

  /// This method is here for backwards compatibility
  MsgStream& log() { return m_msg.get(); }

private:
  std::string strategyName;
  std::map<std::string,int> MCActiveArea;//key=volumeName, val=volumeLevel
  bool activated;
  bool m_init;
  void checkVolumeDepth( G4LogicalVolume * lv , int d=0 );
};

#endif
