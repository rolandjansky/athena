///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// xAODBranchAddress.cxx 
// Implementation file for class Athena::xAODBranchAddress
// Author: Johannes ELmsheuser, Will Buttinger
/////////////////////////////////////////////////////////////////// 

// AthenaRootComps includes
#include "xAODBranchAddress.h"
#include "xAODTEvent.h"

// STL includes

// fwk includes
#include "GaudiKernel/System.h"
//#include "GaudiKernel/ClassID.h"
#include "SGTools/CLIDRegistry.h"

// ROOT includes
#include "TBranch.h"
#include "TTree.h"
#include "TLeaf.h"

#include "xAODRootAccess/TEvent.h"
#include "TClass.h"
#include <typeinfo> 


namespace Athena {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

/// Default constructor: 
xAODBranchAddress::xAODBranchAddress() :
  GenericAddress(),
  m_ptr    ()
{}

/// Copy constructor: 
xAODBranchAddress::xAODBranchAddress( const xAODBranchAddress& rhs ) :
  GenericAddress(rhs),
  m_ptr    (rhs.m_ptr)
{}

/// Assignment operator: 
xAODBranchAddress& 
xAODBranchAddress::operator=( const xAODBranchAddress& rhs )
{
  if (this != &rhs) {
    GenericAddress::operator=(rhs);
    m_ptr    = rhs.m_ptr;
  }
  return *this;
}

/// Constructor with parameters: 
xAODBranchAddress::xAODBranchAddress(long svc,
                                     const CLID& clid,
                                     const std::string& p1,
                                     const std::string& p2,
                                     unsigned long ip1,
                                     unsigned long ip2) :
  GenericAddress(svc, clid, p1, p2, ip1, ip2),
  m_ptr(0) //the actual data object that is retrieved
{
  // std::cerr << "::RBA::+RBA... (this=" << this << ") br=[" 
  //           << this->par()[1] << "]\n";
}

/// Destructor: 
xAODBranchAddress::~xAODBranchAddress()
{
  // std::cerr << "::RBA::~RBA... (this=" << this << ") br=["
  //           << this->par()[1] << "]\n";
}


xAOD::xAODTEvent*
  xAODBranchAddress::tevent()
{
  //std::cout << "xAODBranchAddress::tevent ..." << std::endl;
  return reinterpret_cast<xAOD::xAODTEvent*>(reinterpret_cast<unsigned long*>(this->ipar()[0]));  
}

void
xAODBranchAddress::setTEventAddress() 
{

  //std::cout << "xAODBranchAddress::setTEvent ..." << std::endl;



  const std::string& br_name = this->par()[1];
  xAOD::xAODTEvent* tevent = this->tevent();
  //xAOD::TEvent* tevent = this->tevent();
  //std::cout << "xAODBranchAddress::setTEventAddress  br_name=" << br_name << std::endl;
  //std::cout << "xAODBranchAddress::setTEventAddress  tevent=" << tevent << " entries = " << tevent->getEntries() << std::endl;
  //std::cout << "xAODBranchAddress::setTEventAddress  tlID()=" << this->clID() << std::endl;
  
  const std::type_info* ti = CLIDRegistry::CLIDToTypeinfo(this->clID());
  //std::cout << "xAODBranchAddress::setTEventAddress ti=" << ti << std::endl;

  //  void* addr = *(char**)&tevent;


  //we use the ipar()[1] to flag if we retrieve main event info (0) or metadata (1) (see xAODEventSelector where it creates BranchAddress)
  switch( (this->ipar()[1]) ) {
  case 0: 
    {//reading event level info
      tevent->setActive(); //ensure we are active tevent
      const void* addr = tevent->getInputObject(br_name, *ti);
      m_ptr = (void*)addr; 
    }
    break;
  case 1: 
    {//reading metadata
      //FIXME: NEEED TO ACCESS METADATA
      //std::cout << "xAODBranchAddress::setTEventAddress metadata " << br_name << std::endl;
      tevent->setActive(); //ensure we are active tevent
      const void* addr = tevent->getInputObject(br_name, *ti, false, true);
      m_ptr = (void*)addr; 
    }
    break;
  }

  //std::cout << "xAODBranchAddress::setTEventAddress m_ptr=" << m_ptr << std::endl;

 
}

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace Athena
