/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: DblQ00Wded.cxx,v 1.4 2007-02-12 17:33:50 stefspa Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "MuonGMdbObjects/DblQ00Wded.h"
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

DblQ00Wded::DblQ00Wded(IRDBQuery* wded)
 : m_nObj(0)
{
  if(wded) {
    wded->execute();
    m_nObj = wded->size();
    m_d = new WDED[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Wded banks in the MuonDD Database"<<std::endl;

    int i=0;
    while(wded->next()) {
        m_d[i].version     = wded->data<int>("WDED_DATA.VERS");    
        m_d[i].jsta        = wded->data<int>("WDED_DATA.JSTA");
        m_d[i].nb        = wded->data<int>("WDED_DATA.NB");
        m_d[i].x0          = wded->data<float>("WDED_DATA.X0");
        m_d[i].auphcb      = wded->data<float>("WDED_DATA.AUPHCB");
        m_d[i].tckded      = wded->data<float>("WDED_DATA.TCKDED");
        i++;
    }
    wded->finalize();
  }
  else {
    m_d = new WDED[0];
    std::cerr<<"NO Wded banks in the MuonDD Database"<<std::endl;
  }
}
    
DblQ00Wded::~DblQ00Wded()
{
    delete [] m_d;
}

} // end of namespace MuonGM
