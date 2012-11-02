/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTCONDITIONSDATA_LINEARPACKER_H
#define TRTCONDITIONSDATA_LINEARPACKER_H

#include <limits>

namespace TRTCond
{
  /** @class LinearPacker
       Template class for packing data
  */
  template <class UnpackedType, class PackedType>
  class LinearPacker
  {
  public:

    /** default constructor */
    LinearPacker() : m_xmin(1),m_xmax(0) {}

    /** normal constructor */ 
    LinearPacker(UnpackedType xmin, UnpackedType xmax) : m_xmin(xmin), m_xmax(xmax) {}

    /** Unpack method */
    UnpackedType unpack(PackedType val ) const { return m_xmin + (val-valmin())  * dx() ; }

    /** Pack method */
    PackedType pack(UnpackedType x )const {
      return x>=m_xmax ? valmax() : (x<=m_xmin ? valmin() : valmin() + PackedType((x - m_xmin) /dx() + 0.5) ) ;
    }
    
  private:

    /** returns numeric upper limit of packed type */
    PackedType valmax() const { return std::numeric_limits<PackedType>::max() ; }

    /** returns numeric lower limit of packed type */
    PackedType valmin() const { return std::numeric_limits<PackedType>::min() ; }

    /** returns precision of the packed data */
    UnpackedType dx() const { return (m_xmax - m_xmin)/(UnpackedType(valmax()-valmin())+1) ; }
  private:
    UnpackedType m_xmin ; //!< minimum data value
    UnpackedType m_xmax ; //!< maximum data value
  } ;
  
}

#endif
