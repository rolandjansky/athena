// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: QuadrupletCompositeParticleAssociationTool.h 485019 2012-02-24 16:16:04Z krasznaa $
#ifndef HIGGSD3PDMAKER_QuadrupletCompositeParticleAssociationTool_H
#define HIGGSD3PDMAKER_QuadrupletCompositeParticleAssociationTool_H

// EDM include(s):
#include "HSG2Event/Quadruplet.h"
#include "ParticleEvent/CompositeParticle.h"

// D3PD include(s):
#include "D3PDMakerUtils/SingleAssociationTool.h"

namespace D3PD {

   /**
    *  @short Tool "associating" Quadruplet objects to a VxCandiadte 
    *
    *
    * @author Kirill.Prokofiev@cern.ch
    *
    * $Revision: 485019 $
    * $Date: 2012-02-24 17:16:04 +0100 (Fri, 24 Feb 2012) $
    */
   class QuadrupletCompositeParticleAssociationTool :
      public SingleAssociationTool< HSG2::Quadruplet, CompositeParticle > {

   public:
      /// Regular AlgTool constructor
      QuadrupletCompositeParticleAssociationTool( const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent );

      /// Function performing the association
      virtual const CompositeParticle* get( const HSG2::Quadruplet& q );

   }; // class QuadrupletCompositeParticleAssociationTool

} // namespace D3PD

#endif // HIGGSD3PDMAKER_QuadrupletCompositeParticleAssociationTool_H
