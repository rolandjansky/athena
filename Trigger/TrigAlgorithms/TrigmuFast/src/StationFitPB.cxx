/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigmuFast/StationFitPB.h"
#include "TrigmuFast/Datatypes.h"
#include "MuonCablingTools/OutputDef.h"
#include "TrigMuonEvent/MuonFeatureDetails.h"
#include <math.h>
#include <iomanip>

#include "TMath.h"

using namespace std;

// #define ibits ibits_
// #define prob prob_

// extern "C" int ibits(int *,int *,int *);
// extern "C" float prob(float *,int *);

#define CSPEED  2.99979e+10    //     light speed (cm/s)
#define MDT_RED 0.7
//#define DRIFTVE 0.00292      //     1.46/500 drif velocity (cm/ns)
#define DRIFTVE 0.00209        //     1.46/700 drif velocity (cm/ns)
#define SIGMA   0.0080         //     sigma (cm)
//#define PI      3.141593

const string StationFitPB::s_name = "Link and MDT fit";

StationFitPB::StationFitPB(MuMessageInterface& msg,
                           const TrackFit& track_fit,
                           LayerDigits (*digits)[MAXLAYER],
                           SpointData super_points[])
    : StationFitStd(msg,track_fit, digits, super_points)
{
   // CID 23229: UNINIT_CTOR
   m_fitres.ALIN  = 0;
   m_fitres.BLIN  = 0;
   m_fitres.NPOI  = 0;
   m_fitres.NDOF  = 0;
   m_fitres.CHI2  = 0;
   m_fitres.PCHI2 = 0;
   for(int i=0; i<NMEAMX; i++) {
      m_fitres.IDMEA[i] = 0;
      m_fitres.XILIN[i] = 0;
      m_fitres.YILIN[i] = 0;
      m_fitres.IGLIN[i] = 0;
      m_fitres.RILIN[i] = 0;
      m_fitres.WILIN[i] = 0;
      m_fitres.DISTJ[i] = 0;
      m_fitres.JLINE[i] = 0;
      m_fitres.RESI[i]  = 0;
   }
   for(int i=0; i<MNLINE; i++) {
      m_fitres.NLINE[i] = 0;
   }
   for(int i=0; i<2; i++) {
      for(int j=0; i<2; i++) {
	 m_fitres.DABLIN[i][j] = 0;
      }
   }
}

StationFitPB::~StationFitPB(void)
{
}

