/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ForwardTracker/TwissFile.h"
#include "ForwardTracker/ForwardTrackerConstants.h"
#include "ForwardTracker/STLHelpers.h"

#include <fstream>
#include <iomanip>

namespace ForwardTracker {
 
  boost::shared_ptr<std::ifstream> TwissFile(const ConfigData& cData, const Side& side) {
    
    if (side != beam1 && side != beam2) throw std::runtime_error("Allowed side values are beam1 or beam2.");
    
    std::string fn = (side == beam1) ? cData.twissFile1 : cData.twissFile2;
    
    boost::shared_ptr<std::ifstream> twissFile(new std::ifstream);
    
    ForwardTracker::openFile(fn, twissFile);

    ForwardTracker::TwissFilePrint(*twissFile, cData.endMarker);

    return twissFile;
  }
  
  void TwissFilePrint(std::ifstream& pfile, double endMarker) {
    
    std::ios::fmtflags cout_flags = std::cout.flags();
 
    std::cout << std::left
	      << std::setw(15) << "NAME" 
	      << std::setw(15) << "KEYWORD" 
	      << std::right
	      << std::setw( 8) << "S"
	      << std::setw( 8) << "L"
	      << std::setw( 8) << "BETX"
	      << std::setw( 8) << "BETY"
	      << std::setw(10) << "HKICK"
	      << std::setw(10) << "VKICK"
	      << std::setw(10) << "K0L"
	      << std::setw(10) << "K1L"
	      << std::setw(10) << "K2L"
	      << std::setw(10) << "K3L"
	      << std::setw(15) << "APERTYPE"
	      << std::setw( 8) << "APER_1"
	      << std::setw( 8) << "APER_2"
	      << std::setw( 8) << "APER_3"
	      << std::setw( 8) << "APER_4"
	      << std::endl;
    
    while (pfile.peek() == '@' || pfile.peek() == '*' || pfile.peek() == '$') pfile.ignore(1000, '\n');

    while (true) {
      
      std::string NAME, KEYWORD, PARENT;
      double      ENDPOS, LENGTH;
      double      HKICK, VKICK;
      double      K0L, K1L, K2L, K3L;
      double      X, PX, BETX, ALFX, MUX, DX, DPX;
      double      Y, PY, BETY, ALFY, MUY, DY, DPY;
      std::string APERTYPE;
      double      APER_1, APER_2, APER_3, APER_4;
      
      pfile >> NAME >> KEYWORD >> PARENT >> ENDPOS >> LENGTH
	    >> HKICK >> VKICK 
	    >> K0L >> K1L >> K2L >> K3L
	    >>    X >>    Y >>   PX >>   PY 
	    >> BETX >> BETY >> ALFX >> ALFY >> MUX >> MUY 
	    >>   DX >>   DY >>  DPX >>  DPY 
	    >> APERTYPE 
	    >> APER_1 >> APER_2 >> APER_3 >> APER_4;
      
      if (ENDPOS > endMarker) break;
      
      if (NAME.find("DRIFT") == 1) continue;
      
      std::cout.precision(2);
      std::cout << std::fixed 
		<< std::left
		<< std::setw(15) << NAME 
		<< std::setw(15) << KEYWORD 
		<< std::right
		<< std::setw( 8) << ENDPOS
		<< std::setw( 8) << LENGTH
		<< std::setw( 8) << BETX
		<< std::setw( 8) << BETY;
      std::cout.precision(6);
      std::cout << std::setw(10) << HKICK
		<< std::setw(10) << VKICK
		<< std::setw(10) << K0L
		<< std::setw(10) << K1L
		<< std::setw(10) << K2L
		<< std::setw(10) << K3L;
      std::cout.precision(4);
      std::cout << std::setw(15) << APERTYPE 
		<< std::setw( 8) << APER_1
		<< std::setw( 8) << APER_2
		<< std::setw( 8) << APER_3
		<< std::setw( 8) << APER_4
		<< std::endl;
    }
    
    std::cout.flags(cout_flags);

    pfile.clear();
    pfile.seekg(0, std::ios::beg);
  }
  
  double GetBeamEnergy(std::ifstream& pfile) {   

    double      beamEnergy = -1;
    std::string dummy; 

    while (pfile >> dummy) { if (dummy == "ENERGY") { pfile >> dummy; pfile >> beamEnergy; break; } }
    
    if (beamEnergy == -1) throw std::runtime_error("Could not read beamEnergy from twiss file");  
    
    pfile.clear();
    pfile.seekg(0, std::ios::beg);

    return beamEnergy;
  }
}
