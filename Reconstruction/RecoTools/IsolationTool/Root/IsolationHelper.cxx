/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <IsolationTool/IsolationHelper.h>
#include "xAODTracking/VertexContainer.h"
#include <xAODPrimitives/tools/getIsolationAccessor.h>
#include <xAODPrimitives/tools/getIsolationCorrectionAccessor.h>
#include <xAODPrimitives/IsolationHelpers.h>
#include <xAODTracking/TrackParticle.h>
#include <xAODMuon/Muon.h>
#include "xAODEgamma/Egamma.h"
#include <xAODEgamma/Electron.h>
#include <xAODEgamma/Photon.h>
#include <InDetTrackSelectionTool/IInDetTrackSelectionTool.h>

#ifdef XAOD_ANALYSIS
#include "IsolationSelection/IsolationSelectionTool.h"
#endif

#ifndef XAOD_ANALYSIS
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/SmartIF.h"
#include "TrkParameters/TrackParameters.h"
// #include "TrkCaloExtension/CaloExtension.h"
#endif // XAOD_ANALYSIS
#include <iomanip>

struct strObj;

namespace CP {
  	IsolationHelper::IsolationHelper(const std::string& name):
  	asg::AsgTool( name ),
#ifndef XAOD_ANALYSIS
	m_tracksInConeTool("xAOD::TrackParticlesInConeTool/TrackParticlesInConeTool"), 
#endif
#ifdef XAOD_ANALYSIS
#ifndef XAOD_STANDALONE
        m_selectorTool("CP::IsolationSelectionTool/"+name + "_isoSelTool",this), //THIS IS A PRIVATE TOOLHANDLE (cmt only at the moment)
#endif // XAOD_STANDALONE
#endif // XAOD_ANALYSIS
   	m_trkselTool( "InDet::InDetTrackSelectionTool/" + name + "_isoTrackSelTool") //THIS IS A PUBLIC TOOLHANDLE (cmt and rootcore ok!)

	{
#ifndef XAOD_ANALYSIS
		declareInterface<IIsolationHelper>(this);
		declareProperty("TracksInConeTool", m_tracksInConeTool);
// 		declareProperty("SelectorTool", m_selectorTool);
// 		declareProperty("ParticleCaloExtensionTool", m_caloExtTool);  
#endif // XAOD_ANALYSIS
		declareProperty("TrackParticleLocation",m_indetTrackParticleLocation = "InDetTrackParticles"); 
		declareProperty("OverlapCone", m_overlapCone = 0.1);
		declareProperty("CoreCone", m_coreCone = 0.1);
		declareProperty("PtvarconeRadius", m_ptvarconeRadius = 10000.0);
		m_isCoreSubtracted = false;
		m_isInitialised = false;
		
#ifdef XAOD_STANDALONE
        declareProperty("TrackSelectionTool", m_trkselTool = new InDet::InDetTrackSelectionTool(name + "_isoTrackSelTool") );
#ifdef XAOD_ANALYSIS
        m_selectorTool = new CP::IsolationSelectionTool(name + "_isoSelTool");
#endif // XAOD_ANALYSIS
#else // XAOD_STANDALONE
        declareProperty("TrackSelectionTool", m_trkselTool );
#ifdef XAOD_ANALYSIS
        declareProperty("IsolationSelectionTool", m_selectorTool );
#endif // XAOD_ANALYSIS
#endif // XAOD_STANDALONE
	}

