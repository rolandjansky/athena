#ifndef ATHEXHIVE_BASEALG_H
#define ATHEXHIVE_BASEALG_H 1


#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/WriteHandle.h"
#include "AthExHive/HiveDataObj.h"
#include "GaudiKernel/ContextSpecificPtr.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthExHive/IHiveExSvc.h"
#include "rGen.h"

#include <string>

class HiveAlgBase  :  public AthAlgorithm, public rGen {
  
public:
  
 HiveAlgBase (const std::string& name, ISvcLocator* pSvcLocator);
  ~HiveAlgBase();
  
  // Define the initialize, execute and finalize methods:
  
  StatusCode initialize();
  // StatusCode execute();
  // StatusCode finalize();

protected:

  int sleep_for(int);
  int sleep();

  ServiceHandle<IHiveExSvc> m_hes;
  
private:

  int m_time;
  

};
#endif