bool
StationFitPB::run(MuonFeatureDetails* det)
{
    int i,st,ly,count,FitFlag;
    float Xor,Yor,sigma,rm=0.,phim=0;
    
    float Ymid,Xmid,Amid;
    
    DigitData *tmp;
    /*
    for(st=0;st<MAXSTATION;st++){
      m_super_points[st].Npoint = 0;
      m_super_points[st].Ndigi  = 0;
      m_super_points[st].R      = 0.;
      m_super_points[st].Z      = 0.;
      m_super_points[st].Alin   = 0.;
      m_super_points[st].Blin   = 0.;
      m_super_points[st].Xor    = 0.;
      m_super_points[st].Yor    = 0.;
      m_super_points[st].Chi2   = 0.;
      m_super_points[st].PChi2  = 0.;
      for(i=0;i<8;i++) m_super_points[st].Residual[i] = 0.;
    }
    */
    for(st=0;st<MAXSTATION;st++) {
      count = 0;
      Xor = 0.;
      Yor = 0.;
      Amid = 0.;
      Xmid = 0.;
      Ymid = 0.;
      for(ly=0;ly<MAXLAYER;ly++) {
        m_super_points[st].Ndigi += m_digits[st][ly].ndigit;
	
        if ((tmp=m_digits[st][ly].first)) {
	  if(!count)
	  { 
	    rm = m_digits[st][ly].Ymid;
	    Amid = m_digits[st][ly].Amid;
	    Xmid = m_digits[st][ly].Xmid;
	    Ymid = m_digits[st][ly].Ymid;
	  }
	  if(!Xor) {Xor = tmp->R; Yor = tmp->Z;}
	  
	  phim = m_digits[st][ly].PhiMed;
	  sigma = (tmp->DriftSigma)?  tmp->DriftSigma : SetDriftSigma (1.0);
          //std::cout << "Xor=" << Xor << ",  Yor=" << Yor << std::endl;
	  //std::cout << "DriftSpace=" << tmp->DriftSpace << ",  DriftSigma=" << tmp->DriftSigma << std::endl;
          if( tmp->DriftSpace!=0. && fabs(tmp->DriftSpace) < 16. && 
	                                  tmp->DriftTime!=0. ) {
	    m_fitres.XILIN[count] = tmp->R - Xor;
	    m_fitres.YILIN[count] = tmp->Z - Yor;
	    m_fitres.IGLIN[count] = 2;
	    m_fitres.RILIN[count] = (tmp->DriftSpace)? tmp->DriftSpace :
	                       SetDriftSpace(tmp->DriftTime,tmp->R,tmp->Z,phim);

	    m_fitres.WILIN[count] = 1./(sigma*sigma);
	    m_fitres.JLINE[count] = count;
	    m_fitres.IDMEA[count] = st*10+ly;
	    m_fitres.NLINE[count]++;
	    m_fitres.NPOI = ++count;

	  } else m_super_points[st].Ndigi--;
	  
	}
      }
      for(i=0;i<count;i++) {m_fitres.DISTJ[i] = 0.; m_fitres.RESI[i] = 0.;}
//      printf("fase1\n");
      if((count-2)>=1) {
        FitFlag = Evlfit(1);
//      printf("fase2\n");

        
	float ac = Amid;
	float bc = (Ymid - Xor) -ac*(Xmid - Yor);
	float X = ( (m_fitres.ALIN*bc)+m_fitres.BLIN )/( 1-ac*m_fitres.ALIN );
	
        m_super_points[st].Npoint = m_fitres.NPOI;
	//if ( m_track_fit.Type==0 && m_track_fit.Xcenter==0 && m_track_fit.Ycenter==0) {
        //if ( m_track_fit.isEndcap) {
	if ( m_digits[st][0].isEndcap ) {
	//m_super_points[st].R      = rm;
        //m_super_points[st].Z      = m_fitres.ALIN*(rm-Xor) + m_fitres.BLIN + Yor;
	    if(m_fitres.ALIN!=0. ) {
	       // std::cout << "m_fitres.ALIN=" << m_fitres.ALIN
	       //           << ",  m_fitres.BLIN=" << m_fitres.BLIN
	       //	    << ",  rm=" << rm << std::endl;
	        m_super_points[st].Z = rm;
	        m_super_points[st].R = (rm-Yor)/m_fitres.ALIN - 
	                            m_fitres.BLIN/m_fitres.ALIN + Xor;
	        m_super_points[st].Alin = 1./m_fitres.ALIN;
	        m_super_points[st].Blin = -m_fitres.BLIN/m_fitres.ALIN;
	    }
	} else {
            m_super_points[st].R      = ac*X + bc + Xor;
            m_super_points[st].Z      = X + Yor;
	    m_super_points[st].Alin   = m_fitres.ALIN;
	    m_super_points[st].Blin   = m_fitres.BLIN; 
        }
	m_super_points[st].Phim   = phim;
	m_super_points[st].Xor    = Xor;
        m_super_points[st].Yor    = Yor;
        m_super_points[st].Chi2   = m_fitres.CHI2;
        m_super_points[st].PChi2  = m_fitres.PCHI2;
	for(i=0;i<m_fitres.NPOI;i++) m_super_points[st].Residual[i] =  m_fitres.RESI[i];
      
        if(det!=0) {
	    float aa  = m_super_points[st].Alin;
	    float bb  = m_super_points[st].Blin;
	    float zz  = m_super_points[st].Z;
	    float rr  = m_super_points[st].R;
	    float chi = m_super_points[st].Chi2;
	    
	    if(st%3==0)       det->setSP1(aa,bb,zz,rr,chi);
	    else if (st%3==1) det->setSP2(aa,bb,zz,rr,chi);
	    else if (st%3==2) det->setSP3(aa,bb,zz,rr,chi);   
	}
      
      }
    }
    
    
    return true;
}
  
