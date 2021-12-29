/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_XAODCHARGEDTRACKSWEIGHTFILTER_H
#define GENERATORFILTERS_XAODCHARGEDTRACKSWEIGHTFILTER_H

#include "GeneratorModules/GenFilter.h"
#include "GaudiKernel/ServiceHandle.h"

#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthEventContainer.h"

/// Filter events based on presence of charged tracks
class xAODChargedTracksWeightFilter : public GenFilter {
public:

  xAODChargedTracksWeightFilter(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode filterInitialize();
  StatusCode filterEvent();
  StatusCode filterFinalize();

  struct Spline {
     /// Linear spline representation of a function used to calculate weights

     struct Point {
        /// Single point and slope to next point
        Point(double _x, double _y, double _slope) :
           x(_x), y(_y), slope(_slope) {};

        double x;
        double y;
        double slope;
     };

     /// n-1 spline points filled at initialization
     std::vector<Point> points;

     StatusCode initialize( std::vector<double> & x, std::vector<double> & y);

     /// get minimum over certain range (requires initialization to extrapolate)
     double get_minimum(double min, double max) const;

     /// get value for certain x
     double value(int nch) const;
  };

private:

  /// get weight to filter and weight events
  double get_nch_weight(int nch) const;
  
  /// read the event weight
  StatusCode event_weight(double & event_weight) const;

  /// modify the event weight by weight
  void weight_event(double weight);

  /// Rndm generator service
  ServiceHandle<IAtRndmGenSvc> m_randSvc;  

  // Minimum pT for a track to count
  double m_Ptmin;

  // Maximum |pseudorapidity| for a track to count
  double m_EtaRange;

  // Minimum number of tracks
  int m_nchmin;

  // Maximum number of tracks
  int m_nchmax;

  Spline m_spline;

  /// Spline points
  std::vector<double> m_weight_fun_x;
  std::vector<double> m_weight_fun_y;

  double m_min_weight = 0;

  /// Number of events passing selection (weighted, orig weight)
  double m_nevents_selected = 0;
  /// Number of events passing weight-filtering (weighted, origin weight)
  double m_nevents_accepted = 0;
};

#endif
