/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonCoinHierarchy/MuonRoiChain.h"

#include "TrigT1Result/MuCTPI_DataWord_Decoder.h" 
#include "MuonTrigCoinData/TgcCoinData.h"

namespace Trigger {
  MuonRoiChain::MuonRoiChain(void) {
    m_muonRoi = 0;
    m_roiIndex = -1;
    m_muctpiRdoDataWord = 0;
    m_dataWordIndex = -1;
    m_tgcSL = 0; 

    m_source = Muon_ROI::Barrel;
    m_hemisphere = Muon_ROI::Positive;  
    m_isInEndcapChamberBoundary = false; 
  }
  
  bool MuonRoiChain::setMuon_ROI(const Muon_ROI* muonRoi, int roiIndex) {
    if(m_muonRoi || m_roiIndex!=-1) return false; // MuonRoiChain is not designed to update data members. 

    Muon_ROI::RoISource source = muonRoi->getSource();
    Muon_ROI::Hemisphere hemisphere = muonRoi->getHemisphere();

    if((m_muctpiRdoDataWord || m_dataWordIndex!=-1) || m_tgcSL) { 
      if(m_source!=source) return false;
      if(m_hemisphere!=hemisphere) return false;
    }

    m_source = source;
    m_hemisphere = hemisphere;
    m_muonRoi = muonRoi;
    m_roiIndex = roiIndex; 
    if(m_source==Muon_ROI::Endcap) setInEndcapChamberBoundary(muonRoi->getRoI());
    return true; 
  }

  bool MuonRoiChain::setMuCTPI_RDO_dataWord(uint32_t muctpiRdoDataWord, int dataWordIndex) {
    if(m_muctpiRdoDataWord || m_dataWordIndex!=-1) return false; // MuonRoiChain is not designed to update data members. 

    MuCTPI_DataWord_Decoder muctpiDecoder(muctpiRdoDataWord);
    MuCTPI_RDO::SectorLocation sectorLocation = muctpiDecoder.getSectorLocation();
    Muon_ROI::RoISource source = (sectorLocation==MuCTPI_RDO::BARREL ? Muon_ROI::Barrel : 
				  (sectorLocation==MuCTPI_RDO::ENDCAP ? Muon_ROI::Endcap : 
				   Muon_ROI::Forward));
    Muon_ROI::Hemisphere hemisphere = (muctpiDecoder.getHemisphere()==1 ? Muon_ROI::Positive : Muon_ROI::Negative);
    if((m_muonRoi || m_roiIndex!=-1) || m_tgcSL) {
      if(m_source!=source) return false;  
      if(m_hemisphere!=hemisphere) return false;
    }

    m_source = source;
    m_hemisphere = hemisphere;
    m_muctpiRdoDataWord = muctpiRdoDataWord;
    m_dataWordIndex = dataWordIndex;
    if(m_source==Muon_ROI::Endcap) setInEndcapChamberBoundary(muctpiDecoder.getRoiNumber());
    return true; 
  }
  
  bool MuonRoiChain::setTgcCoinData(const Muon::TgcCoinData* tgcSL) {
    if(tgcSL->type()!=Muon::TgcCoinData::TYPE_SL) return false;  

    if(m_tgcSL) return false; // MuonRoiChain is not designed to update data members. 
    
    Muon_ROI::RoISource source = (tgcSL->isForward() ? Muon_ROI::Forward : Muon_ROI::Endcap);
    Muon_ROI::Hemisphere hemisphere = (tgcSL->isAside() ? Muon_ROI::Positive : Muon_ROI::Negative);

    if((m_muonRoi || m_roiIndex!=-1) || (m_muctpiRdoDataWord || m_dataWordIndex!=-1)) {
      if(m_source!=source) return false; 
      if(m_hemisphere!=hemisphere) return false;
    }

    m_source = source;
    m_hemisphere = hemisphere;
    m_tgcSL = tgcSL;
    if(m_source==Muon_ROI::Endcap) setInEndcapChamberBoundary(tgcSL->roi()); 
    return true;
  }

  const Muon_ROI* MuonRoiChain::getMuon_ROI() const {
    return m_muonRoi; 
  }
  int MuonRoiChain::getMuon_ROIIndex() const {
    return m_roiIndex; 
  }

  uint32_t MuonRoiChain::getMuCTPI_RDO_dataWord() const {
    return m_muctpiRdoDataWord;
  }
  int MuonRoiChain::getMuCTPI_RDO_dataWordIndex() const {
    return m_dataWordIndex;
  }
  
  const Muon::TgcCoinData* MuonRoiChain::getTgcCoinData() const {
    return m_tgcSL;
  }

  Muon_ROI::RoISource MuonRoiChain::getSource() const {
    return m_source; 
  }

  Muon_ROI::Hemisphere MuonRoiChain::getHemisphere() const {
    return m_hemisphere; 
  }

  void MuonRoiChain::dump(MsgStream& ms, MSG::Level level) const {
    MuCTPI_DataWord_Decoder dataWordDecoder(m_muctpiRdoDataWord);

    ms << level << "*** MuonRoiChain object : "
       << (m_source==Muon_ROI::Barrel ? "Barrel  " : (m_source==Muon_ROI::Endcap ? "Endcap  " : "Forward "))  
       << (m_hemisphere==Muon_ROI::Positive ? "Side A " : "Side C ") 
       << " ***" << endreq; 
    ms << level << " Muon_ROI= " << m_muonRoi << " , roiIndex= " << m_roiIndex << endreq; 
    ms << level << " MuCTPI_RDO_dataWord= " << m_muctpiRdoDataWord << " , dataWordIndex= " << m_dataWordIndex << endreq;
    ms << level << "   SectorID= " << dataWordDecoder.getSectorID() << " , RoiNumber= " << dataWordDecoder.getRoiNumber() 
       << " , Pt= " << dataWordDecoder.getPt() << " , SentRoi= " << dataWordDecoder.getSentRoi() << endreq; 
    ms << level << " TgcCoinData= " << m_tgcSL << endreq; 
    ms << level << "*********************************************" << endreq; 
  }

  bool MuonRoiChain::isInEndcapChamberBoundary() const {
    return m_isInEndcapChamberBoundary;
  }

  void MuonRoiChain::setInEndcapChamberBoundary(int roiNumber) {
    int roiRow = roiNumber/4;
    if(roiRow== 3 || roiRow== 4 || // SSC 2 : ROI 12-19
       roiRow== 7 || roiRow== 8 || // SSC 4 : ROI 28-35
       roiRow==11 || roiRow==12 || // SSC 6 : ROI 44-51
       roiRow==23 || roiRow==24) { // SSC12 : ROI 92-99
      m_isInEndcapChamberBoundary = true;
    } else {
      m_isInEndcapChamberBoundary = false; 
    }
  } 

} // end of namespace Trigger
