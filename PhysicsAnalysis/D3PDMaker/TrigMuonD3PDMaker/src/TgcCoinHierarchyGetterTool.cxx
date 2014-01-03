/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TgcCoinHierarchyGetterTool.cxx 452219 2011-08-04 15:08:00Z ssnyder $

// Athena/Gaudi include(s):
#include "AthenaKernel/errorcheck.h"
#include "TrigMuonCoinHierarchy/TgcCoinHierarchy.h"
#include "TrigMuonCoinHierarchy/ITgcCoinHierarchySvc.h"

// EDM include(s):

// Local include(s):
#include "TgcCoinHierarchyGetterTool.h"

namespace D3PD {

   TgcCoinHierarchyGetterTool::TgcCoinHierarchyGetterTool( const std::string& type,
                                                           const std::string& name,
                                                           const IInterface* parent )
      : CollectionGetterToolImpl( type, name, parent )
      , m_itr_valid(false)
      , m_getPrior(false), m_getCurrent(false), m_getNext(false)
      , m_incidentSvc("IncidentSvc", name)
      , m_new_event(true)
      , m_tgcCoinHierSvc("Trigger::TgcCoinHierarchySvc",name)
   {
      // Declare the tool's properties:
      declareProperty( "GetPrior", m_getPrior );
      declareProperty( "GetCurrent", m_getCurrent );
      declareProperty( "GetNext", m_getNext );

   }

   StatusCode TgcCoinHierarchyGetterTool::initialize(){
      CHECK( CollectionGetterToolImpl::initialize() );
      CHECK( m_incidentSvc.retrieve() );
      m_incidentSvc->addListener(this, IncidentType::BeginEvent);
      CHECK( m_tgcCoinHierSvc.retrieve() );

      return StatusCode::SUCCESS;
   }

   void TgcCoinHierarchyGetterTool::handle(const Incident& inc) { 
      if(inc.type()==IncidentType::BeginEvent){
        m_new_event = true;
      }
   }

   const std::type_info& TgcCoinHierarchyGetterTool::elementTypeinfo() const {
      return typeid( Trigger::TgcCoinHierarchy );
   }

   StatusCode TgcCoinHierarchyGetterTool::reset( bool allowMissing ) {

      m_itr_valid = false;

      CHECK( resetInternalContainer(allowMissing) );

      m_it    = m_container.begin();
      m_itend = m_container.end();
      m_itr_valid = true;
      return StatusCode::SUCCESS;
   }

   const void* TgcCoinHierarchyGetterTool::nextUntyped() {

      if( ! m_itr_valid ) return 0;
      if( m_it == m_itend ) return 0;

      return *(m_it++);

   }

   size_t TgcCoinHierarchyGetterTool::sizeHint( bool allowMissing ) {
      // Should consider these cases:
      // sizeHint => reset => nextUntyped
      //   call resetInternalContainer and return m_container.size()
      // reset => sizeHint => nextUntyped
      //   just return m_container.size()
      // reset => nextUntyped .. nextUntyped => sizeHint => nextUntyped
      //   be care. must keep m_it valid
      //
      // ...but,
      // function resetInternalContainer takes care m_new_event flags.
      // m_it validity will never be broken.
      if( resetInternalContainer( allowMissing ).isFailure() ){
         if( allowMissing ){
            return 0;
         }else{
            REPORT_ERROR( StatusCode::FAILURE )
              << "Couldn't get TgcCoinHierarchy vector from TgcCoinHierarchySvc Service";
            return 0;
         }
      }
      return m_container.size();
   }

   StatusCode TgcCoinHierarchyGetterTool::resetInternalContainer( bool allowMissing ){
      if( !m_new_event ){
         // Do not need to fill the container again
         return StatusCode::SUCCESS;
      }
      m_new_event = false;


      m_container.clear();
      m_itr_valid = false;

      std::vector< const Trigger::TgcCoinHierarchy* > tmp_container;
      StatusCode sc = m_tgcCoinHierSvc->getHierarchy( &tmp_container );
      if( sc.isFailure() ){
         if( allowMissing ){
            return StatusCode::SUCCESS;
         }else{
            REPORT_ERROR( sc )
              << "Couldn't get TgcCoinHierarchy vector from TgcCoinHierarchySvc Service";
            return StatusCode::FAILURE;
         }
      }

      std::vector< const Trigger::TgcCoinHierarchy* >::iterator tmp_it    = tmp_container.begin();
      std::vector< const Trigger::TgcCoinHierarchy* >::iterator tmp_itend = tmp_container.end();
      for( ; tmp_it!=tmp_itend; ++tmp_it ){
         Trigger::TgcCoinHierarchy::TIMING bctime = (*tmp_it)->getTimingCoin();
         if(    ( m_getPrior   && Trigger::TgcCoinHierarchy::PRIOR == bctime )
             || ( m_getCurrent && Trigger::TgcCoinHierarchy::CURR  == bctime )
             || ( m_getNext    && Trigger::TgcCoinHierarchy::NEXT  == bctime ) )
         {
            m_container.push_back(*tmp_it);
         }
      }

      return StatusCode::SUCCESS;
   }


} // namespace D3PD
