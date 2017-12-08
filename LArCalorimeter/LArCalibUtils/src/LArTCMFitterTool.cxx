/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArTCMFitterTool.h" 
#include "LArCalibUtils/LArTCMPredictor.h"

#include "TROOT.h"
#include "TApplication.h"
#include "TSystem.h"

#include <TMinuit.h>

#include <TH1.h>

//must be renAMED TO LArTCMFitterToolTool.cxx

static LArTCMFitterTool* static_LArTCMFitterTool_pointer = NULL;

void LArTCMFitterTool_fcn_wrapper(int &npar, double *gin, double &f, double *par, int iflag) {
	if (static_LArTCMFitterTool_pointer==NULL) {
		std::cout << "Ugly things happening here..." << std::endl;
		exit(-1);
	}
	static_LArTCMFitterTool_pointer->fcn(npar, gin, f, par, iflag);
}


LArTCMFitterTool::LArTCMFitterTool ( const std::string& type, const std::string& name,const IInterface* parent )
  : AthAlgTool(type,name,parent),
    m_minuit(0),
    m_minuitoutputlevel(0),
    m_taud(0),
    m_f(0),
    m_tauexp(0),
    m_taur(0),
    m_w0(0),
    m_alpha(0),
    m_tauderr(0),
    m_ferr(0),
    m_tauexperr(0),
    m_taurerr(0),
    m_w0err(0),
    m_alphaerr(0),
    m_caliStart(0),
    m_caliStarterr(0),
    m_physShift(0),
    m_physShifterr(0),
    m_Emean(0),
    m_MphyMcal(0),
    m_chi2(0)
{
}

// destructor 
LArTCMFitterTool::~LArTCMFitterTool()
{
}

