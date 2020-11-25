/*
 *   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#ifndef CUSTOMTOPOCLUSTER_H
#define CUSTOMTOPOCLUSTER_H

#include <vector>
#include <iostream>

class CustomTopoCluster
{

 public:
  CustomTopoCluster() {}
  ~CustomTopoCluster() {}

  void erase() {
    ncells = 0;
    e  = 0.0;
    et = 0.0;
    time = 0.0;
    eta = 0.0;
    phi = 0.0;
    cell_id = {};
  }
  bool isEmptyCluster() const{
    if (ncells == 0) return true;
    return false;
  }

  int ncells;
  float e;
  float et;
  float time;
  float eta;
  float phi;
  std::vector<unsigned int> cell_id;

 private:

};
#endif //CUSTOMTOPOCLUSTER_H
