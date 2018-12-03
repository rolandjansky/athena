/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: DblQ00Wspa.cxx,v 1.4 2007-02-12 17:33:50 stefspa Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "MuonGMdbObjects/DblQ00Wspa.h"
#include "RDBAccessSvc/IRDBQuery.h"
#include <iostream>
#include <sstream>
//#include <stdio>

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

DblQ00Wspa::DblQ00Wspa(IRDBQuery* wspa)
 : m_nObj(0)
{
  if(wspa) {
    wspa->execute();
    m_nObj = wspa->size();
    m_d = new WSPA[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Wspa banks in the MuonDD Database"<<std::endl;

    int i=0;
    while(wspa->next()) {
        m_d[i].version     = wspa->data<int>("WSPA_DATA.VERS");    
        m_d[i].jsta        = wspa->data<int>("WSPA_DATA.JSTA");
        m_d[i].nb        = wspa->data<int>("WSPA_DATA.NB");
        m_d[i].x0          = wspa->data<float>("WSPA_DATA.X0");
        m_d[i].tckspa      = wspa->data<float>("WSPA_DATA.TCKSPA");
        i++;
    }
    wspa->finalize();
  }
  else {
    m_d = new WSPA[0];
    std::cerr<<"NO Wspa banks in the MuonDD Database"<<std::endl;
  }
}
    
DblQ00Wspa::~DblQ00Wspa()
{
    delete [] m_d;
}

} // end of namespace MuonGM
