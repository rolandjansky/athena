/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SixObjectsAccessor.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUTILS_SIXOBJECTSACCESSOR_H
#define TRKDETDESCRUTILS_SIXOBJECTSACCESSOR_H

//Gaudi
#include "GaudiKernel/MsgStream.h"
//Trk
#include "TrkDetDescrUtils/ObjectAccessor.h"
//STL
#include <vector>
#include <iostream>

namespace Trk {

  /**
    @class SixObjectsAccessor

    Explicit Accessor for four Objects in a specif way.
    The size is fixed to be four objects to guarantee right memory
    allocation in case of a static object.

    @author Andreas.Salzburger@cern.ch
    */

    class SixObjectsAccessor : public ObjectAccessor {

      public:
        /** Default constructor*/
        SixObjectsAccessor();

        /**Constructor of vector*/
        SixObjectsAccessor(int first,
                           int second,
                           int third,
                           int fourth,
                           int fifth,
                           int sixth);

        /**Copy constructor*/
        SixObjectsAccessor(const SixObjectsAccessor& accessor);

        /**Destructor*/
        ~SixObjectsAccessor(){}

        /**Assignment operator*/
        SixObjectsAccessor& operator= (const SixObjectsAccessor& rhs);

        /**The single interface  method*/
        int accessor() const;

        /** Resets the accessor: begin*/
        bool begin() const;

        /** End of accessor */
        bool end() const;

        /**Required: operator++*/
        const SixObjectsAccessor& operator++ () const;

        /** Output Method for MsgStream, to be overloaded by child classes */
        MsgStream& dump(MsgStream& sl) const;
        /** Output Method for std::ostream, to be overloaded by child classes */
        std::ostream& dump(std::ostream& sl) const;

      private:
        mutable int      m_jumper;
        int              m_accessors[6];
        mutable bool     m_break;

    };

   inline int SixObjectsAccessor::accessor() const
   { return m_accessors[m_jumper]; }

   inline bool SixObjectsAccessor::begin() const
   { m_jumper=0;
     m_break  = false;
     return true; }

   inline bool SixObjectsAccessor::end() const
   { return (!m_break); }

   inline const SixObjectsAccessor& SixObjectsAccessor::operator++() const
   {
     m_jumper = (m_jumper<5) ? m_jumper+1 : 0;
     m_break  = (!m_jumper);
     return (*this);
   }
}

#endif //  TRKDETDESCRUTILS_SIXOBJECTSACCESSOR_H

