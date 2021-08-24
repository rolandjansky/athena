/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSamplesMon/GraphShape.h"
#include <algorithm>

using namespace LArSamples;


GraphShape::GraphShape(const std::vector<double>& times, const std::vector<double>& values, 
                                   const std::vector<double>& errors)
{
  for (unsigned int k = 0; k < times.size(); k++)
    m_points.emplace_back(times[k], values[k], errors[k]);
}


GraphShape::GraphShape(const AbsShape& shape, double scale, double shift)
{
  for (unsigned int k = 0; k < shape.nPoints(); k++)
    m_points.emplace_back(shape.time(k) + shift, shape.value(k)*scale, shape.error(k)*scale);
}


bool GraphShape::add(const AbsShape& other)
{
  for (unsigned int k = 0; k < other.nPoints(); k++)
    m_points.emplace_back(other.time(k), other.value(k), other.error(k));

  std::sort(m_points.begin(), m_points.end());
  return true;
}
