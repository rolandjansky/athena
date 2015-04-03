/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////
//
// Filename : TileCorrelation.cxx
//
//  Author   : Valencia TileCal group, cristobal.cuenca@cern.ch,
//  Mantained by Ximo Poveda, jpoveda@cern.ch
// 
//  Created  : May, 2004
//  Moved to TileRecUtils on Jan'05
//
////////////////////////////////////////


#include "TileRecUtils/TileCorrelation.h"
#include "PathResolver/PathResolver.h"
#include "TileConditions/TileInfo.h"
#include "TileIdentifier/TileHWID.h"

#include "CLHEP/Matrix/Matrix.h"
//#include "TileConditions/TilePulseShapes.h"

#include <cmath>

using CLHEP::HepMatrix;


////////////////////////////////////////
TileCorrelation::TileCorrelation()
  : SS()
  , S()
  , R()
  , corr()
  , corr_sum()
  , corr_sum_sq()
  , ncorr(0.0)
  , N()
  , jentry(0) 
  , lag(0)
  , N_pairs()
  , N_d(0.0)
  , S1()
  , S2()
  , S11()
  , S12()
  , S22()
{
}


////////////////////////////////////////
TileCorrelation::~TileCorrelation()
{
}


////////////////////////////////////////
void TileCorrelation::SetCorrelationZero(MsgStream & log, int dignum)
{
  if (log.level()<=MSG::DEBUG)
    log<<MSG::DEBUG<<"TileCorrelation::SetCorrelationZero(log)"<<endreq;
  for (int ros=0;ros<4;ros++)
    for (int drawer=0;drawer<64;drawer++)
      for (int channel=0;channel<48;channel++)
	for (int gain=0;gain<2;gain++){
	  N[ros][drawer][channel][gain]=0;
	  //N[ros][drawer][channel][gain]=0;
	  for (int i=0;i<dignum;i++){
	    S[ros][drawer][channel][gain][i]=0.;
	    for (int j=0;j<dignum;j++){
	      SS[ros][drawer][channel][gain][i][j]=0.;
	      R[ros][drawer][channel][gain][i][j]=0.;	  
	    }
	  }
	  
	  for (lag=0;lag<9;lag++){
		S1[ros][drawer][channel][gain][lag]=0.;
		S2[ros][drawer][channel][gain][lag]=0.;
		S11[ros][drawer][channel][gain][lag]=0.;
		S12[ros][drawer][channel][gain][lag]=0.;
		S22[ros][drawer][channel][gain][lag]=0.;
		N_pairs[ros][drawer][channel][gain][lag]=0;
		corr_sum[ros][drawer][channel][gain][lag]=0.;
		corr_sum_sq[ros][drawer][channel][gain][lag]=0.;
	  }
	}

  for (lag=0;lag<9;lag++){
    corr[lag]=0.;
  }
  
}


////////////////////////////////////////
void TileCorrelation::SetCorrelationDelta(MsgStream & log, int dignum)
{
  if (log.level()<=MSG::DEBUG)
    log<<MSG::DEBUG<<"TileCorrelation::SetCorrelationDelta(log)"<<endreq;

  for (int ros=0;ros<4;ros++)
    for (int drawer=0;drawer<64;drawer++)
      for (int channel=0;channel<48;channel++)
	for (int gain=0;gain<2;gain++){
	  N[ros][drawer][channel][gain]=1;
	  for (int i=0;i<dignum;i++)    
	    for (int j=0;j<dignum;j++)
	      if (i==j) R[ros][drawer][channel][gain][i][j]=1.;	  
		else R[ros][drawer][channel][gain][i][j]=0.;	   
	}
}



////////////////////////////////////////
void TileCorrelation::Sum(vector<double> &digits, int ros, int drawer, int channel, int gain, MsgStream & log, bool m_debug, int &dignum)
{
  if (log.level()<=MSG::VERBOSE)
    log<<MSG::VERBOSE<<"TileCorrelation::Sum"<<endreq;

  double N_d=0.;
  dignum=digits.size();
  
  N[ros][drawer][channel][gain]++;
  N_d=double(N[ros][drawer][channel][gain]);
  for (int i=0;i<dignum;i++){
    S[ros][drawer][channel][gain][i]+=digits[i];	  
    for (int j=0;j<dignum;j++) SS[ros][drawer][channel][gain][i][j]+=digits[i]*digits[j];
    //for (int j=0;j<i+1;j++) SS[ros][drawer][channel][gain][i][j]+=digits[i]*digits[j];
  }
  
  if (m_debug){
    cout 
      <<" TileCorrelation::Sum, ros="<<ros
      <<" drawer="<<drawer
      <<" channel="<<channel
      <<" gain="<<gain
      <<" N="<<N[ros][drawer][channel][gain]
      <<" Sum[1]="<<S[ros][drawer][channel][gain][1] 
      <<" Sum[2]="<<S[ros][drawer][channel][gain][2] 
      <<" Sum[1][2]="<<SS[ros][drawer][channel][gain][1][2] 
      <<" Sum[1][1]="<<SS[ros][drawer][channel][gain][1][1] 
      <<" Sum[2][2]="<<SS[ros][drawer][channel][gain][2][2] 
      <<" B[1][2]="<<SS[ros][drawer][channel][gain][1][2]/N_d-S[ros][drawer][channel][gain][1]/N_d*S[ros][drawer][channel][gain][2]/N_d
      <<" Correlation[1][2]="<<(N_d*SS[ros][drawer][channel][gain][1][2]-S[ros][drawer][channel][gain][1]*S[ros][drawer][channel][gain][2])/sqrt((N_d*SS[ros][drawer][channel][gain][1][1]-S[ros][drawer][channel][gain][1]*S[ros][drawer][channel][gain][1])*(N_d*SS[ros][drawer][channel][gain][2][2]-S[ros][drawer][channel][gain][2]*S[ros][drawer][channel][gain][2]))
      <<endl;
  }
  
}



