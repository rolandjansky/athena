/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKHIT_H
#define FTKHIT_H


#include <Rtypes.h>

#include <vector>
#include <istream>
#include <ostream>
#include "MultiTruth.h"
#include "FTKRawHit.h"

/** class describing a generic coordinate */
class FTKCoord {
 private:
  int m_dim; // dimension
  
  float *m_coord; //[m_dim] coordinate

  /* store SCT and Pixel cluster positions as integers using the same FTK_IM HW definition */
  unsigned int m_hw_word;
  bool m_includesGangedHits;
  
 public:
  FTKCoord();
  FTKCoord(int); // construct passing the dimension
  FTKCoord(const FTKCoord&);
  FTKCoord& operator=(const FTKCoord&); // assignment operator
  virtual ~FTKCoord();
  
  void setDim(int);
  int getDim() const { return m_dim; };
  bool getIncludesGanged() const { return m_includesGangedHits; };
  void setIncludesGanged(bool b) { m_includesGangedHits = b; };
  
  float &operator[](int i) { return m_coord[i]; }
  float &operator[](int i) const { return m_coord[i]; }

  void setHwWord(unsigned int hw_word) { m_hw_word = hw_word; }
  unsigned int getHwWord() const { return m_hw_word; }
  void setHwCoord(int i, unsigned int v) {
    if (m_dim==1 && i==0)
      m_hw_word = (m_hw_word & ~FTKRawHit::strip_coord_mask) |
	( (v<<FTKRawHit::strip_coord_bit) & FTKRawHit::strip_coord_mask);
    if (m_dim==2 && i==0) 
      m_hw_word = (m_hw_word & ~FTKRawHit::row_coord_mask) |
	( (v<<FTKRawHit::row_coord_bit) & FTKRawHit::row_coord_mask);
    if (m_dim==2 && i==1) 
      m_hw_word = (m_hw_word & ~FTKRawHit::column_coord_mask) |
	( (v<<FTKRawHit::column_coord_bit) & FTKRawHit::column_coord_mask);
  }
  unsigned int getHwCoord(int i) const { 
    if (m_dim==1)
      return (m_hw_word & FTKRawHit::strip_coord_mask) >> FTKRawHit::strip_coord_bit; 
    else if (m_dim==2) {
      if (i==0) 
	return (m_hw_word & FTKRawHit::row_coord_mask) >> FTKRawHit::row_coord_bit;
      else if (i==1) 
	return (m_hw_word & FTKRawHit::column_coord_mask) >> FTKRawHit::column_coord_bit;
      else return -9999;
    } else return -9999;
  }

  friend bool operator==(const FTKCoord &left, const FTKCoord &right);
  friend std::ostream &operator<<(std::ostream&,const FTKCoord&);

  ClassDef(FTKCoord,3)
};

/** this class describe one hit as is used by the
    FTKSim structures */
class FTKHit {
protected:
  unsigned int m_IdentifierHash;
  int m_sector;
  int m_plane;
  int m_etaWidth; // clustering width along eta
  int m_phiWidth; // clustering width in phi direction
  int m_n_strips; // # of strips in a cluster for SCT; time-over-threshold for pixels
  int m_bankID; // bank ID for 8L banks
  bool m_ITkMode; // true for ITk sector mapping
  float m_scalingCoordToHWCoord[2]; // scaling of coordinate to FTK type
  FTKCoord m_coord;
  MultiTruth m_truth;
  std::vector<FTKHit> m_channels; // channels within the current

public:
  FTKHit(); // default constructor needed for ROOT
  FTKHit(const int); // base constructor needs the dimension
  FTKHit(const FTKHit&); // copy constructor
  FTKHit& operator=(const FTKHit&); // assignment operator
  virtual ~FTKHit();

  unsigned int getIdentifierHash() const { return m_IdentifierHash; }
  bool getIncludesGanged() const { 
    for (unsigned int i=0; i<getNChannels(); i++) 
      if (getChannel(i).getCoord().getIncludesGanged()) 
	return true;
    if (getNChannels()==0 && this->getCoord().getIncludesGanged()) 
      return true;
    return false;
  }

  float getScalingCoordToHWCoord(unsigned int i) {
    if (i < 2) return m_scalingCoordToHWCoord[i];
    else return -999;
  }
  void setScalingCoordToHWCoord(unsigned int i, float f) {
    if (i < 2) m_scalingCoordToHWCoord[i] = f;
  }

