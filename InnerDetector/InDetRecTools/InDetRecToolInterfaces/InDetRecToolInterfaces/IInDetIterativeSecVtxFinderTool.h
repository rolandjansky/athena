/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          ISecVertexFinder.h  -  Description
                             -------------------
    begin   : Nov 16, 2016
    authors : Lianyou SHAN ( IHEP-Beijing ) 
    email   : shanly@ihep.ac.cn
    changes : add the setPriVtxPosition to InDetRecToolInterfaces/IVertexFinder 
              by Andreas Wildauer et.al
 ***************************************************************************/


#ifndef InDetRecToolInterfaces_ISecVertexFinder_H
#define InDetRecToolInterfaces_ISecVertexFinder_H
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
static const InterfaceID IID_ISecVertexFinder("ISecVertexFinder", 1, 0);
class ISecVertexFinder : virtual public IAlgTool
  {
public:
    virtual ~ISecVertexFinder() {};
    static const InterfaceID& interfaceID()
    {
        return IID_ISecVertexFinder;
    }
    
    virtual std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> 
                    findVertex(const TrackCollection* trackTES)=0;
 
    virtual std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> 
                    findVertex(const Trk::TrackParticleBaseCollection* trackTES)=0;

    /** Find vertex from xAOD::TrackParticleContainer.
     * @param trackParticles input track container
     * @return a pair of newly created container and auxiliary store
     */
    virtual std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> findVertex(const xAOD::TrackParticleContainer* trackParticles) = 0;
    virtual std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> findVertex( const std::vector<const xAOD::IParticle*> & inputTracks ) = 0 ;

    virtual void setPriVtxPosition( double, double, double ) = 0 ;

  };
}
#endif
