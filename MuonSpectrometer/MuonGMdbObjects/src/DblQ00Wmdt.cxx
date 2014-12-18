/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 Copyright (C) 2004 by ATLAS Collaboration
 ***************************************************************************/

//<doc><file>	$Id: DblQ00Wmdt.cxx,v 1.4 2007-02-12 17:33:50 stefspa Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "MuonGMdbObjects/DblQ00Wmdt.h"
#include "RDBAccessSvc/IRDBQuery.h"
#include <iostream>
#include <sstream>
#include <stdio.h>

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

DblQ00Wmdt::DblQ00Wmdt(IRDBQuery* m_wmdt)
 : m_nObj(0)
{
  if(m_wmdt) {
    m_wmdt->execute();
    m_nObj = m_wmdt->size();
    m_d = new WMDT[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Wmdt banks in the MuonDD Database"<<std::endl;

    int i=0;
    while(m_wmdt->next()) {
        m_d[i].version     = m_wmdt->data<int>("WMDT_DATA.VERS");    
        m_d[i].iw          = m_wmdt->data<int>("WMDT_DATA.IW");
        m_d[i].laymdt      = m_wmdt->data<int>("WMDT_DATA.LAYMDT");
        sprintf(m_d[i].typ,"%s",m_wmdt->data<std::string>("WMDT_DATA.TYP").c_str());
        m_d[i].x0          = m_wmdt->data<float>("WMDT_DATA.X0");
        m_d[i].tubpit          = m_wmdt->data<float>("WMDT_DATA.TUBPIT");
        m_d[i].tubrad          = m_wmdt->data<float>("WMDT_DATA.TUBRAD");
        m_d[i].tubsta          = m_wmdt->data<float>("WMDT_DATA.TUBSTA");
        m_d[i].tubdea          = m_wmdt->data<float>("WMDT_DATA.TUBDEA");
        m_d[i].tubwal          = m_wmdt->data<float>("WMDT_DATA.TUBWAL");
        for (unsigned int j=0; j<4; j++)
        {
            std::ostringstream tem;
            tem << j;
            std::string tagx = "WMDT_DATA.TUBXCO_"+tem.str();
            std::string tagy = "WMDT_DATA.TUBYCO_"+tem.str();
            m_d[i].tubxco[j]     = m_wmdt->data<float>(tagx);        
            m_d[i].tubyco[j]     = m_wmdt->data<float>(tagy);        
        }
//         std::cerr<<i<<" WMDT"<<m_d[i].iw<<" nlay, ptch, wal "<<m_d[i].laymdt <<" "<<m_d[i].tubpit<<" "<<m_d[i].tubwal
//                  <<"\n tubx "; for (int j=0; j<m_d[i].laymdt; j++) std::cerr<<m_d[i].tubxco[j]<<" ";
//         std::cerr<<"\n tuby "; for (int j=0; j<m_d[i].laymdt; j++) std::cerr<<m_d[i].tubyco[j]<<" ";
//         std::cerr<<std::endl;
        i++;
    }
    m_wmdt->finalize();
  }
  else {
    m_d = new WMDT[0];
    std::cerr<<"NO Wmdt banks in the MuonDD Database"<<std::endl;
  }
}
    
DblQ00Wmdt::~DblQ00Wmdt()
{
    delete [] m_d;
}

} // end of namespace MuonGM
