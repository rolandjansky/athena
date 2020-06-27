/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibT0/T0CalibrationClassic.h" 
#include "MdtCalibT0/T0CalibrationOutput.h" 
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MuonCalibIdentifier/MuonFixedId.h"

#include "MuonCalibStl/ToString.h"
#include "MuonCalibStl/DeleteObject.h"

#include <algorithm>
#include <iostream>
#include <vector>
#include <string>

#include "TH1.h"
#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TMinuit.h"

namespace MuonCalib {

//function to be fitted to time spectrum

  inline Double_t TimeSpectrum_func(Double_t *xx , Double_t *par) { 
    Double_t &x(xx[0]);
    return par[0]+(par[1]*(1+par[2]*std::exp(-(x-par[4])/par[3])))/((1+std::exp((-x+par[4])/par[6]))*(1+std::exp((x-par[5])/par[7])));
  }

  T0CalibrationClassic::T0CalibrationClassic( std::string name, const T0ClassicSettings* settings ) : 
    IMdtCalibration(name), m_settings(settings), m_converged(false), m_name(name),m_result(0), m_delete_settings(false) {
    if (!m_settings) {
      double *params=new double[8];   // warning: this would give a memory leak
      params[0] = 0. ;   // noise level
      params[1] = 6.5 ; 
      params[2] = 6.5 ; 
      params[3] = 155. ;
      params[4] = 280. ; //t0
      params[5] = 980. ; //tmax
      params[6] = 9. ;   //t0 slope
      params[7] = 11.5 ; //tmax slope
      m_settings = new T0ClassicSettings(0.,300.,100,-100.,900.,1000,1,1000,1,8,params,10.,4); 
      m_delete_settings=true;
    }
    MsgStream log(Athena::getMessageSvc(),"T0ClassicSettings");
    if (log.level()<=MSG::INFO) log << MSG::INFO << "T0CalibrationClassic::T0CalibrationClassic"<<m_name<<" "<<name<<endmsg;
    if(m_settings->printLevel() <= 2) m_settings->print();
    m_currentItnum=1;
    std::string HistoFileName="T0Classic_"+m_name+".root";
    if (log.level()<=MSG::INFO) log << MSG::INFO << "T0CalibrationClassic::T0CalibrationClassic"<<m_name
	       <<" "<<name<<" "<<HistoFileName<<endmsg;
    p_file = new TFile(HistoFileName.c_str(),"recreate");
    m_regiondir = p_file->mkdir(m_name.c_str()); 
  }

  T0CalibrationClassic::~T0CalibrationClassic() {
    p_file->Write();
    p_file->Close();
    delete p_file;
    std::for_each(m_histos.begin(),m_histos.end(),DeleteObject());
    if(m_delete_settings) delete m_settings;
  }

