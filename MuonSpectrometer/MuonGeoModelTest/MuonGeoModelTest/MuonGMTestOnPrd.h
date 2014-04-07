/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 test MuonGeoModel from digits to pos. in space
 ----------------------------------------------
 Copyright (C) 2004 by ATLAS Collaboration
 ***************************************************************************/

#ifndef MUONGEOMODEL_MUONGMTESTONPRD_H
# define MUONGEOMODEL_MUONGMTESTONPRD_H

//<<<<<< INCLUDES                                                       >>>>>>
//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>
//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

#include "GaudiKernel/Algorithm.h"

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


using namespace MuonGM;
using namespace Muon;



class StoreGateSvc;
class ActiveStoreSvc;


//class MuonReadoutElement;


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>
class MuonGMTestOnPrd: public Algorithm
{
public:
    
    MuonGMTestOnPrd(const std::string& name, ISvcLocator* pSvcLocator);
    ~MuonGMTestOnPrd();
    
    StatusCode	initialize();
    StatusCode 	execute();
    StatusCode 	finalize();

private:

  // bool
  bool m_debug, m_verbose;
  
  
    // Pointers to the event
    StoreGateSvc*      	        p_EventStore;
    // ActiveStoreSvc*      	        p_ActiveStore;

    const MuonDetectorManager*	  p_MuonMgr;
    const RpcIdHelper*            p_RpcIdHelper;
    const TgcIdHelper*            p_TgcIdHelper;
    const CscIdHelper*            p_CscIdHelper;
    const MdtIdHelper*            p_MdtIdHelper;

    bool _mdt;
    bool _rpc;
    bool _tgc;
    bool _csc;
    bool _check_misal;
    
    void processMdtCollection(const MdtPrepDataCollection* mdtColl,
                              Identifier& collid,
                              IdentifierHash& collidh) const;
    
    void processMdtCollectionOld(const MdtPrepDataCollection* mdtColl,
                                 Identifier& collid,
                                 IdentifierHash& collidh) const;
    

};


#endif // MUONGEOMODEL_MUONGMTESTONPRD_H
