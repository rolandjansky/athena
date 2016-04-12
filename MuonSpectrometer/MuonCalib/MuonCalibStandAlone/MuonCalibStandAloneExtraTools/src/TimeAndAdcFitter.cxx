/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 13.08.2008, AUTHOR: MAURO IODICE
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
#include "MuonCalibStandAloneExtraTools/TimeAndAdcFitter.h"

//   TimeAndAdcFitter::TimeAndAdcFitter() {
//   cout<<"Hello !"<<endl;
//
//   }
//   TimeAndAdcFitter::~TimeAndAdcFitter() {
//   cout<<"Bye Bye"<<endl;
//   }

void TimeAndAdcFitter::doTimeFit(TH1F *h, const int nParams, double *pfit, double *errfit, double *pchi2, int *pndof ) {

  // TMinuit *gMinuit = new TMinuit();
  
  // const int nParams = 8;

  double minTime = 0.;
  double maxTime = 2000.;

  // double * pfit    = new double[nParams];
  // double * errfit  = new double[nParams];
  // double * pdefault= new double[nParams];
 /*
    pdefault[0]= 0.;
    pdefault[1]= 100.;
    pdefault[2]= 5.;
    pdefault[3]= 100.;
    pdefault[4]= 600.;
    pdefault[5]= 1300.;
    pdefault[6]= 10.;
    pdefault[7]= 10.;
*/ 

  double chi2;
  int ndof;

  // THIS SETTINGS VARIABLE HAS TO BE IMPLEMENTED (as of March 7, 2007)  : 
  // int fitMezz = m_settings->fitMezzanine();
  // int fitMezz(123); 

  /********************************
    int fitMezz(1); 

    int nParams=m_settings->numParams();
    double * errfit=new double[nParams];
    double chi2;
    int ndof;
           
    MuonFixedId fId(T0h->id);

    int isMultilayer(0);

    if (T0h->id==1 || T0h->id==2 || (T0h->id>10&&T0h->id<=23)) isMultilayer=1;

    if (( fId.isValid() && fId.is_mdt()) || isMultilayer ) {}

      std::cout<<" STARTING doTimeFit "<< std::endl;
      TH1 * h; 
      if (!fitMezz || isMultilayer) {
        std::cout<< " DEBUGGGG : " << T0h->id << std::endl;
	h=T0h->time;
      }

      if (fitMezz==123 && !isMultilayer) {                       // FIT Mixed 
	h=T0h->time;
        if (h->GetEntries()<m_settings->entries()) {
	   int hIdMezz = fId.mdtMezzanine();
	   ToString ts;
   	   std::string HistoId(std::string("time_mezz_") + ts((hIdMezz)%(900000000)));
   	   TH1F * timeHis=(TH1F*) m_regiondir->Get(HistoId.c_str());
	   if(!timeHis) {
	     delete [] pfit;
	     delete [] errfit;
	     return;
	   }

	   T0ClassicHistos* histosMezz = getHistos(fId.mdtMezzanine());
	   h= histosMezz->time;
           if (h->GetEntries()<m_settings->entries()) {
	      int nML=fId.mdtMultilayer();
	      T0ClassicHistos* histosML = getHistos(nML);
	      h= histosML->time;
           }
        }
      } 

      if (fitMezz==1 && !isMultilayer) {                       // FIT MEZZANINE 
	int hIdMezz = fId.mdtMezzanine();

	ToString ts;
	std::string HistoId(std::string("time_mezz_") + ts((hIdMezz)%(900000000)));
	if (m_settings->printLevel() <= 2) {
	  std::cout<<" doTimeFit HistogramId : "<< T0h->id << std::endl;
	  std::cout<<" doTimeFit Histogram : "<< HistoId << std::endl;
	}
	TH1F * timeHis=(TH1F*) m_regiondir->Get(HistoId.c_str());
	if(!timeHis) {
	  delete [] pfit;
	  delete [] errfit;
	  return;
	}

	T0ClassicHistos* histosMezz = getHistos(fId.mdtMezzanine());
	h= histosMezz->time;
      }

      if (fitMezz==2 && !isMultilayer) {                        // FIT MULTILAYER 
	 int nML=fId.mdtMultilayer();
	 T0ClassicHistos* histosML = getHistos(nML);
	 h= histosML->time;
      }

  ********************************/

  //int minEntries = 1000;     
  //Double_t entries=h->GetEntries();

  //  if( m_verbose > 0) cout <<" histogram "<<h->GetName()
  //			  <<" "<<h->GetTitle()
  //			  <<" entries="<<h->GetEntries() <<endl;

      // CHECK whether the Selected Histogram has enough entries 
      // if (entries >= minEntries) {
    
      // CHECK whether the histogram has been already fitted 
      /*
      TF1 * FitFunction = h->GetFunction("TimeSpectrum");
      if (FitFunction){
	 double chiquadro = FitFunction->GetChisquare();
	 for (int i=0; i<nParams; i++){
	     pfit[i]    = FitFunction->GetParameter(i);
	     errfit[i] = FitFunction->GetParError(i);
	 }
	 chi2     = FitFunction->GetChisquare(); // total chi2
	 ndof     = FitFunction->GetNDF();        // number of degrees of freedom
      } 
         else    // The Selected Histo has NOT been fitted. Fit Now   
      {
*/
      
  TF1 *TimeSpectrum = new TF1("TimeSpectrum",
			      "[0]+([1]*(1+[2]*exp(-(x-[4])/[3])))/((1+exp((-x+[4])/[6]))*(1+exp((x-[5])/[7]))) ",
			      minTime,maxTime); 
  TimeSpectrum->SetLineColor(3);
  //  for (int i=0;i<nParams;i++) {
  //	     pfit[i]=*(pdefault++);
  //    if ( m_verbose >=1 ) 
  //      cout <<"TimeAndAdcFitter::doTimeFit initial parameter " <<i<< " = " << pfit[i] << endl;
  //  }
  //	if ( !m_settings->initParamFlag() ) {}
	 
  searchParams(h,&pfit[0],nParams);  
  //  if ( m_verbose>=1 ) {
  //    cout<<" doTimeFit parameters after searchParams " << endl;
  //    for(int i=0;i<nParams;++i) { cout << "i,pfit(i) "<<i<<" "<<pfit[i]<< endl;}
  //  }

  //DEBUG cout <<" Sto per mettere i limiti"<<std::endl;
  TimeSpectrum->SetParameters(pfit);
  TimeSpectrum->SetParLimits(0,0.,100.);
  TimeSpectrum->SetParLimits(1,0.,10000.);
  TimeSpectrum->SetParLimits(2,0.,40.);
  TimeSpectrum->SetParLimits(3,50.,400.);
  TimeSpectrum->SetParLimits(4,0.,1500.);
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
  double xmax = TimeSpectrum->GetParameter(5)+250.;
  h->Fit("TimeSpectrum","QLB","",xmin,xmax);

//	gMinuit->mnemat(&matrix[0][0],nParams);

  for (int i=0; i<nParams; i++){
    pfit[i]    = TimeSpectrum->GetParameter(i);
    errfit[i]  = TimeSpectrum->GetParError(i);
    // std::cout << " cov(ii) = " << matrix[i][i] << std::endl;
  }
  chi2     = TimeSpectrum->GetChisquare(); // total chi2
  ndof     = TimeSpectrum->GetNDF();        // number of degrees of freedom
  //}
  // THE NEW HISTOGRAM HAS BEEN FIT 

  //  if ( m_verbose>=1 ) {
  //    cout<<" doTimeFit FINAL parameters : " << endl;
  //    for(int i=0;i<nParams;++i) { cout << "i,pfit(i) "<<i<<" "<<pfit[i]<< endl;}
  //  }
  //  if( m_verbose>=1 ) cout<<" fit results chi2/ndof="<<chi2/ndof<<" T0="<<pfit[4]<<" err="<<errfit[4]<<std::endl; 

/*
	if(chi2/ndof < m_settings->chi2max()) {
	  stc.statusCode=0;// success
	} else {
	  stc.statusCode=3; // bad chi2 
	}
	stc.t0=pfit[4]; 
	fi.chi2Tdc=chi2/ndof;
	for (int i=0; i<nParams; i++) fi.par[i]=pfit[i];
*/


        // NOW we get rid of the covariance matrix 
        /******************************************************* 
	int jj=0;
	for (int i=0; i<8; i++) {
	  for (int k=0; k<i+1; k++) {
	    std::cout << " i k matrix[i][k] = "<<i<<" "<<k<<" "<< matrix[i][k] <<std::endl;
	    fi.cov[jj++] = matrix[k][i]; 
	  } 
	}
        *******************************************************/
        // NOW we  set the the first 8 values of fi.cov to errfit
//        for (int i=0; i<nParams; i++) fi.cov[i]=errfit[i];

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

//       } else {
// 
//         if ( m_verbose>=1) cout << " Too few entries " << endl;
//	stc.statusCode=2; // too few entries 
//      }

     // delete [] pfit;
     // delete [] errfit;
	
  *pchi2 = chi2;
  *pndof = ndof;
  //  if ( m_verbose>=1) cout<<" ENDING doTimeFit "<< endl;
  return;
}  //end TimeAndAdcFitter::doTimeFit