////////////////////////////////////////
void TileCorrelation::CalcCorrelation(MsgStream & log, int dignum)
{
  if (log.level()<=MSG::DEBUG)
    log<<MSG::DEBUG<<"TileCorrelation::CalcCorrelation"<<endreq;
  
  for (int ros=0;ros<4;ros++)
    for (int drawer=0;drawer<64;drawer++)
      for (int channel=0;channel<48;channel++)
	for (int gain=0;gain<2;gain++){
	  double N_d=double(N[ros][drawer][channel][gain]);
	  if (N_d>0.) cout<<" TileCorrelation::CalcCorrelation, ros="<<ros<<" drawer="<<drawer<<" channel="<<channel<<" gain="<<gain<<" N_d="<<N_d<<endl;
	  for (int i=0;i<dignum;i++)    
	      //	      for (int j=0;j<i+1;j++)
	    for (int j=0;j<dignum;j++){
	      if (N_d>0.){
		//		      cout<<"b  i="<<i<<" j="<<j<<endl
		// 			  <<" R[ros][drawer][channel][gain][i][j]="<<R[ros][drawer][channel][gain][i][j]<<endl
		// 			  <<" N_d="<<N_d<<endl			  
		// 			  <<" S[ros][drawer][channel][gain][i]="<<S[ros][drawer][channel][gain][i]<<endl
		// 			  <<" S[ros][drawer][channel][gain][j]="<<S[ros][drawer][channel][gain][j]<<endl
		// 			  <<" SS[ros][drawer][channel][gain][i][j]="<<SS[ros][drawer][channel][gain][i][j]<<endl
		// 			  <<" SS[ros][drawer][channel][gain][i][i]="<<SS[ros][drawer][channel][gain][i][i]<<endl
		// 			  <<" SS[ros][drawer][channel][gain][j][j]="<<SS[ros][drawer][channel][gain][j][j]<<endl
		//<<" S[ros][drawer][channel][gain][i]*S[ros][drawer][channel][gain][j]="<<S[ros][drawer][channel][gain][i]*S[ros][drawer][channel][gain][j]<<endl
		//<<" N_d*SS[ros][drawer][channel][gain][i][j]="<<N_d*SS[ros][drawer][channel][gain][i][j]<<endl
		// 			  <<" N_d*SS[ros][drawer][channel][gain][i][j]-S[ros][drawer][channel][gain][i]*S[ros][drawer][channel][gain][j]="<<N_d*SS[ros][drawer][channel][gain][i][j]-S[ros][drawer][channel][gain][i]*S[ros][drawer][channel][gain][j]<<endl
		// 			  <<" N_d*SS[ros][drawer][channel][gain][j][j]-S[ros][drawer][channel][gain][j]*S[ros][drawer][channel][gain][j])="<<N_d*SS[ros][drawer][channel][gain][j][j]-S[ros][drawer][channel][gain][j]*S[ros][drawer][channel][gain][j]<<endl
		// 			  <<" N_d*SS[ros][drawer][channel][gain][i][i]-S[ros][drawer][channel][gain][i]*S[ros][drawer][channel][gain][i]="<<N_d*SS[ros][drawer][channel][gain][i][i]-S[ros][drawer][channel][gain][i]*S[ros][drawer][channel][gain][i]<<endl
		// 			  <<" sqrt((N_d*SS[ros][drawer][channel][gain][i][i]-S[ros][drawer][channel][gain][i]*S[ros][drawer][channel][gain][i])*(N_d*SS[ros][drawer][channel][gain][j][j]-S[ros][drawer][channel][gain][j]*S[ros][drawer][channel][gain][j]))="<< sqrt((N_d*SS[ros][drawer][channel][gain][i][i]-S[ros][drawer][channel][gain][i]*S[ros][drawer][channel][gain][i])*(N_d*SS[ros][drawer][channel][gain][j][j]-S[ros][drawer][channel][gain][j]*S[ros][drawer][channel][gain][j]))
		//			  <<endl;																								   
		R[ros][drawer][channel][gain][i][j]=
		  (N_d*SS[ros][drawer][channel][gain][i][j]-S[ros][drawer][channel][gain][i]*S[ros][drawer][channel][gain][j])
		  /sqrt(
			(N_d*SS[ros][drawer][channel][gain][i][i]-S[ros][drawer][channel][gain][i]*S[ros][drawer][channel][gain][i])*
			(N_d*SS[ros][drawer][channel][gain][j][j]-S[ros][drawer][channel][gain][j]*S[ros][drawer][channel][gain][j]));
		//		      cout<<"R[ros][drawer][channel][gain][i][j]="<<R[ros][drawer][channel][gain][i][j]<<endl;
	      }
	      else R[ros][drawer][channel][gain][i][j]=-1234.;
	    }
	}
}



