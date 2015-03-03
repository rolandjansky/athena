// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef TRIGMUOND3PDMAKER_TGCCOINDATACOLLECTIONASSOCIATIONTOOL_H
#define TRIGMUOND3PDMAKER_TGCCOINDATACOLLECTIONASSOCIATIONTOOL_H

// EDM include(s):
#include "MuonTrigCoinData/TgcCoinDataCollection.h"
#include "MuonTrigCoinData/TgcCoinData.h"

// D3PDMaker include(s):
#include "D3PDMakerUtils/MultiAssociationTool.h"

namespace D3PD {

   /**
    *  @short Tool associating TgcCoinData objects to TgcCoinDataCollection objects
    *  
    *  
    *
    * @author Takayuki Kanno
    *
    * $Revision: 332515 $
    * $Date$
    */
   class TgcCoinDataCollectionAssociationTool :
      public MultiAssociationTool< Muon::TgcCoinDataCollection, Muon::TgcCoinData > {

   public:
      /// Regular AlgTool constructor
      TgcCoinDataCollectionAssociationTool( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent );

      /// Initialize the tool with a new object
      virtual StatusCode reset( const Muon::TgcCoinDataCollection& tgccoll );
      /// Return the next associated object
      virtual const Muon::TgcCoinData* next();

   private:
      const Muon::TgcCoinDataCollection* m_coll; ///< Pointer to the collection
      Muon::TgcCoinDataCollection::const_iterator m_currentElem; ///< Pointer to the current element

   }; // class TgcCoinDataCollectionAssociationTool 

} // namespace D3PD

#endif // TRIGMUOND3PDMAKER_TGCCOINDATACOLLECTIONASSOCIATIONTOOL_H
