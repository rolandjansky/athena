/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DecisionHandling_HLTIdentifier_h
#define DecisionHandling_HLTIdentifier_h

#include <string>
#include <vector>
#include <set>
#include <map>

#include "GaudiKernel/MsgStream.h"
#include "CxxUtils/checker_macros.h"

/**
 * @brief An trigger identifier class, used to provide mapping fromt the human readable IDs to efficienct unsigned ints
 **/
namespace HLT {
class Identifier {
public:
  /**
   * @brief disables reporting human readable names == no debugging, fast execution
   **/
  static void reportStringIDs( bool report ) { s_reportStringIDs = report; }
  static bool reportStringIDs() { return s_reportStringIDs; }
  
  static HLT::Identifier fromToolName( const std::string& tname );
  /**
   * @brief constructs identifier from human redable name
   **/  
  Identifier( const std::string& stringID );

  /**
   * @brief Construct wiht numeric ID
   **/
 Identifier( unsigned id ) : m_id( id ) {}

  /**
   * @brief reports human redable name if it is enabled or, empty string
   **/  
  std::string name() const;
  
  /**
   * @brief numeric ID
   **/    
  inline unsigned numeric() const { return m_id; }
  inline operator unsigned () const { return numeric(); }
  /**
   *  @brief comparisons, for containers of identifiers
   **/      
  bool operator == ( const Identifier& rhs )  const { return numeric() == rhs.numeric(); }
  bool operator < ( const Identifier& rhs )  const { return numeric() < rhs.numeric(); }
  bool operator < ( unsigned id ) const { return numeric() < id; } 
private:
  unsigned m_id;
  static bool s_reportStringIDs ATLAS_THREAD_SAFE;
  
};
 typedef std::vector<HLT::Identifier> IDVec;
 typedef std::set<HLT::Identifier> IDSet;
 typedef std::map<HLT::Identifier,  IDVec> IDtoIDVecMap;
}

MsgStream& operator<< ( MsgStream& m, const HLT::Identifier& id );



#endif // HLTIdentifier
