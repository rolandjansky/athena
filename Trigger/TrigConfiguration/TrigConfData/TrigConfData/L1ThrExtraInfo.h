/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFDATA_L1THREXTRAINFO_H
#define TRIGCONFDATA_L1THREXTRAINFO_H

#include "TrigConfData/DataStructure.h"
#include "TrigConfData/L1ThresholdBase.h"

#include <map>
#include <vector>
#include <iostream>

namespace TrigConf {

   class L1ThrExtraInfoBase;
   class L1ThrExtraInfo_EMTAULegacy;
   class L1ThrExtraInfo_JETLegacy;
   class L1ThrExtraInfo_XSLegacy;
   class L1ThrExtraInfo_eEM;
   class L1ThrExtraInfo_jEM;
   class L1ThrExtraInfo_eTAU;
   class L1ThrExtraInfo_jTAU;
   class L1ThrExtraInfo_cTAU;
   class L1ThrExtraInfo_jJ;
   class L1ThrExtraInfo_jLJ;
   class L1ThrExtraInfo_gJ;
   class L1ThrExtraInfo_gLJ;
   class L1ThrExtraInfo_gXE;
   class L1ThrExtraInfo_jXE;
   class L1ThrExtraInfo_gTE;
   class L1ThrExtraInfo_jTE;
   class L1ThrExtraInfo_MU;

   class L1ThrExtraInfo {
   public:
      static std::unique_ptr<L1ThrExtraInfoBase> createExtraInfo(const std::string & thrTypeName, const boost::property_tree::ptree & data);

      std::weak_ptr<TrigConf::L1ThrExtraInfoBase> addExtraInfo(const std::string & thrTypeName, const boost::property_tree::ptree & data);

      const L1ThrExtraInfo_EMTAULegacy & EM() const;
      const L1ThrExtraInfo_EMTAULegacy & TAU() const;
      const L1ThrExtraInfo_JETLegacy & JET() const;
      const L1ThrExtraInfo_XSLegacy & XS() const;
      const L1ThrExtraInfo_eEM & eEM() const;
      const L1ThrExtraInfo_jEM & jEM() const;
      const L1ThrExtraInfo_eTAU & eTAU() const;
      const L1ThrExtraInfo_jTAU & jTAU() const;
      const L1ThrExtraInfo_cTAU & cTAU() const;
      const L1ThrExtraInfo_jJ & jJ() const;
      const L1ThrExtraInfo_gJ & gJ() const;
      const L1ThrExtraInfo_gLJ & gLJ() const;
      const L1ThrExtraInfo_jLJ & jLJ() const;
      const L1ThrExtraInfo_gXE & gXE() const;
      const L1ThrExtraInfo_jXE & jXE() const;
      const L1ThrExtraInfo_gTE & gTE() const;
      const L1ThrExtraInfo_jTE & jTE() const;
      const L1ThrExtraInfo_MU & MU() const;

      const L1ThrExtraInfoBase & thrExtraInfo(const std::string & thrTypeName) const;

      bool hasInfo(const std::string & typeName) const;

      void clear();

   private:
      // map from threshold type to L1ThrExtraInfo object
      std::map<std::string, std::shared_ptr<TrigConf::L1ThrExtraInfoBase>> m_thrExtraInfo{};
      std::shared_ptr<TrigConf::L1ThrExtraInfoBase> m_emptyInfo{ nullptr };
   };



   class L1ThrExtraInfo_EMTAULegacy final : public L1ThrExtraInfoBase {
   public:
      L1ThrExtraInfo_EMTAULegacy(const std::string & thrTypeName, const ptree & data) :
         L1ThrExtraInfoBase(thrTypeName, data) { load(); }
      virtual ~L1ThrExtraInfo_EMTAULegacy() = default;
      virtual std::string className() const { return "L1ThrExtraInfo_EMTAULegacy"; }
      unsigned int emScale() const { return 1000 / resolutionMeV(); }
      float ptMinToTopo() const { return m_ptMinToTopoMeV/1000.0f; }
      unsigned int ptMinToTopoMeV() const { return m_ptMinToTopoMeV; }
      unsigned int ptMinToTopoCounts() const { return energyInCounts( m_ptMinToTopoMeV, resolutionMeV() ); }
      const IsolationLegacy & isolation(const std::string & thrType, size_t bit) const;
   private:
      /** Update the internal members */
      void load();
      /** EM specific data */
      unsigned int m_ptMinToTopoMeV{0}; // in MeV
      std::map<std::string,std::vector<IsolationLegacy>> m_isolation{};
   };


