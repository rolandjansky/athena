/*
	Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
	2020 Matthias Schott - Uni Mainz
*/

#include "MuonTrackMonitoring/MuonTrackMonitorAlgorithm.h"

MuonTrackMonitorAlgorithm::MuonTrackMonitorAlgorithm (const std::string& name, ISvcLocator* pSvcLocator)
    :AthMonitorAlgorithm(name,pSvcLocator){}


StatusCode MuonTrackMonitorAlgorithm::initialize()
{
	ATH_CHECK(AthMonitorAlgorithm::initialize());
	ATH_CHECK(m_MuonContainerKey.initialize());
	ATH_CHECK(m_MuonIsoDecorKey.initialize());
	return StatusCode::SUCCESS;
}

StatusCode MuonTrackMonitorAlgorithm::FillMuonInformation(std::string sIdentifier, std::vector<const xAOD::Muon*>	&vecMuons) const 
{
	/// Declaring all variables that are initialized via Python will be plotted
	using namespace Monitored;
	auto 	tool = getGroup("MuonTrackMonitorAlgorithm");
	auto	MuonEta = Monitored::Scalar<float>((sIdentifier+"MuonEta").c_str(), 0);	
	auto	MuonPhi = Monitored::Scalar<float>((sIdentifier+"MuonPhi").c_str(), 0);	
	auto	MuonEtaTight = Monitored::Scalar<float>((sIdentifier+"MuonEtaTight").c_str(), 0);	
	auto	MuonPhiTight = Monitored::Scalar<float>((sIdentifier+"MuonPhiTight").c_str(), 0);	
	auto	MuonEtaMedium = Monitored::Scalar<float>((sIdentifier+"MuonEtaMedium").c_str(), 0);	
	auto	MuonPhiMedium = Monitored::Scalar<float>((sIdentifier+"MuonPhiMedium").c_str(), 0);	
	auto	MuonD0 = Monitored::Scalar<float>((sIdentifier+"MuonD0").c_str(), 0);	
	auto	MuonZ0 = Monitored::Scalar<float>((sIdentifier+"MuonZ0").c_str(), 0);	
	auto	MuonPt = Monitored::Scalar<float>((sIdentifier+"MuonPt").c_str(), 0);	
	auto	MuonDPTIDME = Monitored::Scalar<float>((sIdentifier+"MuonDPTIDME").c_str(), 0);
	auto	MuonsNBHits = Monitored::Scalar<float>((sIdentifier+"MuonNBHits").c_str(), 0);	
	auto	MuonsNPixHits = Monitored::Scalar<float>((sIdentifier+"MuonNPixHits").c_str(), 0);	
	auto	MuonsNSCTHits = Monitored::Scalar<float>((sIdentifier+"MuonNSCTHits").c_str(), 0);	
	auto	MuonsNTRTHits = Monitored::Scalar<float>((sIdentifier+"MuonNTRTHits").c_str(), 0);	
	auto	MuonsIDChi2NDF = Monitored::Scalar<float>((sIdentifier+"MuonIDChi2NDF").c_str(), 0);	
	auto	MuonsMEChi2NDF = Monitored::Scalar<float>((sIdentifier+"MuonMEChi2NDF").c_str(), 0);	

	/// Loop over all Muons
	for(unsigned int n=0; n<vecMuons.size(); n++) {
		const xAOD::Muon* muon = vecMuons[n];
		xAOD::Muon::MuonType muonType = muon->muonType();
		xAOD::Muon::Quality muonQuality	= muon->quality();

		if (muonType==xAOD::Muon::Combined) {
			const xAOD::TrackParticle *cbtp = nullptr;
			const xAOD::TrackParticle *idtp = nullptr;
			const xAOD::TrackParticle *mstp = nullptr;

			ElementLink<xAOD::TrackParticleContainer> cbtpLink = muon->combinedTrackParticleLink();
			if (cbtpLink.isValid()) cbtp = *cbtpLink;
			if (cbtp) {
				uint8_t hitval_numberOfBLayerHits, hitval_numberOfPixelHits, hitval_numberOfSCTHits, hitval_numberOfTRTHits;
				//	uint8_t hitval_innerSmallHits, hitval_innerLargeHits, hitval_middleSmallHits, hitval_middleLargeHits, hitval_outerSmallHits, hitval_outerLargeHits;
				cbtp->summaryValue(hitval_numberOfBLayerHits,	xAOD::SummaryType::numberOfInnermostPixelLayerHits);
				cbtp->summaryValue(hitval_numberOfPixelHits,	xAOD::SummaryType::numberOfPixelHits);
				cbtp->summaryValue(hitval_numberOfSCTHits,		xAOD::SummaryType::numberOfSCTHits);
				cbtp->summaryValue(hitval_numberOfTRTHits,		xAOD::SummaryType::numberOfTRTHits);		

				/// Basic kinematic Information
				MuonEta = cbtp->eta();
				MuonPhi	= cbtp->phi();
				MuonPt	= cbtp->pt();
				MuonZ0	= cbtp->z0();
				fill(tool, MuonEta, MuonPhi, MuonPt, MuonZ0);

				/// Hit Information of the ID
				MuonsNBHits 	= static_cast<unsigned int>(hitval_numberOfBLayerHits);
				MuonsNPixHits 	= static_cast<unsigned int>(hitval_numberOfPixelHits);
				MuonsNSCTHits 	= static_cast<unsigned int>(hitval_numberOfSCTHits);
				MuonsNTRTHits 	= static_cast<unsigned int>(hitval_numberOfTRTHits);
				fill(tool, MuonsNBHits, MuonsNPixHits, MuonsNSCTHits, MuonsNTRTHits);

				/// Save Eta/Phi Information for medium and tight muons, 
				/// to be used for lates efficiency studies
				if (muonQuality==xAOD::Muon::Medium) {
					MuonEtaMedium = cbtp->eta();
					MuonPhiMedium = cbtp->phi();
					fill(tool, MuonEtaMedium, MuonPhiMedium);
				}
				if (muonQuality==xAOD::Muon::Tight) {
					MuonEtaTight = cbtp->eta();
					MuonPhiTight = cbtp->phi();
					fill(tool, MuonEtaTight, MuonPhiTight);
				}

				/// Momentum Resolution and chi2 studies of MS and ID only tracks
				ElementLink<xAOD::TrackParticleContainer> idtpLink = muon->inDetTrackParticleLink();
				ElementLink<xAOD::TrackParticleContainer> mstpLink = muon->muonSpectrometerTrackParticleLink();
				if (idtpLink.isValid()) idtp = *idtpLink;
				if (mstpLink.isValid()) mstp = *mstpLink;
				if (idtp && mstp) {
					MuonDPTIDME 	= (idtp->pt() - mstp->pt()) / idtp->pt();
					MuonsIDChi2NDF 	= idtp->chiSquared()/idtp->numberDoF();
					MuonsMEChi2NDF 	= mstp->chiSquared()/mstp->numberDoF();	
					fill(tool, MuonDPTIDME, MuonsIDChi2NDF, MuonsMEChi2NDF);
				}
			}
		}
	}
	return StatusCode::SUCCESS;
}
	
