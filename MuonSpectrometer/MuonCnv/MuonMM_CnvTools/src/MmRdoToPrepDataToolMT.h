/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMmRdoToPrepDataToolMT_H
#define MUONMmRdoToPrepDataToolMT_H

#include "MmRdoToPrepDataToolCore.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"

namespace Muon 
{
  class MmRdoToPrepDataToolMT : virtual public MmRdoToPrepDataToolCore
  {
  public:
    MmRdoToPrepDataToolMT(const std::string&,const std::string&,const IInterface*);
    
    /** default destructor */
    virtual ~MmRdoToPrepDataToolMT ()=default;
    
    /** standard Athena-Algorithm method */
    virtual StatusCode initialize() override;
  
  protected:
    virtual SetupMM_PrepDataContainerStatus setupMM_PrepDataContainer() override;
  }; 
} // end of namespace

#endif 
