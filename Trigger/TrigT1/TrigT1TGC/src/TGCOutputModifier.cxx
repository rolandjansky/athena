/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// First the corresponding header.
#include "TrigT1TGC/TGCOutputModifier.h"

#include "TrigT1Interfaces/Lvl1MuCTPIInputPhase1.h"
#include "TrigT1Interfaces/Lvl1MuSectorLogicConstantsPhase1.h"
#include "TrigT1Interfaces/MuCTPIL1Topo.h"

namespace LVL1TGCTrigger {

  TGCOutputModifier::TGCOutputModifier( const std::string& name, ISvcLocator* pSvcLocator )
    :
    AthReentrantAlgorithm(name, pSvcLocator)
  {
  }
  
  TGCOutputModifier::~TGCOutputModifier()
  {
  }



  /// Regular Gaudi algorithm initialization function
  StatusCode TGCOutputModifier::initialize()
  {
    ATH_MSG_INFO( "=======================================" );
    ATH_MSG_INFO( "Initialisation for TGCOutputModifier"  );
    ATH_MSG_INFO( "NSWVetoMode="<<m_nswVetoMode.value());
    ATH_MSG_INFO( "EmulateA="<<m_emulateA.value());
    ATH_MSG_INFO( "EmulateC="<<m_emulateC.value());
    ATH_MSG_INFO( "DeltaEtaCut="<<m_deltaEtaCut.value() );
    ATH_MSG_INFO( "DeltaPhiCut="<<m_deltaPhiCut.value() );
    ATH_MSG_INFO( "DeltaThetaCut="<<m_deltaThetaCut.value() );
    ATH_MSG_INFO( "InputMuctpiLocation="<<m_inputKey.key() );
    ATH_MSG_INFO( "OutputMuctpiLocation="<<m_outputKey.key() );
    ATH_MSG_INFO( "MuonSegmentContainer="<<m_musegKey.key() );
    ATH_MSG_INFO( "=======================================" );

    ATH_CHECK(m_inputKey.initialize());
    ATH_CHECK(m_outputKey.initialize());
    ATH_CHECK(m_recTGCRoiTool.retrieve());
    ATH_CHECK(m_musegKey.initialize(m_nswVetoMode.value()));
    return StatusCode::SUCCESS;
  }

