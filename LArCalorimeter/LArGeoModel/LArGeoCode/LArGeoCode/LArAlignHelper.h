/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARGEOCODE_LARALIGNHELPER_H
#define LARGEOCODE_LARALIGNHELPER_H

/**
 * @class LArAlignHelper
 *
 * @brief Helper class for applying LAr alignments either from LArDetectorToolNV or from LArAlignCondAlg
 *
 **/

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthMessaging.h"
#include "StoreGate/StoreGateSvc.h"
#include <vector>
#include <string>

class DetCondKeyTrans;
class GeoAlignmentStore;

class LArAlignHelper : public AthMessaging
{
 public:
  LArAlignHelper();

  StatusCode applyAlignments(const ServiceHandle<StoreGateSvc>& detStore
			     , const DetCondKeyTrans* detCondKeyTrans
			     , GeoAlignmentStore* alignmentStore) const;
  
 private:
  std::vector<std::string> m_alignNames{ "LARCRYO_B"
      , "LARCRYO_EC_POS"
      , "LARCRYO_EC_NEG"
      , "PRESAMPLER_B_POS"
      , "PRESAMPLER_B_NEG"
      , "EMB_POS"
      , "EMB_NEG"
      , "PRESAMPLER_EC_POS"
      , "PRESAMPLER_EC_NEG"
      , "EMEC_POS"
      , "EMEC_NEG"
      , "HEC_POS"   // Policy:  either HEC_POS is present or HEC1_POS and HEC2_POS. The same applies to HEC_NEG
      , "HEC_NEG"   // Now in recent releases if HEC_POS is found it will be applied to both HEC1 and HEC2...
      , "HEC1_POS"
      , "HEC1_NEG"
      , "HEC2_POS"
      , "HEC2_NEG"
      , "FCAL1_POS"
      , "FCAL1_NEG"
      , "FCAL2_POS"
      , "FCAL2_NEG"
      , "FCAL3_POS"
      , "FCAL3_NEG"
      , "SOLENOID"
      };
};

#endif
