/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SAMPLEPDF_H
#define SAMPLEPDF_H

#include <map>

/** @class SamplePDFSvc
 **    
 ** @author David Calvet <calvet@in2p3.fr>
 **
 ** @brief A class to provide access to Monte Carlo sample luminosity information.
 **
*/
class SamplePDF {

public:
  
  enum showering{ pythia=0, herwig=1, sherpa=2, pythia8=4, herwigpp=5, sherpa21=6, unknown=10};
  
  inline SamplePDF() : m_PDFs() {}
  inline SamplePDF(const SamplePDF &xs) : m_PDFs(xs.m_PDFs) {}
  inline ~SamplePDF() {}

  SamplePDF &operator=(const SamplePDF &xs);

  bool readFromFile(const char *fName);
  void setSample(const int dsid,const int PDFid);

  int getPDFid(const int dsid) const;

private:

  std::map<int, int > m_PDFs;
};

#endif // SAMPLEPDF_H
