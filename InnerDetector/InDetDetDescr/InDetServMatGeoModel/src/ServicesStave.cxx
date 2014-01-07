/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/ServicesStave.h"

#include "InDetServMatGeoModel/DetType.h"

ServicesStave::ServicesStave( DetType::Type type,
			      DetType::Part part,
			      int nInlets, const std::string& inletPipe,
			      int nOutlets, const std::string& outletPipe,
			      int nCables, const std::string& cable,
			      int nFibres, const std::string& fibre) :
  m_type(type), m_part(part), 
  m_nInlets(nInlets), m_nOutlets(nOutlets),
  m_nCables(nCables), m_nFibres(nFibres),
  m_inletPipe(inletPipe), m_outletPipe(outletPipe),
  m_cable(cable), m_fibre(fibre) 
{}
