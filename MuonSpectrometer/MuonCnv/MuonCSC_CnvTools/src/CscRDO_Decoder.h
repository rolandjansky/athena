/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCSC_CNVTOOLS_CSCRDO_DECODER_H
#define MUONCSC_CNVTOOLS_CSCRDO_DECODER_H

#include <inttypes.h>

#include <mutex>
#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "CSCcabling/CSCcablingSvc.h"
#include "CscCalibTools/ICscCalibTool.h"
#include "CscRODReadOut.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCSC_CnvTools/ICSC_RDO_Decoder.h"
#include "MuonIdHelpers/CscIdHelper.h"

class Identifier;
class CscRawData;

/** This class provides conversion from CSC RDO data to CSC Digits
 * @author Ketevi A. Assamagan
 * BNL January 24 2004
 */
namespace Muon {

    class CscRDO_Decoder : public extends<AthAlgTool, ICSC_RDO_Decoder> {
    public:
        /** constructor
         */
        CscRDO_Decoder(const std::string& type, const std::string& name, const IInterface* parent);

        /** destructor
         */
        virtual ~CscRDO_Decoder() = default;

        virtual StatusCode initialize() override final;

        virtual void getDigit(const CscRawData* rawData, const CscIdHelper* cscIdHelper, Identifier& moduleId, Identifier& channelId,
                              double& adc, double& time) const override final;
        virtual Identifier stationIdentifier(const CscRawData* rawData, const CscIdHelper* cscIdHelper) const override final;
        virtual Identifier channelIdentifier(const CscRawData* rawData, const CscIdHelper* cscIdHelper, int j) const override final;

    private:
        std::string m_detdescr;
        ServiceHandle<CSCcablingSvc> m_cabling{this, "CSCcablingSvc", "CSCcablingSvc", "CSC cabling service handle"};
        ToolHandle<ICscCalibTool> m_cscCalibTool{this, "cscCalibTool", "CscCalibTool", "CSC calibration tool handle"};
        double m_timeOffset = 0.0;
        double m_samplingTime = 0.0;
        double m_signalWidth = 0.0;
    };
}  // namespace Muon

#endif  // MUONCSC_CNVTOOL_CSCRDO_DECODER_H
