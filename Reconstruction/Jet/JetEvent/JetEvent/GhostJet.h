/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENT_GHOSTJET_H
#define JETEVENT_GHOSTJET_H
///////////////////////////////////////////////////////////////////////////////
/// \brief basic GhostJet data class
///
///  The purpose of GhostJets is only to compute Jet 'areas'
///  By adding lots of proto-jets of energy ~0 (GhostJet) to 
///  the input of a jet finder. 
///  This class is here to be able to distinguish a GhostJet than a signal 
///  Jet and to hold the size of the ghost jets grid
///
/// \author P.A. Delsart
/// \date   March 2007
///
///////////////////////////////////////////////////////////////////////////////


#include "JetEvent/Jet.h"

class I4Momentum;

class GhostJet :   public Jet
{
  
 public:
  GhostJet(const I4Momentum& pFourVector) : Jet(pFourVector) {};

  static double get_delta_eta(){return s_delta_eta;}
  static double get_delta_phi(){return s_delta_phi;}

  static void set_delta_eta(double eta){s_delta_eta = eta;}
  static void set_delta_phi(double phi){s_delta_phi = phi;}
 protected:
  static double s_delta_eta ;
  static double s_delta_phi ;
};

#endif //JETEVENT_GHOSTJET_H