  /// Regular Gaudi algorithm execute function
  StatusCode TGCOutputModifier::execute(const EventContext& eventContext) const
  {
    SG::ReadHandle<LVL1MUONIF::Lvl1MuCTPIInputPhase1> rh_muctpiTgc(m_inputKey, eventContext);
    if(!rh_muctpiTgc.isValid()){
      ATH_MSG_ERROR("Cannot retrieve LVL1MUONIF::Lvl1MuCTPIInputPhase1");
      return StatusCode::FAILURE;
    }
    const LVL1MUONIF::Lvl1MuCTPIInputPhase1* inTgc2Muctpi = rh_muctpiTgc.cptr();

    std::vector<MuSegData> muSegDataColl;
    if( m_nswVetoMode.value() ){
      SG::ReadHandle<xAOD::MuonSegmentContainer> rh_museg(m_musegKey, eventContext);
      if(!rh_museg.isValid()){
	ATH_MSG_ERROR("Cannot retrieve xAOD::MuonSegmentContainer");
	return StatusCode::FAILURE;
      }
      const xAOD::MuonSegmentContainer* muSegContainer = rh_museg.cptr();
      for(auto seg : *muSegContainer){
	if(seg->chamberIndex() != Muon::MuonStationIndex::EIS &&
	   seg->chamberIndex() != Muon::MuonStationIndex::EIL &&
	   seg->chamberIndex() != Muon::MuonStationIndex::CSS &&
	   seg->chamberIndex() != Muon::MuonStationIndex::CSL   )continue;
	MuSegData data;
	data.pos.SetXYZ(seg->x(),seg->y(),seg->z());
	data.vec.SetXYZ(seg->px(),seg->py(),seg->pz());
	muSegDataColl.push_back(data);
	ATH_MSG_DEBUG(" found a muon segment:"
		      << " segEta=" << data.pos.Eta()
		      << " segPhi=" << data.pos.Phi()
		      << " segEtaVec=" << data.vec.Eta()
		      << " segPhiVec=" << data.vec.Phi());
      }
    }

    SG::WriteHandle<LVL1MUONIF::Lvl1MuCTPIInputPhase1> wh_muctpiTgc(m_outputKey, eventContext);
    ATH_CHECK(wh_muctpiTgc.record(std::make_unique<LVL1MUONIF::Lvl1MuCTPIInputPhase1>()));
    LVL1MUONIF::Lvl1MuCTPIInputPhase1* outTgc2Muctpi = wh_muctpiTgc.ptr();

    LVL1MUONIF::Lvl1MuCTPIInputPhase1::MuonSystem tgc_systems[2]
      = {LVL1MUONIF::Lvl1MuCTPIInputPhase1::MuonSystem::Endcap,
	 LVL1MUONIF::Lvl1MuCTPIInputPhase1::MuonSystem::Forward};
    for(auto isys : tgc_systems){
      auto data = &inTgc2Muctpi->getData(isys); // const std::vector<Lvl1MuVectWithBC>&
      for(auto bcmu : *data){ // Lvl1MuVectWithBC
	int bcid = bcmu.first;
	auto muvec = bcmu.second; // Lvl1MuVect
	for(size_t i = 0 ; i < muvec.size() ; i++){ // i = system index = MaxSectors * Side + Sectors
	  auto sectorData = muvec.at(i).get();// std::shared_ptr <Lvl1MuSectorLogicDataPhase1>
	  if(!sectorData){
	    ATH_MSG_WARNING("sectorData is null!");
	    continue;
	  }
	  if(isys==LVL1MUONIF::Lvl1MuCTPIInputPhase1::MuonSystem::Endcap){
	    size_t numberOfSectors = LVL1MUONIF::Lvl1MuCTPIInputPhase1::NumberOfEndcapSector;
	    size_t isub = (i<numberOfSectors)
	      ?(LVL1MUONIF::Lvl1MuCTPIInputPhase1::MuonSubSystem::C_side):(LVL1MUONIF::Lvl1MuCTPIInputPhase1::MuonSubSystem::A_side);
	    size_t isec = i % numberOfSectors;
	    if(i != inTgc2Muctpi->getSystemIndex(isys,isub,isec)){
	      ATH_MSG_WARNING("System Index Mismatch!!");
	      continue;
	    }
	    LVL1MUONIF::Lvl1MuEndcapSectorLogicDataPhase1 sldata;
	    Copy(sectorData,sldata);
	    Update(muSegDataColl,sldata,isys,isub,isec);
	    outTgc2Muctpi->setSectorLogicData(sldata,isys,isub,isec,bcid);
	  }else if(isys==LVL1MUONIF::Lvl1MuCTPIInputPhase1::MuonSystem::Forward){
	    size_t numberOfSectors = LVL1MUONIF::Lvl1MuCTPIInputPhase1::NumberOfForwardSector;
	    size_t isub = (i<numberOfSectors)
	      ?(LVL1MUONIF::Lvl1MuCTPIInputPhase1::MuonSubSystem::C_side):(LVL1MUONIF::Lvl1MuCTPIInputPhase1::MuonSubSystem::A_side);
	    size_t isec = i % numberOfSectors;
	    if(i != inTgc2Muctpi->getSystemIndex(isys,isub,isec)){
	      ATH_MSG_WARNING("System Index Mismatch!!");
	      continue;
	    }
	    LVL1MUONIF::Lvl1MuForwardSectorLogicDataPhase1 sldata;
	    Copy(sectorData,sldata);
	    Update(muSegDataColl,sldata,isys,isub,isec);
	    outTgc2Muctpi->setSectorLogicData(sldata,isys,isub,isec,bcid);
	  }else{
	    ATH_MSG_WARNING("Unknown system. Barrel? isys="<<isys);
	  }
	}
      }
    }

    return StatusCode::SUCCESS;
  }

