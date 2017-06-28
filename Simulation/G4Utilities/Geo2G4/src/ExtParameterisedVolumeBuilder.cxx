/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ExtParameterisedVolumeBuilder.h"
#include "Geo2G4AssemblyFactory.h"
#include "Geo2G4AssemblyVolume.h"
#include "Geo2G4LVFactory.h"
#include "Geo2G4STParameterisation.h"
#include "G4LogicalVolume.hh"

#include "G4PVPlacement.hh"
#include "G4ReflectionFactory.hh"
#include "G4VPVParameterisation.hh"
#include "G4PVParameterised.hh"
#include "G4AffineTransform.hh"
#include "G4UnitsTable.hh"
#include "G4LogicalVolume.hh"
#include "G4VSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4PhysicalVolumeStore.hh"
#include "globals.hh"

#include "SimHelpers/ServiceAccessor.h"

#include "GeoModelKernel/GeoAccessVolAndSTAction.h"
#include "GeoModelKernel/GeoVolumeCursor.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoSerialTransformer.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "StoreGate/StoreGateSvc.h"
#include <iostream>

ExtParameterisedVolumeBuilder::ExtParameterisedVolumeBuilder(std::string n):
  VolumeBuilder(n),
  m_getMatEther(true),
  m_matEther(0),m_matHypUr(0),m_msg(n)
{
}

//stolen from G4bool G4PVPlacement::CheckOverlaps, but revised so we can tell what it overlaps with...
//returns 0 if no overlap, overwise returns overlapping volume pointer...and whether it's with the mother...
G4VPhysicalVolume* check_overlap(G4VPhysicalVolume *me, int &mom){
  int res = 1000;//number of points to check per volume!
  mom=-1;//will return whether it is the mom that is overlapping or not...
  G4VSolid* solid = me->GetLogicalVolume()->GetSolid();
  G4LogicalVolume* motherLog = me->GetMotherLogical();
  //std::cout<<"check_overlap"<<std::endl;
  if (!motherLog) { return 0; }
  G4VSolid* motherSolid = motherLog->GetSolid();

  // Create the transformation from daughter to mother
  G4AffineTransform Tm( me->GetRotation(), me->GetTranslation() );
  std::cout<<"check_overlap for "<<me->GetName()<<" with "<<res<<" points"<<std::endl;
  for (G4int n=0; n<res; n++)
  {
    // Generate a random point on the solid's surface
    G4ThreeVector point = solid->GetPointOnSurface();

    // Transform the generated point to the mother's coordinate system
    G4ThreeVector mp = Tm.TransformPoint(point);

    // Checking overlaps with the mother volume
    if (motherSolid->Inside(mp)==kOutside)
    {
      G4double distin = motherSolid->DistanceToIn(mp);
      if (distin>0)
      {
        // std::ostringstream message;
//         message << "Overlap with mother volume !" << G4endl
//                 << "          Overlap is detected for volume "
//                 << me->GetName() << G4endl
//                 << "          with its mother volume "
//                 << motherLog->GetName() << G4endl
//                 << "          at mother local point " << mp << ", "
//                 << "overlapping by at least: "
//                 << G4BestUnit(distin, "Length");
        //G4Exception("G4PVPlacement::CheckOverlaps()","GeomVol1002", JustWarning, message);
       mom=1;
        return 0;//motherLog;
      }
    }

    // Checking overlaps with each 'sister' volume
    for (G4int i=0; i<motherLog->GetNoDaughters(); i++)
    {
      G4VPhysicalVolume* daughter = motherLog->GetDaughter(i);

      if (daughter == me) { continue; }

      // Create the transformation for daughter volume and transform point
      G4AffineTransform Td( daughter->GetRotation(),
                            daughter->GetTranslation() );
      G4ThreeVector md = Td.Inverse().TransformPoint(mp);

      G4VSolid* daughterSolid = daughter->GetLogicalVolume()->GetSolid();
      if (daughterSolid->Inside(md)==kInside)
      {
        G4double distout = daughterSolid->DistanceToOut(md);
        if (distout>0)
        {
          // std::ostringstream message;
//           message << "Overlap with volume already placed !" << G4endl
//                   << "          Overlap is detected for volume "
//                   << me->GetName() << G4endl
//                   << "          with " << daughter->GetName() << " volume's"
//                   << G4endl
//                   << "          local point " << md << ", "
//                   << "overlapping by at least: "
//                   << G4BestUnit(distout,"Length");
//           G4Exception("G4PVPlacement::CheckOverlaps()",
//                       "GeomVol1002", JustWarning, message);
         mom=0;
          return daughter;
        }
      }

      // Now checking that 'sister' volume is not totally included and
      // overlapping. Do it only once, for the first point generated
      if (n==0)
      {
        // Generate a single point on the surface of the 'sister' volume
        // and verify that the point is NOT inside the current volume
        G4ThreeVector dPoint = daughterSolid->GetPointOnSurface();

        // Transform the generated point to the mother's coordinate system
        // and finally to current volume's coordinate system
        G4ThreeVector mp2 = Td.TransformPoint(dPoint);
        G4ThreeVector msi = Tm.Inverse().TransformPoint(mp2);

        if (solid->Inside(msi)==kInside)
        {
          // std::ostringstream message;
//           message << "Overlap with volume already placed !" << G4endl
//                   << "          Overlap is detected for volume "
//                   << me->GetName() << G4endl
//                   << "          apparently fully encapsulating volume "
//                   << daughter->GetName() << G4endl
//                   << "          at the same level !";
//           G4Exception("G4PVPlacement::CheckOverlaps()",
//                       "GeomVol1002", JustWarning, message);
         mom=2;
          return daughter;
        }
      }
    }
  }
  return 0;//no overlap!
}


