/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_DIGITIZATION_SICHARGEDDIODECOLLECTIONFILLER_H
#define SCT_DIGITIZATION_SICHARGEDDIODECOLLECTIONFILLER_H

#include "HitToDiodeConverter.h"  // src dir
#include "boost/shared_ptr.hpp"
#include <vector>
#include <utility> //std::pair

class SCT_ID;
class ISCT_SurfaceChargesGenerator;
class Identifier;
class IdentifierHash;
class SiChargedDiodeCollection;
class HitIndexAndTime;
template<class T> class ToolHandle;

namespace InDetDD
{
  class SiDetectorManager;
}

class SiChargedDiodeCollectionFiller {
 public:
  SiChargedDiodeCollectionFiller(const SCT_ID*, 
                                 const InDetDD::SiDetectorManager*,
                                 ToolHandle<ISCT_SurfaceChargesGenerator>&);
  
  void operator() (const std::pair<const IdentifierHash, std::vector<HitIndexAndTime>>&);

  boost::shared_ptr<SiChargedDiodeCollection> chargedDiodes() const;
  bool errorCondition()                                       const;
  std::string errMsg()                                        const;
  
 private:
  HitToDiodeConverter m_hitToDiodeConverter;
};
#endif // SCT_DIGITIZATION_SICHARGEDDIODECOLLECTIONFILLER_H
