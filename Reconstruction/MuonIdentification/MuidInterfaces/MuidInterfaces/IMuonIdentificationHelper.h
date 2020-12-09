/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//
//
//   tools used during combined Track, Particle and Muon creation
//
///////////////////////////////////////////////////////////////////

#ifndef MUIDINTERFACES_IMUONIDENTIFICATIONHELPER_H
#define MUIDINTERFACES_IMUONIDENTIFICATIONHELPER_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkEventPrimitives/VertexType.h"

class CaloEnergy;
class MsgStream;
namespace Analysis
{
    class Muon;
    class MuonContainer;
}
namespace Rec
{
    class TrackParticle;
    class TrackParticleContainer;
}
namespace Trk
{
    class Track;
    class VxCandidate;
}

namespace Rec
{
  
/** Interface ID for IMuonIdentificationHelper*/  
static const InterfaceID IID_IMuonIdentificationHelper("IMuonIdentificationHelper", 1, 0);
  
/**@class IMuonIdentificationHelper

Base class for MuonIdentificationHelper AlgTool
     
     
@author Alan.Poppleton@cern.ch
*/
class IMuonIdentificationHelper : virtual public IAlgTool
{
public:

    /**Virtual destructor*/
    virtual ~IMuonIdentificationHelper() {}
       
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID&	interfaceID() { return IID_IMuonIdentificationHelper; }
 
    /**IMuonIdentificationHelper interface:
       make a muon TrackParticle */
    virtual TrackParticle*	createParticle(const Trk::Track*,
					       const Trk::VxCandidate*	= 0,
					       Trk::VertexType		= Trk::NoVtx) = 0;
   
    /**IMuonIdentificationHelper interface:
       replace track by its copy with CaloEnergy cast to its base class,
       a clone of the full CaloEnergy object is returned */
    virtual const CaloEnergy*	downcastCaloEnergy(Trk::Track*& track) = 0;
    
    /**IMuonIdentificationHelper interface:
       dress a Muon */
    virtual void		dressMuon(Analysis::Muon*	muon,
					  const CaloEnergy*	caloEnergy = 0) = 0;

    /**IMuonIdentificationHelper interface:
       is the calo cell map available */
    virtual bool	       	haveCalorimeter(void) = 0;

    /**IMuonIdentificationHelper interface:
       print out information on the passed muon collection. Used for debugging.*/
    virtual void		summariseMuons(MsgStream& log,
					       const Analysis::MuonContainer& muons) = 0;
    
    /**IMuonIdentificationHelper interface:
       print out information on the passed track collection. Used for debugging.*/
    virtual void		summariseParticles(MsgStream& log,
						   const TrackParticleContainer& particles) = 0;
};

}	// end of namespace

#endif // MUIDINTERFACES_IMUONIDENTIFICATIONHELPER_H


