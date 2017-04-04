/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __COMMON_ALGORITHMS_H__
#define __COMMON_ALGORITHMS_H__

typedef struct WeightedCoordinate{
  struct Comparator {
    bool operator()(const struct WeightedCoordinate& wc1, const struct WeightedCoordinate& wc2) {
      return (wc2.m_x > wc1.m_x);
    }
  };
public:
WeightedCoordinate(double x, double w) : m_x(x), m_w(w) {};
WeightedCoordinate(const WeightedCoordinate& wc) : m_x(wc.m_x), m_w(wc.m_w) {};
  double m_x, m_w;
private:
WeightedCoordinate() : m_x(0.0), m_w(0.0) {};
} WEIGHTED_COORDINATE;

typedef class CdfApproximator {
 public:
  CdfApproximator(){};
  double findMedian(std::vector<WEIGHTED_COORDINATE>&);
  double findModes(std::vector<WEIGHTED_COORDINATE>&, std::vector<double>&, int);
} CDF_APPROXIMATOR;

#endif
