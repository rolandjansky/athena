/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRT_SegmentToTrackTool/TRT_SegmentToTrackTool.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "InDetIdentifier/TRT_ID.h"
#include "TrkSurfaces/Surface.h"

///Needed for the track refitter
#include "TrkFitterInterfaces/ITrackFitter.h"
//Extrapolator tool
#include "TrkExInterfaces/IExtrapolator.h"
//Association tool
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
//Scoring tool
#include "TrkToolInterfaces/ITrackScoringTool.h"
//Magnetic field tool
#include "MagFieldInterfaces/IMagFieldSvc.h"

using Amg::Vector3D;
using CLHEP::mm;

namespace InDet {

  // Constructor with parameters:
  TRT_SegmentToTrackTool::TRT_SegmentToTrackTool(const std::string &type,
							     const std::string &name,
							     const IInterface *parent) :
    AthAlgTool(type,name,parent),
    m_fieldUnitConversion(1000.),
    m_fitterTool("Trk::KalmanFitter/InDetTrackFitter"),
    m_extrapolator ("Trk::Extrapolator/InDetExtrapolator"),
    m_assotool("InDet::InDetPRD_AssociationToolGangedPixels"),
    m_scoringTool("Trk::TrackScoringTool/TrackScoringTool"),
    m_magFieldSvc("AtlasFieldSvc", name)
  {
    declareInterface<InDet::ITRT_SegmentToTrackTool>( this );

    m_doRefit            = false                                ;       //Do a final careful refit of tracks
    m_useasso            = false                                ;       //Use association tool to clean up the seeds

    m_suppressHoleSearch = false                                ;       //Suppress hole search
    m_sharedFrac         = 0.3                                  ;       //Maximum fraction of shared hits !!!!!!!!!!!!!!!!!!!!!! offline 0.3!!!!!!!!!!!!!!!!!!!!!!!

    declareProperty("FinalRefit"                 ,m_doRefit           ); //Do a final careful refit of tracks
    declareProperty("UseAssociationTool"         ,m_useasso           ); //Use the association tool

    declareProperty("SuppressHoleSearch"         ,m_suppressHoleSearch); //Suppress hole search during the track summary creation
    declareProperty("MaxSharedHitsFraction"      ,m_sharedFrac        ); //Maximum fraction of shared drift circles

    declareProperty("RefitterTool"               ,m_fitterTool        ); //Track refit tool
    declareProperty("Extrapolator"               ,m_extrapolator      ); //Extrapolator tool
    declareProperty("AssociationTool"            ,m_assotool          ); //Association tool
    declareProperty("ScoringTool"                ,m_scoringTool       ); //Track scoring tool

    declareProperty( "MagFieldSvc"              ,m_magFieldSvc      ); //Magnetic field tool

  }

  TRT_SegmentToTrackTool::~TRT_SegmentToTrackTool()
  {}


  StatusCode TRT_SegmentToTrackTool::initialize() {

    ATH_CHECK( AthAlgTool::initialize() );

    ATH_MSG_DEBUG( "Initializing TRT_SegmentToTrackTool" );

    //Get the refitting tool
    //
    if(m_doRefit){
      ATH_CHECK( m_fitterTool.retrieve() );
      ATH_MSG_INFO( "Retrieved tool " << m_extrapolator );
    }

    ATH_CHECK(  m_extrapolator.retrieve() );

    // Get association tool
    //
    if(m_useasso){
      ATH_CHECK( m_assotool.retrieve() );
    }

    // Get the scoring tool
    //
    ATH_CHECK( m_scoringTool.retrieve() );

    ATH_CHECK( m_magFieldSvc.retrieve() );

    ATH_CHECK( detStore()->retrieve(m_trtId, "TRT_ID") );

    // Get output print level
    //
    ATH_MSG_DEBUG( *this );

    return StatusCode::SUCCESS;

  }

  StatusCode InDet::TRT_SegmentToTrackTool::finalize(){
    StatusCode sc = AthAlgTool::finalize();
    return sc;
  }


  ///////////////////////////////////////////////////////////////////
  // Dumps relevant information into the MsgStream
  ///////////////////////////////////////////////////////////////////
  MsgStream&  InDet::TRT_SegmentToTrackTool::dump( MsgStream& out ) const {
    out<<std::endl;
    return dumpconditions(out);
  }