  bool  T0CalibrationClassic::handleSegment( MuonCalibSegment& seg ) {
    for(std::vector<MdtCalibHitBase*>::iterator it=seg.mdtHOTBegin(); it!=seg.mdtHOTEnd(); ++it) {
        // 
        //  M.I. 16 October 2007
        //  ATTENTION ATTENTION Including cut on DistanceToReadOut 
        //  ONLY FOR P1 data
        //
      float distanceToRO = (*it)->distanceToReadout();

      bool ROside = distanceToRO<130000. ; // this means that there is no selection along the tube
      if (ROside) {
	MuonFixedId id=(*it)->identify();
      
	// get the T0 originally subtracted for this hit 
	int nML=id.mdtMultilayer();
	int nL=id.mdtTubeLayer();
	int nT=id.mdtTube();
	const MdtTubeFitContainer::SingleTubeCalib * stc=m_result->getCalib(nML-1,nL-1,nT-1);
	if(!stc) {
		MsgStream log(Athena::getMessageSvc(),"T0ClassicSettings");
		log << MSG::WARNING << "no Single Tube Calib info found for ML="<<nML<<" L="<<nL<<" T="<<nT<<endmsg; 
		log << MSG::WARNING << "container size "<<m_result->size()<<endmsg;
		log << MSG::WARNING << "container nML "<<m_result->numMultilayers()<<endmsg;
		log << MSG::WARNING << "container nL "<<m_result->numLayers()<<endmsg;
		log << MSG::WARNING << "container nT "<<m_result->numTubes()<<endmsg;
	}

	// get histos
	T0ClassicHistos* histos = getHistos(id.getIdInt());

	// fill histos
	 	 
	float ttime=((*it)->driftTime());
	  
	histos->time->Fill(ttime);
	histos->adc->Fill((*it)->adcCount());
	// book an additional dummy set of histograms to fit the global T0
	T0ClassicHistos* histosAll = getHistos(0);
	histosAll->time->Fill(ttime);
	histosAll->adc->Fill((*it)->adcCount());
	
	// M.I. Jun20-07 ---- Adding MultiLayer Histos :
	T0ClassicHistos* histosML = getHistos(nML);
	histosML->time->Fill(ttime);
	histosML->adc->Fill((*it)->adcCount());
	// M.I. Jan1507 ---- Adding Mezzanine Histos :
	T0ClassicHistos* histosMezz = getHistos(id.mdtMezzanine());
	histosMezz->time->Fill(ttime);
	histosMezz->adc->Fill((*it)->adcCount());

	// M.I. 16Oct07 ---- Adding "SerialGas" Histos :
        int serialGas = nML*10+(nT-1)%3+1 ;
	T0ClassicHistos* histosSerialGas = getHistos(serialGas);
	histosSerialGas->time->Fill(ttime);
	histosSerialGas->adc->Fill((*it)->adcCount());
      }    //end if (ROside) 
    }      //end loop over seg.mdtHOT
    return true;
  }
  
  bool  T0CalibrationClassic::analyse() {
    MsgStream log(Athena::getMessageSvc(),"T0ClassicSettings");
    if (log.level()<=MSG::INFO) log << MSG::INFO << "T0CalibrationClassic::analyse iteration "<<m_currentItnum << endmsg;

    // loop over m_histos histograms 
    for(std::vector<T0ClassicHistos*>::iterator it=m_histos.begin(); it!=m_histos.end();++it) {
      if(m_settings->fitTime()) {
	MdtTubeFitContainer::SingleTubeFit full;
	MdtTubeFitContainer::SingleTubeCalib st;
	int idtube=(*it)->id ;
	if((*it)->id!=0 && (int)(idtube/100000000)!=9 ) {
	  doTimeFit(*it,full,st);
	  doAdcFit(*it,full,st);
	  MuonFixedId fId((*it)->id); 
	  int nML=fId.mdtMultilayer();
	  int nL=fId.mdtTubeLayer();
	  int nT=fId.mdtTube();
             
	  bool setInfo=m_result->setCalib(nML-1,nL-1,nT-1,st);
	  if(!setInfo) log << MSG::WARNING << "T0CalibrationClassic::PROBLEM! could not set SingleTubeCalib info "<<endmsg;
	  setInfo=m_result->setFit(nML-1,nL-1,nT-1,full);
	  if(!setInfo) log << MSG::WARNING << "T0CalibrationClassic::PROBLEM! could not set SingleTubeFullInfo info "<<endmsg;

	}
      }
    }

    m_currentItnum++;
    p_file->Write();
    return true;
  }

  const IMdtCalibrationOutput* T0CalibrationClassic::analyseSegments( const std::vector<MuonCalibSegment*> &segs ) {
    for(unsigned int i=0; i<segs.size(); i++) handleSegment(*segs[i]);
    analyse();
    return getResults();
  }