   class L1ThrExtraInfo_JETLegacy final : public L1ThrExtraInfoBase {
   public:
      L1ThrExtraInfo_JETLegacy(const std::string & thrTypeName, const ptree & data) :
         L1ThrExtraInfoBase(thrTypeName, data) { load(); }
      virtual ~L1ThrExtraInfo_JETLegacy() = default;
      virtual std::string className() const { return "L1ThrExtraInfo_JETLegacy"; }
      unsigned int jetScale() const { return 1000 / resolutionMeV(); }
      double ptMinToTopoLargeWindow() const { return m_ptMinToTopoLargeWindowMeV / 1000.0; }
      double ptMinToTopoSmallWindow() const { return m_ptMinToTopoSmallWindowMeV / 1000.0; }
      unsigned int ptMinToTopoLargeWindowMeV() const { return m_ptMinToTopoLargeWindowMeV; }
      unsigned int ptMinToTopoSmallWindowMeV() const { return m_ptMinToTopoSmallWindowMeV; }
      unsigned int ptMinToTopoLargeWindowCounts() const { return energyInCounts( m_ptMinToTopoLargeWindowMeV, resolutionMeV() ); }
      unsigned int ptMinToTopoSmallWindowCounts() const { return energyInCounts( m_ptMinToTopoSmallWindowMeV, resolutionMeV() ); }
   private:
      /** Update the internal members */
      void load();
      /** JET specific data */
      unsigned int m_ptMinToTopoLargeWindowMeV{0};
      unsigned int m_ptMinToTopoSmallWindowMeV{0};
   };


   class L1ThrExtraInfo_XSLegacy final : public L1ThrExtraInfoBase {
   public:
      L1ThrExtraInfo_XSLegacy(const std::string & thrTypeName, const ptree & data) :
         L1ThrExtraInfoBase(thrTypeName, data) { load(); }
      virtual ~L1ThrExtraInfo_XSLegacy() = default;
      virtual std::string className() const { return "L1ThrExtraInfo_XSLegacy"; }
      unsigned int xeMin() const { return m_xeMin; };
      unsigned int xeMax() const { return m_xeMax; };
      unsigned int teSqrtMin() const { return m_teSqrtMin; };
      unsigned int teSqrtMax() const { return m_teSqrtMax; };
      unsigned int xsSigmaScale() const { return m_xsSigmaScale; };
      unsigned int xsSigmaOffset() const { return m_xsSigmaOffset; };
   private:
      /** Update the internal members */
      void load();
      /** XS specific data */
      unsigned int m_xeMin{0};
      unsigned int m_xeMax{0};
      unsigned int m_teSqrtMin{0};
      unsigned int m_teSqrtMax{0};
      unsigned int m_xsSigmaScale{0}; 
      unsigned int m_xsSigmaOffset{0};
   };


   /***********************************
    * Extra info for new thresholds
    ***********************************/
   class L1ThrExtraInfo_eEM final : public L1ThrExtraInfoBase {
   public:
      class WorkingPoints_eEM {
      public:
         WorkingPoints_eEM() = default;
         WorkingPoints_eEM( const boost::property_tree::ptree & );
         bool isDefined() const { return m_isDefined; } 
         int reta_fw()       const { return m_reta_fw; } 
         int wstot_fw()      const { return m_wstot_fw; }
         int rhad_fw()       const { return m_rhad_fw; }
         float reta_d()       const { return m_reta_d; } 
         float wstot_d()     const { return m_wstot_d; }
         float rhad_d()       const { return m_rhad_d; }
      private:
         bool m_isDefined { false };
         float m_reta_d { 0 };
         float m_wstot_d { 0 };
         float m_rhad_d { 0 };
         int   m_reta_fw { 0 };
         int   m_wstot_fw { 0 };
         int   m_rhad_fw { 0 };
      };
      L1ThrExtraInfo_eEM(const std::string & thrTypeName, const ptree & data) :
         L1ThrExtraInfoBase(thrTypeName, data) { load(); }
      virtual ~L1ThrExtraInfo_eEM() = default;
      virtual std::string className() const { return "L1ThrExtraInfo_eEM"; }
      unsigned int maxEtMeV()  const { return m_maxEt; }
      unsigned int maxEtCounts(const unsigned int resolutionMeV)  const { return TrigConf::energyInCounts( m_maxEt, resolutionMeV ); }
      float maxEt()  const { return m_maxEt/1000.0f; }
      float ptMinToTopo() const { return m_ptMinToTopoMeV/1000.0f; }
      unsigned int ptMinToTopoMeV() const { return m_ptMinToTopoMeV; }
      unsigned int ptMinToTopoCounts() const { return energyInCounts( m_ptMinToTopoMeV, resolutionMeV() ); }
      const WorkingPoints_eEM & isolation(TrigConf::Selection::WP wp, int eta) const { return m_isolation.at(wp).at(eta); }
      const ValueWithEtaDependence<WorkingPoints_eEM> & isolation(TrigConf::Selection::WP wp) const { return m_isolation.at(wp); }
   private:
      /** Update the internal members */
      void load();
      /** eEM specific data */
      unsigned int m_ptMinToTopoMeV{0};
      unsigned int m_maxEt { 0 };
      std::map<TrigConf::Selection::WP, ValueWithEtaDependence<WorkingPoints_eEM>> m_isolation{};
   };
   std::ostream & operator<<(std::ostream & os, const TrigConf::L1ThrExtraInfo_eEM::WorkingPoints_eEM & iso);

