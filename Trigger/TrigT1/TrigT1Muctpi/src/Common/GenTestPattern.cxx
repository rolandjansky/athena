/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: GenTestPattern.cxx 362102 2011-04-28 13:17:28Z krasznaa $

// STL include(s):
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>

// TrigT1 include(s):
#include "TrigT1Interfaces/Lvl1MuCTPIInput.h"

// Local include(s):
#include "GenTestPattern.h"
#include "../Mioct/SectorID.h"
#include "MuonCandidate.h"
#include "EventID.h"

/*******************************************************************
 * $Date: 2011-04-28 15:17:28 +0200 (Thu, 28 Apr 2011) $ 
 *
 * Implementation of class GenTestPattern
 * @author   Author: Thorsten Wengler
 * @version $Revision: 362102 $
 ******************************************************************/

namespace LVL1MUCTPI {

  // Constructor of the GenTestPattern class
  GenTestPattern::GenTestPattern( LVL1MUONIF::Lvl1MuCTPIInput* theLvl1Input ) {

    m_input = theLvl1Input;
    //  initialize random number generator
    srand( static_cast< unsigned int >( time( 0 ) ) );

  }

  // method to directly fill sectors
  void GenTestPattern::fillSectorDirect( const MuonCandidate& muonCand ) {

    // just call the fill function with all the separat arguments
    this->fillSectorDirect( muonCand.secID(), muonCand.bcid(), muonCand.roi1(), muonCand.roi2(),
                            muonCand.ovl1(), muonCand.ovl2(), muonCand.pt1(), muonCand.pt2(),
                            muonCand.twoCand1(), muonCand.twoCand2(), muonCand.gtTwoCandInSec() );
    return;

  }


  // method to directly fill sectors with candidates read from a file
  void GenTestPattern::fillEventFromFile( std::string fileName ) {

    std::ifstream fileStream( fileName.c_str() );
    if ( ! fileStream ) {
      std::cout << "Could not open : " << fileName << std::endl;
      std::cout << "==> no candidates ! " << std::endl;
      exit( -1 );
    }

    unsigned int sysID; 
    unsigned int subSysID; 
    unsigned int SectorNo; 
    SectorID id;  
    unsigned int bcid; 
    unsigned int roi1; 
    unsigned int roi2; 
    unsigned int ovl1; 
    unsigned int ovl2; 
    unsigned int pt1; 
    unsigned int pt2; 
    bool twoCand1; 
    bool twoCand2; 
    bool twoCandinSector;

    while ( ! fileStream.eof() ) {

      fileStream >> sysID;
      fileStream >> subSysID >> SectorNo >> bcid >> roi1 >> roi2 >> ovl1 >> ovl2
                 >> pt1  >> pt2 >> twoCand1 >> twoCand2 >> twoCandinSector;

      if( fileStream.eof() ) break;
      this->fillSectorDirect( id.ret( sysID, subSysID, SectorNo), bcid, roi1, roi2,
                              ovl1, ovl2, pt1, pt2, twoCand1, twoCand2, twoCandinSector );

      std::cout << sysID << " " << subSysID << " " << SectorNo << " "
                << bcid << " " << roi1 << " " << roi2 << " "
                << ovl1 << " " << ovl2 << " " << pt1 << " " << pt2 << " "
                << twoCand1 << " " << twoCand2 << " " << twoCandinSector << std::endl;

    }

    return;

  }

