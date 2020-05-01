/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoSpecialShapes/LArCustomShape.h"
#include "GeoModelKernel/GeoShapeAction.h"

#include <string>
#include <iostream>
#include <stdexcept>

const std::string LArCustomShape::s_classType = "CustomShape";
const ShapeType LArCustomShape::s_classTypeID = 500;

const LArCustomShape::ShapeCalc_typemap LArCustomShape::s_calculatorTypes = {
  { "LAr::EMEC::InnerWheel::Absorber",           {LArG4::InnerAbsorberWheel, 1} },
  { "LAr::EMEC::InnerWheel::Electrode",          {LArG4::InnerElectrodWheel, 1} },
  { "LAr::EMEC::InnerWheel::Glue",               {LArG4::InnerGlueWheel, 1} },
  { "LAr::EMEC::InnerWheel::Lead",               {LArG4::InnerLeadWheel, 1} },

  { "LAr::EMEC::OuterWheel::Absorber",           {LArG4::OuterAbsorberWheel, 1} },
  { "LAr::EMEC::OuterWheel::Electrode",          {LArG4::OuterElectrodWheel, 1} },
  { "LAr::EMEC::OuterWheel::Glue",               {LArG4::OuterGlueWheel, 1} },
  { "LAr::EMEC::OuterWheel::Lead",               {LArG4::OuterLeadWheel, 1} },

  { "LAr::EMEC::Pos::InnerWheel::Absorber",      {LArG4::InnerAbsorberWheel, 1} },
  { "LAr::EMEC::Pos::InnerWheel::Electrode",     {LArG4::InnerElectrodWheel, 1} },
  { "LAr::EMEC::Pos::InnerWheel::Glue",          {LArG4::InnerGlueWheel, 1} },
  { "LAr::EMEC::Pos::InnerWheel::Lead",          {LArG4::InnerLeadWheel, 1} },
  { "LAr::EMEC::Pos::OuterWheel::Absorber",      {LArG4::OuterAbsorberWheel, 1} },
  { "LAr::EMEC::Pos::OuterWheel::Electrode",     {LArG4::OuterElectrodWheel, 1} },
  { "LAr::EMEC::Pos::OuterWheel::Glue",          {LArG4::OuterGlueWheel, 1} },
  { "LAr::EMEC::Pos::OuterWheel::Lead",          {LArG4::OuterLeadWheel, 1} },

  { "LAr::EMEC::Neg::InnerWheel::Absorber",      {LArG4::InnerAbsorberWheel, -1} },
  { "LAr::EMEC::Neg::InnerWheel::Electrode",     {LArG4::InnerElectrodWheel, -1} },
  { "LAr::EMEC::Neg::InnerWheel::Glue",          {LArG4::InnerGlueWheel, -1} },
  { "LAr::EMEC::Neg::InnerWheel::Lead",          {LArG4::InnerLeadWheel, -1} },
  { "LAr::EMEC::Neg::OuterWheel::Absorber",      {LArG4::OuterAbsorberWheel, -1} },
  { "LAr::EMEC::Neg::OuterWheel::Electrode",     {LArG4::OuterElectrodWheel, -1} },
  { "LAr::EMEC::Neg::OuterWheel::Glue",          {LArG4::OuterGlueWheel, -1} },
  { "LAr::EMEC::Neg::OuterWheel::Lead",          {LArG4::OuterLeadWheel, -1} },

  { "LAr::EMEC::InnerModule::Absorber",          {LArG4::InnerAbsorberModule, 1} },
  { "LAr::EMEC::InnerModule::Electrode",         {LArG4::InnerElectrodModule, 1} },
  { "LAr::EMEC::OuterModule::Absorber",          {LArG4::OuterAbsorberModule, 1} },
  { "LAr::EMEC::OuterModule::Electrode",         {LArG4::OuterElectrodModule, 1} },

  { "LAr::EMEC::Pos::InnerCone::Absorber",       {LArG4::InnerAbsorberWheel, 1} },
  { "LAr::EMEC::Pos::InnerCone::Electrode",      {LArG4::InnerElectrodWheel, 1} },
  { "LAr::EMEC::Pos::InnerCone::Glue",           {LArG4::InnerGlueWheel, 1} },
  { "LAr::EMEC::Pos::InnerCone::Lead",           {LArG4::InnerLeadWheel, 1} },
  { "LAr::EMEC::Neg::InnerCone::Absorber",       {LArG4::InnerAbsorberWheel, -1} },
  { "LAr::EMEC::Neg::InnerCone::Electrode",      {LArG4::InnerElectrodWheel, -1} },
  { "LAr::EMEC::Neg::InnerCone::Glue",           {LArG4::InnerGlueWheel, -1} },
  { "LAr::EMEC::Neg::InnerCone::Lead",           {LArG4::InnerLeadWheel, -1} },

  { "LAr::EMEC::Pos::OuterFrontCone::Absorber",  {LArG4::OuterAbsorberWheel, 1} },
  { "LAr::EMEC::Pos::OuterFrontCone::Electrode", {LArG4::OuterElectrodWheel, 1} },
  { "LAr::EMEC::Pos::OuterFrontCone::Glue",      {LArG4::OuterGlueWheel, 1} },
  { "LAr::EMEC::Pos::OuterFrontCone::Lead",      {LArG4::OuterLeadWheel, 1} },
  { "LAr::EMEC::Neg::OuterFrontCone::Absorber",  {LArG4::OuterAbsorberWheel, -1} },
  { "LAr::EMEC::Neg::OuterFrontCone::Electrode", {LArG4::OuterElectrodWheel, -1} },
  { "LAr::EMEC::Neg::OuterFrontCone::Glue",      {LArG4::OuterGlueWheel, -1} },
  { "LAr::EMEC::Neg::OuterFrontCone::Lead",      {LArG4::OuterLeadWheel, -1} },

  { "LAr::EMEC::Pos::OuterBackCone::Absorber",   {LArG4::OuterAbsorberWheel, 1} },
  { "LAr::EMEC::Pos::OuterBackCone::Electrode",  {LArG4::OuterElectrodWheel, 1} },
  { "LAr::EMEC::Pos::OuterBackCone::Glue",       {LArG4::OuterGlueWheel, 1} },
  { "LAr::EMEC::Pos::OuterBackCone::Lead",       {LArG4::OuterLeadWheel, 1} },
  { "LAr::EMEC::Neg::OuterBackCone::Absorber",   {LArG4::OuterAbsorberWheel, -1} },
  { "LAr::EMEC::Neg::OuterBackCone::Electrode",  {LArG4::OuterElectrodWheel, -1} },
  { "LAr::EMEC::Neg::OuterBackCone::Glue",       {LArG4::OuterGlueWheel, -1} },
  { "LAr::EMEC::Neg::OuterBackCone::Lead",       {LArG4::OuterLeadWheel, -1} },

  { "LAr::EMEC::Pos::InnerSlice00::Absorber",    {LArG4::InnerAbsorberWheel, 1} },
  { "LAr::EMEC::Pos::InnerSlice00::Electrode",   {LArG4::InnerElectrodWheel, 1} },
  { "LAr::EMEC::Pos::InnerSlice00::Glue",        {LArG4::InnerGlueWheel, 1} },
  { "LAr::EMEC::Pos::InnerSlice00::Lead",        {LArG4::InnerLeadWheel, 1} },
  { "LAr::EMEC::Neg::InnerSlice00::Absorber",    {LArG4::InnerAbsorberWheel, -1} },
  { "LAr::EMEC::Neg::InnerSlice00::Electrode",   {LArG4::InnerElectrodWheel, -1} },
  { "LAr::EMEC::Neg::InnerSlice00::Glue",        {LArG4::InnerGlueWheel, -1} },
  { "LAr::EMEC::Neg::InnerSlice00::Lead",        {LArG4::InnerLeadWheel, -1} },

  { "LAr::EMEC::Pos::OuterSlice00::Absorber",    {LArG4::OuterAbsorberWheel, 1} },
  { "LAr::EMEC::Pos::OuterSlice00::Electrode",   {LArG4::OuterElectrodWheel, 1} },
  { "LAr::EMEC::Pos::OuterSlice00::Glue",        {LArG4::OuterGlueWheel, 1} },
  { "LAr::EMEC::Pos::OuterSlice00::Lead",        {LArG4::OuterLeadWheel, 1} },
  { "LAr::EMEC::Neg::OuterSlice00::Absorber",    {LArG4::OuterAbsorberWheel, -1} },
  { "LAr::EMEC::Neg::OuterSlice00::Electrode",   {LArG4::OuterElectrodWheel, -1} },
  { "LAr::EMEC::Neg::OuterSlice00::Glue",        {LArG4::OuterGlueWheel, -1} },
  { "LAr::EMEC::Neg::OuterSlice00::Lead",        {LArG4::OuterLeadWheel, -1} },
};