////////////////////////////////////////
void TileCorrelation::RunningCorrelation(vector<double> &digits, int ros, int drawer, int channel, int gain, MsgStream & log, bool /* m_debug */, int &dignum, int chthres)
{
  MSG::Level logLevel = log.level();
  bool lDebug = (logLevel<=MSG::DEBUG);
  bool lVerbose = (logLevel<=MSG::VERBOSE);
  if (lVerbose)
    log<<MSG::VERBOSE<<"TileCorrelation::RunningCorrelation"<<endreq;
  dignum=digits.size();

  //chthres=10;
  //update sums
  N[ros][drawer][channel][gain]++;
  jentry=N[ros][drawer][channel][gain];
  N_d=double(jentry);

  if (ros==1 && drawer==1 && channel==0 && gain==1)
    log<<MSG::INFO<<"Computing RunningCorrelation for jentry="<<jentry<<endreq;

  for (lag=1;lag<dignum;lag++){
    for (int i=0; i<dignum-lag; i++){	            
      S1[ros][drawer][channel][gain][lag-1]+=digits[i];
      S2[ros][drawer][channel][gain][lag-1]+=digits[i+lag];
      S12[ros][drawer][channel][gain][lag-1]+=digits[i]*digits[i+lag];
      S11[ros][drawer][channel][gain][lag-1]+=digits[i]*digits[i];
      S22[ros][drawer][channel][gain][lag-1]+=digits[i+lag]*digits[i+lag];
      N_pairs[ros][drawer][channel][gain][lag-1]++;	
    }
    if (lag==1 && ros==1 && drawer==1 && channel==0 && gain==1)
      if (lVerbose)
        log<<MSG::VERBOSE<<" jentry="<<jentry<<" N="<<N_pairs[ros][drawer][channel][gain][lag-1]<<" S1="<<S1[ros][drawer][channel][gain][lag-1]<<endreq;
    
    
    if (jentry>chthres){
      ncorr=double(N_pairs[ros][drawer][channel][gain][lag-1]);
      corr[lag-1]=
	(ncorr*S12[ros][drawer][channel][gain][lag-1]-
	 S1[ros][drawer][channel][gain][lag-1]*S2[ros][drawer][channel][gain][lag-1])/
	sqrt(
	     (ncorr*S11[ros][drawer][channel][gain][lag-1]-
	      S1[ros][drawer][channel][gain][lag-1]*S1[ros][drawer][channel][gain][lag-1])*
	     (ncorr*S22[ros][drawer][channel][gain][lag-1]-
	      S2[ros][drawer][channel][gain][lag-1]*S2[ros][drawer][channel][gain][lag-1]));
	  
      if (lag==1 && ros==1 && drawer==1 && channel==0 && gain==1)
        if (lDebug)
          log<<MSG::DEBUG
             <<" corr="<<corr[lag-1]
             <<" corr_sum="<<corr_sum[ros][drawer][channel][gain][lag-1]
             <<" corr_sum_sq="<<corr_sum_sq[ros][drawer][channel][gain][lag-1]
             <<endreq;

      corr_sum[ros][drawer][channel][gain][lag-1]+=corr[lag-1];
      corr_sum_sq[ros][drawer][channel][gain][lag-1]+=corr[lag-1]*corr[lag-1];
      //  	  corr_mean=corr_sum[lag-1]/(chthres-jentry);
      //  	  corr_RMS=sqrt(corr_sum_sq[lag-1]*(chthres-jentry)-corr_sum[lag-1]*corr_sum[lag-1])/(chthres-jentry)
      
      
      if (lag==1 && ros==1 && drawer==1 && channel==0 && gain==1)
        if (lDebug)
          log<<MSG::DEBUG
             <<" jentry="<<jentry<<" jentry-chthres="<<jentry-chthres<<" lag=1, ros=1, drawer=1, channel=0, gain=1"
             <<" corr="<<corr[lag-1]
	  // 	       <<" corr_mean="<<corr_sum[lag-1]/(jentry-chthres)
             <<" sum corr_mean="<<corr_sum[ros][drawer][channel][gain][lag-1]
             <<" corr_mean="<<corr_sum[ros][drawer][channel][gain][lag-1]/(jentry-chthres)
	  // 	       <<" RMS="<<sqrt(corr_sum_sq[lag-1]*(jentry-chthres)-corr_sum[lag-1]*corr_sum[lag-1])/(jentry-chthres)
             <<" sum RMS="<<corr_sum_sq[ros][drawer][channel][gain][lag-1]
             <<" RMS="<<sqrt(corr_sum_sq[ros][drawer][channel][gain][lag-1]*(jentry-chthres)
			   -corr_sum[ros][drawer][channel][gain][lag-1]*corr_sum[ros][drawer][channel][gain][lag-1])/(jentry-chthres)
             <<endreq;
    }
  }
}



