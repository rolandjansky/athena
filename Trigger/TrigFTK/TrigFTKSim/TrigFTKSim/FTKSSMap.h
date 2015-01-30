/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTK_SSMAP_H
#define FTK_SSMAP_H

#include "FTKRegionMap.h"
#include "FTKHit.h"
#include "ftktypes.h"
#include <vector>

// Notes on hardware-like SSID calculation
// ---------------------------------------
// used inside FTKSSmap getSSTower when (hardware ss map) is enabled
// (code in AM map uses
//   ss = m_HWModeSS==0 ? m_ssmap->getSSGlobal(*(h_elm->car)) : m_ssmap->getSSTower(*(h_elm->car),m_BankID);
//  i.e. code for virtual slice / ssmap called when m_HWModeSS!=0)
//
// old calculation:
//   SCT SSID is simply moduleLocalID*ssoff + x/phiss for a given module
//   pixel SSID is 2d version of this (y/etass*ssoff+(x/phiss))
//
// new encoding: (lowest order bits of the eta and phi superstrip number are stored in consecutive bits.)
//    	(eta/eta_ss_width)[n...1] (phi/phi_ss_width)[n...1] (eta/eta_ss_width)[0] (phi/phi_ss_width)[0]
//
// When the available number of bits is not sufficient to express the total number of unique superstrips, a
// compression scheme is employed that (first) packs the information more tightly and then, failing
// that (second) drops some information by multiplexing different superstrips to the same SSID. At
// present this multiplexing simply drops higher order bits, but if superstrip sizes decrease to the
// point that multiplexing becomes necessary, someone should study the best multiplexing assignment in detail
// (what assignment gives the lowest false pattern rate / avoids geometric correlations across layers).
//
//  The module_id -> tower id hash
//  -> max ss/module * tower mod_id
//
//  the max ss/module can be compressed or expanded depending on how many ssid bits are available.
//
//  The global ssid is the arithmetic sum of the ssid_offset and the local ssid.


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
    int m_ndcx;         /* number of DC bits in x */
    int m_ndcy;         /* number of DC bits in y (pixels only) */

    /* Constantinos case: track parameters from SCT-only configuration */
    int m_fiss,m_fiwidth,m_fioffset;
    int m_cottss,m_cottwidth,m_cottoffset;
    int m_curvss,m_curvwidth,m_curvoffset;
  };

   struct ssm ***m_ssm;

   // coordinate offset fraction. E.g, 0.5 for 1/2-ss shifted banks
   double m_fraction;
   bool m_bound_checks;
  
   // global parameters for hardware compressed/DC SSID representation
   int m_eta_nondcbits; // number of eta non-dc bits
   int m_nbitsssid; // total number of ssid bits
   int m_nbitsssid_module_pixel; // number of ssid bits to devote to the module id
   int m_nbitsssid_module_strip; // number of ssid bits to devote to the module id
  
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
  
   const int& getSSOffset(const FTKHit& hit) const
     {  return m_ssm[hit.getPlane()][hit.getSection()][0].m_ssoff; }
   const int& getSSOffset(int pl, int sec) const
   { return m_ssm[pl][sec][0].m_ssoff; }
   const int& getSSOffsetInternal(const FTKHit& hit) const
     {  return m_ssm[hit.getPlane()][hit.getSection()][0].m_ssoff_int; }
   const int& getSSOffsetInternal(int pl, int sec) const
      { return m_ssm[pl][sec][0].m_ssoff_int; }

   const int& getSSDCX(const FTKHit& hit) const
     {  return m_ssm[hit.getPlane()][hit.getSection()][0].m_ndcx; }
   const int& getSSDCY(const FTKHit& hit) const
     {  return m_ssm[hit.getPlane()][hit.getSection()][0].m_ndcy; }

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
  
   // generic, multi-use library function (is there a better place for this?)
   // Gray code input n
   unsigned int gray_code(unsigned int n, int s=0) const;
  
   unsigned int compressed_ssid_word(const FTKHit& hit, unsigned localID) const;
   // const unsigned int compressed_local_ssid_word_pixel(unsigned int x, unsigned int y, const int plane, const int section) const;
   // const unsigned int compressed_local_ssid_word_strip(unsigned int x, const int plane, const int section) const;
   unsigned int compressed_ssid_word_pixel(const FTKHit& hit, unsigned localID) const;
   unsigned int compressed_ssid_word_strip(const FTKHit& hit, unsigned localID) const;

   // method to create hardware LUT of local SSIDs for a given module
   std::vector<unsigned int> get_lookup_table(const int plane,const int section, const int id) const;
  
   // accessors
   void setOffsetFraction(double f) { m_fraction = f; }
   double getOffsetFraction() const { return m_fraction; }

   // encoding step for sct and pixels
   static int getPhiOffset(bool isSCT) {
     if (isSCT) return 100;
     return 200; // change to 300 if pixel-eta ss<13
   }

  unsigned int getNumSuperstrips(const int plane, const int section, const int towerid) const;

  // parameters for hardware compressed/DC SSID representation
  void set_eta_nondcbits( const int eta_nondcbits ) { m_eta_nondcbits = eta_nondcbits; }
  void set_nbitsssid( const int nbitsssid ) { m_nbitsssid = nbitsssid; }
  void set_nbitsssid_module_pixel( const int nbitsssid_module ) { m_nbitsssid_module_pixel = nbitsssid_module; }
  void set_nbitsssid_module_strip( const int nbitsssid_module ) { m_nbitsssid_module_strip = nbitsssid_module; }

  int get_eta_nondcbits() const { return m_eta_nondcbits; }
  int get_nbitsssid() const { return m_nbitsssid; }
  int get_nbitsssid_module_pixel() const { return m_nbitsssid_module_pixel; }
  int get_nbitsssid_module_strip() const { return m_nbitsssid_module_strip; }

   const struct ssm& getMap(int plane, int section, int id) const
     { return m_ssm[plane][section][id]; }
};

#endif // FTK_SSMAP_H
