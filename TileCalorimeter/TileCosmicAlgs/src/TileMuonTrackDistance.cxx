/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     TileMuonTrackDistance
 * PACKAGE:  offline/TileCalorimeter/TileCosmicAlgs
 *
 * AUTHOR :  J. Maneira
 * CREATED:  10-Jul-2006
 *
 * PURPOSE:  Minuit fit function for TileMuonFitter
 *           Supplies summed distance from cells to track
 *
 *  Input: Cells
 *  Output: Track parameters
 *   
 ********************************************************************/
//
#include "TileCosmicAlgs/TileMuonTrackDistance.h"

using CLHEP::Hep3Vector;

namespace ROOT {
  namespace Minuit2 {

    void TileMuonTrackDistance::Means() {
      m_meanX = m_meanY = m_meanZ = 0.;
      double eSum = 0.;
      int ncells = m_theX.size();

      if (m_doWeighted) {
        for (int i = 0; i < ncells; i++) {
          m_meanX += m_theE[i] * m_theX[i];
          m_meanY += m_theE[i] * m_theY[i];
          m_meanZ += m_theE[i] * m_theZ[i];
          eSum += m_theE[i];
        }
        if (eSum > 0) {
          m_meanX /= eSum;
          m_meanY /= eSum;
          m_meanZ /= eSum;
        } else {
          m_meanX = 0;
          m_meanY = 0;
          m_meanZ = 0;
        }
      } else {
        for (int i = 0; i < ncells; i++) {
          m_meanX += m_theX[i];
          m_meanY += m_theY[i];
          m_meanZ += m_theZ[i];
        }
        m_meanX /= (double) (ncells);
        m_meanY /= (double) (ncells);
        m_meanZ /= (double) (ncells);
      }

    }
    /*==========================================================================*/
    double TileMuonTrackDistance::operator()(const std::vector<double>& par) const {
      double distSum2 = 0;
      std::vector<double> fourPar;

      fourPar.push_back(m_meanY - par[0] * m_meanX);
      fourPar.push_back(par[0]);
      fourPar.push_back(m_meanZ - par[1] * m_meanX);
      fourPar.push_back(par[1]);

      for (int i = 0; i < (int) (m_theX.size()); i++) {
        if (m_doWeighted) {
          distSum2 += m_theE[i] * Distance2SinglePoint(m_theX[i], m_theY[i], m_theZ[i], fourPar);
        } else {
          distSum2 += Distance2SinglePoint(m_theX[i], m_theY[i], m_theZ[i], fourPar);
        }
      }

      return distSum2;

    }
    /*==========================================================================*/
    double TileMuonTrackDistance::Distance2SinglePoint(double x, double y, double z,
        const std::vector<double>& par) const {

      Hep3Vector dataP(x, y, z);
      Hep3Vector lineP(ClosestPoint(&dataP, par));

      return (dataP - lineP).mag2();
    }
    /*==========================================================================*/
    Hep3Vector TileMuonTrackDistance::ClosestPoint(Hep3Vector *dataPoint, const std::vector<double>& par) const {

      double aa, bb, cc, dd;
      double xort = 0;
      Hep3Vector linePoint;

      aa = par[0];
      bb = par[1];
      cc = par[2];
      dd = par[3];

      xort = dataPoint->getX() + bb * (dataPoint->getY() - aa) + dd * (dataPoint->getZ() - cc);
      xort /= (1 + bb * bb + dd * dd);
      linePoint.setX(xort);
      linePoint.setY(aa + bb * xort);
      linePoint.setZ(cc + dd * xort);

      return linePoint;
    }

  } //namespace Minuit2

}   //namespace ROOT

