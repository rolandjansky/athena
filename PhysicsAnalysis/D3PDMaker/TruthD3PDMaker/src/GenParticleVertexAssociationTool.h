// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: GenParticleVertexAssociationTool.h 348274 2011-02-28 16:25:06Z krasznaa $
/**
 * @file TruthD3PDMaker/src/GenParticleVertexAssociationTool.h
 * @author Georges Aad
 * @date Oct, 2010
 * @brief association from MC particles to MC vertices
 * 
 */
#ifndef TRUTHD3PDMAKER_GENPARTICLEVERTEXASSOCIATIONTOOL_H
#define TRUTHD3PDMAKER_GENPARTICLEVERTEXASSOCIATIONTOOL_H

// D3PDMaker include(s):
#include "D3PDMakerUtils/SingleAssociationTool.h"

// EDM include(s):
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"

namespace D3PD {

   /**
    * @brief Associate Gen particles to there corresponding production and decay
    *        vertices
    *
    * $Revision: 348274 $
    * $Date: 2011-02-28 17:25:06 +0100 (Mon, 28 Feb 2011) $
    */
   class GenParticleVertexAssociationTool
      : public SingleAssociationTool< HepMC::GenParticle, HepMC::GenVertex > {

   public:
      /// Convenience typedef
      typedef SingleAssociationTool< HepMC::GenParticle, HepMC::GenVertex > Base;

      /**
       * @brief Standard Gaudi tool constructor.
       * @param type The name of the tool type.
       * @param name The tool name.
       * @param parent The tool's Gaudi parent.
       */
      GenParticleVertexAssociationTool( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent );

      /**
       * @brief Return the target object.
       * @param p The source object for the association.
       *
       * Return the target of the association, or 0.
       */
      virtual const HepMC::GenVertex* get( const HepMC::GenParticle& p );

   private:
      /// When set to true, return the decay vertex of the particle
      bool m_decay; 

   }; // class GenParticleVertexAssociationTool

} // namespace D3PD

#endif // TRUTHD3PDMAKER_GENPARTICLEVERTEXASSOCIATIONTOOL_H
