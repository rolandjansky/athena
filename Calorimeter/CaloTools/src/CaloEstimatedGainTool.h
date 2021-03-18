// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file CaloTools/src/CaloEstimatedGain.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2019
 * @brief Estimate gain used to read out a certain energy.
 *
 * Used to be part of ICaloNoiseTool.
 */


#ifndef CALOTOOLS_CALOESTIMATEDGAINTOOL_H
#define CALOTOOLS_CALOESTIMATEDGAINTOOL_H


#include "CaloInterface/ICaloEstimatedGainTool.h"
#include "LArRawConditions/LArADC2MeV.h"
#include "TileConditions/TileCondIdTransforms.h"
#include "TileConditions/TileCondToolNoiseSample.h"
#include "TileConditions/TileCondToolEmscale.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "GaudiKernel/ToolHandle.h"


class LArEM_ID;
class TileInfo;


class CaloEstimatedGainTool
  : public extends<AthAlgTool, ICaloEstimatedGainTool>
{
public:
  using base_class::base_class;


  typedef ICaloEstimatedGainTool::Step Step;


  virtual StatusCode initialize() override;


  virtual CaloGain::CaloGain estimatedGain (const EventContext& ctx,
                                            const CaloCell& caloCell,
                                            const Step step) const override;

  virtual CaloGain::CaloGain estimatedGain (const EventContext& ctx,
                                            const CaloCell& caloCell,
                                            const CaloDetDescrElement& caloDDE,
                                            const Step step) const override;

  virtual CaloGain::CaloGain estimatedGain (const EventContext& ctx,
                                            const CaloDetDescrElement& caloDDE,
                                            const float energy,
                                            const Step step) const override;


private:
  CaloGain::CaloGain estimatedLArGain (const EventContext& ctx,
                                       const CaloCell_ID::SUBCALO iCalo,
                                       const CaloDetDescrElement& caloDDE,
                                       const float energy, 
                                       const Step step) const;


  CaloGain::CaloGain estimatedTileGain (const EventContext& ctx,
                                        const CaloCell& caloCell,
                                        const CaloDetDescrElement& caloDDE,
                                        const Step /*step*/) const;


  float adc2mev (const EventContext& ctx,
                 const CaloDetDescrElement& caloDDE,
                 const CaloGain::CaloGain gain) const;

  SG::ReadCondHandleKey<LArADC2MeV> m_adc2mevKey
    { this, "ADC2MeVKey", "LArADC2MeV", "SG Key of the LArADC2MeV CDO" };


  static const int m_nCalos=4;// number of calorimeters

  float m_LowGainThresh[m_nCalos] = {0};
  float m_HighGainThresh[m_nCalos] = {0};

  const LArEM_ID*    m_lar_em_id = nullptr;
  const TileInfo* m_tileInfo;

  ToolHandle<TileCondIdTransforms> m_tileIdTransforms
    { this, "TileCondIdTransforms", "TileCondIdTransforms",
      "Tile tool to translate hardware identifiers to the drawerIdx, channel, and adc" };

  ToolHandle<TileCondToolEmscale> m_tileToolEmscale
    { this, "TileCondToolEmscale", "TileCondToolEmscale",
      "Tile EM scale calibration tool" };

  ToolHandle<TileCondToolNoiseSample> m_tileToolNoiseSample
    { this, "TileCondToolNoiseSample", "TileCondToolNoiseSample",
      "Tile sample noise tool" };
};


#endif // not CALOTOOLS_CALOESTIMATEDGAINTOOL_H
