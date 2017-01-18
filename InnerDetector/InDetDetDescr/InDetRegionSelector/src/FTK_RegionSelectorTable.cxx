/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetRegionSelector/FTK_RegionSelectorTable.h"

// #include "GaudiKernel/MsgStream.h"
// #include "StoreGate/StoreGateSvc.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "Identifier/IdentifierHash.h"

#include <iostream>
#include <fstream>
#include <fstream>

using namespace std;

#include "RegSelLUT/StoreGateIDRS_ClassDEF.h" 

#include "RegSelLUT/RegSelEtaPhiLUT.h" 


#define USE_STOREGATE


/////////////////////////////////////////////////////////////////////////////

FTK_RegionSelectorTable::FTK_RegionSelectorTable(const std::string& type,
						 const std::string& name,
						 const IInterface* parent)
  :  AthAlgTool(type,name,parent),
     m_regionLUT(0),
     m_managerName(""),
     m_deltaZ(168 * CLHEP::mm),
     m_roiFileName("RoITable.txt"),
     m_printHashId(true),
     m_printTable(false)
{
  declareInterface<IRegionFTKLUT_Creator>(this);
  declareProperty("ManagerName", m_managerName);
  declareProperty("DeltaZ",      m_deltaZ);

  // The remaining properties are for debugging purposes.
  declareProperty("OutputFile",  m_roiFileName);
  declareProperty("PrintHashId", m_printHashId);
  declareProperty("PrintTable",  m_printTable);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode 
FTK_RegionSelectorTable::initialize(){
  msg(MSG::INFO) << "initialize() " << name() << " " << PACKAGE_VERSION << endmsg;
  msg(MSG::INFO)  << "Tool Properties" << endmsg;
  msg(MSG::INFO)  << " Detector Manager: " << m_managerName << endmsg;
  msg(MSG::INFO)  << " DeltaZ:           " << m_deltaZ/CLHEP::mm << " mm <<< NB: this parameter is now OBSOLETE" << endmsg;
  if( msgLvl(MSG::DEBUG) ) {
    msg(MSG::DEBUG) << " Output File:      " << m_roiFileName <<endmsg;
    msg(MSG::DEBUG) << " Print hashId:     " << ((m_printHashId) ? "true" : "false") <<endmsg;
    msg(MSG::DEBUG) << " Print Table:      " << ((m_printTable) ? "true" : "false") <<endmsg;
  }    

  if (m_managerName.empty()) {
    msg(MSG::WARNING) << "no manager found - so far none for the FTK tables" << endmsg;
  } 
 
  StatusCode  sc;
  sc = createTable();
  return sc;
}


FTK_RegionSelectorTable::~FTK_RegionSelectorTable()
{
  // table is stored in storegate so nothing to delete.
#ifndef USE_STOREGATE
  if ( m_regionLUT ) delete m_regionLUT;
#endif
}


// Get the lookup table.
RegSelEtaPhiLUT* FTK_RegionSelectorTable::getLUT() const
{
  return m_regionLUT;
}
	 



StatusCode 
FTK_RegionSelectorTable::createTable()
{
  if ( msgLvl(MSG::DEBUG) )  msg(MSG::DEBUG) << "Creating region selector table"  << endmsg;
  RegSelEtaPhiLUT* rd = new RegSelEtaPhiLUT();  /// NB: use default internal size parameters here

  /// Whay hey!!! so here we actually need to somehow create the lookup objects and
  /// add them to the table
  msg(MSG::INFO) << " initialising new map " << endmsg;
  std::string detName;
  std::string newkey;
  newkey  = "FTKRegSelEtaPhiLUT";
  detName = "FTK";

#ifndef USE_STOREGATE
  if ( m_regionLUT ) delete m_regionLUT;
#endif
  m_regionLUT = rd;

  /// create here, some dummy modules and add them to the lookup table
  /// NB: when the ftk code is set up, these modules will need to be
  ///     created from parameters extracted from an fk helper of some
  ///     sort  

  int hashid=0;
  int Neta = 32;
  int Nphi = 32;
  double deta = 6./Neta;
  double dphi = 2*M_PI/Nphi;
  msg(MSG::INFO) << "generating dummy FTK modules" << endmsg;
  for ( int i=0; i<Neta ; i++ ) { 
    for ( int j=0; j<Nphi ; j++ ) { 
      double eta=-3+i*deta;
      double phi=j*dphi;
      /// use int rather than IdentifierHash here, since an IdentiferHash
      /// is just a wrapper to an int, but has no ++ operator etc
      RegSelEtaPhiLUT::EtaPhiModule m(  eta, eta+deta, phi, phi+dphi,  hashid++ );
      rd->addModule( m );
      msg(MSG::DEBUG) << "\t" << m << endmsg;
    }
  }    
  

#ifdef USE_STOREGATE
  // save new map in StoreGate RegSelEtaPhiLUT
  StatusCode sc = detStore()->contains< RegSelEtaPhiLUT >(newkey);
  if (sc == StatusCode::SUCCESS ) {
    msg(MSG::FATAL) << " RegSelEtaPhiLUT " << newkey << " already exists " << endmsg;
  } else {
    // create and store LUT
    // needs to be modifiable so we can enable/disable modules 
    // from the RegSelSvc (probably not for FTK however)
    sc = detStore()->record(rd, newkey, true);
    if ( sc.isFailure() ) {
      msg(MSG::ERROR) << " could not register " << detName << " RegSelEtaPhiLUT" << endmsg;
      return( StatusCode::FAILURE );
    } else {
      msg(MSG::INFO) << detName << " RegSelEtaPhiLUT successfully saved in detector Store" << endmsg;
    }
  }
#endif
 
  return StatusCode::SUCCESS;
}



StatusCode FTK_RegionSelectorTable::finalize() {
  msg(MSG::INFO) << "finalize()" << endmsg;
  return StatusCode::SUCCESS;
}

