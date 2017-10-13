/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthenaPoolTestB.h
 * @brief Class definition for AthenaPoolTestB
 */
/**
 * @class AthenaPoolTestB
 * @brief Simple class with int,int,float,double for testing pool i/o from Athena
 * @author RD Schaffer
 * Created May 2004 for DC2 validation
 */
/***************************************************************************
 Athena Pool Test Data package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: AthenaPoolTestB.h,v 1.3 2005-01-07 20:33:29 cranshaw Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef ATHENAPOOLTESTDATA_ATHENAPOOLTESTB_H
# define ATHENAPOOLTESTDATA_ATHENAPOOLTESTB_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaPoolTestData/IAthenaPoolTestData.h"
#include <string>

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>
//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class AthenaPoolTestB : public IAthenaPoolTestData
{
public:
    AthenaPoolTestB();
    AthenaPoolTestB(int c, int d);
    
    virtual ~AthenaPoolTestB (void);
    virtual std::string printWhoYouAre   (void) const;

private:
    float  m_fa;
    int    m_c;
    double m_da;
    int    m_d;
};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // ATHENAPOOLTESTDATA_ATHENAPOOLTESTB_H
