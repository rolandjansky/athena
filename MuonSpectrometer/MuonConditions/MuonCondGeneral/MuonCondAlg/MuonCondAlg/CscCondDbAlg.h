/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDALG_CSCCONDDBALG_H
#define MUONCONDALG_CSCCONDDBALG_H

//STL includes
#include <string>
#include <sstream>
#include <vector>
#include <zlib.h>

//Gaudi includes
#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/ToolHandle.h"

//Athena includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonCondData/CscCondDbData.h"
#include "MuonCondSvc/MdtStringUtils.h"

//forward declarations
class Identifier;
class IdentifierHash;
class MuonIdHelper;
class StatusCode;

namespace Muon {
  class MuonIdHelperTool;
}



class CscCondDbAlg: public AthReentrantAlgorithm{

public:

    CscCondDbAlg( const std::string & name, ISvcLocator* svc);
    virtual ~CscCondDbAlg() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode execute   (const EventContext&) const override;

 
private:

    StatusCode loadDataHv   (EventIDRange &, CscCondDbData*, const EventContext&) const;
    StatusCode loadDataStat (EventIDRange &, CscCondDbData*, const EventContext&) const;
    StatusCode cacheVersion1(std::string   , CscCondDbData*) const;
    StatusCode cacheVersion2(std::string   , CscCondDbData*) const;
	StatusCode onlineToOfflineIds(const unsigned int &, Identifier &, Identifier &) const;

    bool m_isOnline{false};
    bool m_isData{false};  
    bool m_isRun1{false};   

    bool m_phiSwapVersion1Strings{false};
    bool m_onlineOfflinePhiFlip{false};

    ServiceHandle<ICondSvc> m_condSvc;
    ToolHandle<Muon::MuonIdHelperTool> m_idHelper;
    std::string m_defaultDatabaseReadVersion;
 
    SG::WriteCondHandleKey<CscCondDbData> m_writeKey{this, "WriteKey", "CscCondDbData", "Key of output CSC condition data"};    

    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_folder_da_hv  {this, "ReadKey_HV", "/CSC/DCS/LAYERSTATE", "Key of input CSC condition data HV"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_folder_da_stat{this, "ReadKey_ST", "/CSC/STAT"          , "Key of input CSC condition data stat"};

};


#endif
