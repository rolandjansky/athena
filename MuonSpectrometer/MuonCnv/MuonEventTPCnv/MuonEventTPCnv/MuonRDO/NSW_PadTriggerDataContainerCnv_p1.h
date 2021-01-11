#ifndef MUONEVENTTPCNV_NSW_PADTRIGGERDATACONTAINERCNV_P1_H
#define MUONEVENTTPCNV_NSW_PADTRIGGERDATACONTAINERCNV_P1_H

#include "MuonRDO/NSW_PadTriggerDataContainer.h"
#include "MuonEventTPCnv/MuonRDO/NSW_PadTriggerDataContainer_p1.h"
#include "MuonEventTPCnv/MuonRDO/NSW_PadTriggerSegmentCnv_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

namespace Muon {
class NSW_PadTriggerDataContainerCnv_p1 : public T_AthenaPoolTPCnvBase<NSW_PadTriggerDataContainer, NSW_PadTriggerDataContainer_p1> {
public:
    void persToTrans(const NSW_PadTriggerDataContainer_p1* persistentObj, NSW_PadTriggerDataContainer* transientObj, MsgStream &log) final;
    void transToPers(const NSW_PadTriggerDataContainer* transientObj, NSW_PadTriggerDataContainer_p1* persistentObj, MsgStream &log) final;
private:
    NSW_PadTriggerSegmentCnv_p1 m_segmentConverter;
};
} // namespace Muon

#endif // MUONEVENTTPCNV_NSW_PADTRIGGERDATACONTAINERCNV_P1_H