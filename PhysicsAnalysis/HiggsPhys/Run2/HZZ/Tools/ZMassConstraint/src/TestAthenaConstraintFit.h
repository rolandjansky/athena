// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TESTATHENACONSTRAINTFIT_H
#define TESTATHENACONSTRAINTFIT_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AsgTools/ToolHandle.h"

// Local include(s):
#include "ZMassConstraint/IConstraintFit.h"

class TestAthenaConstraintFit : public AthAlgorithm {

public:
    /// Regular Algorithm constructor
    TestAthenaConstraintFit( const std::string& name, ISvcLocator* svcLoc );
  
    /// Function initialising the algorithm
    virtual StatusCode initialize();
    /// Function executing the algorithm
    virtual StatusCode execute();
  
private:
    std::string m_electronContName;
    std::string m_vertexContName;
  
    ToolHandle<ZMassConstraint::IConstraintFit> m_constraintFit;

}; // class TestAthenaConstraintFit

#endif // TESTATHENACONSTRAINTFIT_H

