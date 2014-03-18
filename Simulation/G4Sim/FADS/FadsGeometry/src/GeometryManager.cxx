/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsGeometry/GeometryManager.h"
#include "FadsGeometry/FadsLogicalVolumeNotifier.h"
#include "FadsGeometry/FadsPhysicalVolumeNotifier.h"

#include "G4VisAttributes.hh"
#include "G4UImanager.hh"
#include "G4VisManager.hh"
#include "G4LogicalVolumeModel.hh"
#include <map>

#include <iostream>

namespace FADS {

  GeometryManager* GeometryManager::thePointer=0;

  GeometryManager::GeometryManager()
  {
    std::cout<<" Geometry manager being built now"<<std::endl;
    currentDetectorName="Generic";
    AddDetectorStore(currentDetectorName);

    LVNotifier=new FadsLogicalVolumeNotifier();
    PVNotifier=new FadsPhysicalVolumeNotifier();
  }

  GeometryManager::~GeometryManager()
  {
    delete LVNotifier;
    delete PVNotifier;
  }

  GeometryManager* GeometryManager::GetGeometryManager()
  {
    if (!thePointer)
      thePointer=new GeometryManager;
    return thePointer;
  }
  void GeometryManager::RegisterSolid(std::string n,G4VSolid *v)
  {
    // cout<<"********** Registering Solid "<<nam<<endl;

    GetDetectorStore(currentDetectorName)->Smap.insert(make_pair(n,v));
  }
  void GeometryManager::RegisterVolume(G4LogicalVolume *v)
  {
    std::string nam=v->GetName();
    // cout<<"********** Registering Volume "<<nam<<endl;
    if (currentDetectorName.empty())
      GetDetectorStore("Generic")->LVmap.insert(make_pair(nam,v));
    else
      GetDetectorStore(currentDetectorName)->LVmap.insert(make_pair(nam,v));

  }
  void GeometryManager::RegisterPhysicalVolume(G4VPhysicalVolume *v)
  {
    std::string nam=v->GetName();
    // cout<<"********** Registering Volume "<<nam<<endl;
    GetDetectorStore(currentDetectorName)->PVmap.insert(make_pair(nam,v));

  }
  std::vector<G4LogicalVolume*>& GeometryManager::GetVolume(std::string name)
  {

    static std::vector<G4LogicalVolume*> v;
    v.clear();

    int i=name.find_first_of(':');

    std::string det;
    if (i<=0) det="*";
    else det=name.substr(0,i);

    i=name.find_last_of(':');
    std::string vol=name.substr(i+1,name.size()-i-1);

    if (det=="*")
      {
        volumeMap& volMap=GetDetectorStore("Generic")->LVmap;

        if (volMap.find(vol)==volMap.end())
          {
            std::cout<<"Volume "<<vol<<" not found!!!"<<std::endl;
          }
        else
          {
            volumeMap::const_iterator it;
            for (it=volMap.lower_bound(vol);it!=volMap.upper_bound(vol);
                 it++)
              {
                v.push_back((*it).second);
              }
          }
      }
    else
      v=GetVolume(det,vol);
    return v;
  }
  std::vector<G4LogicalVolume*>& GeometryManager::GetVolume(std::string detname,std::string name)
  {

    static std::vector<G4LogicalVolume*> v;
    v.clear();

    volumeMap& volMap=GetDetectorStore(detname)->LVmap;

    if (volMap.find(name)==volMap.end())
      {
        std::cout<<"Volume "<<name<<" not found!!!"<<std::endl;
      }
    else
      {
        volumeMap::const_iterator it;
        for (it=volMap.lower_bound(name);it!=volMap.upper_bound(name);
             it++)
          {
            v.push_back((*it).second);
          }
      }
    return v;
  }
  void GeometryManager::RegisterSolid(G4VSolid* /*v*/)
  {
    // solidMap& solMap=GetDetectorStore("Generic")->Smap;

    // std::string nam=v->GetName();
    // solMap.insert(make_pair(nam,v));
  }
  std::vector<G4VSolid*>& GeometryManager::GetSolid(std::string name)
  {

    solidMap& solMap=GetDetectorStore("Generic")->Smap;

    static std::vector<G4VSolid*> v;
    v.clear();

    if (solMap.find(name)==solMap.end())
      {
        std::cout<<"Solid "<<name<<" not found!!!"<<std::endl;
      }
    else
      {
        solidMap::const_iterator it;
        for (it=solMap.lower_bound(name);it!=solMap.upper_bound(name);
             it++)
          {
            v.push_back((*it).second);
          }
      }
    return v;
  }
  std::vector<G4VSolid*>& GeometryManager::GetSolid(std::string detname,std::string name)
  {

    solidMap& solMap=GetDetectorStore(detname)->Smap;

    static std::vector<G4VSolid*> v;
    v.clear();

    if (solMap.find(name)==solMap.end())
      {
        std::cout<<"Solid "<<name<<" not found!!!"<<std::endl;
      }
    else
      {
        solidMap::const_iterator it;
        for (it=solMap.lower_bound(name);it!=solMap.upper_bound(name);
             it++)
          {
            v.push_back((*it).second);
          }
      }
    return v;
  }
  void GeometryManager::PrintVolumes()
  {
    detectorMap::const_iterator dt;
    for (dt=detMap.begin();dt!=detMap.end();dt++)
      {
        std::cout<<"--->Detector "<<(*dt).first<<std::endl;
        std::map<std::string,int, std::less<std::string> > tempMap;
        volumeMap::const_iterator it;
        for (it=(*dt).second->LVmap.begin();it!=(*dt).second->LVmap.end();it++)
          {
            if (tempMap.find((*it).first)!=tempMap.end())
              tempMap[(*it).first]++;
            else
              tempMap[(*it).first]=1;
          }
        std::map<std::string,int, std::less<std::string> >::const_iterator jt;
        for (jt=tempMap.begin();jt!=tempMap.end();jt++)
          {
            std::string dname=(*dt).first;
            std::string vname=(*jt).first;
            std::vector<G4LogicalVolume *> vec=GetVolume(dname,vname);
            G4LogicalVolume *lv=vec[0];
            char ch;
            const G4VisAttributes *va=lv->GetVisAttributes();
            if (va)
              if (va->IsVisible()) ch='V';
              else ch='I';
            else
              ch='N';
            std::cout<<"\t--- Volume "<<(*jt).first<<" "<<(*jt).second
                     <<" "<<ch<<" "<<lv->GetNoDaughters()<<
              " ---"<<std::endl;
          }
      }
  }
  void GeometryManager::PrintSolids()
  {
    detectorMap::const_iterator dt;
    for (dt=detMap.begin();dt!=detMap.end();dt++)
      {
        std::cout<<"--->Detector "<<(*dt).first<<std::endl;
        solidMap::const_iterator it;
        for (it=(*dt).second->Smap.begin();it!=(*dt).second->Smap.end();it++)
          std::cout<<"--- Solid "<<(*it).first<<" ---"<<std::endl;
      }
  }

  void GeometryManager::AddDetectorStore(std::string name)
  {
    detMap[name]=new DetectorStore;
  }
  DetectorStore* GeometryManager::GetDetectorStore(std::string name)
  {
    if (detMap.find(name)!=detMap.end())
      return detMap[name];
    else
      {
        std::cout<<" Detector "<<name<<" not found!!!"<<std::endl;
        return 0;
      }
  }

}     // end namespace