float StationFitPB::SetDriftSpace(float tdr,float rad,float zeta,float phim) {
  
    float flyspa,sy,x,y,phip,phis,prt,tof;
  
        //printf("tdr = %9.3f  rad = %9.3f  zeta = %9.3f  phim = %9.3f\n",
        //        tdr,rad,zeta,phim);
    sy = 1.;
    if(phim>=M_PI) {phim = phim - 2*M_PI; sy = -1.;}
    y = sy*fabs(m_track_fit.ap*rad*sqrt(1./(1.+m_track_fit.ap*m_track_fit.ap)));
    x = y/m_track_fit.ap;
    phip = atan2(y,x);
    phis = tan(fabs(phim-phip))*rad;
        //printf("phip=%9.3f, phim=%9.3f, phis=%9.3f\n",phip,phim,phis);
    if(phim>=phip) prt = - phis/(CSPEED*MDT_RED);
    else prt = + phis/(CSPEED*MDT_RED);
    flyspa = sqrt(rad*rad+zeta*zeta+phis*phis);
    tof = flyspa/CSPEED;
        //printf("tof = %9.3f   prt = %9.3f\n",tof*1.e+9,prt*1.e+9);
    return (tdr - tof*1.e+9 - prt*1.e+9)*DRIFTVE;
    //    return (tdr)*DRIFTVE;
}
  
  
float StationFitPB::SetDriftSigma(float /* ddr */) {
  
    // CID 22998: PARAM_SET_BUT_NOT_USED 
    // ddr = 0;      // for the time being use only the nominal tube resolution
    return SIGMA;
  
}


/*==============================================================================

      Call CIRCLES to fit a line, then rejects the worst point(s)

      IFIT = 0 ok,     = 1 failure
      IFLA = 0 normal, = 1 do not try to exclude points

==============================================================================*/

int StationFitPB::Evlfit(int Ifla) {

    int i,j,k,Ifit,Ntry,IGcur,Jbad;
    float Xnor,rlin,Xbad,test;
  
    m_fitres.NDOF = -2;
    Ntry = 0;
    Ifit = 0;
  
    for(j=0;j<m_fitres.NPOI;j++) if(m_fitres.IGLIN[j]>=1) m_fitres.NDOF++;
  
    //  printf("m_fitres.NDOF = %2d\n",m_fitres.NDOF);
  
    for( ; ;) {
        while(m_fitres.NDOF>=1) {
            //      printf("m_fitres.NDOF = %2d\n",m_fitres.NDOF);
            Ntry++;
            Circles(m_fitres.NPOI,m_fitres.XILIN,m_fitres.YILIN,m_fitres.RILIN,m_fitres.WILIN,
                    m_fitres.IGLIN,&m_fitres.ALIN,&m_fitres.BLIN,m_fitres.DABLIN,&m_fitres.CHI2,
                    &m_fitres.PCHI2);
            if(m_fitres.CHI2<=0.) break;
            Xnor = 1. / sqrt(1. + m_fitres.ALIN * m_fitres.ALIN);
            for(i=0;i<m_fitres.NPOI;i++) m_fitres.RESI[i] = 0.;
            for(j=0;j<m_fitres.NPOI;j++) {
                m_fitres.DISTJ[j] = (m_fitres.ALIN * m_fitres.XILIN[j] + m_fitres.BLIN - 
                                   m_fitres.YILIN[j]) * Xnor;
                IGcur = abs(m_fitres.IGLIN[j])%100;               // mod 961017
                if(IGcur==1) m_fitres.RESI[j] = m_fitres.DISTJ[j];
                else if (IGcur==2) {
                    rlin = (m_fitres.DISTJ[j]>=0.) ? m_fitres.RILIN[j] : -m_fitres.RILIN[j];
                    m_fitres.RESI[j] = m_fitres.DISTJ[j] - rlin;
                }
                else if (IGcur==3) m_fitres.RESI[j] = m_fitres.DISTJ[j] - m_fitres.RILIN[j];
            }
     
            if(m_fitres.PCHI2>=0.01||Ifla==1) return Ifit;

            if(m_fitres.NDOF<=1||Ntry>=6) {
                Ifit  = 1;
                m_fitres.NDOF  = 0;
                m_fitres.ALIN  = 0.;
                m_fitres.BLIN  = 0.;
                m_fitres.CHI2  = -1.;
                m_fitres.PCHI2 = -.5;
                for(i=0;i<2;i++) for(k=0;k<2;k++) m_fitres.DABLIN[i][k] = 0.;
                for(j=0;j<m_fitres.NPOI;j++) m_fitres.IGLIN[j] = - abs(m_fitres.IGLIN[j])%100;
                return Ifit;
            }

            //    Exclude the worst point

            Jbad = 0;
            Xbad = -1.;
            for(j=0;j<m_fitres.NPOI;j++) {
                if(m_fitres.IGLIN[j]>=1) {
                    test = m_fitres.RESI[j] * m_fitres.RESI[j] * m_fitres.WILIN[j];
                    if(test>=Xbad) {
                        Xbad = test;
                        Jbad = j;
                    }
                }
            }
      
            //    Try again

            m_fitres.IGLIN[Jbad] = - m_fitres.IGLIN[Jbad] - 100;
            m_fitres.NDOF        = m_fitres.NDOF - 1;

#ifndef NDEBUG
            if(m_msg->debug_printout()>>number() & 1) 
            {
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
                __osstream display;
#else                                // put your gcc 2.95 specific code here
                char buffer[100];
                for (int i=0;i<100;++i) buffer[i] = '\0';
                __osstream display(buffer,100);
#endif

		display << "                EVLFIT. Remove " << setw(2)
			<< m_fitres.IDMEA[Jbad] << "  " << setw(1)
			<< setprecision(4) << Xbad << endl;
    
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
    //nothing to do
#else                                // put your gcc 2.95 specific code here
    //close the stream
    display << ends;
#endif  

		m_msg->displayDebug(display.str());
            } 
#endif
        }
        return Ifit;
    }
}


