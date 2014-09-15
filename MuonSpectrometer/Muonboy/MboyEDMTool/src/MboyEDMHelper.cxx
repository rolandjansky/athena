/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

//----------------------------------------------------------------//
#include "TrkParameters/TrackParameters.h"

#include "TrkEventPrimitives/JacobianThetaPToCotThetaPt.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"

#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"

#include "TrkEventPrimitives/DefinedParameter.h"
#include "TrkEventPrimitives/LocalParameters.h"

#include "Identifier/Identifier.h"

#include "TrkMeasurementBase/MeasurementBase.h"

#include "MuonSegment/MuonSegment.h"
#include "MuonSegment/MuonSegmentQuality.h"

//----------------------------------------------------------------//
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"

#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"

#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkSegment/Segment.h"

#include "TrkPrepRawData/PrepRawData.h"
#include "MuonPrepRawData/CscPrepData.h"
#include "MuonPrepRawData/MdtPrepData.h"
#include "MuonPrepRawData/RpcPrepData.h"
#include "MuonPrepRawData/TgcPrepData.h"

/////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyEDMHelper.h"

MboyEDMHelper::MboyEDMHelper(const std::string& t, 
                             const std::string& n,
                             const IInterface*  p ):AthAlgTool(t,n,p)
{

  declareInterface<IMboyEDMHelper>(this);

}

MboyEDMHelper::~MboyEDMHelper(){}

// Initialize
StatusCode MboyEDMHelper::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " << name() << " - package version " << PACKAGE_VERSION  ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " AthAlgTool::initialize() failed" );
    return( StatusCode::FAILURE );
  }

//Set pointer on AtlasDetectorID
  sc = detStore()->retrieve(m_detID, "AtlasID" );
  if (sc.isFailure()) {
    ATH_MSG_FATAL("Could not get AtlasDetectorID ");
    return( StatusCode::FAILURE );
  }
  ATH_MSG_INFO( "Found AtlasDetectorID ");

  sc = detStore()->retrieve(m_mdtId,"MDTIDHELPER");
  if (sc.isFailure()){
    ATH_MSG_FATAL( "Cannot retrieve MDTIDHELPER" );
    return( StatusCode::FAILURE );
  }
  sc = detStore()->retrieve(m_cscId,"CSCIDHELPER");
  if (sc.isFailure()){
    ATH_MSG_FATAL( "Cannot retrieve CSCIDHELPER" );
    return( StatusCode::FAILURE );
  }
  sc = detStore()->retrieve(m_rpcId,"RPCIDHELPER");
  if (sc.isFailure()){
    ATH_MSG_FATAL( "Cannot retrieve RPCIDHELPER" );
    return( StatusCode::FAILURE );
  }
  sc = detStore()->retrieve(m_tgcId,"TGCIDHELPER");
  if (sc.isFailure()){
    ATH_MSG_FATAL( "Cannot retrieve TGCIDHELPER" );
    return( StatusCode::FAILURE );
  }

  SetISC();

  ATH_MSG_INFO( "Initialisation ended     " );

  return StatusCode::SUCCESS;

}

StatusCode MboyEDMHelper::finalize(){return StatusCode::SUCCESS;}

std::vector<const Rec::TrackParticle*> MboyEDMHelper::AlignTrackParticleContainers(
                                const Rec::TrackParticleContainer* pTrackParticleContainer ,
                                const Rec::TrackParticleContainer* pTrackParticleContainer_Aux
){

//Minimal distance for identification
  double DistanceSmall = 0.001;

//Collect in AuxVecColl_Aligned the TP from pTrackParticleContainer_Aux corresponding to those in pTrackParticleContainer
  std::vector<const Rec::TrackParticle*>   AuxVecColl_Aligned;


//Copy pTrackParticleContainer_Aux in AuxVecColl
  std::vector<const Rec::TrackParticle*>   AuxVecColl;
  Rec::TrackParticleContainer::const_iterator it_Aux     = pTrackParticleContainer_Aux->begin() ;
  Rec::TrackParticleContainer::const_iterator it_Aux_End = pTrackParticleContainer_Aux->end()   ;
  for ( ; it_Aux!=it_Aux_End; ++it_Aux ){
    AuxVecColl.push_back(*it_Aux);
  }		    

  
//Loop on pTrackParticleContainer
  Rec::TrackParticleContainer::const_iterator it     = pTrackParticleContainer->begin() ;
  Rec::TrackParticleContainer::const_iterator it_End = pTrackParticleContainer->end()   ;
  for ( ; it!=it_End; ++it ){
  
//     std::cout 
//       << "----------"
//       << "----------"
//       << "----------"
//       << "----------"
//       << "----------"
//       << "----------"
//       << "----------"
//       << "----------"
//       << std::endl ;
      
    std::vector<const Rec::TrackParticle*>::iterator it_AuxVecColl_Associated = AuxVecColl.end();
    
//  Loop on AuxVecColl
    std::vector<const Rec::TrackParticle*>::iterator it_AuxVecColl     = AuxVecColl.begin();
    std::vector<const Rec::TrackParticle*>::iterator it_AuxVecColl_End = AuxVecColl.end();
    for ( ; it_AuxVecColl!= it_AuxVecColl_End ;++it_AuxVecColl ) {
      if (it_AuxVecColl_Associated != it_AuxVecColl_End ) break ;

//       std::cout 
//         << "----------"
//         << "----------"
//         << "----------"
//         << "----------"
//         << std::endl ;
        
//    Get the TrkTrack
      const Trk::Track* pTrack     = (*it)->originalTrack();    
      const Trk::Track* pTrack_Aux = (*it_AuxVecColl)->originalTrack();

//    Loop on the TrackParameters to decide if the tracks match
      std::vector<const Trk::TrackParameters*>::const_iterator TrkPar     = pTrack->trackParameters()->begin(); 
      std::vector<const Trk::TrackParameters*>::const_iterator TrkPar_End = pTrack->trackParameters()->end(); 
      for ( ; TrkPar!=TrkPar_End; ++TrkPar){
        if (it_AuxVecColl_Associated != it_AuxVecColl_End ) break ;
        const Trk::TrackParameters* MEAS_TrkPar = dynamic_cast<const Trk::TrackParameters*>(*TrkPar);
        if ( !MEAS_TrkPar ) continue ;

        std::vector<const Trk::TrackParameters*>::const_iterator TrkPar_Aux     = pTrack_Aux->trackParameters()->begin(); 
        std::vector<const Trk::TrackParameters*>::const_iterator TrkPar_Aux_End = pTrack_Aux->trackParameters()->end(); 
        for ( ; TrkPar_Aux!=TrkPar_Aux_End; ++TrkPar_Aux){
          if (it_AuxVecColl_Associated != it_AuxVecColl_End ) break ;
          const Trk::TrackParameters* MEAS_TrkPar_Aux = dynamic_cast<const Trk::TrackParameters*>(*TrkPar_Aux);
          if ( !MEAS_TrkPar_Aux ) continue ;
         
          double Dx = ((*TrkPar)->position()).x() - ((*TrkPar_Aux)->position()).x() ;
          double Dy = ((*TrkPar)->position()).y() - ((*TrkPar_Aux)->position()).y() ;
          double Dz = ((*TrkPar)->position()).z() - ((*TrkPar_Aux)->position()).z() ;
          double Distance = sqrt ( Dx * Dx + Dy * Dy + Dz * Dz ) ;
          if ( Distance <= DistanceSmall ) it_AuxVecColl_Associated = it_AuxVecColl ;     

//           std::cout 
//             << " " << Distance
//             << " " << Dx 
//             << " " << Dy 
//             << " " << Dz 
//             << " " << ((*TrkPar)->position()).x() 
//             << " " << ((*TrkPar)->position()).y() 
//             << " " << ((*TrkPar)->position()).z() 
//             << " " << ((*TrkPar_Aux)->position()).x() 
//             << " " << ((*TrkPar_Aux)->position()).y() 
//             << " " << ((*TrkPar_Aux)->position()).z() 
//             << std::endl ;
          
        }
      }
      
    }
    
//  Collect the corresponding menber of AuxVecColl or 0
    if (it_AuxVecColl_Associated != it_AuxVecColl_End ) {
      AuxVecColl_Aligned.push_back(*it_AuxVecColl_Associated);
      AuxVecColl.erase(it_AuxVecColl_Associated);
    }else{
      AuxVecColl_Aligned.push_back(0);
    }
    
  }

  return AuxVecColl_Aligned;
  
}

const Trk::TrackParameters* MboyEDMHelper::GetSecuredAtaPlane(
     const Amg::Vector3D &            aGlobalPosition ,
     const Amg::Vector3D &            aGlobalMomentum ,
     double                           TheCharge       ,
     const Trk::PlaneSurface   &      aPlaneSurface
){

//  return ( new Trk::AtaPlane (aGlobalPosition, aGlobalMomentum , TheCharge , aPlaneSurface) );

    double locx = 0. ;
    double locy = 0. ;
    
    Amg::Vector2D* pLocalPosition = 0;
    bool gotLocalPosition = aPlaneSurface.globalToLocal( aGlobalPosition,  aGlobalPosition, *pLocalPosition );
    if (pLocalPosition && gotLocalPosition) {
      locx = (*pLocalPosition)[Trk::locX] ;
      locy = (*pLocalPosition)[Trk::locY] ;
    }else{
      ATH_MSG_DEBUG( " globalToLocal failed in  GetSecuredAtaPlane  " );
      Amg::Vector3D aLocalPosition = aPlaneSurface.transform().inverse()*aGlobalPosition ;
      locx = aLocalPosition[Trk::locX] ;
      locy = aLocalPosition[Trk::locY] ;
    }
    delete pLocalPosition;
    double qoverp = TheCharge/fabs(aGlobalMomentum.mag());
    return ( new Trk::AtaPlane(locx,locy,aGlobalMomentum.phi(),aGlobalMomentum.theta(),qoverp, aPlaneSurface) );

}
int MboyEDMHelper::HitsPerML( const Trk::Segment*  pSegment,  int ML ){
  if ( IsValid(pSegment) == 0 ) return 0;
  
  const Muon::MuonSegment* pMuonSegment = dynamic_cast<const Muon::MuonSegment*>(pSegment); 
  if( !pMuonSegment ) return 0;
  
  int hitsOnML(0);
  const std::vector<const Trk::RIO_OnTrack*> pRIOSet = pMuonSegment->containedROTs(); 
  std::vector<const Trk::RIO_OnTrack*>::const_iterator rotIter = pRIOSet.begin(); 
  for (; rotIter!=pRIOSet.end(); ++rotIter){ 
    if ( (*rotIter)!=0 ){ 
      Identifier id = (*rotIter)->identify() ;
      if ( m_mdtId->is_mdt( id ) )
	if(  ML == m_mdtId->multilayer( id ) ) ++hitsOnML;
    }     
  }

  return hitsOnML;
}

