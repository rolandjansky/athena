/////////////////// -*- C++ -*- //////////////////////////////////// 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DecayParser.h 
// Header file for class Object
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEUTILS_DECAYPARSER_H 
#define MCPARTICLEUTILS_DECAYPARSER_H 

/** DecayParser is the class which parses and models decay patterns.
 *  You can describe <i>via</i> a std::string the decay pattern you are
 *  looking for.
 *  <i>Eg:</i> you just have to say "Z0[e+;e-]" to create a pattern which
 *  can then be used to filter for vertices where such a decay occurs.
 */

// STL includes
#include <string>
#include <vector>

// EventKernel includes
#include "EventKernel/PdtPdg.h"

// fwd declare
struct _object;
typedef _object PyObject;

namespace McUtils {
  typedef std::vector<std::string> Strings;
}

class DecayParser
{
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /// Constructor with parameters: 
  DecayParser( const std::string& cmd );

  /// Destructor: 
  virtual ~DecayParser(); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////
  const std::vector<McUtils::Strings>& getParents() const;
  const std::vector<McUtils::Strings>& getChildren()  const;

  void dump() const;

  PDG::pidType pdgId( const std::string& pdgIdString ) const;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 
  void parse( const std::string& cmd );


  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /// Default constructor: 
  DecayParser(); //> not implemented

  /// Copy constructor: 
  DecayParser( const DecayParser& rhs ); //> not implemented

  /// Assignment operator: 
  DecayParser &operator=(const DecayParser &obj); //> not implemented

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** Print the content of a vector of McUtils::Strings to std::cout
   */
  void printMcUtilsStrings( const std::vector<McUtils::Strings>& list ) const;

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** python function to parse the input string modeling the decay pattern
   *  to look for.
   */
  PyObject *m_parseFct;

  /** List of parents : each slot of the vector is a list of candidates
   *  So one could have something like : [ ["22","23"] ] to model a decay
   *  of a gamma or a Z0
   */
  std::vector<McUtils::Strings> m_parents;

  /** List of children : each slot of the vector is a list of candidates
   *  So one could have something like : [ ["11","13"], ["-11","-13" ] ]
   *  to model a decay into a pair of electrons or muons
   */
  std::vector<McUtils::Strings> m_children;

}; 

/// I/O operators
//////////////////////
//MsgStream& operator<<( MsgStream & msg, const DecayParser &obj );

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline
const std::vector<McUtils::Strings>& 
DecayParser::getParents() const
{ 
  return m_parents;
}

inline 
const std::vector<McUtils::Strings>&
DecayParser::getChildren() const
{ 
  return m_children;
}

#endif //> MCPARTICLEUTILS_DECAYPARSER_H
