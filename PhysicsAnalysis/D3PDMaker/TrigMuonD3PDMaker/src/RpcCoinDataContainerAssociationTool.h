// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef TRIGMUOND3PDMAKER_RPCCOINDATACOLLECTIONASSOCIATIONTOOL_H
#define TRIGMUOND3PDMAKER_RPCCOINDATACOLLECTIONASSOCIATIONTOOL_H

// EDM include(s):
#include "MuonTrigCoinData/RpcCoinDataContainer.h"
#include "MuonTrigCoinData/RpcCoinData.h"

// D3PDMaker include(s):
#include "D3PDMakerUtils/MultiAssociationTool.h"

// STL include:
#include <vector>


namespace D3PD {

   /**
    *  @short Tool associating RpcCoinData objects to RpcCoinDataContainer objects
    *  
    *  
    *
    * @author Takayuki Kanno
    *
    * $Revision: 343688 $
    * $Date$
    */
   class RpcCoinDataContainerAssociationTool :
      public MultiAssociationTool< Muon::RpcCoinDataContainer, Muon::RpcCoinData > {

   private:
      typedef MultiAssociationTool< Muon::RpcCoinDataContainer, Muon::RpcCoinData > BASE;

   public:
      /// Regular AlgTool constructor
      RpcCoinDataContainerAssociationTool( const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent );

      /// Initialize the tool with a new object
      virtual StatusCode reset( const Muon::RpcCoinDataContainer& rpccoll );
      /// Return the next associated object
      virtual const Muon::RpcCoinData* next();

   private:
      std::vector<const Muon::RpcCoinData*> m_container; //Container to store pointers to objects
      std::vector<const Muon::RpcCoinData*>::const_iterator m_elem;//Iterator pointing to current element

   }; // class RpcCoinDataContainerAssociationTool 

} // namespace D3PD

#endif // TRIGMUOND3PDMAKER_RPCCOINDATACONTAINERASSOCIATIONTOOL_H