//========================================================================================================
StatusCode	MuonTrackMonitorAlgorithm::analyseLowLevelMuonFeatures(const xAOD::MuonContainer& Muons, uint32_t lumiBlockID) const 
{
	using namespace Monitored;
	/// Declaring all variables that are initialized via Python will be plotted
	auto 	tool = getGroup("MuonTrackMonitorAlgorithm");
	auto	MSAuthor = Monitored::Scalar<float>("MSAuthor", 0);
	auto	MSQuality = Monitored::Scalar<float>("MSQuality", 0);
	auto	MSType 	= Monitored::Scalar<float>("MSType", 0);
	auto	MSLargeSectorR 	= Monitored::Scalar<float>("MSLargeSectorR", 0);
	auto	MSLargeSectorZ 	= Monitored::Scalar<float>("MSLargeSectorZ", 0);
	auto	MSSmallSectorR 	= Monitored::Scalar<float>("MSSmallSectorR", 0);
	auto	MSSmallSectorZ 	= Monitored::Scalar<float>("MSSmallSectorZ", 0);
	auto	MSEta = Monitored::Scalar<float>("MSEta", 0);
	auto	MSPhi = Monitored::Scalar<float>("MSPhi", 0);
	auto	MSPt = Monitored::Scalar<float>("MSPt", 0);
	auto	NonCBMuonEta = Monitored::Scalar<float>("NonCBMuonEta", 0);	
	auto	NonCBMuonPhi = Monitored::Scalar<float>("NonCBMuonPhi", 0);	
	auto	CBMuonAuthor = Monitored::Scalar<float>("CBMuonAuthor", 0);	
	auto	MSLumiBlockNumberOfMuonTracks = Monitored::Scalar<float>("MSLumiBlockNumberOfMuonTracks", 0);
	auto	MSLumiBlockNumberOfSegments = Monitored::Scalar<float>("MSLumiBlockNumberOfSegments", 0);

	/// Loop over all muons
	for(const auto& muon : Muons) {
		xAOD::Muon::Quality muonQuality	= muon->quality();
		xAOD::Muon::MuonType muonType =	muon->muonType();
		xAOD::Muon::Author muonAuthor =	muon->author();

		/// General Muon Control Plots
		MSAuthor = muonAuthor;
		MSQuality = muonQuality;
		MSType = muonType;
		MSEta = muon->eta();
		MSPhi = muon->phi();
		MSPt = muon->pt();
		MSLumiBlockNumberOfMuonTracks  = lumiBlockID;
		fill(tool, MSAuthor, MSQuality, MSType, MSEta, MSPhi, MSPt, MSLumiBlockNumberOfMuonTracks);

		/// Do Muon Segments Plots
		for (size_t nSeg=0; nSeg<muon->nMuonSegments();nSeg++) {
			MSLumiBlockNumberOfSegments  = lumiBlockID;
			fill(tool, MSLumiBlockNumberOfSegments);
			const xAOD::MuonSegment* muonSegment = muon->muonSegment(nSeg);
                        if (!muonSegment) {
                           continue;
                        }
			MSLargeSectorR 	= sqrt(muonSegment->x()*muonSegment->x() + muonSegment->y()*muonSegment->y());
			MSLargeSectorZ	= muonSegment->z();
			MSSmallSectorR 	= sqrt(muonSegment->x()*muonSegment->x() + muonSegment->y()*muonSegment->y());
			MSSmallSectorZ	= muonSegment->z();
			int sector		= muonSegment->sector();
			if(sector % 2 == 0) {
				fill(tool, MSLargeSectorZ, MSLargeSectorR);
			} else {
				fill(tool, MSSmallSectorZ, MSSmallSectorR);
			}
		}
		CBMuonAuthor = muonAuthor;
		NonCBMuonEta = muon->eta();
		NonCBMuonPhi = muon->phi();
		fill(tool, CBMuonAuthor, NonCBMuonEta, NonCBMuonPhi);
	}

	return StatusCode::SUCCESS;
}



