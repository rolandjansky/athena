// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: GenParticleEventAssociationTool.h 348274 2011-02-28 16:25:06Z krasznaa $
/**
 * @file TruthD3PDMaker/src/GenParticleEventAssociationTool.h
 * @author Georges Aad
 * @date Oct, 2010
 * @brief association from MC particles to MC events 
 * 
 */
#ifndef TRUTHD3PDMAKER_GENPARTICLEEVENTASSOCIATIONTOOL_H
#define TRUTHD3PDMAKER_GENPARTICLEEVENTASSOCIATIONTOOL_H

// D3PDMaker include(s):
#include "D3PDMakerUtils/SingleAssociationTool.h"

// EDM include(s):
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenParticle.h"

namespace D3PD {

   /**
    * @brief Associate Gen particles to there corresponding event
    *
    */
   class GenParticleEventAssociationTool
      : public SingleAssociationTool< HepMC::GenParticle, HepMC::GenEvent > {

   public:
      /// Convenience typedef
      typedef SingleAssociationTool< HepMC::GenParticle, HepMC::GenEvent > Base;

      /**
       * @brief Standard Gaudi tool constructor.
       * @param type The name of the tool type.
       * @param name The tool name.
       * @param parent The tool's Gaudi parent.
       */
      GenParticleEventAssociationTool( const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent );

      /**
       * @brief Return the target object.
       * @param p The source object for the association.
       *
       * Return the target of the association, or 0.
       */
      virtual const HepMC::GenEvent* get( const HepMC::GenParticle& p );

   }; // class GenParticleEventAssociationTool

} // namespace D3PD

#endif // TRUTHD3PDMAKER_GENPARTICLEEVENTASSOCIATIONTOOL_H
