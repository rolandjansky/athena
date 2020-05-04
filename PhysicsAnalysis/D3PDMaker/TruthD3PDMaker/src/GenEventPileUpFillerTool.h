// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: GenEventPileUpFillerTool.h 348274 2011-02-28 16:25:06Z krasznaa $
/**
 * @file TruthD3PDMaker/src/GenEventPileUpFillerTool.h
 * @author Georges Aad
 * @date Nov, 2010
 * @brief Block filler tool for the jet truth info object
 */
#ifndef TRUTHD3PDMAKER_GENEVENTPILEUPFILLERTOOL_H
#define TRUTHD3PDMAKER_GENEVENTPILEUPFILLERTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/ToolHandle.h"

// EDM include(s):
#include "AtlasHepMC/GenEvent.h"

// D3PDMaker include(s):
#include "D3PDMakerInterfaces/ICollectionGetterRegistryTool.h"
#include "D3PDMakerUtils/BlockFillerTool.h"

namespace D3PD {

   // Forward declaration(s):
   class ICollectionGetterTool;

   class GenEventPileUpFillerTool
      : public BlockFillerTool< HepMC::GenEvent > {

   public:
      /// Regular AlgTool constructor
      GenEventPileUpFillerTool( const std::string& type, const std::string& name,
                                const IInterface* parent );

      /**
       * @brief Configure during initialization: type-check.
       * @param tree Our parent for tuple making.
       * @param ti Gives the type of the object being passed to @c fillUntyped.
       *
       * @c configureD3PD should check that the type of the object coming as input
       * is compatible with what it expects, and raise an error otherwise.
       */
      virtual StatusCode configureD3PD( IAddVariable* tree,
                                        const std::type_info& ti );

      /// Book variables for this block.
      virtual StatusCode book();

      /**
       * @brief Fill one block --- type-safe version.
       * @param p The input object.
       *
       * This is called once per object.  The caller
       * is responsible for arranging that all the pointers for booked variables
       * are set appropriately upon entry.
       */
      virtual StatusCode fill( const HepMC::GenEvent& p );

   private:
      // Variables to write out:
      int *m_nparticle;
      short *m_pileUpType;

      /// Prefix of the collection the tool is scheduled for
      std::string m_getterLabel;
      /// The getter tool for accessing the McEventCollection
      ICollectionGetterTool* m_getter;

      /// Tool keeping track of all the getter tools
      ToolHandle< ICollectionGetterRegistryTool > m_registry;

   }; // class GenEventPileUpFillerTool

} // namespace D3PD

#endif // TRUTHD3PDMAKER_GENEVENTPILEUPFILLERTOOL_H
