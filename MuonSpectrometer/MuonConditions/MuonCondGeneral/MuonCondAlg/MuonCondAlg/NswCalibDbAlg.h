/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDALG_NSWCALIBDBALG_H
#define MUONCONDALG_NSWCALIBDBALG_H

// STL includes
#include <string>
#include <vector>

// Gaudi includes
#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// Athena includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

// Muon includes
#include "MuonCondData/NswCalibDbTimeChargeData.h"
#include "MuonCondData/NswCalibDbThresholdData.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"


// Forward declarations
class CondAttrListCollection;


class NswCalibDbAlg: public AthReentrantAlgorithm{

public:

    using AthReentrantAlgorithm::AthReentrantAlgorithm;
    virtual ~NswCalibDbAlg() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode execute (const EventContext&) const override;

 
private:

    typedef SG::WriteCondHandleKey<NswCalibDbTimeChargeData> writeKeyTdoPdo_t;
    typedef SG::WriteCondHandleKey<NswCalibDbThresholdData > writeKeyVmm_t;
    typedef SG::ReadCondHandleKey<CondAttrListCollection> readKey_t;

	StatusCode loadDataCalibMm  (const EventContext& ctx) const;
	StatusCode loadDataCalibStgc(const EventContext& ctx) const;

	StatusCode loadTimeChargeData(const EventContext&, const readKey_t&, const std::string&) const;
	StatusCode loadThresholdData (const EventContext&, const readKey_t&                   ) const;

	bool buildChannelId(Identifier*& channelId, unsigned int elinkId, unsigned int vmm, unsigned int channel) const;
    
    Gaudi::Property<bool> m_isOnline{this, "isOnline", false, "This algo is being used online"};
    Gaudi::Property<bool> m_isData  {this, "isData"  , true , "Processing data"};

    ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
 
    writeKeyTdoPdo_t m_writeKey_tdopdo{this, "WriteKey_TdoPdo", "NswCalibDbTimeChargeData", "Key of output calibration data (TDOs and PDOs)" };
    writeKeyVmm_t    m_writeKey_vmm   {this, "WriteKey_VMM"   , "NswCalibDbThresholdData" , "Key of output calibration data (VMM thresholds)"};

    readKey_t m_readKey_data_mm_sidea_tdo  {this, "ReadKey_MM_SIDEA_TDO"  , "/MDT/MM/TIME/SIDEA"   , "Key of input MM condition data for side A data TDO"};
    readKey_t m_readKey_data_mm_sidec_tdo  {this, "ReadKey_MM_SIDEC_TDO"  , "/MDT/MM/TIME/SIDEC"   , "Key of input MM condition data for side C data TDO"};
    readKey_t m_readKey_data_mm_sidea_pdo  {this, "ReadKey_MM_SIDEA_PDO"  , "/MDT/MM/CHARGE/SIDEA" , "Key of input MM condition data for side A data PDO"};
    readKey_t m_readKey_data_mm_sidec_pdo  {this, "ReadKey_MM_SIDEC_PDO"  , "/MDT/MM/CHARGE/SIDEC" , "Key of input MM condition data for side C data PDO"};
    readKey_t m_readKey_data_mm_sidea_vmm  {this, "ReadKey_MM_SIDEA_VMM"  , "/MDT/MM/VMM/SIDEA"    , "Key of input MM condition data for side A data VMM"};
    readKey_t m_readKey_data_mm_sidec_vmm  {this, "ReadKey_MM_SIDEC_VMM"  , "/MDT/MM/VMM/SIDEC"    , "Key of input MM condition data for side C data VMM"};
    readKey_t m_readKey_data_stgc_sidea_tdo{this, "ReadKey_STGC_SIDEA_TDO", "/TGC/NSW/TIME/SIDEA"  , "Key of input sTGC condition data for side A data TDO"};
    readKey_t m_readKey_data_stgc_sidec_tdo{this, "ReadKey_STGC_SIDEC_TDO", "/TGC/NSW/TIME/SIDEC"  , "Key of input sTGC condition data for side C data TDO"};
    readKey_t m_readKey_data_stgc_sidea_pdo{this, "ReadKey_STGC_SIDEA_PDO", "/TGC/NSW/CHARGE/SIDEA", "Key of input sTGC condition data for side A data PDO"};
    readKey_t m_readKey_data_stgc_sidec_pdo{this, "ReadKey_STGC_SIDEC_PDO", "/TGC/NSW/CHARGE/SIDEC", "Key of input sTGC condition data for side C data PDO"};
    readKey_t m_readKey_data_stgc_sidea_vmm{this, "ReadKey_STGC_SIDEA_VMM", "/TGC/NSW/VMM/SIDEA"   , "Key of input sTGC condition data for side A data VMM"};
    readKey_t m_readKey_data_stgc_sidec_vmm{this, "ReadKey_STGC_SIDEC_VMM", "/TGC/NSW/VMM/SIDEC"   , "Key of input sTGC condition data for side C data VMM"};
 
};


#endif