G4LogicalVolume* ExtParameterisedVolumeBuilder::Build(const PVConstLink theGeoPhysVolume, OpticalVolumesMap* optical_volumes) const
{
  MsgStream log(msgSvc(),GetKey());
  PVConstLink theGeoPhysChild;
  const GeoSerialTransformer* serialTransformerChild=0;
  G4LogicalVolume* theG4LogChild;
  unsigned int numChildNodes;                      // number of child nodes (PV and ST)
  bool descend;                                    // flag to continue geo tree navigation
  bool serialExists = false;                       // flag for existence of ST among childs
  std::string nameChild;

  if(m_getMatEther) getMatEther();

  static Geo2G4LVFactory LVFactory;

  G4LogicalVolume* theG4LogVolume = LVFactory.Build(theGeoPhysVolume,descend);

  if(!descend) return theG4LogVolume;

  numChildNodes = theGeoPhysVolume->getNChildVolAndST();

  // *****************************************************************
  // **
  // ** If m_ST2Param flag is set:
  // ** Check if there's any serial transformer among child volumes
  // **
  // *****************************************************************

  if(m_paramOn)
    for(size_t counter1=0; counter1<numChildNodes; counter1++)
      {
        GeoAccessVolAndSTAction actionVolAndST(counter1);
        theGeoPhysVolume->exec(&actionVolAndST);

        if((serialTransformerChild=actionVolAndST.getSerialTransformer()))
          {
            nameChild = actionVolAndST.getName();
            serialExists = true;
            break;
          }
      }
  // ***************************************************************************
  // **                Next steps:
  // **
  // ** 1. If ST exists and numChildNodes==1, translate ST to G4 ST
  // **
  // ** 2. If ST exists and numChildNodes !=1, print information message and
  // **    translate ST to single placements as well as all other child volumes
  // **
  // ** 3. There's no ST - ok, nothing special ...
  // **
  // ***************************************************************************

  if(serialExists && (numChildNodes==1))
    {
      theGeoPhysChild = serialTransformerChild->getVolume();

      // Build the child
      if(!(theG4LogChild = Build(theGeoPhysChild,optical_volumes))) return 0;

      if (nameChild == "ANON") nameChild=theG4LogChild->GetName();
      nameChild += "_Param";

      Geo2G4STParameterisation* stParameterisation = new Geo2G4STParameterisation(serialTransformerChild->getFunction(),
                                                                                  serialTransformerChild->getNCopies());

      G4VPhysicalVolume* pvParametrised __attribute__ ((unused)) = new G4PVParameterised(nameChild,
                                                                                         theG4LogChild,
                                                                                         theG4LogVolume,
                                                                                         kUndefined,
                                                                                         serialTransformerChild->getNCopies(),
                                                                                         stParameterisation);
    }
  else
    {
      if(serialExists)
        {
          std::string volName = theGeoPhysVolume->getLogVol()->getName();
          PrintSTInfo(volName);
        }

      GeoVolumeCursor av(theGeoPhysVolume);
      while (!av.atEnd())
        {
          int id = 16969;

          // Get child phys volume
          theGeoPhysChild = av.getVolume();
          // Get its transform
          G4Transform3D theG4Position(av.getTransform());

          Query<int> Qint =  av.getId();
          if(Qint.isValid()) id = Qint;

          if(m_matEther == theGeoPhysChild->getLogVol()->getMaterial())
            {
              Geo2G4AssemblyVolume* assembly = BuildAssembly(theGeoPhysChild);

              if(Qint.isValid())
                assembly->MakeImprint(theG4LogVolume,theG4Position,id);
              else
                assembly->MakeImprint(theG4LogVolume,theG4Position);
            }
          else if(m_matHypUr == theGeoPhysChild->getLogVol()->getMaterial())
            {
              Geo2G4AssemblyVolume* assembly = BuildAssembly(theGeoPhysChild);

              if(Qint.isValid())
                assembly->MakeImprint(theG4LogVolume,theG4Position,id,true);
              else
                assembly->MakeImprint(theG4LogVolume,theG4Position,0,true);
            }
          else
            {
              nameChild = av.getName();

              // Build the child
              if(!(theG4LogChild = Build(theGeoPhysChild,optical_volumes))) return 0;

              if (nameChild == "ANON") nameChild=theG4LogChild->GetName();

              log << MSG::DEBUG << "ACH655 Placing child: "<<nameChild<<"["<<id<<"]"<<" in mother "<<theG4LogVolume->GetName()<<endmsg;

                     bool surfcheck=true; //whether to check for overlaps
                     bool doplace=true;
                     if ("TRT::MainRadiatorA"==nameChild && 16969==id) surfcheck=true;
                     if ("TRT::ThinRadiatorA"==nameChild && 16969==id) surfcheck=true;
                     if ("TRT::InnerSupportA"==nameChild && 16969==id) surfcheck=true;
                     if ("TRT::OuterSupportA"==nameChild && 16969==id) surfcheck=true;
                     if ("TRT::MiddleRadiatorA"==nameChild && 16969==id) surfcheck=true;
                     //if ("TRT::Straw"==nameChild) doplace=false;//don't build straws for now
                     //if ("TRT::StrawPlane"==nameChild && id!=2) doplace=false;//only build one strawplane for now
                     if (std::string(nameChild).find("LAr:")!=std::string::npos) surfcheck=false;
                     if (std::string(nameChild).find("Tile:")!=std::string::npos) surfcheck=false;
                     if (std::string(nameChild).find("DriftTube")!=std::string::npos) surfcheck=false;
                     if (std::string(nameChild).find("Muon:")!=std::string::npos) surfcheck=false;
                     if (std::string(theG4LogVolume->GetName()).find("Muon:")!=std::string::npos) surfcheck=false;//or if the mom is muon
                      if (std::string(nameChild).find("Pixel:")!=std::string::npos) surfcheck=false;
                      if (std::string(nameChild).find("SCT:")!=std::string::npos) surfcheck=false;
                      if (std::string(theG4LogVolume->GetName()).find("SCT:")!=std::string::npos) surfcheck=false;//or if the mom is sct
                     if (std::string(nameChild).find("_cut")!=std::string::npos) {
                             doplace=false;//don't add again for now, if we already cut it
                             log<<MSG::WARNING<<"Why is something that was cut already now being placed again?"<<endmsg;
                     }
                     bool dosurfcheckfuringplacement=false;//to run the printout in G4 for surfcheck placements
                     if (doplace) {
                       G4PhysicalVolumesPair pvPair = G4ReflectionFactory::Instance()->Place(theG4Position,
                                                                                           nameChild,
                                                                                           theG4LogChild,
                                                                                           theG4LogVolume,
                                                                                           false,
                                                                                           id,
                                                                                           surfcheck&&dosurfcheckfuringplacement);

                     static int trytofixoverlaps=-1;
                     if (trytofixoverlaps<0) {//just check it once...
                             char* pTRY; pTRY=getenv("HAAS_TryToFixOverlaps");
                             if (pTRY!=NULL) trytofixoverlaps=atoi(pTRY); else trytofixoverlaps=0;
                             log<<MSG::INFO<<"Try to fix overlaps: "<<trytofixoverlaps<<endmsg;
                     }
                     static int docheckoverlaps=-1;
                             if (docheckoverlaps<0) {//just check it once...
                             char* pTRY; pTRY=getenv("HAAS_CheckOverlaps");
                             if (pTRY!=NULL) docheckoverlaps=atoi(pTRY); else docheckoverlaps=0;
                             log<<MSG::INFO<<"Check for overlaps: "<<docheckoverlaps<<endmsg;
                     }

                     while (surfcheck){//keep looping until we break out...
                       bool triedtofix=false;
                       int mom=-2; G4VPhysicalVolume* other = 0; if (docheckoverlaps||trytofixoverlaps) {other=check_overlap(pvPair.first,mom);}
                       if (other){
                         log<<MSG::INFO<<"ACH649 : "<<other->GetName()<<" is overlapping "<<pvPair.first->GetName()<<" and mom is "<<mom<<endmsg;
                         if (!trytofixoverlaps){
                             static int ci = 0; std::string cis; std::stringstream cisout;
                             cisout<<"_cut"; cisout << ++ci; cis = cisout.str();//keep track of increasing cut N

                             other->SetName(other->GetName()+cis);
                             pvPair.first->SetName(pvPair.first->GetName()+cis);
                             std::cout<<"Now have "<<pvPair.first->GetName()<<" and "<<other->GetName()<<std::endl;
                             break;//
                         }
                       }
                       else {
                             break;//no overlaps
                       }
                       if (other&&trytofixoverlaps){
                         //do something to try to fix the overlap!
                         if (0==mom){
                             static int ci = 0; std::string cis; std::stringstream cisout;
                             cisout<<"_cut"; cisout << ++ci; cis = cisout.str();//keep track of increasing cut N

                           //easiest case, overlaping sisters within the same mother
                           //first make the new solid, via subtraction
                           G4RotationMatrix *myrot=new G4RotationMatrix();//initially Identity
                           if (other->GetRotation()) (*myrot)= *(other->GetRotation());//make sure it's not null
                           (*myrot)*=pvPair.first->GetObjectRotationValue();//straw rot * inverse support rot?
                           G4SubtractionSolid* cutout = new G4SubtractionSolid(pvPair.first->GetName()+cis, //TRT::InnerSupportB_cut5
                                                                               pvPair.first->GetLogicalVolume()->GetSolid(), //TRT::InnerSupportB solid
                                                                               other->GetLogicalVolume()->GetSolid(), //TRT::StrawPlane solid
                                                                               myrot,//made just above
                                                                               other->GetTranslation() - pvPair.first->GetTranslation() //straw - support
                                                                               );
                           log<<MSG::INFO<<"ACH544 Replacing "<<pvPair.first->GetName()<<" volume of size "
                               <<pvPair.first->GetLogicalVolume()->GetSolid()->GetCubicVolume()
                              <<" with cutout volume "<<cutout->GetCubicVolume()<<endmsg;

                           //then we copy the logical volume, since it will have a different (cutout) solid
                            G4LogicalVolume *lv=pvPair.first->GetLogicalVolume();
                            std::cout<<"old name is "<<lv->GetName()<<" and cis is "<<cis<<std::endl;
                           G4LogicalVolume *lvnew = new G4LogicalVolume(cutout, lv->GetMaterial(),
                               lv->GetName()+cis, lv->GetFieldManager(), lv->GetSensitiveDetector(), lv->GetUserLimits() );

                           //copy the daughters of the old volume into the new volume
                           for (int d=0; d<lv->GetNoDaughters(); ++d){
                             lvnew->AddDaughter(lv->GetDaughter(d));
                           }

                           //really we should copy the daughters too, and then do daughter->SetMotherLogical(pvPair.first->GetLogicalVolume()); !!!
                           //TODO...
                           log<<MSG::INFO<<"New LogicalVolume "<<lvnew->GetName()<<" has "<<lvnew->GetNoDaughters()<<" daughters"<<endmsg;

                           //remove the old and make a new PhysicalVolume
                           G4String newvolname = pvPair.first->GetName()+cis;
                           pvPair.first->SetName(pvPair.first->GetName()+"_old");
                           G4PhysicalVolumeStore::DeRegister(pvPair.first);
                           delete pvPair.first;//!!!

                           G4PhysicalVolumesPair pvPair2 = G4ReflectionFactory::Instance()->Place(theG4Position,
                                                                                  newvolname,
                                                                                  lvnew,
                                                                                  theG4LogVolume,
                                                                                  false,
                                                                                  id,
                                                                                  true);//surfcheck&&dosurfcheckfuringplacement);

                           other->SetName(other->GetName()+cis);
                           if (pvPair2.first) std::cout<<"Now have first volume "<<pvPair2.first->GetName()<<" and "<<other->GetName()<<std::endl;
                           if (pvPair2.second) std::cout<<"Now have second volume "<<pvPair2.first->GetName()<<" and "<<other->GetName()<<std::endl;
                           triedtofix=true;
                           pvPair.first=pvPair2.first;//for the next time through the while loop checking for more overlaps...

                         }
                         else if (2==mom){
                           //a sister is totally inside me?
                         }
                         else {
                           log<<MSG::ERROR<<" How can mom be "<<mom<<" here ???"<<endmsg;
                         }
                         //

                       }//other
                       else if (mom==1 && trytofixoverlaps){
                         log<<MSG::WARNING<<"ACH649 : overlapping pvPair first with mother (not fixing yet), "
                               <<pvPair.first->GetName()<<" and mom is "<<mom<<endmsg;
                       }

                       if (pvPair.second){
                         mom=-2;
                         other = check_overlap(pvPair.second,mom);
                         if (other){
                           log<<MSG::WARNING<<"ACH649 : "<<other->GetName()<<" is overlapping pvPair second (not fixing yet), "
                             <<pvPair.second->GetName()<<" and mom is "<<mom<<endmsg;
                         }
                         else if (mom==1){
                           log<<MSG::WARNING<<"ACH649 : overlapping pvPair second (not fixing yet), "
                             <<pvPair.second->GetName()<<" and mom is "<<mom<<endmsg;
                         }
                       }

                       if (triedtofix){
                         int mom2=-2; G4VPhysicalVolume* other2 = check_overlap(pvPair.first,mom2);//recheck!
                         if (other2){
                           log<<MSG::INFO<<"ACH644 : "<<other2->GetName()<<" is still overlapping "<<pvPair.first->GetName()<<" and mom2 is "<<mom2<<endmsg;
                           //try again...
                         }
                         else if (mom2==1){
                           log<<MSG::WARNING<<"ACH644 : overlapping pvPair first with mother STILL "<<pvPair.first->GetName()<<" and mom2 is "<<mom2<<endmsg;
                           break;//not fixing it yet...
                         }
                         else{
                           log<<MSG::INFO<<"ACH644 : no overlaps for "<<pvPair.first->GetName()<<" now and mom2 is "<<mom2<<endmsg;
                           break;//good jorb!
                         }
                       }

                     }//while surfcheck



              // if GeoModel volume is optical store it in the map
              if(optical_volumes!=0)
                {
                  const GeoOpticalPhysVol* opticalGeoPhysChild =
                    dynamic_cast < const GeoOpticalPhysVol* >(theGeoPhysChild.operator->());
                  if(opticalGeoPhysChild)
                    (*optical_volumes)[opticalGeoPhysChild] = pvPair.first;
                }
              }//doplace
            }

          av.next();
        }
    }

  return theG4LogVolume;
}

