/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "SiSPSeededTrackFinderData/ITkSiSpacePointsProSeed.h"

#include "SiSpacePointsSeed/SiSpacePointsSeed.h"
#include "SiSPSeededTrackFinderData/ITkSiSpacePointForSeed.h"

#include <cmath>

namespace ITk
{

  SiSpacePointsProSeed::SiSpacePointsProSeed ()
    {
      m_s0 = nullptr ;
      m_s1 = nullptr ;
      m_s2 = nullptr ;
      m_z  = 0.;
      m_q  = 0.;
    }

  SiSpacePointsProSeed& SiSpacePointsProSeed::operator = 
    (const SiSpacePointsProSeed& sp) 
    {
      if(&sp!=this) {

	m_z   = sp.m_z ;
	m_q   = sp.m_q ;
	m_s0  = sp.m_s0;
	m_s1  = sp.m_s1;
	m_s2  = sp.m_s2;
      }
      return(*this);
    }
 
  SiSpacePointsProSeed::SiSpacePointsProSeed
    (SiSpacePointForSeed*& s0,SiSpacePointForSeed*& s1,SiSpacePointForSeed*& s2,float z)
    {
      set(s0,s1,s2,z); m_q = 0.;
    }

  /////////////////////////////////////////////////////////////////////////////////
  // Copy constructor
  /////////////////////////////////////////////////////////////////////////////////

  SiSpacePointsProSeed::SiSpacePointsProSeed (const SiSpacePointsProSeed& sp): m_s0(sp.m_s0),m_s1(sp.m_s1),m_s2(sp.m_s2)
    {
      *this = sp;
    }

  /////////////////////////////////////////////////////////////////////////////////
  // Destructor
  /////////////////////////////////////////////////////////////////////////////////

  SiSpacePointsProSeed::~SiSpacePointsProSeed() 
  = default;

  /////////////////////////////////////////////////////////////////////////////////
  // Set 
  /////////////////////////////////////////////////////////////////////////////////

  void SiSpacePointsProSeed::set
    (SiSpacePointForSeed*& s0,SiSpacePointForSeed*& s1,SiSpacePointForSeed*& s2,float z)
    {
      m_z   = z ;
      m_s0  = s0;
      m_s1  = s1;
      m_s2  = s2;
    }

  /////////////////////////////////////////////////////////////////////////////////
  // Set two space points seed
  /////////////////////////////////////////////////////////////////////////////////

  void SiSpacePointsProSeed::set2(InDet::SiSpacePointsSeed& s)
    {
      s.erase();
      s.add(m_s0->spacepoint);
      s.add(m_s1->spacepoint);
      s.setZVertex(double(m_z));
    }

  /////////////////////////////////////////////////////////////////////////////////
  // Set three space points seed
  /////////////////////////////////////////////////////////////////////////////////

  bool SiSpacePointsProSeed::set3(InDet::SiSpacePointsSeed& s, float pTPerHelixRadius)
    {
      bool pixt = !m_s2->spacepoint->clusterList().second;
      
      if(pixt) {
        if(m_q > m_s0->quality() && m_q > m_s1->quality() && m_q > m_s2->quality()) return false;
      }
     
      m_s0->setQuality(m_q);
      m_s1->setQuality(m_q);
      m_s2->setQuality(m_q);
      
      s.erase();
      s.add(m_s0->spacepoint);
      s.add(m_s1->spacepoint);
      s.add(m_s2->spacepoint);
      s.setZVertex(double(m_z)); 
      s.setX1(m_s0->x());
      s.setX2(m_s1->x());
      s.setX3(m_s2->x());
      s.setY1(m_s0->y());
      s.setY2(m_s1->y());
      s.setY3(m_s2->y());
      s.setZ1(m_s0->z());
      s.setZ2(m_s1->z());
      s.setZ3(m_s2->z());
      s.setR1(m_s0->radius());
      s.setR2(m_s1->radius());
      s.setR3(m_s2->radius());

      estimateParameters(pTPerHelixRadius);
      s.setD0(m_s2->param());
      s.setEta(m_s2->eta());
      s.setDZDR_B(m_s0->dzdr());
      s.setDZDR_T(m_s2->dzdr());
      s.setPt(m_s2->pt());

      return true;
    }

