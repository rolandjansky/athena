/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file IAthenaPoolTestData.h
 * @brief Class definition for IAthenaPoolTestData
 */
/**
 * @class IAthenaPoolTestData
 * @brief Interface class for AthenaPoolTest i/o tests
 * @author RD Schaffer
 * Created May 2004 for DC2 validation
 */
/***************************************************************************
 Athena Pool Test Data package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: IAthenaPoolTestData.h,v 1.3 2005-01-07 23:24:57 cranshaw Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef ATHENAPOOLTESTDATA_IATHENAPOOLTESTDATA_H
# define ATHENAPOOLTESTDATA_IATHENAPOOLTESTDATA_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <string>

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>
//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class IAthenaPoolTestData 
{
public:
    virtual ~IAthenaPoolTestData (void);
    virtual std::string printWhoYouAre   (void) const = 0;
};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // ATHENAPOOLTESTDATA_IATHENAPOOLTESTDATA_H