Geo2G4AssemblyVolume* ExtParameterisedVolumeBuilder::BuildAssembly(PVConstLink pv) const
{
  PVConstLink theGeoPhysChild;
  G4LogicalVolume* theG4LogChild = 0;
  Geo2G4AssemblyVolume* theG4AssemblyChild = 0;
  bool descend;                                    // flag to continue geo tree navigation

  if(m_getMatEther) getMatEther();

  static Geo2G4AssemblyFactory AssemblyFactory;

  Geo2G4AssemblyVolume* assemblyVolume = AssemblyFactory.Build(pv,descend);

  if(!descend) return assemblyVolume;

  // Loop over child volumes and add them to the Geo2G4AssemblyVolume
  GeoVolumeCursor av(pv);
  while (!av.atEnd())
    {
      theGeoPhysChild = av.getVolume();
      std::string nameChild = av.getName();

      std::string strVolume = std::string("Volume ") + nameChild + " ("
        + theGeoPhysChild->getLogVol()->getName() + ")";

      // Check if it is an assembly
      if(m_matEther == theGeoPhysChild->getLogVol()->getMaterial() || 
         m_matHypUr == theGeoPhysChild->getLogVol()->getMaterial() )
        {
          // Build the child assembly
          if(!(theG4AssemblyChild = BuildAssembly(theGeoPhysChild))) return 0;

          // Get its transform
          G4Transform3D theG4Position(av.getTransform());

          assemblyVolume->AddPlacedAssembly(theG4AssemblyChild,theG4Position);
        }
      else
        {
          Query<int> Qint =  av.getId();

          // Build the child
          if(!(theG4LogChild = Build(theGeoPhysChild))) return 0;

          // Get its transform
          G4Transform3D theG4Position(av.getTransform());

          int placedID = 0;
          if(Qint.isValid()) placedID = Qint;

          std::string placedName = nameChild=="ANON" ? "" : nameChild;


          assemblyVolume->AddPlacedVolume(theG4LogChild,theG4Position,placedID,placedName);
        }

      av.next();
    }

  return assemblyVolume;
}

