/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibFitters/DCSLFitter.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include "cmath"

namespace MuonCalib {

  void DCSLFitter::printLevel(int level) {
    if(level > 0 ) m_debug = true;
  }

  bool DCSLFitter::fit( MuonCalibSegment& seg ) const
  {
    // select all hits
    HitSelection selection(seg.mdtHitsOnTrack(),0);

    // call fit function
    return fit(seg,selection);
  }
  

  bool DCSLFitter::fit( MuonCalibSegment& seg, HitSelection selection ) const
  {
    if(m_debug) {
      MsgStream log(Athena::getMessageSvc(),"DCSLFitter");
      log<<MSG::DEBUG<<"New seg: "<<endmsg;
    }

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
        MsgStream log(Athena::getMessageSvc(),"DCSLFitter");
        log<<MSG::WARNING<<"TO FEW HITS SELECTED"<<endmsg;
        return false;
      }
    }

    Amg::Vector3D pos = seg.position(); 
    Amg::Vector3D dir = seg.direction(); 

    double S(0),Sz(0),Sy(0);
    double Zc(0),Yc(0);

    std::vector<double> y(N);
    std::vector<double> z(N);
    std::vector<double> r(N);
    std::vector<double> w(N);
    {
      MuonCalibSegment::MdtHitIt hit = seg.mdtHOTBegin();

      int ii(0);
      while(hit != seg.mdtHOTEnd()){
	const MdtCalibHitBase& h = **hit;
      
	y[ii] = getY( h.localPosition() );
	z[ii] = getZ( h.localPosition() );
	r[ii] = std::abs( h.driftRadius() );
	if(h.sigma2DriftRadius() > 0)
	  w[ii] = 1./( h.sigma2DriftRadius() );
	else
	  w[ii] = 0.;
	if(r[ii]<0){
	  r[ii] = 0.;
    MsgStream log(Athena::getMessageSvc(),"DCSLFitter");
    log<<MSG::WARNING<<"<Negative r> " << r[ii]<<endmsg;
	}
	if(m_debug) {
    MsgStream log(Athena::getMessageSvc(),"DCSLFitter");
    log<<MSG::DEBUG<<"DC:  (" << y[ii] << "," << z[ii] << ")  R = " << r[ii] << " W " << w[ii]<<endmsg;
  }

	if(selection[ii]){
	  ++hit;++ii;
	  continue;
	}
	S+=w[ii];
	Sz+= w[ii]*z[ii];
	Sy+= w[ii]*y[ii];
	++hit;++ii;
      }
    }
    Zc = Sz/S;
    Yc = Sy/S;
  
    if(m_debug) {
      MsgStream log(Athena::getMessageSvc(),"DCSLFitter");
      log<<MSG::DEBUG<<"Yc " << Yc << " Zc " << Zc<<endmsg;
    }
    //
    //    shift hits
    //
    Sy = 0;
    Sz = 0;
    double Szz(0),Syy(0),Szy(0),Syyzz(0);
    std::vector<double> rw(N);
    std::vector<double> ryw(N);
    std::vector<double> rzw(N);
  
    for(int i=0;i<N;++i){

      y[i]  -= Yc;
      z[i]  -= Zc;

      if(selection[i]) continue;

      rw[i]  = r[i]*w[i];
      ryw[i] = rw[i]*y[i];
      rzw[i] = rw[i]*z[i];

      Szz   += z[i]*z[i]*w[i];
      Syy   += y[i]*y[i]*w[i];
      Szy   += y[i]*z[i]*w[i];
      Syyzz += (y[i]-z[i])*(y[i]+z[i])*w[i];
    }

    if(m_debug) {
      MsgStream log(Athena::getMessageSvc(),"DCSLFitter");
      log<<MSG::DEBUG<<" Szz " << Szz << " Syy " << Syy << " Szy " << Szy << " Syyzz " << Syyzz<<endmsg;
    }
  
    int count(0);
    double R(0),Ry(0),Rz(0);
    double Att(0);
    double Add = S;
    double Bt(0);
    double Bd(0);
    double Stt(0);
    double Sd(0);
  
    double cosin = getZ( dir )/dir.mag();
    double sinus = getY( dir )/dir.mag();

    if(m_debug) {
      MsgStream log(Athena::getMessageSvc(),"DCSLFitter");
      log<<MSG::DEBUG<<"cos " << cosin << " sin " << sinus<<endmsg;
    }

    // make sure 0 <= theta < PI
    if ( sinus < 0.0 ) {
      if(m_debug) {
        MsgStream log(Athena::getMessageSvc(),"DCSLFitter");
        log<<MSG::DEBUG<<"sinus=" << sinus<<endmsg;
      }     
      sinus = -sinus;
      cosin = -cosin;
    } else if ( sinus == 0.0 && cosin < 0.0 ) {
      if(m_debug) {
        MsgStream log(Athena::getMessageSvc(),"DCSLFitter");
        log<<MSG::DEBUG<<"sinus == 0.0 && cosin < 0.0"<<endmsg;
      }     
      cosin = -cosin;
    }
    //
    // calculate shift
    //
    double d = -( getZ( pos )-Zc)*sinus+( getY( pos )-Yc)*cosin;
    double theta = std::acos(cosin);
    if(m_debug) {
      MsgStream log(Athena::getMessageSvc(),"DCSLFitter");
      log<<MSG::DEBUG<<"____________INITIAL VALUES________________" << count<<endmsg;
      log<<MSG::DEBUG<<"Theta " << theta << " d " << d << count<<endmsg;
    }

    while(count<100){
      if(m_debug) {
        MsgStream log(Athena::getMessageSvc(),"DCSLFitter");
        log<<MSG::DEBUG<<"____________NEW ITERATION________________" << count<<endmsg;
      }
      R=0;Ry=0;Rz=0;
      for(int i=0;i<N;++i){
	if(selection[i]) continue;

	double dist = y[i]*cosin-z[i]*sinus;
	if(dist>d){
	  R  -= rw[i];
	  Ry -= ryw[i];
	  Rz -= rzw[i];
	  if(m_debug) {
      MsgStream log(Athena::getMessageSvc(),"DCSLFitter");
      log<<MSG::DEBUG<<" < - > " << dist - d << " r " << r[i]<<endmsg;
    }
	}else{
	  R  += rw[i];
	  Ry += ryw[i];
	  Rz += rzw[i];
	  if(m_debug) {
      MsgStream log(Athena::getMessageSvc(),"DCSLFitter");
      log<<MSG::DEBUG<<" < + > " << dist - d << " r " << r[i]<<endmsg;
    }
	}
      }
      Att = Syy + cosin*(2*sinus*Szy - cosin*Syyzz);
      Bt = -Szy + cosin*(sinus*Syyzz + 2*cosin*Szy + Rz) + sinus*Ry;
      Bd = -S*d + R;
      if(Att==0){
        MsgStream log(Athena::getMessageSvc(),"DCSLFitter");
        log<<MSG::WARNING<<"NewtonSLDCFitter ZERO Determinant"<<endmsg;
        return false;
      }
      theta += Bt/Att;
      if ( theta < 0.0   ) theta += M_PI;
      if ( theta >= M_PI ) theta -= M_PI;
      cosin = std::cos(theta);
      sinus = std::sqrt(1-cosin*cosin);
      d = R/S;
      if(m_debug) {
        MsgStream log(Athena::getMessageSvc(),"DCSLFitter");
        log<<MSG::DEBUG<<"R " << R << " Ry " << Ry << " Rz " << Rz<<endmsg;
        log<<MSG::DEBUG<<"Att " << Att << " Add " << Add << " Bt " << Bt << " Bd " << Bd<<endmsg;
        log<<MSG::DEBUG<<"dTheta " << Bt/Att << " dD " << Bd/Add<<endmsg;
      }
      if(std::abs(Bt/Att) < 0.001 && std::abs(Bd/Add) < 0.001){
	Stt = std::sqrt(1/Att);
	Sd = std::sqrt(1/Add);

	if(m_debug) {
    MsgStream log(Athena::getMessageSvc(),"DCSLFitter");
    log<<MSG::DEBUG<<"Fit converged after " << count << " iterations "<<endmsg;
    log<<MSG::DEBUG<<"Theta " << theta << "d " << d<<endmsg;
    log<<MSG::DEBUG<<"Errors: theta " << Stt << " d " << Sd<<endmsg;
  }

        seg.setErrors(Sd,Stt);

	break;
      }
      ++count;
    }
    if(m_debug) {
      MsgStream log(Athena::getMessageSvc(),"DCSLFitter");
      log<<MSG::DEBUG<<"Calculating chi2"<<endmsg;
    }

    std::vector< double > yl(N);
    std::vector< double > dyl(N);
    std::vector< double > res(N);
    double chi2(0);
    if(m_debug) {
      MsgStream log(Athena::getMessageSvc(),"DCSLFitter");
      log<<MSG::DEBUG<<"contributions to chi2: "<<endmsg;
    }

    // calculate predicted hit positions from track parameters
    for(int i=0;i<N;++i){
      yl[i] = cosin*y[i] - sinus*z[i] - d;
      double dth = -(sinus*y[i] + cosin*z[i])*Stt;
      dyl[i] = std::hypot(dth, Sd);
      res[i] = std::abs(yl[i]) - r[i];
      if(m_debug) {
        MsgStream log(Athena::getMessageSvc(),"DCSLFitter");
        log<<MSG::DEBUG<<" r_track " << yl[i] << " dr " << dyl[i]<< " r_rt " << r[i] << " res " << res[i]<< " pull " << res[i]*res[i]*w[i]<<endmsg;
        }
      //skip hits that are not used in fit
      if(selection[i]) continue;

      chi2 += res[i]*res[i]*w[i];

    }

    if(m_debug) {
      MsgStream log(Athena::getMessageSvc(),"DCSLFitter");
      log<<MSG::DEBUG<<"Fit complete: Chi2 tof " << chi2/(N-2)<< " " << !(chi2/(N-2) > 5)<<endmsg;
      }
    if(chi2/(N-2) > 5) {
      if(m_debug) {
        MsgStream log(Athena::getMessageSvc(),"DCSLFitter");
        log<<MSG::DEBUG<<"_______NOT GOOD "<<endmsg;
      }
    }

    if(m_debug) {
      MsgStream log(Athena::getMessageSvc(),"DCSLFitter");
      log<<MSG::DEBUG<<"Transforming back to real world"<<endmsg;
    }

    Amg::Vector3D ndir = getVec( dir.x() , sinus, cosin );
    Amg::Vector3D npos = getVec( pos.x() , Yc + cosin*d, Zc - sinus*d );

    if(m_debug) {
      MsgStream log(Athena::getMessageSvc(),"DCSLFitter");
      log<<MSG::DEBUG<<"New line: position " << npos << " direction " << ndir<<endmsg;
    }

    seg.set( chi2/(N-2), npos, ndir );

    MuonCalibSegment::MdtHitIt it = seg.mdtHOTBegin();
  
    int i(0);
    while(it!=seg.mdtHOTEnd()){
      MdtCalibHitBase& hit = const_cast< MdtCalibHitBase& >( **it );

      hit.setDistanceToTrack( yl[i], dyl[i] );
    
      ++it;++i;
    }

    if(m_debug) {
      MsgStream log(Athena::getMessageSvc(),"DCSLFitter");
      log<<MSG::DEBUG<<"fit done"<<endmsg;
    }
 //tracking failed if position and directins are not real numbers
    return (std::isfinite(ndir.y()) && std::isfinite(ndir.z()) && std::isfinite(npos.y()) && std::isfinite(npos.z()));
  }

}
