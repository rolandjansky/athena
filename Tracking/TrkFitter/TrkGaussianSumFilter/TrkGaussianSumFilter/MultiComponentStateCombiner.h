/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*******************************************************************************
      MultiComponentStateCombiner.h  -  description
      -----------------------------------------------
begin                : Monday 20th December 2004
author               : atkinson
email                : Tom.Atkinson@cern.ch
description          : This class takes a multi-component state and collapses
      all components. It returns a single set of track
      parameters with single mean and covariance matrix.
*******************************************************************************/

#ifndef MultiComponentStateCombiner_H
#define MultiComponentStateCombiner_H

#include "TrkGaussianSumFilter/IMultiComponentStateCombiner.h"

#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"
#include "TrkGaussianSumFilter/IMultiComponentStateModeCalculator.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace Trk {

class MultiComponentStateCombiner : public AthAlgTool, virtual public IMultiComponentStateCombiner {

  public:
  
  /** Constructor with AlgTool parameters */
  MultiComponentStateCombiner (const std::string&, const std::string&, const IInterface*);

  /** Virtual destructor */
  virtual ~MultiComponentStateCombiner () {};

  /** AlgTool initialise method */
  StatusCode initialize();

  /** AlgTool finalise method */
  StatusCode finalize();

  /** Calculate combined state of many components */
  virtual const TrackParameters* combine( const MultiComponentState&, bool useModeTemp = false) const;

  /** Calculate combined state and weight of many components */
  virtual const ComponentParameters* combineWithWeight( const MultiComponentState&, bool useModeTemp = false ) const;
  
  /** */
  virtual void useMode( bool );
  

 private:

  const ComponentParameters* compute( const MultiComponentState*, bool useModeTemp = false ) const;
  
  ToolHandle<IMultiComponentStateModeCalculator> m_modeCalculator;
  
  mutable bool  m_useMode;
  mutable bool  m_useModeD0;
  mutable bool  m_useModeZ0;
  mutable bool  m_useModePhi;
  mutable bool  m_useModeTheta;
  mutable bool  m_useModeqOverP;
  
  mutable int   m_NumberOfCalls;
  mutable float m_fractionPDFused;
};

} // end Trk namespace

#endif
