/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


  ////////////////////////////////////////////////////////////////
  //                                                            //
  //  Implementation of class TrackPropagationHelper            //
  //                                                            //
  //  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
  //          Initial extrapolation code by Thijs Cornelissen   //
  //  Initial version: February 2008                            //
  //                                                            //
  ////////////////////////////////////////////////////////////////

#include "VP1TrackSystems/TrackPropagationHelper.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkTrack/Track.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkParameters/TrackParameters.h"
#include <stdexcept>
//#include "TrkEventPrimitives/GlobalMomentum.h"
#include "VP1TrackSystems/VP1TrackSanity.h"
#include "TrkEventPrimitives/CurvilinearUVT.h"
#include "TrkVolumes/Volume.h"
#include "TrkVolumes/BoundarySurface.h"
#include "TrkVolumes/VolumeBounds.h"


#include "TrkDetDescrUtils/ObjectAccessor.h"
#include "VP1Base/VP1QtUtils.h"
#include <limits>

//____________________________________________________________________
class TrackPropagationHelper::Imp {
public:
  Imp(TrackPropagationHelper* tc)
    : theclass(tc),
    tracksanity(new VP1TrackSanity(tc?tc->systemBase():0)),
      maxInDetr(1.1*CLHEP::m),
      maxInDetrsq(maxInDetr*maxInDetr),
      maxz(3.5*CLHEP::m),
    // maxR2(std::numeric_limits<float>::max()),
    fallback_flydist(10.0*CLHEP::cm), 
    showExtrapSurfaces(VP1QtUtils::environmentVariableIsOn("VP1_TRKSYS_SHOWEXTRAPSURFACES")) 
  {
  }
  ~Imp() { delete tracksanity; }

  TrackPropagationHelper * theclass;
  VP1TrackSanity * tracksanity;
  const double maxInDetr;
  const double maxInDetrsq;
  const double maxz;
  // double maxR2;
  const double fallback_flydist;

  bool outsideIDVolume(const Amg::Vector3D& p) {
    return fabs(p.z())>maxz||p.perp2()>maxInDetrsq;
  }

    //Fixme: These are similar to internal methods in InDetProjHelper in guideline system!!:
  void movePoint1ToZPlaneAndPoint2( Amg::Vector3D& p1, const Amg::Vector3D& p2, const double& z ) const;
  void movePoint1ToInfiniteCylinderAndPoint2( Amg::Vector3D&p1, const Amg::Vector3D&p2, const double& r ) const;

  bool makePointsNeutral_SinglePar( std::vector<Amg::Vector3D >& points, const Trk::Track* );
  bool makePointsCharged_SinglePar( std::vector<Amg::Vector3D >& points, const Trk::Track*,
            Trk::IExtrapolator * extrapolator, Trk::ParticleHypothesis hypo );
  bool addPointsBetweenParameters_Charged( std::vector<Amg::Vector3D >& points, const Trk::Track*,
             const Trk::TrackParameters * par1, const Trk::TrackParameters * par2,
             Trk::IExtrapolator * extrapolator, Trk::ParticleHypothesis hypo );

    //Will create new track par:
  const Trk::TrackParameters * extrapolateToNewPar( Trk::IExtrapolator * extrapolator,
                const Trk::Track* trk,
                const Trk::TrackParameters * prevpars,
                Trk::ParticleHypothesis hypo,
                const double& dist );
  //Granularity:
  double maxPointDistSq(const Amg::Vector3D& /* p */){
      //Vertex region:
    /** all paths return the same number
    const double absz(fabs(p.z())), absrsq(p.perp2());
    if ( (absz<2.75*CLHEP::m) and (absrsq<(1.2*CLHEP::m)*(1.2*CLHEP::m)) ) {
      if ( (absz<0.67*CLHEP::m) and (absrsq<(0.16*CLHEP::m)*(0.16*CLHEP::m)) ) {
        return 2*CLHEP::cm;//inside pixel
      }
      return 2*CLHEP::cm;//ID outside pixel
    }
    **/
    return 2*CLHEP::cm;//Outside ID
  }
  
