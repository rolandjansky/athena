// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: GenVertexEventAssociationTool.h 348274 2011-02-28 16:25:06Z krasznaa $
/**
 * @file TruthD3PDMaker/src/GenVertexEventAssociationTool.h
 * @author Georges Aad
 * @date Jul, 2010
 * @brief association from MC vertices to MC events 
 * 
 */
#ifndef TRUTHD3PDMAKER_GENVERTEXEVENTASSOCIATIONTOOL_H
#define TRUTHD3PDMAKER_GENVERTEXEVENTASSOCIATIONTOOL_H

// D3PDMaker include(s):
#include "D3PDMakerUtils/SingleAssociationTool.h"

// EDM include(s):
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenVertex.h"

namespace D3PD {

   /**
    * @brief Associate Gen particles to there corresponding event
    *
    */
   class GenVertexEventAssociationTool
      : public SingleAssociationTool< HepMC::GenVertex, HepMC::GenEvent > {

   public:
      /// Convenience typedef
      typedef SingleAssociationTool< HepMC::GenVertex, HepMC::GenEvent > Base;

      /**
       * @brief Standard Gaudi tool constructor.
       * @param type The name of the tool type.
       * @param name The tool name.
       * @param parent The tool's Gaudi parent.
       */
      GenVertexEventAssociationTool( const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent );

      /**
       * @brief Return the target object.
       * @param p The source object for the association.
       *
       * Return the target of the association, or 0.
       */
      virtual const HepMC::GenEvent* get( const HepMC::GenVertex& p );

   }; // class GenVertexEventAssociationTool

} // namespace D3PD

#endif // TRUTHD3PDMAKER_GENVERTEXEVENTASSOCIATIONTOOL_H
