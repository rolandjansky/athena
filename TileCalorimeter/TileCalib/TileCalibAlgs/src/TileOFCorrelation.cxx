/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

#include <TRobustEstimator.h>
#include <TMatrix.h>

using namespace std;
using CLHEP::HepMatrix;
using CLHEP::HepVector;



////////////////////////////////////////
TileOFCorrelation::TileOFCorrelation()
  : m_ncorr(0.0)
  , m_jentry(0)
  , m_lag(0)
  , m_N_d(0.0)
{
  memset(m_corr2,0,sizeof(m_corr2));
  memset(m_corr,0,sizeof(m_corr));
}


////////////////////////////////////////
TileOFCorrelation::~TileOFCorrelation()
{
}


////////////////////////////////////////
void TileOFCorrelation::SetCorrelationZero(MsgStream & log, int dignum)
{

  log<<MSG::DEBUG<<"TileOFCorrelation::SetCorrelationZero(log)"<<endmsg;
  for (int ros=0;ros<4;ros++)
    for (int drawer=0;drawer<64;drawer++)
      for (int channel=0;channel<48;channel++)
	for (int gain=0;gain<2;gain++){
	  m_N[ros][drawer][channel][gain]=0;
	  //m_N[ros][drawer][channel][gain]=0;
	  for (int i=0;i<dignum;i++){
	    m_S[ros][drawer][channel][gain][i]=0.;
	    for (int j=0;j<dignum;j++){
	      m_SS[ros][drawer][channel][gain][i][j]=0.;
	      m_R[ros][drawer][channel][gain][i][j]=0.;	  
	    }
	  }
	  
	  for (m_lag=0;m_lag<9;m_lag++){
		m_S1[ros][drawer][channel][gain][m_lag]=0.;
		m_S2[ros][drawer][channel][gain][m_lag]=0.;
		m_S11[ros][drawer][channel][gain][m_lag]=0.;
		m_S12[ros][drawer][channel][gain][m_lag]=0.;
		m_S22[ros][drawer][channel][gain][m_lag]=0.;
		m_N_pairs[ros][drawer][channel][gain][m_lag]=0;
		m_corr_sum[ros][drawer][channel][gain][m_lag]=0.;
		m_corr_sum_sq[ros][drawer][channel][gain][m_lag]=0.;
	  }
	}

  for (m_lag=0;m_lag<9;m_lag++){
    m_corr2[m_lag]=0.;
    if(m_lag<8)
      m_corr[m_lag]=0;
  }
  
}


////////////////////////////////////////
void TileOFCorrelation::SetCorrelationDelta(MsgStream & log, int dignum)
{
  log<<MSG::DEBUG<<"TileOFCorrelation::SetCorrelationDelta(log)"<<endmsg;

  for (int ros=0;ros<4;ros++)
    for (int drawer=0;drawer<64;drawer++)
      for (int channel=0;channel<48;channel++)
	for (int gain=0;gain<2;gain++){
	  m_N[ros][drawer][channel][gain]=1;
	  for (int i=0;i<dignum;i++)    
	    for (int j=0;j<dignum;j++)
	      if (i==j) m_R[ros][drawer][channel][gain][i][j]=1.;	  
		else m_R[ros][drawer][channel][gain][i][j]=0.;	   
	}
}



////////////////////////////////////////
void TileOFCorrelation::Sum(vector<float> &digits, int ros, int drawer, int channel, int gain, MsgStream & /*log*/, bool debug, int &dignum, bool doRobustCov)
{
  
  double N_d=0.;
  dignum=digits.size();

  if (doRobustCov){
    m_DataVector[ros][drawer][channel][gain].push_back(digits);
  }
  
  m_N[ros][drawer][channel][gain]++;
  N_d=double(m_N[ros][drawer][channel][gain]);
  for (int i=0;i<dignum;i++){
    m_S[ros][drawer][channel][gain][i]+=digits[i];	  
    for (int j=0;j<dignum;j++) m_SS[ros][drawer][channel][gain][i][j]+=digits[i]*digits[j];
  }
  
  if (debug){
    cout 
      <<" TileOFCorrelation::Sum, ros="<<ros
      <<" drawer="<<drawer
      <<" channel="<<channel
      <<" gain="<<gain
      <<" N="<<m_N[ros][drawer][channel][gain]
      <<" Sum[1]="<<m_S[ros][drawer][channel][gain][1] 
      <<" Sum[2]="<<m_S[ros][drawer][channel][gain][2] 
      <<" Sum[1][2]="<<m_SS[ros][drawer][channel][gain][1][2] 
      <<" Sum[1][1]="<<m_SS[ros][drawer][channel][gain][1][1] 
      <<" Sum[2][2]="<<m_SS[ros][drawer][channel][gain][2][2] 
      <<" B[1][2]="<<m_SS[ros][drawer][channel][gain][1][2]/N_d-m_S[ros][drawer][channel][gain][1]/N_d*m_S[ros][drawer][channel][gain][2]/N_d
      <<" Correlation[1][2]="<<(N_d*m_SS[ros][drawer][channel][gain][1][2]-m_S[ros][drawer][channel][gain][1]*m_S[ros][drawer][channel][gain][2])/sqrt((N_d*m_SS[ros][drawer][channel][gain][1][1]-m_S[ros][drawer][channel][gain][1]*m_S[ros][drawer][channel][gain][1])*(N_d*m_SS[ros][drawer][channel][gain][2][2]-m_S[ros][drawer][channel][gain][2]*m_S[ros][drawer][channel][gain][2]))
      <<endl;
  }
  
}



