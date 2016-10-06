// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerTest/MapDumper.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2010
 * @brief Helper to access contents of a stl map from python.
 */


#include <map>
#include <string>
#include <vector>


#ifndef D3PDMAKERTEST_MAPDUMPER_H
#define D3PDMAKERTEST_MAPDUMPER_H


namespace D3PDTest {


class MapDumper
{
public:
  typedef std::map<std::string, int> mapi_t;
  static void dump (const mapi_t& map);
  static std::vector<std::string> keys (const mapi_t& map);
  static std::vector<int> values (const mapi_t& map);
  static bool equal (const mapi_t& m1, const mapi_t& m2);

  typedef std::map<std::string, float> mapf_t;
  static void dump (const mapf_t& map);
  static std::vector<std::string> keys (const mapf_t& map);
  static std::vector<float> values (const mapf_t& map);
  static bool equal (const mapf_t& m1, const mapf_t& m2);

  typedef std::map<std::string, std::string> maps_t;
  static void dump (const maps_t& map);
  static std::vector<std::string> keys (const maps_t& map);
  static std::vector<std::string> values (const maps_t& map);
  static bool equal (const maps_t& m1, const maps_t& m2);
};


} // namespace D3PDTest


#endif // not D3PDMAKERTEST_MAPDUMPER_H
