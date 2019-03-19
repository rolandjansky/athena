/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKRAWHIT_H
#define FTKRAWHIT_H

#include <TObject.h>

#include <vector>
#include <iostream>
#include <sstream>
#include "FTKPMap.h"


class FTKTrack;
class FTKHit;
class MultiTruth;

/**
 * The FTKRawHit represent the lowest order representation of the ID
 * hit in the FTK simulation, this originally designed to read data
 * from ASCII files produced with the wrapper.
 * The object of this class are used for clustering or other manipulations
 * but before road finding and track fitting. From the road finding
 * the hits will be moved using the FTKHit format.
 */
class FTKRawHit : public TObject {
 private:
  float m_x;
  float m_y;
  float m_z;  /** Hit position in global coordinates */
  int m_hitType;     /** Flag to know if the module belongs to Pixel or SCT or SCTtrk */
  int m_moduleType;  /** Flag that indicates the module type */
  unsigned int m_IdentifierHash; // Athena identifier hash
  int m_barrel_ec;   /** and if it's on barrel or endcaps, default values below */
  int m_layer_disk;  /** Layer or disk number */
  int m_phi_module;  /** Module id along phi */
  int m_eta_module;  /** Module id along eta */
  int m_pi_side;     /** phi index for pixel, side for SCT */
  int m_ei_strip;    /** eta index for pixel, strip for SCT */
  int m_n_strips;    /** # of strips in a cluster for SCT; time-over-threshold for pixels */
  int m_etaWidth;
  int m_phiWidth; // width of cluster along the 2 directions
  float m_dPhi;
  float m_dEta; /** float correction for distance from center */

  /** store SCT and Pixel cluster positions as integers using the same FTK_IM HW definition */
  unsigned int m_hw_word;

  bool m_includesGangedHits; /** cluster includes one or more ganged pixel hits */

  signed long m_eventindex; /** athena event index assigned to this channel */
  signed long m_barcode; /** geant particle barcode assigned to this channel */
  float m_barcode_pt; /** maximum 'pt' for any 'good'
                         geant particle contributing to the
                         channel. corresponds to the particle with
                         m_barcode */
  unsigned long m_parentage_mask; /** ancestor information of this channel */
  /** geant truth data (clusters only). filled
     during clustering using the m_barcode and
     m_barcode_frac data for each raw hit in the
     cluster. */
  MultiTruth* m_truth;  //!

  std::vector<FTKRawHit> m_channels; //! single channels that compose a cluster, this enabled only for debugging reason

public:
  FTKRawHit();
  FTKRawHit(const FTKTrack*, int);
  FTKRawHit(const FTKRawHit &);
  ~FTKRawHit();

  // shallow assignment operator - did not used to copy MultiTruth pointer, but does now!
  FTKRawHit& operator=(const FTKRawHit&);

  void reset();

  void addX(float v) { m_x += v; }
  void addY(float v) { m_y += v; }
  void addZ(float v) { m_z += v; }
  void addPhiSide(int v) { m_pi_side += v; }
  void addEtaStrip(int v) { m_ei_strip += v; }
  void addNStrips(int v) { m_n_strips += v; }
  void addTot(int v) { m_n_strips += v; }

  void divX(float v) { m_x /= v; }
  void divY(float v) { m_y /= v; }
  void divZ(float v) { m_z /= v; }

  void setHitType(int v) { m_hitType = v; }
  void setModuleType(int v) { m_moduleType = v; }

  void setIdentifierHash(unsigned int val) { m_IdentifierHash = val; }
  void setBarrelEC(int v) { m_barrel_ec = v; }
  void setLayer(int v) { m_layer_disk = v; }
  void setPhiModule(int v) { m_phi_module = v; }
  void setEtaModule(int v) { m_eta_module = v; }
  void setPhiSide(int v) { m_pi_side = v; }
  void setEtaStrip(int v) { m_ei_strip = v; }
  void setNStrips(int v) { m_n_strips = v; }
  void setTot(int v) { m_n_strips = v; }

  /*!
   * Function setting the column width in m_hw_word for pixel 
   * @param v the new column width
   * @return void
   */
  void setEtaWidth(int v) {
      if (v <8) m_hw_word = (m_hw_word&~eta_width_mask) | (((v-1) << eta_width_bit)&eta_width_mask);
      else if (v >= 8) m_hw_word = (m_hw_word&~eta_width_mask) | ((7 << eta_width_bit)&eta_width_mask);
      m_etaWidth = v;
  }

