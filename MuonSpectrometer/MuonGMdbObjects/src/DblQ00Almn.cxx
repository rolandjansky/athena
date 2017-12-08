/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: DblQ00Almn.cxx,v 1.5 2007-02-12 17:33:50 stefspa Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "MuonGMdbObjects/DblQ00Almn.h"
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

DblQ00Almn::DblQ00Almn(IRDBQuery* almn)
 : m_nObj(0)
{
  if(almn) {
    almn->execute();
    m_nObj = almn->size();
    m_d = new ALMN[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Almn banks in the MuonDD Database"<<std::endl;

     unsigned fieldVers(1);
     unsigned fieldI(2);
     unsigned fieldDx(3);
     unsigned fieldDy(4);
     unsigned fieldDz(5);
     unsigned fieldJob(6);
     unsigned fieldTec(7);
     unsigned fieldIw(8);
     unsigned fieldIsplit_x(9);
     unsigned fieldIsplit_y(10);
     unsigned fieldIshape(11);
     unsigned fieldWidth_xs(12);
     unsigned fieldWidth_xl(13);
     unsigned fieldLength_y(14);
     unsigned fieldExcent(15);
     unsigned fieldDead1(16);
     unsigned fieldDead2(17);
     unsigned fieldDead3(18);
     unsigned fieldJtyp(19);
     unsigned fieldIndx(20);

    int i=0;
    while(almn->next()) {
        m_d[i].version =           almn->data<int>(fieldVers);
        m_d[i].i =                 almn->data<int>(fieldI);
        m_d[i].dx =                almn->data<float>(fieldDx);
        m_d[i].dy =                almn->data<float>(fieldDy);
        m_d[i].dz =                almn->data<float>(fieldDz);
        m_d[i].job =               almn->data<int>(fieldJob);
        sprintf(m_d[i].tec,"%s",almn->data<std::string>(fieldTec).c_str());
        m_d[i].iw =                almn->data<int>(fieldIw);
        m_d[i].isplit_x =          almn->data<int>(fieldIsplit_x);
        m_d[i].isplit_y =          almn->data<int>(fieldIsplit_y);
        m_d[i].ishape =            almn->data<int>(fieldIshape);
        m_d[i].width_xs =          almn->data<float>(fieldWidth_xs);
        m_d[i].width_xl =          almn->data<float>(fieldWidth_xl);
        m_d[i].length_y =          almn->data<float>(fieldLength_y);
        m_d[i].excent =            almn->data<float>(fieldExcent);
        m_d[i].dead1 =             almn->data<float>(fieldDead1);
        m_d[i].dead2 =             almn->data<float>(fieldDead2);
        m_d[i].dead3 =             almn->data<float>(fieldDead3);
        m_d[i].jtyp =              almn->data<int>(fieldJtyp);
        m_d[i].indx =              almn->data<int>(fieldIndx);
        //std::cerr<<" here we are i = "<<i<<" dx, dy, dz "<<m_d[i].dx<<" "<<m_d[i].dy<<" "<<m_d[i].dz<<" tec "<<m_d[i].tec<<std::endl;
        i++;
    }
    almn->finalize();
  }
  else {
    m_d = new ALMN[0];
    std::cerr<<"NO Almn banks in the MuonDD Database"<<std::endl;
  }
}
    
DblQ00Almn::~DblQ00Almn()
{
    delete [] m_d;
}

} // end of namespace MuonGM