LArCustomShape::LArCustomShape(const std::string& a_shapeName)
  : m_shapeName(a_shapeName),
    m_calculator(0)
    #ifndef XAOD_STANDALONE
        ,m_detStore( "StoreGateSvc/DetectorStore", a_shapeName )
    #endif // XAOD_STANDALONE
{
#ifdef XAOD_STANDALONE
    if ( createCalculator( s_calculatorTypes.at(a_shapeName) ) == 1 ) { // map.at throws std::out_of_range exception on unknown shape name
            std::string error = std::string("Can't create LArWheelCalculator for name ") + a_shapeName + " in LArCustomShape constructor";
            throw std::runtime_error(error);
        }
#else // XAOD_STANDALONE
    std::string name = a_shapeName;
    size_t index = name.find("Slice");
    if(index != std::string::npos) name.replace(index + 5, 2, "00");
    if(s_calculatorTypes.find(name) == s_calculatorTypes.end()){
        std::string error =
            std::string("LArCustomShape: unknown shape name ") +
                a_shapeName;
        throw std::runtime_error(error);
    } else {
        if(createCalculator(s_calculatorTypes.at(name)).isFailure()){ // map.at throws std::out_of_range exception on unknown shape name
            std::string error =
                std::string("Can't create LArWheelCalculator for name ") +
                a_shapeName + " in LArCustomShape constructor";
            throw std::runtime_error(error);
        }
    }
#endif // Athena
}

