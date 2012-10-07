/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 Copyright (C) 2004 by ATLAS Collaboration
 ***************************************************************************/

//<doc><file>	$Id: DblQ00Awln.cxx,v 1.4 2007-02-12 17:33:50 stefspa Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "MuonGMdbObjects/DblQ00Awln.h"
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

DblQ00Awln::DblQ00Awln(IRDBQuery* m_awln)
{
  if(m_awln) {
    m_awln->execute();
    m_nObj = m_awln->size();
    m_d = new AWLN[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Awln banks in the MuonDD Database"<<std::endl;

    int i=0;
    while(m_awln->next()) {
        m_d[i].version       = m_awln->data<int>("AWLN_DATA.VERS");    
        m_d[i].jsta          = m_awln->data<int>("AWLN_DATA.JSTA");    
        m_d[i].spitch        = m_awln->data<float>("AWLN_DATA.SPITCH");  
        m_d[i].zpitch        = m_awln->data<float>("AWLN_DATA.ZPITCH");  
        m_d[i].dedstr        = m_awln->data<float>("AWLN_DATA.DEDSTR");  
        m_d[i].nsrest        = m_awln->data<int>("AWLN_DATA.NSREST");  
        m_d[i].nzrest        = m_awln->data<int>("AWLN_DATA.NZREST");
        m_d[i].sfirst        = m_awln->data<float>("AWLN_DATA.SFIRST");
        m_d[i].zfirst        = m_awln->data<float>("AWLN_DATA.ZFIRST");
        m_d[i].dedsep        = m_awln->data<float>("AWLN_DATA.DEDSEP");
        m_d[i].nsrost        = m_awln->data<int>("AWLN_DATA.NSROST");  
        m_d[i].nzrost        = m_awln->data<int>("AWLN_DATA.NZROST");
        //        std::cerr<<" wRPC"<<m_d[i].jsta<<" s,zpitch  "<<m_d[i].spitch <<" "<<m_d[i].zpitch
        //         <<" ns,zrest "<<m_d[i].nsrest<<" "<<m_d[i].nzrest<<std::endl;
        i++;
    }
    m_awln->finalize();
  }
  else {
    m_d = new AWLN[0];
    std::cerr<<"NO Awln banks in the MuonDD Database"<<std::endl;
  }
}
    
DblQ00Awln::~DblQ00Awln()
{
    delete [] m_d;
}

} // end of namespace MuonGM