   class L1ThrExtraInfo_jEM final : public L1ThrExtraInfoBase {
   public:
      class WorkingPoints_jEM {
      public:
         WorkingPoints_jEM() = default;
         WorkingPoints_jEM( const boost::property_tree::ptree & );
         bool isDefined() const { return m_isDefined; }
         int iso_fw()       const { return m_iso_fw; }
         int frac_fw()      const { return m_frac_fw; }
         int frac2_fw()       const { return m_frac2_fw; }
         float iso_d()       const { return m_iso_d; }
         float frac_d()     const { return m_frac_d; }
         float frac2_d()       const { return m_frac2_d; }
      private:
         bool m_isDefined { false };
         float m_iso_d { 0 };
         float m_frac_d { 0 };
         float m_frac2_d { 0 };
         int   m_iso_fw { 0 };
         int   m_frac_fw { 0 };
         int   m_frac2_fw { 0 };
      };
      L1ThrExtraInfo_jEM(const std::string & thrTypeName, const ptree & data) :
         L1ThrExtraInfoBase(thrTypeName, data) { load(); }
      virtual ~L1ThrExtraInfo_jEM() = default;
      virtual std::string className() const { return "L1ThrExtraInfo_jEM"; }
      const WorkingPoints_jEM & isolation(TrigConf::Selection::WP wp, int eta) const { return m_isolation.at(wp).at(eta); }
      const ValueWithEtaDependence<WorkingPoints_jEM> & isolation(TrigConf::Selection::WP wp) const { return m_isolation.at(wp); }
      unsigned int maxEtMeV()  const { return m_maxEt; }
      unsigned int maxEtCounts(const unsigned int resolutionMeV)  const { return TrigConf::energyInCounts( m_maxEt, resolutionMeV ); }
      float maxEt()  const { return m_maxEt/1000.0f; }
      float ptMinToTopo(const std::string& module) const { return ptMinToTopoMeV(module)/ 1000.0; }
      unsigned int ptMinToTopoCounts(const std::string& module) const { return energyInCounts(ptMinToTopoMeV(module), resolutionMeV()); }
      unsigned int ptMinToTopoMeV(const std::string& module) const {
          if(module=="1C" || module=="1A") return m_ptMinToTopoMeV1;
          if(module=="2C" || module=="2A") return m_ptMinToTopoMeV2;
          if(module=="3C" || module=="3A") return m_ptMinToTopoMeV3;
          throw std::runtime_error("L1ThrExtraInfo: Module" + module + " not recongnised for jEM ptMinToTopo");
      }
      float ptMinxTOB(const std::string& module) const { return ptMinxTOBMeV(module)/ 1000.0; }
      unsigned int ptMinxTOBCounts(const std::string& module) const { return energyInCounts(ptMinxTOBMeV(module), resolutionMeV()); }
      unsigned int ptMinxTOBMeV(const std::string& module) const {
          if(module=="1C" || module=="1A") return m_ptMinxTOBMeV1;
          if(module=="2C" || module=="2A") return m_ptMinxTOBMeV2;
          if(module=="3C" || module=="3A") return m_ptMinxTOBMeV3;
          throw std::runtime_error("L1ThrExtraInfo: Module" + module + " not recongnised for jEM ptMinxTOB");
      }
   private:
      /** Update the internal members */
      void load();
      /** jEM specific data */
      unsigned int m_maxEt { 0 };
      unsigned int m_ptMinToTopoMeV1{0};
      unsigned int m_ptMinToTopoMeV2{0};
      unsigned int m_ptMinToTopoMeV3{0};
      unsigned int m_ptMinxTOBMeV1{0};
      unsigned int m_ptMinxTOBMeV2{0};
      unsigned int m_ptMinxTOBMeV3{0};
      std::map<TrigConf::Selection::WP, ValueWithEtaDependence<WorkingPoints_jEM>> m_isolation{};
   };
   std::ostream & operator<<(std::ostream & os, const TrigConf::L1ThrExtraInfo_jEM::WorkingPoints_jEM & iso);

