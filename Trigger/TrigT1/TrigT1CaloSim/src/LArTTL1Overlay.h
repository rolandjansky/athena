// -*- C++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ================================================
// LArTTL1Overlay class description
// ================================================
//
// THIS TEXT TO BE REPLACED BY ATLAS STANDARD FORMAT
//
//
// class: LArTTL1Overlay
//
// Description:
//
//The LArTTL1Overlay class takes calorimeter cells from the TES and
// forms Trigger Towers, which it then places back into the TES  The calorimeter
// cells can be produced either by GEANT or other fast simulation packages
// - this is defined by setting a parameter
// CellType to 1 for CaloCells, 2 to reprocess TriggerTowers and 3 for LAr/Tile TTL1 input (a simulation of analogue towers);
//
// ................................................................
//

#ifndef TRIGT1CALOSIM_LARTTL1OVERLAY_H
#define TRIGT1CALOSIM_LARTTL1OVERLAY_H

// STL
#include <map>
#include <string>
#include <vector>

// Athena/Gaudi
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "Identifier/Identifier.h"

// Calorimeter tower includes
#include "LArRawEvent/LArTTL1Container.h"

namespace LVL1
{

class LArTTL1Overlay : public AthReentrantAlgorithm
{
public:
  LArTTL1Overlay(const std::string& name, ISvcLocator* pSvcLocator);

  // main AthReentrantAlgorithm methods
  virtual StatusCode initialize() override final;
  virtual StatusCode execute(const EventContext& ctx) const override final;

private:
  // locations of background TTL1 data
  SG::ReadHandleKey<LArTTL1Container> m_bkgEmTTL1Key{this,"BkgEmTTL1Key","Bkg_LArTTL1EM","ReadHandleKey for Background Input EM LArTTL1Container"};
  SG::ReadHandleKey<LArTTL1Container> m_bkgHadTTL1Key{this,"BkgHadTTL1Key","Bkg_LArTTL1HAD","ReadHandleKey for Background Input Had LArTTL1Container"};
  // locations of signal TTL1 data
  SG::ReadHandleKey<LArTTL1Container> m_signalEmTTL1Key{this,"SignalEmTTL1Key","Sig_LArTTL1EM","ReadHandleKey for Signal Input EM LArTTL1Container"};
  SG::ReadHandleKey<LArTTL1Container> m_signalHadTTL1Key{this,"SignalHadTTL1Key","Sig_LArTTL1HAD","ReadHandleKey for Signal Input Had LArTTL1Container"};
  // locations of output TTL1 data
  SG::WriteHandleKey<LArTTL1Container> m_outputEmTTL1Key{this,"OutputEmTTL1Key","LArTTL1EM","WriteHandleKey for Output EM LArTTL1Container"};
  SG::WriteHandleKey<LArTTL1Container> m_outputHadTTL1Key{this,"OutputHadTTL1Key","LArTTL1HAD","WriteHandleKey for Output Had LArTTL1Container"};

  // overlay amplitudes from other TTL1
  void groupTowers(const LArTTL1Container *towers,
                   std::map<Identifier, std::vector<const LArTTL1*>> &towerMap) const;

  // specialised overlay functions
  StatusCode overlay(const EventContext& ctx,
                     const SG::ReadHandleKey<LArTTL1Container> &bkgKey,
                     const SG::ReadHandleKey<LArTTL1Container> &signalKey,
                     const SG::WriteHandleKey<LArTTL1Container> &outputKey,
                     const std::string &label) const;
};

} // namespace LVL1
#endif
