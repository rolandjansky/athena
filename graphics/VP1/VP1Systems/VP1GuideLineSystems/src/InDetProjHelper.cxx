/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class InDetProjHelper               //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1GuideLineSystems/InDetProjHelper.h"
#include "VP1GuideLineSystems/InDetProjParams.h"
#include "VP1Base/VP1Msg.h"

#include "GeoPrimitives/GeoPrimitives.h"

#include "GaudiKernel/SystemOfUnits.h"

//Fixme: Epsilon in projections! (at least take surface thickness into account!)

//____________________________________________________________________
InDetProjHelper * InDetProjHelper::createPixelHelper( IVP1System* system )
{
  return new InDetProjHelper( InDetProjParams::surfacethickness(),
			      InDetProjParams::pixel_data_disttosurface_epsilon(),
			      InDetProjParams::pixel_barrel_inner_radius(),
			      InDetProjParams::pixel_barrel_outer_radius(),
			      InDetProjParams::pixel_barrel_posneg_z(),
			      InDetProjParams::pixel_endcap_surface_z(),
			      InDetProjParams::pixel_endcap_surface_length(),
			      InDetProjParams::pixel_endcap_inner_radius(),
			      InDetProjParams::pixel_endcap_outer_radius(),
			      InDetProjParams::pixel_endcap_zasr_innerradius(),
			      InDetProjParams::pixel_endcap_zasr_endcapz_begin(),
			      InDetProjParams::pixel_endcap_zasr_squeezefact(),
			      system );
}

//____________________________________________________________________
InDetProjHelper * InDetProjHelper::createSCTHelper(IVP1System* system )
{
  return new InDetProjHelper( InDetProjParams::surfacethickness(),
			      InDetProjParams::sct_data_disttosurface_epsilon(),
			      InDetProjParams::sct_barrel_inner_radius(),
			      InDetProjParams::sct_barrel_outer_radius(),
			      InDetProjParams::sct_barrel_posneg_z(),
			      InDetProjParams::sct_endcap_surface_z(),
			      InDetProjParams::sct_endcap_surface_length(),
			      InDetProjParams::sct_endcap_inner_radius(),
			      InDetProjParams::sct_endcap_outer_radius(),
			      InDetProjParams::sct_endcap_zasr_innerradius(),
			      InDetProjParams::sct_endcap_zasr_endcapz_begin(),
			      InDetProjParams::sct_endcap_zasr_squeezefact(),
			      system );
}

//____________________________________________________________________
InDetProjHelper * InDetProjHelper::createTRTHelper(IVP1System* system )
{
  return new InDetProjHelper( InDetProjParams::surfacethickness(),
			      InDetProjParams::trt_data_disttosurface_epsilon(),
			      InDetProjParams::trt_barrel_inner_radius(),
			      InDetProjParams::trt_barrel_outer_radius(),
			      InDetProjParams::trt_barrel_posneg_z(),
			      InDetProjParams::trt_endcap_surface_z(),
			      InDetProjParams::trt_endcap_surface_length(),
			      InDetProjParams::trt_endcap_inner_radius(),
			      InDetProjParams::trt_endcap_outer_radius(),
			      InDetProjParams::trt_endcap_zasr_innerradius(),
			      InDetProjParams::trt_endcap_zasr_endcapz_begin(),
			      InDetProjParams::trt_endcap_zasr_squeezefact(),
			      system );
}

//____________________________________________________________________
class InDetProjHelper::Imp {
public:
  InDetProjHelper * theclass;

  //Applicable projections:
  InDetProjFlags::InDetProjPartsFlags parts;

  //The parameters:
  double surfacethickness;
  double data_disttosurface_epsilon;
  double barrel_inner_radius;
  double barrel_outer_radius;
  double barrel_posneg_z;
  double endcap_surface_z;
  double endcap_surface_length;
  double endcap_inner_radius;
  double endcap_outer_radius;
  double endcap_zasr_innerradius;
  double endcap_zasr_endcapz_begin;
  double endcap_zasr_squeezefact;

  //Parameters of maximal cylinder covering all enabled parts of
  //detector:
  double covercyl_zmin;
  double covercyl_zmax;
  double covercyl_rmin;
  double covercyl_rmax;

//   //Helper methods:
  void lineCircleIntersection( const Amg::Vector3D&a, const Amg::Vector3D&b,const double& r,
			       double & u1, double& u2 ) const;

  //Clip segments to cylinders and planes:
  void movePoint1ToZPlaneAndPoint2( Amg::Vector3D& p1, const Amg::Vector3D& p2, const double& z ) const;
  bool clipSegmentToZInterval( Amg::Vector3D&a, Amg::Vector3D&b, const double& zmin, const double& zmax ) const;
  void movePoint1ToInfiniteCylinderAndPoint2( Amg::Vector3D&p1, const Amg::Vector3D&p2, const double& r ) const;
  bool clipSegmentToInfiniteHollowCylinder( Amg::Vector3D&a, Amg::Vector3D&b,
					    const double& rmin, const double& rmax,
					    Amg::Vector3D&seg2_a, Amg::Vector3D&seg2_b ) const;

  bool clipSegmentToHollowCylinder( Amg::Vector3D&a, Amg::Vector3D&b,
				    const double& rmin, const double& rmax,
				    const double& zmin, const double& zmax,
				    Amg::Vector3D&seg2_a, Amg::Vector3D&seg2_b ) const;

  void clipPathToHollowCylinder( const std::vector<Amg::Vector3D >& in,
//				 Amg::SetVectorVector3D& out,//<- where clipped pieces of the paths will be appended.
		         Amg::SetVectorVector3D& out,//<- where clipped pieces of the paths will be appended.
				 const double& rmin, const double& rmax,
				 const double& zmin, const double& zmax ) const;

