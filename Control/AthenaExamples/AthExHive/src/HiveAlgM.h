#ifndef ATHEXHIVE_ALGM_H
#define ATHEXHIVE_ALGM_H 1

#include "HiveAlgBase.h"
#include "StoreGate/ReadHandleKey.h"
#include "AthExHive/HiveDataObj.h"
#include "rGen.h"

#include <string>

class HiveAlgM  :  public HiveAlgBase {
  
public:
  
  // Standard Algorithm Constructor:
  
  HiveAlgM (const std::string& name, ISvcLocator* pSvcLocator);
  ~HiveAlgM();

  // Define the initialize, execute and finalize methods:
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  
  int m_off { 1 };
  SG::ReadHandleKey<HiveDataObj> m_rdh1;
  SG::ReadHandleKey<HiveDataObj> m_rdh2;
  
};
#endif
