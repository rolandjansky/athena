/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthenaPoolTestD.h
 * @brief Class definition for AthenaPoolTestD
 */
/**
 * @class AthenaPoolTestD
 * @brief Simple class with HepGeom::Point3D<double> for testing pool i/o from Athena
 * @author RD Schaffer
 * Created May 2004 for DC2 validation
 */
/***************************************************************************
 Athena Pool Test Data package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: AthenaPoolTestD.h,v 1.5 2005-09-02 15:53:01 gemmeren Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef ATHENAPOOLTESTDATA_ATHENAPOOLTESTD_H
# define ATHENAPOOLTESTDATA_ATHENAPOOLTESTD_H


#include "AthenaPoolTestData/AthenaPoolTestC.h"
//#include "CLHEP/Geometry/Point3D.h"
#include <string>


class AthenaPoolTestD : public AthenaPoolTestC 
{
public:
    AthenaPoolTestD();
    AthenaPoolTestD(int a, int b, int c, int d, int e, int f);
    
    virtual ~AthenaPoolTestD (void);
    virtual std::string printWhoYouAre   (void) const;

private:
    int                   m_e;
    int                   m_f;
};

//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>



#endif // ATHENAPOOLTESTDATA_ATHENAPOOLTESTD_H
