#ifndef STGCDIGITCONTAINERCNV_p1_H
#define STGCDIGITCONTAINERCNV_p1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonDigitContainer/sTgcDigitContainer.h"
#include "MuonEventTPCnv/MuonDigitContainer/MuonDigitContainer_p1.h" // Has typedefs to all concrete types e.g. STGC_DigitContainer_p1

#include <iostream>
/*******************************************************************************/
class StoreGateSvc;
class sTgcIdHelper;
namespace MuonGM {class MuonDetectorManager;}
/*******************************************************************************/
namespace Muon{
  /** Class to handle the conversion of the transient sTgcDigitContainer into its persistent representation */
  class STGC_DigitContainerCnv_p1 : public T_AthenaPoolTPCnvBase<sTgcDigitContainer, Muon::STGC_DigitContainer_p1>
    {
    public:
      STGC_DigitContainerCnv_p1() : m_sTgcId(0), m_storeGate(0), m_muonDetMgr(0), m_isInitialized(0)  {};

	virtual void	persToTrans(const Muon::STGC_DigitContainer_p1* persCont, sTgcDigitContainer* transCont, MsgStream &log) ;
	virtual void	transToPers(const sTgcDigitContainer* transCont, Muon::STGC_DigitContainer_p1* persCont, MsgStream &log) ;
	virtual sTgcDigitContainer* createTransient(const Muon::STGC_DigitContainer_p1* persObj, MsgStream& log);

    private:
	const sTgcIdHelper *m_sTgcId;
	StoreGateSvc *m_storeGate;
	const MuonGM::MuonDetectorManager* m_muonDetMgr;
	bool m_isInitialized;
	StatusCode initialize(MsgStream &log);
    };
}
/*******************************************************************************/

#endif


