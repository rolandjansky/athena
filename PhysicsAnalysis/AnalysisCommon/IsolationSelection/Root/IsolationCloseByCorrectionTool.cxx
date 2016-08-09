/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <IsolationSelection/IsolationCloseByCorrectionTool.h>

#include <xAODPrimitives/IsolationHelpers.h>
#include <xAODPrimitives/tools/getIsolationAccessor.h>
#include <xAODPrimitives/tools/getIsolationCorrectionAccessor.h>

#include "IsolationSelection/IsolationSelectionTool.h"

#include <InDetTrackSelectionTool/IInDetTrackSelectionTool.h>

#include "xAODEgamma/Egamma.h"

namespace CP {

    IsolationCloseByCorrectionTool::IsolationCloseByCorrectionTool(const std::string& toolName):
  	asg::AsgTool(toolName),
// 	m_tracksInConeTool("xAOD::TrackParticlesInConeTool/TrackParticlesInConeTool"), // TO BE ADDED AT SOME POINT!!!
	m_selectorTool(""), 
	m_isInitialised(false), m_isCoreSubtracted(false),
	m_trkselTool("InDet::InDetTrackSelectionTool/IsolationCloseByCorrectionTool_isoTrackSelTool")
	{ 	    
	    //IMPORTANT USER PROPERTIES
		declareProperty("IsolationSelectionTool", m_selectorTool, "Please give me your configured IsolationSelectionTool!");
		//declareProperty("TracksInConeTool", m_tracksInConeTool, "Please give me your configured TrackParticlesInConeTool"); // TO BE ADDED AT SOME POINT!!!

		//OPTIONAL PROPERTIES
		m_trkselTool.declarePropertyFor(this, "TrackSelectionTool"); // Makes the track selection tool a settable property of this tool

		//EXPERT PROPERTIES
		declareProperty("TrackParticleLocation", m_indetTrackParticleLocation = "InDetTrackParticles", "This is the location of the Inner Detector track particle (for the container)."); 
		declareProperty("CoreCone", m_coreCone = 0.1, "This is the size of the core cone for the topoetcone variables.");
		declareProperty("PtvarconeRadius", m_ptvarconeRadius = 10000.0, "This is the kT parameter for the ptvarcone variables.");

	}

    StatusCode IsolationCloseByCorrectionTool::initialize() {
    
// 	    if(m_tracksInConeTool.retrieve().isFailure()) { // TO BE ADDED AT SOME POINT!!!
// 	        ATH_MSG_FATAL("Could not retrieve TrackParticlesInConeTool.");
// 			return StatusCode::FAILURE;
// 		}
        

      //set default properties of track selection tool, if the user hasn't configured it
      if(!m_trkselTool.isUserConfigured()) {
	ATH_MSG_INFO("No TrackSelectionTool provided, so I will create and configure my own, called: " << m_trkselTool.name());
            if(m_trkselTool.setProperty("maxZ0SinTheta", 3.).isFailure()) {
                ATH_MSG_FATAL("Could not set maxZ0SinTheta.");
                return StatusCode::FAILURE;
            }
            if(m_trkselTool.setProperty("minPt", 1000.).isFailure()) {
                ATH_MSG_FATAL("Could not set minPt.");
                return StatusCode::FAILURE;
            }
            if(m_trkselTool.setProperty("CutLevel", "Loose").isFailure()) {
                ATH_MSG_FATAL("Could not set cut level.");
                return StatusCode::FAILURE;
            }
      }
      
      //now retrieve the tool, in all cases
      if(m_trkselTool.retrieve().isFailure()){
	            ATH_MSG_FATAL("Could not initialize/retrieve InDetTrackSelectionTool");    
	            return StatusCode::FAILURE;
      }
        
        m_muon_isoTypesSet = false;
        m_electron_isoTypesSet = false;
        m_photon_isoTypesSet = false;
        
        m_muon_isoTypes.clear();
        m_electron_isoTypes.clear();
        m_photon_isoTypes.clear();
        
        if(!m_selectorTool.empty()) {
	    
            ATH_CHECK(m_selectorTool.retrieve());
	   
            CP::IsolationSelectionTool* theSelectionTool = dynamic_cast<CP::IsolationSelectionTool*>(&*m_selectorTool);

            for(auto x: theSelectionTool->getMuonWPs()) {
                ATH_CHECK(theSelectionTool->addWP(x, xAOD::Type::Other));
                ATH_MSG_INFO("adding WP " << x->name());
            }
            for(auto x: theSelectionTool->getElectronWPs()) {
                ATH_CHECK(theSelectionTool->addWP(x, xAOD::Type::Other));
                ATH_MSG_INFO("adding WP " << x->name());
            }
            for(auto x: theSelectionTool->getPhotonWPs()) {
                ATH_CHECK(theSelectionTool->addWP(x, xAOD::Type::Other));
                ATH_MSG_INFO("adding WP " << x->name());
            }
		    
        }
        
        m_isInitialised = true;
         
		return StatusCode::SUCCESS;
	}

