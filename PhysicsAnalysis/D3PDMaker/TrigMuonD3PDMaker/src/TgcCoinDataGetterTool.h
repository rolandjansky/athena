// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TgcCoinDataGetterTool.h 351020 2011-03-10 22:18:44Z kanno $
#ifndef TRIGMUOND3PDMAKER_TGCCOINDATAGETTERTOOL_H
#define TRIGMUOND3PDMAKER_TGCCOINDATAGETTERTOOL_H

// Gaudi include(s):
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ServiceHandle.h"

// STL include(s):
#include <vector>
#include <string>

// EDM include(s):
#include "MuonTrigCoinData/TgcCoinData.h"
#include "MuonTrigCoinData/TgcCoinDataContainer.h"

// D3PD include(s):
#include "D3PDMakerUtils/CollectionGetterToolImpl.h"

// Forward declaration(s):

namespace D3PD {

   /**
    *  @short A tool getting TgcCoinData from Store Gate.
    *
    *         Three types of TGC coincidence (SL, Hpt, Tracklet) have been
    *         stored in one container. Since these are quite different
    *         with each other, these will be stored in different branches
    *         of D3PD ntuple. This Getter tool enables it by switching
    *         GetSL & GetHpt properties.
    *
    * @author Takayuki Kanno <Takayuki.kanno@cern.ch>
    *
    * $Revision: 351020 $
    * $Date: 2011-03-10 23:18:44 +0100 (Thu, 10 Mar 2011) $
    */
   class TgcCoinDataGetterTool : public CollectionGetterToolImpl,
                                 public IIncidentListener
   {

   public:
      /// Regular AlgTool constructor
      TgcCoinDataGetterTool( const std::string& type, const std::string& name,
                             const IInterface* parent );

      /// Regular AlgTool initializer
      virtual StatusCode initialize();

      /// Function from IIncidentListener
      virtual void handle(const Incident& inc);

      /// Get TgcCoinDataContainer from StoreGate
      const  Muon::TgcCoinDataContainer* get( bool allowMissing = false );

      /// Functions from IObjGetterTool
      virtual const std::type_info& typeinfo() const;
      virtual const void* getUntyped( bool allowMissing = false );

      /// Functions from ICollectionGetterTool
      virtual const std::type_info& elementTypeinfo() const;
      virtual StatusCode reset( bool allowMissing = false );
      virtual const void* nextUntyped();
      virtual size_t sizeHint( bool allowMissing = false );

   private:
      // Function to fill m_container;
      StatusCode resetInternalContainer( bool allowMissing = false );

      ///< SG key for TgcCoinDataContainer
      std::string m_key; 

      /*
       * Flags to set target
       * |    Property    | Target to  |
       * | GetSL | GetHpt | be stored  |
       * +-------+--------+------------+
       * | true  | any    |  SL        |
       * | false | true   |  Hpt       |
       * | false | false  |  Tracklet  |
       */
      bool m_getSL;
      bool m_getHpt;
      Muon::TgcCoinData::CoinDataType m_TargetType;

      // Flag showing if the iterators are valid
      bool m_itr_valid; 
      // Vector containig elements, which are filtered according to m_getSL and m_getHpt
      std::vector< const Muon::TgcCoinData* > m_container;
      // Iterator pointing to the current element
      std::vector< const Muon::TgcCoinData* >::const_iterator m_it;
      // Iterator pointing at the end of the coll.
      std::vector< const Muon::TgcCoinData* >::const_iterator m_itend;

      // Incident Service
      ServiceHandle<IIncidentSvc> m_incidentSvc;
      // Flag determining if m_container must be renewed.
      // Will be switched when handle() called. 
      bool m_new_event;

   }; // class TgcCoinDataGetterTool

} // namespace D3PD

#endif // TRIGMUOND3PDMAKER_TGCCOINDATAGETTERTOOL_H
