/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#pragma once
#ifndef TrigSteeringEvent_RobRequestInfo_h
#define TrigSteeringEvent_RobRequestInfo_h

#include <vector>
#include <stdint.h>

namespace HLT {
   class RobRequestInfo {
   public:
      RobRequestInfo(){}
      ~RobRequestInfo(){}
      
      void clearRequestScheduledRobIDs() {
         m_requestScheduledRobIDs.clear();
      }

      const std::vector<uint32_t>& requestScheduledRobIDs() const {
         return m_requestScheduledRobIDs;
      }

      void addRequestScheduledRobIDs(const std::vector<uint32_t>& roblist);
      void addRequestScheduledRobIDs(uint32_t rob);

   private:

      std::vector<uint32_t> m_requestScheduledRobIDs;  //!< list of scheduled rob IDs to request in preparation stage


   }; // end class
} // end namespace

#endif
