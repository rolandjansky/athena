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
#include "AthenaBaseComps/AthAlgorithm.h"
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



class CscCondDbAlg: public AthAlgorithm{

public:

    CscCondDbAlg( const std::string & name, ISvcLocator* svc);
    virtual ~CscCondDbAlg() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode execute   () override;
    virtual StatusCode finalize  () override;

 
private:

    virtual StatusCode loadDataHv   (EventIDRange &, std::unique_ptr<CscCondDbData>&);
    virtual StatusCode loadDataStat (EventIDRange &, std::unique_ptr<CscCondDbData>&);
    virtual StatusCode cacheVersion1(std::string   , std::unique_ptr<CscCondDbData>&);
    virtual StatusCode cacheVersion2(std::string   , std::unique_ptr<CscCondDbData>&);
	virtual StatusCode onlineToOfflineIds(const unsigned int &, Identifier &, Identifier &) const;

    bool m_isOnline;
    bool m_isData;  
    bool m_isRun1;   

    bool m_phiSwapVersion1Strings;
    bool m_onlineOfflinePhiFlip;

    ServiceHandle<ICondSvc> m_condSvc;
    ToolHandle<Muon::MuonIdHelperTool> m_idHelper;
    std::string m_defaultDatabaseReadVersion;
 
    SG::WriteCondHandleKey<CscCondDbData> m_writeKey{this, "WriteKey", "CscCondDbData", "Key of output CSC condition data"};    

    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_folder_da_hv  {this, "ReadKey_HV", "/CSC/DCS/LAYERSTATE", "Key of input CSC condition data HV"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_folder_da_stat{this, "ReadKey_ST", "/CSC/STAT"          , "Key of input CSC condition data stat"};

};


#endif