  std::vector<Trk::PlaneSurface> surfaces; //!< For debugging.
  bool showExtrapSurfaces;
};

std::vector<Trk::PlaneSurface>& TrackPropagationHelper::getExtrapolationSurfaces() const {
  return m_d->surfaces;
}

//____________________________________________________________________
void TrackPropagationHelper::Imp::movePoint1ToZPlaneAndPoint2( Amg::Vector3D& p1, const Amg::Vector3D& p2, const double& z ) const
{
  double dx(p2.x()-p1.x()), dy(p2.y()-p1.y()), dz(p2.z()-p1.z());
  if (dz==0.0) {
    theclass->message("movePoint1ToZPlaneAndPoint2 Error: Points have same z!!");
    return;
  }
  double s( (z-p1.z())/dz );
  // p1.set( p1.x()+dx*s, p1.y()+dy*s, z );
  p1[0]= p1.x()+dx*s;
  p1[1]= p1.y()+dy*s;
  p1[2]= z;
  
}

  //____________________________________________________________________
void TrackPropagationHelper::Imp::movePoint1ToInfiniteCylinderAndPoint2( Amg::Vector3D&p1, const Amg::Vector3D&p2, const double& r ) const
{
    //Fixme: what happens here if we don't cross? And how can we be sure
    //that we don't move FURTHER than the other point? (i.e. if the
    //infinite line with p1 and p2 crosses, but the segment p1p2 does
    //not!?
  double p1r(p1.perp());
  double dr(p2.perp()-p1r);
  if (dr==0.0) {
    theclass->message("movePoint1ToInfiniteCylinderAndPoint2 Error: Points have same r!!");
    return;
  }
  double s((r-p1r)/dr);
  double t(1.0-s);
  // p1.set( p1.x()*t + p2.x()*s, p1.y()*t + p2.y()*s, p1.z()*t + p2.z()*s );
  p1[0]= p1.x()*t + p2.x()*s;
  //p1[1]= p2.y()*s, p1.z()*t; original
  p1[1]= p1.y()*t + p2.y()*s; //sroe, compiler warning fix
  p1[2]= p1.z()*t + p2.z()*s;
}

  //____________________________________________________________________
TrackPropagationHelper::TrackPropagationHelper(IVP1System* sys )
  : VP1HelperClassBase(sys,"TrackPropagationHelper"), m_d(new Imp(this))
{
}

  //____________________________________________________________________
TrackPropagationHelper::~TrackPropagationHelper()
{
  delete m_d;
}

  //____________________________________________________________________
bool TrackPropagationHelper::makePointsNeutral( std::vector<Amg::Vector3D >& points, const Trk::Track* track )
{
  if (verbose())
    messageVerbose("makePointsNeutral start");

  points.clear();
  if (!track) {
    message("makePointsNeutral called with null track pointer");
    return false;
  }
  const unsigned npars = track->trackParameters()->size();
  if (npars==0) {
    message("makePointsNeutral Error: No TrackParameters on track!");
    return false;
  }
  if (npars==1)
    return m_d->makePointsNeutral_SinglePar(points,track);

  points.reserve(track->trackParameters()->size());


  DataVector<const Trk::TrackStateOnSurface>::const_iterator tsos_iter = track->trackStateOnSurfaces()->begin();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator tsos_end = track->trackStateOnSurfaces()->end();
  bool problems(false);
  for (; tsos_iter != tsos_end; ++tsos_iter) {
    if (!m_d->tracksanity->isSafe(*tsos_iter)) {
      problems = true;
      continue;
    }
    //if ((*tsos_iter)->materialEffectsOnTrack())
    //  continue;
    const Trk::TrackParameters* trackParam = (*tsos_iter)->trackParameters();
    if (!m_d->tracksanity->isSafe(trackParam)) {
      problems = true;
      continue;
    }
    points.push_back(trackParam->position());
  }
  if (problems) {
    messageDebug("makePointsNeutral WARNING: Track had one or more track parameters which is unsafe to use in job.");
    if (points.size()<2) {
      messageDebug("makePointsNeutral ERROR: Track did not have at least two safe parameters.");
      points.clear();
      return false;
    }
  }

  if (verbose())
    messageVerbose("makePointsNeutral_SinglePar end");
  return true;
}

  //____________________________________________________________________
