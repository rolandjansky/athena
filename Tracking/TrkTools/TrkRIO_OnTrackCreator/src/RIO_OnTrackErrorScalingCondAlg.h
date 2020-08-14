/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef _RIO_OnTrackErrorScalingCondAlg_H_
#define _RIO_OnTrackErrorScalingCondAlg_H_

#include "AthenaBaseComps/AthAlgorithm.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
//#include "StoreGate/ReadCondHandleKey.h"
#//include "StoreGate/WriteCondHandleKey.h"
#include "StoreGate/CondHandleKeyArray.h"
#include "TrkRIO_OnTrack/RIO_OnTrackErrorScaling.h"
#include "GaudiKernel/ICondSvc.h"
#include "Gaudi/Property.h"
#include <iostream>

class RIO_OnTrackErrorScalingKit;

class RIO_OnTrackErrorScalingCondAlg : public AthAlgorithm 
{  
 public:
  RIO_OnTrackErrorScalingCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~RIO_OnTrackErrorScalingCondAlg() = default;
  StatusCode initialize() override;
  StatusCode execute() override;
  StatusCode finalize() override;

 private:

  StatusCode addErrorScaling ATLAS_NOT_THREAD_SAFE (const std::string &type_name);
  void registerAttribute(const std::string& name, unsigned int type_idx, unsigned int param_idx);

  SG::ReadCondHandleKey<CondAttrListCollection> m_readKey
     {this, "ReadKey", "/Indet/TrkErrorScaling", "Conditions folder namer for innder detector error scaling parameters."};

  Gaudi::Property< std::vector<std::string> > m_errorScalingType
    { this,
      "ErrorScalingType",
      {"PixelRIO_OnTrackErrorScalingRun1","SCTRIO_OnTrackErrorScaling","TRTRIO_OnTrackErrorScaling"},
      "Error scaling type. (git grep  \"RIO_OnTrackErrorScalingKitManager.*registerKit\")"
     };

  Gaudi::Property< std::vector<std::string> > m_attributIgnoreList
    { this,"IgnoreAttributes", {}, "List of attributes that should be ignored."};

  SG::WriteCondHandleKeyArray<RIO_OnTrackErrorScaling> m_writeKey
    { this,
      "OutKeys",
      {"/Indet/TrkErrorScalingPixel","/Indet/TrkErrorScalingSCT","/Indet/TrkErrorScalingTRT"},
      "Output conditions key names, for the specified error scaling types"};

  std::vector<const RIO_OnTrackErrorScalingKit *>               m_kits {};
  std::map<std::string, std::pair<unsigned int, unsigned int> > m_attributeMap;

  ServiceHandle<ICondSvc> m_condSvc;
};

#endif // SCT_CONDITIONSPARAMETERCONDALG
