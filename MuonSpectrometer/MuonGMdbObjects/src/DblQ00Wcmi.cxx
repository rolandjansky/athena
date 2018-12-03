/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: DblQ00Wcmi.cxx,v 1.5 2007-02-12 17:33:50 stefspa Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "MuonGMdbObjects/DblQ00Wcmi.h"
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

DblQ00Wcmi::DblQ00Wcmi(IRDBQuery* wcmi)
 : m_nObj(0)
{
  if(wcmi) {
    wcmi->execute();
    m_nObj = wcmi->size();
    m_d = new WCMI[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Wcmi banks in the MuonDD Database"<<std::endl;

    int i=0;
    while(wcmi->next()) {
        m_d[i].version     = wcmi->data<int>("WCMI_DATA.VERS");    
        m_d[i].jsta        = wcmi->data<int>("WCMI_DATA.JSTA");
        m_d[i].num         = wcmi->data<int>("WCMI_DATA.NUM");
        m_d[i].heightness     = wcmi->data<float>("WCMI_DATA.HEIGHTNESS");
        m_d[i].largeness      = wcmi->data<float>("WCMI_DATA.LARGENESS");
        m_d[i].thickness      = wcmi->data<float>("WCMI_DATA.THICKNESS");
        //std::cerr<<" FROM ORACLE: CMI "<<m_d[i].jsta<<" h/l/t "<<m_d[i].heightness<<" "<<m_d[i].largeness <<" "<<m_d[i].thickness <<std::endl;
	i++;
    }
    wcmi->finalize();
  }
  else {
    m_d = new WCMI[0];
    std::cerr<<"NO Wcmi banks in the MuonDD Database"<<std::endl;
  }
}
    
DblQ00Wcmi::~DblQ00Wcmi()
{
    delete [] m_d;
}

} // end of namespace MuonGM
