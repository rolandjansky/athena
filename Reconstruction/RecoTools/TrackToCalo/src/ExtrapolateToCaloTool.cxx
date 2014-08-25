/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ***************************************************************************
// ExtrapolateToCaloTool
//****************************************************************************

#include "TrackToCalo/ExtrapolateToCaloTool.h"
#include "TrackToCalo/ImpactInCalo.h"

#include "GaudiKernel/MsgStream.h"
/*#include "GaudiKernel/Property.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IMessageSvc.h"
#include "StoreGate/StoreGateSvc.h"*/
#include "GaudiKernel/ISvcLocator.h"
#include <vector>

// Stuff needed for the extrapolation :
//#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkParameters/TrackParameters.h"
// #include "TrkNeutralParameters/NeutralParameters.h"
// #include "TrkNeutralParameters/NeutralPerigee.h"
#include "TrkTrack/Track.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkExInterfaces/IExtrapolator.h"
// CLHEP
// #include "CLHEP/Units/SystemOfUnits.h"
// #include "CLHEP/Vector/Rotation.h"
// #include "CLHEP/Vector/ThreeVector.h"

// Calo specific stuff :
/*#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/ICaloCoordinateTool.h"*/
#include "CaloTrackingGeometry/ICaloSurfaceBuilder.h"
#include "CaloDetDescr/CaloDepthTool.h"

#include "xAODTracking/Vertex.h"
//#include <cmath>


ExtrapolateToCaloTool::ExtrapolateToCaloTool(const std::string& type,
        const std::string& name,
        const IInterface* parent) :
        AthAlgTool(type, name, parent),
        m_calosurf("CaloSurfaceBuilder"),
        m_calodepth("CaloDepthTool"),
        m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
        m_offsetPresampler(0.),
        m_offsetStrip(0.),
        m_offsetMiddle(0.),
        m_offsetBack(0.),
        m_offsetTile(0.),
        m_assumeOrderedTracks(true),
        m_ignoreMaterialEffectsInsideCalo(true) {
    declareInterface<IExtrapolateToCaloTool>( this );

    declareProperty ("Extrapolator",        m_extrapolator, "Extrapolation tool");
    declareProperty ("CaloSurfaceBuilder",  m_calosurf,     "Calo Surface builder tool");
    declareProperty ("CaloDepthTool",       m_calodepth,    "Calo Depth tool");
    declareProperty ("OffsetPresampler",    m_offsetPresampler, "extrapolation depth (offset) for presampler");
    declareProperty ("OffsetStrip",         m_offsetStrip,      "extrapolation depth (offset) for strip");
    declareProperty ("OffsetMiddle",        m_offsetMiddle,     "extrapolation depth (offset) for em middle");
    declareProperty ("OffsetBack",          m_offsetBack,       "extrapolation depth (offset) for em back");
    declareProperty ("OffsetTile",          m_offsetTile,       "extrapolation depth (offset) for em tile");
    declareProperty ("OrderedTracks",       m_assumeOrderedTracks, "assume ordered tracks");
    declareProperty ("IgnoreMaterialEffectsInsideCalo",       m_ignoreMaterialEffectsInsideCalo, "ignore material effects inside calo, if step-wise extrapolation to all layers of calo samplings is performed (used for getParametersInCalo() and getImpactInCalo() )");
}

ExtrapolateToCaloTool::~ExtrapolateToCaloTool() {}

