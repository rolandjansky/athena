/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetSimEvent/TRTUncompressedHit.h"
#include "InDetSimEvent/TRTUncompressedHitCollection.h"
#include "InDetSimEventTPCnv/InDetHits/TRT_HitCollection_p2.h"
#include "InDetSimEventTPCnv/InDetHits/TRT_HitCollectionCnv_p2.h"

#include <cmath>

// CLHEP
#include "CLHEP/Geometry/Point3D.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"

// Athena
#include "StoreGate/StoreGateSvc.h"





void TRT_HitCollectionCnv_p2::transToPers(const TRTUncompressedHitCollection* transCont, TRT_HitCollection_p2* persCont, MsgStream& /*log*/)
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

  //    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "In TRT_HitCollectionCnv_p2::transToPers()" << endmsg;

  int lastBarcode = -1;
  int lastId = -1;
  double lastT = 0.0;
  unsigned int idx = 0;
  unsigned int endBC = 0;
  unsigned int endId = 0;
  unsigned int endHit = 0;
  HepGeom::Point3D<double> lastEnd(0.0, 0.0, 0.0);

  for (TRTUncompressedHitCollection::const_iterator it = transCont->begin(); it != transCont->end(); ++it) {


    if ( it->particleLink().barcode() != lastBarcode ) {

      // store barcode once for set of consecutive hits with same barcode

      lastBarcode = it->particleLink().barcode();
      persCont->m_barcode.push_back(lastBarcode);

      if (idx > 0) {
        persCont->m_nBC.push_back(idx - endBC);
        endBC = idx;
      }
    }

    if ( (int)it->GetParticleEncoding() != lastId ) {

      // store id once for set of consecutive hits with same id

      lastId = it->GetParticleEncoding();
      persCont->m_id.push_back(lastId);

      if (idx > 0) {
        persCont->m_nId.push_back(idx - endId);
        endId = idx;
      }
    }

    const HepGeom::Point3D<double> hitStart(it->GetPreStepX(), it->GetPreStepY(), it->GetPreStepZ());
    const double t = it->GetGlobalTime();

    const double dRLast = lastEnd.distance(hitStart);  // dR between end of previous hit and start of current one
    const double dTLast = fabs(t - lastT);

    if (dRLast >= dRcut || dTLast >= dTcut) {

      // begin new hit string

      persCont->m_hit1_startX.push_back( hitStart.x() );
      persCont->m_hit1_startY.push_back( hitStart.y() );
      persCont->m_hit1_startZ.push_back( hitStart.z() );

      if (idx > 0) {
        persCont->m_nHits.push_back(idx - endHit);
        endHit = idx;
      }
    }

    persCont->m_hitId.push_back( it->GetHitID() );
    persCont->m_kinEne.push_back( it->GetKineticEnergy() );
    persCont->m_meanTime.push_back( t );

    persCont->m_hitEne.push_back( it->GetEnergyDeposit() );
    persCont->m_endX.push_back( it->GetPostStepX() );
    persCont->m_endY.push_back( it->GetPostStepY() );
    persCont->m_endZ.push_back( it->GetPostStepZ() );

    lastEnd = HepGeom::Point3D<double>(it->GetPostStepX(), it->GetPostStepY(), it->GetPostStepZ());
    lastT = t;
    ++idx;
  }

  persCont->m_nBC.push_back(idx - endBC);
  persCont->m_nId.push_back(idx - endId);
  persCont->m_nHits.push_back(idx - endHit);
}


TRTUncompressedHitCollection* TRT_HitCollectionCnv_p2::createTransient(const TRT_HitCollection_p2* persObj, MsgStream &log) {
  std::auto_ptr<TRTUncompressedHitCollection> trans(new TRTUncompressedHitCollection("DefaultCollectionName",persObj->m_nHits.size()));
  persToTrans(persObj, trans.get(), log);
  return(trans.release());
}


void TRT_HitCollectionCnv_p2::persToTrans(const TRT_HitCollection_p2* persCont, TRTUncompressedHitCollection* transCont, MsgStream& /*log*/)
{
  //    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "In TRT_HitCollectionCnv_p2::persToTrans()" << endmsg;

  unsigned int hitCount = 0;
  unsigned int idxBC = 0;
  unsigned int idxId = 0;
  unsigned int endHit = 0;
  unsigned int endBC = 0;
  unsigned int endId = 0;

  for (unsigned int i = 0; i < persCont->m_nHits.size(); i++) {

    if (persCont->m_nHits[i]) {

      const unsigned int start = endHit;
      endHit += persCont->m_nHits[i];

      HepGeom::Point3D<double> endLast(persCont->m_hit1_startX[i], persCont->m_hit1_startY[i], persCont->m_hit1_startZ[i]);

      for (unsigned int j = start; j < endHit; j++) {

        if (j >= endBC + persCont->m_nBC[idxBC])
          endBC += persCont->m_nBC[idxBC++];

        if (j >= endId + persCont->m_nId[idxId])
          endId += persCont->m_nId[idxId++];

        const double eneLoss = persCont->m_hitEne[hitCount];

        HepGeom::Point3D<double> endThis(persCont->m_endX[j], persCont->m_endY[j], persCont->m_endZ[j]);

        HepMcParticleLink partLink(persCont->m_barcode[idxBC]);
        partLink.setExtendedBarCode(HepMcParticleLink::ExtendedBarCode(persCont->m_barcode[idxBC], 0, EBC_MAINEVCOLL, HepMcParticleLink::IS_POSITION));;
        transCont->Emplace( persCont->m_hitId[hitCount], partLink, persCont->m_id[idxId], persCont->m_kinEne[hitCount],
                            eneLoss, endLast.x(), endLast.y(), endLast.z(), endThis.x(), endThis.y(), endThis.z(),
                            persCont->m_meanTime[hitCount] );

        endLast = endThis;
        ++hitCount;
      }
    }
  }
}