////////////////////////////////////////
void TileOFCorrelation::CalcCorrelation(MsgStream & log, int dignum, bool flag_7to9, bool doRobustCov)
{
  log<<MSG::DEBUG<<"TileOFCorrelation::CalcCorrelation"<<endmsg;

  double denominator=0.;
  
  TMatrixDSym TempMatrix(dignum);
  
  double *Row = new double[dignum];
  for (int ros=0;ros<4;ros++)
    for (int drawer=0;drawer<64;drawer++)
      for (int channel=0;channel<48;channel++)
	for (int gain=0;gain<2;gain++){
	  double N_d=double(m_N[ros][drawer][channel][gain]);
	  //if (N_d>0.) cout<<" TileOFCorrelation::CalcCorrelation, ros="<<ros<<" drawer="<<drawer<<" channel="<<channel<<" gain="<<gain<<" N_d="<<N_d<<endl;
	   if (doRobustCov){
             if (N_d>10){
               TRobustEstimator REstimator(m_N[ros][drawer][channel][gain],dignum);
               for (int i=0;i<m_N[ros][drawer][channel][gain];i++){
                  for (int j=0; j<dignum;j++){
                     Row[j]=m_DataVector[ros][drawer][channel][gain].at(i).at(j);
                  }
                  REstimator.AddRow(Row);
               }
               REstimator.Evaluate();
               REstimator.GetCovariance(TempMatrix);
               for (int i=0;i<dignum;i++)
                  for (int j=0;j<dignum;j++)
                     m_R[ros][drawer][channel][gain][i][j]= TempMatrix(i,j);

             } else {
               if(dignum==7 && flag_7to9)
                 for (int j=0;j<9;j++)
                   m_R[ros][drawer][channel][gain][j][j]=1.;

               for (int i=0;i<dignum;i++){
                 for (int j=0;j<dignum;j++){
                   if (N_d>0.){
                      denominator= (N_d*m_SS[ros][drawer][channel][gain][i][i]-m_S[ros][drawer][channel][gain][i]*m_S[ros][drawer][channel][gain][i])*
                        (N_d*m_SS[ros][drawer][channel][gain][j][j]-m_S[ros][drawer][channel][gain][j]*m_S[ros][drawer][channel][gain][j]);
                      if(denominator!=0){
                        m_R[ros][drawer][channel][gain][i][j]=
                          (N_d*m_SS[ros][drawer][channel][gain][i][j]-m_S[ros][drawer][channel][gain][i]*m_S[ros][drawer][channel][gain][j])/sqrt(denominator);
                      }else{
                        m_R[ros][drawer][channel][gain][i][j]=0.;
                      }
                   }
                   else m_R[ros][drawer][channel][gain][i][j]=-1234.;
                 }
               }
             }
	   } else {
             if(dignum==7 && flag_7to9)
               for (int j=0;j<9;j++)
                 m_R[ros][drawer][channel][gain][j][j]=1.;

             for (int i=0;i<dignum;i++){
               for (int j=0;j<dignum;j++){
                 if (N_d>0.){
                   denominator= (N_d*m_SS[ros][drawer][channel][gain][i][i]-m_S[ros][drawer][channel][gain][i]*m_S[ros][drawer][channel][gain][i])*
                     (N_d*m_SS[ros][drawer][channel][gain][j][j]-m_S[ros][drawer][channel][gain][j]*m_S[ros][drawer][channel][gain][j]);
                   if(denominator!=0){
                     m_R[ros][drawer][channel][gain][i][j]=
                       (N_d*m_SS[ros][drawer][channel][gain][i][j]-m_S[ros][drawer][channel][gain][i]*m_S[ros][drawer][channel][gain][j])/sqrt(denominator);
                   }else{
                     m_R[ros][drawer][channel][gain][i][j]=0.;
                   }
                 }
                 else m_R[ros][drawer][channel][gain][i][j]=-1234.;
               }
             }
	   }
       }
  delete [] Row;
  for (int ros=0;ros<4;ros++)
     for (int drawer=0;drawer<64;drawer++)
        for (int channel=0;channel<48;channel++)
           for (int gain=0;gain<2;gain++){
	      vector < vector <float> > x;
              m_DataVector[ros][drawer][channel][gain].swap(x);
	   }
}



