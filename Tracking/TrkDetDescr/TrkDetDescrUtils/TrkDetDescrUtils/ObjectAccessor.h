/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ObjectAccessor.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUTILS_OBJECTACCESSOR_H
#define TRKDETDESCRUTILS_OBJECTACCESSOR_H
//Gaudi
#include "GaudiKernel/MsgStream.h"
// STL
#include <iostream>
#include <vector>

namespace Trk {

  /**
    @class ObjectAccessor
    
    Base class for accessors to std::vectors,
    these should be optimized for different cases and
    surface types.
        
    @author Andreas.Salzburger@cern.ch
    */
    
    class ObjectAccessor {
      
      public:
        /** Default constructor*/
        ObjectAccessor(bool inverse=false) :
         m_inverseCase(inverse)
       {}

        /** Copy constructor */
        ObjectAccessor(const ObjectAccessor& oa);

        /** Destructor*/
        virtual ~ObjectAccessor(){}
    
        /** Assignment operator*/
        ObjectAccessor& operator=(const ObjectAccessor& oa);

        /** Interface method for surface the accessor int*/
        virtual int accessor() const = 0;
        
        /** Resets the accessor: begin*/
        virtual bool begin() const = 0;
        
        /** End of accessor */
        virtual bool end() const = 0;
        
        /** Required: operator++*/
        virtual const ObjectAccessor& operator++ () const = 0;
   
        /** Output Method for MsgStream, to be overloaded by child classes */
        virtual MsgStream& dump(MsgStream& sl) const = 0;
        /** Output Method for std::ostream, to be overloaded by child classes */
        virtual std::ostream& dump(std::ostream& sl) const = 0;

        /** Flag if object has to retrieved by backward search */
        virtual bool inverseRetrieval() const;

      private:
        bool m_inverseCase; //!< boolean flag to mark inverse retrieval
        
    };
 
  inline bool ObjectAccessor::inverseRetrieval() const 
    {  return m_inverseCase; }

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& operator << ( MsgStream& sl, const ObjectAccessor& oac);
std::ostream& operator << ( std::ostream& sl, const ObjectAccessor& oac);

} // end of namespace Trk

#endif

