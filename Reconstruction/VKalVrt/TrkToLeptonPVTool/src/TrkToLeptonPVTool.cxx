/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/*
   Tool to match a track to a Primary Vertex obtained with other leptons/tracks

    Author: Vadim Kostyukhin
    e-mail: vadim.kostyukhin@cern.ch
*/
#include "TrkToLeptonPVTool/TrkToLeptonPVTool.h"

#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
//
#include "GaudiKernel/IChronoStatSvc.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
//
#include  "AthContainers/AuxStoreInternal.h"
//
//Constructor-------------------------------------------------------------- 
TrkToLeptonPVTool::TrkToLeptonPVTool(const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent):
  AthAlgTool(type,name,parent),
  m_beamService("BeamCondSvc",name),
  m_fitterSvc("Trk::TrkVKalVrtFitter/VertexFitterTool",this),
  m_timingProfile("ChronoStatSvc",name)
  {
     declareInterface<ITrkToLeptonPV>(this);
     declareProperty("BeamSpotSvc", m_beamService, "Name of the BeamSpot service");
     declareProperty("VertexFitter", m_fitterSvc, "Name of the Vertex Fitter tool");
     declareProperty("ChronoStatSvc", m_timingProfile, "Name of the timing service");
  }

//Destructor---------------------------------------------------------------
  TrkToLeptonPVTool::~TrkToLeptonPVTool(){
    ATH_MSG_DEBUG("TrkToLeptonPVTool destructor called");
  }