  bool touchesHollowCylinder( const std::vector<Amg::Vector3D >& path,
			      const double& rmin, const double& rmax,
			      const double& zmin, const double& zmax ) const;
  //Project points to cylinders and planes:
  void projectPathToInfiniteCylinder( const std::vector<Amg::Vector3D >& in,
				      Amg::SetVectorVector3D& outset, const double& r  ) const;
  void projectPathToZPlane( const std::vector<Amg::Vector3D >& in,
			    Amg::SetVectorVector3D& outset, const double& z  ) const;
  void projectPathToZPlane_specialZtoR( const std::vector<Amg::Vector3D >& in,
					Amg::SetVectorVector3D& outset, const double& z  ) const;

};

//____________________________________________________________________
InDetProjHelper::InDetProjHelper( double surfacethickness,
				  double data_disttosurface_epsilon,
				  double barrel_inner_radius,
				  double barrel_outer_radius,
				  double barrel_posneg_z,
				  double endcap_surface_z,
				  double endcap_surface_length,
				  double endcap_inner_radius,
				  double endcap_outer_radius,
				  double endcap_zasr_innerradius,
				  double endcap_zasr_endcapz_begin,
				  double endcap_zasr_squeezefact,
				  IVP1System* sys )
  : VP1HelperClassBase(sys,"InDetProjHelper"), m_d(new Imp)
{
  m_d->theclass = this;

  m_d->surfacethickness = surfacethickness;
  m_d->data_disttosurface_epsilon = data_disttosurface_epsilon;
  m_d->barrel_inner_radius = barrel_inner_radius;
  m_d->barrel_outer_radius = barrel_outer_radius;
  m_d->barrel_posneg_z = barrel_posneg_z;
  m_d->endcap_surface_z = endcap_surface_z;
  m_d->endcap_surface_length = endcap_surface_length;
  m_d->endcap_inner_radius = endcap_inner_radius;
  m_d->endcap_outer_radius = endcap_outer_radius;
  m_d->endcap_zasr_innerradius = endcap_zasr_innerradius;
  m_d->endcap_zasr_endcapz_begin = endcap_zasr_endcapz_begin;
  m_d->endcap_zasr_squeezefact = endcap_zasr_squeezefact;

  m_d->parts = InDetProjFlags::NoProjections;
  m_d->covercyl_zmin = 0.0;
  m_d->covercyl_zmax = 0.0;
  m_d->covercyl_rmin = 0.0;
  m_d->covercyl_rmax = 0.0;

}

//____________________________________________________________________
InDetProjHelper::~InDetProjHelper()
{
  delete m_d;
}

//____________________________________________________________________
InDetProjFlags::InDetProjPartsFlags InDetProjHelper::setParts( InDetProjFlags::InDetProjPartsFlags newparts )
{
  if ( m_d->parts==newparts )
    return m_d->parts;
  InDetProjFlags::InDetProjPartsFlags oldparts = m_d->parts;
  m_d->parts = newparts;

  //Update parameters of smallest cylinder covering all enabled clip volumes.
  if (m_d->parts == InDetProjFlags::NoProjections) {
    m_d->covercyl_zmin = 0.0;
    m_d->covercyl_zmax = 0.0;
    m_d->covercyl_rmin = 0.0;
    m_d->covercyl_rmax = 0.0;
    return oldparts;
  }

  bool no_ec_neg = !( m_d->parts & InDetProjFlags::EndCap_AllNeg );
  bool no_ec_pos = !( m_d->parts & InDetProjFlags::EndCap_AllPos );
  bool no_brl_neg = !( m_d->parts & InDetProjFlags::Barrel_AllNeg );
  bool no_brl_pos = !( m_d->parts & InDetProjFlags::Barrel_AllPos );
  bool barrel = m_d->parts & InDetProjFlags::Barrel_All;
  bool endcap = m_d->parts & InDetProjFlags::EndCap_All;

  m_d->covercyl_zmin = - m_d->endcap_surface_z - 0.5*m_d->endcap_surface_length;
  if ( no_ec_neg ) {
    m_d->covercyl_zmin = - m_d->barrel_posneg_z;
    if ( no_brl_neg ) {
      m_d->covercyl_zmin = 0.0;
      if ( no_brl_pos ) {
	m_d->covercyl_zmin = m_d->barrel_posneg_z;
	if ( no_ec_pos )
	  m_d->covercyl_zmin = m_d->endcap_surface_z + 0.5*m_d->endcap_surface_length + 1.0e99;
      }
    }
  }
  m_d->covercyl_zmax = m_d->endcap_surface_z + 0.5*m_d->endcap_surface_length;
  if ( no_ec_pos ) {
    m_d->covercyl_zmax = m_d->barrel_posneg_z;
    if ( no_brl_pos ) {
      m_d->covercyl_zmax = 0.0;
      if ( no_brl_neg ) {
	m_d->covercyl_zmax = - m_d->barrel_posneg_z;
	if ( no_ec_neg )
	  m_d->covercyl_zmax = - m_d->endcap_surface_z - 0.5*m_d->endcap_surface_length - 1.0e99;
      }
    }
  }
  if ( m_d->covercyl_zmin >= m_d->covercyl_zmax )
    m_d->covercyl_zmin = m_d->covercyl_zmax = 0;

  if ( barrel && endcap ) {
    m_d->covercyl_rmin = std::min(m_d->barrel_inner_radius,m_d->endcap_inner_radius);
    m_d->covercyl_rmax = std::max(m_d->barrel_outer_radius,m_d->endcap_outer_radius);
  } else {
    if (barrel) {
      m_d->covercyl_rmin = m_d->barrel_inner_radius;
      m_d->covercyl_rmax = m_d->barrel_outer_radius;
    } else if (endcap) {
      m_d->covercyl_rmin = m_d->endcap_inner_radius;
      m_d->covercyl_rmax = m_d->endcap_outer_radius;
    } else {
      message("Unforeseen execution path encountered.");
      m_d->covercyl_rmin = 0;
      m_d->covercyl_rmax = 0;
    }
  }
  if ( m_d->covercyl_rmin >= m_d->covercyl_rmax )
    m_d->covercyl_rmin = m_d->covercyl_rmax = 0;
  return oldparts;
}

