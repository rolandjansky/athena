/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "GaudiKernel/IChronoStatSvc.h"
//-------------------------------------------------
// Other stuff
#include<iostream>

namespace Trk {
    extern vkalMagFld      myMagFld;
}
namespace Trk{

//
//Constructor-------------------------------------------------------------- 
TrkVKalVrtFitter:: TrkVKalVrtFitter(const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_Robustness(0),
    m_RobustScale(1.),
    m_cascadeCnstPrecision(1.e-4),
    m_massForConstraint(-1.),
    m_IterationNumber(0),
    m_IterationPrecision(0),
    m_IDsizeR(1150.),
    m_IDsizeZ(3000.),
    m_extPropagator("DefaultVKalPropagator"),                   // Internal propagator
    // m_extPropagator("Trk::Extrapolator/InDetExtrapolator"),  // External propagator
    // m_magFieldAthenaSvc("MagFieldAthenaSvc",name),           //Athena magnetic field---old version
    m_magFieldAthenaSvc("AtlasFieldSvc", name),                 //Athena magnetic field
    m_firstMeasuredPoint(false),
    m_firstMeasuredPointLimit(false),
    m_makeExtendedVertex(false),
    m_useAprioriVertex(false),
    m_useThetaCnst(false),
    m_usePhiCnst(false),
    m_usePointingCnst(false),
    m_useZPointingCnst(false),
    m_usePassNear(false),
    m_usePassWithTrkErr(false),
    m_Charge(0),
    m_Chi2(0.),
    m_cascadeSize(0),
    m_timingProfile(nullptr),
    m_ErrMtx(nullptr)
   {
    declareInterface<IVertexFitter>(this);
    declareInterface<ITrkVKalVrtFitter>(this);
    declareInterface<IVertexCascadeFitter>(this);
    


    m_PosTrack0Charge.clear();
    m_MassInputParticles.clear();
    
    m_BMAG    = 1.997;      /*constant ATLAS magnetic field if no exact map*/     
    m_CNVMAG  = 0.29979246;  /* conversion constant for MeV and MM */

    m_c_VertexForConstraint.clear();
    for( int i=0; i<3; i++){m_c_VertexForConstraint.push_back(0.);}
    m_c_CovVrtForConstraint.clear();
    for( int i=0; i<6; i++){m_c_CovVrtForConstraint.push_back(0.);}
    m_c_MassInputParticles.clear();
    m_c_TrackCharge.clear();
    m_ApproximateVertex.clear();
    m_ApproximateVertex.reserve(3);
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
    declareProperty("ZeroChgTracks",        m_c_TrackCharge, "Numbers of neutral tracks in input set (numbering from zero)");
    declareProperty("Extrapolator",         m_extPropagator);
    declareProperty("AtlasMagFieldSvc",     m_magFieldAthenaSvc);
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
    m_iflag=0;
    m_ifcovv0=0;
    m_FitStatus=0;
    m_refFrameX = 0.;   
    m_refFrameY = 0.;
    m_refFrameZ = 0.;
    m_globalFirstHit = 0;
    m_planeCnstNDOF = 0;
    m_vkalFitControl = 0;
                               
/*--------------------------------------------------------------------------*/
/*  New magnetic field object is created. It's provided to VKalVrtCore.     */
/*  VKalVrtFitter must set up Core BEFORE any call required propagation!!!  */  
/*           myMagFld is a static oblect in VKalVrtCore                     */ 

    m_fitField = new VKalAtlasMagFld();
    m_fitField->setAtlasField(m_BMAG);

/*--------------------------------------------------------------------------*/
/*  New propagator object is created. It's provided to VKalVrtCore.         */
/*  VKalVrtFitter must set up Core BEFORE any call required propagation!!!  */  
/*  This object is created ONLY if IExtrapolator pointer is provideded.     */
/*         see VKalExtPropagator.cxx for details                            */
/*--------------------------------------------------------------------------*/
    m_fitPropagator = 0;       //Pointer to VKalVrtFitter propagator object to supply to VKalVrtCore (specific interface) 
    m_InDetExtrapolator = 0;   //Direct pointer to Athena propagator

    m_isAtlasField       = false;   // To allow callback and then field first call only at execute stage
    m_isFieldInitialized = false;   //
}


//Destructor---------------------------------------------------------------
TrkVKalVrtFitter::~TrkVKalVrtFitter(){
    //log << MSG::DEBUG << "TrkVKalVrtFitter destructor called" << endmsg;
    if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<"TrkVKalVrtFitter destructor called" << endmsg;
    delete m_fitField;
    if(m_fitPropagator) delete m_fitPropagator;
    if(m_ErrMtx)delete[] m_ErrMtx;
    if(m_vkalFitControl) delete m_vkalFitControl;
}



StatusCode TrkVKalVrtFitter::finalize()
{
    if(m_ErrMtx)delete[] m_ErrMtx;
    m_ErrMtx=0;
    if(m_timingProfile)m_timingProfile->chronoPrint("Trk_VKalVrtFitter");
    if(msgLvl(MSG::INFO))msg(MSG::INFO)<<"TrkVKalVrtFitter finalize() successful" << endmsg;
    return StatusCode::SUCCESS;
}


void TrkVKalVrtFitter::setInitializedField() {
  if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Execute() time magnetic field setting is called" << endmsg;
  if(m_isAtlasField){
      MagField::IMagFieldSvc* mtmp =   & (*m_magFieldAthenaSvc);
      setAthenaField(mtmp);
      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "MagFieldAthenaSvc is initialized and used" << endmsg;  
  }
  m_isFieldInitialized = true;   //  to signal end of mag.field init procedure 
  return;
}




StatusCode TrkVKalVrtFitter::initialize()
{
    
// Checking ROBUST algoritms
    if(m_Robustness<0 || m_Robustness>7 ) m_Robustness=0; 

// Copy input parameters to internal structures
    int nItr=m_c_CovVrtForConstraint.size(); if(nItr>6)nItr=6;
    m_CovVrtForConstraint.resize(6);  for(int itr=0; itr<nItr; itr++)m_CovVrtForConstraint[itr]=m_c_CovVrtForConstraint[itr];
        nItr=m_c_VertexForConstraint.size(); if(nItr>3)nItr=3;
    m_VertexForConstraint.resize(3);  for(int itr=0; itr<nItr; itr++)m_VertexForConstraint[itr]=m_c_VertexForConstraint[itr];
        nItr=m_c_MassInputParticles.size();
    m_MassInputParticles.resize(nItr);for(int itr=0; itr<nItr; itr++)m_MassInputParticles[itr] =m_c_MassInputParticles[itr];
        nItr=m_c_TrackCharge.size();
    m_TrackCharge.resize(nItr);       for(int itr=0; itr<nItr; itr++)m_TrackCharge[itr]        =m_c_TrackCharge[itr];


// Setting constraint type - not used anymore, left for old code reference here....
//    if( m_Constraint == 2)  m_usePointingCnst   = true;    
//    if( m_Constraint == 3)  m_useZPointingCnst  = true;    
//    if( m_Constraint == 4)  m_usePointingCnst   = true;    
//    if( m_Constraint == 5)  m_useZPointingCnst  = true;    
//    if( m_Constraint == 6)  m_useAprioriVertex  = true;    
//    if( m_Constraint == 7)  m_usePassWithTrkErr = true;    
//    if( m_Constraint == 8)  m_usePassWithTrkErr = true;    
//    if( m_Constraint == 9)  m_usePassNear       = true;    
//    if( m_Constraint == 10) m_usePassNear       = true;    
//    if( m_Constraint == 11) m_usePhiCnst = true;    
//    if( m_Constraint == 12) { m_usePhiCnst = true; m_useThetaCnst = true;}
//    setCnstType((int)m_Constraint);

    StatusCode sc=m_magFieldAthenaSvc.retrieve();
    if (sc.isFailure() ){
        if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<"Could not find MagFieldAthenaSvc"<< endmsg;
    }else{
	m_isAtlasField = true;
        if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "MagFieldAthenaSvc is retrieved" << endmsg;  
    }
//
// Only here the VKalVrtFitter propagator object is created if ATHENA propagator is provided (see setAthenaPropagator)
// In this case the ATHENA propagator can be used via pointers:
//     m_InDetExtrapolator - direct access
//     m_fitPropagator     - via VKalVrtFitter object VKalExtPropagator
// If ATHENA propagator is not provided, only defined object is
//     myPropagator      - extern propagator from TrkVKalVrtCore
//
    if (m_extPropagator.name() == "DefaultVKalPropagator" ){
      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "External propagator is not supplied - use internal one"<<endmsg;
      m_extPropagator.disable();
    }else{
      if (m_extPropagator.retrieve().isFailure()) {
        if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "Could not find external propagator=" <<m_extPropagator<<endmsg;
        if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "TrkVKalVrtFitter will uses internal propagator" << endmsg;
        m_extPropagator.disable();
      }else{
        if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "External propagator="<<m_extPropagator<<" retrieved" << endmsg;
        const IExtrapolator * tmp =& (*m_extPropagator);
        setAthenaPropagator(tmp); 
      }
    }
//
//---Set Control object for TrkVKalVrtCore with constraints defined in jobO
//
    m_vkalFitControl = new VKalVrtControl(VKalVrtControlBase(m_fitField,0,m_fitPropagator,0));  // Create main control object for TrkVKalVrtCore

    for(int it=0; it<(int)m_MassInputParticles.size(); it++) m_vkalFitControl->m_forcft.wm[it]=m_MassInputParticles[it]; //jobO track masses
    m_vkalFitControl->setRobustness(m_Robustness);
    m_vkalFitControl->setRobustScale(m_RobustScale);
    for(int it=0; it<(int)m_VertexForConstraint.size(); it++) m_vkalFitControl->m_forcft.vrt[it]=m_VertexForConstraint[it]; //jobO vertex for cnst
    for(int it=0; it<(int)m_CovVrtForConstraint.size(); it++) m_vkalFitControl->m_forcft.covvrt[it]=m_CovVrtForConstraint[it]; //jobO vertex covariance
    if(m_massForConstraint>0.) m_vkalFitControl->setMassCnstData(m_MassInputParticles.size(),m_massForConstraint); // configure general mass constraint
    if(m_IterationPrecision>0.)m_vkalFitControl->setIterationPrec(m_IterationPrecision);
    if(m_IterationNumber)  m_vkalFitControl->setIterationNum(m_IterationNumber);
    if(m_useAprioriVertex) m_vkalFitControl->setUseAprioriVrt();
    if(m_useThetaCnst)     m_vkalFitControl->setUseThetaCnst();
    if(m_usePhiCnst)       m_vkalFitControl->setUsePhiCnst();
    if(m_usePointingCnst)  m_vkalFitControl->setUsePointingCnst(1);
    if(m_useZPointingCnst) m_vkalFitControl->setUsePointingCnst(2);
    if(m_usePassNear)      m_vkalFitControl->setUsePassNear(1);
    if(m_usePassWithTrkErr)m_vkalFitControl->setUsePassNear(2);
//
//
    m_timingProfile=0;
    sc = service("ChronoStatSvc", m_timingProfile);
    if ( sc.isFailure() || 0 == m_timingProfile) {
      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<"Can not find ChronoStatSvc name="<<m_timingProfile << endmsg;
    }


//
    m_ErrMtx=0;            // pointer to double array for error matrix
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
         for(int i=0; i<(int)m_MassInputParticles.size(); i++) msg(MSG::DEBUG)<<m_MassInputParticles[i]<<", ";
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
 





   /** Interface for Track with starting point */

xAOD::Vertex * TrkVKalVrtFitter::fit(const std::vector<const Track*> & vectorTrk,
                                    const Amg::Vector3D & firstStartingPoint)
{
    m_globalFirstHit = 0;
    setApproximateVertex(firstStartingPoint.x(),
                         firstStartingPoint.y(),
                         firstStartingPoint.z());
    setMomCovCalc(1);
    StatusCode sc=VKalVrtFit( vectorTrk,
               m_Vertex, m_Momentum, m_Charge, m_ErrorMatrix, m_Chi2PerTrk, m_TrkAtVrt, m_Chi2 );

    xAOD::Vertex * tmpVertex = 0;
    if(sc.isSuccess()) {
      tmpVertex = makeXAODVertex( 0, m_Vertex, m_ErrorMatrix, m_Chi2PerTrk, m_TrkAtVrt, m_Chi2 );
      std::vector<VxTrackAtVertex> & vxavList=tmpVertex->vxTrackAtVertex();
      for(int it=0; it<(int)vxavList.size(); it++){
          LinkToTrack * linkTT = new LinkToTrack();
          linkTT->setElement( vectorTrk.at(it) );
          vxavList[it].setOrigTrack(linkTT);           //pointer to initial Track
      }
    }
    if(m_ErrMtx)delete[] m_ErrMtx;
    m_ErrMtx=0;
    m_planeCnstNDOF = 0;       // No plane constraint by default
    setMomCovCalc(0);          // No full covariance by default
    return tmpVertex;

}




xAOD::Vertex * TrkVKalVrtFitter::fit(const std::vector<const TrackParticleBase*> & vectorTrk,
                                    const Amg::Vector3D & firstStartingPoint)
{
    m_globalFirstHit = 0;
    setApproximateVertex(firstStartingPoint.x(),
                         firstStartingPoint.y(),
                         firstStartingPoint.z());
    setMomCovCalc(1);
    StatusCode sc=VKalVrtFit( vectorTrk,
               m_Vertex, m_Momentum, m_Charge, m_ErrorMatrix, m_Chi2PerTrk, m_TrkAtVrt, m_Chi2 );

    xAOD::Vertex * tmpVertex = 0;
    if(sc.isSuccess()) {
      tmpVertex = makeXAODVertex( 0, m_Vertex, m_ErrorMatrix, m_Chi2PerTrk, m_TrkAtVrt, m_Chi2 );
      std::vector<VxTrackAtVertex> & vxavList=tmpVertex->vxTrackAtVertex();
      for(int it=0; it<(int)vxavList.size(); it++){
        ElementLink<TrackParticleBaseCollection> TEL;  TEL.setElement( vectorTrk.at(it) );
        LinkToTrackParticleBase * ITL = new LinkToTrackParticleBase(TEL); 
        vxavList[it].setOrigTrack(ITL);              //pointer to initial TrackParticle(Base)
      }
    }
    if(m_ErrMtx)delete[] m_ErrMtx;
    m_ErrMtx=0;
    m_planeCnstNDOF = 0;       // No plane constraint by default
    setMomCovCalc(0);          // No full covariance by default
    return tmpVertex;

}







    /** Interface for Track with vertex constraint */
    /** the position of the constraint is ALWAYS the starting point */
 

xAOD::Vertex * TrkVKalVrtFitter::fit(const std::vector<const Track*>& vectorTrk,
                                                 const xAOD::Vertex& firstStartingPoint)
{   
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<< "A priori vertex constraint is added to VKalVrt fitter!" << endmsg;
//    m_fitSvc->setDefault();
    m_globalFirstHit = 0;
    Amg::Vector3D VertexIni(0.,0.,0.);
    StatusCode sc=VKalVrtFitFast(vectorTrk, VertexIni); 
    if( sc.isSuccess()){
       setApproximateVertex(VertexIni.x(),VertexIni.y(),VertexIni.z());
    }else{
       setApproximateVertex(firstStartingPoint.position().x(),
                            firstStartingPoint.position().y(),
                            firstStartingPoint.position().z());
    }
    setVertexForConstraint(firstStartingPoint.position().x(),
                           firstStartingPoint.position().y(),
                           firstStartingPoint.position().z());
    setCovVrtForConstraint(firstStartingPoint.covariancePosition()(Trk::x,Trk::x),
                           firstStartingPoint.covariancePosition()(Trk::y,Trk::x),
                           firstStartingPoint.covariancePosition()(Trk::y,Trk::y),
                           firstStartingPoint.covariancePosition()(Trk::z,Trk::x),
                           firstStartingPoint.covariancePosition()(Trk::z,Trk::y),
                           firstStartingPoint.covariancePosition()(Trk::z,Trk::z) );
    //setCnstType(6);
    m_useAprioriVertex=true;
    setMomCovCalc(1);
    sc=VKalVrtFit( vectorTrk,
               m_Vertex, m_Momentum, m_Charge, m_ErrorMatrix, m_Chi2PerTrk, m_TrkAtVrt, m_Chi2 );

    xAOD::Vertex * tmpVertex = 0;
    if(sc.isSuccess()) {
      tmpVertex = makeXAODVertex( 0, m_Vertex, m_ErrorMatrix, m_Chi2PerTrk, m_TrkAtVrt, m_Chi2 );
      std::vector<VxTrackAtVertex> & vxavList=tmpVertex->vxTrackAtVertex();
      for(int it=0; it<(int)vxavList.size(); it++){
          LinkToTrack * linkTT = new LinkToTrack();
          linkTT->setElement( vectorTrk.at(it) );
          vxavList[it].setOrigTrack(linkTT);           //pointer to initial Track
      }
    }
    if(m_ErrMtx)delete[] m_ErrMtx;
    m_ErrMtx=0;
    m_useAprioriVertex=false;
    m_planeCnstNDOF = 0;       // No plane constraint by default
    setMomCovCalc(0);          // No full covariance by default
    return tmpVertex;

//    return  new VxCandidate(*m_tmpRecV,*m_tmpVTAV);
}
xAOD::Vertex * TrkVKalVrtFitter::fit(const std::vector<const TrackParticleBase*>& vectorTrk,
                                    const xAOD::Vertex & firstStartingPoint)
{   
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<< "A priori vertex constraint is added to VKalVrt fitter!" << endmsg;
//    m_fitSvc->setDefault();
    m_globalFirstHit = 0;
    Amg::Vector3D VertexIni(0.,0.,0.);
    StatusCode sc=VKalVrtFitFast(vectorTrk, VertexIni); 
    if( sc.isSuccess()){
       setApproximateVertex(VertexIni.x(),VertexIni.y(),VertexIni.z());
    }else{
       setApproximateVertex(firstStartingPoint.position().x(),
                            firstStartingPoint.position().y(),
                            firstStartingPoint.position().z());
    }
    setVertexForConstraint(firstStartingPoint.position().x(),
                           firstStartingPoint.position().y(),
                           firstStartingPoint.position().z());
    setCovVrtForConstraint(firstStartingPoint.covariancePosition()(Trk::x,Trk::x),
                           firstStartingPoint.covariancePosition()(Trk::y,Trk::x),
                           firstStartingPoint.covariancePosition()(Trk::y,Trk::y),
                           firstStartingPoint.covariancePosition()(Trk::z,Trk::x),
                           firstStartingPoint.covariancePosition()(Trk::z,Trk::y),
                           firstStartingPoint.covariancePosition()(Trk::z,Trk::z) );
    //setCnstType(6);
    m_useAprioriVertex=true;
    setMomCovCalc(1);
    sc=VKalVrtFit( vectorTrk,
               m_Vertex, m_Momentum, m_Charge, m_ErrorMatrix, m_Chi2PerTrk, m_TrkAtVrt, m_Chi2 );

    xAOD::Vertex * tmpVertex = 0;
    if(sc.isSuccess()) {
      tmpVertex = makeXAODVertex( 0, m_Vertex, m_ErrorMatrix, m_Chi2PerTrk, m_TrkAtVrt, m_Chi2 );
      std::vector<VxTrackAtVertex> & vxavList=tmpVertex->vxTrackAtVertex();
      for(int it=0; it<(int)vxavList.size(); it++){
        ElementLink<TrackParticleBaseCollection> TEL;  TEL.setElement( vectorTrk.at(it) );
        LinkToTrackParticleBase * ITL = new LinkToTrackParticleBase(TEL); 
        vxavList[it].setOrigTrack(ITL);              //pointer to initial TrackParticle(Base)
      }
    }
    if(m_ErrMtx)delete[] m_ErrMtx;
    m_ErrMtx=0;
    m_useAprioriVertex=false;
    m_planeCnstNDOF = 0;       // No plane constraint by default
    setMomCovCalc(0);          // No full covariance by default
    return tmpVertex;

}




/*  VxCandidate * TrkVKalVrtFitter::fit(const vector<const ParametersBase*> & perigeeList,
                                      const Vertex& startingPoint)
{
    int NTrk=perigeeList.size();
    const Trk::FitQuality* fitQuality = new Trk::FitQuality(10.,1);
    std::vector<const Trk::Track*>  trackList;
    for(int i=0; i<NTrk; i++) {
       const Trk::Perigee* perigee = dynamic_cast<const Trk::Perigee*> (perigeeList[i]);
       DataVector<const TrackStateOnSurface>* trkStOnSurf=new DataVector<const TrackStateOnSurface>;
       const TrackStateOnSurface* trackSOS = new TrackStateOnSurface(0, perigee, 0,  0);
       trkStOnSurf->push_back(trackSOS);
       trackList.push_back( new Track( Track::unknown, trkStOnSurf, fitQuality) );
    }
    VxCandidate * tmp=fit( trackList, startingPoint);
    for(int i=0; i<NTrk; i++) { delete trackList[i];}  delete fitQuality;
    return tmp;
 }
VxCandidate * TrkVKalVrtFitter::fit(const vector<const ParametersBase*> & perigeeList,
                                    const RecVertex& constraint)
{
    int NTrk=perigeeList.size();
    const Trk::FitQuality* fitQuality = new Trk::FitQuality(10.,1);
    vector<const Trk::Track*>  trackList;
    for(int i=0; i<NTrk; i++) {
       const Trk::Perigee* perigee = dynamic_cast<const Trk::Perigee*> (perigeeList[i]);
       DataVector<const TrackStateOnSurface>* trkStOnSurf=new DataVector<const TrackStateOnSurface>;
       const TrackStateOnSurface* trackSOS = new TrackStateOnSurface(0, perigee, 0,  0);
       trkStOnSurf->push_back(trackSOS);
       trackList.push_back( new Track( Track::unknown, trkStOnSurf, fitQuality) );
    }
    VxCandidate * tmp=fit( trackList, constraint);
    for(int i=0; i<NTrk; i++) { delete trackList[i];}  delete fitQuality;
    return tmp;
}*/

     /** Interface for MeasuredPerigee with starting point */
xAOD::Vertex * TrkVKalVrtFitter::fit(const std::vector<const TrackParameters*> & perigeeListC,
                                    const Amg::Vector3D & startingPoint)
{
    m_globalFirstHit = 0;
    setApproximateVertex(startingPoint.x(),
                         startingPoint.y(),
                         startingPoint.z());
    setMomCovCalc(1);
    std::vector<const NeutralParameters*> perigeeListN(0);
    StatusCode sc=VKalVrtFit( perigeeListC, perigeeListN,
               m_Vertex, m_Momentum, m_Charge, m_ErrorMatrix, m_Chi2PerTrk, m_TrkAtVrt, m_Chi2 );

    xAOD::Vertex * tmpVertex = 0;
    if(sc.isSuccess()) {
      tmpVertex = makeXAODVertex( 0, m_Vertex, m_ErrorMatrix, m_Chi2PerTrk, m_TrkAtVrt, m_Chi2 );
    }
    if(m_ErrMtx)delete[] m_ErrMtx;
    m_ErrMtx=0;
    m_planeCnstNDOF = 0;       // No plane constraint by default
    setMomCovCalc(0);          // No full covariance by default
    return tmpVertex;
}


xAOD::Vertex * TrkVKalVrtFitter::fit(const std::vector<const TrackParameters*>   & perigeeListC,
                                    const std::vector<const NeutralParameters*> & perigeeListN,
                                    const Amg::Vector3D & startingPoint)
{
    m_globalFirstHit = 0;
    setApproximateVertex(startingPoint.x(),
                         startingPoint.y(),
                         startingPoint.z());
    setMomCovCalc(1);
    StatusCode sc=VKalVrtFit( perigeeListC,perigeeListN,
               m_Vertex, m_Momentum, m_Charge, m_ErrorMatrix, m_Chi2PerTrk, m_TrkAtVrt, m_Chi2 );

    xAOD::Vertex * tmpVertex = 0;
    if(sc.isSuccess()) {
      tmpVertex = makeXAODVertex( (int)perigeeListN.size() , m_Vertex, m_ErrorMatrix, m_Chi2PerTrk, m_TrkAtVrt, m_Chi2 );
    }
    if(m_ErrMtx)delete[] m_ErrMtx;
    m_ErrMtx=0;
    m_planeCnstNDOF = 0;       // No plane constraint by default
    setMomCovCalc(0);          // No full covariance by default
    return tmpVertex;
}






     /** Interface for MeasuredPerigee with vertex constraint */
     /** the position of the constraint is ALWAYS the starting point */
xAOD::Vertex * TrkVKalVrtFitter::fit(const std::vector<const TrackParameters*> & perigeeListC,
                                    const xAOD::Vertex & constraint)
{
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<< "A priori vertex constraint is activated in VKalVrt fitter!" << endmsg;
    m_globalFirstHit = 0;
    Amg::Vector3D VertexIni(0.,0.,0.);
    StatusCode sc=VKalVrtFitFast(perigeeListC, VertexIni); 
    if( sc.isSuccess()){
       setApproximateVertex(VertexIni.x(),VertexIni.y(),VertexIni.z());
    }else{
       setApproximateVertex(constraint.position().x(),
                            constraint.position().y(),
                            constraint.position().z());
    }
    setVertexForConstraint(constraint.position().x(),
                           constraint.position().y(),
                           constraint.position().z());
    setCovVrtForConstraint(constraint.covariancePosition()(Trk::x,Trk::x),
                           constraint.covariancePosition()(Trk::y,Trk::x),
                           constraint.covariancePosition()(Trk::y,Trk::y),
                           constraint.covariancePosition()(Trk::z,Trk::x),
                           constraint.covariancePosition()(Trk::z,Trk::y),
                           constraint.covariancePosition()(Trk::z,Trk::z) );
    m_useAprioriVertex=true;
    setMomCovCalc(1);
    std::vector<const NeutralParameters*> perigeeListN(0);
    sc=VKalVrtFit( perigeeListC, perigeeListN,
               m_Vertex, m_Momentum, m_Charge, m_ErrorMatrix, m_Chi2PerTrk, m_TrkAtVrt, m_Chi2 );


    xAOD::Vertex * tmpVertex = 0;
    if(sc.isSuccess()) {
      tmpVertex = makeXAODVertex( 0, m_Vertex, m_ErrorMatrix, m_Chi2PerTrk, m_TrkAtVrt, m_Chi2 );
    }
    if(m_ErrMtx)delete[] m_ErrMtx;
    m_ErrMtx=0;
    m_useAprioriVertex=false;
    m_planeCnstNDOF = 0;       // No plane constraint by default
    setMomCovCalc(0);          // No full covariance by default
    return tmpVertex;
}


xAOD::Vertex * TrkVKalVrtFitter::fit(const std::vector<const TrackParameters*>   & perigeeListC,
                                    const std::vector<const NeutralParameters*> & perigeeListN,
                                    const xAOD::Vertex & constraint)
{
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<< "A priori vertex constraint is activated in VKalVrt fitter!" << endmsg;
    m_globalFirstHit = 0;
    Amg::Vector3D VertexIni(0.,0.,0.);
    StatusCode sc=VKalVrtFitFast(perigeeListC, VertexIni); 
    if( sc.isSuccess()){
       setApproximateVertex(VertexIni.x(),VertexIni.y(),VertexIni.z());
    }else{
       setApproximateVertex(constraint.position().x(),
                            constraint.position().y(),
                            constraint.position().z());
    }
    setVertexForConstraint(constraint.position().x(),
                           constraint.position().y(),
                           constraint.position().z());
    setCovVrtForConstraint(constraint.covariancePosition()(Trk::x,Trk::x),
                           constraint.covariancePosition()(Trk::y,Trk::x),
                           constraint.covariancePosition()(Trk::y,Trk::y),
                           constraint.covariancePosition()(Trk::z,Trk::x),
                           constraint.covariancePosition()(Trk::z,Trk::y),
                           constraint.covariancePosition()(Trk::z,Trk::z) );
    m_useAprioriVertex=true;
    setMomCovCalc(1);
    sc=VKalVrtFit( perigeeListC, perigeeListN,
               m_Vertex, m_Momentum, m_Charge, m_ErrorMatrix, m_Chi2PerTrk, m_TrkAtVrt, m_Chi2 );


    xAOD::Vertex * tmpVertex = 0;
    if(sc.isSuccess()) {
      tmpVertex = makeXAODVertex( (int)perigeeListN.size(), m_Vertex, m_ErrorMatrix, m_Chi2PerTrk, m_TrkAtVrt, m_Chi2 );
    }
    if(m_ErrMtx)delete[] m_ErrMtx;
    m_ErrMtx=0;
    m_useAprioriVertex=false;
    m_planeCnstNDOF = 0;       // No plane constraint by default
    setMomCovCalc(0);          // No full covariance by default
    return tmpVertex;
}



     /** Interface for xAOD::TrackParticle with starting point */
xAOD::Vertex * TrkVKalVrtFitter::fit(const std::vector<const xAOD::TrackParticle*> & xtpListC,
                                     const Amg::Vector3D & startingPoint)
{
    m_globalFirstHit = 0;
    xAOD::Vertex * tmpVertex = 0;
    setApproximateVertex(startingPoint.x(),
                         startingPoint.y(),
                         startingPoint.z());
    setMomCovCalc(1);
    std::vector<const xAOD::NeutralParticle*> xtpListN(0);
    StatusCode sc=VKalVrtFit( xtpListC, xtpListN,
               m_Vertex, m_Momentum, m_Charge, m_ErrorMatrix, m_Chi2PerTrk, m_TrkAtVrt, m_Chi2 );
    if(sc.isSuccess()) {
       tmpVertex = makeXAODVertex( 0, m_Vertex, m_ErrorMatrix, m_Chi2PerTrk, m_TrkAtVrt, m_Chi2 );
       dvect fittrkwgt;  
       sc=VKalGetTrkWeights(fittrkwgt); if(sc.isFailure())fittrkwgt.clear();
       for(int ii=0; ii<m_FitStatus; ii++) {
          ElementLink<xAOD::TrackParticleContainer> TEL;  TEL.setElement( xtpListC[ii] );
          if(fittrkwgt.size())  tmpVertex->addTrackAtVertex(TEL,fittrkwgt[ii]);
          else                    tmpVertex->addTrackAtVertex(TEL,1.);
       }
    }

    if(m_ErrMtx)delete[] m_ErrMtx;
    m_ErrMtx=0;
    m_planeCnstNDOF = 0;       // No plane constraint by default
    setMomCovCalc(0);          // No full covariance by default

    return tmpVertex;
}

xAOD::Vertex * TrkVKalVrtFitter::fit(const std::vector<const xAOD::TrackParticle*>   & xtpListC,
                                     const std::vector<const xAOD::NeutralParticle*> & xtpListN,
                                     const Amg::Vector3D & startingPoint)
{
    m_globalFirstHit = 0;
    xAOD::Vertex * tmpVertex = 0;
    setApproximateVertex(startingPoint.x(),
                         startingPoint.y(),
                         startingPoint.z());
    setMomCovCalc(1);
    StatusCode sc=VKalVrtFit( xtpListC, xtpListN,
               m_Vertex, m_Momentum, m_Charge, m_ErrorMatrix, m_Chi2PerTrk, m_TrkAtVrt, m_Chi2 );
    if(sc.isSuccess()) {
       tmpVertex = makeXAODVertex( (int)xtpListN.size(), m_Vertex, m_ErrorMatrix, m_Chi2PerTrk, m_TrkAtVrt, m_Chi2 );
       dvect fittrkwgt;  
       sc=VKalGetTrkWeights(fittrkwgt); if(sc.isFailure())fittrkwgt.clear();
       for(int ii=0; ii<m_FitStatus; ii++) {
          if(ii<(int)xtpListC.size()) {
             ElementLink<xAOD::TrackParticleContainer> TEL;  TEL.setElement( xtpListC[ii] );
             if(fittrkwgt.size())  tmpVertex->addTrackAtVertex(TEL,fittrkwgt[ii]);
             else                    tmpVertex->addTrackAtVertex(TEL,1.);
          }else{
             ElementLink<xAOD::NeutralParticleContainer> TEL;  TEL.setElement( xtpListN[ii] );
             if(fittrkwgt.size())  tmpVertex->addNeutralAtVertex(TEL,fittrkwgt[ii]);
             else                    tmpVertex->addNeutralAtVertex(TEL,1.);
          }
       }
    }

    if(m_ErrMtx)delete[] m_ErrMtx;
    m_ErrMtx=0;
    m_planeCnstNDOF = 0;       // No plane constraint by default
    setMomCovCalc(0);          // No full covariance by default

    return tmpVertex;
}

     /** Interface for xAOD::TrackParticle with vertex constraint */
     /** the position of the constraint is ALWAYS the starting point */
xAOD::Vertex * TrkVKalVrtFitter::fit(const std::vector<const xAOD::TrackParticle*> & xtpListC,
                                     const xAOD::Vertex & constraint)
{
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<< "A priori vertex constraint is activated in VKalVrt fitter!" << endmsg;
    m_globalFirstHit = 0;
    xAOD::Vertex * tmpVertex = 0;
    setApproximateVertex(constraint.position().x(), constraint.position().y(),constraint.position().z());
    setVertexForConstraint(constraint.position().x(),
                           constraint.position().y(),
                           constraint.position().z());
    setCovVrtForConstraint(constraint.covariancePosition()(Trk::x,Trk::x),
                           constraint.covariancePosition()(Trk::y,Trk::x),
                           constraint.covariancePosition()(Trk::y,Trk::y),
                           constraint.covariancePosition()(Trk::z,Trk::x),
                           constraint.covariancePosition()(Trk::z,Trk::y),
                           constraint.covariancePosition()(Trk::z,Trk::z) );
    m_useAprioriVertex=true;
    setMomCovCalc(1);
    std::vector<const xAOD::NeutralParticle*> xtpListN(0);
    StatusCode sc=VKalVrtFit( xtpListC, xtpListN,
               m_Vertex, m_Momentum, m_Charge, m_ErrorMatrix, m_Chi2PerTrk, m_TrkAtVrt, m_Chi2 );
    if(sc.isSuccess()) {
       tmpVertex = makeXAODVertex( 0, m_Vertex, m_ErrorMatrix,m_Chi2PerTrk, m_TrkAtVrt, m_Chi2 );
       dvect fittrkwgt;  
       sc=VKalGetTrkWeights(fittrkwgt); if(sc.isFailure())fittrkwgt.clear();
       for(int ii=0; ii<m_FitStatus; ii++) {
          ElementLink<xAOD::TrackParticleContainer> TEL;  TEL.setElement( xtpListC[ii] );
          if(fittrkwgt.size())  tmpVertex->addTrackAtVertex(TEL,fittrkwgt[ii]);
          else                    tmpVertex->addTrackAtVertex(TEL,1.);
       }
    }

    if(m_ErrMtx)delete[] m_ErrMtx;
    m_ErrMtx=0;
    m_useAprioriVertex=false;
    m_planeCnstNDOF = 0;       // No plane constraint by default
    setMomCovCalc(0);          // No full covariance by default

    return tmpVertex;
}

xAOD::Vertex * TrkVKalVrtFitter::fit(const std::vector<const xAOD::TrackParticle*>   & xtpListC,
                                     const std::vector<const xAOD::NeutralParticle*> & xtpListN,
                                     const xAOD::Vertex & constraint)
{
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<< "A priori vertex constraint is activated in VKalVrt fitter!" << endmsg;
    m_globalFirstHit = 0;
    xAOD::Vertex * tmpVertex = 0;
    setApproximateVertex(constraint.position().x(), constraint.position().y(),constraint.position().z());
    setVertexForConstraint(constraint.position().x(),
                           constraint.position().y(),
                           constraint.position().z());
    setCovVrtForConstraint(constraint.covariancePosition()(Trk::x,Trk::x),
                           constraint.covariancePosition()(Trk::y,Trk::x),
                           constraint.covariancePosition()(Trk::y,Trk::y),
                           constraint.covariancePosition()(Trk::z,Trk::x),
                           constraint.covariancePosition()(Trk::z,Trk::y),
                           constraint.covariancePosition()(Trk::z,Trk::z) );
    m_useAprioriVertex=true;
    setMomCovCalc(1);
    StatusCode sc=VKalVrtFit( xtpListC, xtpListN,
               m_Vertex, m_Momentum, m_Charge, m_ErrorMatrix, m_Chi2PerTrk, m_TrkAtVrt, m_Chi2 );
    if(sc.isSuccess()){
       tmpVertex = makeXAODVertex( (int)xtpListN.size(), m_Vertex, m_ErrorMatrix,m_Chi2PerTrk, m_TrkAtVrt, m_Chi2 );
       dvect fittrkwgt;  
       sc=VKalGetTrkWeights(fittrkwgt); if(sc.isFailure())fittrkwgt.clear();
       for(int ii=0; ii<m_FitStatus; ii++) {
          if(ii<(int)xtpListC.size()) {
             ElementLink<xAOD::TrackParticleContainer> TEL;  TEL.setElement( xtpListC[ii] );
             if(fittrkwgt.size())  tmpVertex->addTrackAtVertex(TEL,fittrkwgt[ii]);
             else                    tmpVertex->addTrackAtVertex(TEL,1.);
          }else{
             ElementLink<xAOD::NeutralParticleContainer> TEL;  TEL.setElement( xtpListN[ii] );
             if(fittrkwgt.size())  tmpVertex->addNeutralAtVertex(TEL,fittrkwgt[ii]);
             else                    tmpVertex->addNeutralAtVertex(TEL,1.);
          }
       }
    }

    if(m_ErrMtx)delete[] m_ErrMtx;
    m_ErrMtx=0;
    m_useAprioriVertex=false;
    m_planeCnstNDOF = 0;       // No plane constraint by default
    setMomCovCalc(0);          // No full covariance by default

    return tmpVertex;
}







xAOD::Vertex * TrkVKalVrtFitter::fit(const std::vector<const Track*> & vectorTrk)
{
    Amg::Vector3D VertexIni(0.,0.,0.);
    m_globalFirstHit = 0;

    StatusCode sc=VKalVrtFitFast(vectorTrk, VertexIni); 
    if(sc.isSuccess()) {
      setApproximateVertex(VertexIni.x(),VertexIni.y(),VertexIni.z());
      setMomCovCalc(1); 
      sc=VKalVrtFit( vectorTrk,
          m_Vertex, m_Momentum, m_Charge, m_ErrorMatrix, m_Chi2PerTrk, m_TrkAtVrt, m_Chi2 );

    }
    
    xAOD::Vertex * tmpVertex = 0;
    if(sc.isSuccess()){
      tmpVertex = makeXAODVertex( 0, m_Vertex, m_ErrorMatrix, m_Chi2PerTrk, m_TrkAtVrt, m_Chi2 );
      std::vector<VxTrackAtVertex> & vxavList=tmpVertex->vxTrackAtVertex();
      for(int it=0; it<(int)vxavList.size(); it++){
          LinkToTrack * linkTT = new LinkToTrack();
          linkTT->setElement( vectorTrk.at(it) );
          vxavList[it].setOrigTrack(linkTT);           //pointer to initial Track
      }
    }
    if(m_ErrMtx)delete[] m_ErrMtx;
    m_ErrMtx=0;
    setMomCovCalc(0);          // No full covariance by default
    return tmpVertex;
}

xAOD::Vertex * TrkVKalVrtFitter::fit(const std::vector<const  TrackParameters*> & perigeeListC)
{
    m_globalFirstHit = 0;
    Amg::Vector3D VertexIni(0.,0.,0.);
    StatusCode sc=VKalVrtFitFast(perigeeListC, VertexIni); 
    if( sc.isSuccess()) setApproximateVertex(VertexIni.x(),VertexIni.y(),VertexIni.z());
    setMomCovCalc(1);
    std::vector<const NeutralParameters*> perigeeListN(0);
    sc=VKalVrtFit( perigeeListC, perigeeListN,
               m_Vertex, m_Momentum, m_Charge, m_ErrorMatrix, m_Chi2PerTrk, m_TrkAtVrt, m_Chi2 );

    xAOD::Vertex * tmpVertex = 0;
    if(sc.isSuccess()) {
       tmpVertex = makeXAODVertex( 0, m_Vertex, m_ErrorMatrix, m_Chi2PerTrk, m_TrkAtVrt, m_Chi2 );
    }
    if(m_ErrMtx)delete[] m_ErrMtx;
    m_ErrMtx=0;
    m_planeCnstNDOF = 0;       // No plane constraint by default
    setMomCovCalc(0);          // No full covariance by default
    return tmpVertex;
}

xAOD::Vertex * TrkVKalVrtFitter::fit(const std::vector<const  TrackParameters*>   & perigeeListC,
                                    const std::vector<const  NeutralParameters*> & perigeeListN)
{
    m_globalFirstHit = 0;
    Amg::Vector3D VertexIni(0.,0.,0.);
    StatusCode sc=VKalVrtFitFast(perigeeListC, VertexIni); 
    if( sc.isSuccess()) setApproximateVertex(VertexIni.x(),VertexIni.y(),VertexIni.z());
    setMomCovCalc(1);
    sc=VKalVrtFit( perigeeListC, perigeeListN,
               m_Vertex, m_Momentum, m_Charge, m_ErrorMatrix, m_Chi2PerTrk, m_TrkAtVrt, m_Chi2 );

    xAOD::Vertex * tmpVertex = 0;
    if(sc.isSuccess()) {
       tmpVertex = makeXAODVertex( (int)perigeeListN.size(), m_Vertex, m_ErrorMatrix, m_Chi2PerTrk, m_TrkAtVrt, m_Chi2 );
    }
    if(m_ErrMtx)delete[] m_ErrMtx;
    m_ErrMtx=0;
    m_planeCnstNDOF = 0;       // No plane constraint by default
    setMomCovCalc(0);          // No full covariance by default
    return tmpVertex;
}



/* Filling of 3x3 HepSymMatrix with content of symmetric matrix 
   in packed form vector<double> (6x6 - 21 elem) 
            (VxVyVzPxPyPz)                                      */

// Fills 5x5 matrix. Input Matrix is track covariance only.
void TrkVKalVrtFitter::FillMatrixP(AmgSymMatrix(5)& CovMtx, std::vector<double> & Matrix)
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
void TrkVKalVrtFitter::FillMatrixP(int iTrk, AmgSymMatrix(5)& CovMtx, std::vector<double> & Matrix)
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



Amg::MatrixX * TrkVKalVrtFitter::GiveFullMatrix(int NTrk, std::vector<double> & Matrix)
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


void TrkVKalVrtFitter::clearMemory()
{
  if(m_ErrMtx) {
    delete[]m_ErrMtx;
    m_ErrMtx=0;
  }

  std::vector<double>().swap (m_ErrorMatrix);
  std::vector<double>().swap (m_Chi2PerTrk);
  std::vector< std::vector<double> >().swap (m_TrkAtVrt);
  std::vector < TrkMatControl >().swap (m_trkControl);
  return;
}


/*-------------  End of VxCandidate lifetime
VxCandidate * TrkVKalVrtFitter::makeVxCandidate( int Neutrals,
        const Amg::Vector3D& Vertex, const std::vector<double> & fitErrorMatrix, 
	const std::vector<double> & Chi2PerTrk,  const std::vector< std::vector<double> >& TrkAtVrt,
	double Chi2 ) 
{
    long int NTrk = m_FitStatus;
    long int Ndf = VKalGetNDOF()+m_planeCnstNDOF;
    AmgSymMatrix(3) CovMtxV;  
    std::vector<VxTrackAtVertex*> * tmpVTAV;
    RecVertex                     * tmpRecV; 

    CovMtxV(0,0)                = fitErrorMatrix[0];
    CovMtxV(1,0) = CovMtxV(0,1) = fitErrorMatrix[1]; 
    CovMtxV(1,1)                = fitErrorMatrix[2];
    CovMtxV(2,0) = CovMtxV(0,2) = fitErrorMatrix[3]; 
    CovMtxV(2,1) = CovMtxV(1,2) = fitErrorMatrix[4]; 
    CovMtxV(2,2)                = fitErrorMatrix[5];

    std::vector <double> CovFull;
    StatusCode sc = VKalGetFullCov( NTrk, CovFull); 
    int covarExist=0; if( sc.isSuccess() ) covarExist=1;
    tmpVTAV = new std::vector<VxTrackAtVertex*>();
    tmpRecV = new RecVertex( Vertex, CovMtxV, Ndf, Chi2 );
    for(int ii=0; ii<NTrk ; ii++) {
      AmgSymMatrix(5) *CovMtxP=new AmgSymMatrix(5);
      if(covarExist){ FillMatrixP( ii, (*CovMtxP), CovFull );}
      else          { (*CovMtxP).setIdentity();}
      Perigee *        tmpChargPer=0;
      NeutralPerigee * tmpNeutrPer=0;
      if(ii<NTrk-Neutrals){
        tmpChargPer  =  new Perigee( 0.,0., TrkAtVrt[ii][0],
	                                                TrkAtVrt[ii][1],
							TrkAtVrt[ii][2],
							PerigeeSurface(Vertex),
					                    CovMtxP );
      }else{
        tmpNeutrPer  =  new NeutralPerigee( 0.,0., TrkAtVrt[ii][0],
	                                           TrkAtVrt[ii][1],
						   TrkAtVrt[ii][2],
						    PerigeeSurface(Vertex),
					                    CovMtxP );
      }
      VxTrackAtVertex* trkV = new VxTrackAtVertex(Chi2PerTrk[ii], tmpChargPer, tmpNeutrPer);
      tmpVTAV->push_back(trkV);
    }
    VxCandidate * tmpVertex;
    if( m_makeExtendedVertex && covarExist ){
       tmpVertex = new ExtendedVxCandidate(*tmpRecV,*tmpVTAV, 
	           GiveFullMatrix(NTrk,CovFull) );
	           ///new ErrorMatrix(new CovarianceMatrix(SetFullMatrix(NTrk,CovFull)))  ); //VK -Old version
    }else{
       tmpVertex = new VxCandidate(*tmpRecV,*tmpVTAV);
    }
    delete tmpRecV; delete tmpVTAV;
    return tmpVertex;
}
*/


xAOD::Vertex * TrkVKalVrtFitter::makeXAODVertex( int Neutrals,
        const Amg::Vector3D& Vertex, const std::vector<double> & fitErrorMatrix, 
	const std::vector<double> & Chi2PerTrk,  const std::vector< std::vector<double> >& TrkAtVrt,
	double Chi2 ) 
{
    long int NTrk = m_FitStatus;
    long int Ndf = VKalGetNDOF()+m_planeCnstNDOF;

    xAOD::Vertex * tmpVertex=new xAOD::Vertex();
    tmpVertex->makePrivateStore();
    tmpVertex->setPosition(Vertex);
    tmpVertex->setFitQuality(Chi2, (float)Ndf);

    std::vector<VxTrackAtVertex> & tmpVTAV=tmpVertex->vxTrackAtVertex();
    tmpVTAV.clear();
    std::vector <double> CovFull;
    StatusCode sc = VKalGetFullCov( NTrk, CovFull); 
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
      Perigee *        tmpChargPer=0;
      NeutralPerigee * tmpNeutrPer=0;
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
      tmpVTAV.push_back( VxTrackAtVertex(Chi2PerTrk[ii], tmpChargPer, tmpNeutrPer) );
    }

    return tmpVertex;
}


} // End Of Namespace