  ///////////////////////////////////////////////////////////////////
  // Dumps conditions information into the MsgStream
  ///////////////////////////////////////////////////////////////////

  MsgStream& InDet::TRT_SegmentToTrackTool::dumpconditions( MsgStream& out ) const
  {
    int n = 65-m_fitterTool.type().size();
    std::string s1; for(int i=0; i<n; ++i) s1.append(" "); s1.append("|");
    n     = 65-m_assotool.type().size();
    std::string s2; for(int i=0; i<n; ++i) s2.append(" "); s2.append("|");
    n     = 65-m_scoringTool.type().size();
    std::string s5; for(int i=0; i<n; ++i) s5.append(" "); s5.append("|");


    out<<std::endl
       <<"|----------------------------------------------------------------------"
       <<"-------------------|"<<std::endl;
    out<<"| Tool for final track refitting    | "<<m_fitterTool.type()   <<s1<<std::endl;
    out<<"| Tool for track scoring            | "<<m_scoringTool.type()  <<s5<<std::endl;
    out<<"| Association tool                  | "<<m_assotool.type()     <<s2<<std::endl;
    out<<"|----------------------------------------------------------------------"
       <<"-------------------|";
    return out;
  }


  ///////////////////////////////////////////////////////////////////
  // Dumps event information into the MsgStream
  ///////////////////////////////////////////////////////////////////
  MsgStream& InDet::TRT_SegmentToTrackTool::dumpevent( MsgStream& out ) const {
    return out;
  }

  ///////////////////////////////////////////////////////////////////
  // Dumps relevant information into the ostream
  ///////////////////////////////////////////////////////////////////

  std::ostream& InDet::TRT_SegmentToTrackTool::dump( std::ostream& out ) const
  {
    return out;
  }


