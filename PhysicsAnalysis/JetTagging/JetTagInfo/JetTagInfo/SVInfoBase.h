/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFO_SVINFOBASE_H
#define JETTAGINFO_SVINFOBASE_H
#include "JetTagInfo/BaseTagInfo.h"
#include <iostream>

class MsgStream;

namespace Analysis { 

  /**
  @class SVInfoBase
  Basic class to hold SV taggers results
  */

class SVInfoBase : public BaseTagInfo { 
public:
    /** default constructor */
    SVInfoBase();

    /** constructor with info type */
    SVInfoBase(TagInfoType x);
    
    /** copy destructor */
    SVInfoBase(const SVInfoBase& SVInfoBase);
    
    /** assigenment operator */
    SVInfoBase &operator= (const SVInfoBase& rhs);

    /** default destructor */
    virtual ~SVInfoBase();
    virtual SVInfoBase* clone() const;

}
; // End class

/**output. This dumps the values of each of the possible summary enums*/
MsgStream& operator<<(MsgStream& out, const SVInfoBase&);
 
/**output. This dumps the values of each of the possible summary enums*/
std::ostream& operator<<(std::ostream& out, const SVInfoBase&);

inline Analysis::SVInfoBase* Analysis::SVInfoBase::clone() const { 
    return new SVInfoBase(*this);
}

} // End namespace
#endif

