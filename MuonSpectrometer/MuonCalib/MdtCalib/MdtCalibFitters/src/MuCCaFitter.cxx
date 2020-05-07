/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibFitters/MuCCaFitter.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"
#include <cmath>

namespace MuonCalib {

void MuCCaFitter::printLevel(int level) {
  if(level > 0 ) m_debug = true;
}

bool MuCCaFitter::fit( MuonCalibSegment& seg ) const
{
  // select all hits
  HitSelection selection(seg.mdtHitsOnTrack(),0);
  // call fit function
  return fit(seg,selection);
}

bool MuCCaFitter::fit( MuonCalibSegment& seg, HitSelection selection ) const
{
  MsgStream log(Athena::getMessageSvc(),"MuCCaFitter");
  if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"fit() New seg:"<<endmsg;

  int N = seg.mdtHitsOnTrack();

  if(N<2) {
    return false;
  }

  if((int)selection.size() != N){
    selection.clear();
    selection.assign(N,0);
  }else{
    int used(0);
    for(int i=0;i<N;++i){
      if(selection[i] == 0) ++used;
    }
    if(used < 2){
      log<<MSG::WARNING<<"fit() TO FEW HITS SELECTED"<<endmsg;
      return false;
    }
  }

  Amg::Vector3D pos = seg.position(); 

  double S(0),Sy(0),Sz(0);
  double Zc,Yc;
  std::vector<double> y(N);
  std::vector<double> x(N);
  std::vector<double> z(N);
  std::vector<double> r(N);
  std::vector<double> sr(N);
  std::vector<double> w(N);
  std::vector<double> rw(N);
  int ii;
  int jj;
  {
    MuonCalibSegment::MdtHitIt hit = seg.mdtHOTBegin();

    ii=0;
    jj=0;
    while(hit != seg.mdtHOTEnd()){
      const MdtCalibHitBase& h = **hit;
      y[ii] = getY( h.localPosition() );
      z[ii] = getZ( h.localPosition() );
      r[ii] = std::abs( h.driftRadius() );
      
      if(h.sigma2DriftRadius() > 0){
	w[ii] = 1./( h.sigma2DriftRadius() );
	sr[ii] =std::sqrt( h.sigma2DriftRadius() );
      } else {
	w[ii] = 0.;
	sr[ii]=0.;
      }
      if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"fit() MuCCaFitter: (" << x[ii] << "," << y[ii] << ")  R = " << r[ii] << " W " << w[ii]<<endmsg;
      rw[ii]  = r[ii]*w[ii];
      if(selection[jj]){
	++hit;++jj;
	continue;
      }
      S+=w[ii];
      Sz+= w[ii]*z[ii];
      Sy+= w[ii]*y[ii];
      ++hit;++ii;++jj;
    }
  }
  Zc = Sz/S;
  Yc = Sy/S;

  MuCCaFitterImplementation *Fitter=new MuCCaFitterImplementation();
  if(log.level()<=MSG::DEBUG){
  for (int i = 0; i != ii; i++)  
    {  
      log<<MSG::DEBUG<<"fit() MuCCaFitter hits passed to computepam Z=" << x[i]<<" phi="<<y[i]<<" zzz="<<z[i]<<" drift="<<r[i]<<" error="<<w[i]<<endmsg;
    } 
  }
  Fitter->Computeparam3(ii,z,y,r,sr);
  if(log.level()<=MSG::DEBUG){
    log<<MSG::DEBUG<<"fit() MuCCaFitter computed track a=" <<Fitter->get_a()
       <<" da= "<<Fitter->get_da()
       <<" b= "<<Fitter->get_b()
       <<" db= "<<Fitter->get_db()
       <<" corrab= "<<Fitter->get_corrab()
       <<" chi2f= "<<Fitter->get_chi2f()<<endmsg;
  }

  double afit=Fitter->get_a();
  double chi2f=Fitter->get_chi2f();

  std::vector<double> dist(N);
  std::vector<double> ddist(N);
  double R,dis;
  double theta = std::atan(afit);
  if(theta<0.) theta= M_PI+theta;
  double sinus = std::sin(theta);
  double cosin = std::cos(theta);
  double d = -( getZ( pos )-Zc)*sinus+( getY( pos )-Yc)*cosin;
  if(log.level()<=MSG::DEBUG){
    log<<MSG::DEBUG<<"fit() MuCCaFitter>>> theta= " << theta << " sinus= " << sinus << " cosin= " << cosin<<endmsg;
    log<<MSG::DEBUG<<"fit() MuCCaFitter>>> getZ( pos )= " << getZ( pos ) << " getY( pos )= " << getY( pos ) << " Zc= " << Zc << " Yc= " << Yc<<endmsg;
  }
  double Szz(0),Syy(0),Szy(0),Syyzz(0),Att(0);
  R=0;
  for(int i=0;i<N;++i){
    if(selection[i]) continue;
    Szz   += (z[i]-Zc)*(z[i]-Zc)*w[i];
    Syy   += (y[i]-Yc)*(y[i]-Yc)*w[i];
    Szy   += (y[i]-Yc)*(z[i]-Zc)*w[i];
    Syyzz += ((y[i]-Yc)-(z[i]-Zc))*((y[i]-Yc)+(z[i]-Zc))*w[i];    
    dis = (y[i]-Yc)*cosin-(z[i]-Zc)*sinus;
    if(dis>d){
      R  -= rw[i];
    }else{
      R  += rw[i];
    }
  }
  Att = Syy + cosin*(2*sinus*Szy - cosin*Syyzz);
  d = R/S;
  if(log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"fit() MuCCaFitter>>> d= " << d << " R= " << R << " S= " << S << " Att= " << Att<<endmsg;
  for(int i=0;i<N;++i){
    if(selection[i]) continue;
    dist[i] = cosin*(y[i]-Yc) - sinus*(z[i]-Zc) - d;
    double dth = -(sinus*(y[i]-Yc) + cosin*(z[i]-Zc))*(std::sqrt(1./Att));
    ddist[i] = std::sqrt( dth*dth + (1./S) );  
  }
  if(log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"fit() Transforming back to real world"<<endmsg;
  Amg::Vector3D ndir = getVec( 0., sinus, cosin );
  Amg::Vector3D npos = getVec( 0., Yc + cosin*d, Zc - sinus*d );
  
  if(log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"fit() New line: position " << npos << " direction " << ndir << " chi2f " << chi2f<<endmsg;
   
   seg.set( chi2f/(N-2), npos, ndir );
   
   MuonCalibSegment::MdtHitIt it = seg.mdtHOTBegin();
   
   int i(0);
   while(it!=seg.mdtHOTEnd()){
     MdtCalibHitBase& hit = const_cast< MdtCalibHitBase& >( **it );
     
     hit.setDistanceToTrack( dist[i], ddist[i] );
     ++it;++i;
   }
   
   delete Fitter;
   if(log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"fit() fit done"<<endmsg;
   return true;
}
void MuCCaFitterImplementation::Computeparam3(int number_of_hits,std::vector<double> x,std::vector<double> y,std::vector<double> r,std::vector<double> sr)
{
/***************************************/
/* Fit a line to n=number_of_hits hits */
/***************************************/ 
  double xout[100],yout[100];
  double dist[100],rsub[100];
  double chi2outn[4],chi2min;
  double aref[4],bref[4];
  int fhit,i,j,icouple=0,lhit;
/* compute 4 tanget lines from first and last hit */  
  fhit = 0;
  lhit = number_of_hits-1;
  Computelinparnew(x[fhit],y[fhit],r[fhit],x[lhit],y[lhit],r[lhit]);
  for(int ii=0;ii<4;ii++){
    bref[ii]=m_bfpar[ii];
    aref[ii]=m_angularcoefficient[ii];
  }
/* choose the REFERENCE LINE (aref[icouple],bref[icouple])*/
  for(i=0;i<4;i++){
     for(j=0;j<number_of_hits;j++){
        double d;
        d = std::abs(aref[i]*x[j]+bref[i]-y[j]); 
        dist[j] = d/std::sqrt(1.+aref[i]*aref[i]);
        rsub[j] = r[j]-dist[j];
     }
     double chi2out = 0;
     for(int ii=1;ii<number_of_hits-1;ii++){
       chi2out += rsub[ii]*rsub[ii]/(sr[ii]*sr[ii]);
     } 
     chi2outn[i]=chi2out;
  }
  chi2min = 999999999.;
  for(i=0;i<4;i++){
     if(chi2outn[i]<chi2min){
         chi2min = chi2outn[i];
         icouple = i;
     }
  } 
/* Compute TRACK POINTS */
  Computehitpsemes(number_of_hits,x,y,r,aref[icouple],bref[icouple]);
  for(i=0;i<number_of_hits;i++){
   xout[i]=m_xpoint[i];
   yout[i]=m_ypoint[i];
  }
/* Compute a & b parameters of the track from TRACK POINTS */
  // int idim;
  //  ifail;
  double aoutn, bout, sig2a, sig2b, corrab;
  double temp,det; 
  double hesse[2][2];
  double W,WX,WX2,WY,WY2,WXY;
  //  double errormatrix[2][2];
  W=0.;
  WX=0.;
  WX2=0.;
  WY=0.;
  WY2=0.;
  WXY=0;
  for(int i=0;i<number_of_hits;i++){
    temp = 1./(sr[i]*sr[i]);
    W  += temp;
    WX += xout[i]*temp;
    WX2+= xout[i]*xout[i]*temp;
    WY += yout[i]*temp;
    WY2+= yout[i]*yout[i]*temp;
    WXY+= xout[i]*yout[i]*temp;
  }
  det = W*WX2-WX*WX;
  aoutn       = (W*WXY-WY*WX)/det;
  bout        = (WY*WX2-WX*WXY)/det;
  hesse[1][1] = W;
  hesse[0][0] = WX2;
  hesse[1][0] = WX;
  hesse[0][1] = WX;
  // idim        = 2;
/* invert hessian matrix */
  hesse[1][1] = hesse[0][0]/det;
  hesse[0][0] = hesse[1][1]/det;
  hesse[1][0] = -1.*(hesse[0][1]/det);
  hesse[0][1] = -1.*(hesse[0][1]/det);
  sig2a = hesse[0][0];
  sig2b = hesse[1][1];
  corrab = hesse[1][0];
  double deno,btest,bs1,bs2,db1,db2;
  btest = bout;
  bout   = 0;
  deno = 0;
  for(int i=0;i<number_of_hits;i++){
     bs1 = (y[i]-aoutn*x[i]-r[i]*std::sqrt(1+aoutn*aoutn));
     bs2 = (y[i]-aoutn*x[i]+r[i]*std::sqrt(1+aoutn*aoutn));
     db1 = std::abs(bs1-btest);
     db2 = std::abs(bs2-btest);
     if(db1<db2){
       bout  += bs1/(sr[i]*sr[i]); 
     }else{
       bout  +=bs2/(sr[i]*sr[i]);
     } 
     deno+= 1/(sr[i]*sr[i]);
  }
  bout /=deno;
/* compute chi2 from residuals*/
  double resd;
  double chi2f = 0;
  for(int i=0;i<number_of_hits;i++){ 
     double xi,yi;
     xi = (aoutn*y[i]-aoutn*bout+x[i])/(1.+aoutn*aoutn);
     yi = aoutn*xi+bout;
     resd = (std::sqrt((xi-x[i])*(xi-x[i])+(yi-y[i])*(yi-y[i]))-r[i])/sr[i];
     chi2f += resd*resd;
 }
/* set global variables (method output)*/
  m_aoutn=aoutn;
  m_sig2a=sig2a;
  m_bout=bout;
  m_sig2b=sig2b;
  m_corrab=corrab;
  m_chi2f=chi2f;
}

double MuCCaFitterImplementation::get_a() 
{
  return m_aoutn;
}
double MuCCaFitterImplementation::get_b() {return m_bout;}
double MuCCaFitterImplementation::get_da() {return m_sig2a;}
double MuCCaFitterImplementation::get_db() {return m_sig2b;}
double MuCCaFitterImplementation::get_corrab() {return m_corrab;}
double MuCCaFitterImplementation::get_chi2f() {return m_chi2f;}

void MuCCaFitterImplementation::Computelinparnew(double x1, double y1, double r1, double x2, double y2, double r2)
{
/********************************************/
/* Compute the 4 lines tangent to 2 circles */
/********************************************/
   double delta;
   double averagephi,dist,dphiin,dphiex;
   double bpar[4],phi;
   double bfparn[2];
   bfparn[0]=0; bfparn[1]=0;
   // int segnobfn[2];
   // int segnoc[2][4],ncandid[4],ncand;
   int ncandid[4],ncand;
   double bcand[2][4];
   int segnob[]={1,-1,1,-1};
   int firsttime,i;
   double angularcoefficient[4];
   double bfpar[4];
   // int segnobf[4];
   double dy, dx;

/* compute a parameters */
   dx = x2-x1;
   dy = y2-y1;
   averagephi = std::atan2(dy,dx);
   dist = std::sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)); 
   delta = r2+r1;

   dphiin = std::asin(delta/dist);

   delta = r2-r1;
   dphiex = std::asin(delta/dist);

   int f=1;
   phi = averagephi+f*dphiex;
   if(phi < 0){phi = 2*M_PI+(phi);}
   angularcoefficient[0] = std::tan(phi);

   f=-1;
   phi = averagephi+f*dphiex;
   if(phi < 0){phi = 2*M_PI+(phi);}
   angularcoefficient[1] = std::tan(phi);

   f=1;
   phi = averagephi+f*dphiin;
   if(phi < 0){phi = 2*M_PI+(phi);}
   angularcoefficient[2] = std::tan(phi);

   f=-1;
   phi = averagephi+f*dphiin;
   if(phi < 0){phi = 2*M_PI+(phi);}
   angularcoefficient[3] = std::tan(phi);

/* Compute b parameters */
   for(i=0;i<4;i++){
     bpar[0] = y1-angularcoefficient[i]*x1+segnob[0]*r1*
               std::sqrt(1+angularcoefficient[i]*angularcoefficient[i]);
     bpar[1] = y1-angularcoefficient[i]*x1+segnob[1]*r1*
               std::sqrt(1+angularcoefficient[i]*angularcoefficient[i]);
     bpar[2] = y2-angularcoefficient[i]*x2+segnob[2]*r2*
               std::sqrt(1+angularcoefficient[i]*angularcoefficient[i]);
     bpar[3] = y2-angularcoefficient[i]*x2+segnob[3]*r2*
               std::sqrt(1+angularcoefficient[i]*angularcoefficient[i]);
     double delta = 0.00001;
     ncand = 0;
     if(std::abs(bpar[0]-bpar[2])<delta){
       bfparn[ncand]= bpar[0];
       ncand           = ncand+1;     
     }
     if(std::abs(bpar[0]-bpar[3])<delta){
       bfparn[ncand]   = bpar[0];
       ncand           = ncand+1;     
     }
     if(std::abs(bpar[1]-bpar[2])<delta){
       bfparn[ncand]   = bpar[1];
       ncand           = ncand+1;     
     }
     if(std::abs(bpar[1]-bpar[3])<delta){
       bfparn[ncand]   = bpar[1];
       ncand           = ncand+1;
     }
       bcand[0][i]  = bfparn[0];
       bcand[1][i]  = bfparn[1];
       ncandid[i]   = ncand;
     }
     firsttime = 0;
     for(i=0;i<4;i++){
       if(ncandid[i]==1){
         bfpar[i] = bcand[0][i];
       }else{
         bfpar[i] = bcand[firsttime][i];
         firsttime++;
       }
     }
/* set global variables (method output)*/
   for(i=0;i<4;i++){
     m_bfpar[i]=bfpar[i];
     m_angularcoefficient[i]=angularcoefficient[i];
   }
}

void MuCCaFitterImplementation::Computelin(double x1, double y1, double r1, double x2, double y2, double r2)
{
/********************************************/
/* Compute the 4 lines tangent to 2 circles */
/********************************************/
   double delta;
   double averagephi,dist,dphiin,dphiex;
   double bpar[4],phi;
   double bfparn[2];
   bfparn[0]=0; bfparn[1]=0;
   double bcand[2][4];
   int segnob[]={1,-1,1,-1};
   int ncandid[4],ncand;
   int i,firsttime; 
   double angularcoefficient[4];
   double bfpar[4];
   double dy, dx;

/* compute a parameters */
   dx = x2-x1;
   dy = y2-y1;
   averagephi = std::atan2(dy,dx);
   dist = std::sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)); 

   delta = r2+r1;
   dphiin = std::asin(delta/dist);

   delta = r2-r1;
   dphiex = std::asin(delta/dist);

   int f=1;
   phi = averagephi+f*dphiex;
   if(phi < 0){phi = 2*M_PI+(phi);}
   angularcoefficient[0] = std::tan(phi);

   f=-1;
   phi = averagephi+f*dphiex;
   if(phi < 0){phi = 2*M_PI+(phi);}
   angularcoefficient[1] = std::tan(phi);

   f=1;
   phi = averagephi+f*dphiin;
   if(phi < 0){phi = 2*M_PI+(phi);}
   angularcoefficient[2] = std::tan(phi);

   f=-1;
   phi = averagephi+f*dphiin;
   if(phi < 0){phi = 2*M_PI+(phi);}
   angularcoefficient[3] = std::tan(phi);

/* Compute b parameters */
   for(i=0;i<4;i++){
     bpar[0] = y1-angularcoefficient[i]*x1+segnob[0]*r1*
               std::sqrt(1+angularcoefficient[i]*angularcoefficient[i]);  
     bpar[1] = y1-angularcoefficient[i]*x1+segnob[1]*r1*
               std::sqrt(1+angularcoefficient[i]*angularcoefficient[i]);
     bpar[2] = y2-angularcoefficient[i]*x2+segnob[2]*r2*
               std::sqrt(1+angularcoefficient[i]*angularcoefficient[i]);
     bpar[3] = y2-angularcoefficient[i]*x2+segnob[3]*r2*
               std::sqrt(1+angularcoefficient[i]*angularcoefficient[i]);
     double delta = 0.00001;
     ncand = 0;
     if(std::abs(bpar[0]-bpar[2])<delta){
       bfparn[ncand]  = bpar[0];
       ncand          = ncand+1;     
     }
     if(std::abs(bpar[0]-bpar[3])<delta){
       bfparn[ncand]  = bpar[0];
       ncand          = ncand+1;     
     }
     if(std::abs(bpar[1]-bpar[2])<delta){
       bfparn[ncand]  = bpar[1];
       ncand          = ncand+1;     
     }
     if(std::abs(bpar[1]-bpar[3])<delta){
       bfparn[ncand]  = bpar[1];
       ncand          = ncand+1;
     }
     bcand[0][i]  = bfparn[0];
     bcand[1][i]  = bfparn[1];
     ncandid[i]   = ncand;
   }
   firsttime = 0;
   for(i=0;i<4;i++){
     if(ncandid[i]==1){
       bfpar[i]   = bcand[0][i];
     }else{
       bfpar[i]   = bcand[firsttime][i];
       firsttime++;
     }
   }
/* set global variables (method output)*/
   for(int i=0; i<4; i++){
     m_bfpar[i]=bfpar[i];
     m_angularcoefficient[i]=angularcoefficient[i];
   }
}
void MuCCaFitterImplementation::Computehitpsemes(int nhit,std::vector<double> xcirc, std::vector<double> ycirc, std::vector<double>rcirc, double a, double b)
{
/************************/
/* Compute TRACK POINTS */
/************************/
    int i,nhitc;
    //    double xpoint[nhit],ypoint[nhit];
/* loop over hit couples */
    nhitc = nhit/2;
    for(i=0;i<nhitc;i++){
/* Compute TRACK POINTS for 2 circles */
       computehitsfromcircles(xcirc[2*i],ycirc[2*i],rcirc[2*i],xcirc[2*i+1],ycirc[2*i+1],rcirc[2*i+1],a,b);
       m_xpoint[2*i]=m_xout0;
       m_ypoint[2*i]=m_yout0;
       m_xpoint[2*i+1]=m_xout1;
       m_ypoint[2*i+1]=m_yout1;
    }
    if(2*nhitc!=nhit){
/* Compute TRACK POINTS for 2 circles */
       computehitsfromcircles(xcirc[nhit-2],ycirc[nhit-2],rcirc[nhit-2],xcirc[nhit-1],ycirc[nhit-1],rcirc[nhit-1],a,b);
       m_xpoint[nhit-2]=m_xout0;
       m_ypoint[nhit-2]=m_yout0;
       m_xpoint[nhit-1]=m_xout1;
       m_ypoint[nhit-1]=m_yout1;
    }
}

void MuCCaFitterImplementation::computehitsfromcircles(double x0,double y0,double r0,double x1,double y1,double r1,double a,double b)
{
/**************************************/
/* Compute TRACK POINTS for 2 circles */
/**************************************/
  double ang[4],bpar[4];
  int i;
  double xout0=0,yout0=0;
  double xout1=0,yout1=0;
  double dist,dist1,xint,yint,xref,yref;

/* Compute the 4 lines tangent to 2 circles */
  Computelin(x0,y0,r0,x1,y1,r1);
  for(i=0;i<4;i++){
    bpar[i]=m_bfpar[i];
    ang[i]=m_angularcoefficient[i];
  }

  dist = 9999999;
/* for each of the 4 tangents : */
  for(i=0;i<4;i++){
/* compute tangent point */
     xint = (x0+ang[i]*y0-ang[i]*bpar[i])/(1+ang[i]*ang[i]);
     yint = ang[i]*(xint)+bpar[i];
/* compute point from reference line */
   double bprime;
   if(a!=0){
     bprime = y0+x0/a;
     xref = (bprime-b)*a/(a*a+1);
     yref = (b+bprime*a*a)/(a*a+1);
   }else{
     xref = x0;
     yref = b; 
   }
/* choose tangent point closest to that of the reference lines (TRACK POINT)*/
   dist1 = std::sqrt((xint-xref)*(xint-xref)+(yint-yref)*(yint-yref));
   if(dist1<dist){
     dist   = dist1;
     xout0 = xint;
     yout0 = yint; 
   }
  }
  dist = 9999999;
  for(i=0;i<4;i++){
/* compute tangent point */
     xint = (x1+ang[i]*y1-ang[i]*bpar[i])/(1+ang[i]*ang[i]);
     yint = ang[i]*(xint)+bpar[i];
/* compute point from reference line */
   double bprime;
   if(a!=0){
     bprime = y1+x1/a;
     xref = (bprime-b)*a/(a*a+1);
     yref = (b+bprime*a*a)/(a*a+1);
   }else{
     xref = x1;
     yref = b; 
   }
/* choose tangent point closest to that of the reference line (TRACK POINT)*/
   dist1 = std::sqrt((xint-xref)*(xint-xref)+(yint-yref)*(yint-yref));
   if(dist1<dist){
     dist   = dist1;
     xout1 = xint;
     yout1 = yint; 
   }
  }
/* set global variables (method output)*/
  m_xout0=xout0;
  m_yout0=yout0;
  m_xout1=xout1;
  m_yout1=yout1;  
}
}
