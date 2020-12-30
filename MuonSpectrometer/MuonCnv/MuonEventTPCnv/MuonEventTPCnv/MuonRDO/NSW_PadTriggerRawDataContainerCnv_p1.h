#ifndef MUONEVENTTPCNV_NSW_PADTRIGGERRAWDATACONTAINERCNV_P1_H
#define MUONEVENTTPCNV_NSW_PADTRIGGERRAWDATACONTAINERCNV_P1_H

#include "MuonRDO/NSW_PadTriggerRawDataContainer.h"
#include "MuonEventTPCnv/MuonRDO/NSW_PadTriggerRawDataContainer_p1.h"
#include "MuonEventTPCnv/MuonRDO/NSW_PadTriggerRawDataCnv_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

namespace Muon {
class NSW_PadTriggerRawDataContainerCnv_p1 : public T_AthenaPoolTPCnvBase<NSW_PadTriggerRawDataContainer, NSW_PadTriggerRawDataContainer_p1> {
public:
    void persToTrans(const NSW_PadTriggerRawDataContainer_p1* persistentObj, NSW_PadTriggerRawDataContainer* transientObj, MsgStream &log) final;
    void transToPers(const NSW_PadTriggerRawDataContainer* transientObj, NSW_PadTriggerRawDataContainer_p1* persistentObj, MsgStream &log) final;
private:
    NSW_PadTriggerRawDataCnv_p1 m_dataConverter;
};
} // namespace Muon

#endif // MUONEVENTTPCNV_NSW_PADTRIGGERRAWDATACONTAINERCNV_P1_H