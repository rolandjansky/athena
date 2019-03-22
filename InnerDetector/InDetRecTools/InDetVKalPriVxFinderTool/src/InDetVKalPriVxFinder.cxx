/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "InDetVKalPriVxFinderTool/InDetVKalPriVxFinderTool.h"
//-------------------------------------------------
// Other stuff
//#include "GaudiKernel/MsgStream.h"
//
#include  "InDetBeamSpotService/IBeamCondSvc.h"
//----
#include  "TrkTrackLink/ITrackLink.h"
#include  "TrkTrack/LinkToTrack.h"
#include  "TrkParticleBase/LinkToTrackParticleBase.h"
#include  "TrkTrackSummary/TrackSummary.h"
#include  "VxVertex/VxTrackAtVertex.h"
//#include  "VxVertex/VxContainer.h"
#include  "TrkEventPrimitives/FitQuality.h"
#include  "TrkToolInterfaces/ITrackSummaryTool.h"
#include  "TrkToolInterfaces/ITrackSelectorTool.h"
//----

//#include <iostream>


namespace InDet {


InDetVKalPriVxFinderTool::InDetVKalPriVxFinderTool(const std::string& type,
                                                   const std::string& name,
                                                   const IInterface* parent):
    AthAlgTool(type,name,parent),

    m_BeamPositionX(0.),
    m_BeamPositionY(0.),
    m_BeamConstraint(1),
    m_TypeRobust(5),
    m_RobustScale(1.),

    m_CutSctHits(5),
    m_CutPixelHits(2),
    m_CutSiHits(8),
    m_CutBLayHits(0),
    m_CutSharedHits(0),
    m_CutPt(300.),
    m_CutA0(1.0),
    m_CutChi2(3.5),

    m_A0TrkErrorCut(1.0),
    m_ZTrkErrorCut(5.0),
    m_RImpSelCut(50.),
    m_ZImpSelCut(50.),
    m_RDistSelCut(1.0),
    m_ZDistSelCut(3.5),
    m_SignifSelCut(50.),
    m_SecTrkChi2Cut(5.5),
    m_WeightCut(0.2),
 
    m_NPVertexMax(100),

    m_fitSvc("Trk::TrkVKalVrtFitter",this),

    m_sumSvc("Trk::TrackSummaryTool/AtlasTrackSummaryTool"),

    m_iBeamCondSvc("BeamCondSvc",name),
    
    m_trkSelector("DefaultTrackSelection")
    
    {
        declareInterface<IVertexFinder>(this);
        declareProperty("BeamPositionX",  m_BeamPositionX);
        declareProperty("BeamPositionY",  m_BeamPositionY);
        declareProperty("BeamConstraint", m_BeamConstraint);
        declareProperty("TypeRobust",     m_TypeRobust);
        declareProperty("RobustScale",    m_RobustScale);

        declareProperty("CutSctHits",    m_CutSctHits  );
        declareProperty("CutPixelHits",  m_CutPixelHits  );
        declareProperty("CutSiHits",     m_CutSiHits  );
        declareProperty("CutBLayHits",   m_CutBLayHits  );
        declareProperty("CutSharedHits", m_CutSharedHits );
        declareProperty("CutPt",         m_CutPt );
        declareProperty("CutA0",         m_CutA0 );
        declareProperty("CutChi2",       m_CutChi2 );

        declareProperty("A0TrkErrorCut",         m_A0TrkErrorCut );
        declareProperty("ZTrkErrorCut",          m_ZTrkErrorCut );

        declareProperty("RImpSelCut",     m_RImpSelCut  );
        declareProperty("ZImpSelCut",     m_ZImpSelCut  );
        declareProperty("RDistSelCut",    m_RDistSelCut  );
        declareProperty("ZDistSelCut",    m_ZDistSelCut  );
        declareProperty("SignifSelCut",   m_SignifSelCut  );

        declareProperty("SecTrkChi2Cut",   m_SecTrkChi2Cut  );
        declareProperty("WeightCut",       m_WeightCut  );

        declareProperty("NPVertexMax",     m_NPVertexMax  );

        declareProperty("FitterTool",          m_fitSvc  );
        declareProperty("TrackSummaryTool",    m_sumSvc  );
        declareProperty("BeamPositionSvc",     m_iBeamCondSvc);
        declareProperty("TrackSelectorTool",   m_trkSelector);

        m_BeamCondSvcExist=0;

  }

