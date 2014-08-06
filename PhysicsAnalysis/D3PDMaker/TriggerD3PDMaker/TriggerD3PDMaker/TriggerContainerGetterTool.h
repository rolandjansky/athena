// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerContainerGetterTool.h 278470 2010-02-10 15:05:51Z krasznaa $
#ifndef TRIGGERD3PDMAKER_TRIGGERCONTAINERGETTERTOOL_H
#define TRIGGERD3PDMAKER_TRIGGERCONTAINERGETTERTOOL_H

// STL include(s):
#include <vector>
#include <string>

// Gaudi/Athena include(s):
#include "GaudiKernel/ToolHandle.h"

// Trigger include(s):
#include "TrigDecisionTool/TrigDecisionTool.h"

// D3PDMaker include(s):
#include "D3PDMakerUtils/CollectionGetterToolImpl.h"

namespace D3PD {

   /**
    *  @short Base class for getters of HLT objects in containers
    *
    *         This class should be used as a base class for the getters that
    *         retrieve trigger objects using the TrigDecisionTool, which have been
    *         attached to the HLT chain (the appropriate TriggerElement actually)
    *         in containers. This is the case for the LVL2 electron reconstruction
    *         for instance.
    *
    *         The specialized getters don't need to implement anything, they can
    *         be as simple as the following:
    *
    *         <code>
    *         class TrigElectronTrigGetterTool :
    *            public TriggerContainerGetterTool< TrigElectronContainer, TrigElectron > {
    *
    *         public:
    *            TrigElectronTrigGetterTool( const std::string& type, const std::string& name,
    *                                        const IInterface* parent )
    *              : TriggerContainerGetterTool< TrigElectronContainer, TrigElectron >( type,
    *                                                                                   name,
    *                                                                                   parent ) {}
    *         };
    *         </code>
    *
    * @author Attila Krasznahorkay Jr.
    *
    * $Revision: 278470 $
    * $Date: 2010-02-10 16:05:51 +0100 (Wed, 10 Feb 2010) $
    */
   template< class Cont, class Elem >
   class TriggerContainerGetterTool : public CollectionGetterToolImpl {

   public:
      /// Standard Gaudi tool constructor
      TriggerContainerGetterTool( const std::string& type, const std::string& name,
                                  const IInterface* parent );

      /// Standard Gaudi initialize method
      virtual StatusCode initialize();

      /// Get the type info of the container
      virtual const std::type_info& typeinfo() const;
      /// Unused function inherited from IObjectGetterTool
      virtual const void* getUntyped( bool allowMissing = false );

      /// Get the type info of the object
      virtual const std::type_info& elementTypeinfo() const;
      /// Retrieve all the trigger objects from the specified chains
      virtual StatusCode reset( bool allowMissing = false );
      /// Return the next trigger object
      virtual const void* nextUntyped();
      /// Get the expected number of trigger objects
      virtual size_t sizeHint( bool allowMissing = false );

   private:
      /// Link to all the HLT features selected
      std::vector< const Elem* >                          m_features;
      /// Iterator used to loop over the selected HLT features
      typename std::vector< const Elem* >::const_iterator m_itr;
      /// Flag showing if the iterator is valid at the moment
      bool                                                m_itrValid;

      /// Internal flag for printing some message in the first event
      bool m_firstEvent;

      /// Access to the TrigDecisionTool
      ToolHandle< Trig::TrigDecisionTool > m_trigDecTool;
      /// The configured ChainGroup to get the HLT objects from
      const Trig::ChainGroup*              m_chainGroup;

      std::vector< std::string > m_triggerNames; ///< HLT chain names to investigate
      std::string                m_label; ///< Label given to the attached collection
      unsigned int               m_condition; ///< Condition required from the HLT chains
      std::string                m_teName; ///< Name of the TriggerElement from which to get the objects

   }; // class TriggerContainerGetterTool

} // namespace D3PD

// Include the implementation:
#include "TriggerContainerGetterTool.icc"

#endif // TRIGGERD3PDMAKER_TRIGGERCONTAINERGETTERTOOL_H
