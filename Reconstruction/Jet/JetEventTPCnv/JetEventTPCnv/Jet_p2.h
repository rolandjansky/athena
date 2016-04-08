///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Jet_p2.h 
// Header file for class Jet_p2
// Author: S.Binet<binet@cern.ch>
// Author: R.Seuster<seuster@cern.ch>
// Date:   March 2007
// Date:   Feb 2008
/////////////////////////////////////////////////////////////////// 
#ifndef JETEVENTTPCNV_JET_P2_H 
#define JETEVENTTPCNV_JET_P2_H 

// STL includes
//#include <vector>

// DataModelAthenaPool includes
#include "DataModelAthenaPool/Navigable_p1.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4PxPyPzE_p1.h"

// forward declarations
class JetTagInfoBase;
class JetAssociationBase;

class JetCnv_p2;

class Jet_p2 
{
  /////////////////////////////////////////////////////////////////// 
  // Friend classes
  /////////////////////////////////////////////////////////////////// 

  // Make the AthenaPoolCnv class our friend
  friend class JetCnv_p2;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
public: 

  /** Default constructor: 
   */
  Jet_p2();

  // Warning: this is a _move_ constructor, not really a copy constructor.
  // See the description in the .cxx file.
  Jet_p2(const Jet_p2& other);

  /** Destructor: 
   */
  ~Jet_p2();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
private: 

  /// the navigable part 
  Navigable_p1<uint32_t, double> m_nav;

  /// the 4-mom part
  P4PxPyPzE_p1 m_momentum;
  
  unsigned int m_author;
  
  /// combined likelihoods
  std::vector<double> m_combinedLikelihood;

  /// JetMomentStore

  std::vector<float> m_shapeStore;
  
  /// JetTagInfoBase objects
  std::vector<const JetTagInfoBase*> m_tagJetInfo;
  
  /// JetAssociationBase objects
  std::vector<const JetAssociationBase*> m_associations;

  /// True if we own the objects pointed to from the above collections.
  // This should be true for the case of reading, but false for writing.
  // Therefore, it is set to true in the default constructor.
  // When the T->P converter puts pointers in one of these objects,
  // it should also clear this flag.
  bool m_ownPointers;
  
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline Jet_p2::Jet_p2()
  : m_ownPointers (true)
{}

#endif //> JETEVENTTPCNV_JET_P1_H
