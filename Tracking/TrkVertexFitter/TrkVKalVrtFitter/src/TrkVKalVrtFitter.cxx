/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrkVKalVrtFitter/VKalVrtAtlas.h"
#include "TrkVKalVrtCore/TrkVKalVrtCore.h"
#include "TrkTrack/LinkToTrack.h"
#include "TrkTrack/Track.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "GaudiKernel/IChronoStatSvc.h"
//-------------------------------------------------
// Other stuff
#include<iostream>

namespace Trk{


//
//Constructor--------------------------------------------------------------
TrkVKalVrtFitter:: TrkVKalVrtFitter(const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent):
    base_class(type,name,parent),
    m_Robustness(0),
    m_RobustScale(1.),
    m_cascadeCnstPrecision(1.e-4),
    m_massForConstraint(-1.),
    m_IterationNumber(0),
    m_IterationPrecision(0),
    m_IDsizeR(1150.),
    m_IDsizeZ(3000.),
    m_extPropagator(this),                   // Internal propagator
    // m_extPropagator("Trk::Extrapolator/InDetExtrapolator"),  // External propagator
    m_firstMeasuredPoint(false),
    m_firstMeasuredPointLimit(false),
    m_makeExtendedVertex(false),
    m_useFixedField(false),
    m_useAprioriVertex(false),
    m_useThetaCnst(false),
    m_usePhiCnst(false),
    m_usePointingCnst(false),
    m_useZPointingCnst(false),
    m_usePassNear(false),
    m_usePassWithTrkErr(false)
   {
    declareInterface<IVertexFitter>(this);
    declareInterface<ITrkVKalVrtFitter>(this);
    declareInterface<IVertexCascadeFitter>(this);

    m_BMAG    = 1.997;      /*constant ATLAS magnetic field if no exact map*/
    m_CNVMAG  = 0.29979246;  /* conversion constant for MeV and MM */

    m_c_VertexForConstraint.clear();
    for( int i=0; i<3; i++){m_c_VertexForConstraint.push_back(0.);}
    m_c_CovVrtForConstraint.clear();
    for( int i=0; i<6; i++){m_c_CovVrtForConstraint.push_back(0.);}
    m_c_MassInputParticles.clear();
//
    declareProperty("Robustness",   m_Robustness);
    declareProperty("RobustScale",  m_RobustScale);
    declareProperty("CascadeCnstPrecision", m_cascadeCnstPrecision);
    declareProperty("MassForConstraint",    m_massForConstraint);
    declareProperty("IterationNumber",      m_IterationNumber);
    declareProperty("IterationPrecision",   m_IterationPrecision);
    declareProperty("IDsizeR",              m_IDsizeR);
    declareProperty("IDsizeZ",              m_IDsizeZ);
    declareProperty("VertexForConstraint",  m_c_VertexForConstraint);
    declareProperty("CovVrtForConstraint",  m_c_CovVrtForConstraint);
    declareProperty("InputParticleMasses",  m_c_MassInputParticles, "List of masses of input particles (pions assumed if this list is absent)" );
    declareProperty("Extrapolator",         m_extPropagator);
    declareProperty("useFixedField",        m_useFixedField, " Use fixed magnetic field instead of exact Atlas one");
    declareProperty("FirstMeasuredPoint",   m_firstMeasuredPoint);
    declareProperty("FirstMeasuredPointLimit",   m_firstMeasuredPointLimit);
    declareProperty("MakeExtendedVertex",   m_makeExtendedVertex, "VKalVrt returns VxCandidate with full covariance matrix");
//
    declareProperty("useAprioriVertexCnst",   m_useAprioriVertex);
    declareProperty("useThetaCnst",           m_useThetaCnst);
    declareProperty("usePhiCnst",             m_usePhiCnst);
    declareProperty("usePointingCnst",        m_usePointingCnst);
    declareProperty("useZPointingCnst",       m_useZPointingCnst);
    declareProperty("usePassNearCnst",        m_usePassNear);
    declareProperty("usePassWithTrkErrCnst",  m_usePassWithTrkErr);
//

/*--------------------------------------------------------------------------*/
/*  New propagator object is created. It's provided to VKalVrtCore.         */
/*  VKalVrtFitter must set up Core BEFORE any call required propagation!!!  */
/*  This object is created ONLY if IExtrapolator pointer is provideded.     */
/*         see VKalExtPropagator.cxx for details                            */
/*--------------------------------------------------------------------------*/
    m_fitPropagator = nullptr;       //Pointer to VKalVrtFitter propagator object to supply to VKalVrtCore (specific interface)
    m_InDetExtrapolator = nullptr;   //Direct pointer to Athena propagator

    m_isAtlasField       = false;   // To allow callback and then field first call only at execute stage
}


//Destructor---------------------------------------------------------------
TrkVKalVrtFitter::~TrkVKalVrtFitter(){
    //log << MSG::DEBUG << "TrkVKalVrtFitter destructor called" << endmsg;
    if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<"TrkVKalVrtFitter destructor called" << endmsg;
    if(m_fitPropagator) delete m_fitPropagator;
}

std::unique_ptr<IVKalState>
TrkVKalVrtFitter::makeState(const EventContext& ctx) const
{
  auto state = std::make_unique<State>();
  initState(ctx, *state);
  return state;
}

StatusCode TrkVKalVrtFitter::finalize()
{
    if(msgLvl(MSG::INFO))msg(MSG::INFO)<<"TrkVKalVrtFitter finalize() successful" << endmsg;
    return StatusCode::SUCCESS;
}


StatusCode TrkVKalVrtFitter::initialize()
{

// Checking ROBUST algoritms
    if(m_Robustness<0 || m_Robustness>7 ) m_Robustness=0;


    if(!m_useFixedField){
      // Read handle for AtlasFieldCacheCondObj
      if (!m_fieldCacheCondObjInputKey.key().empty()){
        if( (m_fieldCacheCondObjInputKey.initialize()).isSuccess() ){
           m_isAtlasField = true;
           ATH_MSG_DEBUG( "Found AtlasFieldCacheCondObj with key ="<< m_fieldCacheCondObjInputKey.key());
        }else{
           ATH_MSG_INFO( "No AtlasFieldCacheCondObj with key ="<< m_fieldCacheCondObjInputKey.key());
           ATH_MSG_INFO( "Use fixed magnetic field instead");
        }
      }
    }
//
// Only here the VKalVrtFitter propagator object is created if ATHENA propagator is provided (see setAthenaPropagator)
// In this case the ATHENA propagator can be used via pointers:
//     m_InDetExtrapolator - direct access
//     m_fitPropagator     - via VKalVrtFitter object VKalExtPropagator
// If ATHENA propagator is not provided, only defined object is
//     myPropagator      - extern propagator from TrkVKalVrtCore
//
    if (m_extPropagator.empty()){
      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "External propagator is not supplied - use internal one"<<endmsg;
      m_extPropagator.disable();
    }else{
      if (m_extPropagator.retrieve().isFailure()) {
        if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "Could not find external propagator=" <<m_extPropagator<<endmsg;
        if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "TrkVKalVrtFitter will uses internal propagator" << endmsg;
        m_extPropagator.disable();
      }else{
        if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "External propagator="<<m_extPropagator<<" retrieved" << endmsg;
        setAthenaPropagator(m_extPropagator.get());
      }
    }

//
//
//
    if(msgLvl(MSG::INFO))msg(MSG::INFO)<< "TrkVKalVrtFitter initialize() successful" << endmsg;
    if(msgLvl(MSG::DEBUG)){
       msg(MSG::DEBUG)<< "TrkVKalVrtFitter configuration:" << endmsg;
       msg(MSG::DEBUG)<< "   A priori vertex constraint:           "<< m_useAprioriVertex <<endmsg;
       msg(MSG::DEBUG)<< "   Angle dTheta=0 constraint:            "<< m_useThetaCnst <<endmsg;
       msg(MSG::DEBUG)<< "   Angle dPhi=0 constraint:              "<< m_usePhiCnst <<endmsg;
       msg(MSG::DEBUG)<< "   Pointing to other vertex constraint:  "<< m_usePointingCnst <<endmsg;
       msg(MSG::DEBUG)<< "   ZPointing to other vertex constraint: "<< m_useZPointingCnst <<endmsg;
       msg(MSG::DEBUG)<< "   Comb. particle pass near other vertex:"<< m_usePassNear <<endmsg;
       msg(MSG::DEBUG)<< "   Pass near with comb.particle errors:  "<< m_usePassWithTrkErr <<endmsg;
       if(m_massForConstraint>0){
         msg(MSG::DEBUG)<< "   Mass constraint M="<< m_massForConstraint <<endmsg;
         msg(MSG::DEBUG)<< " with particles M=";
         for(int i=0; i<(int)m_c_MassInputParticles.size(); i++) msg(MSG::DEBUG)<<m_c_MassInputParticles[i]<<", ";
         msg(MSG::DEBUG)<<endmsg; ;
       }

       if(m_isAtlasField){ msg(MSG::DEBUG)<< " ATLAS magnetic field is used!"<<endmsg;	}
       else            { msg(MSG::DEBUG)<< " Constant magnetic field is used! B="<<m_BMAG<<endmsg;	}

       if(m_InDetExtrapolator){ msg(MSG::DEBUG)<< " InDet extrapolator is used!"<<endmsg; }
       else { msg(MSG::DEBUG)<< " Internal VKalVrt extrapolator is used!"<<endmsg;}

       if(m_Robustness) { msg(MSG::DEBUG)<< " VKalVrt uses robust algorithm! Type="<<m_Robustness<<" with Scale="<<m_RobustScale<<endmsg; }

       if(m_firstMeasuredPoint){ msg(MSG::DEBUG)<< " VKalVrt will use FirstMeasuredPoint strategy in fits with InDetExtrapolator"<<endmsg; }
       else                    { msg(MSG::DEBUG)<< " VKalVrt will use Perigee strategy in fits with InDetExtrapolator"<<endmsg; }
       if(m_firstMeasuredPointLimit){ msg(MSG::DEBUG)<< " VKalVrt will use FirstMeasuredPointLimit strategy "<<endmsg; }
    }


    return StatusCode::SUCCESS;
}


void TrkVKalVrtFitter::initState (State& state) const
{
    initState(Gaudi::Hive::currentContext(), state);
}


void TrkVKalVrtFitter::initState (const EventContext& ctx, State& state) const

{
  //----------------------------------------------------------------------
  //  New magnetic field object is created. It's provided to VKalVrtCore.
  //  VKalVrtFitter must set up Core BEFORE any call required propagation!!!
  //
  if (m_isAtlasField) {
     // For the moment, use Gaudi Hive for the event context - would need to be passed in from clients
     SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCacheCondObjInputKey, ctx};
     const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};
     if (fieldCondObj == nullptr) {
        ATH_MSG_ERROR("Failed to retrieve AtlasFieldCacheCondObj with key " << m_fieldCacheCondObjInputKey.key());
        return;
     }
     fieldCondObj->getInitializedCache (state.m_fieldCache);
     state.m_fitField.setAtlasField(&state.m_fieldCache);
  } else {
     state.m_fitField.setAtlasField(m_BMAG);
  }

  state.m_vkalFitControl.vk_objProp = m_fitPropagator;
  state.m_useAprioriVertex = m_useAprioriVertex;
  state.m_useThetaCnst = m_useThetaCnst;
  state.m_usePhiCnst = m_usePhiCnst;
  state.m_usePointingCnst = m_usePointingCnst;
  state.m_useZPointingCnst = m_useZPointingCnst;
  state.m_usePassNear = m_usePassNear;
  state.m_usePassWithTrkErr = m_usePassWithTrkErr;
  state.m_VertexForConstraint = m_c_VertexForConstraint;
  state.m_CovVrtForConstraint = m_c_CovVrtForConstraint;
  state.m_massForConstraint = m_massForConstraint;
  state.m_Robustness = m_Robustness;
  state.m_RobustScale = m_RobustScale;
  state.m_MassInputParticles = m_c_MassInputParticles;
}