  InDetVKalPriVxFinderTool::~InDetVKalPriVxFinderTool()
  {    //MsgStream log( msgSvc(), name() ) ;
       //m_sc.isSuccess();
       if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "InDetVKalPriVxFinderTool destructor called" << endreq;
  }



//__________________________________________________________________________
  StatusCode InDetVKalPriVxFinderTool::initialize()
  { 
    if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<"InDetVKalVrtPriVxTool initialize()" << endreq;
    m_SummaryToolExist = 0;
//       ---------------
/* Tool Service initialisation.  NOT NEEDED now*/
//    IToolSvc* toolSvc;
//    StatusCode sc = service("ToolSvc", toolSvc);
//    if (sc.isFailure()) {
//      if(msgLvl(MSG::ERROR))msg(MSG::ERROR) << "Could not find ToolSvc" << endreq;
//      return StatusCode::SUCCESS; 
//    }

//TrkVKalVrtFitter
//    sc = toolSvc->retrieveTool("TrkVKalVrtFitter",m_fitSvc,this);
//    if (sc.isFailure()) { 
//       if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << " No VKalVrtFitSvc for InDetVKalPriVxFinderTool !" << endreq;
//       return StatusCode::SUCCESS; 
//    } 
//    if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "VKalVrtPromCpp's VKalVrtFitSvc found" << endreq;
//-----------------------------------------------------------------------------------------
//
//  Temporary to use old VKalVrt interface
//
//     Trk::IVertexFitter * tmp;
//     sc = toolSvc->retrieveTool("Trk::TrkVKalVrtFitter",tmp,this);
//     if (sc.isFailure()) { 
//        if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << " No Trk::TrkVKalVrtFitter for InDetVKalPriVxFinderTool !" << endreq;
//     }else{
//        m_fitSvc = dynamic_cast<Trk::TrkVKalVrtFitter*>(tmp);
//        if(!m_fitSvc && msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<" No Trk::TrkVKalVrtFitter" << endreq;
//     }
    if (m_fitSvc.retrieve().isFailure()) {
        if(msgLvl(MSG::ERROR))msg(MSG::ERROR) << "No Trk::TrkVKalVrtFitter for InDetVKalPriVxFinderTool !" << endreq;
    }else{
        if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Trk::TrkVKalVrtFitter found" << endreq;
    }
//
//TrackSummaryTool
//
    if (m_sumSvc.retrieve().isFailure()) {
        if(msgLvl(MSG::ERROR))msg(MSG::ERROR) << "Could not find TrackSummaryTool" << endreq;
    } else { m_SummaryToolExist = 1;}
//
//  Data for beam spot constraint
//
    m_BeamCnst[0]=m_BeamPositionX;
    m_BeamCnst[1]=m_BeamPositionY;
    m_BeamCnst[2]=0.;
    m_BeamCnstWid.resize(3);
    m_BeamCnstWid[0]=0.015;
    m_BeamCnstWid[1]=0.015;
    m_BeamCnstWid[2]=56.;

    if(m_BeamConstraint){
      //StatusCode sc = service("BeamCondSvc", m_iBeamCondSvc);
      if (m_iBeamCondSvc.retrieve().isFailure() ) 
      {	if(msgLvl(MSG::INFO))msg(MSG::INFO) << "Could not find BeamCondSvc." << endreq;
        m_BeamCondSvcExist=0;
      }else{
        m_BeamCondSvcExist=1;
      }
    }
//
//TrackSelectorTool
//
    if (m_trkSelector.name() == "DefaultTrackSelection" ){
       m_trkSelectorExist=0;
       if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Use internal track selection - see  default/jobO cuts" << endreq;
    }else{
       if (m_trkSelector.retrieve().isFailure()) {
          if(msgLvl(MSG::ERROR)){ msg(MSG::ERROR) << "Could not find TrackSelectorTool" << endreq;
                                  msg(MSG::ERROR) << "Use internal track selection - see  default/jobO cuts" << endreq;}
          m_trkSelectorExist=0;
       }else{ 
          if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Finder uses TrackSelectorTool" << endreq;
          m_trkSelectorExist=1; 
       }
    }

    return StatusCode::SUCCESS; 
  }



//__________________________________________________________________________


  StatusCode InDetVKalPriVxFinderTool::finalize()
  {
    if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) <<"InDetVKalPriVxFinderTool finalize()" << endreq;
    return StatusCode::SUCCESS; 
  }
  



   std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> InDetVKalPriVxFinderTool::findVertex(const xAOD::TrackParticleContainer* trackTES) 
   {  
        if(msgLvl(MSG::DEBUG)){
        msg(MSG::DEBUG) << "N="<<trackTES->size()<<" xAOD::TrackParticles found" << endreq;
        msg(MSG::DEBUG) << "No InDetVKalPriVxFinderTool implementation for xAOD::TrackParticle" << endreq;
     }
     return std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> (0,0);
   }