  void T0CalibrationClassic::doTimeFit(T0ClassicHistos *T0h, MdtTubeFitContainer::SingleTubeFit &fi, MdtTubeFitContainer::SingleTubeCalib &stc) {
    TMinuit *gMinuit = new TMinuit();
    int fitMezz(1) ; 

    const int np=m_settings->numParams();
    double *pfit   = new double[np];
    double *errfit = new double[np];
    Double_t **matrix = new Double_t*[np];
    // initialize everything to make Coverity happy
    for(int i=0; i<np; i++) {
      matrix[i]=new Double_t[np];
      pfit[i] = 0.;
      errfit[i] = 0.;
      for(int j=0; j<np; j++) matrix[i][j] = 0.;
    }
    double *pdefault=m_settings->params();
    double chi2;
    int ndof;
           
    MuonFixedId fId(T0h->id);
    int isMultilayer(0);

    if (T0h->id==1 || T0h->id==2 || (T0h->id>10&&T0h->id<=23)) isMultilayer=1;

    MsgStream log(Athena::getMessageSvc(),"T0ClassicSettings");
    if (( fId.isValid() && fId.is_mdt()) || isMultilayer ) {
      if (log.level()<=MSG::INFO) log << MSG::INFO << " STARTING doTimeFit "<<endmsg;
      TH1* h = nullptr; 

      if ( isMultilayer) {
        if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << T0h->id << endmsg;
	h=T0h->time;
      }

      if (fitMezz==1 && !isMultilayer) {                       // FIT MEZZANINE 
	int hIdMezz = fId.mdtMezzanine() ;

	ToString ts;
	std::string HistoId(std::string("time_mezz_") + ts((hIdMezz)%(900000000)));
	if (log.level()<=MSG::DEBUG) {
	   log << MSG::DEBUG << " doTimeFit HistogramId : "<< T0h->id <<endmsg;
	   log << MSG::DEBUG << " doTimeFit Histogram : "<< HistoId <<endmsg;
	}
	TH1F * timeHis=(TH1F*) m_regiondir->Get(HistoId.c_str());
	if(!timeHis) {
	  delete [] pfit ;
	  delete [] errfit ;
	  for(int i=0; i<np; i++) delete [] matrix[i];
	  delete [] matrix;
	  return;
	}

	T0ClassicHistos* histosMezz = getHistos(fId.mdtMezzanine());
	h= histosMezz->time;
      }

      Stat_t entries=h->GetEntries();
      fi.statistics=(int)entries;

      if(log.level()<=MSG::VERBOSE) log << MSG::VERBOSE << " histogram "<<h->GetName()
						 <<" "<<h->GetTitle()
						 <<" entries="<<h->GetEntries()
						 <<" min entries="<<m_settings->entries()
						 <<std::endl; 

      // CHECK whether the Selected Histogram has enough entries 
      if ((int)entries > m_settings->entries()){
    
      // CHECK whether the histogram has been already fitted 
      TF1 * FitFunction = h->GetFunction("TimeSpectrum");
      if(FitFunction){
	for(int i=0; i<np; i++){
	  pfit[i]   = FitFunction->GetParameter(i);
	  errfit[i] = FitFunction->GetParError(i) ;
	}
	chi2 = FitFunction->GetChisquare() ; // total chi2
	ndof = FitFunction->GetNDF();        // number of degrees of freedom
      } else {  // The Selected Histo has NOT been fitted. Fit Now         
	TF1 *TimeSpectrum = new TF1("TimeSpectrum",
				    "[0]+([1]*(1+[2]*exp(-(x-[4])/[3])))/((1+exp((-x+[4])/[6]))*(1+exp((x-[5])/[7]))) ",
				    m_settings->minTime(),m_settings->maxTime()); 
	for(int i=0;i<np;i++) {
	  pfit[i]=*(pdefault++);
	  if(log.level()<=MSG::DEBUG) 
	    log << MSG::DEBUG << "T0CalibrationClassic::doTimeFit initial parameters"
		     <<i<<"="<<pfit[i]<<endmsg;
        }
	searchParams(h,&pfit[0],np);  
	if(log.level()<=MSG::DEBUG) {
	  log << MSG::DEBUG << "T0CalibrationClassic::doTimeFit parameters after searchParams "<<endmsg;
	  for(int i=0;i<np;++i){log << MSG::DEBUG << "i,pfit(i) "<<i<<" "<<pfit[i]<<endmsg;}
	}
	TimeSpectrum->SetParameters(pfit);
	TimeSpectrum->SetParLimits(0,0.,5.);
	TimeSpectrum->SetParLimits(1,0.,1000.);
	TimeSpectrum->SetParLimits(2,0.,40.);
	TimeSpectrum->SetParLimits(3,50.,400.);
	TimeSpectrum->SetParLimits(4,0.,1000.);
	// 5 parameters fit
	TimeSpectrum->SetParLimits(5,pfit[5],pfit[5]);
	TimeSpectrum->SetParLimits(6,pfit[6],pfit[6]);
	TimeSpectrum->SetParLimits(7,pfit[7],pfit[7]);
	h->Fit("TimeSpectrum","QLB");
	// 6 parameters fit
	TimeSpectrum->SetParLimits(5,500.,2000.);
	TimeSpectrum->SetParLimits(6,pfit[6],pfit[6]);
	TimeSpectrum->SetParLimits(7,pfit[7],pfit[7]);
	h->Fit("TimeSpectrum","QLB");
	// 7 parameters fit
	TimeSpectrum->SetParLimits(6,4.,30.);
	h->Fit("TimeSpectrum","QLB");
	// final 8 parameters fit
	TimeSpectrum->SetParLimits(6,4.,30.);
	TimeSpectrum->SetParLimits(7,4.,30.);
        double xmin = h->GetBinLowEdge(1);
        double xmax = pfit[5]+250.;
	h->Fit("TimeSpectrum","QLB","",xmin,xmax);

	gMinuit->mnemat(&matrix[0][0],np) ;
	for(int i=0; i<np; i++){
	  pfit[i]    = TimeSpectrum->GetParameter(i);
	  errfit[i] = TimeSpectrum->GetParError(i) ;
	}
	chi2     = TimeSpectrum->GetChisquare() ; // total chi2
	ndof     = TimeSpectrum->GetNDF();        // number of degrees of freedom
      }
      // THE NEW HISTOGRAM HAS BEEN FITTED 
      if( ndof == 0. ) ndof = -1;

      if(log.level()<=MSG::VERBOSE) 
	log << MSG::VERBOSE << " fit results chi2/ndof="<<chi2/ndof<<" T0="<<pfit[4]<<" err="<<errfit[4]<<endmsg; 
      if(chi2/ndof < m_settings->chi2max()) {
	stc.statusCode=0 ;// success
      } else {
	stc.statusCode=3 ; // bad chi2 
      }
      stc.t0=pfit[4]; 
      fi.chi2Tdc=chi2/ndof;
      for (int i=0 ; i<np; i++) fi.par[i]=pfit[i] ;
      
      // NOW we get rid of the covariance matrix 
      /******************************************************* 

        *******************************************************/
        // NOW we  set the the first 8 values of fi.cov to errfit
      for (int i=0 ; i<np; i++) fi.cov[i]=errfit[i] ;

	//
	// Try 4 parameters fit now
	// NOW COMMENTED BUT SHOULD BE TRIED !!!! (as it was in calib )
        /*****************************************************
	double lowt;
	double hight;
	double * pfd=new double[4];
	pfd[0]=pfit[0];
	pfd[1]=pfit[1];
	pfd[2]=pfit[4];
	pfd[3]=pfit[7];
	lowt=pfit[4]-2.*pfit[6];
	hight=pfit[4]+50.;
	TF1 * FermiDirac = new TF1("FermiDirac",
				   "[0]+([1]/(1+exp(-(x-[3])/[2])))",
				   lowt,hight); 
	//		     FermiDirac->SetParameters(pfd);
	//h->Fit("FermiDirac","LBV");
        *****************************************************/

      } else {
	stc.statusCode=2; // too few entries 
      }
    }
    delete [] pfit;
    delete [] errfit;
    for(int i=0; i<np; i++) delete [] matrix[i];
    delete [] matrix;
	
    if(log.level()<=MSG::DEBUG) log << MSG::DEBUG << " ENDING doTimeFit "<< endmsg;

  }

