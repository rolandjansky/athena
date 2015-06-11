/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_MuCTPiGeometry
#define TrigConf_MuCTPiGeometry

#include <string>
#include <vector>

#include "TrigConfMuctpi/L1MuonPtEncoding.h"

class MioctGeometry;

class MuCTPiGeometry{
   public:
      MuCTPiGeometry();
      ~MuCTPiGeometry();
    
   // getters
   const  std::vector<MioctGeometry>& octants() {return m_Octants; };
   const L1MuonPtEncoding & ptEncoding() const { return m_ptEncoding; }

   // setters
   void addOctant( const MioctGeometry& octant) {m_Octants.push_back(octant);};   
   void setPtEncoding(const L1MuonPtEncoding & ptEncoding) { m_ptEncoding = ptEncoding; }
   
   
   virtual void print(const std::string& indent="", unsigned int detail=1) const;
   virtual void writeXML(std::ostream & xmlfile, int indentLevel=0, int indentWidth=4) const;
   
private:
   std::vector<MioctGeometry> m_Octants;
   L1MuonPtEncoding m_ptEncoding;

};


#endif 
  
