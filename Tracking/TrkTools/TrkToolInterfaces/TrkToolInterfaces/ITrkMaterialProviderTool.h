/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRKMATERIALPROVIDERTOOL_H
#define ITRKMATERIALPROVIDERTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkSurfaces/Surface.h"
#include "TrkParameters/TrackParameters.h" 
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "AthContainers/DataVector.h"
#include "TrkTrack/Track.h"
#include <vector>

static const InterfaceID IID_ITrkMaterialProviderTool("Trk::ITrkMaterialProviderTool", 1, 0);

class CaloEnergy;

namespace Trk {

  class TrackStateOnSurface;

  /** @brief Interface for tool to add alignmenties to a global chi2 fit

  @author Massimiliano.Bellomo@cern.ch */
  class ITrkMaterialProviderTool : virtual public IAlgTool
  {
    public:

      static const InterfaceID& interfaceID( ) ;
      
      /** Update Calorimeter TSOS from input ID and MS tracks*/
      virtual void updateCaloTSOS(const Trk::Track& idTrack, Trk::Track& extrapolatedTrack) const = 0;
      
      /** Update Calorimeter TSOS from input MS tracks*/
      virtual void updateCaloTSOS(Trk::Track& msTrack, const Trk::TrackParameters* startParamaters=nullptr) const = 0;
      
      /** Get Calorimeter MEOT*/
      virtual void getCaloMEOT(const Trk::Track& idTrack, const Trk::Track& msTrack, std::vector<MaterialEffectsOnTrack>& calomeots) const = 0;

      /** Retrieve Calorimeter TSOS from TG and apply corrections*/
      virtual std::vector<const Trk::TrackStateOnSurface*>*
	getCaloTSOS (const Trk::TrackParameters& parm, const Trk::Track& muonTrack, const Trk::TrackParameters* parms=nullptr) const = 0;
                  
      /** Retrieve a clone of the parametrised energy loss*/
      virtual CaloEnergy* getParamCaloELoss(Trk::Track* track) const = 0;      
  };  

}

inline const InterfaceID& Trk::ITrkMaterialProviderTool::interfaceID()
{
  return IID_ITrkMaterialProviderTool;
}

#endif 
