/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HECCell_h
#define HECCell_h 1
#include "LArReadoutGeometry/HECDetDescr.h"
#include "GeoModelKernel/CellBinning.h"
#include "GeoModelKernel/RCBase.h"
#include "LArHV/HECHVModule.h"
#include "LArHV/HECHVSubgap.h"
/**
 * @brief A cell of the hadronic endcap calorimeter readout
 * geometry
 */

/**
 *	This class represents a single HEC Cell.  A cell spans
 *	multiple blocks and has different boundaries at each
 *	block.  For access to the HEC  Cells, use the HECDetector
 *	Region class.
 *
 *	Use the HEC Cells through HEC Cell Links please!!!  This
 *	will free the memory used by the cell when the last link
 *	to the cell disappears.
 */


class HECCell : public RCBase  
{
  
 public:

  typedef enum {FRONT,CENTER,BACK} CELLPOS;
  

  /**
   * @brief Constructor      
   */
  HECCell (unsigned int endcap, const HECDetDescr *hecDescriptor, unsigned int eta, unsigned int phi);
      
  /**
   * @brief	The Phi Index of this Cell.
   */
  unsigned int getPhiIndex () const;
      
  /**
   * @brief	The EndcapIndex of this Cell (0=negative, 1=positive)
   */
  unsigned int getEndcapIndex () const;
      
  /**
   * @brief	The eta index of this cell.
   */
  unsigned int getEtaIndex () const;
      
  /**
   * @brief	The sampling index of this cell.
   */
  unsigned int getSamplingIndex () const;
      
  /**
   * @brief	The region index of this cell.
   */
  unsigned int getRegionIndex () const;
      
  /**
   * @brief	The number of blocks in this cell.
   */
  unsigned int getNumBlocks () const;
      
  /**
   * @brief	The block index of cell i.
   *
   *	In more detail:  there are 7 longitudinal blocks in the
   *	full HEC.  This cell contains (n=numBlocks()) of them.
   *	The block index of cell i tells you which block you are
   *	in within the full HEC.
   */
  unsigned int getBlockIndex (unsigned int block) const;

  /**
   * @brief	Minimum Z of Block i.
   */
  double getZMin (unsigned int block) const;
      
  /**
   * @brief	Maximum Z of block i.
   */
  double getZMax (unsigned int block) const;
      
  /**
   * @brief	Gets the phi value at the "lower" edge of a cell.  This
   *	is a local phi!!! It's useful for constructing exact phi
   *	boundaries in the local coordinate system.
   */
  double getPhiLocalLower () const;
      
  /**
   * @brief	Gets the phi value at the "upper" edge of the cell.
   *	This is a local phi!!! It's useful for constructing
   *	exact phi boundaries in the local coordinate system.
   */
  double getPhiLocalUpper () const;
      
  /**
   * @brief returns a nominal phi min in global coords; assume canonical placement
   **/
  double getPhiMinNominal() const;
  
  /**
   * @brief returns a nominal phi max in global coords; assume canonical placement
   **/
  double getPhiMaxNominal() const;

  /**
   * @brief	Nominal Eta Min (geometry is not projective)
   */
  double getEtaMinNominal () const;

  /**
   * @brief	Nominal Eta Min (geometry is not projective)
   */
  double getEtaMaxNominal () const;

  /**
   * @brief	Z for the cell at (FRONT,CENTER,BACK)
   */
  double getZLocal (CELLPOS position) const;
      
  /**
   * @brief	Nominal RMin for the cell at (FRONT,CENTER,BACK)
   */
  double getRMinLocalNominal (CELLPOS position) const;
      
  /**
   * @brief	Nominal RMax for the cell at (FRONT,CENTER,BACK)
   */
  double getRMaxLocalNominal (CELLPOS position) const;
      
  /**
   * @brief	Minimum r of block i.
   */
  double getRMin (unsigned int block) const;
      
  /**
   * @brief	Maximum r of block i.
   */
  double getRMax (unsigned int block) const;
      
  /**
   * @brief	Returns a pointer to the descriptor.
   */
  const HECDetDescr * getDescriptor () const;
      
  /**
   * @brief	Returns true if the cell occupies (or overlaps) this
   *	longitudinal block.
   */
  bool occupiesBlock (unsigned int block) const;

  /**
   * @Get num subgaps
   */
  unsigned int getNumSubgaps() const;
  
  /**
   * @Get subgap
   */
  const HECHVSubgapConstLink & getSubgap (unsigned int i) const;
      


 private:
    
  HECCell(const HECCell &right);

  virtual ~HECCell();

  HECCell & operator=(const HECCell &right);
      