  void TGCOutputModifier::Copy(const LVL1MUONIF::Lvl1MuSectorLogicDataPhase1* sldataOrg,
			       LVL1MUONIF::Lvl1MuSectorLogicDataPhase1& sldataNew  ) const
  {
    sldataNew.clear2candidatesInSector();
    if(sldataOrg->is2candidatesInSector()) sldataNew.set2candidatesInSector();
    sldataNew.bcid(sldataOrg->bcid());
    for (unsigned int icand=0;
	 icand<LVL1MUONIF::NCAND[LVL1MUONIF::Lvl1MuCTPIInputPhase1::Endcap];icand++){
      sldataNew.roi(icand,sldataOrg->roi(icand));
      sldataNew.pt(icand,sldataOrg->pt(icand));
      sldataNew.ovl(icand,sldataOrg->ovl(icand));
      sldataNew.charge(icand,sldataOrg->charge(icand));
      sldataNew.bw2or3(icand,sldataOrg->bw2or3(icand));
      sldataNew.goodmf(icand,sldataOrg->goodmf(icand));
      sldataNew.innercoin(icand,sldataOrg->innercoin(icand));
      sldataNew.clear2candidates(icand);
      if(sldataOrg->is2candidates(icand))sldataNew.set2candidates(icand);
    }
  }
  void TGCOutputModifier::Update(const std::vector<MuSegData>& muSegDataColl,
				 LVL1MUONIF::Lvl1MuSectorLogicDataPhase1& sldata,
				 size_t systemId,
				 size_t subSystemId,
				 size_t sectorId) const
  {
    if(subSystemId==LVL1MUONIF::Lvl1MuCTPIInputPhase1::idSideA() && !m_emulateA.value()) return;
    if(subSystemId==LVL1MUONIF::Lvl1MuCTPIInputPhase1::idSideC() && !m_emulateC.value()) return;
    int tmpside = (subSystemId==LVL1MUONIF::Lvl1MuCTPIInputPhase1::idSideA())?(1):(0);
    int tmpsector = sectorId;
    int tmptype = (systemId==LVL1MUONIF::Lvl1MuCTPIInputPhase1::idForwardSystem())?(1):(2);
    unsigned int sectorAddress = (tmpside) + (tmpsector<<1) + (tmptype<<6);
    for (unsigned int icand=0;
	 icand<LVL1MUONIF::NCAND[LVL1MUONIF::Lvl1MuCTPIInputPhase1::Endcap];icand++){
      if(sldata.pt(icand)<0)continue;
      int roi = sldata.roi(icand);
      unsigned long int roiWord = (roi<<2)+(sectorAddress<<14);
      auto roiData = m_recTGCRoiTool->roiData(roiWord);
      TVector3 roiPos;
      roiPos.SetPtEtaPhi(10,roiData.eta(),roiData.phi());
      if( std::abs(roiPos.Eta()) < 1.3 ) continue; // only NSW region
      ATH_MSG_DEBUG("RoI pT=" << sldata.pt(icand)
		    << " roiEta=" << roiPos.Eta()
		    << " roiPhi=" << roiPos.Phi() );
      bool matched = !m_nswVetoMode.value();
      for(auto muSegData : muSegDataColl){
	if(matched)break;
	float deltaEta = std::abs( muSegData.pos.Eta() - roiPos.Eta() );
	float deltaPhi = std::abs( muSegData.pos.DeltaPhi( roiPos ) );
	float deltaTheta = std::abs( muSegData.vec.Theta() - muSegData.pos.Theta() );
	matched |= (deltaEta < m_deltaEtaCut.value() &&
		    deltaPhi < m_deltaPhiCut.value() &&
		    deltaTheta < m_deltaThetaCut.value());
	ATH_MSG_DEBUG(" matched=" << matched
		      << " segEta=" << muSegData.pos.Eta()
		      << " segPhi=" << muSegData.pos.Phi()
		      << " segEtaVec=" << muSegData.vec.Eta()
		      << " segPhiVec=" << muSegData.vec.Phi()
		      << " deltaEta=" << deltaEta
		      << " deltaPhi=" << deltaPhi
		      << " deltaTheta=" << deltaTheta);
      }
      sldata.innercoin(icand,matched);
    }
  }

}