std::string MboyEDMHelper::EtaStationString( const Trk::Segment* pSegment ){
  std::string s("0");
  if( IsValid(pSegment) == 0 ) return s;

  const Muon::MuonSegment* pMuonSegment = dynamic_cast<const Muon::MuonSegment*>(pSegment); 
  if( !pMuonSegment ) return s;

  std::stringstream ss;
  const std::vector<const Trk::RIO_OnTrack*> pRIOSet = pMuonSegment->containedROTs(); 
  std::vector<const Trk::RIO_OnTrack*>::const_iterator rotIter = pRIOSet.begin(); 
  for (; rotIter!=pRIOSet.end(); ++rotIter){ 
    if ( (*rotIter)!=0 ){ 
      Identifier id = (*rotIter)->identify() ;
      if( m_mdtId->is_mdt(id) ) ss << m_mdtId->stationEta(id); 
      if( m_cscId->is_csc(id) ) ss << m_cscId->stationEta(id);
      ss >> s;
      return s;
    }
  }
  return s;
  
}
// Dumps TrkTrack
StatusCode MboyEDMHelper::dump_TrkTrack(const Trk::Track* pTrack,std::ofstream* pOutCurrent){

  StatusCode sc = StatusCode::SUCCESS;

  *pOutCurrent <<  "* dump_TrkTrack "<< std::endl;
  
  std::vector<const Trk::TrackParameters*>::const_iterator TrkPar    = pTrack->trackParameters()->begin(); 
  std::vector<const Trk::TrackParameters*>::const_iterator TrkParEnd = pTrack->trackParameters()->end(); 
  for ( ; TrkPar!=TrkParEnd; ++TrkPar){

    *pOutCurrent <<  "* ID surface? "<< std::endl;
    if ( &((*TrkPar)->associatedSurface()) ){
      *pOutCurrent <<  "*              ID  " 
                   << (&((*TrkPar)->associatedSurface()))->associatedDetectorElementIdentifier ()  
                   << std::endl;
    }else{
      *pOutCurrent <<  "*              No surface "<< std::endl;
    }
    
    const Trk::Perigee* pPerigee = dynamic_cast<const Trk::Perigee*>(*TrkPar);
    if ( pPerigee !=0 ) {
      sc = dump_Perigee(pPerigee,pOutCurrent) ;
      if (sc.isFailure()) ATH_MSG_WARNING( "dump_Perigee failed" );
    }
    const Trk::AtaCylinder* pAtaCylinder = dynamic_cast<const Trk::AtaCylinder*>(*TrkPar);
    if ( pAtaCylinder !=0 ) {
      sc = dump_AtaCylinder(pAtaCylinder,pOutCurrent) ;
      if (sc.isFailure()) ATH_MSG_WARNING( "dump_AtaCylinder failed" );
    }
    const Trk::AtaDisc* pAtaDisc = dynamic_cast<const Trk::AtaDisc*>(*TrkPar);
    if ( pAtaDisc !=0 ) {
      sc = dump_AtaDisc(pAtaDisc,pOutCurrent) ;
      if (sc.isFailure()) ATH_MSG_WARNING( "dump_AtaDisc failed" );
    }
    if ( pPerigee ==0 && pAtaCylinder ==0 && pAtaDisc ==0 ) {
      *pOutCurrent <<  "*  NOT Perigee, AtaCylinder, AtaDisc"<<std::endl;
      *pOutCurrent << *(*TrkPar) << std::endl; 
    }
  }

  *pOutCurrent << *pTrack << std::endl; 

  return StatusCode::SUCCESS;

}
StatusCode MboyEDMHelper::dump_Perigee(const Trk::Perigee* pPerigee,std::ofstream* pOutCurrent){

  if (pPerigee == 0) return StatusCode::SUCCESS;
  *pOutCurrent <<  "*  Its parameters at Perigee are:"<<std::endl;
  if (IsThisPositionBeyondThelimitCaloSpectro(
       (pPerigee->position()).x(),
       (pPerigee->position()).y(), 
       (pPerigee->position()).z()) == 1 ) {
    *pOutCurrent <<  "*  At Muon Spectro Entrance ?"<<std::endl;
  }else{
    *pOutCurrent <<  "*  At beam?"<<std::endl;
  }

  *pOutCurrent << "*parameters()[Trk::d0]       " << std::setw(12)<<std::setprecision(5)<<pPerigee->parameters()[Trk::d0]       << std::endl; 
  *pOutCurrent << " parameters()[Trk::z0]       " << std::setw(12)<<std::setprecision(5)<<pPerigee->parameters()[Trk::z0]       << std::endl; 
  *pOutCurrent << " parameters()[Trk::phi0]     " << std::setw(12)<<std::setprecision(5)<<pPerigee->parameters()[Trk::phi0]     << std::endl; 
  *pOutCurrent << " parameters()[Trk::theta]    " << std::setw(12)<<std::setprecision(5)<<pPerigee->parameters()[Trk::theta]    << std::endl; 
  *pOutCurrent << " parameters()[Trk::qOverP]   " << std::setw(12)<<std::setprecision(5)<<pPerigee->parameters()[Trk::qOverP]   << std::endl; 
  *pOutCurrent << " cotTheta()                  " << std::setw(12)<<std::setprecision(5)<<1./tan(pPerigee->parameters()[Trk::theta]) << std::endl; 
  *pOutCurrent << " charge()/peri->pT()         " << std::setw(12)<<std::setprecision(5)<<pPerigee->charge()/pPerigee->pT()     << std::endl; 

  const Trk::Perigee* pMeasuredPerigee = dynamic_cast<const Trk::Perigee*>(pPerigee);
  if (pMeasuredPerigee != 0){
   double mtheta = (pPerigee->parameters())[Trk::theta];
   double mqp = (pPerigee->parameters())[Trk::qOverP];
   Trk::JacobianThetaPToCotThetaPt TheJac(mtheta,mqp);
   AmgSymMatrix(5) covVert = pMeasuredPerigee->covariance()->similarity(TheJac);
   *pOutCurrent 
        << "*     at Perigee          :  "
        <<  std::setw(12)<<std::setprecision(5)<<pPerigee->parameters()[Trk::d0]
        <<  std::setw(12)<<std::setprecision(5)<<pPerigee->parameters()[Trk::z0]
        <<  std::setw(12)<<std::setprecision(5)<<pPerigee->parameters()[Trk::phi0]
        <<  std::setw(12)<<std::setprecision(5)<<1./tan(pPerigee->parameters()[Trk::theta])
        <<  std::setw(12)<<std::setprecision(5)<<pPerigee->charge()/pPerigee->pT()
        <<  std::setw(12)<<std::setprecision(5)<<pPerigee->pT()/pPerigee->charge()
        <<std::endl;
   *pOutCurrent 
        << "*                       +/-  "
        <<  std::setw(12)<<std::setprecision(5)<<sqrt(covVert(0,0))
        <<  std::setw(12)<<std::setprecision(5)<<sqrt(covVert(1,1))
        <<  std::setw(12)<<std::setprecision(5)<<sqrt(covVert(2,2))
        <<  std::setw(12)<<std::setprecision(5)<<sqrt(covVert(3,3))
        <<  std::setw(12)<<std::setprecision(5)<<sqrt(covVert(4,4))
        <<std::endl;
   *pOutCurrent 
        << "*    Covariance matrice    "
        <<std::endl;
   *pOutCurrent 
        << "*                         :  "
        <<  std::setw(12)<<std::setprecision(2)<<covVert(0,0)*100./sqrt(covVert(0,0)*covVert(0,0))
        <<  std::setw(12)<<std::setprecision(2)<<covVert(0,1)*100./sqrt(covVert(0,0)*covVert(1,1))
        <<  std::setw(12)<<std::setprecision(2)<<covVert(0,2)*100./sqrt(covVert(0,0)*covVert(2,2))
        <<  std::setw(12)<<std::setprecision(2)<<covVert(0,3)*100./sqrt(covVert(0,0)*covVert(3,3))
        <<  std::setw(12)<<std::setprecision(2)<<covVert(0,4)*100./sqrt(covVert(0,0)*covVert(4,4))
        <<std::endl;
   for (int i1=1; i1<=4; i1++){
    *pOutCurrent 
         << "*                            "
         <<  std::setw(12)<<std::setprecision(2)<<covVert(i1,0)*100./sqrt(covVert(i1,i1)*covVert(0,0))
         <<  std::setw(12)<<std::setprecision(2)<<covVert(i1,1)*100./sqrt(covVert(i1,i1)*covVert(1,1))
         <<  std::setw(12)<<std::setprecision(2)<<covVert(i1,2)*100./sqrt(covVert(i1,i1)*covVert(2,2))
         <<  std::setw(12)<<std::setprecision(2)<<covVert(i1,3)*100./sqrt(covVert(i1,i1)*covVert(3,3))
         <<  std::setw(12)<<std::setprecision(2)<<covVert(i1,4)*100./sqrt(covVert(i1,i1)*covVert(4,4))
         <<std::endl;
   }
  }
  *pOutCurrent 
       << "*    position                "
       <<  std::setw(12)<<std::setprecision(3)<<(pPerigee->position()).x()
       <<std::endl;
  *pOutCurrent 
       << "*                            "
       <<  std::setw(12)<<std::setprecision(3)<<(pPerigee->position()).y()
       <<std::endl;
  *pOutCurrent 
       << "*                            "
       <<  std::setw(12)<<std::setprecision(3)<<(pPerigee->position()).z()
       <<std::endl;
  *pOutCurrent 
       << "*    momentum                "
       <<  std::setw(12)<<std::setprecision(3)<<(pPerigee->momentum()).x()
       <<std::endl;
  *pOutCurrent 
       << "*                            "
       <<  std::setw(12)<<std::setprecision(3)<<(pPerigee->momentum()).y()
       <<std::endl;
  *pOutCurrent 
       << "*                            "
       <<  std::setw(12)<<std::setprecision(3)<<(pPerigee->momentum()).z()
       <<std::endl;

  return StatusCode::SUCCESS;

}
StatusCode MboyEDMHelper::dump_AtaCylinder(const Trk::AtaCylinder* pAtaCylinder,std::ofstream* pOutCurrent){

  if (pAtaCylinder == 0) return StatusCode::SUCCESS;
  *pOutCurrent <<  "*  Its parameters at AtaCylinder are:"<<std::endl;
  if (IsThisPositionBeyondThelimitCaloSpectro(
       (pAtaCylinder->position()).x(),
       (pAtaCylinder->position()).y(), 
       (pAtaCylinder->position()).z()) == 1 ) {
    *pOutCurrent <<  "*  At Muon Spectro Entrance ?"<<std::endl;
  }else{
    *pOutCurrent <<  "*  At Calo Entrance ?"<<std::endl;
  }

  *pOutCurrent << "*parameters()[Trk::locRPhi]  " << std::setw(12)<<std::setprecision(5)<<pAtaCylinder->parameters()[Trk::locRPhi]  << std::endl; 
  *pOutCurrent << " parameters()[Trk::locZ]     " << std::setw(12)<<std::setprecision(5)<<pAtaCylinder->parameters()[Trk::locZ]     << std::endl; 
  *pOutCurrent << " parameters()[Trk::phi]      " << std::setw(12)<<std::setprecision(5)<<pAtaCylinder->parameters()[Trk::phi]      << std::endl; 
  *pOutCurrent << " parameters()[Trk::theta]    " << std::setw(12)<<std::setprecision(5)<<pAtaCylinder->parameters()[Trk::theta]    << std::endl; 
  *pOutCurrent << " parameters()[Trk::qOverP]   " << std::setw(12)<<std::setprecision(5)<<pAtaCylinder->parameters()[Trk::qOverP]   << std::endl; 

  double RCrossing = sqrt ( (pAtaCylinder->position()).x()*(pAtaCylinder->position()).x() + (pAtaCylinder->position()).y()*(pAtaCylinder->position()).y() );
//double ZCrossing = (pAtaCylinder->position()).z()   ;
  double ThetaX = atan( RCrossing/pAtaCylinder->parameters()[Trk::locZ] ) ;
  if ( ThetaX < 0. ) ThetaX = ThetaX + 3.1415927 ;
  double PhiX   = pAtaCylinder->parameters()[Trk::locRPhi]/RCrossing   ;
  double ThetaV = pAtaCylinder->parameters()[Trk::theta]     ;
  double PhiV   = pAtaCylinder->parameters()[Trk::phi]       ;
  double qOnp   = pAtaCylinder->parameters()[Trk::qOverP]    ;  

  const Trk::AtaCylinder* pMeasuredAtaCylinder = dynamic_cast<const Trk::AtaCylinder*>(pAtaCylinder);
  if (pMeasuredAtaCylinder != 0){
   AmgMatrix(5,5) TheJac;
   TheJac.setZero();
   TheJac(1,2) = -std::pow( sin(ThetaX) , 2 )/RCrossing ;
   TheJac(2,1) = 1./RCrossing ;
   TheJac(3,4) = 1. ;
   TheJac(4,3) = 1. ;
   TheJac(5,5) = 1. ;
   AmgSymMatrix(5) covVert = pMeasuredAtaCylinder->covariance()->similarity(TheJac);
   *pOutCurrent 
        << "*     at AtaCylinder      :  "
        <<  std::setw(12)<<std::setprecision(5)<<ThetaX
        <<  std::setw(12)<<std::setprecision(5)<<PhiX  
        <<  std::setw(12)<<std::setprecision(5)<<ThetaV
        <<  std::setw(12)<<std::setprecision(5)<<PhiV  
        <<  std::setw(12)<<std::setprecision(5)<<qOnp  
        <<  std::setw(12)<<std::setprecision(5)<<1./qOnp
        <<std::endl;
   *pOutCurrent 
        << "*                       +/-  "
        <<  std::setw(12)<<std::setprecision(5)<<sqrt(covVert(0,0))
        <<  std::setw(12)<<std::setprecision(5)<<sqrt(covVert(1,1))
        <<  std::setw(12)<<std::setprecision(5)<<sqrt(covVert(2,2))
        <<  std::setw(12)<<std::setprecision(5)<<sqrt(covVert(3,3))
        <<  std::setw(12)<<std::setprecision(5)<<sqrt(covVert(4,4))
        <<std::endl;
   *pOutCurrent 
        << "*    Covariance matrice    "
        <<std::endl;
   *pOutCurrent 
        << "*                         :  "
        <<  std::setw(12)<<std::setprecision(2)<<covVert(0,0)*100./sqrt(covVert(0,0)*covVert(0,0))
        <<  std::setw(12)<<std::setprecision(2)<<covVert(0,1)*100./sqrt(covVert(0,0)*covVert(1,1))
        <<  std::setw(12)<<std::setprecision(2)<<covVert(0,2)*100./sqrt(covVert(0,0)*covVert(2,2))
        <<  std::setw(12)<<std::setprecision(2)<<covVert(0,3)*100./sqrt(covVert(0,0)*covVert(3,3))
        <<  std::setw(12)<<std::setprecision(2)<<covVert(0,4)*100./sqrt(covVert(0,0)*covVert(4,4))
        <<std::endl;
   for (int i1=1; i1<=4; i1++){
    *pOutCurrent 
         << "*                            "
         <<  std::setw(12)<<std::setprecision(2)<<covVert(i1,0)*100./sqrt(covVert(i1,i1)*covVert(0,0))
         <<  std::setw(12)<<std::setprecision(2)<<covVert(i1,1)*100./sqrt(covVert(i1,i1)*covVert(1,1))
         <<  std::setw(12)<<std::setprecision(2)<<covVert(i1,2)*100./sqrt(covVert(i1,i1)*covVert(2,2))
         <<  std::setw(12)<<std::setprecision(2)<<covVert(i1,3)*100./sqrt(covVert(i1,i1)*covVert(3,3))
         <<  std::setw(12)<<std::setprecision(2)<<covVert(i1,4)*100./sqrt(covVert(i1,i1)*covVert(4,4))
         <<std::endl;
   }
  }
  *pOutCurrent 
       << "*    position                "
       <<  std::setw(12)<<std::setprecision(3)<<(pAtaCylinder->position()).x()
       <<std::endl;
  *pOutCurrent 
       << "*                            "
       <<  std::setw(12)<<std::setprecision(3)<<(pAtaCylinder->position()).y()
       <<std::endl;
  *pOutCurrent 
       << "*                            "
       <<  std::setw(12)<<std::setprecision(3)<<(pAtaCylinder->position()).z()
       <<std::endl;
  *pOutCurrent 
       << "*    momentum                "
       <<  std::setw(12)<<std::setprecision(3)<<(pAtaCylinder->momentum()).x()
       <<std::endl;
  *pOutCurrent 
       << "*                            "
       <<  std::setw(12)<<std::setprecision(3)<<(pAtaCylinder->momentum()).y()
       <<std::endl;
  *pOutCurrent 
       << "*                            "
       <<  std::setw(12)<<std::setprecision(3)<<(pAtaCylinder->momentum()).z()
       <<std::endl;

  return StatusCode::SUCCESS;

}
StatusCode MboyEDMHelper::dump_AtaDisc(const Trk::AtaDisc* pAtaDisc,std::ofstream* pOutCurrent){

  if (pAtaDisc == 0) return StatusCode::SUCCESS;
  *pOutCurrent <<  "*  Its parameters at AtaDisc are:"<<std::endl;
  if (IsThisPositionBeyondThelimitCaloSpectro(
       (pAtaDisc->position()).x(),
       (pAtaDisc->position()).y(), 
       (pAtaDisc->position()).z()) == 1 ) {
    *pOutCurrent <<  "*  At Muon Spectro Entrance ?"<<std::endl;
  }else{
    *pOutCurrent <<  "*  At Calo Entrance ?"<<std::endl;
  }

  *pOutCurrent << "*parameters()[Trk::locR]     " <<std::setw(12)<<std::setprecision(5)<< pAtaDisc->parameters()[Trk::locR]     << std::endl; 
  *pOutCurrent << " parameters()[Trk::locPhi]   " << std::setw(12)<<std::setprecision(5)<<pAtaDisc->parameters()[Trk::locPhi]   << std::endl; 
  *pOutCurrent << " parameters()[Trk::phi]      " << std::setw(12)<<std::setprecision(5)<<pAtaDisc->parameters()[Trk::phi]      << std::endl; 
  *pOutCurrent << " parameters()[Trk::theta]    " << std::setw(12)<<std::setprecision(5)<<pAtaDisc->parameters()[Trk::theta]    << std::endl; 
  *pOutCurrent << " parameters()[Trk::qOverP]   " << std::setw(12)<<std::setprecision(5)<<pAtaDisc->parameters()[Trk::qOverP]   << std::endl; 

//double RCrossing = sqrt ( (pAtaDisc->position()).x()*(pAtaDisc->position()).x() + (pAtaDisc->position()).y()*(pAtaDisc->position()).y() );
  double ZCrossing = (pAtaDisc->position()).z()   ;
  double ThetaX = atan( pAtaDisc->parameters()[Trk::locR]/ZCrossing ) ;
  if ( ThetaX < 0. ) ThetaX = ThetaX + 3.1415927 ;
  double PhiX   = pAtaDisc->parameters()[Trk::locPhi]   ;
  double ThetaV = pAtaDisc->parameters()[Trk::theta]     ;
  double PhiV   = pAtaDisc->parameters()[Trk::phi]       ;
  double qOnp   = pAtaDisc->parameters()[Trk::qOverP]    ;  

  const Trk::AtaDisc* pMeasuredAtaDisc = dynamic_cast<const Trk::AtaDisc*>(pAtaDisc);
  if (pMeasuredAtaDisc != 0){
   AmgMatrix(5,5) TheJac;
   TheJac.setZero();
   TheJac(1,1) = std::pow( cos(ThetaX) , 2 )/ZCrossing;
   TheJac(2,2) = 1. ;
   TheJac(3,4) = 1. ;
   TheJac(4,3) = 1. ;
   TheJac(5,5) = 1. ;
   AmgSymMatrix(5) covVert = pMeasuredAtaDisc->covariance()->similarity(TheJac);
   *pOutCurrent 
        << "*     at AtaDisc          :  "
        <<  std::setw(12)<<std::setprecision(5)<<ThetaX
        <<  std::setw(12)<<std::setprecision(5)<<PhiX  
        <<  std::setw(12)<<std::setprecision(5)<<ThetaV
        <<  std::setw(12)<<std::setprecision(5)<<PhiV  
        <<  std::setw(12)<<std::setprecision(5)<<qOnp  
        <<  std::setw(12)<<std::setprecision(5)<<1./qOnp
        <<std::endl;
   *pOutCurrent 
        << "*                       +/-  "
        <<  std::setw(12)<<std::setprecision(5)<<sqrt(covVert(0,0))
        <<  std::setw(12)<<std::setprecision(5)<<sqrt(covVert(1,1))
        <<  std::setw(12)<<std::setprecision(5)<<sqrt(covVert(2,2))
        <<  std::setw(12)<<std::setprecision(5)<<sqrt(covVert(3,3))
        <<  std::setw(12)<<std::setprecision(5)<<sqrt(covVert(4,4))
        <<std::endl;
   *pOutCurrent 
        << "*    Covariance matrice    "
        <<std::endl;
   *pOutCurrent 
        << "*                         :  "
        <<  std::setw(12)<<std::setprecision(2)<<covVert(0,0)*100./sqrt(covVert(0,0)*covVert(0,0))
        <<  std::setw(12)<<std::setprecision(2)<<covVert(0,1)*100./sqrt(covVert(0,0)*covVert(1,1))
        <<  std::setw(12)<<std::setprecision(2)<<covVert(0,2)*100./sqrt(covVert(0,0)*covVert(2,2))
        <<  std::setw(12)<<std::setprecision(2)<<covVert(0,3)*100./sqrt(covVert(0,0)*covVert(3,3))
        <<  std::setw(12)<<std::setprecision(2)<<covVert(0,4)*100./sqrt(covVert(0,0)*covVert(4,4))
        <<std::endl;
   for (int i1=1; i1<=4; i1++){
    *pOutCurrent 
         << "*                            "
         <<  std::setw(12)<<std::setprecision(2)<<covVert(i1,0)*100./sqrt(covVert(i1,i1)*covVert(0,0))
         <<  std::setw(12)<<std::setprecision(2)<<covVert(i1,1)*100./sqrt(covVert(i1,i1)*covVert(1,1))
         <<  std::setw(12)<<std::setprecision(2)<<covVert(i1,2)*100./sqrt(covVert(i1,i1)*covVert(2,2))
         <<  std::setw(12)<<std::setprecision(2)<<covVert(i1,3)*100./sqrt(covVert(i1,i1)*covVert(3,3))
         <<  std::setw(12)<<std::setprecision(2)<<covVert(i1,4)*100./sqrt(covVert(i1,i1)*covVert(4,4))
         <<std::endl;
   }
  }
  *pOutCurrent 
       << "*    position                "
       <<  std::setw(12)<<std::setprecision(3)<<(pAtaDisc->position()).x()
       <<std::endl;
  *pOutCurrent 
       << "*                            "
       <<  std::setw(12)<<std::setprecision(3)<<(pAtaDisc->position()).y()
       <<std::endl;
  *pOutCurrent 
       << "*                            "
       <<  std::setw(12)<<std::setprecision(3)<<(pAtaDisc->position()).z()
       <<std::endl;
  *pOutCurrent 
       << "*    momentum                "
       <<  std::setw(12)<<std::setprecision(3)<<(pAtaDisc->momentum()).x()
       <<std::endl;
  *pOutCurrent 
       << "*                            "
       <<  std::setw(12)<<std::setprecision(3)<<(pAtaDisc->momentum()).y()
       <<std::endl;
  *pOutCurrent 
       << "*                            "
       <<  std::setw(12)<<std::setprecision(3)<<(pAtaDisc->momentum()).z()
       <<std::endl;

  return StatusCode::SUCCESS;

}

