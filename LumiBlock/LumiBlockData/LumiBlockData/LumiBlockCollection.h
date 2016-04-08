/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// Header file for class LumiBlockCollection
// Author: Marjorie Shapiro <mdshapiro@lbl.gov>
/////////////////////////////////////////////////////////////////// 
#ifndef LUMIBLOCKCOLLECTION_H 
#define LUMIBLOCKCOLLECTION_H 
 
// STL includes
#include <iostream>

// DataModel includes
#include "DataModel/DataVector.h"

// CLIDSvc includes (for persistency)
#include "CLIDSvc/CLASS_DEF.h"

#include "LumiBlockData/LB_IOVRange.h"

class LumiBlockCollection: public DataVector<LB_IOVRange> 
{
 public:
  // Default Constructor
  LumiBlockCollection();

  // Copy constructor: 
  LumiBlockCollection( const LumiBlockCollection& rhs );
 
  // Assignment operator: 
  LumiBlockCollection& operator=( const LumiBlockCollection& rhs ); 

  //Constructor with parameters: to setup the ownership policy
  LumiBlockCollection( const SG::OwnershipPolicy own );

  //  Destructor
  ~LumiBlockCollection();

  // const methods:
   std::ostream& dump( std::ostream& out = std::cout ) const;

  // This is just a useful class for sorting inside the collection
   class SortIOVRangeByStart {
     public:
        bool operator() ( const IOVRange& t1, const IOVRange& t2 ) const {
            return (t1.start()).re_time() < (t2.start()).re_time();
       }
        bool operator() ( const IOVRange* t1, const IOVRange* t2 ) const {
            return (t1->start()).re_time() < (t2->start()).re_time();
        }
        bool operator() ( IOVRange& t1,  IOVRange& t2 ) const {
            return (t1.start()).re_time() < (t2.start()).re_time();
       }
        bool operator() ( IOVRange* t1,  IOVRange* t2 ) const {
            return (t1->start()).re_time() < (t2->start()).re_time();
        }
      };
};

 inline 
 LumiBlockCollection::~LumiBlockCollection()
 {}

CLASS_DEF( LumiBlockCollection , 210948284 , 1 )

#endif //> LUMIBLOCKCOLLECTION_H
