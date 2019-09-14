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
class CscCondDbData;

namespace Muon {
  class MuonIdHelperTool;
}



class CscCondDbAlg: public AthAlgorithm{

public:

    CscCondDbAlg( const std::string & name, ISvcLocator* svc);
    virtual ~CscCondDbAlg() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode execute   () override;

 
private:

    virtual StatusCode loadDataHv     (EventIDRange &, std::unique_ptr<CscCondDbData>&);

    virtual StatusCode loadData       (EventIDRange &, std::unique_ptr<CscCondDbData>&, SG::ReadCondHandle<CondAttrListCollection>, const std::string, bool = false);
    virtual StatusCode loadDataF001   (EventIDRange &, std::unique_ptr<CscCondDbData>&);
    virtual StatusCode loadDataNoise  (EventIDRange &, std::unique_ptr<CscCondDbData>&);
    virtual StatusCode loadDataPed    (EventIDRange &, std::unique_ptr<CscCondDbData>&);
    virtual StatusCode loadDataPSlope (EventIDRange &, std::unique_ptr<CscCondDbData>&);
    virtual StatusCode loadDataRMS    (EventIDRange &, std::unique_ptr<CscCondDbData>&);
    virtual StatusCode loadDataStatus (EventIDRange &, std::unique_ptr<CscCondDbData>&);
    virtual StatusCode loadDataT0Base (EventIDRange &, std::unique_ptr<CscCondDbData>&);
    virtual StatusCode loadDataT0Phase(EventIDRange &, std::unique_ptr<CscCondDbData>&);

    virtual StatusCode cacheVersion1   (std::string   , std::unique_ptr<CscCondDbData>&, const std::string);
    virtual StatusCode cacheVersion2   (std::string   , std::unique_ptr<CscCondDbData>&, const std::string);
    virtual StatusCode cacheVersion2ASM(std::string   , std::unique_ptr<CscCondDbData>&, const std::string);
    virtual StatusCode getAsmScope(int, int&, int&, int&, int&, int&);

    virtual StatusCode recordParameter(unsigned int  , std::string, std::unique_ptr<CscCondDbData>&, const std::string);
    virtual StatusCode recordParameter(IdentifierHash, std::string, std::unique_ptr<CscCondDbData>&, const std::string);
	virtual StatusCode recordParameterF001   (IdentifierHash, std::string, std::unique_ptr<CscCondDbData>&);
	virtual StatusCode recordParameterNoise  (IdentifierHash, std::string, std::unique_ptr<CscCondDbData>&);
	virtual StatusCode recordParameterPed    (IdentifierHash, std::string, std::unique_ptr<CscCondDbData>&);
	virtual StatusCode recordParameterPSlope (IdentifierHash, std::string, std::unique_ptr<CscCondDbData>&);
	virtual StatusCode recordParameterRMS    (IdentifierHash, std::string, std::unique_ptr<CscCondDbData>&);
	virtual StatusCode recordParameterStatus (IdentifierHash, std::string, std::unique_ptr<CscCondDbData>&);
	virtual StatusCode recordParameterT0Base (IdentifierHash, std::string, std::unique_ptr<CscCondDbData>&);
	virtual StatusCode recordParameterT0Phase(IdentifierHash, std::string, std::unique_ptr<CscCondDbData>&);

    bool m_isOnline{false};
    bool m_isData{false};  
    bool m_isRun1{false};   

    bool m_phiSwapVersion1Strings{false};
    bool m_onlineOfflinePhiFlip{false};

    ServiceHandle<ICondSvc> m_condSvc;
    ToolHandle<Muon::MuonIdHelperTool> m_idHelper;
    std::string m_defaultDatabaseReadVersion;
 
    SG::WriteCondHandleKey<CscCondDbData> m_writeKey{this, "WriteKey", "CscCondDbData", "Key of output CSC condition data"};    

    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_folder_da_hv     {this, "ReadKey_HV", "/CSC/DCS/LAYERSTATE", "Key of input CSC condition data HV"       };
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_folder_da_f001   {this, "ReadKey_FT", "/CSC/FTHOLD"        , "Key of input CSC condition data F001"     };
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_folder_da_noise  {this, "ReadKey_NO", "/CSC/NOISE"         , "Key of input CSC condition data NOISE"    };
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_folder_da_ped    {this, "ReadKey_PD", "/CSC/PED"           , "Key of input CSC condition data PEDESTALS"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_folder_da_pslope {this, "ReadKey_PS", "/CSC/PSLOPE"        , "Key of input CSC condition data PSLOPE"   };
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_folder_da_rms    {this, "ReadKey_RM", "/CSC/RMS"           , "Key of input CSC condition data RMS"      };
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_folder_da_status {this, "ReadKey_ST", "/CSC/STAT"          , "Key of input CSC condition data STATUS"   };
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_folder_da_t0base {this, "ReadKey_TB", "/CSC/T0BASE"        , "Key of input CSC condition data T0BASE"   };
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_folder_da_t0phase{this, "ReadKey_TP", "/CSC/T0PHASE"       , "Key of input CSC condition data T0PHASE"  };


    // getParameter
    template <typename T>
    StatusCode getParameter(IdentifierHash chanHash, std::string data, T& token){
    
        // next element is the status bit
        std::istringstream iss(data);
        iss >> token;
        
        ATH_MSG_DEBUG("Recorded token " << token << " for channelHash " << chanHash);
        return StatusCode::SUCCESS;
    }

};


#endif