/** Interface for MeasuredPerigee with starting point */
xAOD::Vertex * TrkVKalVrtFitter::fit(const std::vector<const TrackParameters*> & perigeeListC,
                                     const Amg::Vector3D & startingPoint) const
{
    State state;
    initState (state);
    setApproximateVertex(startingPoint.x(),
                         startingPoint.y(),
                         startingPoint.z(),
                         state);
    std::vector<const NeutralParameters*> perigeeListN(0);
    Amg::Vector3D Vertex;
    TLorentzVector Momentum;
    long int Charge;
    std::vector<double> ErrorMatrix;
    std::vector<double> Chi2PerTrk;
    std::vector< std::vector<double> >  TrkAtVrt;
    double Chi2;
    StatusCode sc=VKalVrtFit( perigeeListC, perigeeListN,
                              Vertex, Momentum, Charge, ErrorMatrix, Chi2PerTrk, TrkAtVrt, Chi2, state, true );

    xAOD::Vertex * tmpVertex = nullptr;
    if(sc.isSuccess()) {
      tmpVertex = makeXAODVertex( 0, Vertex, ErrorMatrix, Chi2PerTrk, TrkAtVrt, Chi2, state );
    }
    return tmpVertex;
}


xAOD::Vertex * TrkVKalVrtFitter::fit(const std::vector<const TrackParameters*>   & perigeeListC,
                                    const std::vector<const NeutralParameters*> & perigeeListN,
                                    const Amg::Vector3D & startingPoint) const
{
    State state;
    initState (state);
    setApproximateVertex(startingPoint.x(),
                         startingPoint.y(),
                         startingPoint.z(),
                         state);
    Amg::Vector3D Vertex;
    TLorentzVector Momentum;
    long int Charge;
    std::vector<double> ErrorMatrix;
    std::vector<double> Chi2PerTrk;
    std::vector< std::vector<double> >  TrkAtVrt;
    double Chi2;
    StatusCode sc=VKalVrtFit( perigeeListC,perigeeListN,
                              Vertex, Momentum, Charge, ErrorMatrix, Chi2PerTrk, TrkAtVrt, Chi2, state, true );

    xAOD::Vertex * tmpVertex = nullptr;
    if(sc.isSuccess()) {
      tmpVertex = makeXAODVertex( (int)perigeeListN.size(), Vertex, ErrorMatrix, Chi2PerTrk, TrkAtVrt, Chi2, state );
    }
    return tmpVertex;
}






     /** Interface for MeasuredPerigee with vertex constraint */
     /** the position of the constraint is ALWAYS the starting point */
