// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RpcPadContainerAssociationTool.h 439443 2011-05-26 01:35:49Z kanno $
#ifndef TRIGMUOND3PDMAKER_RPCPADCOLLECTIONASSOCIATIONTOOL_H
#define TRIGMUOND3PDMAKER_RPCPADCOLLECTIONASSOCIATIONTOOL_H

// EDM include(s):
#include "MuonRDO/RpcPadContainer.h"
#include "MuonRDO/RpcPad.h"

// D3PDMaker include(s):
#include "D3PDMakerUtils/MultiAssociationTool.h"

// STL include:
#include <vector>

struct RpcTree {
  const RpcPad* Padid;
  const RpcCoinMatrix* Cmaid;
  const RpcFiredChannel* Chid;
};


namespace D3PD {

   /**
    *  @short Tool associating RpcPad objects to RpcPadContainer objects
    *  
    *  
    *
    * @author Lorenzo Bellagamba
    *
    * $Revision: 439443 $
    * $Date: 2011-05-26 03:35:49 +0200 (Thu, 26 May 2011) $
    */
   class RpcPadContainerAssociationTool :
      public MultiAssociationTool< RpcPadContainer, RpcTree > {

   private:
      typedef MultiAssociationTool< RpcPadContainer, RpcTree > BASE;

   public:
      /// Regular AlgTool constructor
      RpcPadContainerAssociationTool( const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent );

      /// Initialize the tool with a new object
      virtual StatusCode reset( const RpcPadContainer& rpccoll );
      /// Return the next associated object
      virtual const RpcTree* next();

   private:
      std::vector<const RpcTree*> m_container; //Container to store pointers to objects
      std::vector<const RpcTree*>::const_iterator m_elem;//Iterator pointing to current element

   }; // class RpcPadContainerAssociationTool 

} // namespace D3PD

#endif // TRIGMUOND3PDMAKER_RPCPADCONTAINERASSOCIATIONTOOL_H
