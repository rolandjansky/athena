/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
  InDetConversionFinderTools.cxx  -  Description
  -------------------
  begin   : 28-08-2006
  authors : Tatjana Lenz
  email   : tatjana.lenz@cern.ch
  changes :
***************************************************************************/
#include "InDetConversionFinderTools/InDetConversionFinderTools.h"

#include "CLHEP/Vector/ThreeVector.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "DataModel/ElementLink.h"
#include "TrkTrack/LinkToTrack.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "InDetConversionFinderTools/ConversionPostSelector.h"
#include "InDetConversionFinderTools/SingleTrackConversionTool.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "InDetConversionFinderTools/ConversionFinderUtils.h"
#include "InDetConversionFinderTools/VertexPointEstimator.h"
#include "InDetConversionFinderTools/ConversionPostSelector.h"
#include "InDetConversionFinderTools/SingleTrackConversionTool.h"
#include "InDetConversionFinderTools/TrackPairsSelector.h"
#include <vector>
#include <utility> 


#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"

//using CLHEP::Hep3Vector;

//Necessary for the TrackParticleContainer
#include "Particle/TrackParticleContainer.h"

namespace InDet
{
  double InDetConversionFinderTools::s_innerDetectorR = 500.;
  
  InDetConversionFinderTools::InDetConversionFinderTools(const std::string& t, const std::string& n, const IInterface* p) 
    :  AthAlgTool(t,n,p),
       m_TrkParticleCollection("TrackParticleCandidate"),
       m_iVertexFitter("Trk::FullVertexFitter"),
       m_trackPairsSelector("InDet::TrackPairsSelector"),
       m_helpertool("InDet::ConversionFinderUtils"),
       m_vertexEstimator("InDet::VertexPointEstimator"),
       m_postSelector("InDet::ConversionPostSelector"),
       m_singleTrkConvTool("InDet::SingleTrackConversionTool"),
       m_extrapolator ("Trk::Extrapolator/InDetExtrapolator"),
       m_trkSelector("InDet::TrackSelectorTool")
  {
    m_mindR          = -350.;
    m_maxdR          = 250.;
    m_MinInitVtxR    = 0.;
    m_MinFlightAngle = 0.;

    declareInterface<IVertexFinder>(this);
    declareProperty("VertexFitterTool",           m_iVertexFitter);
    declareProperty("TrackPairsSelector",         m_trackPairsSelector);
    declareProperty("ConversionFinderHelperTool", m_helpertool);
    declareProperty("VertexPointEstimator",       m_vertexEstimator);
    declareProperty("PostSelector",               m_postSelector);
    declareProperty("SingleTrackConversionTool",  m_singleTrkConvTool);
    declareProperty("Extrapolator",               m_extrapolator );
    declareProperty("TrackParticleCollection",    m_TrkParticleCollection); //Input track particle collection
    declareProperty("RemoveTrtTracks",            m_removeTrt); //Remove standalone TRT tracks
    declareProperty("IsConversion",               m_isConversion); //Conversion or V0s
    declareProperty("DecorateVertices",           m_decorateVertices=true); //Decorate vertices with values used for vertex selection  
    declareProperty("TrackSelectorTool",          m_trkSelector);
    declareProperty("MinDistVtxHit",              m_mindR);
    declareProperty("MaxDistVtxHit",              m_maxdR);
    declareProperty("MinInitVtxR",                m_MinInitVtxR);
    declareProperty("MinFlightAngle",             m_MinFlightAngle);
  }
  
  InDetConversionFinderTools::~InDetConversionFinderTools(){}
  
