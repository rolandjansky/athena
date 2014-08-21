/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////
//
// Filename : TileOFCorrelation.cxx
//
//  Author   : Ximo Poveda, jpoveda@cern.ch
// 
////////////////////////////////////////


#include "TileCalibAlgs/TileOFCorrelation.h"
#include "TileIdentifier/TileHWID.h"

#include "CLHEP/Matrix/Matrix.h"
#include <CLHEP/Matrix/Vector.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cmath>


using namespace std;
using CLHEP::HepMatrix;
using CLHEP::HepVector;



////////////////////////////////////////
TileOFCorrelation::TileOFCorrelation()
  : ncorr(0.0)
  , jentry(0)
  , lag(0)
  , N_d(0.0)
{
  memset(corr,0,sizeof(corr));
  memset(m_corr,0,sizeof(m_corr));
}


////////////////////////////////////////
TileOFCorrelation::~TileOFCorrelation()
{
}


////////////////////////////////////////
void TileOFCorrelation::SetCorrelationZero(MsgStream & log, int dignum)
{

  log<<MSG::DEBUG<<"TileOFCorrelation::SetCorrelationZero(log)"<<endreq;
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
    if(lag<8)
      m_corr[lag]=0;
  }
  
}


////////////////////////////////////////
void TileOFCorrelation::SetCorrelationDelta(MsgStream & log, int dignum)
{
  log<<MSG::DEBUG<<"TileOFCorrelation::SetCorrelationDelta(log)"<<endreq;

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
void TileOFCorrelation::Sum(vector<float> &digits, int ros, int drawer, int channel, int gain, MsgStream & /*log*/, bool m_debug, int &dignum)
{
  
  double N_d=0.;
  dignum=digits.size();
  
  N[ros][drawer][channel][gain]++;
  N_d=double(N[ros][drawer][channel][gain]);
  for (int i=0;i<dignum;i++){
    S[ros][drawer][channel][gain][i]+=digits[i];	  
    for (int j=0;j<dignum;j++) SS[ros][drawer][channel][gain][i][j]+=digits[i]*digits[j];
  }
  
  if (m_debug){
    cout 
      <<" TileOFCorrelation::Sum, ros="<<ros
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
void TileOFCorrelation::CalcCorrelation(MsgStream & log, int dignum, bool m_7to9)
{
  log<<MSG::DEBUG<<"TileOFCorrelation::CalcCorrelation"<<endreq;

  double denominator=0.;
  
  for (int ros=0;ros<4;ros++)
    for (int drawer=0;drawer<64;drawer++)
      for (int channel=0;channel<48;channel++)
	for (int gain=0;gain<2;gain++){
	  double N_d=double(N[ros][drawer][channel][gain]);
	  //if (N_d>0.) cout<<" TileOFCorrelation::CalcCorrelation, ros="<<ros<<" drawer="<<drawer<<" channel="<<channel<<" gain="<<gain<<" N_d="<<N_d<<endl;
	  
	  if(dignum==7 && m_7to9)
	    for (int j=0;j<9;j++)
	      R[ros][drawer][channel][gain][j][j]=1.;

	  for (int i=0;i<dignum;i++)    
	      //	      for (int j=0;j<i+1;j++)
	    for (int j=0;j<dignum;j++){
	      if (N_d>0.){
		denominator= (N_d*SS[ros][drawer][channel][gain][i][i]-S[ros][drawer][channel][gain][i]*S[ros][drawer][channel][gain][i])*
		  (N_d*SS[ros][drawer][channel][gain][j][j]-S[ros][drawer][channel][gain][j]*S[ros][drawer][channel][gain][j]);
		if(denominator!=0){
		  R[ros][drawer][channel][gain][i][j]=
		    (N_d*SS[ros][drawer][channel][gain][i][j]-S[ros][drawer][channel][gain][i]*S[ros][drawer][channel][gain][j])/sqrt(denominator);
		}else{
		  R[ros][drawer][channel][gain][i][j]=0.;
		}
	
	      }
	      else R[ros][drawer][channel][gain][i][j]=-1234.;
	    }
	}
}



////////////////////////////////////////
void TileOFCorrelation::RunningCorrelation(vector<float> &digits, int ros, int drawer, int channel, int gain, MsgStream & log, bool /*m_debug*/, int &dignum, int chthres)
{
  log<<MSG::VERBOSE<<"TileOFCorrelation::RunningCorrelation"<<endreq;
  dignum=digits.size();

  double denominator=0.;

  //chthres=10;
  //update sums
  N[ros][drawer][channel][gain]++;
  jentry=N[ros][drawer][channel][gain];
  N_d=double(jentry);

  if (ros==1 && drawer==1 && channel==0 && gain==1)
    log<<MSG::VERBOSE<<"Computing RunningCorrelation for jentry="<<jentry<<endreq;

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
      log<<MSG::VERBOSE<<" jentry="<<jentry<<" N="<<N_pairs[ros][drawer][channel][gain][lag-1]<<" S1="<<S1[ros][drawer][channel][gain][lag-1]<<endreq;
    
    
    if (jentry>chthres){
      ncorr=double(N_pairs[ros][drawer][channel][gain][lag-1]);

      denominator=(ncorr*S11[ros][drawer][channel][gain][lag-1]-
		   S1[ros][drawer][channel][gain][lag-1]*S1[ros][drawer][channel][gain][lag-1])*
	(ncorr*S22[ros][drawer][channel][gain][lag-1]-
	 S2[ros][drawer][channel][gain][lag-1]*S2[ros][drawer][channel][gain][lag-1]);
      
      if(denominator!=0){
	corr[lag-1]=
	(ncorr*S12[ros][drawer][channel][gain][lag-1]-
	 S1[ros][drawer][channel][gain][lag-1]*S2[ros][drawer][channel][gain][lag-1])/sqrt(denominator);
      }else{
	corr[lag-1]=0.;
      }
      
      if (lag==1 && ros==1 && drawer==1 && channel==0 && gain==1)
	log<<MSG::DEBUG
	   <<" corr="<<corr[lag-1]
	   <<" corr_sum="<<corr_sum[ros][drawer][channel][gain][lag-1]
	   <<" corr_sum_sq="<<corr_sum_sq[ros][drawer][channel][gain][lag-1]
	   <<endreq;

      corr_sum[ros][drawer][channel][gain][lag-1]+=corr[lag-1];
      corr_sum_sq[ros][drawer][channel][gain][lag-1]+=corr[lag-1]*corr[lag-1];
      	
      if (lag==1 && ros==1 && drawer==1 && channel==0 && gain==1)
	log<<MSG::DEBUG
	   <<" jentry="<<jentry<<" jentry-chthres="<<jentry-chthres<<" lag=1, ros=1, drawer=1, channel=0, gain=1"
	   <<" corr="<<corr[lag-1]
	   <<" sum corr_mean="<<corr_sum[ros][drawer][channel][gain][lag-1]
	   <<" corr_mean="<<corr_sum[ros][drawer][channel][gain][lag-1]/(jentry-chthres)
	   <<" sum RMS="<<corr_sum_sq[ros][drawer][channel][gain][lag-1]
	   <<" RMS="<<sqrt(corr_sum_sq[ros][drawer][channel][gain][lag-1]*(jentry-chthres)
			   -corr_sum[ros][drawer][channel][gain][lag-1]*corr_sum[ros][drawer][channel][gain][lag-1])/(jentry-chthres)
	   <<endreq;
    }
  }
}



////////////////////////////////////////
void TileOFCorrelation::CalcRunningCorrelation(MsgStream & /*log*/, int dignum, int chthres, bool m_7to9)
{
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
void TileOFCorrelation::GetCorrelation(int dignum, float  tmpCorr[][9],int ros,int drawer,int chan,int gain)
{
	
	for(int i=0;i<dignum;i++)
	{
		for(int j=0;j<dignum;j++)
		{
			tmpCorr[i][j] = R[ros][drawer][chan][gain][i][j];
		}
	}
}

////////////////////////////////////////
void TileOFCorrelation::PrintCorrelation(int dignum)
{

  cout<<" TileOFCorrelation::PrintCorrelation()..."<<endl;
  for (int ros=3;ros<4;ros++){
      cout<<" ros="<<ros<<endl;
      for (int drawer=0;drawer<64;drawer++){
	  cout<<" drawer="<<drawer<<endl;
	  for (int channel=0;channel<48;channel++){
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
void TileOFCorrelation::SaveCorrelationSumm(bool m_deltaCorrelation, 
					  string m_OptFilterFile_CorrelationSumm,
					  const TileHWID *m_tileHWID,
					  MsgStream & log,
					  int dignum)
{
  log<<MSG::DEBUG<<" TileOFCorrelation::SaveCorrelationSumm"<<endreq;
  
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
		
	      }
	    }	  
	}    
  }
  f_correlation->close();  
}

////////////////////////////////////////
float * TileOFCorrelation::getCorrelationSumm(bool m_deltaCorrelation, 
					      int ros,
					      int drawer,
					      int channel,
					      int gain,
					      int dignum)
{

  if(!m_deltaCorrelation){
  
    if (N[ros][drawer][channel][gain]>0){
      for (int i=1;i<dignum;i++){
	if (R[ros][drawer][channel][gain][0][i]>-100000. 
	    &&  R[ros][drawer][channel][gain][0][i]<100000.)
	  m_corr[i-1]=R[ros][drawer][channel][gain][0][i];
	else
	  m_corr[i-1]=0.0;
      }
    }
    
  }
  
  return m_corr;
  
}

////////////////////////////////////////
void TileOFCorrelation::SaveCorrelationMatrix(bool m_deltaCorrelation, 
					      string m_OptFilterFile_CorrelationMatrix,
					      const TileHWID *m_tileHWID,
					      MsgStream & log,
					      int dignum)
{
  log<<MSG::DEBUG<<" TileOFCorrelation::SaveCorrelationMatrix"<<endreq;
  
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
	    }
	  }	  
      }    
  }
  f_correlation->close();  
}