  void  T0CalibrationClassic::doAdcFit(T0ClassicHistos *T0h, MdtTubeFitContainer::SingleTubeFit &fi, MdtTubeFitContainer::SingleTubeCalib &stc) {
    // THIS SETTINGS VARIABLE HAS TO BE IMPLEMENTED (as of March 7, 2007)  : 
    // int fitMezz = m_settings->fitMezzanine();
    // int fitMezz(123) ; 
    int fitMezz(1); 

    double chi2(0);
    int ndof(0) ;	
    Double_t par[4];
    Double_t errpar[4];

    for (int ii=0; ii<4; ii++) {
      par[ii] = 0.0 ;
      errpar[ii] = 0.0 ;
    }

    MsgStream log(Athena::getMessageSvc(),"T0ClassicSettings");
    MuonFixedId fId(T0h->id);
    if (fId.isValid() && fId.is_mdt()) {
      if(log.level()<=MSG::DEBUG) log << MSG::DEBUG << " doAdcFit : checking Single tube entries"<< endmsg;
      double adcThreshold = 50. ;
      TH1 * hcheck ;
      hcheck = T0h->adc;
      int nhits = static_cast<int>(hcheck->GetEntries()) ;

      int adcBinThreshold = (int)((adcThreshold-hcheck->GetBinLowEdge(1))/(hcheck->GetBinWidth(1))+1)   ;  //
      int nhitsAboveAdcCut = (int)  hcheck->Integral(adcBinThreshold,hcheck->GetNbinsX()) ;
      if(log.level()<=MSG::DEBUG) log << MSG::DEBUG << " doAdcFit : TotHits, nhitsAboveAdcCut "<< nhits <<" "<<nhitsAboveAdcCut<< endmsg;

      fi.cov[20]=nhits ;
      fi.cov[21]=nhitsAboveAdcCut ;
 
      if(log.level()<=MSG::DEBUG) log << MSG::DEBUG << " STARTING doAdcFit "<< endmsg;

      TH1 * h = nullptr;

      if (fitMezz==1) {
	int hIdMezz = fId.mdtMezzanine() ;
	ToString ts;
	std::string HistoId(std::string("charge_mezz_") + ts((hIdMezz)%(900000000)));
	if (log.level()<=MSG::DEBUG) {
	  log << MSG::DEBUG <<" doAdcFit HistogramId : "<< T0h->id << endmsg;
	  log << MSG::DEBUG <<" doAdcFit Histogram : "<< HistoId << endmsg;
	}
	TH1F * adcHis=(TH1F*) m_regiondir->Get(HistoId.c_str());
	if(!adcHis) return;

	T0ClassicHistos* histosMezz = getHistos(fId.mdtMezzanine());
	h= histosMezz->adc;
      }

      if(log.level()<=MSG::VERBOSE)
	log << MSG::VERBOSE <<" histogram "<<h->GetName()<<" "<<h->GetTitle()
		 <<" entries="<<h->GetEntries()<<endmsg; 
      Stat_t entries=h->GetEntries();

      // CHECK whether the Selected Histogram has enough entries
      if ((int)entries > m_settings->entries()) {

        // CHECK whether the histogram has been already fitted
	TF1 * FitFunction = h->GetFunction("AdcSpectrum");
	if (FitFunction){
	  chi2	= FitFunction->GetChisquare();
	  ndof	= FitFunction->GetNDF();	
	  for (int i=0; i<4; i++) {
	    par[i] = FitFunction->GetParameter(i);
	    errpar[i] = FitFunction->GetParError(i);
	  }

	} else {    // The Selected Histo has NOT been fitted. Fit Now
	  double m=h->GetMean();
	  double r=h->GetRMS();
	  double maxval=h->GetMaximum();
	  double adcpar[4] ;
	  adcpar[0] = maxval*2. ;
	  adcpar[1] = m ;
	  adcpar[2] = r ;
	  adcpar[3] = r/3. ;
   
	  TF1 * AdcSpectrum = new TF1("AdcSpectrum"," ([0]*exp((x-[1])/[2]))/ (1.+exp((x-[1])/[3])) ",
				      m_settings->minAdc(),m_settings->maxAdc() );
	  AdcSpectrum->SetParameters(adcpar) ;
	  double fitMin = m-(3*r) ;
	  double fitMax = m+(3*r) ;
	  if (fitMin<adcThreshold) fitMin = adcThreshold ;
	  if (fitMax>300 ) fitMax = 300. ;
	  h->Fit("AdcSpectrum","Q"," ",fitMin,fitMax);
	  chi2	= AdcSpectrum->GetChisquare();
	  ndof	= AdcSpectrum->GetNDF();	
	  for (int i=0; i<4; i++) {
	    par[i] = AdcSpectrum->GetParameter(i);
	    errpar[i] = AdcSpectrum->GetParError(i);
	  }
	  if (log.level()<=MSG::VERBOSE)
	    log << MSG::VERBOSE <<"chi2/ndof="<<chi2/ndof<<" "<<"Mean="<<m<<" "<<"RMS="<<r
		     <<" par 0 1 2 3 " << par[0] <<" "<<par[1]<<" "<<par[2]<< " "<<par[3]<<endmsg;

	}
	// THE NEW HISTOGRAM HAS BEEN FITTED

      }

      stc.adcCal=par[1]; 
      fi.adc_par[0] = par[1] ;
      fi.adc_par[1] = par[2] ;
      fi.adc_err[0] = errpar[1] ;
      fi.adc_err[1] = errpar[2] ;
      if(std::abs(ndof)==0) {fi.adc_chi2 = -1;}
      else { fi.adc_chi2 = chi2/static_cast<float>(ndof);}
    }
  }

