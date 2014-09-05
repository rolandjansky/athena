/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TESTDRIVER_H
#define TESTDRIVER_H

#include <string>


class TestDriver {
public:
  TestDriver( const std::string& name,
              const std::string& connection );

  ~TestDriver();

  void write();

  void update();


private:
  std::string m_name;
  std::string m_connection;
};

#endif