  Trk::Track* TRT_SegmentToTrackTool::segToTrack(const Trk::TrackSegment& tS) {


    ATH_MSG_DEBUG ("Transforming the TRT segment into a track...");
  
    //
    // Get the track segment fit quality. If not there drop segment
    //
    if (!tS.fitQuality()) {
      ATH_MSG_DEBUG ("Segment has no fit quality ! Discard...");
      return 0;
    }
    const Trk::FitQuality* fq = tS.fitQuality()->clone();

    //
    // Get the track segment information about the initial track parameters
    //
    const AmgVector(5)&             par  = tS.localParameters();
    AmgSymMatrix(5)*                ep   = new AmgSymMatrix(5)(tS.localCovariance());
    const Trk::TrackParameters* segPar = tS.associatedSurface().createTrackParameters(par[Trk::loc1],
										      par[Trk::loc2],
										      par[Trk::phi],
										      par[Trk::theta],
										      par[Trk::qOverP], ep); 

    if(segPar) {
      ATH_MSG_VERBOSE ("Initial TRT Segment Parameters : " << (*segPar));
    }else{
      ATH_MSG_DEBUG ("Could not get initial TRT segment parameters! ");
      // clean up
      delete fq; fq = 0;
      delete ep; ep = 0;
      return 0;
    }

    // --- create new track state on surface vector
    DataVector<const Trk::TrackStateOnSurface>* ntsos = new DataVector<const Trk::TrackStateOnSurface>;

    //
    // if no refit, make it a perigee
    //
    if (!m_doRefit) {

      const Trk::TrackStateOnSurface* par_tsos = 0;

      // --- create surface at perigee
      Amg::Vector3D perigeePosition(0.,0.,0.);
      Trk::PerigeeSurface perigeeSurface(perigeePosition);
      // --- turn parameters into perifgee... 
      const Trk::Perigee* perParm = dynamic_cast<const Trk::Perigee*>( m_extrapolator->extrapolate(*segPar, perigeeSurface) );
      if(perParm) {
	ATH_MSG_VERBOSE ("Perigee version of Parameters : " << (*segPar));
      } else {
	ATH_MSG_DEBUG ("Failed to build perigee parameters.Discard...");
	delete ntsos; ntsos = 0;
	delete segPar; segPar = 0;
	delete fq; fq = 0;
	return 0;
      }

      // now create a perigee TSOS
      std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
      typePattern.set(Trk::TrackStateOnSurface::Perigee);
      par_tsos = new Trk::TrackStateOnSurface(0,perParm,0,0,typePattern);
      // push new TSOS into the list
      ntsos->push_back(par_tsos);
    }

    //
    // now loop over the TSOS and copy them in
    //

    // psuedo measurement information 
    int                        npseudo     = 0;
    double                     pseudotheta = 0;
    const Trk::MeasurementBase *pseudo     = 0;
    // other measurement information
    const Trk::Surface         *firstsurf=0,*firstecsurf=0,*lastsurf=0;
    const Trk::MeasurementBase *firstmeas=0; 
    // counters for barrel and endcap
    int nbarrel=0,nendcap=0;
    // some variables for endcaps
    std::vector<std::pair<double,double> > points;
    points.reserve(40);
    double oldphi=0;


    // loop over the measurements in track segment (tS)
    for(int it=0; it<int(tS.numberOfMeasurementBases()); it++){

      // the track state on service we like to constuct ...
      const Trk::TrackStateOnSurface* seg_tsos = 0;
    
      // is this ROT a psuedo-measurement ?
      if ( dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(tS.measurement(it)) ) {
	// did we have a speudo-measurement before ?
	if (pseudo) {
	  // get theta from pseudo measurements
	  pseudotheta=atan2(tS.measurement(it)->associatedSurface().center().perp(),
			    tS.measurement(it)->associatedSurface().center().z());
	}
	// keep this pseudo measurement
	pseudo=tS.measurement(it);
	// update counter
	npseudo++;

	if(m_doRefit) {
	  // refit means we can simply copy the state, otherwise we skip it
	  seg_tsos = new Trk::TrackStateOnSurface(tS.measurement(it)->clone(),0);
	}

      } else {
	//
	// normal measurement, not a pseudo measurement
	//
	// copy measurement
	seg_tsos = new Trk::TrackStateOnSurface(tS.measurement(it)->clone(),0);
    
	//
	// --- following is for the hack below
	//

	// remember first measurement
	if (!firstmeas) firstmeas = tS.measurement(it);
	if (!firstsurf) firstsurf = &tS.measurement(it)->associatedSurface();
	// it is always the last one
	lastsurf = &tS.measurement(it)->associatedSurface();
      
	// this is a rubbish way to find out it is endcap
	if (fabs(tS.measurement(it)->associatedSurface().transform().rotation().col(2).z())<.5) {
	  // increase counter and keep some information
	  nendcap++;
	  if (!firstecsurf) firstecsurf=&tS.measurement(it)->associatedSurface();
        
	  double tmpphi=tS.measurement(it)->associatedSurface().center().phi();
	  if (!points.empty() && fabs(tmpphi-oldphi)>M_PI){ // correct for boundary at +/- pi
	    if (tmpphi<0) tmpphi+=2*M_PI;
	    else tmpphi-=2*M_PI;
	  }
	  // remember oldphi
	  oldphi=tmpphi;
	  //std::cout << "oldphi: " << oldphi << " tmpphi: " << tmpphi << std::endl;
	
	  // copy the points
	  points.push_back(std::make_pair(tS.measurement(it)->associatedSurface().center().z(),tmpphi));
	
	} else nbarrel++;

	//
	// --- end of hack stuff  
	//
      
      }

      // push new TSOS into the list
      if (seg_tsos) ntsos->push_back(seg_tsos);
    
    }

    // Construct the new track
    Trk::TrackInfo info;
    info.setPatternRecognitionInfo(Trk::TrackInfo::TRTStandalone);

    // create new track candidate
    Trk::Track* newTrack = new Trk::Track(info, ntsos, fq);

    if (!m_doRefit) {
      return newTrack;

    } else {

      //
      // ----------------------------- this is a horrible hack to make the segments fittable
      //
    
      // in case of only 1 pseudo measurement, use the theta from it.
      if (npseudo==1) pseudotheta=pseudo->localParameters()[Trk::theta];

      // we need new perigee parameters
      double myqoverp=0,myphi=0,myd0=0,myz0=0, mytheta = pseudotheta;

      if (nendcap<4) {
	//
	// --- are we in the barrel mostly
	//
    
	// momentum
	myqoverp=par[4]*sin(pseudotheta)/sin(par[3]);    

	// --- create surface at perigee
	Amg::Vector3D perigeePosition(0.,0.,0.);
	Trk::PerigeeSurface perigeeSurface(perigeePosition);
	// -- get perigee
	const Trk::Perigee *tempper=dynamic_cast<const Trk::Perigee*>(m_extrapolator->extrapolateDirectly(*segPar, perigeeSurface));
	if (!tempper) {
	  ATH_MSG_DEBUG ("Could not produce perigee");
	  delete newTrack; newTrack = 0;
	  delete segPar; segPar = 0;
	  return 0;
	}
      
	// keep some values
	myd0   = tempper->parameters()[Trk::d0];
	myphi  = tempper->parameters()[Trk::phi0];

	// delete extrapolation
	delete tempper; 
	tempper = 0;

      } else {
	//
	// --- endcap or transition track
	//

	// get estimate of parameters
	double sx=0,sy=0,sxx=0,sxy=0,d=0;
	float zmin=0, zmax=0;
	// loop over all points 
	for (unsigned int i=0;i<points.size();i++) {
	  sx  += points[i].first;
	  sy  += points[i].second;
	  sxy += points[i].first*points[i].second;
	  sxx += points[i].first*points[i].first;
	  if (fabs(points[i].first)>fabs(zmax)){
	    zmax = points[i].first;
	  }
	  if (fabs(points[i].first)<fabs(zmin)){
	    zmin = points[i].first;
	  }
	}

	if (fabs(pseudotheta)<1.e-6) {
	  ATH_MSG_DEBUG("pseudomeasurements missing on the segment?");
	  const float Rinn= 644., Rout=1004.;
	  if (zmax*zmin>0.){
	    pseudotheta = atan2(Rout-Rinn,zmax-zmin);
	  }
	  else if (std::abs(zmax*zmin)<1.e-6){
	    if (std::abs(zmax)>1.e-6){
	      pseudotheta = atan2(Rout, zmax) ;
	    } else {
	      ATH_MSG_DEBUG("no points in endcap?");
	    }
	  }
	  else {
	    pseudotheta = atan2(2.*Rout,zmax-zmin);
	  }
	}

	// get q/p
	d             = (points.size()*sxx-sx*sx);
	double dphidz = ((points.size()*sxy-sy*sx)/d);
	myqoverp      = (fabs(pseudotheta)>1e-6) ? -dphidz/(0.6*tan(pseudotheta)) : 1000.;
	// std::cout << "pt: " << sin(pseudotheta)/myqoverp << std::endl;    

	// some geometry stuff to estimate further paramters...
	double halfz  = 200.;
	const Trk::CylinderBounds *cylb  = dynamic_cast<const Trk::CylinderBounds *>(&firstsurf->bounds());
	if (!cylb) ATH_MSG_ERROR ("Cast of bounds failed, should never happen");
	else halfz  = cylb->halflengthZ();
	const Trk::CylinderBounds *cylb2 = dynamic_cast<const Trk::CylinderBounds *>(&lastsurf->bounds());
	double halfz2 = 200.;
	if (!cylb2) ATH_MSG_ERROR ("Cast of bounds failed, should never happen");
	else halfz2 = cylb2->halflengthZ();
	Amg::Vector3D strawdir1 = -firstsurf->transform().rotation().col(2);
	Amg::Vector3D strawdir2 = -lastsurf->transform().rotation().col(2);
	Amg::Vector3D pos1;
	Amg::Vector3D pos2;

	// ME: this is hardcoding, not nice and should be fixed
	if (fabs(lastsurf->center().z())<2650*mm) {
	  pos2 = lastsurf->center()+halfz2*strawdir2;
	  //std::cout << "pos2: " << pos2 << std::endl;
	  if (nbarrel==0){
	    double dr = fabs(tan(pseudotheta)*(lastsurf->center().z()-firstsurf->center().z()));
	    pos1      = firstsurf->center()+(halfz-dr)*strawdir1; 
	  } else {
	    double dz = fabs((pos2.perp()-firstsurf->center().perp())/tan(pseudotheta));
	    if (pos2.z()>0) dz = -dz;
	    double z1 = pos2.z()+dz;
	    pos1 = Amg::Vector3D(firstsurf->center().x(),firstsurf->center().y(),z1);
	  }
	} else {
	  double dr = fabs(tan(pseudotheta)*(lastsurf->center().z()-firstsurf->center().z()));
	  pos2      = lastsurf->center()+(dr-halfz2)*strawdir2;
	  pos1      = firstsurf->center()-halfz*strawdir1;
	}

	// ME: I don't understand this yet, why is this done only if barrel == 0, while above this nendcap < 4 ?
	if (nbarrel==0 &&
	    std::abs(tan(pseudotheta)*(firstsurf->center().z()-lastsurf->center().z())) < 250*mm &&
	    std::abs(firstsurf->center().z()) > 1000*mm) {

	  // ME: wow this is hacking the vector ... 
	  const Trk::MeasurementBase *firstmeas = (**ntsos->begin()).measurementOnTrack();
	  Amg::MatrixX newcov(2,2);
	  newcov.setZero();
	  newcov(0,0) = (firstmeas->localCovariance())(0,0);
	  newcov(1,1) = (myqoverp!=0) ? .0001*myqoverp*myqoverp : 1.e-8;
	  Trk::LocalParameters newpar(std::make_pair(0,Trk::locZ),std::make_pair(myqoverp,Trk::qOverP));
	  Trk::PseudoMeasurementOnTrack *newpseudo=new Trk::PseudoMeasurementOnTrack(newpar,newcov,firstmeas->associatedSurface());
	  // hack replace first measurement with pseudomeasurement
	  ntsos->erase(ntsos->begin());
	  ntsos->insert(ntsos->begin(),new Trk::TrackStateOnSurface(newpseudo,0));  
	}

	Amg::Vector3D field1;
	m_magFieldSvc->getField(Amg::Vector3D(.5*(pos1+pos2)).data(),field1.data());
	field1 *= m_fieldUnitConversion; // field in Tesla

	double phideflection = -.3*(pos2-pos1).perp()*field1.z()*myqoverp/sin(pseudotheta);
	double precisephi    = (nbarrel==0) ? (pos2-pos1).phi()-.5*phideflection : (pos2-pos1).phi()+.5*phideflection;
	double radius        = (myqoverp!=0. && field1.z()!=0.) ? -sin(pseudotheta)/(.3*field1.z()*myqoverp) : 1.e6;
	double precisetheta  = (myqoverp!=0.) ? atan2(std::abs(radius*phideflection),pos2.z()-pos1.z()) : pseudotheta;
	if (precisetheta < 0)     precisetheta += M_PI; 
	if (precisephi   < -M_PI) precisephi   += 2*M_PI;
	if (precisephi   > M_PI)  precisephi   -= 2*M_PI;

	// now extrapolate backwards from the first surface to get starting parameters
	const Trk::StraightLineSurface* surfforpar=0;
	if (nbarrel==0) surfforpar=dynamic_cast<const Trk::StraightLineSurface*>(firstsurf);
	else surfforpar=dynamic_cast<const Trk::StraightLineSurface*>(lastsurf);
	if (!surfforpar) ATH_MSG_ERROR ("Cast of surface failed, should never happen");

	Trk::AtaStraightLine ataline(((nbarrel==0) ? pos1 : pos2),precisephi,precisetheta,myqoverp,*surfforpar);
	Trk::PerigeeSurface persurf;
	const Trk::TrackParameters *extrappar=m_extrapolator->extrapolateDirectly(ataline,persurf);
	//std::cout << "ataline: " << ataline << " extrap par: " << *extrappar << std::endl;

	// now get parameters
	if (extrappar){
	  if (nendcap>=4){
	    myphi=extrappar->parameters()[Trk::phi0];
	    myd0=extrappar->parameters()[Trk::d0];
	  }

	  // construct theta again
	  double z0 = extrappar->parameters()[Trk::z0];
	  //mytheta=extrappar->parameters()[Trk::theta];
	  if (nbarrel==0) mytheta=atan(tan(extrappar->parameters()[Trk::theta])*std::abs((z0-pos1.z())/pos1.z()));
	  else mytheta=atan(tan(extrappar->parameters()[Trk::theta])*std::abs((z0-pos2.z())/pos2.z()));

	  if (mytheta<0) mytheta+=M_PI;

	  delete extrappar;
	  extrappar = 0;
	}
    
      }
      while (myphi> M_PI) myphi-=2*M_PI;
      while (myphi<-M_PI) myphi+=2*M_PI;

      double P[5]={myd0, myz0, myphi, mytheta, myqoverp};

      // create perigee TSOS and add as first (!) TSOS
      Trk::Perigee *per=new Trk::Perigee(P[0],P[1],P[2],P[3],P[4],Trk::PerigeeSurface());
      std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
      typePattern.set(Trk::TrackStateOnSurface::Perigee);
      Trk::TrackStateOnSurface *seg_tsos = new Trk::TrackStateOnSurface(0,per,0,0,typePattern);
      ntsos->insert(ntsos->begin(),seg_tsos);

      ATH_MSG_VERBOSE ("Constructed perigee at input to fit : " << (*per) );

      //
      // ------------------------------------------------------- now refit the track
      //
    
      // ME: this is a hack and should be replaced

      //Trk::ParticleSwitcher partSwitch;
      //Trk::ParticleHypothesis partHypothesis = partSwitch.particle[m_matEffects];
      //Trk::Track* fitTrack = m_fitterTool->fit(*newTrack,true,partHypothesis);

      Trk::Track* fitTrack = m_fitterTool->fit(*newTrack,true,Trk::nonInteracting);
    
      //cleanup
      delete newTrack; 
      newTrack = 0;
      if(segPar) {
	delete segPar;
	segPar = 0;
      }

      if(!fitTrack){
	ATH_MSG_DEBUG ("Refit of TRT track segment failed!");
	return 0;
      }

      //
      // -------------------------------------- hack the covarinace back to something reasonable
      //
      const Trk::TrackParameters *firstmeaspar=0;
      DataVector<const Trk::TrackParameters>::const_iterator parit = fitTrack->trackParameters()->begin();
      do {
	// skip pesudo measurements on perigee
	if ( (*parit)->covariance() && ((*parit)->associatedSurface()  == tS.associatedSurface())) firstmeaspar = *parit;
	++parit;
      } while (firstmeaspar==0 && parit != fitTrack->trackParameters()->end());

      //Create new perigee starting from the modified first measurement that has a more reasonable covariance matrix
      // const Trk::Perigee* perTrack=dynamic_cast<const Trk::Perigee*>(fitTrack->perigeeParameters());
      const Trk::Perigee* perTrack = fitTrack->perigeeParameters();

      if (!perTrack || !perTrack->covariance() ){
	ATH_MSG_ERROR ("Cast of perigee fails, should never happen !");
	return 0;
      }else {
	ATH_MSG_VERBOSE ("Perigee after refit with fudges to make it converge : " << (*perTrack) );

	if(firstmeaspar && firstmeaspar->position().perp()<2000*mm && std::abs(firstmeaspar->position().z())<3000*mm){

	  // Modify first measurement so that it has reasonable errors on z and theta
	  AmgSymMatrix(5)* fcovmat = new AmgSymMatrix(5)(*(firstmeaspar->covariance()));
	  // factors by which we like to scale the cov, this takes the original segment errors into account
	  double scaleZ     = sqrt(tS.localCovariance()(1,1))/sqrt( (*fcovmat)(1,1));
	  double scaleTheta = sqrt(tS.localCovariance()(3,3))/sqrt( (*fcovmat)(3,3));
	  // now do it
	  (*fcovmat)(1,0)=scaleZ*((*fcovmat)(1,0));
	  (*fcovmat)(0,1) = (*fcovmat)(1,0);
	  (*fcovmat)(1,1)=tS.localCovariance()(1,1);
	  (*fcovmat)(2,1)=scaleZ*((*fcovmat)(2,1));
	  (*fcovmat)(1,2) = (*fcovmat)(2,1);
	  (*fcovmat)(3,1)=scaleZ*scaleTheta*((*fcovmat)(3,1));
	  (*fcovmat)(1,3) = (*fcovmat)(3,1);
	  (*fcovmat)(4,1)=scaleZ*((*fcovmat)(4,1));
	  (*fcovmat)(1,4) = (*fcovmat)(4,1);
	  (*fcovmat)(3,0)=scaleTheta*((*fcovmat)(3,0));
	  (*fcovmat)(0,3) = (*fcovmat)(3,0);
	  (*fcovmat)(3,2)=scaleTheta*((*fcovmat)(3,2));
	  (*fcovmat)(2,3) = (*fcovmat)(3,2);
	  (*fcovmat)(3,3)=tS.localCovariance()(3,3);
	  (*fcovmat)(4,3)=scaleTheta*((*fcovmat)(4,3));
	  (*fcovmat)(3,4) = (*fcovmat)(4,3);

	  // const Amg::VectorX& par = firstmeaspar->parameters();
	  const AmgVector(5)& par = firstmeaspar->parameters();
	  const Trk::TrackParameters* updatedPars = firstmeaspar->associatedSurface().createTrackParameters(par[Trk::loc1],par[Trk::loc2],
													    par[Trk::phi],par[Trk::theta],par[Trk::qOverP],fcovmat); 


	  // now take parameters at first measurement and exptrapolate to perigee
	  const Trk::TrackParameters *newperpar   = m_extrapolator->extrapolate(*updatedPars,perTrack->associatedSurface(),
										Trk::anyDirection,false,Trk::nonInteracting);
	  delete updatedPars; updatedPars = 0;

	  if (!newperpar || !newperpar->covariance()) {
	    ATH_MSG_WARNING ("Can not hack perigee parameters, extrapolation failed");
	    delete newperpar; newperpar = 0;
	  } else {
	    // this is a HACK !!!
	    AmgSymMatrix(5)& errmat = const_cast<AmgSymMatrix(5)&>(*perTrack->covariance());
	    // overwrite cov in perTrack
	    errmat = *newperpar->covariance();
	    delete newperpar; newperpar = 0;
	    // check that new cov makes sense !
	    const AmgSymMatrix(5)& CM = *perTrack->covariance();
	    if( CM(1,1)==0.||CM(3,3)==0. ) {
	      ATH_MSG_DEBUG ("Hacked perigee covariance is CRAP, reject track");
	      delete fitTrack; return 0;
	    } else {
	      ATH_MSG_VERBOSE ("Perigee after fit with scaled covariance matrix : " << *perTrack);
	    }
	  }
	}
      }
      // return fitted track
      return fitTrack;
    }  

  }