////////////////////////////////////////
void TileOFCorrelation::RunningCorrelation(vector<float> &digits, int ros, int drawer, int channel, int gain, MsgStream & log, bool /*debug*/, int &dignum, int chthres)
{
  log<<MSG::VERBOSE<<"TileOFCorrelation::RunningCorrelation"<<endmsg;
  dignum=digits.size();

  double denominator=0.;

  //chthres=10;
  //update sums
  m_N[ros][drawer][channel][gain]++;
  m_jentry=m_N[ros][drawer][channel][gain];
  m_N_d=double(m_jentry);

  if (ros==1 && drawer==1 && channel==0 && gain==1)
    log<<MSG::VERBOSE<<"Computing RunningCorrelation for jentry="<<m_jentry<<endmsg;

  for (m_lag=1;m_lag<dignum;m_lag++){
    for (int i=0; i<dignum-m_lag; i++){	            
      m_S1[ros][drawer][channel][gain][m_lag-1]+=digits[i];
      m_S2[ros][drawer][channel][gain][m_lag-1]+=digits[i+m_lag];
      m_S12[ros][drawer][channel][gain][m_lag-1]+=digits[i]*digits[i+m_lag];
      m_S11[ros][drawer][channel][gain][m_lag-1]+=digits[i]*digits[i];
      m_S22[ros][drawer][channel][gain][m_lag-1]+=digits[i+m_lag]*digits[i+m_lag];
      m_N_pairs[ros][drawer][channel][gain][m_lag-1]++;	
    }
    if (m_lag==1 && ros==1 && drawer==1 && channel==0 && gain==1)
      log<<MSG::VERBOSE<<" jentry="<<m_jentry<<" m_N="<<m_N_pairs[ros][drawer][channel][gain][m_lag-1]<<" S1="<<m_S1[ros][drawer][channel][gain][m_lag-1]<<endmsg;
    
    
    if (m_jentry>chthres){
      m_ncorr=double(m_N_pairs[ros][drawer][channel][gain][m_lag-1]);

      denominator=(m_ncorr*m_S11[ros][drawer][channel][gain][m_lag-1]-
		   m_S1[ros][drawer][channel][gain][m_lag-1]*m_S1[ros][drawer][channel][gain][m_lag-1])*
	(m_ncorr*m_S22[ros][drawer][channel][gain][m_lag-1]-
	 m_S2[ros][drawer][channel][gain][m_lag-1]*m_S2[ros][drawer][channel][gain][m_lag-1]);
      
      if(denominator!=0){
	m_corr2[m_lag-1]=
	(m_ncorr*m_S12[ros][drawer][channel][gain][m_lag-1]-
	 m_S1[ros][drawer][channel][gain][m_lag-1]*m_S2[ros][drawer][channel][gain][m_lag-1])/sqrt(denominator);
      }else{
	m_corr2[m_lag-1]=0.;
      }
      
      if (m_lag==1 && ros==1 && drawer==1 && channel==0 && gain==1)
	log<<MSG::DEBUG
	   <<" m_corr2="<<m_corr2[m_lag-1]
	   <<" m_corr_sum="<<m_corr_sum[ros][drawer][channel][gain][m_lag-1]
	   <<" m_corr_sum_sq="<<m_corr_sum_sq[ros][drawer][channel][gain][m_lag-1]
	   <<endmsg;

      m_corr_sum[ros][drawer][channel][gain][m_lag-1]+=m_corr2[m_lag-1];
      m_corr_sum_sq[ros][drawer][channel][gain][m_lag-1]+=m_corr2[m_lag-1]*m_corr2[m_lag-1];
      	
      if (m_lag==1 && ros==1 && drawer==1 && channel==0 && gain==1)
	log<<MSG::DEBUG
	   <<" jentry="<<m_jentry<<" jentry-chthres="<<m_jentry-chthres<<" m_lag=1, ros=1, drawer=1, channel=0, gain=1"
	   <<" m_corr2="<<m_corr2[m_lag-1]
	   <<" sum corr_mean="<<m_corr_sum[ros][drawer][channel][gain][m_lag-1]
	   <<" corr_mean="<<m_corr_sum[ros][drawer][channel][gain][m_lag-1]/(m_jentry-chthres)
	   <<" sum RMS="<<m_corr_sum_sq[ros][drawer][channel][gain][m_lag-1]
	   <<" RMS="<<sqrt(m_corr_sum_sq[ros][drawer][channel][gain][m_lag-1]*(m_jentry-chthres)
			   -m_corr_sum[ros][drawer][channel][gain][m_lag-1]*m_corr_sum[ros][drawer][channel][gain][m_lag-1])/(m_jentry-chthres)
	   <<endmsg;
    }
  }
}



