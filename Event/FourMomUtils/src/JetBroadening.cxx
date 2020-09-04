/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "FourMomUtils/JetBroadening.h"

// AthAnalysisBase/ManaCore doesn't currently include the Trigger Service
#ifndef XAOD_ANALYSIS

#include <cmath>

namespace FourMomUtils {

using std::abs;
using std::exp;

bool jetBroadening( const I4MomIter_t& iBeg, const I4MomIter_t& iEnd,
        double& wideJetBroadening, double& totalJetBroadening,
        CLHEP::Hep3Vector thrust, bool useThreeD )
{
  if(abs(thrust.mag()-1)>0.01)
    return false;

  double Qu=0;
  double Qd=0;
  double etau=0;
  double etad=0;
  double phiu=0;
  double phid=0;

  // ensure z component is zero
  if(!useThreeD)
    thrust.setZ(0);

  // determine average eta/phi for each hemisphere
  for ( I4MomIter_t itr = iBeg; itr != iEnd; ++itr )
    {
      // default : standard definition of z component zero
      double z=0;
      if(useThreeD)
        z=(*itr)->pz();

      CLHEP::Hep3Vector c( (*itr)->px(), (*itr)->py(), z );
      if(c.dot(thrust)>0)
        {
          Qu   += c.mag();
          etau += (*itr)->eta() * c.mag();
          phiu += (*itr)->phi() * c.mag();
        }else{
          Qd   += c.mag();
          etad += (*itr)->eta() * c.mag();
          phid += (*itr)->phi() * c.mag();
        }
    }

  const double inv_Qu = 1. / Qu;
  const double inv_Qd = 1. / Qd;

  etau *= inv_Qu;
  etad *= inv_Qd;

  phiu *= inv_Qu;
  phid *= inv_Qd;

  double Bu=0;
  double Bd=0;

  // now detemine JetBroadenings in each hemisphere
  for ( I4MomIter_t itr = iBeg; itr != iEnd; ++itr )
    {
      // default : standard definition of z component zero
      double z=0;
      if(useThreeD)
        z=(*itr)->pz();

      CLHEP::Hep3Vector c( (*itr)->px(), (*itr)->py(), z );

      if(c.dot(thrust)>0)
        {
          Bu   += c.mag() *
            sqrt((etau-(*itr)->eta())*(etau-(*itr)->eta())+
                 (phiu-(*itr)->phi())*(phiu-(*itr)->phi()));
        }else{
          Bd   += c.mag() *
            sqrt((etad-(*itr)->eta())*(etad-(*itr)->eta())+
                 (phid-(*itr)->phi())*(phid-(*itr)->phi()));
        }
    }

  Bu /= 2*(Qu+Qd);
  Bd /= 2*(Qu+Qd);

  totalJetBroadening=Bu+Bd;
  wideJetBroadening=Bu;
  if(Bd>Bu)
    wideJetBroadening=Bd;

  return true;
}

} //> end namespace FourMomUtils

#endif