xAOD::Vertex * TrkVKalVrtFitter::fit(const std::vector<const TrackParameters*> & perigeeListC,
                                     const xAOD::Vertex & constraint) const
{
    State state;
    initState (state);
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<< "A priori vertex constraint is activated in VKalVrt fitter!" << endmsg;
    Amg::Vector3D VertexIni(0.,0.,0.);
    StatusCode sc=VKalVrtFitFast(perigeeListC, VertexIni, state);
    if( sc.isSuccess()){
       setApproximateVertex(VertexIni.x(),VertexIni.y(),VertexIni.z(),state);
    }else{
       setApproximateVertex(constraint.position().x(),
                            constraint.position().y(),
                            constraint.position().z(),
                            state);
    }
    setVertexForConstraint(constraint.position().x(),
                           constraint.position().y(),
                           constraint.position().z(),
                           state);
    setCovVrtForConstraint(constraint.covariancePosition()(Trk::x,Trk::x),
                           constraint.covariancePosition()(Trk::y,Trk::x),
                           constraint.covariancePosition()(Trk::y,Trk::y),
                           constraint.covariancePosition()(Trk::z,Trk::x),
                           constraint.covariancePosition()(Trk::z,Trk::y),
                           constraint.covariancePosition()(Trk::z,Trk::z),
                           state);
    state.m_useAprioriVertex=true;
    std::vector<const NeutralParameters*> perigeeListN(0);
    Amg::Vector3D Vertex;
    TLorentzVector Momentum;
    long int Charge;
    std::vector<double> ErrorMatrix;
    std::vector<double> Chi2PerTrk;
    std::vector< std::vector<double> >  TrkAtVrt;
    double Chi2;
    sc=VKalVrtFit( perigeeListC, perigeeListN,
                   Vertex, Momentum, Charge, ErrorMatrix, Chi2PerTrk, TrkAtVrt, Chi2, state, true );


    xAOD::Vertex * tmpVertex = nullptr;
    if(sc.isSuccess()) {
      tmpVertex = makeXAODVertex( 0, Vertex, ErrorMatrix, Chi2PerTrk, TrkAtVrt, Chi2, state );
    }
    return tmpVertex;
}