// Dumps TrkSegment
StatusCode MboyEDMHelper::dump_TrkSegment(
 const Trk::Segment*  pSegment,
 std::ofstream* pOutCurrent){

  if ( IsValid(pSegment) == 0 ) return StatusCode::SUCCESS;
  const Muon::MuonSegment* pMuonSegment = dynamic_cast<const Muon::MuonSegment*>(pSegment);
  if (pMuonSegment==0) return StatusCode::SUCCESS; 

//Get ErrorMatrix
//Change of interface of MuonSegment: JFL Fri Jan 19 10:00:33 CET 2007
//const Amg::MatrixX  TheErrorMatrix =  pMuonSegment->fullLocalErrorMatrix() ;
  const Amg::MatrixX  TheCovarianceMatrix =  pMuonSegment->localCovariance();
  const Amg::MatrixX  TheWeightMatrix = TheCovarianceMatrix.inverse();

//   *pOutCurrent 
//      << "Station Index/Name   " 
//      << " " << SegmentAssociatedStationIndex(pSegment)
//      << " " << SegmentAssociatedStationName(pSegment)
//      <<std::endl;

  int Ihas2ndCoord = 0 ;
  if (SegmentHas2ndCoordMeasurement(pSegment)) Ihas2ndCoord = 1 ;
  *pOutCurrent 
    << "Has2ndCoord      " 
    <<  std::setw(20)<< Ihas2ndCoord    
    << std::endl;

  double X_Xaxis,Y_Xaxis,Z_Xaxis;
  GetXaxis(pSegment,X_Xaxis,Y_Xaxis,Z_Xaxis);
  *pOutCurrent 
     << "Xaxis     " 
     << " " << std::setw(12)<<std::setprecision(5) << X_Xaxis
     << " " << std::setw(12)<<std::setprecision(5) << Y_Xaxis
     << " " << std::setw(12)<<std::setprecision(5) << Z_Xaxis
     <<std::endl;

  double X_Yaxis,Y_Yaxis,Z_Yaxis;
  GetYaxis(pSegment,X_Yaxis,Y_Yaxis,Z_Yaxis);
  *pOutCurrent 
     << "Yaxis     " 
     << " " << std::setw(12)<<std::setprecision(5) << X_Yaxis
     << " " << std::setw(12)<<std::setprecision(5) << Y_Yaxis
     << " " << std::setw(12)<<std::setprecision(5) << Z_Yaxis
     <<std::endl;

  double ThePositionX,ThePositionY,ThePositionZ;
  GetPoint(pSegment,ThePositionX,ThePositionY,ThePositionZ);
  *pOutCurrent 
     << "Position  " 
     << " " << std::setw(12)<<std::setprecision(5) << ThePositionX
     << " " << std::setw(12)<<std::setprecision(5) << ThePositionY
     << " " << std::setw(12)<<std::setprecision(5) << ThePositionZ
     <<std::endl;

  double TheDirectionX,TheDirectionY,TheDirectionZ;
  GetDirection(pSegment,TheDirectionX,TheDirectionY,TheDirectionZ);
  *pOutCurrent 
     << "Direction " 
     << " " << std::setw(12)<<std::setprecision(5) << TheDirectionX
     << " " << std::setw(12)<<std::setprecision(5) << TheDirectionY
     << " " << std::setw(12)<<std::setprecision(5) << TheDirectionZ
     <<std::endl;

  *pOutCurrent 
     << "Errors    " 
     <<std::endl;
  for (int Ind1=0; Ind1<4; Ind1++){
    *pOutCurrent 
       << " " << std::setw(12)<<std::setprecision(5) << TheWeightMatrix(Ind1,0)
       << " " << std::setw(12)<<std::setprecision(5) << TheWeightMatrix(Ind1,1)
       << " " << std::setw(12)<<std::setprecision(5) << TheWeightMatrix(Ind1,2)
       << " " << std::setw(12)<<std::setprecision(5) << TheWeightMatrix(Ind1,3)
       <<std::endl;
  }
  *pOutCurrent
       << "dx Axz mm/mrad         "
       << std::setw(12)<<std::setprecision(4)<< 1./sqrt(TheWeightMatrix(0,0))
       << std::setw(12)<<std::setprecision(4)<< 1000./sqrt(TheWeightMatrix(1,1))
       << std::setw(12)<<std::setprecision(4)<< TheWeightMatrix(0,1)/(sqrt(TheWeightMatrix(0,0)*TheWeightMatrix(1,1)))
       << std::endl;
  *pOutCurrent 
       << "dy Axz micron/microrad "
       << std::setw(12)<<std::setprecision(4)<< 1000./sqrt(TheWeightMatrix(2,2))
       << std::setw(12)<<std::setprecision(4)<< 1000000./sqrt(TheWeightMatrix(3,3))
       << std::setw(12)<<std::setprecision(4)<< TheWeightMatrix(2,3)/(sqrt(TheWeightMatrix(2,2)*TheWeightMatrix(3,3)))
       << std::endl;

  *pOutCurrent 
     << "CErrors   " 
     <<std::endl;
  for (int Ind1=0; Ind1<4; Ind1++){
    *pOutCurrent 
       << " " << std::setw(12)<<std::setprecision(5) << TheCovarianceMatrix(Ind1,0)
       << " " << std::setw(12)<<std::setprecision(5) << TheCovarianceMatrix(Ind1,1)
       << " " << std::setw(12)<<std::setprecision(5) << TheCovarianceMatrix(Ind1,2)
       << " " << std::setw(12)<<std::setprecision(5) << TheCovarianceMatrix(Ind1,3)
       <<std::endl;
  }
  *pOutCurrent
       << "Cdx Axz mm/mrad        "
       << std::setw(12)<<std::setprecision(4)<< sqrt(TheCovarianceMatrix(0,0))
       << std::setw(12)<<std::setprecision(4)<< 1000.*sqrt(TheCovarianceMatrix(1,1))
       << std::setw(12)<<std::setprecision(4)<< TheCovarianceMatrix(0,1)/(sqrt(TheCovarianceMatrix(0,0)*TheCovarianceMatrix(1,1)))
       << std::endl;
  *pOutCurrent 
       << "Cdy Axz micron/microrad"
       << std::setw(12)<<std::setprecision(4)<< 1000.*sqrt(TheCovarianceMatrix(2,2))
       << std::setw(12)<<std::setprecision(4)<< 1000000.*sqrt(TheCovarianceMatrix(3,3))
       << std::setw(12)<<std::setprecision(4)<< TheCovarianceMatrix(2,3)/(sqrt(TheCovarianceMatrix(2,2)*TheCovarianceMatrix(3,3)))
       << std::endl;

  return StatusCode::SUCCESS;

}