//__________________________________________________________________________
  //VxContainer* InDetVKalPriVxFinderTool::findVertex(const TrackCollection* trackTES)
  std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> InDetVKalPriVxFinderTool::findVertex(const TrackCollection* trackTES)
  {
    //.............................................
    
    if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Primary vertex with InDetVKalPriVxFinderTool starts" << endreq;
//
//
    savedTrkFittedPerigees.clear();
    m_fittedTrkCov.clear();
//------------------------------------------------------------------------------------------
//  Creating the necessary vectors

    std::vector<const Trk::Track*>              SelectedTrkTracks;
    std::vector<const Trk::TrackParticleBase*>  SelectedTrackParticles;
    std::vector<Amg::Vector3D>                  PrimVrtList;
    std::vector< AmgSymMatrix(3) >              ErrorMatrixPerVrt;
    std::vector<double>                         Chi2PerVrt;
    std::vector<double>                         ControlVariablePerVrt;
    std::vector< std::vector<const Trk::Track*> >              TrkPerVrt;
    std::vector< std::vector<const Trk::TrackParticleBase*> >  PrtPerVrt;
    std::vector< std::vector<double> >                         TrkWgtPerVrt;
    std::vector<int> NTrkPerVrt;
//
    int  NFoundVrt=0;
    SelectedTrkTracks.clear(); SelectedTrackParticles.clear(); NTrkPerVrt.clear();
    TrkPerVrt.clear(); PrtPerVrt.clear();

    const Trk::Perigee* m_mPer=NULL;
    AmgVector(5) VectPerig; VectPerig<<0.,0.,0.,0.,0.;
    const Trk::FitQuality*  TrkQual=0;
    std::vector<double> Impact,ImpactError;

//
//   Now we start a work.
//
    const DataVector<Trk::Track>*    newTrkCol = trackTES;

    if(m_BeamConstraint){
      if(m_BeamCondSvcExist){
         Amg::Vector3D beam=m_iBeamCondSvc->beamPos();
         m_BeamCnst[0]=beam.x();
         m_BeamCnst[1]=beam.y();
         m_BeamCnst[2]=beam.z();
         m_BeamCnstWid[0]=m_iBeamCondSvc->beamSigma(0);
         m_BeamCnstWid[1]=m_iBeamCondSvc->beamSigma(1);
         if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "BeamSpot from SVC="<<m_BeamCnst[0]<<", "<<m_BeamCnst[1]<<
	               ", "<<m_BeamCnst[2]<<" wid="<<m_BeamCnstWid[0]<<", "<<m_BeamCnstWid[1]<<endreq;
         if(msgLvl(MSG::DEBUG) && m_BeamCnst[2]!=0.)msg(MSG::DEBUG) << "BeamSpot Z must be 0 in finder!!! Make Z=0."<<endreq;
         m_BeamCnst[2]=0.;
      }
    }else{
       Amg::Vector3D approx_beam=findIniXY(newTrkCol);
       m_BeamCnst[0]=approx_beam.x();
       m_BeamCnst[1]=approx_beam.y();
       m_BeamCnst[2]=0.;
       if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Approx. BeamSpot="<<m_BeamCnst[0]<<", "<<m_BeamCnst[1]<<endreq;
    }
    Trk::Vertex selectionVertex(m_BeamCnst);

       if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Trk::Track number=" <<newTrkCol->size()<< endreq;
       DataVector<Trk::Track>::const_iterator    i_ntrk;
       for (i_ntrk = newTrkCol->begin(); i_ntrk < newTrkCol->end(); ++i_ntrk) {
          TrkQual   = (*i_ntrk)->fitQuality();
          m_mPer=GetPerigee( (*i_ntrk) ) ;
 	  if( m_mPer == NULL ){
            if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) <<" Error!!! No Perigee in TrackParameters"<< endreq;
	    continue;
	  } 
	  VectPerig = m_mPer->parameters(); 
          double CovTrkMtx11 = (*(m_mPer->covariance()))(0,0);
          double CovTrkMtx22 = (*(m_mPer->covariance()))(1,1);
	  if ( CovTrkMtx11 > m_A0TrkErrorCut*m_A0TrkErrorCut )  continue;
	  if ( CovTrkMtx22 > m_ZTrkErrorCut*m_ZTrkErrorCut )    continue;
          if( m_trkSelectorExist ) {
	     if(!m_trkSelector->decision(**i_ntrk,&selectionVertex)) continue;
	  }else{
//---------------------------------------------------------
            long int PixelHits = 3, SctHits = 9, SharedHits = 0, BLayHits = 1;
	    if(m_SummaryToolExist) {
              const Trk::TrackSummary* testSum = m_sumSvc->createSummary(*(*i_ntrk));
              PixelHits = (long int) testSum->get(Trk::numberOfPixelHits);
              SctHits   = (long int) testSum->get(Trk::numberOfSCTHits);
              BLayHits  = (long int) testSum->get(Trk::numberOfBLayerHits);
              SharedHits= (long int) testSum->get(Trk::numberOfPixelSharedHits);
	      if(PixelHits<0)PixelHits=0;
              if(SctHits<0)SctHits=0;
              if(BLayHits<0)BLayHits=0; 
              delete testSum;
            }
//---------------------------------------------------------	
            //double ImpactSignif = m_fitSvc->VKalGetImpact((*i_ntrk), m_BeamCnst, 1, Impact, ImpactError);  //VK ImpactSignif not needed
            m_fitSvc->VKalGetImpact((*i_ntrk), m_BeamCnst, 1, Impact, ImpactError);
	    double ImpactA0=VectPerig[0]; //double ImpactZ=VectPerig[1];   // Temporary
	    ImpactA0=Impact[0];           //ImpactZ=Impact[1];   

            StatusCode sc = CutTrk( VectPerig[4] , VectPerig[3] , ImpactA0 , 
		          TrkQual->chiSquared() / TrkQual->numberDoF(),
		          PixelHits, SctHits, SharedHits, BLayHits);
	    if( sc == StatusCode::FAILURE ) continue;
          }
	  SelectedTrkTracks.push_back(*i_ntrk);
       }
       if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << " Found="<<SelectedTrkTracks.size()<<" tracks" <<endreq;
       if(SelectedTrkTracks.size()<2){
	 SelectedTrkTracks.clear();
         for (i_ntrk = newTrkCol->begin(); i_ntrk < newTrkCol->end(); ++i_ntrk) {
           TrkQual   = (*i_ntrk)->fitQuality();
           m_mPer=GetPerigee( (*i_ntrk) ) ; if( m_mPer == NULL ) continue;
 	   VectPerig = m_mPer->parameters(); 
           double CovTrkMtx11 = (*(m_mPer->covariance()))(0,0);
           double CovTrkMtx22 = (*(m_mPer->covariance()))(1,1);
	   if ( CovTrkMtx11 > 4.*m_A0TrkErrorCut*m_A0TrkErrorCut )  continue;
	   if ( CovTrkMtx22 > 4.*m_ZTrkErrorCut*m_ZTrkErrorCut )    continue;
//           if( m_trkSelectorExist ) {
//	      if(!m_trkSelector->decision(**i_ntrk,&selectionVertex)) continue;
//	   }else{
//---------------------------------------------------------
             long int PixelHits = 3, SctHits = 9, SharedHits = 0, BLayHits = 1;
	     if(m_SummaryToolExist) {
               const Trk::TrackSummary* testSum = m_sumSvc->createSummary(*(*i_ntrk));
               PixelHits = (long int) testSum->get(Trk::numberOfPixelHits);
               SctHits   = (long int) testSum->get(Trk::numberOfSCTHits);
               BLayHits  = (long int) testSum->get(Trk::numberOfBLayerHits);
               SharedHits= (long int) testSum->get(Trk::numberOfPixelSharedHits);
	       if(PixelHits<0)PixelHits=0;
               if(SctHits<0)SctHits=0;
               if(BLayHits<0)BLayHits=0; 
               delete testSum;
             }
//---------------------------------------------------------	
	     if(SharedHits>0) SharedHits--; BLayHits++; PixelHits++;
             m_fitSvc->VKalGetImpact((*i_ntrk), m_BeamCnst, 1, Impact, ImpactError);
	     double ImpactA0=Impact[0]; 
             StatusCode sc = CutTrk( VectPerig[4] , VectPerig[3] , ImpactA0 , 
		          TrkQual->chiSquared() / TrkQual->numberDoF(),
		          PixelHits, SctHits, SharedHits, BLayHits);
	     if( sc == StatusCode::FAILURE ) continue;
//	   }
	   SelectedTrkTracks.push_back(*i_ntrk);
         }
         if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << " Found relaxed="<<SelectedTrkTracks.size()<<" tracks" <<endreq;
       }
                      /* And now primary vertex search itself*/
