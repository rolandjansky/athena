/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file PixelRDOElemLinkVec.h
 * @brief Class definition for PixelRDOElemLinkVec
 */
/**
 * @class PixelRDOElemLinkVec
 * @brief vector containing element links to pixel RDOs
 * @author RD Schaffer
 */
/***************************************************************************
 Athena Pool Test Data package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: PixelRDOElemLinkVec.h,v 1.1 2007-03-03 18:25:06 schaffer Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef ATHENAPOOLTESTDATA_PIXELRDOELEMLINKVEC_H
# define ATHENAPOOLTESTDATA_PIXELRDOELEMLINKVEC_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "InDetRawData/PixelRDO_Container.h"
#include "AthLinks/ElementLink.h"
#include "AthenaKernel/CLASS_DEF.h"
#include <vector>

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>
//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class PixelRDOElemLinkVec
{
public:

    typedef PixelRDO_Container                               cont_type;
    typedef ElementLink<cont_type>                           elem_type;
    typedef std::vector<elem_type>                           vec_type;
    

    PixelRDOElemLinkVec();
    const std::vector<elem_type>& pixelRDOs() const;
    std::vector<elem_type>& pixelRDOs();
    
private:

    std::vector<elem_type> m_pixelRDOs;
};


CLASS_DEF(PixelRDOElemLinkVec, 57613525, 0)



//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>


inline 
PixelRDOElemLinkVec::PixelRDOElemLinkVec() 
{}

inline 
std::vector<PixelRDOElemLinkVec::elem_type>& 
PixelRDOElemLinkVec::pixelRDOs()
{
    return m_pixelRDOs;
}

inline 
const std::vector<PixelRDOElemLinkVec::elem_type>& 
PixelRDOElemLinkVec::pixelRDOs() const 
{
    return m_pixelRDOs;
}

    

#endif // ATHENAPOOLTESTDATA_PIXELRDOELEMLINKVEC_H
