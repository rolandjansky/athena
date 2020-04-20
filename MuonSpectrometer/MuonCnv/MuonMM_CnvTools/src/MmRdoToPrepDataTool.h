/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMmRdoToPrepDataTool_H
#define MUONMmRdoToPrepDataTool_H

#include "MmRdoToPrepDataToolCore.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"

namespace Muon 
{
  class MmRdoToPrepDataTool : virtual public MmRdoToPrepDataToolCore
  {
  public:
    MmRdoToPrepDataTool(const std::string&,const std::string&,const IInterface*);
    
    /** default destructor */
    virtual ~MmRdoToPrepDataTool()=default;
    
    /** standard Athena-Algorithm method */
    virtual StatusCode initialize() override;
  
  protected:
    virtual SetupMM_PrepDataContainerStatus setupMM_PrepDataContainer() override;
  }; 
} // end of namespace

#endif 
