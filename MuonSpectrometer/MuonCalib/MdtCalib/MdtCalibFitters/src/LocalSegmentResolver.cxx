/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibFitters/LocalSegmentResolver.h"
#include "MuonCalibEventBase/MdtCalibHitBase.h"
#include "MdtCalibFitters/LocalToPrecision.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include <iostream>

namespace MuonCalib {

  LocalSegmentResolver::LocalSegmentResolver() : m_printLevel(1) {}

  bool LocalSegmentResolver::resolve(MuonCalibSegment* seg) const{
    if( !seg ){
      if( m_printLevel >= 1 ) {
        MsgStream log(Athena::getMessageSvc(),"LocalSegmentResolver");
        log<<MSG::VERBOSE<<"LocalSegmentResolver::resolve: <got nullptr>"<<endmsg;
      }
      return false;
    }
    if( seg->mdtHitsOnTrack() < 2 ) {
      if( m_printLevel >= 1 ) {
        MsgStream log(Athena::getMessageSvc(),"LocalSegmentResolver");
        log<<MSG::VERBOSE<<"LocalSegmentResolver::resolve: <to few hits, cannot resolve direction>"<<endmsg;
      }
      return false;
    }

    // get tangent lines
    LineVec lines = getLines( *seg->mdtHOT().front(), *seg->mdtHOT().back() );

    // find line which yields smallest residuals
    int lineIndex = bestLine( seg->mdtHOT(), lines );

    // check if everything went right
    if( lineIndex < 0 ) return false;

    // reset local position and direction of segment
    seg->set( seg->chi2(), MuonCalib::LocalToPrecision::local(lines[lineIndex].first), 
	      MuonCalib::LocalToPrecision::local(lines[lineIndex].second) );

    return true;
  }

