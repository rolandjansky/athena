// -*- C++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ================================================
// TileTTL1Overlay class description
// ================================================
//
// THIS TEXT TO BE REPLACED BY ATLAS STANDARD FORMAT
//
//
// class: TileTTL1Overlay
//
// Description:
//
//The TileTTL1Overlay class takes calorimeter cells from the TES and
// forms Trigger Towers, which it then places back into the TES  The calorimeter
// cells can be produced either by GEANT or other fast simulation packages
// - this is defined by setting a parameter
// CellType to 1 for CaloCells, 2 to reprocess TriggerTowers and 3 for LAr/Tile TTL1 input (a simulation of analogue towers);
//
// ................................................................
//

#ifndef TRIGT1CALOSIM_TILETTL1OVERLAY_H
#define TRIGT1CALOSIM_TILETTL1OVERLAY_H

// STL
#include <map>
#include <string>
#include <vector>

// Athena/Gaudi
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "Identifier/Identifier.h"

// Calorimeter tower includes
#include "TileEvent/TileTTL1Container.h"

namespace LVL1
{

class TileTTL1Overlay : public AthReentrantAlgorithm
{
public:
  TileTTL1Overlay(const std::string& name, ISvcLocator* pSvcLocator);

  // main AthReentrantAlgorithm methods
  virtual StatusCode initialize() override final;
  virtual StatusCode execute(const EventContext& ctx) const override final;

private:
  // locations of background TTL1 data
  SG::ReadHandleKey<TileTTL1Container> m_bkgTileTTL1Key{this,"BkgTileTTL1Key","Bkg_TileTTL1Cnt","ReadHandleKey for Background Input TileTTL1Container"};
  SG::ReadHandleKey<TileTTL1Container> m_bkgTileMBTSTTL1Key{this,"BkgTileMBTSTTL1Key","Bkg_TileTTL1MBTS","ReadHandleKey for Background Input MBTS TileTTL1Container"};
  // locations of signal TTL1 data
  SG::ReadHandleKey<TileTTL1Container> m_signalTileTTL1Key{this,"SignalTileTTL1Key","Sig_TileTTL1Cnt","ReadHandleKey for Signal Input TileTTL1Container"};
  SG::ReadHandleKey<TileTTL1Container> m_signalTileMBTSTTL1Key{this,"SignalTileMBTSTTL1Key","Sig_TileTTL1MBTS","ReadHandleKey for Signal Input MBTS TileTTL1Container"};
  // locations of output TTL1 data
  SG::WriteHandleKey<TileTTL1Container> m_outputTileTTL1Key{this,"OutputTileTTL1Key","TileTTL1Cnt","WriteHandleKey for Output TileTTL1Container"};
  SG::WriteHandleKey<TileTTL1Container> m_outputTileMBTSTTL1Key{this,"OutputTileMBTSTTL1Key","TileTTL1MBTS","WriteHandleKey for Output MBTS TileTTL1Container"};

  // overlay amplitudes from other TTL1
  void groupTowers(const TileTTL1Container *towers,
                   std::map<Identifier, std::vector<const TileTTL1*>> &towerMap) const;

  // specialised overlay functions
  StatusCode overlay(const EventContext& ctx,
                     const SG::ReadHandleKey<TileTTL1Container> &bkgKey,
                     const SG::ReadHandleKey<TileTTL1Container> &signalKey,
                     const SG::WriteHandleKey<TileTTL1Container> &outputKey,
                     const std::string &label) const;
};

} // namespace LVL1
#endif