bool TrackPropagationHelper::Imp::makePointsNeutral_SinglePar( std::vector<Amg::Vector3D >& points, const Trk::Track* track )
{
  if (theclass->verbose())
    theclass->messageVerbose("makePointsNeutral_SinglePar start");
  points.clear();
  const Trk::TrackParameters * par = *(track->trackParameters()->begin());
  if (!tracksanity->isSafe(par)) {
    theclass->messageDebug("makePointsNeutral_SinglePar called with unsafe track parameter");
    return false;
  }
  Amg::Vector3D u(par->momentum().unit());
  Amg::Vector3D a(par->position()),b;
  if (outsideIDVolume(a)) {
      //We start outside. We just add fallback_flydist. (fixme: warn?)
    b = a +fallback_flydist*u;
  } else {
      //Make a second point, where trajectory hits maxInDetr,maxz
    b = a +999*CLHEP::m*u;
    if (b.z()<-maxz)
      movePoint1ToZPlaneAndPoint2( b, a, -maxz );
    else
      if (b.z()>maxz)
  movePoint1ToZPlaneAndPoint2( b, a, maxz );
    if (b.perp2()>maxInDetrsq)
      movePoint1ToInfiniteCylinderAndPoint2( b,a, maxInDetr );
  }
  points.reserve(2);
  points.push_back(a);
  points.push_back(b);
  if (theclass->verbose())
    theclass->messageVerbose("makePointsNeutral_SinglePar (single track parameter) end");
  return true;
}

  //____________________________________________________________________
