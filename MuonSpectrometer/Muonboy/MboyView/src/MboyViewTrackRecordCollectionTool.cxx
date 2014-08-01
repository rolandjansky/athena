/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IPartPropSvc.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "HepPDT/ParticleData.hh"

#include "TrackRecord/TrackRecordCollection.h"

/////////////////////////////////////////////////////////
#include "MboyView/MboyViewTrackRecordCollectionTool.h"

static const InterfaceID IID_IMboyViewTrackRecordCollectionTool("MboyViewTrackRecordCollectionTool", 1, 0);

const InterfaceID& MboyViewTrackRecordCollectionTool::interfaceID()
{
  return IID_IMboyViewTrackRecordCollectionTool;
}

MboyViewTrackRecordCollectionTool::MboyViewTrackRecordCollectionTool(const std::string& t, 
                                            const std::string& n,
                                            const IInterface*  p ):AthAlgTool(t,n,p)
, p_MboyViewPointerDumperTool        ( "MboyViewPointerDumperTool/MboyViewPointerDumperTool"              ) 
{

  declareInterface<MboyViewTrackRecordCollectionTool>(this);

  m_KounterErrorMessage    =  0; 
  m_KounterErrorMessageMax = 10;   

  const HepPDT::ParticleDataTable m_particleDataTable("PDG Table");

  declareProperty("MboyViewPointerDumperTool"        , p_MboyViewPointerDumperTool        ) ; 

}

MboyViewTrackRecordCollectionTool::~MboyViewTrackRecordCollectionTool(){}

// Initialize
StatusCode MboyViewTrackRecordCollectionTool::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " AthAlgTool::initialize() failed" ) ;
    return( StatusCode::FAILURE );
  }

  // get the Particle Properties Service
  IPartPropSvc* partPropSvc = 0;
  if (StatusCode::SUCCESS != service("PartPropSvc", partPropSvc, true))
  {
    ATH_MSG_FATAL( " Could not initialize Particle Properties Service" ) ;
    return StatusCode::FAILURE;
  }      
  m_particleDataTable = partPropSvc->PDT();
  
//Retrieve p_MboyViewPointerDumperTool
  if ( p_MboyViewPointerDumperTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_MboyViewPointerDumperTool ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_MboyViewPointerDumperTool ) ;

  ATH_MSG_INFO( "Initialisation ended     " ) ;

  return StatusCode::SUCCESS;

}

StatusCode MboyViewTrackRecordCollectionTool::finalize(){return StatusCode::SUCCESS;}

StatusCode MboyViewTrackRecordCollectionTool::ViewTrackRecordCollection(
               std::string TrackRecordCollectionLocation,
               std::ofstream* pOut
){

//Access
  if (!(evtStore()->contains<TrackRecordCollection>(TrackRecordCollectionLocation))) {
    m_KounterErrorMessage = m_KounterErrorMessage + 1 ;
    if (m_KounterErrorMessageMax >= m_KounterErrorMessage){
      ATH_MSG_INFO( " Not existing " << TrackRecordCollectionLocation ) ;
    }
    return( StatusCode::SUCCESS );
  }
  const TrackRecordCollection *pTrackRecordCollection(0);
  StatusCode sc = evtStore()->retrieve(pTrackRecordCollection, TrackRecordCollectionLocation);
  if ( sc.isFailure() ) {
    m_KounterErrorMessage = m_KounterErrorMessage + 1 ;
    if (m_KounterErrorMessageMax >= m_KounterErrorMessage) {
      ATH_MSG_INFO( " pTrackRecordCollection not found at existing " << TrackRecordCollectionLocation ) ;
    }
    return( StatusCode::SUCCESS );
  }

//Dump
  for (TrackRecordCollection::const_iterator trkit=pTrackRecordCollection->begin(); 
       trkit != pTrackRecordCollection->end() ; ++trkit) {
    int    PDGCode ;
    double PosX    ;
    double PosY    ;
    double PosZ    ;
    double PX      ;
    double PY      ;
    double PZ      ;      
    double Pt      ;
    GetKineTrackRecord(
     (*trkit),
     PDGCode ,
     PosX    ,
     PosY    ,
     PosZ    ,
     PX      ,
     PY      ,
     PZ      ,      
     Pt      
    );
    // charged particle
    double charge = 0;
    const HepPDT::ParticleData*	particleData	= m_particleDataTable->particle(abs(PDGCode));
    if (particleData != 0 )
      charge = PDGCode > 0 ? particleData->charge() : - particleData->charge();

    *pOut << "TRK" << " "
          << "TrackRecord" << " "
          << TrackRecordCollectionLocation << " "
          << " dummy " << " "
          << std::setw(12)<<std::setprecision(3) <<PosX    << " "
          << std::setw(12)<<std::setprecision(3) <<PosY    << " "
          << std::setw(12)<<std::setprecision(3) <<PosZ    << " "
          << std::setw(12)<<std::setprecision(3) <<PX       << " "
          << std::setw(12)<<std::setprecision(3) <<PY      << " "
          << std::setw(12)<<std::setprecision(3) <<PZ      << " "
          << std::setw(3)<<std::setprecision(0) <<charge   << " "
          << std::setw(8)<<std::setprecision(3) <<PDGCode << " "
          ;
    p_MboyViewPointerDumperTool->DumpPointer(pOut,(*trkit)); 
    *pOut << std::endl;
  }

  return StatusCode::SUCCESS;

}
void MboyViewTrackRecordCollectionTool::GetKineTrackRecord(
 TrackRecord* pTrackRecord,
 int&    PDGCode ,
 double& PosX    ,
 double& PosY    ,
 double& PosZ    ,
 double& PX      ,
 double& PY      ,
 double& PZ      ,
 double& Pt      
){

  PDGCode = 0  ;
  PosX    = 0. ;
  PosY    = 0. ;
  PosZ    = 0. ;
  PX      = 0. ;
  PY      = 0. ;
  PZ      = 0. ;
  Pt      = 0. ;

  if (pTrackRecord){
    Pt = pTrackRecord->GetEnergy()
         /sqrt(1.+sinh(pTrackRecord->GetMomentum().eta())*sinh(pTrackRecord->GetMomentum().eta()) );
    PDGCode = pTrackRecord->GetPDGCode()                 ;
    PosX    = pTrackRecord->GetPosition().x()            ;
    PosY    = pTrackRecord->GetPosition().y()            ;
    PosZ    = pTrackRecord->GetPosition().z()            ;
    PX      = Pt*cos(pTrackRecord->GetMomentum().phi())  ;
    PY      = Pt*sin(pTrackRecord->GetMomentum().phi())  ;
    PZ      = Pt*sinh(pTrackRecord->GetMomentum().eta()) ;
  }

}

