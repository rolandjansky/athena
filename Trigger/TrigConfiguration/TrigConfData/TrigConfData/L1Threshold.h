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
      L1Threshold_EM( const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> extraInfo, const ptree & data) :
         L1Threshold_Calo(name, type, extraInfo, data) { load(); }
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
      L1Threshold_TAU( const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> extraInfo, const ptree & data) :
         L1Threshold_Calo(name, type, extraInfo, data) { load(); }
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
      L1Threshold_JET( const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> extraInfo, const ptree & data) :
         L1Threshold_Calo(name, type, extraInfo, data) { load(); };
      virtual ~L1Threshold_JET() = default;
      virtual std::string className() const override { return "L1Threshold_JET"; }
      unsigned int window(int eta = 0) const;
   protected:
      virtual void update() override {
         L1Threshold_Calo::update();
         load();
      }
   private:
      ValueWithEtaDependence<unsigned int> m_etaDepWindow{""}; ///< eta-dependent threshold value in MeV
      void load();
   };

   class L1Threshold_XE final : public L1Threshold_Calo {
   public:
      L1Threshold_XE( const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> extraInfo, const ptree & data) :
         L1Threshold_Calo(name, type, extraInfo, data) {};
      virtual ~L1Threshold_XE() = default;
      virtual std::string className() const override { return "L1Threshold_XE"; }
   };

   class L1Threshold_XS final : public L1Threshold_Calo {
   public:
      L1Threshold_XS( const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> extraInfo, const ptree & data) :
         L1Threshold_Calo(name, type, extraInfo, data) {};
      virtual ~L1Threshold_XS() = default;
      virtual std::string className() const override { return "L1Threshold_XS"; }
   };

   class L1Threshold_TE final : public L1Threshold_Calo {
   public:
      L1Threshold_TE( const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> extraInfo, const ptree & data) :
         L1Threshold_Calo(name, type, extraInfo, data) {};
      virtual ~L1Threshold_TE() = default;
      virtual std::string className() const override { return "L1Threshold_TE"; }
   };

   /************************************
    *
    *  ZB, NIM and internal thresholds
    *
    ************************************/
   class L1Threshold_ZB final : public L1Threshold {
   public:
      L1Threshold_ZB( const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> extraInfo, const ptree & data) :
         L1Threshold(name, type, extraInfo, data) { load(); };
      virtual ~L1Threshold_ZB() = default;
      virtual std::string className() const override { return "L1Threshold_ZB"; }
      const std::string & seed() const { return m_seed; }
      unsigned int seedBcdelay() const { return m_seedBcdelay; }
      unsigned int seedMultiplicity() const { return m_seedMultiplicity; }
   protected:
      virtual void update() override { load(); }
   private:
      std::string m_seed{""};
      unsigned int m_seedBcdelay{0};
      unsigned int m_seedMultiplicity{1};
      void load();
   };

   class L1Threshold_NIM final : public L1Threshold {
   public:
      L1Threshold_NIM( const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> extraInfo, const ptree & data) :
         L1Threshold(name, type, extraInfo, data) {};
      virtual ~L1Threshold_NIM() = default;
      virtual std::string className() const override { return "L1Threshold_NIM"; }
   };

   class L1Threshold_internal final : public L1Threshold {
   public:
      L1Threshold_internal( const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> extraInfo, const ptree & data) :
         L1Threshold(name, type, extraInfo, data) {};
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
      L1Threshold_eEM( const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> extraInfo, const ptree & data) :
         L1Threshold_Calo(name, type, extraInfo, data) { load(); }
      virtual ~L1Threshold_eEM() = default;
      virtual std::string className() const override { return "L1Threshold_eEM"; }
      // access functions
      Selection::WP reta() const { return m_reta; }
      Selection::WP rhad() const { return m_rhad; }
      Selection::WP wstot() const { return m_wstot; }
   protected:
      virtual void update() override {
         L1Threshold_Calo::update();
         load();
      }
   private:
      void load();
      // the isolation requirement
      Selection::WP m_reta { Selection::WP::NONE };
      Selection::WP m_rhad { Selection::WP::NONE };
      Selection::WP m_wstot { Selection::WP::NONE };
   };

   class L1Threshold_jEM final : public L1Threshold_Calo {
   public:
      L1Threshold_jEM( const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> extraInfo, const ptree & data) :
         L1Threshold_Calo(name, type, extraInfo, data) { load(); }
      virtual ~L1Threshold_jEM() = default;
      virtual std::string className() const override { return "L1Threshold_jEM"; }
      // access functions
      Selection::WP iso() const { return m_iso; }
      Selection::WP frac() const { return m_frac; }
      Selection::WP frac2() const { return m_frac2; }
   protected:
      virtual void update() override {
         L1Threshold_Calo::update();
         load();
      }
   private:
      void load();
      // the isolation requirement
      Selection::WP m_iso { Selection::WP::NONE };
      Selection::WP m_frac { Selection::WP::NONE };
      Selection::WP m_frac2 { Selection::WP::NONE };
   };
      
   class L1Threshold_eTAU final : public L1Threshold_Calo {
   public:
      L1Threshold_eTAU( const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> extraInfo, const ptree & data) :
         L1Threshold_Calo(name, type, extraInfo, data) { load(); }
      virtual ~L1Threshold_eTAU() = default;
      virtual std::string className() const override { return "L1Threshold_eTAU"; }
      // access functions
      Selection::WP rCore() const { return m_rCore; }
      Selection::WP rHad() const { return m_rHad; }
   protected:
      virtual void update() override {
         L1Threshold_Calo::update();
         load();
      }
   private:
      void load();
      // the isolation requirement
      Selection::WP m_rCore { Selection::WP::NONE };
      Selection::WP m_rHad { Selection::WP::NONE };
   };

   class L1Threshold_jTAU final : public L1Threshold_Calo {
   public:
      L1Threshold_jTAU( const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> extraInfo, const ptree & data) :
         L1Threshold_Calo(name, type, extraInfo, data) { load(); }
      virtual ~L1Threshold_jTAU() = default;
      virtual std::string className() const override { return "L1Threshold_jTAU"; }
      // access functions
      Selection::WP isolation() const { return m_isolation; }
   protected:
      virtual void update() override {
         L1Threshold_Calo::update();
         load();
      }
   private:
      void load();
      // the isolation requirement
      Selection::WP m_isolation { Selection::WP::NONE };
   };

   class L1Threshold_cTAU final : public L1Threshold_Calo {
   public:
      L1Threshold_cTAU( const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> extraInfo, const ptree & data) :
         L1Threshold_Calo(name, type, extraInfo, data) { load(); }
      virtual ~L1Threshold_cTAU() = default;
      virtual std::string className() const override { return "L1Threshold_cTAU"; }
      // access functions
      Selection::WP isolation() const { return m_isolation; }
   protected:
      virtual void update() override {
         L1Threshold_Calo::update();
         load();
      }
   private:
      void load();
      // the isolation requirement
      Selection::WP m_isolation { Selection::WP::NONE };
   };

   class L1Threshold_jJ final : public L1Threshold_Calo {
   public:
      L1Threshold_jJ( const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> extraInfo, const ptree & data) :
         L1Threshold_Calo(name, type, extraInfo, data) { load(); }
      virtual ~L1Threshold_jJ() = default;
      virtual std::string className() const override { return "L1Threshold_jJ"; }
   protected:
      virtual void update() override {
         L1Threshold_Calo::update();
         load();
      }
   private:
      void load();
   };

   class L1Threshold_jLJ final : public L1Threshold_Calo {
   public:
      L1Threshold_jLJ( const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> extraInfo, const ptree & data) :
         L1Threshold_Calo(name, type, extraInfo, data) { load(); }
      virtual ~L1Threshold_jLJ() = default;
      virtual std::string className() const override { return "L1Threshold_jLJ"; }
   protected:
      virtual void update() override {
         L1Threshold_Calo::update();
         load();
      }
   private:
      void load();
   };

   class L1Threshold_gJ final : public L1Threshold_Calo {
   public:
      L1Threshold_gJ( const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> extraInfo, const ptree & data) :
         L1Threshold_Calo(name, type, extraInfo, data) { load(); }
      virtual ~L1Threshold_gJ() = default;
      virtual std::string className() const override { return "L1Threshold_gJ"; }
   protected:
      virtual void update() override {
         L1Threshold_Calo::update();
         load();
      }
   private:
      void load();
   };

   class L1Threshold_gLJ final : public L1Threshold_Calo {
   public:
      L1Threshold_gLJ( const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> extraInfo, const ptree & data) :
         L1Threshold_Calo(name, type, extraInfo, data) { load(); }
      virtual ~L1Threshold_gLJ() = default;
      virtual std::string className() const override { return "L1Threshold_gLJ"; }
   protected:
      virtual void update() override {
         L1Threshold_Calo::update();
         load();
      }
   private:
      void load();
   };

   class L1Threshold_jXE final : public L1Threshold_Calo {
   public:
      L1Threshold_jXE( const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> extraInfo, const ptree & data) :
         L1Threshold_Calo(name, type, extraInfo, data) { load(); }
      virtual ~L1Threshold_jXE() = default;
      virtual std::string className() const override { return "L1Threshold_jXE"; }
   protected:
      virtual void update() override {
         L1Threshold_Calo::update();
         load();
      }
   private:
      void load();
   };

   class L1Threshold_jTE final : public L1Threshold_Calo {
   public:
      L1Threshold_jTE( const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> extraInfo, const ptree & data) :
         L1Threshold_Calo(name, type, extraInfo, data) { load(); }
      virtual ~L1Threshold_jTE() = default;
      virtual std::string className() const override { return "L1Threshold_jTE"; }
   protected:
      virtual void update() override {
         L1Threshold_Calo::update();
         load();
      }
   private:
      void load();
   };

   class L1Threshold_gXE final : public L1Threshold_Calo {
   public:
      L1Threshold_gXE( const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> extraInfo, const ptree & data) :
         L1Threshold_Calo(name, type, extraInfo, data) { load(); }
      virtual ~L1Threshold_gXE() = default;
      virtual std::string className() const override { return "L1Threshold_gXE"; }
   protected:
      virtual void update() override { 
         L1Threshold_Calo::update();
         load();
      }  
   private:
      void load();
   };

   class L1Threshold_gTE final : public L1Threshold_Calo {
   public:
      L1Threshold_gTE( const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> extraInfo, const ptree & data) :
         L1Threshold_Calo(name, type, extraInfo, data) { load(); }
      virtual ~L1Threshold_gTE() = default;
      virtual std::string className() const override { return "L1Threshold_gTE"; }
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
      L1Threshold_MU( const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> extraInfo, const ptree & data) :
         L1Threshold(name, type, extraInfo, data) { load(); }
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
      const std::string & tgcFlags() const { return m_tgcFlags; }
      const std::string & rpcFlags() const { return m_rpcFlags; }
      const std::string & rpcExclROIList() const { return m_rpcExclROIList; }
      std::optional<std::string> rpcExclROIList_optional() const {
         return m_rpcExclROIList.empty() ? std::nullopt : std::optional<std::string>{m_rpcExclROIList};
      }
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
      std::string m_tgcFlags{""}; ///< a logical expression like 'F & C | F & H | C & H'
      std::string m_rpcFlags{""}; ///< a logical expression like 'M'
      std::string m_rpcExclROIList{""}; ///< a string sepcifying the list of ROIs to be excluded (the lists are defined in the extraInfo_MU)
   };


}

#endif
