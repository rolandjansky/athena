/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "InDetVKalPriVxFinderTool/InDetVKalPriVxFinderTool.h"
//-------------------------------------------------
// Other stuff

//
//----
#include  "TrkTrackLink/ITrackLink.h"
#include  "TrkTrack/LinkToTrack.h"
#include  "TrkParticleBase/LinkToTrackParticleBase.h"
#include  "TrkTrackSummary/TrackSummary.h"
#include  "VxVertex/VxTrackAtVertex.h"
#include  "TrkEventPrimitives/FitQuality.h"
#include  "TrkToolInterfaces/ITrackSummaryTool.h"
#include  "TrkToolInterfaces/ITrackSelectorTool.h"
#include  "xAODTracking/VertexContainer.h"
#include  "xAODTracking/Vertex.h"
#include  "xAODTracking/TrackParticle.h"
//----



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
 
    m_NPVertexMax(100)

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
        declareProperty("TrackSelectorTool",   m_trkSelector);


  }

  InDetVKalPriVxFinderTool::~InDetVKalPriVxFinderTool()
  {    //MsgStream log( msgSvc(), name() ) ;
       //m_sc.isSuccess();
       if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "InDetVKalPriVxFinderTool destructor called" << endmsg;
  }



//__________________________________________________________________________
  StatusCode InDetVKalPriVxFinderTool::initialize()
  { 
    if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<"InDetVKalVrtPriVxTool initialize()" << endmsg;
    m_SummaryToolExist = 0;


    if (m_fitSvc.retrieve().isFailure()) {
        if(msgLvl(MSG::ERROR))msg(MSG::ERROR) << "No Trk::TrkVKalVrtFitter for InDetVKalPriVxFinderTool !" << endmsg;
    }else{
        if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Trk::TrkVKalVrtFitter found" << endmsg;
    }
//
//TrackSummaryTool
//
    if (m_sumSvc.retrieve().isFailure()) {
        if(msgLvl(MSG::ERROR))msg(MSG::ERROR) << "Could not find TrackSummaryTool" << endmsg;
    } else { m_SummaryToolExist = 1;}
//
//  Data for beam spot constraint
//

    ATH_CHECK(m_beamSpotKey.initialize(m_BeamConstraint));

//
//TrackSelectorTool
//
    if (m_trkSelector.name() == "DefaultTrackSelection" ){
       m_trkSelectorExist=0;
       if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Use internal track selection - see  default/jobO cuts" << endmsg;
    }else{
       if (m_trkSelector.retrieve().isFailure()) {
          if(msgLvl(MSG::ERROR)){ msg(MSG::ERROR) << "Could not find TrackSelectorTool" << endmsg;
                                  msg(MSG::ERROR) << "Use internal track selection - see  default/jobO cuts" << endmsg;}
          m_trkSelectorExist=0;
       }else{ 
          if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Finder uses TrackSelectorTool" << endmsg;
          m_trkSelectorExist=1; 
       }
    }

    return StatusCode::SUCCESS; 
  }



//__________________________________________________________________________


  StatusCode InDetVKalPriVxFinderTool::finalize()
  {
    if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) <<"InDetVKalPriVxFinderTool finalize()" << endmsg;
    return StatusCode::SUCCESS; 
  }

  std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
  InDetVKalPriVxFinderTool::findVertex (
    const xAOD::TrackParticleContainer* trackTES) const
  {
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "N=" << trackTES->size()
                      << " xAOD::TrackParticles found" << endmsg;
      msg(MSG::DEBUG)
        << "No InDetVKalPriVxFinderTool implementation for xAOD::TrackParticle"
        << endmsg;
    }
    return std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>(0, 0);
   }