   class L1ThrExtraInfo_eTAU final : public L1ThrExtraInfoBase {
   public:
      class WorkingPoints_eTAU {
      public:
         WorkingPoints_eTAU( const boost::property_tree::ptree & );
         bool isDefined() const { return m_isDefined; }
         int rCore_fw() const { return m_rCore_fw; }
         float rCore_d() const { return m_rCore_d; }
         int rHad_fw() const { return m_rHad_fw; }
         float rHad_d() const { return m_rHad_d; }
      private:
         bool m_isDefined { false };
         int m_rCore_fw {0};
         int m_rHad_fw {0};
         float m_rCore_d { 0 };
         float m_rHad_d { 0 };      
      };
      L1ThrExtraInfo_eTAU(const std::string & thrTypeName, const ptree & data) :
         L1ThrExtraInfoBase(thrTypeName, data) { load(); }
      virtual ~L1ThrExtraInfo_eTAU() = default;
      virtual std::string className() const { return "L1ThrExtraInfo_eTAU"; }
      unsigned int maxEtMeV()  const { return m_maxEt; }
      unsigned int maxEtCounts(const unsigned int resolutionMeV)  const { return TrigConf::energyInCounts( m_maxEt, resolutionMeV ); }
      float maxEt()  const { return m_maxEt/1000.0f; }
      float ptMinToTopo() const { return m_ptMinToTopoMeV/1000.0f; }
      unsigned int ptMinToTopoMeV() const { return m_ptMinToTopoMeV; }
      unsigned int ptMinToTopoCounts() const { return energyInCounts( m_ptMinToTopoMeV, resolutionMeV() ); }
      const WorkingPoints_eTAU & isolation(TrigConf::Selection::WP wp, int eta) const { return m_isolation.at(wp).at(eta); }
      const ValueWithEtaDependence<WorkingPoints_eTAU> & isolation(TrigConf::Selection::WP wp) const  { return m_isolation.at(wp); }
   private:
      /** Update the internal members */
      void load();
      /** eTAU specific data */
      unsigned int m_maxEt { 0 };
      unsigned int m_ptMinToTopoMeV{0};
      std::map<TrigConf::Selection::WP, ValueWithEtaDependence<WorkingPoints_eTAU>> m_isolation{};
   };
   std::ostream & operator<<(std::ostream & os, const TrigConf::L1ThrExtraInfo_eTAU::WorkingPoints_eTAU & iso);

   class L1ThrExtraInfo_jTAU final : public L1ThrExtraInfoBase {
   public:
      class WorkingPoints_jTAU {
      public:
         WorkingPoints_jTAU( const boost::property_tree::ptree & );
         bool isDefined() const { return m_isDefined; }
         int isolation_fw() const { return m_isolation_fw; }
         float isolation_d() const { return m_isolation_d; }
      private:
         bool m_isDefined { false };
         int m_isolation_fw {0};
         float m_isolation_d { 0 };
      };
      L1ThrExtraInfo_jTAU(const std::string & thrTypeName, const ptree & data) :
         L1ThrExtraInfoBase(thrTypeName, data) { load(); }
      virtual ~L1ThrExtraInfo_jTAU() = default;
      virtual std::string className() const { return "L1ThrExtraInfo_jTAU"; }
      const WorkingPoints_jTAU & isolation(TrigConf::Selection::WP wp, int eta) const { return m_isolation.at(wp).at(eta); }
      const ValueWithEtaDependence<WorkingPoints_jTAU> & isolation(TrigConf::Selection::WP wp) const  { return m_isolation.at(wp); }
      unsigned int maxEtMeV()  const { return m_maxEt; }
      unsigned int maxEtCounts(const unsigned int resolutionMeV)  const { return TrigConf::energyInCounts( m_maxEt, resolutionMeV ); }
      float maxEt()  const { return m_maxEt/1000.0f; }
      float ptMinToTopo(const std::string& module) const { return ptMinToTopoMeV(module)/ 1000.0; }
      unsigned int ptMinToTopoCounts(const std::string& module) const { return energyInCounts(ptMinToTopoMeV(module), resolutionMeV()); }
      unsigned int ptMinToTopoMeV(const std::string& module) const {
          if(module=="1C" || module=="1A") return m_ptMinToTopoMeV1;
          if(module=="2C" || module=="2A") return m_ptMinToTopoMeV2;
          if(module=="3C" || module=="3A") return m_ptMinToTopoMeV3;
          throw std::runtime_error("L1ThrExtraInfo: Module" + module + " not recongnised for jTAU ptMinToTopo");
      }
      float ptMinxTOB(const std::string& module) const { return ptMinxTOBMeV(module)/ 1000.0; }
      unsigned int ptMinxTOBCounts(const std::string& module) const { return energyInCounts(ptMinxTOBMeV(module), resolutionMeV()); }
      unsigned int ptMinxTOBMeV(const std::string& module) const {
          if(module=="1C" || module=="1A") return m_ptMinxTOBMeV1;
          if(module=="2C" || module=="2A") return m_ptMinxTOBMeV2;
          if(module=="3C" || module=="3A") return m_ptMinxTOBMeV3;
          throw std::runtime_error("L1ThrExtraInfo: Module" + module + " not recongnised for jTAU ptMinxTOB");
      }
   private:
      /** Update the internal members */
      void load();
     /** jTAU specific data */
      unsigned int m_maxEt { 0 };
      unsigned int m_ptMinToTopoMeV1{0};
      unsigned int m_ptMinToTopoMeV2{0};
      unsigned int m_ptMinToTopoMeV3{0};
      unsigned int m_ptMinxTOBMeV1{0};
      unsigned int m_ptMinxTOBMeV2{0};
      unsigned int m_ptMinxTOBMeV3{0};
      std::map<TrigConf::Selection::WP, ValueWithEtaDependence<WorkingPoints_jTAU>> m_isolation{};
   };
   std::ostream & operator<<(std::ostream & os, const TrigConf::L1ThrExtraInfo_jTAU::WorkingPoints_jTAU & iso);

