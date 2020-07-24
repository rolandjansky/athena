/* Dear emacs, this is -*-c++-*- */
#ifndef _ClusterSplitProbabilityContainer_H_
#define _ClusterSplitProbabilityContainer_H_

#include "Identifier/Identifier.h"
#include "TrkPrepRawData/PrepRawData.h"
#include <unordered_map>
#include <algorithm>
#include <stdexcept>

namespace Trk {
   class ClusterSplitProbabilityContainer
   {
   public:
      using ClusterIdentifier=unsigned int;

      struct ProbabilityInfo : public std::pair<float,float> {
         ProbabilityInfo(float prob1, float prob2) : std::pair<float,float>(prob1,prob2) {}
         float splitProbability1() const { return isTooBigToBeSplit() ? 0.f : this->first; }
         float splitProbability2() const { return isTooBigToBeSplit() ? 0.f : this->second; }
         bool isTooBigToBeSplit() const  { return first<0; }
         bool isSplit() const            { return m_isSplit;}
         void setSplit(bool split)       { m_isSplit=split;}
         void updateSplitProbabilities(float prob1, float prob2) { this->first=prob1; this->second=prob2; m_isSplit=false;}
      private:
         bool m_isSplit = false;
      };
      static const ProbabilityInfo &getNoSplitProbability()  {return  s_unset; }

      const ProbabilityInfo &splitProbability(const PrepRawData*cluster) const {
         assert(cluster);
         std::unordered_map<ClusterIdentifier, ProbabilityInfo>::const_iterator iter = m_splitProbMap.find(cluster->getHashAndIndex().hashAndIndex());
         return (iter != m_splitProbMap.end()) ? iter->second : s_unset;
      }
      ProbabilityInfo *getSplitProbability(const PrepRawData*cluster) {
         assert(cluster);
         std::unordered_map<ClusterIdentifier, ProbabilityInfo>::iterator iter = m_splitProbMap.find(cluster->getHashAndIndex().hashAndIndex());
         return (iter != m_splitProbMap.end()) ? &iter->second : nullptr;
      }
      ProbabilityInfo &setSplitInformation(const PrepRawData*cluster, float prob1, float prob2)  {
         assert(cluster);
         // @TODO remove the precision clipping, which is currently implemented here to produce identical results wrt. the current version.
         constexpr  int splitmask =  0x7fff;
         prob1 = int(prob1 * splitmask)/(1.*splitmask);
         prob2 = int(prob2 * splitmask)/(1.*splitmask);
         std::pair<std::unordered_map<ClusterIdentifier, ProbabilityInfo>::iterator,bool>
            ret = m_splitProbMap.insert( std::make_pair( cluster->getHashAndIndex().hashAndIndex(), ProbabilityInfo(prob1, prob2) ) );
         if (!ret.second) {
            std::unordered_map<ClusterIdentifier, ProbabilityInfo>::iterator iter = m_splitProbMap.find(cluster->getHashAndIndex().hashAndIndex());
            if (iter == m_splitProbMap.end()) {
               throw std::runtime_error("Failed to set cluster splitting probability.");
            }
            iter->second.updateSplitProbabilities(prob1,prob2);
            return iter->second;
         }
         else {
            return ret.first->second;
         }
      }
      void setTooBigToBeSplit(const PrepRawData*cluster) {
         assert(cluster);
         m_splitProbMap.insert( std::make_pair( cluster->getHashAndIndex().hashAndIndex(), ProbabilityInfo(-1.,-1.) ) );
      }
   protected:
      std::unordered_map<ClusterIdentifier, ProbabilityInfo> m_splitProbMap;
      static const ProbabilityInfo s_unset;
   };
}
#ifndef CLIDSVC_CLASSDEF_H
# include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( Trk::ClusterSplitProbabilityContainer, 1313584800, 1)
#endif
#endif