LArCustomShape::~LArCustomShape()
{
  //delete m_calculator;
}

#if defined XAOD_STANDALONE
int LArCustomShape::createCalculator(const CalcDef_t & cdef) { // LArG4::LArWheelCalculator_t wheelType, int zside
	LArG4::LArWheelCalculator_t wheelType = cdef.first;
	int zside = cdef.second;

	m_calculator = new LArWheelCalculator(wheelType, zside);
	std::string calcDSname = std::string("LAr::EMEC::")+ (zside>0?"Pos::":"Neg::")+LArWheelCalculator::LArWheelCalculatorTypeString(wheelType);

	// ownership is passed to detStore
    if (m_calculator==NULL)
        return 1;
    else
        return 0;
}
#else // XAOD_STANDALONE
StatusCode LArCustomShape::createCalculator(const CalcDef_t & cdef) { // LArG4::LArWheelCalculator_t wheelType, int zside
    LArG4::LArWheelCalculator_t wheelType = cdef.first;
    int zside = cdef.second;

    std::string calcDSname = std::string("LAr::EMEC::")+ (zside>0?"Pos::":"Neg::")+LArWheelCalculator::LArWheelCalculatorTypeString(wheelType);
    if(detStore()->contains<LArWheelCalculator>(calcDSname)){
        return detStore()->retrieve(m_calculator, calcDSname);
    }

    m_calculator = new LArWheelCalculator(wheelType, zside);

    // ownership is passed to detStore
    return detStore()->record(m_calculator,  calcDSname);
}
#endif // Athena

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
  action->handleShape(this);
}
