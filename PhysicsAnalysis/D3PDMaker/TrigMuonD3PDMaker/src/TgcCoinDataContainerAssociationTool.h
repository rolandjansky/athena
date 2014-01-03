// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef TRIGMUOND3PDMAKER_TGCCOINDATACOLLECTIONASSOCIATIONTOOL_H
#define TRIGMUOND3PDMAKER_TGCCOINDATACOLLECTIONASSOCIATIONTOOL_H

// EDM include(s):
#include "MuonTrigCoinData/TgcCoinDataContainer.h"
#include "MuonTrigCoinData/TgcCoinData.h"

// D3PDMaker include(s):
#include "D3PDMakerUtils/MultiAssociationTool.h"

// STL include:
#include <vector>


namespace D3PD {

   /**
    *  @short Tool associating TgcCoinData objects to TgcCoinDataContainer objects
    *  
    *  
    *
    * @author Takayuki Kanno
    *
    * $Revision: 343690 $
    * $Date$
    */
   class TgcCoinDataContainerAssociationTool :
      public MultiAssociationTool< Muon::TgcCoinDataContainer, Muon::TgcCoinData > {

   private:
      typedef MultiAssociationTool< Muon::TgcCoinDataContainer, Muon::TgcCoinData > BASE;

   public:
      /// Regular AlgTool constructor
      TgcCoinDataContainerAssociationTool( const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent );

      /// Algorithm Initalization
      virtual StatusCode initialize();
      /// Initialize the tool with a new object
      virtual StatusCode reset( const Muon::TgcCoinDataContainer& tgccoll );
      /// Return the next associated object
      virtual const Muon::TgcCoinData* next();

   private:
      bool m_AssocSL;
      bool m_AssocHpt;

      Muon::TgcCoinData::CoinDataType m_AssocType;
      /* Note;
         if(m_AssocSL)       m_AssocType = Muon::TgcCoinData::TYPE_SL;
         else if(m_AssocHpt) m_AssocType = Muon::TgcCoinData::TYPE_HIPT;
         else                m_AssocType = Muon::TgcCoinData::TYPE_TRACKLET;
      */
      std::vector<const Muon::TgcCoinData*> m_container; //Container to store pointers to objects
      std::vector<const Muon::TgcCoinData*>::const_iterator m_elem;//Iterator pointing to current element

   }; // class TgcCoinDataContainerAssociationTool 

} // namespace D3PD

#endif // TRIGMUOND3PDMAKER_TGCCOINDATACONTAINERASSOCIATIONTOOL_H
