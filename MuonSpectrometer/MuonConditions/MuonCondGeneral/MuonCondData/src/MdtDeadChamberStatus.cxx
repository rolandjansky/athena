/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//----------------------------------------------------------------------
//The class storing a map of dead chamber name <-> map<multilayer, layer x tube>
// Z. Rurikova
//----------------------------------------------------------------------
#include "MuonCondData/MdtDeadChamberStatus.h"
#include <utility>
#include <sstream>
#include <iostream>
#include <iomanip>
using namespace std;

//----------------------------------------------------------------------
//constructor
MdtDeadChamberStatus::MdtDeadChamberStatus()
{}

//----------------------------------------------------------------------
//destructor
MdtDeadChamberStatus::~MdtDeadChamberStatus()
{ }

//----------------------------------------------------------------------
// Add map entries of Mdt data
bool MdtDeadChamberStatus::addChamberDead(const std::string & chamberId, const MdtMultilayerData & multilayerData)
{
  return (m_dataMap.insert(make_pair(chamberId, multilayerData))).second;
}

//----------------------------------------------------------------------
// Search the maps for a module, returns empty module container if not found
MdtDeadChamberStatus::MdtMultilayerData MdtDeadChamberStatus::findChamber(const std::string & chamberId)
{
  // Create container 
  MdtDeadChamberStatus::MdtMultilayerData MultilayerData;
  // Iterate over the map and look for the requested module
  MdtChamberData::const_iterator iter(m_dataMap.find(chamberId));
  if (iter not_eq m_dataMap.end()){
    MultilayerData  = iter->second;
  }
  return MultilayerData;
}

//----------------------------------------------------------------------
// Method to print the map - useful debug tool
std::string MdtDeadChamberStatus::str()
{
  ostringstream txt;
  if (m_dataMap.empty()){
    txt<<"No dead chambers are in the  MdtDeadChamber data map.\n";
  } else {
    MdtChamberData::const_iterator itr(m_dataMap.begin());
    for(; itr!=m_dataMap.end(); ++itr){
      txt << "\nData for chamber: " << itr->first << "\n" << chamberText(itr->first);
    }
  }
  return txt.str();
}

//----------------------------------------------------------------------
// Print data for one module
std::string MdtDeadChamberStatus::chamberText( const std::string & chamberId ){
  MdtMultilayerData thisMultilayerData = findChamber( chamberId );
  ostringstream txt;
  ostringstream tmp;

  if ( thisMultilayerData.empty() ){
    txt << "The MdtDeadChamberStatus data map is empty for chamber " << chamberId <<".\n";
  } else {
    txt << setw(10) << "Multilayer" 
        << setw(10) << "Layer" << setw(20) << "Tube" << "\n";
    MdtMultilayerData::const_iterator MdtMultilayerItr(thisMultilayerData.begin());
    for (; MdtMultilayerItr not_eq thisMultilayerData.end(); ++MdtMultilayerItr){
      std::vector<std::vector<int> > MdtLayerValues =MdtMultilayerItr->second;
      for ( unsigned int iLayer = 0; iLayer < MdtLayerValues.size(); iLayer++){
	txt << setw(10) << MdtMultilayerItr->first 
	    << setw(10) << iLayer +1 << setw(10) ;
	for ( unsigned int iTube = 0; iTube < MdtLayerValues.at(iLayer).size(); iTube++){
	   txt << MdtLayerValues.at(iLayer).at(iTube) << ", ";
	}  
	txt << "\n";	 
      }    
          
    }
  }
  return txt.str();
}

//----------------------------------------------------------------------
// Is the map empty?
bool MdtDeadChamberStatus::empty() const{
  return m_dataMap.empty();
}

//----------------------------------------------------------------------
// Clear map
void MdtDeadChamberStatus::clear() {
  m_dataMap.clear();
}

//----------------------------------------------------------------------
// return map object
MdtDeadChamberStatus::MdtChamberData MdtDeadChamberStatus::getDeadChamberMap(){
  return m_dataMap;
} 

//----------------------------------------------------------------------
// return vector of dead chambers
std::vector<std::string> MdtDeadChamberStatus::getDeadChambers() {

  std::vector<std::string> DeadChambers;
  MdtChamberData::const_iterator itr(m_dataMap.begin());
  
  for(; itr!=m_dataMap.end(); ++itr){
    DeadChambers.push_back(itr->first);
  }
  return DeadChambers;
} 