	StatusCode IsolationCloseByCorrectionTool::finalize() {
		return StatusCode::SUCCESS;
	}
	
	const xAOD::IParticle* IsolationCloseByCorrectionTool::getReferenceParticle(const xAOD::IParticle& particle) const {
        const xAOD::TrackParticle* tp = dynamic_cast<const xAOD::TrackParticle*>(&particle);
        if( tp ) return tp;
        const xAOD::Muon* muon = dynamic_cast<const xAOD::Muon*>(&particle);
        if( muon ) {
            const xAOD::TrackParticle* tp = 0;
            if(muon->inDetTrackParticleLink().isValid()) tp = *muon->inDetTrackParticleLink();
            if( !tp ) tp = muon->primaryTrackParticle();
            if( !tp ) {
                ATH_MSG_WARNING(" No TrackParticle found for muon " );
                return 0;
            }
            return tp;
        }
        return &particle;
    }

	CP::CorrectionCode IsolationCloseByCorrectionTool::getCloseByCorrection(std::vector<float>& corrections, const xAOD::IParticle& par, const std::vector<xAOD::Iso::IsolationType>& types, const std::vector<const xAOD::IParticle*>& closePar, int topoetconeModel) const {

        if(!m_isInitialised) {
	        ATH_MSG_WARNING("The IsolationCloseByCorrectionTool was not initialised!!!");
	    }
	    
		corrections.clear();
		CP::CorrectionCode Error = CP::CorrectionCode::Error;

		for(unsigned int i = 0; i < types.size(); i++){
		
		    xAOD::Iso::IsolationType isoType = types.at(i);

			float correction = 0.0;
			
	        bool typeFound = false;
			if((isoType == xAOD::Iso::IsolationType::topoetcone20) || (isoType == xAOD::Iso::IsolationType::topoetcone30) || (isoType == xAOD::Iso::IsolationType::topoetcone40)) {
				if(getCloseByCorrectionTopoetcone(correction, par, isoType, closePar, topoetconeModel) == Error) {
					return Error;
				} else {
				    typeFound = true;
				}
			}
				
			if((isoType == xAOD::Iso::IsolationType::ptcone20) || (isoType == xAOD::Iso::IsolationType::ptcone30) || (isoType == xAOD::Iso::IsolationType::ptcone40)) {
				if(getCloseByCorrectionTrackIso(correction, par, isoType, closePar) == Error) {
					return Error;
				} else {
				    typeFound = true;
				}
			}
			if((isoType == xAOD::Iso::IsolationType::ptvarcone20) || (isoType == xAOD::Iso::IsolationType::ptvarcone30) || (isoType == xAOD::Iso::IsolationType::ptvarcone40)) {
				if(getCloseByCorrectionTrackIso(correction, par, isoType, closePar) == Error) {
					return Error;
				} else {
				    typeFound = true;
				}
			}
			
			if(!typeFound) {
				ATH_MSG_WARNING("The isolation type was " << isoType << " was not recognized.");
				return Error;	
			}
	
			corrections.push_back(correction);

		}

		return CP::CorrectionCode::Ok;

	}