//____________________________________________________________________
InDetProjFlags::InDetProjPartsFlags InDetProjHelper::parts() const
{
  return m_d->parts;
}

//____________________________________________________________________
void InDetProjHelper::clipPath( const std::vector<Amg::Vector3D >& path,
				Amg::SetVectorVector3D& resulting_subpaths ) const
{
  clipPath(path,resulting_subpaths,resulting_subpaths,resulting_subpaths,resulting_subpaths);
}

//____________________________________________________________________
void InDetProjHelper::clipPath( const std::vector<Amg::Vector3D >& path,
				Amg::SetVectorVector3D& resulting_subpaths_barrelA,
				Amg::SetVectorVector3D& resulting_subpaths_barrelC,
				Amg::SetVectorVector3D& resulting_subpaths_endcapA,
				Amg::SetVectorVector3D& resulting_subpaths_endcapC ) const
{
  if (verbose())
    messageVerbose("clipPath(..) called. Input path has "+QString::number(path.size())+" points.");

  resulting_subpaths_barrelA.clear();
  resulting_subpaths_barrelC.clear();
  resulting_subpaths_endcapA.clear();
  resulting_subpaths_endcapC.clear();

  //Fixme: If verbose - perform sanity check of input data (check for NAN's).
  if (m_d->parts == InDetProjFlags::NoProjections ) {
    if (verbose())
      messageVerbose("All projections currently off.");
    return;
  }
  if ( path.size()<2 ) {
    if (verbose())
      messageVerbose("Input path too short.");
    return;
  }

  // Find the clipped path's in all of the enabled detector parts.

  //For efficiency, we first clip the path to the smallest
  //axis-aligned cylinder containing all of the projective volumes
  Amg::SetVectorVector3D paths_clipped;
  m_d->clipPathToHollowCylinder( path, paths_clipped,
			       m_d->covercyl_rmin, m_d->covercyl_rmax,
			       m_d->covercyl_zmin, m_d->covercyl_zmax );

  if (paths_clipped.empty()) {
    if (verbose())
      messageVerbose("Path entirely outside clip volumes.");
    return;
  }

  const bool enabled_brlA = m_d->parts & InDetProjFlags::Barrel_AllPos;
  const bool enabled_brlC = m_d->parts & InDetProjFlags::Barrel_AllNeg;
  const bool enabled_ecA = m_d->parts & InDetProjFlags::EndCap_AllPos;
  const bool enabled_ecC = m_d->parts & InDetProjFlags::EndCap_AllNeg;

  //Special case: If exactly one of the four parts is enabled, we already have our result:
  if ( ( (enabled_brlA?1:0) + (enabled_brlC?1:0) + (enabled_ecA?1:0) + (enabled_ecC?1:0) ) == 1 ) {
    if (enabled_brlA) {
      resulting_subpaths_barrelA = paths_clipped;
      if (verbose())
	messageVerbose("clipPath(..) only brlA enabled. Returning.");
      return;
    }
    if (enabled_brlC) {
      resulting_subpaths_barrelC = paths_clipped;
      if (verbose())
	messageVerbose("clipPath(..) only brlC enabled. Returning.");
      return;
    }
    if (enabled_ecA) {
      resulting_subpaths_endcapA = paths_clipped;
      if (verbose())
	messageVerbose("clipPath(..) only ecA enabled. Returning.");
      return;
    }
    if (enabled_ecC) {
      resulting_subpaths_endcapC = paths_clipped;
      if (verbose())
	messageVerbose("clipPath(..) only ecC enabled. Returning.");
      return;
    }
  }


  //For each of the segments, we then find its clipped parts inside
  //the four detector volumes: BarrelA, BarrelC, EndCapA, EndCapC.
  //   Amg::SetVectorVector3D paths_brlA, paths_brlC, paths_ecA,paths_ecC;
  Amg::SetVectorVector3D::const_iterator it, itE(paths_clipped.end());
  for (it = paths_clipped.begin();it!=itE;++it) {
    if ( enabled_brlA )
      m_d->clipPathToHollowCylinder( *it, resulting_subpaths_barrelA, m_d->barrel_inner_radius, m_d->barrel_outer_radius, 0, m_d->barrel_posneg_z );
    if ( enabled_brlC )
      m_d->clipPathToHollowCylinder( *it, resulting_subpaths_barrelC, m_d->barrel_inner_radius, m_d->barrel_outer_radius, - m_d->barrel_posneg_z, 0 );
    if ( enabled_ecA )
      m_d->clipPathToHollowCylinder( *it, resulting_subpaths_endcapA, m_d->endcap_inner_radius, m_d->endcap_outer_radius,
			     m_d->endcap_surface_z - m_d->endcap_surface_length * 0.5, m_d->endcap_surface_z + m_d->endcap_surface_length * 0.5 );
    if ( enabled_ecC )
      m_d->clipPathToHollowCylinder( *it, resulting_subpaths_endcapC, m_d->endcap_inner_radius, m_d->endcap_outer_radius,
			     - m_d->endcap_surface_z - m_d->endcap_surface_length * 0.5, - m_d->endcap_surface_z + m_d->endcap_surface_length * 0.5 );
  }

  messageVerbose("clipPath(..) end.");
  //Fixme: If verbose: sanity check on output!
}