bool TrackPropagationHelper::makePointsCharged( std::vector<Amg::Vector3D >& points, const Trk::Track* track,
            Trk::IExtrapolator * extrapolator, Trk::ParticleHypothesis hypo, bool useMEOT,const Trk::Volume* volume )
{
  if (verbose())
    messageVerbose("makePointsCharged start with hypo="+str(hypo)+", useMEOT="+str(useMEOT)+", volume=" +str(volume));
  // if (volume) std::cout<<volume->volumeBounds()<<std::endl;
    ///////////////////////////////////////////////////////////////
    //  Clear input points, check/sanitise input and initialise  //
    ///////////////////////////////////////////////////////////////

  points.clear();
  if (!extrapolator) {
    message("makePointsCharged ERROR: Null extrapolator tool provided!");
    return false;
  }
  if (!track) {
    message("makePointsCharged ERROR: Called with null track pointer");
    return false;
  }
  const unsigned npars = track->trackParameters()->size();
  if (npars==0) {
    message("makePointsCharged ERROR: No TrackParameters on track!");
    return false;
  }
  if (npars==1)
    return m_d->makePointsCharged_SinglePar(points,track,extrapolator,hypo);

  points.reserve(npars);//At least we need this.

    //Add a point for each parameter, and add extra points between them where appropriate.
  DataVector<const Trk::TrackStateOnSurface>::const_iterator tsos_iter = track->trackStateOnSurfaces()->begin();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator tsos_end = track->trackStateOnSurfaces()->end();
  const Trk::TrackParameters* prevpar(0);
  const Trk::TrackParameters* trackParam(0);
  bool problems(false);
  for (; tsos_iter != tsos_end; ++tsos_iter) {
    if (!m_d->tracksanity->isSafe(*tsos_iter))
      continue;
    if ((*tsos_iter)->measurementOnTrack()==0 && ( (*tsos_iter)->materialEffectsOnTrack()&&!useMEOT ) )
      continue;
    trackParam = (*tsos_iter)->trackParameters();
    if (!m_d->tracksanity->isSafe(trackParam))
      continue;
    if (!prevpar) {
        //first time.
      prevpar = trackParam;
      points.push_back(prevpar->position());
      continue;
    }
    if (!m_d->addPointsBetweenParameters_Charged(points,track,prevpar,trackParam,extrapolator,hypo))
      problems = true;
    points.push_back(trackParam->position());
    prevpar = trackParam;
  }

  if (problems)
    messageDebug("WARNING: Problems encountered adding point(s) between track parameters");

// restrict to ID tracks for now.
  if (volume && trackParam && !m_d->outsideIDVolume(trackParam->position())) {
    messageVerbose("Extending to Volume");
    //get individual surfaces

    //TODO - optimise this!
    const std::vector< const Trk::Surface * > * bsurfs = volume->volumeBounds ().decomposeToSurfaces (volume->transform ());

    if (bsurfs){
      messageVerbose("Has this many surfaces:"+str(bsurfs->size()));
      
      std::vector< const Trk::Surface * >::const_iterator bSurfsIt = bsurfs->begin();
      for (;bSurfsIt!= bsurfs->end(); bSurfsIt++){

        messageVerbose("Extrap value:"+str((extrapolator)));
        messageVerbose("trackParam:"+str((trackParam)));
        const Trk::TrackParameters*  trackPar = extrapolator->extrapolate(*trackParam,**bSurfsIt,Trk::alongMomentum,true,hypo); // change this to extrapolate current param to surface.

        if (trackPar){
          messageVerbose("Extrapolation succeeded");
          
          if (!m_d->addPointsBetweenParameters_Charged(points,track,trackParam,trackPar,extrapolator,hypo))
            messageDebug("WARNING: Problems encountered adding point(s) between track parameters in extending to Volume");
        }
      }
    } else {
      messageDebug("WARNING: Problems encountered getting boundary surfaces from Volume");
    }
  }
  if (verbose())
    messageVerbose("makePointsCharged end with "+str(points.size())+"points");
  return true;
}

  //____________________________________________________________________
