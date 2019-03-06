//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/** 
   * @author D. Prieur prieur@lapp.in2p3.fr
   * 5. 4. 2004
   */

#ifndef LArTCMFitterTool_H
#define LArTCMFitterTool_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "LArRawConditions/LArCaliWave.h"
#include "LArRawConditions/LArPhysWave.h"

class TMinuit;
class TCanvas;
class TH1D;

static const InterfaceID IID_LArTCMFitterTool("LArTCMFitterTool", 1 , 0); 

class LArTCMFitterTool : public AthAlgTool
{

public: 

	// Retrieve interface ID
	static const InterfaceID& interfaceID() { return IID_LArTCMFitterTool; }


	// constructor
	LArTCMFitterTool(const std::string& type, const std::string& name, const IInterface* parent );
	virtual ~LArTCMFitterTool();

	virtual StatusCode initialize();
	virtual StatusCode finalize(){return StatusCode::SUCCESS;}
  
	LArPhysWave Fit(const LArCaliWave&, const LArPhysWave&);
	void fcn(int &, double *, double &, double *, int);
  
  	double getTaud() 	const { return m_taud; };
	double getTauexp()  const { return m_tauexp; };
	double getTaur() 	const { return m_taur; };
	double getf() 		const { return m_f; };
	double getW0() 		const { return m_w0; };
	double getAlpha() 	const { return m_alpha; };

	double getcaliStart() const { return m_caliStart; };
	double getphysShift() const { return m_physShift; };
	
	double getEmean() 	 const { return m_Emean; };
	double getMphyMcal() const { return m_MphyMcal; };
	double getchi2() 	 const { return m_chi2; };
	
	void setminuitoutputlevel(int level) {m_minuitoutputlevel = level; };

private:

	TMinuit* m_minuit;
	
	//int m_nparams;
	int m_minuitoutputlevel;

	LArCaliWave m_gCali;
	LArPhysWave m_gPhys;
	
	//int m_iGain, m_iLayer, m_iPhi, m_iEta, m_dac, m_ipdg; //not necessary
	
	double m_taud, m_f, m_tauexp, m_taur, m_w0, m_alpha;
	double m_tauderr, m_ferr, m_tauexperr, m_taurerr, m_w0err, m_alphaerr;
	
	double m_caliStart, m_caliStarterr;
	double m_physShift, m_physShifterr;

	double m_Emean, m_MphyMcal, m_chi2;
	
	double computeChi2(const LArPhysWave &, const LArPhysWave &);
	
	
	unsigned int getStart(const LArCaliWave &); 
	unsigned int getStart(const LArWave &);
	unsigned int getindMax(const LArWave &);
	
	TH1D LArCaliWave2TH1D(const LArCaliWave &);
	TH1D LArPhysWave2TH1D(const LArPhysWave &);
	TH1D LArWave2TH1D(const LArWave &);
	
};

#endif
