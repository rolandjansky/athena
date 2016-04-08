/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCSC_CNVTOOLS_CSCRDO_DECODER_H
#define MUONCSC_CNVTOOLS_CSCRDO_DECODER_H

#include "MuonCSC_CnvTools/ICSC_RDO_Decoder.h"        
#include "CscCalibTools/ICscCalibTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "CSCcabling/CSCcablingSvc.h"

#include "GaudiKernel/ServiceHandle.h"
#include "CSCcabling/CSCcablingSvc.h"

#include <inttypes.h>
#include <vector>
#include <string>
#include "CscRODReadOut.h"

class CscIdHelper;
class Identifier;
class CscRawData;

/** This class provides conversion from CSC RDO data to CSC Digits
   * @author Ketevi A. Assamagan
   * BNL January 24 2004
   */
namespace Muon {
  
  class CscRDO_Decoder : virtual public ICSC_RDO_Decoder, public AthAlgTool {
    
  public:
    
    /** constructor 
     */
    CscRDO_Decoder(const std::string& type, const std::string& name,
                   const IInterface* parent ) ;
    
    /** destructor 
     */
    ~CscRDO_Decoder(); 
    
    virtual StatusCode initialize();
    virtual StatusCode finalize() { return StatusCode::SUCCESS; }

    void getDigit(const CscRawData * rawData, Identifier& moduleId, 
                  Identifier& channelId, double& adc, double& time) const;
    Identifier stationIdentifier(const CscRawData* rawData) const;
    Identifier channelIdentifier(const CscRawData * rawData, int j) const;

  private:
    std::string m_detdescr;
    const CscIdHelper *               m_cscHelper;
    ServiceHandle<CSCcablingSvc>      m_cabling;
    ToolHandle<ICscCalibTool>         m_cscCalibTool;
    double   m_timeOffset   ;
    double   m_samplingTime ;
    double   m_signalWidth  ;
    // the read out structure
    mutable CscRODReadOut m_rodReadOut;

  };
}


#endif  // MUONCSC_CNVTOOL_CSCRDO_DECODER_H



