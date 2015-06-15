/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 Copyright (C) 2004 by ATLAS Collaboration
 ***************************************************************************/

//<doc><file>	$Id: DblQ00Wrpc.cxx,v 1.4 2007-02-12 17:33:50 stefspa Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "MuonGMdbObjects/DblQ00Wrpc.h"
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

DblQ00Wrpc::DblQ00Wrpc(IRDBQuery* m_wrpc)
 : m_nObj(0)
{
  if(m_wrpc) {
    m_wrpc->execute();
    m_nObj = m_wrpc->size();
    m_d = new WRPC[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Wrpc banks in the MuonDD Database"<<std::endl;

    int i=0;
    while(m_wrpc->next()) {
        m_d[i].version     = m_wrpc->data<int>("WRPC_DATA.VERS");    
        m_d[i].nvrs        = m_wrpc->data<int>("WRPC_DATA.NVRS");
        m_d[i].layrpc      = m_wrpc->data<int>("WRPC_DATA.LAYRPC");
        m_d[i].tckrla      = m_wrpc->data<float>("WRPC_DATA.TCKRLA");
        m_d[i].tottck      = m_wrpc->data<float>("WRPC_DATA.TOTTCK");
        m_d[i].tckfsp      = m_wrpc->data<float>("WRPC_DATA.TCKFSP");
        m_d[i].ackfsp      = m_wrpc->data<float>("WRPC_DATA.ACKFSP");
        m_d[i].tlohcb      = m_wrpc->data<float>("WRPC_DATA.TLOHCB");
        m_d[i].alohcb      = m_wrpc->data<float>("WRPC_DATA.ALOHCB");
        m_d[i].tckbak      = m_wrpc->data<float>("WRPC_DATA.TCKBAK");
        m_d[i].tckgas      = m_wrpc->data<float>("WRPC_DATA.TCKGAS");
        m_d[i].tckssu      = m_wrpc->data<float>("WRPC_DATA.TCKSSU");
        m_d[i].tckstr      = m_wrpc->data<float>("WRPC_DATA.TCKSTR");
        m_d[i].sdedmi      = m_wrpc->data<float>("WRPC_DATA.SDEDMI");
        m_d[i].zdedmi      = m_wrpc->data<float>("WRPC_DATA.ZDEDMI");
        m_d[i].spdiam      = m_wrpc->data<float>("WRPC_DATA.SPDIAM");
        m_d[i].sppitc      = m_wrpc->data<float>("WRPC_DATA.SPPITC");
        m_d[i].stroff[0]   = m_wrpc->data<float>("WRPC_DATA.STROFF_0");
        m_d[i].stroff[1]   = m_wrpc->data<float>("WRPC_DATA.STROFF_1");
        m_d[i].stroff[2]   = m_wrpc->data<float>("WRPC_DATA.STROFF_2");
        i++;
    }
    m_wrpc->finalize();
  }
  else {
    m_d = new WRPC[0];
    std::cerr<<"NO Wrpc banks in the MuonDD Database"<<std::endl;
  }
}
    
DblQ00Wrpc::~DblQ00Wrpc()
{
    delete [] m_d;
}

} // end of namespace MuonGM