// Dumps Rots of Trk Objects
StatusCode MboyEDMHelper::dump_TrkTrackDigits(
 const Trk::Track* pTrack,
 std::ofstream* pOutCurrent){

  if ( pTrack == 0 ) return StatusCode::SUCCESS;
  if ( pTrack->measurementsOnTrack() == 0 ) return StatusCode::SUCCESS;

  *pOutCurrent <<  "* dump_TrkTrackDigits 1st loop "<< std::endl;
  
  std::vector<const Trk::MeasurementBase*>::const_iterator imEnd = pTrack->measurementsOnTrack()->end(); 

  std::vector<const Trk::MeasurementBase*>::const_iterator im   = pTrack->measurementsOnTrack()->begin(); 
  for ( ; im!=imEnd; ++im){
    const Trk::RIO_OnTrack* pRIO_OnTrack = dynamic_cast<const Trk::RIO_OnTrack*>(*im);
    if ( pRIO_OnTrack!=0 ){
      const Identifier& channelId = pRIO_OnTrack->identify();
      if (m_detID->is_muon(channelId)){
        StatusCode sc = dump_rot(pRIO_OnTrack,pOutCurrent);
        if (sc.isFailure()) ATH_MSG_WARNING( "dump_rot failed" );
      }
    }else{
      ATH_MSG_WARNING("Null pointer to ROT found on Track " );
    }
  }

  *pOutCurrent <<  "* dump_TrkTrackDigits 2nd loop "<< std::endl;
  
  im   = pTrack->measurementsOnTrack()->begin(); 
  for ( ; im!=imEnd; ++im){
    const Trk::RIO_OnTrack* pRIO_OnTrack = dynamic_cast<const Trk::RIO_OnTrack*>(*im);
    if ( pRIO_OnTrack!=0 ){

      *pOutCurrent <<  "* ID surface? "<< std::endl;
      *pOutCurrent <<  "*              ID  " 
                   << (pRIO_OnTrack->associatedSurface()).associatedDetectorElementIdentifier ()  
                   << std::endl;

      const Identifier& channelId = pRIO_OnTrack->identify();
      if (!m_detID->is_muon(channelId)) *pOutCurrent << *pRIO_OnTrack << std::endl;
    }else{
      ATH_MSG_WARNING("Null pointer to ROT found on Track " );
    }
  }

  return StatusCode::SUCCESS;

}
StatusCode MboyEDMHelper::dump_TrkSegmentDigits(
 const Trk::Segment*  pSegment,
 std::ofstream* pOutCurrent){

  if ( IsValid(pSegment) == 0 ) return StatusCode::SUCCESS;
  const Muon::MuonSegment* pMuonSegment = dynamic_cast<const Muon::MuonSegment*>(pSegment);
  if (pMuonSegment==0) return StatusCode::SUCCESS; 

  const std::vector<const Trk::RIO_OnTrack*> pRIOSet = pMuonSegment->containedROTs();

  std::vector<const Trk::RIO_OnTrack*>::const_iterator pRIO_OnTrackIter = pRIOSet.begin();
  for (; pRIO_OnTrackIter!=pRIOSet.end(); ++pRIO_OnTrackIter){
    if ( (*pRIO_OnTrackIter)!=0 ){
      const Identifier& channelId = (*pRIO_OnTrackIter)->identify();
      if (m_detID->is_muon(channelId)){
        StatusCode sc= dump_rot((*pRIO_OnTrackIter),pOutCurrent);
        if (sc.isFailure()) ATH_MSG_WARNING( "dump_rot failed" );
      }
    }else{
      ATH_MSG_WARNING("Null pointer to ROT found in pRIOSet " );
    }
  }

  pRIO_OnTrackIter = pRIOSet.begin();
  for (; pRIO_OnTrackIter!=pRIOSet.end(); ++pRIO_OnTrackIter){
    if ( (*pRIO_OnTrackIter)!=0 ){
      const Identifier& channelId = (*pRIO_OnTrackIter)->identify();
      if (!m_detID->is_muon(channelId))  *pOutCurrent << *(*pRIO_OnTrackIter) << std::endl;
    }else{
      ATH_MSG_WARNING("Null pointer to ROT found in pRIOSet " );
    }
  }

  return StatusCode::SUCCESS;

}
StatusCode MboyEDMHelper::dump_TrkSegmentHoles(
 const Trk::Segment*  pSegment,
 std::ofstream* pOutCurrent){

  if ( IsValid(pSegment) == 0 ) return StatusCode::SUCCESS;
  const Muon::MuonSegment* pMuonSegment = dynamic_cast<const Muon::MuonSegment*>(pSegment);
  if (pMuonSegment==0) return StatusCode::SUCCESS; 

  const Muon::MuonSegmentQuality* pMuonSegmentQuality = dynamic_cast<const Muon::MuonSegmentQuality*>(pMuonSegment->fitQuality());
  if( pMuonSegmentQuality ){
    if (pMuonSegmentQuality->channelsWithoutHit().size() != 0 ){
      std::vector<Identifier>::const_iterator IdCurrent     = pMuonSegmentQuality->channelsWithoutHit().begin();
      std::vector<Identifier>::const_iterator IdCurrent_end = pMuonSegmentQuality->channelsWithoutHit().end();
      for( ;IdCurrent!=IdCurrent_end;++IdCurrent ){
        const Identifier& channelId = *IdCurrent;
        StatusCode sc = dump_id(channelId,pOutCurrent);
        if (sc.isFailure())  ATH_MSG_WARNING( "dump_id failed" );
      }
    }else{
      *pOutCurrent << "This Segment does not contain holes " << std::endl;
    }
  }else{
    *pOutCurrent << "This Segment does not contain a  pMuonSegmentQuality" << std::endl;
  }

  return StatusCode::SUCCESS;

}

// Dumps Rot
StatusCode MboyEDMHelper::dump_rot(const Trk::RIO_OnTrack* rot,std::ofstream* pOutCurrent){

  if ( rot == 0 ) return StatusCode::SUCCESS;

  *pOutCurrent <<  "* ID surface? "<< std::endl;
  *pOutCurrent <<  "*              ID  " 
               << (rot->associatedSurface()).associatedDetectorElementIdentifier ()  
               << std::endl;

  const Identifier& channelId = rot->identify();
  StatusCode sc = dump_id(channelId,pOutCurrent);
  *pOutCurrent << *rot << std::endl;
  if (sc.isFailure())  ATH_MSG_WARNING( "dump_id failed" );

  return StatusCode::SUCCESS;

}
// Dumps Ids
StatusCode MboyEDMHelper::dump_id(Identifier channelId,std::ofstream* pOutCurrent){

  if (m_detID->is_muon(channelId)){
    if(m_mdtId->is_mdt(channelId)){
      StatusCode sc = dump_mdt(channelId,pOutCurrent);
      if (sc.isFailure())  ATH_MSG_WARNING( "dump_mdt failed" );
    }
    if(m_rpcId->is_rpc(channelId)){
      StatusCode sc = dump_rpc(channelId,pOutCurrent);
      if (sc.isFailure()) ATH_MSG_WARNING( "dump_rpc failed" );
    }
    if(m_tgcId->is_tgc(channelId)){
      StatusCode sc = dump_tgc(channelId,pOutCurrent);
      if (sc.isFailure()) ATH_MSG_WARNING( "dump_tgc failed" );
    }
    if(m_cscId->is_csc(channelId)){
      StatusCode sc = dump_csc(channelId,pOutCurrent);
      if (sc.isFailure()) ATH_MSG_WARNING( "dump_csc failed" );
    }
  }

  return StatusCode::SUCCESS;

}
StatusCode MboyEDMHelper::dump_mdt(Identifier channelId,std::ofstream* pOutCurrent){

  if(m_mdtId->is_mdt(channelId)){
    int         SGStationNber = m_mdtId->stationName(channelId);
    std::string SGStationName = m_mdtId->stationNameString(SGStationNber);
    int         SGStationEta  = m_mdtId->stationEta(channelId);
    int         SGStationPhi  = m_mdtId->stationPhi(channelId);
    int         SGMultilayer  = m_mdtId->multilayer(channelId) ; 
    int         SGTubeLayer   = m_mdtId->tubeLayer(channelId)  ;
    int         SGTube        = m_mdtId->tube(channelId)       ; 
    *pOutCurrent 
        << "MDT "
        << "  "
        << std::setw(3)<< SGStationName
        << std::setw(4)<< SGStationEta
        << std::setw(4)<< SGStationPhi
        << std::setw(4)<< SGMultilayer
        << std::setw(4)<< SGTubeLayer
        << std::setw(4)<< SGTube
        << std::endl;
  }

  return StatusCode::SUCCESS;

}
StatusCode MboyEDMHelper::dump_mdt(Identifier channelId,std::stringstream* pOutCurrent){
  
  if(m_mdtId->is_mdt(channelId)){
    int         SGStationNber = m_mdtId->stationName(channelId);
    std::string SGStationName = m_mdtId->stationNameString(SGStationNber);
    int         SGStationEta  = m_mdtId->stationEta(channelId);
    int         SGStationPhi  = m_mdtId->stationPhi(channelId);
    int         SGMultilayer  = m_mdtId->multilayer(channelId) ; 
    int         SGTubeLayer   = m_mdtId->tubeLayer(channelId)  ;
    int         SGTube        = m_mdtId->tube(channelId)       ; 
    *pOutCurrent 
      << "MDT "
      << "  "
      << std::setw(3)<< SGStationName
      << std::setw(4)<< SGStationEta
      << std::setw(4)<< SGStationPhi
      << std::setw(4)<< SGMultilayer
      << std::setw(4)<< SGTubeLayer
      << std::setw(4)<< SGTube
      << std::endl;
  }
 
  return StatusCode::SUCCESS;
 
}
StatusCode MboyEDMHelper::give_mdt(Identifier channelId,std::string& name, int& eta, int& phi, int& multilayer, int& tubeLayer, int& tube){
 
  if(m_mdtId->is_mdt(channelId)){
    int         SGStationNber = m_mdtId->stationName(channelId);
    name = m_mdtId->stationNameString(SGStationNber);
    eta  = m_mdtId->stationEta(channelId);
    phi  = m_mdtId->stationPhi(channelId);
    multilayer  = m_mdtId->multilayer(channelId) ; 
    tubeLayer   = m_mdtId->tubeLayer(channelId)  ;
    tube        = m_mdtId->tube(channelId)       ; 
  }
 
   return StatusCode::SUCCESS;
 
}
StatusCode MboyEDMHelper::dump_rpc(Identifier channelId,std::ofstream* pOutCurrent){

  if(m_rpcId->is_rpc(channelId)){
    int         SGStationNber = m_rpcId->stationName(channelId);
    std::string SGStationName = m_rpcId->stationNameString(SGStationNber);
    int         SGStationEta  = m_rpcId->stationEta(channelId);
    int         SGStationPhi  = m_rpcId->stationPhi(channelId);
    int         SGDoubletR    = m_rpcId->doubletR(channelId)    ;
    int         SGDoubletZ    = m_rpcId->doubletZ(channelId)    ;
    int         SGDoubletPhi  = m_rpcId->doubletPhi(channelId)  ;
    int         SGGasGap      = m_rpcId->gasGap(channelId)      ;
    int         SGMeasuresPhi = m_rpcId->measuresPhi(channelId) ; 
    int         SGStrip       = m_rpcId->strip(channelId)       ;
    *pOutCurrent 
        << "RPC "
        << "  "
        << std::setw(3)<< SGStationName
        << std::setw(4)<< SGStationEta
        << std::setw(4)<< SGStationPhi
        << std::setw(4)<< SGDoubletR
        << std::setw(4)<< SGDoubletZ
        << std::setw(4)<< SGDoubletPhi
        << std::setw(4)<< SGGasGap
        << std::setw(4)<< SGMeasuresPhi
        << std::setw(4)<< SGStrip
        << std::endl;
  }

  return StatusCode::SUCCESS;

}
StatusCode MboyEDMHelper::give_rpc(Identifier channelId,std::string& name, int& eta, int& phi, int& doubletR, int& doubletZ, int& doubletPhi, 
				   int& gasGap, int& measuresPhi, int& strip){

  if(m_rpcId->is_rpc(channelId)){
    int         SGStationNber = m_rpcId->stationName(channelId);
    name        = m_rpcId->stationNameString(SGStationNber);
    eta         = m_rpcId->stationEta(channelId);
    phi         = m_rpcId->stationPhi(channelId);
    doubletR    = m_rpcId->doubletR(channelId);
    doubletZ    = m_rpcId->doubletZ(channelId);
    doubletPhi  = m_rpcId->doubletPhi(channelId);
    gasGap      = m_rpcId->gasGap(channelId);
    measuresPhi = m_rpcId->measuresPhi(channelId);
    strip       = m_rpcId->strip(channelId);
  }
  return StatusCode::SUCCESS;
}

StatusCode MboyEDMHelper::dump_tgc(Identifier channelId,std::ofstream* pOutCurrent){

  if(m_tgcId->is_tgc(channelId)){
    int         SGStationNber = m_tgcId->stationName(channelId);
    std::string SGStationName = m_tgcId->stationNameString(SGStationNber);
    int         SGStationEta  = m_tgcId->stationEta(channelId);
    int         SGStationPhi  = m_tgcId->stationPhi(channelId);
    int         SGGasGap      = m_tgcId->gasGap(channelId)  ;
    int         SGIsStrip     = m_tgcId->isStrip(channelId) ;
    int         SGChannel     = m_tgcId->channel(channelId) ;
    *pOutCurrent 
        << "TGC "
        << "  "
        << std::setw(3)<< SGStationName
        << std::setw(4)<< SGStationEta
        << std::setw(4)<< SGStationPhi
        << std::setw(4)<< SGGasGap
        << std::setw(4)<< SGIsStrip
        << std::setw(4)<< SGChannel
        << std::endl;
  }

  return StatusCode::SUCCESS;

}
StatusCode MboyEDMHelper::give_tgc(Identifier channelId, std::string& name, int& eta, int& phi, 
				   int& gasGap, int& isStrip, int& channel){
 
  if(m_tgcId->is_tgc(channelId)){
    int         SGStationNber = m_tgcId->stationName(channelId);
    name    = m_tgcId->stationNameString(SGStationNber);
    eta     = m_tgcId->stationEta(channelId);
    phi     = m_tgcId->stationPhi(channelId);
    gasGap  = m_tgcId->gasGap(channelId);
    isStrip = m_tgcId->isStrip(channelId);
    channel = m_tgcId->channel(channelId);
  }
  return StatusCode::SUCCESS;
}