/*==============================================================================

      Computes the best line tangent to many circles or points

      Version 1 - P.B. 960715 - First attempt
      Version 2 - P.B. 960910 - New algorithm for choice of the signs

      y = A x + b

      Input :
      =======

      NMEAS   : number of meas
      IG(i)   : for each meas, a flag.

      ---- if IG = 1 ---
      Xi, Yi  : x, y of the points (i=1...NMEAS) (e.g. wire of a mwp)
      RRi     : meaningless

      ---- if IG = 2 ---
      Xi, Yi  : x, y of the centers (i=1...NMEAS) (i.e. wire of a mdt)
      RRi     : distance line-center (i.e. measured drift distance)

      ---- if IG = 3 ---
      Xi, Yi  : as above, for IG=2
      RRi     : as above, but with +ve or -ve sign
 
      ---- if IG.le.0 ---> point NOT to be used
      Xi, Yi  : meaningless
      RRi     : meaningless

      Wi      : weight for the i-th meas (= 1/sigma**2)

      Output :
      ========

      A,B,DAB : line coefficients (y = Ax + b) + error matrix (2x2)
      CHI2    : Chi square = Sum Wi * (RRi - dist) ** 2 (error if .lt.0)
      PCHI2   : P(chisquare,d.o.f.)

==============================================================================*/

