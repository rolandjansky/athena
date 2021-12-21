/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EnergyLossValidation.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Tracking
#include "TrkExAlgs/EnergyLossExtrapolationValidation.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkTrack/TrackStateOnSurface.h"
// Validation mode - TTree includes
#include "TTree.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SystemOfUnits.h"
// Framework
#include "AthContainers/DataVector.h"


//================ Constructor =================================================

Trk::EnergyLossExtrapolationValidation::EnergyLossExtrapolationValidation(const std::string& name, ISvcLocator* pSvcLocator)
: AthAlgorithm(name,pSvcLocator),
  m_highestVolume(nullptr),
  m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
  m_gaussDist(nullptr),
  m_flatDist(nullptr),
  m_materialCollectionValidation(false),
  m_validationTree(nullptr),
  m_validationRunTree(nullptr),
  m_validationTreeFolder("/val/EventTreeTG"),
  m_validationTreeName("EventTreeTG"),
  m_validationTreeDescription("Event output of the EnergyLossExtrapolationValidation Algorithm"),
  m_validationRunTreeFolder("/val/RunTreeTG"),
  m_validationRunTreeName("RunTreeTG"),
  m_validationRunTreeDescription("Run stats of the EnergyLossExtrapolationValidation Algorithm"),
  m_maximumR(0.),
  m_maximumZ(0.),
  m_cylinders(6),
  m_onion(true),
  m_momentum(10.*Gaudi::Units::GeV),
  m_usePt(true),
  m_minEta(-3.),
  m_maxEta(3.),
  m_events(0),
  m_totalRecordedLayers(0),
  m_avgRecordedLayers(0.),
  m_pdg(0),
  m_particleType(2),
  m_entries(0),
  m_energy{},         
	m_energyLoss{},     
	m_parameterX0{},    
	m_radius{},         
	m_positionX{},      
	m_positionY{},      
	m_positionZ{},      
	m_parameterPhi{},   
	m_parameterEta{},   
	m_parameterTheta{}, 
	m_parameterQoverP{},
	m_parameterP{},     
	m_layer{},          
  m_triesForward(0),
  m_breaksForward(0),
  m_triesBack(0),
  m_breaksBack(0),
  m_collectedLayerForward(0),
  m_collectedLayerBack(0),
  m_cylinderR{},
  m_cylinderZ{},
  m_theCylinders(nullptr),
  m_theDiscs1(nullptr),
  m_theDiscs2(nullptr)
{
    // used algorithms and alg tools
    declareProperty("Extrapolator"              , m_extrapolator);
    declareProperty("UseMaterialCollection"     , m_materialCollectionValidation);
    // TTree handling
    declareProperty("ValidationTreeName"        , m_validationTreeName);
    declareProperty("ValidationRunTreeName"     , m_validationRunTreeName);
    declareProperty("ValidationTreeDescription" , m_validationTreeDescription);
    declareProperty("ValidationRunTreeDescription" , m_validationRunTreeDescription);
    declareProperty("ValidationTreeFolder"      , m_validationTreeFolder);
    declareProperty("ValidationRunTreeFolder"   , m_validationRunTreeFolder);

    declareProperty("StartPerigeeMinEta"        , m_minEta);
    declareProperty("StartPerigeeMaxEta"        , m_maxEta);
    declareProperty("StartPerigeeUsePt"         , m_usePt);
    declareProperty("StartPerigeeMomentum"      , m_momentum);

    declareProperty("ValidationCylinders"       , m_cylinders);
    declareProperty("ValidationCylinderR"       , m_cylinderVR);
    declareProperty("ValidationCylinderZ"       , m_cylinderVZ);
    declareProperty("StrictOnionMode"           , m_onion);

    declareProperty("ParticleType"              , m_particleType);

}

//================ Destructor =================================================

Trk::EnergyLossExtrapolationValidation::~EnergyLossExtrapolationValidation()
{
  // clear random number generators
  delete m_gaussDist;
  delete m_flatDist;
  // clear data vectors
  delete m_theCylinders;
  delete m_theDiscs1;
  delete m_theDiscs2;

}


