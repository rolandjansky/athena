/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: DblQ00Wcro.cxx,v 1.4 2007-02-12 17:33:50 stefspa Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "MuonGMdbObjects/DblQ00Wcro.h"
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

DblQ00Wcro::DblQ00Wcro(std::unique_ptr<IRDBQuery>&& wcro)
 : m_nObj(0)
{
  if(wcro) {
    wcro->execute();
    m_nObj = wcro->size();
    m_d = new WCRO[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Wcro banks in the MuonDD Database"<<std::endl;

    int i=0;
    while(wcro->next()) {
        m_d[i].version     = wcro->data<int>("WCRO_DATA.VERS");    
        m_d[i].jsta        = wcro->data<int>("WCRO_DATA.JSTA");
        m_d[i].num         = wcro->data<int>("WCRO_DATA.NUM");
        m_d[i].heightness     = wcro->data<float>("WCRO_DATA.HEIGHTNESS");
        m_d[i].largeness      = wcro->data<float>("WCRO_DATA.LARGENESS");
        m_d[i].thickness      = wcro->data<float>("WCRO_DATA.THICKNESS");
	i++;
    }
    wcro->finalize();
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
