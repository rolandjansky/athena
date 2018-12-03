/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: DblQ00Wtgc.cxx,v 1.4 2007-02-12 17:33:50 stefspa Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "MuonGMdbObjects/DblQ00Wtgc.h"
#include "RDBAccessSvc/IRDBQuery.h"
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

DblQ00Wtgc::DblQ00Wtgc(IRDBQuery* wtgc)
 : m_nObj(0)
{
  if(wtgc) {
    wtgc->execute();
    m_nObj = wtgc->size();
    m_d = new WTGC[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Wtgc banks in the MuonDD Database"<<std::endl;

    int i=0;
    while(wtgc->next()) {
        m_d[i].version     = wtgc->data<int>("WTGC_DATA.VERS");    
        m_d[i].jsta        = wtgc->data<int>("WTGC_DATA.JSTA");
        m_d[i].nbevol      = wtgc->data<int>("WTGC_DATA.NBEVOL");
        m_d[i].x0          = wtgc->data<float>("WTGC_DATA.X0");
        m_d[i].widchb      = wtgc->data<float>("WTGC_DATA.WIDCHB");
        m_d[i].fwirch      = wtgc->data<float>("WTGC_DATA.FWIRCH");
        m_d[i].fwixch      = wtgc->data<float>("WTGC_DATA.FWIXCH");
        for (unsigned int j=0; j<9; j++)
        {
            std::ostringstream tem;
            tem << j;
            std::string tag = "WTGC_DATA.ALLNAME_"+tem.str();
            try {
	      sprintf(m_d[i].allname[j],"%s",wtgc->data<std::string>(tag).c_str());
            }
            catch (std::runtime_error)
            {
                //std::cerr<<"MuonGM::DblQ00-Wtgc- End of material-name list"<<std::endl;
                break;
            }
        }
        i++;
    }
    wtgc->finalize();
  }
  else {
    m_d = new WTGC[0];
    std::cerr<<"NO Wtgc banks in the MuonDD Database"<<std::endl;
  }
}
    
DblQ00Wtgc::~DblQ00Wtgc()
{
    delete [] m_d;
}

} // end of namespace MuonGM
