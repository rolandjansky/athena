/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
    Trk2dDistanceSeeder.cxx - Description in header file
*********************************************************************/

//#define TRK2DDISTANCESEEDER_DEBUG

#include "TrkVertexSeedFinderUtils/Trk2dDistanceSeeder.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "MagFieldElements/AtlasFieldCache.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkVertexSeedFinderUtils/TwoTracks.h"
#include <TMath.h>
#include <cmath>

namespace {
  inline double dist2d(const Amg::Vector3D & a, const Amg::Vector3D & b) {
    return std::sqrt(std::pow(a.x()-b.x(),2)+std::pow(a.y()-b.y(),2));
  }
  inline double takenearest(const double of, const double to) {
        if (of-to>=M_PI) return of-2*M_PI;
	if (of-to<-M_PI) return of+2*M_PI;
	return of;
  }
  inline double getangle(const double Dy, const double Dx) {//get the right angle between -M_PI and M_PI
    return TMath::ATan2(Dy,Dx);
    //    if (Dx==0.) return  Dy>=0 ?  M_PI/2. : -M_PI/2.;
    //    if (Dx<0) {
    //      if (Dy<0) {
    //	return atan(Dy/Dx)-M_PI;
    //      } else {
    //	return atan(Dy/Dx)+M_PI;
    //      }
    //    }
    //    return atan(Dy/Dx);
  }
  inline double oppositeangle(const double angle) {
    return  angle>0. ? angle-M_PI : angle+M_PI;
  }
#if 0
  inline const std::pair<double,double> gettwoanglesfromcos(const double cos) {
    return std::pair<double,double>(acos(cos),-acos(cos)); 
  }
  inline double shiftangle(const double angle,const double of) {
    if (angle+of>M_PI) return angle+of-2*M_PI;
    if (angle+of<=-M_PI) return angle+of+2*M_PI;
    return angle+of;
  }
#endif
  inline double square(const double tosquare) {
    return std::pow(tosquare,2);
  }
  //inline double getphipoca(const Trk::Perigee & myPerigee) {
  //  return myPerigee.parameters()[Trk::d0]>=0 ? myPerigee.parameters()[Trk::phi0]+M_PI/2. :
  //    myPerigee.parameters()[Trk::phi0]-M_PI/2.;
  //}
  inline double getphipoca(const Trk::Perigee & myPerigee) {
    return myPerigee.parameters()[Trk::phi0]+M_PI/2;
  }
  inline double setphipoca(const double phi) {
    return phi-M_PI/2.;
  }
  inline double goFromPhipocaToPhi(const double phipoca)
  {
    return phipoca+M_PI/2.;
  }

  inline double getRadiusOfCurvature(const Trk::Perigee & myPerigee,const double Bzfield) {
    return sin(myPerigee.parameters()[Trk::theta])/(Bzfield*myPerigee.parameters()[Trk::qOverP]);
  }
  inline Amg::Vector3D getCenterOfCurvature(const Trk::Perigee & myPerigee,const double RadiusOfCurvature,const double phipoca) {
    return Amg::Vector3D(myPerigee.associatedSurface().center().x()+myPerigee.parameters()[Trk::d0]*cos(phipoca)-RadiusOfCurvature*cos(phipoca),
			 myPerigee.associatedSurface().center().y()+myPerigee.parameters()[Trk::d0]*sin(phipoca)-RadiusOfCurvature*sin(phipoca),
			 myPerigee.associatedSurface().center().z()+myPerigee.parameters()[Trk::z0]+RadiusOfCurvature*
			 myPerigee.parameters()[Trk::phi0]/tan(myPerigee.parameters()[Trk::theta]));
  }
  inline Amg::Vector3D getSeedPoint(const Trk::Perigee & myPerigee,const Amg::Vector3D & center,
					  const double radius,const double newphi) {
    //    short int sgnd0=(short int)(myPerigee.parameters()[Trk::d0]/fabs(myPerigee.parameters()[Trk::d0]));
    return Amg::Vector3D(center.x()+radius*cos(newphi+M_PI/2.),
			       center.y()+radius*sin(newphi+M_PI/2.),
			       //		eliminated sgnd0 from center.z()-radius*(newphi-sgnd0*M_PI/2.)/tan(myPerigee.parameters()[Trk::theta]));
			       center.z()-radius*newphi/tan(myPerigee.parameters()[Trk::theta]));
  }
  inline Amg::Vector3D getSeedPoint(const Amg::Vector3D & center,
					  const double radius,const double newphi) {
    //    short int sgnd0=(short int)(myPerigee.parameters()[Trk::d0]/fabs(myPerigee.parameters()[Trk::d0]));
    return Amg::Vector3D(center.x()+radius*cos(newphi+M_PI/2.),
			 center.y()+radius*sin(newphi+M_PI/2.),
			 0.);
  }
  
}
  
