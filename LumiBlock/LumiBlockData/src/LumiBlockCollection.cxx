/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// LumiBlockCollection.cxx 
// Implementation file for class LumiBlockCollection
// Author: Marjorie Shapiro <mdshapiro@lbl.gov>
/////////////////////////////////////////////////////////////////// 
 
// STL includes

// LumiBlockCollection include
#include "LumiBlockData/LumiBlockCollection.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

///////////////////////////////////////////////////////////////////
/// Constructors
////////////////

LumiBlockCollection::LumiBlockCollection(): DataVector<LB_IOVRange> (SG::OWN_ELEMENTS) {}

LumiBlockCollection::LumiBlockCollection( const LumiBlockCollection& rhs ) : 
   DataVector<LB_IOVRange> (rhs) {}
 
LumiBlockCollection& LumiBlockCollection::operator=( const LumiBlockCollection& rhs ) 
   {
     if ( this != &rhs ) {
       DataVector<LB_IOVRange>::operator=(rhs);
     }
   return *this;
   }

   LumiBlockCollection::LumiBlockCollection( const SG::OwnershipPolicy own ) :
     DataVector<LB_IOVRange>(own)
   {}

/////////////////////////////////////////////////////////////////////
// Destructor
///////////////

 /////////////////////////////////////////////////////////////////// 
 /// Const methods: 
 ///////////////////////////////////////////////////////////////////
    std::ostream& LumiBlockCollection::dump( std::ostream& out ) const 
   {
     out << " Dump of LumiBlockCollection with size: " << this->size()
	 << " has the following (run,lumiblock) ranges: "
         << std::endl;
     LumiBlockCollection::const_iterator it;
     for(it=begin(); it!=end(); ++it) {
       out << "\t [ (" 
           << (*it)->start().run() << "," << (*it)->start().event() 
	   << "):("
           << (*it)->stop().run() << "," << (*it)->stop().event() 
     	   << ") eventsSeen = " << (*it)->getNumSeen()  
           << ", eventsExpected = " << (*it)->getNumExpected() 
           << " ]" 
           << std::endl;
     }
     return out;
    }

