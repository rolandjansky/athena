// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TruthVertexFwd.h 574253 2013-12-06 15:21:49Z krasznaa $
#ifndef XAODTRUTH_TRUTHVERTEXFWD_H
#define XAODTRUTH_TRUTHVERTEXFWD_H

// This is a forward declaration for the latest truth vertex type.
// We need it because TruthVertex needs to use TruthParticle, and
// TruthParticle also needs to use TruthVertex.

namespace xAOD {
   class TruthVertex_v1;
   typedef TruthVertex_v1 TruthVertex;
}

#endif // XAODTRUTH_TRUTHVERTEXFWD_H