   class L1ThrExtraInfo_cTAU final : public L1ThrExtraInfoBase {
   public:
      class WorkingPoints_cTAU {
      public:
         WorkingPoints_cTAU( const boost::property_tree::ptree & );
         bool isDefined() const { return m_isDefined; }
         int isolation_fw() const { return m_isolation_fw; }
         float isolation_d() const { return m_isolation_d; }
      private:
         bool m_isDefined { false };
         int m_isolation_fw {0};
         float m_isolation_d { 0 };
      };
      L1ThrExtraInfo_cTAU(const std::string & thrTypeName, const ptree & data) :
         L1ThrExtraInfoBase(thrTypeName, data) { load(); }
      virtual ~L1ThrExtraInfo_cTAU() = default;
      virtual std::string className() const { return "L1ThrExtraInfo_cTAU"; }
      const WorkingPoints_cTAU & isolation(TrigConf::Selection::WP wp, int eta) const { return m_isolation.at(wp).at(eta); }
      const ValueWithEtaDependence<WorkingPoints_cTAU> & isolation(TrigConf::Selection::WP wp) const  { return m_isolation.at(wp); }
   private:
      /** Update the internal members */
      void load();
     /** cTAU specific data */
      std::map<TrigConf::Selection::WP, ValueWithEtaDependence<WorkingPoints_cTAU>> m_isolation{};
   };
   std::ostream & operator<<(std::ostream & os, const TrigConf::L1ThrExtraInfo_cTAU::WorkingPoints_cTAU & iso);

   class L1ThrExtraInfo_jJ final : public L1ThrExtraInfoBase {
   public:
      L1ThrExtraInfo_jJ(const std::string & thrTypeName, const ptree & data) :
         L1ThrExtraInfoBase(thrTypeName, data) { load(); }
      virtual ~L1ThrExtraInfo_jJ() = default;
      virtual std::string className() const { return "L1ThrExtraInfo_jJ"; }
      float ptMinToTopo(const std::string& module) const { return ptMinToTopoMeV(module)/ 1000.0; }
      unsigned int ptMinToTopoCounts(const std::string& module) const { return energyInCounts(ptMinToTopoMeV(module), resolutionMeV()); }
      unsigned int ptMinToTopoMeV(const std::string& module) const {
          if(module=="1C" || module=="1A") return m_ptMinToTopoMeV1;
          if(module=="2C" || module=="2A") return m_ptMinToTopoMeV2;
          if(module=="3C" || module=="3A") return m_ptMinToTopoMeV3;
          throw std::runtime_error("L1ThrExtraInfo: Module" + module + " not recongnised for jJ ptMinToTopo");
      }
      float ptMinxTOB(const std::string& module) const { return ptMinxTOBMeV(module)/ 1000.0; }
      unsigned int ptMinxTOBCounts(const std::string& module) const { return energyInCounts(ptMinxTOBMeV(module), resolutionMeV()); }
      unsigned int ptMinxTOBMeV(const std::string& module) const {
          if(module=="1C" || module=="1A") return m_ptMinxTOBMeV1;
          if(module=="2C" || module=="2A") return m_ptMinxTOBMeV2;
          if(module=="3C" || module=="3A") return m_ptMinxTOBMeV3;
          throw std::runtime_error("L1ThrExtraInfo: Module" + module + " not recongnised for jJ ptMinxTOB");
      }
   private:
      /** Update the internal members */
      void load();
      /** jJ specific data */
      unsigned int m_ptMinToTopoMeV1{0};
      unsigned int m_ptMinToTopoMeV2{0};
      unsigned int m_ptMinToTopoMeV3{0};
      unsigned int m_ptMinxTOBMeV1{0};
      unsigned int m_ptMinxTOBMeV2{0};
      unsigned int m_ptMinxTOBMeV3{0};
   };

