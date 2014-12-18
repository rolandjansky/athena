/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 Copyright (C) 2004 by ATLAS Collaboration
 ***************************************************************************/


#include "MuonGMdbObjects/DblQ00Xtomo.h"
#include "RDBAccessSvc/IRDBQuery.h"
#include <iostream>

namespace MuonGM
{
DblQ00Xtomo::DblQ00Xtomo() : m_nObj(0)
{
   m_d = NULL;
}
    
DblQ00Xtomo::DblQ00Xtomo(IRDBQuery* m_xtomo)
 : m_nObj(0)
{
  std::cout << "In DblQ00Xtomo(data)" << std::endl;
  if(m_xtomo) {
    m_xtomo->execute();
        
    m_nObj = m_xtomo->size();
    m_d = new XTOMO[m_nObj];
    if (m_nObj == 0) std::cerr << "No XtomoData entries in the MuonDD Database" << std::endl;

    int i=0;

    while(m_xtomo->next()) {
      m_d[i].line = i;
      m_d[i].XTOMOCHBERNAME = m_xtomo->data<std::string>("XTOMODATA_DATA.XTOMOCHBERNAME"); 
      m_d[i].XTOMOSITE = m_xtomo->data<std::string>("XTOMODATA_DATA.XTOMOSITE"); 
      m_d[i].XTOMOSITEID = m_xtomo->data<int>("XTOMODATA_DATA.XTOMOSITEID");
      m_d[i].XTOMOTIME = m_xtomo->data<int>("XTOMODATA_DATA.XTOMOTIME"); 
      m_d[i].XTOMOPASSED = m_xtomo->data<int>("XTOMODATA_DATA.XTOMOPASSED");
      m_d[i].XTOMOSIDE = m_xtomo->data<std::string>("XTOMODATA_DATA.XTOMOSIDE");
      m_d[i].XTOMONBERTUBE1 = m_xtomo->data<int>("XTOMODATA_DATA.XTOMONBERTUBE1");
      m_d[i].XTOMONBERTUBE2 = m_xtomo->data<int>("XTOMODATA_DATA.XTOMONBERTUBE2");
      m_d[i].XTOMONBERML = m_xtomo->data<int>("XTOMODATA_DATA.XTOMONBERML");
      m_d[i].XTOMONBERLAYER = m_xtomo->data<int>("XTOMODATA_DATA.XTOMONBERLAYER");
      m_d[i].XTOMOML1NYTUB = m_xtomo->data<float>("XTOMODATA_DATA.XTOMOML1NYTUB");
      m_d[i].XTOMOML1NZTUB = m_xtomo->data<float>("XTOMODATA_DATA.XTOMOML1NZTUB"); 
      m_d[i].XTOMOML1NDELY = m_xtomo->data<float>("XTOMODATA_DATA.XTOMOML1NDELY"); 
      m_d[i].XTOMOML1NDELZ = m_xtomo->data<float>("XTOMODATA_DATA.XTOMOML1NDELZ"); 
      m_d[i].XTOMOML1NDELA = m_xtomo->data<float>("XTOMODATA_DATA.XTOMOML1NDELA"); 
      m_d[i].XTOMOML1NYPIT = m_xtomo->data<float>("XTOMODATA_DATA.XTOMOML1NYPIT"); 
      m_d[i].XTOMOML1NZPIT = m_xtomo->data<float>("XTOMODATA_DATA.XTOMOML1NZPIT"); 
      m_d[i].XTOMOML1PYTUB = m_xtomo->data<float>("XTOMODATA_DATA.XTOMOML1PYTUB"); 
      m_d[i].XTOMOML1PZTUB = m_xtomo->data<float>("XTOMODATA_DATA.XTOMOML1PZTUB"); 
      m_d[i].XTOMOML1PDELY = m_xtomo->data<float>("XTOMODATA_DATA.XTOMOML1PDELY"); 
      m_d[i].XTOMOML1PDELZ = m_xtomo->data<float>("XTOMODATA_DATA.XTOMOML1PDELZ"); 
      m_d[i].XTOMOML1PDELA = m_xtomo->data<float>("XTOMODATA_DATA.XTOMOML1PDELA"); 
      m_d[i].XTOMOML1PYPIT = m_xtomo->data<float>("XTOMODATA_DATA.XTOMOML1PYPIT"); 
      m_d[i].XTOMOML1PZPIT = m_xtomo->data<float>("XTOMODATA_DATA.XTOMOML1PZPIT"); 
      m_d[i].XTOMOML2NYTUB = m_xtomo->data<float>("XTOMODATA_DATA.XTOMOML2NYTUB"); 
      m_d[i].XTOMOML2NZTUB = m_xtomo->data<float>("XTOMODATA_DATA.XTOMOML2NZTUB"); 
      m_d[i].XTOMOML2NDELY = m_xtomo->data<float>("XTOMODATA_DATA.XTOMOML2NDELY"); 
      m_d[i].XTOMOML2NDELZ = m_xtomo->data<float>("XTOMODATA_DATA.XTOMOML2NDELZ"); 
      m_d[i].XTOMOML2NDELA = m_xtomo->data<float>("XTOMODATA_DATA.XTOMOML2NDELA"); 
      m_d[i].XTOMOML2NYPIT = m_xtomo->data<float>("XTOMODATA_DATA.XTOMOML2NYPIT"); 
      m_d[i].XTOMOML2NZPIT = m_xtomo->data<float>("XTOMODATA_DATA.XTOMOML2NZPIT"); 
      m_d[i].XTOMOML2PYTUB = m_xtomo->data<float>("XTOMODATA_DATA.XTOMOML2PYTUB"); 
      m_d[i].XTOMOML2PZTUB = m_xtomo->data<float>("XTOMODATA_DATA.XTOMOML2PZTUB"); 
      m_d[i].XTOMOML2PDELY = m_xtomo->data<float>("XTOMODATA_DATA.XTOMOML2PDELY"); 
      m_d[i].XTOMOML2PDELZ = m_xtomo->data<float>("XTOMODATA_DATA.XTOMOML2PDELZ"); 
      m_d[i].XTOMOML2PDELA = m_xtomo->data<float>("XTOMODATA_DATA.XTOMOML2PDELA"); 
      m_d[i].XTOMOML2PYPIT = m_xtomo->data<float>("XTOMODATA_DATA.XTOMOML2PYPIT"); 
      m_d[i].XTOMOML2PZPIT = m_xtomo->data<float>("XTOMODATA_DATA.XTOMOML2PZPIT"); 
      i++;
    }

    m_xtomo->finalize();

  } else {
    m_d = new XTOMO[0];
    std::cerr << "No XtomoData table in the MuonDD Database" << std::endl;
  }

}
  
 
DblQ00Xtomo::~DblQ00Xtomo()
{
    if  (m_nObj > 0) delete [] m_d;
}

} // end of namespace MuonGM
