/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetSimEvent/TRTUncompressedHit.h"
#include "InDetSimEvent/TRTUncompressedHitCollection.h"
#include "InDetSimEventTPCnv/InDetHits/TRT_HitCollection_p3.h"
#include "InDetSimEventTPCnv/InDetHits/TRT_HitCollectionCnv_p3.h"

#include <cmath>

// CLHEP
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/SystemOfUnits.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"

// Athena
#include "StoreGate/StoreGateSvc.h"

// Transient(Geant) to Persistent(Disk)
void TRT_HitCollectionCnv_p3::transToPers(const TRTUncompressedHitCollection* transCont, TRT_HitCollection_p3* persCont, MsgStream &log)
{

  /*
    Spring 2009
    Andrew Beddall - lossy TRT G4hit compression [p3]

    In p1, p2 versions, GEANT hits are persistified on disk as floats.
    In this p3 version, floats are compressed to "integers"/"short-floats" before persistifying.
    The saving is about 75%; see http://cern.ch/beddall/TRThitCompression/

    Spring 2008
    Rob Duxfield - lossless TRT G4hit compression [p2]

    Finds hits belonging to a "string" (in which the end point of one hit is
    the same as the start point of the next) and persistifies the end point
    of each hit plus the start point of the first hit in each string.
  */

  //  The original units from the hit simulation are indicated in comments;
  //  they are all in CLHEP units except for hitEne which is in keV.
  //  I sometimes make use of CLHEP scales *CLHEP::mm and *CLHEP::ns (both=1) for clarity (I hope!).
  //  See also https://twiki.cern.ch/twiki/bin/view/Atlas/TrtSoftware#Production_of_Hits

  static const double dRcut = 1.0e-7*CLHEP::mm;
  static const double dTcut = 1.0*CLHEP::ns; // redundant?

  //    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "In TRT_HitCollectionCnv_p3::transToPers()" << endmsg;

  int lastBarcode = -1;
  int lastId = -1;
  double lastT = 0.0*CLHEP::ns;
  unsigned int idx = 0;
  unsigned int endBC = 0;
  unsigned int endId = 0;
  unsigned int endHit = 0;
  HepGeom::Point3D<double> lastEnd(0.0, 0.0, 0.0); // mm

  for (TRTUncompressedHitCollection::const_iterator it = transCont->begin(); it != transCont->end(); ++it) {

    //   const TRTUncompressedHit* trtHit = *it;
    TRTUncompressedHitCollection::const_iterator trtHit = it;

    if ( trtHit->particleLink().barcode() != lastBarcode  ||  idx - endBC > 65500) {   // max unsigned short =  65535;
      // store barcode once for set of consecutive hits with same barcode
      lastBarcode = trtHit->particleLink().barcode();
      persCont->m_barcode.push_back(lastBarcode);
      if ( idx > 0 ) {
        persCont->m_nBC.push_back(idx - endBC);
        endBC = idx;
      }
    }

    if ( (int)trtHit->GetParticleEncoding() != lastId || idx - endId >  65500) { // max unsigned short =  65535;
      // store id once for set of consecutive hits with same id
      lastId = trtHit->GetParticleEncoding();
      persCont->m_id.push_back(lastId);
      if ( idx > 0 ) {
        persCont->m_nId.push_back(idx - endId);
        endId = idx;
      }
    }

    const HepGeom::Point3D<double> hitStart(trtHit->GetPreStepX(), trtHit->GetPreStepY(), trtHit->GetPreStepZ()); // mm

    const double meanTime = trtHit->GetGlobalTime(); // ns  // Time of flight from the I.P. to the center of the hit.
    const double dTLast = fabs(meanTime - lastT);      // |d(meantime)| between the previous hit and the current one.
    const double dRLast = lastEnd.distance(hitStart);  // Distance between end of previous hit and start of current one;
    // this is zero if the hit is a continuation of the same particle in the same straw.

    // Begin a new string if the current and previous hits are disconnected;
    // it looks like dTcut is redundant (but not sure about this).
    if ( dRLast >= dRcut || dTLast >= dTcut ) {

      // if ( dRLast < dRcut) std::cout << "AJBdTLastTriggeredNewString " << dRLast << " " << dTLast << std::endl;

      ////////////////////
      // new hit string //
      ////////////////////

      //
      // Persistify string *strawId* using 24 bits.
      // Assumes 0 <= strawId <= 16,777,215 (strawId appears to be < 4,000,000)
      //
      const unsigned int strawId = trtHit->GetHitID();
      persCont->m_strawId1b.push_back(  (unsigned char)(strawId % 256) ); //  8 bits
      persCont->m_strawId2b.push_back( (unsigned short)(strawId / 256) ); // 16 bits
      if ( strawId>16777215 )
        log << MSG::WARNING << "TRT_HitCollectionCnv: strawId > 2^24-1 cannot be persistified correctly! " << endmsg;

      //
      // Persistify string start radius using 1 bit (istartRflag) or 8 bits (startR)
      // Note that the smallest value of R is the wire radius (0.0155 mm)
      //
      // R will be flagged as 2 mm if it is within 0.1 um of the straw wall => max error = 0.1 um,
      // otherwise compress with 8 bits => max error = 3.9 um (0.078 ns), RMS error = 1.1 um (0.022 ns)
      //
      const double startR = sqrt( hitStart.x()*hitStart.x() + hitStart.y()*hitStart.y() ); // mm
      unsigned short istartRflag;
      if ( startR > 1.9999*CLHEP::mm ) {
        istartRflag=1; // persistify as a 1-bit flag
      }
      else {
        istartRflag=0; // compress to 8 bits with a span of 2 mm
        persCont->m_startR.push_back( (unsigned char)(startR/(2.0*CLHEP::mm)*256.0) );
      }

      //
      // Persistify string *startPhi* using 8 bits (min=-pi, max=+pi)
      // Max. error = 12 mrad (< 24 um, 0.48 ns); RMS error = 7 mrad (< 14 um, 0.28 ns)
      //
      const double startPhi = atan2( hitStart.y(), hitStart.x() ); // returns range -pi to +pi rad
      persCont->m_startPhi.push_back( (unsigned char)( (startPhi+M_PI)/(2.0*M_PI)*256.0 ) );

      //
      // Persistify *startZ* using a 4 bits (min = -365 mm, max= +365 mm)
      // Max. error = 25 mm (25e-3/(0.75c) = 0.111 ns * 2 reflect = 0.222 ns)
      // RMS  error = 14 mm (14e-3/(0.75c) = 0.062 ns * 2 reflect = 0.124 ns)
      // Also the 1-bit *istartRflag* is packed into this variable.
      //
      // Note:
      // In the digi code we need to allow for something like 22.5 mm outside straw.
      // Also because we have short straws,
      // short straws are about < +-180 mm, long straws are about < +-350 mm
      // The following compressions can give a large "out of straw" value;
      // *don't* use these: (2.0), 32.0, 128.0, 256.0.

      unsigned char istartZ = (unsigned char)( (hitStart.z()+365.0*CLHEP::mm)/(730.0*CLHEP::mm)*16.0 );
      istartZ = (istartZ << 1) | istartRflag;
      persCont->m_startZ.push_back( istartZ );

      if ( idx > 0 ) {
        persCont->m_nHits.push_back( idx - endHit );
        endHit = idx;
      }
      /*
      // Validation output
      std::cout.precision(15);
      std::cout << "AJBTtoPstrawId "    << strawId      << std::endl;
      std::cout << "AJBTtoPstartR "     << startR       << std::endl;
      std::cout << "AJBTtoPstartPhi "   << startPhi     << std::endl;
      std::cout << "AJBTtoPstartX "     << hitStart.x() << std::endl;
      std::cout << "AJBTtoPstartY "     << hitStart.y() << std::endl;
      std::cout << "AJBTtoPstartZ "     << hitStart.z() << std::endl;
      */
    } // end of "begin new hit string"

    //////////////////////////
    // Now for the end hits //
    //////////////////////////

    const HepGeom::Point3D<double> hitEnd(trtHit->GetPostStepX(), trtHit->GetPostStepY(), trtHit->GetPostStepZ()); // mm
    const HepGeom::Point3D<double> hitLength = (hitEnd - hitStart);

    //
    // Here both *kinEne* (kinetic energy of the particle causing the hit) and
    // *steplength* (g4hit length) are persistified using a 15-bit "short float"
    // (9 bit unsigned mantissa, 6 bit unsigned exponent).
    // This stores values in the range 0.51*2^0 = 0.51 to 1.00*2^63 = 9.2e18.
    // I enforce the limits 1.0 and 9.0e18; see below.
    // Max relative error = 0.0010, RMS = 0.0004
    //
    // Notes:
    //
    //  - G4 gives kinEne in MeV; I sometimes see values ~ 1e-7 MeV (100 meV) [float round-off?]
    //    So I multiply by 1e9 and store in units of meV => range 1.0 meV to 9.0e18 meV (9000 TeV!)
    //  - About 1 in 10000 hits have steplength ~ 1e-7 mm [float round-off?]
    //    so again I multiply by 1e9 and store in units of pm => range 1.0 pm to 9.0e18 pm (9000 km)
    //  - The mantissa has maximum 9 bits, the exponent has maximum 6 bits,
    //    Note: a rare condition causes an 10-bit mantissa (mantissa=512).
    //
    double kinEne = trtHit->GetKineticEnergy()    * 1.0e9;  // nano Mev = meV.
    double steplength = hitLength.distance() * 1.0e9;  // nano mm  = pm.
    if ( kinEne     < 1.0 )        kinEne=1.0;         // Keep  the value
    if ( steplength < 1.0 )    steplength=1.0;         // well within the
    if ( kinEne     > 9.0e18 )     kinEne=9.0e18;      // range  of   the
    if ( steplength > 9.0e18 ) steplength=9.0e18;      // short    float.
    const unsigned int kexponent = (unsigned int)ceil(log10(kinEne)/0.30102999566398);
    const unsigned int sexponent = (unsigned int)ceil(log10(steplength)/0.30102999566398);
    const unsigned int kmantissa = (unsigned int)(kinEne/pow(2.0,kexponent)*1024) - 512;
    const unsigned int smantissa = (unsigned int)(steplength/pow(2.0,sexponent)*1024) - 512;
    persCont->m_kinEne.push_back(     (kmantissa << 6) | kexponent );
    persCont->m_steplength.push_back( (smantissa << 6) | sexponent );

    //
    // Persistify hit end radius using 1 bit (iendRflag) or 8 bits (endR).
    // Note that the smallest value of R is the wire radius (0.0155 mm)
    //
    // R will be flagged as 2 mm if it is within 0.1 um of the straw wall => max error = 0.1 um,
    // otherwise compress with 8 bits. The errors are as for startR, but can increased greatly
    // after steplength preservation in PtoT.
    //
    const double endR = sqrt( hitEnd.x()*hitEnd.x() + hitEnd.y()*hitEnd.y() ); // mm
    unsigned short iendRflag;
    if ( endR > 1.9999*CLHEP::mm ) {
      iendRflag=1; // persistify as a 1-bit flag
    }
    else {
      iendRflag=0; // compress to 8 bits with a span of 2 mm
      persCont->m_endR.push_back( (unsigned char)(endR/(2.0*CLHEP::mm)*256.0) );
    }

    //
    // Persistify string *endPhi* using 8 bits (min=-pi, max=+pi)
    // The errors are as for startPhi, but are very different after steplength
    // preservation in PtoT.
    //
    const double endPhi = atan2( hitEnd.y(), hitEnd.x() ); // returns range -pi to +pi rad
    persCont->m_endPhi.push_back( (unsigned char)( (endPhi+M_PI)/(2.0*M_PI)*256.0 ) );

    //
    // Persistify hit *meanTime* using 10 bits (min=0.,span=75 ns)
    // with float overflow for meanTime >= 75ns (the tail of the distribution).
    // Max. error = 0.037 ns; RMS error = 0.021 ns.
    // Also the 1-bit *iendRflag* and 1-bit *idZsign* are packed into this variable.
    //
    unsigned short idZsign = (hitLength.z()>0.0) ? 1 : 0; // flag the sign of dZ
    unsigned short imeanTime = ( meanTime < 75.0*CLHEP::ns ) ? (unsigned short)(meanTime/(75.0*CLHEP::ns)*1024.0) : 1023;
    if ( imeanTime == 1023 ) persCont->m_meanTimeof.push_back( (float)meanTime ); // "overflow flag"
    imeanTime = (imeanTime << 2) | (idZsign << 1) | iendRflag;
    persCont->m_meanTime.push_back( imeanTime );

    //
    // Persistify hit *hitEne* (the energy deposited by the hit in keV) using a float but only for photons
    // (relatively very few of these). Digitisation does not use hitEne for charged particles.
    //
    if ( lastId == 22 ||
         (int)(abs(lastId)/100000) == 41 ||
         (int)(abs(lastId)/10000000) == 1
         ) persCont->m_hitEne.push_back( (float)(trtHit->GetEnergyDeposit()) );  // keV

    lastEnd = hitEnd;
    lastT = meanTime;
    ++idx;
    /*
    // Validation output
    std::cout.precision(15);
    std::cout << "AJBTtoPendR "     << endR                  << std::endl;
    std::cout << "AJBTtoPendPhi "   << endPhi                << std::endl;
    std::cout << "AJBTtoPendX "     << hitEnd.x()            << std::endl;
    std::cout << "AJBTtoPendY "     << hitEnd.y()            << std::endl;
    std::cout << "AJBTtoPendZ "     << hitEnd.z()            << std::endl;
    std::cout << "AJBTtoPmeanTime " << meanTime              << std::endl;
    std::cout << "AJBTtoPkinEne "   << trtHit->kineticEnergy << std::endl;
    std::cout << "AJBTtoPhitEne "   << trtHit->energyDeposit << std::endl;
    std::cout << "AJBTtoPsteplength " << hitLength.distance() << std::endl;
    */
  }

  persCont->m_nBC.push_back(idx - endBC);
  persCont->m_nId.push_back(idx - endId);
  persCont->m_nHits.push_back( idx - endHit );

} // transToPers


// Create Transient
TRTUncompressedHitCollection* TRT_HitCollectionCnv_p3::createTransient(const TRT_HitCollection_p3* persObj, MsgStream &log) {
  std::auto_ptr<TRTUncompressedHitCollection> trans(new TRTUncompressedHitCollection("DefaultCollectionName",persObj->m_nHits.size()));
  persToTrans(persObj, trans.get(), log);
  return(trans.release());
} //createTransient


// Persistent(Disk) to Transient
void TRT_HitCollectionCnv_p3::persToTrans(const TRT_HitCollection_p3* persCont, TRTUncompressedHitCollection* transCont, MsgStream& /*log*/)
{

  //    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "In TRT_HitCollectionCnv_p3::persToTrans()" << endmsg;

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

        HepMcParticleLink partLink(persCont->m_barcode[idxBC]);
        partLink.setExtendedBarCode(HepMcParticleLink::ExtendedBarCode(persCont->m_barcode[idxBC], 0, EBC_MAINEVCOLL, HepMcParticleLink::IS_POSITION));;
        transCont->Emplace( strawId, partLink, persCont->m_id[idxId],
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