xAOD::Vertex * TrkVKalVrtFitter::fit(const std::vector<const TrackParameters*>   & perigeeListC,
                                    const std::vector<const NeutralParameters*> & perigeeListN,
                                    const xAOD::Vertex & constraint) const
{
    State state;
    initState (state);

    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<< "A priori vertex constraint is activated in VKalVrt fitter!" << endmsg;
    Amg::Vector3D VertexIni(0.,0.,0.);
    StatusCode sc=VKalVrtFitFast(perigeeListC, VertexIni, state);
    if( sc.isSuccess()){
       setApproximateVertex(VertexIni.x(),VertexIni.y(),VertexIni.z(),state);
    }else{
       setApproximateVertex(constraint.position().x(),
                            constraint.position().y(),
                            constraint.position().z(),
                            state);
    }
    setVertexForConstraint(constraint.position().x(),
                           constraint.position().y(),
                           constraint.position().z(),
                           state);
    setCovVrtForConstraint(constraint.covariancePosition()(Trk::x,Trk::x),
                           constraint.covariancePosition()(Trk::y,Trk::x),
                           constraint.covariancePosition()(Trk::y,Trk::y),
                           constraint.covariancePosition()(Trk::z,Trk::x),
                           constraint.covariancePosition()(Trk::z,Trk::y),
                           constraint.covariancePosition()(Trk::z,Trk::z),
                           state);
    state.m_useAprioriVertex=true;
    Amg::Vector3D Vertex;
    TLorentzVector Momentum;
    long int Charge;
    std::vector<double> ErrorMatrix;
    std::vector<double> Chi2PerTrk;
    std::vector< std::vector<double> >  TrkAtVrt;
    double Chi2;
    sc=VKalVrtFit( perigeeListC, perigeeListN,
                   Vertex, Momentum, Charge, ErrorMatrix, Chi2PerTrk, TrkAtVrt, Chi2, state, true );


    xAOD::Vertex * tmpVertex = nullptr;
    if(sc.isSuccess()) {
      tmpVertex = makeXAODVertex( (int)perigeeListN.size(), Vertex, ErrorMatrix, Chi2PerTrk, TrkAtVrt, Chi2, state );
    }
    return tmpVertex;
}

