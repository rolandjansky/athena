/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file dummy_A.h 
 * 
 * @brief
 * 
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 * 
 * $Id: dummy_A.h,v 1.2 2005-03-09 15:55:09 schaffer Exp $
 */

#ifndef ATHENAPOOLTESTDATA_DUMMY_A_H
# define ATHENAPOOLTESTDATA_DUMMY_A_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaKernel/CLASS_DEF.h"
#include <vector>

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/**
 * @class dummy_A 
 * 
 * @brief test nested vectors - problem seen in root3/4 transition
 * 
 */

class dummy_C {
public:
    dummy_C() : m_dummy(10) {};
    virtual ~dummy_C() {};
    unsigned int value() const { return(m_dummy); }
private:
    unsigned int m_dummy;
};


class dummy_B {
public:
    dummy_B() : m_dummy(4) {};
    virtual ~dummy_B() {};
    const std::vector<dummy_C>& dummy() const {return m_dummy;}
private:
    std::vector<dummy_C> m_dummy;
};

class dummy_A {
public:
    dummy_A() : m_dummy(5) {};
    virtual ~dummy_A() {};
    const std::vector<dummy_B>& dummy() const {return m_dummy;}
    
private:
    std::vector<dummy_B> m_dummy;
};
CLASS_DEF(dummy_A, 153882670, 1)


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // ATHENAPOOLTESTDATA_DUMMY_A_H