  /*!
   * Function setting the row width in m_hw_word for pixel 
   * @param v the new row width
   * @return void
   */
  void setPhiWidth(int v) {
      int mask(~0);
      if (m_hitType == 0) mask = strip_width_mask;
      else if (m_hitType == 1) mask = phi_width_mask;

      if (v < 8) m_hw_word = (m_hw_word&~mask) | (((v-1) << phi_width_bit) & mask);
      else m_hw_word = (m_hw_word&~mask) | ((7 << phi_width_bit) & mask);
      m_phiWidth = v;
  }
    
  /*!
   * Function setting the split cluster flag in m_hw_word
   * @param b split cluster flag
   * @return void
   */
  void setSplit(bool b) {
      if (b) m_hw_word = (m_hw_word&~split_mask) | ((1 << split_bit)&split_mask);
      else   m_hw_word = (m_hw_word&~split_mask) | ((0 << split_bit)&split_mask);
  }

  /*!
   * Function setting the row coordinate in m_hw_word for pixel 
   * @param v the new row coordinate
   * @return void
   */
  void setRowCoordinate(int v) { m_hw_word = (m_hw_word&~row_coord_mask) | ((v << row_coord_bit)&row_coord_mask); }

  /*!
   * Function setting the column coordinate in m_hw_word for pixel 
   * @param v the new column coordinate
   * @return void
   */
  void setColumnCoordinate(int v) { m_hw_word = (m_hw_word&~column_coord_mask) | ((v << column_coord_bit)&column_coord_mask); }

  /*!
   * Function setting the column coordinate in m_hw_word for pixel 
   * @param v the new column coordinate
   * @return void
   */
  void setStripCoordinate(int v) { m_hw_word = (m_hw_word &~ strip_coord_mask) | ((v << 0) & strip_coord_mask); }
  void setX(float v) { m_x = v; }
  void setY(float v) { m_y = v; }
  void setZ(float v) { m_z = v; }
  void setDeltaPhi(float v) { m_dPhi = v; }
  void setDeltaEta(float v) { m_dEta = v; }
  void setIncludesGanged(bool b) { m_includesGangedHits = b; }
  void setTruth(const MultiTruth&); // truth for clusters

  int getHitType() const { return m_hitType; }
  int getModuleType() const { return m_moduleType; }

  int getIsPixel() const { return m_hitType==ftk::PIXEL; }
  int getIsSCT() const { return m_hitType==ftk::SCT; }
  int getIsSCTtrk() const { return m_hitType==ftk::SCTtrk; }
  unsigned int getIdentifierHash() const { return m_IdentifierHash; }
  int getBarrelEC() const { return m_barrel_ec; }
  int getLayer() const { return m_layer_disk; }
  int getPhiModule() const { return m_phi_module; }
  int getEtaModule() const { return m_eta_module; }
  int getPhiSide() const { return m_pi_side; }
  int getEtaStrip() const { return m_ei_strip; }
  int getNStrips() const { return m_n_strips; }
  int getTot() const { return m_n_strips; }
  int getEtaWidth() const { return m_etaWidth; }
  int getPhiWidth() const { return m_phiWidth; }
  int getRowCoordinate() const { return (m_hw_word & row_coord_mask) >> row_coord_bit; }
  int getStripCoordinate() const { return (m_hw_word & strip_coord_mask) >> strip_coord_bit; }
  int getColumnCoordinate()   const { return (m_hw_word & column_coord_mask) >> column_coord_bit; }
  bool getSplit() const { return (m_hw_word & split_mask) >> split_bit; }
  unsigned int getHWWord() const { return m_hw_word; }
  float getX() const { return m_x; }
  float getY() const { return m_y; }
  float getZ() const { return m_z; }
  float getDeltaPhi() const { return m_dPhi; }
  float getDeltaEta() const { return m_dEta; }
  bool getIncludesGanged() const { return m_includesGangedHits; }
  const signed long& getBarcode() const { return m_barcode; }
  void setBarcode(signed long v) { m_barcode = v; }
  const signed long& getEventIndex() const { return m_eventindex; }
  void setEventIndex(signed long v) { m_eventindex = v; }
  const float& getBarcodePt() const { return m_barcode_pt; }
  void setBarcodePt(float v) { m_barcode_pt = v; }
  const unsigned long& getParentageMask() const { return m_parentage_mask; }
  void setParentageMask(unsigned long v) { m_parentage_mask = v; }
  const MultiTruth* getTruth() const { return m_truth; } // truth for clusters only
  void resetTruthPointer() { m_truth=0; } // careful - MultiTruth memory managed elsewhere

