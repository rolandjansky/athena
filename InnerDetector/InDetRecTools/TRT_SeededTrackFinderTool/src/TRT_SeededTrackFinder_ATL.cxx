/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class InDet::TRT_SeededTrackFinder_ATL
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// Version 10 04/12/2006 Thomas Koffas
///////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>
#include <utility>
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include "CLHEP/Vector/ThreeVector.h"

//Track
//
#include "TrkSurfaces/PerigeeSurface.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"

//The main Input file
//
#include "TRT_SeededTrackFinderTool/TRT_SeededTrackFinder_ATL.h"

//SCT Geometry
//
#include "InDetIdentifier/TRT_ID.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "TrkTrack/TrackInfo.h"

//Updator tool
#include "TrkToolInterfaces/IUpdator.h"

//Propagator tool
#include "TrkExInterfaces/IPropagator.h"

//Tool for getting the SiDetElements from geometry
#include "InDetRecToolInterfaces/ISiDetElementsRoadMaker.h" 

//Needed for the RIO_OnTrackCreator
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"

//Magnetic field tool
//#include "TrkMagFieldInterfaces/IMagneticFieldTool.h"
//#include "TrkMagFieldUtils/MagneticFieldMapSolenoid.h"

//Space point seed finding tool
#include "InDetRecToolInterfaces/ITRT_SeededSpacePointFinder.h"

#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "TrkEventPrimitives/FitQuality.h"


//To do the search only if clusters are around:
#include "TrkCaloClusterROI/CaloClusterROI.h"
#include "TrkCaloClusterROI/CaloClusterROI_Collection.h"

using namespace std;

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::TRT_SeededTrackFinder_ATL::TRT_SeededTrackFinder_ATL
(const std::string& t,const std::string& n,const IInterface* p)
  : AthAlgTool(t,n,p),
    m_fieldServiceHandle("AtlasFieldSvc",n),
    m_roadmaker   ("InDet::SiDetElementsRoadMaker_xk"    ),
    m_proptool("Trk::RungeKuttaPropagator/InDetPropagator"),
    m_updatorTool("Trk::KalmanUpdator_xk/InDetPatternUpdator"),
    m_tracksfinder("InDet::SiCombinatorialTrackFinder_xk")
{
  m_fieldmode    = "MapSolenoid"    ;   //Field Mode
  m_xi2max       = 15.              ;   //Maximum chi2 per DOF to accept track candidate
  m_xi2maxNoAdd  = 50.              ;   //Chi2 to accept as hit
  m_xi2maxlink   = 100.             ;   //Chi2 during cluster search
  m_nholesmax    = 1                ;   //Maximum number of holes
  m_dholesmax    = 1                ;   //Maximum hole gap
  m_nclusmin     = 4                ;   //Minimum number of clusters
  m_nwclusmin    = 4                ;   //Minimum number of weighted clusters
  m_pTmin        = 500.             ;   //Minimal Pt cut
  m_bremCorrect  = false            ;   //Repeat seed search after brem correction
  m_propR        = false            ;   //Clean-up seeds by propagating to the first endcap hit
  m_useassoTool  = false            ;   //Use prd-track association tool during combinatorial track finding
  m_errorScale.push_back(1.)        ;   //Error scaling vector
  m_errorScale.push_back(1.)        ;
  m_errorScale.push_back(1.)        ;
  m_errorScale.push_back(1.)        ;
  m_errorScale.push_back(1.)        ;
  m_outlierCut   = 25.              ;
  m_searchInCaloROI   = false       ;
  m_phiWidth     = .3                 ;
  m_etaWidth     = 4.0                ;
  m_ClusterE     = 15000.0                ;

  m_inputClusterContainerName = "InDetCaloClusterROIs";

  declareInterface<ITRT_SeededTrackFinder>(this);

  declareProperty("MagneticTool"            ,m_fieldServiceHandle  );
  declareProperty("PropagatorTool"          ,m_proptool      );
  declareProperty("UpdatorTool"             ,m_updatorTool   );
  declareProperty("RoadTool"                ,m_roadmaker     );
  declareProperty("SeedTool"                ,m_seedmaker     );
  declareProperty("CombinatorialTrackFinder",m_tracksfinder  );
  declareProperty("MagneticFieldMode"       ,m_fieldmode     );
  declareProperty("Xi2max"                  ,m_xi2max        );
  declareProperty("Xi2maxNoAdd"             ,m_xi2maxNoAdd   );
  declareProperty("Xi2maxlink"              ,m_xi2maxlink    );
  declareProperty("pTmin"                   ,m_pTmin         );
  declareProperty("nHolesMax"               ,m_nholesmax     );
  declareProperty("nHolesGapMax"            ,m_dholesmax     );
  declareProperty("nClustersMin"            ,m_nclusmin      );
  declareProperty("nWClustersMin"           ,m_nwclusmin     );
  declareProperty("ErrorScaling"            ,m_errorScale    );
  declareProperty("BremCorrection"          ,m_bremCorrect   );
  declareProperty("ConsistentSeeds"         ,m_propR         );
  declareProperty("UseAssociationTool"      ,m_useassoTool   ); 
  declareProperty("OutlierCut"              ,m_outlierCut    );
  declareProperty("SearchInCaloROI"         ,m_searchInCaloROI);
  declareProperty("phiWidth"                ,m_phiWidth    );
  declareProperty("etaWidth"                ,m_etaWidth    );
  declareProperty("CaloClusterE"            ,m_ClusterE    );
  declareProperty("InputClusterContainerName",m_inputClusterContainerName);
}

///////////////////////////////////////////////////////////////////
// Destructor  
///////////////////////////////////////////////////////////////////

