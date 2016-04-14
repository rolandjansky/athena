/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoSpecialShapes/LArCustomShape.h"
#include "GeoModelKernel/GeoShapeAction.h"

#include <string>
#include <iostream>
#include <stdexcept>

const std::string LArCustomShape::s_classType = "CustomShape";
const ShapeType LArCustomShape::s_classTypeID = 500;

const LArCustomShape::ShapeCalc_typemap LArCustomShape::s_calculatorTypes = {
	{ "LAr::EMEC::InnerWheel::Absorber",		{LArWheelCalculator::InnerAbsorberWheel, 1} },
	{ "LAr::EMEC::Pos::InnerWheel::Absorber",	{LArWheelCalculator::InnerAbsorberWheel, 1} },

	{ "LAr::EMEC::OuterWheel::Absorber",		{LArWheelCalculator::OuterAbsorberWheel, 1} },
	{ "LAr::EMEC::Pos::OuterWheel::Absorber",	{LArWheelCalculator::OuterAbsorberWheel, 1} },

	{ "LAr::EMEC::InnerWheel::Electrode",		{LArWheelCalculator::InnerElectrodWheel, 1} },
	{ "LAr::EMEC::Pos::InnerWheel::Electrode",	{LArWheelCalculator::InnerElectrodWheel, 1} },

	{ "LAr::EMEC::OuterWheel::Electrode",		{LArWheelCalculator::OuterElectrodWheel, 1} },
	{ "LAr::EMEC::Pos::OuterWheel::Electrode",	{LArWheelCalculator::OuterElectrodWheel, 1} },

	{ "LAr::EMEC::Neg::InnerWheel::Absorber",	{LArWheelCalculator::InnerAbsorberWheel, -1} },

	{ "LAr::EMEC::Neg::OuterWheel::Absorber",	{LArWheelCalculator::OuterAbsorberWheel, -1} },

	{ "LAr::EMEC::Neg::InnerWheel::Electrode",	{LArWheelCalculator::InnerElectrodWheel, -1} },

	{ "LAr::EMEC::Neg::OuterWheel::Electrode",	{LArWheelCalculator::OuterElectrodWheel, -1} },

	{ "LAr::EMEC::InnerModule::Absorber",		{LArWheelCalculator::InnerAbsorberModule, 1} },

	{ "LAr::EMEC::OuterModule::Absorber",		{LArWheelCalculator::OuterAbsorberModule, 1} },

	{ "LAr::EMEC::InnerModule::Electrode",		{LArWheelCalculator::InnerElectrodModule, 1} },

	{ "LAr::EMEC::OuterModule::Electrode",	{LArWheelCalculator::OuterElectrodModule, 1} },

	{ "LAr::EMEC::InnerWheel::Glue",			{LArWheelCalculator::InnerGlueWheel, 1} },
	{ "LAr::EMEC::Pos::InnerWheel::Glue",		{LArWheelCalculator::InnerGlueWheel, 1} },

	{ "LAr::EMEC::InnerWheel::Lead",			{LArWheelCalculator::InnerLeadWheel, 1} },
	{ "LAr::EMEC::Pos::InnerWheel::Lead",		{LArWheelCalculator::InnerLeadWheel, 1} },

	{ "LAr::EMEC::OuterWheel::Glue",			{LArWheelCalculator::OuterGlueWheel, 1} },
	{ "LAr::EMEC::Pos::OuterWheel::Glue",		{LArWheelCalculator::OuterGlueWheel, 1} },

	{ "LAr::EMEC::OuterWheel::Lead",			{LArWheelCalculator::OuterLeadWheel, 1} },
	{ "LAr::EMEC::Pos::OuterWheel::Lead",		{LArWheelCalculator::OuterLeadWheel, 1} },

	{ "LAr::EMEC::Neg::InnerWheel::Glue",		{LArWheelCalculator::InnerGlueWheel, -1} },

	{ "LAr::EMEC::Neg::OuterWheel::Glue",		{LArWheelCalculator::OuterGlueWheel, -1} },

	{ "LAr::EMEC::Neg::InnerWheel::Lead",		{LArWheelCalculator::InnerLeadWheel, -1} },

	{ "LAr::EMEC::Neg::OuterWheel::Lead",		{LArWheelCalculator::OuterLeadWheel, -1} }
};