//Initialize---------------------------------------------------------------
   StatusCode TrkToLeptonPVTool::initialize(){
     //-----
     if (m_fitterSvc.retrieve().isFailure()) {
        ATH_MSG_DEBUG("Could not find Trk::TrkVKalVrtFitter");
        return StatusCode::FAILURE;
     } else {
        ATH_MSG_DEBUG("TrkToLeptonPVTool TrkVKalVrtFitter found");
     }
     //-----
     if(m_beamService.retrieve().isFailure()) {
       ATH_MSG_DEBUG("Can't retrieve BeamService");
       return StatusCode::FAILURE;
     }
     //-----
     if(m_timingProfile.retrieve().isFailure()) {
       ATH_MSG_DEBUG("Can't retrieve ChronoStat service");
     }
     //-----
     return StatusCode::SUCCESS;
   }

   StatusCode TrkToLeptonPVTool::finalize()
   {
    if(m_timingProfile)m_timingProfile->chronoPrint("TrkToLeptonPVTool");
    ATH_MSG_DEBUG(" finalize()");
    return StatusCode::SUCCESS; 
   }


   std::unique_ptr<xAOD::Vertex>  TrkToLeptonPVTool::matchTrkToPV( const xAOD::TrackParticle *trk, const xAOD::Vertex * PV,
                                                                         const xAOD::EventInfo * eventINFO) const
   {
     if(trk->isAvailable<float>("vy")) {
       std::vector<const xAOD::TrackParticle *> tpv(1,trk);
       return std::unique_ptr<xAOD::Vertex>(m_fitterSvc->fit(tpv,(*PV)));
     }

     //---DAOD case     
     if( !eventINFO ) return std::unique_ptr<xAOD::Vertex>(nullptr);
     std::unique_ptr< SG::AuxStoreInternal > pAux;
     xAOD::TrackParticleContainer   TPC;
     std::vector<const xAOD::TrackParticle*>     wrkTrkC(1);
     pAux = std::make_unique< SG::AuxStoreInternal >();
     TPC.setStore( pAux.get() );
     TPC.reserve( 1 );
     TPC.push_back(new xAOD::TrackParticle(*trk));
     if(!TPC[0])return std::unique_ptr<xAOD::Vertex>(nullptr);
     const float mvx= (eventINFO) ? eventINFO->beamPosX() : 0.;
     const float mvy= (eventINFO) ? eventINFO->beamPosY() : 0.;
     const float mvz= (trk->isAvailable<float>("vz")) ? trk->vz() : 0.;
     TPC[0]->setParametersOrigin( mvx, mvy, mvz);
     wrkTrkC[0]=TPC[0];
     return std::unique_ptr<xAOD::Vertex>(m_fitterSvc->fit(wrkTrkC,(*PV)));

   }


   std::unique_ptr<xAOD::Vertex>TrkToLeptonPVTool::npartVertex( const std::vector<const xAOD::TrackParticle*> & particles,
                                                                      const xAOD::EventInfo * eventINFO) const
   {
     if(particles.size()<2) return std::unique_ptr<xAOD::Vertex>(nullptr);

     std::vector<const xAOD::TrackParticle*> tmpp(particles);
     std::sort(tmpp.begin(),tmpp.end());
     auto tst=std::unique(tmpp.begin(),tmpp.end());
     if( tst !=  tmpp.end()) {
       ATH_MSG_DEBUG(" Duplicated particles on input!");
       return std::unique_ptr<xAOD::Vertex>(nullptr);
     }

     bool fullxAOD=false;   if(particles[0]->isAvailable<float>("vy")) fullxAOD=true;

     xAOD::Vertex BEAM; 
     BEAM.makePrivateStore();
     BEAM.setX(0.); BEAM.setY(0.); BEAM.setZ(0.);
     std::vector<float> defaultCovar={0.015*0.015,0.,0.015*0.015,0.,0.,1.e6};
     BEAM.setCovariance(defaultCovar);
     //-------------------------------
     if(eventINFO){
          BEAM.setX(eventINFO->beamPosX());
          BEAM.setY(eventINFO->beamPosY());
          BEAM.setZ(eventINFO->beamPosZ());
          AmgSymMatrix(3) cov;
          cov(0,0) = eventINFO->beamPosSigmaX() * eventINFO->beamPosSigmaX();
          cov(1,1) = eventINFO->beamPosSigmaY() * eventINFO->beamPosSigmaY();
          cov(2,2) = eventINFO->beamPosSigmaZ() * eventINFO->beamPosSigmaZ()*1.e6;  //No real constraint in Z
          BEAM.setCovariancePosition(cov);
     }         
     if(m_beamService && fullxAOD ){
          ATH_MSG_DEBUG("Beam service is present");
          BEAM.setPosition(m_beamService->beamVtx().position());
          BEAM.setCovariancePosition(m_beamService->beamVtx().covariancePosition());
     }         
     if(fullxAOD){ ATH_MSG_DEBUG("xAOD data"); }
     else        { ATH_MSG_DEBUG("DxAOD data");}
     ATH_MSG_DEBUG("BEAM  x,y,z="<<BEAM.x()<<","<<BEAM.y()<<","<<BEAM.z());
     ATH_MSG_DEBUG("BEAM covariance="<<BEAM.covariance()[0]<<","<<BEAM.covariance()[1]<<","<<BEAM.covariance()[2]
                                <<","<<BEAM.covariance()[3]<<","<<BEAM.covariance()[4]<<","<<BEAM.covariance()[5]);


     if(fullxAOD) return std::unique_ptr<xAOD::Vertex>(m_fitterSvc->fit(particles,BEAM));

     //
     //---DAOD case
     //
     int NPRT=particles.size();
     std::unique_ptr< SG::AuxStoreInternal > pAux;
     xAOD::TrackParticleContainer   TPC;
     std::vector<const xAOD::TrackParticle*>     wrkTrkC(NPRT);
     pAux = std::make_unique< SG::AuxStoreInternal >();
     TPC.setStore( pAux.get() );
     TPC.reserve( NPRT );
     for(int i=0; i<NPRT; i++){
	TPC.push_back(new xAOD::TrackParticle(*particles[i]));
	if(!TPC[i])return std::unique_ptr<xAOD::Vertex>(nullptr);
	const float mvx= (eventINFO) ? eventINFO->beamPosX() : 0.;
	const float mvy= (eventINFO) ? eventINFO->beamPosY() : 0.;
	const float mvz= (particles[i]->isAvailable<float>("vz")) ? particles[i]->vz() : 0.;
	TPC[i]->setParametersOrigin( mvx, mvy, mvz);
	wrkTrkC[i]=TPC[i];
     }
     return std::unique_ptr<xAOD::Vertex>(m_fitterSvc->fit(wrkTrkC,BEAM));

   }