   class L1ThrExtraInfo_jLJ final : public L1ThrExtraInfoBase {
   public:
      L1ThrExtraInfo_jLJ(const std::string & thrTypeName, const ptree & data) :
         L1ThrExtraInfoBase(thrTypeName, data) { load(); }
      virtual ~L1ThrExtraInfo_jLJ() = default;
      virtual std::string className() const { return "L1ThrExtraInfo_jLJ"; }
      float ptMinToTopo(const std::string& module) const { return ptMinToTopoMeV(module)/ 1000.0; }
      unsigned int ptMinToTopoCounts(const std::string& module) const { return energyInCounts(ptMinToTopoMeV(module), resolutionMeV()); }
      unsigned int ptMinToTopoMeV(const std::string& module) const {
          if(module=="1C" || module=="1A") return m_ptMinToTopoMeV1;
          if(module=="2C" || module=="2A") return m_ptMinToTopoMeV2;
          if(module=="3C" || module=="3A") return m_ptMinToTopoMeV3;
          throw std::runtime_error("L1ThrExtraInfo: Module" + module + " not recongnised for jLJ ptMinToTopo");
      }
      float ptMinxTOB(const std::string& module) const { return ptMinxTOBMeV(module)/ 1000.0; }
      unsigned int ptMinxTOBCounts(const std::string& module) const { return energyInCounts(ptMinxTOBMeV(module), resolutionMeV()); }
      unsigned int ptMinxTOBMeV(const std::string& module) const {
          if(module=="1C" || module=="1A") return m_ptMinxTOBMeV1;
          if(module=="2C" || module=="2A") return m_ptMinxTOBMeV2;
          if(module=="3C" || module=="3A") return m_ptMinxTOBMeV3;
          throw std::runtime_error("L1ThrExtraInfo: Module" + module + " not recongnised for jLJ ptMinxTOB");
      }
   private:
      /** Update the internal members */
      void load();
      /** jLJ specific data */
      unsigned int m_ptMinToTopoMeV1{0};
      unsigned int m_ptMinToTopoMeV2{0};
      unsigned int m_ptMinToTopoMeV3{0};
      unsigned int m_ptMinxTOBMeV1{0};
      unsigned int m_ptMinxTOBMeV2{0};
      unsigned int m_ptMinxTOBMeV3{0};
   };

   class L1ThrExtraInfo_gJ final : public L1ThrExtraInfoBase {
   public:
      L1ThrExtraInfo_gJ(const std::string & thrTypeName, const ptree & data) :
         L1ThrExtraInfoBase(thrTypeName, data) { load(); }
      virtual ~L1ThrExtraInfo_gJ() = default;
      virtual std::string className() const { return "L1ThrExtraInfo_gJ"; }
      float ptMinToTopo(const unsigned int eta_range) const { return ptMinToTopoMeV(eta_range)/ 1000.0; }
      unsigned int ptMinToTopoCounts(const unsigned int eta_range) const { return energyInCounts(ptMinToTopoMeV(eta_range), resolutionMeV()); }
      unsigned int ptMinToTopoMeV(const unsigned int eta_range) const {
          if(eta_range==1) return m_ptMinToTopoMeV1;
          if(eta_range==2) return m_ptMinToTopoMeV2;
          throw std::runtime_error("L1ThrExtraInfo: Eta range " + std::to_string(eta_range) + " not valid for gJ ptMinToTopo");
      }
   private:
      /** Update the internal members */
      void load();
      /** gJ specific data */
      unsigned int m_ptMinToTopoMeV1{0};
      unsigned int m_ptMinToTopoMeV2{0};
   };

   class L1ThrExtraInfo_gLJ final : public L1ThrExtraInfoBase {
   public:
      L1ThrExtraInfo_gLJ(const std::string & thrTypeName, const ptree & data) :
         L1ThrExtraInfoBase(thrTypeName, data) { load(); }
      virtual ~L1ThrExtraInfo_gLJ() = default;
      virtual std::string className() const { return "L1ThrExtraInfo_gLJ"; }
      float ptMinToTopo(const unsigned int eta_range) const { return ptMinToTopoMeV(eta_range)/ 1000.0; }
      unsigned int ptMinToTopoCounts(const unsigned int eta_range) const { return energyInCounts(ptMinToTopoMeV(eta_range), resolutionMeV()); }
      unsigned int ptMinToTopoMeV(const unsigned int eta_range) const {
          if(eta_range==1) return m_ptMinToTopoMeV1;
          if(eta_range==2) return m_ptMinToTopoMeV2;
          throw std::runtime_error("L1ThrExtraInfo: Eta range " + std::to_string(eta_range) + " not valid for gLJ ptMinToTopo");
      }
      float seedThr(const char fpga) const { return seedThrMeV(fpga)/ 1000.0; }
      unsigned int seedThrCounts(const char fpga) const { return energyInCounts(seedThrMeV(fpga), resolutionMeV()); }
      unsigned int seedThrMeV(const char fpga) const {
          if(fpga=='A') return m_seedThrMeVA;
          if(fpga=='B') return m_seedThrMeVB;
          if(fpga=='C') return m_seedThrMeVC;
          throw std::runtime_error(std::string("L1ThrExtraInfo: FPGA ") + fpga + " not recongnised for gLJ seedThr");
      }
      float rhoTowerMin(const char fpga) const { return rhoTowerMinMeV(fpga)/ 1000.0; }
      int rhoTowerMinMeV(const char fpga) const {
          if(fpga=='A') return m_rhoTowerMinMeVA;
          if(fpga=='B') return m_rhoTowerMinMeVB;
          if(fpga=='C') return m_rhoTowerMinMeVC;
          throw std::runtime_error(std::string("L1ThrExtraInfo: FPGA ") + fpga + " not recongnised for gLJ rhoTowerMin");
      }
      float rhoTowerMax(const char fpga) const { return rhoTowerMaxMeV(fpga)/ 1000.0; }
      int rhoTowerMaxMeV(const char fpga) const {
          if(fpga=='A') return m_rhoTowerMaxMeVA;
          if(fpga=='B') return m_rhoTowerMaxMeVB;
          if(fpga=='C') return m_rhoTowerMaxMeVC;
          throw std::runtime_error(std::string("L1ThrExtraInfo: FPGA ") + fpga + " not recongnised for gLJ rhoTowerMax");
      }
   private:
      /** Update the internal members */
      void load();
      /** gLJ specific data */
      unsigned int m_ptMinToTopoMeV1{0};
      unsigned int m_ptMinToTopoMeV2{0};
      unsigned int m_seedThrMeVA{0};
      unsigned int m_seedThrMeVB{0};
      unsigned int m_seedThrMeVC{0};
      int m_rhoTowerMaxMeVA{0};
      int m_rhoTowerMaxMeVB{0};
      int m_rhoTowerMaxMeVC{0};
      int m_rhoTowerMinMeVA{0};
      int m_rhoTowerMinMeVB{0};
      int m_rhoTowerMinMeVC{0};
   };

