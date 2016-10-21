/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTK_SSMAP_H
#define FTK_SSMAP_H

#include "FTKRegionMap.h"
#include "FTKHit.h"
#include "ftktypes.h"
#include <vector>
#include <TFile.h>

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
  bool m_isok;
  FTKRegionMap *m_rmap;
  std::string m_path;

  struct ssm {
    int m_phiss = 0;	/* width of SS in phi */
    int m_phiwidth = 0;     /* width of module in phi */
    int m_phioffset = 0;    /* global offset of module in phi */
    int m_etass = 0;        /* width of SS in eta (for pixels) */
    int m_etawidth = 0;     /* width of module in eta (for pixels) */
    int m_etaoffset = 0;    /* global offset in eta (for pixels) */
    
    int m_ssoff = 0;        /* offset, in n-bits, for the local module ID */
    int m_ssoff_int = 0;    /* internal n-bits offset for the eta part of the local SS ID */
    int m_ndcx = 0;         /* number of DC bits in x */
    int m_ndcy = 0;         /* number of DC bits in y (pixels only) */

    /* Constantinos case: track parameters from SCT-only configuration */
    int m_fiss = 0,m_fiwidth = 0,m_fioffset = 0;
    int m_cottss = 0,m_cottwidth = 0,m_cottoffset = 0;
    int m_curvss = 0,m_curvwidth = 0,m_curvoffset = 0;
  };

   struct ssm ***m_ssm;

   // coordinate offset fraction. E.g, 0.5 for 1/2-ss shifted banks
   double m_fraction;
   bool m_bound_checks;
   bool m_useHWCoords;
  
   // global parameters for hardware compressed/DC SSID representation
   int m_eta_nondcbits; // number of eta non-dc bits
   int m_nbitsssid; // total number of ssid bits
   int m_nbitsssid_SSB; // total number of ssid bits
   int m_nbitsssid_module_pixel; // number of ssid bits to devote to the module id
   int m_nbitsssid_module_strip; // number of ssid bits to devote to the module id
  
   int ssround(int, int);
  
protected:
   static const TString s_dirnameDC;
   static const TString s_dirnameTSP;

public:
   FTKSSMap();
   FTKSSMap(FTKRegionMap *rmap, const char*, bool);
   FTKSSMap(TDirectory* file, TString DCorTSP);
   virtual ~FTKSSMap();

   static FTKSSMap* GetSSMapFromRootFile(TDirectory* file, TString DCorTSP);
   void WriteMapToRootFile(TDirectory* file, TString DCorTSP );
   static int GetChecksum(TDirectory* file, TString DCorTSP);
   int CalcChecksum() const;
   static TString GetDCorTSPDirname(TString DCorTSP);

   const FTKRegionMap* getRegionMap() const { return m_rmap; }
   const FTKPlaneMap* getPlaneMap() const { return m_rmap->getPlaneMap(); }
   FTKRegionMap* getModifyableRegionMap() const { return m_rmap; }
   FTKPlaneMap* getModifyablePlaneMap() const { return m_rmap->getModifyablePlaneMap(); }

   void checkPrint(const char *tag) const;

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
  
   //const int& getSSOffset(const FTKHit& hit) const;
   //{  return m_ssm[hit.getPlane()][hit.getSection()][0].m_ssoff; }
   const int& getSSOffset(int pl, int sec) const
      { return m_ssm[pl][sec][0].m_ssoff; }
   //const int& getSSOffsetInternal(const FTKHit& hit) const;
      // {  return m_ssm[hit.getPlane()][hit.getSection()][0].m_ssoff_int; }
   const int& getSSOffsetInternal(int pl, int sec) const
      { return m_ssm[pl][sec][0].m_ssoff_int; }

   //const int& getSSDCX(const FTKHit& hit) const;
   //     {  return m_ssm[hit.getPlane()][hit.getSection()][0].m_ndcx; }
   //const int& getSSDCY(const FTKHit& hit) const;
   //{  return m_ssm[hit.getPlane()][hit.getSection()][0].m_ndcy; }
   const int& getSSDCX(int plane) const
     {  return m_ssm[plane][0][0].m_ndcx; }
   const int& getSSDCY(int plane) const
     {  return m_ssm[plane][0][0].m_ndcy; }

   const char *getPath() const { return m_path.c_str(); }
   void setSSDCX(const int plane, const int dcx) 
     {  m_ssm[plane][0][0].m_ndcx = dcx; }
   void setSSDCY(const int plane, const int dcy)
     { m_ssm[plane][0][0].m_ndcy = dcy; }

   // primary method that converts a hit to superstrip id
   int getSSGlobal(const FTKHit&, bool quiet = false) const;
   int getSSTower(const FTKHit&, unsigned int,bool SecondStage=false) const;
   int getSS(const FTKHit &hit) const { return getSSGlobal(hit); } // deprecated, use getSSGlobal()

   // methods to encode module and local coordinates to Global SSID 
   // in these methods, "etaofff" is the eta module in the barrel,
   //  but encodes the "aside" and "section" information in the endcaps 
   int getSSx(int plane,int section,int phimod,int etaoff,int x) const;
   int getSSxy(int plane,int section,int phimod,int etaoff,
               int localX,int localY) const;

   // methods to decode Global SSID to local coordinates
   void decodeSSx(int SSid,int plane,int &section,
		 int &phimod,int &localX,int &etacode) const;
   void decodeSSxy(int SSid,int plane,int &section,
                   int &phimod, int &localX, 
                   int &etacode,int &localY) const;

