/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CNVTOOLS_STGCRDOTOPREPDATATOOLMT
#define MUONTGC_CNVTOOLS_STGCRDOTOPREPDATATOOLMT

#include "sTgcRdoToPrepDataToolCore.h"

namespace Muon 
{
  /** @class STGC_RawDataToPrepDataTool 
   *  This is the algorithm that convert STGC Raw data  To STGC PRD  as a tool.
   */  

  class sTgcRdoToPrepDataToolMT : public extends<sTgcRdoToPrepDataToolCore, IMuonRdoToPrepDataTool>
    {
    public:
      /** Constructor */
      sTgcRdoToPrepDataToolMT(const std::string& t, const std::string& n, const IInterface* p);
      
      /** Destructor */
      virtual ~sTgcRdoToPrepDataToolMT()=default;
      
      /** Standard AthAlgTool initialize method */
      virtual StatusCode initialize() override;

    protected:
      virtual Muon::sTgcPrepDataContainer* setupSTGC_PrepDataContainer() const override;
   }; 
} // end of namespace

#endif // MUONTGC_CNVTOOLS_STGCRDOTOPREPDATATOOL_H