/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetSimEvent/TRTUncompressedHit.h"
#include "InDetSimEvent/TRTUncompressedHitCollection.h"
#include "InDetSimEventTPCnv/InDetHits/TRT_HitCollection_p4.h"
#include "InDetSimEventTPCnv/InDetHits/TRT_HitCollectionCnv_p4.h"

#include <cmath>

// CLHEP
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/SystemOfUnits.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"

// Athena
#include "StoreGate/StoreGateSvc.h"

// Transient(Geant) to Persistent(Disk)
void TRT_HitCollectionCnv_p4::transToPers(const TRTUncompressedHitCollection*, TRT_HitCollection_p4*, MsgStream &/*log*/)
{
  throw std::runtime_error("TRT_HitCollectionCnv_p4::transToPers is not supported in this release!");
} // transToPers


// Create Transient
TRTUncompressedHitCollection* TRT_HitCollectionCnv_p4::createTransient(const TRT_HitCollection_p4* persObj, MsgStream &log) {
  std::auto_ptr<TRTUncompressedHitCollection> trans(new TRTUncompressedHitCollection("DefaultCollectionName",persObj->m_nHits.size()));
  persToTrans(persObj, trans.get(), log);
  return(trans.release());
} //createTransient


// Persistent(Disk) to Transient
void TRT_HitCollectionCnv_p4::persToTrans(const TRT_HitCollection_p4* persCont, TRTUncompressedHitCollection* transCont, MsgStream& /*log*/)
{

  //    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "In TRT_HitCollectionCnv_p4::persToTrans()" << endreq;

  // some values are read less than once per hit, these need counters.
  unsigned int meanTimeofCount=0, startRCount=0, endRCount=0, hitEneCount=0;
  unsigned int idxBC=0, idxId=0, endHit=0, endBC=0, endId=0;

  //
  // loop over strings - index [i]
  //

  for ( unsigned int i = 0; i < persCont->m_nHits.size(); i++ ) {

    if ( persCont->m_nHits[i] ) { // at least one hit in the string

      const unsigned int startHit = endHit;
      endHit += persCont->m_nHits[i];

      //
      // string strawId
      //
      const unsigned int i1 = persCont->m_strawId1b[i]; //  8 bits
      const unsigned int i2 = persCont->m_strawId2b[i]; // 16 bits
      const unsigned int strawId = i2*256+i1;           // => 24 bits (0 to 16,777,215)

      //
      // string startPhi
      //
      const unsigned int istartPhi = persCont->m_startPhi[i];         // 8 bits
      const double startPhi = -M_PI + (istartPhi+0.5)*2.0*M_PI/256.0; // rad (min = -pi, max = +pi)

      //
      // string startZ
      //
      const unsigned int istartZ = persCont->m_startZ[i] >> 1; // 4 bits
      double startZ = -365.0*CLHEP::mm + (istartZ+0.5)*730.0*CLHEP::mm/16.0; // (min = -365 mm, max = +365 mm)

      //
      // start Rflag
      //
      const unsigned int istartRflag = persCont->m_startZ[i] & 1; // 1 bit

      //
      // string startR
      //
      double startR;
      if ( istartRflag == 1 ) {
        startR = 2.0*CLHEP::mm; // 1 bit
      }
      else {
        const unsigned int istartR = persCont->m_startR[startRCount++]; // 8 bits
        startR = (istartR+0.5)*2.0*CLHEP::mm/256.0;                            // (range 0 - 2 mm)
        if ( startR < 0.0155*CLHEP::mm ) startR = 0.0155*CLHEP::mm;                   // The wire radius
      }

      //
      // string startX, startY (derived from R,Phi)
      //
      double startX = startR*cos(startPhi);
      double startY = startR*sin(startPhi);
      /*
      // Validation output
      std::cout.precision(15);
      std::cout << "AJBPtoTstrawId "    << strawId              << std::endl;
      std::cout << "AJBPtoTstartR "     << startR               << std::endl;
      std::cout << "AJBPtoTstartPhi "   << startPhi             << std::endl;
      std::cout << "AJBPtoTstartX "     << startX               << std::endl;
      std::cout << "AJBPtoTstartY "     << startY               << std::endl;
      std::cout << "AJBPtoTstartZ "     << startZ               << std::endl;
      std::cout << "AJBPtoTnHits "      << persCont->m_nHits[i] << std::endl;
      */
      //
      // loop over end hits in the string - index [j]
      //

      for ( unsigned int j = startHit; j < endHit; j++ ) {

        if ( j >= endBC + persCont->m_nBC[idxBC] ) endBC += persCont->m_nBC[idxBC++];
        if ( j >= endId + persCont->m_nId[idxId] ) endId += persCont->m_nId[idxId++];

        //
        // hit meanTime
        //
        const unsigned int imeanTime = persCont->m_meanTime[j] >> 2; // 10 bits
        double meanTime = (imeanTime+0.5)*75.0*CLHEP::ns/1024.0;            // (min = 0.0 ns, max = 75.0 ns)
        if ( imeanTime == 1023 ) meanTime = (double)persCont->m_meanTimeof[meanTimeofCount++]; // ns, 32-bit float overflow

        //
        // dZ sign
        //
        const unsigned int idZsign = (persCont->m_meanTime[j] >> 1 ) & 1; // 1 bit

        //
        // endR flag
        //
        const unsigned int iendRflag =  persCont->m_meanTime[j] & 1;         // 1 bit

        //
        // hit energy deposited in keV (only relevant for photons) 32-bit float
        //
        const double hitEne = ( persCont->m_id[idxId] == 22 ||
                                (int)(abs(persCont->m_id[idxId])/100000) == 41 ||
                                (int)(abs(persCont->m_id[idxId])/10000000) == 1
                                ) ? (double)persCont->m_hitEne[hitEneCount++] : 0.0;

        //
        // hit endPhi (can be modified later during "steplength preservation")
        //
        const unsigned int iendPhi =  persCont->m_endPhi[j];  // 8 bits
        double endPhi = -M_PI + (iendPhi+0.5)*2.0*M_PI/256.0; // rad (min = -pi, max = +pi)

        //
        // string endR (can be modified later during "steplength preservation")
        //
        double endR;
        if ( iendRflag==1 ) {
          endR = 2.0*CLHEP::mm;  // 1 bit
        }
        else {
          const unsigned int iendR = persCont->m_endR[endRCount++];
          endR = (iendR+0.5)*2.0*CLHEP::mm/256.0;          // 8 bits
          if ( endR < 0.0155*CLHEP::mm ) endR = 0.0155*CLHEP::mm; // the wire radius
        }

        //
        // hit endX, endY (derived from R,Phi)
        //
        double endX = endR*cos(endPhi); // can be modified later during "steplength preservation"
        double endY = endR*sin(endPhi); // can be modified later during "steplength preservation"

        // Save the (o)riginal endX, endY values for the next hit start because
        // they might get shrunk to fit the g4 steplength of the current hit.
        double endXo = endX;
        double endYo = endY;

        //
        // g4 step length of the hit, m_steplength, and
        // kinetic energy of the hit, m_kinEne, are both 15-bit short floats.
        // Note: a rare condition causes a 16-bit short float (mantissa=512).
        //
        const int kmantissa = persCont->m_kinEne[j]     >> 6;   // 9 bits (expected)
        const int smantissa = persCont->m_steplength[j] >> 6;
        const int kexponent = persCont->m_kinEne[j]     & 0x3F; // 6 bits
        const int sexponent = persCont->m_steplength[j] & 0x3F;
        const double kinEne = (kmantissa+512.5)/1024 * pow(2.0,kexponent) / 1.0e9; // MeV
        double g4steplength = (smantissa+512.5)/1024 * pow(2.0,sexponent) / 1.0e9; // mm
        if ( idZsign==0 ) g4steplength = -g4steplength;

        //
        // Preserving the steplength of the hit by setting endZ or shrinking dX,dY.
        //
        double dX = endX-startX;
        double dY = endY-startY;
        double dZ;
        double dXY2 = dX*dX+dY*dY;
        double dL2 = g4steplength*g4steplength;
        if ( dL2 > dXY2 ) {         // define dZ such that steplength = g4steplength
          dZ = sqrt(dL2-dXY2);
          if (g4steplength<0.0) dZ=-dZ;
        }
        else {    // dL2 < dXY2   // shrink dX,dY such that dXY = g4steplength
          dX = dX * sqrt(dL2/dXY2); // this includes the cases where dL2=0!
          dY = dY * sqrt(dL2/dXY2);
          dZ = 0.0*CLHEP::mm;
          endX = startX + dX;
          endY = startY + dY;
          //endR = sqrt( endX*endX + endY*endY ); // for validation information
          //endPhi = atan2(endY,endX);            // for validation information
        }
        double endZ = startZ + dZ;
        //dX = endX-startX; // for validation information
        //dY = endY-startY; // for validation information
        /*
        // Validation output
        std::cout.precision(15);
        std::cout << "AJBPtoTendR "     << endR     << std::endl;
        std::cout << "AJBPtoTendPhi "   << endPhi   << std::endl;
        std::cout << "AJBPtoTendX "     << endX     << std::endl;
        std::cout << "AJBPtoTendY "     << endY     << std::endl;
        std::cout << "AJBPtoTendZ "     << endZ     << std::endl;
        std::cout << "AJBPtoTmeanTime " << meanTime << std::endl;
        std::cout << "AJBPtoTkinEne "   << kinEne   << std::endl;
        std::cout << "AJBPtoThitEne "   << hitEne   << std::endl;
        std::cout << "AJBPtoTsteplength " << sqrt(dX*dX+dY*dY+dZ*dZ) << std::endl;
        */
        //
        // Notes:
        // - All units are CLHEP, except hitEne which is in keV.
        // - For charged particles kinEne is *zero*!
        //

        transCont->Emplace( strawId, persCont->m_barcode[idxBC], persCont->m_id[idxId],
                            kinEne, hitEne, startX, startY, startZ,
                            endX, endY, endZ, meanTime );
        //
        // End of this hit becomes the start of the next;
        // use the original (uncorrected) values for X,Y
        // but the derived value for Z.
        //
        startX = endXo; startY = endYo; startZ = endZ;

      }
    } // nhits>0
  } // straw loop
} // persToTrans
