/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthenaPoolTestC.h
 * @brief Class definition for AthenaPoolTestC
 */
/**
 * @class AthenaPoolTestC
 * @brief Simple class with four int's divided among protected/private 
 * for testing pool i/o from Athena
 * @author RD Schaffer
 * Created May 2004 for DC2 validation
 */
/***************************************************************************
 Athena Pool Test Data package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: AthenaPoolTestC.h,v 1.2 2005-01-07 20:33:29 cranshaw Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef ATHENAPOOLTESTDATA_ATHENAPOOLTESTC_H
# define ATHENAPOOLTESTDATA_ATHENAPOOLTESTC_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaPoolTestData/IAthenaPoolTestData.h"
#include <string>

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>
//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class AthenaPoolTestC : public IAthenaPoolTestData 
{
public:
    AthenaPoolTestC();
    AthenaPoolTestC(int a, int b, int c, int d);
    
    virtual ~AthenaPoolTestC (void);
    virtual std::string printWhoYouAre   (void) const;

protected:
    int  m_a;
    int  m_b;

private:
    int  m_c;
    int  m_d;

};

//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>



#endif // ATHENAPOOLTESTDATA_ATHENAPOOLTESTC_H
