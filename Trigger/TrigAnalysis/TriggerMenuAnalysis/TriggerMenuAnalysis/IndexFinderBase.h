/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __IndexFinderBase_h__
#define __IndexFinderBase_h__
/*
  IndexFinderBase.h
*/
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include "TriggerMenuNtuple/FeatureIndex.h"

namespace Trig {
  class Combination;
  class TrigDecisionTool;
}

class MsgStream;
class StoreGateSvc;

class IndexFinderBase {
public:
  IndexFinderBase();
  virtual ~IndexFinderBase() {}

  virtual const std::string& label() const=0;
  virtual std::string typeLabel() const=0;

  virtual void setTrigDecisionTool(Trig::TrigDecisionTool* tdt)=0;
  virtual void setLogger(MsgStream* logger)=0;
  virtual void setL1RoIName(std::string RoIName)=0;
  virtual void setL1ROIWord(const long RoIWord)=0;
  virtual bool checkROIWord(const HLT::TriggerElement* te)=0;

  virtual FeatureIndex findIndex(const Trig::Combination& comb, 
				 std::string& te_label)=0;
  virtual std::vector<FeatureIndex> findIndexes(const Trig::Combination& comb, 
						std::string& te_label)=0;

//   /** Obsolete member functions */
//   virtual FeatureIndex findIndex(const Trig::Combination& comb, 
// 				 MsgStream* logger)=0;
//   virtual FeatureIndex findIndex(const Trig::Combination& comb, 
// 				 StoreGateSvc& sg_svc, MsgStream& msg)=0;
//   virtual std::vector<FeatureIndex> findIndexes(const Trig::Combination& comb, 
// 						MsgStream* logger)=0;
protected:
  int subTypeIndex(unsigned int clid, const std::string& label, 
		   const Trig::Combination& comb) const;
  void fillIndexes(std::vector<FeatureIndex>& index, 
		   const HLT::TriggerElement* te, unsigned int clid, 
		   unsigned int subtypeOfLabel, 
		   const std::string& label, 
		   MsgStream* logger);
};

#endif // __IndexFinderBase_h__
