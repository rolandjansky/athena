/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ThreeObjectsAccessor.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUTILS_THREEOBJECTSACCESSOR_H
#define TRKDETDESCRUTILS_THREEOBJECTSACCESSOR_H

//Gaudi
#include "GaudiKernel/MsgStream.h"
//Trk
#include "TrkDetDescrUtils/ObjectAccessor.h"
//STL
#include <vector>
#include <iostream>

namespace Trk {

  /**
    @class ThreeObjectsAccessor

    Explicit Accessor for three Objects in a specif way.
    The size is fixed to be three objects to guarantee right memory
    allocation in case of a static object.

    @author Andreas.Salzburger@cern.ch
    */

    class ThreeObjectsAccessor : public ObjectAccessor {

      public:
        /** Default constructor*/
        ThreeObjectsAccessor();

        /**Constructor of vector*/
        ThreeObjectsAccessor(int first,
                             int second,
                             int third);

        /**Copy constructor*/
        ThreeObjectsAccessor(const ThreeObjectsAccessor& accessor);

        /**Destructor*/
        ~ThreeObjectsAccessor(){}

        /**Assignment operator*/
        ThreeObjectsAccessor& operator= (const ThreeObjectsAccessor& rhs);

        /**The single interface  method*/
        int accessor() const;

        /** Resets the accessor: begin*/
        bool begin() const;

        /** End of accessor */
        bool end() const;

        /**Required: operator++*/
        const ThreeObjectsAccessor& operator++ () const;

        /** Output Method for MsgStream, to be overloaded by child classes */
        MsgStream& dump(MsgStream& sl) const;
        /** Output Method for std::ostream, to be overloaded by child classes */
        std::ostream& dump(std::ostream& sl) const;

      private:
        mutable int      m_jumper;
        int              m_accessors[3];
        mutable bool     m_break;

    };

   inline int ThreeObjectsAccessor::accessor() const
   { return m_accessors[m_jumper]; }

   inline bool ThreeObjectsAccessor::begin() const
   { m_jumper =0;
     m_break  = false;
     return true; }

   inline bool ThreeObjectsAccessor::end() const
   { return (!m_break); }

   inline const ThreeObjectsAccessor& ThreeObjectsAccessor::operator++() const
   {
     m_jumper = (m_jumper<2) ? m_jumper+1 : 0;
     m_break  = (!m_jumper);
     return (*this);
   }
}

#endif //  TRKDETDESCRUTILS_FOUROBJECTSACCESSOR_H