//
// Cleaning
//
        UniqList(SelectedTrkTracks);
                      /* And now primary vertex search itself*/

        NFoundVrt =  PVrtListFind( SelectedTrackParticles, SelectedTrkTracks,
                                   PrimVrtList,
				   ErrorMatrixPerVrt,
				   Chi2PerVrt,
		                   ControlVariablePerVrt,
				   PrtPerVrt, TrkPerVrt, TrkWgtPerVrt );

        for (int i=0; i<NFoundVrt; i++) NTrkPerVrt.push_back((int)TrkPerVrt[i].size()); 
     

//--------------------------------------------------------------------

    if( NFoundVrt == 0){
       if(msgLvl(MSG::ERROR))msg(MSG::ERROR) << "Primary vertex not found" <<endreq;
    }else{ 
       if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<"VKalVrtCpp Primary  Vertex=" << PrimVrtList[0].x()<<", "<<
             PrimVrtList[0].y()<<", "<<PrimVrtList[0].z()<< endreq;
    }
//------------------------------------------------------------------------------- 
//
//---- Save vertices 
//
    return  SaveResults( NFoundVrt, PrimVrtList,ErrorMatrixPerVrt, Chi2PerVrt,
                                         NTrkPerVrt,PrtPerVrt,TrkPerVrt,TrkWgtPerVrt,trackTES);
  }









