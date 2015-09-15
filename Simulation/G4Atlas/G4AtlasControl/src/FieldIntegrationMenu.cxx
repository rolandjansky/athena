/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>

#include "G4AtlasControl/FieldIntegrationMenu.h"

#include "FadsField/RegionInformation.h"

#include "G4Region.hh"
#include "G4RegionStore.hh"
#include <string>

FieldIntegrationMenu::FieldIntegrationMenu():SimpleMenu("FieldIntegrationMenu")
{
}

void FieldIntegrationMenu::list()
{
  G4RegionStore *rs = G4RegionStore::GetInstance();
  for (unsigned int i=0;i<rs->size();i++){
    if (! (*rs)[i] ) continue;
    FADS::RegionInformation* regionInformation = (FADS::RegionInformation*)((*rs)[i]->GetUserInformation());
    if (!regionInformation) continue;
    FADS::FieldIntParameters* fip = regionInformation->GetFieldIntParameters();
    if (!fip) continue;
    log() << MSG::INFO << "For region " << (*rs)[i]->GetName() << " FieldIntegrationParameters object:\n"
		   << "KineticEnergyThreshold : " << fip->GetCurrent_kineticEnergyThreshold() << " MeV\n"
		   << "   idx=0 for e+/e- track below E-threshold\n"
		   << "   idx=1 for e+/e- track above E-threshold and all other charged track except mu+/mu- of any kinetic energy\n"
		   << "   idx=2 for mu+/mu- track of any kinetic energy" << endreq;

    double params[7][3]; // oh, the trouble we go to for pretty formatting...
    for (unsigned int j=0;j<3;j++){
      params[0][j] = fip->GetCurrent_maxLengthForConstField(j);
      params[1][j] = fip->GetCurrent_missDistance(j);
      params[2][j] = fip->GetCurrent_typicalLongStep(j);
      params[3][j] = fip->GetCurrent_tolerableBiasError(j);
      params[4][j] = fip->GetCurrent_tolerableIntegrationError(j);
      params[5][j] = fip->GetCurrent_expectedNumBoundaries(j);
      params[6][j] = fip->GetCurrent_expectedTrackLength(j);
    }
    std::string names[7] = { "Max Length For Const Field \t",
                             "Miss Distance              \t",
                             "Typical Long Step          \t",
                             "Tolerable Bias Error       \t",
                             "Tolerable Integration Error\t",
                             "Expected Num Boundaries    \t",
                             "Expected Track Length      \t" };  
    log() << MSG::INFO <<             "Parameter                  \tIdx0\tIdx1\tIdx2" << endreq;
    for (unsigned int j=0;j<7;j++){
      log() << MSG::INFO << names[j] << params[j][0] << "\t" << params[j][1] << "\t" << params[j][2] << endreq;
    }
    log() << MSG::INFO << "\n" << endreq;
  }

}

void FieldIntegrationMenu::Set_kineticEnergyThreshold(const std::string regionName,double val)
{ 
  FADS::FieldIntParameters* fieldIntParam = Get_fieldIntParameters(regionName);
  if(fieldIntParam)
  {
    fieldIntParam->SetCurrent_kineticEnergyThreshold(val);
  }
  else
  {
    log() << MSG::WARNING << "Region <"<<regionName<<"> was not found in the region store. kineticEnergyThreshold has not been modified." << endreq;
  }
}

void FieldIntegrationMenu::Set_maxLengthForConstField(const std::string regionName,int idx,double val)
{ 
  FADS::FieldIntParameters* fieldIntParam = Get_fieldIntParameters(regionName);
  if(fieldIntParam)
  {
    fieldIntParam->SetCurrent_maxLengthForConstField(idx,val);
  }
  else
  {
    log() << MSG::WARNING << "Region <"<<regionName<<"> was not found in the region store. maxLengthForConstField has not been modified." << endreq;
  }
}

void FieldIntegrationMenu::Set_missDistance(const std::string regionName,int idx,double val)
{ 
  FADS::FieldIntParameters* fieldIntParam = Get_fieldIntParameters(regionName);
  if(fieldIntParam)
  {
    fieldIntParam->SetCurrent_missDistance(idx,val);
  }
  else
  {
    log() << MSG::WARNING << "Region <"<<regionName<<"> was not found in the region store. missDistance has not been modified." << endreq;
  }
}

void FieldIntegrationMenu::Set_typicalLongStep(const std::string regionName,int idx,double val)
{ 
  FADS::FieldIntParameters* fieldIntParam = Get_fieldIntParameters(regionName);
  if(fieldIntParam)
  {
    fieldIntParam->SetCurrent_typicalLongStep(idx,val);
  }
  else
  {
    log() << MSG::WARNING << "Region <"<<regionName<<"> was not found in the region store. typicalLongStep has not been modified." << endreq;
  }
}

void FieldIntegrationMenu::Set_tolerableBiasError(const std::string regionName,int idx,double val)
{ 
  FADS::FieldIntParameters* fieldIntParam = Get_fieldIntParameters(regionName);
  if(fieldIntParam)
  {
    fieldIntParam->SetCurrent_tolerableBiasError(idx,val);
  }
  else
  {
     log() << MSG::WARNING << "Region <"<<regionName<<"> was not found in the region store. tolerableBiasError has not been modified." << endreq;
  }
}

void FieldIntegrationMenu::Set_tolerableIntegrationError(const std::string regionName,int idx,double val)
{ 
  FADS::FieldIntParameters* fieldIntParam = Get_fieldIntParameters(regionName);
  if(fieldIntParam)
  {
    fieldIntParam->SetCurrent_tolerableIntegrationError(idx,val);
  }
  else
  {
     log() << MSG::WARNING << "Region <"<<regionName<<"> was not found in the region store. tolerableIntegrationError has not been modified." << endreq;
  }
}

void FieldIntegrationMenu::Set_expectedNumBoundaries(const std::string regionName,int idx,double val)
{ 
  FADS::FieldIntParameters* fieldIntParam = Get_fieldIntParameters(regionName);
  if(fieldIntParam)
  {
    fieldIntParam->SetCurrent_expectedNumBoundaries(idx,val);
  }
  else
  {
     log() << MSG::WARNING << "Region <"<<regionName<<"> was not found in the region store. expectedNumBoundaries has not been modified." << endreq;
  }
}

void FieldIntegrationMenu::Set_expectedTrackLength(const std::string regionName,int idx,double val)
{ 
  FADS::FieldIntParameters* fieldIntParam = Get_fieldIntParameters(regionName);
  if(fieldIntParam)
  {
    fieldIntParam->SetCurrent_expectedTrackLength(idx,val);
  }
  else
  {
     log() << MSG::WARNING << "Region <"<<regionName<<"> was not found in the region store. expectedTrackLength has not been modified." << endreq;
  }
}

FADS::FieldIntParameters* FieldIntegrationMenu::Get_fieldIntParameters(const std::string regionName)
{
  G4Region* pRegion = G4RegionStore::GetInstance()->GetRegion(regionName);
  if(!pRegion) return NULL;
  FADS::RegionInformation* regionInformation = (FADS::RegionInformation*)(pRegion->GetUserInformation());
  if(!regionInformation) return NULL;
  return regionInformation->GetFieldIntParameters();
}

