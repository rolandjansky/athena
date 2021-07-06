#ifndef MUONEVENTTPCNV_NSW_PADTRIGGERSEGMENTCNV_P1_H
#define MUONEVENTTPCNV_NSW_PADTRIGGERSEGMENTCNV_P1_H

#include "MuonRDO/NSW_PadTriggerSegment.h"
#include "MuonEventTPCnv/MuonRDO/NSW_PadTriggerSegment_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include <algorithm>

class MsgStream;

namespace Muon {
class NSW_PadTriggerSegmentCnv_p1 : public T_AthenaPoolTPCnvBase<NSW_PadTriggerSegment, NSW_PadTriggerSegment_p1> {
public:
    void persToTrans(const NSW_PadTriggerSegment_p1* persistentObj, NSW_PadTriggerSegment* transientObj, MsgStream &log) final;
    void transToPers(const NSW_PadTriggerSegment* transientObj, NSW_PadTriggerSegment_p1* persistentObj, MsgStream &log) final;
};
} // namespace Muon

#endif // MUONEVENTTPCNV_NSW_PADTRIGGERSEGMENTCNV_P1_H