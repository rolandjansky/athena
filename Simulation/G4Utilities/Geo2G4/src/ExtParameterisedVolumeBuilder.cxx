/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ExtParameterisedVolumeBuilder.h"
#include "Geo2G4AssemblyVolume.h"
#include "Geo2G4LVFactory.h"
#include "Geo2G4STParameterisation.h"
#include "G4LogicalVolume.hh"

#include "G4PVPlacement.hh"
#include "G4ReflectionFactory.hh"
#include "G4VPVParameterisation.hh"
#include "G4PVParameterised.hh"
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

ExtParameterisedVolumeBuilder::ExtParameterisedVolumeBuilder(std::string n, Geo2G4AssemblyFactory* G4AssemblyFactory):
  VolumeBuilder(n),
  m_getMatEther(true),
  m_matEther(0),
  m_matHypUr(0),
  m_msg(n),
  m_G4AssemblyFactory(G4AssemblyFactory)
{
}

G4LogicalVolume* ExtParameterisedVolumeBuilder::Build(const PVConstLink theGeoPhysVolume, OpticalVolumesMap* optical_volumes)
{
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

              G4PhysicalVolumesPair pvPair = G4ReflectionFactory::Instance()->Place(theG4Position,
                                                                                    nameChild,
                                                                                    theG4LogChild,
                                                                                    theG4LogVolume,
                                                                                    false,
                                                                                    id);

              // if GeoModel volume is optical store it in the map
              if(optical_volumes!=0)
                {
                  const GeoOpticalPhysVol* opticalGeoPhysChild =
                    dynamic_cast < const GeoOpticalPhysVol* >(theGeoPhysChild.operator->());
                  if(opticalGeoPhysChild)
                    (*optical_volumes)[opticalGeoPhysChild] = pvPair.first;
                }
            }

          av.next();
        }
    }

  return theG4LogVolume;
}

Geo2G4AssemblyVolume* ExtParameterisedVolumeBuilder::BuildAssembly(PVConstLink pv)
{
  PVConstLink theGeoPhysChild;
  G4LogicalVolume* theG4LogChild = 0;
  Geo2G4AssemblyVolume* theG4AssemblyChild = 0;
  bool descend;                                    // flag to continue geo tree navigation

  if(m_getMatEther) getMatEther();

  Geo2G4AssemblyVolume* assemblyVolume = m_G4AssemblyFactory->Build(pv,descend);

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
