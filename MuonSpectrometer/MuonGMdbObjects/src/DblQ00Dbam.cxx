/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: DblQ00Dbam.cxx,v 1.5 2007-02-12 17:33:50 stefspa Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "MuonGMdbObjects/DblQ00Dbam.h"
#include <iostream>
#include <sstream>
#include <stdio.h>
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

DblQ00Dbam::DblQ00Dbam(std::unique_ptr<IRDBQuery>&& dbam)
 : m_nObj(0)
{
  if(dbam) {
    dbam->execute();
    m_nObj = dbam->size();
    m_d = new DBAM[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Dbam banks in the MuonDD Database"<<std::endl;

    int i=0;
    while(dbam->next()) {
        m_d[i].version     = dbam->data<int>("DBAM_DATA.VERS");    
        m_d[i].nvrs        = dbam->data<int>("DBAM_DATA.NVRS");
        m_d[i].mtyp        = dbam->data<int>("DBAM_DATA.MTYP");
        m_d[i].numbox      = dbam->data<int>("DBAM_DATA.NUMBOX");
        sprintf(m_d[i].amdb,"%s",dbam->data<std::string>("DBAM_DATA.AMDB").c_str()); 
        try {
            sprintf(m_d[i].test,"%s",dbam->data<std::string>("DBAM_DATA.TEST").c_str());
        }
        catch (const std::runtime_error&)
        {
            //std::cerr<<"no TEST field available in DBAM"<<std::endl;
            sprintf(m_d[i].test,"unknown");
        }
        for (unsigned int j=0; j<53; j++)
        {
            std::ostringstream tem;
            tem << j;
            std::string tag = "DBAM_DATA.NAME_"+tem.str();
            try {
                sprintf(m_d[i].name[j],"%s",dbam->data<std::string>(tag).c_str());
            }
            catch (const std::runtime_error&)
            {
                //std::cerr<<"End of station-name list"<<std::endl;
                break;
            }
            //std::cerr<<j<<" Dbam amdb = "<<m_d[i].amdb<<" "<<j<<" "<<m_d[i].name[j]<<std::endl;
        }
//         std::cerr<<" Dbam amdb = "<<m_d[i].amdb<<" test "<<m_d[i].test
//                  <<" version/nvrs "<<m_d[i].version<<"/"<<m_d[i].nvrs<<std::endl;
        i++;
    }
    dbam->finalize();
  }
  else {
    m_d = new DBAM[0];
    std::cerr<<"NO Dbam banks in the MuonDD Database"<<std::endl;
  }
}
    
DblQ00Dbam::~DblQ00Dbam()
{
    delete [] m_d;
}

} // end of namespace MuonGM