	StatusCode IsolationHelper::initialize() {
#ifndef XAOD_ANALYSIS
		if(m_tracksInConeTool.retrieve().isFailure()) {
			ATH_MSG_FATAL("Could not retrieve TrackParticlesInConeTool.");
			return StatusCode::FAILURE;
		}
		if(m_trkselTool.retrieve().isFailure()){
	        ATH_MSG_FATAL("Could not retrieve InDetTrackSelectionTool");    
	        return StatusCode::FAILURE;
        }
// 		if(m_caloExtTool.retrieve().isFailure()) {
// 			ATH_MSG_FATAL("Could not retrieve ParticleCaloExtensionTool.");
// 			return StatusCode::FAILURE;
// 		}
//         if (!m_caloExtTool.empty()) ATH_CHECK(m_caloExtTool.retrieve());
#endif // XAOD_ANALYSIS
#ifdef ROOTCORE	
        InDet::InDetTrackSelectionTool* theTrackSelectionTool = dynamic_cast<InDet::InDetTrackSelectionTool*>(&*m_trkselTool);
		if(theTrackSelectionTool->setProperty("maxZ0SinTheta", 3.).isFailure()) {
			ATH_MSG_FATAL("Could not set maxZ0SinTheta.");
			return StatusCode::FAILURE;
		}
		if(theTrackSelectionTool->setProperty("minPt", 1000.).isFailure()) {
			ATH_MSG_FATAL("Could not set minPt.");
			return StatusCode::FAILURE;
		}
		if(theTrackSelectionTool->setProperty("CutLevel", "Loose").isFailure()) {
			ATH_MSG_FATAL("Could not set cut level.");
			return StatusCode::FAILURE;
		}
		if(theTrackSelectionTool->initialize().isFailure()) {
			ATH_MSG_FATAL("Could not initialize the TrackSelectionTool.");
			return StatusCode::FAILURE;
		} else {
			ATH_MSG_INFO("The TrackSelectionTool was properly initialized.");
		}
#endif // ROOTCORE
		
#ifdef XAOD_ANALYSIS
		ATH_CHECK( m_selectorTool->initialize() );
        
        CP::IsolationSelectionTool* theSelectionTool = dynamic_cast<CP::IsolationSelectionTool*>(&*m_selectorTool);

        for(auto x: theSelectionTool->getMuonWPs()) {ATH_CHECK(theSelectionTool->addWP(x, xAOD::Type::Other)); Info("adding WP", x->name().c_str());}
        for(auto x: theSelectionTool->getElectronWPs()) {ATH_CHECK(theSelectionTool->addWP(x, xAOD::Type::Other)); Info("adding WP", x->name().c_str());}
        for(auto x: theSelectionTool->getPhotonWPs()) {ATH_CHECK(theSelectionTool->addWP(x, xAOD::Type::Other)); Info("adding WP", x->name().c_str());}
        
#endif // XAOD_ANALYSIS
        
        m_isInitialised = true;
         
		return StatusCode::SUCCESS;
	}

	StatusCode IsolationHelper::finalize() {
		return StatusCode::SUCCESS;
	}


	bool IsolationHelper::isolation(float& value, const xAOD::IParticle& par, xAOD::Iso::IsolationType type) const {
		auto acc = xAOD::getIsolationAccessor( type );
		if(acc) {
			value = (*acc)(par);
		} else {
			return false;
		}

		return true;
	}

	bool IsolationHelper::correctionBitset(xAOD::Iso::IsolationCaloCorrectionBitset& mask, const xAOD::IParticle& par, xAOD::Iso::IsolationType type) const {
		auto acc = xAOD::getIsolationCorrectionBitsetAccessor(xAOD::Iso::isolationFlavour(type));
		if(acc) {
			mask = (*acc)(par);
		} else {
			return false;
		}

		return true;
	}

	bool IsolationHelper::correction(float& value, const xAOD::IParticle& par, xAOD::Iso::IsolationType type, xAOD::Iso::IsolationCaloCorrection corr, xAOD::Iso::IsolationCaloCorrectionBitset corrMask) const {
		if(corr == xAOD::Iso::pileupCorrection){
			if(getPileupCorrection(value, par, type, corrMask)) {
				return true;
			} else {
				return false;
			}
		} else {
			auto acc = (corr == xAOD::Iso::ptCorrection)? xAOD::getIsolationCorrectionAccessor(type, corr) : xAOD::getIsolationCorrectionAccessor(xAOD::Iso::isolationFlavour(type), corr, xAOD::Iso::coreEnergy);
			if(acc) {
				value = (*acc)(par);
			} else {
				return false;
			}
		}
		return true;
	}

