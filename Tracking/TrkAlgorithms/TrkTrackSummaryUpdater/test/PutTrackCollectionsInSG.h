/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
* @file PutTrackCollectionsInSG.h
*
* @brief Header file for the PutTrackCollectionsInSG class 
*  for testing the TrkTrackCollectionMerger
*
* @author Shaun Roe
**/

#ifndef PutTrackCollectionsInSG_H
#define PutTrackCollectionsInSG_H 

//Athena
#include "AthenaBaseComps/AthAlgorithm.h"

//Gaudi

//STL
#include <string>

///Example algorithm to show calling the SCT_ModuleVetoSvc to exclude bad components
class PutTrackCollectionsInSG : public AthAlgorithm {
 public:
  PutTrackCollectionsInSG(const std::string &name,ISvcLocator *pSvcLocator) ;
  virtual ~PutTrackCollectionsInSG() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;
  virtual bool isClonable() const override { return false; };

 private:
}; //end of class

#endif // PutTrackCollectionsInSG_H