// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: GenEventGetterTool.h 348274 2011-02-28 16:25:06Z krasznaa $
/**
 * @file TruthD3PDMaker/src/GenEventGetterTool.h
 * @author Georges Aad
 * @date Nov, 2010
 * @brief getter for GenEvent
*/
#ifndef TRUTHD3PDMAKER_GENEVENTGETTERTOOL_H
#define TRUTHD3PDMAKER_GENEVENTGETTERTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/ToolHandle.h"

// EDM include(s):
#include "GeneratorObjects/McEventCollection.h"

// Helper tool(s):
#include "TruthD3PDAnalysis/IGenObjectsFilterTool.h"

// D3PDMaker include(s):
#include "D3PDMakerUtils/SGCollectionGetterTool.h"

namespace D3PD { 

   /**
    *  @short Getter tool iterating over "good" GenEvent objects
    *
    *         This tool can be used to feed a D3PD job with "good"
    *         GenEvent objects.
    *
    * @author Georges Aad <aad@cern.ch>
    *
    * $Revision: 348274 $
    * $Date: 2011-02-28 17:25:06 +0100 (Mon, 28 Feb 2011) $
    */
   class GenEventGetterTool 
      : public SGCollectionGetterTool< McEventCollection > {

   public:
      /// Convenience typedef
      typedef SGCollectionGetterTool< McEventCollection > Base; 

      /// Default AlgTool constructor
      GenEventGetterTool( const std::string& type, const std::string& name, 
                          const IInterface* parent );

      /// Initialization function
      virtual StatusCode initialize();
      virtual size_t sizeHint( bool allowMissing = false );
      virtual StatusCode reset( bool sllowMissing = false );
      virtual const void* nextUntyped();
  
   private:
      /// Pointer to the current McEventCollection object
      const McEventCollection* m_mcColl;

      /// Iterator pointing at the current GenEvent object
      McEventCollection::const_iterator m_evtItr;
      /// Iterator pointing at the last GenEvent object
      McEventCollection::const_iterator m_evtEnd;

      /// Tool used to select "good" GenEvent objects
      ToolHandle< IGenObjectsFilterTool > m_selector;

   }; // class GenEventGetterTool 

} // namespace D3PD

#endif //  TRUTHD3PDMAKER_GENEVENTGETTERTOOL_H
