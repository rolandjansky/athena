/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ***************************************************************************
// Interface for ExtrapolateToCaloTool
// ***************************************************************************

#ifndef IEXTRAPOLATETOCALO_H
#define IEXTRAPOLATETOCALO_H



//<<<<<< INCLUDES >>>>>>
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DataModel/DataVector.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkNeutralParameters/NeutralParameters.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "CaloDetDescr/CaloDepthTool.h"

// xAOD
#include "xAODTracking/NeutralParticle.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/VertexFwd.h"

class ImpactInCalo;

namespace Trk {
class Track;
class Surface;
class TrackParticleBase;
}

// namespace xAOD {
//     class TrackParticle;
//     class NeutralParticle;
// }

static const InterfaceID IID_IExtrapolateToCaloTool("IExtrapolateToCaloTool", 1, 0);

/** @class IExtrapolateToCaloTool
  Interface for the Reconstruction/RecoTools/TrackToCalo/ExtrapolateToCaloTool
  This tool can be used to extrapolate Trk::Track, Trk::TrackParameters, Trk::TrackParticleBase.

  @author Sebastian.Fleischmann@cern.ch
*/

class IExtrapolateToCaloTool : virtual public IAlgTool {
public:
    /** enum of the return vector indeces in getParametersInCalo() */
    enum CaloLayers { PreSampler = 0, Strips, Middle, Back, Tile, NCaloLayers};

    /** Virtual destructor */
    virtual ~IExtrapolateToCaloTool(){}

    /** AlgTool interface methods */
    static const InterfaceID& interfaceID(){ return IID_IExtrapolateToCaloTool; };

    /** get Trk::TrackParameters at the calorimeter sample from Trk::Track 
        Particle hypothesis of the track is used, if given hypothesis is set to Trk::undefined */

    virtual const Trk::TrackParameters* extrapolate (const Trk::Track& trk,
            const CaloCell_ID::CaloSample sample,
            const double offset,
            const Trk::PropDirection dir=Trk::alongMomentum,
            const Trk::ParticleHypothesis particle=Trk::undefined) const = 0;


    /** get Trk::TrackParameters at the calorimeter sample from Trk::TrackParticleBase.
        If the Trk::ParticleHypothesis is undefined the tool tries to extract the hypothesis
        from the given Trk::TrackParticleBase (if this does not succeed Trk::nonInteracting is used) */

    virtual const Trk::TrackParameters* extrapolate (const Trk::TrackParticleBase& trk,
            const CaloCell_ID::CaloSample sample,
            const double offset,
            const Trk::PropDirection dir=Trk::alongMomentum,
            const Trk::ParticleHypothesis particle=Trk::undefined) const = 0;

    /** get Trk::TrackParameters at the calorimeter sample from Trk::TrackParameters */
    virtual const Trk::TrackParameters* extrapolate (const Trk::TrackParameters& trkPar,
            const CaloCell_ID::CaloSample sample,
            const double offset,
            Trk::ParticleHypothesis particle,
            const Trk::PropDirection dir=Trk::alongMomentum) const = 0;

    /** get Trk::NeutralParameters at the calorimeter sample from Trk::NeutralParameters */
    virtual const Trk::NeutralParameters* extrapolate (const Trk::NeutralParameters& trkPar,
            const CaloCell_ID::CaloSample sample,
            const double offset,
            Trk::ParticleHypothesis particle,
            const Trk::PropDirection dir=Trk::alongMomentum) const = 0;

    /** get Trk::TrackParameters at the calorimeter sample from xAOD::TrackParticle */
    virtual const Trk::TrackParameters* extrapolate (const  xAOD::TrackParticle& trk,
            const CaloCell_ID::CaloSample sample,
            const double offset,
            const Trk::PropDirection dir=Trk::alongMomentum,
            const Trk::ParticleHypothesis particle=Trk::undefined) const = 0;

    /** get Trk::NeutralParameters at the calorimeter sample from xAOD::NeutralParticle */
    virtual const Trk::NeutralParameters* extrapolate (const xAOD::NeutralParticle& trk,
            const CaloCell_ID::CaloSample sample,
            const double offset,
            const Trk::PropDirection dir=Trk::alongMomentum ) const = 0;

    /** get vector of Trk::TrackParameters 
     at the different calorimeter samples from Trk::TrackParameters */

    virtual const DataVector< const Trk::TrackParameters >* getParametersInCalo (const Trk::TrackParameters& parameters,
            Trk::ParticleHypothesis particle,
            const Trk::PropDirection dir=Trk::alongMomentum) const = 0;