//================ Initialization =================================================

StatusCode Trk::EnergyLossExtrapolationValidation::initialize()
{
	// Code entered here will be executed once at program start.
	ATH_MSG_INFO( "initialize()" );
	ATH_MSG_DEBUG( "initialize() m_materialCollectionValidation = " << m_materialCollectionValidation );

	// Get Extrapolator from ToolService
	if (m_extrapolator.retrieve().isFailure()) {
		ATH_MSG_FATAL( "initialize() Could not retrieve Tool " << m_extrapolator << ". Exiting." );
		return StatusCode::FAILURE;
	}

	// create the new Trees
	m_validationTree = new TTree(m_validationTreeName.c_str(), m_validationTreeDescription.c_str());
	m_validationRunTree = new TTree(m_validationRunTreeName.c_str(), m_validationRunTreeDescription.c_str());

	// the branches for the parameters
	m_validationTree->Branch("Entries",    &m_entries,         "entries/i");
	m_validationTree->Branch("Energy",      m_energy,          "energy[entries]/F");
	m_validationTree->Branch("EnergyLoss",  m_energyLoss,      "eLoss[entries]/F");
	m_validationTree->Branch("tInX0",       m_parameterX0,     "tinX0[entries]/F");
	m_validationTree->Branch("Radius",      m_radius,          "radius[entries]/F");
	m_validationTree->Branch("PosX",        m_positionX,       "posX[entries]/F");
	m_validationTree->Branch("PosY",        m_positionY,       "posY[entries]/F");
	m_validationTree->Branch("PosZ",        m_positionZ,       "posZ[entries]/F");
	m_validationTree->Branch("Eta",         m_parameterEta,    "eta[entries]/F");
	m_validationTree->Branch("Phi",         m_parameterPhi,    "phi[entries]/F");
	m_validationTree->Branch("Layer",       m_layer,           "layer[entries]/i");

	m_validationRunTree->Branch("Layers",  &m_cylinders,       "layers/i");
	m_validationRunTree->Branch("CylR",     m_cylinderR,       "cylR[layers]/F");
	m_validationRunTree->Branch("CylZ",     m_cylinderZ,       "cylZ[layers]/F");
	m_validationRunTree->Branch("Momentum",&m_momentum,        "momentum/F");
	m_validationRunTree->Branch("UsePt",   &m_usePt,           "usePt/O");
	m_validationRunTree->Branch("MinEta",  &m_minEta,          "minEta/F");
	m_validationRunTree->Branch("MaxEta",  &m_maxEta,          "maxEta/F");
	m_validationRunTree->Branch("PDG",     &m_pdg,             "pdg/I");
  m_validationRunTree->Branch("Events",  &m_events,          "events/i");
  m_validationRunTree->Branch("AvgRecordedLayers", &m_avgRecordedLayers, "avgRecLayers/F");

	// now register the Trees
	ITHistSvc* tHistSvc = nullptr;
	if (service("THistSvc",tHistSvc).isFailure()){
		ATH_MSG_ERROR( "initialize() Could not find Hist Service -> Switching ValidationMode Off !" );
		delete m_validationTree; m_validationTree = nullptr;
		delete m_validationRunTree; m_validationRunTree = nullptr;
	}
	if ((tHistSvc->regTree(m_validationTreeFolder, m_validationTree)).isFailure()
			|| (tHistSvc->regTree(m_validationRunTreeFolder, m_validationRunTree)).isFailure() ) {
		ATH_MSG_ERROR( "initialize() Could not register the validation Trees -> Switching ValidationMode Off !" );
		delete m_validationTree; m_validationTree = nullptr;
		delete m_validationRunTree; m_validationRunTree = nullptr;
	}

	// initialize the random number generators
	ATH_MSG_INFO( "initialize() RandomService = " << randSvc()->name() );
	m_gaussDist = new Rndm::Numbers(randSvc(), Rndm::Gauss(0.,1.));
	m_flatDist  = new Rndm::Numbers(randSvc(), Rndm::Flat(0.,1.));

	// initialize cylinders if they are not set in jobOptions
	double const s_cylInitR[TRKEXALGS_MAXPARAMETERS] = { 0.5, 34.5,  250,  550, 1120, 4250, 13000, 0, 0, 0 };
	double const s_cylInitZ[TRKEXALGS_MAXPARAMETERS] = { 100, 10e6,  680, 2820, 3120, 6500, 22000, 0, 0, 0 };

	// output of vector from jobOptions
    ATH_MSG_INFO( "initialize() cylinder dimensions vector from jobOptions :" );
    for (size_t lay=0; lay<m_cylinders+1; ++lay) {
        ATH_MSG_INFO( "initialize() m_cylinderVR[" << lay << "] = " << m_cylinderVR[lay] << "\t ... m_cylinderVZ[" << lay << "] = " << m_cylinderVZ[lay] );
    }
	// transform vector (from jobOptions) into array (for ROOT tree)
    ATH_MSG_INFO( "initialize() cylinder dimensions array for algorithm and ROOT tree :" );
    for (size_t lay=0; lay<m_cylinders+1; ++lay) {
        m_cylinderR[lay] = m_cylinderVR[lay] > 0 ? m_cylinderVR[lay] : s_cylInitR[lay];
        m_cylinderZ[lay] = m_cylinderVZ[lay] > 0 ? m_cylinderVZ[lay] : s_cylInitZ[lay];
		    // in "strict onion mode", constrain m_cylinders if the values don't make sense
		    /** sroe; original line was:
		        if (m_onion && lay>0 && (m_cylinderR[lay] < m_cylinderR[lay-1] || m_cylinderR[lay] < m_cylinderR[lay-1])) {
        but the two sides of the 'or' are equal
        **/
        if (m_onion && lay>0 && (m_cylinderR[lay] < m_cylinderR[lay-1])) {
            ATH_MSG_WARNING( "initialize() layer " << lay << "dimensions are smaller than those of layer " << lay-1 << " - constraining m_cylinders to " << lay-1 );
            ATH_MSG_INFO( "initialize() cutting off here :" );
            ATH_MSG_INFO( "initialize() m_cylinderR[" << lay << "] = " << m_cylinderR[lay] << "\t ... m_cylinderZ[" << lay << "] = " << m_cylinderZ[lay] );
            m_cylinders = lay-1;
            break;
        }
		    ATH_MSG_INFO( "initialize() m_cylinderR[" << lay << "] = " << m_cylinderR[lay] << "\t ... m_cylinderZ[" << lay << "] = " << m_cylinderZ[lay] );
    }

    // fill data vector with cylinders once (in order not to create them every time)
    m_theCylinders = new DataVector<const Trk::CylinderSurface>();
    m_theDiscs1 = new DataVector<const Trk::DiscSurface>();
    m_theDiscs2 = new DataVector<const Trk::DiscSurface>();
    for (size_t lay=0; lay<m_cylinders+1; ++lay) {
    	m_theCylinders->push_back(new Trk::CylinderSurface(Amg::Transform3D(), m_cylinderR[lay], m_cylinderZ[lay]));
		  ATH_MSG_INFO( "initialize() Cylinder " << lay << ": " << *m_theCylinders->at(lay) );
      m_theDiscs1->push_back(new Trk::DiscSurface(*createTransform(0.,0.,-m_cylinderZ[lay]), 0., m_cylinderR[lay]));
      ATH_MSG_INFO( "initialize() Disc1 " << lay << ": " << *m_theDiscs1->at(lay) );
      m_theDiscs2->push_back(new Trk::DiscSurface(*createTransform(0.,0., m_cylinderZ[lay]), 0., m_cylinderR[lay]));
      ATH_MSG_INFO( "initialize() Disc2 " << lay << ": " << *m_theDiscs2->at(lay) );
    }

    if      (m_particleType==0) m_pdg = 999; // geantino
    else if (m_particleType==1) m_pdg =  11; // electron
    else if (m_particleType==2) m_pdg =  13; // muon-
    else if (m_particleType==3) m_pdg = 211; // pion+
    else if (m_particleType==4) m_pdg = 321; // kaon+
    ATH_MSG_INFO( "initialize() ParticleType = " << m_particleType << " ... PDG = " << m_pdg );

	ATH_MSG_INFO( "initialize() successful" );
	return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode Trk::EnergyLossExtrapolationValidation::finalize()
{
	// Code entered here will be executed once at the end of the program run.
	ATH_MSG_INFO( "finalize() ================== Output Statistics =========================" );
	ATH_MSG_INFO( "finalize() = Navigation : " );
	ATH_MSG_INFO( "finalize() =  - breaks fwd : " << double(m_breaksForward)/double(m_triesForward)
			<< " (" << m_breaksForward << "/" << m_triesForward << ")" );
	ATH_MSG_INFO( "finalize() =  - breaks bwd : " << double(m_breaksBack)/double(m_triesBack)
			<< " (" << m_breaksBack << "/" << m_triesBack << ")" );
	if (m_materialCollectionValidation){
		ATH_MSG_INFO( "finalize() = Material collection : " );
		ATH_MSG_INFO( "finalize() =  - layer collected fwd : " << m_collectedLayerForward );
		ATH_MSG_INFO( "finalize() =  - layer collected bwd : " << m_collectedLayerBack  );
	}
	ATH_MSG_INFO( "finalize() ==============================================================" );

    m_avgRecordedLayers = m_events ? (float)m_totalRecordedLayers / (float)m_events : 0;
    ++m_cylinders;
	if (m_validationRunTree)
		m_validationRunTree->Fill();
	--m_cylinders;

	return StatusCode::SUCCESS;
}

//================ Execution ====================================================

StatusCode Trk::EnergyLossExtrapolationValidation::execute()
{

    // get the overall dimensions
    if (!m_highestVolume){
        // get TrackingGeometry and highest volume
        const Trk::TrackingGeometry* trackingGeometry = m_extrapolator->trackingGeometry();
        m_highestVolume = trackingGeometry ? trackingGeometry->highestTrackingVolume() : nullptr;
        const Trk::CylinderVolumeBounds* cylBounds = m_highestVolume ?
                dynamic_cast<const Trk::CylinderVolumeBounds*>(&(m_highestVolume->volumeBounds())) : nullptr;
        // bail out
        if (!cylBounds){
            ATH_MSG_WARNING( "execute() No highest TrackingVolume / no VolumeBounds ... pretty useless!" );
            return StatusCode::SUCCESS;
        }
        // get the numbers
        m_maximumR = cylBounds->outerRadius();
        m_maximumZ = cylBounds->halflengthZ();
    }


    m_entries      = 0;
    for (size_t par=0; par<TRKEXALGS_MAXPARAMETERS; ++par) {
        // -----------> start parameters

        m_parameterP[par]        = 0.;
        m_energy[par]            = 0.;
        m_energyLoss[par]        = 0.;
        m_parameterX0[par]       = 0.;
        m_radius[par]            = 0.;
        m_positionX[par]         = 0.;
        m_positionY[par]         = 0.;
        m_positionZ[par]         = 0.;
        m_parameterEta[par]      = 0.;
        m_parameterPhi[par]      = 0.;
        m_parameterTheta[par]    = 0.;
//        m_parameterQoverP[par]   = 0.;
        m_layer[par]             = 0;
    }

    // the local start parameters
    // are adopted for planar and straight line surfaces
    m_parameterPhi[0]   = M_PI * (2 * m_flatDist->shoot() - 1);
    m_parameterEta[0]   = m_minEta + m_flatDist->shoot()*(m_maxEta-m_minEta);
    m_parameterTheta[0] = 2.*atan(exp(-m_parameterEta[0]));

    double charge = -1.;
    m_parameterP[0] = m_momentum;
    // convert transverse momentum (pt) to momentum (p) if flag is set: p = pt/sin(theta)
    if (m_usePt)
        m_parameterP[0] /= sin(m_parameterTheta[0]);
    m_parameterQoverP[0] = charge/m_parameterP[0];

    double mass = Trk::ParticleMasses().mass[m_particleType];

    double energy1 = sqrt(m_parameterP[0]*m_parameterP[0] + mass*mass);

    double newX0 = 0;

    const Trk::PerigeeSurface perigeeSurface;
    // the initial perigee with random numbers
    Trk::Perigee startParameters(0, // m_parameterLoc1[0],
                                 0, // m_parameterLoc2[0],
                                 m_parameterPhi[0],
                                 m_parameterTheta[0],
                                 m_parameterQoverP[0],
				 perigeeSurface);

    ATH_MSG_VERBOSE( "execute() Start Parameters : " << startParameters );
    ATH_MSG_DEBUG( "execute() Start Parameters : [phi,eta] = [ " << startParameters.momentum().phi() << ", " << startParameters.eta() << " ]" );

    // --------------- propagate to find an intersection ---------------------

	// fill the TrackParameters vector with extrapolation from startParameters to dummy cylinder surface
    const Trk::TrackParameters* lastParameters = nullptr;
    const Trk::TrackParameters* newParameters = nullptr;

    if (!m_materialCollectionValidation) {

        lastParameters = m_extrapolator->extrapolate(
                startParameters,
                *(m_theCylinders->at(0)),
                Trk::alongMomentum,
                true,
                (Trk::ParticleHypothesis)m_particleType);

    } else { // material collection validation

        // get the vector of TrackStateOnSurfaces back
        const std::vector<const Trk::TrackStateOnSurface*>* collectedMaterial =
                              m_extrapolator->extrapolateM(startParameters,
                                                           *(m_theCylinders->at(0)),
                                                           Trk::alongMomentum,
                                                           true,
                                                           (Trk::ParticleHypothesis)m_particleType);

        // get the last one and clone it
        if (collectedMaterial && !collectedMaterial->empty()) {
            // get the last track state on surface & clone the destination parameters
            const Trk::TrackStateOnSurface* destinationState = collectedMaterial->back();
            lastParameters = destinationState->trackParameters() ? destinationState->trackParameters()->clone() : nullptr;
            m_collectedLayerForward += collectedMaterial->size();
            // delete the layers / cleanup
            std::vector<const Trk::TrackStateOnSurface*>::const_iterator tsosIter    =  collectedMaterial->begin();
            std::vector<const Trk::TrackStateOnSurface*>::const_iterator tsosIterEnd =  collectedMaterial->end();
            for ( ; tsosIter != tsosIterEnd; ++tsosIter) {
                newX0 += (*tsosIter)->materialEffectsOnTrack() ? (*tsosIter)->materialEffectsOnTrack()->thicknessInX0() : 0;
                delete(*tsosIter);
            }
            ATH_MSG_VERBOSE( "execute() newX0 = " << newX0 );
        }
    }


    for (size_t lay = 1; lay<m_cylinders+1; ++lay) {

        if (!m_onion) newX0 = 0;

        if (m_onion) {
            // safety check
            if (!lastParameters) {
                ATH_MSG_WARNING( "execute() Layer " << lay << ": start parameters for cylinder NOT found - skip event !" );
                break;
            }
            ATH_MSG_VERBOSE( "execute() Layer " << lay << ": start parameters for cylinder found: " << *lastParameters );
        }

        // trying to extrapolate to cylinder barrel
        newParameters = nullptr;
        if (!m_materialCollectionValidation) {

            newParameters = m_extrapolator->extrapolate(
                    m_onion ? *lastParameters : startParameters,
                    *(m_theCylinders->at(lay)),
                    Trk::alongMomentum,
                    true,
                    (Trk::ParticleHypothesis)m_particleType);

        } else { // material collection validation

            // get the vector of TrackStateOnSurfaces back
            const std::vector<const Trk::TrackStateOnSurface*>* collectedMaterial =
                                  m_extrapolator->extrapolateM(m_onion ? *lastParameters : startParameters,
                                                               *(m_theCylinders->at(lay)),
                                                               Trk::alongMomentum,
                                                               true,
                                                               (Trk::ParticleHypothesis)m_particleType);

            // get the last one and clone it
            if (collectedMaterial && !collectedMaterial->empty()){
                // get the last track state on surface & clone the destination parameters
                const Trk::TrackStateOnSurface* destinationState = collectedMaterial->back();
                newParameters = destinationState->trackParameters() ? destinationState->trackParameters()->clone() : nullptr;
                if (m_onion)
                    m_collectedLayerForward += collectedMaterial->size();
                else
                    m_collectedLayerForward  = collectedMaterial->size(); // TODO: shouldn't there be something else here?
                // delete the layers / cleanup
                std::vector<const Trk::TrackStateOnSurface*>::const_iterator tsosIter    =  collectedMaterial->begin();
                std::vector<const Trk::TrackStateOnSurface*>::const_iterator tsosIterEnd =  collectedMaterial->end();
                for ( ; tsosIter != tsosIterEnd; ++tsosIter) {
                    newX0 += (*tsosIter)->materialEffectsOnTrack() ? (*tsosIter)->materialEffectsOnTrack()->thicknessInX0() : 0;
                    delete(*tsosIter);
                }
                ATH_MSG_VERBOSE( "execute() newX0 = " << newX0 );
            }
        }

            // no intersection with cylinder barrel, now trying disc endcaps
        if (!newParameters) {

            if (!m_materialCollectionValidation) {

                newParameters = m_extrapolator->extrapolate(m_onion ? *lastParameters : startParameters,
                                                            (m_parameterEta[0] < 0) ? *(m_theDiscs1->at(lay)) : *(m_theDiscs2->at(lay)),
                                                            Trk::alongMomentum,
                                                            true,
                                                            (Trk::ParticleHypothesis)m_particleType);

            } else { // material collection validation

                // get the vector of TrackStateOnSurfaces back
                const std::vector<const Trk::TrackStateOnSurface*>* collectedMaterial =
                                      m_extrapolator->extrapolateM(m_onion ? *lastParameters : startParameters,
                                                                   (m_parameterEta[0] < 0) ? *(m_theDiscs1->at(lay)) : *(m_theDiscs2->at(lay)),
                                                                   Trk::alongMomentum,
                                                                   true,
                                                                   (Trk::ParticleHypothesis)m_particleType);

                // get the last one and clone it
                if (collectedMaterial && !collectedMaterial->empty()){
                    // get the last track state on surface & clone the destination parameters
                    const Trk::TrackStateOnSurface* destinationState = collectedMaterial->back();
                    newParameters = destinationState->trackParameters() ? destinationState->trackParameters()->clone() : nullptr;
                    if (m_onion)
                        m_collectedLayerForward += collectedMaterial->size();
                    else
                        m_collectedLayerForward  = collectedMaterial->size(); // TODO: shouldn't there be something else here?
                    // delete the layers / cleanup
                    std::vector<const Trk::TrackStateOnSurface*>::const_iterator tsosIter    =  collectedMaterial->begin();
                    std::vector<const Trk::TrackStateOnSurface*>::const_iterator tsosIterEnd =  collectedMaterial->end();
                    for ( ; tsosIter != tsosIterEnd; ++tsosIter) {
                        newX0 += (*tsosIter)->materialEffectsOnTrack() ? (*tsosIter)->materialEffectsOnTrack()->thicknessInX0() : 0;
                        delete(*tsosIter);
                    }
                    ATH_MSG_VERBOSE( "execute() newX0 = " << newX0 );
                }
            }
        }

        // still no intersection
        if (!newParameters) {
            ATH_MSG_WARNING( "execute() Layer " << lay << " intersection did not work !" );
        }

		else if (m_highestVolume && newParameters && !(m_highestVolume->inside(newParameters->position()))) {
		    ATH_MSG_WARNING( "execute() Layer " << lay << " intersection is outside the known world !" );
		}

		else {

		    // get the current surface intersection position
		    const Amg::Vector3D& newPosition = newParameters->position();
            ATH_MSG_VERBOSE( "execute() Track Parameters at layer " << lay << ": " << *newParameters );
            ATH_MSG_DEBUG( "execute() Track Parameters at layer " << lay << ": [r,z] = [ " << newPosition.perp() << ", " << newPosition.z() );

		    // record the surface parameters
		    ++m_triesForward;
		    ++m_entries;
		    m_parameterPhi[m_entries]    = newParameters->parameters()[Trk::phi];
		    m_parameterEta[m_entries]    = newParameters->momentum().eta();
		    m_parameterTheta[m_entries]  = newParameters->parameters()[Trk::theta];
		    m_parameterP[m_entries]      = newParameters->momentum().mag();
		    m_parameterX0[m_entries]     = (float)newX0;
            ATH_MSG_DEBUG( "execute() Layer " << lay << ": cumulated X0 = " << m_parameterX0[m_entries] );

		    // get the current energy and calculate energy loss
		    m_energy[m_entries] = sqrt(m_parameterP[m_entries]*m_parameterP[m_entries] + mass*mass);
		    m_energyLoss[m_entries]      = energy1-m_energy[m_entries];
		    ATH_MSG_DEBUG( "execute() Layer " << lay << ": cumulated Energy Loss = " << m_energyLoss[m_entries] );

		    // record the current layer ID
		    m_layer[m_entries]           = lay;
		    // record the current position
		    m_radius[m_entries]          = newPosition.perp();
		    m_positionX[m_entries]       = newPosition.x();
		    m_positionY[m_entries]       = newPosition.y();
		    m_positionZ[m_entries]       = newPosition.z();

		}

        lastParameters = newParameters;

	}


    m_totalRecordedLayers += m_entries;
    ++m_events;
	// increase m_entries once more before the fill (to account for the "start layer" at index 0 with initial track parameters)
	++m_entries;

	// fill the event tree
	if (m_validationTree)
	    m_validationTree->Fill();

	// memory cleanup
	ATH_MSG_DEBUG( "execute() deleting DataVector parameters ... " );

	return StatusCode::SUCCESS;
}

//============================================================================================
std::unique_ptr<Amg::Transform3D> Trk::EnergyLossExtrapolationValidation::createTransform(
        double x, double y, double z, double phi, double theta, double alphaZ)
{

    if (phi!=0. && theta != 0.){
        // create the Start Surface
        Amg::Vector3D surfacePosition(x,y,z);
        // z direction
        Amg::Vector3D surfaceZdirection(cos(phi)*sin(theta),
                sin(phi)*sin(theta),
                cos(theta));
        // the global z axis
        Amg::Vector3D zAxis(0.,0.,1.);
        // the y direction
        Amg::Vector3D surfaceYdirection(zAxis.cross(surfaceZdirection));
        // the x direction
        Amg::Vector3D surfaceXdirection(surfaceYdirection.cross(surfaceZdirection));
        // the rotation
        Amg::RotationMatrix3D surfaceRotation;
        surfaceRotation.col(0) = surfaceXdirection;
        surfaceRotation.col(1) = surfaceYdirection;
        surfaceRotation.col(2) = surfaceZdirection;
        // return it
        if (alphaZ==0.){
            return std::make_unique<Amg::Transform3D>(surfaceRotation, surfacePosition);
        }
        Amg::Transform3D nominalTransform(surfaceRotation, surfacePosition);
        return std::make_unique<Amg::Transform3D>(nominalTransform*Amg::AngleAxis3D(alphaZ,zAxis));

    }

    return std::make_unique<Amg::Transform3D>(Amg::Translation3D(x,y,z));
}