/** Interface for xAOD::TrackParticle with starting point
 *  Implements the new style (unique_ptr,EventContext)
 * */
std::unique_ptr<xAOD::Vertex>
TrkVKalVrtFitter::fit(const EventContext& ctx,
                      const std::vector<const xAOD::TrackParticle*>& xtpListC,
                      const Amg::Vector3D& startingPoint) const
{
  State state;
  initState(ctx, state);
  return std::unique_ptr<xAOD::Vertex>(fit(xtpListC, startingPoint, state));
}

xAOD::Vertex * TrkVKalVrtFitter::fit(const std::vector<const xAOD::TrackParticle*> & xtpListC,
                                     const Amg::Vector3D & startingPoint,
                                     IVKalState& istate) const
{
    assert(dynamic_cast<State*> (&istate)!=nullptr);
    State& state = static_cast<State&> (istate);

    xAOD::Vertex * tmpVertex = nullptr;
    setApproximateVertex(startingPoint.x(),
                         startingPoint.y(),
                         startingPoint.z(),
                         state);
    std::vector<const xAOD::NeutralParticle*> xtpListN(0);
    Amg::Vector3D Vertex;
    TLorentzVector Momentum;
    long int Charge;
    std::vector<double> ErrorMatrix;
    std::vector<double> Chi2PerTrk;
    std::vector< std::vector<double> >  TrkAtVrt;
    double Chi2;
    StatusCode sc=VKalVrtFit( xtpListC, xtpListN,
                              Vertex, Momentum, Charge, ErrorMatrix, Chi2PerTrk, TrkAtVrt, Chi2, state, true );
    if(sc.isSuccess()) {
       tmpVertex = makeXAODVertex( 0, Vertex, ErrorMatrix, Chi2PerTrk, TrkAtVrt, Chi2, state );
       dvect fittrkwgt;
       sc=VKalGetTrkWeights(fittrkwgt, state); if(sc.isFailure())fittrkwgt.clear();
       for(int ii=0; ii<state.m_FitStatus; ii++) {
          ElementLink<xAOD::TrackParticleContainer> TEL;  TEL.setElement( xtpListC[ii] );
          if(!fittrkwgt.empty())  tmpVertex->addTrackAtVertex(TEL,fittrkwgt[ii]);
          else                    tmpVertex->addTrackAtVertex(TEL,1.);
       }
    }

    return tmpVertex;
}

xAOD::Vertex * TrkVKalVrtFitter::fit(const std::vector<const xAOD::TrackParticle*>   & xtpListC,
                                     const std::vector<const xAOD::NeutralParticle*> & xtpListN,
                                     const Amg::Vector3D & startingPoint) const
{
    State state;
    initState (state);
    xAOD::Vertex * tmpVertex = nullptr;
    setApproximateVertex(startingPoint.x(),
                         startingPoint.y(),
                         startingPoint.z(),
                         state);
    Amg::Vector3D Vertex;
    TLorentzVector Momentum;
    long int Charge;
    std::vector<double> ErrorMatrix;
    std::vector<double> Chi2PerTrk;
    std::vector< std::vector<double> >  TrkAtVrt;
    double Chi2;
    StatusCode sc=VKalVrtFit( xtpListC, xtpListN,
                              Vertex, Momentum, Charge, ErrorMatrix, Chi2PerTrk, TrkAtVrt, Chi2, state, true );
    if(sc.isSuccess()) {
       tmpVertex = makeXAODVertex( (int)xtpListN.size(), Vertex, ErrorMatrix, Chi2PerTrk, TrkAtVrt, Chi2, state );
       dvect fittrkwgt;
       sc=VKalGetTrkWeights(fittrkwgt, state); if(sc.isFailure())fittrkwgt.clear();
       for(int ii=0; ii<state.m_FitStatus; ii++) {
          if(ii<(int)xtpListC.size()) {
             ElementLink<xAOD::TrackParticleContainer> TEL;  TEL.setElement( xtpListC[ii] );
             if(!fittrkwgt.empty())  tmpVertex->addTrackAtVertex(TEL,fittrkwgt[ii]);
             else                    tmpVertex->addTrackAtVertex(TEL,1.);
          }else{
             ElementLink<xAOD::NeutralParticleContainer> TEL;  TEL.setElement( xtpListN[ii] );
             if(!fittrkwgt.empty())  tmpVertex->addNeutralAtVertex(TEL,fittrkwgt[ii]);
             else                    tmpVertex->addNeutralAtVertex(TEL,1.);
          }
       }
    }

    return tmpVertex;
}

     /** Interface for xAOD::TrackParticle with vertex constraint */
     /** the position of the constraint is ALWAYS the starting point */
