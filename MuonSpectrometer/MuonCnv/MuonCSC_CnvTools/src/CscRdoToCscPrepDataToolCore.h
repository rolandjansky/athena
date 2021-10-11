/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCSC_CNVTOOLS_CSCRDOTOCSCPREPDATATOOLCORE_H
#define MUONCSC_CNVTOOLS_CSCRDOTOCSCPREPDATATOOLCORE_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "CSCcabling/CSCcablingSvc.h"
#include "CscCalibTools/ICscCalibTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCSC_CnvTools/ICSC_RDO_Decoder.h"
#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonPrepRawData/CscStripPrepDataContainer.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "StoreGate/ReadCondHandleKey.h"

class CscRawDataContainer;

////////////////////////////////////////////////////////////////////////////////////////
/// Author: Ketevi A. Assamagan
/// BNL, April 03, 2005
///
/// algorithm to decode RDO into CscStripPrepData
/// get the RDO container from Storegate
/// loop over the RDO
/// Decode RDO into PrepRawData
/// loop over the PrepRawData and build the PrepRawData container
/// store the PrepRawData container in StoreGate
////////////////////////////////////////////////////////////////////////////////////////

namespace Muon {

    /// This class is only used in a single-thread mode as CscRdoToCscPrepDataToolMT has the
    /// equivalent functions defined for a thread-safe setup
    class CscRdoToCscPrepDataToolCore : public extends<AthAlgTool, IMuonRdoToPrepDataTool> {
    public:
        CscRdoToCscPrepDataToolCore(const std::string& type, const std::string& name, const IInterface* parent);

        virtual ~CscRdoToCscPrepDataToolCore() = default;

        virtual StatusCode initialize() override;
        // debugging
        virtual void printInputRdo() const override;

    protected:
        void printPrepDataImpl(const Muon::CscStripPrepDataContainer* outputCollection) const;

        SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_muDetMgrKey{this, "DetectorManagerKey", "MuonDetectorManager",
                                                                         "Key of input MuonDetectorManager condition data"};

        ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

        /// CscStripPrepRawData containers
        SG::WriteHandleKey<Muon::CscStripPrepDataContainer> m_outputCollectionKey;

        SG::ReadHandleKey<CscRawDataContainer> m_rdoContainerKey;

        /// CSC Calibration tools
        ToolHandle<ICscCalibTool> m_cscCalibTool{this, "CscCalibTool", "CscCalibTool/CscCalibTool"};
        ToolHandle<ICSC_RDO_Decoder> m_cscRdoDecoderTool{this, "CscRdoDecoderTool", "Muon::CscRDO_Decoder/CscRDO_Decoder"};

        ServiceHandle<CSCcablingSvc> m_cabling;
        /// Identifier hash offset
        int m_cscOffset;

        bool m_decodeData;  //!< toggle on/off the decoding of CSC RDO into CscStripPrepData
    };
}  // namespace Muon
#endif  /// MUONCSC_CNVTOOL_CSCRDOTOCSCPREPDATA_H
