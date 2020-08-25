/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTRKMUIDTOOLS_TRACKENERGYINCALOTOOL_H
#define CALOTRKMUIDTOOLS_TRACKENERGYINCALOTOOL_H

// --- Interface ---
#include "ICaloTrkMuIdTools/ITrackEnergyInCaloTool.h"

// --- Gaudi includes ---
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// --- Tool interfaces ---
#include "TrkExInterfaces/IExtrapolator.h"
#include "CaloTrackingGeometry/ICaloSurfaceBuilder.h"

class CaloDetDescrManager;


class TrackEnergyInCaloTool: public AthAlgTool,  virtual public ITrackEnergyInCaloTool{
  /** @class TrackEnergyInCaloTool
      
  will take a track and will extrapolated it to a surface in the Calorimeter created by CaloSurfaceBuilder
  
  @author  Gustavo.Ordonez.Sanz <Gustavo.Ordonez.Sanz@cern.ch>
  */  
 public:
  TrackEnergyInCaloTool(const std::string& type, const std::string& name, const IInterface* pInterface);
  /** default destructor */
  virtual ~TrackEnergyInCaloTool()=default;

  virtual StatusCode initialize();
   /** 
       Extrapolate to a given sample using track as input
   */
  const Trk::TrackParameters* paramInSample(const Trk::Track *trk,
					     const double offset,  
					     CaloCell_ID::CaloSample sample, 
					     const bool checkBoundary, 
					     Trk::ParticleHypothesis particleHypo) const;
  
   /** 
       Extrapolate to a given sample using trackParameters as input
   */
  const Trk::TrackParameters* paramInSample(const Trk::TrackParameters *par,
					     const double offset,  
					     CaloCell_ID::CaloSample sample,
					     const bool checkBoundary, 
					     Trk::ParticleHypothesis particleHypo) const;

   /** 
       Extrapolate to a next sample using track as input
   */
  const Trk::TrackParameters* paramInNextSample(const Trk::Track *trk,
					     const double offset,  
					     CaloCell_ID::CaloSample sample, 
					     const bool checkBoundary, 
					     Trk::ParticleHypothesis particleHypo) const;
  
  
  
   /** 
       Extrapolate to a next  sample using trackParameters as input
   */
  const Trk::TrackParameters* paramInNextSample(const Trk::TrackParameters *par,
					     const double offset,  
					     CaloCell_ID::CaloSample sample,
					     const bool checkBoundary, 
					     Trk::ParticleHypothesis particleHypo) const;

   /** 
       Extrapolate to a last surface in calo using track as input
   */
  const Trk::TrackParameters* paramInLastSample(const Trk::Track *trk, 
						 const double offset, 
						 CaloCell_ID::CaloSample sample,
						 Trk::ParticleHypothesis particleHypo) const;
   /** 
       Extrapolate to a last surface in calo using trackParameters as input
   */
  const Trk::TrackParameters* paramInLastSample(const Trk::TrackParameters *par, 
						 const double offset, 
						 CaloCell_ID::CaloSample sample, 
						 Trk::ParticleHypothesis particleHypo) const;
  /** 
      Extrapolate to the surface after the Girder surface in calo using track as input
  */
  const Trk::TrackParameters* paramAfterGirder(const Trk::Track *trk,
						Trk::ParticleHypothesis particleHypo) const; 
  /** 
       Extrapolate to the surface after the Girder surface in calo using trackParameters as input
   */
  const Trk::TrackParameters* paramAfterGirder(const Trk::TrackParameters *par,
						Trk::ParticleHypothesis particleHypo) const; 
  /** 
      Extrapolate to the all surface and fills CaloPair track as input
  */
  std::vector < CaloPair > paramInAllSamples(const Trk::Track *trk, 
						   const double offset,
						   const bool checkBoundary, 
						   Trk::ParticleHypothesis particleHypo) const;
  /** 
      Extrapolate to the all surface and fills CaloPair trackParameters as input
  */
  std::vector < CaloPair > paramInAllSamples(const Trk::TrackParameters *par, 
						   const double offset,
						   const bool checkBoundary, 
						   Trk::ParticleHypothesis particleHypo) const;
  

  /** 
      get max eta of the sample to extrapolate to
  */
  double etamax(const CaloCell_ID::CaloSample sample, 
		const bool isTile) const;
  /** 
      get min eta of the sample to extrapolate to
  */
  double etamin(const CaloCell_ID::CaloSample sample, 
		      const bool isTile) const;
  /** 
      Compute parametrised energy Loss
  */
  double energy(const Trk::TrackParameters *par, 
		      Trk::ParticleHypothesis particleHypo) const;
  /** 
    Get next sample in eta
  */
  CaloCell_ID::CaloSample getNextSample( const CaloCell_ID::CaloSample sample, double eta) const;

 private:
  CaloCell_ID::CaloSample getSample( double eta, const int layer) const;

  const CaloDetDescrManager*         m_calo_dd{};       

  ToolHandle<Trk::IExtrapolator> m_extrapolator{this, "ExtrapolatorHandle", "Trk::Extrapolator/AtlasExtrapolator", "Pre-configured extrapolator: will be choosen via jobOpt"};
  ToolHandle<ICaloSurfaceBuilder> m_calosurf{this, "CaloSurfaceBuilder", "CaloSurfaceBuilder", "Defines the surfaces for extrapolation"};

  const Trk::ParticleMasses m_particlemasses;
};



 
#endif