////////////////////////////////////////
void TileOFCorrelation::CalcRunningCorrelation(MsgStream & /*log*/, int dignum, int chthres, bool flag_7to9)
{
  for (int ros=0;ros<4;ros++)
    for (int drawer=0;drawer<64;drawer++)
      for (int channel=0;channel<48;channel++)
	for (int gain=0;gain<2;gain++){
	  m_jentry=m_N[ros][drawer][channel][gain];
	  m_ncorr=double(m_jentry-chthres);
	  
	  if (m_jentry>0){
	    if (flag_7to9 && dignum==7){
	      for (int i=0;i<9;i++)
		m_R[ros][drawer][channel][gain][i][i]=1.;
	      
	      for (m_lag=1;m_lag<9;m_lag++)
		for (int i=0;i<9-m_lag;i++){
		  if (m_lag<7){
		    m_R[ros][drawer][channel][gain][i][i+m_lag]=m_corr_sum[ros][drawer][channel][gain][m_lag-1]/m_ncorr;
		    m_R[ros][drawer][channel][gain][i+m_lag][i]=m_corr_sum[ros][drawer][channel][gain][m_lag-1]/m_ncorr;
		  }else{
		    m_R[ros][drawer][channel][gain][i][i+m_lag]=0.;
		    m_R[ros][drawer][channel][gain][i+m_lag][i]=0.;
		  }
		  if (-1.>m_R[ros][drawer][channel][gain][i][i+m_lag] || m_R[ros][drawer][channel][gain][i][i+m_lag]>1.)
		    m_R[ros][drawer][channel][gain][i][i+m_lag]=0.;
		  
		  if (-1.>m_R[ros][drawer][channel][gain][i+m_lag][i] || m_R[ros][drawer][channel][gain][i+m_lag][i]>1.)
		    m_R[ros][drawer][channel][gain][i+m_lag][i]=0.;
		  
		}
	    }else{
	      for (int i=0;i<dignum;i++)
		m_R[ros][drawer][channel][gain][i][i]=1.;
	      
	      for (m_lag=1;m_lag<dignum;m_lag++)
		for (int i=0;i<dignum-m_lag;i++){
		  m_R[ros][drawer][channel][gain][i][i+m_lag]=m_corr_sum[ros][drawer][channel][gain][m_lag-1]/m_ncorr;
		  m_R[ros][drawer][channel][gain][i+m_lag][i]=m_corr_sum[ros][drawer][channel][gain][m_lag-1]/m_ncorr;
	
		  if (-1.>m_R[ros][drawer][channel][gain][i][i+m_lag] || m_R[ros][drawer][channel][gain][i][i+m_lag]>1.)
		    m_R[ros][drawer][channel][gain][i][i+m_lag]=0.;
		  if (-1.>m_R[ros][drawer][channel][gain][i+m_lag][i] || m_R[ros][drawer][channel][gain][i+m_lag][i]>1.)
		    m_R[ros][drawer][channel][gain][i+m_lag][i]=0.;
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
			tmpCorr[i][j] = m_R[ros][drawer][chan][gain][i][j];
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
			  cout<<" "<<m_R[ros][drawer][channel][gain][i][j];
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
void TileOFCorrelation::SaveCorrelationSumm(bool deltaCorrelation, 
					  string OptFilterFile_CorrelationSumm,
					  const TileHWID *tileHWID,
					  MsgStream & log,
					  int dignum)
{
  log<<MSG::DEBUG<<" TileOFCorrelation::SaveCorrelationSumm"<<endmsg;
  
  HepMatrix M_correlation(dignum,1,0);

  fstream *f_correlation = new fstream(OptFilterFile_CorrelationSumm.c_str(),fstream::out);
  if (f_correlation->is_open()) log<<MSG::INFO<<OptFilterFile_CorrelationSumm<<" file open"<<endmsg;

  if (deltaCorrelation){
      //      for (int i=0;i<dignum;i++)
    for (int j=0;j<dignum;j++){
      int i=0;
      if (m_R[0][0][0][0][i][j]>-100000. &&  m_R[0][0][0][0][i][j]<100000.)
	M_correlation[i][j]=m_R[0][0][0][0][i][j];
      else
	M_correlation[i][j]=0.0;
    }
    
    
    *f_correlation<<M_correlation.T()<<endl;
  }else{
      for (int ros=0;ros<4;ros++)    
	for (int drawer=0;drawer<64;drawer++){
	  int frag= tileHWID->frag(ros+1,drawer);	    
	  for (int channel=0;channel<48;channel++)
	    for (int gain=0;gain<2;gain++){
	      log<<MSG::VERBOSE
		 <<"ros "<<ros
		 <<"  drawer "<<drawer<<MSG::hex
		 <<"  frag0x "<<frag<<MSG::dec
		 <<"  channel "<<channel
		 <<"  gain "<<gain
		 <<"  m_N "<<m_N[ros][drawer][channel][gain]
		 <<endmsg;
	      
	      if (m_N[ros][drawer][channel][gain]>0){
		//for (int i=0;i<dignum;i++)
		for (int j=0;j<dignum;j++){
		  int i=0;
		  if (m_R[ros][drawer][channel][gain][i][j]>-100000. 
		      &&  m_R[ros][drawer][channel][gain][i][j]<100000.)
		    M_correlation[i][j]=m_R[ros][drawer][channel][gain][i][j];
		  else
		    M_correlation[i][j]=0.0;
		}
		
		
		
		*f_correlation<<"ros "<<ros
			      <<"  drawer "<<drawer<<std::hex
			      <<"  frag0x "<<frag<<std::dec
			      <<"  channel "<<channel
			      <<"  gain "<<gain
			      <<"  m_N "<<m_N[ros][drawer][channel][gain]
			      <<M_correlation.T();
		
	      }
	    }	  
	}    
  }
  f_correlation->close();  
}

////////////////////////////////////////
float * TileOFCorrelation::getCorrelationSumm(bool deltaCorrelation, 
					      int ros,
					      int drawer,
					      int channel,
					      int gain,
					      int dignum)
{

  if(!deltaCorrelation){
  
    if (m_N[ros][drawer][channel][gain]>0){
      for (int i=1;i<dignum;i++){
	if (m_R[ros][drawer][channel][gain][0][i]>-100000. 
	    &&  m_R[ros][drawer][channel][gain][0][i]<100000.)
	  m_corr[i-1]=m_R[ros][drawer][channel][gain][0][i];
	else
	  m_corr[i-1]=0.0;
      }
    }
    
  }
  
  return m_corr;
  
}

////////////////////////////////////////
void TileOFCorrelation::SaveCorrelationMatrix(bool deltaCorrelation, 
					      string OptFilterFile_CorrelationMatrix,
					      const TileHWID *tileHWID,
					      MsgStream & log,
					      int dignum)
{
  log<<MSG::DEBUG<<" TileOFCorrelation::SaveCorrelationMatrix"<<endmsg;
  
  HepMatrix M_correlation(dignum,dignum,0);
  
  fstream *f_correlation = new fstream(OptFilterFile_CorrelationMatrix.c_str(),fstream::out);
  if (f_correlation->is_open()) log<<MSG::INFO<<OptFilterFile_CorrelationMatrix<<" file open"<<endmsg;

  if (deltaCorrelation){
    for (int i=0;i<dignum;i++)
      for (int j=0;j<dignum;j++){
	if (m_R[0][0][0][0][i][j]>-100000. &&  m_R[0][0][0][0][i][j]<100000.)
	  M_correlation[i][j]=m_R[0][0][0][0][i][j];
	else
	  M_correlation[i][j]=0.0;
      }
    
    
    *f_correlation<<M_correlation<<endl;
  }else{
    for (int ros=0;ros<4;ros++)    
      for (int drawer=0;drawer<64;drawer++){
	int frag= tileHWID->frag(ros+1,drawer);	    
	for (int channel=0;channel<48;channel++)
	  for (int gain=0;gain<2;gain++){
	    log<<MSG::VERBOSE
	       <<"ros "<<ros
	       <<"  drawer "<<drawer<<MSG::hex
	       <<"  frag0x "<<frag<<MSG::dec
	       <<"  channel "<<channel
	       <<"  gain "<<gain
	       <<"  m_N "<<m_N[ros][drawer][channel][gain]
	       <<endmsg;
	    
	    if (m_N[ros][drawer][channel][gain]>0){
	      for (int i=0;i<dignum;i++)
		for (int j=0;j<dignum;j++){
		  if (m_R[ros][drawer][channel][gain][i][j]>-100000.
		      &&  m_R[ros][drawer][channel][gain][i][j]<100000.)
		    M_correlation[i][j]=m_R[ros][drawer][channel][gain][i][j];
		  else
		    M_correlation[i][j]=0.0;
		}
	      
	      
	      
	      *f_correlation<<"ros "<<ros
			    <<"  drawer "<<drawer<<std::hex
			    <<"  frag0x "<<frag<<std::dec
			    <<"  channel "<<channel
			    <<"  gain "<<gain
			    <<"  m_N "<<m_N[ros][drawer][channel][gain]
			    <<M_correlation<<endl;
	    }
	  }	  
      }    
  }
  f_correlation->close();  
}



////////////////////////////////////////
void TileOFCorrelation::CalcWeights(bool of2,
				    bool deltaCorrelation,
				    const vector<double>& LshapeForm,
				    const vector<double>& HshapeForm,
				    const vector<double>& LdshapeForm,
				    const vector<double>& HdshapeForm,
				    MsgStream & log,
				    int ros,
				    int drawer,
				    int channel,
				    int gain,
				    int dignum)
{
  log<<MSG::DEBUG<<" TileOFCorrelation::CalcWeights"<<endmsg;
  
  HepMatrix Correlation(dignum,dignum,0), Inverse(dignum,dignum,0), Zero(dignum,dignum,0);
  HepMatrix PulseShape(dignum,1,0), DPulseShape(dignum,1,0);
  HepMatrix a(dignum,1,0), b(dignum,1,0), WeightZero(dignum,1,0);
  
  int ierr=0;

  if(of2)
    log<<MSG::INFO<<" Calculating OF2 weights ";
  else
    log<<MSG::INFO<<" Calculating OF1 weights ";
  if (deltaCorrelation)
    log<<MSG::INFO<<"with Delta correlation matrix "<<endmsg;
  else
    log<<MSG::INFO<<"with correlation matrix obtained from data "<<endmsg;
  log<<MSG::INFO<<"for ros="<<ros<<" drawer="<<drawer<<" channel="<<channel<<
    " gain="<<gain<<endmsg;
  
  

  if (deltaCorrelation){
    
    for (int i=0;i<dignum;i++)    
      for (int j=0;j<dignum;j++)
	Correlation[i][j]=m_R[0][0][0][0][i][j];
    
    Inverse=Correlation.inverse(ierr);
    
    for (int pha=-100;pha<101;pha++){
      
      if (ierr==0){		
	for (int i=0;i<dignum;i++){
	  if (gain==0){
	    PulseShape[i][0]=LshapeForm[i*25+100+pha];
	    DPulseShape[i][0]=LdshapeForm[i*25+100+pha];
	  }else{
	    PulseShape[i][0]=HshapeForm[i*25+100+pha];
	    DPulseShape[i][0]=HdshapeForm[i*25+100+pha];
	  }
	  g[pha+100][i]=PulseShape[i][0];
	}			

	// Build System Matrix with Correlations and pulse function points
	
	if(!of2){  // OF1
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
	   ros,drawer,channel,gain,m_N[ros][drawer][channel][gain]);	  
    if (m_N[ros][drawer][channel][gain]>0){
      
      for (int i=0;i<dignum;i++)    
	for (int j=0;j<dignum;j++)
	  Correlation[i][j]=m_R[ros][drawer][channel][gain][i][j];
      
      Inverse=Correlation.inverse(ierr);
      
      if(ierr!=0){
	log<<MSG::WARNING<<" Correlation matrix cannot be inverted for ros="<<ros<<" drawer="<<drawer
	   <<" channel="<<channel<<" gain="<<gain<<endmsg;
	log<<MSG::WARNING<<" Weights set to zero for this channel..."<<endmsg;
	for (int i=0; i<dignum; i++){
	  for (int j=0; j<dignum; j++){
	    printf("%4.4f  ",m_R[ros][drawer][channel][gain][i][j]);
	  }
	  printf("\n");
	}
      }
      
      for (int pha=-100;pha<101;pha++){
	
	if (ierr==0){		   
	  for (int i=0;i<dignum;i++){
	    if (gain==0){
	      PulseShape[i][0]=LshapeForm[i*25+100+pha];
	      DPulseShape[i][0]=LdshapeForm[i*25+100+pha];
	    }else{
	      PulseShape[i][0]=HshapeForm[i*25+100+pha];
	      DPulseShape[i][0]=HdshapeForm[i*25+100+pha];
	    }
	    g[pha+100][i]=PulseShape[i][0];
	  }   
	  
	  // Build System Matrix with Correlations and pulse function points
	  if(!of2){  // OF1
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
		  printf("%4.4f  ",m_R[ros][drawer][channel][gain][i][j]);
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
    }// END ch + gain + if m_N>0
    
  }// IF delta
  
  log<<MSG::VERBOSE<<"...weights calculated"<<endmsg;
 
}




////////////////////////////////////////
void TileOFCorrelation::BuildPulseShape(vector<double> &pulseShape,
				      vector<double> &pulseShapeX,
				      vector<double> &pulseShapeT,
				      int dignum,
				      MsgStream &log)
{ 
  log<<MSG::DEBUG<<"TileCalorimeter::BuildPulseShape"<<endmsg;
  
  //1: set pulseShape
  int shape_size=(dignum-1)*25+200;
  pulseShape.resize(shape_size);
  log<<MSG::DEBUG<<"Set dimension of pulseShape to shape_sie="<<shape_size<<endmsg;

  //2: scan pulseShapeT for: tmin, tmax, nt0 and size: pulseShapeX[nt0]=1.0;
  int nt0=0, size;
  double tmin=10000., tmax=-10000.;
  int ntmin=0, ntmax=0;
  size=pulseShapeT.size();
  for (int i=0; i<size;i++){
    if (pulseShapeT[i]<tmin) {
      tmin=pulseShapeT[i];
      ntmin=i;
    }
    if (pulseShapeT[i]>tmax){
      tmax=pulseShapeT[i];
      ntmax=i;
    }
    if (pulseShapeT[i]==0) nt0=i;
  }
  log<<MSG::DEBUG<<"pulseShapeX & pulseShapeT size ="<<size<<", tmin="<<tmin<<", tmax="<<tmax<<"  nt0="<<nt0<<" pulseShapeT[nt0]="<<pulseShapeT[nt0]<<" pulseShapeX[nt0]="<<pulseShapeX[nt0]<<endmsg;
  
  //3: fill pulseShape
  bool exact;
  int nminn, nminp;
  double minn, minp, tdist;
  pulseShape[(shape_size)/2]=pulseShapeX[nt0];
  //  for (int i=1;i<(shape_size)/2+1;i++){
  for (int i=1;i<(shape_size)/2;i++){
    // negative times: 0->(shape_size-1)/2    
    //   if (-i<tmin) pulseShape[(shape_size)/2-i]=0.;
    if (-i<tmin) pulseShape[(shape_size)/2-i]=pulseShapeX[ntmin];
    else{
      exact=false;
      minn=-10000.;
      minp=10000.;
      nminn=0;
      nminp=size-1;
      for (int j=0;j<nt0+1&&!exact;j++){
	if (pulseShapeT[j]==double(-i)){
	  pulseShape[(shape_size)/2-i]=pulseShapeX[j];
	  exact=true;
	}else{
	  tdist=pulseShapeT[j]-double(-i);
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
	log<<MSG::VERBOSE<<"exact value found for time="<<-i<<" pulseShape="<<pulseShape[(shape_size)/2-i]<<endmsg;
      else{
	//	  if (exact)	    
	log<<MSG::VERBOSE<<"exact value NOT found for time="<<-i
	   <<" nminn="<<nminn<<" pulseShapeT="<<pulseShapeT[nminn]<<" pulseShapeX="<<pulseShapeX[nminn]<<std::endl 
	   <<" nminp="<<nminp<<" pulseShapeT="<<pulseShapeT[nminp]<<" pulseShapeX="<<pulseShapeX[nminp]<<endmsg;	  
	pulseShape[(shape_size)/2-i]=pulseShapeX[nminn]+(pulseShapeX[nminp]-pulseShapeX[nminn])/(pulseShapeT[nminp]-pulseShapeT[nminn])*(-i-pulseShapeT[nminn]);
      }
      
    }
    
    // positive times: (shape_size-1)/2->shape_size
    //    if (i>tmax) pulseShape[(shape_size)/2+i]=0.;
    if (i>tmax) pulseShape[(shape_size)/2+i]=pulseShapeX[ntmax];
    else{
      exact=false;
      minn=-10000.;
      minp=10000.;	  
      nminn=0;
      nminp=size;
      for (int j=nt0;j<size&&!exact;j++){
	if (pulseShapeT[j]==double(i)){
	  pulseShape[(shape_size)/2+i]=pulseShapeX[j];
	  exact=true;
	}else{
	  tdist=pulseShapeT[j]-double(i);
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
	log<<MSG::VERBOSE<<"exact value found for time="<<i<<" pulseShape="<<pulseShape[(shape_size)/2+i]<<endmsg;
      else{
	//	  if (exact)	    
	log<<MSG::VERBOSE<<"exact value NOT found for time="<<i
	   <<" nminn="<<nminn<<" pulseShapeT="<<pulseShapeT[nminn]<<" pulseShapeX="<<pulseShapeX[nminn]<<std::endl 
	   <<" nminp="<<nminp<<" pulseShapeT="<<pulseShapeT[nminp]<<" pulseShapeX="<<pulseShapeX[nminp]<<endmsg;	  
	
	pulseShape[(shape_size)/2+i]=pulseShapeX[nminn]+(pulseShapeX[nminp]-pulseShapeX[nminn])/(pulseShapeT[nminp]-pulseShapeT[nminn])*(i-pulseShapeT[nminn]);
      }
    }
  }

}

 ////////////////////////////////////////
void TileOFCorrelation::WriteWeightsToFile(bool deltaCorrelation,
					   int dignum,
					   string OptFilterFile_ai_lo,
					   string OptFilterFile_bi_lo,
					   string OptFilterFile_ai_hi,
					   string OptFilterFile_bi_hi, 
					   int ros,
					   int drawer,
					   int channel,
					   int gain,
					   const TileHWID *tileHWID,
					   MsgStream & log)
{
  log<<MSG::DEBUG<<" TileOFCorrelation::WriteWeightsToFile"<<endmsg;
  fstream *f_ai_lo = new fstream(OptFilterFile_ai_lo.c_str(), fstream::app| fstream::out);
  fstream *f_bi_lo = new fstream(OptFilterFile_bi_lo.c_str(), fstream::app|fstream::out);
  fstream *f_ai_hi = new fstream(OptFilterFile_ai_hi.c_str(), fstream::app|fstream::out);
  fstream *f_bi_hi = new fstream(OptFilterFile_bi_hi.c_str(), fstream::app|fstream::out);

  //Open Weights files
  if (f_ai_lo->is_open()&&f_ai_lo->is_open()&&f_ai_lo->is_open()&&f_ai_lo->is_open()) log<<MSG::INFO<<" Weights files open"<<endmsg;
  else log<<MSG::INFO<<" Weights files didn't open succesfully"<<endmsg;
  
  if(!deltaCorrelation && m_N[ros][drawer][channel][gain]>0){
    int frag= tileHWID->frag(ros+1,drawer);
    if (gain==0){
      std::cout<<"ros "<<ros
	       <<"  drawer "<<drawer<<std::hex
	       <<"  frag0x "<<frag<<std::dec
	       <<"  channel "<<channel
	       <<"  N "<<m_N[ros][drawer][channel][0]
	       <<std::endl;
      *f_ai_lo<<"ros "<<ros
	      <<"  drawer "<<drawer<<std::hex
	      <<"  frag0x "<<frag<<std::dec
	      <<"  channel "<<channel
	      <<"  N "<<m_N[ros][drawer][channel][0]
	      <<endl;
      
      *f_bi_lo<<"ros "<<ros
	      <<"  drawer "<<drawer<<std::hex
	      <<"  frag0x "<<frag<<std::dec
	      <<"  channel "<<channel
	      <<"  N "<<m_N[ros][drawer][channel][0]
	      <<endl;
    }
    if (gain==1){
      *f_ai_hi<<"ros "<<ros
	      <<"  drawer "<<drawer<<std::hex
	      <<"  frag0x "<<frag<<std::dec
	      <<"  channel "<<channel
	      <<"  N "<<m_N[ros][drawer][channel][1]
	      <<endl;
      
      *f_bi_hi<<"ros "<<ros
	      <<"  drawer "<<drawer<<std::hex
	      <<"  frag0x "<<frag<<std::dec
	      <<"  channel "<<channel
	      <<"  N "<<m_N[ros][drawer][channel][1]
	      <<endl;
    }
    
  }
  
   if(deltaCorrelation || m_N[ros][drawer][channel][gain]>0){
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
void TileOFCorrelation::OpenWeightsFile(string OptFilterFile_ai_lo,
					 string OptFilterFile_bi_lo,
					 string OptFilterFile_ai_hi,
					 string OptFilterFile_bi_hi, 
					 MsgStream & log)
{
  log<<MSG::DEBUG<<" TileOFCorrelation::OpenWeightsFile"<<endmsg;
  fstream *f_ai_lo = new fstream(OptFilterFile_ai_lo.c_str(), fstream::trunc|fstream::out);
  fstream *f_bi_lo = new fstream(OptFilterFile_bi_lo.c_str(), fstream::trunc|fstream::out);
  fstream *f_ai_hi = new fstream(OptFilterFile_ai_hi.c_str(), fstream::trunc|fstream::out);
  fstream *f_bi_hi = new fstream(OptFilterFile_bi_hi.c_str(), fstream::trunc|fstream::out);
   
  f_ai_lo->open(OptFilterFile_ai_hi.c_str(), fstream::trunc|fstream::out);
  f_bi_lo->open(OptFilterFile_ai_hi.c_str(), fstream::trunc|fstream::out);
  f_ai_hi->open(OptFilterFile_ai_hi.c_str(), fstream::trunc|fstream::out);
  f_bi_hi->open(OptFilterFile_ai_hi.c_str(), fstream::trunc|fstream::out);
}