void TimeAndAdcFitter::doAdcFit(TH1F *h, const int /*nAdcParams*/, double *Adcpfit, double *errAdcpfit, double* /*chi2Adc*/, int* /*ndofAdc*/ ) {

  //  double chi2;
  //  int ndof;
  double adcThreshold = 50.;
  double minAdc = 80.;
  double maxAdc = 300.;

  //  if( m_verbose > 0) cout <<" histogram "<<h->GetName()
  //                          <<" "<<h->GetTitle()
  //                          <<" entries="<<h->GetEntries() <<endl;

  double m=h->GetMean();
  double r=h->GetRMS();
  double maxval=h->GetMaximum();
  double adcpar[4];
  adcpar[0] = maxval*2.;
  adcpar[1] = m;
  adcpar[2] = r;
  adcpar[3] = r/3.;
  
  TF1 *AdcSpectrum = new TF1("AdcSpectrum"," ([0]*exp((x-[1])/[2]))/ (1.+exp((x-[1])/[3])) ", minAdc,maxAdc );
  AdcSpectrum->SetLineColor(3);
  AdcSpectrum->SetParameters(adcpar);
  double fitMin = m-(3*r);
  double fitMax = m+(3*r);
  if (fitMin<adcThreshold) fitMin = adcThreshold;
  if (fitMax>maxAdc ) fitMax = maxAdc;
  h->Fit("AdcSpectrum","Q"," ",fitMin,fitMax);
  //  chi2	= AdcSpectrum->GetChisquare();
  //  ndof	= AdcSpectrum->GetNDF();	
  for (int i=0; i<4; i++) {
    Adcpfit[i] = AdcSpectrum->GetParameter(i);
    errAdcpfit[i] = AdcSpectrum->GetParError(i);
  }
  //  if (m_verbose>0)
  //    std::cout<<"chi2/ndof="<<chi2/ndof<<" "<<"Mean="<<m<<" "<<"RMS="<<r
  //	     <<" par 0 1 2 3 " << Adcpfit[0] <<" "<<Adcpfit[1]<<" "<<Adcpfit[2]<< " "<<Adcpfit[3]<< std::endl;

  // THE NEW HISTOGRAM HAS BEEN FIT

  return;
}  //end TimeAndAdcFitter::doAdcFit

