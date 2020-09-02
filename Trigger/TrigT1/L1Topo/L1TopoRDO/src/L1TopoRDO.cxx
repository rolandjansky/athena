/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TopoRDO/L1TopoRDO.h"

#include <cstdint>
#include <vector>
#include <sstream>
#include <iomanip>
#include <string>
#include "L1TopoRDO/ModuleID.h"


const std::vector<uint32_t>& L1TopoRDO::getDataWords () const
{
  return m_dataWords;
}

void L1TopoRDO::setDataWords (std::vector<uint32_t> &&dataWords) noexcept
{
  m_dataWords = std::move(dataWords);
}

const std::vector<uint32_t>& L1TopoRDO::getStatusWords () const
{
  return m_statusWords;
}

void L1TopoRDO::setStatusWords (std::vector<uint32_t> &&statusWords) noexcept
{
  m_statusWords = std::move(statusWords);
}

std::vector<L1Topo::Error> L1TopoRDO::getErrors() const
{
  std::vector<L1Topo::Error> errors;
  // bit of a hack: use the map of error enum to string as a way to iterate over all the errors that are defined
  for (auto it : L1Topo::errorText){
    auto e = it.first;
    if (checkError(e)){
      errors.push_back(e);
    }
  }
  return errors;
}


bool L1TopoRDO::checkError(L1Topo::Error e) const{
  return (m_error>>static_cast<unsigned int>(e) & 1);
}

void L1TopoRDO::setError(L1Topo::Error e){
  m_error |= (1 << static_cast<unsigned int>(e));
}

uint32_t L1TopoRDO::getSourceID() const
{
  return m_sourceID;
}

void L1TopoRDO::setSourceID(const uint32_t id)
{
  m_sourceID=id;
}

bool L1TopoRDO::isDAQModule() const
{
  return L1Topo::ModuleID(static_cast<uint16_t>(m_sourceID)).isDAQ();
}

bool L1TopoRDO::isROIModule() const
{
  return L1Topo::ModuleID(static_cast<uint16_t>(m_sourceID)).isROI();
}

const std::string L1TopoRDO::dump() const
{
  std::ostringstream s;
  s << " ";
  for (auto & word: m_dataWords){
    s << std::hex << std::showbase << std::setfill('0') << std::setw(8) 
      << word << " " << std::dec << std::noshowbase;
  }
  return s.str();
}

