/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ParameterisedVolumeBuilder.h"
#include "Geo2G4LVFactory.h"
#include "Geo2G4STParameterisation.h"
#include "Geo2G4/LogicalVolume.h"
#include "GeoModelKernel/GeoAccessVolAndSTAction.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoSerialTransformer.h"
#include "G4PVPlacement.hh"
#include "G4ReflectionFactory.hh"
#include "G4VPVParameterisation.hh"
#include "G4PVParameterised.hh"
#include "globals.hh"
#include <iostream>


// static ParameterisedVolumeBuilder b("Parameterised_Volume_Builder");

LogicalVolume* ParameterisedVolumeBuilder::Build(const PVConstLink theGeoPhysVolume, OpticalVolumesMap* /*optical_volumes*/) const
{
  PVConstLink theGeoPhysChild;
  const GeoSerialTransformer* serialTransformerChild;
  G4LogicalVolume* theG4LogChild;
  unsigned int numChild;                           // number of child nodes (PV and ST)
  bool descend;                                    // flag to continue geo tree navigation

  //  static SingleLogicalVolumeFactory LVFactory;
  static Geo2G4LVFactory LVFactory;

  //  const GeoLogVol* geoLog = theGeoPhysVolume->getLogVol();
  G4LogicalVolume* theG4LogVolume = LVFactory.Build(theGeoPhysVolume,descend);


  if(!descend) return theG4LogVolume;

  //  if (numChild==0) return theG4LogVolume;
  //  if (theG4LogVolume->GetNoDaughters()) return theG4LogVolume;

  // Loop over the children of the GeoVolume and place them
  numChild = theGeoPhysVolume->getNChildVolAndST();

  for(size_t ii=0; ii<numChild; ii++)
    {
      // Get the child Phys volume or SerialTransformer with ii index
      GeoAccessVolAndSTAction action(ii);
      theGeoPhysVolume->exec(&action);

      std::string nameChild = action.getName();
      int id=0;

      if(theGeoPhysChild=action.getVolume())
        {
          // The ii node is a GeoPhysVol
          // Build the child
          theG4LogChild = Build(theGeoPhysChild);
          if(!theG4LogChild) return 0;

          // Get its transform
          const G4Transform3D theG4Position(action.getTransform());
          if (nameChild == "ANON") nameChild=theG4LogChild->GetName();

          G4ReflectionFactory::Instance()->Place(theG4Position,
                                                 nameChild,
                                                 theG4LogChild,
                                                 theG4LogVolume,
                                                 false,
                                                 id);
        }
      else if((serialTransformerChild=action.getSerialTransformer()))
        {
          // The ii node is a GeoSerialTransformer
          if(numChild > 1)
            {
              std::cerr << "\n\nParameterisedVolumeBuilder::Build ERROR building " << theGeoPhysVolume->getLogVol()->getName()
                        << ". In GEANT4 the parameterisation MUST be the mother's single daughter.\n";
              return 0;
            }
          else
            {
              theGeoPhysChild = serialTransformerChild->getVolume();

              // Build the child
              theG4LogChild = Build(theGeoPhysChild);
              if(!theG4LogChild) return 0;

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
        }
      else // node type is neither PV nor ST
        {
          std::cerr << "\n\nParameterisedVolumeBuilder::Build UNEXPECTED ERROR: action is returning 0.\n";
          return 0;
        }
    }

  return theG4LogVolume;
}
