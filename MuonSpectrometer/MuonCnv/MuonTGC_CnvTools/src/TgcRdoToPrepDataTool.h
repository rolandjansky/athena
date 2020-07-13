/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CNVTOOLS_TGCRDOTOPREPDATATOOL_H
#define MUONTGC_CNVTOOLS_TGCRDOTOPREPDATATOOL_H

/// This code is only used in the single-thread setup   
/// As such, deactivating the check in this file        
#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

#include "TgcRdoToPrepDataToolCore.h"

namespace Muon 
{
  /** @class TgcRdoToPrepDataTool 
   *  This is the algorithm that convert TGCRdo To TGCPrepdata as a tool.
   * 
   * @author Susumu Oda <Susumu.Oda@cern.ch> 
   * @author  Edward Moyse 
   * 
   * This class was developed by Takashi Kubota. 
   */  

  class TgcRdoToPrepDataTool : virtual public TgcRdoToPrepDataToolCore
  {
    public:
      /** Constructor */
      TgcRdoToPrepDataTool(const std::string& t, const std::string& n, const IInterface* p);
      
      /** Destructor */
      virtual ~TgcRdoToPrepDataTool()=default;
      
      /** Standard AthAlgTool initialize method */
      virtual StatusCode initialize() override;
    
      using TgcRdoToPrepDataToolCore::decode; // To prevent the decode below from hiding the superclass decode methods
      virtual StatusCode decode(std::vector<IdentifierHash>& idVect, std::vector<IdentifierHash>& idWithDataVect) override;
   }; 
} // end of namespace

#endif // MUONTGC_CNVTOOLS_TGCRDOTOPREPDATATOOL_H