//__________________________________________________________________________
   std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
   InDetVKalPriVxFinderTool::findVertex(const TrackCollection* trackTES) const
   {
     //.............................................

     if (msgLvl(MSG::DEBUG))
       msg(MSG::DEBUG) << "Primary vertex with InDetVKalPriVxFinderTool starts"
                       << endmsg;
     //
     //
     Cache cache{};
     cache.m_BeamCnst[0] = m_BeamPositionX;
     cache.m_BeamCnst[1] = m_BeamPositionY;
     cache.m_BeamCnst[2] = 0.;
     cache.m_BeamCnstWid.resize(3);
     cache.m_BeamCnstWid[0] = 0.015;
     cache.m_BeamCnstWid[1] = 0.015;
     cache.m_BeamCnstWid[2] = 56.;
     cache.m_savedTrkFittedPerigees.clear();
     cache.m_fittedTrkCov.clear();
     //------------------------------------------------------------------------------------------
     //  Creating the necessary vectors

     std::vector<const Trk::Track*> SelectedTrkTracks;
     std::vector<const Trk::TrackParticleBase*> SelectedTrackParticles;
     std::vector<Amg::Vector3D> PrimVrtList;
     std::vector<AmgSymMatrix(3)> ErrorMatrixPerVrt;
     std::vector<double> Chi2PerVrt;
     std::vector<double> ControlVariablePerVrt;
     std::vector<std::vector<const Trk::Track*>> TrkPerVrt;
     std::vector<std::vector<const Trk::TrackParticleBase*>> PrtPerVrt;
     std::vector<std::vector<double>> TrkWgtPerVrt;
     std::vector<int> NTrkPerVrt;
     //
     int NFoundVrt = 0;
     SelectedTrkTracks.clear();
     SelectedTrackParticles.clear();
     NTrkPerVrt.clear();
     TrkPerVrt.clear();
     PrtPerVrt.clear();

     const Trk::Perigee* mPer = NULL;
     AmgVector(5) VectPerig;
     VectPerig << 0., 0., 0., 0., 0.;
     const Trk::FitQuality* TrkQual = 0;
     std::vector<double> Impact, ImpactError;

     //
     //   Now we start a work.
     //
     const DataVector<Trk::Track>* newTrkCol = trackTES;

     if (m_BeamConstraint) {
       SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle{ m_beamSpotKey };
       if (beamSpotHandle.isValid()) {
         const Amg::Vector3D &beam=beamSpotHandle->beamPos();
         cache.m_BeamCnst[0]=beam.x();
         cache.m_BeamCnst[1]=beam.y();
         cache.m_BeamCnst[2]=beam.z();
         cache.m_BeamCnstWid[0]=beamSpotHandle->beamSigma(0);
         cache.m_BeamCnstWid[1]=beamSpotHandle->beamSigma(1);
         if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "BeamSpot from SVC="<<cache.m_BeamCnst[0]<<", "<<cache.m_BeamCnst[1]<<
	               ", "<<cache.m_BeamCnst[2]<<" wid="<<cache.m_BeamCnstWid[0]<<", "<<cache.m_BeamCnstWid[1]<<endmsg;
         if(msgLvl(MSG::DEBUG) && cache.m_BeamCnst[2]!=0.)msg(MSG::DEBUG) << "BeamSpot Z must be 0 in finder!!! Make Z=0."<<endmsg;
         cache.m_BeamCnst[2]=0.;
      }
    }else{
       Amg::Vector3D approx_beam=findIniXY(newTrkCol);
       cache.m_BeamCnst[0]=approx_beam.x();
       cache.m_BeamCnst[1]=approx_beam.y();
       cache.m_BeamCnst[2]=0.;
       if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Approx. BeamSpot="<<cache.m_BeamCnst[0]<<", "<<cache.m_BeamCnst[1]<<endmsg;
    }
    Trk::Vertex selectionVertex(cache.m_BeamCnst);

       if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Trk::Track number=" <<newTrkCol->size()<< endmsg;
       DataVector<Trk::Track>::const_iterator    i_ntrk;
       for (i_ntrk = newTrkCol->begin(); i_ntrk < newTrkCol->end(); ++i_ntrk) {
          TrkQual   = (*i_ntrk)->fitQuality();
          mPer=GetPerigee( (*i_ntrk) ) ;
 	  if( mPer == NULL ){
            if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) <<" Error!!! No Perigee in TrackParameters"<< endmsg;
	    continue;
	  } 
	  VectPerig = mPer->parameters(); 
          double CovTrkMtx11 = (*(mPer->covariance()))(0,0);
          double CovTrkMtx22 = (*(mPer->covariance()))(1,1);
	  if ( CovTrkMtx11 > m_A0TrkErrorCut*m_A0TrkErrorCut )  continue;
	  if ( CovTrkMtx22 > m_ZTrkErrorCut*m_ZTrkErrorCut )    continue;
          if( m_trkSelectorExist ) {
	     if(!m_trkSelector->decision(**i_ntrk,&selectionVertex)) continue;
	  }else{
//---------------------------------------------------------
            long int PixelHits = 3, SctHits = 9, SharedHits = 0, BLayHits = 1;
	    if(m_SummaryToolExist) {
              std::unique_ptr<const Trk::TrackSummary> testSum = m_sumSvc->summary(*(*i_ntrk));
              PixelHits = (long int) testSum->get(Trk::numberOfPixelHits);
              SctHits   = (long int) testSum->get(Trk::numberOfSCTHits);
              BLayHits  = (long int) testSum->get(Trk::numberOfInnermostPixelLayerHits);
              SharedHits= (long int) testSum->get(Trk::numberOfPixelSharedHits);
	      if(PixelHits<0)PixelHits=0;
              if(SctHits<0)SctHits=0;
              if(BLayHits<0)BLayHits=0; 
            }
//---------------------------------------------------------	
            //double ImpactSignif = m_fitSvc->VKalGetImpact((*i_ntrk), cache.m_BeamCnst, 1, Impact, ImpactError);  //VK ImpactSignif not needed
            m_fitSvc->VKalGetImpact((*i_ntrk), cache.m_BeamCnst, 1, Impact, ImpactError);
	    double ImpactA0=VectPerig[0]; //double ImpactZ=VectPerig[1];   // Temporary
	    ImpactA0=Impact[0];           //ImpactZ=Impact[1];   

            StatusCode sc = CutTrk( VectPerig[4] , VectPerig[3] , ImpactA0 , 
		          TrkQual->chiSquared() / TrkQual->numberDoF(),
		          PixelHits, SctHits, SharedHits, BLayHits);
	    if( sc == StatusCode::FAILURE ) continue;
          }
	  SelectedTrkTracks.push_back(*i_ntrk);
       }
       if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << " Found="<<SelectedTrkTracks.size()<<" tracks" <<endmsg;
       if(SelectedTrkTracks.size()<2){
	 SelectedTrkTracks.clear();
         for (i_ntrk = newTrkCol->begin(); i_ntrk < newTrkCol->end(); ++i_ntrk) {
           TrkQual   = (*i_ntrk)->fitQuality();
           mPer=GetPerigee( (*i_ntrk) ) ; if( mPer == NULL ) continue;
 	   VectPerig = mPer->parameters(); 
           double CovTrkMtx11 = (*(mPer->covariance()))(0,0);
           double CovTrkMtx22 = (*(mPer->covariance()))(1,1);
	   if ( CovTrkMtx11 > 4.*m_A0TrkErrorCut*m_A0TrkErrorCut )  continue;
	   if ( CovTrkMtx22 > 4.*m_ZTrkErrorCut*m_ZTrkErrorCut )    continue;

//---------------------------------------------------------
             long int PixelHits = 3, SctHits = 9, SharedHits = 0, BLayHits = 1;
	     if(m_SummaryToolExist) {
               std::unique_ptr<const Trk::TrackSummary> testSum = m_sumSvc->summary(*(*i_ntrk));
               PixelHits = (long int) testSum->get(Trk::numberOfPixelHits);
               SctHits   = (long int) testSum->get(Trk::numberOfSCTHits);
               BLayHits  = (long int) testSum->get(Trk::numberOfInnermostPixelLayerHits);
               SharedHits= (long int) testSum->get(Trk::numberOfPixelSharedHits);
	       if(PixelHits<0)PixelHits=0;
               if(SctHits<0)SctHits=0;
               if(BLayHits<0)BLayHits=0; 
             }
//---------------------------------------------------------	
	     if(SharedHits>0) SharedHits--;
             BLayHits++;
             PixelHits++;
             m_fitSvc->VKalGetImpact((*i_ntrk), cache.m_BeamCnst, 1, Impact, ImpactError);
	     double ImpactA0=Impact[0]; 
             StatusCode sc = CutTrk( VectPerig[4] , VectPerig[3] , ImpactA0 , 
		          TrkQual->chiSquared() / TrkQual->numberDoF(),
		          PixelHits, SctHits, SharedHits, BLayHits);
	     if( sc == StatusCode::FAILURE ) continue;
//	   }
	   SelectedTrkTracks.push_back(*i_ntrk);
         }
         if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << " Found relaxed="<<SelectedTrkTracks.size()<<" tracks" <<endmsg;
       }
                      /* And now primary vertex search itself*/
