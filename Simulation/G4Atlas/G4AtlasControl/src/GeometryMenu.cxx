/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasControl/GeometryMenu.h"
//#include "FadsGeometry/DetectorFacility.h"
#include "FadsUtilities/Tokenizer.h"
#include "FadsGeometry/FadsDetectorConstruction.h"
#include <iostream>

#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4VisAttributes.hh"
#include "G4Color.hh"
#include "G4SDManager.hh"
#include "G4VSensitiveDetector.hh"
#include "G4UserLimits.hh"
#include "G4RunManager.hh"

#include <string>
#include <map>
#include <vector>

#include "FadsGeometry/DetectorFacilityCatalog.h"

bool isMatch(const std::string& a,const std::string b)
{
  // straightforward cases
  if (a=="*") return true;
  if (a==b) return true;
  // wildcards
  FADS::Tokenizer tokens("*",a);
  bool returnValue=true;
  std::string temp=b;
  for (unsigned int i=0;i<tokens.size();i++)
    {
      if (tokens[i].empty()) continue;
      std::string::size_type npos=temp.find(tokens[i]);
      if (i==0 && npos) 
	return false;
      if (npos!=std::string::npos)
	{
	  temp=temp.substr(npos, temp.size()-npos);
	}
      else 
	{
	  returnValue=false;
	  break;
	}
    }
  if (returnValue && !tokens[tokens.size()-1].empty())
    {
      std::string temp=tokens[tokens.size()-1];
      std::string temp2=b.substr(b.size()-temp.size(),temp.size());
      if (temp!=temp2) 
	return false;
    }
  return returnValue;
}

GeometryMenu::GeometryMenu():SimpleMenu("GeometryMenu")
{
  //	log() << MSG::VERBOSE << "Creating the GeometryMenu " << endreq;
}

void GeometryMenu::ListDetectorFacilities()
{
  FADS::DetectorFacilityCatalog *dfs=FADS::DetectorFacilityCatalog::GetInstance();
  dfs->PrintAll();
}
FADS::DetectorFacility* GeometryMenu::GetDetectorFacility(const std::string fName, const std::string gName) const
{
  FADS::DetectorFacilityCatalog *dfs=FADS::DetectorFacilityCatalog::GetInstance();
  std::string newName;
  if (gName.empty()) newName=fName;
  else newName=gName;
  return dfs->GetFacility(fName,newName);
}

void GeometryMenu::PrintDetectorVolumes(const std::string detName)
{
  log() << MSG::INFO << "PrintDetectorVolumes" << endreq;
  if( msgLvl(MSG::INFO) ) {
    
    G4LogicalVolumeStore& lvs=*(G4LogicalVolumeStore::GetInstance());
    std::cout << "Detector "<<detName<<": Total nr. of LV ";
    std::map<std::string, int, std::less<std::string> > vNames;
    int totVol=0;
    for (unsigned int i=0;i<lvs.size();i++)
      {
	G4LogicalVolume *lv=lvs[i];
	std::string volName=lv->GetName();
	std::string dName="";
	std::string::size_type npos=volName.find("::");
	if (npos!=std::string::npos)
	  dName=volName.substr(0,npos);
	if (!dName.empty() && dName==detName)
	  {
	    totVol++;
	    vNames[volName]++;
	  }
      }
    std::cout<<totVol<<std::endl<<std::endl;
    std::map<std::string, int, std::less<std::string> >::const_iterator it;
    for (it=vNames.begin();it!=vNames.end();it++)
      {
	std::cout << "--- "<<std::setw(50)<<(*it).first<<"  " <<
	  (*it).second<<std::endl;
      }
  }
  return;
}

