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
: AthAlgTool(t,n,p)
{
    declareInterface<Trk::IMaterialEffectsEngine>(this);
    // steering of the screen outoput (SOP)
    declareProperty("OutputPrefix"                          , m_sopPrefix);
    declareProperty("OutputPostfix"                         , m_sopPostfix);
}

// destructor
Trk::MaterialEffectsEngine::~MaterialEffectsEngine()
{}


// the interface method initialize
StatusCode Trk::MaterialEffectsEngine::initialize()
{
    EX_MSG_INFO( "init", " starting initialize()" );
    return StatusCode::SUCCESS;
}    

// the interface method finalize
StatusCode Trk::MaterialEffectsEngine::finalize()
{    
    EX_MSG_INFO( "fini", "finalize() successful" );    
    return StatusCode::SUCCESS;
}


/** charged extrapolation */
Trk::ExtrapolationCode Trk::MaterialEffectsEngine::handleMaterial(Trk::ExCellCharged& eCell,
                                                                  Trk::PropDirection dir,
                                                                  Trk::MaterialUpdateStage matupstage) const
{

    // the Extrapolator made sure that the layer is the lead layer && the parameters are the lead parameters
    if (eCell.leadLayer && eCell.leadLayer->layerMaterialProperties()){
        EX_MSG_DEBUG(++eCell.navigationStep, "handleMaterial for charged TrackParameters called."); 
        // update the track parameters    
        eCell.leadParameters = updateTrackParameters(*eCell.leadParameters,eCell,dir,matupstage);
    }
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
    Trk::PropDirection rlDir = (pathCorrection > 0. ? Trk::alongMomentum : Trk::oppositeMomentum);
    // multiply by the pre-and post-update factor
    double mFactor = layer->layerMaterialProperties()->factor(rlDir, matupstage);
    if (mFactor == 0.){
        EX_MSG_VERBOSE(eCell.navigationStep, "material update with pre/post update factor 0. No update done."); 
        // return the parameters untouched -
        return (&parameters);
    }
    pathCorrection = mFactor*pathCorrection;
    // screen output
    EX_MSG_VERBOSE(eCell.navigationStep, "material update on layer with index " << layer->layerIndex().value() << " - corr factor = " << pathCorrection); 
    // get the actual material bin
    const Trk::MaterialProperties* materialProperties = layer->layerMaterialProperties()->fullMaterial(parameters.position());
    // and let's check if there's acutally something to do
    if (materialProperties){
        // a simple cross-check if the parameters are the initial ones
        AmgVector(5)      uParameters = parameters.parameters();
        AmgSymMatrix(5)*  uCovariance = parameters.covariance() ? new AmgSymMatrix(5)(*parameters.covariance()) : 0;
        // get the material itself & its parameters
        const Trk::Material& material = materialProperties->material();
        double thicknessInX0          = materialProperties->thicknessInX0();
        double thickness              = materialProperties->thickness();
        // calculate energy loss and multiple scattering
        double p      = parameters.momentum().mag();
        double m      = m_particleMasses.mass[eCell.pHypothesis];
        double E      = sqrt(p*p+m*m);
        double beta   = p/E;
        double sigmaP = 0.;
        double kazL   = 0.;
        /** dE/dl ionization energy loss per path unit */
        double dEdX = m_interactionFormulae.dEdl_ionization(p, &material, eCell.pHypothesis, sigmaP, kazL);    
        double dE   = dEdX*thickness*pathCorrection;
        // calcuate the new momentum
        double newP = sqrt((E+dE)*(E+dE)-m*m);
        uParameters[Trk::qOverP] = parameters.charge()/newP;
        // @TODO add straggling
        
        // update the covariance if needed
        if (uCovariance){
            /** multiple scattering as function of dInX0 */
            double sigmaMS = m_interactionFormulae.sigmaMS(thicknessInX0*pathCorrection, p, beta);    
            double sinTheta = sin(parameters.parameters()[Trk::theta]);
            double sigmaDeltaPhiSq = sigmaMS*sigmaMS/(sinTheta*sinTheta);
            double sigmaDeltaThetaSq = sigmaMS*sigmaMS;
            // and add them 
            int sign = int(eCell.materialUpdateMode);
            // add or remove @TODO implement check for covariance matrix -> 0
            (*uCovariance)(Trk::phi,Trk::phi)      += sign*sigmaDeltaPhiSq;
            (*uCovariance)(Trk::theta, Trk::theta) += sign*sigmaDeltaThetaSq;
        }
        // check if material filling was requested
        if (eCell.checkConfigurationMode(Trk::ExtrapolationMode::CollectMaterial)){
            EX_MSG_VERBOSE(eCell.navigationStep, "collecting material at layer " << layer->layerIndex().value() << " of - t/X0 = " << thicknessInX0); 
            eCell.stepMaterial(parameters.associatedSurface(), layer, parameters.position(), pathCorrection, materialProperties);
        }
        // now either create new ones or update - only start parameters can not be updated
        if (eCell.leadParameters != eCell.startParameters ){
            EX_MSG_VERBOSE(eCell.navigationStep, "material update on non-initial parameters."); 
            parameters.updateParameters(uParameters,uCovariance);
        } else {
            EX_MSG_VERBOSE(eCell.navigationStep, "material update on initial parameters, creating new ones."); 
            // create new parameters
            const Trk::Surface& tSurface = parameters.associatedSurface();
            const Trk::TrackParameters* tParameters = tSurface.createTrackParameters(uParameters[Trk::loc1],
                                                                                     uParameters[Trk::loc2],
                                                                                     uParameters[Trk::phi],
                                                                                     uParameters[Trk::theta],
                                                                                     uParameters[Trk::qOverP],
                                                                                     uCovariance);
             // these are newly created                                                                                     
             return tParameters;                                                                                                                                         
        }                
    }        
    return (&parameters);
}
