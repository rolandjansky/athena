/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BCM_ZEROSUPPRESSION_H
#define BCM_ZEROSUPPRESSION_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "InDetBCM_RawData/BCM_RDO_Container.h"

/** @class BCM_ZeroSuppression
    
  @author Bilge M. Demirkoz <demirkoz@cern.ch>

  This algorithm takes the BCM RDO and zero-suppresses it 
  providing a smaller object to go into the AOD.

*/  
class BCM_ZeroSuppression : public AthAlgorithm {
 public:
  BCM_ZeroSuppression(const std::string& name, ISvcLocator* pSvcLocator);
  ~BCM_ZeroSuppression();
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
 private:
  /** A data member to contain a pointer to the Bcm RDO
      that contains the input BCM information. */ 
  const BCM_RDO_Container* m_bcmRDO;

  /** A data member to contain a pointer to the compact BCM RDO
      that contains the output BCM information that will go into the AOD. */ 
  BCM_RDO_Container* m_bcmCompactDO;
  
  /** A data member to contain the name of the BCM RDO
      that contains the input BCM information.  This data member is
      used as a job option property.
  */
  std::string m_bcmContainerName;
  std::string m_bcmOutputName;

  BCM_RDO_Collection* m_my_collection;

};

#endif