void GeometryMenu::PrintVolumes()
{
  log() << MSG::INFO << "PrintVolumes" << endreq;
  if( msgLvl(MSG::INFO) ) {
    
    typedef std::vector<std::string> volumeNames;
    typedef std::map<std::string, volumeNames,
      std::less<std::string> > volumeNameMap;
    G4LogicalVolumeStore& lvs=*(G4LogicalVolumeStore::GetInstance());
    volumeNameMap theMap;
    for (unsigned int i=0;i<lvs.size();i++)
      {
	G4LogicalVolume *lv=lvs[i];
	std::string volName=lv->GetName();
	std::string detName;
	std::string::size_type npos=volName.find("::");
	if (npos!=std::string::npos)
	  detName=volName.substr(0,npos);
	else
	  detName="Generic";
	if (theMap.find(detName) != theMap.end())
	  {
	    volumeNames v=theMap[detName];
	    if (find(v.begin(),v.end(),volName)==v.end())
	      theMap[detName].push_back(volName);
	  }
	else
	  {
	    volumeNames v;
	    v.push_back(volName);
	    theMap[detName]=v;
	  }
	
      }
    volumeNameMap::const_iterator it;
    for (it=theMap.begin();it!=theMap.end();it++)
      {
	if ((*it).first=="Generic") { continue; }
	std::cout << "Detector "<<(*it).first<<
	  ":"<<std::endl<<std::endl;
	volumeNames v=(*it).second;
	for (unsigned int i=0;i<v.size();i++)
	  {
	    std::cout<<std::setw(40)<<v[i];
	    if (((i+1)%2)==0) std::cout<<std::endl; 
	  }
	std::cout<<std::endl;
      }
    volumeNames v=theMap["Generic"];
    std::cout << "For the following volumes no association with a detector was possible"<<std::endl<<std::endl;
    for (unsigned int i=0;i<v.size();i++)
      {
	std::cout<<"  "<<std::setw(40)<<v[i]<<"   ";
	if (((i+1)%2)==0) std::cout<<std::endl; 
      }
    std::cout<<std::endl;
  }
  return;
}

void GeometryMenu::AssignSD(const std::string volName, const std::string sdName)
{
  G4SDManager *sdm=G4SDManager::GetSDMpointer();
  G4VSensitiveDetector *sd=sdm->FindSensitiveDetector(sdName);
  if (sd==0)
    {
      log() << MSG::WARNING << "Sensitive detector "<<sdName<<" not found!" << endreq;
      return;
    }
  G4LogicalVolumeStore& lvs=*(G4LogicalVolumeStore::GetInstance());
  for (unsigned int i=0;i<lvs.size();i++)
    {
      G4LogicalVolume *lv=lvs[i];
      if (lv==0) continue;
      if (isMatch(volName,lv->GetName()))
	{
	  //			std::cout<<"Assigning sensitive detector "<<sdName<<" to volume "<<
	  //					   volName<<std::endl;
	  lv->SetSensitiveDetector(sd);
	}
    }
}

void GeometryMenu::SetMaxStep(const std::string volName, double cut, const std::string part)
{
  G4LogicalVolumeStore& lvs=*(G4LogicalVolumeStore::GetInstance());
  int ndone=0;
  for (unsigned int i=0;i<lvs.size();i++)
    {
      G4LogicalVolume *lv=lvs[i];
      if (isMatch(volName,lv->GetName()))
	{
	  G4UserLimits *ul=lv->GetUserLimits();
	  if (!ul) ul=new G4UserLimits;
	  ul->SetMaxAllowedStep(cut);
	  lv->SetUserLimits(ul);
	  ndone++;
	}
    }
	
  // now the process mess
  bool allParticles= (part=="*");
  std::vector<std::string> pNames;
  if (!allParticles)
    {
		
    }
	
  log() << MSG::INFO << "MaxStep set to "<<cut<<" to "<<ndone<<" copies of "<<volName << endreq;
}

