/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: DblQ00Atln.cxx,v 1.4 2007-02-12 17:33:50 stefspa Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "MuonGMdbObjects/DblQ00Atln.h"
#include <iostream>
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

DblQ00Atln::DblQ00Atln(std::unique_ptr<IRDBQuery>&& atln)
 : m_nObj(0)
{
  if(atln) {
    atln->execute();
    m_nObj = atln->size();
    m_d = new ATLN[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Atln banks in the MuonDD Database"<<std::endl;

     unsigned fieldVers(1);
     unsigned fieldI(2);
     unsigned fieldIcovol(3);
     unsigned fieldZpovol(4);
     unsigned fieldWidvol(5);
     unsigned fieldNamvol(6);
     unsigned fieldJsta(7);

    int i=0;
    while(atln->next()) {
        m_d[i].version     = atln->data<int>(fieldVers);    
        m_d[i].i           = atln->data<int>(fieldI);          
        m_d[i].icovol      = atln->data<int>(fieldIcovol);
        m_d[i].zpovol      = atln->data<float>(fieldZpovol);
        m_d[i].widvol      = atln->data<float>(fieldWidvol);
        sprintf(m_d[i].namvol,"%s",atln->data<std::string>(fieldNamvol).c_str());
        m_d[i].jsta        = atln->data<int>(fieldJsta);          
        i++;
    }
    atln->finalize();
  }
  else {
    m_d = new ATLN[0];
    std::cerr<<"NO Atln banks in the MuonDD Database"<<std::endl;
  }
}
    
DblQ00Atln::~DblQ00Atln()
{
    delete [] m_d;
}

} // end of namespace MuonGM
