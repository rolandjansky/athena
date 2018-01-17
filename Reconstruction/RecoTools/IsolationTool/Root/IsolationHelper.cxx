/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <IsolationTool/IsolationHelper.h>
#include <xAODPrimitives/tools/getIsolationAccessor.h>
#include <xAODPrimitives/tools/getIsolationCorrectionAccessor.h>
#include <xAODPrimitives/IsolationHelpers.h>

#ifndef XAOD_ANALYSIS
#include "TrkParameters/TrackParameters.h"
// #include "TrkCaloExtension/CaloExtension.h"
#endif // XAOD_ANALYSIS
#include <iomanip>

namespace CP {
  	IsolationHelper::IsolationHelper(const std::string& name):
  	asg::AsgTool( name )
	{
#ifndef XAOD_ANALYSIS
		declareInterface<IIsolationHelper>(this);
#endif // XAOD_ANALYSIS
// 		declareProperty("CoreCone", m_coreCone = 0.1);
// 		declareProperty("OverlapCone", m_overlapCone = 0.05);
		declareProperty("CoreCone", m_coreCone = 0.05);
	}

	StatusCode IsolationHelper::initialize() {
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
		ATH_CHECK( evtStore()->retrieve(muons,muonkey), false );
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
			ATH_CHECK( evtStore()->record(shallowcopy.first, "IsoFixed_"+muonkey), false );
			ATH_CHECK( evtStore()->record(shallowcopy.second,"IsoFixed_"+muonkey+"Aux."), false );
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
			areacore = m_coreCone*m_coreCone*M_PI;
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

}
