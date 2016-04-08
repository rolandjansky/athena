/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4Field/ConstantFieldHandler.h"

#include "FadsField/FieldManager.h"
#include "FadsField/MagneticFieldMap.h"
#include "G4Field/ConstantField.h"
// Geant4 includes
#include "G4MagneticField.hh"
#include "G4SystemOfUnits.hh"

ConstantFieldHandler::ConstantFieldHandler()
{
  std::cout << "Creating the ConstantFieldHandler" << std::endl;
}

void ConstantFieldHandler::setFieldValues(std::string fieldName, float Bx, float By, float Bz)
{
    FADS::FieldManager* fm = FADS::FieldManager::GetFieldManager();

    FADS::MagneticFieldMap* magField=fm->GetMagneticField("ConstantField");
    std::cout <<"Getting the field: "<<magField->GetName()<<std::endl;

    FADS::MagneticFieldMap* theMap=magField->Create(fieldName);
    std::cout<<" Field map pointer to "<<fieldName <<" is : "<<theMap<<std::endl;

    ConstantField *constantmagField=dynamic_cast<ConstantField*>(theMap);
    if(NULL!=constantmagField)
      {
	constantmagField->SetXComponent(Bx*CLHEP::tesla);
	constantmagField->SetYComponent(By*CLHEP::tesla);
	constantmagField->SetZComponent(Bz*CLHEP::tesla);
      }
    else std::cout << "ERROR - failed to cast FADS::MagneticFieldMap " <<fieldName << " to ConstantField." << std::endl;
}

void ConstantFieldHandler::setXField(std::string fieldName, double xmin, double xmax)
{
    FADS::FieldManager *fm=FADS::FieldManager::GetFieldManager();

    FADS::MagneticFieldMap *magField=fm->GetMagneticField("ConstantField");
    std::cout <<"Getting the field: "<<magField->GetName()<<std::endl;

    FADS::MagneticFieldMap* theMap=magField->Create(fieldName);
    std::cout<<" Field map pointer to "<<fieldName <<" is : "<<theMap<<std::endl;

    ConstantField *constantmagField=dynamic_cast<ConstantField*>(theMap);
    if(NULL!=constantmagField)
      {
	constantmagField->SetXMinField(xmin);
	constantmagField->SetXMaxField(xmax);
      }
    else std::cout << "ERROR - failed to cast FADS::MagneticFieldMap " <<fieldName << " to ConstantField." << std::endl;
}

void ConstantFieldHandler::setYField(std::string fieldName, double ymin, double ymax)
{
    FADS::FieldManager *fm=FADS::FieldManager::GetFieldManager();

    FADS::MagneticFieldMap *magField=fm->GetMagneticField("ConstantField");
    std::cout <<"Getting the field: "<<magField->GetName()<<std::endl;

    FADS::MagneticFieldMap* theMap=magField->Create(fieldName);
    std::cout<<" Field map pointer to "<<fieldName <<" is : "<<theMap<<std::endl;

    ConstantField *constantmagField=dynamic_cast<ConstantField*>(theMap);
    if(NULL!=constantmagField)
      {
	constantmagField->SetYMinField(ymin);
	constantmagField->SetYMaxField(ymax);
      }
    else std::cout << "ERROR - failed to cast FADS::MagneticFieldMap " <<fieldName << " to ConstantField." << std::endl;
}

void ConstantFieldHandler::setZField(std::string fieldName, double zmin, double zmax)
{
    FADS::FieldManager *fm=FADS::FieldManager::GetFieldManager();

    FADS::MagneticFieldMap *magField=fm->GetMagneticField("ConstantField");
    magField->GetName();
    std::cout <<"Getting the field: "<<magField->GetName()<<std::endl;

    FADS::MagneticFieldMap* theMap=magField->Create(fieldName);
    std::cout<<" Field map pointer to "<<fieldName <<" is : "<<theMap<<std::endl;

    ConstantField *constantmagField=dynamic_cast<ConstantField*>(theMap);
    if(NULL!=constantmagField)
      {
	constantmagField->SetZMinField(zmin);
	constantmagField->SetZMaxField(zmax);
      }
    else std::cout << "ERROR - failed to cast FADS::MagneticFieldMap " <<fieldName << " to ConstantField." << std::endl;
}

