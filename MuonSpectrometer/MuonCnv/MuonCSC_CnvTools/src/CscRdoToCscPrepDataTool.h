/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCSC_CNVTOOLS_CSCRDOTOCSCPREPDATATOOL_H
#define MUONCSC_CNVTOOLS_CSCRDOTOCSCPREPDATATOOL_H 

#include "CxxUtils/checker_macros.h"

#include "CscRdoToCscPrepDataToolCore.h"

#include <string>

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
  class ATLAS_NOT_THREAD_SAFE CscRdoToCscPrepDataTool : public CscRdoToCscPrepDataToolCore {

  public:
    
    CscRdoToCscPrepDataTool(const std::string& type, const std::string& name,
                            const IInterface* parent);
    
    /** destructor 
     */ 
    virtual ~CscRdoToCscPrepDataTool()=default;
    
    virtual StatusCode initialize() override;
    
    using CscRdoToCscPrepDataToolCore::decode;
    
    virtual StatusCode decode(std::vector<IdentifierHash>& givenIdhs, std::vector<IdentifierHash>& decodedIdhs) override;
  };
}
#endif /// MUONCSC_CNVTOOL_CSCRDOTOCSCPREPDATA_H


