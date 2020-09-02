// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: GenParticleParticleAssociationTool.h 348274 2011-02-28 16:25:06Z krasznaa $
/**
 * @file TruthD3PDMaker/src/GenParticleParticleAssociationTool.h
 * @author Georges Aad
 * @date Oct, 2010
 * @brief association from GenParticle to GenParticle
 * 
 */
#ifndef TRUTHD3PDMAKER_GENPARTICLEPARTICLEASSOCIATIONTOOL_H
#define TRUTHD3PDMAKER_GENPARTICLEPARTICLEASSOCIATIONTOOL_H

// D3PDMaker include(s):
#include "D3PDMakerUtils/SingleAssociationTool.h"

// EDM include(s):
#include "AtlasHepMC/GenParticle.h"

namespace D3PD {

   /**
    * @brief Associate Gen particles to Gen particles
    * used to associate charged particles with truth track info 
    * that is needed for a subset of particles
    *
    * $Revision: 348274 $
    * $Date: 2011-02-28 17:25:06 +0100 (Mon, 28 Feb 2011) $
    */
   class GenParticleParticleAssociationTool
      : public SingleAssociationTool< HepMC::GenParticle, HepMC::GenParticle > {

   public:
      /// Convenience typedef
      typedef SingleAssociationTool< HepMC::GenParticle, HepMC::GenParticle > Base;

      /**
       * @brief Standard Gaudi tool constructor.
       * @param type The name of the tool type.
       * @param name The tool name.
       * @param parent The tool's Gaudi parent.
       */
      GenParticleParticleAssociationTool( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent );

      /**
       * @brief Return the target object.
       * @param p The source object for the association.
       *
       * Return the target of the association, or 0.
       */
      virtual const HepMC::GenParticle* get( const HepMC::GenParticle& p );

   }; // class GenParticleParticleAssociationTool

} // namespace D3PD

#endif // TRUTHD3PDMAKER_GENPARTICLEPARTICLEASSOCIATIONTOOL_H
