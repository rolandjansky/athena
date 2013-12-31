// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef TRIGBJETD3PDMAKER_L2BJETTOTRACKASSOCIATIONTOOL_H
#define TRIGBJETD3PDMAKER_L2BJETTOTRACKASSOCIATIONTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/ToolHandle.h"

// Trigger include(s):
#include "TrigDecisionTool/TrigDecisionTool.h"

// EDM include(s):
#include "TrigParticle/TrigL2Bjet.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"

// D3PDMaker include(s):
#include "D3PDMakerUtils/MultiAssociationTool.h"

namespace D3PD {

   class L2BjetToTrackAssociationTool :
      public MultiAssociationTool< TrigL2Bjet, TrigInDetTrack > {

      typedef MultiAssociationTool< TrigL2Bjet, TrigInDetTrack > Base;

   public:
      L2BjetToTrackAssociationTool( const std::string &type, const std::string &name,
                                    const IInterface  *parent);

      virtual StatusCode initialize();
      virtual StatusCode reset( const TrigL2Bjet &p );
      virtual const TrigInDetTrack *next();

   private:
      bool m_valid;
      TrigInDetTrackCollection::const_iterator m_trkBegin;
      TrigInDetTrackCollection::const_iterator m_trkEnd;

      ToolHandle< Trig::TrigDecisionTool > m_trigDec;

   }; // class L2BjetToTrackAssociationTool

} // namespace D3PD

#endif // TRIGBJETD3PDMAKER_L2BJETTOTRACKASSOCIATIONTOOL_H