////////////////////////////////////////
void TileCorrelation::CalcRunningCorrelation(MsgStream & log, int dignum, int chthres, bool m_7to9)
{
  if (log.level()<=MSG::VERBOSE)
    log<<MSG::VERBOSE<<"TileCorrelation::CalcRunningCorrelation"<<endreq;

  for (int ros=0;ros<4;ros++)
    for (int drawer=0;drawer<64;drawer++)
      for (int channel=0;channel<48;channel++)
	for (int gain=0;gain<2;gain++){
	  jentry=N[ros][drawer][channel][gain];
	  ncorr=double(jentry-chthres);
	  
	  if (jentry>0){
	    if (m_7to9 && dignum==7){
	      for (int i=0;i<9;i++)
		R[ros][drawer][channel][gain][i][i]=1.;
	      
	      for (lag=1;lag<9;lag++)
		for (int i=0;i<9-lag;i++){
		  if (lag<7){
		    R[ros][drawer][channel][gain][i][i+lag]=corr_sum[ros][drawer][channel][gain][lag-1]/ncorr;
		    R[ros][drawer][channel][gain][i+lag][i]=corr_sum[ros][drawer][channel][gain][lag-1]/ncorr;
		  }else{
		    R[ros][drawer][channel][gain][i][i+lag]=0.;
		    R[ros][drawer][channel][gain][i+lag][i]=0.;
		  }
		  
		  if (-1.>R[ros][drawer][channel][gain][i][i+lag] || R[ros][drawer][channel][gain][i][i+lag]>1.)
		    R[ros][drawer][channel][gain][i][i+lag]=0.;
		  if (-1.>R[ros][drawer][channel][gain][i+lag][i] || R[ros][drawer][channel][gain][i+lag][i]>1.)
		    R[ros][drawer][channel][gain][i+lag][i]=0.;
		  
		}
	    }else{
	      for (int i=0;i<dignum;i++)
		R[ros][drawer][channel][gain][i][i]=1.;
	      
	      for (lag=1;lag<dignum;lag++)
		for (int i=0;i<dignum-lag;i++){
		  R[ros][drawer][channel][gain][i][i+lag]=corr_sum[ros][drawer][channel][gain][lag-1]/ncorr;
		  R[ros][drawer][channel][gain][i+lag][i]=corr_sum[ros][drawer][channel][gain][lag-1]/ncorr;
		  if (-1.>R[ros][drawer][channel][gain][i][i+lag] || R[ros][drawer][channel][gain][i][i+lag]>1.)
		    R[ros][drawer][channel][gain][i][i+lag]=0.;
		  if (-1.>R[ros][drawer][channel][gain][i+lag][i] || R[ros][drawer][channel][gain][i+lag][i]>1.)
		    R[ros][drawer][channel][gain][i+lag][i]=0.;
		}
	    }
	  }
	}
}



////////////////////////////////////////
void TileCorrelation::PrintCorrelation(int dignum)
{

  cout<<" TileCorrelation::PrintCorrelation()..."<<endl;
  for (int ros=0;ros<1;ros++){
      cout<<" ros="<<ros<<endl;
      for (int drawer=31;drawer<32;drawer++){
	  cout<<" drawer="<<drawer<<endl;
	  for (int channel=17;channel<24;channel++){
	      cout<<" channel="<<channel<<endl;
	      for (int gain=0;gain<2;gain++){
		  cout<<" gain="<<gain<<endl;
		  for (int i=0;i<dignum;i++){		      
		      for (int j=0;j<dignum;j++){
			  cout<<" "<<R[ros][drawer][channel][gain][i][j];
		      }
		      cout<<endl;
		  }
		  cout<<endl;
	      }
	  }
      }
  }
  
}




////////////////////////////////////////
void TileCorrelation::SaveCorrelationSumm(bool m_deltaCorrelation, 
					  string m_OptFilterFile_CorrelationSumm,
					  const TileHWID *m_tileHWID,
					  MsgStream & log,
					  int dignum)
{
  MSG::Level logLevel = log.level();
  bool lDebug = (logLevel<=MSG::DEBUG);
  bool lVerbose = (logLevel<=MSG::VERBOSE);
  if (lDebug)
    log<<MSG::DEBUG<<" TileCorrelation::SaveCorrelationSumm"<<endreq;
  
  HepMatrix M_correlation(dignum,1,0);

  fstream *f_correlation = new fstream(m_OptFilterFile_CorrelationSumm.c_str(),fstream::out);
  if (f_correlation->is_open()) log<<MSG::INFO<<m_OptFilterFile_CorrelationSumm<<" file open"<<endreq;

  if (m_deltaCorrelation){
      //      for (int i=0;i<dignum;i++)
    for (int j=0;j<dignum;j++){
      int i=0;
      if (R[0][0][0][0][i][j]>-100000. &&  R[0][0][0][0][i][j]<100000.)
	M_correlation[i][j]=R[0][0][0][0][i][j];
      else
	M_correlation[i][j]=0.0;
    }
    
    
    *f_correlation<<M_correlation.T()<<endl;
  }else{
      for (int ros=0;ros<4;ros++)    
	for (int drawer=0;drawer<64;drawer++){
	  int frag= m_tileHWID->frag(ros+1,drawer);	    
	  for (int channel=0;channel<48;channel++)
	    for (int gain=0;gain<2;gain++){
              if (lVerbose)
                log<<MSG::VERBOSE
                   <<"ros "<<ros
                   <<"  drawer "<<drawer<<MSG::hex
                   <<"  frag0x "<<frag<<MSG::dec
                   <<"  channel "<<channel
                   <<"  gain "<<gain
                   <<"  N "<<N[ros][drawer][channel][gain]
                   <<endreq;

	      if (N[ros][drawer][channel][gain]>0){
		//for (int i=0;i<dignum;i++)
		for (int j=0;j<dignum;j++){
		  int i=0;
		  if (R[ros][drawer][channel][gain][i][j]>-100000. 
		      &&  R[ros][drawer][channel][gain][i][j]<100000.)
		    M_correlation[i][j]=R[ros][drawer][channel][gain][i][j];
		  else
		    M_correlation[i][j]=0.0;
		}
		
		
		
		*f_correlation<<"ros "<<ros
			      <<"  drawer "<<drawer<<std::hex
			      <<"  frag0x "<<frag<<std::dec
			      <<"  channel "<<channel
			      <<"  gain "<<gain
			      <<"  N "<<N[ros][drawer][channel][gain]
			      <<M_correlation.T();
		//				    <<M_correlation.T()<<endl;
		// 		for (int i=0;i<dignum;i++)
		// 		  for (int j=0;j<dignum;j++)				    
		// 		      *f_correlation<<R[ros][drawer][channel][gain][i][j]<< M_correlation[i][j]<<endl;
		
	      }
	    }	  
	}    
  }
  f_correlation->close();  
}


