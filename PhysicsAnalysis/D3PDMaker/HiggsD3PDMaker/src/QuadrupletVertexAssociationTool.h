// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: QuadrupletVertexAssociationTool.h 485019 2012-02-24 16:16:04Z krasznaa $
#ifndef HIGGSD3PDMAKER_QuadrupletVertexAssociationTool_H
#define HIGGSD3PDMAKER_QuadrupletVertexAssociationTool_H

// EDM include(s):
#include "HSG2Event/Quadruplet.h"
#include "VxVertex/VxCandidate.h"

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
   class QuadrupletVertexAssociationTool :
      public SingleAssociationTool< HSG2::Quadruplet, Trk::VxCandidate > {

   public:
      /// Regular AlgTool constructor
      QuadrupletVertexAssociationTool( const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent );

      /// Function performing the association
      virtual const Trk::VxCandidate* get( const HSG2::Quadruplet& q );

   }; // class QuadrupletVertexAssociationTool

} // namespace D3PD

#endif // HIGGSD3PDMAKER_QuadrupletVertexAssociationTool_H
