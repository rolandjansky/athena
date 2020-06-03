/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  NSWHitsTree.c
//  
//  Created by Ourania Sidiropoulou on 1/03/2013.
//

#define NSWHitsTree_cxx
#include "NSWHitsTree.h"

#include <TH1.h>
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TColor.h>

#include <map>
#include <vector>
#include <iostream>

using namespace std;


void NSWHitsTree::Loop()
{

	gROOT->ForceStyle();
	//gROOT->SetStyle("Plain");
	gStyle->SetOptStat(000000);

	/*********************************************** Compined Plots ***********************************************/

        /// Compined Hits for eta>0
	TH2F* hEtaPhiPlusSmallTGGLargeMM = new TH2F("hEtaPhiPlusSmallTGGLargeMM", "",  100,  1.2,  2.8, 200, -3.2, 3.2);
	TH2F* hwEtaPhiPlusSmallTGGLargeMM = new TH2F("hwEtaPhiPlusSmallTGGLargeMM", "",  100,  1.2,  2.8, 200, -3.2, 3.2);
	TH2F* hEtaPhiPlusSmallMMLargeTGC = new TH2F("hEtaPhiPlusSmallMMLargeTGC", "",  100,  1.2,  2.8, 200, -3.2, 3.2);
	TH2F* hwEtaPhiPlusSmallMMLargeTGC = new TH2F("hwEtaPhiPlusSmallMMLargeTGC", "",  100,  1.2,  2.8, 200, -3.2, 3.2);
	TH2F* hwEtaPhiPlusHits = new TH2F("hwEtaPhiPlusHits", "",  100,  1.2,  2.8, 200, -3.2, 3.2);
	TH2F* hEtaPhiPlusHits = new TH2F("hEtaPhiPlusHits", "",  100,  1.2,  2.8, 200, -3.2, 3.2);

	/// Compined Hits for eta<0
	TH2F* hEtaPhiMinusSmallTGGLargeMM = new TH2F("hEtaPhiMinusSmallTGGLargeMM", "",  100,  -2.8,  -1.2, 200, -3.2, 3.2);
	TH2F* hwEtaPhiMinusSmallTGGLargeMM = new TH2F("hwEtaPhiMinusSmallTGGLargeMM", "",  100,  -2.8,  -1.2, 200, -3.2, 3.2);
	TH2F* hEtaPhiMinusSmallMMLargeTGC = new TH2F("hEtaPhiMinusSmallMMLargeTGC", "",  100,  -2.8,  -1.2, 200, -3.2, 3.2);
	TH2F* hwEtaPhiMinusSmallMMLargeTGC = new TH2F("hwEtaPhiMinusSmallMMLargeTGC", "",  100,  -2.8,  -1.2, 200, -3.2, 3.2);
	TH2F* hwEtaPhiMinusHits = new TH2F("hwEtaPhiMinusHits", "",  100,  -2.8,  -1.2, 200, -3.2, 3.2);
	TH2F* hEtaPhiMinusHits = new TH2F("hEtaPhiMinusHits", "",  100,  -2.8,  -1.2, 200, -3.2, 3.2);

	/// Compined Digits (MM - sTGC pads) for eta>0
	TH2F* hEtaPhiPlusSmallTGGLargeMM_pad = new TH2F("hEtaPhiPlusSmallTGGLargeMM_pad", "",  100,  1.2,  2.8, 200, -3.2, 3.2);
	TH2F* hwEtaPhiPlusSmallTGGLargeMM_pad = new TH2F("hwEtaPhiPlusSmallTGGLargeMM_pad", "",  100,  1.2,  2.8, 200, -3.2, 3.2);
	TH2F* hEtaPhiPlusSmallMMLargeTGC_pad = new TH2F("hEtaPhiPlusSmallMMLargeTGC_pad", "",  100,  1.2,  2.8, 200, -3.2, 3.2);
	TH2F* hwEtaPhiPlusSmallMMLargeTGC_pad = new TH2F("hwEtaPhiPlusSmallMMLargeTGC_pad", "",  100,  1.2,  2.8, 200, -3.2, 3.2);
	TH2F* hwEtaPhiPlusDigits_pad = new TH2F("hwEtaPhiPlusDigits_pad", "",  100,  1.2,  2.8, 200, -3.2, 3.2);
	TH2F* hEtaPhiPlusDigits_pad = new TH2F("hEtaPhiPlusDigits_pad", "",  100,  1.2,  2.8, 200, -3.2, 3.2);

	/// Compined Digits (MM - sTGC pads) for eta<0
	TH2F* hEtaPhiMinusSmallTGGLargeMM_pad = new TH2F("hEtaPhiMinusSmallTGGLargeMM_pad", "",  100,  -2.8,  -1.2, 200, -3.2, 3.2);
	TH2F* hwEtaPhiMinusSmallTGGLargeMM_pad = new TH2F("hwEtaPhiMinusSmallTGGLargeMM_pad", "",  100,  -2.8,  -1.2, 200, -3.2, 3.2);
	TH2F* hEtaPhiMinusSmallMMLargeTGC_pad = new TH2F("hEtaPhiMinusSmallMMLargeTGC_pad", "",  100,  -2.8,  -1.2, 200, -3.2, 3.2);
	TH2F* hwEtaPhiMinusSmallMMLargeTGC_pad = new TH2F("hwEtaPhiMinusSmallMMLargeTGC_pad", "",  100,  -2.8,  -1.2, 200, -3.2, 3.2);
	TH2F* hwEtaPhiMinusDigits_pad = new TH2F("hwEtaPhiMinusDigits_pad", "",  100,  -2.8,  -1.2, 200, -3.2, 3.2);
	TH2F* hEtaPhiMinusDigits_pad = new TH2F("hEtaPhiMinusDigits_pad", "",  100,  -2.8,  -1.2, 200, -3.2, 3.2);

	/// Compined Digits (MM - sTGC strip) for eta>0
	TH2F* hEtaPhiPlusSmallTGGLargeMM_strip = new TH2F("hEtaPhiPlusSmallTGGLargeMM_strip", "",  100,  1.2,  2.8, 200, -3.2, 3.2);
	TH2F* hwEtaPhiPlusSmallTGGLargeMM_strip = new TH2F("hwEtaPhiPlusSmallTGGLargeMM_strip", "",  100,  1.2,  2.8, 200, -3.2, 3.2);
	TH2F* hEtaPhiPlusSmallMMLargeTGC_strip = new TH2F("hEtaPhiPlusSmallMMLargeTGC_strip", "",  100,  1.2,  2.8, 200, -3.2, 3.2);
	TH2F* hwEtaPhiPlusSmallMMLargeTGC_strip = new TH2F("hwEtaPhiPlusSmallMMLargeTGC_strip", "",  100,  1.2,  2.8, 200, -3.2, 3.2);
	TH2F* hwEtaPhiPlusDigits_strip = new TH2F("hwEtaPhiPlusDigits_strip", "",  100,  1.2,  2.8, 200, -3.2, 3.2);
	TH2F* hEtaPhiPlusDigits_strip = new TH2F("hEtaPhiPlusDigits_strip", "",  100,  1.2,  2.8, 200, -3.2, 3.2);

	/// Compined Digits (MM - sTGC strip) for eta<0
	TH2F* hEtaPhiMinusSmallTGGLargeMM_strip = new TH2F("hEtaPhiMinusSmallTGGLargeMM_strip", "",  100,  -2.8,  -1.2, 200, -3.2, 3.2);
	TH2F* hwEtaPhiMinusSmallTGGLargeMM_strip = new TH2F("hwEtaPhiMinusSmallTGGLargeMM_strip", "",  100,  -2.8,  -1.2, 200, -3.2, 3.2);
	TH2F* hEtaPhiMinusSmallMMLargeTGC_strip = new TH2F("hEtaPhiMinusSmallMMLargeTGC_strip", "",  100,  -2.8,  -1.2, 200, -3.2, 3.2);
	TH2F* hwEtaPhiMinusSmallMMLargeTGC_strip = new TH2F("hwEtaPhiMinusSmallMMLargeTGC_strip", "",  100,  -2.8,  -1.2, 200, -3.2, 3.2);
	TH2F* hwEtaPhiMinusDigits_strip = new TH2F("hwEtaPhiMinusDigits_strip", "",  100,  -2.8,  -1.2, 200, -3.2, 3.2);
	TH2F* hEtaPhiMinusDigits_strip = new TH2F("hEtaPhiMinusDigits_strip", "",  100,  -2.8,  -1.2, 200, -3.2, 3.2);

	/// Compined Digits (MM - sTGC wire) for eta>0
	TH2F* hEtaPhiPlusSmallTGGLargeMM_wire = new TH2F("hEtaPhiPlusSmallTGGLargeMM_wire", "",  100,  1.2,  2.8, 200, -3.2, 3.2);
	TH2F* hwEtaPhiPlusSmallTGGLargeMM_wire = new TH2F("hwEtaPhiPlusSmallTGGLargeMM_wire", "",  100,  1.2,  2.8, 200, -3.2, 3.2);
	TH2F* hEtaPhiPlusSmallMMLargeTGC_wire = new TH2F("hEtaPhiPlusSmallMMLargeTGC_wire", "",  100,  1.2,  2.8, 200, -3.2, 3.2);
	TH2F* hwEtaPhiPlusSmallMMLargeTGC_wire = new TH2F("hwEtaPhiPlusSmallMMLargeTGC_wire", "",  100,  1.2,  2.8, 200, -3.2, 3.2);
	TH2F* hwEtaPhiPlusDigits_wire = new TH2F("hwEtaPhiPlusDigits_wire", "",  100,  1.2,  2.8, 200, -3.2, 3.2);
	TH2F* hEtaPhiPlusDigits_wire = new TH2F("hEtaPhiPlusDigits_wire", "",  100,  1.2,  2.8, 200, -3.2, 3.2);

	/// Compined Digits (MM - sTGC wire) for eta<0
	TH2F* hEtaPhiMinusSmallTGGLargeMM_wire = new TH2F("hEtaPhiMinusSmallTGGLargeMM_wire", "",  100,  -2.8,  -1.2, 200, -3.2, 3.2);
	TH2F* hwEtaPhiMinusSmallTGGLargeMM_wire = new TH2F("hwEtaPhiMinusSmallTGGLargeMM_wire", "",  100,  -2.8,  -1.2, 200, -3.2, 3.2);
	TH2F* hEtaPhiMinusSmallMMLargeTGC_wire = new TH2F("hEtaPhiMinusSmallMMLargeTGC_wire", "",  100,  -2.8,  -1.2, 200, -3.2, 3.2);
	TH2F* hwEtaPhiMinusSmallMMLargeTGC_wire = new TH2F("hwEtaPhiMinusSmallMMLargeTGC_wire", "",  100,  -2.8,  -1.2, 200, -3.2, 3.2);
	TH2F* hwEtaPhiMinusDigits_wire = new TH2F("hwEtaPhiMinusDigits_wire", "",  100,  -2.8,  -1.2, 200, -3.2, 3.2);
	TH2F* hEtaPhiMinusDigits_wire = new TH2F("hEtaPhiMinusDigits_wire", "",  100,  -2.8,  -1.2, 200, -3.2, 3.2);


	/************************************** Hits and Digits for MM ******************************************/
        /// Hits MM sim
	TH2F* hEtaPhiPlusMM    = new TH2F("hEtaPhiPlusMM",   "",  100,  1.2,  2.8, 200, -3.2, 3.2);
	TH2F* hwEtaPhiPlusMM   = new TH2F("hwEtaPhiPlusMM",  "",  100,  1.2,  2.8, 200, -3.2, 3.2);
	TH2F* hEtaPhiMinusMM   = new TH2F("hEtaPhiMinusMM",  "",  100, -2.8, -1.2, 200, -3.2, 3.2);
	TH2F* hwEtaPhiMinusMM  = new TH2F("hwEtaPhiMinusMM", "",  100, -2.8, -1.2, 200, -3.2, 3.2);
	TH2F* wetaPhiGeneralMM = new TH2F("wetaPhiGeneralMM","",  100, -3.0,  3.0, 200, -3.2, 3.2);
	TH2F* etaPhiGeneralMM  = new TH2F("etaPhiGeneralMM", "",  100, -3.0,  3.0, 200, -3.2, 3.2);

	TH1F* hEtaMinusMM    = new TH1F("hEtaMinusMM",   "", 100, -2.8,-1.2);
	TH1F* hwEtaMinusMM   = new TH1F("hwEtaMinusMM",  "", 100, -2.8,-1.2);
	TH1F* hEtaPlusMM     = new TH1F("hEtaPlusMM",    "", 100,  1.2, 2.8);
	TH1F* hwEtaPlusMM    = new TH1F("hwEtaPlusMM",   "", 100,  1.2, 2.8);
	TH1F* hPhiMinusMM    = new TH1F("hPhiMinusMM",   "", 400, -3.2, 3.2);
	TH1F* hwPhiMinusMM   = new TH1F("hwPhiMinusMM",  "", 400, -3.2, 3.2);
        TH1F* hDivPhiMinusMM = new TH1F("hDivPhiMinusMM","", 400, -3.2, 3.2);
	TH1F* hPhiPlusMM     = new TH1F("hPhiPlusMM",    "", 400, -3.2, 3.2);
	TH1F* hwPhiPlusMM    = new TH1F("hwPhiPlusMM",   "", 400, -3.2, 3.2);
        TH1F* hDivPhiPlusMM  = new TH1F("hDivPhiPlusMM", "", 400, -3.2, 3.2);

        /// Fast Digitization MM
	TH2F* hEtaPhiPlusMM_fdg   = new TH2F("hEtaPhiPlusMM_fdg",  "",  100,  1.2,  2.8, 200, -3.2, 3.2);
	TH2F* hwEtaPhiPlusMM_fdg  = new TH2F("hwEtaPhiPlusMM_fdg", "",  100,  1.2,  2.8, 200, -3.2, 3.2);
	TH2F* hEtaPhiMinusMM_fdg  = new TH2F("hEtaPhiMinusMM_fdg", "",  100, -2.8, -1.2, 200, -3.2, 3.2);
	TH2F* hwEtaPhiMinusMM_fdg = new TH2F("hwEtaPhiMinusMM_fdg","",  100, -2.8, -1.2, 200, -3.2, 3.2);
	TH2F* wetaPhiGeneralMM_fdg = new TH2F("wetaPhiGeneralMM_fdg","", 100, -3.0,  3.0, 200, -3.2, 3.2);
	TH2F* etaPhiGeneralMM_fdg = new TH2F("etaPhiGeneralMM_fdg","",   100, -3.0,  3.0, 200, -3.2, 3.2);

	TH1F* hEtaMinusMM_fdg    = new TH1F("hEtaMinusMM_fdg",   "", 100,  -2.8,-1.2);
	TH1F* hwEtaMinusMM_fdg   = new TH1F("hwEtaMinusMM_fdg",  "", 100,  -2.8,-1.2);
	TH1F* hEtaPlusMM_fdg     = new TH1F("hEtaPlusMM_fdg",    "", 100,   1.2, 2.8);
	TH1F* hwEtaPlusMM_fdg    = new TH1F("hwEtaPlusMM_fdg",   "", 100,   1.2, 2.8);
	TH1F* hPhiMinusMM_fdg    = new TH1F("hPhiMinusMM_fdg",   "", 400,  -3.2, 3.2);
	TH1F* hwPhiMinusMM_fdg   = new TH1F("hwPhiMinusMM_fdg",  "", 400,  -3.2, 3.2);
        TH1F* hDivPhiMinusMM_fdg = new TH1F("hDivPhiMinusMM_fdg","", 400,  -3.2, 3.2);
	TH1F* hPhiPlusMM_fdg     = new TH1F("hPhiPlusMM_fdg",    "", 400,  -3.2, 3.2);
	TH1F* hwPhiPlusMM_fdg    = new TH1F("hwPhiPlusMM_fdg",   "", 400,  -3.2, 3.2);
        TH1F* hDivPhiPlusMM_fdg  = new TH1F("hDivPhiPlusMM_fdg", "", 400,  -3.2, 3.2);	

	/************************************** Hits and Digits for sTGC ******************************************/
        /// Hits sim sTG
	TH2F* hEtaPhiPlusTGC   = new TH2F("hEtaPhiPlusTGC",  "", 100,  1.2,  2.8, 200, -3.2, 3.2);
	TH2F* hwEtaPhiPlusTGC  = new TH2F("hwEtaPhiPlusTGC", "", 100,  1.2,  2.8, 200, -3.2, 3.2);
	TH2F* hEtaPhiMinusTGC  = new TH2F("hEtaPhiMinusTGC", "", 100, -2.8, -1.2, 200, -3.2, 3.2);
	TH2F* hwEtaPhiMinusTGC = new TH2F("hwEtaPhiMinusTGC","", 100, -2.8, -1.2, 200, -3.2, 3.2);
	TH2F* etaPhiGeneralTGC = new TH2F("etaPhiGeneralTGC","", 100, -3.0,  3.0, 200, -3.2, 3.2);
	TH2F* wetaPhiGeneralTGC = new TH2F("wetaPhiGeneralTGC","", 100, -3.0,  3.0, 200, -3.2, 3.2);

	TH1F* hEtaMinusTGC    = new TH1F("hEtaMinusTGC",   "", 100, -2.8,-1.2);
	TH1F* hwEtaMinusTGC   = new TH1F("hwEtaMinusTGC",  "", 100, -2.8,-1.2);
	TH1F* hEtaPlusTGC     = new TH1F("hEtaPlusTGC",    "", 100,  1.2, 2.8);
	TH1F* hwEtaPlusTGC    = new TH1F("hwEtaPlusTGC",   "", 100,  1.2, 2.8);
	TH1F* hPhiMinusTGC    = new TH1F("hPhiMinusTGC",   "", 400, -3.2, 3.2);
	TH1F* hwPhiMinusTGC   = new TH1F("hwPhiMinusTGC",  "", 400, -3.2, 3.2);
	TH1F* hDivPhiMinusTGC = new TH1F("hDivPhiMinusTGC","", 400, -3.2, 3.2);
	TH1F* hPhiPlusTGC     = new TH1F("hPhiPlusTGC",    "", 400, -3.2, 3.2);
	TH1F* hwPhiPlusTGC    = new TH1F("hwPhiPlusTGC",   "", 400, -3.2, 3.2);	
	TH1F* hDivPhiPlusTGC  = new TH1F("hDivPhiPlusTGC", "", 400, -3.2, 3.2);

        /// Fast Digitization sTG
        // For pads
	TH2F* hEtaPhiPlusTGC_fdg_pad   = new TH2F("hEtaPhiPlusTGC_fdg_pad",  "", 100,  1.2,  2.8, 200, -3.2, 3.2);
	TH2F* hwEtaPhiPlusTGC_fdg_pad  = new TH2F("hwEtaPhiPlusTGC_fdg_pad", "", 100,  1.2,  2.8, 200, -3.2, 3.2);
	TH2F* hEtaPhiMinusTGC_fdg_pad  = new TH2F("hEtaPhiMinusTGC_fdg_pad", "", 100, -2.8, -1.2, 200, -3.2, 3.2);
	TH2F* hwEtaPhiMinusTGC_fdg_pad = new TH2F("hwEtaPhiMinusTGC_fdg_pad","", 100, -2.8, -1.2, 200, -3.2, 3.2);
	TH2F* etaPhiGeneralTGC_fdg_pad = new TH2F("etaPhiGeneralTGC_fdg_pad","", 100, -3.0,  3.0, 200, -3.2, 3.2);
	TH2F* wetaPhiGeneralTGC_fdg_pad = new TH2F("wetaPhiGeneralTGC_fdg_pad","", 100, -3.0,  3.0, 200, -3.2, 3.2);

	TH1F* hEtaMinusTGC_fdg_pad    = new TH1F("hEtaMinusTGC_fdg_pad",   "", 100, -2.8,-1.2);
	TH1F* hwEtaMinusTGC_fdg_pad   = new TH1F("hwEtaMinusTGC_fdg_pad",  "", 100, -2.8,-1.2);
	TH1F* hEtaPlusTGC_fdg_pad     = new TH1F("hEtaPlusTGC_fdg_pad",    "", 100,  1.2, 2.8);
	TH1F* hwEtaPlusTGC_fdg_pad    = new TH1F("hwEtaPlusTGC_fdg_pad",   "", 100,  1.2, 2.8);
	TH1F* hPhiMinusTGC_fdg_pad    = new TH1F("hPhiMinusTGC_fdg_pad",   "", 400, -3.2, 3.2);
	TH1F* hwPhiMinusTGC_fdg_pad   = new TH1F("hwPhiMinusTGC_fdg_pad",  "", 400, -3.2, 3.2);
	TH1F* hDivPhiMinusTGC_fdg_pad = new TH1F("hDivPhiMinusTGC_fdg_pad","", 400, -3.2, 3.2);
	TH1F* hPhiPlusTGC_fdg_pad     = new TH1F("hPhiPlusTGC_fdg_pad",    "", 400, -3.2, 3.2);
	TH1F* hwPhiPlusTGC_fdg_pad    = new TH1F("hwPhiPlusTGC_fdg_pad",   "", 400, -3.2, 3.2);	
	TH1F* hDivPhiPlusTGC_fdg_pad  = new TH1F("hDivPhiPlusTGC_fdg_pad", "", 400, -3.2, 3.2);

        // For strips
	TH2F* hEtaPhiPlusTGC_fdg_strip   = new TH2F("hEtaPhiPlusTGC_fdg_strip",  "", 100,  1.2,  2.8, 200, -3.2, 3.2);
	TH2F* hwEtaPhiPlusTGC_fdg_strip  = new TH2F("hwEtaPhiPlusTGC_fdg_strip", "", 100,  1.2,  2.8, 200, -3.2, 3.2);
	TH2F* hEtaPhiMinusTGC_fdg_strip  = new TH2F("hEtaPhiMinusTGC_fdg_strip", "", 100, -2.8, -1.2, 200, -3.2, 3.2);
	TH2F* hwEtaPhiMinusTGC_fdg_strip = new TH2F("hwEtaPhiMinusTGC_fdg_strip","", 100, -2.8, -1.2, 200, -3.2, 3.2);
	TH2F* etaPhiGeneralTGC_fdg_strip = new TH2F("etaPhiGeneralTGC_fdg_strip","", 100, -3.0,  3.0, 200, -3.2, 3.2);
	TH2F* wetaPhiGeneralTGC_fdg_strip = new TH2F("wetaPhiGeneralTGC_fdg_strip","", 100, -3.0,  3.0, 200, -3.2, 3.2);

	TH1F* hEtaMinusTGC_fdg_strip    = new TH1F("hEtaMinusTGC_fdg_strip", "", 100, -2.8,-1.2);
	TH1F* hwEtaMinusTGC_fdg_strip   = new TH1F("hwEtaMinusTGC_fdg_strip","", 100, -2.8,-1.2);
	TH1F* hEtaPlusTGC_fdg_strip     = new TH1F("hEtaPlusTGC_fdg_strip",  "", 100,  1.2, 2.8);
	TH1F* hwEtaPlusTGC_fdg_strip    = new TH1F("hwEtaPlusTGC_fdg_strip", "", 100,  1.2, 2.8);
	TH1F* hPhiMinusTGC_fdg_strip    = new TH1F("hPhiMinusTGC_fdg_strip", "", 400, -3.2, 3.2);
	TH1F* hwPhiMinusTGC_fdg_strip   = new TH1F("hwPhiMinusTGC_fdg_strip","", 400, -3.2, 3.2);
	TH1F* hDivPhiMinusTGC_fdg_strip = new TH1F("hDivPhiMinusTGC_fdg_strip","", 400, -3.2, 3.2);
	TH1F* hPhiPlusTGC_fdg_strip     = new TH1F("hPhiPlusTGC_fdg_strip",    "", 400, -3.2, 3.2);
	TH1F* hwPhiPlusTGC_fdg_strip    = new TH1F("hwPhiPlusTGC_fdg_strip",   "", 400, -3.2, 3.2);	
	TH1F* hDivPhiPlusTGC_fdg_strip  = new TH1F("hDivPhiPlusTGC_fdg_strip", "", 400, -3.2, 3.2);

        // For wires
	TH2F* hEtaPhiPlusTGC_fdg_wire   = new TH2F("hEtaPhiPlusTGC_fdg_wire",  "", 100,  1.2,  2.8, 200, -3.2, 3.2);
	TH2F* hwEtaPhiPlusTGC_fdg_wire  = new TH2F("hwEtaPhiPlusTGC_fdg_wire", "", 100,  1.2,  2.8, 200, -3.2, 3.2);
	TH2F* hEtaPhiMinusTGC_fdg_wire  = new TH2F("hEtaPhiMinusTGC_fdg_wire", "", 100, -2.8, -1.2, 200, -3.2, 3.2);
	TH2F* hwEtaPhiMinusTGC_fdg_wire = new TH2F("hwEtaPhiMinusTGC_fdg_wire","", 100, -2.8, -1.2, 200, -3.2, 3.2);
	TH2F* etaPhiGeneralTGC_fdg_wire = new TH2F("etaPhiGeneralTGC_fdg_wire","", 100, -3.0,  3.0, 200, -3.2, 3.2);
	TH2F* wetaPhiGeneralTGC_fdg_wire = new TH2F("wetaPhiGeneralTGC_fdg_wire","", 100, -3.0,  3.0, 200, -3.2, 3.2);

	TH1F* hEtaMinusTGC_fdg_wire    = new TH1F("hEtaMinusTGC_fdg_wire", "",   100, -2.8,-1.2);
	TH1F* hwEtaMinusTGC_fdg_wire   = new TH1F("hwEtaMinusTGC_fdg_wire","",   100, -2.8,-1.2);
	TH1F* hEtaPlusTGC_fdg_wire     = new TH1F("hEtaPlusTGC_fdg_wire",  "",   100,  1.2, 2.8);
	TH1F* hwEtaPlusTGC_fdg_wire    = new TH1F("hwEtaPlusTGC_fdg_wire", "",   100,  1.2, 2.8);
	TH1F* hPhiMinusTGC_fdg_wire    = new TH1F("hPhiMinusTGC_fdg_wire", "",   400, -3.2, 3.2);
	TH1F* hwPhiMinusTGC_fdg_wire   = new TH1F("hwPhiMinusTGC_fdg_wire","",   400, -3.2, 3.2);
	TH1F* hDivPhiMinusTGC_fdg_wire = new TH1F("hDivPhiMinusTGC_fdg_wire","", 400, -3.2, 3.2);
	TH1F* hPhiPlusTGC_fdg_wire     = new TH1F("hPhiPlusTGC_fdg_wire",  "",   400, -3.2, 3.2);
	TH1F* hwPhiPlusTGC_fdg_wire    = new TH1F("hwPhiPlusTGC_fdg_wire", "",   400, -3.2, 3.2);	
	TH1F* hDivPhiPlusTGC_fdg_wire  = new TH1F("hDivPhiPlusTGC_fdg_wire", "", 400, -3.2, 3.2);

	// File to be written
	TFile* valResults = new TFile("NSWValResultsHitsDigits.root","RECREATE");
   
	// For contour plots
	UInt_t Number = 3;  Int_t nab=50;
	Double_t Red[3]    = { 1.00, 0.00, 0.00 };
	Double_t Green[3]  = { 0.00, 1.00, 0.00 };
	Double_t Blue[3]   = { 1.00, 0.00, 1.00 };
	Double_t Length[3] = { 0.00, 0.50, 1.00 };
	TColor::CreateGradientColorTable(Number,Length,Red,Green,Blue,nab);

	// Global vars 
        /// For hits
	int muonsHitsEtaPlusMM,  muonsHitsEtaPlusMMSmallSector,  muonsHitsEtaPlusMMLargeSector;
        int muonsHitsEtaMinusMM, muonsHitsEtaMinusMMSmallSector, muonsHitsEtaMinusMMLargeSector;
	int multilayerMMplus,  layerMMplus; // for side A
        int multilayerMMminus, layerMMminus;// for side C
	bool hitexistsMMplus, hitexistsMMminus; 
 
	int muonsHitsEtaPlusTGC,  muonsHitsEtaPlusTGCSmallSector, muonsHitsEtaPlusTGCLargeSector;
        int muonsHitsEtaMinusTGC, muonsHitsEtaMinusTGCSmallSector, muonsHitsEtaMinusTGCLargeSector;
        int multilayerTGCplus,  layerTGCplus;       // for side A
        int multilayerTGCminus, layerTGCminus;       //for side C
	bool hitexistsTGCplus, hitexistsTGCminus;

        /// For digits
	int muonsDigitsEtaPlusMM,  muonsDigitsEtaPlusMMSmallSector,  muonsDigitsEtaPlusMMLargeSector;
	int muonsDigitsEtaMinusMM, muonsDigitsEtaMinusMMSmallSector, muonsDigitsEtaMinusMMLargeSector;
	int multilayerMMplus_fdg,  layerMMplus_fdg;  // for side A
        int multilayerMMminus_fdg, layerMMminus_fdg; // for side C
	bool digitexistsMMplus, digitexistsMMminus; 
 
        int muonsDigitsEtaPlusTGC, muonsDigitsEtaMinusTGC;
	int muonsDigitsEtaPlusTGC_pad, muonsDigitsEtaPlusTGCSmallSector_pad, muonsDigitsEtaPlusTGCLargeSector_pad;
        int muonsDigitsEtaPlusTGC_strip, muonsDigitsEtaPlusTGCSmallSector_strip, muonsDigitsEtaPlusTGCLargeSector_strip;
        int muonsDigitsEtaPlusTGC_wire, muonsDigitsEtaPlusTGCSmallSector_wire, muonsDigitsEtaPlusTGCLargeSector_wire;
        int muonsDigitsEtaMinusTGC_pad, muonsDigitsEtaMinusTGCSmallSector_pad, muonsDigitsEtaMinusTGCLargeSector_pad;
        int muonsDigitsEtaMinusTGC_strip, muonsDigitsEtaMinusTGCSmallSector_strip, muonsDigitsEtaMinusTGCLargeSector_strip;
        int muonsDigitsEtaMinusTGC_wire,muonsDigitsEtaMinusTGCSmallSector_wire, muonsDigitsEtaMinusTGCLargeSector_wire;
        int multilayerTGCplus_fdg, layerTGCplus_fdg;   // for side A
        int multilayerTGCminus_fdg, layerTGCminus_fdg; // for side C
	bool digitexistsTGCplus, digitexistsTGCminus;


	int temp, temp2;
	int nLarge, nSmall; //total num of hits or digits in the Large or Small sectors

	// For truth particles
	double etaMinusTruth, etaPlusTruth, phiMinusTruth, phiPlusTruth;

	if (fChain == 0) return;
	Long64_t nentries = fChain->GetEntriesFast();
	Long64_t nbytes = 0, nb = 0;

	/***********************************************************
	 *              Main loop to the entries of the ntuple 
	 ***********************************************************/     
	for (Long64_t jentry=0; jentry < nentries; jentry++) {

		/***************** Initiliazation *****************************/
		temp = 0; temp2 = 0;
		nLarge = 0;
		nSmall = 0;
	
                /// For MM hits
		map< int, map<int,int> > hitposMMplus; 
		multilayerMMplus = 0;
		layerMMplus = 0;
		hitexistsMMplus = false;

		map< int, map<int,int> > hitposMMminus;
		multilayerMMminus = 0;
		layerMMminus = 0;
		hitexistsMMminus = false;

		muonsHitsEtaPlusMM = 0;
		muonsHitsEtaPlusMMSmallSector = 0;
		muonsHitsEtaPlusMMLargeSector = 0;
		muonsHitsEtaMinusMM = 0;
		muonsHitsEtaMinusMMSmallSector = 0;
		muonsHitsEtaMinusMMLargeSector = 0;

                /// For MM digits
		map< int, map<int,int> > digitposMMplus; 
		multilayerMMplus_fdg = 0;
		layerMMplus_fdg = 0;
		digitexistsMMplus = false;

		map< int, map<int,int> > digitposMMminus;
		multilayerMMminus_fdg = 0;
		layerMMminus_fdg = 0;
		digitexistsMMminus = false;

		muonsDigitsEtaPlusMM = 0;
		muonsDigitsEtaPlusMMSmallSector = 0;
		muonsDigitsEtaPlusMMLargeSector = 0;
		muonsDigitsEtaMinusMM = 0;
		muonsDigitsEtaMinusMMSmallSector = 0;
		muonsDigitsEtaMinusMMLargeSector = 0;
		
                /// For sTGC hits
		map< int, map<int,int> > hitposTGCplus;	
		multilayerTGCplus = 0;
		layerTGCplus = 0;
		hitexistsTGCplus = false;

		map< int, map<int,int> > hitposTGCminus;	
		multilayerTGCminus = 0;
		layerTGCminus = 0;
		hitexistsTGCminus = false;

		muonsHitsEtaPlusTGC = 0;
		muonsHitsEtaPlusTGCSmallSector = 0;
		muonsHitsEtaPlusTGCLargeSector = 0;
		muonsHitsEtaMinusTGC = 0;
		muonsHitsEtaMinusTGCSmallSector = 0;
		muonsHitsEtaMinusTGCLargeSector = 0;

                /// For sTGC digits
		map< int, map<int,int> > digitposTGCplus;	
		multilayerTGCplus_fdg = 0;
		layerTGCplus_fdg = 0;
		digitexistsTGCplus = false;

		map< int, map<int,int> > digitposTGCminus;	
		multilayerTGCminus_fdg = 0;
		layerTGCminus_fdg = 0;
		digitexistsTGCminus = false;

		muonsDigitsEtaPlusTGC = 0;
		muonsDigitsEtaPlusTGC_pad   = 0;
		muonsDigitsEtaPlusTGCSmallSector_pad   = 0;
		muonsDigitsEtaPlusTGCLargeSector_pad   = 0;
		muonsDigitsEtaPlusTGC_strip = 0;
		muonsDigitsEtaPlusTGCSmallSector_strip = 0;
		muonsDigitsEtaPlusTGCLargeSector_strip = 0;
		muonsDigitsEtaPlusTGC_wire  = 0;
		muonsDigitsEtaPlusTGCSmallSector_wire  = 0;
		muonsDigitsEtaPlusTGCLargeSector_wire  = 0;

		muonsDigitsEtaMinusTGC = 0;
		muonsDigitsEtaMinusTGC_pad   = 0;
		muonsDigitsEtaMinusTGCSmallSector_pad   = 0;
		muonsDigitsEtaMinusTGCLargeSector_pad   = 0;
		muonsDigitsEtaMinusTGC_strip = 0;
		muonsDigitsEtaMinusTGCSmallSector_strip = 0;
		muonsDigitsEtaMinusTGCLargeSector_strip = 0;
		muonsDigitsEtaMinusTGC_wire  = 0;
		muonsDigitsEtaMinusTGCSmallSector_wire  = 0;
		muonsDigitsEtaMinusTGCLargeSector_wire  = 0;

	
		Long64_t ientry = LoadTree(jentry);
		if (ientry < 0) break;
		nb = fChain->GetEntry(jentry);   nbytes += nb;

	       /***********************************************************
	        *                    Loop to MM hits
	        ***********************************************************/  
		for (unsigned int nMMHit=0; nMMHit < Hits_MM_n; nMMHit++) {  

			// Hits on Side A
			hitexistsMMplus  = false;
			multilayerMMplus = Hits_MM_sim_multilayer->at(nMMHit);
			layerMMplus      = Hits_MM_sim_layer->at(nMMHit);

                        if(abs(Hits_MM_detector_globalPositionZ->at(nMMHit)) > 7400)//large sectors
                             multilayerMMplus = (multilayerMMplus == 1)? 3 : 4;
                        else 
                             multilayerMMplus = Hits_MM_sim_multilayer->at(nMMHit);//small sectors
  

			if(hitposMMplus.find(multilayerMMplus)!=hitposMMplus.end()){
				if(hitposMMplus[multilayerMMplus].find(layerMMplus) != 
                                   hitposMMplus[multilayerMMplus].end())
                                {
					hitexistsMMplus = true;
				}
			}


			// Hits on Side C
			hitexistsMMminus  = false;
			multilayerMMminus = Hits_MM_sim_multilayer->at(nMMHit);
			layerMMminus      = Hits_MM_sim_layer->at(nMMHit);

                        if(abs(Hits_MM_detector_globalPositionZ->at(nMMHit)) > 7400)//large sectors
                             multilayerMMminus = (multilayerMMminus == 1)? 3 : 4;
                        else 
                             multilayerMMminus = Hits_MM_sim_multilayer->at(nMMHit);//small sectors


			if(hitposMMminus.find(multilayerMMminus)!=hitposMMminus.end()){
				if(hitposMMminus[multilayerMMminus].find(layerMMminus) != 
                                   hitposMMminus[multilayerMMminus].end())
                                {
					hitexistsMMminus = true;
				}
			}


			if(hitexistsMMplus && hitexistsMMminus) continue;

			if(!hitexistsMMplus){
				if ( (Hits_MM_hitGlobalPositionZ->at(nMMHit) > 0) && 
					(abs(Hits_MM_particleEncoding->at(nMMHit)) == 13) )
				{   
					muonsHitsEtaPlusMM++;
					hitposMMplus[multilayerMMplus].insert(std::pair<int,int>(layerMMplus,1));

                                        if ( (multilayerMMplus == 1)||(multilayerMMplus == 2))//hits on small sectors
						muonsHitsEtaPlusMMSmallSector++;
                                        else
						muonsHitsEtaPlusMMLargeSector++;


				}
			}
			if(!hitexistsMMminus){
				if( (Hits_MM_hitGlobalPositionZ->at(nMMHit) < 0) && 
				        (abs(Hits_MM_particleEncoding->at(nMMHit)) == 13) )
				{   
					muonsHitsEtaMinusMM++;     
					hitposMMminus[multilayerMMminus].insert(std::pair<int,int>(layerMMminus,1));

                                        if ( (multilayerMMminus == 1)||(multilayerMMminus == 2))//hits on small sectors
						muonsHitsEtaMinusMMSmallSector++;
                                        else
						muonsHitsEtaMinusMMLargeSector++;
				}
							
			}


		}// Close loop to MM Hits


        	/***********************************************************
         	*                    Loop to sTGC hits
         	***********************************************************/  
		for (unsigned int nTGCHit=0; nTGCHit < Hits_sTGC_n; nTGCHit++) {  

                        // Hits on Side A
			hitexistsTGCplus =  false;
			multilayerTGCplus = Hits_sTGC_sim_multilayer->at(nTGCHit);
			layerTGCplus      = Hits_sTGC_sim_layer->at(nTGCHit);

                        if(abs(Hits_sTGC_detector_globalPositionZ->at(nTGCHit)) > 7400)//large sectors
                             multilayerTGCplus = (multilayerTGCplus == 1)? 3 : 4;
                        else 
                             multilayerTGCplus = Hits_sTGC_sim_multilayer->at(nTGCHit);//small sectors


                        // Hits on Side C
			hitexistsTGCminus =  false;
			multilayerTGCminus = Hits_sTGC_sim_multilayer->at(nTGCHit);
			layerTGCminus      = Hits_sTGC_sim_layer->at(nTGCHit);

                        if(abs(Hits_sTGC_detector_globalPositionZ->at(nTGCHit)) > 7400)//large sectors
                             multilayerTGCminus = (multilayerTGCminus == 1)? 3 : 4;
                        else 
                             multilayerTGCminus = Hits_sTGC_sim_multilayer->at(nTGCHit);//small sectors


			if(hitposTGCplus.find(multilayerTGCplus) !=hitposTGCplus.end()){
				if(hitposTGCplus[multilayerTGCplus].find(layerTGCplus) != 
                                   hitposTGCplus[multilayerTGCplus].end())
                                {
					hitexistsTGCplus = true;
				}
			}

			if(hitposTGCminus.find(multilayerTGCminus) !=hitposTGCminus.end()){
				if(hitposTGCminus[multilayerTGCminus].find(layerTGCminus) != 
                                   hitposTGCminus[multilayerTGCminus].end())
                                {
					hitexistsTGCminus = true;
				}
			}

			if(hitexistsTGCplus && hitexistsTGCminus) continue;

			if(!hitexistsTGCplus){
				if ( (Hits_sTGC_hitGlobalPositionZ->at(nTGCHit) > 0) && 
				     (abs(Hits_sTGC_particleEncoding->at(nTGCHit)) == 13) )
				{   
					muonsHitsEtaPlusTGC++;
					hitposTGCplus[multilayerTGCplus].insert(std::pair<int,int>(layerTGCplus,1));

                                        if ( (multilayerTGCplus == 1)||(multilayerTGCplus == 2))//hits on small sectors
						muonsHitsEtaPlusTGCSmallSector++;
                                        else
						muonsHitsEtaPlusTGCLargeSector++;

				}
			}
			if(!hitexistsTGCminus){
				if( (Hits_sTGC_hitGlobalPositionZ->at(nTGCHit) < 0) && 
				    (abs(Hits_sTGC_particleEncoding->at(nTGCHit)) == 13) )
				{   
					muonsHitsEtaMinusTGC++;     
					hitposTGCminus[multilayerTGCminus].insert(std::pair<int,int>(layerTGCminus,1));


                                        if ( (multilayerTGCminus == 1)||(multilayerTGCminus == 2))//hits on small sectors
						muonsHitsEtaMinusTGCSmallSector++;
                                        else
						muonsHitsEtaMinusTGCLargeSector++;
				}
							
			}



		}// Close loop to sTGCs Hits



	       /***********************************************************
	        *                    Loop to MM digits
	        ***********************************************************/  
		for (unsigned int nMMDigit=0; nMMDigit < FastDigit_MM; nMMDigit++) {  

			// Digits on Side A
			digitexistsMMplus  = false;
			multilayerMMplus_fdg = FastDigit_MM_multiplet->at(nMMDigit);// multilayer
			layerMMplus_fdg      = FastDigit_MM_gas_gap->at(nMMDigit);  // layer


                        if(abs(FastDigit_MM_globalPosZ->at(nMMDigit)) > 7400)//large sectors
                             multilayerMMplus_fdg = (multilayerMMplus_fdg == 1)? 3 : 4;
                        else 
                             multilayerMMplus_fdg = FastDigit_MM_multiplet->at(nMMDigit);//small sectors
  

			if(digitposMMplus.find(multilayerMMplus_fdg)!=digitposMMplus.end()){
				if(digitposMMplus[multilayerMMplus_fdg].find(layerMMplus_fdg) != 
                                   digitposMMplus[multilayerMMplus_fdg].end())
                                {
					digitexistsMMplus = true;
				}
			}


			// Digits on Side C
			digitexistsMMminus  = false;
			multilayerMMminus_fdg = FastDigit_MM_multiplet->at(nMMDigit); // multilayer
			layerMMminus_fdg      = FastDigit_MM_gas_gap->at(nMMDigit);   // layer

                        if(abs(FastDigit_MM_globalPosZ->at(nMMDigit)) > 7400)//large sectors
                             multilayerMMminus_fdg = (multilayerMMminus_fdg == 1)? 3 : 4;
                        else 
                             multilayerMMminus_fdg = FastDigit_MM_multiplet->at(nMMDigit);//small sectors


			if(digitposMMminus.find(multilayerMMminus_fdg)!=digitposMMminus.end()){
				if(digitposMMminus[multilayerMMminus_fdg].find(layerMMminus_fdg) != 
                                   digitposMMminus[multilayerMMminus_fdg].end())
                                {
					digitexistsMMminus = true;
				}
			}


			if(digitexistsMMplus && digitexistsMMminus) continue;

			if(!digitexistsMMplus){
				if ( FastDigit_MM_truth_barcode->at(nMMDigit) == 10001)// for globalPosZ>0
				{   
                                        
					muonsDigitsEtaPlusMM++;
					digitposMMplus[multilayerMMplus_fdg].insert(std::pair<int,int>(layerMMplus_fdg,1));

					if ( (multilayerMMplus_fdg==1) || (multilayerMMplus_fdg==2) ) // small sector
						muonsDigitsEtaPlusMMSmallSector++;			
					else
						muonsDigitsEtaPlusMMLargeSector++;

				}
			}
			if(!digitexistsMMminus){
				if ( FastDigit_MM_truth_barcode->at(nMMDigit) == 10002)// for globalPosZ<0
				{   
					muonsDigitsEtaMinusMM++;     
					digitposMMminus[multilayerMMminus_fdg].insert(std::pair<int,int>(layerMMminus_fdg,1));

					if ( (multilayerMMminus_fdg==1) || (multilayerMMminus_fdg==2) ) // small sector
						muonsDigitsEtaMinusMMSmallSector++;			
					else
						muonsDigitsEtaMinusMMLargeSector++;
				}
							
			}


		}// Close loop to MM Digits

        	/****************************************************************************************
         	*                    Loop to sTGC digits
         	****************************************************************************************/  
		for (unsigned int nTGCDigit=0; nTGCDigit < FastDigit_sTGC; nTGCDigit++) {  

                        /************************* Digits on Side A ********************************/
			digitexistsTGCplus = false;

			multilayerTGCplus_fdg = FastDigit_sTGC_multiplet->at(nTGCDigit);// multilayer
			layerTGCplus_fdg      = FastDigit_sTGC_gas_gap->at(nTGCDigit);  // layer


                        // (0)Pad:   1, 2 (small sector)  3, 4 (large sector)
                        // (1)Strip: 5, 6 (small sector)  7, 8 (large sector)
                        // (2)Wire:  9,10 (small sector) 11,12 (large sector)


			// Small Sectors
                        if(abs(FastDigit_sTGC_globalPosZ->at(nTGCDigit)) < 7400){ 

                             // Define channel type:strip
			     if ( FastDigit_sTGC_channel_type->at(nTGCDigit) == 1) {
                             
                                 multilayerTGCplus_fdg = (multilayerTGCplus_fdg == 1)? 5 : 6;
                             }
                             
                             // Define channel type:wire    
			     if ( FastDigit_sTGC_channel_type->at(nTGCDigit) == 2) {
                             
                                 multilayerTGCplus_fdg = (multilayerTGCplus_fdg == 1)? 9 : 10;
                             }        
                                                
                        
                        // Large Sectors  
                        }else{ 

                              // Define channel type:pad
			     if ( FastDigit_sTGC_channel_type->at(nTGCDigit) == 0) {
                             
                                 multilayerTGCplus_fdg = (multilayerTGCplus_fdg == 1)? 3 : 4;
                             }

                              // Define channel type:strip
			     if ( FastDigit_sTGC_channel_type->at(nTGCDigit) == 1) {
                             
                                 multilayerTGCplus_fdg = (multilayerTGCplus_fdg == 1)? 7 : 8;
                             }
                             
                             // Define channel type:wire    
			     if ( FastDigit_sTGC_channel_type->at(nTGCDigit) == 2) {
                             
                                 multilayerTGCplus_fdg = (multilayerTGCplus_fdg == 1)? 11 : 12;
                             }    
                       
                        }


			if(digitposTGCplus.find(multilayerTGCplus_fdg) !=digitposTGCplus.end()){
				if(digitposTGCplus[multilayerTGCplus_fdg].find(layerTGCplus_fdg) != 
                                   digitposTGCplus[multilayerTGCplus_fdg].end())
                                {
					digitexistsTGCplus = true;

				}

			}

                        /******************************* Digits on Side C ******************************/
			digitexistsTGCminus = false;

			multilayerTGCminus_fdg = FastDigit_sTGC_multiplet->at(nTGCDigit);// multilayer
			layerTGCminus_fdg      = FastDigit_sTGC_gas_gap->at(nTGCDigit);  // layer

                        // (0)Pad:   1, 2 (small sector)  3, 4 (large sector)
                        // (1)Strip: 5, 6 (small sector)  7, 8 (large sector)
                        // (2)Wire:  9,10 (small sector) 11,12 (large sector)


			// Small Sectors
                        if(abs(FastDigit_sTGC_globalPosZ->at(nTGCDigit)) < 7400){ 

                             // Define channel type:strip
			     if ( FastDigit_sTGC_channel_type->at(nTGCDigit) == 1) {
                             
                                 multilayerTGCminus_fdg = (multilayerTGCminus_fdg == 1)? 5 : 6;
                             }
                             
                             // Define channel type:wire    
			     if ( FastDigit_sTGC_channel_type->at(nTGCDigit) == 2) {
                             
                                 multilayerTGCminus_fdg = (multilayerTGCminus_fdg == 1)? 9 : 10;
                             }                           
                        
                        // Large Sectors  
                        }else{ 

                              // Define channel type:pad
			     if ( FastDigit_sTGC_channel_type->at(nTGCDigit) == 0) {
                             
                                 multilayerTGCminus_fdg = (multilayerTGCminus_fdg == 1)? 3 : 4;
                             }

                              // Define channel type:strip
			     if ( FastDigit_sTGC_channel_type->at(nTGCDigit) == 1) {
                             
                                 multilayerTGCminus_fdg = (multilayerTGCminus_fdg == 1)? 7 : 8;
                             }
                             
                             // Define channel type:wire    
			     if ( FastDigit_sTGC_channel_type->at(nTGCDigit) == 2) {
                             
                                 multilayerTGCminus_fdg = (multilayerTGCminus_fdg == 1)? 11 : 12;
                             }    
                       
                        }


			if(digitposTGCminus.find(multilayerTGCminus_fdg) !=digitposTGCminus.end()){
				if(digitposTGCminus[multilayerTGCminus_fdg].find(layerTGCminus_fdg) != 
                                   digitposTGCminus[multilayerTGCminus_fdg].end())
                                {
					digitexistsTGCminus = true;
				}
			}

			if(digitexistsTGCplus && digitexistsTGCminus) continue;

			if(!digitexistsTGCplus){
                                /***************** For z>0  *****************************/
                                // for pads
				if ( FastDigit_sTGC_truth_barcode->at(nTGCDigit) == 10001 )
                                 	muonsDigitsEtaPlusTGC++;
                                else
					muonsDigitsEtaMinusTGC++;

						
				if ( (FastDigit_sTGC_truth_barcode->at(nTGCDigit) == 10001) &&
                                     (FastDigit_sTGC_channel_type->at(nTGCDigit)  == 0) )
				{   
					muonsDigitsEtaPlusTGC_pad++;
					digitposTGCplus[multilayerTGCplus_fdg].insert(std::pair<int,int>(layerTGCplus_fdg,1));

					if ( (multilayerTGCplus_fdg==1)||(multilayerTGCplus_fdg==2) )//small sector
						muonsDigitsEtaPlusTGCSmallSector_pad++;
					if ( (multilayerTGCplus_fdg==3)||(multilayerTGCplus_fdg==4) )//large sector
						muonsDigitsEtaPlusTGCLargeSector_pad++;


				}

                                // for strips
				if ( (FastDigit_sTGC_truth_barcode->at(nTGCDigit) == 10001) &&
                                     (FastDigit_sTGC_channel_type->at(nTGCDigit)  == 1) )
				{   
					muonsDigitsEtaPlusTGC_strip++;
					digitposTGCplus[multilayerTGCplus_fdg].insert(std::pair<int,int>(layerTGCplus_fdg,1));

					if ( (multilayerTGCplus_fdg==5)||(multilayerTGCplus_fdg==6) )//small sector
						muonsDigitsEtaPlusTGCSmallSector_strip++;
					if ( (multilayerTGCplus_fdg==7)||(multilayerTGCplus_fdg==8) )//large sector
						muonsDigitsEtaPlusTGCLargeSector_strip++;

				}

                                // for wire
				if ( (FastDigit_sTGC_truth_barcode->at(nTGCDigit) == 10001) &&
                                     (FastDigit_sTGC_channel_type->at(nTGCDigit)  == 2) )
				{   
					muonsDigitsEtaPlusTGC_wire++;
					digitposTGCplus[multilayerTGCplus_fdg].insert(std::pair<int,int>(layerTGCplus_fdg,1));

					if ( (multilayerTGCplus_fdg==9) || (multilayerTGCplus_fdg==10) )//small sector
						muonsDigitsEtaPlusTGCSmallSector_wire++;
					if ( (multilayerTGCplus_fdg==11)|| (multilayerTGCplus_fdg==12) )//large sector
						muonsDigitsEtaPlusTGCLargeSector_wire++;

				}

			}

                        /***************** For z>0  *****************************/
                        // for pads
			if(!digitexistsTGCminus){
				if ( (FastDigit_sTGC_truth_barcode->at(nTGCDigit) == 10002) &&
                                     (FastDigit_sTGC_channel_type->at(nTGCDigit)  == 0) )
				{   
					muonsDigitsEtaMinusTGC_pad++;     
					digitposTGCminus[multilayerTGCminus_fdg].insert(std::pair<int,int>(layerTGCminus_fdg,1));

					if ( (multilayerTGCminus_fdg==1)||(multilayerTGCminus_fdg==2) )//small sector
						muonsDigitsEtaMinusTGCSmallSector_pad++;
					if ( (multilayerTGCminus_fdg==3)||(multilayerTGCminus_fdg==4) )//large sector
						muonsDigitsEtaMinusTGCLargeSector_pad++;
				}
							
			}

                        // for strips
			if(!digitexistsTGCminus){
				if ( (FastDigit_sTGC_truth_barcode->at(nTGCDigit) == 10002) &&
                                     (FastDigit_sTGC_channel_type->at(nTGCDigit)  == 1) )
				{   
					muonsDigitsEtaMinusTGC_strip++;     
					digitposTGCminus[multilayerTGCminus_fdg].insert(std::pair<int,int>(layerTGCminus_fdg,1));

					if ( (multilayerTGCminus_fdg==5)||(multilayerTGCminus_fdg==6) )//small sector
						muonsDigitsEtaMinusTGCSmallSector_strip++;
					if ( (multilayerTGCminus_fdg==7)||(multilayerTGCminus_fdg==8) )//large sector
						muonsDigitsEtaMinusTGCLargeSector_strip++;
				}
							
			}

                        // for wires
			if(!digitexistsTGCminus){
				if ( (FastDigit_sTGC_truth_barcode->at(nTGCDigit) == 10002) &&
                                     (FastDigit_sTGC_channel_type->at(nTGCDigit)  == 2) )
				{   
					muonsDigitsEtaMinusTGC_wire++;     
					digitposTGCminus[multilayerTGCminus_fdg].insert(std::pair<int,int>(layerTGCminus_fdg,1));

					if ( (multilayerTGCminus_fdg==9)||(multilayerTGCminus_fdg==10) )//small sector
						muonsDigitsEtaMinusTGCSmallSector_wire++;
					if ( (multilayerTGCminus_fdg==11)||(multilayerTGCminus_fdg==12) )//large sector
						muonsDigitsEtaMinusTGCLargeSector_wire++;
				}
							
			}


		}// Close loop to sTGCs digits


        	/***********************************************************
        	*                    Loop to MuEntry_Particle_n
                ***********************************************************/    
                etaMinusTruth = -999.0; phiMinusTruth = -999.0; 
                etaPlusTruth  =  999.0; phiPlusTruth  =  999.0;    
        
		for (unsigned int nTruth=0; nTruth < MuEntry_Particle_n; nTruth++){
			if (abs(MuEntry_Particle_Pdg->at(nTruth)) ==13) {
				if (MuEntry_Position_Eta->at(nTruth) < 0) {
					phiMinusTruth = MuEntry_Position_Phi->at(nTruth);
                                        //if (MuEntry_Particle_Pt->at(nTruth) > 10000){
						etaMinusTruth = MuEntry_Position_Eta->at(nTruth);
                                        //}
				}

				if (MuEntry_Position_Eta->at(nTruth) > 0){
					phiPlusTruth = MuEntry_Position_Phi->at(nTruth);
                                        //if (MuEntry_Particle_Pt->at(nTruth) > 10000){
						etaPlusTruth = MuEntry_Position_Eta->at(nTruth);
                                        //}
				}
			}                           
		}

                /***********************************************************
                *                       Fill Histos 
                ***********************************************************/
                // For MM hits in eta minus 
		if ( muonsHitsEtaMinusMM > 0){
      
                     if((etaMinusTruth >= -2.8) && (etaMinusTruth <= -1.2)){

			hwEtaMinusMM->Fill(etaMinusTruth,muonsHitsEtaMinusMM);
			hEtaMinusMM->Fill(etaMinusTruth);
                     

			hDivPhiMinusMM->Fill(phiMinusTruth,muonsHitsEtaMinusMM);
			hwPhiMinusMM->Fill(phiMinusTruth,muonsHitsEtaMinusMM);
			hPhiMinusMM->Fill(phiMinusTruth);
                	
			hwEtaPhiMinusMM->Fill(etaMinusTruth, phiMinusTruth,muonsHitsEtaMinusMM);
			hEtaPhiMinusMM->Fill(etaMinusTruth, phiMinusTruth);

                        wetaPhiGeneralMM->Fill(etaMinusTruth,phiMinusTruth,muonsHitsEtaMinusMM);
                        etaPhiGeneralMM->Fill(etaMinusTruth,phiMinusTruth);
                    }

		}
		// For MM hits in eta plus
		if (muonsHitsEtaPlusMM > 0){

		   if((etaPlusTruth >= 1.2) && (etaPlusTruth <= 2.8)){
		        
			hwEtaPlusMM->Fill(etaPlusTruth,muonsHitsEtaPlusMM);
			hEtaPlusMM->Fill(etaPlusTruth);

			hDivPhiPlusMM->Fill(phiPlusTruth,muonsHitsEtaPlusMM);
			hwPhiPlusMM->Fill(phiPlusTruth,muonsHitsEtaPlusMM);
			hPhiPlusMM->Fill(phiPlusTruth);

			hwEtaPhiPlusMM->Fill(etaPlusTruth, phiPlusTruth, muonsHitsEtaPlusMM);
			hEtaPhiPlusMM->Fill(etaPlusTruth, phiPlusTruth);

                        wetaPhiGeneralMM->Fill(etaPlusTruth,phiPlusTruth,muonsHitsEtaPlusMM);
                        etaPhiGeneralMM->Fill(etaPlusTruth,phiPlusTruth);
                   }

		}

                // For MM digits in eta minus 
		if ( muonsDigitsEtaMinusMM > 0){
      
                     if((etaMinusTruth >= -2.8) && (etaMinusTruth <= -1.2)){

			hwEtaMinusMM_fdg->Fill(etaMinusTruth,muonsDigitsEtaMinusMM);
			hEtaMinusMM_fdg->Fill(etaMinusTruth);

			hDivPhiMinusMM_fdg->Fill(phiMinusTruth,muonsDigitsEtaMinusMM);
			hwPhiMinusMM_fdg->Fill(phiMinusTruth,muonsDigitsEtaMinusMM);
			hPhiMinusMM_fdg->Fill(phiMinusTruth);
                	
			hwEtaPhiMinusMM_fdg->Fill(etaMinusTruth, phiMinusTruth,muonsDigitsEtaMinusMM);
			hEtaPhiMinusMM_fdg->Fill(etaMinusTruth, phiMinusTruth);

                        wetaPhiGeneralMM_fdg->Fill(etaMinusTruth,phiMinusTruth,muonsDigitsEtaMinusMM);
                        etaPhiGeneralMM_fdg->Fill(etaMinusTruth,phiMinusTruth);
                    }

		}
		// For MM digits in eta plus
		if (muonsDigitsEtaPlusMM > 0){

		   if((etaPlusTruth >= 1.2) && (etaPlusTruth <= 2.8)){
		        
			hwEtaPlusMM_fdg->Fill(etaPlusTruth,muonsDigitsEtaPlusMM);
			hEtaPlusMM_fdg->Fill(etaPlusTruth);

			hDivPhiPlusMM_fdg->Fill(phiPlusTruth,muonsDigitsEtaPlusMM);
			hwPhiPlusMM_fdg->Fill(phiPlusTruth,muonsDigitsEtaPlusMM);
			hPhiPlusMM_fdg->Fill(phiPlusTruth);

			hwEtaPhiPlusMM_fdg->Fill(etaPlusTruth, phiPlusTruth, muonsDigitsEtaPlusMM);
			hEtaPhiPlusMM_fdg->Fill(etaPlusTruth, phiPlusTruth);

                        wetaPhiGeneralMM_fdg->Fill(etaPlusTruth,phiPlusTruth,muonsDigitsEtaPlusMM);
                        etaPhiGeneralMM_fdg->Fill(etaPlusTruth,phiPlusTruth);
                   }

		}


                /////////////////////////////////////////////////////////////////////////////////////////

                // For sTGC hits in eta minus 
		if ( muonsHitsEtaMinusTGC > 0 ){

                     if((etaMinusTruth >= -2.8) && (etaMinusTruth <= -1.2)){
      
			hwEtaMinusTGC->Fill(etaMinusTruth,muonsHitsEtaMinusTGC);
			hEtaMinusTGC->Fill(etaMinusTruth);

			hDivPhiMinusTGC->Fill(phiMinusTruth,muonsHitsEtaMinusTGC);
			hwPhiMinusTGC->Fill(phiMinusTruth,muonsHitsEtaMinusTGC);
			hPhiMinusTGC->Fill(phiMinusTruth);

			hwEtaPhiMinusTGC->Fill(etaMinusTruth, phiMinusTruth,muonsHitsEtaMinusTGC);
			hEtaPhiMinusTGC->Fill(etaMinusTruth, phiMinusTruth);
                       
                        wetaPhiGeneralTGC->Fill(etaMinusTruth, phiMinusTruth,muonsHitsEtaMinusTGC);
                        etaPhiGeneralTGC->Fill(etaMinusTruth, phiMinusTruth);

                     }

		}
		// For sTGC hits in eta plus
		if (muonsHitsEtaPlusTGC > 0){

		   if((etaPlusTruth >= 1.2) && (etaPlusTruth <= 2.8)){
		        
			hwEtaPlusTGC->Fill(etaPlusTruth,muonsHitsEtaPlusTGC);
			hEtaPlusTGC->Fill(etaPlusTruth);

			hDivPhiPlusTGC->Fill(phiPlusTruth,muonsHitsEtaPlusTGC);
			hwPhiPlusTGC->Fill(phiPlusTruth,muonsHitsEtaPlusTGC);
			hPhiPlusTGC->Fill(phiPlusTruth);

			hwEtaPhiPlusTGC->Fill(etaPlusTruth, phiPlusTruth, muonsHitsEtaPlusTGC);
			hEtaPhiPlusTGC->Fill(etaPlusTruth, phiPlusTruth);

                        wetaPhiGeneralTGC->Fill(etaPlusTruth, phiPlusTruth,muonsHitsEtaPlusTGC);
                        etaPhiGeneralTGC->Fill(etaPlusTruth, phiPlusTruth);
                   }

		}

                /************************ For sTGC digits in eta minus ***********************************/
                // For pads
		if ( muonsDigitsEtaMinusTGC_pad > 0 ){

                     if((etaMinusTruth >= -2.8) && (etaMinusTruth <= -1.2)){
      
			hwEtaMinusTGC_fdg_pad->Fill(etaMinusTruth,muonsDigitsEtaMinusTGC_pad);
			hEtaMinusTGC_fdg_pad->Fill(etaMinusTruth);

			hDivPhiMinusTGC_fdg_pad->Fill(phiMinusTruth,muonsDigitsEtaMinusTGC_pad);
			hwPhiMinusTGC_fdg_pad->Fill(phiMinusTruth,muonsDigitsEtaMinusTGC_pad);
			hPhiMinusTGC_fdg_pad->Fill(phiMinusTruth);

			hwEtaPhiMinusTGC_fdg_pad->Fill(etaMinusTruth, phiMinusTruth,muonsDigitsEtaMinusTGC_pad);
			hEtaPhiMinusTGC_fdg_pad->Fill(etaMinusTruth, phiMinusTruth);
                       
                        wetaPhiGeneralTGC_fdg_pad->Fill(etaMinusTruth, phiMinusTruth,muonsDigitsEtaMinusTGC_pad);
                        etaPhiGeneralTGC_fdg_pad->Fill(etaMinusTruth, phiMinusTruth);

                     }

		}

                // For strips
		if ( muonsDigitsEtaMinusTGC_strip > 0 ){

                     if((etaMinusTruth >= -2.8) && (etaMinusTruth <= -1.2)){
      
			hwEtaMinusTGC_fdg_strip->Fill(etaMinusTruth,muonsDigitsEtaMinusTGC_strip);
			hEtaMinusTGC_fdg_strip->Fill(etaMinusTruth);

			hDivPhiMinusTGC_fdg_strip->Fill(phiMinusTruth,muonsDigitsEtaMinusTGC_strip);
			hwPhiMinusTGC_fdg_strip->Fill(phiMinusTruth,muonsDigitsEtaMinusTGC_strip);
			hPhiMinusTGC_fdg_strip->Fill(phiMinusTruth);

			hwEtaPhiMinusTGC_fdg_strip->Fill(etaMinusTruth, phiMinusTruth,muonsDigitsEtaMinusTGC_strip);
			hEtaPhiMinusTGC_fdg_strip->Fill(etaMinusTruth, phiMinusTruth);
                       
                        wetaPhiGeneralTGC_fdg_strip->Fill(etaMinusTruth, phiMinusTruth,muonsDigitsEtaMinusTGC_strip);
                        etaPhiGeneralTGC_fdg_strip->Fill(etaMinusTruth, phiMinusTruth);

                     }

		}

                // For wires
		if ( muonsDigitsEtaMinusTGC_wire > 0 ){

                     if((etaMinusTruth >= -2.8) && (etaMinusTruth <= -1.2)){
      
			hwEtaMinusTGC_fdg_wire->Fill(etaMinusTruth,muonsDigitsEtaMinusTGC_wire);
			hEtaMinusTGC_fdg_wire->Fill(etaMinusTruth);

			hDivPhiMinusTGC_fdg_wire->Fill(phiMinusTruth,muonsDigitsEtaMinusTGC_wire);
			hwPhiMinusTGC_fdg_wire->Fill(phiMinusTruth,muonsDigitsEtaMinusTGC_wire);
			hPhiMinusTGC_fdg_wire->Fill(phiMinusTruth);

			hwEtaPhiMinusTGC_fdg_wire->Fill(etaMinusTruth, phiMinusTruth,muonsDigitsEtaMinusTGC_wire);
			hEtaPhiMinusTGC_fdg_wire->Fill(etaMinusTruth, phiMinusTruth);
                       
                        wetaPhiGeneralTGC_fdg_wire->Fill(etaMinusTruth, phiMinusTruth,muonsDigitsEtaMinusTGC_wire);
                        etaPhiGeneralTGC_fdg_wire->Fill(etaMinusTruth, phiMinusTruth);

                     }

		}

		/*********************** For sTGC digits in eta plus *************************************/
                // For pads
		if (muonsDigitsEtaPlusTGC_pad > 0){

		   if((etaPlusTruth >= 1.2) && (etaPlusTruth <= 2.8)){
		        
			hwEtaPlusTGC_fdg_pad->Fill(etaPlusTruth,muonsDigitsEtaPlusTGC_pad);
			hEtaPlusTGC_fdg_pad->Fill(etaPlusTruth);

			hDivPhiPlusTGC_fdg_pad->Fill(phiPlusTruth,muonsDigitsEtaPlusTGC_pad);
			hwPhiPlusTGC_fdg_pad->Fill(phiPlusTruth,muonsDigitsEtaPlusTGC_pad);
			hPhiPlusTGC_fdg_pad->Fill(phiPlusTruth);

			hwEtaPhiPlusTGC_fdg_pad->Fill(etaPlusTruth, phiPlusTruth, muonsDigitsEtaPlusTGC_pad);
			hEtaPhiPlusTGC_fdg_pad->Fill(etaPlusTruth, phiPlusTruth);

                        wetaPhiGeneralTGC_fdg_pad->Fill(etaPlusTruth, phiPlusTruth,muonsDigitsEtaPlusTGC_pad);
                        etaPhiGeneralTGC_fdg_pad->Fill(etaPlusTruth, phiPlusTruth);
                   }

		}

                // For strips
		if (muonsDigitsEtaPlusTGC_strip > 0){

		   if((etaPlusTruth >= 1.2) && (etaPlusTruth <= 2.8)){
		        
			hwEtaPlusTGC_fdg_strip->Fill(etaPlusTruth,muonsDigitsEtaPlusTGC_strip);
			hEtaPlusTGC_fdg_strip->Fill(etaPlusTruth);

			hDivPhiPlusTGC_fdg_strip->Fill(phiPlusTruth,muonsDigitsEtaPlusTGC_strip);
			hwPhiPlusTGC_fdg_strip->Fill(phiPlusTruth,muonsDigitsEtaPlusTGC_strip);
			hPhiPlusTGC_fdg_strip->Fill(phiPlusTruth);

			hwEtaPhiPlusTGC_fdg_strip->Fill(etaPlusTruth, phiPlusTruth, muonsDigitsEtaPlusTGC_strip);
			hEtaPhiPlusTGC_fdg_strip->Fill(etaPlusTruth, phiPlusTruth);

                        wetaPhiGeneralTGC_fdg_strip->Fill(etaPlusTruth, phiPlusTruth,muonsDigitsEtaPlusTGC_strip);
                        etaPhiGeneralTGC_fdg_strip->Fill(etaPlusTruth, phiPlusTruth);
                   }

		}   

                // For wires
		if (muonsDigitsEtaPlusTGC_wire > 0){

		   if((etaPlusTruth >= 1.2) && (etaPlusTruth <= 2.8)){
		        
			hwEtaPlusTGC_fdg_wire->Fill(etaPlusTruth,muonsDigitsEtaPlusTGC_wire);
			hEtaPlusTGC_fdg_wire->Fill(etaPlusTruth);

			hDivPhiPlusTGC_fdg_wire->Fill(phiPlusTruth,muonsDigitsEtaPlusTGC_wire);
			hwPhiPlusTGC_fdg_wire->Fill(phiPlusTruth,muonsDigitsEtaPlusTGC_wire);
			hPhiPlusTGC_fdg_wire->Fill(phiPlusTruth);

			hwEtaPhiPlusTGC_fdg_wire->Fill(etaPlusTruth, phiPlusTruth, muonsDigitsEtaPlusTGC_wire);
			hEtaPhiPlusTGC_fdg_wire->Fill(etaPlusTruth, phiPlusTruth);

                        wetaPhiGeneralTGC_fdg_wire->Fill(etaPlusTruth, phiPlusTruth,muonsDigitsEtaPlusTGC_wire);
                        etaPhiGeneralTGC_fdg_wire->Fill(etaPlusTruth, phiPlusTruth);
                   }

		}


		/******************* For the compined plots *****************************/
 
               /// For eta>0
		if((etaPlusTruth >= 1.2) && (etaPlusTruth <= 2.8)){
			
			if ( ( (muonsHitsEtaPlusTGCSmallSector>0) && (muonsHitsEtaPlusMMSmallSector==0) ) &&
                             ( (muonsHitsEtaPlusMMLargeSector>0)  && (muonsHitsEtaPlusTGCLargeSector==0) ) ){
	                   
		                temp = muonsHitsEtaPlusTGCSmallSector + muonsHitsEtaPlusMMLargeSector;
				hwEtaPhiPlusSmallTGGLargeMM->Fill(etaPlusTruth, phiPlusTruth, temp);
				hEtaPhiPlusSmallTGGLargeMM ->Fill(etaPlusTruth, phiPlusTruth);

			}

			if ( ( (muonsHitsEtaPlusMMSmallSector>0)  && (muonsHitsEtaPlusTGCSmallSector==0) ) &&
                             ( (muonsHitsEtaPlusTGCLargeSector>0) && (muonsHitsEtaPlusMMLargeSector==0) ) ){
	                   
		                temp = muonsHitsEtaPlusMMSmallSector + muonsHitsEtaPlusTGCLargeSector;
				hwEtaPhiPlusSmallMMLargeTGC->Fill(etaPlusTruth, phiPlusTruth, temp);
				hEtaPhiPlusSmallMMLargeTGC ->Fill(etaPlusTruth, phiPlusTruth);

			}
                   
          
                        // MM - pads
			if ( ( (muonsDigitsEtaPlusTGCSmallSector_pad>0) && (muonsDigitsEtaPlusMMSmallSector == 0 ) ) &&
                             ( (muonsDigitsEtaPlusMMLargeSector > 0)    && (muonsDigitsEtaPlusTGCLargeSector_pad == 0) ) ){

				temp = muonsDigitsEtaPlusTGCSmallSector_pad + muonsDigitsEtaPlusMMLargeSector;
				hwEtaPhiPlusSmallTGGLargeMM_pad->Fill(etaPlusTruth, phiPlusTruth,temp);
				hEtaPhiPlusSmallTGGLargeMM_pad->Fill(etaPlusTruth, phiPlusTruth);
	
			}


			if ( ( (muonsDigitsEtaPlusMMSmallSector>0) && (muonsDigitsEtaPlusTGCSmallSector_pad ==0) ) &&
                             ( (muonsDigitsEtaPlusTGCLargeSector_pad > 0) && (muonsDigitsEtaPlusMMLargeSector==0) ) ){

				temp = muonsDigitsEtaPlusMMSmallSector + muonsDigitsEtaPlusTGCLargeSector_pad;
				hwEtaPhiPlusSmallMMLargeTGC_pad->Fill(etaPlusTruth, phiPlusTruth,temp);
				hEtaPhiPlusSmallMMLargeTGC_pad->Fill(etaPlusTruth, phiPlusTruth);
	
			}

                        // MM - wires
			if ( ( (muonsDigitsEtaPlusTGCSmallSector_wire>0) && (muonsDigitsEtaPlusMMSmallSector==0) ) &&
                             ( (muonsDigitsEtaPlusMMLargeSector > 0) && (muonsDigitsEtaPlusTGCLargeSector_wire==0) ) ){

				temp = muonsDigitsEtaPlusTGCSmallSector_wire + muonsDigitsEtaPlusMMLargeSector;
				hwEtaPhiPlusSmallTGGLargeMM_wire->Fill(etaPlusTruth, phiPlusTruth,temp);
				hEtaPhiPlusSmallTGGLargeMM_wire->Fill(etaPlusTruth, phiPlusTruth);
	
			}


			if ( ( (muonsDigitsEtaPlusMMSmallSector>0) && (muonsDigitsEtaPlusTGCSmallSector_wire==0) ) &&
                             ( (muonsDigitsEtaPlusTGCLargeSector_wire > 0) && (muonsDigitsEtaPlusMMLargeSector==0) ) ){

				temp = muonsDigitsEtaPlusMMSmallSector + muonsDigitsEtaPlusTGCLargeSector_wire;
				hwEtaPhiPlusSmallMMLargeTGC_wire->Fill(etaPlusTruth, phiPlusTruth,temp);
				hEtaPhiPlusSmallMMLargeTGC_wire->Fill(etaPlusTruth, phiPlusTruth);
	
			}

                        // MM - strip
			if ( ( (muonsDigitsEtaPlusTGCSmallSector_strip>0) && (muonsDigitsEtaPlusMMSmallSector==0) ) &&
                             ( (muonsDigitsEtaPlusMMLargeSector > 0) && (muonsDigitsEtaPlusTGCLargeSector_strip==0) ) ){

				temp = muonsDigitsEtaPlusTGCSmallSector_strip + muonsDigitsEtaPlusMMLargeSector;
				hwEtaPhiPlusSmallTGGLargeMM_strip->Fill(etaPlusTruth, phiPlusTruth,temp);
				hEtaPhiPlusSmallTGGLargeMM_strip->Fill(etaPlusTruth, phiPlusTruth);
	
			}


			if ( ( (muonsDigitsEtaPlusMMSmallSector>0) && (muonsDigitsEtaPlusTGCSmallSector_strip==0) ) &&
                             ( (muonsDigitsEtaPlusTGCLargeSector_strip > 0) && (muonsDigitsEtaPlusMMLargeSector==0) ) ){

				temp = muonsDigitsEtaPlusMMSmallSector + muonsDigitsEtaPlusTGCLargeSector_strip;
				hwEtaPhiPlusSmallMMLargeTGC_strip->Fill(etaPlusTruth, phiPlusTruth,temp);
				hEtaPhiPlusSmallMMLargeTGC_strip->Fill(etaPlusTruth, phiPlusTruth);
	
			}

			/*******************************************************************************
			*           Check for all hits in Large and Small sectors seperately
			********************************************************************************/
			nLarge = muonsHitsEtaPlusMMLargeSector + muonsHitsEtaPlusTGCLargeSector;
			nSmall = muonsHitsEtaPlusMMSmallSector + muonsHitsEtaPlusTGCSmallSector;
			
						
			if ( nLarge > nSmall )
			{

				hwEtaPhiPlusHits->Fill(etaPlusTruth, phiPlusTruth, nLarge);
				hEtaPhiPlusHits ->Fill(etaPlusTruth, phiPlusTruth);

			}else{
				hwEtaPhiPlusHits->Fill(etaPlusTruth, phiPlusTruth, nSmall);
				hEtaPhiPlusHits ->Fill(etaPlusTruth, phiPlusTruth);

			}

			/*******************************************************************************
			*           Check for all digits in Large and Small sectors seperately
			********************************************************************************/
                        /// Counting the digits to sTGC strips and MM strips
			nLarge = muonsDigitsEtaPlusMMLargeSector + muonsDigitsEtaPlusTGCLargeSector_strip;
			nSmall = muonsDigitsEtaPlusMMSmallSector + muonsDigitsEtaPlusTGCSmallSector_strip;
			
						
			if ( nLarge > nSmall )
			{

				hwEtaPhiPlusDigits_strip->Fill(etaPlusTruth, phiPlusTruth, nLarge);
				hEtaPhiPlusDigits_strip ->Fill(etaPlusTruth, phiPlusTruth);

			}else{
				hwEtaPhiPlusDigits_strip->Fill(etaPlusTruth, phiPlusTruth, nSmall);
				hEtaPhiPlusDigits_strip ->Fill(etaPlusTruth, phiPlusTruth);

			}

                        /// Counting the digits to sTGC wires and MM wires
			nLarge = muonsDigitsEtaPlusMMLargeSector + muonsDigitsEtaPlusTGCLargeSector_wire;
			nSmall = muonsDigitsEtaPlusMMSmallSector + muonsDigitsEtaPlusTGCSmallSector_wire;
			
						
			if ( nLarge > nSmall )
			{

				hwEtaPhiPlusDigits_wire->Fill(etaPlusTruth, phiPlusTruth, nLarge);
				hEtaPhiPlusDigits_wire ->Fill(etaPlusTruth, phiPlusTruth);

			}else{
				hwEtaPhiPlusDigits_wire->Fill(etaPlusTruth, phiPlusTruth, nSmall);
				hEtaPhiPlusDigits_wire ->Fill(etaPlusTruth, phiPlusTruth);

			}

                        /// Counting the digits to sTGC wires and MM pads
			nLarge = muonsDigitsEtaPlusMMLargeSector + muonsDigitsEtaPlusTGCLargeSector_pad;
			nSmall = muonsDigitsEtaPlusMMSmallSector + muonsDigitsEtaPlusTGCSmallSector_pad;
			
						
			if ( nLarge > nSmall )
			{

				hwEtaPhiPlusDigits_pad->Fill(etaPlusTruth, phiPlusTruth, nLarge);
				hEtaPhiPlusDigits_pad ->Fill(etaPlusTruth, phiPlusTruth);

			}else{
				hwEtaPhiPlusDigits_pad->Fill(etaPlusTruth, phiPlusTruth, nSmall);
				hEtaPhiPlusDigits_pad ->Fill(etaPlusTruth, phiPlusTruth);

			}





		}

                /// For eta<0
		if ((etaMinusTruth >= -2.8) && (etaMinusTruth <= -1.2)){

			
			if ( ( (muonsHitsEtaMinusTGCSmallSector>0) && (muonsHitsEtaMinusMMSmallSector==0) ) &&
                             ( (muonsHitsEtaMinusMMLargeSector>0)  && (muonsHitsEtaMinusTGCLargeSector==0) ) ){
	                   
		                temp = muonsHitsEtaMinusTGCSmallSector + muonsHitsEtaMinusMMLargeSector;
				hwEtaPhiMinusSmallTGGLargeMM->Fill(etaMinusTruth, phiMinusTruth, temp);
				hEtaPhiMinusSmallTGGLargeMM ->Fill(etaMinusTruth, phiMinusTruth);
                                

			}

			if ( ( (muonsHitsEtaMinusMMSmallSector>0)  && (muonsHitsEtaMinusTGCSmallSector==0) ) &&
                             ( (muonsHitsEtaMinusTGCLargeSector>0) && (muonsHitsEtaMinusMMLargeSector==0) ) ){
	                   
		                temp = muonsHitsEtaMinusMMSmallSector + muonsHitsEtaMinusTGCLargeSector;
				hwEtaPhiMinusSmallMMLargeTGC->Fill(etaMinusTruth, phiMinusTruth, temp);
				hEtaPhiMinusSmallMMLargeTGC ->Fill(etaMinusTruth, phiMinusTruth);

			}
                   
          
                        // MM - pads
			if ( ( (muonsDigitsEtaMinusTGCSmallSector_pad>0) && (muonsDigitsEtaMinusMMSmallSector == 0 ) ) &&
                             ( (muonsDigitsEtaMinusMMLargeSector > 0)    && (muonsDigitsEtaMinusTGCLargeSector_pad == 0) ) )
			{

				temp = muonsDigitsEtaMinusTGCSmallSector_pad + muonsDigitsEtaMinusMMLargeSector;
				hwEtaPhiMinusSmallTGGLargeMM_pad->Fill(etaMinusTruth, phiMinusTruth,temp);
				hEtaPhiMinusSmallTGGLargeMM_pad->Fill(etaMinusTruth, phiMinusTruth);
	
			}


			if ( ( (muonsDigitsEtaMinusMMSmallSector>0) && (muonsDigitsEtaMinusTGCSmallSector_pad ==0) ) &&
                             ( (muonsDigitsEtaMinusTGCLargeSector_pad > 0) && (muonsDigitsEtaMinusMMLargeSector==0) ) ){

				temp = muonsDigitsEtaMinusMMSmallSector + muonsDigitsEtaMinusTGCLargeSector_pad;
				hwEtaPhiMinusSmallMMLargeTGC_pad->Fill(etaMinusTruth, phiMinusTruth,temp);
				hEtaPhiMinusSmallMMLargeTGC_pad->Fill(etaMinusTruth, phiMinusTruth);
	
			}

                        // MM - wires
			if ( ( (muonsDigitsEtaMinusTGCSmallSector_wire>0) && (muonsDigitsEtaMinusMMSmallSector==0) ) &&
                             ( (muonsDigitsEtaMinusMMLargeSector > 0) && (muonsDigitsEtaMinusTGCLargeSector_wire==0) ) ){

				temp = muonsDigitsEtaMinusTGCSmallSector_wire + muonsDigitsEtaMinusMMLargeSector;
				hwEtaPhiMinusSmallTGGLargeMM_wire->Fill(etaMinusTruth, phiMinusTruth,temp);
				hEtaPhiMinusSmallTGGLargeMM_wire->Fill(etaMinusTruth, phiMinusTruth);
	
			}


			if ( ( (muonsDigitsEtaMinusMMSmallSector>0) && (muonsDigitsEtaMinusTGCSmallSector_wire==0) ) &&
                             ( (muonsDigitsEtaMinusTGCLargeSector_wire > 0) && (muonsDigitsEtaMinusMMLargeSector==0) ) ){

				temp = muonsDigitsEtaMinusMMSmallSector + muonsDigitsEtaMinusTGCLargeSector_wire;
				hwEtaPhiMinusSmallMMLargeTGC_wire->Fill(etaMinusTruth, phiMinusTruth,temp);
				hEtaPhiMinusSmallMMLargeTGC_wire->Fill(etaMinusTruth, phiMinusTruth);
	
			}

                        // MM - strip
			if ( ( (muonsDigitsEtaMinusTGCSmallSector_strip>0) && (muonsDigitsEtaMinusMMSmallSector==0) ) &&
                             ( (muonsDigitsEtaMinusMMLargeSector > 0) && (muonsDigitsEtaMinusTGCLargeSector_strip==0) ) ){

				temp = muonsDigitsEtaMinusTGCSmallSector_strip + muonsDigitsEtaMinusMMLargeSector;
				hwEtaPhiMinusSmallTGGLargeMM_strip->Fill(etaMinusTruth, phiMinusTruth,temp);
				hEtaPhiMinusSmallTGGLargeMM_strip->Fill(etaMinusTruth, phiMinusTruth);
	
			}


			if ( ( (muonsDigitsEtaMinusMMSmallSector>0) && (muonsDigitsEtaMinusTGCSmallSector_strip==0) ) &&
                             ( (muonsDigitsEtaMinusTGCLargeSector_strip > 0) && (muonsDigitsEtaMinusMMLargeSector==0) ) ){

				temp = muonsDigitsEtaMinusMMSmallSector + muonsDigitsEtaMinusTGCLargeSector_strip;
				hwEtaPhiMinusSmallMMLargeTGC_strip->Fill(etaMinusTruth, phiMinusTruth,temp);
				hEtaPhiMinusSmallMMLargeTGC_strip->Fill(etaMinusTruth, phiMinusTruth);
	
			}

			/*******************************************************************************
			*           Check for all hits in Large and Small sectors seperately
			********************************************************************************/
			nLarge = muonsHitsEtaMinusMMLargeSector + muonsHitsEtaMinusTGCLargeSector;
			nSmall = muonsHitsEtaMinusMMSmallSector + muonsHitsEtaMinusTGCSmallSector;
			
						
			if ( nLarge >= nSmall )
			{

				hwEtaPhiMinusHits->Fill(etaMinusTruth, phiMinusTruth, nLarge);
				hEtaPhiMinusHits ->Fill(etaMinusTruth, phiMinusTruth);

			}else{
				hwEtaPhiMinusHits->Fill(etaMinusTruth, phiMinusTruth, nSmall);
				hEtaPhiMinusHits ->Fill(etaMinusTruth, phiMinusTruth);

			}

			/*******************************************************************************
			*           Check for all digits in Large and Small sectors seperately
			********************************************************************************/
                        /// Counting the digits to sTGC strips and MM strips
			nLarge = muonsDigitsEtaMinusMMLargeSector + muonsDigitsEtaMinusTGCLargeSector_strip;
			nSmall = muonsDigitsEtaMinusMMSmallSector + muonsDigitsEtaMinusTGCSmallSector_strip;
			
						
			if ( nLarge >= nSmall )
			{

				hwEtaPhiMinusDigits_strip->Fill(etaMinusTruth, phiMinusTruth, nLarge);
				hEtaPhiMinusDigits_strip ->Fill(etaMinusTruth, phiMinusTruth);

			}else{
				hwEtaPhiMinusDigits_strip->Fill(etaMinusTruth, phiMinusTruth, nSmall);
				hEtaPhiMinusDigits_strip ->Fill(etaMinusTruth, phiMinusTruth);

			}

                        /// Counting the digits to sTGC wires and MM wires
			nLarge = muonsDigitsEtaMinusMMLargeSector + muonsDigitsEtaMinusTGCLargeSector_wire;
			nSmall = muonsDigitsEtaMinusMMSmallSector + muonsDigitsEtaMinusTGCSmallSector_wire;
			
						
			if ( nLarge >= nSmall )
			{

				hwEtaPhiMinusDigits_wire->Fill(etaMinusTruth, phiMinusTruth, nLarge);
				hEtaPhiMinusDigits_wire ->Fill(etaMinusTruth, phiMinusTruth);

			}else{
				hwEtaPhiMinusDigits_wire->Fill(etaMinusTruth, phiMinusTruth, nSmall);
				hEtaPhiMinusDigits_wire ->Fill(etaMinusTruth, phiMinusTruth);

			}

                        /// Counting the digits to sTGC wires and MM pads
			nLarge = muonsDigitsEtaMinusMMLargeSector + muonsDigitsEtaMinusTGCLargeSector_pad;
			nSmall = muonsDigitsEtaMinusMMSmallSector + muonsDigitsEtaMinusTGCSmallSector_pad;
			
						
			if ( nLarge >= nSmall )
			{

				hwEtaPhiMinusDigits_pad->Fill(etaMinusTruth, phiMinusTruth, nLarge);
				hEtaPhiMinusDigits_pad ->Fill(etaMinusTruth, phiMinusTruth);

			}else{
				hwEtaPhiMinusDigits_pad->Fill(etaMinusTruth, phiMinusTruth, nSmall);
				hEtaPhiMinusDigits_pad ->Fill(etaMinusTruth, phiMinusTruth);

			}


		}

             

	}// Close for Main loop to the entries of the ntuple 

 
	/***********************************************************
	 *                 Write histos to root file 
	 ***********************************************************/
	 
	valResults->cd(); 
	TCanvas *c1 = new TCanvas("c1","c1",500,500);
	c1->cd();

        /******************************************************************
        *                          For MM sim hits
        ******************************************************************/ 
        hwEtaMinusMM->Sumw2();
	hwEtaMinusMM->Divide(hwEtaMinusMM, hEtaMinusMM,1,1,"B");
        hwEtaMinusMM->GetXaxis()->SetTitle("#eta");
        hwEtaMinusMM->GetXaxis()->SetTitleSize(0.06);
        hwEtaMinusMM->GetXaxis()->SetTitleOffset(0.80);
	hwEtaMinusMM->Write();  
	hwEtaMinusMM->Draw();
        c1->SaveAs("MM_hits_eta_minus.pdf");
	c1->SaveAs("MM_hits_eta_minus.eps");

        hwEtaPlusMM->Sumw2();
	hwEtaPlusMM->Divide(hwEtaPlusMM, hEtaPlusMM,1,1,"B");
        hwEtaPlusMM->GetXaxis()->SetTitle("#eta");
        hwEtaPlusMM->GetXaxis()->SetTitleSize(0.06);
        hwEtaPlusMM->GetXaxis()->SetTitleOffset(0.80);
	hwEtaPlusMM->Write();
	hwEtaPlusMM->Draw();
        c1->SaveAs("MM_hits_eta_plus.pdf");
	c1->SaveAs("MM_hits_eta_plus.eps");

        hDivPhiMinusMM->Sumw2();
	hDivPhiMinusMM->Divide(hDivPhiMinusMM, hPhiMinusMM,1,1,"B");
        hDivPhiMinusMM->SetAxisRange(7., 16.0, "Y");
        hDivPhiMinusMM->GetXaxis()->SetTitle("#phi[rad]");
        hDivPhiMinusMM->GetXaxis()->SetTitleSize(0.06);
        hDivPhiMinusMM->GetXaxis()->SetTitleOffset(0.75);
	hDivPhiMinusMM->Write(); 
	hDivPhiMinusMM->Draw("he");
        c1->SaveAs("MM_hits_phi_for_eta_minus.pdf");
	c1->SaveAs("MM_hits_phi_for_eta_minus.eps");

        hwPhiMinusMM->Divide(hPhiMinusMM); 
        hwPhiMinusMM->GetXaxis()->SetTitle("#phi[rad]");
        hwPhiMinusMM->GetXaxis()->SetTitleSize(0.06);
        hwPhiMinusMM->GetXaxis()->SetTitleOffset(0.75);
	hwPhiMinusMM->Write(); 	
	

        hDivPhiPlusMM->Sumw2();
	hDivPhiPlusMM->Divide(hDivPhiPlusMM, hPhiPlusMM,1,1,"B");
        hDivPhiPlusMM->SetAxisRange(7., 16.0, "Y");
        hDivPhiPlusMM->GetXaxis()->SetTitle("#phi[rad]");
        hDivPhiPlusMM->GetXaxis()->SetTitleSize(0.06);
        hDivPhiPlusMM->GetXaxis()->SetTitleOffset(0.75);
	hDivPhiPlusMM->Write(); 
	hDivPhiPlusMM->Draw("he");
        c1->SaveAs("MM_hits_phi_for_eta_plus.pdf");
	c1->SaveAs("MM_hits_phi_for_eta_plus.eps");


        hwPhiPlusMM->Divide(hPhiPlusMM);
        hwPhiPlusMM->GetXaxis()->SetTitle("#phi[rad]");
        hwPhiPlusMM->GetXaxis()->SetTitleSize(0.06);
        hwPhiPlusMM->GetXaxis()->SetTitleOffset(0.75);
	hwPhiPlusMM->Write(); 
 
	hwEtaPhiPlusMM->Divide(hEtaPhiPlusMM);
	hwEtaPhiPlusMM->SetOption("COLZ");  
	hwEtaPhiPlusMM->SetMarkerColor(1);  
	hwEtaPhiPlusMM->SetMarkerStyle(21);   
	hwEtaPhiPlusMM->SetMarkerSize(0.1);
	hwEtaPhiPlusMM->GetXaxis()->SetTitle("#eta");
	hwEtaPhiPlusMM->GetYaxis()->SetTitle("#phi[rad]"); 
	hwEtaPhiPlusMM->Write(); 	

	hwEtaPhiMinusMM->Divide(hEtaPhiMinusMM);
	hwEtaPhiMinusMM->SetOption("COLZ");  
	hwEtaPhiMinusMM->SetMarkerColor(1);  
	hwEtaPhiMinusMM->SetMarkerStyle(21);   
	hwEtaPhiMinusMM->SetMarkerSize(0.1);
	hwEtaPhiMinusMM->GetXaxis()->SetTitle("#eta");
	hwEtaPhiMinusMM->GetYaxis()->SetTitle("#phi[rad]");
	hwEtaPhiMinusMM->Write();	

	wetaPhiGeneralMM->Divide(etaPhiGeneralMM);
	wetaPhiGeneralMM->SetOption("COLZ");  
	wetaPhiGeneralMM->SetMarkerColor(1);  
	wetaPhiGeneralMM->SetMarkerStyle(21);   
	wetaPhiGeneralMM->SetMarkerSize(0.1);
	wetaPhiGeneralMM->GetXaxis()->SetTitle("#eta");
	wetaPhiGeneralMM->GetYaxis()->SetTitle("#phi[rad]");
	wetaPhiGeneralMM->Write();

        /******************************************************************
        *                          For MM digits
        ******************************************************************/ 

        hwEtaMinusMM_fdg->Sumw2();
	hwEtaMinusMM_fdg->Divide(hwEtaMinusMM_fdg, hEtaMinusMM_fdg,1,1,"B");
        hwEtaMinusMM_fdg->GetXaxis()->SetTitle("#eta");
        hwEtaMinusMM_fdg->GetXaxis()->SetTitleSize(0.06);
        hwEtaMinusMM_fdg->GetXaxis()->SetTitleOffset(0.80);
	hwEtaMinusMM_fdg->Write(); 
	hwEtaMinusMM_fdg->Draw(); 
	c1->SaveAs("MM_digits_eta_minus.pdf");
	c1->SaveAs("MM_digits_eta_minus.eps");

        hwEtaPlusMM_fdg->Sumw2();
	hwEtaPlusMM_fdg->Divide(hwEtaPlusMM_fdg, hEtaPlusMM_fdg,1,1,"B");
        hwEtaPlusMM_fdg->GetXaxis()->SetTitle("#eta");
        hwEtaPlusMM_fdg->GetXaxis()->SetTitleSize(0.06);
        hwEtaPlusMM_fdg->GetXaxis()->SetTitleOffset(0.80);
	hwEtaPlusMM_fdg->Write();
	hwEtaPlusMM_fdg->Draw(); 
	c1->SaveAs("MM_digits_eta_plus.pdf");
	c1->SaveAs("MM_digits_eta_plus.eps");

        hDivPhiMinusMM_fdg->Sumw2();
	hDivPhiMinusMM_fdg->Divide(hDivPhiMinusMM_fdg, hPhiMinusMM_fdg,1,1,"B");
        hDivPhiMinusMM_fdg->SetAxisRange(7., 16.0, "Y");
        hDivPhiMinusMM_fdg->GetXaxis()->SetTitle("#phi[rad]");
        hDivPhiMinusMM_fdg->GetXaxis()->SetTitleSize(0.06);
        hDivPhiMinusMM_fdg->GetXaxis()->SetTitleOffset(0.75);
	hDivPhiMinusMM_fdg->Write(); 
	hDivPhiMinusMM_fdg->Draw("he");
        c1->SaveAs("MM_digits_phi_for_eta_minus.pdf");
	c1->SaveAs("MM_digits_phi_for_eta_minus.eps");

        hwPhiMinusMM_fdg->Divide(hPhiMinusMM_fdg); 
        hwPhiMinusMM_fdg->GetXaxis()->SetTitle("#phi[rad]");
        hwPhiMinusMM_fdg->GetXaxis()->SetTitleSize(0.06);
        hwPhiMinusMM_fdg->GetXaxis()->SetTitleOffset(0.75);
	hwPhiMinusMM_fdg->Write(); 		

        hDivPhiPlusMM_fdg->Sumw2();
	hDivPhiPlusMM_fdg->Divide(hDivPhiPlusMM_fdg, hPhiPlusMM_fdg,1,1,"B");
        hDivPhiPlusMM_fdg->SetAxisRange(7., 16.0, "Y");
        hDivPhiPlusMM_fdg->GetXaxis()->SetTitle("#phi[rad]");
        hDivPhiPlusMM_fdg->GetXaxis()->SetTitleSize(0.06);
        hDivPhiPlusMM_fdg->GetXaxis()->SetTitleOffset(0.75);
	hDivPhiPlusMM_fdg->Write();
	hDivPhiPlusMM_fdg->Draw("he");
        c1->SaveAs("MM_digits_phi_for_eta_plus.pdf");
	c1->SaveAs("MM_digits_phi_for_eta_plus.eps"); 

        hwPhiPlusMM_fdg->Divide(hPhiPlusMM_fdg);
        hwPhiPlusMM_fdg->GetXaxis()->SetTitle("#phi[rad]");
        hwPhiPlusMM_fdg->GetXaxis()->SetTitleSize(0.06);
        hwPhiPlusMM_fdg->GetXaxis()->SetTitleOffset(0.75);
	hwPhiPlusMM_fdg->Write(); 
 
	hwEtaPhiPlusMM_fdg->Divide(hEtaPhiPlusMM_fdg);
	hwEtaPhiPlusMM_fdg->SetOption("COLZ");  
	hwEtaPhiPlusMM_fdg->SetMarkerColor(1);  
	hwEtaPhiPlusMM_fdg->SetMarkerStyle(21);   
	hwEtaPhiPlusMM_fdg->SetMarkerSize(0.1);
	hwEtaPhiPlusMM_fdg->GetXaxis()->SetTitle("#eta");
	hwEtaPhiPlusMM_fdg->GetYaxis()->SetTitle("#phi[rad]"); 
	hwEtaPhiPlusMM_fdg->Write(); 	

	hwEtaPhiMinusMM_fdg->Divide(hEtaPhiMinusMM_fdg);
	hwEtaPhiMinusMM_fdg->SetOption("COLZ");  
	hwEtaPhiMinusMM_fdg->SetMarkerColor(1);  
	hwEtaPhiMinusMM_fdg->SetMarkerStyle(21);   
	hwEtaPhiMinusMM_fdg->SetMarkerSize(0.1);
	hwEtaPhiMinusMM_fdg->GetXaxis()->SetTitle("#eta");
	hwEtaPhiMinusMM_fdg->GetYaxis()->SetTitle("#phi[rad]");
	hwEtaPhiMinusMM_fdg->Write();	

	wetaPhiGeneralMM_fdg->Divide(etaPhiGeneralMM_fdg);
	wetaPhiGeneralMM_fdg->SetOption("COLZ");  
	wetaPhiGeneralMM_fdg->SetMarkerColor(1);  
	wetaPhiGeneralMM_fdg->SetMarkerStyle(21);   
	wetaPhiGeneralMM_fdg->SetMarkerSize(0.1);
	wetaPhiGeneralMM_fdg->GetXaxis()->SetTitle("#eta");
	wetaPhiGeneralMM_fdg->GetYaxis()->SetTitle("#phi[rad]");
	wetaPhiGeneralMM_fdg->Write();

       /******************************************************************
       *                          For sTGC sim hits
       ******************************************************************/ 	
        hwEtaMinusTGC->Sumw2();
	hwEtaMinusTGC->Divide(hwEtaMinusTGC, hEtaMinusTGC,1,1,"B");
        hwEtaMinusTGC->GetXaxis()->SetTitle("#eta");
        hwEtaMinusTGC->GetXaxis()->SetTitleSize(0.06);
        hwEtaMinusTGC->GetXaxis()->SetTitleOffset(0.80);
	hwEtaMinusTGC->Write();  
	hwEtaMinusTGC->Draw(); 
	c1->SaveAs("sTGC_hits_eta_minus.pdf");
	c1->SaveAs("sTGC_hits_eta_minus.eps");

        hwEtaPlusTGC->Sumw2();
	hwEtaPlusTGC->Divide(hwEtaPlusTGC, hEtaPlusTGC,1,1,"B");
        hwEtaPlusTGC->GetXaxis()->SetTitle("#eta");
        hwEtaPlusTGC->GetXaxis()->SetTitleSize(0.06);
        hwEtaPlusTGC->GetXaxis()->SetTitleOffset(0.80);
	hwEtaPlusTGC->Write();
	hwEtaPlusTGC->Draw(); 
	c1->SaveAs("sTGC_hits_eta_plus.pdf");
	c1->SaveAs("sTGC_hits_eta_plus.eps");

        hDivPhiMinusTGC->Sumw2();
	hDivPhiMinusTGC->Divide(hDivPhiMinusTGC,hPhiMinusTGC,1,1,"B");
        hDivPhiMinusTGC->SetAxisRange(7., 16.0, "Y");
        hDivPhiMinusTGC->GetXaxis()->SetTitle("#phi[rad]");
        hDivPhiMinusTGC->GetXaxis()->SetTitleSize(0.06);
        hDivPhiMinusTGC->GetXaxis()->SetTitleOffset(0.75); 
	hDivPhiMinusTGC->Write(); 
	hDivPhiMinusTGC->Draw("he");
        c1->SaveAs("sTGC_hits_phi_for_eta_minus.pdf");
	c1->SaveAs("sTGC_hits_phi_for_eta_minus.eps"); 

	hwPhiMinusTGC->Divide(hPhiMinusTGC);
        hwPhiMinusTGC->GetXaxis()->SetTitle("#phi[rad]"); 
        hwPhiMinusTGC->GetXaxis()->SetTitleSize(0.06);
        hwPhiMinusTGC->GetXaxis()->SetTitleOffset(0.75); 
	hwPhiMinusTGC->Write(); 

        hDivPhiPlusTGC->Sumw2();
	hDivPhiPlusTGC->Divide(hDivPhiPlusTGC,hPhiPlusTGC,1,1,"B");
        hDivPhiPlusTGC->SetAxisRange(7., 16.0, "Y"); 
        hDivPhiPlusTGC->GetXaxis()->SetTitle("#phi[rad]");
        hDivPhiPlusTGC->GetXaxis()->SetTitleSize(0.06);
        hDivPhiPlusTGC->GetXaxis()->SetTitleOffset(0.75); 
	hDivPhiPlusTGC->Write(); 
	hDivPhiPlusTGC->Draw("he");
        c1->SaveAs("sTGC_hits_phi_for_eta_plus.pdf");
	c1->SaveAs("sTGC_hits_phi_for_eta_plus.eps"); 

	hwPhiPlusTGC->Divide(hPhiPlusTGC);
        hwPhiPlusTGC->GetXaxis()->SetTitle("#phi[rad]");
        hwPhiPlusTGC->GetXaxis()->SetTitleSize(0.06);
        hwPhiPlusTGC->GetXaxis()->SetTitleOffset(0.75); 
	hwPhiPlusTGC->Write();

	hwEtaPhiPlusTGC->Divide(hEtaPhiPlusTGC);
	hwEtaPhiPlusTGC->SetOption("COLZ");  
	hwEtaPhiPlusTGC->SetMarkerColor(1);  
	hwEtaPhiPlusTGC->SetMarkerStyle(21);   
	hwEtaPhiPlusTGC->SetMarkerSize(0.1);
	hwEtaPhiPlusTGC->GetXaxis()->SetTitle("#eta");
	hwEtaPhiPlusTGC->GetYaxis()->SetTitle("#phi[rad]");
	hwEtaPhiPlusTGC->Write(); 	

	hwEtaPhiMinusTGC->Divide(hEtaPhiMinusTGC);
	hwEtaPhiMinusTGC->SetOption("COLZ");  
	hwEtaPhiMinusTGC->SetMarkerColor(1);  
	hwEtaPhiMinusTGC->SetMarkerStyle(21);   
	hwEtaPhiMinusTGC->SetMarkerSize(0.1);
	hwEtaPhiMinusTGC->GetXaxis()->SetTitle("#eta");
	hwEtaPhiMinusTGC->GetYaxis()->SetTitle("#phi[rad]");
	hwEtaPhiMinusTGC->Write();	

	wetaPhiGeneralTGC->Divide(etaPhiGeneralTGC);
	wetaPhiGeneralTGC->SetOption("COLZ");  
	wetaPhiGeneralTGC->SetMarkerColor(1);  
	wetaPhiGeneralTGC->SetMarkerStyle(21);   
	wetaPhiGeneralTGC->SetMarkerSize(0.1);
	wetaPhiGeneralTGC->GetXaxis()->SetTitle("#eta");
	wetaPhiGeneralTGC->GetYaxis()->SetTitle("#phi[rad]");
	wetaPhiGeneralTGC->Write();
	

      /******************************************************************
       *                          For sTGC digits
       ******************************************************************/ 

        // For pads	
        hwEtaMinusTGC_fdg_pad->Sumw2();
	hwEtaMinusTGC_fdg_pad->Divide(hwEtaMinusTGC_fdg_pad, hEtaMinusTGC_fdg_pad,1,1,"B");
        hwEtaMinusTGC_fdg_pad->GetXaxis()->SetTitle("#eta");
        hwEtaMinusTGC_fdg_pad->GetXaxis()->SetTitleSize(0.06);
        hwEtaMinusTGC_fdg_pad->GetXaxis()->SetTitleOffset(0.80); 
	hwEtaMinusTGC_fdg_pad->Write();  
	hwEtaMinusTGC_fdg_pad->Draw(); 
	c1->SaveAs("sTGC_digits_pads_eta_minus.pdf");
	c1->SaveAs("sTGC_digits_pads_eta_minus.eps");

        hwEtaPlusTGC_fdg_pad->Sumw2();
	hwEtaPlusTGC_fdg_pad->Divide(hwEtaPlusTGC_fdg_pad, hEtaPlusTGC_fdg_pad,1,1,"B");
        hwEtaPlusTGC_fdg_pad->GetXaxis()->SetTitle("#eta");
        hwEtaPlusTGC_fdg_pad->GetXaxis()->SetTitleSize(0.06);
        hwEtaPlusTGC_fdg_pad->GetXaxis()->SetTitleOffset(0.80); 
	hwEtaPlusTGC_fdg_pad->Write();
	hwEtaPlusTGC_fdg_pad->Draw(); 
	c1->SaveAs("sTGC_digits_pads_eta_plus.pdf");
	c1->SaveAs("sTGC_digits_pads_eta_plus.eps");

        hDivPhiMinusTGC_fdg_pad->Sumw2();
	hDivPhiMinusTGC_fdg_pad->Divide(hDivPhiMinusTGC_fdg_pad,hPhiMinusTGC_fdg_pad,1,1,"B");
        hDivPhiMinusTGC_fdg_pad->SetAxisRange(7., 16.0, "Y");
        hDivPhiMinusTGC_fdg_pad->GetXaxis()->SetTitle("#phi[rad]"); 
        hDivPhiMinusTGC_fdg_pad->GetXaxis()->SetTitleSize(0.06);
        hDivPhiMinusTGC_fdg_pad->GetXaxis()->SetTitleOffset(0.75); 
	hDivPhiMinusTGC_fdg_pad->Write(); 
	hDivPhiMinusTGC_fdg_pad->Draw("he");
        c1->SaveAs("sTGC_digits_pads_phi_for_eta_minus.pdf");
	c1->SaveAs("sTGC_digits_pads_phi_for_eta_minus.eps"); 

	hwPhiMinusTGC_fdg_pad->Divide(hPhiMinusTGC_fdg_pad);
        hwPhiMinusTGC_fdg_pad->GetXaxis()->SetTitle("#phi[rad]"); 
        hwPhiMinusTGC_fdg_pad->GetXaxis()->SetTitleSize(0.06);
        hwPhiMinusTGC_fdg_pad->GetXaxis()->SetTitleOffset(0.75); 
	hwPhiMinusTGC_fdg_pad->Write(); 

        hDivPhiPlusTGC_fdg_pad->Sumw2();
	hDivPhiPlusTGC_fdg_pad->Divide(hDivPhiPlusTGC_fdg_pad,hPhiPlusTGC_fdg_pad,1,1,"B");
        hDivPhiPlusTGC_fdg_pad->SetAxisRange(7., 16.0, "Y"); 
        hDivPhiPlusTGC_fdg_pad->GetXaxis()->SetTitle("#phi[rad]");
        hDivPhiPlusTGC_fdg_pad->GetXaxis()->SetTitleSize(0.06);
        hDivPhiPlusTGC_fdg_pad->GetXaxis()->SetTitleOffset(0.75); 
	hDivPhiPlusTGC_fdg_pad->Write();
	hDivPhiPlusTGC_fdg_pad->Draw("he");
        c1->SaveAs("sTGC_digits_pads_phi_for_eta_plus.pdf");
	c1->SaveAs("sTGC_digits_pads_phi_for_eta_plus.eps");  

	hwPhiPlusTGC_fdg_pad->Divide(hPhiPlusTGC_fdg_pad);
        hwPhiPlusTGC_fdg_pad->GetXaxis()->SetTitle("#phi[rad]");
        hwPhiPlusTGC_fdg_pad->GetXaxis()->SetTitleSize(0.06);
        hwPhiPlusTGC_fdg_pad->GetXaxis()->SetTitleOffset(0.75);
	hwPhiPlusTGC_fdg_pad->Write();

	hwEtaPhiPlusTGC_fdg_pad->Divide(hEtaPhiPlusTGC_fdg_pad);
	hwEtaPhiPlusTGC_fdg_pad->SetOption("COLZ");  
	hwEtaPhiPlusTGC_fdg_pad->SetMarkerColor(1);  
	hwEtaPhiPlusTGC_fdg_pad->SetMarkerStyle(21);   
	hwEtaPhiPlusTGC_fdg_pad->SetMarkerSize(0.1);
	hwEtaPhiPlusTGC_fdg_pad->GetXaxis()->SetTitle("#eta");
	hwEtaPhiPlusTGC_fdg_pad->GetYaxis()->SetTitle("#phi[rad]");
	hwEtaPhiPlusTGC_fdg_pad->Write(); 	

	hwEtaPhiMinusTGC_fdg_pad->Divide(hEtaPhiMinusTGC_fdg_pad);
	hwEtaPhiMinusTGC_fdg_pad->SetOption("COLZ");  
	hwEtaPhiMinusTGC_fdg_pad->SetMarkerColor(1);  
	hwEtaPhiMinusTGC_fdg_pad->SetMarkerStyle(21);   
	hwEtaPhiMinusTGC_fdg_pad->SetMarkerSize(0.1);
	hwEtaPhiMinusTGC_fdg_pad->GetXaxis()->SetTitle("#eta");
	hwEtaPhiMinusTGC_fdg_pad->GetYaxis()->SetTitle("#phi[rad]");
	hwEtaPhiMinusTGC_fdg_pad->Write();	

	wetaPhiGeneralTGC_fdg_pad->Divide(etaPhiGeneralTGC_fdg_pad);
	wetaPhiGeneralTGC_fdg_pad->SetOption("COLZ");  
	wetaPhiGeneralTGC_fdg_pad->SetMarkerColor(1);  
	wetaPhiGeneralTGC_fdg_pad->SetMarkerStyle(21);   
	wetaPhiGeneralTGC_fdg_pad->SetMarkerSize(0.1);
	wetaPhiGeneralTGC_fdg_pad->GetXaxis()->SetTitle("#eta");
	wetaPhiGeneralTGC_fdg_pad->GetYaxis()->SetTitle("#phi[rad]");
	wetaPhiGeneralTGC_fdg_pad->Write();

        // For strips	
        hwEtaMinusTGC_fdg_strip->Sumw2();
	hwEtaMinusTGC_fdg_strip->Divide(hwEtaMinusTGC_fdg_strip, hEtaMinusTGC_fdg_strip,1,1,"B");
        hwEtaMinusTGC_fdg_strip->GetXaxis()->SetTitle("#eta");
        hwEtaMinusTGC_fdg_strip->GetXaxis()->SetTitleSize(0.06);
        hwEtaMinusTGC_fdg_strip->GetXaxis()->SetTitleOffset(0.80);
	hwEtaMinusTGC_fdg_strip->Write();  
	hwEtaMinusTGC_fdg_strip->Draw();
        c1->SaveAs("sTGC_digits_strips_eta_minus.pdf");
	c1->SaveAs("sTGC_digits_strips_eta_minus.eps"); 

        hwEtaPlusTGC_fdg_strip->Sumw2();
	hwEtaPlusTGC_fdg_strip->Divide(hwEtaPlusTGC_fdg_strip, hEtaPlusTGC_fdg_strip,1,1,"B");
        hwEtaPlusTGC_fdg_strip->GetXaxis()->SetTitle("#eta");
        hwEtaPlusTGC_fdg_strip->GetXaxis()->SetTitleSize(0.06);
        hwEtaPlusTGC_fdg_strip->GetXaxis()->SetTitleOffset(0.80);
	hwEtaPlusTGC_fdg_strip->Write();
	hwEtaPlusTGC_fdg_strip->Draw();
        c1->SaveAs("sTGC_digits_strips_eta_plus.pdf");
	c1->SaveAs("sTGC_digits_strips_eta_plus.eps"); 

        hDivPhiMinusTGC_fdg_strip->Sumw2();
	hDivPhiMinusTGC_fdg_strip->Divide(hDivPhiMinusTGC_fdg_strip,hPhiMinusTGC_fdg_strip,1,1,"B");
        hDivPhiMinusTGC_fdg_strip->SetAxisRange(7., 16.0, "Y");
        hDivPhiMinusTGC_fdg_strip->GetXaxis()->SetTitle("#phi[rad]"); 
        hDivPhiMinusTGC_fdg_strip->GetXaxis()->SetTitleSize(0.06);
        hDivPhiMinusTGC_fdg_strip->GetXaxis()->SetTitleOffset(0.75);
	hDivPhiMinusTGC_fdg_strip->Write(); 
	hDivPhiMinusTGC_fdg_strip->Draw("he");
        c1->SaveAs("sTGC_digits_strips_phi_for_eta_minus.pdf");
	c1->SaveAs("sTGC_digits_strips_phi_for_eta_minus.eps");

	hwPhiMinusTGC_fdg_strip->Divide(hPhiMinusTGC_fdg_strip);
        hwPhiMinusTGC_fdg_strip->GetXaxis()->SetTitle("#phi[rad]"); 
        hwPhiMinusTGC_fdg_strip->GetXaxis()->SetTitleSize(0.06);
        hwPhiMinusTGC_fdg_strip->GetXaxis()->SetTitleOffset(0.75);
	hwPhiMinusTGC_fdg_strip->Write(); 

        hDivPhiPlusTGC_fdg_strip->Sumw2();
	hDivPhiPlusTGC_fdg_strip->Divide(hDivPhiPlusTGC_fdg_strip,hPhiPlusTGC_fdg_strip,1,1,"B");
        hDivPhiPlusTGC_fdg_strip->SetAxisRange(7., 16.0, "Y"); 
        hDivPhiPlusTGC_fdg_strip->GetXaxis()->SetTitle("#phi[rad]");
        hDivPhiPlusTGC_fdg_strip->GetXaxis()->SetTitleSize(0.06);
        hDivPhiPlusTGC_fdg_strip->GetXaxis()->SetTitleOffset(0.75);
	hDivPhiPlusTGC_fdg_strip->Write(); 
	hDivPhiPlusTGC_fdg_strip->Draw("he");
        c1->SaveAs("sTGC_digits_strips_phi_for_eta_plus.pdf");
	c1->SaveAs("sTGC_digits_strips_phi_for_eta_plus.eps");

	hwPhiPlusTGC_fdg_strip->Divide(hPhiPlusTGC_fdg_strip);
        hwPhiPlusTGC_fdg_strip->GetXaxis()->SetTitle("#phi[rad]");
        hwPhiPlusTGC_fdg_strip->GetXaxis()->SetTitleSize(0.06);
        hwPhiPlusTGC_fdg_strip->GetXaxis()->SetTitleOffset(0.75);
	hwPhiPlusTGC_fdg_strip->Write();

	hwEtaPhiPlusTGC_fdg_strip->Divide(hEtaPhiPlusTGC_fdg_strip);
	hwEtaPhiPlusTGC_fdg_strip->SetOption("COLZ");  
	hwEtaPhiPlusTGC_fdg_strip->SetMarkerColor(1);  
	hwEtaPhiPlusTGC_fdg_strip->SetMarkerStyle(21);   
	hwEtaPhiPlusTGC_fdg_strip->SetMarkerSize(0.1);
	hwEtaPhiPlusTGC_fdg_strip->GetXaxis()->SetTitle("#eta");
	hwEtaPhiPlusTGC_fdg_strip->GetYaxis()->SetTitle("#phi[rad]");
	hwEtaPhiPlusTGC_fdg_strip->Write(); 	

	hwEtaPhiMinusTGC_fdg_strip->Divide(hEtaPhiMinusTGC_fdg_strip);
	hwEtaPhiMinusTGC_fdg_strip->SetOption("COLZ");  
	hwEtaPhiMinusTGC_fdg_strip->SetMarkerColor(1);  
	hwEtaPhiMinusTGC_fdg_strip->SetMarkerStyle(21);   
	hwEtaPhiMinusTGC_fdg_strip->SetMarkerSize(0.1);
	hwEtaPhiMinusTGC_fdg_strip->GetXaxis()->SetTitle("#eta");
	hwEtaPhiMinusTGC_fdg_strip->GetYaxis()->SetTitle("#phi[rad]");
	hwEtaPhiMinusTGC_fdg_strip->Write();	

	wetaPhiGeneralTGC_fdg_strip->Divide(etaPhiGeneralTGC_fdg_strip);
	wetaPhiGeneralTGC_fdg_strip->SetOption("COLZ");  
	wetaPhiGeneralTGC_fdg_strip->SetMarkerColor(1);  
	wetaPhiGeneralTGC_fdg_strip->SetMarkerStyle(21);   
	wetaPhiGeneralTGC_fdg_strip->SetMarkerSize(0.1);
	wetaPhiGeneralTGC_fdg_strip->GetXaxis()->SetTitle("#eta");
	wetaPhiGeneralTGC_fdg_strip->GetYaxis()->SetTitle("#phi[rad]");
	wetaPhiGeneralTGC_fdg_strip->Write();

        // For wires	
        hwEtaMinusTGC_fdg_wire->Sumw2();
	hwEtaMinusTGC_fdg_wire->Divide(hwEtaMinusTGC_fdg_wire, hEtaMinusTGC_fdg_wire,1,1,"B");
        hwEtaMinusTGC_fdg_wire->GetXaxis()->SetTitle("#eta");
        hwEtaMinusTGC_fdg_wire->GetXaxis()->SetTitleSize(0.06);
        hwEtaMinusTGC_fdg_wire->GetXaxis()->SetTitleOffset(0.80);
	hwEtaMinusTGC_fdg_wire->Write();  
	hwEtaMinusTGC_fdg_wire->Draw();
        c1->SaveAs("sTGC_digits_wires_eta_minus.pdf");
	c1->SaveAs("sTGC_digits_wires_eta_minus.eps"); 

        hwEtaPlusTGC_fdg_wire->Sumw2();
	hwEtaPlusTGC_fdg_wire->Divide(hwEtaPlusTGC_fdg_wire, hEtaPlusTGC_fdg_wire,1,1,"B");
        hwEtaPlusTGC_fdg_wire->GetXaxis()->SetTitle("#eta");
        hwEtaPlusTGC_fdg_wire->GetXaxis()->SetTitleSize(0.06);
        hwEtaPlusTGC_fdg_wire->GetXaxis()->SetTitleOffset(0.80);
	hwEtaPlusTGC_fdg_wire->Write();
	hwEtaPlusTGC_fdg_wire->Draw();
        c1->SaveAs("sTGC_digits_wires_eta_plus.pdf");
	c1->SaveAs("sTGC_digits_wires_eta_plus.eps"); 

        hDivPhiMinusTGC_fdg_wire->Sumw2();
	hDivPhiMinusTGC_fdg_wire->Divide(hDivPhiMinusTGC_fdg_wire,hPhiMinusTGC_fdg_wire,1,1,"B");
        hDivPhiMinusTGC_fdg_wire->SetAxisRange(7., 16.0, "Y");
        hDivPhiMinusTGC_fdg_wire->GetXaxis()->SetTitle("#phi[rad]"); 
        hDivPhiMinusTGC_fdg_wire->GetXaxis()->SetTitleSize(0.06);
        hDivPhiMinusTGC_fdg_wire->GetXaxis()->SetTitleOffset(0.75);
	hDivPhiMinusTGC_fdg_wire->Write(); 
	hDivPhiMinusTGC_fdg_wire->Draw("he");
        c1->SaveAs("sTGC_digits_wires_phi_for_eta_minus.pdf");
	c1->SaveAs("sTGC_digits_wires_phi_for_eta_minus.eps");

	hwPhiMinusTGC_fdg_wire->Divide(hPhiMinusTGC_fdg_wire);
        hwPhiMinusTGC_fdg_wire->GetXaxis()->SetTitle("#phi[rad]"); 
        hwPhiMinusTGC_fdg_wire->GetXaxis()->SetTitleSize(0.06);
        hwPhiMinusTGC_fdg_wire->GetXaxis()->SetTitleOffset(0.75);
	hwPhiMinusTGC_fdg_wire->Write(); 

        hDivPhiPlusTGC_fdg_wire->Sumw2();
	hDivPhiPlusTGC_fdg_wire->Divide(hDivPhiPlusTGC_fdg_wire,hPhiPlusTGC_fdg_wire,1,1,"B");
        hDivPhiPlusTGC_fdg_wire->SetAxisRange(7., 16.0, "Y"); 
        hDivPhiPlusTGC_fdg_wire->GetXaxis()->SetTitle("#phi[rad]");
        hDivPhiPlusTGC_fdg_wire->GetXaxis()->SetTitleSize(0.06);
        hDivPhiPlusTGC_fdg_wire->GetXaxis()->SetTitleOffset(0.75);
	hDivPhiPlusTGC_fdg_wire->Write(); 
	hDivPhiPlusTGC_fdg_wire->Draw("he");
        c1->SaveAs("sTGC_digits_wires_phi_for_eta_plus.pdf");
	c1->SaveAs("sTGC_digits_wires_phi_for_eta_plus.eps");

	hwPhiPlusTGC_fdg_wire->Divide(hPhiPlusTGC_fdg_wire);
        hwPhiPlusTGC_fdg_wire->GetXaxis()->SetTitle("#phi[rad]");
        hwPhiPlusTGC_fdg_wire->GetXaxis()->SetTitleSize(0.06);
        hwPhiPlusTGC_fdg_wire->GetXaxis()->SetTitleOffset(0.75);
	hwPhiPlusTGC_fdg_wire->Write();

	hwEtaPhiPlusTGC_fdg_wire->Divide(hEtaPhiPlusTGC_fdg_wire);
	hwEtaPhiPlusTGC_fdg_wire->SetOption("COLZ");  
	hwEtaPhiPlusTGC_fdg_wire->SetMarkerColor(1);  
	hwEtaPhiPlusTGC_fdg_wire->SetMarkerStyle(21);   
	hwEtaPhiPlusTGC_fdg_wire->SetMarkerSize(0.1);
	hwEtaPhiPlusTGC_fdg_wire->GetXaxis()->SetTitle("#eta");
	hwEtaPhiPlusTGC_fdg_wire->GetYaxis()->SetTitle("#phi[rad]");
	hwEtaPhiPlusTGC_fdg_wire->Write(); 	

	hwEtaPhiMinusTGC_fdg_wire->Divide(hEtaPhiMinusTGC_fdg_wire);
	hwEtaPhiMinusTGC_fdg_wire->SetOption("COLZ");  
	hwEtaPhiMinusTGC_fdg_wire->SetMarkerColor(1);  
	hwEtaPhiMinusTGC_fdg_wire->SetMarkerStyle(21);   
	hwEtaPhiMinusTGC_fdg_wire->SetMarkerSize(0.1);
	hwEtaPhiMinusTGC_fdg_wire->GetXaxis()->SetTitle("#eta");
	hwEtaPhiMinusTGC_fdg_wire->GetYaxis()->SetTitle("#phi[rad]");
	hwEtaPhiMinusTGC_fdg_wire->Write();	

	wetaPhiGeneralTGC_fdg_wire->Divide(etaPhiGeneralTGC_fdg_wire);
	wetaPhiGeneralTGC_fdg_wire->SetOption("COLZ");  
	wetaPhiGeneralTGC_fdg_wire->SetMarkerColor(1);  
	wetaPhiGeneralTGC_fdg_wire->SetMarkerStyle(21);   
	wetaPhiGeneralTGC_fdg_wire->SetMarkerSize(0.1);
	wetaPhiGeneralTGC_fdg_wire->GetXaxis()->SetTitle("#eta");
	wetaPhiGeneralTGC_fdg_wire->GetYaxis()->SetTitle("#phi[rad]");
	wetaPhiGeneralTGC_fdg_wire->Write();


        /********************************************************************* 
	*  			Compined Plots 
	**********************************************************************/
/*	hwEtaPhiPlusSmallTGGLargeMM->Divide(hEtaPhiPlusSmallTGGLargeMM);
	hwEtaPhiPlusSmallTGGLargeMM->SetOption("COLZ");  
	hwEtaPhiPlusSmallTGGLargeMM->SetMarkerColor(1);  
	hwEtaPhiPlusSmallTGGLargeMM->SetMarkerStyle(21);   
	hwEtaPhiPlusSmallTGGLargeMM->SetMarkerSize(0.1);
	hwEtaPhiPlusSmallTGGLargeMM->GetXaxis()->SetTitle("#eta");
	hwEtaPhiPlusSmallTGGLargeMM->GetYaxis()->SetTitle("#phi[rad]");
	hwEtaPhiPlusSmallTGGLargeMM->Write(); 

	hwEtaPhiPlusSmallMMLargeTGC->Divide(hEtaPhiPlusSmallMMLargeTGC);
	hwEtaPhiPlusSmallMMLargeTGC->SetOption("COLZ");  
	hwEtaPhiPlusSmallMMLargeTGC->SetMarkerColor(1);  
	hwEtaPhiPlusSmallMMLargeTGC->SetMarkerStyle(21);   
	hwEtaPhiPlusSmallMMLargeTGC->SetMarkerSize(0.1);
	hwEtaPhiPlusSmallMMLargeTGC->GetXaxis()->SetTitle("#eta");
	hwEtaPhiPlusSmallMMLargeTGC->GetYaxis()->SetTitle("#phi[rad]");
	hwEtaPhiPlusSmallMMLargeTGC->Write(); 

	hwEtaPhiMinusSmallTGGLargeMM->Divide(hEtaPhiMinusSmallTGGLargeMM);
	hwEtaPhiMinusSmallTGGLargeMM->SetOption("COLZ");  
	hwEtaPhiMinusSmallTGGLargeMM->SetMarkerColor(1);  
	hwEtaPhiMinusSmallTGGLargeMM->SetMarkerStyle(21);   
	hwEtaPhiMinusSmallTGGLargeMM->SetMarkerSize(0.1);
	hwEtaPhiMinusSmallTGGLargeMM->GetXaxis()->SetTitle("#eta");
	hwEtaPhiMinusSmallTGGLargeMM->GetYaxis()->SetTitle("#phi[rad]");
	hwEtaPhiMinusSmallTGGLargeMM->Write(); 
	 
	hwEtaPhiMinusSmallMMLargeTGC->Divide(hEtaPhiMinusSmallMMLargeTGC);
	hwEtaPhiMinusSmallMMLargeTGC->SetOption("COLZ");  
	hwEtaPhiMinusSmallMMLargeTGC->SetMarkerColor(1);  
	hwEtaPhiMinusSmallMMLargeTGC->SetMarkerStyle(21);   
	hwEtaPhiMinusSmallMMLargeTGC->SetMarkerSize(0.1);
	hwEtaPhiMinusSmallMMLargeTGC->GetXaxis()->SetTitle("#eta");
	hwEtaPhiMinusSmallMMLargeTGC->GetYaxis()->SetTitle("#phi[rad]");
	hwEtaPhiMinusSmallMMLargeTGC->Write();  
	 
	


	/// Compined Digits (MM - sTGC pads)

	hwEtaPhiPlusSmallTGGLargeMM_pad->Divide(hEtaPhiPlusSmallTGGLargeMM_pad);
	hwEtaPhiPlusSmallTGGLargeMM_pad->SetOption("COLZ");  
	hwEtaPhiPlusSmallTGGLargeMM_pad->SetMarkerColor(1);  
	hwEtaPhiPlusSmallTGGLargeMM_pad->SetMarkerStyle(21);   
	hwEtaPhiPlusSmallTGGLargeMM_pad->SetMarkerSize(0.1);
	hwEtaPhiPlusSmallTGGLargeMM_pad->GetXaxis()->SetTitle("#eta");
	hwEtaPhiPlusSmallTGGLargeMM_pad->GetYaxis()->SetTitle("#phi[rad]");
	hwEtaPhiPlusSmallTGGLargeMM_pad->Write(); 

	hwEtaPhiPlusSmallMMLargeTGC_pad ->Divide(hEtaPhiPlusSmallMMLargeTGC_pad );
	hwEtaPhiPlusSmallMMLargeTGC_pad ->SetOption("COLZ");  
	hwEtaPhiPlusSmallMMLargeTGC_pad ->SetMarkerColor(1);  
	hwEtaPhiPlusSmallMMLargeTGC_pad ->SetMarkerStyle(21);   
	hwEtaPhiPlusSmallMMLargeTGC_pad ->SetMarkerSize(0.1);
	hwEtaPhiPlusSmallMMLargeTGC_pad ->GetXaxis()->SetTitle("#eta");
	hwEtaPhiPlusSmallMMLargeTGC_pad ->GetYaxis()->SetTitle("#phi[rad]");
	hwEtaPhiPlusSmallMMLargeTGC_pad ->Write(); 

	hwEtaPhiMinusSmallTGGLargeMM_pad->Divide(hEtaPhiMinusSmallTGGLargeMM_pad);
	hwEtaPhiMinusSmallTGGLargeMM_pad->SetOption("COLZ");  
	hwEtaPhiMinusSmallTGGLargeMM_pad->SetMarkerColor(1);  
	hwEtaPhiMinusSmallTGGLargeMM_pad->SetMarkerStyle(21);   
	hwEtaPhiMinusSmallTGGLargeMM_pad->SetMarkerSize(0.1);
	hwEtaPhiMinusSmallTGGLargeMM_pad->GetXaxis()->SetTitle("#eta");
	hwEtaPhiMinusSmallTGGLargeMM_pad->GetYaxis()->SetTitle("#phi[rad]");
	hwEtaPhiMinusSmallTGGLargeMM_pad->Write(); 

	hwEtaPhiMinusSmallMMLargeTGC_pad->Divide(hEtaPhiMinusSmallMMLargeTGC_pad);
	hwEtaPhiMinusSmallMMLargeTGC_pad->SetOption("COLZ");  
	hwEtaPhiMinusSmallMMLargeTGC_pad->SetMarkerColor(1);  
	hwEtaPhiMinusSmallMMLargeTGC_pad->SetMarkerStyle(21);   
	hwEtaPhiMinusSmallMMLargeTGC_pad->SetMarkerSize(0.1);
	hwEtaPhiMinusSmallMMLargeTGC_pad->GetXaxis()->SetTitle("#eta");
	hwEtaPhiMinusSmallMMLargeTGC_pad->GetYaxis()->SetTitle("#phi[rad]");
	hwEtaPhiMinusSmallMMLargeTGC_pad->Write(); 


	/// Compined Digits (MM - sTGC strip)
	hwEtaPhiPlusSmallTGGLargeMM_strip->Divide(hEtaPhiPlusSmallTGGLargeMM_strip);
	hwEtaPhiPlusSmallTGGLargeMM_strip->SetOption("COLZ");  
	hwEtaPhiPlusSmallTGGLargeMM_strip->SetMarkerColor(1);  
	hwEtaPhiPlusSmallTGGLargeMM_strip->SetMarkerStyle(21);   
	hwEtaPhiPlusSmallTGGLargeMM_strip->SetMarkerSize(0.1);
	hwEtaPhiPlusSmallTGGLargeMM_strip->GetXaxis()->SetTitle("#eta");
	hwEtaPhiPlusSmallTGGLargeMM_strip->GetYaxis()->SetTitle("#phi[rad]");
	hwEtaPhiPlusSmallTGGLargeMM_strip->Write(); 

	hwEtaPhiPlusSmallMMLargeTGC_strip ->Divide(hEtaPhiPlusSmallMMLargeTGC_strip );
	hwEtaPhiPlusSmallMMLargeTGC_strip ->SetOption("COLZ");  
	hwEtaPhiPlusSmallMMLargeTGC_strip ->SetMarkerColor(1);  
	hwEtaPhiPlusSmallMMLargeTGC_strip ->SetMarkerStyle(21);   
	hwEtaPhiPlusSmallMMLargeTGC_strip ->SetMarkerSize(0.1);
	hwEtaPhiPlusSmallMMLargeTGC_strip ->GetXaxis()->SetTitle("#eta");
	hwEtaPhiPlusSmallMMLargeTGC_strip ->GetYaxis()->SetTitle("#phi[rad]");
	hwEtaPhiPlusSmallMMLargeTGC_strip ->Write(); 

	hwEtaPhiMinusSmallTGGLargeMM_strip->Divide(hEtaPhiMinusSmallTGGLargeMM_strip);
	hwEtaPhiMinusSmallTGGLargeMM_strip->SetOption("COLZ");  
	hwEtaPhiMinusSmallTGGLargeMM_strip->SetMarkerColor(1);  
	hwEtaPhiMinusSmallTGGLargeMM_strip->SetMarkerStyle(21);   
	hwEtaPhiMinusSmallTGGLargeMM_strip->SetMarkerSize(0.1);
	hwEtaPhiMinusSmallTGGLargeMM_strip->GetXaxis()->SetTitle("#eta");
	hwEtaPhiMinusSmallTGGLargeMM_strip->GetYaxis()->SetTitle("#phi[rad]");
	hwEtaPhiMinusSmallTGGLargeMM_strip->Write(); 

	hwEtaPhiMinusSmallMMLargeTGC_strip->Divide(hEtaPhiMinusSmallMMLargeTGC_strip);
	hwEtaPhiMinusSmallMMLargeTGC_strip->SetOption("COLZ");  
	hwEtaPhiMinusSmallMMLargeTGC_strip->SetMarkerColor(1);  
	hwEtaPhiMinusSmallMMLargeTGC_strip->SetMarkerStyle(21);   
	hwEtaPhiMinusSmallMMLargeTGC_strip->SetMarkerSize(0.1);
	hwEtaPhiMinusSmallMMLargeTGC_strip->GetXaxis()->SetTitle("#eta");
	hwEtaPhiMinusSmallMMLargeTGC_strip->GetYaxis()->SetTitle("#phi[rad]");
	hwEtaPhiMinusSmallMMLargeTGC_strip->Write(); 
	
	/// Compined Digits (MM - sTGC wire) 
	hwEtaPhiPlusSmallTGGLargeMM_wire->Divide(hEtaPhiPlusSmallTGGLargeMM_wire);
	hwEtaPhiPlusSmallTGGLargeMM_wire->SetOption("COLZ");  
	hwEtaPhiPlusSmallTGGLargeMM_wire->SetMarkerColor(1);  
	hwEtaPhiPlusSmallTGGLargeMM_wire->SetMarkerStyle(21);   
	hwEtaPhiPlusSmallTGGLargeMM_wire->SetMarkerSize(0.1);
	hwEtaPhiPlusSmallTGGLargeMM_wire->GetXaxis()->SetTitle("#eta");
	hwEtaPhiPlusSmallTGGLargeMM_wire->GetYaxis()->SetTitle("#phi[rad]");
	hwEtaPhiPlusSmallTGGLargeMM_wire->Write(); 

	hwEtaPhiPlusSmallMMLargeTGC_wire ->Divide(hEtaPhiPlusSmallMMLargeTGC_wire );
	hwEtaPhiPlusSmallMMLargeTGC_wire ->SetOption("COLZ");  
	hwEtaPhiPlusSmallMMLargeTGC_wire ->SetMarkerColor(1);  
	hwEtaPhiPlusSmallMMLargeTGC_wire ->SetMarkerStyle(21);   
	hwEtaPhiPlusSmallMMLargeTGC_wire ->SetMarkerSize(0.1);
	hwEtaPhiPlusSmallMMLargeTGC_wire ->GetXaxis()->SetTitle("#eta");
	hwEtaPhiPlusSmallMMLargeTGC_wire ->GetYaxis()->SetTitle("#phi[rad]");
	hwEtaPhiPlusSmallMMLargeTGC_wire ->Write(); 

	hwEtaPhiMinusSmallTGGLargeMM_wire->Divide(hEtaPhiMinusSmallTGGLargeMM_wire);
	hwEtaPhiMinusSmallTGGLargeMM_wire->SetOption("COLZ");  
	hwEtaPhiMinusSmallTGGLargeMM_wire->SetMarkerColor(1);  
	hwEtaPhiMinusSmallTGGLargeMM_wire->SetMarkerStyle(21);   
	hwEtaPhiMinusSmallTGGLargeMM_wire->SetMarkerSize(0.1);
	hwEtaPhiMinusSmallTGGLargeMM_wire->GetXaxis()->SetTitle("#eta");
	hwEtaPhiMinusSmallTGGLargeMM_wire->GetYaxis()->SetTitle("#phi[rad]");
	hwEtaPhiMinusSmallTGGLargeMM_wire->Write(); 

	hwEtaPhiMinusSmallMMLargeTGC_wire->Divide(hEtaPhiMinusSmallMMLargeTGC_wire);
	hwEtaPhiMinusSmallMMLargeTGC_wire->SetOption("COLZ");  
	hwEtaPhiMinusSmallMMLargeTGC_wire->SetMarkerColor(1);  
	hwEtaPhiMinusSmallMMLargeTGC_wire->SetMarkerStyle(21);   
	hwEtaPhiMinusSmallMMLargeTGC_wire->SetMarkerSize(0.1);
	hwEtaPhiMinusSmallMMLargeTGC_wire->GetXaxis()->SetTitle("#eta");
	hwEtaPhiMinusSmallMMLargeTGC_wire->GetYaxis()->SetTitle("#phi[rad]");
	hwEtaPhiMinusSmallMMLargeTGC_wire->Write(); 

*/
        /********************************************************************* 
	*       Counting total hits on Small and Large Sectors seperately 
	**********************************************************************/
        /// For eta>0
	hwEtaPhiPlusHits->Divide(hEtaPhiPlusHits);
	hwEtaPhiPlusHits->SetOption("COLZ");  
	hwEtaPhiPlusHits->SetMarkerColor(1);  
	hwEtaPhiPlusHits->SetMarkerStyle(21);   
	hwEtaPhiPlusHits->SetMarkerSize(0.1);
	hwEtaPhiPlusHits->GetXaxis()->SetTitle("#eta");
	hwEtaPhiPlusHits->GetYaxis()->SetTitle("#phi[rad]");
	hwEtaPhiPlusHits->Write(); 

	/// For eta<0
	hwEtaPhiMinusHits->Divide(hEtaPhiMinusHits);
	hwEtaPhiMinusHits->SetOption("COLZ");  
	hwEtaPhiMinusHits->SetMarkerColor(1);  
	hwEtaPhiMinusHits->SetMarkerStyle(21);   
	hwEtaPhiMinusHits->SetMarkerSize(0.1);
	hwEtaPhiMinusHits->GetXaxis()->SetTitle("#eta");
	hwEtaPhiMinusHits->GetYaxis()->SetTitle("#phi[rad]");
	hwEtaPhiMinusHits->Write(); 


        /********************************************************************* 
	*       Counting total digits on Small and Large Sectors seperately 
	**********************************************************************/

	/// For eta>0, MM strips - sTGC strips
	hwEtaPhiPlusDigits_strip ->Divide(hEtaPhiPlusDigits_strip);
	hwEtaPhiPlusDigits_strip->SetOption("COLZ");  
	hwEtaPhiPlusDigits_strip->SetMarkerColor(1);  
	hwEtaPhiPlusDigits_strip->SetMarkerStyle(21);   
	hwEtaPhiPlusDigits_strip->SetMarkerSize(0.1);
	hwEtaPhiPlusDigits_strip->GetXaxis()->SetTitle("#eta");
	hwEtaPhiPlusDigits_strip->GetYaxis()->SetTitle("#phi[rad]");
	hwEtaPhiPlusDigits_strip->Write(); 


	/// For eta<0, MM strips - sTGC strips
	hwEtaPhiMinusDigits_strip ->Divide(hEtaPhiMinusDigits_strip);
	hwEtaPhiMinusDigits_strip->SetOption("COLZ");  
	hwEtaPhiMinusDigits_strip->SetMarkerColor(1);  
	hwEtaPhiMinusDigits_strip->SetMarkerStyle(21);   
	hwEtaPhiMinusDigits_strip->SetMarkerSize(0.1);
	hwEtaPhiMinusDigits_strip->GetXaxis()->SetTitle("#eta");
	hwEtaPhiMinusDigits_strip->GetYaxis()->SetTitle("#phi[rad]");
	hwEtaPhiMinusDigits_strip->Write(); 


	/// For eta>0, MM strips - sTGC pads
	hwEtaPhiPlusDigits_pad ->Divide(hEtaPhiPlusDigits_pad);
	hwEtaPhiPlusDigits_pad->SetOption("COLZ");  
	hwEtaPhiPlusDigits_pad->SetMarkerColor(1);  
	hwEtaPhiPlusDigits_pad->SetMarkerStyle(21);   
	hwEtaPhiPlusDigits_pad->SetMarkerSize(0.1);
	hwEtaPhiPlusDigits_pad->GetXaxis()->SetTitle("#eta");
	hwEtaPhiPlusDigits_pad->GetYaxis()->SetTitle("#phi[rad]");
	hwEtaPhiPlusDigits_pad->Write(); 



	/// For eta<0, MM strips - sTGC pads
	hwEtaPhiMinusDigits_pad ->Divide(hEtaPhiMinusDigits_pad);
	hwEtaPhiMinusDigits_pad->SetOption("COLZ");  
	hwEtaPhiMinusDigits_pad->SetMarkerColor(1);  
	hwEtaPhiMinusDigits_pad->SetMarkerStyle(21);   
	hwEtaPhiMinusDigits_pad->SetMarkerSize(0.1);
	hwEtaPhiMinusDigits_pad->GetXaxis()->SetTitle("#eta");
	hwEtaPhiMinusDigits_pad->GetYaxis()->SetTitle("#phi[rad]");
	hwEtaPhiMinusDigits_pad->Write(); 



	/// For eta>0, MM strips - sTGC wires
	hwEtaPhiPlusDigits_wire ->Divide(hEtaPhiPlusDigits_wire);
	hwEtaPhiPlusDigits_wire->SetOption("COLZ");  
	hwEtaPhiPlusDigits_wire->SetMarkerColor(1);  
	hwEtaPhiPlusDigits_wire->SetMarkerStyle(21);   
	hwEtaPhiPlusDigits_wire->SetMarkerSize(0.1);
	hwEtaPhiPlusDigits_wire->GetXaxis()->SetTitle("#eta");
	hwEtaPhiPlusDigits_wire->GetYaxis()->SetTitle("#phi[rad]");
	hwEtaPhiPlusDigits_wire->Write(); 


	/// For eta<0, MM strips - sTGC strips
	hwEtaPhiMinusDigits_wire ->Divide(hEtaPhiMinusDigits_wire);
	hwEtaPhiMinusDigits_wire->SetOption("COLZ");  
	hwEtaPhiMinusDigits_wire->SetMarkerColor(1);  
	hwEtaPhiMinusDigits_wire->SetMarkerStyle(21);   
	hwEtaPhiMinusDigits_wire->SetMarkerSize(0.1);
	hwEtaPhiMinusDigits_wire->GetXaxis()->SetTitle("#eta");
	hwEtaPhiMinusDigits_wire->GetYaxis()->SetTitle("#phi[rad]");
	hwEtaPhiMinusDigits_wire->Write(); 



        /// Write to File
	valResults->Write();
   
}
 