void ExtParameterisedVolumeBuilder::PrintSTInfo(std::string volume) const
{
  ATH_MSG_INFO ( "**********************************************" );
  ATH_MSG_INFO ( "**  " );
  ATH_MSG_INFO ( "**  The Volume " << volume  );
  ATH_MSG_INFO ( "**  Has children of two different types" );
  ATH_MSG_INFO ( "**  PeoPhysVolume and GeoSerialTransformer" );
  ATH_MSG_INFO ( "**  In this case GeoSerialTransformer will be " );
  ATH_MSG_INFO ( "**  translated into G4 placement but not in " );
  ATH_MSG_INFO ( "**  G4Parameterisation" );
  ATH_MSG_INFO ( "**  " );
  ATH_MSG_INFO ( "********************************************** " );
}

void ExtParameterisedVolumeBuilder::getMatEther() const
{
  StoreGateSvc* pDetStore=0;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  if(svcLocator->service("DetectorStore",pDetStore).isFailure()) {
    ATH_MSG_ERROR ( "ExtParameterisedVolumeBuilder: Unable to access Detector Store" );
  }
  else
    {
      DataHandle<StoredMaterialManager> theMaterialManager;
      if(pDetStore->retrieve(theMaterialManager, "MATERIALS").isFailure()) {
        ATH_MSG_ERROR ( "ExtParameterisedVolumeBuilder: Unable to access Material Manager" );
      }
      else {
        m_matEther = theMaterialManager->getMaterial("special::Ether");
        m_matHypUr = theMaterialManager->getMaterial("special::HyperUranium");
      }
    }
  m_getMatEther = false;
}