  void normalizeLayerID(); // set the layer id following the FTK convention

  FTKHit getFTKHit(const FTKPlaneMap *) const;

  // special accessors for SCTTRK case:
  // reuse existing variables to store 8L road and track info
  int getBankID() const { return m_n_strips; }
  int getRoadID() const { return m_phiWidth; }
  int getTrackID() const { return m_etaWidth; }
  void setBankID(int v) { m_n_strips = v; }
  void setRoadID(int v) { m_phiWidth = v; }
  void setTrackID(int v) { m_etaWidth = v; }

  void addChannel(const FTKRawHit &hit) { m_channels.push_back(hit); }
  unsigned int getNChannels() const { return m_channels.size(); }
  const FTKRawHit& getChannel(unsigned int pos) const { return m_channels[pos]; }
  const std::vector<FTKRawHit>& getChannels() const { return m_channels; }

  friend std::istream& operator>>(std::istream &,FTKRawHit&);
  friend std::ostream& operator<<(std::ostream &,const FTKRawHit&);

  friend std::istream& clusterP( std::istream & , FTKRawHit& );
  friend std::istream& clusterC( std::istream & , FTKRawHit& );

  // These are now public because they are needed by FTKHit and FTKCoord
  static const int row_coord_mask    = 0x00000FFF; ///< Bitmask marking the location of the row coordinate in m_hw_word for pixel(12 bits)
  static const int phi_width_mask    = 0x00007000; ///< Bitmask marking the location of the row width in m_hw_word for pixel(3 bits)
  static const int split_mask        = 0x00008000; ///< Bitmask marking the location of the split cluster flag in m_hw_word for pixel(1 bit)
  static const int column_coord_mask = 0x0FFF0000; ///< Bitmask marking the location of the column coordinate in m_hw_word for pixel(12 bits)
  static const int eta_width_mask    = 0x70000000; ///< Bitmask marking the location of the column width in m_hw_word for pixel(3 bits)

  static const int row_coord_bit     = 0; ///< Starting bit of row coordinate in m_hw_word for pixel
  static const int phi_width_bit     = 12; ///< Starting bit of row width in m_hw_word for pixel
  static const int split_bit         = 15; ///< Bit for the split cluster flag in m_hw_word for pixel
  static const int column_coord_bit  = 16; ///<  Starting bit of column coordinate in m_hw_word for pixel
  static const int eta_width_bit     = 28; ///< Starting bit of column width in m_hw_word for pixel

  static const int strip_coord_mask = 0x07FF; ///< Bitmask marking the location of the strip coordinate in m_hw_word for SCT (3 bits)
  static const int strip_coord_bit  = 0; ///< Starting bit of strip coordinate in m_hw_word for SCT
  static const int strip_width_mask = 0x7000; ///< Bitmask marking the location of the strip width in m_hw_word  for SCT (3 bits)
  static const int strip_width_bit  = 12; ///< Starting bit of strip width in m_hw_word for SCT

  const float SCT_row_scaling = 2.;
  const float PIX_row_scaling = 8.;
  const float PIX_column_scaling = 
    (16*ftk::lengthOfPixelModuleIn400umPixels)/ftk::numberOfEtaPixelsInPixelModule; // (16*19)/18);
  const float IBL_planar_row_scaling = 8.;
  const float IBL_planar_column_scaling =
    (10*ftk::lengthOfIblModuleIn250umPixels)/ftk::numberOfEtaPixelsInIblModule; // (10*163.2)/160);
  const float IBL_3D_row_scaling = 8.;
  const float IBL_3D_column_scaling =
    (10*ftk::lengthOfIblModuleIn250umPixels)/ftk::numberOfEtaPixelsInIblModule; // (10*163.2)/160);


  ClassDef(FTKRawHit,4)
};

#endif // FTKRAWHIT_H
