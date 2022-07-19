/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          IEvolvedVertexFinder.h  -  Description
                             -------------------
    begin   : Dec 2019
    authors : Neža Ribarič ( Lancaster university, UK ) 
    email   : neza.ribaric@cern.ch
    
 ***************************************************************************/


#ifndef InDetRecToolInterfaces_IAdaptiveMultiSecVertexFinder_H
#define InDetRecToolInterfaces_IAdaptiveMultiSecVertexFinder_H
#include "GaudiKernel/AlgTool.h"
#include "TrkTrack/TrackCollection.h" // it is a typedef
#include "TrkParticleBase/TrackParticleBaseCollection.h"

//xAOD includes
#include "xAODTracking/VertexFwd.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/VertexContainerFwd.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

namespace InDet
{
static const InterfaceID IID_IAdaptiveMultiSecVertexFinder("IAdaptiveMultiSecVertexFinder", 1, 0);
class IAdaptiveMultiSecVertexFinder : virtual public IAlgTool
  {
public:
    virtual ~IAdaptiveMultiSecVertexFinder() {};
    static const InterfaceID& interfaceID()
    {
        return IID_IAdaptiveMultiSecVertexFinder;
    }
    
    virtual std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> findVertex(const xAOD::TrackParticleContainer* trackParticles) = 0;
    

    virtual void setPrimaryVertexPosition( double, double, double ) = 0 ;

  };
}
#endif
