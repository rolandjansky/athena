#ifndef ATHEXHIVE_ALGG_H
#define ATHEXHIVE_ALGG_H 1

#include "HiveAlgBase.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "AthExHive/HiveDataObj.h"
#include "rGen.h"

#include <string>

class HiveAlgG  :  public HiveAlgBase {
  
public:
  
  // Standard Algorithm Constructor:
  
  HiveAlgG (const std::string& name, ISvcLocator* pSvcLocator);
  ~HiveAlgG();

  // Define the initialize, execute and finalize methods:
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  
  SG::ReadHandleKey<HiveDataObj> m_rdh1;
  SG::WriteHandleKey<HiveDataObj> m_wrh1;

   
  
};
#endif
