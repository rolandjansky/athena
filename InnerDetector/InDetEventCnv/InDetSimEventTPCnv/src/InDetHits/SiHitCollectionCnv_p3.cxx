/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetSimEvent/SiHit.h"
#include "InDetSimEvent/SiHitCollection.h"
#include "InDetSimEventTPCnv/InDetHits/SiHitCollection_p3.h"
#include "InDetSimEventTPCnv/InDetHits/SiHitCollectionCnv_p3.h"
#include "GeneratorObjects/HepMcParticleLink.h"

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


void SiHitCollectionCnv_p3::transToPers(const SiHitCollection* transCont, SiHitCollection_p3* persCont, MsgStream &/*log*/)
{
  // Finds hits belonging to a "string" (in which the end point of one hit is the same as the start point of the next) and
  // persistifies the end point of each hit plus the start point of the first hit in each string.
  //
  // Further compression is achieved by optimising the storage of the position vectors:- start (x,y,z) and (theta,phi) of
  // first hit are stored as floats, (delta_theta,delta_phi) relative to the fisrst hit are stored as 2 byte numbers and
  // used to specify the hit direction. All hit lengths are stored as 2 byte numbers.
  //
  // Additional savings are achieved by storing the energy loss for each hit as a 2 byte number and only storing the mean
  // time of the first hit per string.
  //
  // See http://indico.cern.ch/getFile.py/access?contribId=11&resId=2&materialId=slides&confId=30893 for more info.

  static const double dRcut = 1.0e-7;
  static const double dTcut = 1.0;

  const HepMcParticleLink * lastLink=nullptr;
  int lastId = -1;
  double stringFirstTheta = 0.0;
  double stringFirstPhi = 0.0;
  double lastT = 0.0;
  double persSumE = 0.0;
  double transSumE = 0.0;
  unsigned int idx = 0;
  unsigned int endBC = 0;
  unsigned int endId = 0;
  unsigned int endHit = 0;
  HepGeom::Point3D<double> lastTransEnd(0.0, 0.0, 0.0);
  HepGeom::Point3D<double> lastPersEnd(0.0, 0.0, 0.0);

  for (SiHitCollection::const_iterator it = transCont->begin(); it != transCont->end(); ++it) {

    SiHitCollection::const_iterator siHit = it;


    if ( !lastLink || (siHit->particleLink() != *lastLink) ) {

      // store barcode once for set of consecutive hits with same barcode

      lastLink = &(siHit->particleLink());
      persCont->m_barcode.push_back(lastLink->barcode());
      persCont->m_mcEvtIndex.push_back(lastLink->eventIndex());
      persCont->m_evtColl.push_back(lastLink->getEventCollectionAsChar());

      if (idx > 0) {
        persCont->m_nBC.push_back(idx - endBC);
        endBC = idx;
      }
    }

    if ( (int)siHit->identify() != lastId ) {

      // store id once for set of consecutive hits with same barcode

      lastId = siHit->identify();
      persCont->m_id.push_back(lastId);

      if (idx > 0) {
        persCont->m_nId.push_back(idx - endId);
        endId = idx;
      }
    }

    HepGeom::Point3D<double> st = siHit->localStartPosition();
    HepGeom::Point3D<double> en = siHit->localEndPosition();

    const double dx = st.x() - lastTransEnd.x();
    const double dy = st.y() - lastTransEnd.y();
    const double dz = st.z() - lastTransEnd.z();
    const double t = siHit->meanTime();

    const double dRLast = sqrt(dx * dx + dy * dy + dz * dz);  // dR between end of previous hit and start of current one
    const double dTLast = fabs(t - lastT);

    CLHEP::Hep3Vector direction(0.0, 0.0, 0.0);
    double theta = 0.0;
    double phi = 0.0;
    bool startNewString = false;

    if (dRLast < dRcut && dTLast < dTcut) {

      // hit is part of existing string

      direction = CLHEP::Hep3Vector( en.x() - lastPersEnd.x(), en.y() - lastPersEnd.y(), en.z() - lastPersEnd.z() );

      theta = direction.theta();
      phi = phicorr( direction.phi() );

      const int dTheta_2b = (int)( (theta - stringFirstTheta) / m_persAngUnit + m_2bHalfMaximum + 0.5 );
      const int dPhi_2b = (int)( (cycle(phi, stringFirstPhi) - stringFirstPhi) / m_persAngUnit + m_2bHalfMaximum + 0.5 );

      if ( dTheta_2b < m_2bMaximum && dTheta_2b >= 0 && dPhi_2b < m_2bMaximum && dPhi_2b >= 0) {
        persCont->m_dTheta.push_back(dTheta_2b);
        persCont->m_dPhi.push_back(dPhi_2b);
        theta = stringFirstTheta + ( (double)dTheta_2b - m_2bHalfMaximum ) * m_persAngUnit;
        phi = stringFirstPhi + ( (double)dPhi_2b - m_2bHalfMaximum ) * m_persAngUnit;
        phi = phicorr(phi);
      }
      else {
        startNewString = true;
      }
    }

    if (startNewString || dRLast >= dRcut || dTLast >= dTcut) {

      // begin new hit string

      direction = CLHEP::Hep3Vector( en.x() - st.x(), en.y() - st.y(), en.z() - st.z() );

      theta = direction.theta();
      phi = phicorr( direction.phi() );

      persCont->m_hit1_meanTime.push_back(t);
      persCont->m_hit1_x0.push_back(st.x());
      persCont->m_hit1_y0.push_back(st.y());
      persCont->m_hit1_z0.push_back(st.z());
      persCont->m_hit1_theta.push_back(theta);
      persCont->m_hit1_phi.push_back(phi);

      lastPersEnd = HepGeom::Point3D<double>(st.x(), st.y(), st.z());

      stringFirstTheta = theta;
      stringFirstPhi = phi;

      if (idx > 0) {
        persCont->m_nHits.push_back(idx - endHit);
        endHit = idx;
      }
    }

    lastTransEnd = HepGeom::Point3D<double>(en.x(), en.y(), en.z());
    transSumE += siHit->energyLoss();

    const int eneLoss_2b = (int)((transSumE - persSumE) / m_persEneUnit + 0.5);  // calculated to allow recovery sum over
                                                                                 // whole hit string to chosen precision

    const int hitLength_2b = (int)(direction.mag() / m_persLenUnit + 0.5);  // calculated to give the correct position to
                                                                            // the chosen precision, NOT the length of the
                                                                            // hit (small difference in practice).
    double eneLoss = 0.0;

    if (eneLoss_2b >= m_2bMaximum) {
      eneLoss = siHit->energyLoss();
      persCont->m_hitEne_2b.push_back(m_2bMaximum);
      persCont->m_hitEne_4b.push_back(eneLoss);
    }
    else {
      eneLoss = eneLoss_2b * m_persEneUnit;
      persCont->m_hitEne_2b.push_back(eneLoss_2b);
    }

    double length = 0.0;

    if (hitLength_2b >= m_2bMaximum) {
      length = direction.mag();
      persCont->m_hitLength_2b.push_back(m_2bMaximum);
      persCont->m_hitLength_4b.push_back(direction.mag());
    }
    else {
      length = hitLength_2b * m_persLenUnit;
      persCont->m_hitLength_2b.push_back(hitLength_2b);
    }

    CLHEP::Hep3Vector persDir(length, 0.0, 0.0);
    persDir.setTheta(theta);
    persDir.setPhi(phi);

    lastPersEnd = (CLHEP::Hep3Vector)lastPersEnd + persDir;
    persSumE += eneLoss;
    lastT = t;

    ++idx;
  }

  persCont->m_nBC.push_back(idx - endBC);
  persCont->m_nId.push_back(idx - endId);
  persCont->m_nHits.push_back(idx - endHit);
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

        HepMcParticleLink partLink( persCont->m_barcode[idxBC], persCont->m_mcEvtIndex[idxBC], HepMcParticleLink::ExtendedBarCode::eventCollectionFromChar(persCont->m_evtColl[idxBC]), HepMcParticleLink::IS_INDEX );
        transCont->Emplace( endLast, endThis, eneLoss, meanTime, partLink, persCont->m_id[idxId]);

        endLast = endThis;

        ++hitCount;
        if (j > start) ++angleCount;
      }
    }
  }
}