const Trk::TrackParameters * TrackPropagationHelper::Imp::extrapolateToNewPar( Trk::IExtrapolator * extrapolator,
                         const Trk::Track* trk,
                         const Trk::TrackParameters * prevpars,
                         Trk::ParticleHypothesis hypo,
                         const double& dist )
{
  if (!trk||!prevpars||!extrapolator)
    return 0;


  //Code from thijs begin
  // CLHEP::Hep3Vector dir(prevpars->sinTheta()*prevpars->cosPhi(),prevpars->sinTheta()*prevpars->sinPhi(),prevpars->cosTheta());
  //  double theta=prevpars->parameters()[Trk::theta];
  //  double newtheta=(theta<M_PI/2.) ? theta+M_PI/2. : theta-M_PI/2.;
  //  CLHEP::Hep3Vector dir2(sin(newtheta)*prevpars->cosPhi(),sin(newtheta)*prevpars->sinPhi(),cos(newtheta));
  //  CLHEP::Hep3Vector dir3=dir.cross(dir2);
  //  CLHEP::HepRotation rot;
  //  rot.rotateAxes(dir2,dir3,dir);
  //  Hep3Vector transl=prevpars->position()+dist*dir;
  //  HepTransform3D *transf=new HepTransform3D(rot,transl);
  //  Trk::PlaneSurface plsurf(transf);
  //Code from thijs end
  //   //Alternative test code from thomas begin
  //   //(shows same issues as thijs' code unfortunately)
  // Ed's attempt:
  // HepGeom::Rotate3D    rotation;
  //  HepGeom::Scale3D     scale;
  //  HepGeom::Translate3D translation;
  //  Trk::Surface* surf= prevpars->associatedSurface()->clone();
  //  surf->transform ().getDecomposition(scale,rotation,translation);
  //  translation = HepTranslate3D(prevpars->position()+prevpars->momentum().unit()*dist);

  Trk::CurvilinearUVT uvt(prevpars->momentum().unit());
  
  
  Amg::Transform3D*  t  = new Amg::Transform3D(uvt.curvU(),uvt.curvV(),uvt.curvT(), prevpars->position()+(prevpars->momentum().unit()*dist));
  
  Trk::PlaneSurface surf(t);
  
  // Trk::PlaneSurface surf(new HepGeom::Transform3D(CLHEP::HepRotation(uvt.curvU(),uvt.curvV(),uvt.curvT()),
  //                                           prevpars->position()+(prevpars->momentum().unit()*dist)));
  
  if (showExtrapSurfaces) surfaces.push_back(surf);
  
  // theclass->messageVerbose("extrapolateToNewPar. current position: "+str(prevpars->position())+" ("+str(prevpars->position().mag())+"), dist="+str(dist));
  // theclass->messageVerbose("new position: "+str(surf.center())+", using dir="+str(prevpars->momentum().unit()*dist));
    
  // if ((prevpars->position()-surf.center()).mag()<0.01){
  //   theclass->messageVerbose("Bugger. Translation didn't work. ");
  //   return 0;
  // }
  const Trk::TrackParameters *newpars(0);
  try {
    // newpars = extrapolator->extrapolate(*trk,surf,Trk::anyDirection,false,hypo); // change this to extrapolate current param to surface.
    newpars = extrapolator->extrapolate(*prevpars,surf,Trk::alongMomentum,false,hypo); // change this to extrapolate current param to surface.
  } catch (std::runtime_error e) {
    theclass->message("Failure trying to use extrapolator for track (Exception thrown: " + QString(e.what())+")");
    return 0;
  }
  if (!newpars) {
    theclass->message("Failure trying to use extrapolator for track");
    return 0;
  }

    //in case this lead to a large kink, we try a different, less ambitous, method:
  // Amg::Vector3D actualdirp((newpars->position()-prevpars->position()).unit());
  // const double dot = actualdirp.x()*dir.x()+actualdirp.y()*dir.y()+actualdirp.z()*dir.z();
  // if (dot<0.866025403784439) {//>30deg
  //   theclass->messageVerbose("Detected kink. Trying alternative extrapolation)");
  //   delete newpars;
  //   newpars = 0;
  //   HepTransform3D *transf2=new HepTransform3D(rot,transl);
  //   Trk::PlaneSurface plsurf2(transf2);
  //   try {
  //     newpars = extrapolator->extrapolateDirectly(*prevpars,plsurf2,Trk::anyDirection,false,hypo);
  //   } catch (std::runtime_error e) {
  //     theclass->message("Failure trying to use alternative extrapolation method for track (Exception thrown: " + QString(e.what())+")");
  //     return 0;
  //   }
  // }


  if (!newpars) {
    theclass->message("Failure trying to use extrapolator for track");
    return 0;
  }
  return newpars;
}

bool TrackPropagationHelper::showExtrapolationSurfaces() const {
  return m_d->showExtrapSurfaces;
}

// double TrackPropagationHelper::maxR2ForTracks() const {
//   return m_d->maxR2;
// }


  //____________________________________________________________________