#ifdef UNUSED
   // method to decode Tower SSID
   // *NOT* compatible with HWMODEID==2
   void decodeSSTower(int, int, int, int, int&, int&, float&, float&);
#endif

   // method to decode Tower SSID
   // compatible with HWMODEID==2
   void decodeSSTowerXY(int ssid,int towerid,int plane,int section,
                        int &localmoduleID,int &localX, int &localY, bool SecondStage=false);
   void decodeSSTowerX(int ssid,int towerid,int plane,int &section,
                       int &localmoduleID,int &localX, bool SecondStage=false);

   // generic, multi-use library function (is there a better place for this?)
   // Gray code input n
   static unsigned int gray_code(unsigned int n, int s=0);
  
   unsigned int compressed_ssid_word(const FTKHit& hit, unsigned localID,bool SecondStage=false) const;
   // const unsigned int compressed_local_ssid_word_pixel(unsigned int x, unsigned int y, const int plane, const int section) const;
   // const unsigned int compressed_local_ssid_word_strip(unsigned int x, const int plane, const int section) const;
   unsigned int compressed_ssid_word_pixel(const FTKHit& hit, unsigned localID,bool SecondStage=false) const;
   unsigned int compressed_ssid_word_strip(const FTKHit& hit, unsigned localID,bool SecondStage=false) const;
   unsigned int compressed_ssid_word_pixel
      (unsigned int localID,int plane,int section,
       float localX,float localY,bool SecondStage=false) const;
   unsigned int compressed_ssid_word_strip
     (int localID,int plane,int section,float localX,bool SecondStage=false) const;

   // method to create hardware LUT of local SSIDs for a given module
   std::vector<unsigned int> get_lookup_table(const int plane,const int section, const int id) const;
  
   // accessors
   void setOffsetFraction(double f) { m_fraction = f; }
   double getOffsetFraction() const { return m_fraction; }

   // encoding step for sct and pixels
   static int getPhiOffset(bool isSCT, bool isITk) {
     if (isITk) return 100000;
     if (isSCT) return 100;
     return 200; // change to 300 if pixel-eta ss<13
   }

  unsigned int getNumSuperstrips(const int plane, const int section, const int towerid) const;

  // parameters for hardware compressed/DC SSID representation
  void set_eta_nondcbits( const int eta_nondcbits ) { m_eta_nondcbits = eta_nondcbits; }
  void set_nbitsssid( const int nbitsssid ) { m_nbitsssid = nbitsssid; }
  void set_nbitsssid_SSB( const int nbitsssid_SSB ) { m_nbitsssid_SSB = nbitsssid_SSB; }
  void set_nbitsssid_module_pixel( const int nbitsssid_module ) { m_nbitsssid_module_pixel = nbitsssid_module; }
  void set_nbitsssid_module_strip( const int nbitsssid_module ) { m_nbitsssid_module_strip = nbitsssid_module; }

  int get_eta_nondcbits() const { return m_eta_nondcbits; }
  int get_nbitsssid() const { return m_nbitsssid; }
  int get_nbitsssid_SSB() const { return m_nbitsssid_SSB; }
  int get_nbitsssid_module_pixel() const { return m_nbitsssid_module_pixel; }
  int get_nbitsssid_module_strip() const { return m_nbitsssid_module_strip; }

   const struct ssm& getMap(int plane, int section, int id) const
     { return m_ssm[plane][section][id]; }

   bool getIsOK() const { return m_isok;}
   bool operator!() const { return !m_isok; }
};

#endif // FTK_SSMAP_H
