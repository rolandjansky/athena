/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef FTKGHOSTHITCALCULATOR_H
#define FTKGHOSTHITCALCULATOR_H

/* class FTKGhostHitCalculator
 *
 * extrapolate truth tracks to dead modules and estimate ghost hit positions
 *
 * the mapping of modules to FTK planes and towers is defined by
 * the method setFTKGeometry()
 *
 * the module positions are read from a TTree loadModuleGeometry()
 * in that three, dead modules are defined
 * overwrite the dead module positions using
 *   clearBadModuleList() and  addBadModules()
 *
 * for a given module ID, check whether the module is on the "bad" list
 *    isBad()
 *
 * for a given truth track, module positions are added
 *    using the method addGhostHit()
 *
 * Author: Stefan Schmitt, DESY
 * Date: 2017/06/28
 *
 */

#include "TrigFTKSim/FTKLogging.h"
#include <TVector3.h>
#include <TVector2.h>
#include <vector>
#include <map>
#include <set>

class FTKRegionMap;
class FTKTruthTrack;
class TDirectory;
class FTKHit;

struct FTK_ModuleGeometry {
   // geometry data of a single module
   int m_isPixel;
   int m_id;
   int m_plane;
   int m_hitSector;
   TVector3 m_center;
   TVector3 m_phiAxis;
   TVector3 m_etaAxis;
   TVector2 m_pitch;
   TVector2 m_width;
   // extrapolation method
   bool extrapolate(double rho,double cotTh,double z0,double d0,
                    double sinPhi0,double cosPhi0,TVector3 const *trackVertex,
                    double *xLocPtr,TVector2 *xPtr,double *sPtr) const;
};

class FTK_ModuleLocator {
 public:
   // locate a module
   //
   // for barrel geometry,
   //    determine R[min,max] order all modules by Z for fast lookup
   // for endcap geometry,
   //    determine Z[min,max] order all modules by R for fast lookup
   // 
   void insert(FTK_ModuleGeometry const *module);
   FTKHit *locate(FTKTruthTrack const &track,FTK_ModuleGeometry *module=0) const;
   void print(void) const;
 protected:
   struct FTK_ModuleSet {
      // barrel or disk geometry
      int m_isBarrel;
      // m_x: Rmin,Rmax (barrel) Zmin,Zmax (endcap)
      double m_x[2];
      // m_dy: maximum module width in Z (Barrel), in R (endcap)
      double m_dy;
      // m_modules: modules orderd by Zavg (Barrel), Ravg (Endcap)
      std::multimap<double,FTK_ModuleGeometry const *> m_modules;
      // method to intersect track with modules
      // if a module matches, the intersection and the module
      // are added to the output
      // the candidates are ordered by the arc length
      void getCandidates(double rho,double cotTh,double z0,double d0,
                         double sinPhi0,double cosPhi0,std::multimap
                         <double,std::pair<TVector2,FTK_ModuleGeometry
                         const *> > &candidate) const;
   };
   // barrel,endcap have different module sets
   // also, if R(barrel) or z(endcap) differs too much, start a new set
   std::vector<FTK_ModuleSet> m_moduleSetVector;
};

class FTKGhostHitCalculator : public FTKLogging {
 public:
   FTKGhostHitCalculator(const FTKRegionMap *rmap,
                         std::string const &name="FTKGhostHitCalculator");
   int loadModuleGeometry(std::string const &name,bool markAllBad,bool invertIBLphi);
   virtual int loadModuleGeometry(TDirectory *source,bool markAllBad,bool invertIBLphi);

   virtual void clearBadModuleList(void);
   int addBadModules(std::string const &name);
   int addBadModules(std::istream &in);

   virtual FTKHit *addGhostHit( FTKTruthTrack const &track,int plane,
                               FTK_ModuleGeometry *module=0 ) const;
   virtual bool isBad(int plane,int moduleID) const;

   // returns: 
   //   0: all ok
   //   1: bad plane number
   //   2: bad module id
   //   3: extrapolation failed
   virtual int extrapolateTo
      (size_t plane,int moduleID,double rho,double cotTh,double z0,double d0,
       double sinPhi0,double cosPhi0,TVector3 const *trackVertex,
       double *x) const;
   virtual FTK_ModuleGeometry const *findModule(size_t plane,int moduleID) const;
 protected:
   int addBadPixelModules(std::set<int> const &bad);
   int addBadSCTModules(std::set<int> const &bad);

   void updateCalculator(void);

   FTKRegionMap const *m_rmap;

   // for each module ID give the plane number
   std::map<int,int> m_planeFromPIXid,m_plane_From_SCTid;

   // bad modules
   std::set<int> m_badPixel,m_badSCT;

   // give geometry data by plane and module ID
   // the vector index maps the modules to FTK planes
   // the map index is the module id
   std::vector<std::map<int,FTK_ModuleGeometry> > m_geometryData;
   //
   // each plane has its own ModuleLocator
   std::vector<FTK_ModuleLocator> m_moduleLocatorByPlane;
};

#endif
