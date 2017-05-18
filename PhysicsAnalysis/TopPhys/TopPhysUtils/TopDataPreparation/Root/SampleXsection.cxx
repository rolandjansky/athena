/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <fstream>
#include <sstream>
#include <string>

#include "TopDataPreparation/SampleXsection.h"
#include <iostream>
#include <stdlib.h>

using namespace std;

SampleXsection &SampleXsection::operator=(const SampleXsection &xs)
{
  if (this!=&xs) {
     m_Xsects=xs.m_Xsects;
     m_Uncert=xs.m_Uncert;
  }
  return *this;
}

bool SampleXsection::readFromFile(const char *fName)
{
  ifstream in(fName);
  if (!in) return false;

  for( ; !in.eof() ; ) {
    string line;
    if (!getline(in,line)) break;
    if (!line.empty() && line[0]!='#') {
      istringstream istr(line);
      int dsid=-1;
      double xSect,kFact,xSectDw,xSectUp;
      string s_shower, s_xSectDw, s_xSectUp;
      showering shower;
      istr >> dsid >> xSect >> kFact >> s_shower >> s_xSectDw >> s_xSectUp;
      if(!s_shower.empty() && s_shower[0]!='#') {
        if      (s_shower=="pythia") shower=pythia;
        else if (s_shower=="herwig") shower=herwig;
        else if (s_shower=="sherpa") shower=sherpa;
        else if (s_shower=="sherpa21") shower=sherpa21;
        else if (s_shower=="pythia8") shower=pythia8;
        else if (s_shower=="herwigpp") shower=herwigpp;
        else {
	  cerr << "ERROR!! TopDataPreparation::SampleXsection::readFromFile: unknown showering (which is needed for btagging SF!!!) : " << s_shower << " for DSID= " << dsid <<endl;
	  shower=unknown;
	}
	//cout << "INFO SampleXsection::readFromFile: showering : " << s_shower << " -> " << shower << " for DSIS= " <<dsid <<endl;
	
        if(!s_xSectDw.empty() && s_xSectDw[0]!='#') {
          xSectDw = atof(s_xSectDw.c_str());
          if(!s_xSectUp.empty()){
            xSectUp = atof(s_xSectUp.c_str());
          }else{
            cerr << "ERROR!! SampleXsection::readFromFile: xSectDw of "<<dsid<<" is defined ( "<<s_xSectDw<<" ), but not xSectUp" <<endl;
 	    xSectUp=-1.;
         }
         //cout << " xSectDw= " << xSectDw << "\txSectUp= " << xSectUp << endl;
        }
        else {
           xSectDw=-1.;
           xSectUp=-1.;
        }
      }// !s_showering.empty()
      else{
           xSectDw=-1.;
           xSectUp=-1.;
	   shower = unknown;
      }
      setSample(dsid,xSect,kFact,shower,xSectDw,xSectUp);
    }
  }

  in.close();
  return true;
}

void SampleXsection::setSample(const int dsid,const double xSect,const double kFact, const showering shower, const double xSectDw, const double xSectUp)
{ 
  if (dsid<0) return;
  map<int,pair<double,double> >::const_iterator it=m_Xsects.find(dsid);
  if (it!=m_Xsects.end()) {
     cerr << "ERROR!! SampleXsection::setSample: duplicate entry ! DSID= " << dsid <<endl;
     //cerr << "ERROR!! SampleXsection::setSample:                   xSect=" << xSect << "\told xSect=" << it->second.first <<endl;
  }
  m_Xsects[dsid]=pair<double,double>(xSect,kFact);
  m_Uncert[dsid]=pair<double,double>(xSectDw,xSectUp);
  m_Showering[dsid]=shower;
}

double SampleXsection::getRawXsection(const int dsid) const
{
  map<int,pair<double,double> >::const_iterator it=m_Xsects.find(dsid);
  if (it!=m_Xsects.end()) return it->second.first;
  return -1;
}

double SampleXsection::getKfactor(const int dsid) const
{
  map<int,pair<double,double> >::const_iterator it=m_Xsects.find(dsid);
  if (it!=m_Xsects.end()) return it->second.second;
  return -1;
}

double SampleXsection::getXsection(const int dsid) const
{
  map<int,pair<double,double> >::const_iterator it=m_Xsects.find(dsid);
  if (it!=m_Xsects.end()) return (it->second.first)*(it->second.second);
  return -1;
}

double SampleXsection::getXsectionDown(const int dsid) const
{
  map<int,pair<double,double> >::const_iterator it=m_Uncert.find(dsid);
  if (it!=m_Uncert.end()) return it->second.first;
  return -1;
}

double SampleXsection::getXsectionUp(const int dsid) const
{
  map<int,pair<double,double> >::const_iterator it=m_Uncert.find(dsid);
  if (it!=m_Uncert.end()) return it->second.second;
  return -1;
}

pair<double,double> SampleXsection::getXsectionDownUp(const int dsid) const
{
  map<int,pair<double,double> >::const_iterator it=m_Uncert.find(dsid);
  if (it!=m_Uncert.end()) return it->second;
  pair<double,double> temp=pair<double,double>(-1,-1);
  return temp;
}

SampleXsection::showering SampleXsection::getShowering(const int dsid) const
{
  map<int,showering >::const_iterator it=m_Showering.find(dsid);
  if (it!=m_Showering.end()) return it->second;
  return unknown;
}

int SampleXsection::getShoweringIndex(const int dsid) const
{
  
  // A.Knue: labelling taken from here: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/BTagCalib2015#Pre_Recommendations_for_MC15_Bas

  showering shower = getShowering(dsid);

  if(shower == showering::pythia)
    return 0;
  else if (shower == showering::herwigpp || shower == showering::herwig)// use the herwig++ maps for the few fortran herwig samples we may have
    return 1;
  else if (shower == showering::pythia8)
    return 2;
  else if (shower == showering::sherpa)
    return 3;
  else if (shower == showering::sherpa21)
    return 4;
  else{
    
    std::cout << "===========================================================================================================================================" << std::endl;
    std::cout << "ERROR:TopDataPreparation: The DSID " << dsid << " does not have a showering algorithm defined!!! This is needed for MC/MC SF!!! ---> EXIT." << std::endl;
    std::cout << "\t" << std::endl;
    exit(1);

  }
  
  return 0;

}
