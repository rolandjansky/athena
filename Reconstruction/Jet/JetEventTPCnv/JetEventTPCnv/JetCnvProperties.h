///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENTTPCNV_JETCNVPROPERTIES_H 
#define JETEVENTTPCNV_JETCNVPROPERTIES_H 

#include <string>
#include <vector>



class JetCnvProperties {
public:

  std::vector<std::string> & get_no_constit_colls(){return s_write0constit_colls;}
  void set_no_constit(bool v){s_write0constit = v;}

protected:
  static std::vector<std::string> s_write0constit_colls;
  static bool s_write0constit;
  
};

#endif