  void  T0CalibrationClassic::searchParams(TH1 * h, double * p, int np) {
    int nbinsX=h->GetNbinsX();
    double sizeX = h->GetBinWidth(1);
    double oldSizeX=sizeX;
    int RebinFactor = static_cast<int>(10./sizeX);
    // extract starting values for fit params p[np] from the Time Spectrum h
    TH1 *hnew = h->Rebin(RebinFactor,"hnew"); // creates a new histogram hnew
    //merging 5 bins of h1 in one bin
    MsgStream log(Athena::getMessageSvc(),"T0ClassicSettings");
    if(log.level()<=MSG::DEBUG) log << MSG::DEBUG << "nbinsx,sizex,rebinfactor="<<nbinsX<<" "<<sizeX<<" "<<RebinFactor<<endmsg;
    float minDeriv(9999.); 
    int minDerivBin(0);
    sizeX = hnew->GetBinWidth(1);
    int newbins = hnew->GetNbinsX();
    for(int i=0;i<np;++i){if(log.level()<=MSG::DEBUG) log << MSG::DEBUG << "i,p(i) "<<i<<" "<<p[i]<<endmsg;}
    for(int i=0; i<newbins-1; ++i) {
      if(hnew->GetBinContent(i)-hnew->GetBinContent(i+1)
	 <minDeriv) {
	minDeriv = hnew->GetBinContent(i)-hnew->GetBinContent(i+1) ;
	minDerivBin = i ;
      }
    }
    float t0guess = hnew->GetBinCenter(minDerivBin);
    if (minDerivBin<newbins-1) {
      t0guess += (hnew->GetBinCenter(minDerivBin+1)-hnew->GetBinCenter(minDerivBin))/2.;
    }
    if(log.level()<=MSG::DEBUG) log << MSG::DEBUG << " t0guess is "<<t0guess<<endmsg;
    //
    // =================== Noise level search ===================================
    //
    float noise(0) ;
    int numOfBins(10), numOfBinsOffset(3) ;
    int imin, imax ;
    if (minDerivBin>numOfBins+numOfBinsOffset) {
      imin = minDerivBin-numOfBins-numOfBinsOffset;
      imax = minDerivBin-numOfBinsOffset;
    } else {
      imin = 0 ;
      if (minDerivBin>numOfBinsOffset) {
	imax = minDerivBin-numOfBinsOffset;
      } else {
	imax = minDerivBin;
      }
    }
    int icount(0) ;
    for (int i=imin; i<=imax; ++i) {
      noise += hnew->GetBinContent(i);
      icount++ ;
    }
	   
    noise = noise/(float)(icount) ;
    if(log.level()<=MSG::DEBUG) log << MSG::DEBUG << " noise is "<<noise<<endmsg;
    //
    // =================== Normalization =========================================
    //
    int t0bin = minDerivBin;
    int ix1 = t0bin+(int)(50/sizeX);
    int ix2 = t0bin+(int)(500/sizeX);
    if(log.level()<=MSG::DEBUG) log << MSG::DEBUG << "t0bin,ix1,ix2 "<<t0bin<<" "<<ix1<<" "<<ix2<<endmsg;
    float P1=p[1];
    float P2=p[2];
    float P3=p[3];
    if(log.level()<=MSG::DEBUG) log << MSG::DEBUG << "P1,P2,P3 start are "<<P1<<" "<<P2<<" "<<P3<<endmsg;
      p[0]=noise;
      p[4]=t0guess;
      p[5]=p[4]+700;
      p[1]=20.;
      p[2]=10.; 
    if (0<ix1 && ix1<newbins && 0<ix2 && ix2<newbins ) {
      float a1=hnew->GetBinCenter(ix1);
      float a2=hnew->GetBinCenter(ix2);
      float cont1=hnew->GetBinContent(ix1);
      float cont2=hnew->GetBinContent(ix2);
      if (cont1>0. && cont2>0. ){
      float A1=std::exp(-(a1-t0guess)/P3);
      float A2=std::exp(-(a2-t0guess)/P3);
      // do not forget rebinning!
      P2 = (cont1/cont2-1.)/(A1-cont1/cont2*A2);
      P1 = cont1/(1+P2*A1);
      P1=P1*oldSizeX/sizeX ;
      P2=P2*oldSizeX/sizeX ;
      if(log.level()<=MSG::DEBUG) {
        log << MSG::DEBUG << "a1,a2 "<<a1<<" "<<a2<<" cont1,cont2 "<<cont1<<" "<<cont2<<" A1,A2 "<<A1<<" "<<A2<<endmsg;
        log << MSG::DEBUG << " t0Guess .... P1, P2 " <<P1<<" "<<P2<<endmsg;
      }
      p[1]=P1;
      p[2]=P2;
     }
    }
    delete hnew;
    return;
  }

