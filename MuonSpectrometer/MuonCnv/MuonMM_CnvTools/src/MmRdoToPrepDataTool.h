/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMmRdoToPrepDataTool_H
#define MUONMmRdoToPrepDataTool_H

#include "MmRdoToPrepDataToolCore.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "CxxUtils/checker_macros.h"

namespace Muon 
{
  class ATLAS_NOT_THREAD_SAFE MmRdoToPrepDataTool : virtual public MmRdoToPrepDataToolCore
  {
  public:
    MmRdoToPrepDataTool(const std::string&,const std::string&,const IInterface*);
    
    /** default destructor */
    virtual ~MmRdoToPrepDataTool()=default;
    
    /** standard Athena-Algorithm method */
    virtual StatusCode initialize() override;
  
  protected:
    virtual Muon::MMPrepDataContainer* setupMM_PrepDataContainer() const override;

  private:
    mutable Muon::MMPrepDataContainer* m_mmPrepDataContainer = nullptr;
  }; 
} // end of namespace

#endif 