LArCustomShape::LArCustomShape(const std::string& a_shapeName)
  : m_shapeName(a_shapeName),
    m_calculator(0),
    m_detStore( "StoreGateSvc/DetectorStore", a_shapeName )
{
//	if(a_shapeName == "LAr::EMEC::InnerWheel::Absorber" || a_shapeName == "LAr::EMEC::Pos::InnerWheel::Absorber"){
//		m_calculator =
//			new LArWheelCalculator(LArWheelCalculator::InnerAbsorberWheel, 1);
//	}
//	else if(a_shapeName == "LAr::EMEC::OuterWheel::Absorber" || a_shapeName == "LAr::EMEC::Pos::OuterWheel::Absorber"){
//		m_calculator =
//			new LArWheelCalculator(LArWheelCalculator::OuterAbsorberWheel, 1);
//	}
//	else if(a_shapeName == "LAr::EMEC::InnerWheel::Electrode" || a_shapeName == "LAr::EMEC::Pos::InnerWheel::Electrode"){
//		m_calculator =
//			new LArWheelCalculator(LArWheelCalculator::InnerElectrodWheel, 1);
//	}
//	else if(a_shapeName == "LAr::EMEC::OuterWheel::Electrode"|| a_shapeName == "LAr::EMEC::Pos::OuterWheel::Electrode"){
//		m_calculator =
//			new LArWheelCalculator(LArWheelCalculator::OuterElectrodWheel, 1);
//	}
//	else if(a_shapeName == "LAr::EMEC::Neg::InnerWheel::Absorber"){
//		m_calculator =
//			new LArWheelCalculator(LArWheelCalculator::InnerAbsorberWheel, -1);
//	}
//	else if(a_shapeName == "LAr::EMEC::Neg::OuterWheel::Absorber"){
//		m_calculator =
//			new LArWheelCalculator(LArWheelCalculator::OuterAbsorberWheel, -1);
//	}
//	else if(a_shapeName == "LAr::EMEC::Neg::InnerWheel::Electrode"){
//		m_calculator =
//			new LArWheelCalculator(LArWheelCalculator::InnerElectrodWheel, -1);
//	}
//	else if(a_shapeName == "LAr::EMEC::Neg::OuterWheel::Electrode"){
//		m_calculator =
//			new LArWheelCalculator(LArWheelCalculator::OuterElectrodWheel, -1);
//	}
//	else if(a_shapeName == "LAr::EMEC::InnerModule::Absorber"){
//		m_calculator =
//			new LArWheelCalculator(LArWheelCalculator::InnerAbsorberModule, 1);
//	}
//	else if(a_shapeName == "LAr::EMEC::OuterModule::Absorber"){
//		m_calculator =
//			new LArWheelCalculator(LArWheelCalculator::OuterAbsorberModule, 1);
//	}
//	else if(a_shapeName == "LAr::EMEC::InnerModule::Electrode"){
//		m_calculator =
//			new LArWheelCalculator(LArWheelCalculator::InnerElectrodModule, 1);
//	}
//	else if(a_shapeName == "LAr::EMEC::OuterModule::Electrode"){
//		m_calculator =
//			new LArWheelCalculator(LArWheelCalculator::OuterElectrodModule, 1);
//	}
//	else if(a_shapeName == "LAr::EMEC::InnerWheel::Glue" || a_shapeName == "LAr::EMEC::Pos::InnerWheel::Glue"){
//		m_calculator =
//			new LArWheelCalculator(LArWheelCalculator::InnerGlueWheel, 1);
//	}
//	else if(a_shapeName == "LAr::EMEC::InnerWheel::Lead" || a_shapeName == "LAr::EMEC::Pos::InnerWheel::Lead"){
//		m_calculator =
//			new LArWheelCalculator(LArWheelCalculator::InnerLeadWheel, 1);
//	}
//	else if(a_shapeName == "LAr::EMEC::OuterWheel::Glue" || a_shapeName == "LAr::EMEC::Pos::OuterWheel::Glue"){
//		m_calculator = new LArWheelCalculator(LArWheelCalculator::OuterGlueWheel, 1);
//	}
//	else if(a_shapeName == "LAr::EMEC::OuterWheel::Lead" || a_shapeName == "LAr::EMEC::Pos::OuterWheel::Lead"){
//		m_calculator = new LArWheelCalculator(LArWheelCalculator::OuterLeadWheel, 1);
//	}
//	else if(a_shapeName == "LAr::EMEC::Neg::InnerWheel::Glue") {
//		m_calculator =
//			new LArWheelCalculator(LArWheelCalculator::InnerGlueWheel, -1);
//	}
//	else if(a_shapeName == "LAr::EMEC::Neg::OuterWheel::Glue" ) {
//		m_calculator =
//			new LArWheelCalculator(LArWheelCalculator::OuterGlueWheel, -1);
//	}
//	else if(a_shapeName == "LAr::EMEC::Neg::InnerWheel::Lead") {
//		m_calculator = new LArWheelCalculator(LArWheelCalculator::InnerLeadWheel, -1);
//	}
//	else if(a_shapeName == "LAr::EMEC::Neg::OuterWheel::Lead") {
//		m_calculator = new LArWheelCalculator(LArWheelCalculator::OuterLeadWheel, -1);
//	} else {
//		std::string error = "Unknown name ";
//		error += a_shapeName;
//		error += " in LArCustomShape constructor";
//		throw std::runtime_error(error);
//	}
	
	
	if ( createCalculator( s_calculatorTypes.at(a_shapeName) ).isFailure() ) { // map.at throws std::out_of_range exception on unknown shape name
		std::string error = std::string("Can't create LArWheelCalculator for name ") + a_shapeName + " in LArCustomShape constructor";
		throw std::runtime_error(error);
	}
	//std::cout << "-->>> LArCustomShape::m_calculator=" << m_calculator << "\n";
}

LArCustomShape::~LArCustomShape()
{
	//delete m_calculator;
}

StatusCode LArCustomShape::createCalculator(const CalcDef_t & cdef) { // LArWheelCalculator::LArWheelCalculator_t wheelType, int zside
	LArWheelCalculator::LArWheelCalculator_t wheelType = cdef.first;
	int zside = cdef.second;
	
	m_calculator = new LArWheelCalculator(wheelType, zside); 
	std::string calcDSname = std::string("LAr::EMEC::")+ (zside>0?"Pos::":"Neg::")+LArWheelCalculator::LArWheelCalculatorTypeString(wheelType);
	
	//std::cout << "-->>> LArCustomShape::createCalculator:  m_calculator=" << m_calculator << ", name="<< calcDSname <<"\n";
	// ownership is passed to detStore
	return detStore()->record(m_calculator,  calcDSname);
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
	return s_classType;
}

ShapeType LArCustomShape::typeID() const
{
	return s_classTypeID;
}

const std::string& LArCustomShape::name() const
{
	return m_shapeName;
}

void LArCustomShape::exec(GeoShapeAction* action) const
{
	action->handleLArCustom(this);
}
