#ifndef ATHEXHIVE_ALGF_H
#define ATHEXHIVE_ALGF_H 1

#include "HiveAlgBase.h"
#include "StoreGate/ReadHandleKey.h"
#include "AthExHive/HiveDataObj.h"
#include "rGen.h"

#include <string>

class HiveAlgF  :  public HiveAlgBase {
  
public:
  
  // Standard Algorithm Constructor:
  
  HiveAlgF (const std::string& name, ISvcLocator* pSvcLocator);
  ~HiveAlgF();

  // Define the initialize, execute and finalize methods:
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  
  SG::ReadHandleKey<HiveDataObj> m_rdh1;
  SG::ReadHandleKey<HiveDataObj> m_rdh2;
  SG::ReadHandleKey<HiveDataObj> m_rdh3;

   
  
};
#endif