namespace Trk
{
  Trk2dDistanceSeeder::Trk2dDistanceSeeder(const std::string& t, const std::string& n, const IInterface*  p) : 
    AthAlgTool(t,n,p),
    m_solveAmbiguityUsingZ(true)
  {   
    declareProperty("SolveAmbiguityUsingZ",m_solveAmbiguityUsingZ);
    declareInterface<Trk2dDistanceSeeder>(this);
  }

  Trk2dDistanceSeeder::~Trk2dDistanceSeeder() = default;

  StatusCode Trk2dDistanceSeeder::initialize() 
  { 
    ATH_CHECK( AlgTool::initialize() );
    ATH_CHECK( m_fieldCacheCondObjInputKey.initialize() );
    ATH_MSG_DEBUG( "Initialize successful" );
    return StatusCode::SUCCESS;
  }

StatusCode Trk2dDistanceSeeder::finalize() 
  {
    ATH_MSG_DEBUG( "Finalize successful" );
    return StatusCode::SUCCESS;
  }


  TwoPointOnTrack
  Trk2dDistanceSeeder::GetSeed (const TwoTracks & mytracks,
                                TwoPoints* twopoints /*=nullptr*/) const
  {
    SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCacheCondObjInputKey, Gaudi::Hive::currentContext()};
    const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};

    MagField::AtlasFieldCache fieldCache;
    fieldCondObj->getInitializedCache (fieldCache);

    const double bfield1 = getBField (mytracks.getFirstPerigee(), fieldCache);
    const double bfield2 = getBField (mytracks.getSecondPerigee(), fieldCache);

    //phitanpoca here means not the tan to poca (which is phi0) but the direction from perigee to the center of curvature (I don't know 
    //why I chose this strange name, sorry!)
    const double phitanpoca1=getphipoca(mytracks.getFirstPerigee());
    const double phitanpoca2=getphipoca(mytracks.getSecondPerigee());
    
    //Temporary abs1 and abs2 are the signed radius quantities (later we will reduce it to unsigned quantities)
    double abs1 = getRadiusOfCurvature(mytracks.getFirstPerigee(), bfield1);
    double abs2 = getRadiusOfCurvature(mytracks.getSecondPerigee(),bfield2);
    
    const double sgnr1 = abs1>0 ? 1. : -1.;
    const double sgnr2 = abs2>0 ? 1. : -1.;

    //component x of center of curv of first particle
    const std::pair<Amg::Vector3D,Amg::Vector3D> centersofcurv
      (getCenterOfCurvature(mytracks.getFirstPerigee(),abs1,phitanpoca1),
       getCenterOfCurvature(mytracks.getSecondPerigee(),abs2,phitanpoca2));

#ifdef TRK2DDISTANCESEEDER_DEBUG
    ATH_MSG_DEBUG( " Center of track number 1: " << centersofcurv.first << " center of track 2 " << centersofcurv.second );
#endif

    const double distance2d = dist2d(centersofcurv.first,centersofcurv.second);

    abs1 = std::abs(abs1);
    abs2 = std::abs(abs2);

  
#ifdef TRK2DDISTANCESEEDER_DEBUG
    ATH_MSG_DEBUG( "abs1: " << abs1 << " abs2: " << abs2 );
    ATH_MSG_DEBUG( "phitanpoca1: " << phitanpoca1 << " phitanpoca2: " << phitanpoca2 );
    ATH_MSG_DEBUG( "distance2d " << distance2d );
