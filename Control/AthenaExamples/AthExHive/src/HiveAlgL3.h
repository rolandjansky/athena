#ifndef ATHEXHIVE_ALGL3_H
#define ATHEXHIVE_ALGL3_H 1

#include "HiveAlgBase.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/UpdateHandleKey.h"
#include "AthExHive/HiveDataObj.h"
#include "HiveAlgBase.h"

#include <string>

class HiveAlgL3  :  public HiveAlgBase {
  
public:
  
  // Standard Algorithm Constructor:
  
  HiveAlgL3 (const std::string& name, ISvcLocator* pSvcLocator);
  ~HiveAlgL3();
  
  // Define the initialize, execute and finalize methods:
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  
  SG::UpdateHandleKey<HiveDataObj> m_udh1;
  
};
#endif