  LocalSegmentResolver::LineVec LocalSegmentResolver::getLines( const MdtCalibHitBase& firstHit,
								const MdtCalibHitBase& lastHit ) const
  {
  
    Amg::Vector3D hpos1 = MuonCalib::LocalToPrecision::precision(firstHit.localPosition());
    double  x1             = hpos1.x();
    double  y1             = hpos1.y();
    double  r1             = std::abs(firstHit.driftRadius());
    Amg::Vector3D hpos2 = MuonCalib::LocalToPrecision::precision(lastHit.localPosition());
    double  x2             = hpos2.x();
    double  y2             = hpos2.y();
    double  r2             = std::abs(lastHit.driftRadius());

    double  DeltaX       		=	x2 - x1;
    double  DeltaY       		=	y2 - y1;
    double  DistanceOfCenters	=	std::hypot(DeltaX, DeltaY);
    double  Alpha0       		=	std::atan2(DeltaY,DeltaX);

    LineVec list_of_lines;
    
    if( m_printLevel >= 1 ){
      MsgStream log(Athena::getMessageSvc(),"LocalSegmentResolver");
      log<<MSG::VERBOSE<<"LocalSegmentResolver::getLines: calculating Lines (" << x1 << "," << y1 << ")  " << r1 << "       (" << x2 << "," << y2 << ")  " << r2<<endmsg;
      log<<MSG::VERBOSE<<"LocalSegmentResolver::getLines: general dir " << (hpos2-hpos1).unit()<< " calculated : " << Amg::Vector3D( std::cos(Alpha0), std::sin(Alpha0), 0. ) <<endmsg;
    }

    // Case of 0 drift distances, only 1 line
    if ( r1 == 0. && r2 == 0.) {
      Amg::Vector3D  pos = hpos1;
      Amg::Vector3D dir( std::cos(Alpha0) ,std::sin(Alpha0), 0 );
      if( m_printLevel >= 1 ) {
        MsgStream log(Athena::getMessageSvc(),"LocalSegmentResolver");
        log<<MSG::VERBOSE<<"LocalSegmentResolver::getLines: line pos " << pos << " dir " << dir <<endmsg;
      }
      list_of_lines.push_back( std::make_pair(pos, dir) );
      return list_of_lines;
    }


    // Here are the first 2 "inner" lines ....
    double	RSum	=	r1 + r2;
    double	Alpha1	=	std::asin(RSum/DistanceOfCenters);

    double	line_phi	=	Alpha0 + Alpha1;

    Amg::Vector3D  pos1( x1 + r1*std::sin(line_phi), y1 - r1*std::cos(line_phi), 0. ) ;
    Amg::Vector3D dir1( std::cos(line_phi), std::sin(line_phi), 0. );

    if( m_printLevel >= 1 ) {
      MsgStream log(Athena::getMessageSvc(),"LocalSegmentResolver");
      log<<MSG::VERBOSE<<"LocalSegmentResolver::getLines: line pos " << pos1 << " dir " << dir1 <<endmsg;
    }

    list_of_lines.push_back( std::make_pair(pos1, dir1) );
    
    line_phi	=	Alpha0 - Alpha1;
  
    Amg::Vector3D  pos2( x1 - r1*std::sin(line_phi), y1 + r1*std::cos(line_phi), 0. );
    Amg::Vector3D dir2( std::cos(line_phi), std::sin(line_phi), 0. );
			  
    if( m_printLevel >= 1 ) {
      MsgStream log(Athena::getMessageSvc(),"LocalSegmentResolver");
      log<<MSG::VERBOSE<<"LocalSegmentResolver::getLines: line pos " << pos2 << " dir " << dir2 <<endmsg;
    }

    list_of_lines.push_back( std::make_pair(pos2, dir2) );

    // Case where one of the drifts is 0 ==> Only two lines
    if (r1 == 0. || r2 == 0.)	return list_of_lines;
  
    // ... and here are the other 2 "outer" lines
    double	DeltaR	=	std::abs(r2 - r1);
    double	Alpha2	=	std::asin(DeltaR/DistanceOfCenters);

    if ( r1 < r2 ) {
      if( m_printLevel >= 1 ) {
        MsgStream log(Athena::getMessageSvc(),"LocalSegmentResolver");
        log<<MSG::VERBOSE<<"LocalSegmentResolver::getLines: r1 < r2" <<endmsg;
      }

      line_phi	=	Alpha0 + Alpha2;

      Amg::Vector3D  pos3( x1 - r1*std::sin(line_phi), y1 + r1*std::cos(line_phi), 0. );
      Amg::Vector3D dir3( std::cos(line_phi),std::sin(line_phi), 0. );

      if( m_printLevel >= 1 ) {
        MsgStream log(Athena::getMessageSvc(),"LocalSegmentResolver");
        log<<MSG::VERBOSE<<"LocalSegmentResolver::getLines: line pos " << pos3 << " dir " << dir3<<endmsg;
      }

      list_of_lines.push_back( std::make_pair(pos3, dir3) );

      line_phi	=	Alpha0 - Alpha2;

      Amg::Vector3D  pos4( x1 + r1*std::sin(line_phi), y1 - r1*std::cos(line_phi), 0. );
      Amg::Vector3D dir4( std::cos(line_phi),std::sin(line_phi), 0. );

      if( m_printLevel >= 1 ) {
        MsgStream log(Athena::getMessageSvc(),"LocalSegmentResolver");
        log<<MSG::VERBOSE<<"LocalSegmentResolver::getLines: line pos " << pos4 << " dir " << dir4<<endmsg;
      }

      list_of_lines.push_back( std::make_pair(pos4, dir4) );

    } else {
      if( m_printLevel >= 1 ) {
        MsgStream log(Athena::getMessageSvc(),"LocalSegmentResolver");
        log<<MSG::VERBOSE<<"LocalSegmentResolver::getLines: r1 > r2" <<endmsg;
      }

      line_phi	=	Alpha0 + Alpha2;

      Amg::Vector3D  pos3( x1 + r1*std::sin(line_phi), y1 - r1*std::cos(line_phi), 0. );
      Amg::Vector3D dir3( std::cos(line_phi),std::sin(line_phi), 0. );

      if( m_printLevel >= 1 ) {
        MsgStream log(Athena::getMessageSvc(),"LocalSegmentResolver");
        log<<MSG::VERBOSE<<"LocalSegmentResolver::getLines: line pos " << pos3 << " dir " << dir3<<endmsg;
      }

      list_of_lines.push_back( std::make_pair(pos3, dir3) );

      line_phi	=	Alpha0 - Alpha2;
    
      Amg::Vector3D  pos4( x1 - r1*std::sin(line_phi), y1 + r1*std::cos(line_phi), 0. );
      Amg::Vector3D dir4( std::cos(line_phi),std::sin(line_phi), 0. );

      if( m_printLevel >= 1 ) {
        MsgStream log(Athena::getMessageSvc(),"LocalSegmentResolver");
        log<<MSG::VERBOSE<<"LocalSegmentResolver::getLines: line pos " << pos4 << " dir " << dir4<<endmsg;
      }

      list_of_lines.push_back( std::make_pair(pos4, dir4) );
    }

    return list_of_lines;
  }