#endif

    double phi1 = 0;
    double phi2 = 0;

    // Strictly redundant with phi1 and phi2 above, but needed to get results
    // that are rounded identically with the pre-MT version.
    double phi1poca = 0;
    double phi2poca = 0;

    //first case (the two circles are the one distinct from the other)
    if (distance2d > (abs1+abs2)) {

#ifdef TRK2DDISTANCESEEDER_DEBUG
      ATH_MSG_DEBUG ("Distinct circles " );
#endif

      phi1 = getangle(centersofcurv.second.y()-centersofcurv.first.y(),
                      centersofcurv.second.x()-centersofcurv.first.x());
    
      if (sgnr2<0) {
        phi2=phi1;
      } else {
        phi2=oppositeangle(phi1);
      }      
    
      if (sgnr1<0) {
        phi1=oppositeangle(phi1);
      }

      //Now you are worried that the solution has to be the one nearest to the original phi...
      phi1=takenearest(phi1,phitanpoca1);
      phi2=takenearest(phi2,phitanpoca2);
    }
    else {
      //second case(if distance2d<abs1+abs2 we have still two cases
      //1) one circle inside the other (nobody is touched) (if |abs2-abs1|>d)
      //2) two intersections (if |abs2-abs1|<d)
      //   -->(case with one intersection seems me of no physical interest)
      if (fabs(abs2-abs1)>=distance2d) {

#ifdef TRK2DDISTANCESEEDER_DEBUG
        ATH_MSG_DEBUG( "  one circle inside the other " );
#endif

        //1)
        if (abs2<abs1) {

#ifdef TRK2DDISTANCESEEDER_DEBUG
          ATH_MSG_DEBUG("  second radius smaller than first " );
#endif 
          phi1 = getangle(centersofcurv.second.y()-centersofcurv.first.y(),
                          centersofcurv.second.x()-centersofcurv.first.x());	
          phi2=phi1;
        }
        else {
	
#ifdef TRK2DDISTANCESEEDER_DEBUG
          ATH_MSG_DEBUG( "  first radius smaller than second " );
#endif 
	
	
          phi1 = getangle(-(centersofcurv.second.y()-centersofcurv.first.y()),
                          -(centersofcurv.second.x()-centersofcurv.first.x()));
          phi2=phi1;
        }
      
        //adjust the sign, taking into account possible q(1,2)<0
        if (sgnr2<0) {
          phi2=oppositeangle(phi2);
        }      
      
        if (sgnr1<0) {
          phi1=oppositeangle(phi1);
        }

        phi1=takenearest(setphipoca(phi1),setphipoca(phitanpoca1));
        phi2=takenearest(setphipoca(phi2),setphipoca(phitanpoca2));      
        phi1poca = setphipoca(phi1);
        phi2poca = setphipoca(phi2);
      }
      else {

        //Do it easier...
        const double projection2=(square(abs1)-square(abs2)-square(distance2d))/2./distance2d;
        const double cosinus2=projection2/sgnr2/abs2;


#ifdef TRK2DDISTANCESEEDER_DEBUG
        const double projection1=(square(abs2)-square(abs1)-square(distance2d))/2./distance2d;
        const double cosinus1=projection1/sgnr1/abs1;
        ATH_MSG_DEBUG( "projection1: " << projection1 << " cosinus1 " << cosinus1
                       << "projection2: " << projection2 << " cosinus2 " << cosinus2 );
#endif

        const double phibase2=
          TMath::ATan2(centersofcurv.second.y()-centersofcurv.first.y(),
                       centersofcurv.second.x()-centersofcurv.first.x());


        const double addtophi2=
          TMath::ACos(cosinus2);

#ifdef TRK2DDISTANCESEEDER_DEBUG
        ATH_MSG_DEBUG( " phibase2 is : " << phibase2 << " add to phi " << addtophi2 );
#endif

        const std::pair<double,double> possiblephiontrack2(phibase2+addtophi2,phibase2-addtophi2);

#ifdef TRK2DDISTANCESEEDER_DEBUG
        ATH_MSG_DEBUG( "the two phis are: " << possiblephiontrack2.first << " and " <<  possiblephiontrack2.second );
#endif

        const std::pair<double,double> 
          possiblecosphitrack1((centersofcurv.second.x()-centersofcurv.first.x()
                                +sgnr2*abs2*TMath::Cos(possiblephiontrack2.first))/sgnr1/abs1,
                               (centersofcurv.second.x()-centersofcurv.first.x()
                                +sgnr2*abs2*TMath::Cos(possiblephiontrack2.second))/sgnr1/abs1);

        const std::pair<double,double>
          possiblesigntrack1(sgnr1*(centersofcurv.second.y()-centersofcurv.first.y()
                                    +sgnr2*abs2*TMath::Sin(possiblephiontrack2.first))>0?1.:-1.,
                             sgnr1*(centersofcurv.second.y()-centersofcurv.first.y()
                                    +sgnr2*abs2*TMath::Sin(possiblephiontrack2.second))>0?1.:-1.);

        const std::pair<double,double>
          possiblephiontrack1(possiblesigntrack1.first*TMath::ACos(possiblecosphitrack1.first),
                              possiblesigntrack1.second*TMath::ACos(possiblecosphitrack1.second));
						   
						     
        const std::pair<PointOnTrack,PointOnTrack>
          possiblepointsontrack1(PointOnTrack(mytracks.getFirstPerigee(),
                                              takenearest(setphipoca(possiblephiontrack1.first),
                                                          setphipoca(phitanpoca1))),
                                 PointOnTrack(mytracks.getFirstPerigee(),
                                              takenearest(setphipoca(possiblephiontrack1.second),
                                                          setphipoca(phitanpoca1))));

        const std::pair<PointOnTrack,PointOnTrack>
          possiblepointsontrack2(PointOnTrack(mytracks.getSecondPerigee(),
                                              takenearest(setphipoca(possiblephiontrack2.first),
                                                          setphipoca(phitanpoca2))),
                                 PointOnTrack(mytracks.getSecondPerigee(),
                                              takenearest(setphipoca(possiblephiontrack2.second),
                                                          setphipoca(phitanpoca2))));


        const std::pair<Amg::Vector3D,Amg::Vector3D>
          possiblepoints1(getSeedPoint(possiblepointsontrack1.first.getPerigee(),centersofcurv.first,
                                       abs1*sgnr1,possiblepointsontrack1.first.getPhiPoint()),
                          getSeedPoint(possiblepointsontrack1.second.getPerigee(),centersofcurv.first,
                                       abs1*sgnr1,possiblepointsontrack1.second.getPhiPoint()));

        const std::pair<Amg::Vector3D,Amg::Vector3D>
          possiblepoints2(getSeedPoint(possiblepointsontrack2.first.getPerigee(),centersofcurv.second,
                                       abs2*sgnr2,possiblepointsontrack2.first.getPhiPoint()),
                          getSeedPoint(possiblepointsontrack2.second.getPerigee(),centersofcurv.second,
                                       abs2*sgnr2,possiblepointsontrack2.second.getPhiPoint()));


#ifdef TRK2DDISTANCESEEDER_DEBUG
        ATH_MSG_DEBUG( "Point 1a: x " << possiblepoints1.first.x() << " y " << possiblepoints1.first.y() );
        ATH_MSG_DEBUG( "Point 2a: x " << possiblepoints2.first.x() << " y " << possiblepoints2.first.y() );
        ATH_MSG_DEBUG( "Point 1b: x " << possiblepoints1.second.x() << " y " << possiblepoints1.second.y() );
        ATH_MSG_DEBUG( "Point 2b: x " << possiblepoints2.second.x() << " y " << possiblepoints2.second.y() );
        ATH_MSG_DEBUG( "Distance between 1a and 2a:" << fabs(possiblepoints1.first.z()-possiblepoints2.first.z())  << 
                       "Distance between 1a and 2a:" << fabs(possiblepoints1.second.z()-possiblepoints2.second.z())  );
#endif

      
        //now discover what couple of points are the nearest ones
        if (m_solveAmbiguityUsingZ)
        {
          if (fabs(possiblepoints1.first.z()-possiblepoints2.first.z())<
              fabs(possiblepoints1.second.z()-possiblepoints2.second.z()))
          {
            phi1poca = possiblepointsontrack1.first.getPhiPoint();
            phi2poca = possiblepointsontrack2.first.getPhiPoint();
          } else {
            phi1poca = possiblepointsontrack1.second.getPhiPoint();
            phi2poca = possiblepointsontrack2.second.getPhiPoint();
          }
        }
        else
        {
          if (sqrt(possiblepoints1.first.x()*possiblepoints1.first.x()+possiblepoints1.first.y()*possiblepoints1.first.y())<
              sqrt(possiblepoints2.first.x()*possiblepoints2.first.x()+possiblepoints2.first.y()*possiblepoints2.first.y()))
          {
            phi1poca = possiblepointsontrack1.first.getPhiPoint();
            phi2poca = possiblepointsontrack2.first.getPhiPoint();
          } else {
            phi1poca = possiblepointsontrack1.second.getPhiPoint();
            phi2poca = possiblepointsontrack2.second.getPhiPoint();
          }
        }
        phi1=goFromPhipocaToPhi(phi1poca);
        phi2=goFromPhipocaToPhi(phi2poca);
      }
    }

    if (twopoints) {
      *twopoints = TwoPoints (getSeedPoint(centersofcurv.first,
                                           abs1*sgnr1,setphipoca(phi1)),
                              getSeedPoint(centersofcurv.second,
                                           abs2*sgnr2,setphipoca(phi2)));
    }

    return std::make_pair (PointOnTrack (mytracks.getFirstPerigee(), phi1poca),
                           PointOnTrack (mytracks.getSecondPerigee(),phi2poca));
  }


  double Trk2dDistanceSeeder::getBField (const Perigee& p, MagField::AtlasFieldCache& cache) const
  {
    double magnFieldVect[3];
    double posXYZ[3];
    posXYZ[0] = p.associatedSurface().center().x();
    posXYZ[1] = p.associatedSurface().center().y();
    posXYZ[2] = p.associatedSurface().center().z();

    cache.getField(posXYZ,magnFieldVect);
    const double bfield = magnFieldVect[2]*299.792;//should be in GeV/mm
    if (bfield==0. || isnan(bfield)) {
      ATH_MSG_DEBUG( "Could not find a magnetic field different from zero: very very strange" );
      return 0.60407; //Value in GeV/mm (ATLAS units)
    } 
      ATH_MSG_DEBUG( "Magnetic field projection of z axis in the perigee position is: " << bfield << " GeV/mm " );
    
    return bfield;
  }


} // namespace Trk
