/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: DblQ00Wlbi.cxx,v 1.4 2007-02-12 17:33:50 stefspa Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "MuonGMdbObjects/DblQ00Wlbi.h"
#include "RelationalAccess/ICursor.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/Attribute.h"
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

DblQ00Wlbi::DblQ00Wlbi(std::unique_ptr<IRDBQuery>&& wlbi)
 : m_nObj(0)
{
  if(wlbi) {
    wlbi->execute();
    m_nObj = wlbi->size();
    m_d = new WLBI[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Wlbi banks in the MuonDD Database"<<std::endl;

    int i=0;
    while(wlbi->next()) {
        m_d[i].version     = wlbi->data<int>("WLBI_DATA.VERS");    
        m_d[i].jsta        = wlbi->data<int>("WLBI_DATA.JSTA");
        m_d[i].num         = wlbi->data<int>("WLBI_DATA.NUM");
        m_d[i].height      = wlbi->data<float>("WLBI_DATA.HEIGHT");
        m_d[i].thickness   = wlbi->data<float>("WLBI_DATA.THICKNESS");
	if (!wlbi->isNull("WLBI_DATA.LOWERTHICK"))
	  {
	    m_d[i].lowerThickness   = wlbi->data<float>("WLBI_DATA.LOWERTHICK");
	  }
	else m_d[i].lowerThickness   = m_d[i].thickness ;
	if (!wlbi->isNull("WLBI_DATA.SHIFTYSTATION"))
	  {
	    m_d[i].yShift   = wlbi->data<float>("WLBI_DATA.SHIFTYSTATION");
	  }
	else m_d[i].yShift   = 0.;
        i++;
    }
    wlbi->finalize();
  }
  else {
    m_d = new WLBI[0];
    std::cerr<<"NO Wlbi banks in the MuonDD Database"<<std::endl;
  }
}
    
DblQ00Wlbi::~DblQ00Wlbi()
{
    delete [] m_d;
}

} // end of namespace MuonGM