  // method to directly fill sectors
  void GenTestPattern::fillSectorDirect( SectorID theSectorID, unsigned int bcid, unsigned int roi1, unsigned int roi2,
                                         unsigned int ovl1, unsigned int ovl2, unsigned int pt1, unsigned int pt2,
                                         bool twoCand1, bool twoCand2, bool twoCandInSec ) {

    unsigned int systemAddress = theSectorID.getDetectorType();
    unsigned int subSystemAddress = theSectorID.getRapidityRegion();
    unsigned int id = theSectorID.getSectorNumber();

    LVL1MUONIF::Lvl1MuSectorLogicData *secIn = 0;
    if( systemAddress == 0 ) {
      secIn = new LVL1MUONIF::Lvl1MuBarrelSectorLogicData();
    } else if( systemAddress == 1 ) {
      secIn = new LVL1MUONIF::Lvl1MuEndcapSectorLogicData();
    } else if( systemAddress == 2 ) {
      secIn = new LVL1MUONIF::Lvl1MuForwardSectorLogicData();
    } else {
      std::cerr << "\"systemAddress\" is in non-defined state." << std::endl;
      std::cerr << "There is very serious error with the code." << std::endl;
      exit( -1 );
    }

    secIn->bcid( bcid );
    secIn->roi( 0, roi1 );
    secIn->roi( 1, roi2 );
    secIn->ovl( 0, ovl1 );
    secIn->ovl( 1, ovl2 );
    secIn->pt( 0, pt1 );
    secIn->pt( 1, pt2 );

    if( twoCand1 ) secIn->set2candidates( 0 );
    else secIn->clear2candidates( 0 );

    if( twoCand2 ) secIn->set2candidates( 1 );
    else secIn->clear2candidates( 1 );

    if( twoCandInSec ) secIn->set2candidatesInSector();
    else secIn->clear2candidatesInSector();

    m_input->setSectorLogicData( *secIn, systemAddress, subSystemAddress, id );

    delete secIn;
    return;

  }

  // Generate a random number
  unsigned int GenTestPattern::randomNumber( unsigned int lower, unsigned int upper ) {

    if( lower > upper ) std::swap( lower, upper );
    double range = upper - lower + 1;

    // I already had a lot of trouble with this peace of code,
    // so now I write out everything explicitly.
    return ( lower + static_cast< unsigned int >( static_cast< double >( range * rand() ) / static_cast< double >( RAND_MAX + 1.0 ) ) );

  }