	bool IsolationHelper::correction(float& value, const xAOD::IParticle& par, xAOD::Iso::IsolationType type, xAOD::Iso::IsolationCaloCorrectionBitset corrMask) const {
		value = 0;
		std::vector<xAOD::Iso::IsolationCaloCorrection> results;
		m_caloIsoBitHelper.decode(corrMask, results);
		for(auto c: results){
			float temp = 0;
			if(correction(temp, par, type, c, corrMask)) {
				value+=temp;
			} else {
				return false;
			}
		}

		return true;
	}

	bool IsolationHelper::isolation(float& value, const xAOD::IParticle& par, xAOD::Iso::IsolationType type, const std::vector<xAOD::Iso::IsolationCaloCorrection>& corrs) const {
		xAOD::Iso::IsolationCaloCorrectionBitset mask = 0;
		m_caloIsoBitHelper.encode(corrs, mask);

		return isolation(value, par, type, mask); 
	}

	bool IsolationHelper::isolation(float& value, const xAOD::IParticle& par, xAOD::Iso::IsolationType type, xAOD::Iso::IsolationCaloCorrectionBitset corrMask) const {
		/// get orinial values
		float origValue = 0;
		if(!isolation(origValue, par, type)) return false;
		xAOD::Iso::IsolationCaloCorrectionBitset origMask = 0;
		if(!correctionBitset(origMask, par, type)) return false;

		if(origMask == corrMask) { 
			value=origValue;
		} else{
			float origCorr = 0;
			if(!correction(origCorr, par, type, origMask)) return false;
			float newCorr = 0;
			if(!correction(newCorr, par, type, corrMask)) return false;

			value = origValue+origCorr-newCorr;
		}

		return true;
	}

	bool IsolationHelper::updateIsolation(xAOD::MuonContainer*& copy,xAOD::ShallowAuxContainer*& copyaux, std::vector<xAOD::Iso::IsolationType>& types, xAOD::Iso::IsolationCaloCorrectionBitset corrMask, std::string muonkey, bool recordSG) const {
		const xAOD::MuonContainer* muons(0);
		ATH_CHECK( evtStore()->retrieve(muons,muonkey) );
		std::pair<xAOD::MuonContainer*,xAOD::ShallowAuxContainer*> shallowcopy = xAOD::shallowCopyContainer(*muons);
		copy = shallowcopy.first;
		copyaux = shallowcopy.second;

		for(auto par: *copy){
			for(auto type: types){
				float value = 0;
					if(!isolation(value, *par, type, corrMask)) return false;
					auto acc = xAOD::getIsolationAccessor( type );
					auto acc2 = xAOD::getIsolationCorrectionBitsetAccessor(xAOD::Iso::isolationFlavour(type));

					if(acc && acc2){
						(*acc2)(*par) = corrMask.to_ulong();
						(*acc)(*par) = value;
					} else {
						return false;
					}
				}
			}

			if(recordSG) {
			ATH_CHECK( evtStore()->record(shallowcopy.first, "IsoFixed_"+muonkey) );
			ATH_CHECK( evtStore()->record(shallowcopy.second,"IsoFixed_"+muonkey+"Aux.") );
		}
		return true;
	}

	bool IsolationHelper::getPileupCorrection(float& value, const xAOD::IParticle& par, xAOD::Iso::IsolationType type, xAOD::Iso::IsolationCaloCorrectionBitset corrMask) const {
		// Get the core size
		if(corrMask == 0){
			auto acc = xAOD::getIsolationCorrectionBitsetAccessor(xAOD::Iso::isolationFlavour(type));
			if(acc) corrMask = (*acc)(par);
		}
		float areacore = 0;
		if(corrMask.test(static_cast<unsigned int>(xAOD::Iso::core57cells))){
			areacore = 5*0.025*7*M_PI/128;
		}else if(corrMask.test(static_cast<unsigned int>(xAOD::Iso::coreCone))){
			areacore = 0.01*M_PI; // dR = 0.1, dR^2 = 0.01
		}

		return getPileupCorrection(value, par, type, areacore);
	}