//__________________________________________________________________________

  //VxContainer* InDetVKalPriVxFinderTool::findVertex(const Trk::TrackParticleBaseCollection* newPrtCol)
  std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> InDetVKalPriVxFinderTool::findVertex(const Trk::TrackParticleBaseCollection* newPrtCol)
  {
    //.............................................
    
    if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Primary vertex with InDetVKalPriVxFinderTool starts" << endreq;
//
//
    savedTrkFittedPerigees.clear();
    m_fittedTrkCov.clear();
//------------------------------------------------------------------------------------------
//  Creating the necessary vectors

    std::vector<const Trk::Track*>              SelectedTrkTracks;
    std::vector<const Trk::TrackParticleBase*>  SelectedTrackParticles;
    std::vector<Amg::Vector3D>                     PrimVrtList;
    std::vector< AmgSymMatrix(3) >                 ErrorMatrixPerVrt;
    std::vector<double>                         Chi2PerVrt;
    std::vector<double>                         ControlVariablePerVrt;
    std::vector< std::vector<const Trk::Track*> >              TrkPerVrt;
    std::vector< std::vector<const Trk::TrackParticleBase*> >  PrtPerVrt;
    std::vector< std::vector<double> >                         TrkWgtPerVrt;
    std::vector<int> NTrkPerVrt;
//
    int  NFoundVrt=0;
    SelectedTrkTracks.clear(); SelectedTrackParticles.clear(); NTrkPerVrt.clear();
    TrkPerVrt.clear(); PrtPerVrt.clear();

    const Trk::Perigee* m_mPer=NULL;
    AmgVector(5) VectPerig; VectPerig<<0.,0.,0.,0.,0.;
    const Trk::FitQuality*  TrkQual=0;
    std::vector<double> Impact,ImpactError;

    if(m_BeamConstraint){
      if(m_BeamCondSvcExist){
         Amg::Vector3D beam=m_iBeamCondSvc->beamPos();
         m_BeamCnst[0]=beam.x();
         m_BeamCnst[1]=beam.y();
         m_BeamCnst[2]=beam.z();
         m_BeamCnstWid[0]=m_iBeamCondSvc->beamSigma(0);
         m_BeamCnstWid[1]=m_iBeamCondSvc->beamSigma(1);
         if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "BeamSpot from SVC="<<m_BeamCnst[0]<<", "<<m_BeamCnst[1]<<
	               ", "<<m_BeamCnst[2]<<" wid="<<m_BeamCnstWid[0]<<", "<<m_BeamCnstWid[1]<<endreq;
      }
    }else{
       Amg::Vector3D approx_beam=findIniXY(newPrtCol);
       m_BeamCnst[0]=approx_beam.x();
       m_BeamCnst[1]=approx_beam.y();
       m_BeamCnst[2]=0.;
       if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Approx. BeamSpot="<<m_BeamCnst[0]<<", "<<m_BeamCnst[1]<<endreq;
    }
    Trk::Vertex selectionVertex(m_BeamCnst);

          if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Trk::TrackParticleBase number=" <<newPrtCol->size()<< endreq;
          Trk::TrackParticleBaseCollection::const_iterator i_nprt  = newPrtCol->begin();
          for (i_nprt = newPrtCol->begin(); i_nprt < newPrtCol->end(); ++i_nprt) {
              m_mPer=GetPerigee( (*i_nprt) ); if( m_mPer == NULL ){ continue; } 
              VectPerig = m_mPer->parameters(); 
              TrkQual   = (*i_nprt)->fitQuality();
              double CovTrkMtx11 = (*(m_mPer->covariance()))(0,0);
              double CovTrkMtx22 = (*(m_mPer->covariance()))(2,2);
//if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << " Error track matrix="<<CovTrkMtx11<<", "<<CovTrkMtx22<<endreq;
	      if ( CovTrkMtx11 > m_A0TrkErrorCut*m_A0TrkErrorCut )  continue;
	      if ( CovTrkMtx22 > m_ZTrkErrorCut*m_ZTrkErrorCut )    continue;
              if( m_trkSelectorExist ) {
	         if(!m_trkSelector->decision(**i_nprt,&selectionVertex)) continue;
	      }else{
//----------------------------------- Summary tools
                 long int PixelHits = 3, SctHits = 9, SharedHits = 0, BLayHits = 1;
                 const Trk::TrackSummary* testSum = (*i_nprt)->trackSummary();
                 PixelHits = (long int) testSum->get(Trk::numberOfPixelHits);
                 SctHits   = (long int) testSum->get(Trk::numberOfSCTHits);
                 BLayHits  = (long int) testSum->get(Trk::numberOfBLayerHits);
                 SharedHits= (long int) testSum->get(Trk::numberOfPixelSharedHits);
	         if(PixelHits<0)PixelHits=0;
                 if(SctHits<0)SctHits=0;
                 if(BLayHits<0)BLayHits=0; 
                 //double ImpactSignif = m_fitSvc->VKalGetImpact((*i_nprt), m_BeamCnst, 1, Impact, ImpactError); //VK ImpactSignif not needed
                 m_fitSvc->VKalGetImpact((*i_nprt), m_BeamCnst, 1, Impact, ImpactError);
	         double ImpactA0=VectPerig[0]; //double ImpactZ=VectPerig[1];   // Temporary
//if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << " Track impact="<<ImpactA0<<", "<<ImpactZ<<", "<<ImpactSignif<<endreq;
	         ImpactA0=Impact[0];           //ImpactZ=Impact[1];   

                 StatusCode sc = CutTrk( VectPerig[4] , VectPerig[3] , ImpactA0 ,
		              TrkQual->chiSquared() / TrkQual->numberDoF(),
		              PixelHits, SctHits, SharedHits, BLayHits);
	         if( sc == StatusCode::FAILURE ) continue;
	      }
	      SelectedTrackParticles.push_back(*i_nprt);
	  }

  if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << " Found="<<SelectedTrackParticles.size()<<" particles" <<endreq;
                      /* And now primary vertex search itself*/
          if(SelectedTrackParticles.size()<2){
	    SelectedTrackParticles.clear();
            for (i_nprt = newPrtCol->begin(); i_nprt < newPrtCol->end(); ++i_nprt) {
              m_mPer=GetPerigee( (*i_nprt) ); if( m_mPer == NULL ){ continue; } 
              VectPerig = m_mPer->parameters();  TrkQual   = (*i_nprt)->fitQuality();
              double CovTrkMtx11 = (*(m_mPer->covariance()))(1,1);
              double CovTrkMtx22 = (*(m_mPer->covariance()))(2,2);
	      if ( CovTrkMtx11 > 4.*m_A0TrkErrorCut*m_A0TrkErrorCut )  continue;
	      if ( CovTrkMtx22 > 4.*m_ZTrkErrorCut*m_ZTrkErrorCut )    continue;
//              if( m_trkSelectorExist ) {
//	         if(!m_trkSelector->decision(**i_nprt,&selectionVertex)) continue;
//	      }else{
                 long int PixelHits = 3, SctHits = 9, SharedHits = 0, BLayHits = 1;
                 const Trk::TrackSummary* testSum = (*i_nprt)->trackSummary();
                 PixelHits = (long int) testSum->get(Trk::numberOfPixelHits);
                 SctHits   = (long int) testSum->get(Trk::numberOfSCTHits);
                 BLayHits  = (long int) testSum->get(Trk::numberOfBLayerHits);
                 SharedHits= (long int) testSum->get(Trk::numberOfPixelSharedHits);
	         if(PixelHits<0)PixelHits=0;
                 if(SctHits<0)SctHits=0;
                 if(BLayHits<0)BLayHits=0; 
	         if(SharedHits>0) SharedHits--; BLayHits++; PixelHits++;
                 m_fitSvc->VKalGetImpact((*i_nprt), m_BeamCnst, 1, Impact, ImpactError);
	         double ImpactA0=Impact[0];  
                 StatusCode sc = CutTrk( VectPerig[4] , VectPerig[3] , ImpactA0 ,
		              TrkQual->chiSquared() / TrkQual->numberDoF(),
		              PixelHits, SctHits, SharedHits, BLayHits);
	         if( sc == StatusCode::FAILURE ) continue;
//	      }
	      SelectedTrackParticles.push_back(*i_nprt);
	    }
 if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< " Found relaxed="<<SelectedTrackParticles.size()<<" particles" <<endreq;
                      /* And now primary vertex search itself*/
	  }
//--- Cleaning
// 
          UniqList(SelectedTrackParticles);
                      /* And now primary vertex search itself*/

          NFoundVrt =  PVrtListFind( SelectedTrackParticles, SelectedTrkTracks,
                                     PrimVrtList,
	  	 		     ErrorMatrixPerVrt,
				     Chi2PerVrt,
		                     ControlVariablePerVrt,
				     PrtPerVrt, TrkPerVrt, TrkWgtPerVrt );
	  for (int i=0; i<NFoundVrt; i++) NTrkPerVrt.push_back((int) PrtPerVrt[i].size()); 
       

//--------------------------------------------------------------------

    if( NFoundVrt == 0){
      if(msgLvl(MSG::ERROR))msg(MSG::ERROR) << "Primary vertex not found" <<endreq;
    }else{ 
      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<"Found Primary  Vertex=" << PrimVrtList[0].x()<<", "<<
             PrimVrtList[0].y()<<", "<<PrimVrtList[0].z()<< endreq;
    }
//------------------------------------------------------------------------------- 
//
//---- Save vertices 
//
   return  SaveResults( NFoundVrt, PrimVrtList,ErrorMatrixPerVrt, Chi2PerVrt,
                                         NTrkPerVrt,PrtPerVrt,TrkPerVrt,TrkWgtPerVrt,0,newPrtCol);
  }



  std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>  InDetVKalPriVxFinderTool::SaveResults( int NFoundVrt,
	            std::vector< Amg::Vector3D >                & PrimVrtList,
                    std::vector< AmgSymMatrix(3) >              & ErrorMatrixPerVrt,
		    std::vector<double>                         & Chi2PerVrt,
                    std::vector<int>                            & NTrkPerVrt,
		    std::vector< std::vector<const Trk::TrackParticleBase*> > & PrtPerVrt,
		    std::vector< std::vector<const Trk::Track*> >             & TrkPerVrt,
		    std::vector< std::vector<double> >                        & TrkWgtPerVrt,
                    const TrackCollection* trackTES,
		    const Trk::TrackParticleBaseCollection* partTES)
   {
     xAOD::VertexContainer  *  vrtCont    = new xAOD::VertexContainer;
     xAOD::VertexAuxContainer *vrtAuxCont = new xAOD::VertexAuxContainer;
     vrtCont->setStore(vrtAuxCont);
     int i,ii;
     for( i=0; i< NFoundVrt; i++){ 
       if(PrimVrtList.size()>1 && NTrkPerVrt[i]==2){
          if( Chi2PerVrt[i]>12. ) continue;                  //Drop 2tr vertex with low probability
          if( i==0 && NTrkPerVrt[1]>3 ) continue;            //Drop 2tr first vertex if second is better
       }


       xAOD::Vertex * tmpVertex=new xAOD::Vertex();
       tmpVertex->setPosition(PrimVrtList[i]);
       tmpVertex->setCovariancePosition(ErrorMatrixPerVrt[i]);
       tmpVertex->setFitQuality(Chi2PerVrt[i], (float)(NTrkPerVrt[i]*2.-3.));


       std::vector<Trk::VxTrackAtVertex> & tmpVTAV=tmpVertex->vxTrackAtVertex();    tmpVTAV.clear();
       for(ii=0; ii<NTrkPerVrt[i]; ii++) {
         AmgSymMatrix(5) * tmpCovMatr=new AmgSymMatrix(5)(m_fittedTrkCov.at(i).at(ii));
         Trk::Perigee * tmpMeasPer = new Trk::Perigee( 0.,0.,
		           savedTrkFittedPerigees[i][ii][0] ,        /* Phi   */
		           savedTrkFittedPerigees[i][ii][1] ,        /* Theta */
		           savedTrkFittedPerigees[i][ii][2] ,        /* 1/p   */
	                   Trk::PerigeeSurface(PrimVrtList[i]),
		           tmpCovMatr );
         Trk::VxTrackAtVertex * tmpPointer = new Trk::VxTrackAtVertex( 1., tmpMeasPer ) ;
	 if( (int)TrkPerVrt.size() > 0 ){ 
             ElementLink<TrackCollection>  TEL;   TEL.setElement(TrkPerVrt[i][ii]);
             Trk::LinkToTrack *ITL = new Trk::LinkToTrack(TEL);    //pointer to initial Trk
             ITL->setStorableObject( *trackTES );
             tmpPointer->setOrigTrack(ITL);                       //pointer to initial Trk
         }else{
	      ElementLink<Trk::TrackParticleBaseCollection> TEL; TEL.setElement(PrtPerVrt[i][ii]);
              Trk::LinkToTrackParticleBase *ITL = new Trk::LinkToTrackParticleBase(TEL);    //pointer to initial Prt
              ITL->setStorableObject( *partTES );
              tmpPointer->setOrigTrack(ITL);                                   //pointer to initial Prt
         }
         tmpPointer->setWeight(TrkWgtPerVrt[i][ii]);
         tmpVTAV.push_back( *tmpPointer );
       }
//
//-Save to container
//
       vrtCont->push_back(tmpVertex);
     }
//
//-Defense against 0 found vertices 
//
     if( NFoundVrt == 0 ){
       xAOD::Vertex * tmpVertex=new xAOD::Vertex();
       tmpVertex->setPosition(Amg::Vector3D(0.,0.,0.));
       vrtCont->push_back(tmpVertex);
     }
//
//-Remove refitted track parameters from HEAP
//
     for( i=0; i< NFoundVrt; i++){ 
       double** pntTracks=savedTrkFittedPerigees[i];
       removeWorkArr2(pntTracks,(long int)NTrkPerVrt[i],3);
     }
     savedTrkFittedPerigees.clear();
     m_fittedTrkCov.clear();
     Trk::TrkVKalVrtFitter* vkalPnt = dynamic_cast<Trk::TrkVKalVrtFitter*>(&(*m_fitSvc));
     if(vkalPnt)vkalPnt->clearMemory();

     return std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> (vrtCont,vrtAuxCont);
   }




