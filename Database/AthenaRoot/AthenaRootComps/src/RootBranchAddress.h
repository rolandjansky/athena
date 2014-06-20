///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RootBranchAddress.h 
// Header file for class Athena::RootBranchAddress
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENAROOTCOMPS_ATHENAROOTBRANCHADDRESS_H
#define ATHENAROOTCOMPS_ATHENAROOTBRANCHADDRESS_H 1

// STL includes
#include <string>

// Gaudi includes
#include "GaudiKernel/GenericAddress.h"

#include "DataModelRoot/RootType.h"

// Forward declaration
class TTree;
class TBranch;

namespace Athena {

  /** @class Athena::RootBranchAddress
   *  A simple class to hold the buffer of a @c TBranch from a @c TTree
   */
class RootBranchAddress 
  : public GenericAddress
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /// Default constructor: 
  RootBranchAddress();

  /// Copy constructor: 
  RootBranchAddress( const RootBranchAddress& rhs );

  /// Assignment operator: 
  RootBranchAddress& operator=( const RootBranchAddress& rhs ); 

  /// Constructor with parameters: 
  RootBranchAddress(long svc,
                    const CLID& clid,
                    const std::string& p1="",
                    const std::string& p2="",
                    unsigned long ip1=0,
                    unsigned long ip2=0);

  /// Destructor: 
  virtual ~RootBranchAddress(); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /// setup the ROOT @c TTree internal address for the branch.
  /// as a side-effect, this will load the data from the tree for this
  /// branch and entry.
  /// @param t additional RootType hint (we check the consistency)
  /// FIXME: should the TBranch::GetEntry be lifted into its own method ?
  ///        for the moment, that doesn't seem necessary, as setBranchAddress
  ///        is only used in one place (LeafCnv) where the intent is to load
  ///        the data anyway...
  void
  setBranchAddress(const RootType& t);

  /// the @c TTree whose branch we proxy
  TTree *ttree();

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
  //private: 

  /// the buffer for the @c TBranch
  RootType m_type;
  void* m_ptr;

  /// the branch we are connected to.
  TBranch *m_branch;
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
//std::ostream& operator<<( std::ostream& out, const RootBranchAddress& o );

} //> end namespace Athena

#endif //> !ATHENAROOTCOMPS_ROOTBRANCHADDRESS_H