xAOD::Vertex * TrkVKalVrtFitter::fit(const std::vector<const xAOD::TrackParticle*> & xtpListC,
                                     const xAOD::Vertex & constraint) const
{
    State state;
    initState (state);
    return fit (xtpListC, constraint, state);
}
xAOD::Vertex * TrkVKalVrtFitter::fit(const std::vector<const xAOD::TrackParticle*> & xtpListC,
                                     const xAOD::Vertex & constraint,
                                     IVKalState& istate) const
{
    assert(dynamic_cast<State*> (&istate)!=nullptr);
    State& state = static_cast<State&> (istate);

    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<< "A priori vertex constraint is activated in VKalVrt fitter!" << endmsg;
    xAOD::Vertex * tmpVertex = nullptr;
    setApproximateVertex(constraint.position().x(), constraint.position().y(),constraint.position().z(),state);
    setVertexForConstraint(constraint.position().x(),
                           constraint.position().y(),
                           constraint.position().z(),
                           state);
    setCovVrtForConstraint(constraint.covariancePosition()(Trk::x,Trk::x),
                           constraint.covariancePosition()(Trk::y,Trk::x),
                           constraint.covariancePosition()(Trk::y,Trk::y),
                           constraint.covariancePosition()(Trk::z,Trk::x),
                           constraint.covariancePosition()(Trk::z,Trk::y),
                           constraint.covariancePosition()(Trk::z,Trk::z),
                           state);
    state.m_useAprioriVertex=true;
    std::vector<const xAOD::NeutralParticle*> xtpListN(0);
    Amg::Vector3D Vertex;
    TLorentzVector Momentum;
    long int Charge;
    std::vector<double> ErrorMatrix;
    std::vector<double> Chi2PerTrk;
    std::vector< std::vector<double> >  TrkAtVrt;
    double Chi2;
    StatusCode sc=VKalVrtFit( xtpListC, xtpListN,
                              Vertex, Momentum, Charge, ErrorMatrix, Chi2PerTrk, TrkAtVrt, Chi2, state, true );
    if(sc.isSuccess()) {
       tmpVertex = makeXAODVertex( 0, Vertex, ErrorMatrix,Chi2PerTrk, TrkAtVrt, Chi2, state );
       dvect fittrkwgt;
       sc=VKalGetTrkWeights(fittrkwgt, state); if(sc.isFailure())fittrkwgt.clear();
       for(int ii=0; ii<state.m_FitStatus; ii++) {
          ElementLink<xAOD::TrackParticleContainer> TEL;  TEL.setElement( xtpListC[ii] );
          if(!fittrkwgt.empty())  tmpVertex->addTrackAtVertex(TEL,fittrkwgt[ii]);
          else                    tmpVertex->addTrackAtVertex(TEL,1.);
       }
    }

    return tmpVertex;
}

xAOD::Vertex * TrkVKalVrtFitter::fit(const std::vector<const xAOD::TrackParticle*>   & xtpListC,
                                     const std::vector<const xAOD::NeutralParticle*> & xtpListN,
                                     const xAOD::Vertex & constraint) const
{
    State state;
    initState (state);

    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<< "A priori vertex constraint is activated in VKalVrt fitter!" << endmsg;
    xAOD::Vertex * tmpVertex = nullptr;
    setApproximateVertex(constraint.position().x(), constraint.position().y(),constraint.position().z(),state);
    setVertexForConstraint(constraint.position().x(),
                           constraint.position().y(),
                           constraint.position().z(),
                           state);
    setCovVrtForConstraint(constraint.covariancePosition()(Trk::x,Trk::x),
                           constraint.covariancePosition()(Trk::y,Trk::x),
                           constraint.covariancePosition()(Trk::y,Trk::y),
                           constraint.covariancePosition()(Trk::z,Trk::x),
                           constraint.covariancePosition()(Trk::z,Trk::y),
                           constraint.covariancePosition()(Trk::z,Trk::z),
                           state);
    state.m_useAprioriVertex=true;
    Amg::Vector3D Vertex;
    TLorentzVector Momentum;
    long int Charge;
    std::vector<double> ErrorMatrix;
    std::vector<double> Chi2PerTrk;
    std::vector< std::vector<double> >  TrkAtVrt;
    double Chi2;
    StatusCode sc=VKalVrtFit( xtpListC, xtpListN,
                              Vertex, Momentum, Charge, ErrorMatrix, Chi2PerTrk, TrkAtVrt, Chi2, state, true );
    if(sc.isSuccess()){
       tmpVertex = makeXAODVertex( (int)xtpListN.size(), Vertex, ErrorMatrix,Chi2PerTrk, TrkAtVrt, Chi2, state );
       dvect fittrkwgt;
       sc=VKalGetTrkWeights(fittrkwgt, state); if(sc.isFailure())fittrkwgt.clear();
       for(int ii=0; ii<state.m_FitStatus; ii++) {
          if(ii<(int)xtpListC.size()) {
             ElementLink<xAOD::TrackParticleContainer> TEL;  TEL.setElement( xtpListC[ii] );
             if(!fittrkwgt.empty())  tmpVertex->addTrackAtVertex(TEL,fittrkwgt[ii]);
             else                    tmpVertex->addTrackAtVertex(TEL,1.);
          }else{
             ElementLink<xAOD::NeutralParticleContainer> TEL;  TEL.setElement( xtpListN[ii] );
             if(!fittrkwgt.empty())  tmpVertex->addNeutralAtVertex(TEL,fittrkwgt[ii]);
             else                    tmpVertex->addNeutralAtVertex(TEL,1.);
          }
       }
    }

    return tmpVertex;
}


