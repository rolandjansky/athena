/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MAGFIELDUTILS_IDENTITYMANIPULATOR_H
#define MAGFIELDUTILS_IDENTITYMANIPULATOR_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "MagFieldInterfaces/IMagFieldManipulator.h"

namespace MagField {

  class IdentityManipulator : public IMagFieldManipulator, public AthAlgTool
  {
  public:
    IdentityManipulator(const std::string& name,
  		    const std::string& type,
  		    const IInterface* parent);
  
    /// Initialize AlgTool
    virtual StatusCode initialize();
    
    /// Finalize AlgTool
    virtual StatusCode finalize();

    /// Change the position at which the field is evaluated
    virtual void modifyPosition(const double *xyz_old, double *xyz_new);
  
    /// Correct field (input field must have been evaluated in the point indicated by modifyPosition)
    virtual void modifyField(double *bxyz, double *deriv = 0);
  
    virtual ~IdentityManipulator();
  
  protected:
  };

}


#endif
