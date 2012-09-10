/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_CHANCALIBERRORCODE_H
#define TRIGT1CALOCALIBCONDITIONS_CHANCALIBERRORCODE_H

#include "TrigT1CaloCalibConditions/AbstractErrorCode.h"

#include <string>

/**
 *  ChanCalibErrorCode class
 *  for L1Calo error codes
 *  Adapted from /LVL1/l1calo/coolL1Calo/coolL1Calo/ChanErrorCode.h
 *
 *  @author Damien Prieur <damien.prieur@cern.ch>
 */

class ChanCalibErrorCode: public AbstractErrorCode {

	friend std::ostream& operator<<(std::ostream& output, const ChanCalibErrorCode& r);

public:

	enum ChanErrorCodeBits {
		// Bits 0-3 common for all calibrations
		NoDataBit=0,
		BadDataBit=1,
		BadResultBit=2,

		// Specific bits (4-15) for PPM calibrations

		// PPM DAC scan
		DacScanBadSlopeBit=4,
		DacScanBadOffsetBit=5,
		DacScanBadCorrCoeffBit=6,
		DacScanBadAdcDistribBit=7,

		// PPM pedestal run
		PedRunBadMeanBit=4,
		PedRunBadSigmaBit=5,
		PedRunBadAdcDistribBit=6,
      
      // PPM PHOS4 Scan
      Phos4ScanNoSignalBit=4,
      Phos4ScanBadSignalBit=5,
      Phos4ScanBadWidthBit=6,
      Phos4ScanBadRiseTimeBit=7,
      Phos4ScanBadAdcDistribBit=8,
      Phos4ScanBadSignalShapeBit=9,
      Phos4ScanSignalSaturatedBit=10,
      

		// Specific bits (4-15) for CPM calibrations
		// (none defined yet)

		// Specific bits (4-15) for JEM calibrations
		TotalWindowSizeUndercutBit=5,
		DistToErrorRegionUndercutBit=6,
		ExceededDelayBit=7,
		ExceededDelayAfterCorrBit=8

		// Specific bits (4-15) for CMM calibrations
		// (none defined yet)

		// Bits 16-31 are used as copies of 0-3
		// for component calibrations when combined
		// several results into one validated folder.
	};

	enum ChanErrorComponent {
		// NB the maximum allowed value here is 7.
		// Components of PPrChanCalib folder.
		DacScan=0,
		PedestalRun=1,
		CoarseTiming=2,
		Phos4Scan=3,
		SatBcidValues=4,
		FirFilterValues=5,
		LutValues=6
	};


	// Identifies the various sets of specific bits
	// (only used for printing the meaning of the bits).
	enum ChanErrorSet {
		PPM=0,
		CPM=1,
		JEM=2,
		CMM=3
	};

	ChanCalibErrorCode();
	ChanCalibErrorCode(unsigned int errorCode);
	virtual ~ChanCalibErrorCode() {}

	ChanCalibErrorCode(const ChanCalibErrorCode& e);
	ChanCalibErrorCode& operator=(const ChanCalibErrorCode& e);
	ChanCalibErrorCode& operator|=(const ChanCalibErrorCode& e);

	using AbstractErrorCode::errorCode;
	unsigned int errorCode(ChanErrorComponent component) const;

	bool chanValid() const;
	bool chanValid(ChanErrorComponent component) const;

	bool noData() const { return bitIsSet(NoDataBit); }
	void noData(bool nodata) { setBit(NoDataBit,nodata); }

	bool badData() const { return bitIsSet(BadDataBit); }
	void badData(bool bad) { setBit(BadDataBit,bad); }

	bool badResult() const { return bitIsSet(BadResultBit); }
	void badResult(bool bad) { setBit(BadResultBit,bad); }

	bool dacScanBadSlope() const { return bitIsSet(DacScanBadSlopeBit); }
	void dacScanBadSlope(bool bad) { setBit(DacScanBadSlopeBit,bad); }

