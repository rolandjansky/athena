/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//********************************************************/
// @author <yuriy.pylypchenko@cern.ch>                    /
// @date Oct, 2010                                        /
// @brief setting up an association of clusters to tracks /
//       represented by indices.                          /
//********************************************************/
 
 
#include "TrackParticleToSCTClusterAssociationTool.h"
#include "AthenaKernel/errorcheck.h"
#include "Particle/TrackParticle.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"

#include "TrkTrack/TrackStateOnSurface.h"
#include "Identifier/Identifier.h"
#include "TrkEventPrimitives/TrackStateDefs.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "TrkParameters/TrackParameters.h"



#include "InDetIdentifier/SCT_ID.h"

#include <cmath>
#include <map>

namespace D3PD {


TrackParticleToSCTClusterAssociationTool::TrackParticleToSCTClusterAssociationTool (const std::string& type,
								const std::string& name,
								const IInterface* parent)
  : TrackParticleToSCTClusterToTrackBase(type, name, parent),
    m_idHelper(0),
    m_sctId(0)
{
  book().ignore(); // Avoid coverity warnings.
}
 

/// initialize method.
StatusCode TrackParticleToSCTClusterAssociationTool::initialize()
{
 

  // need Atlas id-helpers to identify sub-detectors, take them from detStore
  if (detStore()->retrieve(m_idHelper, "AtlasID").isFailure()) {
    REPORT_MESSAGE (MSG::ERROR) << "Could not get AtlasDetectorID helper";
    return StatusCode::FAILURE;
  }

  if(detStore()->retrieve(m_sctId,"SCT_ID").isFailure() ){
    REPORT_MESSAGE (MSG::FATAL) << "Could not retrieve SCT helper";
    return StatusCode::FAILURE;
  }

  if (m_SCTDetEleCollKey.initialize().isFailure()) {
    REPORT_MESSAGE (MSG::FATAL) << "Could not initialize " << m_SCTDetEleCollKey.fullKey();
    return StatusCode::FAILURE;
  }

  return  TrackParticleToSCTClusterToTrackBase::initialize();
}
/// booking btuple variables
StatusCode TrackParticleToSCTClusterAssociationTool::book()
{
 
  CHECK( TrackParticleToSCTClusterToTrackBase::book() );
  
  CHECK( addVariable( "clX" , m_clGlobX) );
  CHECK( addVariable( "clY" , m_clGlobY) );
  CHECK( addVariable( "clZ" , m_clGlobZ) );
  CHECK( addVariable( "clLocX" , m_clLocX) );
  CHECK( addVariable( "clLocY" , m_clLocY) );
  CHECK( addVariable( "clId" , m_id) );
  CHECK( addVariable( "clDetElementId" , m_detElementId) );
  CHECK( addVariable( "clDetType" , m_detType) );
  CHECK( addVariable( "clBEC" , m_bec) );
  CHECK( addVariable( "clLayer" , m_layer) );
  CHECK( addVariable( "locX" , m_locX) );
  CHECK( addVariable( "locY" , m_locY) );
  CHECK( addVariable( "errLocX" , m_err_locX) );
  CHECK( addVariable( "errLocY" , m_err_locY) );
  CHECK( addVariable( "covLocXY" , m_cov_locXY) );
  CHECK( addVariable( "x" , m_x) );
  CHECK( addVariable( "y" , m_y) );
  CHECK( addVariable( "z" , m_z) );
  CHECK( addVariable( "trkLocX" , m_trkLocX) );
  CHECK( addVariable( "trkLocY" , m_trkLocY) );
  CHECK( addVariable( "errTrkLocX" , m_err_trkLocX) );
  CHECK( addVariable( "errTrkLocY" , m_err_trkLocY) );
  CHECK( addVariable( "covTrkLocXY" , m_cov_trkLocXY) );
  CHECK( addVariable( "isCompetingRIO" , m_isCompetingRIO) );
  CHECK( addVariable( "size" , m_sizePhi) );
  CHECK( addVariable( "side" , m_sideModule) );
  CHECK( addVariable( "locPhi" , m_locPhi) );
  CHECK( addVariable( "locTheta" , m_locTheta) );
  CHECK( addVariable( "tsosType" , m_tsosType) );
  

  return StatusCode::SUCCESS;
}

///
/// @brief Start the iteration for a new association.
/// @param p The object from which to associate.
///
StatusCode TrackParticleToSCTClusterAssociationTool::reset (const  Rec::TrackParticle& p)
{

  const Trk::Track *track = p.originalTrack();
  if(!track){
    m_clusItr = m_clusEnd;
    //REPORT_MESSAGE (MSG::DEBUG) << "Could not access original track";
    return StatusCode::SUCCESS;
  }
  
  /// extra ntuple variable 
  
  m_clGlobX= 0;
  m_clGlobY= 0;
  m_clGlobZ= 0;
  m_clLocX= 0;
  m_clLocY= 0;
  m_locX= 0;
  m_locY= 0;
  m_err_locX= 0;
  m_err_locY= 0;
  m_cov_locXY= 0;
  m_x= 0;
  m_y= 0;
  m_z= 0;
  m_trkLocX= 0;
  m_trkLocY= 0;
  m_err_trkLocX= 0;
  m_err_trkLocY= 0;
  m_cov_trkLocXY= 0;
  m_isCompetingRIO= 0;
  m_sizePhi= 0;
  m_sideModule = 0;
  m_locPhi= 0;
  m_locTheta= 0;
  m_tsosType= 0;
  m_id= 0; // unique identifier for a cluster
  m_detElementId= 0; // unique identifier for a detector element
  m_detType= 0;
  m_bec= 0;
  m_layer= 0;

  m_sctClusterForAssociation.clear();

  const InDet::SCT_ClusterContainer* riocont; 
  if(!evtStore()->contains<InDet::SCT_ClusterContainer>("SCT_Clusters")){ 

	REPORT_MESSAGE (MSG::WARNING) << "No sct clusters....";

  } 
  else {
    StatusCode sc = evtStore()->retrieve(riocont, "SCT_Clusters");
    if(sc.isFailure()) {
      	REPORT_MESSAGE (MSG::WARNING) << "Could not get clusters....";
    }
  }


  std::vector< Identifier> sctClusterIdentifier;  

  const DataVector<const Trk::TrackStateOnSurface>* trackHits = track->trackStateOnSurfaces();
  
  if (!trackHits){
    m_clusItr = m_clusEnd;
    REPORT_MESSAGE (MSG::WARNING) << "TESTASSOC for current track measurementsOntrack == Null, no clusters will be associated with this track";
    return StatusCode::SUCCESS;
  }

  DataVector<const Trk::TrackStateOnSurface>::const_iterator it = trackHits->begin();

  for ( ; it != trackHits->end(); it++){

    const InDet::SiClusterOnTrack *clus = dynamic_cast<const InDet::SiClusterOnTrack*>((*it)->measurementOnTrack());

    if (clus){

      //const Trk::TrkDetElementBase* detectorElement = clus->detectorElement();
      // Dynamic cast to Silicon element and do Pixel / SCT element test
      //const InDetDD::SiDetectorElement* siliconDetectorElement = dynamic_cast< const InDetDD::SiDetectorElement* >(detectorElement);   

      // Get Pointer to prepRawDataObject	
      const InDet::SCT_Cluster *RawDataClus = dynamic_cast<const InDet::SCT_Cluster*>(clus->prepRawData());

      if (RawDataClus==0){
        //REPORT_MESSAGE (MSG::DEBUG) << "TESTASSOC SiCluster WITHOUT prepRawData!!!!";
      }

      // check if Cluster is in SCT ...
      if (RawDataClus!=0){
        if (RawDataClus->detectorElement()->isSCT()){

          //REPORT_MESSAGE (MSG::DEBUG) << "TESTASSOC SiHIT via prepRawData";
       
          Identifier sctClusterID = RawDataClus->identify();

          sctClusterIdentifier.push_back(sctClusterID);

          m_sctClusterForAssociation.push_back(*it);

          //REPORT_MESSAGE (MSG::DEBUG) << "(2)TESTASSOC TSoS-Type is: "<<   (*it)->types().to_ulong();

        }
      }
    }
  }

  m_clusItr = m_sctClusterForAssociation.begin(); //sct::vector<cont InDet::SCT_Cluster>::const_iterator 
  m_clusEnd = m_sctClusterForAssociation.end(); 

  return StatusCode::SUCCESS;
}

/**
* @brief Return a pointer to the next element in the association.
*
* Return 0 when the association has been exhausted.
*/
const InDet::SCT_Cluster* TrackParticleToSCTClusterAssociationTool::next()
  {

    const InDet::SCT_Cluster* RawDataClus = 0;

    // reached the end
    if(m_clusItr==m_clusEnd){
      return 0;
    }
    else{
    }
    
    *m_clGlobX= -9999.;
    *m_clGlobY= -9999.;
    *m_clGlobZ= -9999.;
    *m_clLocX= -9999.;
    *m_clLocY= -9999.;
    *m_locX= -9999.;
    *m_locY= -9999.;
    *m_err_locX= -9999.;
    *m_err_locY= -9999.;
    *m_cov_locXY= -9999.;
    *m_x= -9999.;
    *m_y= -9999.;
    *m_z= -9999.;
    *m_trkLocX= -9999.;
    *m_trkLocY= -9999.;
    *m_err_trkLocX= -9999.;
    *m_err_trkLocY= -9999.;
    *m_cov_trkLocXY= -9999.;
    *m_isCompetingRIO= 0;
    *m_sizePhi= 0.;
    *m_sideModule = -9999;
    *m_locPhi= -9999.;
    *m_locTheta= -9999.;
    *m_tsosType= (int)Trk::TrackStateOnSurface::Unknown;

    *m_id= 0; // unique identifier for a cluster
    *m_detElementId= 0; // unique identifier for a detector element
    *m_detType= Trk::TrackState::unidentified;
    *m_bec= -9999.;
    *m_layer= -1;

    /*
    *m_locPhiOld = -9999.;
    *m_locThetaOld = -9999.;
    *m_locPhiOldNotCor = -9999.; 
    *m_locThetaOldNotCor = -9999.;
    */

    const Trk::TrackStateOnSurface* pTSoS = *m_clusItr;
    long bitsetValue = (long)pTSoS->types().to_ulong();
    *m_tsosType = (int)bitsetValue;

    // TrackStateOnSurface part 

    // measured hit/outlier
    const Trk::MeasurementBase *measurement = pTSoS->measurementOnTrack();

    // track parameters at wafer
    const Trk::TrackParameters *tp = pTSoS->trackParameters();
    
    // Fill the hit/oultier local/global position variables
    if(measurement){
      
      // local position
      //for SCT only locX, locY ether not defined at all, i.e.-9999.
      // or 0, when the second parameter defined

      //local parameters
      const Trk::LocalParameters pars = measurement->localParameters();
      //local error matrix
      const Amg::MatrixX& err = measurement->localCovariance();

      //>fix1.. this produced FPE OVERFLOW last time 
      // *m_locX = (float)pars[Trk::locX];
      // *m_locY = (float)pars[Trk::locY];
      if ( pars.contains(Trk::locX) ){
        double locX = pars[Trk::locX];
        if ( !(std::isinf(locX) || std::isnan(locX)) ){
          *m_locX = (float)locX;
          *m_err_locX = (float)sqrt( err(Trk::locX,Trk::locX) );
        }
      }
      if ( pars.contains(Trk::locY) ){
        double locY = pars.get(Trk::locY);
        if ( !(std::isinf(locY) || std::isnan(locY)) ){
          if ( locY<1e-07 ) locY=0.;
          *m_locY = (float)locY;
          *m_err_locY = (float)sqrt( err(Trk::locY,Trk::locY) );
        }
      }
      if ( *m_locX > -9900 && *m_locY > -9900 ){
        double locXY = err(Trk::locX,Trk::locY);
        if (  !(std::isinf(locXY) || std::isnan(locXY)) )
          *m_cov_locXY = (float)locXY;
      }
      //<fix1
      //<fix1
   
      // global position
      const Amg::Vector3D& pos = measurement->globalPosition();
      //*m_x = (float)pos[Trk::x];
      //*m_y = (float)pos[Trk::y];
      //*m_z = (float)pos[Trk::z];
      *m_x = (float)pos.x();
      *m_y = (float)pos.y();
      *m_z = (float)pos.z();
    }

    // Fill the track local position at wafer
    if(tp){

      //const HepVector lp0 = tp->parameters();
      if ( tp->parameters().rows() > 1 ){
        *m_trkLocX=(float)tp->parameters()[Trk::locX]; //lp[Trk::locX];
        *m_trkLocY=(float)tp->parameters()[Trk::locY]; //lp[Trk::locY];
  
        const AmgSymMatrix(5)* err = tp->covariance();
       
        if(err){
          *m_err_trkLocX=(float)sqrt((*err)(Trk::locX,Trk::locX));
          *m_err_trkLocY=(float)sqrt((*err)(Trk::locY,Trk::locY));
          *m_cov_trkLocXY=(float)(*err)(Trk::locX, Trk::locY);
        }
      }
    } // if(tp)

    //*m_isCompetingRIO = 0;
    const Trk::RIO_OnTrack *rio = dynamic_cast<const Trk::RIO_OnTrack*>(measurement);
    if(!rio){
      const Trk::CompetingRIOsOnTrack *crot = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(measurement);
      if(crot){
        rio = &crot->rioOnTrack( crot->indexOfMaxAssignProb() );
        *m_isCompetingRIO = 1;
      }
    }
 

    // Fill varaibles representing SCT Cluster local and global coordinates
    if(rio){

      Identifier id = rio->identify();
   
      const Trk::TrkDetElementBase *de = rio->detectorElement();
      if(de){
        Identifier detId = de->identify();

        if ( detId.is_valid() ) {
          *m_detElementId = detId.get_compact();
  
          if (m_idHelper->is_sct(detId)) {
            *m_detType = (int)Trk::TrackState::SCT;
            *m_bec = (int)m_sctId->barrel_ec(id);
            *m_layer = (int)m_sctId->layer_disk(id);
            *m_sideModule = (int)m_sctId->side(id);
            const InDet::SCT_Cluster *clus = dynamic_cast<const InDet::SCT_Cluster*>(rio->prepRawData());
            if(clus){
              const InDet::SiWidth cw = clus->width();
              *m_sizePhi = (int)cw.colRow().x(); //cw.colRow()[0]; 
              RawDataClus = clus;

              *m_clGlobX = (float)RawDataClus->globalPosition().x();
              *m_clGlobY = (float)RawDataClus->globalPosition().y();
              *m_clGlobZ = (float)RawDataClus->globalPosition().z();

              const Amg::VectorX& locpos = RawDataClus->localPosition();
      
              if ( !(std::isinf(locpos[Trk::locX]) || std::isnan(locpos[Trk::locX])) )
                *m_clLocX = (float)locpos[Trk::locX];

              if ( !(std::isinf(locpos[Trk::locY]) || std::isnan(locpos[Trk::locY])) ){
                if (locpos[Trk::locY]<1e-07) 
                  *m_clLocY=0.;
                else
                  *m_clLocY = (float)locpos[Trk::locY];
              }    
            }
          }
        }
        else {
          REPORT_MESSAGE (MSG::WARNING) << "Pointer to detector element is not valid";
        }
      }
    } // if rio


    // Fill local incidence angles
    if ( measurement && rio && tp ) {

      const Trk::TrkDetElementBase *de = rio->detectorElement();

      if(de){
        Identifier detId = de->identify();

        if ( detId.is_valid() ) {
          if ( m_idHelper->is_sct(detId)) {

            const InDetDD::SiDetectorElement *side = dynamic_cast<const InDetDD::SiDetectorElement *>(de);
            if ( side ){

              //fix 2 is the modification of loc inc. angles
              //const InDet::SCT_Cluster *clus = dynamic_cast<const InDet::SCT_Cluster*>(rio->prepRawData());
              double sinAlpha = 0.;
              if(RawDataClus){
                // local Stereo anlge of strip with respect to locY axis
                // always 0. for Barrel modules
                sinAlpha = side->sinStereoLocal(RawDataClus->localPosition());
              }

              // normal component of the wafer's local coordinate system
              // (along global R for Barrel and global Z for Endcaps)
              Amg::Vector3D mynormal = side->normal(); 

              // side->phiAxis() is the X component of wafer's local coordinates 
              //(goes across the strips in increasing global Phi direction)
              // side->etaAxis() is the Y component of wafer's local coordinates
              //(goes along the strips in increasing global Eta direction)

              // Here we rotate around local Z axis to account for strip local stereo angle if any (EC)
              // "-" is because rotation is defined in the counter-clockwise sense,
              // and we need to go back (clockwise) to align the local axes with the
              //HepVector3D myphiax = rotationZOf(side->phiAxis(),asin(-sinAlpha));
              //HepVector3D myetaax = rotationZOf(side->etaAxis(),asin(-sinAlpha));

              //HepVector3D mytrack = tp->momentum();
              Amg::Translation3D translation(0.,0.,0.);
              Amg::Transform3D   transform1 = translation * Amg::AngleAxis3D( asin(-sinAlpha), Amg::Vector3D(0.,0.,1.));        

              Amg::Vector3D myphiax = transform1.linear() * side->phiAxis();
              Amg::Vector3D myetaax = transform1.linear() * side->etaAxis();

              Amg::Vector3D mytrack = tp->momentum();

              float trketacomp = (float)mytrack.dot(myetaax);
              float trkphicomp = (float)mytrack.dot(myphiax);
              float trknormcomp = (float)mytrack.dot(mynormal);
              if (std::abs(trknormcomp*1e07)>0. ){
                const double inv_trknormcomp = 1. / trknormcomp;
                *m_locPhi = (float)atan(trkphicomp * inv_trknormcomp); 
                *m_locTheta = (float)atan(trketacomp * inv_trknormcomp); 
              }
            }
          }
        }
        else{
          REPORT_MESSAGE (MSG::WARNING) << "Pointer to detector element is not valid";
        }
      }
    } // if measurement && rio && tp
       
    m_clusItr++; // to go throuhg all the HitsOnTrack

    return RawDataClus;

  }
 
 } // namespace D3PD
  