//
// Cleaning
//
        UniqList(SelectedTrkTracks);
                      /* And now primary vertex search itself*/

        NFoundVrt = PVrtListFind(cache,
                                 SelectedTrackParticles,
                                 SelectedTrkTracks,
                                 PrimVrtList,
                                 ErrorMatrixPerVrt,
                                 Chi2PerVrt,
                                 ControlVariablePerVrt,
                                 PrtPerVrt,
                                 TrkPerVrt,
                                 TrkWgtPerVrt);

        for (int i=0; i<NFoundVrt; i++) NTrkPerVrt.push_back((int)TrkPerVrt[i].size()); 
     

//--------------------------------------------------------------------

    if( NFoundVrt == 0){
       if(msgLvl(MSG::ERROR))msg(MSG::ERROR) << "Primary vertex not found" <<endmsg;
    }else{ 
       if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<"VKalVrtCpp Primary  Vertex=" << PrimVrtList[0].x()<<", "<<
             PrimVrtList[0].y()<<", "<<PrimVrtList[0].z()<< endmsg;
    }
//------------------------------------------------------------------------------- 
//
//---- Save vertices 
//
    return SaveResults(cache,
                       NFoundVrt,
                       PrimVrtList,
                       ErrorMatrixPerVrt,
                       Chi2PerVrt,
                       NTrkPerVrt,
                       PrtPerVrt,
                       TrkPerVrt,
                       TrkWgtPerVrt,
                       trackTES);
  }