xAOD::Vertex * TrkVKalVrtFitter::fit(const std::vector<const  TrackParameters*> & perigeeListC) const
{
    State state;
    initState (state);
    Amg::Vector3D VertexIni(0.,0.,0.);
    StatusCode sc=VKalVrtFitFast(perigeeListC, VertexIni, state);
    if( sc.isSuccess()) setApproximateVertex(VertexIni.x(),VertexIni.y(),VertexIni.z(),state);
    std::vector<const NeutralParameters*> perigeeListN(0);
    Amg::Vector3D Vertex;
    TLorentzVector Momentum;
    long int Charge;
    std::vector<double> ErrorMatrix;
    std::vector<double> Chi2PerTrk;
    std::vector< std::vector<double> >  TrkAtVrt;
    double Chi2;
    sc=VKalVrtFit( perigeeListC, perigeeListN,
                   Vertex, Momentum, Charge, ErrorMatrix, Chi2PerTrk, TrkAtVrt, Chi2, state, true );

    xAOD::Vertex * tmpVertex = nullptr;
    if(sc.isSuccess()) {
       tmpVertex = makeXAODVertex( 0, Vertex, ErrorMatrix, Chi2PerTrk, TrkAtVrt, Chi2, state );
    }
    return tmpVertex;
}

xAOD::Vertex * TrkVKalVrtFitter::fit(const std::vector<const  TrackParameters*>   & perigeeListC,
                                    const std::vector<const  NeutralParameters*> & perigeeListN) const
{
    State state;
    initState (state);
    Amg::Vector3D VertexIni(0.,0.,0.);
    StatusCode sc=VKalVrtFitFast(perigeeListC, VertexIni, state);
    if( sc.isSuccess()) setApproximateVertex(VertexIni.x(),VertexIni.y(),VertexIni.z(),state);
    Amg::Vector3D Vertex;
    TLorentzVector Momentum;
    long int Charge;
    std::vector<double> ErrorMatrix;
    std::vector<double> Chi2PerTrk;
    std::vector< std::vector<double> >  TrkAtVrt;
    double Chi2;
    sc=VKalVrtFit( perigeeListC, perigeeListN,
                   Vertex, Momentum, Charge, ErrorMatrix, Chi2PerTrk, TrkAtVrt, Chi2, state, true );

    xAOD::Vertex * tmpVertex = nullptr;
    if(sc.isSuccess()) {
       tmpVertex = makeXAODVertex( (int)perigeeListN.size(), Vertex, ErrorMatrix, Chi2PerTrk, TrkAtVrt, Chi2, state );
    }
    return tmpVertex;
}



/* Filling of 3x3 HepSymMatrix with content of symmetric matrix
   in packed form vector<double> (6x6 - 21 elem)
            (VxVyVzPxPyPz)                                      */

// Fills 5x5 matrix. Input Matrix is track covariance only.
void TrkVKalVrtFitter::FillMatrixP(AmgSymMatrix(5)& CovMtx, std::vector<double> & Matrix) const
{
    CovMtx.setIdentity();
    if( Matrix.size() < 21) return;
    CovMtx(0,0) =  0;
    CovMtx(1,1) =  0;
    CovMtx(2,2)= Matrix[ 9];
    CovMtx.fillSymmetric(2,3,Matrix[13]);
    CovMtx(3,3)= Matrix[14];
    CovMtx.fillSymmetric(2,4,Matrix[18]);
    CovMtx.fillSymmetric(3,4,Matrix[19]);
    CovMtx(4,4)= Matrix[20];
}

