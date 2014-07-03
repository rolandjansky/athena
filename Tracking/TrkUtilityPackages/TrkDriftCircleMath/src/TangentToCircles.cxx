/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkDriftCircleMath/TangentToCircles.h"

#include <iostream>

namespace TrkDriftCircleMath {

  
  TangentToCircles::LineVec& TangentToCircles::tangentLines( const DriftCircle& dc1, const DriftCircle& dc2 )
  {

    m_lines.clear();
    double  DeltaX       	=	dc2.x() - dc1.x();
    double  DeltaY       	=	dc2.y() - dc1.y();
    double  DistanceOfCenters	=	sqrt(DeltaX*DeltaX + DeltaY*DeltaY);
    if( DistanceOfCenters == 0. ) {
      std::cout << " TangentToCircles::tangentLines >>> ERROR same tube on input " << std::endl;
      return m_lines;
    }
    double  Alpha0       	=	atan2(DeltaY,DeltaX);
    
    // clean up lines vector
    //if( m_debug )
    //std::cout << "      calculating Lines " << dc1 << "   " << dc2 << std::endl;


    // Case of 0 drift distances, only 1 line
    if ( dc1.r() == 0. && dc2.r() == 0.) {
      //if( m_debug ) std::cout << " line pos " << dc1.position() << " phi " << Alpha0 << std::endl;
      m_lines.push_back( Line( dc1.position(), Alpha0 ) );
      return m_lines;
    }


    // Here are the first 2 "inner" lines ....
    double	RSum	=	dc1.r() + dc2.r();

    //std::cout << " DistanceOfCenters " << DistanceOfCenters << " RSum " << RSum << std::endl;

    double ratio = RSum/DistanceOfCenters;
    if( fabs(ratio) > 1. ) return m_lines;

    double	Alpha1	=	asin(ratio);

    double	line_phi	=	Alpha0 + Alpha1;
    LocPos pos1( dc1.x() + dc1.r()*sin(line_phi), dc1.y() - dc1.r()*cos(line_phi) ) ;
    m_lines.push_back( Line(pos1, line_phi) );

    //if( m_debug ) std::cout << " line pos " << pos1 << " phi " << line_phi << std::endl;

    

    line_phi	=	Alpha0 - Alpha1;
    LocPos pos2( dc1.x() - dc1.r()*sin(line_phi), dc1.y() + dc1.r()*cos(line_phi) );
    m_lines.push_back( Line(pos2, line_phi) );
   
    //if( m_debug ) std::cout << " line pos " << pos2 << " phi " << line_phi << std::endl;


    // Case where one of the drifts is 0 ==> Only two lines
    if (dc1.r() == 0. || dc2.r() == 0.)	return m_lines;
  
    // ... and here are the other 2 "outer" lines
    double	DeltaR	=	fabs(dc2.r() - dc1.r());

    ratio = DeltaR/DistanceOfCenters;
    if( fabs(ratio) > 1. ) return m_lines;

    double	Alpha2	=	asin(ratio);

    if ( dc1.r() < dc2.r() ) {
      //if( m_debug ) std::cout << "   dc1.r() < dc2.r() " << std::endl;

      line_phi	=	Alpha0 + Alpha2;
      LocPos pos3( dc1.x() - dc1.r()*sin(line_phi), dc1.y() + dc1.r()*cos(line_phi) );
      m_lines.push_back( Line(pos3, line_phi) );
      
      //if( m_debug ) std::cout << " line pos " << pos3 << " phi " << line_phi << std::endl;

      line_phi	=	Alpha0 - Alpha2;
      LocPos pos4( dc1.x() + dc1.r()*sin(line_phi), dc1.y() - dc1.r()*cos(line_phi) );
      m_lines.push_back( Line(pos4, line_phi) );

      //if( m_debug ) std::cout << " line pos " << pos4 << " phi " << line_phi << std::endl;

    } else {
      //if( m_debug ) std::cout << "   dc1.r() > dc2.r() " << std::endl;

      line_phi	=	Alpha0 + Alpha2;
      LocPos  pos3( dc1.x() + dc1.r()*sin(line_phi), dc1.y() - dc1.r()*cos(line_phi) );
      m_lines.push_back( Line(pos3, line_phi) );
 
      //if( m_debug ) std::cout << " line pos " << pos3 << " phi " << line_phi << std::endl;

      line_phi	=	Alpha0 - Alpha2;
      LocPos  pos4( dc1.x() - dc1.r()*sin(line_phi), dc1.y() + dc1.r()*cos(line_phi) );
      m_lines.push_back( Line(pos4, line_phi) );
      
      //if( m_debug ) std::cout << " line pos " << pos4 << " phi " << line_phi << std::endl;

    }
    return m_lines;
  }

}
