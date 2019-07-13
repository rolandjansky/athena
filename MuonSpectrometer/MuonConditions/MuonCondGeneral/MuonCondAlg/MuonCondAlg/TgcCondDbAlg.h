/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDALG_TGCCONDDBALG_H
#define MUONCONDALG_TGCCONDDBALG_H

//STL includes
#include <string>
#include <vector>
#include <zlib.h>

//Gaudi includes
#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/ToolHandle.h"

//Athena includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "Identifier/Identifier.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonCondData/TgcCondDbData.h"
#include "MuonCondSvc/MdtStringUtils.h"

//forward declarations
class Identifier;
class MuonIdHelper;
class StatusCode;

namespace Muon {
  class MuonIdHelperTool;
}



class TgcCondDbAlg: public AthAlgorithm{

public:

    TgcCondDbAlg( const std::string & name, ISvcLocator* svc);
    virtual ~TgcCondDbAlg() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode execute   () override;
    virtual StatusCode finalize  () override;

 
private:

    virtual StatusCode loadDetectorStatus(EventIDRange &, std::unique_ptr<TgcCondDbData>&);

    bool m_isOnline;
    bool m_isData;  
    bool m_isRun1;   

    ServiceHandle<ICondSvc> m_condSvc;
    ToolHandle<Muon::MuonIdHelperTool> m_idHelper;
 
    SG::WriteCondHandleKey<TgcCondDbData> m_writeKey{this, "WriteKey", "TgcCondDbData", "Key of output TGC condition data"};    
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_folder_detectorStatus{this, "ReadKey", "/TGC/1/DetectorStatus", "Key of input TGC condition data"};

};


#endif