  // Generate candidates randomly
  MuonCandidate GenTestPattern::randomCandidate() {

    MuonCandidate rndCandidate;

    // first figure out which of Barrel, Endcap, Forward is allowed,
    // choose one randomly
    unsigned int systemID = 0;
    unsigned int sysAllowed = this->rndRange().allowSystems();
    if( ( sysAllowed > 7 ) || ( sysAllowed < 1 ) ) sysAllowed = 7;
    if( sysAllowed >= 7 ) {     // all three subsystems allowed
      systemID = this->randomNumber( 0, 2 );
    } else if ( ( sysAllowed > 2 ) && ( sysAllowed != 4 ) ) { // 2 out of three
      systemID = this->randomNumber( 0, 1 );           // Barrel or Endcap
      if( sysAllowed == 5 ) systemID = systemID * 2;   // Barrel or Forward
      if( sysAllowed == 6 ) ++systemID;                // Endcap or Forward
    } else { // only one Subsystem
      if( sysAllowed == 1 ) systemID = 0; // Barrel
      if( sysAllowed == 2 ) systemID = 1; // Endcap
      if( sysAllowed == 4 ) systemID = 2; // Forward
    }

    // Now choose the Hemisphere
    unsigned int theHem;
    unsigned int hemAllowed = this->rndRange().allowHemispheres();
    if( ( hemAllowed < 1 ) || ( hemAllowed > 3 ) ) hemAllowed = 3;
    if( hemAllowed == 3 ) {
      theHem = this->randomNumber( 0, 1 ); // both Hemispheres
    } else {
      theHem = hemAllowed - 1;
    }

    // Now we can make all choices that depend on the system
    unsigned int minRoI = 0;
    unsigned int maxRoI = 0;
    unsigned int minSecNumber = 0;
    unsigned int maxSecNumber = 0;
    if( systemID == 0 ) {
      minSecNumber = this->rndRange().minBarrelSectorID();
      maxSecNumber = this->rndRange().maxBarrelSectorID();
      minRoI = this->rndRange().minBarrelRoI();
      maxRoI = this->rndRange().maxBarrelRoI();
    } else if( systemID == 1 ) {
      minSecNumber = this->rndRange().minEndcapSectorID();
      maxSecNumber = this->rndRange().maxEndcapSectorID();
      minRoI = this->rndRange().minEndcapRoI();
      maxRoI = this->rndRange().maxEndcapRoI();
    } else if( systemID == 2 ) {
      minSecNumber = this->rndRange().minForwardSectorID();
      maxSecNumber = this->rndRange().maxForwardSectorID();
      minRoI = this->rndRange().minForwardRoI();
      maxRoI = this->rndRange().maxForwardRoI();
    }


    // Dial the sector number
    unsigned int theSecNumber = 0;
    theSecNumber = this->randomNumber( minSecNumber, maxSecNumber );

    // now set the ID in the Muon candidate
    SectorID id;
    rndCandidate.secID( id.ret( systemID,theHem,theSecNumber ) );

    // Choose Pt for both candidates (need to order them)
    unsigned int firstPt  = this->randomNumber( this->rndRange().minPt(), this->rndRange().maxPt() );
    unsigned int secondPt = this->randomNumber( this->rndRange().minPt(), this->rndRange().maxPt() );
    // order candidates - first is >= second
    if( firstPt < secondPt ) std::swap( firstPt, secondPt );

    // The first candidate -----------------------------------
    rndCandidate.pt1( firstPt );                           //Pt value
    unsigned int firstRoI = this->randomNumber( minRoI, maxRoI );
    rndCandidate.roi1( firstRoI );                       // RoI value
    // The Overlap flag
    unsigned int firstOvl = 0;
    if( systemID == 0 ) {                                          // Barrel
      // the Barrel-Barrel Overlap and Barrel-Endcap Overlap
      if( firstRoI <= 2 || ( firstRoI % 2 != theSecNumber % 2 ) ) firstOvl |= 1;
      if( firstRoI >= 20 ) firstOvl |= 2;
    } else if( systemID == 1 ) {                                   // Endcap
      if( firstRoI <= 7 ) firstOvl |= 1; // Barrel-Endcap Overlap
    } else if( systemID == 2 ) {                                   // Forward
      // no overlap in forward sectors possible
    }
    rndCandidate.ovl1( firstOvl );
    // is it allowed to pretend that there were > 1 candiates in one pad?
    bool twoInPad1 = false;
    if( this->rndRange().allowForceGT1InPad1Flag() == 1 ) { // random
      if( this->randomNumber( 0, 1 ) == 1 ) twoInPad1 = true;
    } else if( this->rndRange().allowForceGT1InPad1Flag() == 2 ) { // always set
      twoInPad1 = true;
    }
    rndCandidate.twoCand1( twoInPad1 );
    // end choosing first candidate

    // Now decide if there will be a second candidate
    bool iWantTwo = false;
    if( this->rndRange().allowForce2Cand() == 1 ) { // one or two randomly
      if( this->randomNumber(0,1) == 1 ) iWantTwo = true;
    } else if( this->rndRange().allowForce2Cand() == 2 ) { // always 2
      iWantTwo = true;
    }

    if( iWantTwo ) {

      // The second candidate -----------------------------------
      rndCandidate.pt2( secondPt );                           //Pt value
      unsigned int secondRoI;   // second RoI (should be diff. from first)
      while( ( secondRoI = this->randomNumber( minRoI, maxRoI ) ) == firstRoI )
        ;
      rndCandidate.roi2( secondRoI );                       // RoI value
      // The Overlap flag
      unsigned int secondOvl = 0;
      if( systemID == 0 ) {                                          // Barrel
	// the Barrel-Barrel Overlap and Barrel-Endcap Overlap
	if( secondRoI <= 2 || ( secondRoI % 2 != theSecNumber % 2 ) ) secondOvl |= 1;
	if( secondRoI >= 20 ) secondOvl |= 2;
      } else if( systemID == 1 ) {                                   // Endcap
	if( secondRoI <= 7 ) secondOvl |= 1; // Barrel-Endcap Overlap
      } else if( systemID == 2 ) {                                  // Forward
	// no overlap in forward sectors possible
      }
      rndCandidate.ovl2( secondOvl );
      // is it allowed to pretend that there were > 1 candiates in one pad?
      bool twoInPad2 = false;
      if( this->rndRange().allowForceGT1InPad2Flag() == 1 ) { // random
	if( this->randomNumber( 0, 1 ) == 1) twoInPad2 = true ;
      } else if( this->rndRange().allowForceGT1InPad2Flag() == 2 ) { // always
	twoInPad2 = true;
      }
      rndCandidate.twoCand2( twoInPad2 );

      // since we have two candidates, we can decide if we want to pretend
      // there were more than 2 in this sector
      bool gtTwoInSec = false;
      if( this->rndRange().allowForceGT2InSecFlag() == 1 ) { // random
	if( this->randomNumber( 0, 1 ) == 1 ) gtTwoInSec = true;
      } else if( this->rndRange().allowForceGT2InSecFlag() == 2 ) { // always
	gtTwoInSec = true;
      }
      rndCandidate.gtTwoCandInSec( gtTwoInSec );

    }  // end second candidate

    // set the BCID
    EventID theEventID;            //  get access to the EventID singleton
    rndCandidate.bcid( theEventID.BCID() );

    return rndCandidate;

  }