//========================================================================================================
StatusCode	MuonTrackMonitorAlgorithm::analyseCombinedTracks(const xAOD::MuonContainer& Muons, uint32_t lumiBlockID) const {
	using namespace Monitored;

	/// Declaring all variables that are initialized via Python will be plotted
	auto tool = getGroup("MuonTrackMonitorAlgorithm");
	auto CBMuonSector = Monitored::Scalar<float>("CBMuonSector", 0);	
	auto CBMuonCIndex = Monitored::Scalar<float>("CBMuonCIndex", 0);	
	auto CBMuonEta1Triggered = Monitored::Scalar<float>("CBMuonEta1Triggered", 0);	
	auto CBMuonPhi1Triggered = Monitored::Scalar<float>("CBMuonPhi1Triggered", 0);	
	auto CBMuonEta1All = Monitored::Scalar<float>("CBMuonEta1All", 0);	
	auto CBMuonPhi1All = Monitored::Scalar<float>("CBMuonPhi1All", 0);	
	auto CBMuonLumiBlock = Monitored::Scalar<float>("CBMuonLumiBlock", 0);	

	/// Select Combined Muons
	std::vector<const xAOD::Muon*>	vecCombinedMuonsHighPT;
	std::vector<const xAOD::Muon*>	vecCombinedMuons;

	for(const auto& muon : Muons) {
		xAOD::Muon::MuonType muonType = muon->muonType();
		if (muonType==xAOD::Muon::Combined) {
			CBMuonLumiBlock = lumiBlockID;
			fill(tool, CBMuonLumiBlock);

			vecCombinedMuons.push_back(muon);
			if (muon->pt() > m_CBmuons_minPt) vecCombinedMuonsHighPT.push_back(muon);

			/// Provide Segment and Sector Plots
			for (size_t nSeg=0; nSeg<muon->nMuonSegments();nSeg++) {
				const xAOD::MuonSegment* muonSegment = muon->muonSegment(nSeg);
                                if (!muonSegment) {
                                   continue;
                                }
				CBMuonSector = muonSegment->sector();
				CBMuonCIndex = muonSegment->chamberIndex();
				fill(tool, CBMuonSector,CBMuonCIndex);
			}
		}
	}

	/// Fill the relevant Muon Information for each Combined Muon
	
	ATH_CHECK (FillMuonInformation("CB", vecCombinedMuons) );
	
	/// Trigger Studies
	if (vecCombinedMuonsHighPT.size()==1) {
		CBMuonEta1All	= vecCombinedMuonsHighPT[0]->eta();
		CBMuonPhi1All	= vecCombinedMuonsHighPT[0]->phi();
		fill(tool, CBMuonEta1All, CBMuonPhi1All);

		bool isTriggered = false;
		for(const auto& chain : m_hltchainList){
			if(!getTrigDecisionTool().empty() && getTrigDecisionTool()->isPassed( chain ) ){
				isTriggered = true;
			}
		}

		if (isTriggered) {
			CBMuonEta1Triggered = vecCombinedMuonsHighPT[0]->eta();
			CBMuonPhi1Triggered = vecCombinedMuonsHighPT[0]->phi();
			fill(tool, CBMuonEta1Triggered, CBMuonPhi1Triggered);
		}
	}

	return StatusCode::SUCCESS;
}


