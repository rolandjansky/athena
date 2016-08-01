///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// XAODBranchAddress.h 
// Header file for class Athena::XAODBranchAddress
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENAROOTCOMPS_ATHENAXAODBRANCHADDRESS_H
#define ATHENAROOTCOMPS_ATHENAXAODBRANCHADDRESS_H 1

// STL includes
#include <string>

// Gaudi includes
#include "GaudiKernel/GenericAddress.h"

#include "DataModelRoot/RootType.h"

#include "xAODTEvent.h"

namespace Athena {

  /** @class Athena::xAODBranchAddress
   *  A simple class to do the retrieve from TEvent
   */
class xAODBranchAddress 
  : public GenericAddress
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /// Default constructor: 
  xAODBranchAddress();

  /// Copy constructor: 
  xAODBranchAddress( const xAODBranchAddress& rhs );

  /// Assignment operator: 
  xAODBranchAddress& operator=( const xAODBranchAddress& rhs ); 

  /// Constructor with parameters: 
  xAODBranchAddress(long svc,
                    const CLID& clid,
                    const std::string& p1="",
                    const std::string& p2="",
                    unsigned long ip1=0,
                    unsigned long ip2=0);

  /// Destructor: 
  virtual ~xAODBranchAddress(); 



  xAOD::xAODTEvent *tevent();
  void setTEventAddress();


  //the data we get from the TEvent
  void* m_ptr;


}; 



} //> end namespace Athena

#endif //> !ATHENAROOTCOMPS_XAODBRANCHADDRESS_H
