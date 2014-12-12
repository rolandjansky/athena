/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EightObjectsAccessor.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUTILS_EIGHTOBJECTSACCESSOR_H
#define TRKDETDESCRUTILS_EIGHTOBJECTSACCESSOR_H

//Trk
#include "TrkDetDescrUtils/ObjectAccessor.h"
//STL
#include <vector>
#include <iostream>

namespace Trk {

  /**
    @class EightObjectsAccessor

    Explicit Accessor for four Objects in a specif way.
    The size is fixed to be four objects to guarantee right memory
    allocation in case of a static object.

    @author Andreas.Salzburger@cern.ch
    */

    class EightObjectsAccessor : public ObjectAccessor {

      public:
        /** Default constructor*/
        EightObjectsAccessor();

        /**Constructor of vector*/
        EightObjectsAccessor(int first,
                           int second,
                           int third,
                           int fourth,
                           int fifth,
                           int sixth,
                           int seventh,
                           int eighth);

        /**Copy constructor*/
        EightObjectsAccessor(const EightObjectsAccessor& accessor);

        /**Destructor*/
        ~EightObjectsAccessor(){}

        /**Assignment operator*/
        EightObjectsAccessor& operator= (const EightObjectsAccessor& rhs);

        /**The single interface  method*/
        int accessor() const;

        /** Resets the accessor: begin*/
        bool begin() const;

        /** End of accessor */
        bool end() const;

        /**Required: operator++*/
        const EightObjectsAccessor& operator++ () const;

        /** Output Method for MsgStream, to be overloaded by child classes */
        MsgStream& dump(MsgStream& sl) const;
        /** Output Method for std::ostream, to be overloaded by child classes */
        std::ostream& dump(std::ostream& sl) const;

      private:
        mutable int      m_jumper;
        int              m_accessors[8];

    };

   inline int EightObjectsAccessor::accessor() const
   { return m_accessors[m_jumper]; }

   inline bool EightObjectsAccessor::begin() const
   { m_jumper=0;
     return true; }

   inline bool EightObjectsAccessor::end() const
   { return !(m_jumper==7); }

   inline const EightObjectsAccessor& EightObjectsAccessor::operator++() const
   {
     m_jumper = (m_jumper<7) ? m_jumper+1 : 0;
     return (*this);
   }
}


#endif //  TRKDETDESCRUTILS_EIGHTOBJECTSACCESSOR_H

