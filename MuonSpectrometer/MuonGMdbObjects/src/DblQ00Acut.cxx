/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: DblQ00Acut.cxx,v 1.4 2007-02-12 17:33:50 stefspa Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "MuonGMdbObjects/DblQ00Acut.h"
#include "RDBAccessSvc/IRDBQuery.h"
#include <iostream>
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

DblQ00Acut::DblQ00Acut(IRDBQuery* acut)
 : m_nObj(0)
{
  if(acut) {
    acut->execute();
    m_nObj = acut->size();
    m_d = new ACUT[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Acut banks in the MuonDD Database"<<std::endl;

    int i=0;
    while(acut->next()) {
	m_d[i].version = acut->data<int>("ACUT_DATA.VERS");
	m_d[i].i       = acut->data<int>("ACUT_DATA.I");
	m_d[i].icut    = acut->data<int>("ACUT_DATA.ICUT");
	m_d[i].n       = acut->data<int>("ACUT_DATA.N");
	i++;
    }
    acut->finalize();
  }
  else {
    m_d = new ACUT[0];
    std::cerr<<"NO Acut banks in the MuonDD Database"<<std::endl;
  }
}
    
DblQ00Acut::~DblQ00Acut()
{
    delete [] m_d;
    m_d = 0;
}

} // end of namespace MuonGM
