/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTK_SSMAP_H
#define FTK_SSMAP_H

#include "FTKRegionMap.h"
#include "FTKHit.h"
#include "ftktypes.h"

class FTKSSMap {
private:
  // RegionMap used in this map
  FTKRegionMap *m_rmap;

  struct ssm {
    int m_phiss;	/* width of SS in phi */
    int m_phiwidth;     /* width of module in phi */
    int m_phioffset;    /* global offset of module in phi */
    int m_etass;        /* width of SS in eta (for pixels) */
    int m_etawidth;     /* width of module in eta (for pixels) */
    int m_etaoffset;    /* global offset in eta (for pixels) */
    
    int m_ssoff;        /* offset, in n-bits, for the local module ID */
    int m_ssoff_int;    /* internal n-bits offset for the eta part of the local SS ID */
    /* Constantinos case: track parameters from SCT-only configuration */
    int m_fiss,m_fiwidth,m_fioffset;
    int m_cottss,m_cottwidth,m_cottoffset;
    int m_curvss,m_curvwidth,m_curvoffset;
  };

   struct ssm ***m_ssm;

   // coordinate offset fraction. E.g, 0.5 for 1/2-ss shifted banks
   double m_fraction;
   bool m_bound_checks;

   int ssround(int, int);
public:
   FTKSSMap();
   FTKSSMap(FTKRegionMap *rmap, const char*, bool);
   virtual ~FTKSSMap();

   const FTKRegionMap* getRegionMap() const { return m_rmap; }
   const FTKPlaneMap* getPlaneMap() const { return m_rmap->getPlaneMap(); }

   int getPhiWidthRounded(int) const;
   int getEtaWidthRounded(int) const;

   const int& getSSPhiWidth(const FTKHit &hit) const
     { return m_ssm[hit.getPlane()][hit.getSection()][0].m_phiss; } 
   const int& getSSEtaWidth(const FTKHit &hit) const
     { return m_ssm[hit.getPlane()][hit.getSection()][0].m_etass; } 
   const int& getSSPhiSize(const FTKHit &hit) const
     { return m_ssm[hit.getPlane()][hit.getSection()][0].m_phiwidth; } 
   const int& getSSEtaSize(const FTKHit &hit) const
     { return m_ssm[hit.getPlane()][hit.getSection()][0].m_etawidth; } 

   // primary method that converts a hit to superstrip id
   int getSSGlobal(const FTKHit&, bool quiet = false) const;
   int getSSTower(const FTKHit&, unsigned int) const;
   int getSS(const FTKHit &hit) const { return getSSGlobal(hit); } // deprecated, use getSSGlobal()

   int getSS(const int &, const int &, const int &, const int &,
	     const double &x) const;
   int getSS(const int &, const int &, const int &, const int &,
	     const double &x, const double &y) const;

   void decodeSS(int, const int&, const int&,
		 int &, int &, int &, int &);
   void decodeSS(int, const int&, const int&,
		 int &, int &, int &, 
		 int &, int &, int &);

   void decodeSSTower(int, int, int, int, int&, int&, float&, float&);

   // accessors
   void setOffsetFraction(double f) { m_fraction = f; }
   double getOffsetFraction() const { return m_fraction; }

   // encoding step for sct and pixels
   static int getPhiOffset(bool isSCT) {
     if (isSCT) return 100;
     return 200; // change to 300 if pixel-eta ss<13
   }

   const struct ssm& getMap(int plane, int section, int id) const
     { return m_ssm[plane][section][id]; }
};

#endif // FTK_SSMAP_H
