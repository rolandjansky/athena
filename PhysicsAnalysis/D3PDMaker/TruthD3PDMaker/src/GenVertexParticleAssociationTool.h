// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: GenVertexParticleAssociationTool.h 348274 2011-02-28 16:25:06Z krasznaa $
/**
 * @file TruthD3PDMaker/src/GenVertexParticleAssociationTool.h
 * @author Georges Aad
 * @date Oct, 2010
 * @brief association from MC vertices to MC particles
 * 
 */
#ifndef TRUTHD3PDMAKER_GENVERTEXPARTICLEASSOCIATIONTOOL_H
#define TRUTHD3PDMAKER_GENVERTEXPARTICLEASSOCIATIONTOOL_H

// STL include(s):
#include <vector>

// D3PDMaker include(s):
#include "D3PDMakerUtils/MultiAssociationTool.h"

// EDM include(s):
#include "AtlasHepMC/GenVertex.h"
#include "AtlasHepMC/GenParticle.h"

namespace D3PD {

   /**
    * @brief Associate Gen vertices to there corresponding in and out particles
    * can be used also for particle parent child association if conbined with
    * particle-vertex association
    *
    * $Revision: 348274 $
    * $Date: 2011-02-28 17:25:06 +0100 (Mon, 28 Feb 2011) $
    */
   class GenVertexParticleAssociationTool
      : public MultiAssociationTool< HepMC::GenVertex, HepMC::GenParticle > {

   public:
      /// Convenience typedef
      typedef MultiAssociationTool< HepMC::GenVertex, HepMC::GenParticle > Base;

      /**
       * @brief Standard Gaudi tool constructor.
       * @param type The name of the tool type.
       * @param name The tool name.
       * @param parent The tool's Gaudi parent.
       */
      GenVertexParticleAssociationTool( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent );

      /**
       * @brief Start the iteration for a new association.
       * @param p The object from which to associate.
       */
      virtual StatusCode reset( const HepMC::GenVertex& p );

      /**
       * @brief Return a pointer to the next element in the association.
       * Return 0 when the association has been exhausted.
       */
      const HepMC::GenParticle* next();

   private:
      /// Iterator pointing at the current particle
      std::vector< HepMC::GenParticle* >::const_iterator m_partItr;
      /// Iterator pointing at the last particle
      std::vector< HepMC::GenParticle* >::const_iterator m_partEnd;

      /// Associate the incoming particles of the vertex
      bool m_inParticles; 

   }; // class GenVertexParticleAssociationTool

} // namespace D3PD

#endif // TRUTHD3PDMAKER_GENVERTEXPARTICLEASSOCIATIONTOOL_H
