/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
// Filename : TileL2.h
// Author   : Aranzazu Ruiz
// Created  : March 2006
//
// DESCRIPTION:
//    L2 is the class which keeps various numbers pre-calculated in ROD
//    which can be used in Level 2 trigger
//    One TileL2 contains all numbers for one drawer
//
// HISTORY:
//
// ***************************************************************************

#ifndef TILEEVENT_TILEL2_H
#define TILEEVENT_TILEL2_H

#include <vector>
#include <string>

/**
 * @class TileL2
 * @brief Class to store TileMuId and Et quantities computed at the TileCal ROD DSPs 
 * @author A. Ruiz (aranzazu.ruiz.martinez@cern.ch)
 * Modified in April-2011 to keep sumEt, sumEz, sumE in a vector
 * All doubles replaced by floats as well
 */

class TileL2 {

  public:

  /** Constructor */
  TileL2() : m_ID(0), m_phi(0.0), m_cosphi(0.0), m_sinphi(0.0) {}

  /** Constructor */
  TileL2(int id);

  /** Obsolete constructor - will be removed soon*/
  TileL2(int id, float /* phi */);

  /** Destructor */
  ~TileL2() {}

  /* Set methods */

  /** Set Muon Info in TileL2 */
  void setMu(const std::vector<float>& eta,
	     const std::vector<float>& enemu0,
	     const std::vector<float>& enemu1,
	     const std::vector<float>& enemu2,
	     const std::vector<unsigned int>& qual,
             const std::vector<unsigned int>& val) 
        {
            m_eta = eta;
            m_enemu0 = enemu0;
            m_enemu1 = enemu1;
            m_enemu2 = enemu2;
            m_quality_factor = qual;
            m_val = val;
        }

  void setMu(std::vector<float>&& eta,
	     std::vector<float>&& enemu0,
	     std::vector<float>&& enemu1,
	     std::vector<float>&& enemu2,
	     std::vector<unsigned int>&& qual,
             std::vector<unsigned int>&& val) 
        {
            m_eta = std::move(eta);
            m_enemu0 = std::move(enemu0);
            m_enemu1 = std::move(enemu1);
            m_enemu2 = std::move(enemu2);
            m_quality_factor = std::move(qual);
            m_val = std::move(val);
        }

  /** Set sumE vector in TileL2 */
  inline void setEt(std::vector<float>&& sumE) {m_sumE = std::move(sumE); }

  /* Access methods */

  /** Return Identifier */
  inline int identify(void) const { return m_ID; }

  /** Return number of data words */
  inline unsigned int Ndata() const { return m_val.size(); }

  /** Return Data */
  inline unsigned int val(unsigned int i) const { return m_val[i]; }
  inline const std::vector<unsigned int>& val() const { return m_val; }

  /** Return number of muons */
  inline unsigned int NMuons() const { return m_eta.size(); }

  /** Return eta (computed as the average of the eta values of the TileCal cells where the muon goes through) */
  inline float eta(unsigned int ind) const { return m_eta[ind]; }
  inline const std::vector<float>& eta() const { return m_eta; }

  /** Return phi (average value at the TileCal radius) */
  inline float phi(unsigned int /* ind=0 */) const { return m_phi; }

  /** Return cos(phi) */
  inline float cosphi() const { return m_cosphi; }

  /** Return sin(phi) */
  inline float sinphi() const { return m_sinphi; }

  /** Return energy deposited in innermost layer (A cells) */
  inline float enemu0(unsigned int ind) const { return m_enemu0[ind]; }
  inline const std::vector<float>& enemu0() const { return m_enemu0; }

  /** Return energy deposited in central layer (BC cells) */
  inline float enemu1(unsigned int ind) const { return m_enemu1[ind]; }
  inline const std::vector<float>& enemu1() const { return m_enemu1; }

  /** Return energy deposited in outermost layer (D cells) */
  inline float enemu2(unsigned int ind) const { return m_enemu2[ind]; }
  inline const std::vector<float>& enemu2() const { return m_enemu2; }

  /** Return quality flag (0 or 1):
   * set to 0 if the "energy deposition path" is MIP like in all three samples,
   * set to 1 if the "energy deposition path" is MIP like in two samples and a larger energy deposition is found in the remaining cell
   */
  inline unsigned int qual(unsigned int ind) const { return m_quality_factor[ind]; }
  inline const std::vector<unsigned int>& qual() const { return m_quality_factor; }

  /** Return number of sumE words */
  inline unsigned int NsumE() const { return m_sumE.size(); }

  /** Return Ex in a TileCal superdrawer (ROD-based) */
  inline float Ex() const { return (m_sumE.size()>0)?m_sumE[0]*m_cosphi:0.0; }

  /** Return Ey in a TileCal superdrawer (ROD-based) */
  inline float Ey() const { return (m_sumE.size()>0)?m_sumE[0]*m_sinphi:0.0; }

  /** Return Ez in a TileCal superdrawer (ROD-based) */
  inline float Ez() const { return (m_sumE.size()>1)?((m_ID&0x100)?m_sumE[1]:-m_sumE[1]):0.0; }

  /** Return total Et in a TileCal superdrawer (ROD-based) */
  inline float Et()    const { return (m_sumE.size()>0)?m_sumE[0]:0.0; }
  inline float sumEt() const { return (m_sumE.size()>0)?m_sumE[0]:0.0; }

  /** Return sumEz in a TileCal superdrawer (ROD-based) */
  inline float sumEz() const { return (m_sumE.size()>1)?m_sumE[1]:0.0; }

  /** Return sumE in a TileCal superdrawer (ROD-based) */
  inline float sumE()  const { return (m_sumE.size()>2)?m_sumE[2]:0.0; }

  /** Return sumE i-th element */
  inline float sumE(unsigned int ind) const { return m_sumE[ind]; }
  inline const std::vector<float>& sumEVec() const { return m_sumE; }

  /** Return identification */
  std::string whoami (void) const { return "TileL2"; }

  /** Printing for debugging */
  void print (void) const;

  /** Convertion operator to a std::string,
   * can be used in a cast operation : (std::string) TileL2 */
  operator std::string() const;

  /** Clear all vectors in TileL2 */
  void clear();
  inline void setzero() { clear(); }

private:
  friend class TileL2Cnv_p1;

  /** Drawer ID: 0x100-0x13F, 0x200-0x23F, 0x300-0x33F, 0x400-0x43F */
  int m_ID;

  /** function to set phi from ID **/
  void setphi();
    
  /** drawer phi */
  float m_phi;
  float m_cosphi;
  float m_sinphi;

  /** sumEt, sumEz, sumE energy per TileCal superdrawer */
  std::vector<float> m_sumE;

  /** Muon eta */
  std::vector<float> m_eta;

  /** Energy deposited by the muons in TileCal innermost layer */
  std::vector<float> m_enemu0;

  /** Energy deposited by the muons in TileCal central layer */
  std::vector<float> m_enemu1;

  /** Energy deposited by the muons in TileCal outermost layer */
  std::vector<float> m_enemu2;

  /** Quality flag for tight and loose muon selection */
  std::vector<unsigned int> m_quality_factor;

  /** 32-bit words (packed muon info) */
  std::vector<unsigned int> m_val;
};

#endif  //TILEEVENT_TILEL2_H