////////////////////////////////////////
void TileCorrelation::SaveCorrelationMatrix(bool m_deltaCorrelation, 
					    string m_OptFilterFile_CorrelationMatrix,
					    const TileHWID *m_tileHWID,
					    MsgStream & log,
					    int dignum)
{
  MSG::Level logLevel = log.level();
  bool lDebug = (logLevel<=MSG::DEBUG);
  bool lVerbose = (logLevel<=MSG::VERBOSE);
  if (lDebug)
    log<<MSG::DEBUG<<" TileCorrelation::SaveCorrelationMatrix"<<endreq;
  
  HepMatrix M_correlation(dignum,dignum,0);
  
  fstream *f_correlation = new fstream(m_OptFilterFile_CorrelationMatrix.c_str(),fstream::out);
  if (f_correlation->is_open()) log<<MSG::INFO<<m_OptFilterFile_CorrelationMatrix<<" file open"<<endreq;

  if (m_deltaCorrelation){
    for (int i=0;i<dignum;i++)
      for (int j=0;j<dignum;j++){
	if (R[0][0][0][0][i][j]>-100000. &&  R[0][0][0][0][i][j]<100000.)
	  M_correlation[i][j]=R[0][0][0][0][i][j];
	else
	  M_correlation[i][j]=0.0;
      }
    
    
    *f_correlation<<M_correlation<<endl;
  }else{
    for (int ros=0;ros<4;ros++)    
      for (int drawer=0;drawer<64;drawer++){
	int frag= m_tileHWID->frag(ros+1,drawer);	    
	for (int channel=0;channel<48;channel++)
	  for (int gain=0;gain<2;gain++){
            if (lVerbose)
              log<<MSG::VERBOSE
                 <<"ros "<<ros
                 <<"  drawer "<<drawer<<MSG::hex
                 <<"  frag0x "<<frag<<MSG::dec
                 <<"  channel "<<channel
                 <<"  gain "<<gain
                 <<"  N "<<N[ros][drawer][channel][gain]
                 <<endreq;
	    
	    if (N[ros][drawer][channel][gain]>0){
	      for (int i=0;i<dignum;i++)
		for (int j=0;j<dignum;j++){
		  if (R[ros][drawer][channel][gain][i][j]>-100000.
		      &&  R[ros][drawer][channel][gain][i][j]<100000.)
		    M_correlation[i][j]=R[ros][drawer][channel][gain][i][j];
		  else
		    M_correlation[i][j]=0.0;
		}
	      
	      
	      
	      *f_correlation<<"ros "<<ros
			    <<"  drawer "<<drawer<<std::hex
			    <<"  frag0x "<<frag<<std::dec
			    <<"  channel "<<channel
			    <<"  gain "<<gain
			    <<"  N "<<N[ros][drawer][channel][gain]
			    <<M_correlation<<endl;
	      // 		for (int i=0;i<dignum;i++)
	      // 		  for (int j=0;j<dignum;j++)				    
	      // 		      *f_correlation<<R[ros][drawer][channel][gain][i][j]<< M_correlation[i][j]<<endl;
	      
	    }
	  }	  
      }    
  }
  f_correlation->close();  
}



