/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaterialEffectsEngine.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// STL
#include <sstream>
// Trk include
#include "TrkExEngine/MaterialEffectsEngine.h"
#include "TrkGeometry/Layer.h"

// constructor
Trk::MaterialEffectsEngine::MaterialEffectsEngine(const std::string& t, const std::string& n, const IInterface* p)
: AthAlgTool(t,n,p),
  m_eLossCorrection(true),
  m_eLossMpv(true),
  m_mscCorrection(true)
{
    declareInterface<Trk::IMaterialEffectsEngine>(this);
    // steering of the screen outoput (SOP)
    declareProperty("OutputPrefix"                          , m_sopPrefix);
    declareProperty("OutputPostfix"                         , m_sopPostfix);
    // steering of the material effects engine behaviour
    declareProperty("EnergyLossCorrection"                  , m_eLossCorrection);
    declareProperty("MostProbableEnergyLoss"                , m_eLossMpv);
    declareProperty("MultipleScatteringCorrection"          , m_mscCorrection);
}

// destructor
Trk::MaterialEffectsEngine::~MaterialEffectsEngine()
{}


// the interface method initialize
StatusCode Trk::MaterialEffectsEngine::initialize()
{
    EX_MSG_DEBUG( "", "initialize","",  "successful" );
    return StatusCode::SUCCESS;
}

// the interface method finalize
StatusCode Trk::MaterialEffectsEngine::finalize()
{
    EX_MSG_DEBUG( "", "finalize","",  "successful" );
    return StatusCode::SUCCESS;
}

/** neutral extrapolation - just collect material */
Trk::ExtrapolationCode Trk::MaterialEffectsEngine::handleMaterial(Trk::ExCellNeutral& eCell,
                                                                  Trk::PropDirection dir,
                                                                  Trk::MaterialUpdateStage matupstage) const
{

    // the Extrapolator made sure that the layer is the lead layer && the parameters are the lead parameters
    if (eCell.leadLayer && eCell.leadLayer->layerMaterialProperties()){
        EX_MSG_DEBUG( ++eCell.navigationStep, "layer",  eCell.leadLayer->layerIndex().value(), "handleMaterial for neutral parameters called - collect material.");
        // now calculate the pathCorrection from the layer surface - it is signed, gives you the relative direction to the layer
        const Trk::Layer* layer = eCell.leadLayer;
        // path correction
        double pathCorrection = layer->surfaceRepresentation().pathCorrection(eCell.leadParameters->position(),dir*(eCell.leadParameters->momentum()));
        // the relative direction wrt with the layer
        Trk::PropDirection rlDir = (pathCorrection >= 0. ? Trk::alongMomentum : Trk::oppositeMomentum);
        // multiply by the pre-and post-update factor
        double mFactor = layer->layerMaterialProperties()->factor(rlDir, matupstage);
        if (mFactor == 0.){
            EX_MSG_VERBOSE(eCell.navigationStep, "layer",  layer->layerIndex().value(), "material collection with "  << (matupstage > 0. ? "pre " : "post ")  << "factor 0.");
            // return the parameters untouched -
            return Trk::ExtrapolationCode::InProgress;
        }
        pathCorrection = mFactor*pathCorrection;
        // screen output
        EX_MSG_VERBOSE(eCell.navigationStep, "layer",  layer->layerIndex().value(), "material update with corr factor = " << pathCorrection);
        // get the actual material bin
        const Trk::MaterialProperties* materialProperties = layer->layerMaterialProperties()->fullMaterial(eCell.leadParameters->position());
        // and let's check if there's acutally something to do
        if (materialProperties && std::abs(pathCorrection)>0.){
            // thickness in X0 
            double thicknessInX0          = materialProperties->thicknessInX0();
            // check if material filling was requested
            if (eCell.checkConfigurationMode(Trk::ExtrapolationMode::CollectMaterial)){
                EX_MSG_VERBOSE(eCell.navigationStep, "layer",  layer->layerIndex().value(), "collecting material of [t/X0] = " << thicknessInX0);
                eCell.stepMaterial(eCell.leadParameters->associatedSurface(), layer, eCell.leadParameters->position(), pathCorrection, materialProperties);
            } else {
                EX_MSG_VERBOSE(eCell.navigationStep, "layer",  layer->layerIndex().value(), "adding material of [t/X0] = " << thicknessInX0);
                eCell.addMaterial(pathCorrection, materialProperties);
            }
        }
    }
    // only in case of post update it should not return InProgress
    return Trk::ExtrapolationCode::InProgress;
}


