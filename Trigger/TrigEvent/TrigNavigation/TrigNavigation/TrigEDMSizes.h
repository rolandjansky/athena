/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGNAVIGATION_TRIGEDMSIZES_H
#define TRIGNAVIGATION_TRIGEDMSIZES_H
#include <string>
#include <vector>
#include "GaudiKernel/ClassID.h"
#include "CLIDSvc/CLASS_DEF.h"
namespace HLT {
    /**
     * Class used to pass around information about the sizes of the EDM obejcts.
     *
     **/

  class TrigEDMSizes {
  public:
    TrigEDMSizes();    

    struct EDMObjectInfo {
      EDMObjectInfo(const std::string& c, const std::string& l, unsigned w, unsigned cut);
      bool isTruncated(unsigned size_after_serialization) const;
      std::string  collection;
      std::string  label;
      unsigned int words;
      unsigned int cutsize;
    };
    
    typedef std::vector<EDMObjectInfo> EDMEventInfo;
    const EDMEventInfo& info() const;

    void addObject(const std::string& c, const std::string& label, unsigned words, unsigned cutsize);
    const EDMObjectInfo& lastNotTruncated(unsigned size_after_truncation) const;


  private:    
    EDMEventInfo m_objects;
  };


}

CLASS_DEF( HLT::TrigEDMSizes , 190211077 , 1 )

#endif 
