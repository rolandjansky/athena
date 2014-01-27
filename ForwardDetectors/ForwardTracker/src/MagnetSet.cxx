/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ForwardTracker/ConfigData.h"
#include "ForwardTracker/MagnetSet.h"
#include "ForwardTracker/MagnetFactory.h"
#include "ForwardTracker/STLHelpers.h"
#include "ForwardTracker/ForwardTrackerConstants.h"
#include "ForwardTracker/TwissFile.h"
#include "ForwardTracker/TransverseShift.h"

#include <fstream>

namespace ForwardTracker {
  
  Magnet::Container_t MagnetSet(const ConfigData& cData, const Side& side) {    

    Magnet::Container_t magnets; // (x, y, z) = (left, up, forward)

    boost::shared_ptr<std::ifstream> twissFile = TwissFile(cData, side);   
    
    std::ifstream& pfile = *twissFile;

    double beamEnergy = ForwardTracker::GetBeamEnergy(pfile);
    
    std::cout << "beamEnergy[GeV] = " << beamEnergy << std::endl;
    
    while (pfile.peek() == '@' || pfile.peek() == '*' || pfile.peek() == '$') pfile.ignore(1000, '\n');
    
    std::ios::fmtflags cout_flags = std::cout.flags();
 
    std::cout << std::left
	      << std::setw(15) << "magName" 
	      << std::setw(15) << "magSort" 
	      << std::right
	      << std::setw(10) << "magType" 
              << std::setw(15) << "magEndPos"
	      << std::setw(15) << "magLength"
	      << std::setw(15) << "magStrength"
	      << std::setw(15) << "magAperType"
	      << std::setw(10) << "magAper1"
	      << std::setw(10) << "magAper2"
	      << std::setw(10) << "magAper3"
	      << std::setw(10) << "magAper4"
	      << std::endl;
    
    while (true) { // Read the magnet data from twiss_b1.txt for beam 1 (side 1) and twiss_b2.txt for beam 2 (side 0).
      
      std::string magName, magSort, magBasic;
      double      magEndPos, magLength;
      double      HKICK, VKICK;
      double      K0L, K1L, K2L, K3L;
      double      X, PX, BETX, ALFX, MUX, DX, DPX;
      double      Y, PY, BETY, ALFY, MUY, DY, DPY;
      std::string magAperType;
      double      magAper1, magAper2, magAper3, magAper4;
      
      pfile >> magName >> magSort >> magBasic >> magEndPos >> magLength
	    >> HKICK >> VKICK 
	    >> K0L >> K1L >> K2L >> K3L
	    >>    X >>    Y >>   PX >>   PY 
	    >> BETX >> BETY >> ALFX >> ALFY >> MUX >> MUY 
	    >>   DX >>   DY >>  DPX >>  DPY 
	    >> magAperType 
	    >> magAper1 >> magAper2 >> magAper3 >> magAper4;
      
      if      (magEndPos < 0) throw std::runtime_error("Magnet End Position cannot be negative.");
      else if (magEndPos > cData.endMarker) break;
      
      Magnet::Type magType     = Magnet::notAMagnet;
      double       magStrength = 0;

      if      (K0L   != 0) { magType = Magnet::hbDipole;                                        magStrength = K0L; }
      else if (K1L   != 0) { magType = (K1L > 0) ? Magnet::hfQuadrupole : Magnet::vfQuadrupole; magStrength = K1L; }
      else if (HKICK != 0) { magType = Magnet::hbDipole;                                        magStrength = -HKICK; }
      else if (VKICK != 0) { magType = Magnet::vbDipole;                                        magStrength = +VKICK; } 
      
      if (magType == Magnet::notAMagnet) continue;

      std::cout.precision(6);
      if (magName.find("DRIFT") != 1)
	std::cout << std::fixed 
		  << std::left
		  << std::setw(15) << magName 
		  << std::setw(15) << magSort 
		  << std::right
		  << std::setw(10) << magType
		  << std::setw(15) << magEndPos
		  << std::setw(15) << magLength
		  << std::setw(15) << magStrength
		  << std::setw(15) << magAperType 
		  << std::setw(10) << magAper1
		  << std::setw(10) << magAper2
		  << std::setw(10) << magAper3
		  << std::setw(10) << magAper4
		  << std::endl;

      if      (magLength  < 0)               throw std::runtime_error("magLength must be positive.");
      else if (magLength == 0) {
	if (magType == Magnet::hfQuadrupole) throw std::runtime_error("magLength cannot be zero for hfQuadrupole.");
	if (magType == Magnet::vfQuadrupole) throw std::runtime_error("magLength cannot be zero for vfQuadrupole.");
      }
      
      double z = (magEndPos - 0.5*magLength)*ForwardTrackerConstants::aside[side];
      
      Magnet::ConstPtr_t mptr = MagnetFactory(TransverseShift(z)[0], // account of beam splitting around 155 m
					      TransverseShift(z)[1],
					      z,
					      magStrength,
					      magLength,
					      magAperType,
					      magAper1, 
					      magAper2, 
					      magAper3, 
					      magAper4, 
					      beamEnergy,
					      side,
					      magType);
      
      magnets.push_back(mptr);
    }
    
    std::sort(magnets.begin(), magnets.end(), absZGreater<Magnet::ConstPtr_t>);
    
    std::cout.flags(cout_flags);

    return magnets;
  }
}