StatusCode ExtrapolateToCaloTool::initialize() {
    ATH_MSG_INFO("ExtrapolateToCaloTool initialize()" );

    // Get Extrapolator from ToolService
    if (m_extrapolator.retrieve().isFailure()) {
        ATH_MSG_FATAL( "Could not retrieve " << m_extrapolator );
        return StatusCode::FAILURE;
    }
    // Get CaloSurfaceBuilder from ToolService
    if (m_calosurf.retrieve().isFailure()) {
        ATH_MSG_FATAL( "Could not retrieve " << m_calosurf );
        return StatusCode::FAILURE;
    }
    // Get CaloDepthTool from ToolService
    if (m_calodepth.retrieve().isFailure()) {
        ATH_MSG_FATAL( "Could not retrieve " << m_calodepth );
        return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
}

StatusCode ExtrapolateToCaloTool::finalize() {
    return StatusCode::SUCCESS;
}

const Trk::TrackParameters* ExtrapolateToCaloTool::extrapolate (const Trk::Track& trk,
        const CaloCell_ID::CaloSample sample,
        const double offset,
        const Trk::PropDirection dir,
        const Trk::ParticleHypothesis particle) const {

    const Trk::TrackParameters* parametersAtCalo = 0;
    // get the DataVector of TrackParameters
    const DataVector<const Trk::TrackParameters>* parameterVector = trk.trackParameters();
    if (parameterVector) {
        // use last track parameters on track
        DataVector<const Trk::TrackParameters>::const_iterator curParIter = parameterVector->end();
        --curParIter;
        const Trk::TrackParameters* startParameters = (*curParIter);
        // get the destination Surface
        const double etaDir = (dir==Trk::oppositeMomentum ? -1.*(startParameters->eta()) : startParameters->eta());
        const Trk::Surface* destinationSurface = m_calosurf->CreateUserSurface (sample, offset, etaDir);
        if (!destinationSurface)
            return 0;
        // use particle hypothesis of the track, if given hypothesis is set to undefined
        Trk::ParticleHypothesis particleHypo = (particle == Trk::undefined ? trk.info().particleHypothesis() : particle);
        // extrapolate to calorimeter surface
        if (m_assumeOrderedTracks)
            parametersAtCalo = m_extrapolator->extrapolate(*startParameters,
                               *destinationSurface,
                               dir,
                               false,
                               particleHypo);
        else
            parametersAtCalo = m_extrapolator->extrapolate(trk,
                               *destinationSurface,
                               dir,
                               false,
                               particleHypo);
        delete destinationSurface;
        destinationSurface = 0;
    }
    return parametersAtCalo;
}

const Trk::TrackParameters* ExtrapolateToCaloTool::extrapolate (const Trk::TrackParticleBase& trk,
            const CaloCell_ID::CaloSample sample,
            const double offset,
            const Trk::PropDirection dir,
            const Trk::ParticleHypothesis particle) const {

    // if the TrackParticle contains the pointer to the original track, use this one:
    if (trk.originalTrack()) {
        return extrapolate(*(trk.originalTrack()), sample, offset, dir, particle);
    }

    // get the destination Surface
    const double etaDir = (dir==Trk::oppositeMomentum ? -1.*(trk.definingParameters().momentum().eta()) : trk.definingParameters().momentum().eta());
    const Trk::Surface* destinationSurface = m_calosurf->CreateUserSurface (sample, offset, etaDir);
    if (!destinationSurface) return 0;
    // Without the original track we cannot get the particle hypothesis used during the track fit
    // TODO: (will be added to TrackParticle at some point)!
    // use nonInteracting, if no hypothesis given:
    Trk::ParticleHypothesis hypo = (particle == Trk::undefined ? Trk::nonInteracting : particle);

    const Trk::TrackParameters* parametersAtCalo = m_extrapolator->extrapolate(trk.definingParameters(),
                                                          *destinationSurface,
                                                          dir,
                                                          false,
                                                          hypo);
    delete destinationSurface;
    return parametersAtCalo;
}


const Trk::TrackParameters* ExtrapolateToCaloTool::extrapolate (const Trk::TrackParameters& trkPar,
        const CaloCell_ID::CaloSample sample,
        const double offset, Trk::ParticleHypothesis particle,
        const Trk::PropDirection dir) const {
    const Trk::TrackParameters* parametersAtCalo = 0;
    // get the destination Surface
    const double etaDir = (dir==Trk::oppositeMomentum ? -1.*(trkPar.eta()) : trkPar.eta());
    const Trk::Surface* destinationSurface = m_calosurf->CreateUserSurface (sample, offset, etaDir);
    if (!destinationSurface)
        return 0;
    // extrapolate to calorimeter surface
    parametersAtCalo = m_extrapolator->extrapolate(trkPar,
                       *destinationSurface,
                       dir,
                       false,
                       particle);
    delete destinationSurface;
    destinationSurface = 0;
    return parametersAtCalo;
}
            
const Trk::NeutralParameters* ExtrapolateToCaloTool::extrapolate (const Trk::NeutralParameters& parameters,
            const CaloCell_ID::CaloSample sample,
            const double offset,
            Trk::ParticleHypothesis, // TODO: drop!
            const Trk::PropDirection dir) const {
    const Trk::NeutralParameters* parametersAtCalo = 0;
    // get the destination Surface
    const double etaDir = (dir==Trk::oppositeMomentum ? -1.*(parameters.momentum().eta()) : parameters.momentum().eta());
    const Trk::Surface* destinationSurface = m_calosurf->CreateUserSurface (sample, offset, etaDir);
    if (!destinationSurface) return 0;
    // extrapolate to calorimeter surface
    parametersAtCalo = m_extrapolator->extrapolate(parameters,
                       *destinationSurface,
                       dir,
                       false);
    delete destinationSurface;
    destinationSurface = 0;
    return parametersAtCalo;
}

const Trk::TrackParameters* ExtrapolateToCaloTool::extrapolate (const  xAOD::TrackParticle& trk,
            const CaloCell_ID::CaloSample sample,
            const double offset,
            const Trk::PropDirection dir,
            const Trk::ParticleHypothesis particle) const {
    const Trk::TrackParameters* parametersAtCalo = 0;
    // get the destination Surface
    const double etaDir = (dir==Trk::oppositeMomentum ? -1.*(trk.eta()) : trk.eta());
    const Trk::Surface* destinationSurface = m_calosurf->CreateUserSurface (sample, offset, etaDir);
    if (!destinationSurface)
        return 0;

    Trk::ParticleHypothesis hypo = (particle == Trk::undefined ? Trk::nonInteracting : particle);

    // extrapolate to calorimeter surface
    parametersAtCalo = m_extrapolator->extrapolate(trk,
                       *destinationSurface,
                       dir,
                       false,
                       hypo);
    delete destinationSurface;
    destinationSurface = 0;
    return parametersAtCalo;
}

const Trk::NeutralParameters* ExtrapolateToCaloTool::extrapolate (const xAOD::NeutralParticle& trk,
            const CaloCell_ID::CaloSample sample,
            const double offset,
            const Trk::PropDirection dir ) const {
    const Trk::NeutralParameters* parametersAtCalo = 0;
    // get the destination Surface
    const double etaDir = (dir==Trk::oppositeMomentum ? -1.*(trk.eta()) : trk.eta());
    const Trk::Surface* destinationSurface = m_calosurf->CreateUserSurface (sample, offset, etaDir);
    if (!destinationSurface) return 0;
    // extrapolate to calorimeter surface
    parametersAtCalo = m_extrapolator->extrapolate(trk,
                       *destinationSurface,
                       dir,
                       false);
    delete destinationSurface;
    destinationSurface = 0;
    return parametersAtCalo;
}

const DataVector< const Trk::TrackParameters >* ExtrapolateToCaloTool::getParametersInCalo (
            const Trk::TrackParameters& parameters,
            Trk::ParticleHypothesis particle,
            const Trk::PropDirection dir) const {

    // the return vector
    DataVector< const Trk::TrackParameters >* parameterVector = new DataVector< const Trk::TrackParameters >(IExtrapolateToCaloTool::NCaloLayers, SG::OWN_ELEMENTS);
    // variables to select sampler (barrel or endcap)
    double distbar = 0.;    // distance to barrel
    double distec = 0.;     // distance to end caps
    CaloCell_ID::CaloSample sample;
    // get eta direction of particle:
    double trketa = parameters.momentum().eta();

    // PreSampler :
    // figure out which sampling will be hit
    distbar = m_calodepth->deta(CaloCell_ID::PreSamplerB,trketa);
    distec = m_calodepth->deta(CaloCell_ID::PreSamplerE,trketa);
    
    ATH_MSG_DEBUG(" TrackTo ...PS : for eta= " << trketa << " dist to Barrel =" << distbar 
        << " to endcap =" << distec);
    if (distbar < 0 )  sample = CaloCell_ID::PreSamplerB;
    else if (distec < 0 ) sample = CaloCell_ID::PreSamplerE;
    else if ( distbar < distec) sample = CaloCell_ID::PreSamplerB;
    else sample = CaloCell_ID::PreSamplerE;
    ATH_MSG_DEBUG( "  => will shoot in sample " << sample );

    (*parameterVector)[IExtrapolateToCaloTool::PreSampler] = extrapolate(parameters, sample, m_offsetPresampler, particle, dir);

    // do rest of the extrapolation step-wise:
    extrapolateStepwiseInCalo ( parameters,
            particle,
            dir,
            *parameterVector);

    return parameterVector;
}

const DataVector< const Trk::NeutralParameters >* ExtrapolateToCaloTool::getParametersInCalo (
            const Trk::NeutralParameters& parameters,
            Trk::ParticleHypothesis particle,
            const Trk::PropDirection dir) const {

    // the return vector
    DataVector< const Trk::NeutralParameters >* parameterVector = new DataVector< const Trk::NeutralParameters >(IExtrapolateToCaloTool::NCaloLayers, SG::OWN_ELEMENTS);
    // variables to select sampler (barrel or endcap)
    double distbar = 0.;    // distance to barrel
    double distec = 0.;     // distance to end caps
    CaloCell_ID::CaloSample sample;
    // get eta direction of particle:
    double trketa = parameters.momentum().eta();

    // PreSampler :
    // figure out which sampling will be hit
    distbar = m_calodepth->deta(CaloCell_ID::PreSamplerB,trketa);
    distec = m_calodepth->deta(CaloCell_ID::PreSamplerE,trketa);
    
    ATH_MSG_DEBUG(" TrackTo ...PS : for eta= " << trketa << " dist to Barrel =" << distbar 
        << " to endcap =" << distec);
    if (distbar < 0 )  sample = CaloCell_ID::PreSamplerB;
    else if (distec < 0 ) sample = CaloCell_ID::PreSamplerE;
    else if ( distbar < distec) sample = CaloCell_ID::PreSamplerB;
    else sample = CaloCell_ID::PreSamplerE;
    ATH_MSG_DEBUG( "  => will shoot in sample " << sample );

    (*parameterVector)[IExtrapolateToCaloTool::PreSampler] = extrapolate(parameters, sample, m_offsetPresampler, particle, dir);

    // do rest of the extrapolation step-wise:
    extrapolateStepwiseInCalo ( parameters,
            particle,
            dir,
            *parameterVector);

    return parameterVector;
}


ImpactInCalo* ExtrapolateToCaloTool::getImpactInCalo (const Trk::Track& trk,
            const Trk::ParticleHypothesis particle,
            const Trk::PropDirection dir) const {
    const DataVector< const Trk::TrackParameters >* parametersInCalo = getParametersInCalo (trk, particle, dir);
    if (!parametersInCalo) return 0;
    ImpactInCalo* imp = getImpactInCaloFromParametersInCalo(*parametersInCalo);
    // delete extrapolated parameters
    delete parametersInCalo;
    return imp;
}

ImpactInCalo* ExtrapolateToCaloTool::getImpactInCalo (const Trk::TrackParticleBase& trk,
            const Trk::ParticleHypothesis particle,
            const Trk::PropDirection dir) const {
    const DataVector< const Trk::TrackParameters >* parametersInCalo = getParametersInCalo (trk, particle, dir);
    if (!parametersInCalo) return 0;
    ImpactInCalo* imp = getImpactInCaloFromParametersInCalo(*parametersInCalo);
    // delete extrapolated parameters
    delete parametersInCalo;
    return imp;
}

ImpactInCalo* ExtrapolateToCaloTool::getImpactInCalo (const Trk::TrackParameters& parameters,
            Trk::ParticleHypothesis particleHypo,
            const Trk::PropDirection dir) const {

    const DataVector< const Trk::TrackParameters >* parametersInCalo = getParametersInCalo (parameters, particleHypo, dir);
    if (!parametersInCalo) return 0;
    ImpactInCalo* imp = getImpactInCaloFromParametersInCalo(*parametersInCalo);
    // delete extrapolated parameters
    delete parametersInCalo;
    return imp;
}

ImpactInCalo* ExtrapolateToCaloTool::getImpactInCalo (const Trk::NeutralParameters& parameters,
            Trk::ParticleHypothesis particleHypo,
            const Trk::PropDirection dir) const {

    const DataVector< const Trk::NeutralParameters >* parametersInCalo = getParametersInCalo (parameters, particleHypo, dir);
    if (!parametersInCalo) return 0;
    ImpactInCalo* imp = getImpactInCaloFromParametersInCalo(*parametersInCalo);
    // delete extrapolated parameters
    delete parametersInCalo;
    return imp;
}

ImpactInCalo* ExtrapolateToCaloTool::getImpactInCaloFromParametersInCalo(const DataVector< const Trk::TrackParameters >& parametersInCalo) const {
    double x_0 = 0.;                double y_0 = 0.;            double z_0 = 0.;
    double etaCaloLocal_0 = 0.;     double phiCaloLocal_0 = 0.;
    double trketa_at_0 = 0.;        double trkphi_at_0 = 0.;
    double x_1 = 0.;                double y_1 = 0.;            double z_1 = 0.;
    double etaCaloLocal_1 = 0.;     double phiCaloLocal_1 = 0.;
    double trketa_at_1 = 0.;        double trkphi_at_1 = 0.;
    double x_2 = 0.;                double y_2 = 0.;            double z_2 = 0.;
    double etaCaloLocal_2 = 0.;     double phiCaloLocal_2 = 0.;
    double trketa_at_2 = 0.;        double trkphi_at_2 = 0.;
    double x_3 = 0.;                double y_3 = 0.;            double z_3 = 0.;
    double etaCaloLocal_3 = 0.;     double phiCaloLocal_3 = 0.;
    double trketa_at_3 = 0.;        double trkphi_at_3 = 0.;
    double x_tile = 0.;             double y_tile = 0.;         double z_tile = 0.;
    double etaCaloLocal_tile = 0.;  double phiCaloLocal_tile = 0.;
    double trketa_at_tile = 0.;     double trkphi_at_tile = 0.;

    if (parametersInCalo[IExtrapolateToCaloTool::PreSampler]) {
        x_0             = parametersInCalo[IExtrapolateToCaloTool::PreSampler]->position().x();
        y_0             = parametersInCalo[IExtrapolateToCaloTool::PreSampler]->position().y();
        z_0             = parametersInCalo[IExtrapolateToCaloTool::PreSampler]->position().z();
        etaCaloLocal_0  = parametersInCalo[IExtrapolateToCaloTool::PreSampler]->position().eta();
        phiCaloLocal_0  = parametersInCalo[IExtrapolateToCaloTool::PreSampler]->position().phi();
        trketa_at_0     = parametersInCalo[IExtrapolateToCaloTool::PreSampler]->momentum().eta();
        trkphi_at_0     = parametersInCalo[IExtrapolateToCaloTool::PreSampler]->momentum().phi();
    }
    if (parametersInCalo[IExtrapolateToCaloTool::Strips]) {
        x_1             = parametersInCalo[IExtrapolateToCaloTool::Strips]->position().x();
        y_1             = parametersInCalo[IExtrapolateToCaloTool::Strips]->position().y();
        z_1             = parametersInCalo[IExtrapolateToCaloTool::Strips]->position().z();
        etaCaloLocal_1  = parametersInCalo[IExtrapolateToCaloTool::Strips]->position().eta();
        phiCaloLocal_1  = parametersInCalo[IExtrapolateToCaloTool::Strips]->position().phi();
        trketa_at_1     = parametersInCalo[IExtrapolateToCaloTool::Strips]->momentum().eta();
        trkphi_at_1     = parametersInCalo[IExtrapolateToCaloTool::Strips]->momentum().phi();
    }
    if (parametersInCalo[IExtrapolateToCaloTool::Middle]) {
        x_2             = parametersInCalo[IExtrapolateToCaloTool::Middle]->position().x();
        y_2             = parametersInCalo[IExtrapolateToCaloTool::Middle]->position().y();
        z_2             = parametersInCalo[IExtrapolateToCaloTool::Middle]->position().z();
        etaCaloLocal_2  = parametersInCalo[IExtrapolateToCaloTool::Middle]->position().eta();
        phiCaloLocal_2  = parametersInCalo[IExtrapolateToCaloTool::Middle]->position().phi();
        trketa_at_2     = parametersInCalo[IExtrapolateToCaloTool::Middle]->momentum().eta();
        trkphi_at_2     = parametersInCalo[IExtrapolateToCaloTool::Middle]->momentum().phi();
    }
    if (parametersInCalo[IExtrapolateToCaloTool::Back]) {
        x_3             = parametersInCalo[IExtrapolateToCaloTool::Back]->position().x();
        y_3             = parametersInCalo[IExtrapolateToCaloTool::Back]->position().y();
        z_3             = parametersInCalo[IExtrapolateToCaloTool::Back]->position().z();
        etaCaloLocal_3  = parametersInCalo[IExtrapolateToCaloTool::Back]->position().eta();
        phiCaloLocal_3  = parametersInCalo[IExtrapolateToCaloTool::Back]->position().phi();
        trketa_at_3     = parametersInCalo[IExtrapolateToCaloTool::Back]->momentum().eta();
        trkphi_at_3     = parametersInCalo[IExtrapolateToCaloTool::Back]->momentum().phi();
    }
    if (parametersInCalo[IExtrapolateToCaloTool::Tile]) {
        x_tile             = parametersInCalo[IExtrapolateToCaloTool::Tile]->position().x();
        y_tile             = parametersInCalo[IExtrapolateToCaloTool::Tile]->position().y();
        z_tile             = parametersInCalo[IExtrapolateToCaloTool::Tile]->position().z();
        etaCaloLocal_tile  = parametersInCalo[IExtrapolateToCaloTool::Tile]->position().eta();
        phiCaloLocal_tile  = parametersInCalo[IExtrapolateToCaloTool::Tile]->position().phi();
        trketa_at_tile     = parametersInCalo[IExtrapolateToCaloTool::Tile]->momentum().eta();
        trkphi_at_tile     = parametersInCalo[IExtrapolateToCaloTool::Tile]->momentum().phi();
    }
    return new ImpactInCalo(x_0, y_0, z_0, 
                            etaCaloLocal_0, phiCaloLocal_0,
                            trketa_at_0, trkphi_at_0,
                            x_1, y_1, z_1,
                            etaCaloLocal_1, phiCaloLocal_1,
                            trketa_at_1, trkphi_at_1,
                            x_2, y_2, z_2, 
                            etaCaloLocal_2, phiCaloLocal_2,
                            trketa_at_2, trkphi_at_2,
                            x_3, y_3, z_3, 
                            etaCaloLocal_3, phiCaloLocal_3,
                            trketa_at_3, trkphi_at_3,
                            x_tile, y_tile, z_tile, 
                            etaCaloLocal_tile, phiCaloLocal_tile,
                            trketa_at_tile, trkphi_at_tile );

}

ImpactInCalo* ExtrapolateToCaloTool::getImpactInCaloFromParametersInCalo(const DataVector< const Trk::NeutralParameters >& parametersInCalo) const {
    double x_0 = 0.;                double y_0 = 0.;            double z_0 = 0.;
    double etaCaloLocal_0 = 0.;     double phiCaloLocal_0 = 0.;
    double trketa_at_0 = 0.;        double trkphi_at_0 = 0.;
    double x_1 = 0.;                double y_1 = 0.;            double z_1 = 0.;
    double etaCaloLocal_1 = 0.;     double phiCaloLocal_1 = 0.;
    double trketa_at_1 = 0.;        double trkphi_at_1 = 0.;
    double x_2 = 0.;                double y_2 = 0.;            double z_2 = 0.;
    double etaCaloLocal_2 = 0.;     double phiCaloLocal_2 = 0.;
    double trketa_at_2 = 0.;        double trkphi_at_2 = 0.;
    double x_3 = 0.;                double y_3 = 0.;            double z_3 = 0.;
    double etaCaloLocal_3 = 0.;     double phiCaloLocal_3 = 0.;
    double trketa_at_3 = 0.;        double trkphi_at_3 = 0.;
    double x_tile = 0.;             double y_tile = 0.;         double z_tile = 0.;
    double etaCaloLocal_tile = 0.;  double phiCaloLocal_tile = 0.;
    double trketa_at_tile = 0.;     double trkphi_at_tile = 0.;

    if (parametersInCalo[IExtrapolateToCaloTool::PreSampler]) {
        x_0             = parametersInCalo[IExtrapolateToCaloTool::PreSampler]->position().x();
        y_0             = parametersInCalo[IExtrapolateToCaloTool::PreSampler]->position().y();
        z_0             = parametersInCalo[IExtrapolateToCaloTool::PreSampler]->position().z();
        etaCaloLocal_0  = parametersInCalo[IExtrapolateToCaloTool::PreSampler]->position().eta();
        phiCaloLocal_0  = parametersInCalo[IExtrapolateToCaloTool::PreSampler]->position().phi();
        trketa_at_0     = parametersInCalo[IExtrapolateToCaloTool::PreSampler]->momentum().eta();
        trkphi_at_0     = parametersInCalo[IExtrapolateToCaloTool::PreSampler]->momentum().phi();
    }
    if (parametersInCalo[IExtrapolateToCaloTool::Strips]) {
        x_1             = parametersInCalo[IExtrapolateToCaloTool::Strips]->position().x();
        y_1             = parametersInCalo[IExtrapolateToCaloTool::Strips]->position().y();
        z_1             = parametersInCalo[IExtrapolateToCaloTool::Strips]->position().z();
        etaCaloLocal_1  = parametersInCalo[IExtrapolateToCaloTool::Strips]->position().eta();
        phiCaloLocal_1  = parametersInCalo[IExtrapolateToCaloTool::Strips]->position().phi();
        trketa_at_1     = parametersInCalo[IExtrapolateToCaloTool::Strips]->momentum().eta();
        trkphi_at_1     = parametersInCalo[IExtrapolateToCaloTool::Strips]->momentum().phi();
    }
    if (parametersInCalo[IExtrapolateToCaloTool::Middle]) {
        x_2             = parametersInCalo[IExtrapolateToCaloTool::Middle]->position().x();
        y_2             = parametersInCalo[IExtrapolateToCaloTool::Middle]->position().y();
        z_2             = parametersInCalo[IExtrapolateToCaloTool::Middle]->position().z();
        etaCaloLocal_2  = parametersInCalo[IExtrapolateToCaloTool::Middle]->position().eta();
        phiCaloLocal_2  = parametersInCalo[IExtrapolateToCaloTool::Middle]->position().phi();
        trketa_at_2     = parametersInCalo[IExtrapolateToCaloTool::Middle]->momentum().eta();
        trkphi_at_2     = parametersInCalo[IExtrapolateToCaloTool::Middle]->momentum().phi();
    }
    if (parametersInCalo[IExtrapolateToCaloTool::Back]) {
        x_3             = parametersInCalo[IExtrapolateToCaloTool::Back]->position().x();
        y_3             = parametersInCalo[IExtrapolateToCaloTool::Back]->position().y();
        z_3             = parametersInCalo[IExtrapolateToCaloTool::Back]->position().z();
        etaCaloLocal_3  = parametersInCalo[IExtrapolateToCaloTool::Back]->position().eta();
        phiCaloLocal_3  = parametersInCalo[IExtrapolateToCaloTool::Back]->position().phi();
        trketa_at_3     = parametersInCalo[IExtrapolateToCaloTool::Back]->momentum().eta();
        trkphi_at_3     = parametersInCalo[IExtrapolateToCaloTool::Back]->momentum().phi();
    }
    if (parametersInCalo[IExtrapolateToCaloTool::Tile]) {
        x_tile             = parametersInCalo[IExtrapolateToCaloTool::Tile]->position().x();
        y_tile             = parametersInCalo[IExtrapolateToCaloTool::Tile]->position().y();
        z_tile             = parametersInCalo[IExtrapolateToCaloTool::Tile]->position().z();
        etaCaloLocal_tile  = parametersInCalo[IExtrapolateToCaloTool::Tile]->position().eta();
        phiCaloLocal_tile  = parametersInCalo[IExtrapolateToCaloTool::Tile]->position().phi();
        trketa_at_tile     = parametersInCalo[IExtrapolateToCaloTool::Tile]->momentum().eta();
        trkphi_at_tile     = parametersInCalo[IExtrapolateToCaloTool::Tile]->momentum().phi();
    }
    return new ImpactInCalo(x_0, y_0, z_0, 
                            etaCaloLocal_0, phiCaloLocal_0,
                            trketa_at_0, trkphi_at_0,
                            x_1, y_1, z_1,
                            etaCaloLocal_1, phiCaloLocal_1,
                            trketa_at_1, trkphi_at_1,
                            x_2, y_2, z_2, 
                            etaCaloLocal_2, phiCaloLocal_2,
                            trketa_at_2, trkphi_at_2,
                            x_3, y_3, z_3, 
                            etaCaloLocal_3, phiCaloLocal_3,
                            trketa_at_3, trkphi_at_3,
                            x_tile, y_tile, z_tile, 
                            etaCaloLocal_tile, phiCaloLocal_tile,
                            trketa_at_tile, trkphi_at_tile );

}

// ==================================================
ToolHandle<CaloDepthTool> ExtrapolateToCaloTool::getCaloDepth()
{
  //std::cout << " m_calodepth = " << m_calodepth << std::endl;
  return m_calodepth;
}

const DataVector< const Trk::TrackParameters >* ExtrapolateToCaloTool::getParametersInCalo (const Trk::TrackParticleBase& trk,
            const Trk::ParticleHypothesis particle,
            const Trk::PropDirection dir) const {

    DataVector< const Trk::TrackParameters >* parametersInCalo = new DataVector< const Trk::TrackParameters >(IExtrapolateToCaloTool::NCaloLayers, SG::OWN_ELEMENTS);
    // variables to select sampler (barrel or endcap)
    double distbar = 0.;    // distance to barrel
    double distec = 0.;     // distance to end caps
    CaloCell_ID::CaloSample sample;
    // get eta direction of particle:
    double trketa = trk.definingParameters().momentum().eta();

    // PreSampler :
    // figure out which sampling will be hit
    distbar = m_calodepth->deta(CaloCell_ID::PreSamplerB,trketa);
    distec = m_calodepth->deta(CaloCell_ID::PreSamplerE,trketa);
    
    ATH_MSG_DEBUG(" TrackTo ...PS : for eta= " << trketa << " dist to Barrel =" << distbar 
        << " to endcap =" << distec);
    if (distbar < 0 )  sample = CaloCell_ID::PreSamplerB;
    else if (distec < 0 ) sample = CaloCell_ID::PreSamplerE;
    else if ( distbar < distec) sample = CaloCell_ID::PreSamplerB;
    else sample = CaloCell_ID::PreSamplerE;
    ATH_MSG_DEBUG( "  => will shoot in sample " << sample );

    (*parametersInCalo)[IExtrapolateToCaloTool::PreSampler] = extrapolate(trk, sample, m_offsetPresampler, dir, particle);

    Trk::ParticleHypothesis hypo = particle;
    if (particle == Trk::undefined) {
        if (trk.originalTrack()) {
            hypo = trk.originalTrack()->info().particleHypothesis();
        } else {
            // Without the original track we cannot get the particle hypothesis used during the track fit
            // TODO: (will be added to TrackParticle at some point)!
            // use nonInteracting, if no hypothesis given:
            hypo = Trk::nonInteracting;
        }
    }
    // do rest of the extrapolation step-wise:
    extrapolateStepwiseInCalo ( trk.definingParameters(),
            hypo,
            dir,
            *parametersInCalo);

    return parametersInCalo;
}

const DataVector< const Trk::TrackParameters >* ExtrapolateToCaloTool::getParametersInCalo (const xAOD::TrackParticle& trk,
            const Trk::ParticleHypothesis particle,
            const Trk::PropDirection dir) const {

    DataVector< const Trk::TrackParameters >* parametersInCalo = new DataVector< const Trk::TrackParameters >(IExtrapolateToCaloTool::NCaloLayers, SG::OWN_ELEMENTS);
    // variables to select sampler (barrel or endcap)
    double distbar = 0.;    // distance to barrel
    double distec = 0.;     // distance to end caps
    CaloCell_ID::CaloSample sample;
    // get eta direction of particle:
    double trketa = trk.eta();

    // PreSampler :
    // figure out which sampling will be hit
    distbar = m_calodepth->deta(CaloCell_ID::PreSamplerB,trketa);
    distec = m_calodepth->deta(CaloCell_ID::PreSamplerE,trketa);
    
    ATH_MSG_DEBUG(" TrackTo ...PS : for eta= " << trketa << " dist to Barrel =" << distbar 
        << " to endcap =" << distec);
    if (distbar < 0 )  sample = CaloCell_ID::PreSamplerB;
    else if (distec < 0 ) sample = CaloCell_ID::PreSamplerE;
    else if ( distbar < distec) sample = CaloCell_ID::PreSamplerB;
    else sample = CaloCell_ID::PreSamplerE;
    ATH_MSG_DEBUG( "  => will shoot in sample " << sample );

    (*parametersInCalo)[IExtrapolateToCaloTool::PreSampler] = extrapolate(trk, sample, m_offsetPresampler, dir, particle);

    Trk::ParticleHypothesis hypo = particle;
    if (particle == Trk::undefined) {
        // not very nice, will hopefully have only a single (forwarded) enum at some point,
        // instead of having xAOD::ParticleHypothesis and Trk::ParticleHypothesis
        hypo = static_cast<Trk::ParticleHypothesis>(trk.particleHypothesis());
    }
    // do rest of the extrapolation step-wise:
    extrapolateStepwiseInCalo ( trk.perigeeParameters(),
            hypo,
            dir,
            *parametersInCalo);

    return parametersInCalo;
}

const DataVector< const Trk::TrackParameters >* ExtrapolateToCaloTool::getParametersInCalo (const Trk::Track& trk,
            const Trk::ParticleHypothesis particle,
            const Trk::PropDirection dir) const {

    DataVector< const Trk::TrackParameters >* parametersInCalo = new DataVector< const Trk::TrackParameters >(IExtrapolateToCaloTool::NCaloLayers, SG::OWN_ELEMENTS);
    // variables to select sampler (barrel or endcap)
    double distbar = 0.;    // distance to barrel
    double distec = 0.;     // distance to end caps
    CaloCell_ID::CaloSample sample;

    double trketa = 0;
    const Trk::TrackParameters* firstParam = 0;
    if (trk.perigeeParameters()) {
        trketa = trk.perigeeParameters()->eta();
        firstParam = trk.perigeeParameters();
    } else {
        if (trk.trackParameters()) {
            firstParam = *(trk.trackParameters()->begin());
            trketa = firstParam->momentum().eta();
        } else {
            return parametersInCalo;
        }
    }

    // PreSampler :
    // figure out which sampling will be hit
    distbar = m_calodepth->deta(CaloCell_ID::PreSamplerB,trketa);
    distec = m_calodepth->deta(CaloCell_ID::PreSamplerE,trketa);
    
    ATH_MSG_DEBUG(" TrackTo ...PS : for eta= " << trketa << " dist to Barrel =" << distbar 
        << " to endcap =" << distec);
    if (distbar < 0 )  sample = CaloCell_ID::PreSamplerB;
    else if (distec < 0 ) sample = CaloCell_ID::PreSamplerE;
    else if ( distbar < distec) sample = CaloCell_ID::PreSamplerB;
    else sample = CaloCell_ID::PreSamplerE;
    ATH_MSG_DEBUG( "  => will shoot in sample " << sample );

    (*parametersInCalo)[IExtrapolateToCaloTool::PreSampler] = extrapolate(trk, sample, m_offsetPresampler, dir, particle);

    Trk::ParticleHypothesis hypo = (particle == Trk::undefined ? trk.info().particleHypothesis() : particle);
    // do rest of the extrapolation step-wise:
    extrapolateStepwiseInCalo ( *firstParam,
            hypo,
            dir,
            *parametersInCalo);

    return parametersInCalo;
}


void ExtrapolateToCaloTool::extrapolateStepwiseInCalo (
            const Trk::TrackParameters& parameters,
            Trk::ParticleHypothesis particle,
            const Trk::PropDirection dir,
            DataVector< const Trk::TrackParameters >& parameterVector) const {
    double distbar = 0.;    // distance to barrel
    double distec = 0.;     // distance to end caps
    CaloCell_ID::CaloSample sample;

    // check whether we got the parameters at the first sampling:
    if (parameterVector[IExtrapolateToCaloTool::PreSampler] == 0) return;
    // the last successfully extrapolated parameter in the calo:
    const Trk::TrackParameters* lastParamInCalo = parameterVector[IExtrapolateToCaloTool::PreSampler];
    // take position on first sampling to decide which sampling will be hit next:
    double trketa = lastParamInCalo->position().eta();
    // particle hypo for extrapolation inside calo
    Trk::ParticleHypothesis particleHypoStepWiseInCalo = (m_ignoreMaterialEffectsInsideCalo ? Trk::nonInteracting : particle);

    // strip :
    distbar = m_calodepth->deta(CaloCell_ID::EMB1,trketa);
    distec = m_calodepth->deta(CaloCell_ID::EME1,trketa);
    ATH_MSG_DEBUG(" TrackTo ...Strip : for eta= " << trketa << " dist to Barrel =" << distbar 
        << " to endcap =" << distec);
    if (distbar < 0 ) sample = CaloCell_ID::EMB1;
    else if (distec < 0 ) sample = CaloCell_ID::EME1;
    else if ( distbar < distec) sample = CaloCell_ID::EMB1;
    else sample = CaloCell_ID::EME1;
    ATH_MSG_DEBUG( "  => will shoot in sample " << sample );

    parameterVector[IExtrapolateToCaloTool::Strips] = extrapolate(*lastParamInCalo, sample, m_offsetStrip, particleHypoStepWiseInCalo, dir);

    // fall back to original parameters if extrapolation did not succeed:
    if (!parameterVector[IExtrapolateToCaloTool::Strips]) {
        parameterVector[IExtrapolateToCaloTool::Strips] = extrapolate(parameters, sample, m_offsetStrip, particle, dir);
    }
    if (parameterVector[IExtrapolateToCaloTool::Strips]) {
        lastParamInCalo = parameterVector[IExtrapolateToCaloTool::Strips];
    }

    // EM middle :
    distbar = m_calodepth->deta(CaloCell_ID::EMB2,trketa);
    distec = m_calodepth->deta(CaloCell_ID::EME2,trketa);
    ATH_MSG_DEBUG(" TrackTo ...Middle : for eta= " << trketa << " dist to Barrel =" << distbar 
        << " to endcap =" << distec);
    if (distbar < 0 ) sample = CaloCell_ID::EMB2;
    else if (distec < 0 ) sample = CaloCell_ID::EME2;
    else if ( distbar < distec) sample = CaloCell_ID::EMB2;
    else sample = CaloCell_ID::EME2;
    ATH_MSG_DEBUG( "  => will shoot in sample " << sample );
    parameterVector[IExtrapolateToCaloTool::Middle] = extrapolate(*lastParamInCalo, sample, m_offsetMiddle, particleHypoStepWiseInCalo, dir);
    if (!parameterVector[IExtrapolateToCaloTool::Middle]) {
        parameterVector[IExtrapolateToCaloTool::Middle] = extrapolate(parameters, sample, m_offsetMiddle, particle, dir);
    }
    if (parameterVector[IExtrapolateToCaloTool::Middle]) {
        lastParamInCalo = parameterVector[IExtrapolateToCaloTool::Middle];
    }

    // EM back :
    distbar = m_calodepth->deta(CaloCell_ID::EMB3,trketa);
    distec = m_calodepth->deta(CaloCell_ID::EME3,trketa);
    ATH_MSG_DEBUG(" TrackTo ...Back : for eta= " << trketa << " dist to Barrel =" << distbar 
        << " to endcap =" << distec);
    if (distbar < 0 ) sample = CaloCell_ID::EMB3;
    else if (distec < 0 ) sample = CaloCell_ID::EME3;
    else if ( distbar < distec) sample = CaloCell_ID::EMB3;
    else sample = CaloCell_ID::EME3;
    ATH_MSG_DEBUG( "  => will shoot in sample " << sample );
    parameterVector[IExtrapolateToCaloTool::Back] = extrapolate(*lastParamInCalo, sample, m_offsetBack, particleHypoStepWiseInCalo, dir);
    if (!parameterVector[IExtrapolateToCaloTool::Back]) {
        parameterVector[IExtrapolateToCaloTool::Back] = extrapolate(parameters, sample, m_offsetBack, particle, dir);
    }
    if (parameterVector[IExtrapolateToCaloTool::Back]) {
        lastParamInCalo = parameterVector[IExtrapolateToCaloTool::Back];
    }

    // Tile or HEC0 :
    distbar = m_calodepth->deta(CaloCell_ID::TileBar0,trketa);
    distec = m_calodepth->deta(CaloCell_ID::HEC0,trketa);
    ATH_MSG_DEBUG(" TrackTo ...Tile : for eta= " << trketa << " dist to Barrel =" << distbar 
        << " to endcap =" << distec);
    if (distbar < 0 ) sample = CaloCell_ID::TileBar0;
    else if (distec < 0 ) sample = CaloCell_ID::HEC0;
    else if ( distbar > distec && distec < 10. ) sample = CaloCell_ID::HEC0;
    else sample = CaloCell_ID::TileBar0;
    ATH_MSG_DEBUG( "  => will shoot in sample " << sample );
    parameterVector[IExtrapolateToCaloTool::Tile] = extrapolate(*lastParamInCalo, sample, m_offsetTile, particleHypoStepWiseInCalo, dir);
    if (!parameterVector[IExtrapolateToCaloTool::Tile]) {
        parameterVector[IExtrapolateToCaloTool::Tile] = extrapolate(parameters, sample, m_offsetTile, particle, dir);
    }
    return;
}


void ExtrapolateToCaloTool::extrapolateStepwiseInCalo (
            const Trk::NeutralParameters& parameters,
            Trk::ParticleHypothesis particle,
            const Trk::PropDirection dir,
            DataVector< const Trk::NeutralParameters >& parameterVector) const {
    double distbar = 0.;    // distance to barrel
    double distec = 0.;     // distance to end caps
    CaloCell_ID::CaloSample sample;

    // check whether we got the parameters at the first sampling:
    if (parameterVector[IExtrapolateToCaloTool::PreSampler] == 0) return;
    // the last successfully extrapolated parameter in the calo:
    const Trk::NeutralParameters* lastParamInCalo = parameterVector[IExtrapolateToCaloTool::PreSampler];
    // take position on first sampling to decide which sampling will be hit next:
    double trketa = lastParamInCalo->position().eta();
    // particle hypo for extrapolation inside calo
    Trk::ParticleHypothesis particleHypoStepWiseInCalo = (m_ignoreMaterialEffectsInsideCalo ? Trk::nonInteracting : particle);

    // strip :
    distbar = m_calodepth->deta(CaloCell_ID::EMB1,trketa);
    distec = m_calodepth->deta(CaloCell_ID::EME1,trketa);
    ATH_MSG_DEBUG(" TrackTo ...Strip : for eta= " << trketa << " dist to Barrel =" << distbar 
        << " to endcap =" << distec);
    if (distbar < 0 ) sample = CaloCell_ID::EMB1;
    else if (distec < 0 ) sample = CaloCell_ID::EME1;
    else if ( distbar < distec) sample = CaloCell_ID::EMB1;
    else sample = CaloCell_ID::EME1;
    ATH_MSG_DEBUG( "  => will shoot in sample " << sample );

    parameterVector[IExtrapolateToCaloTool::Strips] = extrapolate(*lastParamInCalo, sample, m_offsetStrip, particleHypoStepWiseInCalo, dir);

    // fall back to original parameters if extrapolation did not succeed:
    if (!parameterVector[IExtrapolateToCaloTool::Strips]) {
        parameterVector[IExtrapolateToCaloTool::Strips] = extrapolate(parameters, sample, m_offsetStrip, particle, dir);
    }
    if (parameterVector[IExtrapolateToCaloTool::Strips]) {
        lastParamInCalo = parameterVector[IExtrapolateToCaloTool::Strips];
    }

    // EM middle :
    distbar = m_calodepth->deta(CaloCell_ID::EMB2,trketa);
    distec = m_calodepth->deta(CaloCell_ID::EME2,trketa);
    ATH_MSG_DEBUG(" TrackTo ...Middle : for eta= " << trketa << " dist to Barrel =" << distbar 
        << " to endcap =" << distec);
    if (distbar < 0 ) sample = CaloCell_ID::EMB2;
    else if (distec < 0 ) sample = CaloCell_ID::EME2;
    else if ( distbar < distec) sample = CaloCell_ID::EMB2;
    else sample = CaloCell_ID::EME2;
    ATH_MSG_DEBUG( "  => will shoot in sample " << sample );
    parameterVector[IExtrapolateToCaloTool::Middle] = extrapolate(*lastParamInCalo, sample, m_offsetMiddle, particleHypoStepWiseInCalo, dir);
    if (!parameterVector[IExtrapolateToCaloTool::Middle]) {
        parameterVector[IExtrapolateToCaloTool::Middle] = extrapolate(parameters, sample, m_offsetMiddle, particle, dir);
    }
    if (parameterVector[IExtrapolateToCaloTool::Middle]) {
        lastParamInCalo = parameterVector[IExtrapolateToCaloTool::Middle];
    }

    // EM back :
    distbar = m_calodepth->deta(CaloCell_ID::EMB3,trketa);
    distec = m_calodepth->deta(CaloCell_ID::EME3,trketa);
    ATH_MSG_DEBUG(" TrackTo ...Back : for eta= " << trketa << " dist to Barrel =" << distbar 
        << " to endcap =" << distec);
    if (distbar < 0 ) sample = CaloCell_ID::EMB3;
    else if (distec < 0 ) sample = CaloCell_ID::EME3;
    else if ( distbar < distec) sample = CaloCell_ID::EMB3;
    else sample = CaloCell_ID::EME3;
    ATH_MSG_DEBUG( "  => will shoot in sample " << sample );
    parameterVector[IExtrapolateToCaloTool::Back] = extrapolate(*lastParamInCalo, sample, m_offsetBack, particleHypoStepWiseInCalo, dir);
    if (!parameterVector[IExtrapolateToCaloTool::Back]) {
        parameterVector[IExtrapolateToCaloTool::Back] = extrapolate(parameters, sample, m_offsetBack, particle, dir);
    }
    if (parameterVector[IExtrapolateToCaloTool::Back]) {
        lastParamInCalo = parameterVector[IExtrapolateToCaloTool::Back];
    }

    // Tile or HEC0 :
    distbar = m_calodepth->deta(CaloCell_ID::TileBar0,trketa);
    distec = m_calodepth->deta(CaloCell_ID::HEC0,trketa);
    ATH_MSG_DEBUG(" TrackTo ...Tile : for eta= " << trketa << " dist to Barrel =" << distbar 
        << " to endcap =" << distec);
    if (distbar < 0 ) sample = CaloCell_ID::TileBar0;
    else if (distec < 0 ) sample = CaloCell_ID::HEC0;
    else if ( distbar > distec && distec < 10. ) sample = CaloCell_ID::HEC0;
    else sample = CaloCell_ID::TileBar0;
    ATH_MSG_DEBUG( "  => will shoot in sample " << sample );
    parameterVector[IExtrapolateToCaloTool::Tile] = extrapolate(*lastParamInCalo, sample, m_offsetTile, particleHypoStepWiseInCalo, dir);
    if (!parameterVector[IExtrapolateToCaloTool::Tile]) {
        parameterVector[IExtrapolateToCaloTool::Tile] = extrapolate(parameters, sample, m_offsetTile, particle, dir);
    }
    return;
}

Amg::Vector3D ExtrapolateToCaloTool::getMomentumAtVertex(const xAOD::Vertex& vertex, 
                                                         bool reuse /* = true */) const
{
  Amg::Vector3D momentum(0., 0., 0.);
  
  // xAOD::Vertex does not have method isAvailable for the moment
  // create a lambda function
  auto isAvailable = [](const xAOD::Vertex& vertex, std::string name) { 
    SG::AuxElement::Accessor<float> acc(name, "");
    return acc.isAvailable(vertex); } ; 
  
  if (vertex.nTrackParticles() == 0)
  {
    ATH_MSG_WARNING("getMomentumAtVertex : vertex has no track particles!");
    return momentum;
  }
  
  if (reuse &&
      isAvailable(vertex, "px") && 
      isAvailable(vertex, "py") && 
      isAvailable(vertex, "pz") )
  {
    // Already decorated with parameters at vertex
    ATH_MSG_DEBUG("getMomentumAtVertex : getting from auxdata");
    return Amg::Vector3D( vertex.auxdata<float>("px"),
                          vertex.auxdata<float>("py"),
                          vertex.auxdata<float>("pz") );
  }
  else if (vertex.vxTrackAtVertexAvailable() && vertex.vxTrackAtVertex().size())
  {
    // Use the parameters at the vertex 
    // (the tracks should be parallel but we will do the sum anyway)
    ATH_MSG_DEBUG("getMomentumAtVertex : getting from vxTrackAtVertex");
    for (const auto& trkAtVertex : vertex.vxTrackAtVertex())
    {
      const Trk::TrackParameters* paramAtVertex = trkAtVertex.perigeeAtVertex();
      if (!paramAtVertex)
        ATH_MSG_WARNING("VxTrackAtVertex does not have perigee at vertex");
      else
        momentum += paramAtVertex->momentum();
    }
  }
  else if (vertex.nTrackParticles() == 1)
  {
    // Use the first measurement
    ATH_MSG_DEBUG("getMomentumAtVertex : 1 track only, getting from first measurement");
    const xAOD::TrackParticle *tp = vertex.trackParticle(0);
    unsigned int index(0);
    if (!tp || !tp->indexOfParameterAtPosition(index, xAOD::FirstMeasurement))
    {
      ATH_MSG_WARNING("No TrackParticle or no have first measurement");
    }
    else
      momentum += tp->curvilinearParameters(index).momentum();
    // OR last 3 values of trackParameters(index)
  }
  else
  {
    // Extrapolate track particles to vertex
    // (the tracks should be parallel but we will do the sum anyway)
    ATH_MSG_DEBUG("getMomentumAtVertex : extrapolating to perigee surface");    
    const Trk::PerigeeSurface *surface = new Trk::PerigeeSurface(vertex.position());
    for (unsigned int i = 0; i < vertex.nTrackParticles(); ++i)
    {
      const xAOD::TrackParticle* tp = vertex.trackParticle( i );
      if (!tp)
      {
        ATH_MSG_WARNING("NULL pointer to TrackParticle in vertex");
        continue;
      }
      const Trk::TrackParameters* params = m_extrapolator->extrapolate(*tp, *surface, Trk::alongMomentum);
      if (!params)
        ATH_MSG_DEBUG("Extrapolation to vertex (perigee) failed");
      else
      {
        momentum += params->momentum();
        delete params;
      }
    }
    delete surface;
  }
  
  return momentum;
}

Trk::SurfaceIntersection ExtrapolateToCaloTool::getIntersectionInCalo(const Amg::Vector3D& position, const Amg::Vector3D& momentum, const CaloCell_ID::CaloSample sample) const
{
  Trk::SurfaceIntersection result{Amg::Vector3D(0., 0., 0.), 0., false};

  // get the destination Surface
  const Trk::Surface* surface = m_calosurf->CreateUserSurface (sample, 0. /* offset */, momentum.eta());
  if (!surface)
    return result;
  
  // intersect with calorimeter surface
  result = surface->straightLineIntersection(position, momentum, true);     
  delete surface;
  return result;
}
