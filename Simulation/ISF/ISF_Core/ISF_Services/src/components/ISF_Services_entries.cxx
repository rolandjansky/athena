#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../AFIIEnvelopeDefSvc.h"
#include "../GeoIDSvc.h"
#include "../InputConverter.h"
#include "../ISFEnvelopeDefSvc.h"
#include "../ParticleBrokerDynamicOnReadIn.h"
#include "../ParticleKillerSimSvc.h"
#include "../SimHitSvc.h"
#include "../TruthSvc.h"

DECLARE_NAMESPACE_SERVICE_FACTORY( ISF , AFIIEnvelopeDefSvc            )
DECLARE_NAMESPACE_SERVICE_FACTORY( ISF , GeoIDSvc                      )
DECLARE_NAMESPACE_SERVICE_FACTORY( ISF , InputConverter                )
DECLARE_NAMESPACE_SERVICE_FACTORY( ISF , ISFEnvelopeDefSvc             )
DECLARE_NAMESPACE_SERVICE_FACTORY( ISF , ParticleBrokerDynamicOnReadIn )
DECLARE_NAMESPACE_SERVICE_FACTORY( ISF , ParticleKillerSimSvc          )
DECLARE_NAMESPACE_SERVICE_FACTORY( ISF , SimHitSvc                     )
DECLARE_NAMESPACE_SERVICE_FACTORY( ISF , TruthSvc                      )

DECLARE_FACTORY_ENTRIES( ISF_Services ) {
  DECLARE_NAMESPACE_SERVICE( ISF ,  AFIIEnvelopeDefSvc            )
  DECLARE_NAMESPACE_SERVICE( ISF ,  GeoIDSvc                      )
  DECLARE_NAMESPACE_SERVICE( ISF ,  InputConverter                )
  DECLARE_NAMESPACE_SERVICE( ISF ,  ISFEnvelopeDefSvc             )
  DECLARE_NAMESPACE_SERVICE( ISF ,  ParticleBrokerDynamicOnReadIn )
  DECLARE_NAMESPACE_SERVICE( ISF ,  ParticleKillerSimSvc          )
  DECLARE_NAMESPACE_SERVICE( ISF ,  SimHitSvc                     )
  DECLARE_NAMESPACE_SERVICE( ISF ,  TruthSvc                      )
}
