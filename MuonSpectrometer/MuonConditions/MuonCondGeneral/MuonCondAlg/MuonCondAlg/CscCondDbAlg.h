/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "GaudiKernel/ServiceHandle.h"

//Athena includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonCondData/CscCondDbData.h"
#include "MuonCondSvc/MdtStringUtils.h"

class CscCondDbAlg: public AthReentrantAlgorithm{

public:

    CscCondDbAlg( const std::string & name, ISvcLocator* svc);
    virtual ~CscCondDbAlg() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode execute   (const EventContext &) const override;

 
private:

    typedef SG::WriteCondHandle<CscCondDbData> writeHandle_t;

    StatusCode loadDataHv     (writeHandle_t &, CscCondDbData*, const EventContext&) const;

    StatusCode loadData       (CscCondDbData*, const CondAttrListCollection*, const std::string, bool = false) const;

    StatusCode loadDataF001   (writeHandle_t &, CscCondDbData*, const EventContext&) const;
    StatusCode loadDataNoise  (writeHandle_t &, CscCondDbData*, const EventContext&) const;
    StatusCode loadDataPed    (writeHandle_t &, CscCondDbData*, const EventContext&) const;
    StatusCode loadDataPSlope (writeHandle_t &, CscCondDbData*, const EventContext&) const;
    StatusCode loadDataRMS    (writeHandle_t &, CscCondDbData*, const EventContext&) const;
    StatusCode loadDataStatus (writeHandle_t &, CscCondDbData*, const EventContext&) const;
    StatusCode loadDataT0Base (writeHandle_t &, CscCondDbData*, const EventContext&) const;
    StatusCode loadDataT0Phase(writeHandle_t &, CscCondDbData*, const EventContext&) const;

    StatusCode cache   (std::string  , CscCondDbData*, const std::string) const;
    StatusCode cacheASM(std::string  , CscCondDbData*, const std::string) const;
    StatusCode getAsmScope(int, int&, int&, int&, int&, int&) const;

    StatusCode recordParameter(unsigned int  , std::string, CscCondDbData*, const std::string) const;
    StatusCode recordParameter(IdentifierHash, std::string, CscCondDbData*, const std::string) const;

	StatusCode recordParameterF001   (IdentifierHash, std::string, CscCondDbData*) const;
	StatusCode recordParameterNoise  (IdentifierHash, std::string, CscCondDbData*) const;
	StatusCode recordParameterPed    (IdentifierHash, std::string, CscCondDbData*) const;
	StatusCode recordParameterPSlope (IdentifierHash, std::string, CscCondDbData*) const;
	StatusCode recordParameterRMS    (IdentifierHash, std::string, CscCondDbData*) const;
	StatusCode recordParameterStatus (IdentifierHash, std::string, CscCondDbData*) const;
	StatusCode recordParameterT0Base (IdentifierHash, std::string, CscCondDbData*) const;
	StatusCode recordParameterT0Phase(IdentifierHash, std::string, CscCondDbData*) const;

    Gaudi::Property<bool> m_isOnline { this, "isOnline", false, "" };
    Gaudi::Property<bool> m_isData { this, "isData", false, "" };
    Gaudi::Property<bool> m_isRun1 { this, "isRun1", false, "" };
    Gaudi::Property<bool> m_onlineOfflinePhiFlip { this, "onlineOfflinePhiFlip", true, "" };
    Gaudi::Property<std::string> m_defaultDatabaseReadVersion { this, "defaultDatabaseReadVersion", "02-00", "" };

    ServiceHandle<ICondSvc> m_condSvc;
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
 
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

    /**
     * The pslope is the gain of each CSC channel. It was intended to be retrieved by calibration with the pulser runs, 
     * but the pulses caused overload in the amplifiers because every channel fires at once. This leads to errors that are 
     * larger than the variation between channels. Consequently, the pslope is the same for all channels. 
     * In the future, one could try to calibrate from data. The support for the pslope in the database is maintained by having
     * a boolean property ReadPSlopeFromDatabase. If it is set to false, the value of the property PSlope is used for all channels.
     */
    Gaudi::Property<bool> m_pslopeFromDB { this, "ReadPSlopeFromDatabase", false, "read the pslope parameter from the conditions database (if it is not the same value for all channels)" };
    /**
     * The CSC gain was originally 5.304 ADC counts per fC, but later increased to
     * 5.7 ADC counts per fC, so the pslope equals 1/5.7 = 0.175438
     */
    const float m_DEFAULT_PSLOPE = 0.175;
    Gaudi::Property<float> m_pslope { this, "PSlope", m_DEFAULT_PSLOPE, "in case someone comes up with a new default value for pslope in the future" };

    // getParameter
    template <typename T>
    StatusCode getParameter(IdentifierHash chanHash, std::string data, T& token) const {
    
        // next element is the status bit
        std::istringstream iss(data);
        iss >> token;
        
        ATH_MSG_VERBOSE("Recorded token " << token << " for channelHash " << chanHash);
        return StatusCode::SUCCESS;
    }

};


#endif