  StatusCode InDetConversionFinderTools::initialize()
  {
    StatusCode sc = AthAlgTool::initialize();
    if ( sc.isFailure() ) {
      msg(MSG::FATAL) << "Unable to initialize InDetConversionFinderTools" << endreq;
      return StatusCode::FAILURE;
    } 
    /* Get the right vertex fitting tool from ToolSvc */
    if ( m_iVertexFitter.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_iVertexFitter << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_iVertexFitter << endreq;
    }
    /* Get the track pairs selector tool from ToolSvc */
    if ( m_trackPairsSelector.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_trackPairsSelector << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_trackPairsSelector << endreq;
    }
    
    /* Get the vertex point estimator tool from ToolSvc */
    if ( m_vertexEstimator.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_vertexEstimator << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_vertexEstimator << endreq;
    }
    /* Get the postselector tool from ToolSvc */
    if ( m_postSelector.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_postSelector << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_postSelector << endreq;
    }
    /* Get the single track conversion tool from ToolSvc */
    if ( m_singleTrkConvTool.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_singleTrkConvTool << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_singleTrkConvTool << endreq;
    }
    /* Get the extrapolator tool from ToolSvc */
    if ( m_extrapolator.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_extrapolator << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_extrapolator << endreq;
    }
    /* Get the track selector tool from ToolSvc */
    if ( m_trkSelector.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_trkSelector << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_trkSelector << endreq;
    }
    
    msg(MSG::INFO) << "Initialization successful" << endreq;
    return StatusCode::SUCCESS;
  }
  
  StatusCode InDetConversionFinderTools::finalize()
  {
    return StatusCode::SUCCESS;
  }

  VxContainer* InDetConversionFinderTools::findVertex(const Trk::TrackParticleBaseCollection* /*trk_coll*/){
    // Make collection for conversions.
    
    ATH_MSG_ERROR("Using old TrackParticle Container no longer supported returning an empty conatiner");
    
    VxContainer* InDetConversionContainer = new VxContainer;
    return  InDetConversionContainer; 
  } 