//____________________________________________________________________
void InDetProjHelper::Imp::movePoint1ToZPlaneAndPoint2( Amg::Vector3D& p1, const Amg::Vector3D& p2, const double& z ) const
{
  double dx(p2.x()-p1.x()), dy(p2.y()-p1.y()), dz(p2.z()-p1.z());
  if (dz==0.0) {
    theclass->message("movePoint1ToZPlaneAndPoint2 Error: Points have same z!!");
    return;
  }
  double s( (z-p1.z())/dz );
//  p1.set( p1.x()+dx*s, p1.y()+dy*s, z );
  Amg::setVector3DCartesian( p1, p1.x()+dx*s, p1.y()+dy*s, z );
}

//____________________________________________________________________
bool InDetProjHelper::Imp::clipSegmentToZInterval( Amg::Vector3D&a, Amg::Vector3D&b,
						   const double& zmin, const double& zmax ) const
{
  if (a.z()<zmin) {
    if (b.z()<zmin)//both <zmin
      return false;
    //a<zmin, b>=zmin:
    movePoint1ToZPlaneAndPoint2(a,b,zmin);
    if (b.z()>zmax)
      movePoint1ToZPlaneAndPoint2(b,a,zmax);
    return true;
  } else {
    if (b.z()<zmin) {
      //a>=zmin, b<zmin
      movePoint1ToZPlaneAndPoint2(b,a,zmin);
      if (a.z()>zmax)
	movePoint1ToZPlaneAndPoint2(a,b,zmax);
      return true;
    } else {
      //Both are > zmin
      if (a.z()>zmax) {
	if (b.z()>zmax)
	  return false;
	movePoint1ToZPlaneAndPoint2(a,b,zmax);
	return true;
      } else {
	//zmin<=a<=zmax, b>=zmin
	if (b.z()>zmax)
	  movePoint1ToZPlaneAndPoint2(b,a,zmax);
	return true;
      }
    }
  }
}

//____________________________________________________________________
void InDetProjHelper::Imp::movePoint1ToInfiniteCylinderAndPoint2( Amg::Vector3D&p1, const Amg::Vector3D&p2,const double& r ) const
{
  //Fixme: what happens here if we don't cross? And how can we be sure
  //that we don't move FURTHER than the other point? (i.e. if the
  //infinite line with p1 and p2 crosses, but the segment p1p2 does
  //not!?

//  double p1r(p1.r());
//  double dr(p2.r()-p1r);
  double p1r( Amg::rVector3D(p1) );
  double dr( Amg::rVector3D(p2)-p1r );

  if (dr==0.0) {
    theclass->message("movePoint1ToInfiniteCylinderAndPoint2 Error: Points have same r!!");
    return;
  }
  double s((r-p1r)/dr);
  double t(1.0-s);
  Amg::setVector3DCartesian( p1, p1.x()*t + p2.x()*s, p1.y()*t + p2.y()*s, p1.z()*t + p2.z()*s );

}

//____________________________________________________________________
void InDetProjHelper::Imp::lineCircleIntersection( const Amg::Vector3D&a, const Amg::Vector3D&b,const double& r,
						   double & u1, double& u2 ) const
{
  const double dx = b.x()-a.x();
  const double dy = b.y()-a.y();
  double A = dx*dx+dy*dy;
  if (A==0.0) {
    //That's not a line => no intersections unless points are exactly on circumference!
    u1 = u2 = ( a.x()*a.x()+a.y()*a.y() == r*r ? 0.0 : 1.0e99 );
    return;
  }
  double B = 2.0*( a.x()*dx + a.y()*dy );
  double C = a.x()*a.x()+a.y()*a.y() - r*r;
  double D = B*B-4*A*C;

  if (D>0.0) {
    //Intersections
    double sqrtD = sqrt(D);
    u1 = 0.5 * ( -B - sqrtD) / A;
    u2 = 0.5 * ( -B + sqrtD) / A;
  } else if (D<0.0) {
    //No intersections:
    u1 = u2 = -1.0e99;
  } else {
    //intersection in one point
    u1 = u2 = -0.5*B/A;
  }

}

