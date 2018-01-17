/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: DblQ00Aptp.cxx,v 1.4 2007-02-12 17:33:50 stefspa Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "MuonGMdbObjects/DblQ00Aptp.h"
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

DblQ00Aptp::DblQ00Aptp(std::unique_ptr<IRDBQuery>&& aptp)
 : m_nObj(0)
{
  if(aptp) {
    aptp->execute();
    m_nObj = aptp->size();
    m_d = new APTP[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Aptp banks in the MuonDD Database"<<std::endl;

     unsigned fieldVers(1);
     unsigned fieldLine(2);
     unsigned fieldTyp(3);
     unsigned fieldI(4);
     unsigned fieldIcut(5);
     unsigned fieldIphi(6);
     unsigned fieldIz(14);
     unsigned fieldDphi(15);
     unsigned fieldZ(16);
     unsigned fieldR(17);
     unsigned fieldS(18);
     unsigned fieldAlfa(19);
     unsigned fieldBeta(20);
     unsigned fieldGamma(21);

    int i=0;
    while(aptp->next()) {
        m_d[i].version     = aptp->data<int>(fieldVers);    
        m_d[i].line        = aptp->data<int>(fieldLine);
        sprintf(m_d[i].type,"%s",aptp->data<std::string>(fieldTyp).c_str()); 
        m_d[i].i           = aptp->data<int>(fieldI);
        m_d[i].icut        = aptp->data<int>(fieldIcut);
        for (unsigned int j=0; j<8; j++)
        {
 //            std::ostringstream tem;
//             tem << j;
//             std::string tag = "APTP_DATA.IPHI_"+tem.str();
            m_d[i].iphi[j]     = aptp->data<int>(fieldIphi+j);        
        }
        m_d[i].iz          = aptp->data<int>(fieldIz);
        m_d[i].dphi        = aptp->data<float>(fieldDphi);  
        m_d[i].z           = aptp->data<float>(fieldZ);  
        m_d[i].r           = aptp->data<float>(fieldR);    
        m_d[i].s           = aptp->data<float>(fieldS);     
        m_d[i].alfa        = aptp->data<float>(fieldAlfa);      
        m_d[i].beta        = aptp->data<float>(fieldBeta);     
        m_d[i].gamma       = aptp->data<float>(fieldGamma);
//         std::cerr<<i<<" type, iz, iphi, z, r, s "<<m_d[i].type<<" "<<m_d[i].iz <<" ";
//         for(unsigned int j=0; j<8; j++)std::cerr<<m_d[i].iphi[j];
//         std::cerr<<" "<<m_d[i].z<<" "<<m_d[i].r<<" "<<m_d[i].s    <<std::endl;
        i++;
    }
    aptp->finalize();
  }
  else {
    m_d = new APTP[0];
    std::cerr<<"NO Aptp banks in the MuonDD Database"<<std::endl;
  }
}
    
DblQ00Aptp::~DblQ00Aptp()
{
    delete [] m_d;
}

} // end of namespace MuonGM
