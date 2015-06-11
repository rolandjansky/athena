/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_L1MuonPtEncoding
#define TrigConf_L1MuonPtEncoding

#include <iostream>

class L1MuonPtEncoding {
public:
   // constructor
   L1MuonPtEncoding();
   // destructor
   ~L1MuonPtEncoding(){}

   void setCodingInfo(unsigned int idx, unsigned int ptCode, unsigned int thresholdValue );

   void getCodingInfo(unsigned int idx, unsigned int & ptCode, unsigned int & thresholdValue ) const;

   void print(const std::string& indent="", unsigned int detail=1) const;
   
   void writeXML(std::ostream & xmlfile, int indentLevel=0, int indentWidth=2) const;

private:
   struct code {
      unsigned int ptCode; // encoding for topo
      unsigned int thresholdValue; // muon pt threshold
   };

   code m_encoding[6];

};

#endif
