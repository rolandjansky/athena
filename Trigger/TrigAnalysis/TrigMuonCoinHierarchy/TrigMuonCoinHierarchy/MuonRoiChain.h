/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGER_MUONROICHAIN_H
#define TRIGGER_MUONROICHAIN_H

#include <inttypes.h>

#include "GaudiKernel/MsgStream.h"

#include "AnalysisTriggerEvent/Muon_ROI.h"

namespace Muon {
  class TgcCoinData;
}

namespace Trigger {
  class MuonRoiChain {
  public:
    /** Constructor */
    MuonRoiChain(void);

    /** Destructor */
    ~MuonRoiChain() {};

    /** Set Muon_ROI with roiIndex */
    bool setMuon_ROI(const Muon_ROI* muonRoi, int roiIndex); 
    /** Set MuCTPI_RDO with dataWordIndex */ 
    bool setMuCTPI_RDO_dataWord(uint32_t muctpiRdoDataWord, int dataWordIndex);
    /** Set TgcCoinData (Sector Logic) */
    bool setTgcCoinData(const Muon::TgcCoinData* tgcSL); 

    /** Get Muon_ROI */
    const Muon_ROI* getMuon_ROI() const; 
    /** Get roiIndex of Muon_ROI */
    int getMuon_ROIIndex() const;
    /** Get MuCTPI_RDO */ 
    uint32_t getMuCTPI_RDO_dataWord() const; 
    /** Get Index of MuCTPI_RDO dataWord */
    int getMuCTPI_RDO_dataWordIndex() const;
    /** Get TgcCoinData (Sector Logic) */
    const Muon::TgcCoinData* getTgcCoinData() const; 

    /** Return Muon_ROI::RoISource */ 
    Muon_ROI::RoISource getSource() const; 

    /** Return Muon_ROI::Hemisphere */ 
    Muon_ROI::Hemisphere getHemisphere() const; 

    /** Dump MuonRoiChain */
    void dump(MsgStream& ms, MSG::Level level) const;

    /** Method to know Endcap chamber boundary */ 
    bool isInEndcapChamberBoundary() const;

  private:
    /** Method to set Endcap chamber boundary */
    void setInEndcapChamberBoundary(int roiNumber); 

    /** Pointer of Muon_ROI */
    const Muon_ROI* m_muonRoi; 
    /** Index of Muon_ROI in LVL1_ROI::muons_type */ 
    int m_roiIndex; 

    /** A data word of MuCTPI_RDO */ 
    uint32_t m_muctpiRdoDataWord;
    /** Index of the data word in MuCTPI_RDO */
    int m_dataWordIndex;

    /** Pointer of TgcCoinData of Tgc Sector Logic */
    const Muon::TgcCoinData* m_tgcSL;

    /** Member enum to distinguish Barrel, Endcap and Forward */   
    Muon_ROI::RoISource m_source;

    /** Member enum to distinguish Side A and Side C */
    Muon_ROI::Hemisphere m_hemisphere;

    /** Flag for Endcap chamber boundary */ 
    bool m_isInEndcapChamberBoundary;
  };
} // end of namespace Trigger

#endif // TRIGGER_MUONROICHAIN_H
