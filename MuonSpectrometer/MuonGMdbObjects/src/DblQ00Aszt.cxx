/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: DblQ00Aszt.cxx,v 1.8 2009-03-30 18:13:51 roberth Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "MuonGMdbObjects/DblQ00Aszt.h"
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
DblQ00Aszt::DblQ00Aszt() : m_d(NULL)
{
    m_nObj = 0;
}
    
DblQ00Aszt::DblQ00Aszt(std::unique_ptr<IRDBQuery>&& aszt)
 : m_nObj(0)
{
  if(aszt) {
    aszt->execute();
    m_nObj = aszt->size();
    m_d = new ASZT[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Aszt banks in the MuonDD Database"<<std::endl;

    int i=0;
    while(aszt->next()) {
        m_d[i].version        = aszt->data<int>("ASZT_DATA.VERS");    
        m_d[i].line           = aszt->data<int>("ASZT_DATA.LINE");          
        m_d[i].jff            = aszt->data<int>("ASZT_DATA.JFF");
        m_d[i].jzz            = aszt->data<int>("ASZT_DATA.JZZ");
        m_d[i].job            = aszt->data<int>("ASZT_DATA.JOB");
        m_d[i].tras           = aszt->data<float>("ASZT_DATA.TRAS");
        m_d[i].traz           = aszt->data<float>("ASZT_DATA.TRAZ");
        m_d[i].trat           = aszt->data<float>("ASZT_DATA.TRAT");
        m_d[i].rots           = aszt->data<float>("ASZT_DATA.ROTS");
        m_d[i].rotz           = aszt->data<float>("ASZT_DATA.ROTZ");
        m_d[i].rott           = aszt->data<float>("ASZT_DATA.ROTT");
        m_d[i].i              = aszt->data<int>("ASZT_DATA.I");
        sprintf(m_d[i].type,"%s",aszt->data<std::string>("ASZT_DATA.TYP").c_str());
        //std::cerr<<" Aszt:: version, type, jtyp, nsta "<<m_d[i].version<<" "<<m_d[i].type<<" "<<m_d[i].jtyp<<" "<<m_d[i].nsta  <<std::endl;
        i++;
    }
    aszt->finalize();
  }
  else {
    m_d = new ASZT[0];
    std::cerr<<"NO Aszt banks in the MuonDD Database"<<std::endl;
  }
}
  
DblQ00Aszt::DblQ00Aszt(std::string asciiFileName)
{

  std::cerr<<"Aszt with asciiFileName = : <"<<asciiFileName<<"> "<<std::endl;
  // open file and count number of lines
  m_nObj=0;
  std::ifstream asztFile(asciiFileName.c_str());
  if (!asztFile.is_open()) 
    std::cerr<<" bad ascii file: "<<asciiFileName<<std::endl;
  
  
  m_nObj = std::count(std::istreambuf_iterator<char>(asztFile),
		      std::istreambuf_iterator<char>(),'\n');
  std::cout<<"Number of lines in the A-line file <"<<asciiFileName<<"> is "<< m_nObj <<std::endl;
  
  
  m_d = new ASZT[m_nObj];
  if (m_nObj == 0) std::cerr<<"NO Aszt banks in "<<asciiFileName<<std::endl;
  
  int j=0;
  //int index;

  // close and reopen file for input
  asztFile.close();
  asztFile.open(asciiFileName.c_str());

  char AlineMarker;
  while ( asztFile 
          //	  >> index 
          //	  >> m_d[j].version 
          //	  >> m_d[j].line
          >> AlineMarker 
	  >> m_d[j].type
	  >> m_d[j].jff
	  >> m_d[j].jzz
	  >> m_d[j].job
	  >> m_d[j].tras
	  >> m_d[j].traz
	  >> m_d[j].trat
	  >> m_d[j].rots
	  >> m_d[j].rotz
	  >> m_d[j].rott
	  )
  {  
    //std::cout<<" Aszt:: line "<<j+1<<" --- jtyp, jff, jzz "<<m_d[j].type<<" "<<m_d[j].jff<<" "<<m_d[j].jzz  <<std::endl;
      m_d[j].line = j+1;
      m_d[j].tras = 0.1*m_d[j].tras; // ProcessAlignments expects cm !
      m_d[j].traz = 0.1*m_d[j].traz; // ProcessAlignments expects cm !
      m_d[j].trat = 0.1*m_d[j].trat; // ProcessAlignments expects cm !
      j++;
  }
  

  if (j!=(int)m_nObj) { 
    std::cerr<<"problem with DblQ00Aszt: j="<<j<<" m_nObj="<<(int)m_nObj<<std::endl; 
    //exit(3); 
  }  

}
  
DblQ00Aszt::~DblQ00Aszt()
{
    if  (m_nObj > 0) delete [] m_d;
}

void DblQ00Aszt::WriteAsztToAsciiFile(std::string filename)
{
  std::ofstream asztFile;
  asztFile.open(filename.c_str());
  for (int j=0;j<(int)m_nObj;j++) {
    asztFile
        //        <<j<<" "
        //        <<m_d[j].version<<" " 
        //        << m_d[j].line  <<" "
        <<"A "
        << m_d[j].type  <<" " 
        << m_d[j].jff   <<" " 
        << m_d[j].jzz   <<" " 
        << m_d[j].job   <<"  "
        << m_d[j].tras*10.  <<" "  // here mm !
        << m_d[j].traz*10.  <<" "  // here mm !
        << m_d[j].trat*10.  <<" "  // here mm !
        << m_d[j].rots  <<" " 
        << m_d[j].rotz  <<" " 
        << m_d[j].rott  <<" " 
        //	    << m_d[j].i 
        << "\n";
  }
  asztFile.close();  
}

} // end of namespace MuonGM
