/*                                                                                                                                 Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration                                                        
*/

#include "TrkGeometry/MagneticFieldProperties.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkExInterfaces/IPropagator.h"

#ifndef TRT_TRACKSEGMENTSTOOLCONDDATA_XK_H
#define TRT_TRACKSEGMENTSTOOLCONDDATA_XK_H

namespace InDet{

 struct TRT_TrackSegmentsToolCondData_xk{
    unsigned int                           m_nlayers[4]      ;
    unsigned int                           m_nstraws[4]      ;
    unsigned int                           m_flayers[4][30]  ;
    unsigned int                           m_begin  [4][200] ;
    unsigned int                           m_end    [4][200] ;
    float                                  m_dzdr[26]        ;
    unsigned int*                          m_ndzdr = 0       ;
    int*                                   m_islope= 0       ;
    float*                                 m_slope = 0       ;
    int                                    m_cirsize         ; // Size of m_circles

    ~TRT_TrackSegmentsToolCondData_xk(){  
        if(m_ndzdr  ) delete [] m_ndzdr  ;
        if(m_slope  ) delete [] m_slope  ;
        if(m_islope ) delete [] m_islope ;
    }
 };


}

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( InDet::TRT_TrackSegmentsToolCondData_xk , 214633214 , 1 )
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( InDet::TRT_TrackSegmentsToolCondData_xk , 97971002 );

#endif