//____________________________________________________________________
bool InDetProjHelper::Imp::clipSegmentToInfiniteHollowCylinder( Amg::Vector3D&a, Amg::Vector3D&b,
								const double& rmin, const double& rmax,
								Amg::Vector3D&seg2_a, Amg::Vector3D&seg2_b ) const
{
  //* if returns false: segment does not intersect hollow cylinder - do
  //                    NOT use returned points for anything.
  //* if returns true and seg2_a==seg2_b: Use "a" and "b" for the clipped segment.
  //* if returns true and seg2_a!=seg2_b: The clip resulting in TWO new segments
  //                                      (it was cut in two by the inner wall).

  //Fixme: Stuff like the following!:
  //   if (VP1SanityCheck::enabled()) {
  //     VP1SanityCheck::beginGroup("InDetProjHelper::Imp::clipSegmentToInfiniteHollowCylinder");
  //     VP1SanityCheck::positiveParameter("rmin",rmin);
  //     VP1SanityCheck::positiveParameter("rmax",rmax);
  //     VP1SanityCheck::parameter("point a",a);
  //     VP1SanityCheck::parameter("point b",b);
  //     VP1SanityCheck::endGroup();
  //   }
  const double ar2 = a.x()*a.x()+a.y()*a.y();
  const double br2 = b.x()*b.x()+b.y()*b.y();
  const double rmin2 = rmin*rmin;
  //We might be inside inner wall:
  if (ar2 <= rmin2 && br2 <= rmin2 ) {
    //    seg2_a=seg2_b;
//     if (verbose())
//       theclass->messageVerbose("clipSegmentToInfiniteHollowCylinder Segment entirely inside rmin.");
    return false;
  }
  //Some fast checks for being outside:
  if ( (a.x()<=-rmax&&b.x()<=-rmax) || (a.x()>=rmax&&b.x()>=rmax) || (a.y()<=-rmax&&b.y()<=-rmax)|| (a.y()>=rmax&&b.y()>=rmax) ) {
//     if (verbose())
//       theclass->messageVerbose("clipSegmentToInfiniteHollowCylinder Segment clearly entirely outside outside rmax.");
//    seg2_a=seg2_b;
    return false;
  }

  //If a==b  (apart from perhaps z coord), the check is simple:
  const double dx = b.x()-a.x();
  const double dy = b.y()-a.y();
  const double rmax2 = rmax*rmax;
  if (dx==0.0&&dy==0.0) {
    //Apparently a==b (apart from perhaps z coord).
//     if (theclass->verbose())
//       theclass->messageVerbose("clipSegmentToInfiniteHollowCylinder a==b.");
    return ar2<=rmax2;
  }
  //Find point which is closest to the z-axis and on the segment:
  const double u = - (a.y()*dy+a.x()*dx)/(dx*dx+dy*dy);
  const double px = ( u <= 0 ? a.x() : ( u >= 1 ? b.x() : a.x()+u*dx ) );
  const double py = ( u <= 0 ? a.y() : ( u >= 1 ? b.y() : a.y()+u*dy ) );
  const double pr2 = px*px+py*py;
  if (pr2>=rmax2) {
//     if (theclass->verbose())
//       theclass->messageVerbose("clipSegmentToInfiniteHollowCylinder segment entirely outside rmax.");
//    seg2_a=seg2_b;
    return false;


  }
  //We now know that the segment does indeed intersect the clip volume.
  seg2_a=seg2_b;//signature of just one segment:

  if (pr2>=rmin2&&ar2<=rmax2&&br2<=rmax2) {
    //We are actually already entirely inside the clip volume.
//     if (theclass->verbose())
//       theclass->messageVerbose("clipSegmentToInfiniteHollowCylinder segment entirely inside clip volume."
// 			       " (pr="+QString::number(sqrt(pr2))+", ar="+QString::number(sqrt(ar2))
// 			       +", br="+QString::number(sqrt(br2))+")");
    return true;
  }

  //First we simply clip to the outer cylinder:
  if (ar2>rmax2||br2>rmax2) {
    //We need to clip a-b to be inside the outer cylinder.
    //Find intersections:
    double u1, u2;
    lineCircleIntersection(a,b,rmax,u1,u2);//u1<=u2 !
    if (u1==u2) {
      //We are just touching - but we already tested against this!
      theclass->message("This should never happen(1).");
      //      seg2_a=seg2_b;
      return false;
    }
    Amg::Vector3D asave(a);
    if (u1>0&&u1<1) {
      //move a to a+u1*(b-a)
      a = a+u1*(b-a);
//       if (verbose())
// 	theclass->messageVerbose("clipSegmentToInfiniteHollowCylinder sliding a towards b, at the rmax circle.");
    }
    if (u2>0&&u2<1) {
      //move b to a+u2*(b-a)
      b = asave+u2*(b-asave);
//       if (verbose())
// 	theclass->messageVerbose("clipSegmentToInfiniteHollowCylinder sliding b towards a, at the rmax circle.");
    }
  }

  if (pr2>=rmin2) {
//     if (verbose())
//       theclass->messageVerbose("clipSegmentToInfiniteHollowCylinder remaining segment is now entirely inside.");
    return true;
  }
  //Ok, we know that we intersect the inner cylinder
  double u1, u2;
  lineCircleIntersection(a,b,rmin,u1,u2);//u1<=u2 !

  if (u1>0&&u1<1) {
    if (u2>0&&u2<1) {
      //We intersect twice. Thus, two line segments:
      //a  to "a+u1*(b-a)" and "a+u2*(b-a)" to b
      //a=a;
      seg2_b = b;
      b = a+u1*(seg2_b-a);
      seg2_a=a+u2*(seg2_b-a);
//       if (verbose())
// 	theclass->messageVerbose("clipSegmentToInfiniteHollowCylinder Two resulting segments!.");
      return true;
    }
    b = a+u1*(b-a);
//     theclass->messageVerbose("clipSegmentToInfiniteHollowCylinder One resulting segment (b->a)!.");
    return true;
  }
  if (u2>0&&u2<1)
    a = a+u2*(b-a);
//   theclass->messageVerbose("clipSegmentToInfiniteHollowCylinder One resulting segment (a->b)!.");
  return true;
}