	bool IsolationHelper::getPileupCorrection(float& value, const xAOD::IParticle& par, xAOD::Iso::IsolationType type, float coreArea) const {
		xAOD::Iso::IsolationFlavour flavor = xAOD::Iso::isolationFlavour(type);

		/// container name
		bool isCentral = fabs(par.eta())<1.5;
		std::string esName = isCentral ? "TopoClusterIsoCentralEventShape" : "TopoClusterIsoForwardEventShape";
		if (flavor == xAOD::Iso::neflowisol) {
			esName = isCentral ? "ParticleFlowIsoCentralEventShape" : "ParticleFlowIsoForwardEventShape";
		}

		/// get desity
		const xAOD::EventShape* edShape;
		if (evtStore()->retrieve(edShape,esName).isFailure()) {
			ATH_MSG_ERROR("Cannot retrieve density container " + esName + " for isolation correction. No ED correction");
			return false;
		} 

		double rho = 0;
		bool gotDensity = edShape->getDensity(xAOD::EventShape::Density,rho);
		if (!gotDensity) {
			ATH_MSG_ERROR("Cannot retrieve density " + esName + " for isolation correction. No ED correction");
			return false;
		}

		/// get correction
		float dR = xAOD::Iso::coneSize(type);
		// ATH_MSG_INFO("rho =" << rho << " dR=" << dR);
		value = rho*(dR*dR*M_PI - coreArea);

		return true;
	}