void GeometryMenu::SetMinEkine(const std::string volName, double cut, const std::string)
{
  G4LogicalVolumeStore& lvs=*(G4LogicalVolumeStore::GetInstance());
  int ndone=0;
  for (unsigned int i=0;i<lvs.size();i++)
    {
      G4LogicalVolume *lv=lvs[i];
      if (isMatch(volName,lv->GetName()))
	{
	  G4UserLimits *ul=lv->GetUserLimits();
	  if (!ul) ul=new G4UserLimits;
	  ul->SetUserMinEkine(cut);
	  lv->SetUserLimits(ul);
	  ndone++;
	}
    }
  log() << MSG::INFO << "MinEkine set to "<<cut<<" to "<<ndone<<" copies of "<<volName << endreq;
}

void GeometryMenu::SetMaxTime(const std::string volName, double cut, const std::string)
{
  G4LogicalVolumeStore& lvs=*(G4LogicalVolumeStore::GetInstance());
  int ndone=0;
  for (unsigned int i=0;i<lvs.size();i++)
    {
      G4LogicalVolume *lv=lvs[i];
      if (isMatch(volName,lv->GetName()))
	{
	  G4UserLimits *ul=lv->GetUserLimits();
	  if (!ul) ul=new G4UserLimits;
	  ul->SetUserMaxTime(cut);
	  lv->SetUserLimits(ul);
	  ndone++;
	}
    }
  log() << MSG::INFO << "MaxTime set to "<<cut<<" to "<<ndone<<" copies of "<<volName << endreq;
}


void GeometryMenu::SetMaxTrackLength(const std::string volName, double cut, const std::string)
{
  G4LogicalVolumeStore& lvs=*(G4LogicalVolumeStore::GetInstance());
  int ndone=0;
  for (unsigned int i=0;i<lvs.size();i++)
    {
      G4LogicalVolume *lv=lvs[i];
      if (isMatch(volName,lv->GetName()))
	{
	  G4UserLimits *ul=lv->GetUserLimits();
	  if (!ul) ul=new G4UserLimits;
	  ul->SetUserMaxTrackLength(cut);
	  lv->SetUserLimits(ul);
	  ndone++;
	}
    }
  log() << MSG::INFO << "MaxTrackLength set to "<<cut<<" to "<<ndone<<" copies of "<<volName << endreq;
}

void GeometryMenu::SetMinRange(const std::string volName, double cut, const std::string)
{
  G4LogicalVolumeStore& lvs=*(G4LogicalVolumeStore::GetInstance());
  int ndone=0;
  for (unsigned int i=0;i<lvs.size();i++)
    {
      G4LogicalVolume *lv=lvs[i];
      if (isMatch(volName,lv->GetName()))
	{
	  G4UserLimits *ul=lv->GetUserLimits();
	  if (!ul) ul=new G4UserLimits;
	  ul->SetUserMinRange(cut);
	  lv->SetUserLimits(ul);
	  ndone++;
	}
    }
  log() << MSG::INFO << "MinRange set to "<<cut<<" to "<<ndone<<" copies of "<<volName << endreq;
}

void GeometryMenu::SetGeometryCheck(int i)
{
  log() << MSG::INFO << "this is GeometryMenu::SetGeometryCheck" << endreq;
  G4RunManager *rm=G4RunManager::GetRunManager();
  if (rm)
    {
      const FADS::FadsDetectorConstruction *dc=dynamic_cast<const FADS::FadsDetectorConstruction *>
	(rm->GetUserDetectorConstruction());
	
      if (dc) 
	{
	  FADS::FadsDetectorConstruction *ddc=const_cast<FADS::FadsDetectorConstruction *>(dc);
	  //			std::cout<<" geometryCheckLevel being set to "<<i<<std::endl;
	  ddc->SetGeometryCheckLevel(i);
	}
      else 
	log() << MSG::ERROR << "GeometryMenu::SetGeometryCheck: FDC is 0!!!!" << endreq;
    }
  else 
    log() << MSG::ERROR << "GeometryMenu::SetGeometryCheck : RunManager is 0!!!" << endreq;
}
