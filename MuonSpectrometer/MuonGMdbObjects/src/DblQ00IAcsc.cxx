/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: DblQ00Iacsc.cxx,v 1.8 2009-03-30 18:13:51 roberth Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "MuonGMdbObjects/DblQ00IAcsc.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstdlib>
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
DblQ00IAcsc::DblQ00IAcsc()
{
    m_nObj = 0;
    m_d = 0;
//    std::cerr <<" here we are"<<std::endl;
    
}
    
DblQ00IAcsc::DblQ00IAcsc(std::unique_ptr<IRDBQuery>&& iacsc)
 : m_nObj(0)
{
    //std::cerr<<" pointer to ISZT data "<< iacsc<<std::endl;
    
  if(iacsc) {
    iacsc->execute();
    //std::cerr<<" after execute"<<std::endl;
    
    m_nObj = iacsc->size();
    m_d = new IACSC[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO IAcsc banks in the MuonDD Database"<<std::endl;

    int i=0;
    while(iacsc->next()) {
	//std::cerr<<" do we ever come here ???"<<std::endl;
	
        m_d[i].version        = iacsc->data<int>("ISZT_DATA.VERS");    
        m_d[i].line           = i; 
        m_d[i].jff            = iacsc->data<int>("ISZT_DATA.JFF");
        m_d[i].jzz            = iacsc->data<int>("ISZT_DATA.JZZ");
        m_d[i].job            = iacsc->data<int>("ISZT_DATA.JOB");
        m_d[i].wireLayer      = iacsc->data<int>("ISZT_DATA.JLAY");
	//std::cerr<<" do we ever come here1 ???"<<std::endl;
	
        m_d[i].tras           = 10.*iacsc->data<float>("ISZT_DATA.TRAS"); // I lines in mm, but ISZT in cm
        m_d[i].traz           = 10.*iacsc->data<float>("ISZT_DATA.TRAZ"); // I lines in mm, but ISZT in cm
        m_d[i].trat           = 10.*iacsc->data<float>("ISZT_DATA.TRAT"); // I lines in mm, but ISZT in cm
        m_d[i].rots           = iacsc->data<float>("ISZT_DATA.ROTS");
        m_d[i].rotz           = iacsc->data<float>("ISZT_DATA.ROTZ");
        m_d[i].rott           = iacsc->data<float>("ISZT_DATA.ROTT");
        //m_d[i].i              = iacsc->data<int>("ISZT_DATA.I");
	//std::cerr<<" do we ever come here 2???"<<std::endl;

        sprintf(m_d[i].type,"%s",iacsc->data<std::string>("ISZT_DATA.TYP").c_str());
	//std::cerr<<" do we ever come here 3???"<<std::endl;

        //std::cerr<<" IAcsc:: version, type, jtyp, nsta "<<m_d[i].version<<" "<<m_d[i].line<<" "<<std::endl;
        i++;
    }
    iacsc->finalize();
  }
  else {
    m_d = new IACSC[0];
    std::cerr<<"NO IAcsc banks in the MuonDD Database"<<std::endl;
  }
}
  
DblQ00IAcsc::DblQ00IAcsc(std::string asciiFileName)
{

  std::cerr<<"IAcsc with asciiFileName = : <"<<asciiFileName<<"> "<<std::endl;
  // open file and count number of lines
  m_nObj=0;
  std::ifstream iacscFile(asciiFileName.c_str());
  if (!iacscFile.is_open()) 
    std::cerr<<" bad ascii file: "<<asciiFileName<<std::endl;
  
  
  m_nObj = std::count(std::istreambuf_iterator<char>(iacscFile),
		      std::istreambuf_iterator<char>(),'\n');
  std::cout<<"Number of lines in the CSc Internal A-line file <"<<asciiFileName<<"> is "<< m_nObj <<std::endl;
  
  
  m_d = new IACSC[m_nObj];
  if (m_nObj == 0) std::cerr<<"NO IAcsc banks in "<<asciiFileName<<std::endl;
  
  int j=0;
  //int index;

  // close and reopen file for input
  iacscFile.close();
  iacscFile.open(asciiFileName.c_str());

  char AlineMarker;
  while ( iacscFile 
          //	  >> index 
          //	  >> m_d[j].version 
          //	  >> m_d[j].line
          >> AlineMarker 
	  >> m_d[j].type
	  >> m_d[j].jff
	  >> m_d[j].jzz
	  >> m_d[j].job
	  >> m_d[j].wireLayer
	  >> m_d[j].tras
	  >> m_d[j].traz
	  >> m_d[j].trat
	  >> m_d[j].rots
	  >> m_d[j].rotz
	  >> m_d[j].rott
	  )
  {  
      std::cout<<" IAcsc:: line "<<j+1<<" --- jtyp, jff, jzz, job, w-layer "<<m_d[j].type<<" "
	     <<m_d[j].jff<<" "<<m_d[j].jzz  <<" "
	     <<m_d[j].job<<" "<<m_d[j].wireLayer  <<std::endl;
      m_d[j].line = j+1;
      // m_d[j].tras = m_d[j].tras; ProcessAlignments expects cm !
      // m_d[j].traz = m_d[j].traz; ProcessAlignments expects cm !
      // m_d[j].trat = m_d[j].trat; ProcessAlignments expects cm !
      j++;
  }
  

  if (j!=(int)m_nObj) { 
    std::cerr<<"problem with DblQ00IAcsc: j="<<j<<" m_nObj="<<(int)m_nObj<<std::endl; 
    //exit(3); 
  }  

}
  
DblQ00IAcsc::~DblQ00IAcsc()
{
    if  (m_nObj > 0) delete [] m_d;
}

void DblQ00IAcsc::WriteIAcscToAsciiFile(std::string filename)
{
  std::ofstream iacscFile;
  iacscFile.open(filename.c_str());
  for (int j=0;j<(int)m_nObj;j++) {
    iacscFile
        //        <<j<<" "
        //        <<m_d[j].version<<" " 
        //        << m_d[j].line  <<" "
        <<"A "
        << m_d[j].type        <<" " 
        << m_d[j].jff         <<" " 
        << m_d[j].jzz         <<" " 
        << m_d[j].job         <<"  "
        << m_d[j].wireLayer   <<"  "
        << m_d[j].tras  <<" "  // here mm !
        << m_d[j].traz  <<" "  // here mm !
        << m_d[j].trat  <<" "  // here mm !
        << m_d[j].rots  <<" " 
        << m_d[j].rotz  <<" " 
        << m_d[j].rott  <<" " 
        //	    << m_d[j].i 
        << "\n";
  }
  iacscFile.close();  
}

} // end of namespace MuonGM
