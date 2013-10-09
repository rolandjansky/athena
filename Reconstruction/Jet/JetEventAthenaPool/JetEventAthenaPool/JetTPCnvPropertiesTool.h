// emacs this is -*- c++ -*- 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef JETEVENTATHENAPOOL_JETCNVPROPERTIESTOOL_H 
#define JETEVENTATHENAPOOL_JETCNVPROPERTIESTOOL_H 

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/AlgTool.h"

//PA Tool
#include "JetEventTPCnv/JetCnvProperties.h"


/** Must declare this, with name of interface*/
static const InterfaceID IID_IJetTPCnvPropertiesTool("IJetTPCnvPropertiesTool", 1, 0);

class IJetTPCnvPropertiesTool : virtual public IAlgTool {
public:
  /**Declared here, and defined below*/
  static const InterfaceID& interfaceID();
        
};

inline const InterfaceID& IJetTPCnvPropertiesTool::interfaceID()
{ 
  return IID_IJetTPCnvPropertiesTool; 
}




class JetTPCnvPropertiesTool :  virtual public IJetTPCnvPropertiesTool, public AlgTool, public JetCnvProperties {
public:
  JetTPCnvPropertiesTool(const std::string&,const std::string&,const IInterface*);
        
  virtual ~JetTPCnvPropertiesTool () {};
  
  virtual StatusCode initialize();
  
};
#endif
