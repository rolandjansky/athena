/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkDriftCircleMath/DCSLFitter.h"

#include <iostream>

namespace TrkDriftCircleMath {

  DCSLFitter::DCSLFitter() : m_result( Line(0.,0.,0.), DCOnTrackVec() ), m_debug(false) {

    // reserve enough space for hits
    m_result.dcs().reserve(100);
    m_data.reserve(100);
  }
 
  bool DCSLFitter::fit( const Line& line, const DCOnTrackVec& dcs, const HitSelection& selection, double ) const
  {
    if(m_debug) std::cout << "New seg: " << std::endl; //<< seg;

    unsigned int N = dcs.size();

    if(N<2) {
      return false;
    }

    if( selection.size() != N ) {
      std::cout << "DCSLFitter <bad HitSelection>" << std::endl;
      return false;
    }else{
      int used(0);
      for(unsigned int i=0;i<N;++i){
	if( selection[i] == 0 ) ++used;
      }
      if(used < 2){
	if(m_debug) std::cout << "TOO FEW HITS SELECTED" << std::endl;
	return false;
      }
    }

    double S(0),Sz(0),Sy(0);
    double Zc(0),Yc(0);
    m_data.resize(N);
    {
      
      
      int ii(0);

      DCOnTrackVec::const_iterator it = dcs.begin();
      DCOnTrackVec::const_iterator it_end = dcs.end();
      for( ;it!=it_end; ++it ){
	
	FitData& data = m_data[ii];
	data.y = it->y();
	data.z = it->x();
	data.r = std::abs( it->r() );
	if( it->dr() > 0.)
	  data.w = 1./( it->dr() );
	else
	  data.w = 0.;
	data.w*=data.w;
	if(data.r<0){
	  data.r = 0.;
	  if(m_debug) std::cout << "DCSLFitter ERROR: <Negative r> " << data.r << std::endl;
	}
	if(m_debug)
	  std::cout << "DC:  (" << data.y << "," << data.z << ")  R = " << data.r
		    << " W " << data.w << " sel " << selection[ii] << " tube="<<it->id().tube()<<std::endl;

	if( selection[ii] == 0 ){
	  S+=data.w;
	  Sz+= data.w*data.z;
	  Sy+= data.w*data.y;
	}
	++ii;
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
  
    for(unsigned int i=0;i<N;++i){

      FitData& data = m_data[i];

      data.y  -= Yc;
      data.z  -= Zc;

      if(selection[i]) continue;

      data.rw  = data.r*data.w;
      data.ryw = data.rw*data.y;
      data.rzw = data.rw*data.z;

      Szz   += data.z*data.z*data.w;
      Syy   += data.y*data.y*data.w;
      Szy   += data.y*data.z*data.w;
      Syyzz += (data.y-data.z)*(data.y+data.z)*data.w;
    }

    if(m_debug)
      std::cout << " Szz " << Szz << " Syy " << Syy 
		<< " Szy " << Szy << " Syyzz " << Syyzz << std::endl; 
  
    unsigned int count(0);
    double R(0),Ry(0),Rz(0);
    double Att(0);
    double Add = S;
    double Bt(0);
    double Bd(0);
    double Stt(0);
    double Sd(0);
  
    double theta = line.phi();
    double cosin = cos( theta );
    double sinus = sin( theta );

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
    //    double d = -( getZ( pos )-Zc)*sinus+( getY( pos )-Yc)*cosin;
    double dshift = Zc*sinus-Yc*cosin;
    if(m_debug) {
      std::cout << " dshift " << dshift << std::endl;
    }
    double d = line.y0() + Zc*sinus-Yc*cosin;

    if(m_debug) {
      std::cout << "____________INITIAL VALUES________________" << count << std::endl;
      std::cout << "Theta " << theta << " d " << d << std::endl;
    }

    while(count<100){
      if(m_debug) std::cout << "____________NEW ITERATION________________" << count << std::endl;
      R=0;Ry=0;Rz=0;
      
      double chi2(0);

      for(unsigned int i=0;i<N;++i){
	if(selection[i]) continue;
	
	FitData& data = m_data[i];
	
	double dist = data.y*cosin-data.z*sinus;
	if(dist>d){
	  R  -= data.rw;
	  Ry -= data.ryw;
	  Rz -= data.rzw;
	  if(m_debug) {
	    double res = std::abs(dist-d) - data.r;
	    std::cout << " < - > " << dist - d << " r " << data.r 
		      << " res " << res;
	    chi2 += res*res*data.w*data.w; 
	  }
	}else{
	  R  += data.rw;
	  Ry += data.ryw;
	  Rz += data.rzw;
	  if(m_debug) {
	    double res = std::abs(dist-d) - data.r;
	    std::cout << " < + > " << dist - d << " r " << data.r 
		      << " res " << res;
	    chi2 += res*res*data.w; 
	  }
	}
	if(m_debug) std::cout << std::endl;
      }
      Att = Syy + cosin*(2*sinus*Szy - cosin*Syyzz);
      Bt = -Szy + cosin*(sinus*Syyzz + 2*cosin*Szy + Rz) + sinus*Ry;
      Bd = -S*d + R;
      if(Att==0){
	if(m_debug) std::cerr << "===> Error NewtonSLDCFitter ZERO Determinant" << std::endl;
	if( m_data.capacity() > 100 ) {
	  m_data.reserve(100);
	  m_result.dcs().reserve(100);
	}
	return false;
      }
      theta += Bt/Att;
      if(m_debug) std::cout << " new theta " << theta << " chi2 " << chi2 << std::endl;
      if ( theta < 0.0   ) theta += M_PI;
      if ( theta >= M_PI ) theta -= M_PI;
      cosin = cos(theta);
      sinus = sqrt(1-cosin*cosin);
      d = R/S;
      if(m_debug) { 
	double dshift = Zc*sinus-Yc*cosin;
	std::cout << " d " << d << " theta " << theta 
		  << " dshift " << dshift << std::endl;
 	std::cout << "R " << R << " Ry " << Ry << " Rz " << Rz << std::endl;
	std::cout << "Att " << Att << " Add " << Add << " Bt " << Bt << " Bd " << Bd << std::endl;
	std::cout << "dTheta " << Bt/Att << " dD " << Bd/Add << std::endl;
      }
      if(fabs(Bt/Att) < 0.001 && fabs(Bd/Add) < 0.001){
	Stt = sqrt(1/Att);
	Sd = sqrt(1/Add);
	if(m_debug) {
	  std::cout << "Fit converged after " << count << " iterations " << std::endl;
	  std::cout << "Theta " << theta << " d " << d << std::endl;
	  std::cout << "Errors: theta " << Stt << " d " << Sd << std::endl;
	}
	break;
      }
      ++count;
    }
    
    if( count >= 100 ){
      if( m_data.capacity() > 100 ) {
	m_result.dcs().reserve(100);
	m_data.reserve(100);
      }
      return false;
    }
    if(m_debug) std::cout << "Calculating chi2" << std::endl;

    double yl;

    double chi2 = 0;
    
    double dtheta = Stt;
    double dy0 = Sd;

    if(m_debug) std::cout << "contributions to chi2: " << std::endl;

    m_result.dcs().clear();
    m_result.clusters().clear();
    m_result.emptyTubes().clear();

    unsigned int nhits(0);

    // calculate predicted hit positions from track parameters
    for(unsigned int i=0;i<N;++i){
      FitData& data = m_data[i];
      yl = cosin*data.y - sinus*data.z - d;
      double dth = -(sinus*data.y + cosin*data.z)*Stt;
      double errorResiduals = sqrt( dth*dth + Sd*Sd );
      double residuals = fabs(yl) - data.r;
      if( selection[i] == 0 ) {
	++nhits;
	chi2 += residuals*residuals*data.w;
      }
      m_result.dcs().push_back(dcs[i]);
      m_result.dcs().back().residual( residuals );
      m_result.dcs().back().errorTrack( errorResiduals );
      
      if( m_debug ){
	  std::cout << " r_track " << yl << " dr " << errorResiduals
		    << " r_rt " << data.r << " d_rt " <<  dcs[i].dr()
		    << " res " << residuals << " pull "  << residuals*sqrt(data.w)
		    << " sel " << selection[i] << std::endl;
      }

    }

    m_result.set( chi2, nhits-2, dtheta, dy0 );
    m_result.line().set( LocPos( Zc - sinus*d, Yc + cosin*d ), theta );

    
    if(m_debug) {
      std::cout << "Fit complete: Chi2 tof " << chi2/(nhits-2)
			  << " " << !(chi2/(nhits-2) > 5) << std::endl;
      if(chi2/(nhits-2) > 5) {
	std::cout << "_______NOT GOOD " << std::endl;
      }
      std::cout << "fit done " << m_result << std::endl;
    }

    if( m_data.capacity() > 100 ) {
      m_result.dcs().reserve(100);
      m_data.reserve(100);
    }
    return true;
  }
}