bool TrackPropagationHelper::Imp::makePointsCharged_SinglePar( std::vector<Amg::Vector3D >& points, const Trk::Track* track,
                     Trk::IExtrapolator * extrapolator, Trk::ParticleHypothesis hypo )
{
  if (theclass->verbose())
    theclass->messageVerbose("makePointsCharged_SinglePar start");
  points.clear();
  if (!extrapolator) {
    theclass->message("makePointsCharged_SinglePar ERROR: Null extrapolator tool provided!");
    return false;
  }
  const Trk::TrackParameters * par = *(track->trackParameters()->begin());
  if (!tracksanity->isSafe(par)) {
    theclass->messageDebug("makePointsCharged_SinglePar called with unsafe track parameter");
    return false;
  }

  Amg::Vector3D p0(par->position());
  points.push_back(p0);
  const bool startoutsideID(outsideIDVolume(p0));

  double distadded(0);
  const double maxdistadded = 2*CLHEP::m;
  points.reserve(20);//skip a few reallocs
  const Trk::TrackParameters * prevpars = par;
  const Trk::TrackParameters * temppars(0);

  while (distadded<maxdistadded) {
    temppars = extrapolateToNewPar( extrapolator, track, prevpars, hypo, maxPointDistSq(prevpars->position()));
    if (temppars)
      distadded += (temppars->position()-prevpars->position()).mag();
    if (prevpars!=par)
      delete prevpars;
    prevpars = 0;
    if (!temppars) {
      theclass->messageDebug("makePointsCharged_SinglePar ERROR: Failed to use extrapolator for next point");
      if (points.size()<2) {
  points.clear();
  return false;
      }
      return true;//Fixme??
    }
    Amg::Vector3D p(temppars->position());
    points.push_back(p);
    prevpars = temppars;
    temppars = 0;
    if (!startoutsideID && outsideIDVolume(p)) {
      if (prevpars!=par)
  delete prevpars;
      prevpars = 0;
      break;
    }
  }
  if (prevpars!=par)
    delete prevpars;

  if (verbose())
    theclass->messageVerbose("makePointsCharged_SinglePar end");
  return true;

}

  //____________________________________________________________________
bool TrackPropagationHelper::Imp::addPointsBetweenParameters_Charged( std::vector<Amg::Vector3D >& points, const Trk::Track* trk,
                      const Trk::TrackParameters * par1, const Trk::TrackParameters * par2,
                      Trk::IExtrapolator * extrapolator, Trk::ParticleHypothesis hypo )
{
  double distbetween = sqrt((par2->position()-par1->position()).mag2());
  if (distbetween<0.001) {
    theclass->messageVerbose("TrackPropagationHelper::Imp::addPointsBetweenParameters_Charged: parameters on top of each other. Skip, but no error.");
    return true;
  }
  // theclass->messageVerbose("TrackPropagationHelper::Imp::addPointsBetweenParameters_Charged: par1="
  //   +str(par1->position())+", par2=" +str(par2->position())+", dist="+str(distbetween));
  
  Amg::Vector3D p2(par2->position());
  const Trk::TrackParameters * prevpars = par1;

  double olddistsq(1.0e99);
  double distadded(0);
  const double maxdistadded = std::max(2*CLHEP::m,(par1->position()-p2).mag()*1.5);
  while ( (prevpars->position()-p2).mag()> maxPointDistSq(prevpars->position()) && distadded<maxdistadded ) {
    
    // theclass->messageVerbose("distadded: "+str(distadded)+", distance left="+str(sqrt((prevpars->position()-p2).mag2()))+", jump="+str(maxPointDistSq(prevpars->position())));
    const Trk::TrackParameters * newpars = extrapolateToNewPar( extrapolator, trk, prevpars, hypo, maxPointDistSq(prevpars->position()) );
    if (!newpars){
      if (verbose())
        theclass->messageVerbose("TrackPropagationHelper::Imp::addPointsBetweenParameters_Charged: Extrapolation failed.");
      return false;
    }
    const double distsq = (par2->position()-newpars->position()).mag2();
    if (distsq>olddistsq) {
      delete newpars;
      if (verbose())
        theclass->messageVerbose("TrackPropagationHelper::Imp::addPointsBetweenParameters_Charged: distq("+str(distsq)+")>olddistsq ("+str(olddistsq)+") so overshot?");
      return false;
    }
    olddistsq = distsq;
      //     if ((prevpars->position()-newpars->position()).mag2() > 4*maxpointdistsq) {
      //       delete newpars;
      //       return false;
      //     }
    points.push_back(newpars->position());
    distadded += (newpars->position()-prevpars->position()).mag();
    if (prevpars!=par1)
      delete prevpars;
    prevpars = newpars;
  }
  if (prevpars!=par1)
    delete prevpars;
  return (distadded<maxdistadded);
}
