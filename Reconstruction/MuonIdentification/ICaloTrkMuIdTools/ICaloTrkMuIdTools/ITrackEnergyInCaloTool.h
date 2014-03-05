/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTRKMUIDTOOLS_ITRACKENERGYINCALOTOOL_H
#define CALOTRKMUIDTOOLS_ITRACKENERGYINCALOTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
//Calo
#include "CaloIdentifier/CaloCell_ID.h"
//Trk
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"
#include "TrkSurfaces/Surface.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
// STL
#include <vector>
#include <utility>

typedef  std::pair < const Trk::TrackParameters*, CaloCell_ID::CaloSample > CaloPair; 

static const InterfaceID IID_ITrackEnergyInCaloTool("ITrackEnergyInCaloTool", 1, 0);

class ITrackEnergyInCaloTool: virtual public IAlgTool{
  /** @class ITrackEnergyInCaloTool
  
  @author  Gustavo.Ordonez.Sanz <Gustavo.Ordonez.Sanz@cern.ch>
  */  
 public:
	 
  virtual ~ITrackEnergyInCaloTool(){}

  static const InterfaceID& interfaceID(){return IID_ITrackEnergyInCaloTool;}

  virtual const Trk::TrackParameters* paramInSample(const Trk::Track *trk,
						     const double offset,  
						     CaloCell_ID::CaloSample sample, 
						     const bool checkBoundary, 
						     Trk::ParticleHypothesis particleHypo) const=0;
  
  virtual const Trk::TrackParameters* paramInSample(const Trk::TrackParameters *par,
						     const double offset,  
						     CaloCell_ID::CaloSample sample,
						     const bool checkBoundary, 
						     Trk::ParticleHypothesis particleHypo) const=0;

  virtual const Trk::TrackParameters* paramInNextSample(const Trk::Track *trk,
						     const double offset,  
						     CaloCell_ID::CaloSample sample, 
						     const bool checkBoundary, 
						     Trk::ParticleHypothesis particleHypo) const=0;
  
  
  
  virtual const Trk::TrackParameters* paramInNextSample(const Trk::TrackParameters *par,
						     const double offset,  
						     CaloCell_ID::CaloSample sample,
						     const bool checkBoundary, 
						     Trk::ParticleHypothesis particleHypo) const=0;  
  

  virtual const Trk::TrackParameters* paramInLastSample(const Trk::Track *trk, 
							 const double offset, 
							 CaloCell_ID::CaloSample sample, 
							 Trk::ParticleHypothesis particleHypo) const=0;
  
  virtual const Trk::TrackParameters* paramInLastSample(const Trk::TrackParameters *par, 
							 const double offset, 
							 CaloCell_ID::CaloSample sample,
							 Trk::ParticleHypothesis particleHypo) const=0;
  
  virtual const Trk::TrackParameters* paramAfterGirder(const Trk::Track *trk,
							Trk::ParticleHypothesis particleHypo) const=0;  
  
  virtual const Trk::TrackParameters* paramAfterGirder(const Trk::TrackParameters *par,
							Trk::ParticleHypothesis particleHypo) const=0; 
  
  
  virtual std::vector < CaloPair > paramInAllSamples(const Trk::Track *trk, 
							   const double offset,
							   const bool checkBoundary, 
							   Trk::ParticleHypothesis particleHypo) const=0;
  
  
  virtual std::vector < CaloPair > paramInAllSamples(const Trk::TrackParameters *par, 
							   const double offset,
							   const bool checkBoundary, 
							   Trk::ParticleHypothesis particleHypo) const=0;
  

  virtual double etamax(const CaloCell_ID::CaloSample sample, 
			const bool isTile) const = 0;
  virtual double etamin(const CaloCell_ID::CaloSample sample, 
			const bool isTile) const = 0;
  virtual double energy(const Trk::TrackParameters *par, 
			      Trk::ParticleHypothesis particleHypo) const = 0;

  virtual CaloCell_ID::CaloSample getNextSample( const CaloCell_ID::CaloSample sample, double eta) const =0;

  
};



 
#endif
