// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: egammaEFlessForTriggerAssociation.h 364392 2011-05-07 15:08:08Z krasznaa $
#ifndef EGAMMAD3PDMAKER_EGAMMAEFLESSFORTRIGGERASSOCIATION_H
#define EGAMMAD3PDMAKER_EGAMMAEFLESSFORTRIGGERASSOCIATION_H

class IAlgTool;
class egamma;
class I4Momentum;
namespace D3PD {
   bool lessForTriggerAssociation (IAlgTool* tool,
                                   const I4Momentum* p0,
                                   const egamma* p1,
                                   const egamma* p2);
}

#endif // EGAMMAD3PDMAKER_EGAMMAEFLESSFORTRIGGERASSOCIATION_H
