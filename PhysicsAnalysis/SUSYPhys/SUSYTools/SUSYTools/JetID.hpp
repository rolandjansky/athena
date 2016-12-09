/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __JETID__
#define __JETID__

namespace JetID
{
 typedef enum  { VeryLooseBad=0, LooseBad, MediumBad, TightBad, NewTightBad   } BadJetCategory;
bool isBadJet(BadJetCategory criteria,
	      double emf,
	      double hecf,
	      double larq,
	      double hecq,
	      double time,
	      double sumpttrk,
	      double eta,
	      double pt,
	      double fmax,
	      double negE,
	      double Qmean=-1
	      )  ;
}

#endif