void StationFitPB::Circles (int Nmeas,float *XI,float *YI,float *RI,float *WI,int *IG,
                       float *A,float *B,float DAB[2][2],float *Chi2,float *Pchi2) {

    float RRi[NMEAMX],WIlim,CHbest,Abest,Bbest;
    float A0,B0,SAA,SBB,SAB,Square,Aj,Bj;
    int i,j,jj,Ntrue,Ngood,Nbest,Ntry,NOgo,Isig,Iflg,Ksign[4],Jbest;
    int Ntrue2,Igg[NMEAMX];

//    Find the four "besrit" points (equispaced, wi.gt.1/2*wimax)

    *Chi2 = -1.;
    if (Nmeas<=2||Nmeas>=NMEAMX+1) return;
    Ntrue = 0;
    Ngood = 0;
    Nbest = 3;
    WIlim = 0.;
    
    Abest = 0.;
    Bbest = 0.;
    Jbest = 0;
    
    for(i=0;i<4;i++) Ksign[i] = -1;

    for(j=0;j<Nmeas;j++) {
        if(IG[j]>=1) {
            Ntrue++;
            WIlim = (WIlim>=WI[j])? WIlim : WI[j];
        }
    }
    if(Ntrue<=2) return;
    WIlim = 0.1 * WIlim;

    for(j=0;j<Nmeas;j++) if(IG[j]>=1&&WI[j]>=WIlim) Ngood++;
      
    for(j=0;j<Nmeas;j++) {
        if(IG[j]>=1&&(WI[j]>=WIlim||Ngood<=3)) {
            if(Ksign[0]==-1.) Ksign[0] = j;
            else if(Ksign[1]==-1.) Ksign[1] = j;
            else if(Ksign[2]==-1.) Ksign[2] = j;
            else if(Ksign[3]==-1.) {
                Ksign[3] = j;
                Nbest    = 4;
            } else {
                Ksign[2] = Ksign[3];
                Ksign[3] = j;
            }
        }
    }

#ifndef NDEBUG
    if(m_msg->debug_printout()>>number() & 1) 
    {
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
        __osstream display;
#else                                // put your gcc 2.95 specific code here
        char buffer[300];
        for (int i=0;i<300;++i) buffer[i] = '\0';
        __osstream display(buffer,300);
#endif

	display << "               Circles. Nmeas :" << setw(2) << Ntrue
		<< " - Best points :";
        for(i=0;i<Nbest;i++) display << setw(2) << Ksign[i] << " "; 
	display << endl;
    
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
    //nothing to do
#else                                // put your gcc 2.95 specific code here
    //close the stream
    display << ends;
#endif  

        m_msg->displayDebug(display.str());
    }
#endif

    //    First attempt, try with a line through the centers

    Xline(XI,YI,WI,IG,Nmeas,&A0,&B0,&SAA,&SBB,&SAB,&Square);



#ifndef NDEBUG
    if(m_msg->debug_printout()>>number() & 1) 
    {
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
        __osstream display;
#else                                // put your gcc 2.95 specific code here
        char buffer[100];
        for (int i=0;i<100;++i) buffer[i] = '\0';
        __osstream display(buffer,100);
#endif

        display << "               Circles. 0-th approx :" << setw(1)
		<< setprecision(4) << A0 << "   " << setw(10)
		<< setprecision(4) << B0 << endl;
    
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
    //nothing to do
#else                                // put your gcc 2.95 specific code here
    //close the stream
    display << ends;
#endif  

	m_msg->displayDebug(display.str());
    }
#endif
 
 
    //    Then try 16 times trough the best four points

    for(i=0;i<NMEAMX;i++) Igg[i] = -1;
    CHbest = 1.e25;
    Ntry   = (int)floor(pow(2.,Nbest)) - 1;         // 2**4 - 1 = 15
    for(j=0;j<=Ntry;j++) {
        NOgo = 0;
        for(jj=1;jj<=Nbest;jj++) {
            Isig = (j&(int)pow(2.,jj-1))? 1 : 0;
            //          Isig = ibits(&j,&jj1,&one);            // alternatively 0, 1
            Iflg = IG[Ksign[jj-1]];
            Igg[Ksign[jj-1]] = Iflg;
            RRi[Ksign[jj-1]] = RI[Ksign[jj-1]];
            if(Iflg==2) {
                Igg[Ksign[jj-1]] = 3;
                if(Isig==1) RRi[Ksign[jj-1]] = - RI[Ksign[jj-1]];
            } else if(Isig==1) NOgo = 1;
        }
        if(NOgo==0) {


#ifndef NDEBUG
            if(m_msg->debug_printout()>>number() & 1) 
            {
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
                __osstream display;
#else                                // put your gcc 2.95 specific code here
                char buffer[300];
                for (int i=0;i<300;++i) buffer[i] = '\0';
                __osstream display(buffer,300);
#endif

                display << "               Circles. Fit " << setw(2)
                        << j << ". Radii :";
                for(i=0;i<Nbest;i++) if(RRi[Ksign[i]]>=0) display << " 1 ";
                else display << "-1 ";
                display << endl;
    
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
    //nothing to do
#else                                // put your gcc 2.95 specific code here
    //close the stream
    display << ends;
#endif  

		m_msg->displayDebug(display.str());
            }
#endif

            Aj = A0;
            Bj = B0;
            Circfit(Nmeas,XI,YI,RRi,WI,Igg,&Aj,&Bj,DAB,Chi2);

#ifndef NDEBUG
            if (m_msg->debug_printout()>>number() & 1) 
            {
 #if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
                __osstream display;
#else                                // put your gcc 2.95 specific code here
                char buffer[100];
                for (int i=0;i<100;++i) buffer[i] = '\0';
                __osstream display(buffer,100);
#endif

		display << "               Result. A, B, CHI2 :" << setw(10)
			<< setprecision(4) << Aj << "  " << setw(10)
			<< setprecision(4) << Bj << "  " << setw(10)
			<< setprecision(4) << *Chi2 << endl << endl;
    
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
    //nothing to do
#else                                // put your gcc 2.95 specific code here
    //close the stream
    display << ends;
#endif

                m_msg->displayDebug(display.str());
            }
#endif

            if(*Chi2>=0.0&&*Chi2<=CHbest) {
                Jbest  = j;
                Abest  = Aj;
                Bbest  = Bj;
                CHbest = *Chi2;
            }
        }
    }

#ifndef NDEBUG
    if(m_msg->debug_printout()>>number() & 1) 
    {
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
        __osstream display;
#else                                // put your gcc 2.95 specific code here
        char buffer[100];
        for (int i=0;i<100;++i) buffer[i] = '\0';
        __osstream display(buffer,100);
#endif

        display << "               Circles. Best fit is n " << setw(2) 
                << Jbest << endl;
    
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
    //nothing to do
#else                                // put your gcc 2.95 specific code here
    //close the stream
    display << ends;
#endif 

        m_msg->displayDebug(display.str());
    }
#endif

    //    ... and finally with all the points

    *A = Abest;
    *B = Bbest;
    Circfit(Nmeas,XI,YI,RI,WI,IG,A,B,DAB,Chi2);

#ifndef NDEBUG
    if(m_msg->debug_printout()>>number() & 1) 
    {
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
        __osstream display;
#else                                // put your gcc 2.95 specific code here
        char buffer[100];
        for (int i=0;i<100;++i) buffer[i] = '\0';
        __osstream display(buffer,100);
#endif

	display << "               Result. A, B, CHI2 :" << setw(10)
			<< setprecision(4) << *A << "  " << setw(10)
			<< setprecision(4) << *B << "  " << setw(10)
			<< setprecision(4) << *Chi2 << endl << endl;
    
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
    //nothing to do
#else                                // put your gcc 2.95 specific code here
    //close the stream
    display << ends;
#endif    
 
	m_msg->displayDebug(display.str());
    }
#endif
      
    if(*Chi2>=0.0) {
        Ntrue2 = Ntrue - 2;
        // *Pchi2 = prob(Chi2,&Ntrue2);
	*Pchi2 = TMath::Prob(*Chi2,Ntrue2);
    }
}