//__________________________________________________________________________

  std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
  InDetVKalPriVxFinderTool::findVertex(
    const Trk::TrackParticleBaseCollection* newPrtCol) const
  {
    //.............................................
    
    if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Primary vertex with InDetVKalPriVxFinderTool starts" << endmsg;
//
//
     Cache cache{};
     cache.m_BeamCnst[0] = m_BeamPositionX;
     cache.m_BeamCnst[1] = m_BeamPositionY;
     cache.m_BeamCnst[2] = 0.;
     cache.m_BeamCnstWid.resize(3);
     cache.m_BeamCnstWid[0] = 0.015;
     cache.m_BeamCnstWid[1] = 0.015;
     cache.m_BeamCnstWid[2] = 56.;
     cache.m_savedTrkFittedPerigees.clear();
     cache.m_fittedTrkCov.clear();
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

    const Trk::Perigee* mPer=NULL;
    AmgVector(5) VectPerig; VectPerig<<0.,0.,0.,0.,0.;
    const Trk::FitQuality*  TrkQual=0;
    std::vector<double> Impact,ImpactError;

    if(m_BeamConstraint){
      SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey };
      if(beamSpotHandle.isValid()){
         const Amg::Vector3D &beam=beamSpotHandle->beamPos();
         cache.m_BeamCnst[0]=beam.x();
         cache.m_BeamCnst[1]=beam.y();
         cache.m_BeamCnst[2]=beam.z();
         cache.m_BeamCnstWid[0]=beamSpotHandle->beamSigma(0);
         cache.m_BeamCnstWid[1]=beamSpotHandle->beamSigma(1);
         if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "BeamSpot from SVC="<<cache.m_BeamCnst[0]<<", "<<cache.m_BeamCnst[1]<<
	               ", "<<cache.m_BeamCnst[2]<<" wid="<<cache.m_BeamCnstWid[0]<<", "<<cache.m_BeamCnstWid[1]<<endmsg;
      }
    }else{
       Amg::Vector3D approx_beam=findIniXY(newPrtCol);
       cache.m_BeamCnst[0]=approx_beam.x();
       cache.m_BeamCnst[1]=approx_beam.y();
       cache.m_BeamCnst[2]=0.;
       if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Approx. BeamSpot="<<cache.m_BeamCnst[0]<<", "<<cache.m_BeamCnst[1]<<endmsg;
    }
    Trk::Vertex selectionVertex(cache.m_BeamCnst);

          if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Trk::TrackParticleBase number=" <<newPrtCol->size()<< endmsg;
          Trk::TrackParticleBaseCollection::const_iterator i_nprt  = newPrtCol->begin();
          for (i_nprt = newPrtCol->begin(); i_nprt < newPrtCol->end(); ++i_nprt) {
              mPer=GetPerigee( (*i_nprt) ); if( mPer == NULL ){ continue; } 
              VectPerig = mPer->parameters(); 
              TrkQual   = (*i_nprt)->fitQuality();
              double CovTrkMtx11 = (*(mPer->covariance()))(0,0);
              double CovTrkMtx22 = (*(mPer->covariance()))(2,2);
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
                 BLayHits  = (long int) testSum->get(Trk::numberOfInnermostPixelLayerHits);
                 SharedHits= (long int) testSum->get(Trk::numberOfPixelSharedHits);
	         if(PixelHits<0)PixelHits=0;
                 if(SctHits<0)SctHits=0;
                 if(BLayHits<0)BLayHits=0; 
                 m_fitSvc->VKalGetImpact((*i_nprt), cache.m_BeamCnst, 1, Impact, ImpactError);
	         double ImpactA0=VectPerig[0]; //double ImpactZ=VectPerig[1];   // Temporary
	         ImpactA0=Impact[0];           //ImpactZ=Impact[1];   

                 StatusCode sc = CutTrk( VectPerig[4] , VectPerig[3] , ImpactA0 ,
		              TrkQual->chiSquared() / TrkQual->numberDoF(),
		              PixelHits, SctHits, SharedHits, BLayHits);
	         if( sc == StatusCode::FAILURE ) continue;
	      }
	      SelectedTrackParticles.push_back(*i_nprt);
	  }

  if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << " Found="<<SelectedTrackParticles.size()<<" particles" <<endmsg;
                      /* And now primary vertex search itself*/
          if(SelectedTrackParticles.size()<2){
	    SelectedTrackParticles.clear();
            for (i_nprt = newPrtCol->begin(); i_nprt < newPrtCol->end(); ++i_nprt) {
              mPer=GetPerigee( (*i_nprt) ); if( mPer == NULL ){ continue; } 
              VectPerig = mPer->parameters();  TrkQual   = (*i_nprt)->fitQuality();
              double CovTrkMtx11 = (*(mPer->covariance()))(1,1);
              double CovTrkMtx22 = (*(mPer->covariance()))(2,2);
	      if ( CovTrkMtx11 > 4.*m_A0TrkErrorCut*m_A0TrkErrorCut )  continue;
	      if ( CovTrkMtx22 > 4.*m_ZTrkErrorCut*m_ZTrkErrorCut )    continue;

                 long int PixelHits = 3, SctHits = 9, SharedHits = 0, BLayHits = 1;
                 const Trk::TrackSummary* testSum = (*i_nprt)->trackSummary();
                 PixelHits = (long int) testSum->get(Trk::numberOfPixelHits);
                 SctHits   = (long int) testSum->get(Trk::numberOfSCTHits);
                 BLayHits  = (long int) testSum->get(Trk::numberOfInnermostPixelLayerHits);
                 SharedHits= (long int) testSum->get(Trk::numberOfPixelSharedHits);
	         if(PixelHits<0)PixelHits=0;
                 if(SctHits<0)SctHits=0;
                 if(BLayHits<0)BLayHits=0; 
	         if(SharedHits>0) SharedHits--;
                 BLayHits++;
                 PixelHits++;
                 m_fitSvc->VKalGetImpact((*i_nprt), cache.m_BeamCnst, 1, Impact, ImpactError);
	         double ImpactA0=Impact[0];  
                 StatusCode sc = CutTrk( VectPerig[4] , VectPerig[3] , ImpactA0 ,
		              TrkQual->chiSquared() / TrkQual->numberDoF(),
		              PixelHits, SctHits, SharedHits, BLayHits);
	         if( sc == StatusCode::FAILURE ) continue;
//	      }
	      SelectedTrackParticles.push_back(*i_nprt);
	    }
 if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< " Found relaxed="<<SelectedTrackParticles.size()<<" particles" <<endmsg;
                      /* And now primary vertex search itself*/
	  }