    CP::CorrectionCode IsolationCloseByCorrectionTool::getCloseByCorrectionTrackIso(float& correction, const xAOD::IParticle& par, xAOD::Iso::IsolationType type, const std::vector<const xAOD::IParticle*>& closePar) const {
        
        if(!m_isInitialised) {
	        ATH_MSG_WARNING("The IsolationCloseByCorrectionTool was not initialised!!!");
	    }
	    
	    float coneSize = xAOD::Iso::coneSize(type);
	    xAOD::Iso::IsolationFlavour flavour = xAOD::Iso::isolationFlavour(type);
	    if(flavour == xAOD::Iso::IsolationFlavour::ptvarcone) {
	        coneSize = getMin(coneSize, m_ptvarconeRadius / par.pt());
	    }
	    
	    const xAOD::Vertex *vertex = retrieveIDBestPrimaryVertex();
        if(!vertex) {
            ATH_MSG_WARNING("Could not find the vertex. Aborting.");
		    return CP::CorrectionCode::Error;
        }
	    
        //// If the track isolation is already nil, no need to perform overlap removal.
        auto accIso = xAOD::getIsolationAccessor(type);
        float isoVal = 999999;
        if(!accIso->isAvailable(par)) {
            ATH_MSG_WARNING("Could not retrieve the isolation variable.");
            return CP::CorrectionCode::Error;
        } else {
            isoVal = (*accIso)(par);
            if(isoVal <= 0) {
                correction = 0.0;
                return CP::CorrectionCode::Ok;
            }
        }

		const xAOD::IParticle *tp = getReferenceParticle(par);
		if(!tp) {
		    ATH_MSG_WARNING("No track particle available. Aborting.");
		    return CP::CorrectionCode::Error;
		}
		std::vector<const xAOD::TrackParticle*> tps;

//  		if( !m_tracksInConeTool->particlesInCone(tp->eta(), tp->phi(), coneSize, tps) ) { return CP::CorrectionCode::Error; } // TO BE ADDED AT SOME POINT!!!

		if( !getparticlesInCone(tp->eta(), tp->phi(), coneSize, tps) ) { return CP::CorrectionCode::Error; } // TO BE REOMVED AT SOME POINT!!!

		correction = 0.0;

		for( unsigned int i = 0; i < closePar.size(); i++ ) {
			const xAOD::IParticle* tp2 = getReferenceParticle(*closePar.at(i));
			if(tp2) {
                if(!(tp == tp2)) {
                    for( unsigned int j = 0; j < tps.size(); j++ ) {
                        const xAOD::TrackParticle *tpTemp = tps.at(j);
                        if(tpTemp == tp2) {
                            float deta = tp->eta() - tpTemp->eta();
                            float dphi = phiInRange(tp->phi() - tpTemp->phi());
                            float dr2 = deta * deta + dphi * dphi;
                            
                            bool considerTrack = !m_isCoreSubtracted;
                                
                            if(considerTrack) {

                                ATH_MSG_DEBUG("Cone size: " << coneSize);
                                    
                                if( ! m_trkselTool->accept( *tpTemp, vertex ) ){
                                    ATH_MSG_DEBUG("reject track pt = " << tpTemp->pt());
                                } else {
                                    ATH_MSG_DEBUG("Accept track, pt = " << tpTemp->pt());
                                    float coreToBeRemoved = 0.0;
                                    auto acc = xAOD::getIsolationCorrectionAccessor(xAOD::Iso::IsolationFlavour::ptcone, xAOD::Iso::IsolationTrackCorrection::coreTrackPtr);
                                    if(!acc->isAvailable(*closePar.at(i))) {
                                        ATH_MSG_WARNING("Could not retrieve ptcore. Will remove the pT of the track instead.");
                                        coreToBeRemoved = tpTemp->pt();
                                    } else {
                                        coreToBeRemoved = (*acc)(*closePar.at(i));
                                    }
                                    correction = correction + coreToBeRemoved;
                                    ATH_MSG_DEBUG("Track " << i << " pT: " << tpTemp->pt());
                                    ATH_MSG_DEBUG("Track " << i << " eta: " << tpTemp->eta());
                                    ATH_MSG_DEBUG("Track " << i << " phi: " << tpTemp->phi());
                                    ATH_MSG_DEBUG("Track " << i << " charge: " << tpTemp->charge());
                                    ATH_MSG_DEBUG("Track " << i << " type: " << tpTemp->type());
                                    ATH_MSG_DEBUG("Track " << i << " delta eta: " << deta);
                                    ATH_MSG_DEBUG("Track " << i << " delta phi: " << dphi);
                                    ATH_MSG_DEBUG("Track " << i << " delta R: " << sqrt(dr2));
                                }
                            }
                        }
                    }
                }
            }	
		}

		return CP::CorrectionCode::Ok;
		
	}

