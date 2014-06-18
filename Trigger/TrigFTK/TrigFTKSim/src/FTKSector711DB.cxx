/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/FTKSector711DB.h"

#include <fstream>
#include <iostream>
using namespace std;

FTKSector711DB::FTKSector711DB(int nsectors, int nextralayers, const char *filename) :
  m_path(filename), m_nsectors(nsectors), m_nextralayers(nextralayers)
{
  ifstream fin(filename);
  if (!fin) {
    FTKSetup::PrintMessageFmt(ftk::sevr,"*** Error opening relation file: %s\n",filename);
  }

  // size the arrays
  m_nrelations = new int[nsectors];
  m_subID = new int*[nsectors];
  m_secID = new int*[nsectors];
  m_stereoIDs = new int**[nsectors];
		     
  int step = (nsectors+9)/10;
  for (int isec=0;isec!=nsectors;++isec) { // sectors loop
    int main_secid;
    int nsimilar;

    if (isec%step==0) cout << "Load connection " << isec << " / " << nsectors << endl;
    // read the id and the number of sectors
    fin >> main_secid >> nsimilar;
    if ( main_secid != isec) 
      FTKSetup::PrintMessageFmt(ftk::sevr,"*** error reading: %s\n",filename);

    // set the number of relations
    m_nrelations[isec] = nsimilar;

    // size the sub-vectors
    m_subID[isec] = new int[nsimilar];
    m_secID[isec] = new int[nsimilar];
    m_stereoIDs[isec] = new int*[nsimilar];

    for (int isim=0;isim!=nsimilar;++isim) { // loop over similar
      int subid;
      int secid;
      fin >> subid >> secid;
      
      // set the relation
      m_subID[isec][isim] = subid;
      m_secID[isec][isim] = secid;

      // store the ID of the stereo modules
      m_stereoIDs[isec][isim] = new int[m_nextralayers];
      for (int ip=0;ip!=m_nextralayers;++ip)
	fin >> m_stereoIDs[isec][isim][ip];
    } // end loop over similar
  } // end sectors loop

  cout << "Load connection " << nsectors << " / " << nsectors << endl;
}


FTKSector711DB::~FTKSector711DB()
{
  for (int isec=0;isec!=m_nsectors;++isec) {
    for (int isim=0;isim!=m_nrelations[isec];++isim) {
      delete [] m_stereoIDs[isec][isim];
    }

    delete [] m_subID[isec];
    delete [] m_secID[isec];
    delete [] m_stereoIDs[isec];
  }

  delete [] m_subID;
  delete [] m_secID;
  delete [] m_stereoIDs;
  
}