  //TrackCollection
  VxContainer* InDetConversionFinderTools::findVertex(const TrackCollection* /*trk_coll*/)
  {
    
    ATH_MSG_ERROR("Using Track Container not currently supported returning an empty conatiner");

    // Make collection for conversions.
    VxContainer* InDetConversionContainer = new VxContainer;
    return  InDetConversionContainer;
   /* 
    ///Retrieve track particles from StoreGate for linking in the end
    //
    const Rec::TrackParticleContainer* TPC;
    StatusCode sc = evtStore()->retrieve(TPC,m_TrkParticleCollection);
    if(sc.isFailure()){
      msg(MSG::ERROR) << "No track particle collection with name " << m_TrkParticleCollection << " found in StoreGate!" << endreq;
      return InDetConversionContainer;
    }else{
      ATH_MSG_DEBUG("Found track particle collection "<<m_TrkParticleCollection);
    }
    ATH_MSG_DEBUG("Track particle container size "<<TPC->size());
    
    // Count for number of successful conversions
    int numConversions = 0;
    
    //have to be used for the vertex fit
    Amg::Vector3D pos(0.,0.,0.); Amg::Vector3D initPos(0.,0.,0.);
    Trk::Vertex vertex(pos); const Trk::Vertex* vx = 0;
    // Make seperate lists of positive and negative tracks (after presection cuts)
    std::vector<const Trk::Track*> negSelectedTracks; negSelectedTracks.clear();
    std::vector<const Trk::Track*> posSelectedTracks; posSelectedTracks.clear();
    std::vector<int> negIndx; std::vector<int> posIndx;
    
    //track preselection -->pt-cut
    TrackCollection::const_iterator iter;
    for ( iter =(*trk_coll).begin(); iter !=(*trk_coll).end(); ++iter ) {
      const Trk::Perigee * perigee;
      perigee=(*iter)->perigeeParameters();
      if (perigee==0) {
        ATH_MSG_DEBUG("No perigees for the tracks!!!");
      } else {
        double qOverP = perigee->parameters()[Trk::qOverP];
        if(m_trkSelector->decision(**iter, vx)){ // Only save if track passes the pt, d0 and z0 cuts
          if (qOverP < 0) {negSelectedTracks.push_back(*iter); negIndx.push_back(0);}
          else {posSelectedTracks.push_back(*iter); posIndx.push_back(0);}
        }
      }
    }// end track preselection cuts
    ATH_MSG_DEBUG ("Of " << trk_coll->size() << " tracks "
       << negSelectedTracks.size() << " neg charged tracks and " << posSelectedTracks.size() 
       << " pos charged tracks " << " survived the preselection.");
    
    // Make track pairs.
    std::vector<const Trk::Track*>::const_iterator iter_pos;
    std::vector<const Trk::Track*>::const_iterator iter_neg;
    std::vector<const Trk::TrackParameters*> trackList; trackList.clear();
    std::vector<const Trk::Track*> singleTrackConvList; singleTrackConvList.clear();
    int ipos = -1; int ineg = -1;
    // Outer loop: Loop over positive tracks 
    for (iter_pos = posSelectedTracks.begin(); iter_pos != posSelectedTracks.end(); ++iter_pos) {
      ineg = -1;
      ipos++;
      // Inner loop: Loop over negative tracks 
      for (iter_neg = negSelectedTracks.begin(); iter_neg != negSelectedTracks.end(); ++iter_neg) {
        ineg++;

        //Position of first measurement of two tracks
        int flag = 0;
        double firstMeasurementRpos = 0.; double firstMeasurementRneg = 0.;
        const DataVector<const Trk::MeasurementBase>* mb_pos = (*iter_pos)->measurementsOnTrack();
        if(!mb_pos) continue;
        DataVector<const Trk::MeasurementBase>::const_iterator itp_pos=mb_pos->begin();
        if(*itp_pos) firstMeasurementRpos = (*itp_pos)->globalPosition().perp();
        const DataVector<const Trk::MeasurementBase>* mb_neg = (*iter_neg)->measurementsOnTrack();
        if(!mb_neg) continue;
        DataVector<const Trk::MeasurementBase>::const_iterator itp_neg=mb_neg->begin();
        if (*itp_neg) firstMeasurementRneg = (*itp_neg)->globalPosition().perp();
        if(firstMeasurementRneg<=s_innerDetectorR && firstMeasurementRpos<=s_innerDetectorR) flag = 0;
        if((firstMeasurementRneg<=s_innerDetectorR && firstMeasurementRpos>s_innerDetectorR) || (firstMeasurementRneg>s_innerDetectorR && firstMeasurementRpos<=s_innerDetectorR)) flag=1;
        if(firstMeasurementRneg>s_innerDetectorR && firstMeasurementRpos>s_innerDetectorR) flag = 2;
        if(m_removeTrt && (flag==1 || flag==2)) continue;

        if(m_trackPairsSelector->selectTrackPair(*iter_pos,*iter_neg)) {
          //New point of minimum approach
          const Trk::Perigee* perPos = (*iter_pos)->perigeeParameters();
          const Trk::Perigee* perNeg = (*iter_neg)->perigeeParameters();
          int errorcode = 0;
          Amg::Vector3D startingPoint(m_vertexEstimator->getCirclesIntersectionPoint(perPos,perNeg,flag,errorcode));
          //Hep3Vector estimatedVtxPos(m_vertexEstimator->estimateVertexPosition(perPos,perNeg));
          if(m_isConversion && errorcode != 0) continue;
          if(!m_isConversion){
            Amg::Vector3D v_direction = perPos->momentum() + perNeg->momentum();
            double d_alpha = (startingPoint.adjoint()*v_direction)[0]/(startingPoint.mag()*v_direction.mag());
            if(d_alpha<m_MinFlightAngle) continue;
          }
          initPos = startingPoint;
          if(initPos.perp()<800.){
            //Get measured track parameters at first track measurement.
            const Trk::TrackParameters* trkPar_pos = m_helpertool->getTrkParameters((*iter_pos));
            const Trk::TrackParameters* trkPar_neg = m_helpertool->getTrkParameters((*iter_neg));
            if(!trkPar_pos || !trkPar_neg) continue;
            if(!m_isConversion){
              double guessR = initPos.perp();
              double posR = trkPar_pos->position().perp();
              double negR = trkPar_neg->position().perp();
              double diffR = 1000.;
              if((guessR-posR)<(guessR-negR)) diffR = guessR-posR;
              else                            diffR = guessR-negR;
              if(guessR<m_MinInitVtxR) continue;
              if(diffR<m_mindR || diffR>m_maxdR) continue;
            }
    
            //Alternative way to estimate the guess vertex
            const Amg::Vector3D global_pos (trkPar_pos->position());
            const Amg::Vector3D global_neg (trkPar_neg->position());
            double refFrame_x = (global_pos.x()+global_neg.x())/2.;
            double refFrame_y = (global_pos.y()+global_neg.y())/2.;
            double refFrame_z = (global_pos.z()+global_neg.z())/2.;
            Amg::Vector3D newInitPos(refFrame_x, refFrame_y, refFrame_z);
            initPos = newInitPos;
    
            //add tracks to the list to fit
            trackList.push_back(trkPar_pos);
            trackList.push_back(trkPar_neg);
          }
        }
  
        // Do the fit
        if (trackList.size() < 2) {
          ATH_MSG_DEBUG(" No tracks to fit ");
        }
        else {
          struct Trk::VxCandidate * myVxCandidate;
          Trk::Vertex estimatedVertex(initPos);
          myVxCandidate = m_iVertexFitter->fit(trackList, estimatedVertex);
          if (myVxCandidate) {
            ATH_MSG_DEBUG("Vertex Fit successful"); int type = -1;
            if ((m_isConversion && m_postSelector->selectConversionCandidate(myVxCandidate, flag, trackList)) ||
                (!m_isConversion && m_postSelector->selectSecVtxCandidate(myVxCandidate, flag, trackList, type))){
              ATH_MSG_DEBUG(" Conversion passed postselection cuts");
              numConversions++;
              std::vector<Trk::VxTrackAtVertex*> * trkAtVtx = myVxCandidate->vxTrackAtVertex();
              for (unsigned int i = 0; i < trkAtVtx->size() ; ++i) {
                Trk::LinkToTrackParticleBase* newLinkToTrackPB=new Trk::LinkToTrackParticleBase();
                if(i == 0) {
                  Rec::TrackParticleContainer::const_iterator TPCIter;
                  for(TPCIter=TPC->begin();TPCIter!=TPC->end();++TPCIter) {
                    const Trk::Track* trkTPC = (*TPCIter)->originalTrack();
                    if((*iter_pos)==trkTPC) {
                      newLinkToTrackPB->setElement((*TPCIter));
                      break;
                    }
                  }
                }
                if(i == 1) {
                  Rec::TrackParticleContainer::const_iterator TPCIter;
                  for(TPCIter=TPC->begin();TPCIter!=TPC->end();++TPCIter) {
                    const Trk::Track* trkTPC = (*TPCIter)->originalTrack();
                    if((*iter_neg)==trkTPC) {
                      newLinkToTrackPB->setElement((*TPCIter));
                      break;
                    }
                  }
                }
                newLinkToTrackPB->setStorableObject(*TPC);
                (*(trkAtVtx))[i]->setOrigTrack(newLinkToTrackPB);
              }
              if(m_isConversion) myVxCandidate->setVertexType(Trk::ConvVtx);
              else{
                if((type == 100) || (type == 110) || (type == 101)) myVxCandidate->setVertexType(Trk::V0KShort);
                if((type == 10) || (type == 11))                    myVxCandidate->setVertexType(Trk::V0Lambda);
                if(type == 1)                                       myVxCandidate->setVertexType(Trk::V0LambdaBar);
              }
              InDetConversionContainer->push_back(myVxCandidate);
              if(!m_isConversion && (type==101 || type==110 || type==11)) {
                //Trk::VxCandidate* myVxCandidate2 = new Trk::VxCandidate(*myVxCandidate);
                Trk::VxCandidate* myVxCandidate2 = myVxCandidate->clone();
                if((type == 101) || (type == 11)) myVxCandidate->setVertexType(Trk::V0LambdaBar);
                if(type == 110)                   myVxCandidate->setVertexType(Trk::V0Lambda);
                InDetConversionContainer->push_back(myVxCandidate2);
              }
              negIndx[ineg] = 1;
              posIndx[ipos] = 1;
            } else {
              ATH_MSG_DEBUG(" Conversion did not pass postselection cuts ");
              delete myVxCandidate;
            }//chi2 cut
          } else {
            ATH_MSG_DEBUG("Vertex Fit failed ");
          }
        }//if tracks to fit
        trackList.clear(); //delete pb_pos; delete pb_neg;
      } // neg loop
    } // pos loop
    ATH_MSG_DEBUG("Number of conversions found passing post selection cuts: "<<numConversions);
    
    if(m_isConversion) {
      //Single Track Conversions
      for(int ip=0;ip<int(posIndx.size());++ip) {
        if(posIndx[ip]==0) singleTrackConvList.push_back(posSelectedTracks[ip]);
      }
      for(int in=0;in<int(negIndx.size());++in) {
        if(negIndx[in]==0) singleTrackConvList.push_back(negSelectedTracks[in]);
      }
      
      std::vector<const Trk::Track*>::const_iterator itk  = singleTrackConvList.begin();
      std::vector<const Trk::Track*>::const_iterator itke = singleTrackConvList.end(); 
      int numSingle = 0;
      for(; itk!=itke; ++itk){
        if(m_singleTrkConvTool->selectSingleTrackConversion(*itk)){
          Trk::VxCandidate * sConver(0);
          sConver = m_singleTrkConvTool->buildSingleTrackConversion(*itk);
          if(sConver) {
            std::vector<Trk::VxTrackAtVertex*> * sTrkAtVtx = sConver->vxTrackAtVertex();
            for (unsigned int i = 0; i < sTrkAtVtx->size() ; ++i) {
              Trk::LinkToTrackParticleBase* newLinkToTrackPB=new Trk::LinkToTrackParticleBase();
              Rec::TrackParticleContainer::const_iterator TPCIter;
              for(TPCIter=TPC->begin();TPCIter!=TPC->end();++TPCIter) {
          const Trk::Track* trkTPC = (*TPCIter)->originalTrack();
          if((*itk)==trkTPC) {
            newLinkToTrackPB->setElement((*TPCIter));
            break;
          }
        }
        newLinkToTrackPB->setStorableObject(*TPC);
        (*(sTrkAtVtx))[i]->setOrigTrack(newLinkToTrackPB);
      }
      sConver->setVertexType(Trk::ConvVtx);
      InDetConversionContainer->push_back(sConver);
      numSingle++;
    }
  }
      }
      ATH_MSG_DEBUG("Number successful reconstructed single track conversion: "<<numSingle);
    }
    
    return InDetConversionContainer;
    */
  }