////////////////////////////////////////
void TileOFCorrelation::CalcWeights(bool m_of2,
				    bool m_deltaCorrelation,
				    vector<double> m_LshapeForm,
				    vector<double> m_HshapeForm,
				    vector<double> m_LdshapeForm,
				    vector<double> m_HdshapeForm,
				    MsgStream & log,
				    int ros,
				    int drawer,
				    int channel,
				    int gain,
				    int dignum)
{
  log<<MSG::DEBUG<<" TileOFCorrelation::CalcWeights"<<endreq;
  
  HepMatrix Correlation(dignum,dignum,0), Inverse(dignum,dignum,0), Zero(dignum,dignum,0);
  HepMatrix PulseShape(dignum,1,0), DPulseShape(dignum,1,0);
  HepMatrix a(dignum,1,0), b(dignum,1,0), WeightZero(dignum,1,0);
  
  int ierr=0;

  if(m_of2)
    log<<MSG::INFO<<" Calculating OF2 weights ";
  else
    log<<MSG::INFO<<" Calculating OF1 weights ";
  if (m_deltaCorrelation)
    log<<MSG::INFO<<"with Delta correlation matrix "<<endreq;
  else
    log<<MSG::INFO<<"with correlation matrix obtained from data "<<endreq;
  log<<MSG::INFO<<"for ros="<<ros<<" drawer="<<drawer<<" channel="<<channel<<
    " gain="<<gain<<endreq;
  
  

  if (m_deltaCorrelation){
    
    for (int i=0;i<dignum;i++)    
      for (int j=0;j<dignum;j++)
	Correlation[i][j]=R[0][0][0][0][i][j];
    
    Inverse=Correlation.inverse(ierr);
    
    for (int pha=-100;pha<101;pha++){
      
      if (ierr==0){		
	for (int i=0;i<dignum;i++){
	  if (gain==0){
	    PulseShape[i][0]=m_LshapeForm[i*25+100+pha];
	    DPulseShape[i][0]=m_LdshapeForm[i*25+100+pha];
	  }else{
	    PulseShape[i][0]=m_HshapeForm[i*25+100+pha];
	    DPulseShape[i][0]=m_HdshapeForm[i*25+100+pha];
	  }
	  g[pha+100][i]=PulseShape[i][0];
	}			

	// Build System Matrix with Correlations and pulse function points
	
	if(!m_of2){  // OF1
	  HepMatrix SystemMatrix(dignum+2,dignum+2,0);
	  HepVector Result(dignum + 2,0);
	  HepVector IndependTermsAmp(dignum+2,0);
	  HepVector IndependTermsTime(dignum+2,0);
	  for (int i=0; i<dignum; i++){
	    
	    for (int j=0; j<dignum; j++){
	      SystemMatrix[i][j]=Correlation[i][j];
	    }
	    SystemMatrix[dignum][i]     = PulseShape[i][0];
	    SystemMatrix[i][dignum]     = PulseShape[i][0];
	    
	    SystemMatrix[dignum + 1][i] = DPulseShape[i][0];
	    SystemMatrix[i][dignum + 1] = DPulseShape[i][0];
	    
	  }
	  
	  IndependTermsAmp[dignum] = 1.;
	  IndependTermsTime[dignum+1] = -1.;
	  
	  Result  = solve(SystemMatrix,IndependTermsAmp);
	  
	  for (int ismp=0; ismp<dignum; ismp++) {
	    w_a[pha+100][ismp] = (double)Result[ismp];
	  }
	  
	  Result = solve(SystemMatrix,IndependTermsTime);
	  
	  for (int ismp=0; ismp<dignum; ismp++){
	    w_b[pha+100][ismp] = (double)Result[ismp];
	  }
	  
	}else{  // OF2
	  HepMatrix SystemMatrix(dignum+3,dignum+3,0);
	  HepVector Result(dignum+3,0);
	  HepVector IndependTermsAmp(dignum+3,0);
	  HepVector IndependTermsTime(dignum+3,0);
	  HepVector IndependTermsPed(dignum+3,0);
	  
	  for (int i=0; i<dignum; i++){
	    for (int j=0; j<dignum; j++){
	      SystemMatrix[i][j]=Correlation[i][j];
	    }
	    SystemMatrix[dignum][i]     = PulseShape[i][0];
	    SystemMatrix[i][dignum]     = PulseShape[i][0];
	    
	    SystemMatrix[dignum + 1][i] = DPulseShape[i][0];
	    SystemMatrix[i][dignum + 1] = DPulseShape[i][0];
	    
	    
	    SystemMatrix[dignum + 2][i] = 1.;
	    SystemMatrix[i][dignum + 2] = 1.;
	  }
	  
	  IndependTermsAmp[dignum] = 1.;
	  IndependTermsTime[dignum+1] = -1.;
	  IndependTermsPed[dignum+2] = 1.;
	  
	  Result  = solve(SystemMatrix,IndependTermsAmp);
	  
	  for (int ismp=0; ismp<dignum; ismp++) {
	    w_a[pha+100][ismp] = (double)Result[ismp];
	  }
	  
	  Result = solve(SystemMatrix,IndependTermsTime);
	  
	  for (int ismp=0; ismp<dignum; ismp++){
	    w_b[pha+100][ismp] = (double)Result[ismp];
	  }
	  
	  Result = solve(SystemMatrix,IndependTermsPed);
	  
	  for (int ismp=0; ismp<dignum; ismp++){
	    w_c[pha+100][ismp] = (double)Result[ismp];
	  }
	  
	}
      }else{
	for (int ismp=0; ismp<dignum; ismp++){
	  w_a[pha+100][ismp] = 0.;
	  w_b[pha+100][ismp] = 0.;
	  w_c[pha+100][ismp] = 0.;
	}
      }
    }// Pha   
  }else{
    printf("ros=%d  drawer=%d  channel=%d   gain=%d N=%d \n",
	   ros,drawer,channel,gain,N[ros][drawer][channel][gain]);	  
    if (N[ros][drawer][channel][gain]>0){
      
      for (int i=0;i<dignum;i++)    
	for (int j=0;j<dignum;j++)
	  Correlation[i][j]=R[ros][drawer][channel][gain][i][j];
      
      Inverse=Correlation.inverse(ierr);
      
      if(ierr!=0){
	log<<MSG::WARNING<<" Correlation matrix cannot be inverted for ros="<<ros<<" drawer="<<drawer
	   <<" channel="<<channel<<" gain="<<gain<<endreq;
	log<<MSG::WARNING<<" Weights set to zero for this channel..."<<endreq;
	for (int i=0; i<dignum; i++){
	  for (int j=0; j<dignum; j++){
	    printf("%4.4f  ",R[ros][drawer][channel][gain][i][j]);
	  }
	  printf("\n");
	}
      }
      
      for (int pha=-100;pha<101;pha++){
	
	if (ierr==0){		   
	  for (int i=0;i<dignum;i++){
	    if (gain==0){
	      PulseShape[i][0]=m_LshapeForm[i*25+100+pha];
	      DPulseShape[i][0]=m_LdshapeForm[i*25+100+pha];
	    }else{
	      PulseShape[i][0]=m_HshapeForm[i*25+100+pha];
	      DPulseShape[i][0]=m_HdshapeForm[i*25+100+pha];
	    }
	    g[pha+100][i]=PulseShape[i][0];
	  }   
	  
	  // Build System Matrix with Correlations and pulse function points
	  if(!m_of2){  // OF1
	    HepMatrix SystemMatrix(dignum+2,dignum+2,0);
	    HepVector Result(dignum + 2,0);
	    HepVector IndependTermsAmp(dignum+2,0);
	    HepVector IndependTermsTime(dignum+2,0);
	    
	    for (int i=0; i<dignum; i++){
	      for (int j=0; j<dignum; j++){
		SystemMatrix[i][j]=Correlation[i][j];
	      }
	      SystemMatrix[dignum][i]     = PulseShape[i][0];
	      SystemMatrix[i][dignum]     = PulseShape[i][0];
	      
	      SystemMatrix[dignum + 1][i] = DPulseShape[i][0];
	      SystemMatrix[i][dignum + 1] = DPulseShape[i][0];
	    }
	    
	    IndependTermsAmp[dignum] = 1.;
	    IndependTermsTime[dignum+1] = -1.;
	    
	    Result  = solve(SystemMatrix,IndependTermsAmp);
	    
	    for (int ismp=0; ismp<dignum; ismp++) {
	      w_a[pha+100][ismp] = (double)Result[ismp];
	    }
	    
	    Result = solve(SystemMatrix,IndependTermsTime);
	    
	    for (int ismp=0; ismp<dignum; ismp++){
	      w_b[pha+100][ismp] = (double)Result[ismp];
	    }
	  }else{ // OF2
	    
	    HepMatrix SystemMatrix(dignum+3,dignum+3,0);
	    HepVector Result(dignum + 3,0);
	    HepVector IndependTermsAmp(dignum+3,0);
	    HepVector IndependTermsTime(dignum+3,0);
	    HepVector IndependTermsPed(dignum+3,0);
	    
	    for (int i=0; i<dignum; i++){
	      for (int j=0; j<dignum; j++){
		SystemMatrix[i][j]=Correlation[i][j];
	      }
	      SystemMatrix[dignum][i]     = PulseShape[i][0];
	      SystemMatrix[i][dignum]     = PulseShape[i][0];
	      
	      SystemMatrix[dignum + 1][i] = DPulseShape[i][0];
	      SystemMatrix[i][dignum + 1] = DPulseShape[i][0];
	      
	      SystemMatrix[dignum + 2][i] = 1.;
	      SystemMatrix[i][dignum + 2] = 1.;
	    }
	    
	    IndependTermsAmp[dignum] = 1.;
	    IndependTermsTime[dignum+1] = -1.;
	    IndependTermsPed[dignum+2] = 1.;
	    
	    Result  = solve(SystemMatrix,IndependTermsAmp);
	    
	    for (int ismp=0; ismp<dignum; ismp++) {
	      w_a[pha+100][ismp] = (double)Result[ismp];
	    }
	    
	    Result = solve(SystemMatrix,IndependTermsTime);
	    
	    for (int ismp=0; ismp<dignum; ismp++){
	      w_b[pha+100][ismp] = (double)Result[ismp];
		}
	    
	    Result = solve(SystemMatrix,IndependTermsPed);
	    
	    for (int ismp=0; ismp<dignum; ismp++){
	      w_c[pha+100][ismp] = (double)Result[ismp];
	    }
	    
	    if(ros==1 && drawer==52 && channel==43 && gain==1){
	      printf("PHASE: %d   deter=%f\n",pha,SystemMatrix.determinant());
	      for (int i=0; i<dignum+2; i++){
		for (int j=0; j<dignum+2; j++){
		  printf("%4.4f  ",SystemMatrix[i][j]);
		}
		printf("\n");
	      }
	      for (int i=0; i<dignum; i++){
		for (int j=0; j<dignum; j++){
		  printf("%4.4f  ",R[ros][drawer][channel][gain][i][j]);
		}
		printf("\n");
	      }
	      for (int ismp=0; ismp<dignum; ismp++)
		printf("samp: %d ->w_a: %f  w_b:%f\n",
		       ismp,w_a[pha+100][ismp],w_b[pha+100][ismp]);
	    }
	  }
	      
	}else{
	  for (int ismp=0; ismp<dignum; ismp++){
	    w_a[pha+100][ismp] = 0.;
	    w_b[pha+100][ismp] = 0.;
	    w_c[pha+100][ismp] = 0.;
	  }
	}
      }// END pha
    }// END ch + gain + if N>0
    
  }// IF delta
  
  log<<MSG::VERBOSE<<"...weights calculated"<<endreq;
 
}