   class L1ThrExtraInfo_jXE final : public L1ThrExtraInfoBase {
   public:
      L1ThrExtraInfo_jXE(const std::string & thrTypeName, const ptree & data) :
         L1ThrExtraInfoBase(thrTypeName, data) { load(); }
      virtual ~L1ThrExtraInfo_jXE() = default;
      virtual std::string className() const { return "L1ThrExtraInfo_jXE"; }
   private:
      /** Update the internal members */
      void load();
      /** jXE specific data */
   };

   class L1ThrExtraInfo_jTE final : public L1ThrExtraInfoBase {
   public:
      L1ThrExtraInfo_jTE(const std::string & thrTypeName, const ptree & data) :
         L1ThrExtraInfoBase(thrTypeName, data) { load(); }
      virtual ~L1ThrExtraInfo_jTE() = default;
      virtual std::string className() const { return "L1ThrExtraInfo_jTE"; }
      unsigned int etaBoundary_fw(const std::string& module) const {
          if(module=="1C" || module=="1A") return m_etaBoundary1_fw;
          if(module=="2C" || module=="2A") return m_etaBoundary2_fw;
          if(module=="3C" || module=="3A") return m_etaBoundary3_fw; 
          throw std::runtime_error("L1ThrExtraInfo: Module" + module + " not recongnised for jTE etaBoundary_fw"); 
      }
      unsigned int etaBoundary(const std::string& module) const {
          if(module=="1C" || module=="1A") return m_etaBoundary1;
          if(module=="2C" || module=="2A") return m_etaBoundary2;
          if(module=="3C" || module=="3A") return m_etaBoundary3;
          throw std::runtime_error("L1ThrExtraInfo: Module" + module + " not recongnised for jTE etaBoundary");
      }
   private:
      /** Update the internal members */
      void load();
      /** jTE specific data */
      unsigned int m_etaBoundary1{0}; // eta for module 1A, 1C
      unsigned int m_etaBoundary1_fw{0}; // tower number for module 1A, 1C
      unsigned int m_etaBoundary2{0}; // eta for module 2A, 2C
      unsigned int m_etaBoundary2_fw{0}; // tower number for module 2A, 2C
      unsigned int m_etaBoundary3{0}; // eta for module 3A, 3C
      unsigned int m_etaBoundary3_fw{0}; // tower number for module 3A, 3C
   };

