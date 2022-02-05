/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* Methods to perform post-processing on run_nnnnnn/HLT/JetMon* histograms
 * Mainly to adjust the impact parameter ranges according to the recorded lumi blocks.
 * This file does a check of existing input and the calls the actual adjustment.
 *
 *
 *
 *
 * ####################
 *	 #Notes on debugLevel#
 *	 ####################
 * - At all debugLevels > 0 errors will be displayed
 * - At debugLevels > 1 hints to the process flow will be shown
 * ####################
 *
 *
 *
 *
 *
 * Author   : Manuel Neumann (mneumann@cern.ch)
 * Date     : Aug 2011
 *
 * Based on the code of:
 * Methods to perform post-processing on run_nnnnnn/HLT/METMon* histograms
 * Author   : Venkatesh Kaushik (venkat.kaushik@cern.ch)
 * Date     : Feb 2010
 */

#include "DataQualityUtils/MonitoringFile.h"

#include "TFile.h"

namespace dqutils
{

	void
	MonitoringFile::BJetTaggingPostProcess(const std::string & inFilename, bool isIncremental) {

		if (isIncremental == true) {
			return;
		}

		int debugLevel = MonitoringFile::getDebugLevel();
		if (debugLevel > 1) {
			std::cout << "--> JetTaggingPostProcess: Begin JetTagging post-processing" << std::endl;
		}
		//open root file
		TFile* f = TFile::Open(inFilename.c_str(), "UPDATE");

		//check files are loaded.
		if (f == 0 || !f->IsOpen()) {
			std::cerr << "--> BJetTaggingPostProcess: Input file not opened" << std::endl;
			return;
		}

		if (f->IsZombie()) {
			std::cerr << "--> BJetTaggingPostProcess: Input file " << inFilename
					<< " cannot be opened. " << std::endl;
			return;
		}

		//check file size is not too small.
		if (f->GetSize() < 1000.) {
			std::cerr << "--> BJetTaggingPostProcess: Input file empty" << std::endl;
			f->Close();
			return;
		}

		//start postprocessing

		//here impact parameter histograms get a range in which they are actually filled
		BJetTaggingAdjustRanges(f);

		//this will normalise some 2D histograms to the track/jet number
		BJetTaggingNormalise(f);

		//close root file
		f->Close();
		//delete f;

		if (debugLevel > 1) {
			std::cout << "--> BJetTaggingPostProcess: End JetTagging post-processing " << std::endl;
		}
		return;

	}

}//namespace