//========================================================================================================
StatusCode	MuonTrackMonitorAlgorithm::analyseZBosonCandidates(const xAOD::MuonContainer& Muons, uint32_t lumiBlockID) const {
	using namespace Monitored;

	/// Declaring all variables that are initialized via Python will be plotted
	auto tool = getGroup("MuonTrackMonitorAlgorithm");
	auto ZSector = Monitored::Scalar<float>("ZSector", 0);
	auto ZChamber = Monitored::Scalar<float>("ZChamber", 0);
	auto ZSectorEff = Monitored::Scalar<float>("ZSectorEff", 0);
	auto ZChamberEff = Monitored::Scalar<float>("ZChamberEff", 0);
	auto ZMass2D = Monitored::Scalar<float>("ZMass2D", 0);
	auto ZEta2D = Monitored::Scalar<float>("ZEta2D", 0);
	auto ZMass = Monitored::Scalar<float>("ZMass", 0);
	auto ZMuonLumiBlock = Monitored::Scalar<float>("ZMuonLumiBlock", 0);	

	std::vector<const xAOD::Muon*>	vecMuons_ZBoson;
	std::vector<const xAOD::Muon*>	vecMuons_ZBoson_Candidates;
	
	/// Select Muons Relevant for Z
	for(const auto& muon : Muons) {
		xAOD::Muon::MuonType muonType = muon->muonType();
		if (muonType==xAOD::Muon::Combined) {
			const xAOD::TrackParticle *cbtp = nullptr;
			ElementLink<xAOD::TrackParticleContainer> cbtpLink = muon->combinedTrackParticleLink();
			if (cbtpLink.isValid()) cbtp = *cbtpLink;

			/// Select Z Boson
			if (cbtp) {
				float trkiso  = muon->isolation(xAOD::Iso::ptcone30)/muon->pt();
				if (muonType==xAOD::Muon::Combined &&
					cbtp &&
					muon->pt()>m_ZBosonSelection_minPt &&
					std::abs(muon->eta())<m_ZBosonSelection_maxEta &&
					trkiso<m_ZBosonSelection_trkIsolation &&
					std::abs(cbtp->z0())<m_ZBosonSelection_Z0Cut &&
					std::abs(cbtp->d0())<m_ZBosonSelection_D0Cut )
						vecMuons_ZBoson_Candidates.push_back(muon);
			}
		}
	}

	/// Z Boson related plots	
	std::map<int, int>	mapTagged_ZBoson;
	for (unsigned int n=0; n<vecMuons_ZBoson_Candidates.size(); n++)	
		mapTagged_ZBoson[n]=0;
	for (unsigned int n=0; n<vecMuons_ZBoson_Candidates.size(); n++)
		for (unsigned int m=n+1; m<vecMuons_ZBoson_Candidates.size(); m++) {
			TLorentzVector tVec1, tVec2;
			tVec1.SetPtEtaPhiM(vecMuons_ZBoson_Candidates[n]->pt(), vecMuons_ZBoson_Candidates[n]->eta(), vecMuons_ZBoson_Candidates[n]->phi(), 0.0);
			tVec2.SetPtEtaPhiM(vecMuons_ZBoson_Candidates[m]->pt(), vecMuons_ZBoson_Candidates[m]->eta(), vecMuons_ZBoson_Candidates[m]->phi(), 0.0);
			if (vecMuons_ZBoson_Candidates[n]->charge()==vecMuons_ZBoson_Candidates[m]->charge()) continue;
			if ((tVec1+tVec2).M()<m_ZBosonSelection_minMass)	continue;
			if ((tVec1+tVec2).M()>m_ZBosonSelection_maxMass)	continue;

			if (mapTagged_ZBoson[n]!=1) vecMuons_ZBoson.push_back(vecMuons_ZBoson_Candidates[n]);
			mapTagged_ZBoson[n]=1;
			if (mapTagged_ZBoson[m]!=1) vecMuons_ZBoson.push_back(vecMuons_ZBoson_Candidates[m]);
			mapTagged_ZBoson[m]=1;

			ZMass2D = (tVec1+tVec2).M();
			ZEta2D	= tVec1.Eta();
			fill(tool, ZMass2D, ZEta2D);
			ZEta2D	= tVec2.Eta();
			fill(tool, ZMass2D, ZEta2D);
			ZMass	= (tVec1+tVec2).M();
			fill(tool, ZMass);
			
			ZMuonLumiBlock =  lumiBlockID;
			fill(tool, ZMuonLumiBlock);			
		}

	/// Fill the relevant Muon Information for each Z Boson Candidate Muon
	ATH_CHECK( FillMuonInformation("Z", vecMuons_ZBoson) );		

	return StatusCode::SUCCESS;
}