  //TrackParticleBaseCollection
  std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> InDetConversionFinderTools::findVertex ( const xAOD::TrackParticleContainer* trk_coll )
  {
    // Make collection for conversions.
    xAOD::VertexContainer* InDetConversionContainer = new xAOD::VertexContainer();
    xAOD::VertexAuxContainer* InDetConversionContainerAux = new xAOD::VertexAuxContainer();
    InDetConversionContainer->setStore( InDetConversionContainerAux ); 
    // Count for number of successful conversions
    int numConversions = 0;
    
    //have to be used for the vertex fit
    Amg::Vector3D pos(0.,0.,0.); Amg::Vector3D initPos(0.,0.,0.);
    Trk::Vertex vertex(pos); 
    
    // Make seperate lists of positive and negative tracks (after presection cuts)
    std::vector<const xAOD::TrackParticle*> negSelectedTracks; negSelectedTracks.clear();
    std::vector<const xAOD::TrackParticle*> posSelectedTracks; posSelectedTracks.clear();
    std::vector<int> negIndx; std::vector<int> posIndx;
    
    //track preselection -->pt-cut
    xAOD::TrackParticleContainer::const_iterator iter;
    for ( iter =(*trk_coll).begin(); iter !=(*trk_coll).end(); ++iter ) {    
      if (m_trkSelector->decision(**iter, 0)){ // Only save if track passes the pt, d0 and z0 cuts
        ATH_MSG_DEBUG("Track passed preselection");
        if ( (*iter)->charge() < 0) {negSelectedTracks.push_back(*iter); negIndx.push_back(0);}
        else {posSelectedTracks.push_back(*iter); posIndx.push_back(0);}
      }
      else         ATH_MSG_DEBUG("Track failed preselection");
    }// end pt,d0.z0-cuts
  
    // Make track pairs.
    std::vector<const xAOD::TrackParticle*>::const_iterator iter_pos;
    std::vector<const xAOD::TrackParticle*>::const_iterator iter_neg;
    std::vector<Amg::Vector3D> positionList; positionList.clear();
    std::vector<const xAOD::TrackParticle*> trackParticleList; trackParticleList.clear();
    std::vector<const xAOD::TrackParticle*> singleTrackConvList; singleTrackConvList.clear();
    int ipos = -1; int ineg = -1;
    // Outer loop: Loop over positive tracks 
    for (iter_pos = posSelectedTracks.begin(); iter_pos != posSelectedTracks.end(); ++iter_pos) {
      ineg = -1;
      ipos++;
      // Inner loop: Loop over negative tracks 
      for (iter_neg = negSelectedTracks.begin(); iter_neg != negSelectedTracks.end(); ++iter_neg) {
        ineg++;
        int flag = 0;
  
        if (!passPreSelection( *iter_pos , *iter_neg, positionList, initPos, flag)){ 
          positionList.clear(); 
          continue;
        }
  
        // Do the fit
        if (positionList.size() < 2) {
          ATH_MSG_DEBUG("No tracks to fit ");
          positionList.clear(); 
          continue;
        } 
        
        trackParticleList.push_back( *iter_pos );
        trackParticleList.push_back( *iter_neg );
        
        xAOD::Vertex* myVertex =0;
        Trk::Vertex estimatedVertex(initPos);
        myVertex = m_iVertexFitter->fit(trackParticleList, estimatedVertex);
        trackParticleList.clear();
        if(myVertex) {
          ATH_MSG_DEBUG("VertexFit successful!"); 
          int type = -1;
          if (( m_isConversion && m_postSelector->selectConversionCandidate(myVertex,flag,positionList)) ||
              (!m_isConversion && m_postSelector->selectSecVtxCandidate(myVertex, flag, positionList, type))){
            ATH_MSG_DEBUG(" Conversion passed postselection cuts");
            
            if (m_decorateVertices)
            {
              ATH_MSG_DEBUG("Decorating vertex with values used in track pair selector");
                for (auto kv : m_trackPairsSelector->getLastValues())
                  myVertex->auxdata<float>(kv.first) = kv.second;

              ATH_MSG_DEBUG("Decorating vertex with values used in vertex point estimator");
                for (auto kv : m_vertexEstimator->getLastValues())
                  myVertex->auxdata<float>(kv.first) = kv.second;
            }
            
            //Really need to check that this correct.
            //Remove old element links
            myVertex->clearTracks();
            ElementLink<xAOD::TrackParticleContainer> newLinkPos;
            newLinkPos.setElement(*iter_pos);
            newLinkPos.setStorableObject(*trk_coll);
            newLinkPos.index();
            ElementLink<xAOD::TrackParticleContainer> newLinkNeg;
            newLinkNeg.setElement(*iter_neg);
            newLinkNeg.setStorableObject(*trk_coll);
            newLinkNeg.index();
            
            myVertex->addTrackAtVertex(newLinkPos);
            myVertex->addTrackAtVertex(newLinkNeg);
              
          
            if (m_isConversion)
            {
              myVertex->setVertexType(xAOD::VxType::ConvVtx);
//               if (myVertex->vxTrackAtVertexAvailable()) myVertex->vxTrackAtVertex().clear();
              InDetConversionContainer->push_back(myVertex);
            }
            else if (type==101 || type==110 || type==11) // V0
            {
              myVertex->setVertexType(xAOD::VxType::V0Vtx);
              InDetConversionContainer->push_back(myVertex);
            }
            negIndx[ineg] = 1;
            posIndx[ipos] = 1;
            numConversions++;
          }else {
            ATH_MSG_DEBUG("VxCandidate failed the post selection cuts!");
            delete myVertex;
          }
        } else {
          ATH_MSG_DEBUG("VertexFit was NOT successful!");
        }      
        positionList.clear();
      } // neg loop
    } // pos loop
    ATH_MSG_DEBUG("Number of conversions found passing post selection cuts: "<<numConversions);
    
    if(m_isConversion) {
      //single track conversions
      for(int ip=0;ip<int(posIndx.size());++ip) {
        if(posIndx[ip]==0) singleTrackConvList.push_back(posSelectedTracks[ip]);
      }
      for(int in=0;in<int(negIndx.size());++in) {
        if(negIndx[in]==0) singleTrackConvList.push_back(negSelectedTracks[in]);
      }
      
      std::vector<const xAOD::TrackParticle*>::iterator itk, itke=singleTrackConvList.end();
      int numSingle = 0;
      for(itk=singleTrackConvList.begin();itk!=itke;++itk){
        if(!m_singleTrkConvTool->selectSingleTrackParticleConversion((*itk)))
          ATH_MSG_DEBUG("Track failed single track conversion selection");
        else
        {
          xAOD::Vertex * sConver(0);
          sConver = m_singleTrkConvTool->buildSingleTrackParticleConversion((*itk), InDetConversionContainer);
          if(sConver) {
            sConver->clearTracks();

            ElementLink<xAOD::TrackParticleContainer> newLink;
            newLink.toContainedElement( *trk_coll, *itk );
            sConver->addTrackAtVertex(newLink);
            sConver->setVertexType(xAOD::VxType::ConvVtx);
            numSingle++;
            
            if (m_decorateVertices)
            {
              ATH_MSG_DEBUG("Decorating single track vertex with dummy values used in track pair selector");
              for (auto kv : m_trackPairsSelector->getLastValues())
                sConver->auxdata<float>(kv.first) = 0.;

              ATH_MSG_DEBUG("Decorating single track vertex with dummy values used in vertex point estimator");
              for (auto kv : m_vertexEstimator->getLastValues())
                sConver->auxdata<float>(kv.first) = 0.;
              
              ATH_MSG_DEBUG("Decorating single track vertex with dummy values used in post selector");
              m_postSelector->decorateVertex(*sConver, 0., 0., 0., 0., 0.);
            }

            
          }
        }
      }
      ATH_MSG_DEBUG("Number successful reconstructed single track conversion: "<<numSingle);
    }
        
    return std::make_pair(InDetConversionContainer,InDetConversionContainerAux); 
  } 

