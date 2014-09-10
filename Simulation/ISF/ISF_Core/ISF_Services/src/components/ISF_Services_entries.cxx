#include "GaudiKernel/DeclareFactoryEntries.h"
#include "ISF_Services/ParticleBrokerDynamicOnReadIn.h"
#include "ISF_Services/SimHitSvc.h"
#include "ISF_Services/ParticleKillerSimSvc.h"
#include "ISF_Services/ISFEnvelopeDefSvc.h"
#include "ISF_Services/AFIIEnvelopeDefSvc.h"
#include "ISF_Services/GeoIDSvc.h"

DECLARE_NAMESPACE_SERVICE_FACTORY( ISF , ParticleBrokerDynamicOnReadIn    )
DECLARE_NAMESPACE_SERVICE_FACTORY( ISF , SimHitSvc                     )
DECLARE_NAMESPACE_SERVICE_FACTORY( ISF , ParticleKillerSimSvc          )
DECLARE_NAMESPACE_SERVICE_FACTORY( ISF , ISFEnvelopeDefSvc             )
DECLARE_NAMESPACE_SERVICE_FACTORY( ISF , AFIIEnvelopeDefSvc            )
DECLARE_NAMESPACE_SERVICE_FACTORY( ISF , GeoIDSvc                      )

DECLARE_FACTORY_ENTRIES( ISF_Services ) {
  DECLARE_NAMESPACE_SERVICE( ISF ,  ParticleBrokerDynamicOnReadIn    )
  DECLARE_NAMESPACE_SERVICE( ISF ,  SimHitSvc                     )
  DECLARE_NAMESPACE_SERVICE( ISF ,  ParticleKillerSimSvc          )
  DECLARE_NAMESPACE_SERVICE( ISF ,  ISFEnvelopeDefSvc             )
  DECLARE_NAMESPACE_SERVICE( ISF ,  AFIIEnvelopeDefSvc            )
  DECLARE_NAMESPACE_SERVICE( ISF ,  GeoIDSvc                      )
}
