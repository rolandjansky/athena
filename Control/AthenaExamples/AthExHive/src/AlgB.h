#ifndef CONDALGS_ALGB_H
#define CONDALGS_ALGB_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "AthExHive/HiveDataObj.h"

#include <string>

class AlgB  :  public AthAlgorithm {
  
public:
    
  AlgB (const std::string& name, ISvcLocator* pSvcLocator);
  ~AlgB();
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  
  SG::ReadHandleKey<HiveDataObj>  m_rdh1;
  SG::WriteHandleKey<HiveDataObj> m_wrh1;
  
};
#endif