	bool dacScanBadOffset() const { return bitIsSet(DacScanBadOffsetBit); }
	void dacScanBadOffset(bool bad) { setBit(DacScanBadOffsetBit,bad); }

	bool dacScanBadCorrCoeff() const { return bitIsSet(DacScanBadCorrCoeffBit); }
	void dacScanBadCorrCoeff(bool bad) { setBit(DacScanBadCorrCoeffBit,bad); }

   bool dacScanBadAdcDistrib() const { return bitIsSet(DacScanBadAdcDistribBit); }
   void dacScanBadAdcDistrib(bool bad) { setBit(DacScanBadAdcDistribBit,bad); }

	bool pedRunBadMean() const { return bitIsSet(PedRunBadMeanBit); }
	void pedRunBadMean(bool bad) { setBit(PedRunBadMeanBit,bad); }

	bool pedRunBadSigma() const { return bitIsSet(PedRunBadSigmaBit); }
	void pedRunBadSigma(bool bad) { setBit(PedRunBadSigmaBit,bad); }

	bool pedRunBadAdcDistrib() const { return bitIsSet(PedRunBadAdcDistribBit); }
	void pedRunBadAdcDistrib(bool bad) { setBit(PedRunBadAdcDistribBit,bad); }
   
   bool phos4ScanNoSignal() const { return bitIsSet(Phos4ScanNoSignalBit); }
   void phos4ScanNoSignal(bool bad) { setBit(Phos4ScanNoSignalBit,bad); }

   bool phos4ScanBadSignal() const { return bitIsSet(Phos4ScanBadSignalBit); }
   void phos4ScanBadSignal(bool bad) { setBit(Phos4ScanBadSignalBit,bad); }

   bool phos4ScanBadWidth() const { return bitIsSet(Phos4ScanBadWidthBit); }
   void phos4ScanBadWidth(bool bad) { setBit(Phos4ScanBadWidthBit,bad); }

   bool phos4ScanBadRiseTime() const { return bitIsSet(Phos4ScanBadRiseTimeBit); }
   void phos4ScanBadRiseTime(bool bad) { setBit(Phos4ScanBadRiseTimeBit,bad); }

   bool phos4ScanBadAdcDistrib() const { return bitIsSet(Phos4ScanBadAdcDistribBit); }
   void phos4ScanBadAdcDistrib(bool bad) { setBit(Phos4ScanBadAdcDistribBit,bad); }

   bool phos4ScanBadSignalShape() const { return bitIsSet(Phos4ScanBadSignalShapeBit); }
   void phos4ScanBadSignalShape(bool bad) { setBit(Phos4ScanBadSignalShapeBit,bad); }

   bool phos4ScanSignalSaturated() const { return bitIsSet(Phos4ScanSignalSaturatedBit); }
   void phos4ScanSignalSaturated(bool bad) { setBit(Phos4ScanSignalSaturatedBit,bad); }

	bool totalWindowSizeUndercut() const { return bitIsSet(TotalWindowSizeUndercutBit); }
	void totalWindowSizeUndercut(bool undercut) { setBit(TotalWindowSizeUndercutBit,undercut); setBad(); }

	bool distToErrorRegionUndercut() const { return bitIsSet(DistToErrorRegionUndercutBit); }
	void distToErrorRegionUndercut(bool undercut) { setBit(DistToErrorRegionUndercutBit,undercut); setBad(); }

	bool exceededDelay() const { return bitIsSet(ExceededDelayBit); }
	void exceededDelay(bool exceeded ){ setBit(ExceededDelayBit,exceeded); setBad(); }

	bool exceededDelayAfterCorr() const { return bitIsSet(ExceededDelayAfterCorrBit); }
	void exceededDelayAfterCorr(bool exceeded ) { setBit(ExceededDelayAfterCorrBit,exceeded); setBad(); }

	std::string printStatus(ChanErrorSet spec) const;

protected:
	void setBad() { setBit(BadDataBit,true); }
};

#endif


