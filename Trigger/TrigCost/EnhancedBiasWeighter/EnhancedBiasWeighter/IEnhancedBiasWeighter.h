/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ENHANCEDBIASWEIGHTER_IENHANCEDBIASWEIGHTER_H
#define ENHANCEDBIASWEIGHTER_IENHANCEDBIASWEIGHTER_H 1

//#include "AsgTools/IAsgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "xAODEventInfo/EventInfo.h"
#include <unordered_map>

struct ChainDetail;

//class IEnhancedBiasWeighter : public virtual asg::IAsgTool {

/**
 * @brief Define interface of EnhancedBias weighting class
 */
class IEnhancedBiasWeighter : public virtual DerivationFramework::IAugmentationTool {
   public:
      ASG_TOOL_INTERFACE( IEnhancedBiasWeighter ) //declares the interface to athena

      virtual double   getEBWeight(const xAOD::EventInfo* eventInfo) const = 0;
      virtual double   getEBWeight(const EventContext& context) const = 0;
      virtual double   getEBLiveTime(const xAOD::EventInfo* eventInfo) const = 0;
      virtual double   getEBLiveTime(const EventContext& context) const = 0;
      virtual double   getLBLumi(const xAOD::EventInfo* eventInfo) const = 0;
      virtual double   getLBLumi(const EventContext& context) const = 0;
      virtual double   getLBLength(const xAOD::EventInfo* eventInfo) const = 0;
      virtual double   getLBLength(const EventContext& context) const = 0;
      virtual double   getAverageLumi() const = 0;
      virtual double   getAverageMu() const = 0;
      virtual double   getDeadtime() const = 0;
      virtual uint32_t getPairedBunches() const = 0;
      virtual uint32_t getDistanceIntoTrain(const xAOD::EventInfo* eventInfo) const = 0;
      virtual bool     isUnbiasedEvent(const xAOD::EventInfo* eventInfo) const = 0;
      virtual bool     isGoodLB(const xAOD::EventInfo* eventInfo) const = 0;
      virtual bool     isGoodLB(const EventContext& context) const = 0;
      virtual bool     isMC() const = 0;
      virtual std::unordered_map<std::string, ChainDetail> parsePrescaleXML(const std::string& prescaleXML) const = 0;

};

#endif //> !ENHANCEDBIASWEIGHTER_IENHANCEDBIASWEIGHTER_H