	CP::CorrectionCode IsolationCloseByCorrectionTool::getCloseByCorrectionTopoetcone(float& correction, const xAOD::IParticle& par, xAOD::Iso::IsolationType type, const std::vector<const xAOD::IParticle*>& closePar, int topoetconeModel) const {
		
		float coneSize = xAOD::Iso::coneSize(type);
		
		if(!m_isInitialised) {
	        ATH_MSG_WARNING("The IsolationCloseByCorrectionTool was not initialised!!!");
	    }
	    
		// If the track isolation is already nil or negative, no need to perform overlap removal.
        auto accIso = xAOD::getIsolationAccessor(type);
        if(!accIso->isAvailable(par)) {
            ATH_MSG_WARNING("Could not retrieve the isolation variable.");
            return CP::CorrectionCode::Error;
        } else {
            float isoVal = (*accIso)(par);
            if(isoVal <= 0) {
                correction = 0.0;
                return CP::CorrectionCode::Ok;
            }
        }
		
		// Retrieves the averaged eta and phi of the particle.
        float par1_eta = par.eta();
        float par1_phi = par.phi();
        getExtrapEtaPhi(par, par1_eta, par1_phi).ignore();

		correction = 0.0;
		
		// Retrieves the topocluster container if not provided and if requested (topoetconeModel == -1).
		std::vector<const xAOD::CaloCluster*> clusters;
		const xAOD::CaloClusterContainer* topoClusters = 0;
		if(topoetconeModel == -1) {
		    bool foundContainer = false;
		    if(!topoClusters) {
		        if (!evtStore()->retrieve(topoClusters, "CaloCalTopoClusters").isSuccess()) {
		            ATH_MSG_WARNING("Could not retrieve the topocluster container. Will use a simple model." );
		            topoetconeModel = 0;
		        } else {
		            foundContainer = true;
		        }
		    } else {
		        foundContainer = true;
		    }
		    
		    // Only keeps the potential topocluster candidates for removal in a vector.
		    if(foundContainer) {
                for (auto cluster : *topoClusters) {
                    float etaClu = cluster->eta();
                    float phiClu = cluster->phi();
                    float deltaEtaClu = par1_eta - etaClu;
                    float deltaPhiClu = phiInRange(par1_phi - phiClu);
                    float drClu = sqrt(deltaEtaClu * deltaEtaClu + deltaPhiClu * deltaPhiClu);
                    if((drClu > m_coreCone) && (drClu <= coneSize)) {
                        clusters.push_back(cluster);
                    }
                }
            }
		}

		for( unsigned int i = 0; i < closePar.size(); i++ ) {
		    
		    // Retrieves the averaged eta and phi of the particle.
		    const xAOD::IParticle* par2 = closePar.at(i);
		    float par2_eta = par2->eta();
		    float par2_phi = par2->phi();
		    getExtrapEtaPhi(*par2, par2_eta, par2_phi).ignore();
		    float deta = par1_eta - par2_eta;
		    float dphi = phiInRange(par1_phi - par2_phi);
		    float dr = sqrt(deta * deta + dphi * dphi);
		    
            ATH_MSG_DEBUG("Cone size: " << coneSize);
            ATH_MSG_DEBUG("Particle " << i << " pT: " << par2->pt());
            ATH_MSG_DEBUG("Particle " << i << " eta: " << par2_eta);
            ATH_MSG_DEBUG("Particle " << i << " phi: " << par2_phi);
            ATH_MSG_DEBUG("Particle " << i << " type: " << par2->type());
        
            // If requested by the model, removes the core topoclusters.
            if(topoetconeModel == -1) {
            
                std::vector<const xAOD::CaloCluster*> clustersTemp;
        
                for ( unsigned int j = 0; j < clusters.size(); j++ ) {
                
                    const xAOD::CaloCluster *cluster = clusters.at(j);
                    float etaClu = cluster->eta();
                    float phiClu = cluster->phi();
                    float deltaEtaClu = par2_eta - etaClu;
                    float deltaPhiClu = phiInRange(par2_phi - phiClu);
                    float drClu = sqrt(deltaEtaClu * deltaEtaClu + deltaPhiClu * deltaPhiClu);
                    bool hasToBeRemoved = (drClu <= m_coreCone) && (fabs(cluster->eta()) <= 7.0) 
                        && (cluster->pt() > 0.001);
                    if(hasToBeRemoved) {
                        // Uncalibrated energy of the topocluster.
                        float etUncal = cluster->p4(xAOD::CaloCluster::State::UNCALIBRATED).Et();
                        // Correction to the energy.
                        float st = 1. / TMath::CosH(cluster->p4(xAOD::CaloCluster::State::UNCALIBRATED).Eta());
                        float tilegap3_et = cluster->eSample(CaloSampling::TileGap3) * st;
                        etUncal = etUncal - tilegap3_et;
                        // Only positive energy topoclusters are removed. 
                        // The others can be removed from the vector (they won't be used).
                        if(etUncal > 0) {
                            correction = correction + etUncal;
                        }
                    } else {
                        // Only keeps the clusters that were not already subtracted.
                        clustersTemp.push_back(cluster);
                    }
                }
            
                clusters = clustersTemp;
        
            } else {
        
                float fraction = 0;
            
                ATH_MSG_DEBUG("Particle " << i << " delta eta: " << deta);
                ATH_MSG_DEBUG("Particle " << i << " delta phi: " << dphi);
                ATH_MSG_DEBUG("Particle " << i << " delta R: " << dr);
        
                if(topoetconeModel == 0) {
                    if(coneSize <= m_coreCone) {
                        fraction = 0.0;
                    } else {
                        if(dr <= coneSize) {
                            fraction = 1.0;
                        }
                    }
                }
        
                if(topoetconeModel > 0) {
                    if(coneSize <= m_coreCone) {
                        fraction = 0.0;
                    } else {
                        if(topoetconeModel == 1) {
                            fraction = 0.0;
                            if((dr > 2 * m_coreCone) && (dr <= (coneSize - m_coreCone))) {
                                fraction = 1.0;
                            }
                        }
                
                        if(topoetconeModel == 2) {
                            float drA = getMin(2 * m_coreCone, coneSize - m_coreCone);
                            fraction = getMin(dr / m_coreCone, 1);
                            if(dr > drA) {
                                float drB = getMax(2 * m_coreCone, coneSize - m_coreCone);
                                fraction = getMin(drA / m_coreCone, 1);
                                if(dr > drB) {
                                    float drC = coneSize + m_coreCone;
                                    fraction = fraction * (drC - dr) / (drC - drB);
                                }
                            }
                        }
                    }
                }
        
                if(fraction < 0) {
                    fraction = 0.0;
                }
        
                if(fraction > 1) {
                    fraction = 1.0;
                }
        
                float coreToBeRemoved = 0.0;
                auto acc = xAOD::getIsolationCorrectionAccessor(xAOD::Iso::topoetcone, xAOD::Iso::coreCone, xAOD::Iso::coreEnergy);
                if(!acc->isAvailable(par)) {
                    bool foundCore = false;
                    const xAOD::Muon* mu = dynamic_cast<const xAOD::Muon*>(&par);
                    if(mu) {
                        mu->isolationCaloCorrection(coreToBeRemoved, xAOD::Iso::topoetcone, xAOD::Iso::coreCone, xAOD::Iso::IsolationCorrectionParameter::coreEnergy);
                        foundCore = true;
                    } else {
                        const xAOD::Electron* el = dynamic_cast<const xAOD::Electron*>(&par);
                        if(el) {
                            el->isolationCaloCorrection(coreToBeRemoved, xAOD::Iso::topoetcone, xAOD::Iso::coreCone, xAOD::Iso::IsolationCorrectionParameter::coreEnergy);
                            foundCore = true;
                        } else {
                            const xAOD::Photon* ph = dynamic_cast<const xAOD::Photon*>(&par);
                            if(ph) {
                                ph->isolationCaloCorrection(coreToBeRemoved, xAOD::Iso::topoetcone, xAOD::Iso::coreCone, xAOD::Iso::IsolationCorrectionParameter::coreEnergy);
                                foundCore = true;
                            }
                        }
                    }
                    if(!foundCore) {
                        ATH_MSG_WARNING("Could not retrieve topocore. No correction could be calculated.");
                    }
                } else {
                    coreToBeRemoved = (*acc)(*par2);
                }
                correction += coreToBeRemoved * fraction;
            
            }
        
        }

		return CP::CorrectionCode::Ok;
	}