InDet::TRT_SeededTrackFinder_ATL::~TRT_SeededTrackFinder_ATL()
{
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_SeededTrackFinder_ATL::initialize()
{
  StatusCode sc = AthAlgTool::initialize(); 

  msg(MSG::DEBUG)<< "Initializing TRT_SeededTrackFinder_ATL" << endreq;

  // Get magnetic field service
  if(m_fieldmode != "NoField" ) {
    
    if( !m_fieldServiceHandle.retrieve() ){
      ATH_MSG_FATAL("Failed to retrieve " << m_fieldServiceHandle );
      return StatusCode::FAILURE;
    }   
    ATH_MSG_DEBUG("Retrieved " << m_fieldServiceHandle );
    m_fieldService = &*m_fieldServiceHandle;
  }
  
  magneticFieldInit();

  // Get propagator tool
  //
  if(m_proptool.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Could not get " << m_proptool << endreq; return StatusCode::FAILURE;
  }else{
    msg(MSG::INFO) << "Got the propagator tool " << m_proptool << endreq;
  }

  // Get updator tool
  //
  if(m_updatorTool.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Could not get " << m_updatorTool << endreq; return StatusCode::FAILURE;
  }else{
    msg(MSG::INFO) << "Got the updator tool " << m_updatorTool << endreq;
  }

  // Get detector elements road maker tool
  //
  if(m_roadmaker.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Could not get " << m_roadmaker << endreq; return StatusCode::FAILURE;
  }else{
    msg(MSG::INFO) << "Got the detector elements road tool " << m_roadmaker << endreq;
  }

  // Get seeds maker tool
  //
  if(m_seedmaker.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Could not get " << m_seedmaker << endreq; return StatusCode::FAILURE;
  }else{
    msg(MSG::INFO) << "Got the seed tool " << m_seedmaker << endreq;
  }

  // Get combinatorial track finder tool
  //
  if ( m_tracksfinder.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_tracksfinder << endreq; return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_tracksfinder << endreq;
  }

  // Set track quality cuts for track finding tool
  //
  setTrackQualityCuts();

  StoreGateSvc* detStore = 0;
  sc = service( "DetectorStore", detStore );
  if (sc.isFailure()){
    msg(MSG::FATAL) << "Could not get DetectorStore"<<endreq;
    return sc;
  }

  sc = detStore->retrieve(m_trtId, "TRT_ID");
  if (sc.isFailure()){
    msg(MSG::FATAL) << "Could not get TRT_ID helper !" << endreq;
    return StatusCode::FAILURE;
  }

  // Get output print level
  //
  if(msgLvl(MSG::DEBUG)){m_nprint=0; msg(MSG::DEBUG) << (*this) << endreq;}

  return sc;

}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_SeededTrackFinder_ATL::finalize()
{
   StatusCode sc = AthAlgTool::finalize(); return sc;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream&  InDet::TRT_SeededTrackFinder_ATL::dump( MsgStream& out ) const
{
  out<<std::endl;
  if(m_nprint)
    return dumpevent(out);
  else
    return dumpconditions(out);
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_SeededTrackFinder_ATL::dumpconditions( MsgStream& out ) const
{
  int n = 62-m_proptool.type().size();
  std::string s1; for(int i=0; i<n; ++i) s1.append(" "); s1.append("|");
  /*
  n     = 62-m_fieldtool.type().size();
  std::string s2; for(int i=0; i<n; ++i) s2.append(" "); s2.append("|");
  */
  std::string fieldmode[9] ={"NoField"       ,"ConstantField","SolenoidalField",
			     "ToroidalField" ,"Grid3DField"  ,"RealisticField" ,
			     "UndefinedField","AthenaField"  , "?????"         };

  int mode = m_fieldprop.magneticFieldMode(); 
  if(mode<0 || mode>8 ) mode = 8; 

  n     = 62-fieldmode[mode].size();
  std::string s3; for(int i=0; i<n; ++i) s3.append(" "); s3.append("|");

  n     = 62-m_updatorTool.type().size();
  std::string s4; for(int i=0; i<n; ++i) s4.append(" "); s4.append("|");

  n     = 62-m_roadmaker.type().size();
  std::string s6; for(int i=0; i<n; ++i) s6.append(" "); s6.append("|");

  n     = 62-m_seedmaker.type().size();
  std::string s7; for(int i=0; i<n; ++i) s7.append(" "); s7.append("|");

  n     = 62-m_tracksfinder.type().size();
  std::string s5; for(int i=0; i<n; ++i) s5.append(" "); s5.append("|");

  out<<"|----------------------------------------------------------------------"
     <<"-------------------|"
       <<std::endl;
  out<<"| Tool for propagation    | "<<m_proptool.type()    <<s1<<std::endl;
  out<<"| Tool for updator        | "<<m_updatorTool.type() <<s4<<std::endl;
  out<<"| Tool for road maker     | "<<m_roadmaker.type()   <<s6<<std::endl;
  out<<"| Tool for seed maker     | "<<m_seedmaker.type()   <<s7<<std::endl;
  out<<"| Tool for track finding  | "<<m_tracksfinder.type()<<s5<<std::endl;
  //out<<"| Tool for magnetic field | "<<m_fieldtool.type()   <<s2<<std::endl;
  out<<"| Magnetic field mode     | "<<fieldmode[mode]      <<s3<<std::endl;
  out<<"| Min pT of track (MeV)   | "<<std::setw(12)<<std::setprecision(5)<<m_pTmin
     <<"                                                  |"<<std::endl;
  out<<"| Max Xi2 for cluster     | "<<std::setw(12)<<std::setprecision(5)<<m_xi2max
     <<"                                                  |"<<std::endl;
  out<<"| Max Xi2 for outlayer    | "<<std::setw(12)<<std::setprecision(5)<<m_xi2maxNoAdd
     <<"                                                  |"<<std::endl;
  out<<"| Max Xi2 for link        | "<<std::setw(12)<<std::setprecision(5)<<m_xi2maxlink
     <<"                                                  |"<<std::endl;
  out<<"| Min number of clusters  | "<<std::setw(12)<<m_nclusmin
     <<"                                                  |"<<std::endl;
  out<<"| Max number holes        | "<<std::setw(12)<<m_nholesmax
     <<"                                                  |"<<std::endl;
  out<<"| Max holes  gap          | "<<std::setw(12)<<m_dholesmax
     <<"                                                  |"<<std::endl;
  out<<"|----------------------------------------------------------------------"
     <<"-------------------|"
      <<std::endl;
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps event information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_SeededTrackFinder_ATL::dumpevent( MsgStream& out ) const
{
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::TRT_SeededTrackFinder_ATL::dump( std::ostream& out ) const
{
  return out;
}

///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::operator <<    
  (MsgStream& sl,const InDet::TRT_SeededTrackFinder_ATL& se)
{ 
  return se.dump(sl); 
}

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::operator << 
  (std::ostream& sl,const InDet::TRT_SeededTrackFinder_ATL& se)
{
  return se.dump(sl); 
}   

///////////////////////////////////////////////////////////////////
// Initiate track finding tool
///////////////////////////////////////////////////////////////////

void InDet::TRT_SeededTrackFinder_ATL::newEvent()
{

  ///Get the seeds
  m_seedmaker->newEvent();

  // New event for track finder tool
  //
  m_tracksfinder->newEvent();

  // Erase cluster to track association
  //
  m_clusterTrack.clear();

  // Print event information 
  //
  if(msgLvl(MSG::DEBUG)) {
    m_nprint=1; msg(MSG::DEBUG) << (*this) << endreq;
  }


  //  Get the calo ROI:
  //
  if(m_searchInCaloROI ) {

    m_caloF.clear();
    m_caloE.clear();

    const CaloClusterROI_Collection* calo = 0;
    StatusCode sc = evtStore()->retrieve(calo,m_inputClusterContainerName);

    if(sc == StatusCode::SUCCESS && calo) {

      CaloClusterROI_Collection::const_iterator c = calo->begin(), ce = calo->end();

      for(; c!=ce; ++c) {
	if ( (*c)->energy() < m_ClusterE) continue;
        double x = (*c)->globalPosition().x();
        double y = (*c)->globalPosition().y();
        double z = (*c)->globalPosition().z();
        m_caloF.push_back(atan2(y,x));
        m_caloE.push_back(atan2(1.,z/sqrt(x*x+y*y)));
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////
void InDet::TRT_SeededTrackFinder_ATL::newRegion(const std::vector<IdentifierHash>& listOfPixIds,
const std::vector<IdentifierHash>& listOfSCTIds)
{

  ///Get the seeds
  m_seedmaker->newRegion(listOfPixIds,listOfSCTIds);

  // New event for track finder tool
  //
  m_tracksfinder->newEvent();

  // Erase cluster to track association
  //
  m_clusterTrack.clear();

  // Print event information
  //
  if(msgLvl(MSG::DEBUG)) {
    m_nprint=1; msg(MSG::DEBUG) << (*this) << endreq;
  }

}

///////////////////////////////////////////////////////////////////
// Finalize track finding tool for given event
///////////////////////////////////////////////////////////////////

void InDet::TRT_SeededTrackFinder_ATL::endEvent()
{
  
  // End event for track finder tool
  //
  m_tracksfinder->endEvent();
  m_clusterTrack.clear();

}

///////////////////////////////////////////////////////////////////
// Main method for back tracking through the Si ID 
// starting from an intial track segment
///////////////////////////////////////////////////////////////////

std::list<Trk::Track*> InDet::TRT_SeededTrackFinder_ATL::getTrack(const Trk::TrackSegment& tS)
{
  // return list, will be copied by value (fix!)
  std::list<Trk::Track*> aSiTrack;

  // protection for having the expected type of segment parameters
  if ( tS.localParameters().parameterKey() != 31 ){
    msg(MSG::WARNING) << "Wrong type of TRT segment, rejected !" << endreq;
    return aSiTrack;
  }

  //Get the track segment information and build the initial track parameters
  const Trk::LocalParameters&     Vp   = tS.localParameters();
  const Trk::StraightLineSurface* surf = dynamic_cast<const Trk::StraightLineSurface*>(&(tS.associatedSurface()));
  if (!surf) {
    throw std::logic_error("Unhandled surface.");
  }

  //Trk::ErrorMatrix* ie = tS.localErrorMatrix().clone();
  const AmgSymMatrix(5)& locCov = tS.localCovariance();
  AmgSymMatrix(5)     * ie      = new AmgSymMatrix(5)(locCov);


  //const Trk::TrackParameters* newPerPar = new Trk::MeasuredAtaStraightLine(Vp(1),Vp(2),Vp(3),Vp(4),Vp(5),*surf,ie);
  std::unique_ptr<const Trk::TrackParameters> newPerPar( surf->createTrackParameters(Vp.get(Trk::loc1),Vp.get(Trk::loc2),Vp.get(Trk::phi),Vp.get(Trk::theta),Vp.get(Trk::qOverP),ie) );

  if(newPerPar){
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Initial Track Parameters created from TRT segment, " << endreq;
    if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << (*newPerPar) << endreq;
  }else{
    msg(MSG::WARNING) << "Could not get initial TRT track parameters, rejected! " << endreq;
    return aSiTrack;
  }

  //Do the actual tracking and smoothing and get the Si track states on surface
  if(msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "==============================================================" << endreq;
    msg(MSG::DEBUG) << "Start initial search with 3-4 SCT layer combinations " << endreq;
  }

  // aalonso
  if (  m_searchInCaloROI && !isCaloCompatible( *newPerPar )) {
//   	msg(MSG::WARNING) << " Track Segment not asociated to a calo clustes!!!! " << endreq;
    	return aSiTrack;

  }


  aSiTrack = findTrack(newPerPar.get(),tS);
  if((aSiTrack.size()==0)&&(m_bremCorrect)){
    if(msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "==============================================================" << endreq;
      msg(MSG::DEBUG) << "Could not create track states on surface for this track!Try to add brem correction." << endreq;
    }
    const Trk::TrackParameters* modTP = modifyTrackParameters(*newPerPar,0);
    if(msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) << "Modified TRT Track Parameters for brem. " << endreq;
      msg(MSG::VERBOSE) << (*modTP) << endreq;
    }
    aSiTrack = findTrack(modTP,tS);
    delete modTP;
    if(aSiTrack.size()==0){
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"Could not create track states on surface for this track after all!"<<endreq;
      return aSiTrack;
    }
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "==============================================================" << endreq;
  }

  //Return list of tracks (by value !?)
  return aSiTrack;
}

///////////////////////////////////////////////////////////////////
// Main method for back tracking through the Si ID 
// starting from initial track parameters
///////////////////////////////////////////////////////////////////

std::list<Trk::Track*> InDet::TRT_SeededTrackFinder_ATL::findTrack
                         (const Trk::TrackParameters* initTP,const Trk::TrackSegment& tS)
{
  //Return list copied by value (fix!!) 
  std::list<Trk::Track*> associatedSiTrack; // List of found tracks per TRT segment
  const double pi2 = 2.*M_PI, pi=M_PI;
  
  //Get the seeds
  std::list<std::pair<const Trk::SpacePoint*,const Trk::SpacePoint*> >* SpE;
  SpE = m_seedmaker->find2Sp(*initTP);                //Get a list of SP pairs

  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "---------------> SP SEED LIST SIZE " << SpE->size() << endreq;
  if(SpE->size()==0){return associatedSiTrack;}

  //
  // --------------- loop over the found seeds
  //

  //Get the track states on surface that correspond to each SP pair that came from the seeding
  std::list<std::pair<const Trk::SpacePoint*,const Trk::SpacePoint*> >::iterator r,re=SpE->end();
  for(r=SpE->begin();r!=re; ++r){

    std::list<Trk::Track*>         aTracks   ; // List of tracks found per seed
    std::list<Trk::Track*>         cTracks   ; // List of cleaned tracks found per seed
    m_noise.initiate();  //Initiate the noise production tool at the beginning of each seed

    //
    // --------------- filter SP to improve prediction, scale errors
    //

    //Get the track parameters to use to get the detector elements for each SP pair
    std::pair<const Trk::SpacePoint*, const Trk::SpacePoint*>& pSP = *r;
    if (pSP.first != pSP.second){
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "----> Seed Pair: SP 1 " << (pSP.first)->r() << " SP 2 " << (pSP.second)->r() << endreq;
    }else {
      if(msgLvl(MSG::DEBUG)) {
	msg(MSG::DEBUG) << "----> Seed Single: SP 1 " << (pSP.first)->r() << endreq;
	msg(MSG::DEBUG) << "      Will not process for the time being ! A special module is needed" << endreq; 
	msg(MSG::DEBUG) << "      to deal with late conversion (no search and stablized input fit )." << endreq; 
	msg(MSG::DEBUG) << "      Current version is unstable in the SP updates and gets unpredictable results." << endreq; 
      }
      continue;
    }

    ///List of space points in the current seed, starting from the one at the smaller radius
    std::list<const Trk::SpacePoint*> SpList;
    SpList.push_back(pSP.second); SpList.push_back(pSP.first);
    if(!newClusters(SpList)) {
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Seed SPs already used by a single track. Ignore..." << endreq; 
      continue;
    }
    if(!newSeed(SpList)) {
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Seed SPs already used by other tracks. Ignore..." << endreq; 
      continue;
    }

    //
    // ----------------Check the SP seed if field is ON
    //
    //if(m_fieldtool->solenoidOn()){
    if(m_fieldService->solenoidOn()){
      bool seedGood = checkSeed(SpList,tS,initTP);
      if(!seedGood && m_propR) {
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Seed not consistent with TRT segment. Ignore..." << endreq; 
	continue;
      }
    }

    //
    // ----------------Get new better track parameters using the SP seed
    //
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Get better track parameters using the seed" << endreq;
    double newTheta = getNewTheta(SpList);

    const AmgVector(5)& iv = initTP->parameters();

    double newPhi = iv[2];

    //Protect for theta and phi being out of range
    if     (newTheta > pi) newTheta = fmod(newTheta+pi,pi2)-pi;
    else if(newTheta <-pi) newTheta = fmod(newTheta-pi,pi2)+pi;
    if(newTheta<0.){ newTheta = -newTheta; newPhi+=pi; }
    if     (newPhi > pi) newPhi = fmod(newPhi+pi,pi2)-pi;
    else if(newPhi <-pi) newPhi = fmod(newPhi-pi,pi2)+pi;
    if(newTheta<0.27) {
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Pseudorapidity greater than 2.Ignore" << endreq; 
      continue;
    }

    //Get the intial Error matrix, associated surface
    //Get the intial Error matrix, associated surface
    //const Trk::TrackParameters *ivMAP = dynamic_cast<const Trk::MeasuredAtaStraightLine*>(initTP);
    //if(!ivMAP) continue;
    //const Trk::CovarianceMatrix& vCM = ivMAP->localErrorMatrix().covariance();

    const AmgSymMatrix(5) * vCM = initTP->covariance();
    AmgSymMatrix(5) * nvCM = new AmgSymMatrix(5);

    (*nvCM)<<
      m_errorScale[0]*m_errorScale[0]*(*vCM)(0,0),0.,0.,0.,0.,
      0.,m_errorScale[1]*m_errorScale[1]*(*vCM)(1,1),0.,0.,0.,
      0.,0.,m_errorScale[2]*m_errorScale[2]*(*vCM)(2,2),0.,0.,
      0.,0.,0.,m_errorScale[3]*m_errorScale[3]*(*vCM)(3,3),0.,
      0.,0.,0.,0.,m_errorScale[4]*m_errorScale[4]*(*vCM)(4,4);


    //New intial track parameters saved as MeasuredAtaStraightLine
    //
    const Trk::TrackParameters* niTP = initTP->associatedSurface().createTrackParameters(iv[0],iv[1],newPhi,newTheta,iv[4],nvCM);

    if(niTP){
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Initial Track Parameters created and scaled from TRT segment, " << endreq;
      if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << (*niTP) << endreq;
    }else{
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Could not get initial TRT track parameters! " << endreq;
      continue;
    }
    
    //
    // ----------------Propagate through the SP seed
    //
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Propagating through the seed" << endreq;
    bool outl = false;
    
    //update with first SP 
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Update with 1st SP from seed" << endreq;
    const Trk::TrackParameters* upTP = getTP(pSP.first,niTP,outl);
    //If no track parameters are found, go to the next seed
    if(!upTP){
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Extrapolation through seed failed!Seed bogus.Move to next seed" << endreq; 
      delete niTP; continue;
    }
    //Not good if SP pair has outliers. Clean up the memory and move to next seed
    if(outl){
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"Seed with outliers. Will not process!"<<endreq;
      delete niTP; delete upTP; continue;
    }
    
    //update with second SP ? 
    if (pSP.first != pSP.second) {
      //update with second SP 
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Update with 2nd SP from seed" << endreq;
      const Trk::TrackParameters* newTP = getTP(pSP.second,upTP,outl);
      delete upTP;
      //If no track parameters are found, go to the next seed
      if(!newTP){
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Extrapolation through seed failed!Seed bogus.Move to next seed" << endreq; 
	delete niTP; continue;
      }
      //Not good if SP pair has outliers. Clean up the memory and move to next seed
      if(outl){
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"Seed with outliers.Will not process!"<<endreq;
	delete niTP; delete newTP; continue;
      }
      // copy the newTP to upTP
      upTP = newTP;
    }

    //Protect the road maker tool when the momentum is too low since the particle will spiral inside the tracker
    if(upTP->momentum().perp()<m_pTmin){
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"Low pT.Stop! "<<endreq;
      delete niTP; delete upTP; continue;
    }

    //
    // --------------- get Si detector element road
    //
    const Trk::PerigeeSurface persurf (Amg::Vector3D(0,0,0));

    //Get track parameters at the end of SCT to start backwards propagation
    const Trk::TrackParameters* per   = m_proptool->propagate(*upTP,persurf,Trk::oppositeMomentum,false,m_fieldprop,Trk::nonInteracting); //Propagate 
    if(!per){
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"No extrapolated track parameters!"<<endreq; 
      delete niTP; delete upTP; continue;
    }

    if(msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) << "Perigee after SP updates at same surface" << endreq;
      msg(MSG::VERBOSE) << *per << endreq;
    }

    //Get list of InDet Elements
    std::vector<const InDetDD::SiDetectorElement*> DE;
    m_roadmaker->detElementsRoad(*per,Trk::alongMomentum,DE);
    delete per;
    if( int(DE.size()) < m_nclusmin){ //Not enough detector elements to satisfy the minimum number of clusters requirement. Stop
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Too few detector elements, not expected" << endreq;
      delete niTP; delete upTP; continue;
    }
    //std::list<const InDetDD::SiDetectorElement*>::iterator ir,ire=DE.end();
    //for(ir=DE.begin(); ir!=ire; ++ir){
    //  IdentifierHash id = (*ir)->identifyHash();  //Get the detElement id
    //  cout << "DET ELEMENT ID " << id << " SCT? " << ((*ir)->isSCT()) << " POSITION " << (*ir)->center().perp() << endl;
    //}

    // 
    // --------------- Cast it to measured parameters at 2nd SP with diagonal error matrix
    //
    const AmgVector(5)& piv = upTP->parameters();

    //Get the intial Error matrix, diagonalize it and scale the errors
    const Trk::TrackParameters* mesTP = 0;
    const AmgSymMatrix(5)* pvCM  = upTP->covariance();

    if(pvCM){

      AmgSymMatrix(5)* pnvCM = new AmgSymMatrix(5);

      (*pnvCM)<< 
	m_errorScale[0]*m_errorScale[0]*(*pvCM)(0,0),0.,0.,0.,0.,
	0.,m_errorScale[1]*m_errorScale[1]*(*pvCM)(1,1),0.,0.,0.,
	0.,0.,m_errorScale[2]*m_errorScale[2]*(*pvCM)(2,2),0.,0.,
	0.,0.,0.,m_errorScale[3]*m_errorScale[3]*(*pvCM)(3,3),0.,
	0.,0.,0.,0.,m_errorScale[4]*m_errorScale[4]*(*pvCM)(4,4);

      mesTP = upTP->associatedSurface().createTrackParameters(piv[0],piv[1],piv[2],piv[3],piv[4],pnvCM);

      if(mesTP){
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Initial Track Parameters at 1st SP created and scaled from TRT segment, " << endreq;
	if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << (*mesTP) << endreq;
      }else{
	delete niTP; delete upTP; continue;
      }
    }else{
      delete niTP; delete upTP; continue;
    }

    //
    // --------------- Get the Si extensions using the combinatorial track finding tool
    //
    std::list<Amg::Vector3D> Gp;
    aTracks = m_tracksfinder->getTracks(*mesTP,SpList,Gp,DE,m_trackquality);
    if(int(aTracks.size())==0) {
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"No tracks found by the combinatorial track finder!"<<endreq;
    }

    //
    // --------------- Drop spurious pixel hits
    //
    cTracks=cleanTrack(aTracks);

    //
    // --------------- Add tracks in the track multimap and in the overall list of TRT segment associated tracks
    //
    std::list<Trk::Track*>::iterator t = cTracks.begin();
    while(t!=cTracks.end()) {
      if(!isNewTrack((*t))) {delete (*t);cTracks.erase(t++);}
      else{
	clusterTrackMap((*t));
        associatedSiTrack.push_back((*t++));
      }
    }
   
    delete niTP; delete upTP; delete mesTP; 

  }  ///end of loop over seeds for this TRT segment

  return  associatedSiTrack;
}

///////////////////////////////////////////////////////////////////
// update track parameters using SP
///////////////////////////////////////////////////////////////////

const Trk::TrackParameters*
InDet::TRT_SeededTrackFinder_ATL::getTP(const Trk::SpacePoint* SP, const Trk::TrackParameters* startTP, bool& outl)
{
  const Trk::TrackParameters* iTP = 0;

  outl = false;

  const Trk::Surface&               surf  = SP->associatedSurface(); //Get the associated surface
  Trk::PropDirection                dir   = Trk::oppositeMomentum;   //Propagate backwards i.e. opposite momentum when filtering
  Trk::ParticleHypothesis           part  = Trk::nonInteracting;     //Choose a non interacting particle
  const Trk::TrackParameters*       eTP   = m_proptool->propagate(*startTP,surf,dir,false,m_fieldprop,part); //Propagate 

  if(!eTP){  

    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Extrapolation to Si element failed"<< endreq;
    if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << surf << endreq;
    return 0;

  }else{

    //Based on the hit information update the track parameters and the error matrix
    const Trk::TrackParameters* uTP         = 0;
    Trk::FitQualityOnSurface*   sct_fitChi2 = 0;

    uTP = m_updatorTool->addToState(*eTP,SP->localParameters(),SP->localCovariance(),sct_fitChi2);

    if(!uTP) { //The updator failed
      if (sct_fitChi2) {
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Updator returned no update, but a DitQuality object, a leak !"<< endreq; 
	delete sct_fitChi2;
      }
      m_noise.production(-1,1,*eTP);
      double covAzim=m_noise.covarianceAzim();
      double covPola=m_noise.covariancePola();
      double covIMom=m_noise.covarianceIMom();
      double corIMom=m_noise.correctionIMom();
      iTP = addNoise(covAzim,covPola,covIMom,corIMom,eTP,0);
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"The updator failed! Count an outlier "<<endreq;
      delete eTP; 
      //if (uTP) {delete uTP; delete sct_fitChi2;}
      outl = true;
      
    }else{
      //Keep as a measurement only if fit chi2 less than 25.Otherwise outlier
      float outlierCut = m_outlierCut;
      if(!m_fieldServiceHandle->solenoidOn()) outlierCut = 1000000.; // Increase the outlier chi2 cut if solenoid field is OFF
      if( sct_fitChi2->chiSquared() < outlierCut && fabs(uTP->parameters()[Trk::theta]) > 0.17 ){
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"Update worked, will update return track parameters, chi2: "<<(sct_fitChi2->chiSquared())<<endreq;
	m_noise.production(-1,1,*uTP);
	double covAzim=m_noise.covarianceAzim();
	double covPola=m_noise.covariancePola();
	double covIMom=m_noise.covarianceIMom();
	double corIMom=m_noise.correctionIMom();
	iTP = addNoise(covAzim,covPola,covIMom,corIMom,uTP,0); 
      }else{
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"Outlier, did not satisfy cuts, chi2: "<<(sct_fitChi2->chiSquared())<<" "<<fabs(uTP->parameters()[Trk::theta])<<endreq;
	m_noise.production(-1,1,*eTP);
	double covAzim=m_noise.covarianceAzim();
	double covPola=m_noise.covariancePola();
	double covIMom=m_noise.covarianceIMom();
	double corIMom=m_noise.correctionIMom();
	iTP = addNoise(covAzim,covPola,covIMom,corIMom,eTP,0); 
	outl = true;
      }
      
      // Clean up
      delete eTP; delete uTP; delete sct_fitChi2;
    }
  }
  return iTP;
}

///////////////////////////////////////////////////////////////////
// Add noise to track parameters
// 0-Filtering, 1-Smoothing
///////////////////////////////////////////////////////////////////
  
const Trk::TrackParameters* InDet::TRT_SeededTrackFinder_ATL::addNoise 
    (double covAzim, double covPola, double covIMom, double corIMom, const Trk::TrackParameters* P1, int isSmooth) 
{
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Adding noise to track parameters... " << endreq;
  const Trk::TrackParameters* noiseTP = 0;  //Initialize the sum of track parameters

  //Get the noise augmented parameters and the 15 lower half covariance matrix elements from the first input track parameters

  const AmgVector(5)& Vp1 = P1->parameters();

  double M[5]={Vp1[0],Vp1[1],Vp1[2],Vp1[3],Vp1[4]};
  if(!isSmooth){
    M[4] *= corIMom;
  }else{
    M[4] /= corIMom;
  }
  const AmgSymMatrix(5)* C = P1->covariance();

  if(C){

    AmgSymMatrix(5)* nC = new AmgSymMatrix(5);
    (*nC) = (*C);
    (*nC)(2,2)+=covAzim;
    (*nC)(3,3)+=covPola;
    (*nC)(4,4)+=covIMom;
    noiseTP = P1->associatedSurface().createTrackParameters(M[0],M[1],M[2],M[3],M[4],nC);

  }else{
    return noiseTP;
  }

  return noiseTP;
}

///////////////////////////////////////////////////////////////////
// Get new theta estimate using the SPs from the seed
///////////////////////////////////////////////////////////////////

double
InDet::TRT_SeededTrackFinder_ATL::getNewTheta(std::list<const Trk::SpacePoint*>& lsp)
{
  double theta = 0.;
  std::vector<double> rad;
  std::vector<double> zl;

  std::list<const Trk::SpacePoint*>::const_iterator isp=lsp.begin(), ispe=lsp.end();
  const std::size_t ispMax=std::distance(isp,ispe);
  rad.reserve(ispMax);
  zl.reserve(ispMax);
  for(; isp!=ispe; ++isp){
    double r = (*isp)->globalPosition().perp();
    rad.push_back(r);
    double z = (*isp)->globalPosition().z();
    zl.push_back(z);
  }

  theta = atan2((rad[1]-rad[0]),(zl[1]-zl[0]));

  return theta;
}

///////////////////////////////////////////////////////////////////
// Get new theta estimate using the SPs from the seed
///////////////////////////////////////////////////////////////////

bool InDet::TRT_SeededTrackFinder_ATL::checkSeed
     (std::list<const Trk::SpacePoint*>& lsp,const Trk::TrackSegment& tS,const Trk::TrackParameters* tP)
{
  bool isGood = true;
  int nEC = 0; double gz = 0.;

  ///Get TRT segment track parameters
  const AmgVector(5)& pTS=tP->parameters();

  ///Process only if endcap-transition region
  if(fabs(log(tan(pTS[3]/2.)))>0.8){

    ///Find the global z position of first endcap hit on TRT segment
    for(int it=0; it<int(tS.numberOfMeasurementBases()); it++){
      //Check if it is a pseudo measurement and move on
      if ( dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(tS.measurement(it)) ) continue;
      const InDet::TRT_DriftCircleOnTrack* trtcircle = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(tS.measurement(it));
      if(trtcircle){
        Identifier id=trtcircle->detectorElement()->identify();
        int isB = m_trtId->barrel_ec(id);
        if(isB==2 || isB==-2) nEC++;
        if(nEC==1){
          gz = trtcircle->globalPosition().z();
          break;
        }
      }
    }

    ///Get theta from the space points of the seed
    double theta = 0.;
    std::vector<double> rad;
    std::vector<double> zl;
    std::list<const Trk::SpacePoint*>::const_iterator isp=lsp.begin(), ispe=lsp.end();
    const std::size_t ispMax=std::distance(isp,ispe);
    rad.reserve(ispMax);
    zl.reserve(ispMax);
    for(; isp!=ispe; ++isp){
      double r = (*isp)->globalPosition().perp();
      rad.push_back(r);
      double z = (*isp)->globalPosition().z();
      zl.push_back(z);
    }
    theta = atan2((rad[1]-rad[0]),(zl[1]-zl[0]));

    ///Propagate at the z position of 1st endcap hit on TRT segment
    double propR = rad[1] + (gz-zl[1])*tan(theta);

    if(propR<620. || propR>1010.) isGood=false;

    double zIn = gz-propR/tan(theta);
    if(zIn>300.) isGood = false;
  }

  return isGood;
}

///////////////////////////////////////////////////////////////////
//  Calculate initial track parameters for back tracking
//  using TRT New Tracking segments 
///////////////////////////////////////////////////////////////////

const Trk::TrackParameters* 
InDet::TRT_SeededTrackFinder_ATL::modifyTrackParameters(const Trk::TrackParameters& TP, int mode)
{
  ///The mode corresponds to whether the track parameters are modified before the seed (0) or before the pixel propagation (1)

  double covarianceIMom = 0;
  double correctionIMom = 1.;
  
  ///Get the track parameters
  const AmgVector(5)& pV = TP.parameters();
  double ip[5] = {pV[0], pV[1], pV[2], pV[3], pV[4]};

  ///Correct inverse momentum and covariance. Inverse momentum halfed, i.e. momentum doubled
  double q = fabs(ip[4]);
  correctionIMom = .5;
  covarianceIMom = (correctionIMom-1.)*(correctionIMom-1.)*q*q;
  ip[4] *= correctionIMom;

  const AmgSymMatrix(5) * CM = TP.covariance();
  AmgSymMatrix(5)*        nM = new AmgSymMatrix(5); (*nM) = (*CM);

  if(mode==0){ //Modification initiated before seed propagation

    (*nM)(4,4) = 10.*(0.1*((*CM)(4,4))+covarianceIMom);
  }
  if(mode==1){  //Modification initiated before pixel propagation

    (*nM)(4,4)+=covarianceIMom;
  }
  const Trk::TrackParameters* newInitTrackParameters = TP.associatedSurface().createTrackParameters(ip[0],ip[1],ip[2],ip[3],ip[4],nM); 
  return newInitTrackParameters;
}

///////////////////////////////////////////////////////////////////
// Set track quality cuts
///////////////////////////////////////////////////////////////////

void  InDet::TRT_SeededTrackFinder_ATL::setTrackQualityCuts()
{
  // Integer cuts
  //
  m_trackquality.setIntCut   ("MinNumberOfClusters",  m_nclusmin   );
  m_trackquality.setIntCut   ("MinNumberOfWClusters", m_nwclusmin  );
  m_trackquality.setIntCut   ("MaxNumberOfHoles"   ,  m_nholesmax  ); 
  m_trackquality.setIntCut   ("MaxHolesGae"        ,  m_dholesmax  ); 
  if( m_useassoTool ) m_trackquality.setIntCut   ("UseAssociationTool",1);
  else                m_trackquality.setIntCut   ("UseAssociationTool",0);

  // Double cuts
  //
  m_trackquality.setDoubleCut("pTmin"              ,m_pTmin      );
  m_trackquality.setDoubleCut("MaxXi2forCluster"   ,m_xi2max     );
  m_trackquality.setDoubleCut("MaxXi2forOutlier"   ,m_xi2maxNoAdd);
  m_trackquality.setDoubleCut("MaxXi2forSearch"    ,m_xi2maxlink );
}

///////////////////////////////////////////////////////////////////
// Clusters-track multimap production
///////////////////////////////////////////////////////////////////

void  InDet::TRT_SeededTrackFinder_ATL::clusterTrackMap(Trk::Track* Tr)
{
  DataVector<const Trk::MeasurementBase>::const_iterator 
    m  = Tr->measurementsOnTrack()->begin(), 
    me = Tr->measurementsOnTrack()->end  ();

  for(; m!=me; ++m) {
    const Trk::PrepRawData* prd = ((const Trk::RIO_OnTrack*)(*m))->prepRawData();
    if(prd) m_clusterTrack.insert(std::make_pair(prd,Tr));
  }
}
 
///////////////////////////////////////////////////////////////////
// New clusters comparison with clusters associated with track
// Reject seeds that all SPs belong to one and the same track
///////////////////////////////////////////////////////////////////

bool InDet::TRT_SeededTrackFinder_ATL::newClusters(const std::list<const Trk::SpacePoint*>& Sp)
{
  const Trk::PrepRawData* prd   [ 40];
  const Trk::Track*       trk[2][200];
  std::multimap<const Trk::PrepRawData*,const Trk::Track*>::const_iterator 
    t[40],te = m_clusterTrack.end();
  std::list<const Trk::SpacePoint*>::const_iterator s=Sp.begin(),se=Sp.end();
  int n = 0;

  //If at least one of the clusters in the seed is not used by a track the seed is good
  for(; s!=se; ++s) {
     if((*s)->clusterList().first ) {
       prd[n] = (*s)->clusterList().first;
       t  [n] = m_clusterTrack.find(prd[n]); if(t[n]==te) return true; ++n;
     }
     if((*s)->clusterList().second) {
       prd[n] = (*s)->clusterList().second;
       t  [n] = m_clusterTrack.find(prd[n]); if(t[n]==te) return true; ++n;
     }
     if(n==40) break;
  }
  if(!n) return false;

  //Array of pointers to the different tracks that the first used cluster belongs to
  int m = 0;
  for(; t[0]!=te; ++t[0]) {
    if (m==30) return false;
    if( (*t[0]).first != prd[0] ) break;
    trk[0][m++] = (*t[0]).second; 
    if(m==200) break;
  } 

  //For a seed to be declared bad, all other clusters should belong to the same track as that of the first used cluster
  int in=0, ou=1;

  for(int i=1; i!=n; ++i) {

    int l = 0; //Number of tracks that share the same clusters
    for(; t[i]!=te; ++t[i]) {
  
      if( (*t[i]).first != prd[i] ) break;
  
      for(int j=0; j!=m; ++j) {
  	if((*t[i]).second == trk[in][j]) {trk[ou][l++]= trk[in][j]; break;}
      }
    }
    if(l==0) return true; //At least one of the seed clusters belongs to a track different from that of the first used clusters
    m=l; if(in==0) {in=1; ou=0;} else {in=0; ou=1;}
  }

  return false;
}

///////////////////////////////////////////////////////////////////
// New clusters comparison with clusters associated with track
// Reject seeds that all SPs have been already used by other tracks
///////////////////////////////////////////////////////////////////

bool InDet::TRT_SeededTrackFinder_ATL::newSeed(const std::list<const Trk::SpacePoint*>& Sp)
{
  const Trk::PrepRawData* prd   [ 40];
  const Trk::Track*       trk[2][200];
  std::multimap<const Trk::PrepRawData*,const Trk::Track*>::const_iterator 
    tt,t[40],te = m_clusterTrack.end();

  std::list<const Trk::SpacePoint*>::const_iterator s=Sp.begin(),se=Sp.end();
  int n  = 0;
  int nc = 0;
  for(; s!=se; ++s) {

     if((*s)->clusterList().first ) {
       prd[n] = (*s)->clusterList().first;
       t  [n] = m_clusterTrack.find(prd[n]); if(t[n]!=te) ++nc; ++n;
     }
     if((*s)->clusterList().second) {
       prd[n] = (*s)->clusterList().second;
       t  [n] = m_clusterTrack.find(prd[n]); if(t[n]!=te) ++nc; ++n; 
     }
     if(n==40) break;
  }
  if(!nc) return true;

  if(n==nc) {

    int m = 0;
    for(tt=t[0]; tt!=te; ++tt) {
      if( (*tt).first != prd[0] ) break;
      trk[0][m++] = (*tt).second; 
      if(m==200) break;
    } 
    
    int in=0, ou=1, i=1;

    for(; i!=n; ++i) {

      int l = 0;
      for(tt=t[i]; t[i]!=te; ++tt) {
	if( (*tt).first != prd[i] ) break;
	for(int j=0; j!=m; ++j) {
	  if((*tt).second == trk[in][j]) {trk[ou][l++]= trk[in][j]; break;}
	}
      }
      if(l==0) break;
      m=l; if(in==0) {in=1; ou=0;} else {in=0; ou=1;}
    }
    if(i==n) return false;
  }

  //if(!(*Sp.rbegin())->clusterList().second) return true;
  
  int h = 0;
  for(int i=0; i!=n; ++i) {
  
    for(tt=t[i]; t[i]!=te; ++tt) {
      if( (*tt).first != prd[i] ) break;
      if((*tt).second->trackStateOnSurfaces()->size() >= 10) {++h; break;}
    }
  }
  if(h)
    return false;
  else
    return true;
}

///////////////////////////////////////////////////////////////////
// Test is it new track
///////////////////////////////////////////////////////////////////

bool InDet::TRT_SeededTrackFinder_ATL::isNewTrack(Trk::Track* Tr)
{

  const Trk::PrepRawData* prd   [100];
  std::multimap<const Trk::PrepRawData*,const Trk::Track*>::const_iterator 
    ti,t[100],te = m_clusterTrack.end();

  int     n   = 0    ;

  DataVector<const Trk::MeasurementBase>::const_iterator 
    m  = Tr->measurementsOnTrack()->begin(), 
    me = Tr->measurementsOnTrack()->end  ();

  for(; m!=me; ++m) {
  
    const Trk::PrepRawData* pr = ((const Trk::RIO_OnTrack*)(*m))->prepRawData();
    if(pr) {
      prd[n] =pr;
      t  [n] = m_clusterTrack.find(prd[n]); if(t[n]==te) return true; ++n;
    }
  }

  int nclt = n;
  
  for(int i=0; i!=n; ++i) {

    int nclmax = 0;

    for(ti=t[i]; ti!=te; ++ti) {

      if( (*ti).first != prd[i] ) break;
      int ncl = (*ti).second->trackStateOnSurfaces()->size();
      if(ncl > nclmax) nclmax = ncl;
    }   
    if(nclt > nclmax) return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////
// Eliminate spurious pixel hits
///////////////////////////////////////////////////////////////////

std::list<Trk::Track*> InDet::TRT_SeededTrackFinder_ATL::cleanTrack
                         (std::list<Trk::Track*> lTrk)
{
  std::list<Trk::Track*> cleanSiTrack; // List of clean Si tracks per TRT segment

  std::list<Trk::Track*>::const_iterator it    = lTrk.begin();
  std::list<Trk::Track*>::const_iterator itEnd = lTrk.end();
  for (; it != itEnd ; ++it){
    int nPixHits = 0;  //Number of Pixel PRDs
    int nSctHits = 0;  //Number of SCT PRDs
    double pixR = 0.;  //Radial position of last pixel PRD
    double sctR = 0.;  //Radial position of first SCT PRD

    const DataVector<const Trk::TrackStateOnSurface>* newtsos = (*it)->trackStateOnSurfaces();
    if(!newtsos) continue;
    DataVector<const Trk::TrackStateOnSurface>::const_iterator itp, itpe=newtsos->end();
    for(itp=newtsos->begin(); itp!=itpe; itp++){
      ///Concentrate on the Si component of the track
      const InDet::SiClusterOnTrack* clus = dynamic_cast<const InDet::SiClusterOnTrack*>((*itp)->measurementOnTrack());
      if(clus && ((*itp)->type(Trk::TrackStateOnSurface::Measurement))){  //Count the number of hits used in the track
        const InDet::SiCluster* RawDataClus=dynamic_cast<const InDet::SiCluster*>(clus->prepRawData());
        if(RawDataClus==0){
	  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Si Cluster without PrepRawData!!!" << endreq;
          continue;
        }
        if(RawDataClus->detectorElement()->isPixel()){
          nPixHits++;
          pixR = RawDataClus->globalPosition().perp();
        }
        if((RawDataClus->detectorElement()->isSCT()) && (nSctHits==0)) {
          sctR = RawDataClus->globalPosition().perp();
          nSctHits++;
          break;
        }
      }
    }

    ///Throw out any spurious pixel hits.Need to rebuild the vector of track states on surface from scratch, since it's const in EDM
    if(nPixHits==1 && (sctR-pixR)>200.){
      DataVector<const Trk::TrackStateOnSurface>* cltsos = new DataVector<const Trk::TrackStateOnSurface>;
      const Trk::FitQuality* fq = (*it)->fitQuality()->clone();
      // copy track Si states into track
      DataVector<const Trk::TrackStateOnSurface>::const_iterator p_tsos;
      for(p_tsos=newtsos->begin()+nPixHits;p_tsos!=newtsos->end();++p_tsos){
        cltsos->push_back( (*p_tsos)->clone() );
      }
      ///Construct the new track
      Trk::TrackInfo info;
 //     info.setPatternRecognitionInfo(Trk::TrackInfo::TRTSeededTrackFinder);
      Trk::Track* nTrack = new Trk::Track(info, cltsos, fq);
      cleanSiTrack.push_back(nTrack);
      delete (*it);
    }else{
      cleanSiTrack.push_back((*it));
    }
  }

  return cleanSiTrack;
}


///////////////////////////////////////////////////////////////////
// Test is it track with calo seed compatible
///////////////////////////////////////////////////////////////////

bool InDet::TRT_SeededTrackFinder_ATL::isCaloCompatible(const Trk::TrackParameters& Tp)
{
  const double pi = M_PI, pi2 = 2.*M_PI;
  if(m_caloF.empty()) return false;   

  std::list<double>::iterator f = m_caloF.begin(), fe = m_caloF.end();
  std::list<double>::iterator e = m_caloE.begin();

  const AmgVector(5)& Vp = Tp.parameters(); 

  double F = Vp[2]; 
//    	msg(MSG::WARNING) << "CheckCal dedf \t" << F << "\t" << E << endreq;


  for(; f!=fe; ++f) {
    double df = fabs(F-(*f));
    if(df > pi        ) df = fabs(pi2-df);
  //  	msg(MSG::WARNING) << "CheckCal df \t" << df << endreq;

    if(df < m_phiWidth) {
   //   double de = fabs(E-(*e));
    //	msg(MSG::WARNING) << "CheckCal de \t" << de << endreq;
//      if(de < m_etaWidth) return true;
	return true;
    }
    ++e;
  }
  return false;
}



///////////////////////////////////////////////////////////////////
// MagneticFieldProperties production
///////////////////////////////////////////////////////////////////
void  InDet::TRT_SeededTrackFinder_ATL::magneticFieldInit()
{
Trk::MagneticFieldProperties* pMF = 0; 
 if(m_fieldmode == "NoField") pMF = new Trk::MagneticFieldProperties(Trk::NoField  ); 
 else if(m_fieldmode == "MapSolenoid") pMF = new Trk::MagneticFieldProperties(Trk::FastField); 
 else pMF = new Trk::MagneticFieldProperties(Trk::FullField); 
 m_fieldprop = *pMF; 

 delete pMF; 
}