  void SiSpacePointsProSeed::estimateParameters(float pTPerHelixRadius)
  {

    ITk::SiSpacePointForSeed& bottom = *m_s0;
    ITk::SiSpacePointForSeed& medium = *m_s1;
    ITk::SiSpacePointForSeed& top    = *m_s2;

    bool isPixel = !m_s2->spacepoint->clusterList().second;

    auto extractCoordinates =
      [] (const ITk::SiSpacePointForSeed& sp) -> std::array<float,4>
      {
        std::array<float, 4> coordinates {sp.x(), sp.y(), sp.z(), sp.radius()};
        return coordinates;
      };

    auto extractQuantities =
      [] (const std::array<float, 4>& sp,
      const std::array<float, 4>& spM,
      bool isBottom) -> std::array<float, 5>
      {
        auto& [xM, yM, zM, rM] = spM;
        auto& [xO, yO, zO, rO] = sp;

        float cosPhiM = xM / rM;
        float sinPhiM = yM / rM;
        float deltaX = xO - xM;
        float deltaY = yO - yM;
        float deltaZ = zO - zM;
        float x = deltaX * cosPhiM + deltaY * sinPhiM;
        float y = deltaY * cosPhiM - deltaX * sinPhiM;
        float iDeltaR2 = 1. / (deltaX * deltaX + deltaY * deltaY);
        float iDeltaR = std::sqrt(iDeltaR2);
        int bottomFactor = 1 * (int(not isBottom)) - 1 * (int(isBottom));
        float cot_theta = deltaZ * iDeltaR * bottomFactor;

        // cotTheta, Zo, iDeltaR, U, V
        std::array<float, 5> params =
        {
          cot_theta,
          zM - rM * cot_theta,
          iDeltaR,
          x * iDeltaR2,
          y * iDeltaR2
        };

        return params;
      };

    auto coo_b = extractCoordinates(bottom);
    auto coo_m = extractCoordinates(medium);
    auto coo_t = extractCoordinates(top);

    // Compute the variables we need
    auto [cotThetaB, Zob, iDeltaRB, Ub, Vb] = extractQuantities(coo_b, coo_m, true);
    auto [cotThetaT, Zot, iDeltaRT, Ut, Vt] = extractQuantities(coo_t, coo_m, false);

    float squarediDeltaR2B = iDeltaRB*iDeltaRB;
    float squarediDeltaR2T = iDeltaRB*iDeltaRT;
    float squarediDeltaR = std::min(squarediDeltaR2B, squarediDeltaR2T);

    auto& [xB, yB, zB, rB] = coo_b;
    auto& [xM, yM, zM, rM] = coo_m;
    auto& [xT, yT, zT, rT] = coo_t;

    float ax = xM / rM;
    float ay = yM/ rM;

    float dxb = xM - xB;
    float dyb = yM - yB;
    float dzb = zM - zB;
    float xb = dxb * ax + dyb *ay;
    float yb = dyb * ax - dxb * ay;
    float dxyb = xb * xb + yb * yb;
    float drb = std::sqrt( xb*xb + yb*yb + dzb*dzb );

    float dxt = xT - xM;
    float dyt = yT - yM;
    float dzt = zT - zM;
    float xt = dxt * ax + dyt *ay;
    float yt = dyt * ax - dxt * ay;
    float dxyt = xt * xt + yt * yt;
    float drt = std::sqrt( xt*xt + yt*yt + dzt*dzt );

    float tzb = dzb * std::sqrt( 1./dxyb );
    float tzt = dzt * std::sqrt( 1./dxyt );

    float sTzb2 = std::sqrt(1 + tzb*tzb);

    float dU = Ut - Ub;
    if (dU == 0.) {
      return;
    }

    float A = (Vt - Vb) / dU;
    float S2 = 1. + A * A;
    float B = Vb - A * Ub;
    float B2 = B * B;

    // dzdr
    float dzdr_b = (zM - zB) / (rM - rB);
    float dzdr_t = (zT - zM) / (rT - rM);

    // eta
    float meanOneOverTanThetaSquare = isPixel ? (cotThetaB * cotThetaT) :
                                                 std::pow((cotThetaB + cotThetaT) / 2.,2);
    float theta = std::atan(1. / std::sqrt(meanOneOverTanThetaSquare)); // [0, pi/2)
    if (top.z()<0) {theta = -theta;} // (-pi/2, pi/2)
    if (theta < 0.) {theta = theta + M_PI;} // [0, pi)
    float eta = -std::log(std::tan(0.5 * theta));

    // pt
    float pt = pTPerHelixRadius*std::sqrt(S2 / B2);

    // d0
    float d0 = std::abs((A - B * rM) * rM);

    // Attach variables to SPs
    top.setDR(drt);
    top.setDZDR(dzdr_t);
    top.setScorePenalty( std::abs((tzb - tzt) / (squarediDeltaR * sTzb2)) );
    top.setParam(d0);
    top.setEta(eta);
    top.setPt(pt);

    bottom.setDR(drb);
    bottom.setDZDR(dzdr_b);

  }

  /////////////////////////////////////////////////////////////////////////////////
  // Set quality in pro seed
  /////////////////////////////////////////////////////////////////////////////////

  bool SiSpacePointsProSeed::setQuality(float q)
    {
      m_q = q;

      if(!m_s2->spacepoint->clusterList().second) {
        if(q > m_s0->quality() && q > m_s1->quality() && q > m_s2->quality()) return false;
      }
      m_s0->setQuality(m_q);
      m_s1->setQuality(m_q);
      m_s2->setQuality(m_q);
      return true;
    }

} // end of name space ITk
