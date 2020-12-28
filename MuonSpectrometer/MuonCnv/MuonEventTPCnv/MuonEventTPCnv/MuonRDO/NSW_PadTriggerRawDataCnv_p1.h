#ifndef MUONEVENTTPCNV_NSW_PADTRIGGERRAWDATACNV_P1_H
#define MUONEVENTTPCNV_NSW_PADTRIGGERRAWDATACNV_P1_H

#include "MuonRDO/NSW_PadTriggerRawData.h"
#include "MuonEventTPCnv/MuonRDO/NSW_PadTriggerRawData_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include <algorithm>

class MsgStream;

namespace Muon {
class NSW_PadTriggerRawDataCnv_p1 : public T_AthenaPoolTPCnvBase<NSW_PadTriggerRawData, NSW_PadTriggerRawData_p1> {
public:
    void persToTrans(const NSW_PadTriggerRawData_p1* persistentObj, NSW_PadTriggerRawData* transientObj, MsgStream &log) final;
    void transToPers(const NSW_PadTriggerRawData* transientObj, NSW_PadTriggerRawData_p1* persistentObj, MsgStream &log) final;
    NSW_PadTriggerRawData* createTransient(const NSW_PadTriggerRawData_p1* persistentObj, MsgStream& log) final;
};
} // namespace Muon

#endif // MUONEVENTTPCNV_NSW_PADTRIGGERRAWDATACNV_P1_H