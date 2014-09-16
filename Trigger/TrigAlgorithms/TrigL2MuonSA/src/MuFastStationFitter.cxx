/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2MuonSA/MuFastStationFitter.h"

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include "CLHEP/Units/PhysicalConstants.h"

#include "xAODTrigMuon/TrigMuonDefs.h"

#include <math.h>
#include <iomanip>

#include "TMath.h"

using namespace TrigL2MuonSA;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

static const InterfaceID IID_MuFastStationFitter("IID_MuFastStationFitter", 1, 0);

const InterfaceID& TrigL2MuonSA::MuFastStationFitter::interfaceID() { return IID_MuFastStationFitter; }

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::MuFastStationFitter::MuFastStationFitter(const std::string& type, 
						       const std::string& name,
						       const IInterface*  parent): 
  AlgTool(type,name,parent),
  m_msg(0),
  m_storeGateSvc( "StoreGateSvc", name )
{
   declareInterface<TrigL2MuonSA::MuFastStationFitter>(this);
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::MuFastStationFitter::~MuFastStationFitter() 
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuFastStationFitter::initialize()
{
   // Get a message stream instance
   m_msg = new MsgStream( msgSvc(), name() );
   msg() << MSG::DEBUG << "Initializing MuFastStationFitter - package version " << PACKAGE_VERSION << endreq ;
   
   StatusCode sc;
   sc = AlgTool::initialize();
   if (!sc.isSuccess()) {
      msg() << MSG::ERROR << "Could not initialize the AlgTool base class." << endreq;
      return sc;
   }
   
   // Locate the StoreGateSvc
   sc =  m_storeGateSvc.retrieve();
   if (!sc.isSuccess()) {
      msg() << MSG::ERROR << "Could not find StoreGateSvc" << endreq;
      return sc;
   }

   // 
   return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuFastStationFitter::findSuperPoints(const LVL1::RecMuonRoI*    p_roi,
							      TrigL2MuonSA::RpcFitResult rpcFitResult,
							      std::vector<TrigL2MuonSA::TrackPattern>& v_trackPatterns)
{
  msg() << MSG::DEBUG << "in findSuperPoints in Barrel" << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  // 
  std::vector<TrigL2MuonSA::TrackPattern>::iterator itTrack;
  for (itTrack=v_trackPatterns.begin(); itTrack!=v_trackPatterns.end(); itTrack++) { // loop for track candidates

    if (rpcFitResult.isSuccess) {
      itTrack->phiDir = rpcFitResult.phiDir;
    } else {
      itTrack->phiDir = (cos(p_roi->phi())!=0)? tan(p_roi->phi()): 0;
      itTrack->isRpcFailure = true;
    }
    
    sc = superPointFitter(*itTrack);
    if (sc != StatusCode::SUCCESS) return sc;
  }
  // 

  return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuFastStationFitter::findSuperPoints(const LVL1::RecMuonRoI*   p_roi,
							      TrigL2MuonSA::TgcFitResult tgcFitResult,
							      std::vector<TrigL2MuonSA::TrackPattern>& v_trackPatterns)
{

  StatusCode sc = StatusCode::SUCCESS;
  
  std::vector<TrigL2MuonSA::TrackPattern>::iterator itTrack;
  for (itTrack=v_trackPatterns.begin(); itTrack!=v_trackPatterns.end(); itTrack++) { // loop for track candidates

    if (tgcFitResult.isSuccess) {
      itTrack->phiDir = tgcFitResult.phiDir;
    } else {
      itTrack->phiDir = (cos(p_roi->phi())!=0)? tan(p_roi->phi()): 0;
      itTrack->isTgcFailure = true;
    }

    sc = superPointFitter(*itTrack);
    if (sc != StatusCode::SUCCESS) return sc;
  }
  // 
  return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuFastStationFitter::superPointFitter(TrigL2MuonSA::TrackPattern& trackPattern)
{
   int   count;
   int   FitFlag;
   float Xor, Yor, sigma, rm=0., phim=0;
   float Ymid, Xmid, Amid;

   const float ZERO_LIMIT         = 1e-6;
   const unsigned int MAX_STATION = 8;
   const float SIGMA              = 0.0080;
   const float DRIFTSPACE_LIMIT   = 16.;
   const int   MIN_MDT_FOR_FIT    = 3;

   TrigL2MuonSA::MdtHits*    mdtSegment;
   TrigL2MuonSA::SuperPoint* superPoint;
   TrigL2MuonSA::PBFitResult pbFitResult;

   for (unsigned int chamber=0; chamber<MAX_STATION; chamber++) { // loop for station
     
     count = 0;
     Xor = 0.;
     Yor = 0.;
     Amid = 0.;
     Xmid = 0.;
     Ymid = 0.;
     
     mdtSegment = &(trackPattern.mdtSegments[chamber]);
     superPoint = &(trackPattern.superPoints[chamber]);
     
     if (mdtSegment->size()==0) continue;

     TrigL2MuonSA::MdtHits::iterator itMdtHit;
     for (itMdtHit=mdtSegment->begin(); itMdtHit!=mdtSegment->end(); itMdtHit++) { // loop for MDT hit
       
       if (itMdtHit->isOutlier) continue;
       
       superPoint->Ndigi++;
       
       if (!count) {
	 rm   = itMdtHit->cYmid;
	 Amid = itMdtHit->cAmid;
	 Xmid = itMdtHit->cXmid;
	 Ymid = itMdtHit->cYmid;
       }
       if (!Xor) {
	 Xor = itMdtHit->R;
	 Yor = itMdtHit->Z;
       }
       
       phim  = itMdtHit->cPhip;
       sigma = (fabs(itMdtHit->DriftSigma) > ZERO_LIMIT)? itMdtHit->DriftSigma: SIGMA;
       int station = 0;
       if (chamber == 0 || chamber == 3 ) station = 0;
       if (chamber == 1 || chamber == 4 ) station = 1;
       if (chamber == 2 || chamber == 5 ) station = 2;
       if (chamber == 6 ) station = 3;
       if ( fabs(itMdtHit->DriftSpace) > ZERO_LIMIT &&
	    fabs(itMdtHit->DriftSpace) < DRIFTSPACE_LIMIT &&
	    fabs(itMdtHit->DriftTime) > ZERO_LIMIT ) {
	 
	 pbFitResult.XILIN[count] = itMdtHit->R - Xor;
	 pbFitResult.YILIN[count] = itMdtHit->Z - Yor;
	 pbFitResult.IGLIN[count] = 2;
	 pbFitResult.RILIN[count] = (fabs(itMdtHit->DriftSpace) > ZERO_LIMIT)?
	   itMdtHit->DriftSpace: SetDriftSpace(itMdtHit->DriftTime, itMdtHit->R, itMdtHit->Z, phim, trackPattern.phiDir);
	 pbFitResult.WILIN[count] = 1/(sigma*sigma);
	 pbFitResult.JLINE[count] = count;
	 pbFitResult.IDMEA[count] = station*10 + itMdtHit->Layer;
	 pbFitResult.NLINE[count]++;
	 
	 pbFitResult.DISTJ[count] = 0.;
	 pbFitResult.RESI[count] = 0.;
	 
	 count++;
	 pbFitResult.NPOI = count;

       } else {
	 superPoint->Ndigi--;
       }
       
     } // end loop for MDT hits

     msg() << MSG::DEBUG << "... MDT hit used in fit #=" << pbFitResult.NPOI << endreq;
     for(int i=0;i<pbFitResult.NPOI;i++) {
       msg() << MSG::DEBUG << "i/XILIN[i]/YILIN[i]/RILIN[i]/WILIN[i] = "
	     << i << "/" << pbFitResult.XILIN[i] << "/" << pbFitResult.YILIN[i]
	     << "/" << pbFitResult.RILIN[i] << "/" << pbFitResult.WILIN[i] << endreq;
     }
     
     if (count >= MIN_MDT_FOR_FIT) {

       FitFlag = Evlfit(1, pbFitResult);
       msg() << MSG::DEBUG << "FitFlag = " << FitFlag << endreq;

       
       float ac = Amid;
       float bc = (Ymid - Xor) -ac*(Xmid - Yor);
       float X = ( (pbFitResult.ALIN*bc)+pbFitResult.BLIN )/( 1-ac*pbFitResult.ALIN );
       
       superPoint->Npoint = pbFitResult.NPOI;
       
       if (trackPattern.s_address == -1) { // Endcap
	 
	 if(pbFitResult.ALIN!=0. ) {
	   superPoint->Z = rm;
	   superPoint->R = (rm-Yor)/pbFitResult.ALIN - pbFitResult.BLIN/pbFitResult.ALIN + Xor;
	   superPoint->Alin = 1./pbFitResult.ALIN;
	   superPoint->Blin = -pbFitResult.BLIN/pbFitResult.ALIN;
           if (chamber==0){//endcap barrel inner
	     superPoint->R      = ac*X + bc + Xor;
	     superPoint->Z      = X + Yor;
	     superPoint->Alin   = pbFitResult.ALIN;
	     superPoint->Blin   = pbFitResult.BLIN;
           }
	 }
	 
       } else { // Barrel
	 
	 superPoint->R      = ac*X + bc + Xor;
	 superPoint->Z      = X + Yor;
	 superPoint->Alin   = pbFitResult.ALIN;
	 superPoint->Blin   = pbFitResult.BLIN; 
	 
       }
       
       superPoint->Phim   = phim;
       superPoint->Xor    = Xor;
       superPoint->Yor    = Yor;
       superPoint->Chi2   = pbFitResult.CHI2;
       superPoint->PChi2  = pbFitResult.PCHI2;
       for(int i=0;i<pbFitResult.NPOI;i++) superPoint->Residual[i] =  pbFitResult.RESI[i];
       
     }
     
     msg() << MSG::DEBUG << "... Superpoint chamber/s_address/count/R/Z/Alin/Blin/Phim/Xor/Yor/Chi2/PChi2="
	   << chamber << "/" << trackPattern.s_address << "/" << count << "/"
	   << superPoint->R << "/" << superPoint->Z << "/" << superPoint->Alin << "/"
	   << superPoint->Blin << "/" << superPoint->Phim << "/" << superPoint->Xor << "/"
	   << superPoint->Yor << "/" << superPoint->Chi2 << "/" << superPoint->PChi2 << endreq;

   } // end loop for stations
   
   //
   return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuFastStationFitter::finalize()
{
   msg() << MSG::DEBUG << "Finalizing MuFastStationFitter - package version " << PACKAGE_VERSION << endreq;
   
   // delete message stream
   if ( m_msg ) delete m_msg;
   
   StatusCode sc = AlgTool::finalize(); 
   return sc;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

float TrigL2MuonSA::MuFastStationFitter::SetDriftSpace(float tdr, float rad, float zeta, float phim, float phiDir) {
  
  const float CSPEED = 2.99979e+10;    //     light speed (cm/s)
  const float MDT_RED = 0.7;
  const float DRIFTVE = 0.00209;        //     1.46/700 drif velocity (cm/ns)

  float flyspa,sy,x,y,phip,phis,prt,tof;
  
  sy = 1.;

  if(phim>=M_PI) {phim = phim - 2*M_PI; sy = -1.;}

  y = sy*fabs(phiDir*rad*sqrt(1./(1.+phiDir*phiDir)));
  x = y/phiDir;
  phip = atan2(y,x);
  phis = tan(fabs(phim-phip))*rad;

  if(phim>=phip) prt = - phis/(CSPEED*MDT_RED);
  else prt = + phis/(CSPEED*MDT_RED);

  flyspa = sqrt(rad*rad+zeta*zeta+phis*phis);
  tof = flyspa/CSPEED;

  return (tdr - tof*1.e+9 - prt*1.e+9)*DRIFTVE;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

/*==============================================================================

      Call CIRCLES to fit a line, then rejects the worst point(s)

      IFIT = 0 ok,     = 1 failure
      IFLA = 0 normal, = 1 do not try to exclude points

==============================================================================*/

int TrigL2MuonSA::MuFastStationFitter::Evlfit(int Ifla, TrigL2MuonSA::PBFitResult& pbFitResult) {

  int i,j,k,Ifit,Ntry,IGcur,Jbad;
  float Xnor,rlin,Xbad,test;
  
  pbFitResult.NDOF = -2;
  Ntry = 0;
  Ifit = 0;
  
  const double ZERO_LIMIT = 1e-6;
  
  for(j=0;j<pbFitResult.NPOI;j++) if(pbFitResult.IGLIN[j]>=1) pbFitResult.NDOF++;
  
  for( ; ;) {
    while(pbFitResult.NDOF>=1) {

      //      printf("pbFitResult.NDOF = %2d\n",pbFitResult.NDOF);
       Ntry++;
       Circles(pbFitResult.NPOI,pbFitResult.XILIN,pbFitResult.YILIN,pbFitResult.RILIN,pbFitResult.WILIN,
	       pbFitResult.IGLIN,&pbFitResult.ALIN,&pbFitResult.BLIN,pbFitResult.DABLIN,&pbFitResult.CHI2,
	       &pbFitResult.PCHI2);
       if(pbFitResult.CHI2<=ZERO_LIMIT) break;
       Xnor = 1. / sqrt(1. + pbFitResult.ALIN * pbFitResult.ALIN);
       for(i=0;i<pbFitResult.NPOI;i++) pbFitResult.RESI[i] = 0.;
       for(j=0;j<pbFitResult.NPOI;j++) {
	 pbFitResult.DISTJ[j] = (pbFitResult.ALIN * pbFitResult.XILIN[j] + pbFitResult.BLIN - 
				 pbFitResult.YILIN[j]) * Xnor;
	 IGcur = abs(pbFitResult.IGLIN[j])%100;               // mod 961017
	 if(IGcur==1) pbFitResult.RESI[j] = pbFitResult.DISTJ[j];
	 else if (IGcur==2) {
	   rlin = (pbFitResult.DISTJ[j]>=0.) ? pbFitResult.RILIN[j] : -pbFitResult.RILIN[j];
	   pbFitResult.RESI[j] = pbFitResult.DISTJ[j] - rlin;
	 }
                else if (IGcur==3) pbFitResult.RESI[j] = pbFitResult.DISTJ[j] - pbFitResult.RILIN[j];
       }
       
       if(pbFitResult.PCHI2>=0.01||Ifla==1) return Ifit;
       
       if(pbFitResult.NDOF<=1||Ntry>=6) {
	 Ifit  = 1;
	 pbFitResult.NDOF  = 0;
	 pbFitResult.ALIN  = 0.;
	 pbFitResult.BLIN  = 0.;
	 pbFitResult.CHI2  = -1.;
	 pbFitResult.PCHI2 = -.5;
	 for(i=0;i<2;i++) for(k=0;k<2;k++) pbFitResult.DABLIN[i][k] = 0.;
	 for(j=0;j<pbFitResult.NPOI;j++) {
	   pbFitResult.IGLIN[j] = - abs(pbFitResult.IGLIN[j])%100;
	 }
	 return Ifit;
       }
       
       //    Exclude the worst point
       
       Jbad = 0;
       Xbad = -1.;
       for(j=0;j<pbFitResult.NPOI;j++) {
	 if(pbFitResult.IGLIN[j]>=1) {
	   test = pbFitResult.RESI[j] * pbFitResult.RESI[j] * pbFitResult.WILIN[j];
	   if(test>=Xbad) {
	     Xbad = test;
	     Jbad = j;
	   }
	 }
       }
       
       //    Try again
       
       pbFitResult.IGLIN[Jbad] = - pbFitResult.IGLIN[Jbad] - 100;
       pbFitResult.NDOF        = pbFitResult.NDOF - 1;
       
     }
     return Ifit;
   }
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

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

void TrigL2MuonSA::MuFastStationFitter::Circles (int Nmeas,float *XI,float *YI,float *RI,float *WI,int *IG,
					       float *A,float *B,float DAB[2][2],float *Chi2,float *Pchi2) {

    float RRi[NMEAMX],WIlim,CHbest,Abest,Bbest;
    float A0,B0,SAA,SBB,SAB,Square,Aj,Bj;
    int i,j,jj,Ntrue,Ngood,Nbest,Ntry,NOgo,Isig,Iflg,Ksign[4];
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

    //    First attempt, try with a line through the centers

    Xline(XI,YI,WI,IG,Nmeas,&A0,&B0,&SAA,&SBB,&SAB,&Square);

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
            Aj = A0;
            Bj = B0;
            Circfit(Nmeas,XI,YI,RRi,WI,Igg,&Aj,&Bj,DAB,Chi2);

            if(*Chi2>=0.0&&*Chi2<=CHbest) {
                Abest  = Aj;
                Bbest  = Bj;
                CHbest = *Chi2;
            }
        }
    }
    //    ... and finally with all the points

    *A = Abest;
    *B = Bbest;
    Circfit(Nmeas,XI,YI,RI,WI,IG,A,B,DAB,Chi2);

    if(*Chi2>=0.0) {
        Ntrue2 = Ntrue - 2;
        *Pchi2 = TMath::Prob(*Chi2, Ntrue2);
    }
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

/*==============================================================================

      Auxiliary to circles (see) - It makes the actual computations

      NB - A and B MUST contain on input a first approximation of
           their values
  
==============================================================================*/

void TrigL2MuonSA::MuFastStationFitter::Circfit (int Nmeas,float *XI,float *YI,float *RI,float *WI,int *IG,
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

    } while(Test>=Toll&&Niter<=20);


    DAB[0][0] = SAA;
    DAB[0][1] = SAB;
    DAB[1][0] = SAB;
    DAB[1][1] = SBB;
    *Chi2     = Square;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

/*==============================================================================
      A simple linear fit : y = A x + B     (see PDG 94, 17.20-25)

      W  = weights ( = 1./err**2)
      IG = flags   (if .le.0, don't use)
==============================================================================*/

void TrigL2MuonSA::MuFastStationFitter::Xline (float *X,float *Y,float *W,int *IG,int NP,
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
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
 
