/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CNVTOOLS_STGCRDOTOPREPDATATOOL
#define MUONTGC_CNVTOOLS_STGCRDOTOPREPDATATOOL

#include "sTgcRdoToPrepDataToolCore.h"
#include "CxxUtils/checker_macros.h"

namespace Muon 
{
  /** @class STGC_RawDataToPrepDataTool 
   *  This is the algorithm that convert STGC Raw data  To STGC PRD  as a tool.
   */  

  class ATLAS_NOT_THREAD_SAFE sTgcRdoToPrepDataTool : virtual public sTgcRdoToPrepDataToolCore
    {
    public:
      /** Constructor */
      sTgcRdoToPrepDataTool(const std::string& t, const std::string& n, const IInterface* p);
      
      /** Destructor */
      virtual ~sTgcRdoToPrepDataTool()=default;
      
      /** Standard AthAlgTool initialize method */
      virtual StatusCode initialize() override;

    protected:
      virtual Muon::sTgcPrepDataContainer* setupSTGC_PrepDataContainer() const override;

    private:
      mutable Muon::sTgcPrepDataContainer* m_stgcPrepDataContainer = nullptr;
   }; 
} // end of namespace

#endif // MUONTGC_CNVTOOLS_STGCRDOTOPREPDATATOOL_H