////////////////////////////////////////
void TileCorrelation::CalcWeights(bool m_deltaCorrelation,
				  vector<double> m_LshapeForm,
				  vector<double> m_HshapeForm,
				  vector<double> m_LdshapeForm,
				  vector<double> m_HdshapeForm,
				  string m_OptFilterFile_ai_lo,
				  string m_OptFilterFile_bi_lo,
				  string m_OptFilterFile_ai_hi,
				  string m_OptFilterFile_bi_hi,
				  const TileHWID *m_tileHWID,
				  MsgStream & log,
				  int dignum)
{
  MSG::Level logLevel = log.level();
  bool lDebug = (logLevel<=MSG::DEBUG);
  bool lVerbose = (logLevel<=MSG::VERBOSE);
  if (lDebug)
    log<<MSG::DEBUG<<" TileCorrelation::CalcWeights"<<endreq;
  
  HepMatrix Correlation(dignum,dignum,0), Inverse(dignum,dignum,0), Zero(dignum,dignum,0);
  HepMatrix PulseShape(dignum,1,0), DPulseShape(dignum,1,0);
  HepMatrix a(dignum,1,0), b(dignum,1,0);
  
  fstream *f_ai_lo = new fstream(m_OptFilterFile_ai_lo.c_str(), fstream::out);
  fstream *f_bi_lo = new fstream(m_OptFilterFile_bi_lo.c_str(), fstream::out);
  fstream *f_ai_hi = new fstream(m_OptFilterFile_ai_hi.c_str(), fstream::out);
  fstream *f_bi_hi = new fstream(m_OptFilterFile_bi_hi.c_str(), fstream::out);


  //Open Weights files
  if (f_ai_lo->is_open()&&f_ai_lo->is_open()&&f_ai_lo->is_open()&&f_ai_lo->is_open()) log<<MSG::INFO<<" Weights files open"<<endreq;
  else log<<MSG::INFO<<" Weights files didn't open succesfully"<<endreq;


  //pulse shape
  //  vector<double> new_shapeForm;
  //   double max=0.;
  //   int nmax=0;
  
  //   for (int i=0; i<int(m_shapeForm.size());i++)
  //     if (m_shapeForm[i]>max)
  //       {
  // 	max=m_shapeForm[i];
  // 	nmax=i;
  //       }
  //   new_shapeForm.resize(dignum*25,0.);

  //   if (lDebug)
  //     log<<MSG::DEBUG<<"m_shapeForm.size()="<<m_shapeForm.size()<<"m_shapeForm nmax="<<nmax<<" new_shapeForm.size()="<<new_shapeForm.size()<<" new_shapeForm nmax="<<dignum*25/2<<endreq; 
  
  //   for (int i=0;i<dignum*25;i++)
  //     {
  //       if (i<(dignum*25/2-nmax)) new_shapeForm[i]=0.;
  //       if (i>=(dignum*25/2-nmax) && i<(dignum*25/2-nmax+int(m_shapeForm.size()))) new_shapeForm[i]=m_shapeForm[i-dignum*25/2 +nmax];
  //       if (i>=(dignum*25/2-nmax+int(m_shapeForm.size()))) new_shapeForm[i]=0.;
  //     }
  
  if (lVerbose) {
    log<<MSG::VERBOSE<<"m_LshapeForm, m_LdshapeForm, m_HshapeForm, m_HdshapeForm"<<endreq;
    for(int i=0;i<int(m_LshapeForm.size());i++) log<<MSG::VERBOSE<<i<<" "
                                                   <<std::setw(18)<<std::setprecision(10)<<m_LshapeForm[i]<<" "
                                                   <<std::setw(18)<<std::setprecision(10)<<m_LdshapeForm[i]<<" "
                                                   <<std::setw(18)<<std::setprecision(10)<<m_HshapeForm[i]<<" "
                                                   <<std::setw(18)<<std::setprecision(10)<<m_HdshapeForm[i]<<" "
                                                   <<endreq;
  }
  //if (lDebug) {
  //   log<<MSG::DEBUG<<"m_HshapeForm"<<endreq;
  //   for(int i=0;i<int(m_HshapeForm.size());i++) log<<MSG::DEBUG<<"    "<<i<<" "<<m_HshapeForm[i]<<endreq;
  //   log<<MSG::DEBUG<<"m_LdshapeForm"<<endreq;
  //   for(int i=0;i<int(m_LdshapeForm.size());i++) log<<MSG::DEBUG<<"    "<<i<<" "<<m_LdshapeForm[i]<<endreq;
  //   log<<MSG::DEBUG<<"m_HdshapeForm"<<endreq;
  //   for(int i=0;i<int(m_HdshapeForm.size());i++) log<<MSG::DEBUG<<"    "<<i<<" "<<m_HdshapeForm[i]<<endreq;
  //}

  double Q1, Q2, Q3, Delta;
  int ierr=0;
  
  if (m_deltaCorrelation){
    for (int gain=0;gain<2;gain++)	  
      for (int pha=-12;pha<13;pha++){
	Correlation=Zero;
	Inverse=Zero;
	
	for (int i=0;i<dignum;i++)    
	  for (int j=0;j<dignum;j++)
	    Correlation[i][j]=R[0][0][0][0][i][j];
	
	Inverse=Correlation.inverse(ierr);
	if (ierr==0){		
	  for (int i=0;i<dignum;i++){
	    if (gain==0){
	      PulseShape[i][0]=m_LshapeForm[i*25+12+pha];
	      DPulseShape[i][0]=m_LdshapeForm[i*25+12+pha];
	    }else{
	      PulseShape[i][0]=m_HshapeForm[i*25+12+pha];
	      DPulseShape[i][0]=m_HdshapeForm[i*25+12+pha];
	    }
	  }			
	  
	  Q1=((PulseShape.T())*Inverse*PulseShape).determinant();
	  Q2=((DPulseShape.T())*Inverse*DPulseShape).determinant();
	  Q3=((DPulseShape.T())*Inverse*PulseShape).determinant();
	  Delta=Q1*Q2-Q3*Q3;
	  
	  a=Q2/Delta*Inverse*PulseShape-Q3/Delta*Inverse*DPulseShape;
	  b=Q3/Delta*Inverse*PulseShape-Q1/Delta*Inverse*DPulseShape;
	  
	  if (gain==0){
	    *f_ai_lo<<std::setw(6)<<pha;
	    for (int i=0;i<dignum;i++) *f_ai_lo<<std::setw(18)<<std::setprecision(10)<<a(i+1,1);
	    *f_ai_lo<<endl;
	    
	    *f_bi_lo<<std::setw(6)<<pha;
	    for (int i=0;i<dignum;i++) *f_bi_lo<<std::setw(18)<<std::setprecision(10)<<b(i+1,1);
	    *f_bi_lo<<endl;
	  }else{
	    *f_ai_hi<<std::setw(6)<<pha;
	    for (int i=0;i<dignum;i++) *f_ai_hi<<std::setw(18)<<std::setprecision(10)<<a(i+1,1);
	    *f_ai_hi<<endl;
	    
	    *f_bi_hi<<std::setw(6)<<pha;
	    for (int i=0;i<dignum;i++) *f_bi_hi<<std::setw(18)<<std::setprecision(10)<<b(i+1,1);
	    *f_bi_hi<<endl;
	  }
	}
      }   
  }else{
    for (int ros=0;ros<4;ros++)
      for (int drawer=0;drawer<64;drawer++){
	int frag = m_tileHWID->frag(ros+1,drawer);
	for (int channel=0;channel<48;channel++)
	  for (int gain=0;gain<2;gain++)	  
	    if (N[ros][drawer][channel][gain]>0){
	      if (gain==0){
		*f_ai_lo<<"ros "<<ros
			<<"  drawer "<<drawer<<std::hex
			<<"  frag0x "<<frag<<std::dec
			<<"  channel "<<channel
			<<"  N "<<N[ros][drawer][channel][0]
			<<endl;
		
		*f_bi_lo<<"ros "<<ros
			<<"  drawer "<<drawer<<std::hex
			<<"  frag0x "<<frag<<std::dec
			<<"  channel "<<channel
			<<"  N "<<N[ros][drawer][channel][0]
			<<endl;
	      }
	      if (gain==1){
		*f_ai_hi<<"ros "<<ros
			<<"  drawer "<<drawer<<std::hex
			<<"  frag0x "<<frag<<std::dec
			<<"  channel "<<channel
			<<"  N "<<N[ros][drawer][channel][1]
			<<endl;
		
		*f_bi_hi<<"ros "<<ros
			<<"  drawer "<<drawer<<std::hex
			<<"  frag0x "<<frag<<std::dec
			<<"  channel "<<channel
			<<"  N "<<N[ros][drawer][channel][1]
			<<endl;
	      }
	      
	      
	      for (int pha=-12;pha<13;pha++){
		Correlation=Zero;
		Inverse=Zero;
		
		for (int i=0;i<dignum;i++)    
		  for (int j=0;j<dignum;j++)
		    Correlation[i][j]=R[ros][drawer][channel][gain][i][j];
		
		Inverse=Correlation.inverse(ierr);
		if (ierr==0){		
		  for (int i=0;i<dignum;i++){
		    if (gain==0){
		      PulseShape[i][0]=m_LshapeForm[i*25+12+pha];
		      DPulseShape[i][0]=m_LdshapeForm[i*25+12+pha];
		    }else{
		      PulseShape[i][0]=m_HshapeForm[i*25+12+pha];
		      DPulseShape[i][0]=m_HdshapeForm[i*25+12+pha];
		    }
		    
		    
		    
		    // 			      PulseShape[i][0]=new_shapeForm[i*25+12+pha];
		    // 			      if ((i*25+12+pha)>0 || (i*25+12+pha)<224)
		    // 				DPulseShape[i][0]=.5*(new_shapeForm[i*25+13+pha]-new_shapeForm[i*25+11+pha]);
		    // 			      else DPulseShape[i][0]=0.;
		  }			
		  
		  //HepStd::cout<<" correlation "<<Correlation<<HepStd::endl;
		  //f_weights<<" correlation "<<Correlation<<endl;
		  //		    HepStd::cout<<" inverse Matrix "<<Correlation.inverse(ierr)<<HepStd::endl;
		  //HepStd::cout<<" inverse Matrix "<<Inverse<<HepStd::endl;
		  //f_weights<<" inverse Matrix "<<Inverse<<endl;
		  //		    HepStd::cout<<" Product "<<Inverse*Correlation<<HepStd::endl;
		  
		  //cout<<" Q1 number of columns="<<((PulseShape.T())*Inverse*PulseShape).num_col()
		  //<<" number of rows="<<((PulseShape.T())*Inverse*PulseShape).num_row()<<endl;
		  
		  Q1=((PulseShape.T())*Inverse*PulseShape).determinant();
		  Q2=((DPulseShape.T())*Inverse*DPulseShape).determinant();
		  Q3=((DPulseShape.T())*Inverse*PulseShape).determinant();
		  Delta=Q1*Q2-Q3*Q3;
		  
		  //cout<<" Coeffs: Q1="<<Q1<<" Q2="<<Q2<<" Q3="<<Q3<<" Delta="<<Delta<<endl;
		  a=Q2/Delta*Inverse*PulseShape-Q3/Delta*Inverse*DPulseShape;
		  b=Q3/Delta*Inverse*PulseShape-Q1/Delta*Inverse*DPulseShape;
		  
		  //HepStd::cout<<" a Weights= "<<a<<HepStd::endl;
		  //HepStd::cout<<" b Weights= "<<b<<HepStd::endl;
		  
		  if (gain==0){
		    *f_ai_lo<<std::setw(6)<<pha;
		    for (int i=0;i<dignum;i++) *f_ai_lo<<std::setw(18)<<std::setprecision(10)<<a(i+1,1);
		    *f_ai_lo<<endl;
				
		    *f_bi_lo<<std::setw(6)<<pha;
		    for (int i=0;i<dignum;i++) *f_bi_lo<<std::setw(18)<<std::setprecision(10)<<b(i+1,1);
		    *f_bi_lo<<endl;
		  }else{
		    *f_ai_hi<<std::setw(6)<<pha;
		    for (int i=0;i<dignum;i++) *f_ai_hi<<std::setw(18)<<std::setprecision(10)<<a(i+1,1);
		    *f_ai_hi<<endl;
		    
		    *f_bi_hi<<std::setw(6)<<pha;
		    for (int i=0;i<dignum;i++) *f_bi_hi<<std::setw(18)<<std::setprecision(10)<<b(i+1,1);
		    *f_bi_hi<<endl;
		  }
		}
	      }
	    }
      }      
  }
  
  f_ai_lo->close();
  f_bi_lo->close();
  f_ai_hi->close();
  f_bi_hi->close();
}




