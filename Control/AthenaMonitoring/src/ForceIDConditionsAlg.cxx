#include "./ForceIDConditionsAlg.h"

StatusCode ForceIDConditionsAlg::initialize() {
  ATH_CHECK( m_pixelDetEleCollKey.initialize() );
  ATH_CHECK( m_SCTDetEleCollKey.initialize() );
  return StatusCode::SUCCESS;
}
  
StatusCode ForceIDConditionsAlg::execute(const EventContext& ctx) const {
  // ironically, point is to cause side effects (in StoreGate)
  SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> pixdetel{m_pixelDetEleCollKey, ctx};
  SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> sctdetel{m_SCTDetEleCollKey, ctx};
  return StatusCode::SUCCESS;
}