  bool TRT_SegmentToTrackTool::segIsUsed(const Trk::TrackSegment& tS ) {

    ATH_MSG_DEBUG ("Checking whether the TRT segment has already been used...");

    // some counters to be handled
    int nShared = 0;  // Shared drift circles in segment
    int nHits   = 0;  // Number of TRT measurements

    // loop over the track states
    for(int it=0; it<int(tS.numberOfMeasurementBases()); ++it){
  
      // remove pseudo measurements
      if ( dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(tS.measurement(it)) )
        continue;
   
      // get the measurment
      const InDet::TRT_DriftCircleOnTrack* trtcircle = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(tS.measurement(it));
      if (!trtcircle) continue;
  
      // get PRD measurement
      const InDet::TRT_DriftCircle* RawDataClus=dynamic_cast<const InDet::TRT_DriftCircle*>(trtcircle->prepRawData());
      if(!RawDataClus) continue;
   
      // count up number of hits
      nHits++;
  
      if(m_useasso && m_assotool->isUsed(*RawDataClus)) nShared++;
    }
  
    if(nShared >= int(m_sharedFrac * nHits)) {
      ATH_MSG_DEBUG ("Too many shared hits.Will drop the TRT segment");
      return true;
    } else {
      return false;
    }


  }

  bool TRT_SegmentToTrackTool::toLower(const Trk::TrackSegment& tS) {

    ATH_MSG_DEBUG ("Try to recover low TRT DC segments in crack...");

    // counters
    int nEC = 0; int nBRL = 0; int firstWheel = -999; int lastLayer = -999;

    // loop over the track states
    for(int it=0; it<int(tS.numberOfMeasurementBases()); ++it){
    
      //test if it is a pseudo measurement
      if ( dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(tS.measurement(it)) )
	continue;
    
      // get measurement
      const InDet::TRT_DriftCircleOnTrack* trtcircle = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(tS.measurement(it));
      if(!trtcircle) continue;
    
      // get identifier
      Identifier id = trtcircle->detectorElement()->identify();
      // barrel or endcap
      int isB = m_trtId->barrel_ec(id);
      if      (isB==2 || isB==-2) {
	nEC++;
	if(nEC == 1)
	  firstWheel = m_trtId->layer_or_wheel(id);
      }
      else if (isB==1 || isB==-1) {
	nBRL++;
	lastLayer  = m_trtId->layer_or_wheel(id);
      }
     
    }
    
    // now the logic
    if((nEC>0  && nBRL>0) ||
       (nEC==0 && nBRL>0  && lastLayer<2) ||
       (nEC>0  && nBRL==0 && (firstWheel>10 || firstWheel<2))) {
      return true;
    } else {
      return false;
    }

  }

