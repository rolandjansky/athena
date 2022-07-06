/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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

	using writeKeyTdoPdo_t = SG::WriteCondHandleKey<NswCalibDbTimeChargeData>;
	using writeKeyThr_t =  SG::WriteCondHandleKey<NswCalibDbThresholdData >;
	using readKey_t = SG::ReadCondHandleKey<CondAttrListCollection>;
	
	using writeHandleTdoPdo_t =  SG::WriteCondHandle<NswCalibDbTimeChargeData>;
	using writeHandleThr_t = SG::WriteCondHandle<NswCalibDbThresholdData >;

	StatusCode processTdoPdoData(const EventContext& ctx) const;
	StatusCode processThrData   (const EventContext& ctx) const;

	using TimeChargeType = NswCalibDbTimeChargeData::CalibDataType;
	using TimeChargeTech = NswCalibDbTimeChargeData::CalibTechType;
	using ThresholdTech  = NswCalibDbThresholdData::ThrsldTechType;
	StatusCode loadTimeChargeData(const EventContext& ctx, const readKey_t& readKey, const TimeChargeTech,
	                              const TimeChargeType type, writeHandleTdoPdo_t& writeHandle, NswCalibDbTimeChargeData* writeCdo) const;
	StatusCode loadThresholdData (const EventContext&, const readKey_t&, const ThresholdTech, writeHandleThr_t   &, NswCalibDbThresholdData* ) const;

	bool buildChannelId(Identifier& channelId, unsigned int elinkId, unsigned int vmm, unsigned int channel) const;

	Gaudi::Property<bool> m_isData  {this, "isData"  , true , "Processing data"};
	
	ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};
	ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
	
	writeKeyTdoPdo_t m_writeKey_tdopdo{this, "WriteKey_TdoPdo", "NswCalibDbTimeChargeData", "Key of output calibration data (TDOs and PDOs)" };
	writeKeyThr_t    m_writeKey_thr   {this, "WriteKey_Thr"   , "NswCalibDbThresholdData" , "Key of output calibration data (VMM thresholds)"};
	
	readKey_t m_readKey_mm_sidea_tdo  {this, "ReadKey_MM_SIDEA_TDO"  , "/MDT/MM/TIME/SIDEA"   , "Key of input MM condition data for side A data TDO"};
	readKey_t m_readKey_mm_sidec_tdo  {this, "ReadKey_MM_SIDEC_TDO"  , "/MDT/MM/TIME/SIDEC"   , "Key of input MM condition data for side C data TDO"};
	readKey_t m_readKey_mm_sidea_pdo  {this, "ReadKey_MM_SIDEA_PDO"  , "/MDT/MM/CHARGE/SIDEA" , "Key of input MM condition data for side A data PDO"};
	readKey_t m_readKey_mm_sidec_pdo  {this, "ReadKey_MM_SIDEC_PDO"  , "/MDT/MM/CHARGE/SIDEC" , "Key of input MM condition data for side C data PDO"};
	readKey_t m_readKey_mm_sidea_thr  {this, "ReadKey_MM_SIDEA_THR"  , "/MDT/MM/THR/SIDEA"    , "Key of input MM condition data for side A data THR"};
	readKey_t m_readKey_mm_sidec_thr  {this, "ReadKey_MM_SIDEC_THR"  , "/MDT/MM/THR/SIDEC"    , "Key of input MM condition data for side C data THR"};
	readKey_t m_readKey_stgc_sidea_tdo{this, "ReadKey_STGC_SIDEA_TDO", "/TGC/NSW/TIME/SIDEA"  , "Key of input sTGC condition data for side A data TDO"};
	readKey_t m_readKey_stgc_sidec_tdo{this, "ReadKey_STGC_SIDEC_TDO", "/TGC/NSW/TIME/SIDEC"  , "Key of input sTGC condition data for side C data TDO"};
	readKey_t m_readKey_stgc_sidea_pdo{this, "ReadKey_STGC_SIDEA_PDO", "/TGC/NSW/CHARGE/SIDEA", "Key of input sTGC condition data for side A data PDO"};
	readKey_t m_readKey_stgc_sidec_pdo{this, "ReadKey_STGC_SIDEC_PDO", "/TGC/NSW/CHARGE/SIDEC", "Key of input sTGC condition data for side C data PDO"};
	readKey_t m_readKey_stgc_sidea_thr{this, "ReadKey_STGC_SIDEA_THR", "/TGC/NSW/THR/SIDEA"   , "Key of input sTGC condition data for side A data THR"};
	readKey_t m_readKey_stgc_sidec_thr{this, "ReadKey_STGC_SIDEC_THR", "/TGC/NSW/THR/SIDEC"   , "Key of input sTGC condition data for side C data THR"};
 
};


#endif
