/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGTOOLS_HISTOLIMITS_H
#define JETTAGTOOLS_HISTOLIMITS_H

/******************************************************
    @class HistoLimits
    Helper struct to define bins, upper and lower bound for the histograms
********************************************************/

namespace Analysis
{

struct HistoLimits {
  /** std constructor: */
  HistoLimits() : bins(0), xmin(0.), xmax(0.), binsy(0), ymin(0.), ymax(0.) , binsz(0), zmin(0.), zmax(0.) {};
  /** 1D: */
  HistoLimits(unsigned int bin, double xmi, double xma) : bins(bin), xmin(xmi), xmax(xma), binsy(0), ymin(0.), ymax(0.), binsz(0), zmin(0.), zmax(0.) {};
  /** 2D: */
  HistoLimits(unsigned int bin, double xmi, double xma, unsigned int biny, double ymi, double yma) : bins(bin), xmin(xmi), xmax(xma), binsy(biny), ymin(ymi), ymax(yma), binsz(0), zmin(0.), zmax(0.) {};
  /** 3D: */
  HistoLimits(unsigned int bin, double xmi, double xma, unsigned int biny, double ymi, double yma, unsigned int binz, double zmi, double zma) : bins(bin), xmin(xmi), xmax(xma), binsy(biny), ymin(ymi), ymax(yma) , binsz(binz), zmin(zmi), zmax(zma) {};
  unsigned int bins;
  double xmin, xmax;
  unsigned int binsy;
  double ymin, ymax;
  unsigned int binsz;
  double zmin, zmax;
};

}
#endif
