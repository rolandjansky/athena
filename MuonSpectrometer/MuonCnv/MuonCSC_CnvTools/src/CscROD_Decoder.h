/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCSC_CNVTOOL_CSCROD_DECODER_H
#define MUONCSC_CNVTOOL_CSCROD_DECODER_H

// MuonCSC_CnvTool includes  
#include "MuonCSC_CnvTools/ICSC_ROD_Decoder.h"        
#include "CscRODReadOut.h"
#include "CscRODReadOutV0.h"
#include "CscRODReadOutV1.h"
#include "CSC_Hid2RESrcID.h"

#include <inttypes.h>

#include "AthenaBaseComps/AthAlgTool.h"
#include "Identifier/Identifier.h"

#include "ByteStreamData/RawEvent.h" 
#include "eformat/SourceIdentifier.h"
#include "eformat/Version.h"

#include "CSCcabling/CSCcablingSvc.h"

class CscRawDataContainer;
class CscIdHelper;

//using namespace OFFLINE_FRAGMENTS_NAMESPACE ; 
//using eformat::helper::SourceIdentifier; 

namespace Muon {

/** This class provides conversion from ROD data to CSC RDO
   * Author: Ketevi A. Assamagan
   * BNL December 27 2003
   */

class CscROD_Decoder : public extends<AthAlgTool, ICSC_ROD_Decoder>  {

public: 
  
  /** constructor 
   */
  CscROD_Decoder(const std::string& type, const std::string& name,
		 const IInterface* parent ) ;

  /** destructor 
   */
  virtual ~CscROD_Decoder(); 

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override { return StatusCode::SUCCESS; }
  
  virtual void fillCollection(const xAOD::EventInfo& eventInfo,
                              const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag,  CscRawDataContainer& rdoIDC) const override;

  virtual Identifier getChannelId(const uint32_t word, std::string detdesription) const override;
  virtual uint32_t getHashId(const uint32_t word, std::string detdesription) const override;
  virtual void getSamples(const std::vector<uint32_t>& words, std::vector<uint16_t>& samples ) const override;

  // put this in Interface header file...
  
private:

  /** the ROD version */
  void rodVersion0(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag,  CscRawDataContainer& rdoIDC) const;

  void rodVersion1(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag,  CscRawDataContainer& rdoIDC) const;

  void rodVersion2(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag,  CscRawDataContainer& rdoIDC) const;

private:

  CSC_Hid2RESrcID                   m_hid2re;
  const CscIdHelper *               m_cscHelper;
  ServiceHandle<CSCcablingSvc>      m_cabling;

  bool                              m_isCosmic;
  bool                              m_isOldCosmic;

}; 

}

#endif