StatusCode MboyEDMHelper::dump_csc(Identifier channelId,std::ofstream* pOutCurrent){

  if(m_cscId->is_csc(channelId)){
    int         SGStationNber  = m_cscId->stationName(channelId);
    std::string SGStationName  = m_cscId->stationNameString(SGStationNber);
    int         SGStationEta   = m_cscId->stationEta(channelId);
    int         SGStationPhi   = m_cscId->stationPhi(channelId);
    int         SGChamberLayer = m_cscId->chamberLayer(channelId) ;
    int         SGWireLayer    = m_cscId->wireLayer(channelId)    ;
    int         SGMeasuresPhi  = m_cscId->measuresPhi(channelId)  ;
    int         SGStrip        = m_cscId->strip(channelId)        ;
    *pOutCurrent 
        << "CSC "
        << "  "
        << std::setw(3)<< SGStationName
        << std::setw(4)<< SGStationEta
        << std::setw(4)<< SGStationPhi
        << std::setw(4)<< SGChamberLayer
        << std::setw(4)<< SGWireLayer
        << std::setw(4)<< SGMeasuresPhi
        << std::setw(4)<< SGStrip
        << std::endl;
  }

  return StatusCode::SUCCESS;

}
StatusCode MboyEDMHelper::give_csc(Identifier channelId,std::string& name, int& eta, int& phi, int& chamberLayer, int& wireLayer, int& measuresPhi, int& strip) {
  if(m_cscId->is_csc(channelId)){
    int         SGStationNber  = m_cscId->stationName(channelId);
    name         = m_cscId->stationNameString(SGStationNber);
    eta          = m_cscId->stationEta(channelId);
    phi          = m_cscId->stationPhi(channelId);
    chamberLayer = m_cscId->chamberLayer(channelId) ;
    wireLayer    = m_cscId->wireLayer(channelId)    ;
    measuresPhi  = m_cscId->measuresPhi(channelId)  ;
    strip        = m_cscId->strip(channelId)        ;
  }
  return StatusCode::SUCCESS;
}
MboyEDMHelper::StationType MboyEDMHelper::is_rotStation(const Trk::RIO_OnTrack* rot) {
 
  MboyEDMHelper::StationType station = Unknown;
 
  const Identifier& id = rot->identify();
  if (m_detID->is_muon(id)){
    if(m_mdtId->is_mdt(id)){
      station = Mdt;
    } else if(m_rpcId->is_rpc(id)) {
      if(m_rpcId->measuresPhi(id) == 1) station = RpcPhi;
      else station = RpcEta;
    } else if(m_tgcId->is_tgc(id)) {
      if(m_tgcId->isStrip(id) == 1) station = TgcPhi;
      else station = TgcEta;
    } else if(m_cscId->is_csc(id)) {
      if(m_cscId->measuresPhi(id) == 1) station = CscPhi;
      else station = CscEta;
    } else station = Unknown;
  }
  return station;
}
int MboyEDMHelper::tgcGasGap(Identifier channelId){
  int gasGap = m_tgcId->gasGap(channelId);
  return gasGap;
}

// Compute Eloss
StatusCode MboyEDMHelper::BackTrackingEloss(
 const Rec::TrackParticle* pTrackParticle, 
 double& Eloss){

  Eloss = 0. ;

  if ( pTrackParticle == 0 ) return( StatusCode::FAILURE );

  StatusCode scEloss = BackTrackingEloss( (pTrackParticle->originalTrack()) , Eloss) ;
  if ( scEloss.isFailure() ) return( StatusCode::FAILURE );

  return StatusCode::SUCCESS;

}
// StatusCode MboyEDMHelper::BackTrackingEloss(const Trk::Track* pTrack, double& Eloss){
// 
// 
//   Eloss = 0. ;
// 
//   if ( pTrack == 0) return StatusCode::FAILURE;
//   if ( pTrack->trackParameters() == 0) return StatusCode::FAILURE;
// 
//   std::vector<const Trk::TrackParameters*>::const_iterator TrkPar    = pTrack->trackParameters()->begin(); 
//   std::vector<const Trk::TrackParameters*>::const_iterator TrkParEnd = pTrack->trackParameters()->end(); 
// 
// //Find the MS entrance track
//   const Trk::AtaCylinder* pAtaCylinder_MS = 0 ;
//   const Trk::AtaDisc* pAtaDisc_MS = 0 ;
//   TrkPar    = pTrack->trackParameters()->begin(); 
//   for ( ; TrkPar!=TrkParEnd; ++TrkPar){
//     pAtaCylinder_MS = dynamic_cast<const Trk::AtaCylinder*>(*TrkPar);
//     if ( pAtaCylinder_MS == 0 ) continue ;
//     if (IsThisPositionBeyondThelimitCaloSpectro(
//          (pAtaCylinder_MS->position()).x(),
//          (pAtaCylinder_MS->position()).y(), 
//          (pAtaCylinder_MS->position()).z()) != 1 ) pAtaCylinder_MS = 0 ;
//     if ( pAtaCylinder_MS !=0 ) break;
//   }
//   if ( pAtaCylinder_MS == 0 ){
//     TrkPar    = pTrack->trackParameters()->begin(); 
//     for ( ; TrkPar!=TrkParEnd; ++TrkPar){
//       pAtaDisc_MS = dynamic_cast<const Trk::AtaDisc*>(*TrkPar);
//       if ( pAtaDisc_MS == 0 ) continue ;
//       if (IsThisPositionBeyondThelimitCaloSpectro(
//            (pAtaDisc_MS->position()).x(),
//            (pAtaDisc_MS->position()).y(),
//            (pAtaDisc_MS->position()).z()) != 1 ) pAtaDisc_MS = 0 ;
//       if ( pAtaDisc_MS !=0 ) break;
//     }
//   }
//   if ( pAtaCylinder_MS == 0 && pAtaDisc_MS == 0) return StatusCode::FAILURE;
// 
//   double E_MS = 0. ;
//   if ( pAtaCylinder_MS != 0 ) {
//     E_MS = sqrt ( (pAtaCylinder_MS->momentum()).x()*(pAtaCylinder_MS->momentum()).x()
//                 + (pAtaCylinder_MS->momentum()).y()*(pAtaCylinder_MS->momentum()).y()
//                 + (pAtaCylinder_MS->momentum()).z()*(pAtaCylinder_MS->momentum()).z() ) ;
//   }else{
//     E_MS = sqrt ( (pAtaDisc_MS->momentum()).x()*(pAtaDisc_MS->momentum()).x()
//                 + (pAtaDisc_MS->momentum()).y()*(pAtaDisc_MS->momentum()).y()
//                 + (pAtaDisc_MS->momentum()).z()*(pAtaDisc_MS->momentum()).z() ) ;
//   }
// 
// //Find the Kalo entrance track
//   const Trk::AtaCylinder* pAtaCylinder_KA = 0 ;
//   const Trk::AtaDisc* pAtaDisc_KA = 0 ;
//   TrkPar    = pTrack->trackParameters()->begin(); 
//   for ( ; TrkPar!=TrkParEnd; ++TrkPar){
//     pAtaCylinder_KA = dynamic_cast<const Trk::AtaCylinder*>(*TrkPar);
//     if ( pAtaCylinder_KA == 0 ) continue ;
//     if (IsThisPositionBeyondThelimitCaloSpectro(
//          (pAtaCylinder_KA->position()).x(),
//          (pAtaCylinder_KA->position()).y(),
//          (pAtaCylinder_KA->position()).z()) == 1 ) pAtaCylinder_KA = 0 ;
//     if ( pAtaCylinder_KA !=0 ) break;
//   }
//   if ( pAtaCylinder_KA == 0 ){
//     TrkPar    = pTrack->trackParameters()->begin(); 
//     for ( ; TrkPar!=TrkParEnd; ++TrkPar){
//       pAtaDisc_KA = dynamic_cast<const Trk::AtaDisc*>(*TrkPar);
//       if ( pAtaDisc_KA == 0 ) continue ;
//       if (IsThisPositionBeyondThelimitCaloSpectro(
//            (pAtaDisc_KA->position()).x(),
//            (pAtaDisc_KA->position()).y(),
//            (pAtaDisc_KA->position()).z()) == 1 ) pAtaDisc_KA = 0 ;
//       if ( pAtaDisc_KA !=0 ) break;
//     }
//   }
//   if ( pAtaCylinder_KA == 0 && pAtaDisc_KA == 0) return StatusCode::FAILURE;
// 
//   double E_KA = 0. ;
//   if ( pAtaCylinder_KA != 0 ) {
//     E_KA = sqrt ( (pAtaCylinder_KA->momentum()).x()*(pAtaCylinder_KA->momentum()).x()
//                 + (pAtaCylinder_KA->momentum()).y()*(pAtaCylinder_KA->momentum()).y()
//                 + (pAtaCylinder_KA->momentum()).z()*(pAtaCylinder_KA->momentum()).z() ) ;
//   }else{
//     E_KA = sqrt ( (pAtaDisc_KA->momentum()).x()*(pAtaDisc_KA->momentum()).x()
//                 + (pAtaDisc_KA->momentum()).y()*(pAtaDisc_KA->momentum()).y()
//                 + (pAtaDisc_KA->momentum()).z()*(pAtaDisc_KA->momentum()).z() ) ;
//   }
// 
//   Eloss = E_KA - E_MS  ;
// 
//   return StatusCode::SUCCESS;
// 
// }
StatusCode MboyEDMHelper::BackTrackingEloss(const Trk::Track* pTrack, double& Eloss){

  Eloss = 0. ;

  if ( pTrack == 0) return StatusCode::FAILURE;
  if ( pTrack->trackParameters() == 0) return StatusCode::FAILURE;

  std::vector<const Trk::TrackParameters*>::const_iterator TrkPar    = pTrack->trackParameters()->begin(); 
  std::vector<const Trk::TrackParameters*>::const_iterator TrkParEnd = pTrack->trackParameters()->end(); 

  double ToCaloMax    = 10000000. ;
  double EatCalo      =        0. ;
  double ToSpectroMax = 10000000. ;
  double EatSpectro   =        0. ;
  int FoundCalo    = 0 ;
  int FoundSpectro = 0 ;
  TrkPar    = pTrack->trackParameters()->begin(); 
  for ( ; TrkPar!=TrkParEnd; ++TrkPar){
    const Trk::TrackParameters* pMeasuredTrackParameters = dynamic_cast<const Trk::TrackParameters*>(*TrkPar);
    if ( pMeasuredTrackParameters ) {
      if (IsThisPositionBeyondThelimitCaloSpectro(
         ((*TrkPar)->position()).x(),
         ((*TrkPar)->position()).y(), 
         ((*TrkPar)->position()).z()) != 1 ) {
        FoundCalo = 1 ;
        double ToCalo = DistanceToCaloCylinder(
          ((*TrkPar)->position()).x(), 
          ((*TrkPar)->position()).y(), 
          ((*TrkPar)->position()).z() 
        );
        if (ToCalo < ToCaloMax ){
         ToCaloMax = ToCalo ;
         EatCalo = sqrt ( ((*TrkPar)->momentum()).x()*((*TrkPar)->momentum()).x()
                        + ((*TrkPar)->momentum()).y()*((*TrkPar)->momentum()).y()
                        + ((*TrkPar)->momentum()).z()*((*TrkPar)->momentum()).z() ) ;
        }
      }else{
        FoundSpectro = 1 ;
        double ToSpectro = DistanceToSpectroCylinder(
          ((*TrkPar)->position()).x(), 
          ((*TrkPar)->position()).y(), 
          ((*TrkPar)->position()).z() 
        );
        if (ToSpectro < ToSpectroMax ){
         ToSpectroMax = ToSpectro ;
         EatSpectro = sqrt ( ((*TrkPar)->momentum()).x()*((*TrkPar)->momentum()).x()
                           + ((*TrkPar)->momentum()).y()*((*TrkPar)->momentum()).y()
                           + ((*TrkPar)->momentum()).z()*((*TrkPar)->momentum()).z() ) ;
        }
      }
    }
  }

  if (( FoundCalo !=1 ) ||( FoundSpectro !=1 ))
  return StatusCode::FAILURE;
  
  Eloss = EatCalo - EatSpectro  ;

  return StatusCode::SUCCESS;

}
double MboyEDMHelper::DistanceToCaloCylinder(double x, double y, double z){

  double TheR     =  1050. ;
  double TheHalfZ =  3200. ;
  return DistanceToCylinder(TheHalfZ,TheR,x,y,z) ;

}
double MboyEDMHelper::DistanceToSpectroCylinder(double x, double y, double z){

  double TheR     =  4250. ;
  double TheHalfZ =  6820. ;
  return DistanceToCylinder(TheHalfZ,TheR,x,y,z) ;

}
double MboyEDMHelper::DistanceToCylinder( double TheHalfZ, double TheR, double x, double y, double z){

  double DeltaR = fabs( sqrt ( x*x + y*y ) - TheR     ) ;
  double DeltaZ = fabs( fabs( z )          - TheHalfZ ) ;

  double ToBeReturned = DeltaR ;
  if ( DeltaZ < ToBeReturned ) ToBeReturned = DeltaZ ;

  return ToBeReturned ;

}
int MboyEDMHelper::IsThisPositionBeyondThelimitCaloSpectro(double x, double y, double z){

  double RLimitCaloSpectro = 3000. ;
  double ZLimitCaloSpectro = 5000. ;

  double R = sqrt ( x*x + y*y ) ;
  if (R >= RLimitCaloSpectro ) return 1 ;
  if (fabs(z) >= fabs(ZLimitCaloSpectro) ) return 1;
  return 0 ;

}

// Compute Eloss
StatusCode MboyEDMHelper::BackTrackingEloss(
 const Rec::TrackParticle* muonSpecTP, 
 const Rec::TrackParticle* extrapTP, 
 double& Eloss){

  Eloss = 0. ;

  if ( muonSpecTP == 0 ) return( StatusCode::FAILURE );
  if ( extrapTP == 0 ) return( StatusCode::FAILURE );

  StatusCode scEloss = BackTrackingEloss( (muonSpecTP->originalTrack()), (extrapTP->originalTrack()) ,Eloss) ;
  if ( scEloss.isFailure() ) return( StatusCode::FAILURE );

  return StatusCode::SUCCESS;

}
StatusCode MboyEDMHelper::BackTrackingEloss(
 const Trk::Track* muonSpecTT,
 const Trk::Track* extrapTT, 
 double& Eloss){

  Eloss = 0. ;

  if ( muonSpecTT == 0 ) return( StatusCode::FAILURE );
  if ( extrapTT == 0 )   return( StatusCode::FAILURE );

  double E_muonSpecTT = sqrt ( 
                        ((muonSpecTT->perigeeParameters())->momentum()).x()*((muonSpecTT->perigeeParameters())->momentum()).x()
                      + ((muonSpecTT->perigeeParameters())->momentum()).y()*((muonSpecTT->perigeeParameters())->momentum()).y()
                      + ((muonSpecTT->perigeeParameters())->momentum()).z()*((muonSpecTT->perigeeParameters())->momentum()).z() ) ;

  double E_extrapTT  = sqrt ( 
                       ((extrapTT->perigeeParameters())->momentum()).x()*((extrapTT->perigeeParameters())->momentum()).x()
                     + ((extrapTT->perigeeParameters())->momentum()).y()*((extrapTT->perigeeParameters())->momentum()).y()
                     + ((extrapTT->perigeeParameters())->momentum()).z()*((extrapTT->perigeeParameters())->momentum()).z() ) ;

  Eloss = E_extrapTT - E_muonSpecTT  ;

  return StatusCode::SUCCESS;

}

