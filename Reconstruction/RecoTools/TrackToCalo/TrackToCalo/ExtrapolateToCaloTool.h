/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ***************************************************************************
// ExtrapolateToCaloTool
// ***************************************************************************

#ifndef TRACKTOCALO_EXTRAPOLATETOCALOTOOL_H
#define TRACKTOCALO_EXTRAPOLATETOCALOTOOL_H

#include "RecoToolInterfaces/IExtrapolateToCaloTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkParameters/TrackParameters.h"  // typedef

// xAOD
#include "xAODTracking/NeutralParticle.h" // typedef
#include "xAODTracking/TrackParticle.h" // typedef


class CaloDepthTool;
class ICaloSurfaceBuilder;

class ImpactInCalo;

namespace Trk {
    class Track;
    class IExtrapolator;
    class Surface;
}

//<<<<<< INCLUDES >>>>>>

/** @class ExtrapolateToCaloTool
This class takes a given Trk::Track, extrapolates it to the Calo
    and returns the variables needed to compare with the cluster
    position.
 
    It heavily uses the Tracking Extrapolator class. 
 
    The client is asked to select explicitely the CaloCell_ID::CaloSample where he wants to extrapolate.
    The 3 steps are combined :
       - first guess of eta : take it from last point measured for Trk::Track or the input parameters
       - CreateUserSurface(sample,offset,trketa)
       - extrapolate
       
   Other methods exist which return a vector of Trk::TrackParameters at all calorimeter layers.
   The CaloDepthTool is used to decide which sample is hit at a layer.
 
 
   INTERFACE :
 
   Calorimeter : use CaloCell_ID::CaloSample to choose the sample to extrapolate to
 
   Trk::Trk  is recommended while running the full reconstruction or on ESD. If so, the 
             extrapolation starts from the last measured point.
 
   Trk::TrackParameters  is mandatory while running on AOD's and can be used if you want to start
                   the extrapolation from the perigee.
 
    @author Sebastian.Fleischmann@cern.ch
 */

