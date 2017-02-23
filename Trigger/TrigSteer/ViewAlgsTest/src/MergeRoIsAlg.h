// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ViewAlgs_MergeRoIsAlg_h
#define ViewAlgs_MergeRoIsAlg_h

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

//#include "AthViews/View.h"

//#include "./TestEDM.h"

class MergeRoIsAlg : public AthAlgorithm {
public:
  MergeRoIsAlg(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();


private:

  bool m_useSuperRoI;

  SG::ReadHandleKeyArray<TrigRoiDescriptorCollection> m_inputRoIContainerKeys;
  //SG::ReadHandle< TrigRoiDescriptorCollection > m_inputRoisContainer;
  SG::WriteHandle< TrigRoiDescriptorCollection > m_outputRoIContainer;

};



#endif
