#ifndef LARFLATCONDITIONSALG_H
#define LARFLATCONDITIONSALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "GaudiKernel/ICondSvc.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "LArCOOLConditions/LArHVScaleCorrFlat.h"

class LArFlatConditionsAlg: public AthAlgorithm {

  LArFlatConditionsAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~LArFlatConditionsAlg();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize() {return StatusCode::SUCCESS;}

 private:
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKey;
  SG::WriteCondHandleKey<LArHVScaleCorrFlat>  m_writeKey;
  ServiceHandle<ICondSvc> m_condSvc;

};


#endif
