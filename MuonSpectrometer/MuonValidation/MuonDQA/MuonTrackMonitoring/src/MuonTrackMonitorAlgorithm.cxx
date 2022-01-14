/*
	Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

StatusCode MuonTrackMonitorAlgorithm::FillMuonInformation(const std::string& sIdentifier, std::vector<const xAOD::Muon*>	&vecMuons) const 
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
	auto	NonCBMuonsNBHits = Monitored::Scalar<float>((sIdentifier+"MuonNBHits").c_str(), 0);	
	auto	NonCBMuonsNPixHits = Monitored::Scalar<float>((sIdentifier+"MuonNPixHits").c_str(), 0);	
	auto	NonCBMuonsNSCTHits = Monitored::Scalar<float>((sIdentifier+"MuonNSCTHits").c_str(), 0);	
	auto	NonCBMuonsNTRTHits = Monitored::Scalar<float>((sIdentifier+"MuonNTRTHits").c_str(), 0);	
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
			const xAOD::TrackParticle *metp = nullptr;

			ElementLink<xAOD::TrackParticleContainer> cbtpLink = muon->combinedTrackParticleLink();
			if (cbtpLink.isValid()) cbtp = *cbtpLink;
			if (cbtp) {
				uint8_t hitval_numberOfBLayerHits, hitval_numberOfPixelHits, hitval_numberOfSCTHits, hitval_numberOfTRTHits;
				cbtp->summaryValue(hitval_numberOfBLayerHits,	xAOD::SummaryType::numberOfInnermostPixelLayerHits);
				cbtp->summaryValue(hitval_numberOfPixelHits,	xAOD::SummaryType::numberOfPixelHits);
				cbtp->summaryValue(hitval_numberOfSCTHits,		xAOD::SummaryType::numberOfSCTHits);
				cbtp->summaryValue(hitval_numberOfTRTHits,		xAOD::SummaryType::numberOfTRTHits);		

				/// Basic kinematic Information
				MuonEta = cbtp->eta();
				MuonPhi	= cbtp->phi();
				MuonPt	= cbtp->pt();
				MuonZ0	= cbtp->z0();
				MuonD0	= cbtp->d0();

				fill(tool, MuonEta, MuonPhi, MuonPt, MuonZ0, MuonD0);

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
				ElementLink<xAOD::TrackParticleContainer> metpLink = muon->muonSpectrometerTrackParticleLink();
				if (idtpLink.isValid()) idtp = *idtpLink;
				if (metpLink.isValid()) metp = *metpLink;
				if (idtp && metp) {
					MuonDPTIDME 	= (idtp->pt() - metp->pt()) / idtp->pt();
					MuonsIDChi2NDF 	= idtp->chiSquared()/std::max(1.f,idtp->numberDoF());
					MuonsMEChi2NDF 	= metp->chiSquared()/std::max(1.f,metp->numberDoF());	
					fill(tool, MuonDPTIDME, MuonsIDChi2NDF, MuonsMEChi2NDF);
				}

			}
		}
        else {
			const xAOD::TrackParticle *ptp = nullptr;
			const xAOD::TrackParticle *idtp = nullptr;
			const xAOD::TrackParticle *metp = nullptr;

            ptp = muon->primaryTrackParticle();
			if (ptp) {
				/// Basic kinematic Information
				MuonEta = ptp->eta();
				MuonPhi	= ptp->phi();
				MuonPt	= ptp->pt();
				MuonZ0	= ptp->z0();
				MuonD0	= ptp->d0();
				fill(tool, MuonEta, MuonPhi, MuonPt, MuonZ0, MuonD0);


                // Information on hits in each layer
				uint8_t hitval_numberOfBLayerHits{0}, hitval_numberOfPixelHits{0}, hitval_numberOfSCTHits{0}, hitval_numberOfTRTHits{0};
				ptp->summaryValue(hitval_numberOfBLayerHits,	xAOD::SummaryType::numberOfInnermostPixelLayerHits);
				ptp->summaryValue(hitval_numberOfPixelHits,	xAOD::SummaryType::numberOfPixelHits);
				ptp->summaryValue(hitval_numberOfSCTHits,    xAOD::SummaryType::numberOfSCTHits);
				ptp->summaryValue(hitval_numberOfTRTHits,	xAOD::SummaryType::numberOfTRTHits);		
				NonCBMuonsNBHits 	= static_cast<unsigned int>(hitval_numberOfBLayerHits);
				NonCBMuonsNPixHits 	= static_cast<unsigned int>(hitval_numberOfPixelHits);
				NonCBMuonsNSCTHits 	= static_cast<unsigned int>(hitval_numberOfSCTHits);
				NonCBMuonsNTRTHits 	= static_cast<unsigned int>(hitval_numberOfTRTHits);
				fill(tool, NonCBMuonsNBHits, NonCBMuonsNPixHits, NonCBMuonsNSCTHits, NonCBMuonsNTRTHits);
                
				/// Momentum Resolution and chi2 studies of MS and ID only tracks
				ElementLink<xAOD::TrackParticleContainer> idtpLink = muon->inDetTrackParticleLink();
				ElementLink<xAOD::TrackParticleContainer> metpLink = muon->extrapolatedMuonSpectrometerTrackParticleLink();
				if (idtpLink.isValid()) idtp = *idtpLink;
				if (metpLink.isValid()) metp = *metpLink;
				if (idtp && metp) {
					MuonDPTIDME 	= (idtp->pt() - metp->pt()) / idtp->pt();
					MuonsIDChi2NDF 	= idtp->chiSquared()/idtp->numberDoF();
					MuonsMEChi2NDF 	= metp->chiSquared()/metp->numberDoF();	
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
	auto	MSLumiBlockNumberOfMuonTracks = Monitored::Scalar<float>("MSLumiBlockNumberOfMuonTracks", 0);
	auto	MSLumiBlockNumberOfSegments = Monitored::Scalar<float>("MSLumiBlockNumberOfSegments", 0);

	/// Loop over all muons
	for(const auto muon : Muons) {
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
		for (size_t nSeg=0; nSeg < muon->nMuonSegments(); nSeg++) {
			MSLumiBlockNumberOfSegments = lumiBlockID;
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
	}

	return StatusCode::SUCCESS;
}



//========================================================================================================
StatusCode	MuonTrackMonitorAlgorithm::analyseCombinedTracks(const xAOD::MuonContainer& Muons, uint32_t lumiBlockID) const {
	using namespace Monitored;

	/// Declaring all variables that are initialized via Python will be plotted
	auto tool = getGroup("MuonTrackMonitorAlgorithm");
    auto MuonType = Monitored::Scalar<int>("MuonType", 0);
	auto CBMuonSector = Monitored::Scalar<float>("CBMuonSector", 0);	
	auto CBMuonCIndex = Monitored::Scalar<float>("CBMuonCIndex", 0);	
	auto CBMuonEta1Triggered = Monitored::Scalar<float>("CBMuonEta1Triggered", 0);	
	auto CBMuonPhi1Triggered = Monitored::Scalar<float>("CBMuonPhi1Triggered", 0);	
	auto CBMuonEta1NoTrig = Monitored::Scalar<float>("CBMuonEta1NoTrig", 0);	
	auto CBMuonPhi1NoTrig = Monitored::Scalar<float>("CBMuonPhi1NoTrig", 0);	
	auto CBMuonEta1All = Monitored::Scalar<float>("CBMuonEta1All", 0);	
	auto CBMuonPhi1All = Monitored::Scalar<float>("CBMuonPhi1All", 0);	
	auto CBMuonLumiBlock = Monitored::Scalar<float>("CBMuonLumiBlock", 0);	

	auto NonCBMuonSector = Monitored::Scalar<float>("NonCBMuonSector", 0);	
	auto NonCBMuonCIndex = Monitored::Scalar<float>("NonCBMuonCIndex", 0);	
	auto NonCBMuonEta1Triggered = Monitored::Scalar<float>("NonCBMuonEta1Triggered", 0);	
	auto NonCBMuonPhi1Triggered = Monitored::Scalar<float>("NonCBMuonPhi1Triggered", 0);	
	auto NonCBMuonEta1NoTrig = Monitored::Scalar<float>("NonCBMuonEta1NoTrig", 0);	
	auto NonCBMuonPhi1NoTrig = Monitored::Scalar<float>("NonCBMuonPhi1NoTrig", 0);	
	auto NonCBMuonEta1All = Monitored::Scalar<float>("NonCBMuonEta1All", 0);	
	auto NonCBMuonPhi1All = Monitored::Scalar<float>("NonCBMuonPhi1All", 0);	
	auto NonCBMuonLumiBlock = Monitored::Scalar<float>("NonCBMuonLumiBlock", 0);

	/// Select Combined Muons
	std::vector<const xAOD::Muon*>	vecCombinedMuonsHighPT;
	std::vector<const xAOD::Muon*>	vecCombinedMuons;

	/// Select not Combined Muons
	std::vector<const xAOD::Muon*>	vecNonCombinedMuonsHighPT;
	std::vector<const xAOD::Muon*>	vecNonCombinedMuons;

	for(const auto muon : Muons) {
		xAOD::Muon::MuonType muonType = muon->muonType();
                MuonType = muonType;
                fill(tool, MuonType);
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
        else {
			NonCBMuonLumiBlock = lumiBlockID;
			fill(tool, NonCBMuonLumiBlock);

            vecNonCombinedMuons.push_back(muon);
			if (muon->pt() > m_CBmuons_minPt) vecNonCombinedMuonsHighPT.push_back(muon);

			/// Provide Segment and Sector Plots
			for (size_t nSeg=0; nSeg<muon->nMuonSegments();nSeg++) {
				const xAOD::MuonSegment* muonSegment = muon->muonSegment(nSeg);
                                if (!muonSegment) {
                                   continue;
                                }
				NonCBMuonSector = muonSegment->sector();
				NonCBMuonCIndex = muonSegment->chamberIndex();
				fill(tool, NonCBMuonSector, NonCBMuonCIndex);
			}
		}
	}

	/// Fill the relevant Muon Information for each Combined Muon
	ATH_CHECK (FillMuonInformation("CB", vecCombinedMuons) );
	ATH_CHECK (FillMuonInformation("NonCB", vecNonCombinedMuons) );
	
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

		if ((isTriggered) && (vecCombinedMuonsHighPT.size() > 0)){
			CBMuonEta1Triggered = vecCombinedMuonsHighPT[0]->eta();
			CBMuonPhi1Triggered = vecCombinedMuonsHighPT[0]->phi();
			fill(tool, CBMuonEta1Triggered, CBMuonPhi1Triggered);
		}
        else if (vecCombinedMuonsHighPT.size() > 0 ) {
			CBMuonEta1NoTrig = vecCombinedMuonsHighPT[0]->eta();
			CBMuonPhi1NoTrig = vecCombinedMuonsHighPT[0]->phi();
			fill(tool, CBMuonEta1NoTrig, CBMuonPhi1NoTrig);
        }
	}
            

	return StatusCode::SUCCESS;
}


//========================================================================================================
StatusCode	MuonTrackMonitorAlgorithm::plotResonanceCandidates(const std::string& resonanceName, std::vector<const xAOD::Muon*>& muonCandidates, uint32_t lumiBlockID) const {
	using namespace Monitored;

	/// Declaring all variables that are initialized via Python will be plotted
	auto tool = getGroup("MuonTrackMonitorAlgorithm");
	auto Eta = Monitored::Scalar<float>((resonanceName+"Eta").c_str(), 0);
	auto Mass = Monitored::Scalar<float>((resonanceName+"Mass").c_str(), 0);
	auto MuonLumiBlock = Monitored::Scalar<float>((resonanceName+"_MuonLumiBlock").c_str(), 0);	
    auto muMinusEta = Monitored::Scalar<float>("muMinusEta", -9);
    auto muPlusEta = Monitored::Scalar<float>("muPlusEta", -9);
    auto Eta2D = Monitored::Scalar<const char*>("Eta2D", "");

	/// Z Boson related plots	
	std::map<int, int>	mapTagged_Resonance;
    std::vector<const xAOD::Muon*>  vecMuons;
	for (unsigned int n=0; n<muonCandidates.size(); n++)	
		mapTagged_Resonance[n]=0;
	for (unsigned int n=0; n<muonCandidates.size(); n++){
        const TLorentzVector& tVec1 = muonCandidates[n]->p4();
		for (unsigned int m=n+1; m<muonCandidates.size(); m++) {
            const TLorentzVector& tVec2 = muonCandidates[m]->p4();
            const TLorentzVector candidate = tVec1 + tVec2;
            const float resonance_Mass   = candidate.M();
			const float resonance_Eta	= candidate.Eta();
			if (muonCandidates[n]->charge()==muonCandidates[m]->charge()) continue;
			if ((resonance_Mass < m_ZBosonSelection_minMass)&&(resonanceName=="Z"))	continue;
			if ((resonance_Mass > m_ZBosonSelection_maxMass)&&(resonanceName=="Z"))	continue;
			if ((resonance_Mass < m_JPsiSelection_minMass)&&(resonanceName=="JPsi"))	continue;
			if ((resonance_Mass > m_JPsiSelection_maxMass)&&(resonanceName=="JPsi"))	continue;

			if (mapTagged_Resonance[n]!=1) vecMuons.push_back(muonCandidates[n]);
			mapTagged_Resonance[n]=1;
			if (mapTagged_Resonance[m]!=1) vecMuons.push_back(muonCandidates[m]);
			mapTagged_Resonance[m]=1;

            if (muonCandidates[n]->charge()<0){
                muMinusEta = tVec1.Eta();
                muPlusEta = tVec2.Eta();
            }
            else{
                muMinusEta = tVec2.Eta();
                muPlusEta = tVec1.Eta();
            }
            if ((muMinusEta>1.05)&&(muPlusEta>1.05)){
                Eta2D = "EA-EA";
            } else if ((muMinusEta>1.05)&&(muPlusEta>0.)&&(muPlusEta<1.05)){
                Eta2D = "EA-BA";
            } else if ((muMinusEta>1.05)&&(muPlusEta>-1.05)&&(muPlusEta<0.)){
                Eta2D = "EA-BC";
            } else if ((muMinusEta>1.05)&&(muPlusEta<-1.05)){
                Eta2D = "EA-EC";
            } else if ((muMinusEta>0.)&&(muMinusEta<1.05)&&(muPlusEta>1.05)){
                Eta2D = "BA-EA";
            } else if ((muMinusEta>0.)&&(muMinusEta<1.05)&&(muPlusEta>0.)&&(muPlusEta<1.05)){
                Eta2D = "BA-BA";
            } else if ((muMinusEta>0.)&&(muMinusEta<1.05)&&(muPlusEta>-1.05)&&(muPlusEta<0.)){
                Eta2D = "BA-BC";
            } else if ((muMinusEta>0.)&&(muMinusEta<1.05)&&(muPlusEta<-1.05)){
                Eta2D = "BA-EC";
            } else if ((muMinusEta>-1.05)&&(muMinusEta<0.)&&(muPlusEta>1.05)){
                Eta2D = "BC-EA";
            } else if ((muMinusEta>-1.05)&&(muMinusEta<0.)&&(muPlusEta>0.)&&(muPlusEta<1.05)){
                Eta2D = "BC-BA";
            } else if ((muMinusEta>-1.05)&&(muMinusEta<0.)&&(muPlusEta>-1.05)&&(muPlusEta<0.)){
                Eta2D = "BC-BC";
            } else if ((muMinusEta>-1.05)&&(muMinusEta<0.)&&(muPlusEta<-1.05)){
                Eta2D = "BC-EC";
            } else if ((muMinusEta<-1.05)&&(muPlusEta>1.05)){
                Eta2D = "EC-EA";
            } else if ((muMinusEta<-1.05)&&(muPlusEta>0.)&&(muPlusEta<1.05)){
                Eta2D = "EC-BA";
            } else if ((muMinusEta<-1.05)&&(muPlusEta>-1.05)&&(muPlusEta<0.)){
                Eta2D = "EC-BC";
            } else if ((muMinusEta<-1.05)&&(muPlusEta<-1.05)){
                Eta2D = "EC-EC";
            }
            Mass = resonance_Mass;
            Eta = resonance_Eta;
			fill(tool, Mass, Eta, Eta2D, muMinusEta, muPlusEta);
		
			MuonLumiBlock =  lumiBlockID;
			fill(tool, MuonLumiBlock);			
		}
    }

	/// Fill the relevant Muon Information for each Z Boson Candidate Muon
	ATH_CHECK( FillMuonInformation(resonanceName, vecMuons) );		

	return StatusCode::SUCCESS;
}


StatusCode	MuonTrackMonitorAlgorithm::analyseResonanceCandidates(const xAOD::MuonContainer& Muons, uint32_t lumiBlockID) const {

	std::vector<const xAOD::Muon*>	vecMuons_ZBoson_Candidates;
	std::vector<const xAOD::Muon*>	vecMuons_JPsi_Candidates;

	/// Select Muons Relevant for Z
	for(const auto muon : Muons) {
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

    ATH_CHECK( plotResonanceCandidates("Z", vecMuons_ZBoson_Candidates, lumiBlockID) );
    ATH_CHECK( plotResonanceCandidates("JPsi", vecMuons_JPsi_Candidates, lumiBlockID) );

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
	ATH_CHECK( analyseResonanceCandidates(*Muons, lumiBlockID) );

	return StatusCode::SUCCESS;
}
