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
   class L1Threshold_EM final : public L1Threshold_Calo {
   public:
      L1Threshold_EM( const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> m_extraInfo, const ptree & data) :
         L1Threshold_Calo(name, type, m_extraInfo, data) { load(); }
      virtual ~L1Threshold_EM() = default;
      // class name
      virtual std::string className() const override { return "L1Threshold_EM"; }
      uint16_t isolationMask(int eta) const { return m_isolationMask.at(eta); }
      void print(std::ostream & os = std::cout) const override;
   protected:
      virtual void update() override {
         L1Threshold_Calo::update();
         load();
      }
   private:
      void load();
      ValueWithEtaDependence<uint16_t> m_isolationMask{""};
   };


   class L1Threshold_TAU final : public L1Threshold_Calo {
   public:
      L1Threshold_TAU( const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> m_extraInfo, const ptree & data) :
         L1Threshold_Calo(name, type, m_extraInfo, data) { load(); }
      virtual ~L1Threshold_TAU() = default;
      virtual std::string className() const override { return "L1Threshold_TAU"; }
      // access functions
      uint16_t isolationMask() const { return m_isolationMask; }
   protected:
      virtual void update() override {
         L1Threshold_Calo::update();
         load();
      }
   private:
      void load();
      uint16_t m_isolationMask;
   };

   class L1Threshold_JET final : public L1Threshold_Calo {
   public:
      L1Threshold_JET( const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> m_extraInfo, const ptree & data) :
         L1Threshold_Calo(name, type, m_extraInfo, data) {};
      virtual ~L1Threshold_JET() = default;
      virtual std::string className() const override { return "L1Threshold_JET"; }
   };

   class L1Threshold_XE final : public L1Threshold_Calo {
   public:
      L1Threshold_XE( const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> m_extraInfo, const ptree & data) :
         L1Threshold_Calo(name, type, m_extraInfo, data) {};
      virtual ~L1Threshold_XE() = default;
      virtual std::string className() const override { return "L1Threshold_XE"; }
   };

   class L1Threshold_XS final : public L1Threshold_Calo {
   public:
      L1Threshold_XS( const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> m_extraInfo, const ptree & data) :
         L1Threshold_Calo(name, type, m_extraInfo, data) {};
      virtual ~L1Threshold_XS() = default;
      virtual std::string className() const override { return "L1Threshold_XS"; }
   };

   class L1Threshold_TE final : public L1Threshold_Calo {
   public:
      L1Threshold_TE( const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> m_extraInfo, const ptree & data) :
         L1Threshold_Calo(name, type, m_extraInfo, data) {};
      virtual ~L1Threshold_TE() = default;
      virtual std::string className() const override { return "L1Threshold_TE"; }
   };

   /************************************
    *
    *  NIM and internal thresholds
    *
    ************************************/

   class L1Threshold_NIM final : public L1Threshold {
   public:
      L1Threshold_NIM( const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> m_extraInfo, const ptree & data) :
         L1Threshold(name, type, m_extraInfo, data) {};
      virtual ~L1Threshold_NIM() = default;
      virtual std::string className() const override { return "L1Threshold_NIM"; }
   };

   class L1Threshold_internal final : public L1Threshold {
   public:
      L1Threshold_internal( const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> m_extraInfo, const ptree & data) :
         L1Threshold(name, type, m_extraInfo, data) {};
      virtual ~L1Threshold_internal() = default;
      virtual std::string className() const override { return "L1Threshold_internal"; }
   };

   /************************************
    *
    *  New eFEX thresholds
    *
    ************************************/
   class L1Threshold_eEM final : public L1Threshold_Calo {
   public:
      L1Threshold_eEM( const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> m_extraInfo, const ptree & data) :
         L1Threshold_Calo(name, type, m_extraInfo, data) { load(); }
      virtual ~L1Threshold_eEM() = default;
      virtual std::string className() const override { return "L1Threshold_eEM"; }
      // access functions
      Isolation::WP reta() const { return m_reta; }
      Isolation::WP rhad() const { return m_rhad; }
      Isolation::WP wstot() const { return m_wstot; }      
   protected:
      virtual void update() override {
         L1Threshold_Calo::update();
         load();
      }
   private:
      void load();
      // the isolation requirement
      Isolation::WP m_reta { Isolation::WP::NONE };
      Isolation::WP m_rhad { Isolation::WP::NONE };
      Isolation::WP m_wstot { Isolation::WP::NONE };
   };

   class L1Threshold_eTAU final : public L1Threshold_Calo {
   public:
      L1Threshold_eTAU( const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> m_extraInfo, const ptree & data) :
         L1Threshold_Calo(name, type, m_extraInfo, data) { load(); }
      virtual ~L1Threshold_eTAU() = default;
      virtual std::string className() const override { return "L1Threshold_eTAU"; }
   protected:
      virtual void update() override {
         L1Threshold_Calo::update();
         load();
      }
   private:
      void load();
   };






   /************************************
    *
    *  Muon threshold
    *
    ************************************/
   class L1Threshold_MU final : public L1Threshold {
   public:
      L1Threshold_MU( const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> m_extraInfo, const ptree & data) :
         L1Threshold(name, type, m_extraInfo, data) { load(); }
      virtual ~L1Threshold_MU() = default;
      virtual std::string className() const override { return "L1Threshold_MU"; }

      float thrValue(int eta = 0) const override;

      unsigned int ptBarrel() const { return m_ptBarrel; }
      unsigned int ptEndcap() const { return m_ptEndcap; }
      unsigned int ptForward() const { return m_ptForward; }
      unsigned int idxBarrel() const { return m_idxBarrel; }
      unsigned int idxEndcap() const { return m_idxEndcap; }
      unsigned int idxForward() const { return m_idxForward; }
      const std::string & region() const { return m_region; }
      const std::string & tgcFlag() const { return m_tgcFlag; }
      const std::string & rpcExclROIList() const { return m_rpcExclROIList; }
   protected:
      virtual void update() override {
         L1Threshold::update();
         load();
      }
   private:
      void load();

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
