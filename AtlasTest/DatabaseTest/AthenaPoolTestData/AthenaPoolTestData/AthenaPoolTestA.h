/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthenaPoolTestA.h
 * @brief Class definition for AthenaPoolTestA
 */ 
/**
 * @class AthenaPoolTestA
 * @brief Simple class with two int's for testing pool i/o from Athena
 * @author RD Schaffer
 * Created May 2004 for DC2 validation
 */
/***************************************************************************
 Athena Pool Test Data package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: AthenaPoolTestA.h,v 1.4 2006-08-30 07:49:42 schaffer Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef ATHENAPOOLTESTDATA_ATHENAPOOLTESTA_H
# define ATHENAPOOLTESTDATA_ATHENAPOOLTESTA_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaPoolTestData/IAthenaPoolTestData.h"
#include <string>
#include <vector> 

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>
//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class AthenaPoolTestA : public IAthenaPoolTestData 
{
public:
    AthenaPoolTestA();
    AthenaPoolTestA(int a, int b);
    
    virtual ~AthenaPoolTestA (void);
    virtual std::string printWhoYouAre   (void) const;

private:
    int  m_a;
    int  m_b;
    std::vector<int> m_c;
    std::vector<float> m_d;
};

//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>



#endif // ATHENAPOOLTESTDATA_ATHENAPOOLTESTA_H
