#ifndef MUONEVENTATHENAPOOL_NSW_PADTRIGGERRAWDATACONTAINERCNV_H
#define MUONEVENTATHENAPOOL_NSW_PADTRIGGERRAWDATACONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "MuonRDO/NSW_PadTriggerRawDataContainer.h"
#include "MuonEventTPCnv/MuonRDO/NSW_PadTriggerRawDataContainer_p1.h"
#include "MuonEventTPCnv/MuonRDO/NSW_PadTriggerRawDataContainerCnv_p1.h"

using NSW_PadTriggerRawDataContainer_PERS = Muon::NSW_PadTriggerRawDataContainer_p1;
using NSW_PadTriggerRawDataContainerCnvBase = T_AthenaPoolCustomCnv<Muon::NSW_PadTriggerRawDataContainer, NSW_PadTriggerRawDataContainer_PERS>;

class NSW_PadTriggerRawDataContainerCnv : public NSW_PadTriggerRawDataContainerCnvBase {
public:
    NSW_PadTriggerRawDataContainerCnv(ISvcLocator* svcLocator);
    NSW_PadTriggerRawDataContainer_PERS* createPersistent(Muon::NSW_PadTriggerRawDataContainer* transientContainer) final;
    Muon::NSW_PadTriggerRawDataContainer* createTransient() final;
private:
    Muon::NSW_PadTriggerRawDataContainerCnv_p1 m_TPConverter;
};

#endif