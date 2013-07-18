/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EMECCell_h
#define EMECCell_h 1
#include <iostream>
#include "LArReadoutGeometry/EMECDetDescr.h"
#include "LArReadoutGeometry/EMECDetectorManager.h"
#include "GeoModelKernel/CellBinning.h"
#include "GeoModelKernel/RCBase.h"
#include "LArHV/EMECPresamplerHVModuleConstLink.h"
#include "LArHV/EMECPresamplerHVModule.h"
#include "LArHV/EMECHVElectrodeConstLink.h"
#include "LArHV/EMECHVElectrode.h"
/**
 * @brief A Cell of the electromagnetic endcap calorimeter readout
 * geometry
 */

/**
 *	This class represents a single EMEC Cell.  These cells
 *	are specified by phi and eta boundaries, the latter
 *	which corresponds to an inner and outer radius computed
 *	from and effective focal length.  Those numbers come
 *	from the database via the EMEC DetectorManager
 *
 *	Use the EMEC Cells through EMEC Cell Links please!!!
 *	This will free the memory used by the cell when the last
 *	link to the cell disappears.
 */
class EMECCell : public RCBase  
{
  
 public:

  typedef enum {FRONT,CENTER,BACK} CELLPOS;
       
  /**
   * @brief	Constructor.
   */
  EMECCell (unsigned int endcap, const EMECDetDescr *emecDescriptor, unsigned int eta, unsigned int phi);
  
  /**
   * @brief	The Phi Index of this Cell.
   */
  unsigned int getPhiIndex () const;
  
  /**
   * @brief	The EndcapIndex of this Cell (0=negatiive, 1=positive)
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
   * @brief	The radial  index of this cell (0=Outer Wheel, 1=Inner Wheel).
   */
  unsigned int getRadialIndex () const;
  
  /**
   * @brief	Returns a pointer to the descriptor.
   */
  const EMECDetDescr * getDescriptor () const;
  
  /**
   * @brief	Z for the cell at (FRONT,CENTER,BACK)
   */
  double getZLocal (CELLPOS position) const;
  
  /**
   * @brief	RMin for the cell at (FRONT,CENTER,BACK)
   */
  double getRMinLocal (CELLPOS position) const;
  
  /**
   * @brief	RMax for the cell at (FRONT,CENTER,BACK)
   */
  double getRMaxLocal (CELLPOS position) const;
  
  /**
   * @brief	Gets the phi value at the "lower" edge of a cell.  This
   *	is a local phi!!! It's useful for constructing exact phi
   *	boundaries in the local coordinate system.
   */
  double getPhiLocalLower () const;
  
  /**
   * @brief	Gets the phi value at the "lower" edge of a cell.  This
   *	is a local phi!!! It's useful for constructing exact phi
   *	boundaries in the local coordinate system.
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
   * @brief	Minimum Eta
   */
  double getEtaMin () const;
  
  /**
   * @brief	Maximum Eta
   */
  double getEtaMax () const;


  /**
   * @Get num electrodes
   */
  unsigned int getNumElectrodes() const;
  
  /**
   * @Get electrode
   */
  const EMECHVElectrodeConstLink & getElectrode (unsigned int i) const;


  /**
   * @Get HVModule (presampler cells)
   */
  const EMECPresamplerHVModuleConstLink & getPresamplerHVModule () const;

 private:
  
  EMECCell(const EMECCell &right);

  virtual ~EMECCell();

  EMECCell & operator=(const EMECCell &right);
  //	Pointer to the Descriptor.  This class responds to its
  //	interface mostly by forwarding requests to the
  //	descriptor.

  const EMECDetDescr *m_emecDetDescr;

  mutable std::vector<EMECHVElectrodeConstLink> m_electrode;

  mutable EMECPresamplerHVModuleConstLink m_presamplerModule;

  unsigned int m_clockwork;


  friend class ImaginaryFriend;

  void initHV() const;
  
};




inline EMECCell::EMECCell (unsigned int endcap, const EMECDetDescr *emecDescriptor, unsigned int eta, unsigned int phi)
  // 8 bits are needed for phi index, 6 for eta, 1 for endcap.
 :m_emecDetDescr(emecDescriptor),m_clockwork(phi | (eta<<8) | (endcap <<15) )
{
  
}




inline unsigned int EMECCell::getPhiIndex () const
{
  
   return m_clockwork & 0x000000FF;
  
}

inline unsigned int EMECCell::getEndcapIndex () const
{
  
  return  (m_clockwork & 0x00008000) >> 15;
  
}

inline unsigned int EMECCell::getEtaIndex () const
{
  
  return  (m_clockwork & 0x00007F00) >> 8;
  
}

inline unsigned int EMECCell::getSamplingIndex () const
{
  
  return getDescriptor()->getSamplingIndex();
  
}

inline unsigned int EMECCell::getRegionIndex () const
{
  
  return getDescriptor()->getRegionIndex();
  
}

inline unsigned int EMECCell::getRadialIndex () const
{
  
  return getDescriptor()->getRadialIndex();
  
}

inline const EMECDetDescr * EMECCell::getDescriptor () const
{
  
  return m_emecDetDescr;
  
}

inline double EMECCell::getZLocal (CELLPOS position) const
{
  
  switch (position) {
  case FRONT:
    return getDescriptor()->getOffset(getEtaIndex())-getDescriptor()->getHalfLength(getEtaIndex());
  case CENTER:
    return getDescriptor()->getOffset(getEtaIndex());
  case BACK:
    return getDescriptor()->getOffset(getEtaIndex())+getDescriptor()->getHalfLength(getEtaIndex());
  default :
    return 0;
  };
  
}

inline double EMECCell::getRMinLocal (CELLPOS position) const
{
  
  double toActive = getDescriptor()->getSamplingIndex()==0 ? getDescriptor()->getManager()->getRefToPresampler() : getDescriptor()->getManager()->getRefToActive();
  return (getDescriptor()->getManager()->getFocalToRef()+toActive+getZLocal(position))/sinh(getEtaMax());
  
}

inline double EMECCell::getRMaxLocal (CELLPOS position) const
{
  
  double toActive = getDescriptor()->getSamplingIndex()==0 ? getDescriptor()->getManager()->getRefToPresampler() : getDescriptor()->getManager()->getRefToActive();
  return (getDescriptor()->getManager()->getFocalToRef()+toActive+getZLocal(position))/sinh(getEtaMin());
  
}

inline double EMECCell::getPhiLocalLower () const
{
  
  return getDescriptor()->getPhiBinning().binLower(getPhiIndex());
  
}

inline double EMECCell::getPhiLocalUpper () const
{
  
  return getDescriptor()->getPhiBinning().binUpper(getPhiIndex());
  
}


inline double EMECCell::getPhiMinNominal () const
{
  return getEndcapIndex()==0 ? M_PI-getPhiLocalLower() : getPhiLocalLower();
}

inline double EMECCell::getPhiMaxNominal () const
{
  return getEndcapIndex()==0 ? M_PI-getPhiLocalUpper() : getPhiLocalUpper();
}

inline double EMECCell::getEtaMin () const
{
  
  return getDescriptor()->getEtaBinning().binLower(getEtaIndex()); 
  
}

inline double EMECCell::getEtaMax () const
{
  
  return getDescriptor()->getEtaBinning().binUpper(getEtaIndex());
  
}


#endif
