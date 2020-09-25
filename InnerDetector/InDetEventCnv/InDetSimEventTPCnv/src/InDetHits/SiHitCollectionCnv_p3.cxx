/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetSimEvent/SiHit.h"
#include "InDetSimEvent/SiHitCollection.h"
#include "InDetSimEventTPCnv/InDetHits/SiHitCollection_p3.h"
#include "InDetSimEventTPCnv/InDetHits/SiHitCollectionCnv_p3.h"

#include <cmath>

//CLHEP
#include "CLHEP/Geometry/Point3D.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
// Athena
#include "StoreGate/StoreGateSvc.h"

//  * * *  stolen from eflowRec  * * *  //
inline double phicorr(double a)
{
  if (a <= -M_PI)
    {
      return a+(2*M_PI*floor(-(a-M_PI)/(2*M_PI)));
    }
  else if (a > M_PI)
    {
      return a-(2*M_PI*floor((a+M_PI)/(2*M_PI)));
    }
  else
    {
      return a;
    }
}

//  * * *  stolen from eflowRec  * * *  //
inline double cycle(double a, double b)
{
  double del = b-a;
  if (del > M_PI)
    {
      return a+2.0*M_PI;
    }
  else if (del < -M_PI)
    {
      return a-2.0*M_PI;
    }
  else
    {
      return a;
    }
}


const double SiHitCollectionCnv_p3::m_persEneUnit = 1.0e-5;
const double SiHitCollectionCnv_p3::m_persLenUnit = 1.0e-5;
const double SiHitCollectionCnv_p3::m_persAngUnit = 1.0e-5;
const double SiHitCollectionCnv_p3::m_2bHalfMaximum = pow(2.0, 15.0);
const int SiHitCollectionCnv_p3::m_2bMaximum = (unsigned short)(-1);


void SiHitCollectionCnv_p3::transToPers(const SiHitCollection*, SiHitCollection_p3*, MsgStream &/*log*/)
{
  throw std::runtime_error("SiHitCollectionCnv_p3::transToPers is not supported in this release!");
}


SiHitCollection* SiHitCollectionCnv_p3::createTransient(const SiHitCollection_p3* persObj, MsgStream &log) {
  std::auto_ptr<SiHitCollection> trans(new SiHitCollection("DefaultCollectionName",persObj->m_nHits.size()));
  persToTrans(persObj, trans.get(), log);
  return(trans.release());
}


void SiHitCollectionCnv_p3::persToTrans(const SiHitCollection_p3* persCont, SiHitCollection* transCont, MsgStream &/*log*/)
{
  unsigned int hitCount = 0;
  unsigned int angleCount = 0;
  unsigned int idxBC = 0;
  unsigned int idxId = 0;
  unsigned int idxEne4b = 0;
  unsigned int idxLen4b = 0;
  unsigned int endHit = 0;
  unsigned int endBC = 0;
  unsigned int endId = 0;

  for (unsigned int i = 0; i < persCont->m_nHits.size(); i++) {

    if (persCont->m_nHits[i]) {

      const unsigned int start = endHit;
      endHit += persCont->m_nHits[i];

      const double t0 = persCont->m_hit1_meanTime[i];
      const double theta0 = persCont->m_hit1_theta[i];
      const double phi0 = persCont->m_hit1_phi[i];
      HepGeom::Point3D<double> endLast(persCont->m_hit1_x0[i], persCont->m_hit1_y0[i], persCont->m_hit1_z0[i]);

      for (unsigned int j = start; j < endHit; j++) {

        if (j >= endBC + persCont->m_nBC[idxBC])
          endBC += persCont->m_nBC[idxBC++];

        if (j >= endId + persCont->m_nId[idxId])
          endId += persCont->m_nId[idxId++];

        const double eneLoss_2b = persCont->m_hitEne_2b[hitCount];
        const double hitLength_2b = persCont->m_hitLength_2b[hitCount];

        const double eneLoss = (eneLoss_2b < m_2bMaximum) ? eneLoss_2b * m_persEneUnit : persCont->m_hitEne_4b[idxEne4b++];
        const double length = (hitLength_2b < m_2bMaximum) ? hitLength_2b * m_persLenUnit : persCont->m_hitLength_4b[idxLen4b++];

        const double dTheta = (j > start) ? ((double)persCont->m_dTheta[angleCount] - m_2bHalfMaximum) * m_persAngUnit : 0.0;
        const double dPhi = (j > start) ? ((double)persCont->m_dPhi[angleCount] - m_2bHalfMaximum) * m_persAngUnit : 0.0;

        const double meanTime = t0;
        const double theta = theta0 + dTheta;
        const double phi = phicorr(phi0 + dPhi);

        CLHEP::Hep3Vector r(length, 0.0, 0.0);
        r.setTheta(theta);
        r.setPhi(phi);

        HepGeom::Point3D<double> endThis( endLast + r );

        transCont->Emplace( endLast, endThis, eneLoss, meanTime, persCont->m_barcode[idxBC], persCont->m_id[idxId]);

        endLast = endThis;

        ++hitCount;
        if (j > start) ++angleCount;
      }
    }
  }
}