/** charged extrapolation */
Trk::ExtrapolationCode Trk::MaterialEffectsEngine::handleMaterial(Trk::ExCellCharged& eCell,
                                                                  Trk::PropDirection dir,
                                                                  Trk::MaterialUpdateStage matupstage) const
{

    // the Extrapolator made sure that the layer is the lead layer && the parameters are the lead parameters
    if (eCell.leadLayer && eCell.leadLayer->layerMaterialProperties()){
        EX_MSG_DEBUG( ++eCell.navigationStep, "layer",  eCell.leadLayer->layerIndex().value(), "handleMaterial for charged parameters called.");
        // update the track parameters
        eCell.leadParameters = updateTrackParameters(*eCell.leadParameters,eCell,dir,matupstage);
    }
    // only in case of post update it should not return InProgress
    return Trk::ExtrapolationCode::InProgress;
}

/** charged extrapolation */
const Trk::TrackParameters* Trk::MaterialEffectsEngine::updateTrackParameters(const Trk::TrackParameters& parameters,
                                                                              Trk::ExCellCharged& eCell,
                                                                              Trk::PropDirection dir,
                                                                              Trk::MaterialUpdateStage matupstage) const
{
    // now calculate the pathCorrection from the layer surface - it is signed, gives you the relative direction to the layer
    const Trk::Layer* layer = eCell.leadLayer;
    // path correction
    double pathCorrection = layer->surfaceRepresentation().pathCorrection(parameters.position(),dir*(parameters.momentum()));
    // the relative direction wrt with the layer
    Trk::PropDirection rlDir = (pathCorrection >= 0. ? Trk::alongMomentum : Trk::oppositeMomentum);
    // multiply by the pre-and post-update factor
    double mFactor = layer->layerMaterialProperties()->factor(rlDir, matupstage);
    if (mFactor == 0.){
        EX_MSG_VERBOSE(eCell.navigationStep, "layer",  layer->layerIndex().value(), "material update with "  << (matupstage > 0. ? "pre " : "post ")  << "factor 0. No update done.");
        // return the parameters untouched -
        return (&parameters);
    }
    pathCorrection = mFactor*pathCorrection;
    // screen output
    EX_MSG_VERBOSE(eCell.navigationStep, "layer",  layer->layerIndex().value(), "material update with corr factor = " << pathCorrection);
    // get the actual material bin
    const Trk::MaterialProperties* materialProperties = layer->layerMaterialProperties()->fullMaterial(parameters.position());
    // and let's check if there's acutally something to do
    if (materialProperties && std::abs(pathCorrection)>0. && 
	( m_eLossCorrection || m_mscCorrection || eCell.checkConfigurationMode(Trk::ExtrapolationMode::CollectMaterial)) ){
        // and add them 
        int sign = int(eCell.materialUpdateMode);
        // a simple cross-check if the parameters are the initial ones
        AmgVector(5)      uParameters = parameters.parameters();
        std::unique_ptr<AmgSymMatrix(5)> uCovariance =
          parameters.covariance()
            ? std::make_unique<AmgSymMatrix(5)>(*parameters.covariance())
            : nullptr;
        // get the material itself & its parameters
        const Trk::Material& material = materialProperties->material();
        double thicknessInX0          = materialProperties->thicknessInX0();
        double thickness              = materialProperties->thickness();
        // calculate energy loss and multiple scattering
        double p      = parameters.momentum().mag();
        double m      = m_particleMasses.mass[eCell.pHypothesis];
        double E      = sqrt(p*p+m*m);
        double beta   = p/E;
        // (A) - energy loss correction
        if (m_eLossCorrection){
            double sigmaP = 0.;
            double kazl   = 0.;
            /** dE/dl ionization energy loss per path unit */
            double dEdl = sign*dir*Trk::MaterialInteraction::dEdl_ionization(p, &material, eCell.pHypothesis, sigmaP, kazl);
            double dE   = thickness*pathCorrection*dEdl;
            sigmaP *= thickness*pathCorrection;
            // calcuate the new momentum
            double newP = sqrt((E+dE)*(E+dE)-m*m);
            uParameters[Trk::qOverP] = parameters.charge()/newP;
            double sigmaDeltaE = thickness*pathCorrection*sigmaP;
            double sigmaQoverP = sigmaDeltaE/std::pow(beta*p,2);
            // update the covariance if needed
            if (uCovariance)
    	       (*uCovariance)(Trk::qOverP, Trk::qOverP) += sign*sigmaQoverP*sigmaQoverP;
	}
        // (B) - update the covariance if needed
        if (uCovariance && m_mscCorrection){
	        /** multiple scattering as function of dInX0 */
	        double sigmaMS = Trk::MaterialInteraction::sigmaMS(thicknessInX0*pathCorrection, p, beta);
	        double sinTheta = sin(parameters.parameters()[Trk::theta]);
	        double sigmaDeltaPhiSq = sigmaMS*sigmaMS/(sinTheta*sinTheta);
	        double sigmaDeltaThetaSq = sigmaMS*sigmaMS;
	        // add or remove @TODO implement check for covariance matrix -> 0
	        (*uCovariance)(Trk::phi,Trk::phi)      += sign*sigmaDeltaPhiSq;
	        (*uCovariance)(Trk::theta, Trk::theta) += sign*sigmaDeltaThetaSq;
        }
        // check if material filling was requested
        if (eCell.checkConfigurationMode(Trk::ExtrapolationMode::CollectMaterial)){
	        EX_MSG_VERBOSE(eCell.navigationStep, "layer",  layer->layerIndex().value(), "collecting material of [t/X0] = " << thicknessInX0);
	        eCell.stepMaterial(parameters.associatedSurface(), layer, parameters.position(), pathCorrection, materialProperties);
        } else {
	        EX_MSG_VERBOSE(eCell.navigationStep, "layer",  layer->layerIndex().value(), "adding material of [t/X0] = " << thicknessInX0);
	        eCell.addMaterial(pathCorrection, materialProperties);
        }
        // now either create new ones or update - only start parameters can not be updated
        if (eCell.leadParameters != eCell.startParameters ){
            EX_MSG_VERBOSE(eCell.navigationStep, "layer",  layer->layerIndex().value(), "material update on non-initial parameters.");
            if (uCovariance)
              const_cast<Trk::TrackParameters*>(&parameters)->updateParameters(uParameters,*uCovariance);
            else 
              const_cast<Trk::TrackParameters*>(&parameters)->updateParameters(uParameters);
        } else {
            EX_MSG_VERBOSE(eCell.navigationStep, "layer",  layer->layerIndex().value(), "material update on initial parameters, creating new ones.");
            // create new parameters
            const Trk::Surface& tSurface = parameters.associatedSurface();
            const Trk::TrackParameters* tParameters = tSurface.createUniqueTrackParameters(uParameters[Trk::loc1],
                                                                                           uParameters[Trk::loc2],
                                                                                           uParameters[Trk::phi],
                                                                                           uParameters[Trk::theta],
                                                                                           uParameters[Trk::qOverP],
                                                                                           *uCovariance).release();
            // these are newly created
            return tParameters;
        }
    }
    //note aliaing input ...
    return (&parameters);
}
