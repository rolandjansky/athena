#include "ISF_Geant4Event/ISFG4GeoHelper.h"

// Athena includes
#include "AtlasDetDescr/AtlasRegion.h"

//Geant4
#include "G4LogicalVolumeStore.hh"
#include "G4Step.hh" 

#include "G4TransportationManager.hh"
#include "SimHelpers/StepHelper.h"

#include <iostream>


//________________________________________________________________________
AtlasDetDescr::AtlasRegion
iGeant4::ISFG4GeoHelper::nextGeoId(const G4Step* aStep, int truthVolLevel, ISF::IGeoIDSvc *geoIDSvc)
{

  static G4LogicalVolume * BPholder=0 , * IDholder=0 , * CALOholder=0 , * MUholder=0 , * TTRholder=0 ;
  if (BPholder==0){ // Initialize

    G4LogicalVolumeStore * lvs = G4LogicalVolumeStore::GetInstance();
    for (size_t i=0;i<lvs->size();++i){

      if ( !(*lvs)[i] ) continue;
      if ( (*lvs)[i]->GetName() == "BeamPipe::BeamPipe" ) BPholder = (*lvs)[i];
      else if ( (*lvs)[i]->GetName() == "IDET::IDET" ) IDholder = (*lvs)[i];
      else if ( (*lvs)[i]->GetName() == "CALO::CALO" ) CALOholder = (*lvs)[i];
      else if ( (*lvs)[i]->GetName() == "MUONQ02::MUONQ02" ) MUholder = (*lvs)[i];
      else if ( (*lvs)[i]->GetName() == "TTR_BARREL::TTR_BARREL" ) TTRholder = (*lvs)[i];

    }

    ISFG4GeoHelper::checkVolumeDepth( G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->GetWorldVolume()->GetLogicalVolume() , truthVolLevel);

  }


  // Static so that it will keep the value from the previous step
  static AtlasDetDescr::AtlasRegion nextGeoID = truthVolLevel>1?AtlasDetDescr::fAtlasCavern:AtlasDetDescr::fUndefinedAtlasRegion;

  static const G4Track* aTrack = 0;

  StepHelper step(aStep);


  if (aTrack != aStep->GetTrack()) {
    
    // First step with this track!
    nextGeoID = AtlasDetDescr::fUndefinedAtlasRegion;
    aTrack = aStep->GetTrack();
    
  } // Otherwise use the cached value via the static


  const G4StepPoint *postStep = aStep->GetPostStepPoint();


  bool leavingG4World       = postStep->GetStepStatus()==fWorldBoundary;

  if ( leavingG4World ) {

    nextGeoID = AtlasDetDescr::fAtlasCavern;

    return nextGeoID;
  }


  // If in mother volume, use the ISF_GeoIDSvc to resolve the geoID
  if (step.PostStepBranchDepth()<truthVolLevel){

    const G4ThreeVector     &postPos  = postStep->GetPosition();
    //const G4ThreeVector     &postMom  = postStep->GetMomentum();
    //nextGeoID = m_geoIDSvcQuick->identifyNextGeoID( postPos.x(),
    //                                                postPos.y(),
    //                                                postPos.z(),
    //                                                postMom.x(),
    //                                                postMom.y(),
    //                                                postMom.z() );
    nextGeoID = geoIDSvc->identifyGeoID( postPos.x(),
                                                postPos.y(),
                                                postPos.z() );

    return nextGeoID;
  }

  // Ordering inside out (most truth in the ID anyway...)
  if ( IDholder==step.GetPostStepLogicalVolume(truthVolLevel) ){
    nextGeoID = AtlasDetDescr::fAtlasID;
  } else if ( CALOholder==step.GetPostStepLogicalVolume(truthVolLevel) ){
    nextGeoID = AtlasDetDescr::fAtlasCalo;
  } else if ( MUholder==step.GetPostStepLogicalVolume(truthVolLevel) ){
    nextGeoID = AtlasDetDescr::fAtlasMS;
  } else if ( BPholder==step.GetPostStepLogicalVolume(truthVolLevel) ){
    nextGeoID = (step.PostStepBranchDepth()>truthVolLevel && step.GetPostStepLogicalVolumeName(truthVolLevel+1)=="BeamPipe::BeamPipeCentral")?AtlasDetDescr::fAtlasID:AtlasDetDescr::fAtlasForward;
  } else if ( TTRholder==step.GetPostStepLogicalVolume(truthVolLevel) ){
    nextGeoID = AtlasDetDescr::fAtlasCavern;
  } else if (truthVolLevel>0 && step.GetPostStepLogicalVolumeName(truthVolLevel-1).find("CavernInfra")!=std::string::npos) {
    nextGeoID = AtlasDetDescr::fAtlasCavern;
  } else {
    // We are in trouble
    //ATH_MSG_ERROR("vol1: "<<step.GetPostStepLogicalVolumeName(1)<<", vol2: "<<step.GetPostStepLogicalVolumeName(2)<<", postname="<<postname<<", returning undefined geoID");
    G4ThreeVector myPos = aStep->GetPostStepPoint()->GetPosition();
    //ATH_MSG_ERROR("Returning undefined geoID from " << step.GetPostStepLogicalVolume() << " requesting " << step.GetPostStepLogicalVolume(truthVolLevel) << " at " << myPos.x() << " " << myPos.y() << " " << myPos.z() );
  }

  return nextGeoID;
}