  int LocalSegmentResolver::bestLine( const LocalSegmentResolver::HitVec& hits,
				      const LocalSegmentResolver::LineVec& localTracks ) const
  {
    // loop over tangent lines
    LineVec::const_iterator lit     = localTracks.begin(); 
    LineVec::const_iterator lit_end = localTracks.end(); 

    // look for line with smallest residual sum
    double ressummin = 1e20;
    unsigned int resnum = 0;

    for( ; lit!=lit_end;++lit ){

      double ressum=0;

      // calculate angle between line and z precision plane
      double alpha = std::atan2(lit->second.y(),lit->second.x());

      // get rotations in track frame
      Amg::AngleAxis3D rotationAroundZ(-alpha,Amg::Vector3D::UnitZ());
      //Amg::AngleAxis3D rotationAroundZ_inv(alpha,Amg::Vector3D::UnitZ());

      // rotate position on track into track frame
      Amg::Vector3D avePosTrk = rotationAroundZ*lit->first;
    
      if( m_printLevel >= 1 ) {
        Amg::Vector3D lTrkDir = rotationAroundZ*lit->second;
        MsgStream log(Athena::getMessageSvc(),"LocalSegmentResolver");
        log<<MSG::VERBOSE<<"   angle " << alpha*57.32 << " trk dir in trk frame " << lTrkDir << " pos " << avePosTrk<<endmsg;
      }
      // loop over local hits
      LocalSegmentResolver::HitVec::const_iterator it = hits.begin();
      LocalSegmentResolver::HitVec::const_iterator it_end = hits.end();
      for( ; it!=it_end; ++it ){
      
	// rotate into track system
	Amg::Vector3D spos = MuonCalib::LocalToPrecision::precision((*it)->localPosition());
	Amg::Vector3D sposAve = rotationAroundZ*spos - avePosTrk;
      
	// get drift radius (unsigned)
	double r = std::abs( (*it)->driftRadius() );
      
	// calculate residual and pull
	double res  = r - std::abs( sposAve.y() );

	if( m_printLevel >= 1 ) {
    MsgStream log(Athena::getMessageSvc(),"LocalSegmentResolver");
    log<<MSG::VERBOSE<<" r " << r << " r_trk " << std::abs( sposAve.y() ) << " residual " << res<<endmsg;
  }
      
	ressum += res*res;
      }  

      if( m_printLevel >= 3 ) {
        MsgStream log(Athena::getMessageSvc(),"LocalSegmentResolver");
        log<<MSG::INFO<<"     line " << lit-localTracks.begin() << " residual sum " << ressum<<endmsg;
      }
      if( ressum < ressummin ) {
	ressummin = ressum;
	resnum    = lit-localTracks.begin();
      }
    }

    if( m_printLevel >= 3 ) {
      MsgStream log(Athena::getMessageSvc(),"LocalSegmentResolver");
      log<<MSG::INFO<<"        Done selected line: ressum " <<  ressummin << " ## " << resnum<<endmsg;
      log<<MSG::INFO<<"        Position " << localTracks[resnum].first << "   direction " << localTracks[resnum].second<<endmsg;
    }
    if( resnum >= localTracks.size() ) {
      MsgStream log(Athena::getMessageSvc(),"LocalSegmentResolver");
      log<<MSG::WARNING<<"wrong line index"<<endmsg;
      return -1;
    }
    
    return resnum;
  }

}
