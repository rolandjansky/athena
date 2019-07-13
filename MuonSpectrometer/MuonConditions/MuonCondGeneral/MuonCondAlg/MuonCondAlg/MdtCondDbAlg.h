/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDALG_MDTCONDDBALG_H
#define MUONCONDALG_MDTCONDDBALG_H

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
#include "MuonCondInterface/IMDT_MapConversion.h"
#include "MuonCondData/MdtCondDbData.h"
#include "MuonCondSvc/MdtStringUtils.h"

//forward declarations
class Identifier;
class MuonIdHelper;
class StatusCode;
class IMDT_MapConversion;

namespace Muon {
  class MuonIdHelperTool;
}


class MdtCondDbAlg: public AthAlgorithm{

public:

    MdtCondDbAlg( const std::string & name, ISvcLocator* svc);
    virtual ~MdtCondDbAlg() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode execute   () override;
    virtual StatusCode finalize  () override;

 
private:

    virtual StatusCode loadDataPsHv           (EventIDRange &, std::unique_ptr<MdtCondDbData>&);
    virtual StatusCode loadDataPsLv           (EventIDRange &, std::unique_ptr<MdtCondDbData>&);
    virtual StatusCode loadDataHv             (EventIDRange &, std::unique_ptr<MdtCondDbData>&);
    virtual StatusCode loadDataLv             (EventIDRange &, std::unique_ptr<MdtCondDbData>&);
    virtual StatusCode loadDataDroppedChambers(EventIDRange &, std::unique_ptr<MdtCondDbData>&);
    virtual StatusCode loadMcDroppedChambers  (EventIDRange &, std::unique_ptr<MdtCondDbData>&);
    virtual StatusCode loadMcDeadElements     (EventIDRange &, std::unique_ptr<MdtCondDbData>&);
    virtual StatusCode loadMcDeadTubes        (EventIDRange &, std::unique_ptr<MdtCondDbData>&);
    virtual StatusCode loadMcNoisyChannels    (EventIDRange &, std::unique_ptr<MdtCondDbData>&);

    bool m_isOnline;
    bool m_isData;  
    bool m_isRun1;   
    bool m_checkOnSetPoint;

    ServiceHandle<ICondSvc> m_condSvc;
    ToolHandle<IMDT_MapConversion> m_condMapTool;
    ToolHandle<Muon::MuonIdHelperTool> m_idHelper;
 
    SG::WriteCondHandleKey<MdtCondDbData> m_writeKey{this, "WriteKey", "MdtCondDbData", "Key of output MDT condition data"};    

    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_folder_da_pshv           {this, "ReadKey_DataR1_HV", "/MDT/DCS/PSHVMLSTATE"      , "Key of input MDT condition data for Run 1 data HV"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_folder_da_psv0           {this, "ReadKey_DataR1_V0", "/MDT/DCS/PSV0SETPOINTS"    , "Key of input MDT condition data for Run 1 data V0"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_folder_da_psv1           {this, "ReadKey_DataR1_V1", "/MDT/DCS/PSV1SETPOINTS"    , "Key of input MDT condition data for Run 1 data V1"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_folder_da_pslv           {this, "ReadKey_DataR1_LV", "/MDT/DCS/PSLVCHSTATE"      , "Key of input MDT condition data for Run 1 data LV"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_folder_da_droppedChambers{this, "ReadKey_DataR1_DC", "/MDT/DCS/DROPPEDCH"        , "Key of input MDT condition data for Run 1 data dropped chambers"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_folder_da_hv             {this, "ReadKey_DataR2_HV", "/MDT/DCS/HV"               , "Key of input MDT condition data for Run 2 data HV"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_folder_da_lv             {this, "ReadKey_DataR2_LV", "/MDT/DCS/LV"               , "Key of input MDT condition data for Run 2 data LV"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_folder_mc_droppedChambers{this, "ReadKey_MC_DC"    , "/MDT/DCS/DROPPEDCH"        , "Key of input MDT condition data for MC dropped chambers"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_folder_mc_deadElements   {this, "ReadKey_MC_DE"    , "/MDT/DQMF/DEAD_ELEMENT"    , "Key of input MDT condition data for MC dead elements"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_folder_mc_deadTubes      {this, "ReadKey_MC_DT"    , "/MDT/TUBE_STATUS/DEAD_TUBE", "Key of input MDT condition data for MC dead tubes"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_folder_mc_noisyChannels  {this, "ReadKey_MC_NC"    , "/MDT/DCS/PSLVCHSTATE"      , "Key of input MDT condition data for MC noisy channels"};
 
};


#endif
