// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef TRIGBJETD3PDMAKER_EFBJETTOTRACKASSOCIATIONTOOL_H
#define TRIGBJETD3PDMAKER_EFBJETTOTRACKASSOCIATIONTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/ToolHandle.h"

// Trigger include(s):
#include "TrigDecisionTool/TrigDecisionTool.h"

// EDM include(s):
#include "TrigParticle/TrigEFBjet.h"
#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"

// D3PDMaker icnlude(s):
#include "D3PDMakerUtils/MultiAssociationTool.h"

namespace D3PD {

   class EFBjetToTrackAssociationTool :
      public MultiAssociationTool< TrigEFBjet, Rec::TrackParticle > {

      typedef MultiAssociationTool< TrigEFBjet, Rec::TrackParticle > Base;

   public:
      EFBjetToTrackAssociationTool( const std::string &type, const std::string &name,
                                    const IInterface  *parent );

      virtual StatusCode initialize();
      virtual StatusCode reset( const TrigEFBjet &p );
      virtual const Rec::TrackParticle* next();

   private:
      bool m_valid;
      Rec::TrackParticleContainer::const_iterator m_trkBegin;
      Rec::TrackParticleContainer::const_iterator m_trkEnd;

      ToolHandle< Trig::TrigDecisionTool > m_trigDec;

   }; // class EFBjetToTrackAssociationTool

} // namespace D3PD

#endif // TRIGBJETD3PDMAKER_EFBJETTOTRACKASSOCIATIONTOOL_H