  void TRT_SegmentToTrackTool::resetAll() {

    //clear internal statistical counter
    m_nTrkScoreZero = 0;
    m_nTrkSegUsed = 0;
    m_nTRTTrk = 0;

    //this is a map which contains pointers to copies of all the input tracks
    m_trackScoreTrackMap.clear();

    //final copy - ownership is passed out of algorithm
    m_finalTracks = new TrackCollection;
 
    return;
  }

  void TRT_SegmentToTrackTool::resetAssoTool() {

    if(m_useasso) m_assotool->reset();
    return;

  }

  void TRT_SegmentToTrackTool::addNewTrack(Trk::Track* trk) {

    ATH_MSG_DEBUG ("Add track to the scoring multimap...");

    //Score the track under investigation
    Trk::TrackScore score = m_scoringTool->score(*trk,m_suppressHoleSearch);
    ATH_MSG_DEBUG ("TRT-only: score is " << score);

    if (score==0) {
      // statistics...
      ATH_MSG_DEBUG ("Track score is zero, reject it");
      m_nTrkScoreZero++;
      // clean up
      delete trk;
    } else {
      // add track to map, map is sorted small to big !
      m_trackScoreTrackMap.insert( std::make_pair(-score, trk) );
    }
   
    return;

  }


  TrackCollection* TRT_SegmentToTrackTool::resolveTracks() {

    ATH_MSG_DEBUG ("Resolving the TRT tracks in score map...");

    m_nTrkSegUsed = 0;
    m_nTRTTrk = 0;

    // now loop as long as map is not empty
    TrackScoreMap::iterator itnext;
    for ( itnext = m_trackScoreTrackMap.begin(); itnext != m_trackScoreTrackMap.end(); ++itnext ) {
   
      ATH_MSG_DEBUG ("--- Trying next track "<<itnext->second<<"\t with score "<<-itnext->first);
   
      // some counters to be handled
      int nShared = 0;  // Shared drift circles in segment
      int nHits   = 0;  // Number of TRT measurements
   
      // get vector of TSOS
      const DataVector<const Trk::TrackStateOnSurface>* tsos = (itnext->second)->trackStateOnSurfaces();
   
      // loop over vector of TSOS
      DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsos    = tsos->begin();
      DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsosEnd = tsos->end();
      for ( ; iTsos != iTsosEnd ; ++iTsos) {
   
	// get measurment from TSOS
	const Trk::MeasurementBase* meas = (*iTsos)->measurementOnTrack();
	if (!meas) continue;
   
	// make sure it is a TRT_DC and not a pseudo measurement 
	const InDet::TRT_DriftCircleOnTrack* rot = dynamic_cast <const InDet::TRT_DriftCircleOnTrack*> (meas);
	if( !rot ) continue;
   
	// get to the PRD object
	const InDet::TRT_DriftCircle* RawDataClus=dynamic_cast<const InDet::TRT_DriftCircle*>(rot->prepRawData());
	if(!RawDataClus) continue;
   
	// count up number of hits
	nHits++;
   
	// count up number of shared hits
	if(m_useasso && m_assotool->isUsed(*RawDataClus)) nShared++;
      }
   
      ATH_MSG_DEBUG ("TRT-only has " << nHits << " hits and " << nShared << " of them are shared");
   
      // cut on the number of shared hits with the max fraction
      if(nShared >=  int(m_sharedFrac * nHits)) {
	// statistics
	m_nTrkSegUsed++;
	ATH_MSG_DEBUG ("Too many shared hits, remove it !");
	delete itnext->second;
	continue;
      }
    
      // ok, this seems like a useful track
      m_finalTracks->push_back(itnext->second);
   
      // statistics
      m_nTRTTrk++;
      ATH_MSG_DEBUG ("TRT-only is accepted");
   
      //Register the track with the association tool
      if(m_useasso) {
	StatusCode sc=m_assotool->addPRDs(*(itnext->second));
	if(sc.isFailure()) {
	  ATH_MSG_WARNING ("addPRDs() failed!");
	}
      }
   
    }
   
    return m_finalTracks;

  }


  ///////////////////////////////////////////////////////////////////
  // Overload of << operator MsgStream
  ///////////////////////////////////////////////////////////////////
  MsgStream& operator << (MsgStream& sl,const InDet::TRT_SegmentToTrackTool& se){
    return se.dump(sl);
  }

  ///////////////////////////////////////////////////////////////////
  // Overload of << operator std::ostream
  ///////////////////////////////////////////////////////////////////
  std::ostream& operator << (std::ostream& sl,const InDet::TRT_SegmentToTrackTool& se){
    return se.dump(sl);
  }   


}
