/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef _RIO_OnTrackErrorScalingDbOverrideCondAlg_H_
#define _RIO_OnTrackErrorScalingDbOverrideCondAlg_H_

#include "AthenaBaseComps/AthAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "TrkRIO_OnTrack/RIO_OnTrackErrorScaling.h"
#include "GaudiKernel/ICondSvc.h"
#include "Gaudi/Property.h"
#include <iostream>

class RIO_OnTrackErrorScalingKit;

class RIO_OnTrackErrorScalingDbOverrideCondAlg : public AthAlgorithm 
{  
 public:
  RIO_OnTrackErrorScalingDbOverrideCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~RIO_OnTrackErrorScalingDbOverrideCondAlg() = default;
  StatusCode initialize() override;
  StatusCode execute() override;
  StatusCode finalize() override;

 private:

  Gaudi::Property< std::vector<unsigned int> > m_nParameterPerSet
    {this,"PrametersPerSet",{},"The number of parameters per set. If a single number is supplied this will be applied to all sets."};

  Gaudi::Property< std::vector<float> > m_errorScalingParameters
    {this,"ErorScalingParameters",{},"The error scaling parameters for all sets lumbed together."};

  Gaudi::Property< std::string > m_errorScalingTypeName
    {this,"ErrorScalingTypeName","","The specific type name of the error scaling conditions data that should be produced."};

  SG::WriteCondHandleKey<RIO_OnTrackErrorScaling>  m_writeKey
    {this, "WriteKey","","The key for the produced error scaling conditions data."};

  ServiceHandle<ICondSvc>           m_condSvc;
  const RIO_OnTrackErrorScalingKit *m_errorScalingDataKit;
  std::vector<unsigned int>         m_useNParametersPerSet;
};

#endif // SCT_CONDITIONSPARAMETERCONDALG
