/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OfflineCalibrationLoop_h
#define OfflineCalibrationLoop_h

#include "CxxUtils/checker_macros.h"
#include "PixelCalibAlgs/PixelNtupleTracks.h"

class string;
class TStopwatch;
class TH2F;

namespace PixelCalib{

class PixelChargeInterpolationCalibration;
class PixelChargeInterpolationValidation;

class OfflineCalibrationLoop;

class ATLAS_NOT_THREAD_SAFE OfflineCalibrationLoop: public PixelNtupleTracks { // Thread unsafe PixelChargeInterpolationCalibration class is used.

public :
	// Modified stuff
	OfflineCalibrationLoop(const std::string& starting_tag,
                               const std::string& input,
                               const std::string& final_tag,
                               const std::string& collection = "Validation/CombinedInDetTracks");
	virtual ~OfflineCalibrationLoop();
	void Iterate(long maxentries);
	void ReadHistoFile(const std::string& input);
	void Analyze(const std::string& reference, const std::string& reference_tag);

private:

  OfflineCalibrationLoop(const OfflineCalibrationLoop &);
  OfflineCalibrationLoop &operator=(const OfflineCalibrationLoop&);

  // My variables
	TStopwatch *m_timer;
	PixelChargeInterpolationCalibration *m_Calibration;
	PixelChargeInterpolationValidation *m_Validation;

	std::string m_StartingTag;
	std::string m_FinalTag;

};

}
#endif