//____________________________________________________________________
bool InDetProjHelper::Imp::clipSegmentToHollowCylinder( Amg::Vector3D&a, Amg::Vector3D&b,
							const double& rmin, const double& rmax,
							const double& zmin, const double& zmax,
							Amg::Vector3D&seg2_a, Amg::Vector3D&seg2_b ) const
{
  //  seg2_a = seg2_b;//test
//   if (theclass->verbose()) {
//     theclass->messageVerbose("clipSegmentToHollowCylinder called with:");
//     theclass->messageVerbose("   rmin = "+QString::number(rmin));
//     theclass->messageVerbose("   rmax = "+QString::number(rmax));
//     theclass->messageVerbose("   zmin = "+QString::number(zmin));
//     theclass->messageVerbose("   zmax = "+QString::number(zmax));
//     theclass->messageVerbose("   a = ("+QString::number(a.x())+", "+QString::number(a.y())+", "+QString::number(a.z())+")");
//     theclass->messageVerbose("   b = ("+QString::number(b.x())+", "+QString::number(b.y())+", "+QString::number(b.z())+")");
//   }
  if (!clipSegmentToZInterval(a,b,zmin,zmax)) {
    //    seg2_a = seg2_b;
//     if (theclass->verbose())
//       theclass->messageVerbose("clipSegmentToHollowCylinder segment outside z-interval.");
    return false;
  }
//   if (theclass->verbose()) {
//     theclass->messageVerbose("clipSegmentToHollowCylinder parameters after clipSegmentToZInterval:");
//     if (a.z()<zmin||a.z()>zmax)
//       theclass->messageVerbose("clipSegmentToHollowCylinder ERROR in clipSegmentToZInterval call (a_z wrong).");
//     if (b.z()<zmin||b.z()>zmax)
//       theclass->messageVerbose("clipSegmentToHollowCylinder ERROR in clipSegmentToZInterval call (b_z wrong).");
//     theclass->messageVerbose("   a = ("+QString::number(a.x())+", "+QString::number(a.y())+", "+QString::number(a.z())+")");
//     theclass->messageVerbose("   b = ("+QString::number(b.x())+", "+QString::number(b.y())+", "+QString::number(b.z())+")");
//   }
  if (!clipSegmentToInfiniteHollowCylinder(a,b,rmin,rmax,seg2_a,seg2_b)) {
    //    seg2_a = seg2_b;
//     if (theclass->verbose())
//       theclass->messageVerbose("clipSegmentToHollowCylinder segment outside infinite hollow cylinder.");
    return false;
  }
//   if (theclass->verbose()) {
//     theclass->messageVerbose("clipSegmentToHollowCylinder parameters after clipSegmentToInfiniteHollowCylinder:");
//     theclass->messageVerbose("   a = ("+QString::number(a.x())+", "+QString::number(a.y())+", "+QString::number(a.z())+")");
//     theclass->messageVerbose("   b = ("+QString::number(b.x())+", "+QString::number(b.y())+", "+QString::number(b.z())+")");
//     const double ar2 = a.x()*a.x()+a.y()*a.y();
//     const double br2 = b.x()*b.x()+b.y()*b.y();
//     if (ar2<rmin*rmin||ar2>rmax*rmax)
//       theclass->messageVerbose("clipSegmentToHollowCylinder ERROR in clipSegmentToInfiniteHollowCylinder call (a wrong).");
//     if (br2<rmin*rmin||br2>rmax*rmax)
//       theclass->messageVerbose("clipSegmentToHollowCylinder ERROR in clipSegmentToInfiniteHollowCylinder call (b wrong).");
//     theclass->messageVerbose("clipSegmentToHollowCylinder returning.");
//   }

  return true;
}

//____________________________________________________________________
void InDetProjHelper::Imp::clipPathToHollowCylinder( const std::vector<Amg::Vector3D >& in,
//						     Amg::SetVectorVector3D& out,
							 Amg::SetVectorVector3D& out,
						     const double& rmin, const double& rmax,
						     const double& zmin, const double& zmax ) const
{
//   if (theclass->verbose()) {
//     theclass->messageVerbose("clipPathToHollowCylinder called");
//     theclass->messageVerbose("    ===>  rmin = "+QString::number(rmin));
//     theclass->messageVerbose("    ===>  rmax = "+QString::number(rmax));
//     theclass->messageVerbose("    ===>  zmin = "+QString::number(zmin));
//     theclass->messageVerbose("    ===>  zmax = "+QString::number(zmax));
//   }

  out.clear();
  if (rmin>=rmax||rmin<0||zmin>=zmax) {
    theclass->message("clipPathToHollowCylinder Error: Non-sensical cylinder parameters!");
    return;
  }
  const unsigned n=in.size();
  if (n<2)
    return;

  Amg::Vector3D a,b;
  Amg::Vector3D seg2_a,seg2_b;
  std::vector<Amg::Vector3D > v;
  for (unsigned i = 1; i<n; ++i) {
    //     theclass->messageVerbose("clipPathToHollowCylinder -> dealing with segment "+QString::number(i-1)+"->"+QString::number(i));

    a = in.at(i-1);//fixme: .at()->[]
    b = in.at(i);
    if ( clipSegmentToHollowCylinder( a,b,rmin,rmax,zmin,zmax,seg2_a,seg2_b ) ) {
      if (v.empty()) {
	v.push_back(a);
	v.push_back(b);
	if (seg2_a!=seg2_b) {
	  out.insert(v);
	  v.clear();
	  v.push_back(seg2_a);
	  v.push_back(seg2_b);
	}
      } else {
	//We know that previous segment was also touching. Therefore
	//it must necessarily be true that v.back()==a.
	if ( v.back() != a ) {
 	  theclass->messageDebug("ERROR: Inconsistency found while building clip part");//Fixme: downgrade to messageDebug for now, but need to understand this!
	  out.insert(v);
	  v.clear();
	  v.push_back(a);
	}
	v.push_back(b);
	if (seg2_a!=seg2_b) {
	  out.insert(v);
	  v.clear();
	  v.push_back(seg2_a);
	  v.push_back(seg2_b);
	}
      }
    } else {
//       theclass->messageVerbose("Segment does not touch");
      //Segment doesn't touch cylinder volume - flush part currently building if any.
      if (!v.empty()) {
	out.insert(v);
	v.clear();
      }
    }
  }
  if (!v.empty()) {
//	  theclass->messageDebug("v not empty");
    out.insert(v);
  }
}

