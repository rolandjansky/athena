/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TgcRdoToPrepDataToolMT.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef MUONTGC_CNVTOOLS_TGCRDOTOPREPDATATOOLMT_H
#define MUONTGC_CNVTOOLS_TGCRDOTOPREPDATATOOLMT_H

#include "TgcRdoToPrepDataToolCore.h"


namespace Muon 
{
  /** @class TgcRdoToPrepDataToolMT 
   *  This is the algorithm that convert TGCRdo To TGCPrepdata as a tool.
   * 
   * @author Susumu Oda <Susumu.Oda@cern.ch> 
   * @author  Edward Moyse 
   * 
   * This class was developed by Takashi Kubota. 
   */  

  class TgcRdoToPrepDataToolMT : virtual public TgcRdoToPrepDataToolCore
  {
    public:
      /** Constructor */
      TgcRdoToPrepDataToolMT(const std::string& t, const std::string& n, const IInterface* p);
      
      /** Destructor */
      virtual ~TgcRdoToPrepDataToolMT()=default;
      
      /** Standard AthAlgTool initialize method */
      virtual StatusCode initialize() override;
    
      using TgcRdoToPrepDataToolCore::decode; // To prevent the decode below from hiding the superclass decode methods
      virtual StatusCode decode(std::vector<IdentifierHash>& idVect, std::vector<IdentifierHash>& idWithDataVect) override;
   }; 
} // end of namespace

#endif // MUONTGC_CNVTOOLS_TGCRDOTOPREPDATATOOLMT_H
