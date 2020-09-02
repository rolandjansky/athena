/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCSC_CNVTOOLS_CSCRDO_DECODER_H
#define MUONCSC_CNVTOOLS_CSCRDO_DECODER_H

#include "MuonCSC_CnvTools/ICSC_RDO_Decoder.h"
#include "CscCalibTools/ICscCalibTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "CSCcabling/CSCcablingSvc.h"

#include "GaudiKernel/ServiceHandle.h"
#include "CSCcabling/CSCcablingSvc.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include <inttypes.h>
#include <vector>
#include <string>
#include <mutex>

#include "CscRODReadOut.h"

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
    CscRDO_Decoder(const std::string& type, const std::string& name,
                   const IInterface* parent ) ;

    /** destructor
     */
    virtual ~CscRDO_Decoder() = default;

    virtual StatusCode initialize() override final;

    virtual void getDigit(const CscRawData * rawData, Identifier& moduleId,
                  Identifier& channelId, double& adc, double& time) const override final;
    virtual Identifier stationIdentifier(const CscRawData* rawData) const override final;
    virtual Identifier channelIdentifier(const CscRawData * rawData, int j) const override final;

  private:
    std::string m_detdescr;
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    ServiceHandle<CSCcablingSvc>       m_cabling{this, "CSCcablingSvc", "CSCcablingSvc", "CSC cabling service handle"};
    ToolHandle<ICscCalibTool>          m_cscCalibTool{this, "cscCalibTool", "CscCalibTool", "CSC calibration tool handle"};
    double   m_timeOffset   ;
    double   m_samplingTime ;
    double   m_signalWidth  ;
    // the read out structure
    mutable CscRODReadOut m_rodReadOut ATLAS_THREAD_SAFE; // guarded by m_mutex
    mutable std::mutex m_mutex;
  };
}


#endif  // MUONCSC_CNVTOOL_CSCRDO_DECODER_H
