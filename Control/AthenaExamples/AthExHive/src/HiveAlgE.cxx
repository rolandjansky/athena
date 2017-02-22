#include "HiveAlgE.h"
#include "CxxUtils/make_unique.h"
#include <thread>
#include <chrono>

HiveAlgE::HiveAlgE( const std::string& name, 
                      ISvcLocator* pSvcLocator ) : 
  ::HiveAlgBase( name, pSvcLocator ),
  m_rdh1("c1"),
  m_rdh2("b1"),
  m_wrh1("e1")
{

  declareProperty("Key_R1",m_rdh1);
  declareProperty("Key_R2",m_rdh2);
  declareProperty("Key_W1",m_wrh1);

}

HiveAlgE::~HiveAlgE() {}

StatusCode HiveAlgE::initialize() {
  ATH_MSG_DEBUG("initialize " << name());

  ATH_CHECK( m_rdh1.initialize() );
  ATH_CHECK( m_rdh2.initialize() );
  ATH_CHECK( m_wrh1.initialize() );

  return HiveAlgBase::initialize();
}

StatusCode HiveAlgE::finalize() {
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

StatusCode HiveAlgE::execute() {

  ATH_MSG_DEBUG("execute " << name());
  
  sleep();

  SG::ReadHandle<HiveDataObj> rdh1( m_rdh1 );
  if (!rdh1.isValid()) {
    ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << rdh1.key());
    return StatusCode::FAILURE;
  }

  SG::ReadHandle<HiveDataObj> rdh2( m_rdh2 );
  if (!rdh2.isValid()) {
    ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << rdh2.key());
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("  read: " << rdh1.key() << " = " << rdh1->val() );
  ATH_MSG_INFO("  read: " << rdh2.key() << " = " << rdh2->val() );
  
  SG::WriteHandle<HiveDataObj> wrh1( m_wrh1 );
  wrh1 = CxxUtils::make_unique< HiveDataObj >( HiveDataObj(50000) );
  
  ATH_MSG_INFO("  write: " << wrh1.key() << " = " << wrh1->val() );


  return StatusCode::SUCCESS;

}

