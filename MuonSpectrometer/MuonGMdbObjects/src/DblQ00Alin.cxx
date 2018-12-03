/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: DblQ00Alin.cxx,v 1.5 2007-02-12 17:33:50 stefspa Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "MuonGMdbObjects/DblQ00Alin.h"
#include "RDBAccessSvc/IRDBQuery.h"
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

DblQ00Alin::DblQ00Alin(IRDBQuery* alin)
 : m_nObj(0)
{
  if(alin) {
    alin->execute();
    m_nObj = alin->size();
    m_d = new ALIN[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Alin banks in the MuonDD Database"<<std::endl;

    // field indexes
    unsigned fieldVers(1);
    unsigned fieldDx(2);
    unsigned fieldDy(3);
    unsigned fieldI(4);
    unsigned fieldWidth_xs(5);
    unsigned fieldWidth_xl(6);
    unsigned fieldLength_y(7);
    unsigned fieldExcent(8);
    unsigned fieldDead1(9);
    unsigned fieldJtyp(10);
    unsigned fieldIndx(11);
    unsigned fieldIcut(12);

    int i=0;
    while(alin->next()) {
        m_d[i].version      = alin->data<int>(fieldVers);    
        m_d[i].dx           = alin->data<float>(fieldDx);          
        m_d[i].dy           = alin->data<float>(fieldDy);       
        m_d[i].i            = alin->data<int>(fieldI);   
        m_d[i].width_xs     = alin->data<float>(fieldWidth_xs);
        m_d[i].width_xl     = alin->data<float>(fieldWidth_xl);  
        m_d[i].length_y     = alin->data<float>(fieldLength_y);  
        m_d[i].excent       = alin->data<float>(fieldExcent);    
        m_d[i].dead1        = alin->data<float>(fieldDead1);     
        m_d[i].jtyp         = alin->data<int>(fieldJtyp);      
        m_d[i].indx         = alin->data<int>(fieldIndx);     
        m_d[i].icut         = alin->data<int>(fieldIcut);      
        i++;
    }
    alin->finalize();
  }
  else {
    m_d = new ALIN[0];
    std::cerr<<"NO Alin banks in the MuonDD Database"<<std::endl;
  }
}
    
DblQ00Alin::~DblQ00Alin()
{
    delete [] m_d;
}

} // end of namespace MuonGM
