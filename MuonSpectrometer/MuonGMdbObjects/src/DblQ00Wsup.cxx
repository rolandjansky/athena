/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: DblQ00Wsup.cxx,v 1.4 2007-02-12 17:33:50 stefspa Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "MuonGMdbObjects/DblQ00Wsup.h"
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

DblQ00Wsup::DblQ00Wsup(std::unique_ptr<IRDBQuery>&& wsup)
 : m_nObj(0)
{
  if(wsup) {
    wsup->execute();
    m_nObj = wsup->size();
    m_d = new WSUP[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Wsup banks in the MuonDD Database"<<std::endl;

    int i=0;
    while(wsup->next()) {
        m_d[i].version     = wsup->data<int>("WSUP_DATA.VERS");    
        m_d[i].jsta        = wsup->data<int>("WSUP_DATA.JSTA");
        m_d[i].nxxsup      = wsup->data<int>("WSUP_DATA.NXXSUP");
        m_d[i].nzzsup      = wsup->data<int>("WSUP_DATA.NZZSUP");
        m_d[i].x0          = wsup->data<float>("WSUP_DATA.X0");
        m_d[i].thickn      = wsup->data<float>("WSUP_DATA.THICKN");
        for (unsigned int j=0; j<4; j++)
        {
            std::ostringstream tem;
            tem << j;
            std::string tagx = "WSUP_DATA.XXSUP_"+tem.str();
            std::string tagy = "WSUP_DATA.ZZSUP_"+tem.str();
            m_d[i].xxsup[j]     = wsup->data<float>(tagx);        
            m_d[i].zzsup[j]     = wsup->data<float>(tagy);        
        }
        i++;
   }
    wsup->finalize();
  }
  else {
    m_d = new WSUP[0];
    std::cerr<<"NO Wsup banks in the MuonDD Database"<<std::endl;
  }
}
    
DblQ00Wsup::~DblQ00Wsup()
{
    delete [] m_d;
}

} // end of namespace MuonGM