/////////////////////////////////////
StatusCode LArTCMFitterTool::initialize()
{
	if( static_LArTCMFitterTool_pointer != NULL ) {
          ATH_MSG_ERROR ( "Problem with static pointer to the data object used by minuits FCN-function!!!" );
          return StatusCode::FAILURE;
	}
	
	//m_nparams = 8;
//	m_minuit = new TMinuit(8); //initialize TMinuit with a maximum of nparams parameters
//	static_LArTCMFitterTool_pointer	 = this;
//	m_minuit->SetFCN(LArTCMFitterTool_fcn_wrapper);
//	log<<MSG::DEBUG<<"Minuit and FCN initialized"<<endreq;
	
	//double arglist[10];
	//int ierflg = 0;
	//m_minuit->SetPrintLevel(1);  // -1: no output, 1: std output
	//m_minuit->mnexcm("SET NOW", arglist, 0, ierflg); // no warning 
	
	m_minuitoutputlevel = -1;
	gSystem->Load("libMinuit");
	
	return StatusCode::SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////
LArPhysWave LArTCMFitterTool::Fit(const LArCaliWave &larCaliWave, const LArPhysWave &larPhysWave) 
{  
	m_gCali = larCaliWave;
	m_gPhys = larPhysWave;

	m_minuit = new TMinuit(8); //initialize TMinuit with a maximum of nparams parameters
	static_LArTCMFitterTool_pointer	 = this;
	m_minuit->SetFCN(LArTCMFitterTool_fcn_wrapper);
	m_minuit->SetPrintLevel(m_minuitoutputlevel); // -1: no output, 1: std output
	ATH_MSG_INFO("Minuit and FCN initialized");


	// Find beginning of the caliWave
	m_caliStart = (double)getStart(m_gCali)-2.; // this need to be tuned
	if (m_caliStart<0.) m_caliStart=0.; // 
	
	
	// Set initial starting values and step sizes for the 8 parameters
	Double_t vstart[8] = {450., 340. , 0.065, 0.184, 1., m_caliStart, 27, 0.025}; 
	Double_t step[8] = {1., 1. , 0.001 , 0.001 , 0.1, 0.5, 1., 1e-3}; 
	
	double arglist[10];
	int ierflg = 0;

   	m_minuit->mnparm(0, "taud",      vstart[0], step[0], 50,     750., ierflg);	
   	m_minuit->mnparm(1, "tauexp",    vstart[1], step[1], 300,    800., ierflg);
	m_minuit->mnparm(2, "f",         vstart[2], step[2], 0.0001,  0.9, ierflg);
	m_minuit->mnparm(3, "w0",        vstart[3], step[3], 0.1e-3,   1., ierflg);
	m_minuit->mnparm(4, "taur",      vstart[4], step[4], 1e-5,    50., ierflg);
	m_minuit->mnparm(5, "caliStart", vstart[5], step[5], 0.,      80., ierflg);
	m_minuit->mnparm(6, "physShift", vstart[6], step[6], 0.,     80., ierflg);
	m_minuit->mnparm(7, "alpha",     vstart[7], step[7], 1e-6,     1., ierflg);

	// Fixed parameters
	// taud, tauexp, f, caliStart and alpha are fixed
	// w0, taur and physShift are left free
	arglist[0]= 1; // taud
	arglist[1]= 2; // tauexp
	arglist[2]= 3; // f
	arglist[3]= 6; // caliStart
	arglist[4]= 8; // alpha   
	m_minuit->mnexcm("FIX",arglist,5,ierflg);

	// Scan of parameters
	arglist[0] = 7; // scan of physShift
	m_minuit->mnexcm("SCAN",arglist,1,ierflg);

	// Set minuit run
	arglist[0] = 1;
	m_minuit->mnexcm("SET ERR", arglist ,1,ierflg);
	arglist[0] = 0;  // 0 fast / 1 standart / 2 improve
	m_minuit->mnexcm("SET STR", arglist ,1,ierflg);

	// Start minimization method
	arglist[0] = 1e11;
	arglist[1] = 0.1;
	//m_minuit->mnexcm("MIGRAD", arglist ,2,ierflg);
	//m_minuit->mnexcm("SIMPLEX", arglist ,2,ierflg);
	//m_minuit->mnexcm("MIGRAD", arglist ,2,ierflg);
	m_minuit->mnexcm("MINIMIZE", arglist ,2,ierflg); // launch minimization
	
	// Print fit results
	double amin, edm, errdef;
	int nvpar, nparx, icstat;
	m_minuit->mnstat(amin, edm, errdef, nvpar, nparx, icstat);
	m_minuit->mnprin(3,amin);

	// Get results
	m_minuit->GetParameter(0,m_taud,m_tauderr);
	m_minuit->GetParameter(1,m_tauexp,m_tauexperr);
	m_minuit->GetParameter(2,m_f,m_ferr);
	m_minuit->GetParameter(3,m_w0,m_w0err);
	m_minuit->GetParameter(4,m_taur,m_taurerr);
	m_minuit->GetParameter(5,m_caliStart,m_caliStarterr);
	m_minuit->GetParameter(6,m_physShift,m_physShifterr);
	m_minuit->GetParameter(7,m_alpha,m_alphaerr);
	m_chi2=amin;
	
	ATH_MSG_INFO("Final set of parameters:");
	ATH_MSG_INFO("taud: "<<m_taud<<" tauexp: "<<m_tauexp<<" f: "<<m_f<<" w0: "<<m_w0<<" taur: "<<m_taur<<" caliStart: "<<m_caliStart<<" phyShift: "<<m_physShift);

	delete m_minuit;
	
	// re-produce LArPhysWave from final fit parameters
	LArTCMPredictor larTCMPredictor;
	larTCMPredictor.setTaud(m_taud);
	larTCMPredictor.setTauexp(m_tauexp);
	larTCMPredictor.setf(m_f);
	larTCMPredictor.setW0(m_w0);
	larTCMPredictor.setTaur(m_taur);
	larTCMPredictor.setcaliStart(m_caliStart);
	larTCMPredictor.setphysShift(m_physShift);
	larTCMPredictor.setAlpha(m_alpha);
	LArPhysWave predLArPhysWave = larTCMPredictor.predictLArPhysWave(larCaliWave);
	m_chi2 = computeChi2(m_gPhys,predLArPhysWave); // fill also m_Emean

	//Compute the ratio MphyMcal
	double Mcal = larTCMPredictor.getMax(larCaliWave);
	double Mphy = larTCMPredictor.getMax(predLArPhysWave);
	
	if(Mcal!=0) m_MphyMcal = Mphy/Mcal;
	else m_MphyMcal=-999;
	ATH_MSG_DEBUG("MphyMcal: "<<m_MphyMcal);
	
	//need to normalize and scale predLArPhysWave by MphyMcal
	const unsigned int N = predLArPhysWave.getSize();
	for (unsigned int i=0; i<N; ++i) {
		predLArPhysWave.setSample(i,predLArPhysWave.getSample(i)/Mcal); //the wave max is normalized to Mphy/Mcal value - needed for OFC
		//predLArPhysWave.setSample(i,predLArPhysWave.getSample(i)); // this line must be used for debug only !!!!
	}
	
	return predLArPhysWave;	
}


void LArTCMFitterTool::fcn(int &/*npar*/, double */*gin*/, double &f, double *par, int /*iflag*/) {
	
	LArTCMPredictor larTCMPredictor;
	
	// Set parameters for LArPhysWave prediction  
	larTCMPredictor.setTaud(par[0]);
	larTCMPredictor.setTauexp(par[1]);
	larTCMPredictor.setf(par[2]);
	larTCMPredictor.setW0(par[3]);
	larTCMPredictor.setTaur(par[4]);
	larTCMPredictor.setcaliStart(par[5]);
	larTCMPredictor.setphysShift(par[6]);
	larTCMPredictor.setAlpha(par[7]);

	// LArPhysWave prediction
	LArPhysWave larpredPhysWave = larTCMPredictor.predictLArPhysWave(m_gCali);
	
	f = computeChi2(m_gPhys,larpredPhysWave);
}


double LArTCMFitterTool::computeChi2(const LArPhysWave &larPhysWave, const LArPhysWave &larpredPhysWave) {

	//const Int_t nbins = 160; //fit only on the nbins first bin
	//  const Int_t nbins = 175;
	const int nbins = (larPhysWave.getSize()<larpredPhysWave.getSize())? larPhysWave.getSize() : larpredPhysWave.getSize();
	//std::cout<<"nbins: "<<nbins<<std::endl;
	double chisq = 0.;
	double S_yf = 0.;
	double S_ff = 0.;
	double S_yy = 0.;	
	double y,z;

	// Fit range
	int start_fit = 0;
	int end_fit = nbins;

	// Compute chi2
	for (Int_t i=start_fit;i<end_fit; ++i) {
		y = larPhysWave.getSample(i);
		z = larpredPhysWave.getSample(i);
		S_yf += y*z;
		S_ff += z*z;
		S_yy += y*y;
	}
	if (S_ff==0) S_ff=0.00000001;

	//chi2 et Emean   
	chisq = S_yy - S_yf*S_yf/S_ff;
	m_Emean = S_yf/S_ff;
	return chisq;
}


unsigned int LArTCMFitterTool::getStart(const LArCaliWave &larCaliWave) 
{
	LArWave larWave(larCaliWave.getWave(),larCaliWave.getDt());
	return getStart(larWave);
}


unsigned int LArTCMFitterTool::getStart(const LArWave &larWave) 
{
	const unsigned int N = larWave.getSize();
	double max=larWave.getSample(getindMax(larWave));
	double treshold = max*0.01;  //need to implement member/propertie for treshold
	unsigned int k=0;
	do k++; while((larWave.getSample(k)<treshold)&&(k<N));
	return k-1;	
}


unsigned int LArTCMFitterTool::getindMax(const LArWave &larWave)
{
	const unsigned int N = larWave.getSize();
	unsigned int indmax=0;
	for (unsigned int i=0; i<N; ++i) {
		if(larWave.getSample(i)>larWave.getSample(indmax)) indmax=i;	
		//if(larWave[i]>larWave[indmax]) indmax=i; //do not work...
	}
	//std::cout<<"indmax"<<indmax<<std::endl;
	return indmax;	
}


TH1D LArTCMFitterTool::LArCaliWave2TH1D(const LArCaliWave &larCaliWave) 
{
	LArWave larWave(larCaliWave.getWave(),larCaliWave.getDt());
	return LArWave2TH1D(larWave);
}

TH1D LArTCMFitterTool::LArPhysWave2TH1D(const LArPhysWave &larPhysWave)
{
	LArWave larWave(larPhysWave.getWave(),larPhysWave.getDt());
	return LArWave2TH1D(larWave);
}

TH1D LArTCMFitterTool::LArWave2TH1D(const LArWave &larWave)
{
	const unsigned int N = larWave.getSize();
	TH1D hist("hist","hist",N,0,N);
	for(unsigned int i=0; i<N; ++i) {
		hist.SetBinContent(i+1,larWave.getSample(i));
	}
	return hist;	
}



