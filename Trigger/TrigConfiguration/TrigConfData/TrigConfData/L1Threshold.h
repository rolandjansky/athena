/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFDATA_L1THRESHOLD_H
#define TRIGCONFDATA_L1THRESHOLD_H

#include "TrigConfData/L1ThresholdBase.h"

namespace TrigConf {

   /************************************
    *
    *  L1Calo legacy thresholds
    *
    ************************************/
   class L1Threshold_EM final : public L1Threshold {
   public:
      L1Threshold_EM( const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> m_extraInfo, const ptree & data);
      virtual ~L1Threshold_EM() = default;
      unsigned int thrValueCounts(int eta = 0) const override;
      uint16_t isolationMask(int eta) const { return m_isolationMask.at(eta); }
      void print(std::ostream & os = std::cout) const override;
   private:
      virtual void update() override;
      ValueWithEtaDependence<uint16_t> m_isolationMask{""};
   };

   class L1Threshold_TAU final : public L1Threshold {
   public:
      L1Threshold_TAU( const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> m_extraInfo, const ptree & data);
      virtual ~L1Threshold_TAU() = default;
      uint16_t isolationMask() const { return m_isolationMask; }
   private:
      virtual void update() override;
      uint16_t m_isolationMask;
   };


   /************************************
    *
    *  New eFEX thresholds
    *
    ************************************/
   class L1Threshold_eEM final : public L1Threshold {
   public:
      L1Threshold_eEM() = default;
      L1Threshold_eEM( const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> m_extraInfo, const ptree & data);
      virtual ~L1Threshold_eEM() = default;
      Isolation::WP reta() const { return m_reta; }
      Isolation::WP rhad() const { return m_rhad; }
      Isolation::WP wstot() const { return m_wstot; }      
   private:
      virtual void update() override;
      // the isolation requirement
      Isolation::WP m_reta { Isolation::WP::NONE };
      Isolation::WP m_rhad { Isolation::WP::NONE };
      Isolation::WP m_wstot { Isolation::WP::NONE };
   };



   /************************************
    *
    *  Muon threshold
    *
    ************************************/
   class L1Threshold_MU final : public L1Threshold {
   public:
      L1Threshold_MU() = default;
      L1Threshold_MU( const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> m_extraInfo, const ptree & data);
      virtual ~L1Threshold_MU() = default;
      unsigned int ptBarrel() const { return m_ptBarrel; }
      unsigned int ptEndcap() const { return m_ptEndcap; }
      unsigned int ptForward() const { return m_ptForward; }
      unsigned int idxBarrel() const { return m_idxBarrel; }
      unsigned int idxEndcap() const { return m_idxEndcap; }
      unsigned int idxForward() const { return m_idxForward; }
      const std::string & region() const { return m_region; }
      const std::string & tgcFlag() const { return m_tgcFlag; }
      const std::string & rpcExclROIList() const { return m_rpcExclROIList; }
   private:
      virtual void update() override;
      unsigned int m_ptBarrel{0};
      unsigned int m_ptEndcap{0};
      unsigned int m_ptForward{0};
      unsigned int m_idxBarrel{0};
      unsigned int m_idxEndcap{0};
      unsigned int m_idxForward{0};
      // the isolation requirement
      std::string m_region{""}; ///< comma-separated list of BA, EC, FW or the string ALL
      std::string m_tgcFlag{""}; ///< a logical expression like 'F & C | F & H | C & H'
      std::string m_rpcExclROIList{""}; ///< a string sepcifying the list of ROIs to be excluded (the lists are defined in the extraInfo_MU)
   };


}

#endif
