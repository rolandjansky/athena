/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKEVENTTPCNV_TRIANGLEBOUNDS_P1_H
#define TRKEVENTTPCNV_TRIANGLEBOUNDS_P1_H

namespace Trk {

   /** Persisent representation of the transient TriangleBounds class.
    @author edward.moyse@cern.ch */

  class TriangleBounds_p1 {
    public:
      /**Default Constructor - needed for persistency*/
      TriangleBounds_p1(){};  
      
      std::vector<std::pair<double,double> > m_vertices;
  };
} // end of namespace
#endif 

