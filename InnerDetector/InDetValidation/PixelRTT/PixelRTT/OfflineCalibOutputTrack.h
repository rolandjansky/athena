/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OfflineCalibOutputTrack_h
#define OfflineCalibOutputTrack_h

#include "PixelCalibAlgs/PixelNtupleTracks.h"

class string;

namespace PixelCalib{

class PixelChargeInterpolationCalibration;
class PixelChargeInterpolationValidation;

}

namespace PixelValid{

class ResPullValidation;

class OfflineCalibOutputTrack: public PixelNtupleTracks {

  //forbid copying
  OfflineCalibOutputTrack(const OfflineCalibOutputTrack &) = delete;
  OfflineCalibOutputTrack &operator=(const OfflineCalibOutputTrack &) = delete;

public :
	// Modified stuff
	OfflineCalibOutputTrack(std::string input,
				std::string output,
				std::string collection = "Validation/CombinedInDetTracks");
	virtual ~OfflineCalibOutputTrack();
	void Iterate(long maxentries);
	void ReadHistoFile(std::string input);
	void Analyse(std::string output, std::string reference = "");


private:
	// My variables
	PixelCalib::PixelChargeInterpolationCalibration *m_Calibration;
	PixelCalib::PixelChargeInterpolationValidation *m_Validation;
	ResPullValidation *ResPhiValid, *ResEtaValid, *PullPhiValid, *PullEtaValid;
   
};

}

#endif