////////////////////////////////////////
void TileCorrelation::BuildPulseShape(vector<double> &m_pulseShape,
				      vector<double> &m_pulseShapeX,
				      vector<double> &m_pulseShapeT,
				      int dignum,
				      MsgStream &log)
{ 
  MSG::Level logLevel = log.level();
  bool lDebug = (logLevel<=MSG::DEBUG);
  bool lVerbose = (logLevel<=MSG::VERBOSE);
  if (lDebug)
    log<<MSG::DEBUG<<"TileCalorimeter::BuildPulseShape"<<endreq;
  
  //1: set m_pulseShape
  m_pulseShape.resize(dignum*25);
  if (lDebug)
    log<<MSG::DEBUG<<"Set dimension of m_pulseShape to dignum*25="<<dignum*25<<endreq;

  //2: scan m_pulseShapeT for: tmin, tmax, nt0 and size: m_pulseShapeX[nt0]=1.0;
  int nt0=0, size;
  double tmin=10000., tmax=-10000.;
  size=m_pulseShapeT.size();
  for (int i=0; i<size;i++){
    if (m_pulseShapeT[i]<tmin) tmin=m_pulseShapeT[i];
    if (m_pulseShapeT[i]>tmax) tmax=m_pulseShapeT[i];
    if (m_pulseShapeT[i]==0) nt0=i;
  }
  if (lDebug)
    log<<MSG::DEBUG<<"m_pulseShapeX & m_pulseShapeT size ="<<size<<", tmin="<<tmin<<", tmax="<<tmax<<" central point="<<nt0<<" m_pulseShapeT[nt0]="<<m_pulseShapeT[nt0]<<" m_pulseShapeX[nt0]="<<m_pulseShapeX[nt0]<<endreq;
  
  //3: fill m_pulseShape
  bool exact;
  int nminn, nminp;
  double minn, minp, tdist;
  m_pulseShape[dignum*25/2]=m_pulseShapeX[nt0];
  for (int i=1;i<dignum*25/2+1;i++){
    // negative times: 0->dignum*25/2    
    if (-i<tmin) m_pulseShape[dignum*25/2-i]=0.;
    else{
      exact=false;
      minn=-10000.;
      minp=10000.;
      nminn=0;
      nminp=size-1;
      for (int j=0;j<nt0+1&&!exact;j++){
	if (m_pulseShapeT[j]==double(-i)){
	  m_pulseShape[dignum*25/2-i]=m_pulseShapeX[j];
	  exact=true;
	}else{
	  tdist=m_pulseShapeT[j]-double(-i);
	  if (tdist < 0. && tdist>minn){
	    minn=tdist;
	    nminn=j;
	  }
	  if (tdist>0. && tdist<minp){
	    minp=tdist;
	    nminp=j;
	  }
	}
      }	  
      
      if (exact) {
        if (lVerbose)
          log<<MSG::VERBOSE<<"exact value found for time="<<-i<<" m_pulseShape="<<m_pulseShape[dignum*25/2-i]<<endreq;
      } else {
        if (lVerbose)
          log<<MSG::VERBOSE<<"exact value NOT found for time="<<-i
             <<" nminn="<<nminn<<" m_pulseShapeT="<<m_pulseShapeT[nminn]<<" m_pulseShapeX="<<m_pulseShapeX[nminn]<<std::endl 
             <<" nminp="<<nminp<<" m_pulseShapeT="<<m_pulseShapeT[nminp]<<" m_pulseShapeX="<<m_pulseShapeX[nminp]<<endreq;	  
	m_pulseShape[dignum*25/2-i]=m_pulseShapeX[nminn]+(m_pulseShapeX[nminp]-m_pulseShapeX[nminn])/(m_pulseShapeT[nminp]-m_pulseShapeT[nminn])*(-i-m_pulseShapeT[nminn]);
      }
      
    }
    
    // positive times: dignum*25/2->dignum*25
    if (i>tmax) m_pulseShape[dignum*25/2+i]=0.;
    else{
      exact=false;
      minn=-10000.;
      minp=10000.;	  
      nminn=0;
      nminp=size;
      for (int j=nt0;j<size&&!exact;j++){
	if (m_pulseShapeT[j]==double(i)){
	  m_pulseShape[dignum*25/2+i]=m_pulseShapeX[j];
	  exact=true;
	}else{
	  tdist=m_pulseShapeT[j]-double(i);
	  if (tdist<0)
	    if (tdist>minn)
	    {
	      minn=tdist;
	      nminn=j;
	    }
	  if (tdist>0)
	    if (tdist<minp){
	      minp=tdist;
	      nminp=j;
	    }
	}
      }
      if (exact) {
        if (lVerbose)
          log<<MSG::VERBOSE<<"exact value found for time="<<i<<" m_pulseShape="<<m_pulseShape[dignum*25/2+i]<<endreq;
      } else {
        if (lVerbose)
          log<<MSG::VERBOSE<<"exact value NOT found for time="<<i
	     <<" nminn="<<nminn<<" m_pulseShapeT="<<m_pulseShapeT[nminn]<<" m_pulseShapeX="<<m_pulseShapeX[nminn]<<std::endl 
	     <<" nminp="<<nminp<<" m_pulseShapeT="<<m_pulseShapeT[nminp]<<" m_pulseShapeX="<<m_pulseShapeX[nminp]<<endreq;	  
	
	m_pulseShape[dignum*25/2+i]=m_pulseShapeX[nminn]+(m_pulseShapeX[nminp]-m_pulseShapeX[nminn])/(m_pulseShapeT[nminp]-m_pulseShapeT[nminn])*(i-m_pulseShapeT[nminn]);
      }
    }
  }
}