    /** get vector of Trk::NeutralParameters 
     at the different calorimeter samples from Trk::NeutralParameters */
    virtual const DataVector< const Trk::NeutralParameters >* getParametersInCalo (const Trk::NeutralParameters& parameters,
            Trk::ParticleHypothesis particle,
            const Trk::PropDirection dir=Trk::alongMomentum) const = 0;


    /** get DataVector of Trk::TrackParameters 
     at the different calorimeter samples from Trk::TrackParticleBase. The DataVector owns the Trk::TrackParameters,
     but the DataVector itself needs to be deleted by the client.
     If the Trk::ParticleHypothesis is undefined the tool tries to extract the hypothesis
     from the given Trk::TrackParticleBase (if this does not succeed Trk::nonInteracting is used)*/

    virtual const DataVector< const Trk::TrackParameters >* getParametersInCalo (const Trk::TrackParticleBase& trk,
            const Trk::ParticleHypothesis particle=Trk::undefined,
            const Trk::PropDirection dir=Trk::alongMomentum) const = 0;

    /** get DataVector of Trk::TrackParameters 
     at the different calorimeter samples from xAOD::TrackParticle. The DataVector owns the Trk::TrackParameters,
     but the DataVector itself needs to be deleted by the client.
     If the Trk::ParticleHypothesis is undefined the tool takes the hypothesis
     of the given TrackParticle*/

    virtual const DataVector< const Trk::TrackParameters >* getParametersInCalo (const xAOD::TrackParticle& trk,
            const Trk::ParticleHypothesis particle=Trk::undefined,
            const Trk::PropDirection dir=Trk::alongMomentum) const = 0;

    /** get DataVector of Trk::TrackParameters 
        at the different calorimeter samples from Trk::Track. The DataVector owns the Trk::TrackParameters,
        but the DataVector itself needs to be deleted by the client.
        Particle hypothesis of the track is used, if given hypothesis is set to Trk::undefined */

    virtual const DataVector< const Trk::TrackParameters >* getParametersInCalo (const Trk::Track& trk,
            const Trk::ParticleHypothesis particle=Trk::undefined,
            const Trk::PropDirection dir=Trk::alongMomentum) const = 0;

    /** get ImpactInCalo from Trk::Track
        Particle hypothesis of the track is used, if given hypothesis is set to Trk::undefined */

    virtual ImpactInCalo* getImpactInCalo ( const Trk::Track& trk,
                                            const Trk::ParticleHypothesis particle=Trk::undefined,
                                            const Trk::PropDirection dir=Trk::alongMomentum) const = 0;

    /** get ImpactInCalo from Trk::TrackParticleBase 
        If the Trk::ParticleHypothesis is undefined the tool tries to extract the hypothesis
        from the given Trk::TrackParticleBase (if this does not succeed Trk::nonInteracting is used)*/

    virtual ImpactInCalo* getImpactInCalo ( const Trk::TrackParticleBase& trk,
                                            const Trk::ParticleHypothesis particle=Trk::undefined,
                                            const Trk::PropDirection dir=Trk::alongMomentum) const = 0;

    /** get ImpactInCalo from Trk::TrackParameters (Trk::TrackParameters or Trk::NeutralParameters)*/

    virtual ImpactInCalo* getImpactInCalo (const Trk::TrackParameters& parameters,
            Trk::ParticleHypothesis particleHypo,
            const Trk::PropDirection dir=Trk::alongMomentum) const = 0;

    virtual ImpactInCalo* getImpactInCalo (const Trk::NeutralParameters& parameters,
            Trk::ParticleHypothesis particleHypo,
            const Trk::PropDirection dir=Trk::alongMomentum) const = 0;
    
    /** get sum of the momenta at the vertex (designed for conversions).
        If reuse=true, try to get it from auxdata first  **/
    virtual Amg::Vector3D getMomentumAtVertex(const xAOD::Vertex&, bool reuse=true) const = 0;
    
    /** get straight line intersection in calo sample from position and momentum vectors **/
    virtual Trk::SurfaceIntersection getIntersectionInCalo(const Amg::Vector3D& position,
            const Amg::Vector3D& momentum, const CaloCell_ID::CaloSample) const = 0;


    /** access to the private tool used to define the extrapolation depth, needed to play with delta-eta */
    virtual ToolHandle<CaloDepthTool> getCaloDepth()=0;
};



#endif // IEXTRAPOLATETOCALO_H