  /**
   * @brief	Pointer to the Descriptor.  This class responds to its
   *	interface mostly by forwarding requests to the
   *	descriptor.
   */
  const HECDetDescr* m_hecDetDescr;
      
  /**
   * @brief	Bitpacked indices.
   */
  unsigned int m_clockwork;

  /**
   * @brief	Cache of subgaps.
   */
  mutable std::vector<HECHVSubgapConstLink> m_subgap;

  /**
   * @brief Additional Implementation Declarations
   */
  friend class ImaginaryFriend;

  void initHV() const;

};





inline HECCell::HECCell (unsigned int endcap, const HECDetDescr *hecDescriptor, unsigned int eta, unsigned int phi)
  
  
  
  :m_hecDetDescr(hecDescriptor),m_clockwork(phi | (eta<<6) | (endcap <<10))
  
{
  
  
}




inline unsigned int HECCell::getPhiIndex () const
{
  
  return m_clockwork & 0x0000003F;
  
}

inline unsigned int HECCell::getEndcapIndex () const
{
  
  return (m_clockwork & 0x00000400) >> 10;
  
}

inline unsigned int HECCell::getEtaIndex () const
{
  
  // 4 Bits for eta:
  return (m_clockwork & 0x000003C0) >> 6;
  
}

inline unsigned int HECCell::getSamplingIndex () const
{
  
  return getDescriptor()->getSamplingIndex();
  
}

inline unsigned int HECCell::getRegionIndex () const
{
  
  return getDescriptor()->getRegionIndex();
  
}

inline unsigned int HECCell::getNumBlocks () const
{
  
  return getDescriptor()->getNumBlocks();
  
}

inline unsigned int HECCell::getBlockIndex (unsigned int block) const
{
  
  return getDescriptor()->getBlockIndex(block);
  
}

inline double HECCell::getZMin (unsigned int block) const
{
  
  return getDescriptor()->getZMin(block);
  
}

inline double HECCell::getZMax (unsigned int block) const
{
  
  return getDescriptor()->getZMax(block);
  
}

inline double HECCell::getPhiLocalLower () const
{
  
  return getDescriptor()->getPhiBinning().binLower(getPhiIndex());
  
}

inline double HECCell::getPhiLocalUpper () const
{
  
  return getDescriptor()->getPhiBinning().binUpper(getPhiIndex());
  
}

inline double HECCell::getEtaMinNominal () const
{
  
  return getDescriptor()->getEtaBinning().binLower(getEtaIndex());
  
}

inline double HECCell::getEtaMaxNominal () const
{
  
  return getDescriptor()->getEtaBinning().binUpper(getEtaIndex());
  
}

inline double HECCell::getZLocal (CELLPOS position) const
{
  
  switch (position) {
  case FRONT:
    return getZMin(0);
  case CENTER:
    return (getZLocal(FRONT) + getZLocal(BACK))/2.0;
  case BACK:
    return getZMax(getNumBlocks()-1);
  default :
    return 0;
  };
  
}

inline double HECCell::getRMinLocalNominal (CELLPOS position) const
{
  bool hec2 = getSamplingIndex() > 1;
  if (hec2) {
    return (getDescriptor()->getManager()->getFocalToRef2() + getZLocal(position))/sinh(getEtaMaxNominal());
  }
  else {
    return (getDescriptor()->getManager()->getFocalToRef1() + getZLocal(position))/sinh(getEtaMaxNominal());
  }

}

inline double HECCell::getRMaxLocalNominal (CELLPOS position) const
{
  bool hec2 = getSamplingIndex() > 1;
  if (hec2) {
    return (getDescriptor()->getManager()->getFocalToRef2() + getZLocal(position))/sinh(getEtaMinNominal());
  }
  else {
    return (getDescriptor()->getManager()->getFocalToRef1() + getZLocal(position))/sinh(getEtaMinNominal());
  }
      
}

inline double HECCell::getRMin (unsigned int block) const
{
  
  return getDescriptor()->getRMin(block,getEtaIndex());
  
}

inline double HECCell::getRMax (unsigned int block) const
{
  
  return getDescriptor()->getRMax(block,getEtaIndex());
  
}

inline const HECDetDescr * HECCell::getDescriptor () const
{
  
  return m_hecDetDescr;
  
}

inline bool HECCell::occupiesBlock (unsigned int block) const
{
  
  return getDescriptor()->getRadialSegment(block,getEtaIndex())!=NULL;
  
}


inline double HECCell::getPhiMinNominal () const
{
  return getEndcapIndex()==0 ? M_PI-getPhiLocalUpper() : getPhiLocalLower();
}

inline double HECCell::getPhiMaxNominal () const
{
  return getEndcapIndex()==0 ? M_PI-getPhiLocalLower() : getPhiLocalUpper();
}



#endif
