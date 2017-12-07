/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
      GsfMeasurementUpdator.h  -  description
      ---------------------------------------
begin                : Friday 25th February 2005
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : Class for performing updates on multi-component states for
                       the gaussian-sum filter. Now an AlgTool
*********************************************************************************/

#ifndef TrkGsfMeasurementUpdator_H
#define TrkGsfMeasurementUpdator_H

#include "TrkGaussianSumFilter/IMultiStateMeasurementUpdator.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"

#include "TrkToolInterfaces/IUpdator.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IChronoStatSvc.h"

namespace Trk{

class IPosteriorWeightsCalculator;
class IMultiComponentStateAssembler;
class LocalParameters;

class GsfMeasurementUpdator : public AthAlgTool, virtual public IMultiStateMeasurementUpdator {

 private:
  /** Private typedef for calling the correct updator member function depending of direction of fitting */
  typedef const Trk::TrackParameters* (Trk::IUpdator::*Updator)( const Trk::TrackParameters&, const LocalParameters&, const Amg::MatrixX&, FitQualityOnSurface*& ) const;

 public:
  /** Constructor with parameters to be passed to AlgTool */
  GsfMeasurementUpdator(const std::string, const std::string, const IInterface*);

  /** Virtual destructor */
  virtual ~GsfMeasurementUpdator() {};

  /** AlgTool initialise method */
  StatusCode initialize();

  /** AlgTool finalise method */
  StatusCode finalize();

  /** Method for updating the multi-state with a new measurement */
  virtual const MultiComponentState* update (const MultiComponentState&, const MeasurementBase&) const;
  
  /** Method for updating the multi-state with a new measurement and calculate the fit qaulity at the same time*/
  virtual const MultiComponentState* update (const Trk::MultiComponentState&, 
                                             const Trk::MeasurementBase&,
                                             std::unique_ptr<FitQualityOnSurface>& fitQoS ) const;


  /** Method for GSF smoother to calculate unbiased parameters of the multi-component state */
  virtual const MultiComponentState* getUnbiasedTrackParameters (const MultiComponentState&, const MeasurementBase&) const;

  /** Method for determining the chi2 of the multi-component state and the number of degrees of freedom */
  virtual const FitQualityOnSurface* fitQuality (const MultiComponentState&, const MeasurementBase&) const;

 private:
  const MultiComponentState* calculateFilterStep( const MultiComponentState&, 
                                                  const MeasurementBase&, 
                                                  const Updator) const;
                                                  
  const MultiComponentState* calculateFilterStep( const MultiComponentState&, 
                                                  const MeasurementBase&, 
                                                  std::unique_ptr<FitQualityOnSurface>& fitQoS ) const;
                                                  
  bool invalidComponent(const Trk::TrackParameters* trackParameters ) const;
  
  Trk::MultiComponentState*  rebuildState(const Trk::MultiComponentState& stateBeforeUpdate) const;
                                                  

 private:
  int                                      m_outputlevel;                      //!< to cache current output level
  ToolHandle<IUpdator>                     m_updator;                          //!< Linear (Kalman) updator
  ToolHandle<IPosteriorWeightsCalculator>  m_posteriorWeightsCalculator;       //!< GSF Weights updator

  ToolHandle<IMultiComponentStateAssembler> m_stateAssembler;                   //!< State assembler

  ServiceHandle<IChronoStatSvc>            m_chronoSvc;                        //!< Timing: The Gaudi time auditing service

};

}

#endif
