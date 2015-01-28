// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerTest/src/MapDumper.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2010
 * @brief Helper to access contents of a stl map from python.
 */


#include "D3PDMakerTest/MapDumper.h"
#include <iostream>


namespace D3PDTest {


void MapDumper::dump (const mapi_t& map)
{
  mapi_t::const_iterator it  = map.begin();
  mapi_t::const_iterator end = map.end();
  for (; it != end; ++it) {
    std::cout << it->first << " " << it->second << "\n";
  }
}


std::vector<std::string> MapDumper::keys (const mapi_t& map)
{
  std::vector<std::string> out;
  out.reserve (map.size());
  mapi_t::const_iterator it  = map.begin();
  mapi_t::const_iterator end = map.end();
  for (; it != end; ++it) {
    out.push_back (it->first);
  }
  return out;
}


std::vector<int> MapDumper::values (const mapi_t& map)
{
  std::vector<int> out;
  out.reserve (map.size());
  mapi_t::const_iterator it  = map.begin();
  mapi_t::const_iterator end = map.end();
  for (; it != end; ++it) {
    out.push_back (it->second);
  }
  return out;
}


bool MapDumper::equal (const mapi_t& m1, const mapi_t& m2)
{
  return m1 == m2;
}


void MapDumper::dump (const mapf_t& map)
{
  mapf_t::const_iterator it  = map.begin();
  mapf_t::const_iterator end = map.end();
  for (; it != end; ++it) {
    std::cout << it->first << " " << it->second << "\n";
  }
}


std::vector<std::string> MapDumper::keys (const mapf_t& map)
{
  std::vector<std::string> out;
  out.reserve (map.size());
  mapf_t::const_iterator it  = map.begin();
  mapf_t::const_iterator end = map.end();
  for (; it != end; ++it) {
    out.push_back (it->first);
  }
  return out;
}


std::vector<float> MapDumper::values (const mapf_t& map)
{
  std::vector<float> out;
  out.reserve (map.size());
  mapf_t::const_iterator it  = map.begin();
  mapf_t::const_iterator end = map.end();
  for (; it != end; ++it) {
    out.push_back (it->second);
  }
  return out;
}


bool MapDumper::equal (const mapf_t& m1, const mapf_t& m2)
{
  return m1 == m2;
}


void MapDumper::dump (const maps_t& map)
{
  maps_t::const_iterator it  = map.begin();
  maps_t::const_iterator end = map.end();
  for (; it != end; ++it) {
    std::cout << it->first << " " << it->second << "\n";
  }
}


std::vector<std::string> MapDumper::keys (const maps_t& map)
{
  std::vector<std::string> out;
  out.reserve (map.size());
  maps_t::const_iterator it  = map.begin();
  maps_t::const_iterator end = map.end();
  for (; it != end; ++it) {
    out.push_back (it->first);
  }
  return out;
}


std::vector<std::string> MapDumper::values (const maps_t& map)
{
  std::vector<std::string> out;
  out.reserve (map.size());
  maps_t::const_iterator it  = map.begin();
  maps_t::const_iterator end = map.end();
  for (; it != end; ++it) {
    out.push_back (it->second);
  }
  return out;
}


bool MapDumper::equal (const maps_t& m1, const maps_t& m2)
{
  return m1 == m2;
}


} // namespace D3PDTest
