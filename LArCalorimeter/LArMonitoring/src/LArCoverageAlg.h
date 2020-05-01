
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class LArCoverageAlg
 * @author Margherita Spalla (margherita.spalla@cern.ch) [migrated from LArCoverage algorithm by Jessica Leveque <jleveque@in2p3.fr>]
 *
 */

#ifndef LARCOVERAGEALG_H
#define LARCOVERAGEALG_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"


#include "StoreGate/ReadHandleKey.h"
#include "CaloIdentifier/CaloGain.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloGeoHelpers/CaloPhiRange.h"

#include "Identifier/HWIdentifier.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "LArRecConditions/ILArBadChannelMasker.h"
#include "LArRecConditions/LArBadChannelCont.h"
#include "CaloConditions/CaloNoise.h"


class LArEM_ID;
class CaloDetDescrElement;


class LArCoverageAlg: public AthMonitorAlgorithm
{
 public:
  LArCoverageAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /** @brief Default destructor */
  virtual ~LArCoverageAlg();

  /** @brief Overwrite dummy method from AlgTool */
  StatusCode initialize() override;

  /** Called each event */
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;



 private:

  class LArChanHelp {
  public:
  LArChanHelp(int chan=-1,int ft_slot=-100,int sampling=-1,double eta=-100,double phi=-100): m_channelNumber(chan), m_channelFtSlot(ft_slot), m_channelSampling(sampling), m_channelEta(eta), m_channelPhi(phi) {};
    ~LArChanHelp() {};

    void setChanPhi(double phi){m_channelPhi=phi;};
    int getChNumber() const { return m_channelNumber; };    
    int getChFtSlot() const { return m_channelFtSlot; };
    int getChSampling() const { return m_channelSampling; };
    double getChEta() const { return m_channelEta; };
    double getChPhi() const { return m_channelPhi; };

  private:
    int m_channelNumber;
    int m_channelFtSlot;
    int m_channelSampling;
    double m_channelEta;
    double m_channelPhi;
  };

  const LArOnlineID* m_LArOnlineIDHelper;
  const LArEM_ID*    m_LArEM_IDHelper;
  const LArFCAL_ID*  m_LArFCAL_IDHelper;
  const LArHEC_ID*   m_LArHEC_IDHelper;
  const CaloIdManager*       m_caloIdMgr;


  /** Handle to LArOnOffIdMapping (former LArCablingService) */
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};

  /** Key for CaloNoise */
  SG::ReadCondHandleKey<CaloNoise> m_noiseCDOKey{this,"CaloNoiseKey","electronicNoise","SG Key of CaloNoise data object"};

  /** Handle to bad-channel tools */
  ToolHandle<ILArBadChannelMasker> m_badChannelMask{this, "LArBadChannelMask", "BadLArRawChannelMask"} ;

  SG::ReadHandleKey<LArRawChannelContainer> m_rawChannelsKey{this, "LArRawChannelKey", "LArRawChannels", "SG Key of raw channels"};
  SG::ReadCondHandleKey<LArBadChannelCont> m_BCKey{this, "BadChanKey", "LArBadChannel", "SG bad channels key"};
  SG::ReadCondHandleKey<LArBadFebCont> m_BFKey{this, "MFKey", "LArBadFeb", "SG missing FEBs key"};


  /** To retrieve bad channel DB keywords  */
  int DBflag(HWIdentifier) const;

  /** Properties */
  Gaudi::Property<EventContext::ContextEvt_t> m_nevents {this,"Nevents",50};
  Gaudi::Property<int> m_Nchannels {this,"Nchannels",128}; 
  Gaudi::Property<int> m_Nsample {this,"Nsample",4};
  Gaudi::Property<int> m_NftEMB {this,"NftEMB",32};
  Gaudi::Property<int> m_NslotEMB {this,"NslotEMB",14};
  Gaudi::Property<int> m_NftEMEC {this,"NftEMEC",25};
  Gaudi::Property<int> m_NslotEMEC {this,"NslotEMEC",15};
  Gaudi::Property<int> m_NftHEC {this,"NftHEC",25};
  Gaudi::Property<int> m_NslotHEC {this,"NslotHEC",15};
  Gaudi::Property<int> m_NftFCAL {this,"NftFCAL",25};
  Gaudi::Property<int> m_NslotFCAL {this,"NslotFCAL",15};
  Gaudi::Property<int> m_NphiBinsEMB1 {this,"NphiBinsEMB1",256};
  Gaudi::Property<int> m_NphiBinsEMEC2 {this,"NphiBinsEMEC2",256};
  Gaudi::Property<std::vector<int> > m_NphiBinsHEC {this,"NphiBinsHEC",{64,64,64,64}};


  Gaudi::Property<std::string> m_CaloNoiseToolGroupName {this,"CaloNoiseToolGroupName","CaloNoise"};
  Gaudi::Property<std::string> m_BadChannelsGroupName {this,"BadChannelsGroupName","BadChannels"};
  Gaudi::Property<std::string> m_CoverageHWGroupName {this,"CoverageHWGroupName","CoverageHW"};
  Gaudi::Property< std::vector<std::string> > m_CoverageBarrelPartitions {this, "CoverageBarrelPartitions", {"EMBA","EMBC"}};
  Gaudi::Property< std::vector<std::string> > m_CoverageEndcapPartitions {this, "CoverageEndcapPartitions", {"EMECA","EMECC","HECA","HECC","FCalA","FCalC"}};
  Gaudi::Property< std::vector<std::string> > m_Sides {this, "Sides", {"A","C"}};
  Gaudi::Property< std::vector<std::string> > m_availableErrorCodes {this, "AvailableErrorCodes", {"0","1","2","3","4"}};

  /** for tools array */
  std::vector<int> m_CaloNoiseGroupArrEM;
  std::vector<int> m_CaloNoiseGroupArrHEC;
  std::vector<int> m_CaloNoiseGroupArrFCAL;

  std::map<std::string,int> m_CoverageToolArrayEMBA;
  std::map<std::string,int> m_CoverageToolArrayEMECA;
  std::map<std::string,int> m_CoverageToolArrayHECA;
  std::map<std::string,int> m_CoverageToolArrayFCalA;
  std::map<std::string,int> m_CoverageToolArrayEMBC;
  std::map<std::string,int> m_CoverageToolArrayEMECC;
  std::map<std::string,int> m_CoverageToolArrayHECC;
  std::map<std::string,int> m_CoverageToolArrayFCalC;

  std::map<std::string,int> m_BadChannelToolArrayBarrel;
  std::map<std::string,int> m_BadChannelToolArrayEndcap;


  /** for CaloNoise*/
  const std::array<CaloGain::CaloGain,CaloCell_Base_ID::NSUBCALO> m_highestGain{ 
    CaloGain::LARHIGHGAIN,     //LAREM
    CaloGain::LARMEDIUMGAIN,   //LARHEC
    CaloGain::LARHIGHGAIN,     //LARFCAL
    CaloGain::TILEHIGHHIGH,    //TILE
    CaloGain::LARHIGHGAIN      //LARMINIFCAL   
   };
};

#endif
