/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPOUNITTESTS_IHYPOJETVECTORGENERATOR
#define TRIGHLTJETHYPOUNITTESTS_IHYPOJETVECTORGENERATOR

#include "GaudiKernel/IAlgTool.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"

class IHypoJetVectorGenerator: virtual public ::IAlgTool {
public:
  DeclareInterfaceID(IHypoJetVectorGenerator, 1, 0);
  virtual ~IHypoJetVectorGenerator(){};

  virtual HypoJetVector get()  = 0;
  virtual std::string  toString() const = 0;

};


#endif //! TRIGHLTJETHYPOUNITTESTS_IHYPOJETVECTORGENERATOR