// Segment Associated Station Name
int MboyEDMHelper::SegmentAssociatedStationIndex(const Trk::Segment*  pSegment){

  int ToBeReturned = -1 ;

  std::string TheStationName = SegmentAssociatedStationName(pSegment);
  if (TheStationName != "XXX" ) ToBeReturned = m_mdtId->stationNameIndex(TheStationName);

  return ToBeReturned ;

}

std::string MboyEDMHelper::SegmentAssociatedStationName(const Trk::Segment*  pSegment){

  std::string ToBeReturned = "XXX" ;

  if ( IsValid(pSegment) == 0 ) return ToBeReturned;
  const Muon::MuonSegment* pMuonSegment = dynamic_cast<const Muon::MuonSegment*>(pSegment);
  if (pMuonSegment==0) return ToBeReturned; 

//JFL 06/09/16 Let the segment with no RDO be named CSC
//JFL 06/09/16 NB: no station has this name
  ToBeReturned = "CSC" ;

  std::vector<int>          NberOfRot ;
  std::vector<std::string>  NameOfStation ;
  const std::vector<const Trk::RIO_OnTrack*> pRIOSet = pMuonSegment->containedROTs();
  std::vector<const Trk::RIO_OnTrack*>::const_iterator rotIter = pRIOSet.begin();
  for (; rotIter!=pRIOSet.end(); ++rotIter){
    if ( (*rotIter)!=0 ){
      const Identifier& id = (*rotIter)->identify();
      if (m_detID->is_muon(id)){
        std::string SGStationName ;
        int ThereIsARotToSort = 0 ;
        if(m_mdtId->is_mdt(id)){
          SGStationName     = m_mdtId->stationNameString(m_mdtId->stationName(id));
          ThereIsARotToSort = 1 ;
        }
        if(m_cscId->is_csc(id)){
          if(m_cscId->measuresPhi(id) == 0 ){
            SGStationName     = m_cscId->stationNameString(m_cscId->stationName(id));
            ThereIsARotToSort = 1 ;
          }
        }
        if (ThereIsARotToSort == 1){
          int NameOfStationSize = NameOfStation.size();
          int IsFound = 0 ;
          for (int ItemNber=0; ItemNber<NameOfStationSize ; ItemNber++){
           if (IsFound == 0 ){
             if ( NameOfStation[ItemNber] == SGStationName ){
               IsFound = 1 ;
               NberOfRot[ItemNber] = NberOfRot[ItemNber] + 1;
             }
           }
          }
          if ( IsFound == 0 ){
            NameOfStation.push_back(SGStationName) ;
            int IntToAdd = 1 ;
            NberOfRot.push_back(IntToAdd) ;
          }
        }
      }
    }
  }

  int IsFound = 0 ;
  std::string NameSelected = "XXX" ;
  int NameOfStationSize = NameOfStation.size();
  int NberOfRotMax = -1 ;
  for (int ItemNber=0; ItemNber<NameOfStationSize ; ItemNber++){
    if (NberOfRot[ItemNber] > NberOfRotMax){
      NberOfRotMax = NberOfRot[ItemNber] ;
      NameSelected = NameOfStation[ItemNber] ;
      IsFound = 1 ;
    }
  }
  if ( IsFound == 1 ) ToBeReturned = NameSelected ;

  return ToBeReturned ;

}

void MboyEDMHelper::SegmentAssociatedStationNames(const Trk::Segment*  pSegment, std::vector<std::string >& VectorOfStationNames){

  VectorOfStationNames.clear();
  
  if ( IsValid(pSegment) == 0 ) return ;
  const Muon::MuonSegment* pMuonSegment = dynamic_cast<const Muon::MuonSegment*>(pSegment);
  if (pMuonSegment==0) return ; 

  std::vector<int> VectorOfISC ; 
  std::vector<int> NberOfRot ;

  const std::vector<const Trk::RIO_OnTrack*> pRIOSet = pMuonSegment->containedROTs();
  std::vector<const Trk::RIO_OnTrack*>::const_iterator rotIter = pRIOSet.begin();
  for (; rotIter!=pRIOSet.end(); ++rotIter){
    if ( (*rotIter)!=0 ){
      const Identifier& id = (*rotIter)->identify();
      if (m_detID->is_muon(id)){
        std::string SGStationName ;
        int theISC = 0 ;
        int ThereIsARotToSort = 0 ;
        if(m_mdtId->is_mdt(id)){
          SGStationName     = m_mdtId->stationNameString(m_mdtId->stationName(id));
          theISC = GetISC(m_mdtId->stationName(id));
          ThereIsARotToSort = 1 ;
        }
        if(m_cscId->is_csc(id)){
          if(m_cscId->measuresPhi(id) == 0 ){
            SGStationName     = m_cscId->stationNameString(m_cscId->stationName(id));
            theISC = GetISC(m_cscId->stationName(id));
            ThereIsARotToSort = 1 ;
          }
        }
        if (ThereIsARotToSort == 1){
          int VectorOfISCSize = VectorOfISC.size();
          int IsFound = 0 ;
          for (int ItemNber=0; ItemNber<VectorOfISCSize ; ItemNber++){
           if (IsFound == 0 ){
             if ( VectorOfISC[ItemNber] == theISC ){
               IsFound = 1 ;
               NberOfRot[ItemNber] = NberOfRot[ItemNber] + 1;
             }
           }
          }
          if ( IsFound == 0 ){
            VectorOfStationNames.push_back(SGStationName) ;
            VectorOfISC.push_back(theISC) ;
            int IntToAdd = 1 ;
            NberOfRot.push_back(IntToAdd) ;
          }
        }
      }
    }
  }

}

int MboyEDMHelper::StationLevel( const std::string& TheSegmentStationName )
{
  char StationNameFragment[1];
  StationNameFragment[0]= TheSegmentStationName[1];
  if      ( StationNameFragment[0] == 'O' ) return 2;
  else if ( StationNameFragment[0] == 'M' ) return 1;
  else if ( StationNameFragment[0] == 'I' ) return 0;
  return -1;
}

bool MboyEDMHelper::IsEndCap( const std::string& TheSegmentStationName )
{
  char StationNameFragment[1];
  StationNameFragment[0]= TheSegmentStationName[0];
  if ( StationNameFragment[0] == 'B' ) return 0;
  else return 1;
}

bool MboyEDMHelper::IsLargeSector( const std::string& TheSegmentStationName )
{
  char StationNameFragment[1];
  StationNameFragment[0]= TheSegmentStationName[2];
  if ( StationNameFragment[0] == 'S' ) return 0;
  else return 1;
}

// 
int MboyEDMHelper::GetNberOfPrecisionDigitsInThisTrack(
 const Trk::Segment*  pSegment, 
 const Trk::Track*    pTrack){ 

  int ToBeReturned = 0 ; 
 
  if ( IsValid(pSegment) == 0 ) return ToBeReturned; 
  const Muon::MuonSegment* pMuonSegment = dynamic_cast<const Muon::MuonSegment*>(pSegment); 
  if (pMuonSegment==0) return ToBeReturned; 
 
  if ( pTrack == 0 ) return 0 ; 
  if ( pTrack->measurementsOnTrack() == 0 ) return 0 ; 
 
  int NberOfRotShared = 0; 
  const std::vector<const Trk::RIO_OnTrack*> pRIOSet = pMuonSegment->containedROTs(); 
  std::vector<const Trk::RIO_OnTrack*>::const_iterator rotIter = pRIOSet.begin(); 
  for (; rotIter!=pRIOSet.end(); ++rotIter){ 
    if ( (*rotIter)!=0 ){ 
      int Is1stCoord = 0 ; 
      if ( m_mdtId->is_mdt( (*rotIter)->identify() ) ) Is1stCoord = 1; 
      if ( m_cscId->is_csc( (*rotIter)->identify() ) ){ 
        if(m_cscId->measuresPhi( (*rotIter)->identify() ) == 0 ) Is1stCoord = 1; 
      } 
      if ( Is1stCoord  == 1 ){ 
        if (IsThisRotInThisTrack( (*rotIter) , pTrack ) == 1 ) NberOfRotShared = NberOfRotShared + 1; 
      } 
    } 
  } 

  ToBeReturned = NberOfRotShared ; 
 
  return ToBeReturned; 
 
} 

// Is this Segment in this Track?
int MboyEDMHelper::IsThisSegmentInThisTrack(
 const Trk::Segment*  pSegment,
 const Trk::Track*    pTrack){

  int ToBeReturned = 0 ;

  if ( IsValid(pSegment) == 0 ) return ToBeReturned;
  const Muon::MuonSegment* pMuonSegment = dynamic_cast<const Muon::MuonSegment*>(pSegment);
  if (pMuonSegment==0) return ToBeReturned; 

  if ( pTrack == 0 ) return 0 ;
  if ( pTrack->measurementsOnTrack() == 0 ) return 0 ;

  int NberOfRot       = 0;
  int NberOfRotShared = 0;
  const std::vector<const Trk::RIO_OnTrack*> pRIOSet = pMuonSegment->containedROTs();
  std::vector<const Trk::RIO_OnTrack*>::const_iterator rotIter = pRIOSet.begin();
  for (; rotIter!=pRIOSet.end(); ++rotIter){
    if ( (*rotIter)!=0 ){
      int Is1stCoord = 0 ;
      if ( m_mdtId->is_mdt( (*rotIter)->identify() ) ) Is1stCoord = 1;
      if ( m_cscId->is_csc( (*rotIter)->identify() ) ){
        if(m_cscId->measuresPhi( (*rotIter)->identify() ) == 0 ) Is1stCoord = 1;
      }
      if ( Is1stCoord  == 1 ){
        NberOfRot = NberOfRot + 1;
        if (IsThisRotInThisTrack( (*rotIter) , pTrack ) == 1 ) NberOfRotShared = NberOfRotShared + 1;
      }
    }
  }
  int NberOfRotNotShared = NberOfRot - NberOfRotShared ;
  if (NberOfRotNotShared <= 2) ToBeReturned = 1 ;
  if (NberOfRot          == 0) ToBeReturned = 0 ;

  return ToBeReturned;

}

// Is this Rot in this Track?
int MboyEDMHelper::IsThisRotInThisTrack(
 const Trk::RIO_OnTrack* rot,
 const Trk::Track* pTrack){

  if ( rot == 0 ) return 0 ;
  if ( pTrack == 0 ) return 0 ;
  if ( pTrack->measurementsOnTrack() == 0 ) return 0 ;

  int ToBeReturned = 0 ;
  std::vector<const Trk::MeasurementBase*>::const_iterator im    = pTrack->measurementsOnTrack()->begin(); 
  std::vector<const Trk::MeasurementBase*>::const_iterator imEnd = pTrack->measurementsOnTrack()->end(); 
  for ( ; im!=imEnd; ++im){
    const Trk::RIO_OnTrack* RotTrack = dynamic_cast<const Trk::RIO_OnTrack*>(*im);
    if ( RotTrack != 0 ){
      int Test = AreTheseRotsIdentical(rot,RotTrack) ;
      if ( Test == 1 ) {
        ToBeReturned = 1 ;
        break ;
      }
    }
  }

  return ToBeReturned;

}

// Are these Rots Indentical?
int MboyEDMHelper::AreTheseRotsIdentical(
 const Trk::RIO_OnTrack* Rot1,
 const Trk::RIO_OnTrack* Rot2){

  if ( Rot1 == 0 ) return 0 ;
  if ( Rot2 == 0 ) return 0 ;

  const Identifier& Id1 = Rot1->identify() ;
  const Identifier& Id2 = Rot2->identify() ;
  if ( Id1 == Id2 ) return 1 ;

  return 0;

}

// Reconstructed Objects Validity
int MboyEDMHelper::IsValid(
 const Trk::Segment*  pSegment){

  int ToBeReturned = 0 ;

  if ( pSegment == 0 ) return ToBeReturned ;
  const Muon::MuonSegment* pMuonSegment = dynamic_cast<const Muon::MuonSegment*>(pSegment);
  if ( pMuonSegment == 0 ) return ToBeReturned;

  ToBeReturned = 1 ;

  return ToBeReturned ;

}