	const xAOD::IParticle* IsolationHelper::getReferenceParticle(const xAOD::IParticle& particle) const {
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


	bool IsolationHelper::removeOverlap(std::vector<float>& results, const xAOD::IParticle& par, const std::vector<xAOD::Iso::IsolationType>& types, const std::vector<const xAOD::IParticle*>& closePar, float trackRadiusMin, float caloRadiusMin, int topoetconeModel, const xAOD::Vertex* vertex, const xAOD::CaloClusterContainer* topoClusters) const {

        if(!m_isInitialised) {
	        ATH_MSG_WARNING("The HelperTool was not initialised!!!");
	    }
	    
		results.clear();
		
		if(!vertex) {
            vertex = retrieveIDBestPrimaryVertex();
            if(!vertex) {
                ATH_MSG_WARNING("Could not find the vertex. Aborting.");
		        return false;
            }
        }
		
		bool status = true;

		for(unsigned int i = 0; i < types.size(); i++){
		
		    xAOD::Iso::IsolationType isoType = types.at(i);

			xAOD::Iso::IsolationFlavour flavour = xAOD::Iso::isolationFlavour(isoType);
			float removal = 0.0;
			float coneSize = xAOD::Iso::coneSize(isoType);
			
	
			switch (flavour) {
	
				case xAOD::Iso::IsolationFlavour::topoetcone:
					status = status && calculateRemovalTopoetcone(removal, par, isoType, closePar, coneSize, caloRadiusMin, topoetconeModel, topoClusters);
					break;
			
				case xAOD::Iso::IsolationFlavour::ptcone:
					status = status && calculateRemovalTrackIso(removal, par, isoType, closePar, vertex, coneSize, trackRadiusMin);
					break;
			
				case xAOD::Iso::IsolationFlavour::ptvarcone:
				    coneSize = getMin(coneSize, m_ptvarconeRadius / par.pt());
					status = status && calculateRemovalTrackIso(removal, par, isoType, closePar, vertex, coneSize, trackRadiusMin);
					break;
			
				default:
					ATH_MSG_WARNING("The isolation flavour " << flavour << " is not handled.");
					status = false;
					break;
	
			}
	
			results.push_back(removal);

		}

		return status;

	}

    bool IsolationHelper::calculateRemovalTrackIso(float& removal, const xAOD::IParticle& par, xAOD::Iso::IsolationType type, const std::vector<const xAOD::IParticle*>& closePar, const xAOD::Vertex* vertex, float coneSize, float trackRadiusMin) const {
        
        if(!m_isInitialised) {
	        ATH_MSG_WARNING("The HelperTool was not initialised!!!");
	    }
	    
        //// If the track isolation is already nil, no need to perform overlap removal.
        auto accIso = xAOD::getIsolationAccessor(type);
        float isoVal = 999999;
        if(!accIso->isAvailable(par)) {
            ATH_MSG_WARNING("Could not retrieve the isolation variable.");
        } else {
            isoVal = (*accIso)(par);
            if(isoVal <= 0) {
                removal = 0.0;
                return true;
            }
        }

		const xAOD::IParticle *tp = getReferenceParticle(par);
		if(!tp) {
		    ATH_MSG_WARNING("No track particle available. Aborting.");
		    return false;
		}
		std::vector<const xAOD::TrackParticle*> tps;

#ifndef XAOD_ANALYSIS
		if( !m_tracksInConeTool->particlesInCone(tp->eta(), tp->phi(), coneSize, tps) ) { return false; }
#else
		if( !getparticlesInCone(tp->eta(), tp->phi(), coneSize, tps) ) { return false; }
#endif

		removal = 0.0;

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
                            
                            bool considerTrack = ((dr2 >= m_overlapCone * m_overlapCone) 
                                || !m_isCoreSubtracted);
                            considerTrack = considerTrack && 
                                (dr2 >= trackRadiusMin * trackRadiusMin);
                                
                            if( considerTrack) {

                                ATH_MSG_DEBUG("Overlap cone: " << m_overlapCone);
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
                                    removal = removal + coreToBeRemoved;
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

		return true;
		
	}

	bool IsolationHelper::calculateRemovalTopoetcone(float& removal, const xAOD::IParticle& par, xAOD::Iso::IsolationType type, const std::vector<const xAOD::IParticle*>& closePar, float coneSize, float caloRadiusMin, int modelOption, const xAOD::CaloClusterContainer* topoClusters) const {
		
		if(!m_isInitialised) {
	        ATH_MSG_WARNING("The HelperTool was not initialised!!!");
	    }
	    
		// If the track isolation is already nil or negative, no need to perform overlap removal.
        auto accIso = xAOD::getIsolationAccessor(type);
        if(!accIso->isAvailable(par)) {
            ATH_MSG_WARNING("Could not retrieve the isolation variable.");
        } else {
            float isoVal = (*accIso)(par);
            if(isoVal <= 0) {
                removal = 0.0;
                return true;
            }
        }
		
		// Retrieves the averaged eta and phi of the particle.
        float par1_eta = par.eta();
        float par1_phi = par.phi();
        getExtrapEtaPhi(par, par1_eta, par1_phi);

		removal = 0.0;
		
		// Retrieves the topocluster container if not provided and if requested (modelOption == -1).
		std::vector<const xAOD::CaloCluster*> clusters;
		if(modelOption == -1) {
		    bool foundContainer = false;
		    if(!topoClusters) {
		        if (!evtStore()->retrieve(topoClusters, "CaloCalTopoClusters").isSuccess()) {
		            ATH_MSG_WARNING("Could not retrieve the topocluster container. Will use a simple model." );
		            modelOption = 0;
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
		    getExtrapEtaPhi(*par2, par2_eta, par2_phi);
		    float deta = par1_eta - par2_eta;
		    float dphi = par1_phi - par2_phi;
		    float dr = sqrt(deta * deta + dphi * dphi);
		    
		    if(dr >= caloRadiusMin) {
		    
                ATH_MSG_DEBUG("Cone size: " << coneSize);
                ATH_MSG_DEBUG("Particle " << i << " pT: " << par2->pt());
                ATH_MSG_DEBUG("Particle " << i << " eta: " << par2_eta);
                ATH_MSG_DEBUG("Particle " << i << " phi: " << par2_phi);
                ATH_MSG_DEBUG("Particle " << i << " type: " << par2->type());
            
                // If requested by the model, removes the core topoclusters.
                if(modelOption == -1) {
                
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
                                removal = removal + etUncal;
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
            
                    if(modelOption == 0) {
                        if(coneSize <= m_coreCone) {
                            fraction = 0.0;
                        } else {
                            if(dr <= coneSize) {
                                fraction = 1.0;
                            }
                        }
                    }
            
                    if(modelOption > 0) {
                        if(coneSize <= m_coreCone) {
                            fraction = 0.0;
                        } else {
                            if(modelOption == 1) {
                                fraction = 0.0;
                                if((dr > 2 * m_coreCone) && (dr <= (coneSize - m_coreCone))) {
                                    fraction = 1.0;
                                }
                            }
                    
                            if(modelOption == 2) {
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
                            ATH_MSG_WARNING("Could not retrieve topocore. No removal performed.");
                        }
                    } else {
                        coreToBeRemoved = (*acc)(*par2);
                    }
                    removal += coreToBeRemoved * fraction;
                
                }
            
            }
            
        }

		return true;
	}


#ifdef XAOD_ANALYSIS
	bool IsolationHelper::getparticlesInCone( float eta, float phi, float dr, std::vector< const xAOD::TrackParticle*>& output ) const {

		/// retrieve container
		const xAOD::TrackParticleContainer* trks = retrieveTrackParticleContainer();
		if(!trks) return false;

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

		return true;
	}
#endif // XAOD_ANALYSIS

	const xAOD::TrackParticleContainer* IsolationHelper::retrieveTrackParticleContainer() const {
		const xAOD::TrackParticleContainer* indetTrackParticles = 0;
		if(evtStore()->contains<xAOD::TrackParticleContainer>(m_indetTrackParticleLocation)) {
			if(evtStore()->retrieve(indetTrackParticles,m_indetTrackParticleLocation).isFailure()) {
				ATH_MSG_FATAL( "Unable to retrieve " << m_indetTrackParticleLocation );
				return 0;
			}
		}
		return indetTrackParticles;
	}
	
	const xAOD::Vertex* IsolationHelper::retrieveIDBestPrimaryVertex() const {
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


    bool IsolationHelper::getExtrapEtaPhi(const xAOD::IParticle& par, float& eta, float& phi) const {
        const xAOD::Muon* mu = dynamic_cast<const xAOD::Muon*>(&par);
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
//                     ATH_MSG_WARNING("GOODCLUSTER");
//                     ATH_MSG_WARNING("Type, quality, pT, eta, phi: " << mu->muonType() 
//                         << ", " << mu->quality() << ", " << mu->pt() << ", " << mu->eta()
//                         << ", " << mu->phi());
                }else{
                    if(mu->muonType() != xAOD::Muon::MuonType::MuonStandAlone) {
                        ATH_MSG_WARNING("BADCLUSTER: Muon calo cluster is empty?");
                        ATH_MSG_WARNING("Type, quality, pT, eta, phi: " << mu->muonType() 
                        << ", " << mu->quality() << ", " << mu->pt() << ", " << mu->eta()
                        << ", " << mu->phi());
                    }
                }
            }else{
                if(mu->muonType() != xAOD::Muon::MuonType::MuonStandAlone) {
                    ATH_MSG_WARNING("BADCLUSTER: Muon calo cluster not found. Calo extension can not be obtained!!!");
                    ATH_MSG_WARNING("Type, quality, pT, eta, phi: " << mu->muonType() 
                        << ", " << mu->quality() << ", " << mu->pt() << ", " << mu->eta()
                        << ", " << mu->phi());
                }
                        
            }
        } else {
            const xAOD::IParticle* refPar = getReferenceParticle(par);
            const xAOD::Egamma* eg = dynamic_cast<const xAOD::Egamma*>(refPar);
            if(eg) {
                phi = eg->caloCluster()->phi();
                eta = eg->caloCluster()->eta();
                foundCluster = true;
            }
        }
        if(!foundCluster) {
// #ifndef XAOD_ANALYSIS
//             const xAOD::IParticle* refPar = getReferenceParticle(par);
//             const xAOD::TrackParticle* tp = dynamic_cast<const xAOD::TrackParticle*>(refPar);
//             if(tp) {
//                 ATH_MSG_DEBUG("Geting calo extension caloExtension tool.");
//                 const Trk::CaloExtension* caloExtension = 0;
//                 if(!m_caloExtTool->caloExtension(*tp, caloExtension, true)){
//                     ATH_MSG_WARNING("Cannot get caloExtension.");
//                     return false;
//                 };
// 
//                 const std::vector<const Trk::CurvilinearParameters*>& intersections = caloExtension->caloLayerIntersections();
//                 if (intersections.size()>0) {
//                     Amg::Vector3D avePoint(0,0,0);
//                     for (unsigned int i = 0; i < intersections.size(); ++i){
//                         const Amg::Vector3D& point = intersections[i]->position();
//                         ATH_MSG_DEBUG("Intersection: " << i << " eta-phi (" << point.eta() << ", " << point.phi() << ")");
//                         avePoint += point;
//                     }
//                     avePoint = (1./intersections.size())*avePoint;
//                     eta = avePoint.eta();
//                     phi = avePoint.phi();
//                 }
//             }
// #else // ATHENA
            return false;
// #endif // XAOD_ANALYSIS
        }
        return true;
    }

//#ifdef ROOTCORE 
#ifdef XAOD_ANALYSIS   
    const Root::TAccept& IsolationHelper::acceptCorrected(const xAOD::IParticle& x, const std::vector<const xAOD::IParticle*>& closePar, const std::vector<xAOD::Iso::IsolationType>& isoTypes, float trackRadiusMin, float caloRadiusMin, int topoetconeModel, const xAOD::Vertex* vertex, const xAOD::CaloClusterContainer* topoClusters) const {
        
        if(!m_isInitialised) {
	        ATH_MSG_WARNING("The HelperTool was not initialised!!!");
	    }
	    
        m_accept.clear();
        strObj m_strPar;
        m_strPar.isolationValues.resize(xAOD::Iso::numIsolationTypes);
        m_strPar.pt = x.pt();
        m_strPar.eta = x.eta();
        std::vector < float > corrections;
        if(!removeOverlap(corrections, x, isoTypes, closePar, trackRadiusMin, caloRadiusMin, topoetconeModel, vertex, topoClusters)) {
            ATH_MSG_WARNING("Could not perform overlap removal.");
            m_accept = acceptCorrected(x);
            return m_accept;
        }
        
        for(unsigned int i = 0; i < isoTypes.size(); i++) {
            SG::AuxElement::Accessor< float > *acc = xAOD::getIsolationAccessor(isoTypes.at(i));
            m_strPar.isolationValues[isoTypes.at(i)] = (*acc)(x) - corrections.at(i);
        }

        m_accept = m_selectorTool->accept(m_strPar);

        return m_accept;
 	}
 	
 	const Root::TAccept& IsolationHelper::acceptCorrected(const xAOD::IParticle& x) const {
 	    
 	    if(!m_isInitialised) {
	        ATH_MSG_WARNING("The HelperTool was not initialised!!!");
	    }
	    
 	    const xAOD::Muon* mu = dynamic_cast<const xAOD::Muon*>(&x);
 	    if(mu) {
 	        m_accept = m_selectorTool->accept(*mu);
 	        return m_accept;
 	    }
 	    const xAOD::Electron* el = dynamic_cast<const xAOD::Electron*>(&x);
 	    if(el) {
 	        m_accept = m_selectorTool->accept(*el);
 	        return m_accept;
 	    }
 	    const xAOD::Photon* ph = dynamic_cast<const xAOD::Photon*>(&x);
 	    if(ph) {
 	        m_accept = m_selectorTool->accept(*ph);
 	        return m_accept;
 	    }
        ATH_MSG_WARNING("Could not cast particle for acceptCorrected. Will return false.");
        m_accept.setCutResult("castCut", false);
        return m_accept;
        
 	}
 	
    bool IsolationHelper::setSelectorProperty(const char* field, const char* property) {
 	    return dynamic_cast<CP::IsolationSelectionTool*>(&*m_selectorTool)->setProperty(field, property);
 	}
//#endif
#endif // XAOD_ANALYSIS
}
