/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 Copyright (C) 2004 by ATLAS Collaboration
 ***************************************************************************/

//<doc><file>	$Id: DblQ00Wcsc.cxx,v 1.6 2007-02-12 17:33:50 stefspa Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "MuonGMdbObjects/DblQ00Wcsc.h"
#include "RDBAccessSvc/IRDBQuery.h"
#include <iostream>
#include <sstream>
//#include <stdio>
#include <stdexcept>

//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

namespace MuonGM
{

DblQ00Wcsc::DblQ00Wcsc(IRDBQuery* m_wcsc)
{
  if(m_wcsc) {
    m_wcsc->execute();
    m_nObj = m_wcsc->size();
    m_d = new WCSC[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Wcsc banks in the MuonDD Database"<<std::endl;

    int i=0;
    while(m_wcsc->next()) {
        m_d[i].version     = m_wcsc->data<int>("WCSC_DATA.VERS");    
        m_d[i].jsta        = m_wcsc->data<int>("WCSC_DATA.JSTA");    
        m_d[i].laycsc      = m_wcsc->data<int>("WCSC_DATA.LAYCSC");
        m_d[i].ttotal      = m_wcsc->data<float>("WCSC_DATA.TTOTAL");
        m_d[i].tnomex      = m_wcsc->data<float>("WCSC_DATA.TNOMEX"); 
        m_d[i].tlag10      = m_wcsc->data<float>("WCSC_DATA.TLAG10"); 
        m_d[i].wispa       = m_wcsc->data<float>("WCSC_DATA.WISPA"); 
        m_d[i].dancat      = m_wcsc->data<float>("WCSC_DATA.DANCAT"); 
        m_d[i].pcatre      = m_wcsc->data<float>("WCSC_DATA.PCATRE"); 
        m_d[i].gstrip      = m_wcsc->data<float>("WCSC_DATA.GSTRIP"); 
        m_d[i].wrestr      = m_wcsc->data<float>("WCSC_DATA.WRESTR"); 
        m_d[i].wflstr      = m_wcsc->data<float>("WCSC_DATA.WFLSTR"); 
        m_d[i].trrwas      = m_wcsc->data<float>("WCSC_DATA.TRRWAS"); 
        m_d[i].wroxa       = m_wcsc->data<float>("WCSC_DATA.WROXA"); 
        m_d[i].groxwi      = m_wcsc->data<float>("WCSC_DATA.GROXWI"); 
        m_d[i].wgasba      = m_wcsc->data<float>("WCSC_DATA.WGASBA"); 
        m_d[i].tgasba      = m_wcsc->data<float>("WCSC_DATA.TGASBA"); 
        m_d[i].wgascu      = m_wcsc->data<float>("WCSC_DATA.WGASCU"); 
        m_d[i].tgascu      = m_wcsc->data<float>("WCSC_DATA.TGASCU"); 
        m_d[i].wfixwi      = m_wcsc->data<float>("WCSC_DATA.WFIXWI"); 
        m_d[i].tfixwi      = m_wcsc->data<float>("WCSC_DATA.TFIXWI"); 
        m_d[i].pba1wi      = m_wcsc->data<float>("WCSC_DATA.PBA1WI"); 
        m_d[i].pba2wi      = m_wcsc->data<float>("WCSC_DATA.PBA2WI"); 
        m_d[i].pba3wi      = m_wcsc->data<float>("WCSC_DATA.PBA3WI"); 
        m_d[i].psndco      = m_wcsc->data<float>("WCSC_DATA.PSNDCO");
        //std::cerr<<" original psndco = "<<m_d[i].psndco<<std::endl;
        m_d[i].azcat = 0.;
        float  azcat = 0.;
        try {
            azcat = m_wcsc->data<float>("WCSC_DATA.AZCAT");
            m_d[i].azcat =   azcat;
            //m_d[i].psndco =  m_d[i].azcat;
            //std::cerr<<" redefined psndco = "<<m_d[i].psndco<<std::endl;
        }
        catch (std::runtime_error)
        {
            std::cerr<<" azcat field does not exists !"<<std::endl;
            m_d[i].azcat =   0.;
            //m_d[i].psndco =  0.;
            //std::cerr<<" redefined psndco to 0 "<<m_d[i].psndco<<std::endl;
        }
       i++;
    }
    m_wcsc->finalize();
  }
  else {
    m_d = new WCSC[0];
    std::cerr<<"NO Wcsc banks in the MuonDD Database"<<std::endl;
  }
}
    
DblQ00Wcsc::~DblQ00Wcsc()
{
    delete [] m_d;
}

} // end of namespace MuonGM