	CP::CorrectionCode IsolationCloseByCorrectionTool::getparticlesInCone( float eta, float phi, float dr, std::vector< const xAOD::TrackParticle*>& output ) const {

		/// retrieve container
		const xAOD::TrackParticleContainer* trks = retrieveTrackParticleContainer();
		if(!trks) return CP::CorrectionCode::Error;

		ATH_MSG_DEBUG("checing track eta=" << eta << ", phi=" << phi);
		/// make selection
		float dr2 = dr*dr;
		for(auto trk: *trks){
			float dEta = fabs(eta-trk->eta());
			if(dEta>dr) continue;
			float dPhi = phiInRange(phi-trk->phi());
			if(dPhi>dr) continue;
			ATH_MSG_DEBUG("ID trk pt=" << trk->pt()*0.001 << " eta=" << trk->eta() << ", phi=" << trk->phi() << " dEta=" << dEta << ", dPhi=" << dPhi << ", dR2 = " << dEta*dEta+dPhi*dPhi);
			if(dr2>(dEta*dEta+dPhi*dPhi)) output.push_back(trk);
		}

		return CP::CorrectionCode::Ok;
	} // TO BE REMOVED AT SOME POINT!!!

	const xAOD::TrackParticleContainer* IsolationCloseByCorrectionTool::retrieveTrackParticleContainer() const {
		const xAOD::TrackParticleContainer* indetTrackParticles = 0;
		if(evtStore()->contains<xAOD::TrackParticleContainer>(m_indetTrackParticleLocation)) {
			if(evtStore()->retrieve(indetTrackParticles,m_indetTrackParticleLocation).isFailure()) {
				ATH_MSG_FATAL( "Unable to retrieve " << m_indetTrackParticleLocation );
				return 0;
			}
		}
		return indetTrackParticles;
	}
	
