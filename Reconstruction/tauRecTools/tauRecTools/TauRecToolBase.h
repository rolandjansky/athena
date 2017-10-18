/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLBASE_H
#define TAURECTOOLBASE_H
/**
 * @brief The base class for all tau tools.
 * 
 * @author Lukasz Janyst
 * @author Justin Griffiths
 * Thanks to Lianyou Shan, Lorenz Hauswald
 */

#include <string>

#include "tauRecTools/TauEventData.h"
#include "tauRecTools/ITauToolBase.h"
#include "AsgTools/AsgTool.h"

extern TauEventData defaultTauEventData;

class TauRecToolBase : public asg::AsgTool, virtual public ITauToolBase {
 public:

  ASG_TOOL_INTERFACE(TauRecToolBase)
  ASG_TOOL_CLASS1( TauRecToolBase, ITauToolBase )

    TauRecToolBase(const std::string& name);
  virtual ~TauRecToolBase() {}

  //-----------------------------------------------------------------
  //! Tool initializer
  //-----------------------------------------------------------------
  virtual StatusCode initialize();

  //-----------------------------------------------------------------
  //! Event initializer - called at the beginning of each event
  //-----------------------------------------------------------------
  virtual StatusCode eventInitialize();

  //-----------------------------------------------------------------
  //! Execute - called for each tau candidate
  //-----------------------------------------------------------------
  virtual StatusCode execute(xAOD::TauJet& pTau);

  //-----------------------------------------------------------------
  //! Event finalizer - called at the end of each event
  //-----------------------------------------------------------------
  virtual StatusCode eventFinalize();

  //-----------------------------------------------------------------
  //! Finalizer
  //-----------------------------------------------------------------
  virtual StatusCode finalize();

  std::string find_file(const std::string& fname) const;
  StatusCode readConfig();

  void setTauEventData(TauEventData* data);
  TauEventData* tauEventData();
  const TauEventData* tauEventData() const;
  
  bool inTrigger() const override;

 protected:
  TauEventData* m_data = 0;
  bool m_in_trigger = false;
  std::string m_tauRecToolsTag;

};

inline bool TauRecToolBase::inTrigger() const { return m_in_trigger; }

inline TauEventData* TauRecToolBase::tauEventData() { 
  return m_data;
}

inline const TauEventData* TauRecToolBase::tauEventData() const { 
  return m_data;
}

#endif //TAURECTOOLBASE_H