////////////////////////////////////////
void TileOFCorrelation::BuildPulseShape(vector<double> &m_pulseShape,
				      vector<double> &m_pulseShapeX,
				      vector<double> &m_pulseShapeT,
				      int dignum,
				      MsgStream &log)
{ 
  log<<MSG::DEBUG<<"TileCalorimeter::BuildPulseShape"<<endreq;
  
  //1: set m_pulseShape
  int shape_size=(dignum-1)*25+200;
  m_pulseShape.resize(shape_size);
  log<<MSG::DEBUG<<"Set dimension of m_pulseShape to shape_sie="<<shape_size<<endreq;

  //2: scan m_pulseShapeT for: tmin, tmax, nt0 and size: m_pulseShapeX[nt0]=1.0;
  int nt0=0, size;
  double tmin=10000., tmax=-10000.;
  int ntmin=0, ntmax=0;
  size=m_pulseShapeT.size();
  for (int i=0; i<size;i++){
    if (m_pulseShapeT[i]<tmin) {
      tmin=m_pulseShapeT[i];
      ntmin=i;
    }
    if (m_pulseShapeT[i]>tmax){
      tmax=m_pulseShapeT[i];
      ntmax=i;
    }
    if (m_pulseShapeT[i]==0) nt0=i;
  }
  log<<MSG::DEBUG<<"m_pulseShapeX & m_pulseShapeT size ="<<size<<", tmin="<<tmin<<", tmax="<<tmax<<"  nt0="<<nt0<<" m_pulseShapeT[nt0]="<<m_pulseShapeT[nt0]<<" m_pulseShapeX[nt0]="<<m_pulseShapeX[nt0]<<endreq;
  
  //3: fill m_pulseShape
  bool exact;
  int nminn, nminp;
  double minn, minp, tdist;
  m_pulseShape[(shape_size)/2]=m_pulseShapeX[nt0];
  //  for (int i=1;i<(shape_size)/2+1;i++){
  for (int i=1;i<(shape_size)/2;i++){
    // negative times: 0->(shape_size-1)/2    
    //   if (-i<tmin) m_pulseShape[(shape_size)/2-i]=0.;
    if (-i<tmin) m_pulseShape[(shape_size)/2-i]=m_pulseShapeX[ntmin];
    else{
      exact=false;
      minn=-10000.;
      minp=10000.;
      nminn=0;
      nminp=size-1;
      for (int j=0;j<nt0+1&&!exact;j++){
	if (m_pulseShapeT[j]==double(-i)){
	  m_pulseShape[(shape_size)/2-i]=m_pulseShapeX[j];
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
      
      if (exact)	    
	log<<MSG::VERBOSE<<"exact value found for time="<<-i<<" m_pulseShape="<<m_pulseShape[(shape_size)/2-i]<<endreq;
      else{
	//	  if (exact)	    
	log<<MSG::VERBOSE<<"exact value NOT found for time="<<-i
	   <<" nminn="<<nminn<<" m_pulseShapeT="<<m_pulseShapeT[nminn]<<" m_pulseShapeX="<<m_pulseShapeX[nminn]<<std::endl 
	   <<" nminp="<<nminp<<" m_pulseShapeT="<<m_pulseShapeT[nminp]<<" m_pulseShapeX="<<m_pulseShapeX[nminp]<<endreq;	  
	m_pulseShape[(shape_size)/2-i]=m_pulseShapeX[nminn]+(m_pulseShapeX[nminp]-m_pulseShapeX[nminn])/(m_pulseShapeT[nminp]-m_pulseShapeT[nminn])*(-i-m_pulseShapeT[nminn]);
      }
      
    }
    
    // positive times: (shape_size-1)/2->shape_size
    //    if (i>tmax) m_pulseShape[(shape_size)/2+i]=0.;
    if (i>tmax) m_pulseShape[(shape_size)/2+i]=m_pulseShapeX[ntmax];
    else{
      exact=false;
      minn=-10000.;
      minp=10000.;	  
      nminn=0;
      nminp=size;
      for (int j=nt0;j<size&&!exact;j++){
	if (m_pulseShapeT[j]==double(i)){
	  m_pulseShape[(shape_size)/2+i]=m_pulseShapeX[j];
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
      if (exact)	    
	log<<MSG::VERBOSE<<"exact value found for time="<<i<<" m_pulseShape="<<m_pulseShape[(shape_size)/2+i]<<endreq;
      else{
	//	  if (exact)	    
	log<<MSG::VERBOSE<<"exact value NOT found for time="<<i
	   <<" nminn="<<nminn<<" m_pulseShapeT="<<m_pulseShapeT[nminn]<<" m_pulseShapeX="<<m_pulseShapeX[nminn]<<std::endl 
	   <<" nminp="<<nminp<<" m_pulseShapeT="<<m_pulseShapeT[nminp]<<" m_pulseShapeX="<<m_pulseShapeX[nminp]<<endreq;	  
	
	m_pulseShape[(shape_size)/2+i]=m_pulseShapeX[nminn]+(m_pulseShapeX[nminp]-m_pulseShapeX[nminn])/(m_pulseShapeT[nminp]-m_pulseShapeT[nminn])*(i-m_pulseShapeT[nminn]);
      }
    }
  }

}

 ////////////////////////////////////////
void TileOFCorrelation::WriteWeightsToFile(bool m_deltaCorrelation,
					   int dignum,
					   string m_OptFilterFile_ai_lo,
					   string m_OptFilterFile_bi_lo,
					   string m_OptFilterFile_ai_hi,
					   string m_OptFilterFile_bi_hi, 
					   int ros,
					   int drawer,
					   int channel,
					   int gain,
					   const TileHWID *m_tileHWID,
					   MsgStream & log)
{
  log<<MSG::DEBUG<<" TileOFCorrelation::WriteWeightsToFile"<<endreq;
  fstream *f_ai_lo = new fstream(m_OptFilterFile_ai_lo.c_str(), fstream::app| fstream::out);
  fstream *f_bi_lo = new fstream(m_OptFilterFile_bi_lo.c_str(), fstream::app|fstream::out);
  fstream *f_ai_hi = new fstream(m_OptFilterFile_ai_hi.c_str(), fstream::app|fstream::out);
  fstream *f_bi_hi = new fstream(m_OptFilterFile_bi_hi.c_str(), fstream::app|fstream::out);

  //Open Weights files
  if (f_ai_lo->is_open()&&f_ai_lo->is_open()&&f_ai_lo->is_open()&&f_ai_lo->is_open()) log<<MSG::INFO<<" Weights files open"<<endreq;
  else log<<MSG::INFO<<" Weights files didn't open succesfully"<<endreq;
  
  if(!m_deltaCorrelation && N[ros][drawer][channel][gain]>0){
    int frag= m_tileHWID->frag(ros+1,drawer);
    if (gain==0){
      std::cout<<"ros "<<ros
	       <<"  drawer "<<drawer<<std::hex
	       <<"  frag0x "<<frag<<std::dec
	       <<"  channel "<<channel
	       <<"  N "<<N[ros][drawer][channel][0]
	       <<std::endl;
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
    
  }
  
   if(m_deltaCorrelation || N[ros][drawer][channel][gain]>0){
     for (int pha=-100;pha<101;pha++){
       if (gain==0){
	 *f_ai_lo<<std::setw(6)<<pha;
	 for (int i=0;i<dignum;i++) *f_ai_lo<<std::setw(18)<<std::setprecision(10)<<w_a[pha+100][i];
	 *f_ai_lo<<endl;
	 
	 *f_bi_lo<<std::setw(6)<<pha;
	 for (int i=0;i<dignum;i++) *f_bi_lo<<std::setw(18)<<std::setprecision(10)<<w_b[pha+100][i];
	 *f_bi_lo<<endl;
       }else{
	 *f_ai_hi<<std::setw(6)<<pha;
	 for (int i=0;i<dignum;i++) *f_ai_hi<<std::setw(18)<<std::setprecision(10)<<w_a[pha+100][i];
	*f_ai_hi<<endl;
	
	*f_bi_hi<<std::setw(6)<<pha;
	for (int i=0;i<dignum;i++) *f_bi_hi<<std::setw(18)<<std::setprecision(10)<<w_b[pha+100][i];
	*f_bi_hi<<endl;
       }
     }
     
   }

  f_ai_lo->close();
  f_bi_lo->close();
  f_ai_hi->close();
  f_bi_hi->close();

}

  
 ////////////////////////////////////////
void TileOFCorrelation::OpenWeightsFile(string m_OptFilterFile_ai_lo,
					 string m_OptFilterFile_bi_lo,
					 string m_OptFilterFile_ai_hi,
					 string m_OptFilterFile_bi_hi, 
					 MsgStream & log)
{
  log<<MSG::DEBUG<<" TileOFCorrelation::OpenWeightsFile"<<endreq;
  fstream *f_ai_lo = new fstream(m_OptFilterFile_ai_lo.c_str(), fstream::trunc|fstream::out);
  fstream *f_bi_lo = new fstream(m_OptFilterFile_bi_lo.c_str(), fstream::trunc|fstream::out);
  fstream *f_ai_hi = new fstream(m_OptFilterFile_ai_hi.c_str(), fstream::trunc|fstream::out);
  fstream *f_bi_hi = new fstream(m_OptFilterFile_bi_hi.c_str(), fstream::trunc|fstream::out);
   
  f_ai_lo->open(m_OptFilterFile_ai_hi.c_str(), fstream::trunc|fstream::out);
  f_bi_lo->open(m_OptFilterFile_ai_hi.c_str(), fstream::trunc|fstream::out);
  f_ai_hi->open(m_OptFilterFile_ai_hi.c_str(), fstream::trunc|fstream::out);
  f_bi_hi->open(m_OptFilterFile_ai_hi.c_str(), fstream::trunc|fstream::out);
}


