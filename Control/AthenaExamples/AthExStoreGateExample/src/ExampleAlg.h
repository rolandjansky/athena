#ifndef EXAMPLEALG_H
#define EXAMPLEALG_H


#include "AthExStoreGateExample/MyDataObj.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"


// An example algorithm that reads and writes objects from the event store
// using handles.
class ExampleAlg
  : public AthReentrantAlgorithm
{
public:
  ExampleAlg (const std::string& name, ISvcLocator* svcLocator);
  virtual StatusCode initialize() override;
  virtual StatusCode execute_r (const EventContext& ctx) const override;

private:
  // Declare the keys used to access the data: one for reading and one
  // for writing.
  SG::ReadHandleKey<MyDataObj> m_readKey;
  SG::WriteHandleKey<MyDataObj> m_writeKey;
};


#endif