class ExtrapolateToCaloTool : public AthAlgTool,
            virtual public IExtrapolateToCaloTool
{
public:

    // Constructor:

    ExtrapolateToCaloTool(const std::string& type,
                          const std::string& name,
                          const IInterface* parent);

    ~ExtrapolateToCaloTool();
    StatusCode initialize();
    StatusCode finalize();

    
    /** get Trk::TrackParameters at the calorimeter sample from Trk::Track
    Particle hypothesis of the track is used, if given hypothesis is set to Trk::undefined */
    virtual const Trk::TrackParameters* extrapolate (const Trk::Track& trk,
            const CaloCell_ID::CaloSample sample,
            const double offset,
            const Trk::PropDirection dir,
            const Trk::ParticleHypothesis particle) const;


    /** get Trk::TrackParameters at the calorimeter sample from Trk::TrackParticleBase.
        If the Trk::ParticleHypothesis is undefined the tool tries to extract the hypothesis
        from the given Trk::TrackParticleBase (if this does not succeed Trk::nonInteracting is used) */
    virtual const Trk::TrackParameters* extrapolate (const Trk::TrackParticleBase& trk,
            const CaloCell_ID::CaloSample sample,
            const double offset,
            const Trk::PropDirection dir,
            const Trk::ParticleHypothesis particle) const;

    /** get Trk::TrackParameters at the calorimeter sample from Trk::TrackParameters */
    virtual const Trk::TrackParameters* extrapolate (const Trk::TrackParameters& trkPar,
            const CaloCell_ID::CaloSample sample,
            const double offset,
            Trk::ParticleHypothesis particle,
            const Trk::PropDirection dir) const;

    /** get Trk::NeutralParameters at the calorimeter sample from Trk::NeutralParameters */
    virtual const Trk::NeutralParameters* extrapolate (const Trk::NeutralParameters& trkPar,
            const CaloCell_ID::CaloSample sample,
            const double offset,
            Trk::ParticleHypothesis particle,
            const Trk::PropDirection dir) const;

    /** get Trk::TrackParameters at the calorimeter sample from xAOD::TrackParticle */
    virtual const Trk::TrackParameters* extrapolate (const  xAOD::TrackParticle& trk,
            const CaloCell_ID::CaloSample sample,
            const double offset,
            const Trk::PropDirection dir=Trk::alongMomentum,
            const Trk::ParticleHypothesis particle=Trk::undefined) const;

    /** get Trk::NeutralParameters at the calorimeter sample from xAOD::NeutralParticle */
    virtual const Trk::NeutralParameters* extrapolate (const xAOD::NeutralParticle& trk,
            const CaloCell_ID::CaloSample sample,
            const double offset,
            const Trk::PropDirection dir=Trk::alongMomentum ) const;
            
    /** get vector of Trk::TrackParameters
    at the different calorimeter samples from Trk::TrackParameters */
    virtual const DataVector< const Trk::TrackParameters >* getParametersInCalo (const Trk::TrackParameters& parameters,
            Trk::ParticleHypothesis particle,
            const Trk::PropDirection dir) const;


    /** get vector of Trk::NeutralParameters
    at the different calorimeter samples from Trk::NeutralParameters */
    virtual const DataVector< const Trk::NeutralParameters >* getParametersInCalo (const Trk::NeutralParameters& parameters,
            Trk::ParticleHypothesis particle,
            const Trk::PropDirection dir) const;


    /** get DataVector of Trk::TrackParameters
    at the different calorimeter samples from Trk::TrackParticleBase. The DataVector owns the Trk::TrackParameters,
    but the DataVector itself needs to be deleted by the client.
    If the Trk::ParticleHypothesis is undefined the tool tries to extract the hypothesis
    from the given Trk::TrackParticleBase (if this does not succeed Trk::nonInteracting is used)*/
    virtual const DataVector< const Trk::TrackParameters >* getParametersInCalo (const Trk::TrackParticleBase& trk,
            const Trk::ParticleHypothesis particle,
            const Trk::PropDirection dir) const;

    virtual const DataVector< const Trk::TrackParameters >* getParametersInCalo (const xAOD::TrackParticle& trk,
            const Trk::ParticleHypothesis particle=Trk::undefined,
            const Trk::PropDirection dir=Trk::alongMomentum) const;

    /** get DataVector of Trk::TrackParameters
        at the different calorimeter samples from Trk::Track. The DataVector owns the Trk::TrackParameters,
        but the DataVector itself needs to be deleted by the client.
        Particle hypothesis of the track is used, if given hypothesis is set to Trk::undefined */
    virtual const DataVector< const Trk::TrackParameters >* getParametersInCalo (const Trk::Track& trk,
            const Trk::ParticleHypothesis particle,
            const Trk::PropDirection dir) const;

    /** get ImpactInCalo from Trk::Track
        Particle hypothesis of the track is used, if given hypothesis is set to Trk::undefined */
    virtual ImpactInCalo* getImpactInCalo ( const Trk::Track& trk,
                                            const Trk::ParticleHypothesis particle,
                                            const Trk::PropDirection dir) const;

    /** get ImpactInCalo from Trk::TrackParticleBase
        If the Trk::ParticleHypothesis is undefined the tool tries to extract the hypothesis
        from the given Trk::TrackParticleBase (if this does not succeed Trk::nonInteracting is used)*/
    virtual ImpactInCalo* getImpactInCalo ( const Trk::TrackParticleBase& trk,
                                            const Trk::ParticleHypothesis particle,
                                            const Trk::PropDirection dir) const;

    /** get ImpactInCalo from Trk::TrackParameters*/
    virtual ImpactInCalo* getImpactInCalo (const Trk::TrackParameters& parameters,
            Trk::ParticleHypothesis particleHypo,
            const Trk::PropDirection dir) const;

    /** get ImpactInCalo from Trk::NeutralParameters*/
    virtual ImpactInCalo* getImpactInCalo (const Trk::NeutralParameters& parameters,
            Trk::ParticleHypothesis particleHypo,
            const Trk::PropDirection dir) const;
    
    /** get sum of the momenta at the vertex (designed for conversions) 
        If reuse=true, try to get it from auxdata first  **/
    virtual Amg::Vector3D getMomentumAtVertex(const xAOD::Vertex&, bool reuse=true) const;
    
    /** get straight line intersection in calo given position and momentum vectors **/
    virtual Trk::SurfaceIntersection getIntersectionInCalo(const Amg::Vector3D& position,
            const Amg::Vector3D& momentum, const CaloCell_ID::CaloSample) const;

    /** access to the private tool used to define the extrapolation depth, needed to play with delta-eta */
    virtual ToolHandle<CaloDepthTool> getCaloDepth();

private:

    ImpactInCalo* getImpactInCaloFromParametersInCalo(
                const DataVector< const Trk::TrackParameters >& parametersInCalo) const;

    ImpactInCalo* getImpactInCaloFromParametersInCalo(
                const DataVector< const Trk::NeutralParameters >& parametersInCalo) const;

    /** helper function to take last known parameter in calo and extrapolate it stepwise to next samplings, adds parameters to the DataVector */
    void extrapolateStepwiseInCalo ( const Trk::TrackParameters& parameters,
            Trk::ParticleHypothesis particle,
            const Trk::PropDirection dir,
            DataVector< const Trk::TrackParameters >& parameterVector) const;

    /** helper function to take last known parameter in calo and extrapolate it stepwise to next samplings, adds parameters to the DataVector */
    void extrapolateStepwiseInCalo ( const Trk::NeutralParameters& parameters,
            Trk::ParticleHypothesis particle,
            const Trk::PropDirection dir,
            DataVector< const Trk::NeutralParameters >& parameterVector) const;

    // Defines the surfaces for extrapolation :
    ToolHandle<ICaloSurfaceBuilder> m_calosurf;         //!< tool handle to the calo surface builder
    // gives the distance to calo border to decide if barrel or endcap is hit
    ToolHandle<CaloDepthTool>       m_calodepth;        //!< tool handle to the calo depth tool

    // Pre-configured extrapolator
    ToolHandle< Trk::IExtrapolator > m_extrapolator;    //!< tool handle to the extrapolator

    double                          m_offsetPresampler; //!< jobOption: offset for presampler
    double                          m_offsetStrip;      //!< jobOption: offset for strip
    double                          m_offsetMiddle;     //!< jobOption: offset for em middle
    double                          m_offsetBack;       //!< jobOption: offset for em back
    double                          m_offsetTile;       //!< jobOption: offset for tile/hec
    bool                            m_assumeOrderedTracks;  //!< jobOption: assume ordered tracks
    bool                            m_ignoreMaterialEffectsInsideCalo;  //!< jobOption: ignore material effects inside calo, if step-wise extrapolation to all layers of calo samplings is performed (used for getParametersInCalo() and getImpactInCalo() )

};


#endif // TRACKTOCALO_EXTRAPOLATETOCALOTOOL_H
