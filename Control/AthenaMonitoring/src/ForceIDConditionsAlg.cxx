#include "./ForceIDConditionsAlg.h"

StatusCode ForceIDConditionsAlg::initialize() {
  ATH_CHECK( m_pixelDetEleCollKey.initialize() );
  ATH_CHECK( m_SCTDetEleCollKey.initialize() );
  return StatusCode::SUCCESS;
}