//____________________________________________________________________
void InDetProjHelper::Imp::projectPathToInfiniteCylinder( const std::vector<Amg::Vector3D >& in,
							  Amg::SetVectorVector3D& outset, const double& r  ) const
{
  std::vector<Amg::Vector3D > out(in);
  std::vector<Amg::Vector3D >::iterator it(out.begin()), itE(out.end());
  double s;
  for (;it!=itE;++it) {
    if ( it->x()==0.0 && it->y()==0.0 ) {
      theclass->message("ERROR: Point has x==0 and y==0. Ambiguous projection of point.");

//      it->setX(1.0);
      it->x() = 1.0;
    }
    s = r / sqrt( it->x()*it->x()+it->y()*it->y() );

//    it->setX(it->x()*s);
//    it->setY(it->y()*s);
    it->x() = it->x()*s;
    it->y() = it->y()*s;

  }
  outset.insert(out);
}

//____________________________________________________________________
void InDetProjHelper::Imp::projectPathToZPlane( const std::vector<Amg::Vector3D >& in,
						Amg::SetVectorVector3D& outset, const double& z  ) const
{
  std::vector<Amg::Vector3D > out(in);
  std::vector<Amg::Vector3D >::iterator it(out.begin()), itE(out.end());
  for (;it!=itE;++it) {
//    it->setZ(z);
    it->z() = z;
  }
  outset.insert(out);
}


//____________________________________________________________________
void InDetProjHelper::transformECPointToZPlane_specialZtoR( Amg::Vector3D& p,
							    const double& planeZ,
							    const double& planeRBegin,
							    const double& endcapZBegin,
							    const double& squeezeFactor )
{
    if ( p.x()==0.0 && p.y()==0.0 ) {
      VP1Msg::message("InDetProjHelper::transformECPointToZPlane_specialZtoR ERROR: "
		      "Point has x==0 and y==0. Ambiguous projection of point.");
//      p.setX(1.0);
      p.x() = 1.0;
    }
    const double r = planeRBegin + (fabs(p.z())-endcapZBegin)/squeezeFactor;
    const double s = r / sqrt( p.x()*p.x()+p.y()*p.y() );
//    p.setX(p.x()*s);
//    p.setY(p.y()*s);
//    p.setZ(planeZ);
    p.x() = p.x()*s;
    p.y() = p.y()*s;
    p.z() = planeZ;
}

//____________________________________________________________________
void InDetProjHelper::Imp::projectPathToZPlane_specialZtoR( const std::vector<Amg::Vector3D >& in,
							    Amg::SetVectorVector3D& outset,
							    const double& z ) const
{
  std::vector<Amg::Vector3D > out(in);
  std::vector<Amg::Vector3D >::iterator it(out.begin()), itE(out.end());
  for (;it!=itE;++it)
    InDetProjHelper::transformECPointToZPlane_specialZtoR(*it,
							  z,
							  endcap_zasr_innerradius,
							  endcap_zasr_endcapz_begin,
							  endcap_zasr_squeezefact);
  outset.insert(out);
}

//____________________________________________________________________
void InDetProjHelper::projectPath( const std::vector<Amg::Vector3D >& path,
				   Amg::SetVectorVector3D& resulting_projs ) const
{
  projectPath(path,resulting_projs,resulting_projs,resulting_projs,resulting_projs);
}

