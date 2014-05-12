/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FiveObjectsAccessor.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUTILS_FIVEOBJECTSACCESSOR_H
#define TRKDETDESCRUTILS_FIVEOBJECTSACCESSOR_H

//Gaudi
#include "GaudiKernel/MsgStream.h"
//Trk
#include "TrkDetDescrUtils/ObjectAccessor.h"

//STL
#include <vector>
#include <iostream>

namespace Trk {

  /**
    @class FiveObjectsAccessor

    Explicit Accessor for five Objects in a specif way.
    The size is fixed to be five objects to guarantee right memory
    allocation in case of a static object.

    @author Andreas.Salzburger@cern.ch
    */

    class FiveObjectsAccessor : public ObjectAccessor {

      public:
        /** Default constructor*/
        FiveObjectsAccessor();

        /**Constructor of vector*/
        FiveObjectsAccessor(int first,
                            int second,
                            int third,
                            int fourth,
                            int five);

        /**Copy constructor*/
        FiveObjectsAccessor(const FiveObjectsAccessor& accessor);

        /**Destructor*/
        ~FiveObjectsAccessor(){}

        /**Assignment operator*/
        FiveObjectsAccessor& operator= (const FiveObjectsAccessor& rhs);

        /**The single interface  method*/
        int accessor() const;

        /** Resets the accessor: begin*/
        bool begin() const;

        /** End of accessor */
        bool end() const;

        /**Required: operator++*/
        const FiveObjectsAccessor& operator++ () const;

        /** Output Method for MsgStream, to be overloaded by child classes */
        MsgStream& dump(MsgStream& sl) const;
        /** Output Method for std::ostream, to be overloaded by child classes */
        std::ostream& dump(std::ostream& sl) const;

      private:
        mutable int      m_jumper;
        int              m_accessors[5];
        mutable bool     m_break;

    };

   inline int FiveObjectsAccessor::accessor() const
   { return m_accessors[m_jumper]; }

   inline bool FiveObjectsAccessor::begin() const
   { m_jumper = 0;
     m_break  = false;
     return true; }

   inline bool FiveObjectsAccessor::end() const
   { return (!m_break); }

   inline const FiveObjectsAccessor& FiveObjectsAccessor::operator++() const
   {
     m_jumper = (m_jumper<4) ? m_jumper+1 : 0;
     m_break  = (!m_jumper);
     return (*this);
   }
}

#endif //  TRKDETDESCRUTILS_FIVEOBJECTSACCESSOR_H