/*
  void  T0CalibrationClassic::doAdcFit(T0ClassicHistos * T0h, MdtTubeFitContainer::SingleTubeFit & fi, MdtTubeFitContainer::SingleTubeCalib & stc)
  {
    // THIS SETTINGS VARIABLE HAS TO BE IMPLEMENTED (as of March 7, 2007)  : 
    // int fitMezz = m_settings->fitMezzanine();
    // int fitMezz(123); 
    int fitMezz(1); 

    double chi2(0)	;
    int ndof(0);	
    Double_t par[4];
    Double_t errpar[4];

    for (int ii=0; ii<4; ii++) {
       par[ii] = 0.0;
       errpar[ii] = 0.0;
    }

    MuonFixedId fId(T0h->id);
    if (fId.isValid() && fId.is_mdt()) {
      std::cout<<" doAdcFit : checking Single tube entries"<< std::endl;
      double adcThreshold = 50.;
      TH1 * hcheck;
      hcheck = T0h->adc;
      int nhits = hcheck->GetEntries();

      int adcBinThreshold = (int)((adcThreshold-hcheck->GetBinLowEdge(1))/(hcheck->GetBinWidth(1))+1)  ;  //
      int nhitsAboveAdcCut = (int)  hcheck->Integral(adcBinThreshold,hcheck->GetNbinsX());
      std::cout<<" doAdcFit : TotHits, nhitsAboveAdcCut "<< nhits <<" "<<nhitsAboveAdcCut<< std::endl;

      fi.cov[20]=nhits;
      fi.cov[21]=nhitsAboveAdcCut;
 
      std::cout<<" STARTING doAdcFit "<< std::endl;

      TH1 * h;
      if (!fitMezz) {
	h=T0h->adc;
      }

      if (fitMezz==123) {                       // FIT Mixed
        h=T0h->adc;
        if (h->GetEntries()<m_settings->entries()) {
           int hIdMezz = fId.mdtMezzanine();
           ToString ts;
           std::string HistoId(std::string("time_mezz_") + ts((hIdMezz)%(900000000)));
           // std::cout <<" DEBUGG FITTING MEZZANINE "<<fId.mdtTube()
           //           <<" "<<fId.mdtTubeLayer()<<" "<<fId.mdtMultilayer()
           //           <<" "<<fId.mdtMezzanine() <<std::endl;

           TH1F * adcHis=(TH1F*) m_regiondir->Get(HistoId.c_str());
           if(!adcHis) {
             return;
           }

           T0ClassicHistos* histosMezz = getHistos(fId.mdtMezzanine());
           h= histosMezz->adc;
           if (h->GetEntries()<m_settings->entries()) {
              int nML=fId.mdtMultilayer();

               // std::cout <<" DEBUGG FITTING MULTILAYER "<<fId.mdtTube()
               //           <<" "<<fId.mdtTubeLayer()<<" "<<nML
               //           <<" "<<fId.mdtMezzanine() <<std::endl;

              T0ClassicHistos* histosML = getHistos(nML);
              h= histosML->adc;
           }
        }
      }

      if (fitMezz==1) {
	int hIdMezz = fId.mdtMezzanine();
	ToString ts;
	std::string HistoId(std::string("charge_mezz_") + ts((hIdMezz)%(900000000)));
	if (m_settings->printLevel() <= 2) {
	  std::cout<<" doAdcFit HistogramId : "<< T0h->id << std::endl;
	  std::cout<<" doAdcFit Histogram : "<< HistoId << std::endl;
	}
	TH1F * adcHis=(TH1F*) m_regiondir->Get(HistoId.c_str());
	if(!adcHis) {
	  return;
	}
	T0ClassicHistos* histosMezz = getHistos(fId.mdtMezzanine());
	h= histosMezz->adc;
      }
      if (fitMezz==2) {                        // FIT MULTILAYER
         int nML=fId.mdtMultilayer();
         T0ClassicHistos* histosML = getHistos(nML);
         h= histosML->adc;
      }

      if(m_settings->printLevel() <= 1)
	std::cout<<" histogram "<<h->GetName()<<" "<<h->GetTitle()
		 <<" entries="<<h->GetEntries()<<std::endl; 
      Stat_t entries=h->GetEntries();

      // CHECK whether the Selected Histogram has enough entries
      if ((int)entries > m_settings->entries()){

        // CHECK whether the histogram has been already fitted
        TF1 * FitFunction = h->GetFunction("AdcSpectrum");
        if (FitFunction){
	   chi2	= FitFunction->GetChisquare();
           ndof	= FitFunction->GetNDF();	
           for (int i=0; i<4; i++) {
	       par[i] = FitFunction->GetParameter(i);
	       errpar[i] = FitFunction->GetParError(i);
           }

      }
        else    // The Selected Histo has NOT been fitted. Fit Now
      {
	double m=h->GetMean();
	double r=h->GetRMS();
	double maxval=h->GetMaximum();
	double adcpar[4];
	adcpar[0] = maxval*2.;
	adcpar[1] = m;
	adcpar[2] = r;
	adcpar[3] = r/3.;
   
	TF1 * AdcSpectrum = new TF1("AdcSpectrum"," ([0]*exp((x-[1])/[2]))/ (1.+exp((x-[1])/[3])) ",
				    m_settings->minAdc(),m_settings->maxAdc() );
	AdcSpectrum->SetParameters(adcpar);
        double fitMin = m-(3*r);
        double fitMax = m+(3*r);
        if (fitMin<adcThreshold) fitMin = adcThreshold;
        if (fitMax>300 ) fitMax = 300.;
	h->Fit("AdcSpectrum","Q"," ",fitMin,fitMax);
	chi2	= AdcSpectrum->GetChisquare();
	ndof	= AdcSpectrum->GetNDF();	
        for (int i=0; i<4; i++) {
	    par[i] = AdcSpectrum->GetParameter(i);
	    errpar[i] = AdcSpectrum->GetParError(i);
        }
	if (m_settings->printLevel() <=1)
	           std::cout<<"chi2/ndof="<<chi2/ndof<<" "<<"Mean="<<m<<" "<<"RMS="<<r
		   <<" par 0 1 2 3 " << par[0] <<" "<<par[1]<<" "<<par[2]<< " "<<par[3]<< std::endl;

        }
        // THE NEW HISTOGRAM HAS BEEN FITTED
      
      } else {
	// stc.statusCode=2; // too few entries - DO NOT CHANGE statusCode coming from doTimeFit
      }

      stc.adcCal=par[1]; 
      // fi.chi2Adc=chi2/ndof; //  ??? esiste fi.chi2Adc ????
      // for (int i=0; i<nParams; i++) fi.par[i]=pfit[i];   ...dove li mettiamo ???
      fi.adc_par[0] = par[1];
      fi.adc_par[1] = par[2];
      fi.adc_cov[0] = errpar[1];
      fi.adc_cov[1] = errpar[2];
      fi.adc_cov[2] = chi2/ndof;
    }
  }

*/