  bool InDetConversionFinderTools::passPreSelection(const xAOD::TrackParticle* track_pos, const xAOD::TrackParticle* track_neg, std::vector<Amg::Vector3D>&  trackList, Amg::Vector3D& initPos,  int& flag )
  {
    //Track summary information
  
    uint8_t nclusPos(0);
    uint8_t dummy(0);
    if(track_pos->summaryValue(dummy,xAOD::numberOfSCTHits)){
      nclusPos += dummy;
    } 
    if(track_pos->summaryValue(dummy,xAOD::numberOfPixelHits)){
      nclusPos += dummy;
    } 
  
    uint8_t nclusNeg(0);
    if(track_neg->summaryValue(dummy,xAOD::numberOfSCTHits)){
      nclusNeg += dummy;
    } 
    if(track_neg->summaryValue(dummy,xAOD::numberOfPixelHits)){
      nclusNeg += dummy;
    } 
  
  
    if(nclusNeg>0 && nclusPos>0) flag= 0;
    if((nclusNeg>0 && nclusPos==0) || (nclusNeg==0 && nclusPos>0)) flag = 1;
    if(nclusNeg==0 && nclusPos==0) flag = 2;
    if(m_removeTrt && (flag==1 || flag==2)) return false;

    if (m_trackPairsSelector->selectTrackParticlePair( track_pos,track_neg)){
  
      const Trk::Perigee& perPos = track_pos->perigeeParameters();
      const Trk::Perigee& perNeg = track_neg->perigeeParameters();
      int errorcode = 0;
      Amg::Vector3D startingPoint(m_vertexEstimator->getCirclesIntersectionPoint(&perPos,&perNeg,flag,errorcode));
      if(m_isConversion && errorcode != 0) return false;
      if(!m_isConversion){ 
        Amg::Vector3D v_direction = perPos.momentum() + perNeg.momentum();
        double d_alpha = (startingPoint.adjoint()*v_direction)[0]/(startingPoint.mag()*v_direction.mag());
        if(d_alpha<m_MinFlightAngle) return false;
      }
      initPos = startingPoint;
      double startingPointR = startingPoint.perp(); 
      if(startingPointR<800.) {
        //Get measured track parameters at first track measurement.
        
        //Position of first hit in track particle
        Trk::CurvilinearParameters trkPar_pos; 
        Trk::CurvilinearParameters trkPar_neg; 

        int index(-1);
        for(unsigned int i(0); i< track_pos->numberOfParameters() ; ++i ){
          if( xAOD::FirstMeasurement == track_pos->parameterPosition(i) ){
            index = i;
            break;
          }
        }
        if(index!=-1){
          trkPar_pos = track_pos->curvilinearParameters(index);
        } else {
          ATH_MSG_WARNING("Track Particle does not contain first Measurement track parameters");
          return false;
        }
    
        index = -1;
        for(unsigned int i(0); i< track_neg->numberOfParameters() ; ++i ){
          if( xAOD::FirstMeasurement == track_neg->parameterPosition(i) ){
            index = i;
            break;
          }
        }
        if(index!=-1){
          trkPar_neg = track_neg->curvilinearParameters(index);
        } else {
          ATH_MSG_WARNING("Track Particle does not contain first Measurement track parameters");
          return false;
        }

        
        if(!m_isConversion){
          double posR = trkPar_pos.position().perp();
          double negR = trkPar_neg.position().perp();
          double diffR = 1000.;
          if((startingPointR-posR)<(startingPointR-negR)) diffR = startingPointR-posR;
          else                            diffR = startingPointR-negR;
          if(startingPointR<m_MinInitVtxR) return false;
          if(diffR<m_mindR || diffR>m_maxdR) return false;
        }

        //Alternative way to estimate the guess vertex
        initPos = trkPar_pos.position() + trkPar_neg.position();
        initPos *= 0.5;
        // Not correct but will do for now
        trackList.push_back(trkPar_pos.position());
        trackList.push_back(trkPar_neg.position());
        
      }
    }
    return true;
  }
}// end namespace InDet