/*==============================================================================

      Auxiliary to circles (see) - It makes the actual computations

      NB - A and B MUST contain on input a first approximation of
           their values
  
==============================================================================*/

void StationFitPB::Circfit (int Nmeas,float *XI,float *YI,float *RI,float *WI,int *IG,
                       float *A,float *B,float DAB[2][2],float *Chi2) {

    float XX[NMEAMX],YY[NMEAMX],Test,Toll,Xnor,Aold,Bold,Epsi;
    float SAA,SAB,SBB,Square;
    int j,Niter;

    Toll    = .1;
    Niter   = 0;
    //      *A      = 0.;
    //      *B      = 0.;
    //      SAA     = 0.;
    //      SAB     = 0.;
    //      SBB     = 0.;
    Square  = 0.;

#ifndef NDEBUG      
    if(m_msg->debug_printout()>>number() & 1) 
    {
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
        __osstream display;
#else                                // put your gcc 2.95 specific code here
        char buffer[300];
        for (int i=0;i<300;++i) buffer[i] = '\0';
        __osstream display(buffer,300);
#endif

        display << "               Circfit. N,a,b : " << setw(2) << Nmeas
		<< "  " << setiosflags(ios::scientific) << setw(15) 
                << setprecision(5) << *A << "  " 
                << setiosflags(ios::scientific)
		<< setw(15) << setprecision(5) << *B << endl;

	display << "               Points flags: ";
        for(int i=0;i<Nmeas;i++) display << setw(2) << IG[i];
	display << endl;
    
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
    //nothing to do
#else                                // put your gcc 2.95 specific code here
    //close the stream
    display << ends;
#endif     

	m_msg->displayDebug(display.str());
    }
#endif

    //    Many iterations ...

    do {
        Niter++;
        Xnor  = 1. / sqrt(1. + *A * *A);
        Aold  = *A;
        Bold  = *B;
        for(j=0;j<Nmeas;j++) {
            XX[j] = 0.;
            YY[j] = 0.;
            if(IG[j]==1) {
                XX[j] = XI[j];
                YY[j] = YI[j];
            } else if(IG[j]==2) {
                if(*A * XI[j] + *B - YI[j]>=0.) Epsi = 1.0;    // mod 961017
                else Epsi = -1.0;
                XX[j] = XI[j] - Epsi * Xnor * fabs(RI[j]) * *A;
                YY[j] = YI[j] + Epsi * Xnor * fabs(RI[j]);
            } else if(IG[j]==3) {
                XX[j] = XI[j] - Xnor * RI[j] * *A;
                YY[j] = YI[j] + Xnor * RI[j];
            }
        }

        Xline(XX,YY,WI,IG,Nmeas,A,B,&SAA,&SBB,&SAB,&Square);
        if(Square<=0.) break;
        Test = ((Aold-*A)*(Aold-*A))/ SAA + ((Bold-*B)*(Bold-*B))/ SBB;

#ifndef NDEBUG
        if(m_msg->debug_printout()>>number() & 1) 
        {
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
            __osstream display;
#else                                // put your gcc 2.95 specific code here
            char buffer[100];
            for (int i=0;i<100;++i) buffer[i] = '\0';
            __osstream display(buffer,100);
#endif

	    display << "               Iter = " << setw(2) << Niter
		    << "    Test = " << setiosflags(ios::scientific) 
		    << setw(10) << setprecision(2) << Test << endl;
    
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
    //nothing to do
#else                                // put your gcc 2.95 specific code here
    //close the stream
    display << ends;
#endif  

            m_msg->displayDebug(display.str());
        }
#endif

    } while(Test>=Toll&&Niter<=20);


    DAB[0][0] = SAA;
    DAB[0][1] = SAB;
    DAB[1][0] = SAB;
    DAB[1][1] = SBB;
    *Chi2     = Square;
}