// Utilities for Segment
Amg::Vector3D MboyEDMHelper::GetXaxis(const Trk::Segment*  pSegment)
{
  double X,Y,Z;
  GetXaxis(pSegment,X,Y,Z);
  Amg::Vector3D v(X,Y,Z);
  return v;
}
void MboyEDMHelper::GetXaxis(
                 const Trk::Segment*  pSegment,
                 double& X_Xaxis,
                 double& Y_Xaxis,
                 double& Z_Xaxis
){

  X_Xaxis = 0. ;
  Y_Xaxis = 0. ;
  Z_Xaxis = 0. ;

  if ( IsValid(pSegment) != 0 ){
    const Muon::MuonSegment* pMuonSegment = dynamic_cast<const Muon::MuonSegment*>(pSegment);
    if (pMuonSegment!=0) {
      const Trk::PlaneSurface ThePlaneSurface =  pMuonSegment->associatedSurface() ;
      double dx = 1.;
      double dy = 0.;
      double dz = 0.;
      Amg::Vector3D LocXaxis(dx,dy,dz);
      const Amg::Vector3D GlobXaxis( (ThePlaneSurface.transform())*LocXaxis );
      X_Xaxis = GlobXaxis.x();
      Y_Xaxis = GlobXaxis.y();
      Z_Xaxis = GlobXaxis.z();
    }
  }

}
Amg::Vector3D MboyEDMHelper::GetYaxis(const Trk::Segment*  pSegment)
{
  double X,Y,Z;
  GetYaxis(pSegment,X,Y,Z);
  Amg::Vector3D v(X,Y,Z);
  return v;
}
void MboyEDMHelper::GetYaxis(
                 const Trk::Segment*  pSegment,
                 double& X_Yaxis,
                 double& Y_Yaxis,
                 double& Z_Yaxis
){

  X_Yaxis = 0. ;
  Y_Yaxis = 0. ;
  Z_Yaxis = 0. ;

  if ( IsValid(pSegment) != 0 ){
    const Muon::MuonSegment* pMuonSegment = dynamic_cast<const Muon::MuonSegment*>(pSegment);
    if (pMuonSegment!=0) {
      const Trk::PlaneSurface ThePlaneSurface =  pMuonSegment->associatedSurface() ;
      double dx = 0.;
      double dy = 1.;
      double dz = 0.;
      Amg::Vector3D LocYaxis(dx,dy,dz);
      const Amg::Vector3D GlobYaxis( (ThePlaneSurface.transform())*LocYaxis );
      X_Yaxis = GlobYaxis.x();
      Y_Yaxis = GlobYaxis.y();
      Z_Yaxis = GlobYaxis.z();
    }
  }

}
Amg::Vector3D MboyEDMHelper::GetPoint(const Trk::Segment*  pSegment)
{
  double X,Y,Z;
  GetPoint(pSegment,X,Y,Z);
  Amg::Vector3D v(X,Y,Z);
  return v;
}
void MboyEDMHelper::GetPoint(
 const Trk::Segment*  pSegment,
 double& Xpt,
 double& Ypt,
 double& Zpt
){

  Xpt = 0. ;
  Ypt = 0. ;
  Zpt = 0. ;

  if ( IsValid(pSegment) != 0 ){
    const Muon::MuonSegment* pMuonSegment = dynamic_cast<const Muon::MuonSegment*>(pSegment);
    if (pMuonSegment!=0) {
      const Amg::Vector3D TheGlobalPosition =  pMuonSegment->globalPosition() ;
      Xpt = TheGlobalPosition.x();
      Ypt = TheGlobalPosition.y();
      Zpt = TheGlobalPosition.z();
    }
  }

}
Amg::Vector3D MboyEDMHelper::GetDirection(const Trk::Segment*  pSegment)
{
  double X,Y,Z;
  GetDirection(pSegment,X,Y,Z);
  Amg::Vector3D v(X,Y,Z);
  return v;
}
void MboyEDMHelper::GetDirection(
 const Trk::Segment*  pSegment,
 double& Xdir,
 double& Ydir,
 double& Zdir
){

  Xdir = 0. ;
  Ydir = 0. ;
  Zdir = 0. ;

  if ( IsValid(pSegment) != 0 ){
    const Muon::MuonSegment* pMuonSegment = dynamic_cast<const Muon::MuonSegment*>(pSegment);
    if (pMuonSegment!=0) {
      const Amg::Vector3D TheGlobalDirection =  pMuonSegment->globalDirection() ;
      Xdir = TheGlobalDirection.x();
      Ydir = TheGlobalDirection.y();
      Zdir = TheGlobalDirection.z();
    }
  }

}
double MboyEDMHelper::GetfitQuality(
 const Trk::Segment*  pSegment
){

  double ToBeReturned = 9999. ;

  if ( IsValid(pSegment) != 0 ){
    const Trk::FitQuality* pFitQuality = pSegment->fitQuality() ;
    ToBeReturned = pFitQuality->chiSquared () ;
  }

  return ToBeReturned ;

}
// nd = total number of digits
// ndMDT1 = number of digits on 1 MDT multilayer
// ndMDT2 = number of digits on the other MDT multilayer
void MboyEDMHelper::GetNberOfDigi(
				   const Trk::Segment*  pSegment, 
				   int& nd, int& ndMDT1, int& ndMDT2
){

  nd  = 0 ;
  ndMDT1 = 0 ;
  ndMDT2 = 0 ;

  if ( IsValid(pSegment) != 0 ){
    const Muon::MuonSegment* pMuonSegment = dynamic_cast<const Muon::MuonSegment*>(pSegment);
    if (pMuonSegment!=0) {
      const std::vector<const Trk::RIO_OnTrack*> pRIOSet = pMuonSegment->containedROTs();
      std::vector<const Trk::RIO_OnTrack*>::const_iterator rotIter = pRIOSet.begin();
      for (; rotIter!=pRIOSet.end(); ++rotIter){
        if ( (*rotIter)!=0 ){
          const Identifier& id = (*rotIter)->identify();
          if (m_detID->is_muon(id)){
            if( m_mdtId->is_mdt(id) 
	      || m_rpcId->is_rpc(id)
	      || m_tgcId->is_tgc(id)
	      || m_cscId->is_csc(id))
	    {
	      ++nd;
	      if(m_mdtId->is_mdt(id))
		{
		  int idML = m_mdtId->multilayer(id);
		  if(1==idML)++ndMDT1;
		  else ++ndMDT2;
		}
	    }
          }
        }
      } 
    } 
  }

}

int MboyEDMHelper::GetNberOfDigi(
 const Trk::Segment*  pSegment
){

  int ToBeReturned = 0 ;

  if ( IsValid(pSegment) != 0 ){
    const Muon::MuonSegment* pMuonSegment = dynamic_cast<const Muon::MuonSegment*>(pSegment);
    if (pMuonSegment!=0) {
      const std::vector<const Trk::RIO_OnTrack*> pRIOSet = pMuonSegment->containedROTs();
      std::vector<const Trk::RIO_OnTrack*>::const_iterator rotIter = pRIOSet.begin();
      for (; rotIter!=pRIOSet.end(); ++rotIter){
        if ( (*rotIter)!=0 ){
          const Identifier& id = (*rotIter)->identify();
          if (m_detID->is_muon(id)){
            if( m_mdtId->is_mdt(id) 
             || m_rpcId->is_rpc(id)
             || m_tgcId->is_tgc(id)
             || m_cscId->is_csc(id))
             ToBeReturned = ToBeReturned + 1 ;
          }
        }
      } 
    } 
  }

  return ToBeReturned ;

}

int MboyEDMHelper::GetNberOfDigiMDT(
 const Trk::Segment*  pSegment
){

  int ToBeReturned = 0 ;

  if ( IsValid(pSegment) != 0 ){
    const Muon::MuonSegment* pMuonSegment = dynamic_cast<const Muon::MuonSegment*>(pSegment);
    if (pMuonSegment!=0) {
      const std::vector<const Trk::RIO_OnTrack*> pRIOSet = pMuonSegment->containedROTs();
      std::vector<const Trk::RIO_OnTrack*>::const_iterator rotIter = pRIOSet.begin();
      for (; rotIter!=pRIOSet.end(); ++rotIter){
        if ( (*rotIter)!=0 ){
          const Identifier& id = (*rotIter)->identify();
          if (m_detID->is_muon(id)){
            if( m_mdtId->is_mdt(id))
             ToBeReturned = ToBeReturned + 1 ;
          }
        }
      } 
    } 
  }

  return ToBeReturned ;

}

int MboyEDMHelper::GetNberOfDigiRPC(
 const Trk::Segment*  pSegment
){

  int ToBeReturned = 0 ;

  if ( IsValid(pSegment) != 0 ){
    const Muon::MuonSegment* pMuonSegment = dynamic_cast<const Muon::MuonSegment*>(pSegment);
    if (pMuonSegment!=0) {
      const std::vector<const Trk::RIO_OnTrack*> pRIOSet = pMuonSegment->containedROTs();
      std::vector<const Trk::RIO_OnTrack*>::const_iterator rotIter = pRIOSet.begin();
      for (; rotIter!=pRIOSet.end(); ++rotIter){
        if ( (*rotIter)!=0 ){
          const Identifier& id = (*rotIter)->identify();
          if (m_detID->is_muon(id)){
            if( m_rpcId->is_rpc(id))
             ToBeReturned = ToBeReturned + 1 ;
          }
        }
      } 
    } 
  }

  return ToBeReturned ;

}

int MboyEDMHelper::GetNberOfDigiTGC(
 const Trk::Segment*  pSegment
){

  int ToBeReturned = 0 ;

  if ( IsValid(pSegment) != 0 ){
    const Muon::MuonSegment* pMuonSegment = dynamic_cast<const Muon::MuonSegment*>(pSegment);
    if (pMuonSegment!=0) {
      const std::vector<const Trk::RIO_OnTrack*> pRIOSet = pMuonSegment->containedROTs();
      std::vector<const Trk::RIO_OnTrack*>::const_iterator rotIter = pRIOSet.begin();
      for (; rotIter!=pRIOSet.end(); ++rotIter){
        if ( (*rotIter)!=0 ){
          const Identifier& id = (*rotIter)->identify();
          if (m_detID->is_muon(id)){
            if( m_tgcId->is_tgc(id))
             ToBeReturned = ToBeReturned + 1 ;
          }
        }
      } 
    } 
  }

  return ToBeReturned ;

}

int MboyEDMHelper::GetNberOfDigiCSC(
 const Trk::Segment*  pSegment
){

  int ToBeReturned = 0 ;

  if ( IsValid(pSegment) != 0 ){
    const Muon::MuonSegment* pMuonSegment = dynamic_cast<const Muon::MuonSegment*>(pSegment);
    if (pMuonSegment!=0) {
      const std::vector<const Trk::RIO_OnTrack*> pRIOSet = pMuonSegment->containedROTs();
      std::vector<const Trk::RIO_OnTrack*>::const_iterator rotIter = pRIOSet.begin();
      for (; rotIter!=pRIOSet.end(); ++rotIter){
        if ( (*rotIter)!=0 ){
          const Identifier& id = (*rotIter)->identify();
          if (m_detID->is_muon(id)){
            if( m_cscId->is_csc(id)){
              ToBeReturned = ToBeReturned + 1 ;
            }
          }
        }
      } 
    } 
  }

  return ToBeReturned ;

}

int MboyEDMHelper::GetNberOfDigiCSC1(
 const Trk::Segment*  pSegment
){

  int ToBeReturned = 0 ;

  if ( IsValid(pSegment) != 0 ){
    const Muon::MuonSegment* pMuonSegment = dynamic_cast<const Muon::MuonSegment*>(pSegment);
    if (pMuonSegment!=0) {
      const std::vector<const Trk::RIO_OnTrack*> pRIOSet = pMuonSegment->containedROTs();
      std::vector<const Trk::RIO_OnTrack*>::const_iterator rotIter = pRIOSet.begin();
      for (; rotIter!=pRIOSet.end(); ++rotIter){
        if ( (*rotIter)!=0 ){
          const Identifier& id = (*rotIter)->identify();
          if (m_detID->is_muon(id)){
            if( m_cscId->is_csc(id)){
              if( !m_cscId->measuresPhi(id))
               ToBeReturned = ToBeReturned + 1 ;
            }
          }
        }
      } 
    } 
  }

  return ToBeReturned ;

}

int MboyEDMHelper::GetNberOfDigiCSC2(
 const Trk::Segment*  pSegment
){

  int ToBeReturned = 0 ;

  if ( IsValid(pSegment) != 0 ){
    const Muon::MuonSegment* pMuonSegment = dynamic_cast<const Muon::MuonSegment*>(pSegment);
    if (pMuonSegment!=0) {
      const std::vector<const Trk::RIO_OnTrack*> pRIOSet = pMuonSegment->containedROTs();
      std::vector<const Trk::RIO_OnTrack*>::const_iterator rotIter = pRIOSet.begin();
      for (; rotIter!=pRIOSet.end(); ++rotIter){
        if ( (*rotIter)!=0 ){
          const Identifier& id = (*rotIter)->identify();
          if (m_detID->is_muon(id)){
            if( m_cscId->is_csc(id)){
              if( m_cscId->measuresPhi(id))
               ToBeReturned = ToBeReturned + 1 ;
            }
          }
        }
      } 
    } 
  }

  return ToBeReturned ;

}

int MboyEDMHelper::GetNberOfPrecisionDigitsInCommon(
						     const Trk::Segment*  pSegment,
						     const Trk::Segment*  pSegment2
						     ){
  
  int ToBeReturned = 0 ;
  
  if ( IsValid(pSegment) != 0 ){
    const Muon::MuonSegment* pMuonSegment = dynamic_cast<const Muon::MuonSegment*>(pSegment);
    if (pMuonSegment!=0) {
      const std::vector<const Trk::RIO_OnTrack*> pRIOSet = pMuonSegment->containedROTs();
      std::vector<const Trk::RIO_OnTrack*>::const_iterator rotIter = pRIOSet.begin();
      for (; rotIter!=pRIOSet.end(); ++rotIter){
        if ( (*rotIter)!=0 ){
          const Identifier& id = (*rotIter)->identify();
          if (m_detID->is_muon(id)){
            if( m_mdtId->is_mdt(id) || (m_cscId->is_csc(id) && m_cscId->measuresPhi(id)) ){
	    //
	    if ( IsValid(pSegment2) != 0 ){
	      const Muon::MuonSegment* pMuonSegment2 = dynamic_cast<const Muon::MuonSegment*>(pSegment2);
              if (pMuonSegment2!=0) {
	        const std::vector<const Trk::RIO_OnTrack*> pRIOSet2 = pMuonSegment2->containedROTs();
	        std::vector<const Trk::RIO_OnTrack*>::const_iterator rotIter2 = pRIOSet2.begin();
	        for (; rotIter2!=pRIOSet2.end(); ++rotIter2){
		if ( (*rotIter2)!=0 ){
		  const Identifier& id2 = (*rotIter2)->identify();
		  if (id == id2){
		    ToBeReturned = ToBeReturned + 1 ;
		  }
		}
	        }
	      }
	    }
	  }
	}
        }
      }
    }
  } 

  return ToBeReturned ;

}

void MboyEDMHelper::GetNberOfDigiVec(
				      const Trk::Segment*  pSegment,
				      std::vector<int>& etaDigitVec, 
				      std::vector<int>& phiDigitVec 
				      ){
  int nmdt(0);
  int nrpcEta(0);
  int ntgcEta(0);
  int ncscEta(0);
  int nrpcPhi(0);
  int ntgcPhi(0);
  int ncscPhi(0);
  if ( IsValid(pSegment) != 0 ){
    const Muon::MuonSegment* pMuonSegment = dynamic_cast<const Muon::MuonSegment*>(pSegment);
    if (pMuonSegment!=0) {
      const std::vector<const Trk::RIO_OnTrack*> pRIOSet = pMuonSegment->containedROTs();
      std::vector<const Trk::RIO_OnTrack*>::const_iterator rotIter = pRIOSet.begin();
      for (; rotIter!=pRIOSet.end(); ++rotIter){
        if ( (*rotIter)!=0 ){
          const Identifier& id = (*rotIter)->identify();
          if (m_detID->is_muon(id)) {
	  if( m_mdtId->is_mdt(id) ) ++nmdt;
	  if( m_rpcId->is_rpc(id) )
	    {
	      if(m_rpcId->measuresPhi(id))++nrpcPhi;
	      else ++nrpcEta;
	    }
	  if( m_tgcId->is_tgc(id) )
	    {
	      if(m_tgcId->isStrip(id))++ntgcPhi;
	      else ++ntgcEta;
	    }
	  if( m_cscId->is_csc(id) ) 
	    {
	      if(m_cscId->measuresPhi(id))++ncscPhi;
	      else ++ncscEta;
	    }
	}
        }
      } 
    } 
  }
  etaDigitVec.clear();
  etaDigitVec.push_back(nmdt); // MDTs provide Eta measurements only
  etaDigitVec.push_back(ncscEta);
  etaDigitVec.push_back(nrpcEta);
  etaDigitVec.push_back(ntgcEta);
  phiDigitVec.clear();
  phiDigitVec.push_back(0);
  phiDigitVec.push_back(ncscPhi);
  phiDigitVec.push_back(nrpcPhi);
  phiDigitVec.push_back(ntgcPhi);

  return;
}

