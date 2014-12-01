/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File: TileAssocTrackClusters.cxx
 * Author: Carlos.Solans@cern.ch
 * June 2014
 * Implementation of a VoidBlockFillerTool
 * PhysicsAnalysis/D3PDMaker/D3PDMakerUtils
 */

#include "TileAssocFillerTool.h"

using namespace std;
using namespace D3PD;

//====================================================================
TileAssocFillerTool::TileAssocFillerTool(const string& type,const string& name, const IInterface* parent):
  BlockFillerTool<void>(type,name,parent){
//====================================================================
  
  /* ContainerName string storegate key that contains the association to be writen to D3PD
   * BrachName string name of the branch used for the association prepended with target and prefix
   */
  declareProperty("ContainerName",m_containerName="AssociationKey"); 
  declareProperty("BranchName",m_branchName="_index");

  /* I have no idea what is this for */
  book().ignore(); // Avoid coverity warnings
} 

//=====================================================
StatusCode TileAssocFillerTool::initialize(){
//=====================================================
  /* Initialize the tool by calling the parent */
  CHECK( BlockFillerTool<void>::initialize() );

  /* Get storegate for later use */
  CHECK( service("StoreGateSvc",m_storeGate) );
 
  /* Create the object that will contain the association */
  m_container = new ASSOCCONTAINER_CONSTRUCTOR(0);
  
  return StatusCode::SUCCESS;
} 

//=====================================================
StatusCode TileAssocFillerTool::book(){
//=====================================================

  /* Register the association object in the D3PD */
  CHECK( addVariable(m_branchName,m_container));

  return StatusCode::SUCCESS;
} 

//=====================================================================
StatusCode TileAssocFillerTool::fill(){
//=====================================================================
  
  /* Get the association from storegate */
  ASSOCCONTAINER * cont;
  CHECK( m_storeGate->retrieve(cont,m_containerName));   

  /* Copy the association to the local variable for the D3PD */
  m_container->clear();
  m_container->resize(cont->size());
  for(unsigned int i=0; i< cont->size(); i++){
    m_container->at(i).resize(cont->at(i).size());
    for(unsigned int j=0; j< cont->at(i).size(); j++){
      m_container->at(i).at(j)=cont->at(i).at(j);
    }
  }
  
  /* Association object will be written to the D3PD after this */

  return StatusCode::SUCCESS;
} 

