/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// AlpineStave.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_ALPINESTAVE_H
#define TRKGEOMETRY_ALPINESTAVE_H

#include <fstream>
#include <vector>

namespace Trk {
  class TrkDetElementBase;
  class Surface;

  class AlpineStave {
  public:
    AlpineStave(double plainStart, double plainStep, double shift, std::vector<double> mountainPositions);
    AlpineStave(double plainStart, double plainStep, double shift, std::vector<double> mountainPosPositions, std::vector<double> mountainNegPositions);
    AlpineStave(double plainStart, double plainStep, double shift, std::vector<double> *mountainPosPositions, std::vector<double> *mountainNegPositions);
    ~AlpineStave();

    // Get all surfaces or just one module surface
    std::vector<const Surface*> &getSurfaces();
    void pushPlainModules   (std::vector<const Trk::Surface*> &v, double z_inner, double z_outer) const;
    void pushMountainModules(std::vector<const Trk::Surface*> &v, double z_inner, double z_outer) const;

    // add a module to plain or get module vectors
    inline void addPlainModule(TrkDetElementBase *mod, bool first)       { 
      if (first){
	m_plainModules.insert(m_plainModules.begin(),mod); 
      } else {
	m_plainModules.push_back(mod); 
      }
      m_plainEnd = m_plainStart+m_plainModules.size()*m_plainStep;
    }
    inline void addMountainPosModule(TrkDetElementBase *mod) { 
      m_mountainPosModules.push_back(mod); 
    }
    inline void addMountainNegModule(TrkDetElementBase *mod) { 
      m_mountainNegModules.push_back(mod); 
    }
    
    // 2D root file dump
    void dump2D(std::ofstream &fout);

  private:
    void pushMountainPosModules(std::vector<const Trk::Surface*> &v, double z_inner, double z_outer) const;
    void pushMountainNegModules(std::vector<const Trk::Surface*> &v, double z_inner, double z_outer) const;

    // Plain modules ordered from -z to z, excluding transition region
    std::vector<TrkDetElementBase *> m_plainModules;
    // Transition plain modules
    std::vector<TrkDetElementBase *> m_transitionModules;
    // Positive z mountains ordered from 0 toward -z
    std::vector<TrkDetElementBase *> m_mountainPosModules;
    // Negative z mountains ordered from 0 toward +z
    std::vector<TrkDetElementBase *> m_mountainNegModules;
    // Surfaces (used for drawing the geometry)
    std::vector<const Surface *> m_surfaces;

    // Signed parameters for plain start and step
    double m_plainStep;
    double m_plainStart, m_plainEnd;

    // Absolute value of the mountain module positions on the plain plane
    double m_mountainShift;
    bool m_ownVector;
    std::vector<double> *m_mountainPosPositions;
    std::vector<double> *m_mountainNegPositions;
  };
}

#endif // TRKGEOMETRY_ALPINESTAVE_H

