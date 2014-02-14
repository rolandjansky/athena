/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibFitters/DCSLFitter.h"
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
    if(m_debug) std::cout << "New seg: " << std::endl; //<< seg;


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
	std::cout << "TO FEW HITS SELECTED" << std::endl;
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
	  std::cout << "DCSLFitter ERROR: <Negative r> " << r[ii] << std::endl;
	}
	if(m_debug)
	  std::cout << "DC:  (" << y[ii] << "," << z[ii] << ")  R = " << r[ii]
		    << " W " << w[ii] << std::endl;

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
  
    if(m_debug)
      std::cout << "Yc " << Yc << " Zc " << Zc << std::endl;
    //
    //    shift hits
    //
    //std::cout << "Shifted d " << d << std::endl;
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

    if(m_debug)
      std::cout << " Szz " << Szz << " Syy " << Syy 
		<< " Szy " << Szy << " Syyzz " << Syyzz << std::endl; 
  
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

    if(m_debug) 
      std::cout << "cos " << cosin << " sin " << sinus << std::endl;

    // make sure 0 <= theta < PI
    if ( sinus < 0.0 ) {
      if(m_debug) {
	std::cout << "sinus < 0.0 " << std::endl;
      }     
      sinus = -sinus;
      cosin = -cosin;
    } else if ( sinus == 0.0 && cosin < 0.0 ) {
      if(m_debug) {
	std::cout << "sinus == 0.0 && cosin < 0.0" << std::endl;
      }     
      cosin = -cosin;
    }
    //
    // calculate shift
    //
    double d = -( getZ( pos )-Zc)*sinus+( getY( pos )-Yc)*cosin;
    double theta = acos(cosin);
    if(m_debug) {
      std::cout << "____________INITIAL VALUES________________" << count << std::endl;
      std::cout << "Theta " << theta << " d " << d << std::endl;
    }

    while(count<100){
      if(m_debug) std::cout << "____________NEW ITERATION________________" << count << std::endl;
      R=0;Ry=0;Rz=0;
      for(int i=0;i<N;++i){
	if(selection[i]) continue;

	double dist = y[i]*cosin-z[i]*sinus;
	if(dist>d){
	  R  -= rw[i];
	  Ry -= ryw[i];
	  Rz -= rzw[i];
	  if(m_debug) std::cout << " < - > " << dist - d << " r " << r[i];
	}else{
	  R  += rw[i];
	  Ry += ryw[i];
	  Rz += rzw[i];
	  if(m_debug) std::cout << " < + > " << dist - d << " r " << r[i];
	}
      }
      if(m_debug) std::cout << std::endl;
      Att = Syy + cosin*(2*sinus*Szy - cosin*Syyzz);
      Bt = -Szy + cosin*(sinus*Syyzz + 2*cosin*Szy + Rz) + sinus*Ry;
      Bd = -S*d + R;
      if(Att==0){
	std::cerr << "===> Error NewtonSLDCFitter ZERO Determinant" << std::endl;
	return false;
      }
      theta += Bt/Att;
      if ( theta < 0.0   ) theta += M_PI;
      if ( theta >= M_PI ) theta -= M_PI;
      cosin = cos(theta);
      sinus = sqrt(1-cosin*cosin);
      d = R/S;
      if(m_debug) std::cout << "R " << R << " Ry " << Ry << " Rz " << Rz << std::endl;
      if(m_debug) std::cout << "Att " << Att << " Add " << Add << " Bt " << Bt << " Bd " << Bd << std::endl;
      if(m_debug) std::cout << "dTheta " << Bt/Att << " dD " << Bd/Add << std::endl;
      if(fabs(Bt/Att) < 0.001 && fabs(Bd/Add) < 0.001){
	Stt = sqrt(1/Att);
	Sd = sqrt(1/Add);
	if(m_debug) std::cout << "Fit converged after " << count << " iterations " << std::endl;

	if(m_debug) std::cout << "Theta " << theta << "d " << d << std::endl;
	if(m_debug) std::cout << "Errors: theta " << Stt << " d " << Sd << std::endl;

//        error_dy0 = Sd;
//        error_dtheta = Stt;
        seg.setErrors(Sd,Stt);

	break;
      }
      ++count;
    }
    if(m_debug) std::cout << "Calculating chi2" << std::endl;

    std::vector< double > yl(N);
    std::vector< double > dyl(N);
    std::vector< double > res(N);
    double chi2(0);
    if(m_debug) std::cout << "contributions to chi2: " << std::endl;

    // calculate predicted hit positions from track parameters
    for(int i=0;i<N;++i){
      yl[i] = cosin*y[i] - sinus*z[i] - d;
      double dth = -(sinus*y[i] + cosin*z[i])*Stt;
      dyl[i] = sqrt( dth*dth + Sd*Sd );
      res[i] = fabs(yl[i]) - r[i];
      if(m_debug) std::cout << " r_track " << yl[i] << " dr " << dyl[i]
			    << " r_rt " << r[i] << " res " << res[i]
			    << " pull " << res[i]*res[i]*w[i] << std::endl;
      //skip hits that are not used in fit
      if(selection[i]) continue;

      chi2 += res[i]*res[i]*w[i];

    }
  
    // std::cout << " Chi2 = " << chi2 << " chi2 per dof " << chi2/(N-2) << std::endl;

    if(m_debug) std::cout << "Fit complete: Chi2 tof " << chi2/(N-2)
			  << " " << !(chi2/(N-2) > 5) << std::endl;
    if(chi2/(N-2) > 5) {
      if(m_debug) {
	std::cout << "_______NOT GOOD " << std::endl;
      }
    }

    if(m_debug) std::cout << "Transforming back to real world" << std::endl;

    Amg::Vector3D ndir = getVec( dir.x() , sinus, cosin );
    Amg::Vector3D npos = getVec( pos.x() , Yc + cosin*d, Zc - sinus*d );

    if(m_debug) std::cout << "New line: position " << npos << " direction " << ndir << std::endl;

    seg.set( chi2/(N-2), npos, ndir );

    MuonCalibSegment::MdtHitIt it = seg.mdtHOTBegin();
  
    int i(0);
    while(it!=seg.mdtHOTEnd()){
      MdtCalibHitBase& hit = const_cast< MdtCalibHitBase& >( **it );

      hit.setDistanceToTrack( yl[i], dyl[i] );
    
      ++it;++i;
    }

    if(m_debug) std::cout << "fit done" << std::endl;
 //tracking failed if position and directins are not real numbers
    return (std::isfinite(ndir.y()) && std::isfinite(ndir.z()) && std::isfinite(npos.y()) && std::isfinite(npos.z()));
  }

}
