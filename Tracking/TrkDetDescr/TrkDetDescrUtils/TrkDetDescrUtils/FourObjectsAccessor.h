/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FourObjectsAccessor.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUTILS_FOUROBJECTSACCESSOR_H
#define TRKDETDESCRUTILS_FOUROBJECTSACCESSOR_H

//Gaudi
#include "GaudiKernel/MsgStream.h"
//Trk
#include "TrkDetDescrUtils/ObjectAccessor.h"
//STL
#include <vector>
#include <iostream>

namespace Trk {

  /**
    @class FourObjectsAccessor

    Explicit Accessor for four Objects in a specif way.
    The size is fixed to be four objects to guarantee right memory
    allocation in case of a static object.

    @author Andreas.Salzburger@cern.ch
    */

    class FourObjectsAccessor : public ObjectAccessor {

      public:
        /** Default constructor*/
        FourObjectsAccessor();

        /**Constructor of vector*/
        FourObjectsAccessor(int first,
                            int second,
                            int third,
                            int fourth,
                            bool inverse=false);

        /**Copy constructor*/
        FourObjectsAccessor(const FourObjectsAccessor& accessor);

        /**Destructor*/
        ~FourObjectsAccessor(){}

        /**Assignment operator*/
        FourObjectsAccessor& operator= (const FourObjectsAccessor& rhs);

        /**The single interface  method*/
        int accessor() const;

        /** Resets the accessor: begin*/
        bool begin() const;

        /** End of accessor */
        bool end() const;

        /**Required: operator++*/
        const FourObjectsAccessor& operator++ () const;

        /** Output Method for MsgStream, to be overloaded by child classes */
        MsgStream& dump(MsgStream& sl) const;
        /** Output Method for std::ostream, to be overloaded by child classes */
        std::ostream& dump(std::ostream& sl) const;

      private:
        mutable int      m_jumper;
        int              m_accessors[4];
        mutable bool     m_break;

    };

   inline int FourObjectsAccessor::accessor() const
   { return m_accessors[m_jumper]; }

   inline bool FourObjectsAccessor::begin() const
   { m_jumper = 0;
     m_break  = false;
     return true; }

   inline bool FourObjectsAccessor::end() const
   { return (!m_break); }

   inline const FourObjectsAccessor& FourObjectsAccessor::operator++() const
   {
     m_jumper = (m_jumper<3) ? m_jumper+1 : 0;
     m_break  = (!m_jumper);
     return (*this);
   }
}

#endif //  TRKDETDESCRUTILS_FOUROBJECTSACCESSOR_H

