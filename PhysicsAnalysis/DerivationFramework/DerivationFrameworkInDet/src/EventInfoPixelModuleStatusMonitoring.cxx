/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkInDet/EventInfoPixelModuleStatusMonitoring.h"

#include <xAODEventInfo/EventInfo.h>
#include "DerivationFrameworkInDet/DecoratorUtils.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

namespace DerivationFramework {

  EventInfoPixelModuleStatusMonitoring::EventInfoPixelModuleStatusMonitoring(const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_pixelID(nullptr) {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
  }

  StatusCode EventInfoPixelModuleStatusMonitoring::initialize() {

    if (m_prefix.empty()) {
      ATH_MSG_WARNING("No decoration prefix name provided for the output of EventInfoPixelModuleStatusMonitoring!");
    }

    ATH_CHECK(m_eventInfoKey.initialize());
    ATH_CHECK(detStore()->retrieve(m_pixelID,"PixelID"));

    ATH_CHECK(m_readKeyTemp.initialize());
    ATH_CHECK(m_readKeyHV.initialize());
    ATH_CHECK(m_condDeadMapKey.initialize());
    ATH_CHECK(m_pixelSummary.retrieve());

    {
      std::vector<std::string> moduleConditionList;
      moduleConditionList.push_back("PixelBiasVoltagePerLB");
      moduleConditionList.push_back("PixelTemperaturePerLB");
      createDecoratorKeys(*this,m_eventInfoKey,m_prefix.value(),moduleConditionList,m_moduleConditionKeys);
    }

    {
      std::vector<std::string> moduleFEmaskList;
      moduleFEmaskList.push_back("PixelFEmaskIndex");
      moduleFEmaskList.push_back("PixelFEmaskPerLB");
      createDecoratorKeys(*this,m_eventInfoKey,m_prefix.value(),moduleFEmaskList,m_moduleFEmaskKeys);
    }

    {
      std::vector<std::string> moduleBSErrList;
      moduleBSErrList.push_back("PixelBSErrIndex");
      moduleBSErrList.push_back("PixelBSErrWord");
      createDecoratorKeys(*this,m_eventInfoKey,m_prefix.value(),moduleBSErrList,m_moduleBSErrKeys);
    }

    return StatusCode::SUCCESS;
  }

  StatusCode EventInfoPixelModuleStatusMonitoring::finalize() {
    return StatusCode::SUCCESS;
  }

  StatusCode EventInfoPixelModuleStatusMonitoring::addBranches() const {

    ATH_MSG_DEBUG("Adding Pixel module status in EventInfo");

    const EventContext& ctx = Gaudi::Hive::currentContext();
    SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfoKey,ctx);
    ATH_CHECK(eventInfo.isValid() ? StatusCode::SUCCESS : StatusCode::FAILURE);

    const xAOD::EventInfo *eventInfoLB = nullptr;
    ATH_CHECK(evtStore()->retrieve(eventInfoLB, "EventInfo"));
    int LB = eventInfoLB->lumiBlock();
    int chkLB = m_lbCounter;
    if (chkLB==0) { chkLB=-1; }

    SG::ReadCondHandle<PixelDCSHVData> dcsHV(m_readKeyHV,ctx);
    SG::ReadCondHandle<PixelDCSTempData> dcsTemp(m_readKeyTemp,ctx);
    SG::ReadCondHandle<PixelDeadMapCondData> deadMap(m_condDeadMapKey,ctx);

    int maxHash = m_pixelID->wafer_hash_max();
    std::vector<float> biasVoltage;
    std::vector<float> temperature;
    std::vector<int> feMaskIndex;
    std::vector<int> feMaskStatus;
    if (chkLB!=LB) {
      for (int ihash=0; ihash<maxHash; ihash++) {
        biasVoltage.push_back(dcsHV->getBiasVoltage(ihash));
        temperature.push_back(dcsTemp->getTemperature(ihash));

        int moduleStatus = deadMap->getModuleStatus(ihash);
        int chipStatus   = deadMap->getChipStatus(ihash);
        if (moduleStatus || chipStatus) {
          feMaskIndex.push_back(ihash);
          if (moduleStatus) {
            feMaskStatus.push_back(0);
          }
          else {
            feMaskStatus.push_back(chipStatus);
          }
        }
      }
      m_lbCounter = LB;
    }

    //=========================================================
    //  Size of Pixel BS Error container
    //
    //  The error would be stored not only for module but also each FE (x16) per module.
    //  In addition, IBL FEI4 provides error counter between trigger, this also extends 
    //  the size beyond nominal module + FE ID. These numbers come from the hardware 
    //  specification so that there is no easy way to document in the header file.
    //  Rather, put the hardcoded number here.
    //
    //      Total number of pixel modules: 2048
    //      Number of FE chips per module:   16
    //     -------------------------------------
    //          2048 x 17 (module + FE) = 34816
    //
    //      IBL extra error information  : 
    //          280(module) x 2(FE) x 32(error counter) = 17920
    //     -------------------------------------
    //
    //      isActive status : 2048
    //     -------------------------------------
    //                             Total : 54784
    //=========================================================
    std::vector<uint64_t> bsErrIndex;
    std::vector<uint64_t> bsErrWord;
    if (maxHash==2048) { // only valid for RUN2/3
      // First, access BS error for each FE chip
      for (int ihash=0; ihash<maxHash; ihash++) {
        for (int chFE=0; chFE<16; chFE++) {
          int indexFE = (1+chFE)*maxHash+ihash;    // (FE_channel+1)*2048 + moduleHash
          uint64_t word = m_pixelSummary->getBSErrorWord(ihash,indexFE,ctx);
          if (word>0) {
            bsErrIndex.push_back(indexFE);
            bsErrWord.push_back(word);
          }
        }
      }
      // Next, access IBL service record
      int indexOffset = 17*maxHash;
      for (int ihash=156; ihash<436; ihash++) {
        for (int chFE=0; chFE<2; chFE++) {
          for (int serviceCode=0; serviceCode<32; serviceCode++) {
            int indexSvcCounter = indexOffset+serviceCode*280*2+2*(ihash-156)+chFE;
            uint64_t word = m_pixelSummary->getBSErrorWord(ihash,indexSvcCounter,ctx);
            if (word>0) {
              bsErrIndex.push_back(indexSvcCounter);
              bsErrWord.push_back(word);
            }
          }
        }
      }
    }

    std::vector<SG::WriteDecorHandle<xAOD::EventInfo,std::vector<float>>> decorModuleCondition(createDecorators<xAOD::EventInfo,std::vector<float>>(m_moduleConditionKeys,ctx));
    assert(decorModuleCondition.size()==2);
    decorModuleCondition[0](*eventInfo) = std::move(biasVoltage);
    decorModuleCondition[1](*eventInfo) = std::move(temperature);

    std::vector<SG::WriteDecorHandle<xAOD::EventInfo,std::vector<int>>> decorModuleFEmask(createDecorators<xAOD::EventInfo,std::vector<int>>(m_moduleFEmaskKeys,ctx));
    assert(decorModuleFEmask.size()==2);
    decorModuleFEmask[0](*eventInfo) = std::move(feMaskIndex);
    decorModuleFEmask[1](*eventInfo) = std::move(feMaskStatus);

    std::vector<SG::WriteDecorHandle<xAOD::EventInfo,std::vector<uint64_t>>> decorModuleBSErr(createDecorators<xAOD::EventInfo,std::vector<uint64_t>>(m_moduleBSErrKeys,ctx));
    assert(decorModuleBSErr.size()==2);
    decorModuleBSErr[0](*eventInfo) = std::move(bsErrIndex);
    decorModuleBSErr[1](*eventInfo) = std::move(bsErrWord);

    return StatusCode::SUCCESS;
  }  
  
}
