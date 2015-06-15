/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 Copyright (C) 2004 by ATLAS Collaboration
 ***************************************************************************/

//<doc><file>	$Id: DblQ00Wcro.cxx,v 1.4 2007-02-12 17:33:50 stefspa Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "MuonGMdbObjects/DblQ00Wcro.h"
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

DblQ00Wcro::DblQ00Wcro(IRDBQuery* m_wcro)
 : m_nObj(0)
{
  if(m_wcro) {
    m_wcro->execute();
    m_nObj = m_wcro->size();
    m_d = new WCRO[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Wcro banks in the MuonDD Database"<<std::endl;

    int i=0;
    while(m_wcro->next()) {
        m_d[i].version     = m_wcro->data<int>("WCRO_DATA.VERS");    
        m_d[i].jsta        = m_wcro->data<int>("WCRO_DATA.JSTA");
        m_d[i].num         = m_wcro->data<int>("WCRO_DATA.NUM");
        m_d[i].heightness     = m_wcro->data<float>("WCRO_DATA.HEIGHTNESS");
        m_d[i].largeness      = m_wcro->data<float>("WCRO_DATA.LARGENESS");
        m_d[i].thickness      = m_wcro->data<float>("WCRO_DATA.THICKNESS");
	i++;
    }
    m_wcro->finalize();
  }
  else {
    m_d = new WCRO[0];
    std::cerr<<"NO Wcro banks in the MuonDD Database"<<std::endl;
  }
}
    
DblQ00Wcro::~DblQ00Wcro()
{
    delete [] m_d;
}

} // end of namespace MuonGM
