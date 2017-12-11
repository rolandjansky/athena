/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file dummy_E.h 
 * 
 * @brief
 * 
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 * 
 * $Id: dummy_E.h,v 1.1 2005-03-09 15:55:09 schaffer Exp $
 */

#ifndef ATHENAPOOLTESTDATA_DUMMY_E_H
# define ATHENAPOOLTESTDATA_DUMMY_E_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaKernel/CLASS_DEF.h"
#include <vector>

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/**
 * @class dummy_E 
 * 
 * @brief Test derived class that contains a list of parent classes
 * 
 */

class dummy_D {
public:
    dummy_D() : m_dummy(10) {};
    virtual ~dummy_D() {};
    unsigned int value() const { return(m_dummy); }
private:
    unsigned int m_dummy;
};


class dummy_E : public dummy_D {
public:
    dummy_E() : dummy_D() {}
    void setData() {m_dummy.push_back(new dummy_D()); m_dummy.push_back(new dummy_D());}
    virtual ~dummy_E() {};
    const std::vector<const dummy_D*>& dummy() const {return m_dummy;}
private:
    std::vector<const dummy_D*> m_dummy;
};

CLASS_DEF(dummy_E, 203377114, 1)



//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // ATHENAPOOLTESTDATA_DUMMY_E_H
