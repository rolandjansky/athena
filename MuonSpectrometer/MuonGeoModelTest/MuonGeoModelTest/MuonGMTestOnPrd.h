/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 test MuonGeoModel from digits to pos. in space
 ----------------------------------------------
 ***************************************************************************/

#ifndef MUONGEOMODEL_MUONGMTESTONPRD_H
# define MUONGEOMODEL_MUONGMTESTONPRD_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"

#include "MuonPrepRawData/MdtPrepDataContainer.h"
#include "MuonPrepRawData/MdtPrepDataCollection.h"
#include "MuonPrepRawData/MdtPrepData.h"

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"


class StoreGateSvc;
class ActiveStoreSvc;


//class MuonReadoutElement;


class MuonGMTestOnPrd: public AthAlgorithm
{
public:
    
    MuonGMTestOnPrd(const std::string& name, ISvcLocator* pSvcLocator);
    ~MuonGMTestOnPrd();
    
    StatusCode	initialize();
    StatusCode 	execute();
    StatusCode 	finalize();

private:

    const MuonGM::MuonDetectorManager*	  m_MuonMgr;
    const RpcIdHelper*            m_RpcIdHelper;
    const TgcIdHelper*            m_TgcIdHelper;
    const CscIdHelper*            m_CscIdHelper;
    const MdtIdHelper*            m_MdtIdHelper;

    bool m_mdt;
    bool m_rpc;
    bool m_tgc;
    bool m_csc;
    bool m_check_misal;
    
    void processMdtCollection(const Muon::MdtPrepDataCollection* mdtColl,
                              Identifier& collid,
                              IdentifierHash& collidh) const;
    
    void processMdtCollectionOld(const Muon::MdtPrepDataCollection* mdtColl,
                                 Identifier& collid,
                                 IdentifierHash& collidh) const;
    

};


#endif // MUONGEOMODEL_MUONGMTESTONPRD_H
