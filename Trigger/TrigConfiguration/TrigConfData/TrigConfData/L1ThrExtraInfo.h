/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
   class L1ThrExtraInfo_eEMTAU;
   class L1ThrExtraInfo_jJ;
   class L1ThrExtraInfo_jTAU;
   class L1ThrExtraInfo_gXE;
   class L1ThrExtraInfo_MU;

   class L1ThrExtraInfo {
   public:
      static std::unique_ptr<L1ThrExtraInfoBase> createExtraInfo(const std::string & thrTypeName, const boost::property_tree::ptree & data);

      std::weak_ptr<TrigConf::L1ThrExtraInfoBase> addExtraInfo(const std::string & thrTypeName, const boost::property_tree::ptree & data);

      const L1ThrExtraInfo_EMTAULegacy & EM() const;
      const L1ThrExtraInfo_EMTAULegacy & TAU() const;
      const L1ThrExtraInfo_JETLegacy & JET() const;
      const L1ThrExtraInfo_eEMTAU & eEM() const;
      const L1ThrExtraInfo_eEMTAU & eTAU() const;
      const L1ThrExtraInfo_jJ & jJ() const;
      const L1ThrExtraInfo_jTAU & jTAU() const;
      const L1ThrExtraInfo_gXE & gXE() const;
      const L1ThrExtraInfo_MU & MU() const;

      const L1ThrExtraInfoBase & thrExtraInfo(const std::string & thrTypeName) const;

      bool hasInfo(const std::string & typeName) const;

   private:
      // map from threshold type to 
      std::map<std::string, std::shared_ptr<TrigConf::L1ThrExtraInfoBase>> m_thrExtraInfo{};
      std::shared_ptr<TrigConf::L1ThrExtraInfoBase> m_emptyInfo{ nullptr };
   };



   class L1ThrExtraInfo_EMTAULegacy final : public L1ThrExtraInfoBase {
   public:
      L1ThrExtraInfo_EMTAULegacy(const std::string & thrTypeName, const ptree & data);
      virtual ~L1ThrExtraInfo_EMTAULegacy();
      virtual std::string className() const;
      unsigned int emScale() const;
      unsigned int ptMinToTopo() const;
      const IsolationLegacy & isolation(const std::string & thrType, size_t bit) const;
   private:
      /** Update the internal members */
      virtual void update();
      /** EM specific data */
      unsigned int m_emScale{1};
      unsigned int m_ptMinToTopo{0};
      std::map<std::string,std::vector<IsolationLegacy>> m_isolation{};
   };

   class L1ThrExtraInfo_JETLegacy final : public L1ThrExtraInfoBase {
   public:
      L1ThrExtraInfo_JETLegacy(const std::string & thrTypeName, const ptree & data);
      virtual ~L1ThrExtraInfo_JETLegacy();
      unsigned int jetScale() const;
      unsigned int ptMinToTopoLargeWindow() const;
      unsigned int ptMinToTopoSmallWindow() const;
   private:
      /** Update the internal members */
      virtual void update();
      /** JET specific data */
      unsigned int m_jetScale{1};
      unsigned int m_ptMinToTopoLargeWindow{0};
      unsigned int m_ptMinToTopoSmallWindow{0};
   };


   /***********************************
    * Extra info for new thresholds
    ***********************************/

   class L1ThrExtraInfo_eEMTAU final : public L1ThrExtraInfoBase {
   public:
      L1ThrExtraInfo_eEMTAU(const std::string & thrTypeName, const ptree & data);
      virtual ~L1ThrExtraInfo_eEMTAU();
      unsigned int ptMinToTopo() const;
      const TrigConf::Isolation & isolation(TrigConf::Isolation::WP wp, int eta) const;
      const ValueWithEtaDependence<TrigConf::Isolation> & isolation(TrigConf::Isolation::WP wp) const;
   private:
      /** Update the internal members */
      virtual void update();
      /** eEM specific data */
      unsigned int m_ptMinToTopo{0};
      std::map<TrigConf::Isolation::WP, ValueWithEtaDependence<Isolation>> m_isolation{};
   };

   class L1ThrExtraInfo_jJ final : public L1ThrExtraInfoBase {
   public:
      L1ThrExtraInfo_jJ(const std::string & thrTypeName, const ptree & data);
      virtual ~L1ThrExtraInfo_jJ();
      unsigned int ptMinToTopoSmall(int eta) const;
      unsigned int ptMinToTopoLarge(int eta) const;
   private:
      /** Update the internal members */
      virtual void update();
      /** jJ specific data */
      std::map<std::pair<int,int>,unsigned int> m_ptMinToTopoSmall{};
      std::map<std::pair<int,int>,unsigned int> m_ptMinToTopoLarge{};
   };

   class L1ThrExtraInfo_jTAU final : public L1ThrExtraInfoBase {
   public:
      L1ThrExtraInfo_jTAU(const std::string & thrTypeName, const ptree & data);
      virtual ~L1ThrExtraInfo_jTAU();
      unsigned int ptMinToTopo() const;
   private:
      /** Update the internal members */
      virtual void update();
      /** jTAU specific data */
      unsigned int m_ptMinToTopo{0};
   };

   class L1ThrExtraInfo_gXE final : public L1ThrExtraInfoBase {
   public:
      L1ThrExtraInfo_gXE(const std::string & thrTypeName, const ptree & data);
      virtual ~L1ThrExtraInfo_gXE();
   private:
      /** Update the internal members */
      virtual void update();
      /** gXE specific data */
   };

   class L1ThrExtraInfo_MU final : public L1ThrExtraInfoBase {
   public:
      L1ThrExtraInfo_MU(const std::string & thrTypeName, const ptree & data);
      virtual ~L1ThrExtraInfo_MU();
      unsigned int rpcIdxForPt(unsigned int pt) const;
      unsigned int tgcIdxForPt(unsigned int pt) const;
      std::vector<unsigned int> knownRpcPtValues() const;
      std::vector<unsigned int> knownTgcPtValues() const;
      std::vector<std::string> exclusionListNames() const;
      const std::map<std::string, std::vector<unsigned int>> & exlusionList(const std::string & listName) const;
   private:
      /** Update the internal members */
      virtual void update();
      /** MU specific data */
      std::map<unsigned int, unsigned int> m_rpcPtMap; // map of pt value (1..~20) to road index (0..5)
      std::map<unsigned int, unsigned int> m_tgcPtMap; // map of pt value (1..~30) to road index (0..15)
      std::map<std::string, std::map<std::string, std::vector<unsigned int>>> m_roiExclusionLists; // named lists of excluded rois
   };

}

#endif
