/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETALIGNGENALGS_SIDISTWRITEALG_H
#define INDETALIGNGENALGS_SIDISTWRITEALG_H

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"

#include "Identifier/Identifier.h"

class PixelID;
class SCT_ID;

class SiDistWriteAlg : public AthAlgorithm {
 public:
  SiDistWriteAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~SiDistWriteAlg();

  StatusCode  initialize();    
  StatusCode  execute();
  StatusCode  finalize();  

private:

  const PixelID* m_pixid;
  const SCT_ID* m_sctid;
  bool m_first; // first event flag
  // parameters
  bool m_par_print; // flag to print out information 
  int m_par_size; // size of distortion array
  std::string m_par_readfile; // text file to read for information
  std::string m_par_distkey; // SG key for distortion information

  // private methods
  bool readFile();
  bool makeIdent(const std::string& ident,
		 const Identifier32::value_type identhash, Identifier& identifier);
  void print();
};

#endif // INDETALIGNGENALGS_SIDISTWRITEALG_H
