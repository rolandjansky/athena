/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoSpecialShapes/LArCustomShape.h"
#include "GeoSpecialShapes/LArWheelCalculator.h"
#include "GeoModelKernel/GeoShapeAction.h"

#include <string>
#include <stdexcept>

const std::string LArCustomShape::classType = "CustomShape";
const ShapeType LArCustomShape::classTypeID = 500;

LArCustomShape::LArCustomShape(const std::string& a_shapeName)
  : m_shapeName(a_shapeName), m_calculator(0)
{
	if(a_shapeName == "LAr::EMEC::InnerWheel::Absorber" || a_shapeName == "LAr::EMEC::Pos::InnerWheel::Absorber"){
		m_calculator =
			new LArWheelCalculator(LArWheelCalculator::InnerAbsorberWheel, 1);
	}
	else if(a_shapeName == "LAr::EMEC::OuterWheel::Absorber" || a_shapeName == "LAr::EMEC::Pos::OuterWheel::Absorber"){
		m_calculator =
			new LArWheelCalculator(LArWheelCalculator::OuterAbsorberWheel, 1);
	}
	else if(a_shapeName == "LAr::EMEC::InnerWheel::Electrode" || a_shapeName == "LAr::EMEC::Pos::InnerWheel::Electrode"){
		m_calculator =
			new LArWheelCalculator(LArWheelCalculator::InnerElectrodWheel, 1);
	}
	else if(a_shapeName == "LAr::EMEC::OuterWheel::Electrode"|| a_shapeName == "LAr::EMEC::Pos::OuterWheel::Electrode"){
		m_calculator =
			new LArWheelCalculator(LArWheelCalculator::OuterElectrodWheel);
	}
	else if(a_shapeName == "LAr::EMEC::Neg::InnerWheel::Absorber"){
		m_calculator =
			new LArWheelCalculator(LArWheelCalculator::InnerAbsorberWheel, -1);
	}
	else if(a_shapeName == "LAr::EMEC::Neg::OuterWheel::Absorber"){
		m_calculator =
			new LArWheelCalculator(LArWheelCalculator::OuterAbsorberWheel, -1);
	}
	else if(a_shapeName == "LAr::EMEC::Neg::InnerWheel::Electrode"){
		m_calculator =
			new LArWheelCalculator(LArWheelCalculator::InnerElectrodWheel, -1);
	}
	else if(a_shapeName == "LAr::EMEC::Neg::OuterWheel::Electrode"){
		m_calculator =
			new LArWheelCalculator(LArWheelCalculator::OuterElectrodWheel, -1);
	}
	else if(a_shapeName == "LAr::EMEC::InnerModule::Absorber"){
		m_calculator =
			new LArWheelCalculator(LArWheelCalculator::InnerAbsorberModule, 1);
	}
	else if(a_shapeName == "LAr::EMEC::OuterModule::Absorber"){
		m_calculator =
			new LArWheelCalculator(LArWheelCalculator::OuterAbsorberModule, 1);
	}
	else if(a_shapeName == "LAr::EMEC::InnerModule::Electrode"){
		m_calculator =
			new LArWheelCalculator(LArWheelCalculator::InnerElectrodModule, 1);
	}
	else if(a_shapeName == "LAr::EMEC::OuterModule::Electrode"){
		m_calculator =
			new LArWheelCalculator(LArWheelCalculator::OuterElectrodModule, 1);
	}
	else if(a_shapeName == "LAr::EMEC::InnerWheel::Glue" || a_shapeName == "LAr::EMEC::Pos::InnerWheel::Glue"){
		m_calculator =
			new LArWheelCalculator(LArWheelCalculator::InnerGlueWheel, 1);
	}
	else if(a_shapeName == "LAr::EMEC::InnerWheel::Lead" || a_shapeName == "LAr::EMEC::Pos::InnerWheel::Lead"){
		m_calculator =
			new LArWheelCalculator(LArWheelCalculator::InnerLeadWheel, 1);
	}
	else if(a_shapeName == "LAr::EMEC::OuterWheel::Glue" || a_shapeName == "LAr::EMEC::Pos::OuterWheel::Glue"){
		m_calculator =
			new LArWheelCalculator(LArWheelCalculator::OuterGlueWheel, 1);
	}
	else if(a_shapeName == "LAr::EMEC::OuterWheel::Lead" || a_shapeName == "LAr::EMEC::Pos::OuterWheel::Lead"){
		m_calculator =
			new LArWheelCalculator(LArWheelCalculator::OuterLeadWheel, 1);
	}
	else if(a_shapeName == "LAr::EMEC::Neg::InnerWheel::Glue"){
		m_calculator =
			new LArWheelCalculator(LArWheelCalculator::InnerGlueWheel, -1);
	}
	else if(a_shapeName == "LAr::EMEC::Neg::OuterWheel::Glue"){
		m_calculator =
			new LArWheelCalculator(LArWheelCalculator::OuterGlueWheel, -1);
	}
	else if(a_shapeName == "LAr::EMEC::Neg::InnerWheel::Lead"){
		m_calculator =
			new LArWheelCalculator(LArWheelCalculator::InnerLeadWheel, -1);
	}
	else if(a_shapeName == "LAr::EMEC::Neg::OuterWheel::Lead"){
		m_calculator =
			new LArWheelCalculator(LArWheelCalculator::OuterLeadWheel, -1);
	} else {
		std::string error = "Unknown name ";
		error += a_shapeName;
		error += " in LArCustomShape constructor";
		throw std::runtime_error(error);
	}
}

LArCustomShape::~LArCustomShape()
{
	delete m_calculator;
}

const LArWheelCalculator *LArCustomShape::calculator() const
{
	return m_calculator;
}

double LArCustomShape::volume() const
{
	return 0;
}

const std::string& LArCustomShape::type() const
{
	return classType;
}

ShapeType LArCustomShape::typeID() const
{
	return classTypeID;
}

const std::string& LArCustomShape::name() const
{
	return m_shapeName;
}

void LArCustomShape::exec(GeoShapeAction* action) const
{
	action->handleLArCustom(this);
}
