//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** 
   * @author D. Prieur  prieur@lapp.in2p3.fr
   * 5. 4. 2004
   */


#ifndef LARTCMPREDICTOR_H
#define LARTCMPREDICTOR_H

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/MsgStream.h"

#include "LArRawConditions/LArCaliWave.h"
#include "LArRawConditions/LArPhysWave.h"

//class TMinuit;

class LArTCMPredictor
{
public: 
 // constructor
	LArTCMPredictor();
 	virtual ~LArTCMPredictor(); 


	LArPhysWave predictLArPhysWave(const LArCaliWave &);	

	double getTaud() 	const { return m_taud; };
	double getTauexp()  const { return m_tauexp; };
	double getTaur() 	const { return m_taur; };
	double getf() 		const { return m_f; };
	double getW0() 		const { return m_w0; };
	double getAlpha() 	const { return m_alpha; };
	
	double getcaliStart() const { return m_caliStart; };
	double getphysShift() const { return m_physShift; };
	
	void setTaud(double taud) 		{ m_taud=taud; };
	void setTauexp(double tauexp) 	{ m_tauexp=tauexp; };
	void setTaur(double taur) 		{ m_taur=taur; };
	void setf(double f) 			{ m_f=f; };
	void setW0(double w0) 			{ m_w0=w0; };
	void setAlpha(double alpha) 	{ m_alpha=alpha; };
	
	void setcaliStart(double caliStart) 	{ m_caliStart=caliStart; };
	void setphysShift(double physShift) 	{ m_physShift=physShift; };
	
//	template <class T> unsigned int getMax(const T &PULSE); 
	double getMax(const LArCaliWave &larCaliWave); 
	double getMax(const LArPhysWave &larPhysWave); 
private:

	LArCaliWave m_gCali;
	LArPhysWave m_gPhysPred;
	
	double m_caliStart; //, m_caliStarterr;
	double m_physShift; //, m_physShifterr;
	
	double m_taud, m_f, m_tauexp, m_taur, m_w0, m_alpha;
	
	double evalLArCaliWave(int);
	double Integrand(int, int);
	double qfast(int , int );
	
	double linearInterpolate(const LArWave&, double);
	LArWave translate(const LArWave&, int);
	unsigned int getindMax(const LArWave &);
	

	unsigned int getStart(const LArCaliWave &); 
	unsigned int getStart(const LArWave &);

	
	
	double Gt2(double t) const;
	double Gt3(double t) const;
	
};

#endif
