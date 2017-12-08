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

#include "RegSelLUT/StoreGateIDRS_ClassDEF.h" 

#include "RegSelLUT/RegSelSiLUT.h" 

using namespace std;

#define USE_STOREGATE


/////////////////////////////////////////////////////////////////////////////

FTK_RegionSelectorTable::FTK_RegionSelectorTable(const std::string& type,
						 const std::string& name,
						 const IInterface* parent)
  :  AthAlgTool(type,name,parent),
     m_regionLUT(nullptr),
     m_managerName(""),
     m_roiFileName("RoITable.txt"),
     m_printHashId(true),
     m_printTable(false)
{
  declareInterface<IRegionIDLUT_Creator>(this);
  declareProperty("ManagerName", m_managerName);

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
RegSelSiLUT* FTK_RegionSelectorTable::getLUT() const
{
  return m_regionLUT;
}
	 


StatusCode 
FTK_RegionSelectorTable::createTable()
{
  if ( msgLvl(MSG::DEBUG) )  msg(MSG::DEBUG) << "Creating region selector table"  << endmsg;
  RegSelSiLUT* rd = new RegSelSiLUT(RegSelSiLUT::FTK);  /// NB: use default internal size parameters here

  /// Whay hey!!! so here we actually need to somehow create the lookup objects and
  /// add them to the table
  msg(MSG::INFO) << " initialising new map " << endmsg;
  std::string detName;
  std::string newkey;
  newkey  = "FTKRegSelSiLUT";
  detName = "FTK";

#ifndef USE_STOREGATE
  if ( m_regionLUT ) delete m_regionLUT;
#endif

  m_regionLUT = rd;

  /// create here, some dummy modules and add them to the lookup table
  /// NB: when the ftk code is set up, these modules will need to be
  ///     created from parameters extracted from an fk helper of some
  ///     sort  


  /// FIXME: these are just *fake* vlaues - really the values should 
  ///        come automatically from the FTK detector manager
  ///        when they are abailable from there, this *must* be changed 
  ///        to use it

  int hashid=0;

  const int Nz   =  4;
  const int Nphi = 32;
  
  /// fake limits in z
  double zmin[Nz+1] = {  -200,  -100, 0,  100,  200 };
  double zmax[Nz+1] = { -5000, -2000, 0, 2000, 5000 };

  /// fake limits in r
  double rmin =  30;
  double rmax = 600;

  /// just replace this by the loop over regions omn the FTK and 
  /// calculating the actual parameters 

  double dphi = 2*M_PI/Nphi;
  msg(MSG::INFO) << "generating dummy FTK modules" << endmsg;
  for ( int i=0; i<Nphi ; i++ ) { 
    for ( int j=0; j<Nz ; j++ ) { 
      double phimin=j*dphi-M_PI;
      double phimax=(j+1)*dphi-M_PI;

      /// use int rather than IdentifierHash here, since an IdentiferHash
      /// is just a wrapper to an int, but has no ++ operator etc
      RegSelModule m( zmin[j], zmin[j+1], 
		      zmax[j], zmax[j+1], 
		      rmin, rmax, phimin, phimax,  0, 0, hashid, hashid );

      rd->addModule( m );

      msg(MSG::DEBUG) << "\t" << m << endmsg;

      hashid++;

    }
  }    
  

#ifdef USE_STOREGATE
  // save new map in StoreGate 
  if ( detStore()->contains< RegSelSiLUT >(newkey) ) {
    msg(MSG::FATAL) << " RegSelSiLUT " << newkey << " already exists " << endmsg;
  } else {
    // create and store LUT
    // needs to be modifiable so we can enable/disable modules 
    // from the RegSelSvc (probably not for FTK however)
    StatusCode sc = detStore()->record(rd, newkey, true);
    if ( sc.isFailure() ) {
      msg(MSG::ERROR) << " could not register " << detName << " RegSelSiLUT" << endmsg;
      return( StatusCode::FAILURE );
    } else {
      msg(MSG::INFO) << detName << " RegSelSiLUT successfully saved in detector Store" << endmsg;
    }
  }
#endif
 
  return StatusCode::SUCCESS;
}



StatusCode FTK_RegionSelectorTable::finalize() {
  msg(MSG::INFO) << "finalize()" << endmsg;
  return StatusCode::SUCCESS;
}

