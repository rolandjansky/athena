/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef LARSTRIPSCROSSTALKCORRECTOR_H
#define LARSTRIPSCROSSTALKCORRECTOR_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "LArRecConditions/LArBadChannelCont.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArElecCalib/ILArPedestal.h"
#include "LArRawEvent/LArAccumulatedCalibDigit.h"
#include "LArRecConditions/LArBadChannelMask.h"

#include <vector>
#include <string>
#include <bitset>

//Poor man's histogramming class to monitor difference introduced by xtalk corr
class XtalkCorrHisto {
public:
  XtalkCorrHisto(const unsigned nBins, const float maxbin);
  void add(const LArAccumulatedCalibDigit* dig, const float reldiff);
  void print(MsgStream& stream, const LArOnlineID* onlId, MSG::Level lvl=MSG::INFO);

private:
  struct maxlistitem {
  public:
    maxlistitem(HWIdentifier chidin, int dacin , int delayin ,float reldiffin) :
      chid(chidin), dac(dacin), delay(delayin), reldiff(reldiffin) {}
    HWIdentifier chid;
    int dac;
    int delay;
    float reldiff;
  };

  const unsigned m_nBins;
  const float m_binwidth;
  std::vector<unsigned> m_diffs;

  std::vector<maxlistitem> m_maxlist;  
};




//===================================================================================

class LArStripsCrossTalkCorrector : public AthAlgorithm
{
 public:
  LArStripsCrossTalkCorrector(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArStripsCrossTalkCorrector();

  //standard algorithm methods
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();


 private:
  StatusCode executeWithAccumulatedDigits();
  StatusCode executeWithStandardDigits();


  SG::ReadCondHandleKey<LArBadChannelCont> m_BCKey {this, "BadChanKey", "LArBadChannel", "SG key for LArBadChan object"};
  SG::ReadCondHandleKey<LArBadFebCont> m_BFKey {this, "MissingFEBKey", "LArBadFeb", "SG key for miffing FEB object"};
  SG::ReadCondHandleKey<LArOnOffIdMapping>  m_cablingKey{this, "OnOffMap", "LArOnOffIdMap", "SG key for mapping object"};
  

  Gaudi::Property<std::vector<std::string> > m_dontUseForCorr{this,"DontUseForXtalkCorr",{}, "Types of bad channel should not be taken into accout for xtalk correction of their neighbor"};


  LArBadChannelMask m_dontUseForCorrMask;
  Gaudi::Property<std::vector<std::string> > m_dontCorr{this,"NoXtalkCorr",{}, "Types of bad channel should be ignored and not x-talk corrected"};
  LArBadChannelMask m_dontCorrMask;



  const LArOnlineID*  m_onlineHelper;
  const LArEM_ID*     m_emId;

  const DataHandle<ILArPedestal> m_larPedestal;
  unsigned int        m_event_counter;
  unsigned int        m_MAXeta, m_MINeta;
  const unsigned int  m_MAXphi;
  int                 m_nStrips;
  //Algorithm-Properties:
  std::vector<std::string> m_keylist;
  unsigned int m_ADCsatur;
  bool m_useAccumulatedDigits;
  float m_acceptableDifference;

  std::string m_pedKey;

  uint16_t m_fatalFebErrorPattern;

  const size_t m_noIdx;

  std::vector < std::vector < const LArAccumulatedCalibDigit* > > m_stripsLookUp;

  std::vector < std::bitset< 128 > > m_knownMissingFebs;

  std::set<HWIdentifier> m_uncorrectedIds;

  struct neighbour_t{
   int dist;
   const LArAccumulatedCalibDigit* dig;
   double ped;
   double weight;
 };


  XtalkCorrHisto m_differences;

  std::string printMaxSample(const LArAccumulatedCalibDigit* thisDig);
  StatusCode initKnownMissingFebs(const int bec);

  size_t getEtaIndex(const Identifier) const;
  size_t getPhiIndex(const Identifier) const;

  void correctSamples(std::vector<double>& dest, const neighbour_t& neighbor) const;


};


inline void LArStripsCrossTalkCorrector::correctSamples(std::vector<double>& dest, const LArStripsCrossTalkCorrector::neighbour_t& nb) const {
  const size_t nSamples=std::min(dest.size(),nb.dig->sampleSum().size());
  const double pedTimesTrig=nb.ped*(double)nb.dig->nTriggers();
  for (size_t i=0;i<nSamples;++i) {
    dest[i]+=nb.weight*(nb.dig->sampleSum()[i]-pedTimesTrig);
  }
  return;
}

inline size_t LArStripsCrossTalkCorrector::getEtaIndex(const Identifier id) const {
  if (m_emId->sampling(id)!=1) return m_noIdx; //Deal only with layer 1
  const int region = m_emId->region(id);
  const int keta = m_emId->eta(id);
  if (abs(m_emId->barrel_ec(id))== 1) {	// Barrel case
    if (region==0)
      return keta;
    else
      return m_noIdx;
  }// end if barrel
  else if (abs(m_emId->barrel_ec(id))== 2) {  //Endcap case
    switch (region) {
    case 2:
      return keta;
      break;
    case 3: 
      return keta+96;
      break;
    case 4:
      return keta+144;
      break;
    default:
      return m_noIdx;
      break;
    }   
  }// end if endcap
  return m_noIdx; //endcap OW or unknown region
}

inline size_t LArStripsCrossTalkCorrector::getPhiIndex(const Identifier id) const {
   const int kphi = m_emId->phi(id);
   const int kside=m_emId->barrel_ec(id)>0; 
   return kphi+kside*m_MAXphi;
 }



#endif