  // check if a generated candidate already exists in the list
  bool GenTestPattern::notUniqueCandidate( MuonCandidate& muonCand ) {

    bool isNotUnique = false;

    // iterate over the list and check the sector ID's
    if( ! m_candidateList.empty() ){              // is the list still empty?
      std::list< MuonCandidate >::const_iterator it;
      for( it = m_candidateList.begin(); it != m_candidateList.end(); ++it ) {
        if( it->secID() == muonCand.secID() ) isNotUnique = true;
      }
    }

    return isNotUnique;

  }

  // Generate a random event
  void GenTestPattern::randomEvent( unsigned int lowCand, unsigned int highCand ) {

    // get access to the EventID singleton
    EventID theEventID;

    // reset the event holders
    this->clearEvent();

    // get the BCID for this event
    theEventID.BCID( this->randomNumber( this->rndRange().minBCID(), this->rndRange().maxBCID() ) );

    unsigned int numberOfCand = this->randomNumber( lowCand, highCand );

    for( unsigned int i = 0; i < numberOfCand; ++i ) {
      MuonCandidate muonCand;
      do {               // need to check that cand. is unique in this event
        muonCand = this->randomCandidate();
      } while( this->notUniqueCandidate( muonCand ) );
      m_candidateList.push_back( muonCand );
    }

    return;

  }

  // Generate one complete event and fill it into the input class(es)
  void GenTestPattern::generateEvent( unsigned int lowCand, unsigned int highCand ) {

    // generate the Event
    this->randomEvent( lowCand, highCand );

    // fill the event into the Sector logic input class
    if( m_candidateList.empty() ) {              // is the list still empty?
      std::cout << " --- TRIED TO FILL EMPTY EVENT --- " << std::endl;
    } else {
      std::list< MuonCandidate >::iterator it ;
      for( it = m_candidateList.begin(); it != m_candidateList.end(); ++it ) {
        this->fillSectorDirect( (*it) ) ;
      }
    }

    return;

  }

  // print the random event
  void GenTestPattern::printRandomEvent() {

    // iterate over the list and check the sector ID's
    if( m_candidateList.empty() ){              // is the list still empty?
      std::cout << " --- TRIED TO PRINT EMPTY EVENT --- GENERATE ONE FIRST --- " << std::endl;
    } else {
      std::list< MuonCandidate >::const_iterator it;
      for( it = m_candidateList.begin(); it != m_candidateList.end(); ++it ) {
        std::cout << ( *it );
      }
    }

    return;

  }

  // Clear the list of sectors
  void GenTestPattern::clearEvent() {

    m_input->clearAll();
    m_candidateList.clear();

    return;

  }


  // Destructor of the GenTestPattern class - nothing to destruct
  GenTestPattern::~GenTestPattern() {

  }

} // namespace LVL1MUCTPI