   class L1ThrExtraInfo_gXE final : public L1ThrExtraInfoBase {
   public:
      L1ThrExtraInfo_gXE(const std::string & thrTypeName, const ptree & data) :
         L1ThrExtraInfoBase(thrTypeName, data) { load(); }
      virtual ~L1ThrExtraInfo_gXE() = default;
      virtual std::string className() const { return "L1ThrExtraInfo_gXE"; }
      float seedThr(const char fpga) const { return seedThrMeV(fpga)/ 1000.0; }
      unsigned int seedThrCounts(const char fpga) const { return energyInCounts(seedThrMeV(fpga), resolutionMeV()); }
      unsigned int seedThrMeV(const char fpga) const {
          if(fpga=='A') return m_seedThrMeVA;
          if(fpga=='B') return m_seedThrMeVB;
          if(fpga=='C') return m_seedThrMeVC;
          throw std::runtime_error(std::string("L1ThrExtraInfo: FPGA ") + fpga + " not recongnised for gXE seedThr");
      }
      unsigned int XERHO_param(const char fpga, const bool sigmapos) const {
          if(fpga=='A' && sigmapos==true) return m_XERHO_sigmaPosA;
          if(fpga=='B' && sigmapos==true) return m_XERHO_sigmaPosB;
          if(fpga=='C' && sigmapos==true) return m_XERHO_sigmaPosC;
          if(fpga=='A' && sigmapos==false) return m_XERHO_sigmaNegA;
          if(fpga=='B' && sigmapos==false) return m_XERHO_sigmaNegB;
          if(fpga=='C' && sigmapos==false) return m_XERHO_sigmaNegC;
          throw std::runtime_error(std::string("L1ThrExtraInfo: ") + fpga + " and " + (sigmapos ? "pos" : "neg") + " not recongnised for gXE XERHO_param");
      }
      unsigned int JWOJ_param(const char fpga, const char param) const {
          if(fpga=='A' && param=='a') return m_XEJWOJ_a_A;
          if(fpga=='B' && param=='a') return m_XEJWOJ_a_B;
          if(fpga=='C' && param=='a') return m_XEJWOJ_a_C;
          if(fpga=='A' && param=='b') return m_XEJWOJ_b_A;
          if(fpga=='B' && param=='b') return m_XEJWOJ_b_B;
          if(fpga=='C' && param=='b') return m_XEJWOJ_b_C;
          if(fpga=='A' && param=='c') return m_XEJWOJ_c_A;
          if(fpga=='B' && param=='c') return m_XEJWOJ_c_B;
          if(fpga=='C' && param=='c') return m_XEJWOJ_c_C;
          throw std::runtime_error(std::string("L1ThrExtraInfo: ") + fpga + " and " + param  + " not recongnised for gXE JWOJ_param");
      }
   private:
      /** Update the internal members */
      void load();
      /** gXE specific data */
      unsigned int m_XERHO_sigmaPosA{0};
      unsigned int m_XERHO_sigmaPosB{0};
      unsigned int m_XERHO_sigmaPosC{0};
      unsigned int m_XERHO_sigmaNegA{0};
      unsigned int m_XERHO_sigmaNegB{0};
      unsigned int m_XERHO_sigmaNegC{0};
      unsigned int m_seedThrMeVA{0};
      unsigned int m_seedThrMeVB{0};
      unsigned int m_seedThrMeVC{0};
      unsigned int m_XEJWOJ_a_A{0};
      unsigned int m_XEJWOJ_a_B{0};
      unsigned int m_XEJWOJ_a_C{0};
      unsigned int m_XEJWOJ_b_A{0};
      unsigned int m_XEJWOJ_b_B{0};
      unsigned int m_XEJWOJ_b_C{0};
      unsigned int m_XEJWOJ_c_A{0};
      unsigned int m_XEJWOJ_c_B{0};
      unsigned int m_XEJWOJ_c_C{0};
   };

   class L1ThrExtraInfo_gTE final : public L1ThrExtraInfoBase {
   public:
      L1ThrExtraInfo_gTE(const std::string & thrTypeName, const ptree & data) :
         L1ThrExtraInfoBase(thrTypeName, data) { load(); }
      virtual ~L1ThrExtraInfo_gTE() = default;
      virtual std::string className() const { return "L1ThrExtraInfo_gTE"; }
   private:
      /** Update the internal members */
      void load();
      /** gTE specific data */
   };

   class L1ThrExtraInfo_MU final : public L1ThrExtraInfoBase {
   public:
      L1ThrExtraInfo_MU(const std::string & thrTypeName, const ptree & data) :
         L1ThrExtraInfoBase(thrTypeName, data) { load(); }
      virtual ~L1ThrExtraInfo_MU() = default;
      virtual std::string className() const { return "L1ThrExtraInfo_MU"; }
      unsigned int rpcIdxForPt(unsigned int pt) const;
      unsigned int tgcIdxForPt(unsigned int pt) const;
      unsigned int ptForRpcIdx(unsigned int idx) const;
      unsigned int ptForTgcIdx(unsigned int idx) const;
      unsigned int tgcIdxForRpcIdx(unsigned int rpcIdx) const; // this maps the rpc idx to the tgc idx for the same pt value
      std::vector<unsigned int> knownRpcPtValues() const;
      std::vector<unsigned int> knownTgcPtValues() const;
      std::vector<std::string> exclusionListNames() const;
      const std::map<std::string, std::vector<unsigned int>> & exclusionList(const std::string & listName) const;
   private:
      /** Update the internal members */
      void load();
      /** MU specific data */
      std::map<unsigned int, unsigned int> m_rpcPtMap; // map of pt value (1..~20) to road index (0..5)
      std::map<unsigned int, unsigned int> m_tgcPtMap; // map of pt value (1..~30) to road index (0..15)
      std::map<std::string, std::map<std::string, std::vector<unsigned int>>> m_roiExclusionLists; // named lists of excluded rois
   };

}

#endif