//========================================================================================================
StatusCode	MuonTrackMonitorAlgorithm::analyseJPsiCandidates(const xAOD::MuonContainer& Muons, uint32_t lumiBlockID) const {
	using namespace Monitored;

	/// Declaring all variables that are initialized via Python will be plotted
	auto 	tool = getGroup("MuonTrackMonitorAlgorithm");
	auto	JPsiSector = Monitored::Scalar<float>("JPsiSector", 0);
	auto	JPsiChamber = Monitored::Scalar<float>("JPsiChamber", 0);
	auto	JPsiSectorEff = Monitored::Scalar<float>("JPsiSectorEff", 0);
	auto	JPsiChamberEff = Monitored::Scalar<float>("JPsiChamberEff", 0);
	auto	JPsiMass2D = Monitored::Scalar<float>("JPsiMass2D", 0);
	auto	JPsiEta2D = Monitored::Scalar<float>("JPsiEta2D", 0);
	auto	JPsiMass = Monitored::Scalar<float>("JPsiMass", 0);
	auto	JPsiMuonLumiBlock = Monitored::Scalar<float>("JPsiMuonLumiBlock", 0);	

	std::vector<const xAOD::Muon*>	vecMuons_JPsi;
	std::vector<const xAOD::Muon*>	vecMuons_JPsi_Candidates;
	
	/// JPsi Muon Selection
	for(const auto& muon : Muons) {
		xAOD::Muon::MuonType	muonType	=	muon->muonType();
		if (muonType==xAOD::Muon::Combined) {
			const xAOD::TrackParticle *cbtp = nullptr;
			ElementLink<xAOD::TrackParticleContainer> cbtpLink = muon->combinedTrackParticleLink();
			if (cbtpLink.isValid()) cbtp = *cbtpLink;

			/// Do Combined Muon Plots
			if (cbtp) {
				float trkiso  = muon->isolation(xAOD::Iso::ptcone30)/muon->pt();
				if (muonType==xAOD::Muon::Combined &&
					cbtp &&
					muon->pt()>m_JPsiSelection_minPt &&
					std::abs(muon->eta())<m_JPsiSelection_maxEta &&
					trkiso<m_JPsiSelection_trkIsolation &&
					std::abs(cbtp->z0())<m_JPsiSelection_Z0Cut &&
					std::abs(cbtp->d0())<m_JPsiSelection_D0Cut )
						vecMuons_JPsi_Candidates.push_back(muon);
			}
		}
	}

	/// JPsi related plots	
	std::map<int, int>	mapTagged_JPsi;
	for (unsigned int n=0; n<vecMuons_JPsi_Candidates.size(); n++)	
		mapTagged_JPsi[n]=0;
	for (unsigned int n=0; n<vecMuons_JPsi_Candidates.size(); n++)
		for (unsigned int m=n+1; m<vecMuons_JPsi_Candidates.size(); m++) {
			TLorentzVector tVec1, tVec2;
			tVec1.SetPtEtaPhiM(vecMuons_JPsi_Candidates[n]->pt(), vecMuons_JPsi_Candidates[n]->eta(), vecMuons_JPsi_Candidates[n]->phi(), 0.0);
			tVec2.SetPtEtaPhiM(vecMuons_JPsi_Candidates[m]->pt(), vecMuons_JPsi_Candidates[m]->eta(), vecMuons_JPsi_Candidates[m]->phi(), 0.0);
			if (vecMuons_JPsi_Candidates[n]->charge()==vecMuons_JPsi_Candidates[m]->charge()) continue;
			if ((tVec1+tVec2).M()<m_JPsiSelection_minMass)	continue;
			if ((tVec1+tVec2).M()>m_JPsiSelection_maxMass)	continue;

			if (mapTagged_JPsi[n]!=1) vecMuons_JPsi.push_back(vecMuons_JPsi_Candidates[n]);
			mapTagged_JPsi[n]=1;
			if (mapTagged_JPsi[m]!=1) vecMuons_JPsi.push_back(vecMuons_JPsi_Candidates[m]);
			mapTagged_JPsi[m]=1;

			JPsiMass2D = (tVec1+tVec2).M();
			JPsiEta2D	= tVec1.Eta();
			fill(tool, JPsiMass2D, JPsiEta2D);
			JPsiEta2D	= tVec2.Eta();
			fill(tool, JPsiMass2D, JPsiEta2D);
			JPsiMass	= (tVec1+tVec2).M();
			fill(tool, JPsiMass);

			JPsiMuonLumiBlock	 =  lumiBlockID;
			fill(tool, JPsiMuonLumiBlock	);			
		}
	
	/// Fill the relevant Muon Information for each JPsi Boson Candidate Muon
	ATH_CHECK( FillMuonInformation("JPsi", vecMuons_JPsi) );		

	return StatusCode::SUCCESS;
}


//========================================================================================================
StatusCode MuonTrackMonitorAlgorithm::fillHistograms(const EventContext& ctx) const
{
	using namespace Monitored;

	//Declare the quantities which should be monitored
	uint32_t lumiBlockID = GetEventInfo(ctx)->lumiBlock();

	SG::ReadHandle<xAOD::MuonContainer> Muons{m_MuonContainerKey, ctx};
	if (ATH_UNLIKELY(! Muons.isValid())) {
		ATH_MSG_ERROR("Unable to retrieve muon container " << m_MuonContainerKey);
		return StatusCode::FAILURE;
	}

	ATH_CHECK( analyseLowLevelMuonFeatures(*Muons, lumiBlockID) );
	ATH_CHECK( analyseCombinedTracks(*Muons, lumiBlockID) );
	ATH_CHECK( analyseZBosonCandidates(*Muons, lumiBlockID) );
	ATH_CHECK( analyseJPsiCandidates(*Muons, lumiBlockID) );

	return StatusCode::SUCCESS;
}