//____________________________________________________________________
void InDetProjHelper::projectPath( const std::vector<Amg::Vector3D >& path,
				   Amg::SetVectorVector3D& resulting_projections_barrelA,
				   Amg::SetVectorVector3D& resulting_projections_barrelC,
				   Amg::SetVectorVector3D& resulting_projections_endcapA,
				   Amg::SetVectorVector3D& resulting_projections_endcapC ) const
{
  if (verbose())
    messageVerbose("projectPath(..) called. Input path has "+QString::number(path.size())+" points.");

  resulting_projections_barrelA.clear();
  resulting_projections_barrelC.clear();
  resulting_projections_endcapA.clear();
  resulting_projections_endcapC.clear();

  //Fixme: If verbose - perform sanity check of input data (check for NAN's).
  if (m_d->parts == InDetProjFlags::NoProjections ) {
    if (verbose())
      messageVerbose("All projections currently off.");
    return;
  }
  if ( path.size()<2 ) {
    if (verbose())
      messageVerbose("Input path too short.");
    return;
  }

  // ===> First we must find the clipped path's in all of the enabled detector parts.

  Amg::SetVectorVector3D paths_brlA, paths_brlC, paths_ecA,paths_ecC;
  clipPath( path,paths_brlA, paths_brlC, paths_ecA,paths_ecC);

  // ===> Then we project those.

  //Fixme: The dependence on surface thickness and epsilon below is very preliminary.

  const double eps = m_d->data_disttosurface_epsilon;
  const double endcapeps(-5*Gaudi::Units::mm);//fixme hardcoding..

  Amg::SetVectorVector3D::const_iterator it,itE;

  if (m_d->parts & InDetProjFlags::Barrel_AllPos) {
    itE = paths_brlA.end();
    if ( m_d->parts & InDetProjFlags::BarrelCentral )
      for ( it = paths_brlA.begin(); it!=itE; ++it )
	m_d->projectPathToZPlane( *it, resulting_projections_barrelA, 0.5*m_d->surfacethickness+eps );
    if ( m_d->parts & InDetProjFlags::BarrelPositive )
      for ( it = paths_brlA.begin(); it!=itE; ++it )
	m_d->projectPathToZPlane( *it, resulting_projections_barrelA, m_d->barrel_posneg_z - eps );
  }
  if ( m_d->parts & InDetProjFlags::Barrel_AllNeg ) {
    itE = paths_brlC.end();
    if ( m_d->parts & InDetProjFlags::BarrelCentral )
      for ( it = paths_brlC.begin(); it!=itE; ++it )
	m_d->projectPathToZPlane( *it, resulting_projections_barrelC, - 0.5*m_d->surfacethickness - eps);
    if ( m_d->parts & InDetProjFlags::BarrelNegative )
      for ( it = paths_brlC.begin(); it!=itE; ++it )
	m_d->projectPathToZPlane( *it, resulting_projections_barrelC, - m_d->barrel_posneg_z );
  }
  if ( m_d->parts & InDetProjFlags::EndCap_AllPos ) {
    itE = paths_ecA.end();
    if ( m_d->parts & InDetProjFlags::EndCapInnerPositive )
      for ( it = paths_ecA.begin(); it!=itE; ++it )
	m_d->projectPathToInfiniteCylinder( *it, resulting_projections_endcapA, m_d->endcap_inner_radius + eps+endcapeps );
    if ( m_d->parts & InDetProjFlags::EndCapOuterPositive )
      for ( it = paths_ecA.begin(); it!=itE; ++it )
	m_d->projectPathToInfiniteCylinder( *it, resulting_projections_endcapA, m_d->endcap_outer_radius + eps+endcapeps );
    //Fixme: Make sure to use the same parameters here as in PRDHandle_TRT.cxx:
    if ( m_d->parts & InDetProjFlags::TRT_EndCapZToRCentral )
      for ( it = paths_ecA.begin(); it!=itE; ++it )
	m_d->projectPathToZPlane_specialZtoR( *it, resulting_projections_endcapA,
					    0.5*m_d->surfacethickness + eps  );
    //Fixme: Make sure to use the same parameters here as in PRDHandle_TRT.cxx:
    if ( m_d->parts & InDetProjFlags::TRT_EndCapZToRPositive )
      for ( it = paths_ecA.begin(); it!=itE; ++it )
	m_d->projectPathToZPlane_specialZtoR( *it, resulting_projections_endcapA,
					    m_d->barrel_posneg_z - 0.5*m_d->surfacethickness - eps /*fixme: +- epsilon??*/  );
  }
  if ( m_d->parts & InDetProjFlags::EndCap_AllNeg ) {
    itE = paths_ecC.end();
    if ( m_d->parts & InDetProjFlags::EndCapInnerNegative )
      for ( it = paths_ecC.begin(); it!=itE; ++it )
	m_d->projectPathToInfiniteCylinder( *it, resulting_projections_endcapC, m_d->endcap_inner_radius + eps+endcapeps );
    if ( m_d->parts & InDetProjFlags::EndCapOuterNegative )
      for ( it = paths_ecC.begin(); it!=itE; ++it )
	m_d->projectPathToInfiniteCylinder( *it, resulting_projections_endcapC, m_d->endcap_outer_radius + eps+endcapeps );
    //Fixme: Make sure to use the same parameters here as in PRDHandle_TRT.cxx:
    if ( m_d->parts & InDetProjFlags::TRT_EndCapZToRCentral )
      for ( it = paths_ecC.begin(); it!=itE; ++it )
	m_d->projectPathToZPlane_specialZtoR( *it, resulting_projections_endcapC,
					    - 0.5*m_d->surfacethickness - eps  );
    //Fixme: Make sure to use the same parameters here as in PRDHandle_TRT.cxx:
    if ( m_d->parts & InDetProjFlags::TRT_EndCapZToRNegative )
      for ( it = paths_ecC.begin(); it!=itE; ++it )
	m_d->projectPathToZPlane_specialZtoR( *it, resulting_projections_endcapC,
					    - m_d->barrel_posneg_z + 0.5*m_d->surfacethickness + eps/*fixme: +- epsilon??*/  );
  }

}

//____________________________________________________________________
InDetProjHelper::PartsFlags InDetProjHelper::touchedParts( const std::vector<Amg::Vector3D >& path ) const
{
  if (verbose())
    messageVerbose("touchedParts(..) called. Input path has "+QString::number(path.size())+" points.");
  PartsFlags touchedparts = NoParts;
  if ( m_d->touchesHollowCylinder(path,m_d->barrel_inner_radius, m_d->barrel_outer_radius, 0, m_d->barrel_posneg_z) )
    touchedparts |= BarrelA;
  if ( m_d->touchesHollowCylinder(path,m_d->barrel_inner_radius, m_d->barrel_outer_radius, - m_d->barrel_posneg_z, 0) )
    touchedparts |= BarrelC;
  if ( m_d->touchesHollowCylinder(path,m_d->endcap_inner_radius, m_d->endcap_outer_radius,
				m_d->endcap_surface_z - m_d->endcap_surface_length * 0.5, m_d->endcap_surface_z + m_d->endcap_surface_length * 0.5 ) )
    touchedparts |= EndCapA;
  if ( m_d->touchesHollowCylinder(path, m_d->endcap_inner_radius, m_d->endcap_outer_radius,
				- m_d->endcap_surface_z - m_d->endcap_surface_length * 0.5, - m_d->endcap_surface_z + m_d->endcap_surface_length * 0.5) )
    touchedparts |= EndCapC;
  return touchedparts;
}

//____________________________________________________________________
bool InDetProjHelper::Imp::touchesHollowCylinder( const std::vector<Amg::Vector3D >& path,
						  const double& rmin, const double& rmax,
						  const double& zmin, const double& zmax ) const
{
  const double rmin2(rmin*rmin), rmax2(rmax*rmax);
  double r2;
  std::vector<Amg::Vector3D >::const_iterator it(path.begin()), itE(path.end());
  for (;it!=itE;++it) {
    if (it->z()<zmin)
      continue;
    if (it->z()>zmax)
      continue;
    r2 = it->x()*it->x()+it->y()*it->y();
    if (r2<rmin2)
      continue;
    if (r2<=rmax2)
      return true;
  }
  return false;
}