  const IMdtCalibrationOutput* T0CalibrationClassic::getResults() const {
    return new T0CalibrationOutput(m_result);
  }

  T0ClassicHistos* T0CalibrationClassic::getHistos(unsigned int idtube) {
    ToString ts;
    std::string HistoId;
    if((int)(idtube/100000000)==9){
      int mezz=(idtube)%(900000000);
      HistoId="time_mezz_"+ts(mezz);
    } else if(idtube==0){
      HistoId="time";
    } else if(idtube==1){
      HistoId="time_ML1";
    } else if(idtube==2){
      HistoId="time_ML2";
    } else if(idtube==11){
      HistoId="time_ML1_series1";
    } else if(idtube==12){
      HistoId="time_ML1_series2";
    } else if(idtube==13){
      HistoId="time_ML1_series3";
    } else if(idtube==21){
      HistoId="time_ML2_series1";
    } else if(idtube==22){
      HistoId="time_ML2_series2";
    } else if(idtube==23){
      HistoId="time_ML2_series3";
    } else {
      MuonFixedId fId(idtube); 
      int nML=fId.mdtMultilayer();
      int nL=fId.mdtTubeLayer();
      int nT=fId.mdtTube();
      int tubeid=(nML*1000)+(nL*100)+nT;
      int nstat=fId.stationName();
      std::string stationName=fId.stationNumberToFixedStationString(nstat);
      int eta = fId.eta();
      int phi = fId.phi();
      HistoId="time_"+stationName+"_"+ts(eta)+"_"+ts(phi)+"_"+ts(tubeid);
    }
    T0ClassicHistos *ret = NULL; 
    TH1F *timeHis = (TH1F*) m_regiondir->Get(HistoId.c_str());
    // We will either find the pointer to an existing histogram or create a new one
    if(!timeHis) {   //book histo if it does not exist
      ret = bookHistos(idtube);
    } else {  // else loop over m_histos histograms to look for the set of histos of tube idtube 
      for(std::vector<T0ClassicHistos*>::iterator it =m_histos.begin(); it!=m_histos.end();++it) {
	if((*it)->time==timeHis) {
	  ret = (*it);
	  break;
	}
      }
    }
    return ret;
  }    //end T0CalibrationClassic::getHistos