bool iGeant4::ISFG4GeoHelper::checkVolumeDepth(G4LogicalVolume* lv, int volLevel, int depth) {
  
  //FIXME - can replace all this code with similar methods to those in MCTruthBase/src/RecordingEnvelope.cxx 
  if (lv==nullptr) return false;
   
  bool Cavern = false;
  // Check the volumes rather explicitly                                                                                                            

  if ( lv->GetName() == "BeamPipe::BeamPipe" ||                   
       lv->GetName() == "IDET::IDET" ||                   
       lv->GetName() == "CALO::CALO" ||  
       lv->GetName() == "MUONQ02::MUONQ02" || 
       lv->GetName() == "TTR_BARREL::TTR_BARREL" ){  

    if (depth==volLevel){
      //ATH_MSG_DEBUG("Volume " << lv->GetName() << " is correctly registered at depth " << depth);           
    } else { 
      //ATH_MSG_ERROR("Volume " << lv->GetName() << " at depth " << depth << " instead of depth " << volLevel);     
      throw "WrongDepth";                                                               
    } // Check of volume level         
  } else if ( lv->GetName()=="BeamPipe::BeamPipeCentral" ){ // Things that are supposed to be one deeper   
    if (depth==volLevel+1){                  
      //ATH_MSG_DEBUG("Volume " << lv->GetName() << " is correctly registered at depth " << depth);      
    } else {
      //ATH_MSG_ERROR("Volume " << lv->GetName() << " at depth " << depth << " instead of depth " << volLevel+1);    
      throw "WrongDepth";                                                            
    } // Check of volume level    
  } else if ( lv->GetName()=="BeamPipe::BeamPipeCentral" ){ // Things that are supposed to be one deeper                                
    if (depth==volLevel+1){
      //ATH_MSG_DEBUG("Volume " << lv->GetName() << " is correctly registered at depth " << depth);                                           
    } else {                                                        
      //ATH_MSG_ERROR("Volume " << lv->GetName() << " at depth " << depth << " instead of depth " << volLevel+1);                             
      throw "WrongDepth";                                                                                                               
    } // Check of volume level                                                                                                          
  } else if ( lv->GetName().find("CavernInfra")!=std::string::npos ){ // Things that are supposed to be one shallower                   

    if (depth==volLevel-1){
      Cavern=true;

      //ATH_MSG_DEBUG("Volume " << lv->GetName() << " is correctly registered at depth " << depth);                                           
      // Note: a number of volumes exist with "CavernInfra" in the name at the wrong depth, so we just need to                          
      //   check that there's at least one at the right depth                                                                           
    } // Check of volume level                                                                                                          
  } // Check of volume name                                                                                                             
  
  // Going through the volume depth                                                                                                     
  for (int i=0; i<lv->GetNoDaughters(); ++i){      

    Cavern = Cavern || checkVolumeDepth( lv->GetDaughter(i)->GetLogicalVolume() , volLevel , depth+1 );

  }

  if (depth==0 && !Cavern && volLevel>1){

    //ATH_MSG_ERROR("No CavernInfra volume registered at depth " << volLevel-1);                                                          
    throw "WrongDepth";                                                                                                                 
  }

  return Cavern;                                                                                                                        
}                                               
