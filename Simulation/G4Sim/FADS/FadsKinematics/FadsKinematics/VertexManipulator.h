/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VertexManipulator_H
#define VertexManipulator_H

#include "FadsKinematics/ManipulatorBase.h"
#include "FadsKinematics/GeneratorCenter.h"
#include <string>
class G4PrimaryVertex;

namespace FADS {


  class VertexManipulator : public ManipulatorBase {
  public:

	VertexManipulator(const std::string& s)
      : ManipulatorBase(s)
    {
      GeneratorCenter::GetGeneratorCenter()->AddVertexManipulator(this);
	}

	virtual ~VertexManipulator() {}

	virtual bool EditVertex(G4PrimaryVertex* v) = 0;
	virtual void EventInitialization() {}

  };


}

#endif