//--- Cleaning
// 
          UniqList(SelectedTrackParticles);
                      /* And now primary vertex search itself*/

          NFoundVrt = PVrtListFind(cache,
                                   SelectedTrackParticles,
                                   SelectedTrkTracks,
                                   PrimVrtList,
                                   ErrorMatrixPerVrt,
                                   Chi2PerVrt,
                                   ControlVariablePerVrt,
                                   PrtPerVrt,
                                   TrkPerVrt,
                                   TrkWgtPerVrt);
          for (int i=0; i<NFoundVrt; i++) NTrkPerVrt.push_back((int) PrtPerVrt[i].size()); 
       

//--------------------------------------------------------------------

    if( NFoundVrt == 0){
      if(msgLvl(MSG::ERROR))msg(MSG::ERROR) << "Primary vertex not found" <<endmsg;
    }else{ 
      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<"Found Primary  Vertex=" << PrimVrtList[0].x()<<", "<<
             PrimVrtList[0].y()<<", "<<PrimVrtList[0].z()<< endmsg;
    }
//------------------------------------------------------------------------------- 
//
//---- Save vertices 
//
    return SaveResults(cache,
                       NFoundVrt,
                       PrimVrtList,
                       ErrorMatrixPerVrt,
                       Chi2PerVrt,
                       NTrkPerVrt,
                       PrtPerVrt,
                       TrkPerVrt,
                       TrkWgtPerVrt,
                       0,
                       newPrtCol);
  }



  std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>  
  InDetVKalPriVxFinderTool::SaveResults(Cache& cache,
   int NFoundVrt,
	 std::vector< Amg::Vector3D >                & PrimVrtList,
   std::vector< AmgSymMatrix(3) >              & ErrorMatrixPerVrt,
	 std::vector<double>                         & Chi2PerVrt,
   std::vector<int>                            & NTrkPerVrt,
	 std::vector< std::vector<const Trk::TrackParticleBase*> > & PrtPerVrt,
	 std::vector< std::vector<const Trk::Track*> >             & TrkPerVrt,
	 std::vector< std::vector<double> >                        & TrkWgtPerVrt,
   const TrackCollection* trackTES,
	 const Trk::TrackParticleBaseCollection* partTES) const{
	 
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
       std::vector<Trk::VxTrackAtVertex> & tmpVTAV=tmpVertex->vxTrackAtVertex();    
       tmpVTAV.clear();
       for(ii=0; ii<NTrkPerVrt[i]; ii++) {
         AmgSymMatrix(5) * tmpCovMatr=new AmgSymMatrix(5)(cache.m_fittedTrkCov.at(i).at(ii));
         Trk::Perigee * tmpMeasPer = new Trk::Perigee( 0.,0.,
		           cache.m_savedTrkFittedPerigees[i][ii][0] ,        /* Phi   */
		           cache.m_savedTrkFittedPerigees[i][ii][1] ,        /* Theta */
		           cache.m_savedTrkFittedPerigees[i][ii][2] ,        /* 1/p   */
	             Trk::PerigeeSurface(PrimVrtList[i]),
		           tmpCovMatr );
         Trk::VxTrackAtVertex * tmpPointer = new Trk::VxTrackAtVertex( 1., tmpMeasPer ) ;
	       if( not TrkPerVrt.empty()){ 
            ElementLink<TrackCollection>  TEL;   
            TEL.setElement(TrkPerVrt[i][ii]);
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
         delete tmpPointer;
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
       double** pntTracks=cache.m_savedTrkFittedPerigees[i];
       removeWorkArr2(pntTracks,(long int)NTrkPerVrt[i],3);
     }
     cache.m_savedTrkFittedPerigees.clear();
     cache.m_fittedTrkCov.clear();
     return std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> (vrtCont,vrtAuxCont);
   }
 
} // end of namespace bracket