	const xAOD::Vertex* IsolationCloseByCorrectionTool::retrieveIDBestPrimaryVertex() const {
         std::string PVXLocation = "PrimaryVertices";
         const xAOD::VertexContainer *vtxC = 0;
         if (evtStore()->contains<xAOD::VertexContainer>(PVXLocation)) {
            if (evtStore()->retrieve(vtxC,PVXLocation).isFailure()) {
                ATH_MSG_FATAL( "Unable to retrieve " << PVXLocation);
                return 0;
            } else {
	            if (vtxC->size() == 0) {
	                ATH_MSG_INFO("No vertex in container " << PVXLocation);
	                return 0;
	            }
	            return vtxC->front(); // the first one, probably the beam spot if a single one in the container
            }
         }
        return 0;
    }

    CP::CorrectionCode IsolationCloseByCorrectionTool::getExtrapEtaPhi(const xAOD::IParticle& par, float& eta, float& phi) const {
        const xAOD::Muon* mu = dynamic_cast<const xAOD::Muon*>(&par);
        eta = 0;
        phi = 0;
        bool foundCluster = false;
        if(mu){
            auto cluster = mu->cluster();
            if(cluster){
                float etaT = 0, phiT = 0;
                int nSample = 0;
                for(unsigned int i=0; i<CaloSampling::Unknown; i++) { 
                    auto s = static_cast<CaloSampling::CaloSample>(i);
                    if(cluster->hasSampling(s)) {
                        ATH_MSG_DEBUG("Sampling: " << i << "eta-phi (" << cluster->etaSample(s) << ", " << cluster->phiSample(s) << ")");
                        etaT += cluster->etaSample(s);
                        phiT += cluster->phiSample(s);
                        nSample++;
                    }
                }
                if(nSample>0){
                    eta = etaT/nSample;
                    phi = phiT/nSample;
                    foundCluster = true;
                }else{
                    if(mu->muonType() == xAOD::Muon::MuonType::MuonStandAlone) {
                        foundCluster = true;
                        eta = mu->eta();
                        phi = mu->phi();
                    }
                }
            }else{
                if(mu->muonType() == xAOD::Muon::MuonType::MuonStandAlone) {
                    foundCluster = true;
                    eta = mu->eta();
                    phi = mu->phi();
                }
            }
        } else {
            const xAOD::IParticle* refPar = getReferenceParticle(par);
            const xAOD::Egamma* eg = dynamic_cast<const xAOD::Egamma*>(refPar);
            if(eg) {
                auto cluster = eg->caloCluster();
                if(cluster) {
                    phi = cluster->phi();
                    eta = cluster->eta();
                    foundCluster = true;
                }
            }
        }
        if(!foundCluster) {
            ATH_MSG_WARNING("BADCLUSTER: the calo cluster could not be retrieved.");
            eta = par.eta();
            phi = par.phi();
            return CP::CorrectionCode::Error;
        }
        return CP::CorrectionCode::Ok;
    }
   