  int getSector() const { return  m_sector; }
  int getPlane() const { return m_plane; }
  int getEtaCode() const { return m_ITkMode ? (m_sector % 10000) : (m_sector % 1000); }
  int getIsBarrel() const { return m_ITkMode ? ( (m_sector % 100) / 10 == 2 ) : ( m_coord.getDim()!=3? (m_sector%1000 < 20 ? 1 : 0) : 0 ); }
  int getASide() const { return m_ITkMode ? ( (m_sector % 100) / 10 == 4 ) : ( m_coord.getDim()!=3? (m_sector%1000 < 20 ? 0 : (getEtaCode()/10)%2 == 0) : 0 ); }
  int getCSide() const { return m_ITkMode ? ( (m_sector % 100) / 10 == 0 ) : ( m_coord.getDim()!=3? (m_sector%1000 < 20 ? 0 : (getEtaCode()/10)%2 != 0) : 0 ); }
  int getEtaWidth() const { return m_etaWidth; }
  int getPhiWidth() const { return m_phiWidth; }
  int getEtaModule() const { return m_ITkMode ? ( ((m_sector % 10000) / 100) - 60 ) : ( m_sector%1000 < 20 ? m_sector%1000 : (m_sector%1000)/20 -1 ); }
  int getPhiModule() const { return m_ITkMode ? ( m_sector / 100000 ) : ( m_coord.getDim()!=3 ? m_sector/1000 : m_sector/10000 ); }
  int getSection() const { return m_ITkMode ? ( m_sector % 10 ) : ( m_coord.getDim()!=3 ? (m_sector%1000 < 20 ? 0 : m_sector%10) : 0 ); } // FIXME
  int getDim() const { return m_coord.getDim(); }
  FTKCoord getCoord() const { return m_coord; }
  FTKCoord &getCoord() { return m_coord; }
  float getCoord(int i) const { return m_coord[i]; }
  float getLocalCoord(unsigned int i) const;
  const MultiTruth& getTruth() const;

  void setIdentifierHash(unsigned int val) { m_IdentifierHash = val; }
  void setSector(int v) { m_sector = v; }
  void setPlane(int v) { m_plane = v; }
  void setEtaWidth(int v) { m_etaWidth = v; }
  void setPhiWidth(int v) { m_phiWidth = v; }
  void setDim(int v) { m_coord.setDim(v); }
  void setCoord(int i, float v) { m_coord[i] = v; }
  void setTruth(const MultiTruth&);

  // special accessors for SCTTRK case:
  // reuse existing variables to store 8L road and track info
  int getBankID() const { return m_bankID; }
  int getRoadID() const { return m_phiWidth; }
  int getTrackID() const { return m_etaWidth; }
  int getNStrips() const { return m_n_strips; }
  int getTot() const { return m_n_strips; }
  void setBankID(int v) { m_bankID = v; }
  void setRoadID(int v) { m_phiWidth = v; }
  void setTrackID(int v) { m_etaWidth = v; }
  void setNStrips(int v) { m_n_strips = v; }
  void setTot(int v) { m_n_strips = v; }

  // functions to toggle ITk sector mapping
  bool getITkMode() const { return m_ITkMode; }
  void setITkMode(bool v) { m_ITkMode = v; }
  
  // clustering debugging features
  void addChannel(const FTKHit &hit) { m_channels.push_back(hit); }
  const FTKHit& getChannel(unsigned int pos) const { return m_channels[pos]; }
  unsigned int getNChannels() const { return m_channels.size(); }

  float &operator[](int i) { return m_coord[i]; }
  float &operator[](int i) const { return m_coord[i]; }

  //  int getHwLocalCoord(int i) const;
  void setHwWord(unsigned int hw_word) { m_coord.setHwWord(hw_word); }
  unsigned int getHwWord() const { return m_coord.getHwWord(); }
  void setHwCoord(int i, unsigned int v) { m_coord.setHwCoord(i, v); }
  unsigned int getHwCoord(int i) const { return m_coord.getHwCoord(i); }



  friend bool operator==(const FTKHit &left, const FTKHit &right);
  friend std::ostream &operator<<(std::ostream&,const FTKHit&);

  const static int single_pixel_row_size  = 8; // one pixel is 8 units of 6.25um
  const static int single_pixel_col_size  = 16; // one nominal pixel is 16 units of 25um
  const static int single_IBL_col_size  = 10; // one nominal IBL pixel is 10 units of 25um

  ClassDef(FTKHit,8)
};
#endif // FTKHIT_H