//----------------------------------------------------------------------
// return vector of dead multilayers
std::vector<int> MdtDeadChamberStatus::getDeadMultilayers(const std::string chamberId) {

  std::vector<int> DeadMultilayers;
  MdtMultilayerData thisMultilayerData = findChamber( chamberId );
 
  if (!thisMultilayerData.empty() ){
     MdtMultilayerData::const_iterator MdtMultilayerItr(thisMultilayerData.begin());
     for (; MdtMultilayerItr not_eq thisMultilayerData.end(); ++MdtMultilayerItr){
        DeadMultilayers.push_back(MdtMultilayerItr->first);  
     }    
  }
 
  return  DeadMultilayers;
} 

//----------------------------------------------------------------------
// return vector of dead layers
std::vector<int> MdtDeadChamberStatus::getDeadLayers(
         const std::string chamberId, const int multilayerId) {

  std::vector<int> DeadLayers;
  MdtMultilayerData thisMultilayerData = findChamber( chamberId );
 
  if (!thisMultilayerData.empty() ){
     MdtMultilayerData::const_iterator MdtMultilayerItr(thisMultilayerData.find(multilayerId));
     if( MdtMultilayerItr not_eq thisMultilayerData.end()){
       std::vector<std::vector<int> > MdtLayerValues = MdtMultilayerItr->second; 
       for ( unsigned int iLayer = 0; iLayer < MdtLayerValues.size(); iLayer++){
         DeadLayers.push_back(iLayer+1);
       }
     }    
  }
 
  return  DeadLayers;
} 

//----------------------------------------------------------------------
// return vector of dead tubes
std::vector<int> MdtDeadChamberStatus::getDeadTubes(
         const std::string chamberId, const int multilayerId, const int layerId) {

  std::vector<int> DeadTubes;
  MdtMultilayerData thisMultilayerData = findChamber( chamberId );
 
  if (!thisMultilayerData.empty() ){
     MdtMultilayerData::const_iterator MdtMultilayerItr(thisMultilayerData.find(multilayerId));
     if( MdtMultilayerItr not_eq thisMultilayerData.end()){
       std::vector<std::vector<int> > MdtLayerValues = MdtMultilayerItr->second; 
       for ( unsigned int iTube = 0; iTube < MdtLayerValues.at(layerId).size(); iTube++){
	  DeadTubes.push_back( MdtLayerValues.at(layerId).at(iTube));  
       }
     }    
  }
 
  return  DeadTubes;
} 

//----------------------------------------------------------------------
// The size of the map
unsigned int MdtDeadChamberStatus::nDeadChambers() const{
  return m_dataMap.size();
} 

//----------------------------------------------------------------------
// Return the number of dead multilayers for given chamber
unsigned int MdtDeadChamberStatus::nDeadMultilayers(const std::string & chamberId ){
  MdtMultilayerData thisMultilayerData = findChamber( chamberId );
  return thisMultilayerData.size();
} 

//----------------------------------------------------------------------
// Return the number of dead layers for given chamber
unsigned int MdtDeadChamberStatus::nDeadLayers(const std::string & chamberId ) {
  MdtMultilayerData thisMultilayerData = findChamber( chamberId );
  MdtMultilayerData::const_iterator MdtMultilayerItr(thisMultilayerData.begin());
  
  unsigned int Nlayers=0;
  for (; MdtMultilayerItr not_eq thisMultilayerData.end(); ++MdtMultilayerItr){
     std::vector<std::vector<int> > MdtLayerValues =MdtMultilayerItr->second; 
     Nlayers+= MdtLayerValues.size();
  }   
  
  return  Nlayers;  
} 

//----------------------------------------------------------------------
// Return the number of dead tubes for given chamber
unsigned int MdtDeadChamberStatus::nDeadTubes(const std::string & chamberId ) {
  MdtMultilayerData thisMultilayerData = findChamber( chamberId );
  MdtMultilayerData::const_iterator MdtMultilayerItr(thisMultilayerData.begin());
  
  unsigned int Ntubes=0;
  for (; MdtMultilayerItr not_eq thisMultilayerData.end(); ++MdtMultilayerItr){
     std::vector<std::vector<int> > MdtLayerValues =MdtMultilayerItr->second; 
     for ( unsigned int iLayer = 0; iLayer < MdtLayerValues.size(); iLayer++){
       Ntubes+= MdtLayerValues.at(iLayer).size();
     }
  }   
  
  return  Ntubes;  
} 