    Root::TAccept& IsolationCloseByCorrectionTool::acceptCorrected(const xAOD::IParticle& x, const std::vector<const xAOD::IParticle*>& closePar, int topoetconeModel) {
        
        if(m_selectorTool.empty()) {
	        ATH_MSG_ERROR("Please set the IsolationSelectionTool property with a valid IsolationSelectionTool");
	        m_accept.clear();
	        return m_accept;
        }

        std::vector<xAOD::Iso::IsolationType> isoTypes;
        const xAOD::Muon* mu = dynamic_cast<const xAOD::Muon*>(&x);
 	    if(mu) {
 	    
 	        if(!m_muon_isoTypesSet) {
 	            CP::IsolationSelectionTool* theSelectionTool = dynamic_cast<CP::IsolationSelectionTool*>(&*m_selectorTool);
 	            m_muon_isoTypes.clear();
 	            for(auto x: theSelectionTool->getMuonWPs()) {
 	                Root::TAccept accept = x->accept(*mu);
 	                for(unsigned int i = 0; i < accept.getNCuts(); i++) {
 	                    const TString& name = accept.getCutName(i);
 	                    m_muon_isoTypes.push_back(getIsolationTypeFromString(name, 0));
 	                }
                }
                m_muon_isoTypesSet = true;
            }
            
 	        isoTypes = m_muon_isoTypes;
        } else {
        
            const xAOD::Electron* el = dynamic_cast<const xAOD::Electron*>(&x);
 	        if(el) {
 	        
 	            if(!m_electron_isoTypesSet) {
 	                CP::IsolationSelectionTool* theSelectionTool = dynamic_cast<CP::IsolationSelectionTool*>(&*m_selectorTool);
 	                m_electron_isoTypes.clear();
 	                for(auto x: theSelectionTool->getElectronWPs()) {
 	                    Root::TAccept accept = x->accept(*el);
 	                    for(unsigned int i = 0; i < accept.getNCuts(); i++) {
 	                        const TString& name = accept.getCutName(i);
 	                        m_electron_isoTypes.push_back(getIsolationTypeFromString(name, 1));
 	                    }
                    }
                    m_electron_isoTypesSet = true;
                }
            
 	            isoTypes = m_electron_isoTypes;
 	        } else {
 	        
 	            const xAOD::Photon* ph = dynamic_cast<const xAOD::Photon*>(&x);
 	            if(ph) {
 	            
 	                if(!m_photon_isoTypesSet) {
 	                    CP::IsolationSelectionTool* theSelectionTool = dynamic_cast<CP::IsolationSelectionTool*>(&*m_selectorTool);
                        m_photon_isoTypes.clear();
                        for(auto x: theSelectionTool->getPhotonWPs()) {
                            Root::TAccept accept = x->accept(*ph);
                            for(unsigned int i = 0; i < accept.getNCuts(); i++) {
                                const TString& name = accept.getCutName(i);
                                m_photon_isoTypes.push_back(getIsolationTypeFromString(name, 2));
                            }
                        }
                        m_photon_isoTypesSet = true;
                    }
                    
 	                isoTypes = m_photon_isoTypes;
 	            } else {
 	                ATH_MSG_WARNING("Could not cast particle for acceptCorrected. Will return false.");
                    m_accept.setCutResult("castCut", false);
                    return m_accept;
 	            }
 	        }
        }
        
        if(!m_isInitialised) {
	        ATH_MSG_WARNING("The IsolationCloseByCorrectionTool was not initialised!!!");
	    }
	    
        m_accept.clear();
        strObj m_strPar;
        m_strPar.isolationValues.resize(xAOD::Iso::numIsolationTypes);
        m_strPar.pt = x.pt();
        m_strPar.eta = x.eta();
        std::vector < float > corrections;
        if(getCloseByCorrection(corrections, x, isoTypes, closePar, topoetconeModel)==CP::CorrectionCode::Error) {
            ATH_MSG_WARNING("Could not calculate the corrections. acceptCorrected(x) is done without the corrections.");
            m_accept = m_selectorTool->accept(x);
            return m_accept;
        }
        
        for(unsigned int i = 0; i < isoTypes.size(); i++) {
            SG::AuxElement::Accessor< float > *acc = xAOD::getIsolationAccessor(isoTypes.at(i));
            m_strPar.isolationValues[isoTypes.at(i)] = (*acc)(x) - corrections.at(i);
        }

        m_accept = m_selectorTool->accept(m_strPar);

        return m_accept;
 	}
 	