void  TimeAndAdcFitter::searchParams(TH1 *h, double *p, int /*nParams*/) {
  //int nbinsX=h->GetNbinsX();
  double sizeX = h->GetBinWidth(1);
  double oldSizeX=sizeX;
  int RebinFactor = (int) (10./sizeX);
  // extract starting values for fit params p[nParams] from the Time Spectrum h
  TH1 *hnew = h->Rebin(RebinFactor,"hnew"); // creates a new histogram hnew
  //merging 5 bins of h1 in one bin
  //DEBUG std::cout << "nbinsx,sizex,rebinfactor="<<nbinsX<<" "<<sizeX<<" "<<RebinFactor<<std::endl;
  float minDeriv(9999.); 
  int minDerivBin(0);
  sizeX = hnew->GetBinWidth(1);
  int newbins = hnew->GetNbinsX();
  //DEBUG  for(int i=0;i<nParams;++i){std::cout << "i,p(i) "<<i<<" "<<p[i]<<std::endl;}
  for(int i=0; i<newbins-1; ++i) {
    if(hnew->GetBinContent(i)-hnew->GetBinContent(i+1)
       <minDeriv) {
      minDeriv = hnew->GetBinContent(i)-hnew->GetBinContent(i+1);
      minDerivBin = i;
    }
  }
  float t0guess = hnew->GetBinCenter(minDerivBin);
  if (minDerivBin<newbins-1) {
    t0guess += (hnew->GetBinCenter(minDerivBin+1)-hnew->GetBinCenter(minDerivBin))/2.;
  }
  //DEBUG  std::cout << " t0guess is "<<t0guess<<std::endl;
  //
  // =================== Noise level search ===================================
  //
  float noise(0);
  int numOfBins(10), numOfBinsOffset(3);
  int imin, imax;
  if (minDerivBin>numOfBins+numOfBinsOffset) {
    imin = minDerivBin-numOfBins-numOfBinsOffset;
    imax = minDerivBin-numOfBinsOffset;
  } else {
    imin = 0;
    if (minDerivBin>numOfBinsOffset) {
      imax = minDerivBin-numOfBinsOffset;
    } else {
      imax = minDerivBin;
    }
  }
  int icount(0);
  for (int i=imin; i<=imax; ++i) {
    noise += hnew->GetBinContent(i);
    icount++;
  }
	   
  noise = noise/(float)(icount);
  //DEBUG  std::cout << " noise is "<<noise<<std::endl;
  //
  // =================== Normalization =========================================
  //
  int t0bin = minDerivBin;
  int ix1 = t0bin+(int)(50/sizeX);
  int ix2 = t0bin+(int)(500/sizeX);
  //DEBUG  std::cout << "t0bin,ix1,ix2 "<<t0bin<<" "<<ix1<<" "<<ix2<<std::endl;
  float P1=p[1];
  float P2=p[2];
  float P3=p[3];
  //DEBUG  std::cout <<"P1,P2,P3 start are "<<P1<<" "<<P2<<" "<<P3<<std::endl;
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
      float A1=exp(-(a1-t0guess)/P3);
      float A2=exp(-(a2-t0guess)/P3);
      // do not forget rebinning!
      P2 = (cont1/cont2-1.)/(A1-cont1/cont2*A2);
      P1 = cont1/(1+P2*A1);
      P1=P1*oldSizeX/sizeX;
      P2=P2*oldSizeX/sizeX;
      //DEBUG  std::cout << "a1,a2 "<<a1<<" "<<a2<<" cont1,cont2 "<<cont1<<" "<<cont2<<" A1,A2 "<<A1<<" "<<A2<<std::endl;
      //DEBUG  std::cout << " t0Guess .... P1, P2 " <<P1<<" "<<P2<<std::endl;
      p[1]=P1;
      p[2]=P2;
    }
  }
  delete hnew;
  return;
}  //end TimeAndAdcFitter::searchParams
