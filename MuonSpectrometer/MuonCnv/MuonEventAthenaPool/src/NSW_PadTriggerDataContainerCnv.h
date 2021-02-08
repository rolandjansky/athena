#ifndef MUONEVENTATHENAPOOL_NSW_PADTRIGGERDATACONTAINERCNV_H
#define MUONEVENTATHENAPOOL_NSW_PADTRIGGERDATACONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "MuonRDO/NSW_PadTriggerDataContainer.h"
#include "MuonEventTPCnv/MuonRDO/NSW_PadTriggerDataContainer_p1.h"
#include "MuonEventTPCnv/MuonRDO/NSW_PadTriggerDataContainerCnv_p1.h"

using NSW_PadTriggerDataContainer_PERS = Muon::NSW_PadTriggerDataContainer_p1;
using NSW_PadTriggerDataContainerCnvBase = T_AthenaPoolCustomCnv<Muon::NSW_PadTriggerDataContainer, NSW_PadTriggerDataContainer_PERS>;

class NSW_PadTriggerDataContainerCnv : public NSW_PadTriggerDataContainerCnvBase {
public:
    NSW_PadTriggerDataContainerCnv(ISvcLocator* svcLocator);
    NSW_PadTriggerDataContainer_PERS* createPersistent(Muon::NSW_PadTriggerDataContainer* transientContainer) final;
    Muon::NSW_PadTriggerDataContainer* createTransient() final;
private:
    Muon::NSW_PadTriggerDataContainerCnv_p1 m_TPConverter;
};

#endif // MUONEVENTATHENAPOOL_NSW_PADTRIGGERDATACONTAINERCNV_H