bool MboyEDMHelper::SegmentHas2ndCoordMeasurement(const Trk::Segment* pSegment)
{
  if ( IsValid(pSegment) != 0 ){
    const Muon::MuonSegment* pMuonSegment = dynamic_cast<const Muon::MuonSegment*>(pSegment);
    if (pMuonSegment!=0) {
      const std::vector<const Trk::RIO_OnTrack*> pRIOSet = pMuonSegment->containedROTs();
      std::vector<const Trk::RIO_OnTrack*>::const_iterator rotIter = pRIOSet.begin();
      for (; rotIter!=pRIOSet.end(); ++rotIter){
        if ( (*rotIter)!=0 ){
          const Identifier& id = (*rotIter)->identify();
          if (m_detID->is_muon(id)) {
	  if( m_rpcId->is_rpc(id) && m_rpcId->measuresPhi(id) ) return true;
	  if( m_tgcId->is_tgc(id) && m_tgcId->isStrip(id)     ) return true;
	  if( m_cscId->is_csc(id) && m_cscId->measuresPhi(id) ) return true; 
	}
        }
      } 
    } 
  }
  return false;
}

void MboyEDMHelper::GiveTrackParametersAtBeamAndCalo(
                                           const Rec::TrackParticle* pTrackParticle,
                                           const Trk::TrackParameters*& pTrackParametersAtBeam,
                                           const Trk::TrackParameters*& pTrackParametersAtCalo,
                                           const Trk::TrackParameters*& pTrackParametersAtSpectro
){

  const Trk::Track* pTrack    = pTrackParticle->originalTrack();
  GiveTrackParametersAtBeamAndCalo(
  				   pTrack,
  				   pTrackParametersAtBeam,
  				   pTrackParametersAtCalo,
  				   pTrackParametersAtSpectro
  				  );

}

void MboyEDMHelper::GiveTrackParametersAtBeamAndCalo(
                                           const Trk::Track* pTrack,
                                           const Trk::TrackParameters*& pTrackParametersAtBeam,
                                           const Trk::TrackParameters*& pTrackParametersAtCalo,
                                           const Trk::TrackParameters*& pTrackParametersAtSpectro
){

  pTrackParametersAtBeam = 0; 
  pTrackParametersAtCalo = 0; 
  pTrackParametersAtSpectro = 0; 

  std::vector<const Trk::TrackParameters*>::const_iterator TrkPar    = pTrack->trackParameters()->begin(); 
  std::vector<const Trk::TrackParameters*>::const_iterator TrkParEnd = pTrack->trackParameters()->end(); 
  for ( ; TrkPar!=TrkParEnd; ++TrkPar){
    if ( pTrackParametersAtBeam == 0 ) {;
      const Trk::Perigee* pMeasuredPerigee = dynamic_cast<const Trk::Perigee*>(*TrkPar);
      if ( pMeasuredPerigee !=0 ) pTrackParametersAtBeam = *TrkPar ;
    }

    if ( pTrackParametersAtCalo == 0  ) {
      const Trk::AtaCylinder* pMeasuredAtaCylinder = dynamic_cast<const Trk::AtaCylinder*>(*TrkPar);
      if ( pMeasuredAtaCylinder !=0 ) {
        double TrkXpt = (pMeasuredAtaCylinder->position()).x() ;
        double TrkYpt = (pMeasuredAtaCylinder->position()).y() ;
        double TrkZpt = (pMeasuredAtaCylinder->position()).z() ;
        if (IsThisPositionBeyondThelimitCaloSpectro(TrkXpt,TrkYpt,TrkZpt) != 1 )  pTrackParametersAtCalo = *TrkPar ; 
      }
    }
    if ( pTrackParametersAtCalo == 0  ) {
      const Trk::AtaDisc* pMeasuredAtaDisc = dynamic_cast<const Trk::AtaDisc*>(*TrkPar);
      if ( pMeasuredAtaDisc !=0 ) {
        double TrkXpt = (pMeasuredAtaDisc->position()).x() ;
        double TrkYpt = (pMeasuredAtaDisc->position()).y() ;
        double TrkZpt = (pMeasuredAtaDisc->position()).z() ;
        if (IsThisPositionBeyondThelimitCaloSpectro(TrkXpt,TrkYpt,TrkZpt) != 1 ) pTrackParametersAtCalo = *TrkPar ; 
      }
    }

    if ( pTrackParametersAtSpectro == 0  ) {
      const Trk::AtaCylinder* pMeasuredAtaCylinder = dynamic_cast<const Trk::AtaCylinder*>(*TrkPar);
      if ( pMeasuredAtaCylinder !=0 ) {
        double TrkXpt = (pMeasuredAtaCylinder->position()).x() ;
        double TrkYpt = (pMeasuredAtaCylinder->position()).y() ;
        double TrkZpt = (pMeasuredAtaCylinder->position()).z() ;
        if (IsThisPositionBeyondThelimitCaloSpectro(TrkXpt,TrkYpt,TrkZpt) == 1 )  pTrackParametersAtSpectro = *TrkPar ; 
      }
    }
    if ( pTrackParametersAtSpectro == 0  ) {
      const Trk::AtaDisc* pMeasuredAtaDisc = dynamic_cast<const Trk::AtaDisc*>(*TrkPar);
      if ( pMeasuredAtaDisc !=0 ) {
        double TrkXpt = (pMeasuredAtaDisc->position()).x() ;
        double TrkYpt = (pMeasuredAtaDisc->position()).y() ;
        double TrkZpt = (pMeasuredAtaDisc->position()).z() ;
        if (IsThisPositionBeyondThelimitCaloSpectro(TrkXpt,TrkYpt,TrkZpt) == 1 ) pTrackParametersAtSpectro = *TrkPar ; 
      }
    }

  }

}
// Returns the segment covariance matrix:
//      - the diagonal in error[4]
//      - the two correlation terms in corX and corY (measurements X and Y are assumed to be uncorrelated)
// 0 = X meas. (phi)
// 1 = X angle meas. (phi)
// 2 = Y meas. (theta)
// 3 = Y angle meas. (theta)
void MboyEDMHelper::GetSegmentCovMatrix(const Trk::Segment*  pSegment, 
					 double error[4], double& corX, double& corY)
{
  error[0] = 1. ;
  error[1] = 1. ;
  error[2] = 1. ;
  error[3] = 1. ;
  corX = 0. ;
  corY = 0. ;
  const Muon::MuonSegment* pMuonSegment = dynamic_cast<const Muon::MuonSegment*>(pSegment);
  if (pMuonSegment!=0) {
    const Amg::MatrixX TheCovarianceMatrix =  pMuonSegment->localCovariance() ;
    error[0] = TheCovarianceMatrix(0,0);
    error[1] = TheCovarianceMatrix(1,1);
    error[2] = TheCovarianceMatrix(2,2);
    error[3] = TheCovarianceMatrix(3,3);
    corX = TheCovarianceMatrix(0,1);
    corY = TheCovarianceMatrix(2,3);
  }
}

//--------------------------------------------------------------------------
double MboyEDMHelper::parametrisedEnergyLoss (double Eta, double Momentum)
{
 Eta=fabs(Eta);
  Momentum=fabs(Momentum)/1000.;
  
  int iEtaBin=-1;

  if(fabs(Eta)<2.7)
    {
      iEtaBin= (int) floor(fabs(Eta)/0.1);
      if(iEtaBin<0)iEtaBin=0;
      if(iEtaBin>26)iEtaBin=26;
    }
  else
    {
      iEtaBin=26;
    }
   
  double a[27]={
    2.9165,
    2.8834,
    2.8793,
    3.0382,
    2.7634,
    3.4017,
    3.2109,
    2.9480,
    3.0102,
    3.3006,
    3.8654,
    4.3560,
    4.7883,
    3.7185,
    3.4498,
    2.5946,
    2.4897,
    2.7347,
    3.5597,
    4.0374,
    4.2499,
    3.4551,
    6.0868,
    6.5463,
    5.3052,
    3.3856,
    3.6250
  };
   
  double b[27]={
    0.89853E-02,
    0.78984E-02,
    0.98239E-02,
    0.89140E-02,
    0.10773E-01,
    0.91722E-02,
    0.92958E-02,
    0.10042E-01,
    0.89647E-02,
    0.92212E-02,
    0.91197E-02,
    0.13101E-01,
    0.13626E-01,
    0.10319E-01,
    0.84956E-02,
    0.84110E-02,
    0.94134E-02,
    0.93789E-02,
    0.11143E-01,
    0.93488E-02,
    0.11773E-01,
    0.12367E-01,
    0.99344E-02,
    0.11851E-01,
    0.11106E-01,
    0.97579E-02,
    0.10341E-01
  };

  double Eloss=3000.;
  double GeVToMeV=1000.;

  Eloss=(a[iEtaBin]+b[iEtaBin]*Momentum)*GeVToMeV;

  return Eloss;   
}

int MboyEDMHelper::GetISC(const Trk::MeasurementBase* pMeasurementBase){
  if ( pMeasurementBase == 0 ) return -1 ;
  const Trk::RIO_OnTrack* pRIO_OnTrack = dynamic_cast<const Trk::RIO_OnTrack*>(pMeasurementBase);
  return GetISC(pRIO_OnTrack);
}

int MboyEDMHelper::GetISC(const Trk::RIO_OnTrack* pRIO_OnTrack){
  if ( pRIO_OnTrack == 0 ) return -1 ;
  const Identifier channelId = pRIO_OnTrack->identify();
  if ( !m_detID->is_muon(channelId) ) return -1 ;
  const Trk::PrepRawData* pPrepRawData = pRIO_OnTrack->prepRawData() ; 
  if ( pPrepRawData == 0 ) return -1 ;
  const Muon::MdtPrepData* pMdtPrepData = dynamic_cast<const Muon::MdtPrepData*>(pPrepRawData);
  if (pMdtPrepData) return GetISC( pMdtPrepData->identify() );
  const Muon::CscPrepData* pCscPrepData = dynamic_cast<const Muon::CscPrepData*>(pPrepRawData);
  if (pCscPrepData) return GetISC( pCscPrepData->identify() );
  const Muon::RpcPrepData* pRpcPrepData = dynamic_cast<const Muon::RpcPrepData*>(pPrepRawData);
  if (pRpcPrepData) return GetISC( pRpcPrepData->identify() );
  const Muon::TgcPrepData* pTgcPrepData = dynamic_cast<const Muon::TgcPrepData*>(pPrepRawData);
  if (pTgcPrepData) return GetISC( pTgcPrepData->identify() );
  return -1 ;
}

int MboyEDMHelper::GetISC(const Identifier& channelId){
  if ( !m_detID->is_muon(channelId) ) return -1 ;
  if ( m_mdtId->is_mdt(channelId) ) return GetISC( m_mdtId->stationName(channelId) ) ;
  if ( m_cscId->is_csc(channelId) ) return GetISC( m_cscId->stationName(channelId) ) ;
  if ( m_rpcId->is_rpc(channelId) ) return GetISC( m_rpcId->stationName(channelId) ) ;
  if ( m_tgcId->is_tgc(channelId) ) return GetISC( m_tgcId->stationName(channelId) ) ;
  return -1 ;
}

int MboyEDMHelper::GetISC(int StationNber){ 
  if ( StationNber < 0 ) return -1 ;
  if ( StationNber >= int (mb_ISC.size() ) ) return -1 ;
  return mb_ISC[StationNber] ; 
}

void MboyEDMHelper::SetISC(){
   mb_ISC.clear(); 
   mb_ISC.push_back( 2 );
   mb_ISC.push_back( 1 );
   mb_ISC.push_back( 4 );
   mb_ISC.push_back( 3 );
   mb_ISC.push_back( 6 );
   mb_ISC.push_back( 5 );
   mb_ISC.push_back( 8 );
   mb_ISC.push_back( 7 );
   mb_ISC.push_back( 3 );
   mb_ISC.push_back( 5 );
   mb_ISC.push_back( 5 );
   mb_ISC.push_back( 5 );
   mb_ISC.push_back(10 );
   mb_ISC.push_back(10 );
   mb_ISC.push_back(12 );
   mb_ISC.push_back(11 );
   mb_ISC.push_back(14 );
   mb_ISC.push_back(14 );
   mb_ISC.push_back(13 );
   mb_ISC.push_back(18 );
   mb_ISC.push_back(16 );
   mb_ISC.push_back(15 );
   mb_ISC.push_back(99 );
   mb_ISC.push_back(99 );
   mb_ISC.push_back(99 );
   mb_ISC.push_back(99 );
   mb_ISC.push_back(99 );
   mb_ISC.push_back(99 );
   mb_ISC.push_back(99 );
   mb_ISC.push_back(99 );
   mb_ISC.push_back(99 );
   mb_ISC.push_back( 0 );
   mb_ISC.push_back( 0 );
   mb_ISC.push_back( 0 );
   mb_ISC.push_back( 0 );
   mb_ISC.push_back(10 );
   mb_ISC.push_back( 9 );
   mb_ISC.push_back(14 );
   mb_ISC.push_back(13 );
   mb_ISC.push_back(18 );
   mb_ISC.push_back(17 );
   mb_ISC.push_back(99 );
   mb_ISC.push_back(99 );
   mb_ISC.push_back(99 );
   mb_ISC.push_back(99 );
   mb_ISC.push_back(99 );
   mb_ISC.push_back(99 );
   mb_ISC.push_back(99 );
   mb_ISC.push_back(99 );
   mb_ISC.push_back( 9 );
   mb_ISC.push_back(19 );
   mb_ISC.push_back(20 );
   mb_ISC.push_back( 2 );
}

