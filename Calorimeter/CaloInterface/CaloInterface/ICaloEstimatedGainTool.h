// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file CaloInterface/ICaloEstimatedGain.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2019
 * @brief Estimate gain used to read out a certain energy.
 */


#ifndef CALOINTERFACE_ICALOESTIMATEDGAINTOOL_H
#define CALOINTERFACE_ICALOESTIMATEDGAINTOOL_H


#include "CaloIdentifier/CaloGain.h"
#include "GaudiKernel/IAlgTool.h"


class CaloCell;
class CaloDetDescrElement;
class EventContext;


/**
 * @brief Estimate gain used to read out a certain energy.
 */
class ICaloEstimatedGainTool
  : virtual public IAlgTool
{
public:
  DeclareInterfaceID( ICaloEstimatedGainTool, 1, 0 );


  enum class Step {
    RAWCHANNELS,
    CELLS
  };


  virtual CaloGain::CaloGain estimatedGain (const EventContext& ctx,
                                            const CaloCell& caloCell,
                                            const Step step) const = 0;

  virtual CaloGain::CaloGain estimatedGain (const EventContext& ctx,
                                            const CaloCell& caloCell,
                                            const CaloDetDescrElement& caloDDE,
                                            const Step step) const = 0;

  virtual CaloGain::CaloGain estimatedGain (const EventContext& ctx,
                                            const CaloDetDescrElement& caloDDE,
                                            const float energy,
                                            const Step step) const = 0;
};


#endif // not CALOINTERFACE_ICALOESTIMATEDGAINTOOL_H
