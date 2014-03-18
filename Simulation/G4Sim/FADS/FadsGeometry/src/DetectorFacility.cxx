/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsGeometry/DetectorFacility.h"
#include "FadsGeometry/DetectorFacilityCatalog.h"
#include "FadsGeometry/GeometryManager.h"
#include "FadsGeometry/FadsDetectorConstruction.h"
#include "FadsGeometry/FadsParallelWorld.h"
#include "G4RunManager.hh"
#include "G4RunManagerKernel.hh"
#include "G4VUserDetectorConstruction.hh"

namespace FADS {

  DetectorFacility::DetectorFacility(std::string n): DetectorFacilityBase(n)
  {
    DetectorFacilityCatalog *dfs=DetectorFacilityCatalog::GetInstance();
    // cout<<"Registering detector "<<n<<endl;
    dfs->RegisterDetector(this);
    envelope.theRotation=0;
    envelope.thePosition=G4ThreeVector();
    envelope.thePositionedVolume=0;
    envelope.theEnvelope=0;
    theParent=0;
    isWorld=false;
    // cout<<"creating "<<name<<endl;
  }

  Envelope& DetectorFacility::Build()
  {
    // cout<<"building "<<name<<endl;
    GeometryManager* gm=GeometryManager::GetGeometryManager();
    gm->SetCurrentDetectorName(name);
    BuildGeometry();
    gm->ResetCurrentDetectorName();
    // cout<<"envelope done"<<endl;
    if (envelope.theEnvelope)
      {
        PositionInParent();
        BuildDetectors();

      }
    return envelope;
  }

  void DetectorFacility::SetAsWorld()
  {
    // cout<<"setting "<<name<<" to world"<<endl;
    std::cout<<" this is the new version of DetectorFacility::SetAsWorld!!"<<std::endl;
    isWorld=true;
    Envelope e=Build();
    std::string n=e.theEnvelope->GetName();
    // cout<<"---->"<<e.theEnvelope->GetName()<<endl;
    G4VPhysicalVolume* pv
      = new G4PVPlacement(0,G4ThreeVector(),e.theEnvelope,n,0,false,0);
    G4RunManager *rm=G4RunManager::GetRunManager();
    FadsDetectorConstruction *dc=(FadsDetectorConstruction *)rm->GetUserDetectorConstruction();
    // cout<<"Calling SetWorld"<<endl;
    dc->SetEnvelope(e.theEnvelope);
    dc->SetPhysicalVolume(pv);
    envelope.thePositionedVolume=pv;
  }

  void DetectorFacility::SetAsParallelWorld(std::string parallelWorldName)
  {
    FadsParallelWorld *theParalleWorld;

    std::cout<<"+-------------------------------------------+"<<std::endl;
    std::cout<<"|                                           |"<<std::endl;
    std::cout<<"  setting a new ParallelWorld:"<<parallelWorldName<<std::endl;
    std::cout<<"|                                           |"<<std::endl;
    std::cout<<"+-------------------------------------------+"<<std::endl;
    theParalleWorld=new FadsParallelWorld(parallelWorldName);
    G4RunManager *rm=G4RunManager::GetRunManager();
    G4VUserDetectorConstruction *dc=const_cast<G4VUserDetectorConstruction *>(rm->GetUserDetectorConstruction());
    dc->RegisterParallelWorld(theParalleWorld);

    Envelope e=Build();
    G4LogicalVolume *lv=e.theEnvelope;
    theParalleWorld->SetTreeTop(lv);

  }

  void DetectorFacility::BuildDetectors()
  {
    assert (envelope.theEnvelope);
    for (unsigned int i=0;i<detectors.size();i++)
      {
        Envelope e=detectors[i]->Build();
      }
  }
  void DetectorFacility::PositionInParent()
  {
    // cout<<" In PositionInParent() --"<<theParent<<endl;
    if (theParent)
      {
        if (!theParent->IsBuilt())
          std::cout<<"DetectorFacility: Something wrong! "
                   <<"the parent is not built"<<std::endl;
        Envelope e=theParent->GetEnvelope();
        // cout<<" \t parent "<<e.theEnvelope->GetName()<<endl;
        G4VPhysicalVolume* pv
          = new G4PVPlacement(envelope.theRotation,envelope.thePosition,
                              envelope.theEnvelope,envelope.theEnvelope->GetName(),
                              e.theEnvelope,false,0);
        envelope.thePositionedVolume=pv;
      }
  }
  void DetectorFacility::MoveTo(G4ThreeVector& newPos)
  {
    envelope.thePosition=newPos;
    if (envelope.thePositionedVolume) envelope.thePositionedVolume->
                                        SetTranslation(newPos);
  }
  void DetectorFacility::RotateX(double angle)
  {
    if (!envelope.theRotation)
      {
        envelope.theRotation=new G4RotationMatrix;
        envelope.theRotation->rotateX(angle);
        if (envelope.thePositionedVolume)
          envelope.thePositionedVolume->SetRotation
            (envelope.theRotation);
      }
    else
      envelope.theRotation->rotateX(angle);
  }
  void DetectorFacility::RotateY(double angle)
  {
    if (!envelope.theRotation)
      {
        envelope.theRotation=new G4RotationMatrix;
        envelope.theRotation->rotateY(angle);
        if (envelope.thePositionedVolume)
          envelope.thePositionedVolume->SetRotation
            (envelope.theRotation);
      }
    else
      envelope.theRotation->rotateY(angle);
  }
  void DetectorFacility::RotateZ(double angle)
  {
    if (!envelope.theRotation)
      {
        envelope.theRotation=new G4RotationMatrix;
        envelope.theRotation->rotateZ(angle);
        if (envelope.thePositionedVolume)
          envelope.thePositionedVolume->SetRotation
            (envelope.theRotation);
      }
    else
      envelope.theRotation->rotateZ(angle);
  }

  void DetectorFacility::SetInitialTransformation(G4Transform3D& transform)
  {
    if (!envelope.theRotation) envelope.theRotation=new G4RotationMatrix;
    *(envelope.theRotation)=transform.getRotation().inverse();
    envelope.thePosition=transform.getTranslation();
  }

} // end namespace