  T0ClassicHistos* T0CalibrationClassic::bookHistos(unsigned int idtube) {
    T0ClassicHistos *histos = new T0ClassicHistos(); 
    ToString ts;
    std::string histonametdc;
    std::string histonameadc;

    histos->id=idtube;
    m_regiondir->cd();
    if((int)(idtube/100000000)==9){
      int mezz=(idtube)%(900000000);
      histonametdc="time_mezz_"+ts(mezz);
      histonameadc="charge_mezz_"+ts(mezz);
    } else if(idtube==0){
      histonametdc="time";
      histonameadc="charge";
    } else if(idtube==1){
      histonametdc="time_ML1";
      histonameadc="charge_ML1";
    } else if(idtube==2){
      histonametdc="time_ML2";
      histonameadc="charge_ML2";
    } else if(idtube==11){
      histonametdc="time_ML1_series1";
      histonameadc="charge_ML1_series1";
    } else if(idtube==12){
      histonametdc="time_ML1_series2";
      histonameadc="charge_ML1_series2";
    } else if(idtube==13){
      histonametdc="time_ML1_series3";
      histonameadc="charge_ML1_series3";
    } else if(idtube==21){
      histonametdc="time_ML2_series1";
      histonameadc="charge_ML2_series1";
    } else if(idtube==22){
      histonametdc="time_ML2_series2";
      histonameadc="charge_ML2_series2";
    } else if(idtube==23){
      histonametdc="time_ML2_series3";
      histonameadc="charge_ML2_series3";
    } else {
      MuonFixedId fId(idtube); 
      int nML=fId.mdtMultilayer();
      int nL=fId.mdtTubeLayer();
      int nT=fId.mdtTube();
      int nstat=fId.stationName();
      int tubeid=(nML*1000)+(nL*100)+nT;
      std::string stationName=fId.stationNumberToFixedStationString(nstat);
      int eta = fId.eta();
      int phi = fId.phi();
      histonametdc="time_"+stationName+"_"+ts(eta)+"_"+ts(phi)+"_"+ts(tubeid);
      histonameadc="charge_"+stationName+"_"+ts(eta)+"_"+ts(phi)+"_"+ts(tubeid);
    }

    histos->time=new TH1F(histonametdc.c_str(),"Drift Time",m_settings->binTime(),m_settings->minTime(),m_settings->maxTime()); 
    histos->adc= new TH1F(histonameadc.c_str(),"ADC",m_settings->binAdc(),m_settings->minAdc(),m_settings->maxAdc());

    m_histos.push_back(histos);  
    return histos;
  }

  void T0CalibrationClassic::setInput( const IMdtCalibrationOutput* calib_in ) {
    // This method is called both by the event loop and by the tool.
    // Only the call from the tool is relevant for this implementation
    // and should be performed only once.
 
    if(m_result) return;

    const T0CalibrationOutput* t0Input = dynamic_cast<const T0CalibrationOutput*>(calib_in);
    if( t0Input != NULL ) {
      m_result = t0Input->t0s(); 
      m_result->setImplementation("T0CalibrationClassic");
    }
  }

  bool T0CalibrationClassic::converged() const {
    return m_converged;
  }

}
