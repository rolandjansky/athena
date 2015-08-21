/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKFIRMWAREOUTPUT_H
#define FTKFIRMWAREOUTPUT_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include "FTKRoad.h"
#include "FTKTrack.h"
#include "FTKConstantBank.h"

class 
FTKFirmwareOutput 
{
  
 private:
  ofstream *m_roadhits;
  ofstream *m_sectors;
  ofstream *m_wenable;
  ofstream *m_constants;
  ofstream *m_constantbank;
  ofstream *m_tracks;
  ofstream *m_besttracks;
  
 private:
  void writeTrack( const FTKTrack& track , ofstream *stream ) {
    (*stream) << track.getHalfInvPt() << " " << track.getIP() << " " << track.getPhi() << " " << track.getZ0() << " " << track.getCotTheta() << " " << track.getChi2() << std::endl;
  }
  void writeConstants( FTKConstantBank *bank , int isec , ofstream *stream ) {
    for( int ipar = 0 ; ipar < bank->getNPars() ; ++ipar ) {
      for( int icoord = 0 ; icoord < bank->getNCoords() ; ++icoord ) (*stream) << bank->getFitPar(isec,ipar,icoord) << " ";
      (*stream) << bank->getFitConst(isec,ipar) << std::endl;
    }
    for( int iconstr = 0 ; iconstr < bank->getNConstr() ; ++iconstr ) {
      for( int icoord = 0 ; icoord < bank->getNCoords() ; ++icoord ) (*stream) << bank->getKernel(isec,iconstr,icoord) << " ";
      (*stream) << bank->getKaverage(isec,iconstr) << std::endl;
    }
  }

 public:
  FTKFirmwareOutput() {
    m_roadhits = new ofstream();
    m_sectors = new ofstream();
    m_wenable = new ofstream();
    m_constants = new ofstream();
    m_constantbank = new ofstream();
    m_tracks = new ofstream();
    m_besttracks = new ofstream();
  }
  ~FTKFirmwareOutput() {
    delete m_roadhits;
    delete m_sectors;
    delete m_wenable;
    delete m_constants;
    delete m_constantbank;
    delete m_tracks;
    delete m_besttracks;
  }
  
  // RoadFinder output
  void beginTrackFitterFiles() {
    m_roadhits->open( "firmware_roads.dat" );
    m_sectors->open( "firmware_sectors.dat" );
    m_wenable->open( "firmware_wenable.dat" );
    m_constants->open( "firmware_constants.dat" );
    m_constantbank->open( "firmware_constantbank.dat" );
    m_tracks->open( "firmware_tracks.dat" );
    m_besttracks->open( "firmware_besttracks.dat" );
  }
  void endTrackFitterFiles() {
    m_roadhits->close();
    m_sectors->close();
    m_wenable->close();
    m_constants->close();
    m_constantbank->close();
    m_tracks->close();
    m_besttracks->close();
  }

  void beginRoad( int ientry , int region = -1 , int subreg = -1 , int sector = -1 , int roadid = -1 ) {
    (*m_sectors) << "# BOR " << ientry << " " << region << " " << subreg << " " << sector << " " << roadid << std::endl;
    (*m_roadhits) << "# BOR " << ientry << " " << region << " " << subreg << " " << sector << " " << roadid << std::endl;
    (*m_wenable) << "# BOR " << ientry << " " << region << " " << subreg << " " << sector << " " << roadid << std::endl; 
    (*m_constants) << "# BOR " << ientry << " " << region << " " << subreg << " " << sector << " " << roadid << std::endl;
    (*m_tracks) << "# BOR " << ientry << " " << region << " " << subreg << " " << sector << " " << roadid << std::endl;
    (*m_besttracks) << "# BOR " << ientry << " " << region << " " << subreg << " " << sector << " " << roadid << std::endl;
  }

  void endRoad( int roadid = -1 ) {
    /*
    (*m_sectors) << "# EOR " << roadid << std::endl;
    (*m_roadhits) << "# EOR " << roadid << std::endl;
    (*m_wenable) << "# EOR " << roadid << std::endl; 
    (*m_constants) << "# EOR " << roadid << std::endl;
    (*m_tracks) << "# EOR " << roadid << std::endl;
    (*m_besttracks) << "# EOR " << roadid << std::endl;
    */
  }

  void addConstants( FTKConstantBank *bank , int isector ) {
    writeConstants( bank , isector , m_constants );
  }

  void addConstantBank( FTKConstantBank *bank , int region , int subreg , int nsectors ) {
    for( int isec = 0 ; isec < nsectors && isec < bank->getNSectors() ; ++isec ) {
      (*m_constants) << region << " " << subreg << " " << isec << std::endl;
      writeConstants( bank , isec , m_constantbank );
    }
  }

  void addFullRoad( const FTKRoad& road ) {
    // loop over layers until out of hits
    bool out_of_hits = false;
    unsigned int hit_iter = 0;
    while( !out_of_hits ) {
      (*m_sectors) << road.getSectorID() << std::endl;
      out_of_hits = true;
      for( int iplane = 0 ; iplane < road.getNPlanes() ; iplane++ ) {
	if( hit_iter < road.getNHits(iplane) ) {
	  out_of_hits = false;
	  (*m_wenable) << "1 ";
	  for( int idim = 0 ; idim < road.getHit(iplane,hit_iter).getDim() ; idim++ ) {
	    (*m_roadhits) << road.getHit(iplane,hit_iter).getCoord(idim) << " ";
	  }
	} else  {
	  if( hit_iter == road.getNHits(iplane) ) (*m_wenable) << "1 ";
	  else (*m_wenable) << "0 ";
	  for( int idim = 0 ; idim < road.getHit(iplane,0).getDim() ; idim++ ) {
	    (*m_roadhits) << "EOH ";
	  }
	}
      }
      (*m_roadhits) << std::endl;
      (*m_wenable) << std::endl;
      hit_iter++;
    }
  }

  void addTrack( const FTKTrack& track ) { writeTrack( track , m_tracks ); }
  void addBestTrack( const FTKTrack& track ) { writeTrack( track , m_besttracks ); }

};

#endif