/*==============================================================================
      A simple linear fit : y = A x + B     (see PDG 94, 17.20-25)

      W  = weights ( = 1./err**2)
      IG = flags   (if .le.0, don't use)
==============================================================================*/

void StationFitPB::Xline (float *X,float *Y,float *W,int *IG,int NP,
                     float *A,float *B,float *SAA,float *SBB,float *SAB,float *Square) {
      
    int j;
    float S1,SX,SY,SXX,SXY,SYY,Deter,DY;

    *Square = -7.;
    S1      = 0.;
    SX      = 0.;
    SY      = 0.;
    SXX     = 0.;
    SXY     = 0.;
    SYY     = 0.;
  
    for(j=0;j<NP;j++) {
        if(IG[j]>=1) {
            S1  = S1  + W[j];
            SX  = SX  + W[j] * X[j];
            SY  = SY  + W[j] * Y[j];
            SXX = SXX + W[j] * X[j] * X[j];
            SXY = SXY + W[j] * X[j] * Y[j];
            SYY = SYY + W[j] * Y[j] * Y[j];
        }
    }
  
    Deter  = S1 * SXX - SX * SX;
  
    if (Deter!=0.) {
        *A      = (S1 * SXY - SX * SY)  / Deter;
        *B      = (SY * SXX - SX * SXY) / Deter;
        *SAA    =   S1  / Deter;
        *SBB    =   SXX / Deter;
        *SAB    = - SX  / Deter;
    
        *Square = 0.;
        for(j=0;j<NP;j++) {
            if(IG[j]>=1) {
                DY = Y[j] - *A * X[j] - *B;
                //	printf("Punto n.=%d , DY = %12.6f\n",j,DY);
                *Square = *Square + W[j] * DY * DY;
            }
        }
    }
   
#ifndef NDEBUG
    if (m_msg->debug_printout()>>number() & 1) 
    {
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
        __osstream display;
#else                                // put your gcc 2.95 specific code here
        char buffer[100];
        for (int i=0;i<100;++i) buffer[i] = '\0';
        __osstream display(buffer,100);
#endif

	display << "               Xline.  a,b,sq: " 
                << setiosflags(ios::scientific) << setw(15) << setprecision(5)
		<< *A << setiosflags(ios::scientific) << setw(15)
		<< setprecision(5) << *B << setiosflags(ios::scientific)
		<< setw(15) << setprecision(5) << *Square << endl;
    
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
    //nothing to do
#else                                // put your gcc 2.95 specific code here
    //close the stream
    display << ends;
#endif

        m_msg->displayDebug(display.str());
    }
#endif
}

/*============================================================================*/