/*   VxContainer* InDetVKalPriVxFinderTool::SaveResults( int NFoundVrt,
	            std::vector< Amg::Vector3D >                & PrimVrtList,
                    std::vector< AmgSymMatrix(3) >              & ErrorMatrixPerVrt,
		    std::vector<double>                         & Chi2PerVrt,
                    std::vector<int>                            & NTrkPerVrt,
		    std::vector< std::vector<const Trk::TrackParticleBase*> > & PrtPerVrt,
		    std::vector< std::vector<const Trk::Track*> >             & TrkPerVrt,
		    std::vector< std::vector<double> >                        & TrkWgtPerVrt,
                    const TrackCollection* trackTES,
		    const Trk::TrackParticleBaseCollection* partTES)
  {
    int i,ii;
    VxContainer*             nVrtVx =new VxContainer;
    for( i=0; i< NFoundVrt; i++){ 
       if(PrimVrtList.size()>1 && NTrkPerVrt[i]==2){
          if( Chi2PerVrt[i]>12. ) continue;                  //Drop 2tr vertex with low probability
          if( i==0 && NTrkPerVrt[1]>3 ) continue;            //Drop 2tr first vertex if second is better
       }

       Trk::RecVertex * tmpRecV = new  Trk::RecVertex( PrimVrtList[i],
                                       Amg::MatrixX(ErrorMatrixPerVrt[i]),
	                               NTrkPerVrt[i]*2-1, Chi2PerVrt[i]);
// Set up track list //
       std::vector<Trk::VxTrackAtVertex*> * tmpVTAV = new std::vector<Trk::VxTrackAtVertex*>();
       for(ii=0; ii<NTrkPerVrt[i]; ii++) {
               Trk::Perigee * tmpMeasPer; 
               AmgSymMatrix(5) * tmpCovMatr=new AmgSymMatrix(5)(m_fittedTrkCov.at(i).at(ii));
	       tmpMeasPer  =  new Trk::Perigee( 0.,0.,
		     savedTrkFittedPerigees[i][ii][0] ,        // Phi   //
		     savedTrkFittedPerigees[i][ii][1] ,        // Theta //
		     savedTrkFittedPerigees[i][ii][2] ,        // 1/p   //
	             Trk::PerigeeSurface(PrimVrtList[i]),
		     tmpCovMatr );

               Trk::VxTrackAtVertex * tmpPointer = new Trk::VxTrackAtVertex( 1., tmpMeasPer ) ;
	       if( (int)TrkPerVrt.size() > 0 ){ 
                   ElementLink<TrackCollection>  TEL;   TEL.setElement(TrkPerVrt[i][ii]);
                   Trk::LinkToTrack *ITL = new Trk::LinkToTrack(TEL);    //pointer to initial Trk
                   ITL->setStorableObject( *trackTES );
                   tmpPointer->setOrigTrack(ITL);                       //pointer to initial Trk
	       }else{
		   ElementLink<Trk::TrackParticleBaseCollection> TEL; TEL.setElement(PrtPerVrt[i][ii]);
                   Trk::LinkToTrackParticleBase *ITL = new Trk::LinkToTrackParticleBase(TEL);    //pointer to initial Prt
                   ITL->setStorableObject( *partTES );
                   tmpPointer->setOrigTrack(ITL);                                   //pointer to initial Prt
               }
	       tmpPointer->setWeight(TrkWgtPerVrt[i][ii]);
               tmpVTAV->push_back( tmpPointer );
       }
       Trk::VxCandidate* tmpVx =new Trk::VxCandidate(*tmpRecV,*tmpVTAV);
       delete tmpVTAV; delete tmpRecV;
//
//  Save to container
//
       nVrtVx->push_back(tmpVx);
    }
//
// Defense against 0 found vertices 
//
    if( NFoundVrt == 0 ){
       Trk::RecVertex * tmpRecV = new Trk::RecVertex(Amg::Vector3D(0.,0.,0.));
       std::vector<Trk::VxTrackAtVertex*> * tmpVTAV = new std::vector<Trk::VxTrackAtVertex*>();
       Trk::VxCandidate* tmpVx =new Trk::VxCandidate(*tmpRecV,*tmpVTAV);
       nVrtVx->push_back(tmpVx);
       delete tmpVTAV; delete tmpRecV;
    }
//
//  Remove refitted track parameters from HEAP
//
    for( i=0; i< NFoundVrt; i++){ 
      double** pntTracks=savedTrkFittedPerigees[i];
      removeWorkArr2(pntTracks,(long int)NTrkPerVrt[i],3);
    }
    savedTrkFittedPerigees.clear();
    m_fittedTrkCov.clear();
    Trk::TrkVKalVrtFitter* vkalPnt = dynamic_cast<Trk::TrkVKalVrtFitter*>(&(*m_fitSvc));
    if(vkalPnt)vkalPnt->clearMemory();
//----------------------------------------------------------------------------------------------------
//
// Additional stuff
//
    if ( nVrtVx->size() >= 1 )
    {
      Trk::VxCandidate * primaryVtx = nVrtVx->front();
      primaryVtx->setVertexType(Trk::PriVtx);
      Trk::VxCandidate * dummyVxCandidate = new Trk::VxCandidate ( primaryVtx->recVertex(),
                                                                   std::vector<Trk::VxTrackAtVertex*>());
      dummyVxCandidate->setVertexType(Trk::NoVtx);
      nVrtVx->push_back ( dummyVxCandidate );
    }
    //---- if no vertex is there let dummy be at beam spot
    else if ( nVrtVx->size() == 0 )
    {
      Trk::VxCandidate * dummyVxCandidate = new Trk::VxCandidate ( Trk::RecVertex( Amg::Vector3D(0.,0.,0.)), 
                                                                   std::vector<Trk::VxTrackAtVertex*>());
      dummyVxCandidate->setVertexType(Trk::NoVtx);
      nVrtVx->push_back ( dummyVxCandidate );
    }

    // loop over the pile up to set it as pile up (EXCLUDE first and last vertex: loop from 1 to size-1)
    for ( int i = 1 ; i < (int)nVrtVx->size()-1 ; i++)
    {
      (*nVrtVx)[i]->setVertexType(Trk::PileUp);
    }

 //    
    return nVrtVx;
  }
*/

 
} // end of namespace bracket
