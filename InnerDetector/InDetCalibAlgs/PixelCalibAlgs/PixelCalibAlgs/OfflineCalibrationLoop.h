/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OfflineCalibrationLoop_h
#define OfflineCalibrationLoop_h

#include "PixelCalibAlgs/PixelNtupleTracks.h"

class string;
class TStopwatch;
class TH2F;

namespace PixelCalib{

class PixelChargeInterpolationCalibration;
class PixelChargeInterpolationValidation;


class OfflineCalibrationLoop: public PixelNtupleTracks {

public :
	// Modified stuff
	OfflineCalibrationLoop(std::string starting_tag,
				std::string input,
				std::string final_tag,
				std::string collection = "Validation/CombinedInDetTracks");
	virtual ~OfflineCalibrationLoop();
	void Iterate(long maxentries);
	void ReadHistoFile(std::string input);
	void Analyze(std::string reference, std::string reference_tag);

private:
	// My variables
	TStopwatch *m_timer;
	PixelChargeInterpolationCalibration *m_Calibration;
	PixelChargeInterpolationValidation *m_Validation;

	std::string m_StartingTag;
	std::string m_FinalTag;

};

}
#endif
