// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TgcCoinHierarchyGetterTool.h 351021 2011-03-10 22:19:39Z kanno $
#ifndef TRIGMUOND3PDMAKER_TGCCOINHIERARCHYGETTERTOOL_H
#define TRIGMUOND3PDMAKER_TGCCOINHIERARCHYGETTERTOOL_H

// Athena/Gaudi include(s):
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ServiceHandle.h"

// EDM include(s):

// D3PD include(s):
#include "D3PDMakerUtils/CollectionGetterToolImpl.h"

// STL include(s):
#include <vector>
#include <string>

// Forward declaration(s):

namespace Trigger{
  class TgcCoinHierarchy;
  class ITgcCoinHierarchySvc;
}
namespace D3PD {

   /**
    *  @short A tool getting TgcCoinHierarchy from TgcCoinHierarchySvc
    *
    * @author Takayuki Kanno <Takayuki.kanno@cern.ch>
    *
    * $Revision: 351021 $
    * $Date: 2011-03-10 23:19:39 +0100 (Thu, 10 Mar 2011) $
    */
   class TgcCoinHierarchyGetterTool : public CollectionGetterToolImpl,
                                      public IIncidentListener
   {

   public:
      // Regular AlgTool constructor
      TgcCoinHierarchyGetterTool( const std::string& type, const std::string& name,
                                  const IInterface* parent );

      // Regular AlgTool initializer
      virtual StatusCode initialize();

      // Function from IIncidentListener
      virtual void handle(const Incident& inc);

      // Functions from IObjGetterTool
      // Nothing will be returned since this tool retrieves info from TgcCoinHierarchySvc
      virtual const std::type_info& typeinfo() const {return typeid(void);}
      virtual const void* getUntyped( bool ) {return 0;}

      // Functions from ICollectionGetterTool
      virtual const std::type_info& elementTypeinfo() const;
      virtual StatusCode reset( bool allowMissing = false );
      virtual const void* nextUntyped();
      virtual size_t sizeHint( bool allowMissing = false );

   private:
      // Function to fill m_container;
      StatusCode resetInternalContainer( bool allowMissing = false );

      // Flag showing if the iterators are valid
      bool m_itr_valid; 
      // Vector containig elements, which are filtered according to m_getSL and m_getHpt
      std::vector< const Trigger::TgcCoinHierarchy* > m_container;
      // Iterator pointing to the current element
      std::vector< const Trigger::TgcCoinHierarchy* >::const_iterator m_it;
      // Iterator pointing at the end of the coll.
      std::vector< const Trigger::TgcCoinHierarchy* >::const_iterator m_itend;

      // Flags to determine which bunch crossing to be collected
      bool m_getPrior;
      bool m_getCurrent;
      bool m_getNext;

      // Incident Service
      ServiceHandle<IIncidentSvc> m_incidentSvc;

      // Flag determining if m_container must be renewed.
      // Will be switched when handle() called. 
      bool m_new_event;
      // TgcCoinHierarchy Service. Provides all info what this tool needs
      ServiceHandle<Trigger::ITgcCoinHierarchySvc> m_tgcCoinHierSvc;


   }; // class TgcCoinHierarchyGetterTool

} // namespace D3PD

#endif // TRIGMUOND3PDMAKER_TGCCOINHIERARCHYGETTERTOOL_H
