/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMmRdoToPrepDataToolMT_H
#define MUONMmRdoToPrepDataToolMT_H

#include "MmRdoToPrepDataToolCore.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/MuonPrepDataCollection_Cache.h"

namespace Muon 
{
  class MmRdoToPrepDataToolMT : public extends<MmRdoToPrepDataToolCore, IMuonRdoToPrepDataTool>
  {
  public:
    MmRdoToPrepDataToolMT(const std::string&,const std::string&,const IInterface*);
    
    /** default destructor */
    virtual ~MmRdoToPrepDataToolMT ()=default;
    
    /** standard Athena-Algorithm method */
    virtual StatusCode initialize() override;
  
  protected:
    virtual Muon::MMPrepDataContainer* setupMM_PrepDataContainer() const override;

  private:
    /// This is the key for the cache for the MM PRD containers, can be empty
    SG::UpdateHandleKey<MMPrepDataCollection_Cache> m_prdContainerCacheKey;
  }; 
} // end of namespace

#endif 
