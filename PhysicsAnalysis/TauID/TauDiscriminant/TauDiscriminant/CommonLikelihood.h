/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * file: CommonLikelihood.h
 *
 * Author: Martin Flechl (mflechl@cern.ch)
 */

#ifndef COMMONLIKELIHOOD_H
#define COMMONLIKELIHOOD_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <assert.h>
#ifndef __STANDALONE
#include "GaudiKernel/MsgStream.h"
#endif

#include "TH3.h"
#include "TFile.h"
#include "TString.h"
#include "TMath.h"
#include "TGraph.h"
using namespace std;

//mw added
//////////////////////////////////////////////////////////////////////////////////////////////
const unsigned int NVAR_SAFE_1P=5;
const unsigned int NVAR_SAFE_3P=6;
const unsigned int NVAR_DEF=16;
//////////////////////////////////////////////////////////////////////////////////////////////


class CommonLikelihood{

    public:

        //Constructor
#ifndef __STANDALONE
        CommonLikelihood(bool _verbose=false,MsgStream* _log = 0):log(_log),
#else
        CommonLikelihood(bool _verbose=false):
#endif      
        m_tauFile(NULL),
        m_jetFile(NULL),
        m_cutFile(NULL),
        m_mu_correction_factor_1P(0.317),
        m_mu_correction_factor_3P(0.117),
        tg_loose_cuts_1P(NULL),
        tg_medium_cuts_1P(NULL),
        tg_tight_cuts_1P(NULL),
        tg_loose_cuts_3P(NULL),
        tg_medium_cuts_3P(NULL),
        tg_tight_cuts_3P(NULL),
		m_doTrigger(false)
        {
            if (_verbose) cout << "CommonLikelihood: Creating Instance..." << endl;
            m_tauFilename="pdfs_tau.root";
            m_jetFilename="pdfs_jets.root";
            m_cutFilename="LMTCutsLLH.root";
            m_llh=-99;
            m_et=-9999.;
            m_prongindex=-1;
            m_ntau[0]=0; m_ntau[1]=0;
            NVAR=56;
            DEBUG=0;
            if (_verbose) DEBUG=1;
            this->verbose = _verbose;
            //	  DEBUG=2;
            m_smooth = false;
	    m_nvtx = 0;
        }
        ~CommonLikelihood(){
		if(m_tauFile) m_tauFile->Close();
		delete m_tauFile;
		if(m_jetFile) m_jetFile->Close();
		delete m_jetFile;
		if(m_cutFile) m_cutFile->Close();
		delete m_cutFile;
	}
        bool build(const string& filename);
        int response(int level=-1, int option=-1) const;
        bool calcLLHValue(const map<string,const float*>* floatVariables,
                const map<string,const int*>* intVariables, int option);
        float getLLHValue(){ return m_llh; }

		void setDoTrigger(bool trig) { m_doTrigger = trig; }

    private:
    
        bool readPDFHistograms();
        bool readLMTCuts();
        bool smoothPDFHistograms();
        void smooth3D(TH3F* hTmp);
        TH3F* divideLog(TH3F* hTmpTau, TH3F* hTmpJet);
        Double_t Interpolate(TH3F* hist, Double_t x, Double_t y, Double_t z);
        int varNameToNumber(std::string varname) const;
        float getSingleLLHRatio(const int ivar, const int numtrack, const int author, const float et, const int nvtx, const float value);
        float getSimpleSingleLLHRatio(const int ivar, const int numtrack, const int author, const float et, const int nvtx, const float value);
        bool skipVar(const TString varname,const int numtrack,const int author,const int option) const;
        void splitString(const string& str, vector<string>& substrings, const string& delimiters = " ") const;

        void print(TString message, int level) const {
#ifndef __STANDALONE
            if (log) {
                if (level<=0) (*log) << "WARNING" << message << endreq;
                else (*log) << "DEBUG" << message << endreq;
            }
            else
#endif
                if (level<=DEBUG) cout << message << endl;
        }

#ifndef __STANDALONE
        MsgStream* log;
#endif
 
		std::string m_tauFilename;
        std::string m_cutFilename;
        std::string m_jetFilename;
        TFile *m_tauFile;
        TFile *m_jetFile;
        TFile *m_cutFile;
        float m_llh;
        float m_et;
        int m_prongindex;
        int m_nvtx;
        int m_ntau[2];
        bool m_smooth;

        double m_mu_correction_factor_1P;
        double m_mu_correction_factor_3P;

        std::vector<TGraph*> m_vLMTCuts1P;
        std::vector<TGraph*> m_vLMTCuts3P;
        int NVAR;
        std::map<std::string,TH3F*> m_pdfHistogramsTau;
        std::map<std::string,TH3F*> m_pdfHistogramsJet;
        std::map<std::string,TH3F*> m_pdfHistogramsRatio;
        TGraph* tg_loose_cuts_1P;
        TGraph* tg_medium_cuts_1P;
        TGraph* tg_tight_cuts_1P;
        TGraph* tg_loose_cuts_3P;
        TGraph* tg_medium_cuts_3P;
        TGraph* tg_tight_cuts_3P;
		
		bool m_doTrigger;

        int DEBUG;
        bool verbose;

        map<string,float> getVariables(const map<string,const float*>* floatVariables,const map<string,const int*>* intVariables, const int numtrack, const int author, const int option) const;
};

#endif
