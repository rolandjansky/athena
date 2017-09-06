/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <fstream>
#include <sstream>
#include <string>

#include "TopDataPreparation/SamplePDF.h"
#include <iostream>
#include <stdlib.h>

using namespace std;

SamplePDF &SamplePDF::operator=(const SamplePDF &xs)
{
  if (this!=&xs) {
     m_PDFs=xs.m_PDFs;
  }
  return *this;
}

bool SamplePDF::readFromFile(const char *fName)
{
  ifstream in(fName);
  if (!in) {cerr << "SamplePDF::readFromFile : issue with " << fName << endl; return false;}

  for( ; !in.eof() ; ) {
    string line;
    if (!getline(in,line)) break;
    if (!line.empty() && line[0]!='#') {
      istringstream istr(line);
      int dsid, PDFid=-1;
      istr >> dsid >> PDFid;
      //cout << dsid << "\t" << PDFid << endl;
      setSample(dsid,PDFid);
    }
  }

  in.close();
  return true;
}

void SamplePDF::setSample(const int dsid, const int PDFid)
{ 
  if (dsid<0) return;
  map<int, int >::const_iterator it=m_PDFs.find(dsid);
  if (it!=m_PDFs.end()) {
     cerr << "ERROR!! SamplePDF::setSample: duplicate entry ! DSID= " << dsid <<endl;
     //cerr << "ERROR!! SamplePDF::setSample:                   xSect=" << xSect << "\told xSect=" << it->second.first <<endl;
  }
  m_PDFs[dsid]=PDFid;
}


int SamplePDF::getPDFid(const int dsid) const
{
  map<int,int >::const_iterator it=m_PDFs.find(dsid);
  if (it!=m_PDFs.end()) return it->second;
  return unknown;
}