 	xAOD::Iso::IsolationType IsolationCloseByCorrectionTool::getIsolationTypeFromString(const TString& isoTypeString, int particleType) {
 	    
 	    if((particleType) < 0 || (particleType > 2)) {
 	        ATH_MSG_WARNING("Could not recognise the particle type: " << particleType);
 	        return xAOD::Iso::IsolationType::numIsolationTypes;
 	    }
 	    if(isoTypeString.Contains("ptvarcone20") || ((particleType == 1) && isoTypeString.Contains("_track"))) {
 	        return xAOD::Iso::IsolationType::ptvarcone20;
 	    }
 	    if(isoTypeString.Contains("ptvarcone30") || ((particleType == 0) && isoTypeString.Contains("_track"))) {
 	        return xAOD::Iso::IsolationType::ptvarcone30;
 	    }
 	    if(isoTypeString.Contains("ptvarcone40")) {
 	        return xAOD::Iso::IsolationType::ptvarcone40;
 	    }
 	    if(isoTypeString.Contains("topoetcone20") || ((particleType < 2) && isoTypeString.Contains("_calo"))) {
 	        return xAOD::Iso::IsolationType::topoetcone20;
 	    }
 	    if(isoTypeString.Contains("topoetcone30")) {
 	        return xAOD::Iso::IsolationType::topoetcone30;
 	    }
 	    if(isoTypeString.Contains("topoetcone40") || ((particleType == 2) && isoTypeString.Contains("_calo"))) {
 	        return xAOD::Iso::IsolationType::topoetcone40;
 	    }
 	    if(isoTypeString.Contains("neflowisol20")) {
 	        return xAOD::Iso::IsolationType::neflowisol20;
 	    }
 	    if(isoTypeString.Contains("neflowisol30")) {
 	        return xAOD::Iso::IsolationType::neflowisol30;
 	    }
 	    if(isoTypeString.Contains("neflowisol40")) {
 	        return xAOD::Iso::IsolationType::neflowisol40;
 	    }
 	    if(isoTypeString.Contains("ptcone20") || ((particleType == 2) && isoTypeString.Contains("_track"))) {
 	        return xAOD::Iso::IsolationType::ptcone20;
 	    }
 	    if(isoTypeString.Contains("ptcone30")) {
 	        return xAOD::Iso::IsolationType::ptcone30;
 	    }
 	    if(isoTypeString.Contains("ptcone40")) {
 	        return xAOD::Iso::IsolationType::ptcone40;
 	    }
 	    if(isoTypeString.Contains("etcone20")) {
 	        return xAOD::Iso::IsolationType::etcone20;
 	    }
 	    if(isoTypeString.Contains("etcone30")) {
 	        return xAOD::Iso::IsolationType::etcone30;
 	    }
 	    if(isoTypeString.Contains("etcone40")) {
 	        return xAOD::Iso::IsolationType::etcone40;
 	    }
 	    ATH_MSG_WARNING("Could not recognise the isolation type from the input string: " << isoTypeString);
 	    return xAOD::Iso::IsolationType::numIsolationTypes;
 	}

}