// Fills 5x5 matrix.  Input Matrix is a full covariance
void TrkVKalVrtFitter::FillMatrixP(int iTrk, AmgSymMatrix(5)& CovMtx, std::vector<double> & Matrix) const
{
    int iTmp=(iTrk+1)*3;
    int NContent = Matrix.size();
    CovMtx.setIdentity();        //Clean matrix for the beginning, then fill needed elements
    CovMtx(0,0) =  0;
    CovMtx(1,1) =  0;
    int pnt = (iTmp+1)*iTmp/2 + iTmp;   if( pnt   > NContent ) return;
    CovMtx(2,2) =  Matrix[pnt];
    pnt = (iTmp+1+1)*(iTmp+1)/2 + iTmp; if( pnt+1 > NContent ){ CovMtx.setIdentity();  return; }
    CovMtx(2,3) =  Matrix[pnt];
    CovMtx(3,3) =  Matrix[pnt+1];
    pnt = (iTmp+2+1)*(iTmp+2)/2 + iTmp; if( pnt+2 > NContent ){ CovMtx.setIdentity();  return; }
    CovMtx(2,4) = Matrix[pnt];
    CovMtx(3,4) = Matrix[pnt+1];
    CovMtx(4,4) = Matrix[pnt+2];
}



Amg::MatrixX * TrkVKalVrtFitter::GiveFullMatrix(int NTrk, std::vector<double> & Matrix) const
{
   Amg::MatrixX * mtx = new Amg::MatrixX(3+3*NTrk,3+3*NTrk);
   long int ij=0;
   for(int i=1; i<=(3+3*NTrk); i++){
      for(int j=1; j<=i; j++){
         if(i==j){ (*mtx)(i-1,j-1)=Matrix[ij];}
	 else    { (*mtx).fillSymmetric(i-1,j-1,Matrix[ij]);}
	 ij++;
      }
   }
   return mtx;
}



xAOD::Vertex * TrkVKalVrtFitter::makeXAODVertex( int Neutrals,
        const Amg::Vector3D& Vertex, const std::vector<double> & fitErrorMatrix,
	const std::vector<double> & Chi2PerTrk,  const std::vector< std::vector<double> >& TrkAtVrt,
                                                 double Chi2,
                                                 const State& state) const
{
    long int NTrk = state.m_FitStatus;
    long int Ndf = VKalGetNDOF(state)+state.m_planeCnstNDOF;

    xAOD::Vertex * tmpVertex=new xAOD::Vertex();
    tmpVertex->makePrivateStore();
    tmpVertex->setPosition(Vertex);
    tmpVertex->setFitQuality(Chi2, (float)Ndf);

    std::vector<VxTrackAtVertex> & tmpVTAV=tmpVertex->vxTrackAtVertex();
    tmpVTAV.clear();
    std::vector <double> CovFull;
    StatusCode sc = VKalGetFullCov( NTrk, CovFull, state);
    int covarExist=0; if( sc.isSuccess() ) covarExist=1;

    std::vector<float> floatErrMtx;
    if( m_makeExtendedVertex && covarExist ) {
       floatErrMtx.resize(CovFull.size());
       for(int i=0; i<(int)CovFull.size(); i++) floatErrMtx[i]=CovFull[i];
    }else{
       floatErrMtx.resize(fitErrorMatrix.size());
       for(int i=0; i<(int)fitErrorMatrix.size(); i++) floatErrMtx[i]=fitErrorMatrix[i];
    }
    tmpVertex->setCovariance(floatErrMtx);

    for(int ii=0; ii<NTrk ; ii++) {
      AmgSymMatrix(5) *CovMtxP=new AmgSymMatrix(5);
      if(covarExist){ FillMatrixP( ii, (*CovMtxP), CovFull );}
      else          { (*CovMtxP).setIdentity();}
      Perigee *        tmpChargPer=nullptr;
      NeutralPerigee * tmpNeutrPer=nullptr;
      if(ii<NTrk-Neutrals){
         tmpChargPer  =  new Perigee( 0.,0., TrkAtVrt[ii][0],
	                                     TrkAtVrt[ii][1],
				   	     TrkAtVrt[ii][2],
				 	     PerigeeSurface(Vertex), CovMtxP );
      }else{
         tmpNeutrPer  =  new NeutralPerigee( 0.,0., TrkAtVrt[ii][0],
	                                            TrkAtVrt[ii][1],
						    TrkAtVrt[ii][2],
						    PerigeeSurface(Vertex),
					                    CovMtxP );
      }
      tmpVTAV.emplace_back(Chi2PerTrk[ii], tmpChargPer, tmpNeutrPer );
    }

    return tmpVertex;
}


} // End Of Namespace
