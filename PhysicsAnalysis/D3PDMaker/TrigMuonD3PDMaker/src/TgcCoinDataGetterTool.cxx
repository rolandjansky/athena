/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TgcCoinDataGetterTool.cxx 452219 2011-08-04 15:08:00Z ssnyder $

// Athena/Gaudi include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "MuonTrigCoinData/TgcCoinData.h"
#include "MuonTrigCoinData/TgcCoinDataCollection.h"
#include "MuonTrigCoinData/TgcCoinDataContainer.h"

// Local include(s):
#include "TgcCoinDataGetterTool.h"

namespace D3PD {

   TgcCoinDataGetterTool::TgcCoinDataGetterTool( const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent )
     : CollectionGetterToolImpl( type, name, parent )
      , m_TargetType(Muon::TgcCoinData::TYPE_SL)
      , m_itr_valid(false)
      , m_incidentSvc("IncidentSvc", name)
      , m_new_event(true)

   {
      // Declare the tool's properties:
      declareProperty( "SGKey",  m_key = "TrigT1CoinDataCollection" );
      declareProperty( "GetSL",  m_getSL  = true );
      declareProperty( "GetHpt", m_getHpt = true );
   }

   StatusCode TgcCoinDataGetterTool::initialize(){
      CHECK( CollectionGetterToolImpl::initialize() );
      CHECK(  m_incidentSvc.retrieve().isSuccess() );
      // BeginEvent incident is registered 
      m_incidentSvc->addListener(this, IncidentType::BeginEvent);

      /*
       * Flags to set target
       * |    Property    | Target to  |
       * | GetSL | GetHpt | be stored  |
       * +-------+--------+------------+
       * | true  | any    |  SL        |
       * | false | true   |  Hpt       |
       * | false | false  |  Tracklet  |
       */
      if(m_getSL)       m_TargetType = Muon::TgcCoinData::TYPE_SL;
      else if(m_getHpt) m_TargetType = Muon::TgcCoinData::TYPE_HIPT;
      else              m_TargetType = Muon::TgcCoinData::TYPE_TRACKLET;

      return StatusCode::SUCCESS;
   }

   void TgcCoinDataGetterTool::handle(const Incident& inc) { 
      if(inc.type()==IncidentType::BeginEvent)
         m_new_event = true;
   }

   const Muon::TgcCoinDataContainer* TgcCoinDataGetterTool::get( bool allowMissing ) {

      if( allowMissing && ( ! evtStore()->contains< Muon::TgcCoinDataContainer >( m_key ) ) ) {
         return 0;
      }

      // Retrieve TgcCoinDataContainer:
      const Muon::TgcCoinDataContainer* tgccont = 0;
      if( evtStore()->retrieve( tgccont, m_key ).isFailure() ) {
         REPORT_ERROR( StatusCode::FAILURE )
            << "Muon::TgcCoinDataContainer could not be found with key: "
            << m_key;
         return 0;
      }

      return tgccont;
   }

   const std::type_info& TgcCoinDataGetterTool::typeinfo() const {
      return typeid( Muon::TgcCoinDataContainer );
   }

   const void* TgcCoinDataGetterTool::getUntyped( bool allowMissing ) {
      return get( allowMissing );
   }

   const std::type_info& TgcCoinDataGetterTool::elementTypeinfo() const {
      return typeid( Muon::TgcCoinData );
   }

   StatusCode TgcCoinDataGetterTool::reset( bool allowMissing ) {

      m_itr_valid = false;

      CHECK( resetInternalContainer(allowMissing) );

      m_it    = m_container.begin();
      m_itend = m_container.end();
      m_itr_valid = true;
      return StatusCode::SUCCESS;
   }

   const void* TgcCoinDataGetterTool::nextUntyped() {

      if( ! m_itr_valid ) return 0;
      if( m_it == m_itend ) return 0;

      return *(m_it++);

   }

   size_t TgcCoinDataGetterTool::sizeHint( bool allowMissing ) {

      if( resetInternalContainer(allowMissing).isFailure() ){
         if( allowMissing ) {
            return 0;
         } else {
            REPORT_ERROR( StatusCode::FAILURE )
              << "Muon::TgcCoinDataContainer object not available";
            return 0;
         }
      }
      return m_container.size();
   }

   StatusCode TgcCoinDataGetterTool::resetInternalContainer( bool allowMissing ){
      if( !m_new_event ){
         // Do not filter the container again
         return StatusCode::SUCCESS;
      }
      m_new_event = false;

      m_container.clear();
      m_itr_valid = false;

      const Muon::TgcCoinDataContainer* tgccont = get( allowMissing );
      if( ! tgccont ) {
         if( allowMissing ) {
            return StatusCode::SUCCESS;
         } else {
            REPORT_ERROR( StatusCode::FAILURE )
              << "Muon::TgcCoinDataContainer object not available";
            return StatusCode::FAILURE;
         }
      }

      // Filter the container
      Muon::TgcCoinDataContainer::const_iterator itcont     = tgccont->begin();
      Muon::TgcCoinDataContainer::const_iterator itcont_end = tgccont->end();
      for(;itcont!=itcont_end;++itcont){
         Muon::TgcCoinDataCollection::const_iterator itcoll     = (*itcont)->begin();
         Muon::TgcCoinDataCollection::const_iterator itcoll_end = (*itcont)->end();
         for(;itcoll!=itcoll_end;++itcoll){
            if( (*itcoll)->type()==m_TargetType )
               m_container.push_back(*itcoll);
         }
      }

      return StatusCode::SUCCESS;
   }



} // namespace D3PD
