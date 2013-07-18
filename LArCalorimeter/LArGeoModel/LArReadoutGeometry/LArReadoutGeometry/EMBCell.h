/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EMBCell_h
#define EMBCell_h 1

#include "LArReadoutGeometry/EMBDetDescr.h"
#include "LArReadoutGeometry/EMBHVPathologies.h"
#include "LArHV/EMBPresamplerHVModuleConstLink.h"
#include "LArHV/EMBPresamplerHVModule.h"
#include "LArHV/EMBHVElectrodeConstLink.h"
#include "LArHV/EMBHVElectrode.h"
#include <cmath>
#include "GeoModelKernel/RCBase.h"
#include <vector>
/**
 * @brief A cell of the electromagnetic barrel calorimeter readout
 * geometry
 */

/** 
 *  This class gives dimensions and positions, and indices of an 
 *  electromagnetic barrel calorimeter readout cell.  Positions are
 *  in local coordinates (local to a "big piece", or in this case the
 *  EMB half barrel).  Use the EMB Cells through EMB Cell Links please!!!
 *  This will free the memory used by the cell when the last
 *  link to the cell disappears.
 */

class EMBCell : public RCBase  
{
 public:
  typedef enum {FRONT,CENTER,BACK} CELLPOS;

  public:
  /**
   * @brief       Constructor.
   */
      EMBCell (unsigned int side, const EMBDetDescr *embDescriptor, unsigned int eta, unsigned int phi);


      /**
       * @brief	The Phi Index of this Cell.
       */
      unsigned int getPhiIndex () const;

      /**
       * @brief	The Side Index of this Cell (0=negatiive, 1=positive)
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
       * @brief	Returns a pointer to the descriptor.
       */
      const EMBDetDescr * getDescriptor () const;

      /**
       * @brief	Z for the cell at (FRONT,CENTER,BACK)
       */
      double getRLocal (CELLPOS position) const;

      /**
       * @brief	RMin for the cell at (FRONT,CENTER,BACK)
       */
      double getZMinLocal (CELLPOS position) const;

      /**
       * @brief	RMax for the cell at (FRONT,CENTER,BACK)
       */
      double getZMaxLocal (CELLPOS position) const;

      /**
       * @brief	Gets the phi value at the "lower" edge of a cell.  This
       *	is a local phi!!! It's useful for constructing exact phi
       *	boundaries in the local coordinate system.
       */
      double getPhiLocalLower () const;

      /**
       * @brief	Gets the phi value at the "upper" edge of a cell.  This
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
       * @brief Overloaded.  Returns the local lower cell
       * boundaries in phi but they take account of the accordion shape, which
       * undulates in phi, in the barrel.
       */
      double getPhiLocalLower (double r) const;
      /** 
       * @brief Overloaded.  Returns the local upper cell
       * boundaries in phi but they take account of the accordion shape, which
       * undulates in phi, in the barrel.
       */
      double getPhiLocalUpper (double r) const;

      /**
       * @Get num electrodes
       */
      unsigned int getNumElectrodes() const;

      /**
       * @Get electrode
       */
      const EMBHVElectrodeConstLink & getElectrode (unsigned int i) const;

      /**
       * @Get num HVPathologies
       */
      unsigned int getNumHVPathologies ()const ;

      /**
       * @Get HVPathologies
       */
      const EMBHVPathologiesConstLink & getHVPathologies (unsigned int i) const;

      /**
       * @Get HVModule (presampler cells)
       */
      const EMBPresamplerHVModuleConstLink & getPresamplerHVModule () const;


      /**
       * @Add HVPathologies 
       */
      void addHVPathologies (EMBHVPathologiesConstLink);


  private:

      EMBCell(const EMBCell &right);

      virtual ~EMBCell();

      EMBCell & operator=(const EMBCell &right);

      const EMBDetDescr *m_embDetDescr;

      mutable std::vector<EMBHVElectrodeConstLink> m_electrode;

      mutable EMBPresamplerHVModuleConstLink m_presamplerModule;

      mutable std::vector<EMBHVPathologiesConstLink> m_hvPathologies;

      unsigned int m_clockwork;

      friend class ImaginaryFriend;

      void initHV() const;
};


// Class EMBCell 

inline EMBCell::EMBCell (unsigned int side, const EMBDetDescr *embDescriptor, unsigned int eta, unsigned int phi)
  :m_embDetDescr(embDescriptor),m_clockwork(phi | (eta<<8) | (side <<17) )
{
}



inline unsigned int EMBCell::getPhiIndex () const
{
  return m_clockwork & 0x000000FF;
}

inline unsigned int EMBCell::getEndcapIndex () const
{
  return  (m_clockwork & 0x00020000) >> 17;
}

inline unsigned int EMBCell::getEtaIndex () const
{
  return  (m_clockwork & 0x0001FF00) >> 8;
}

inline unsigned int EMBCell::getSamplingIndex () const
{
  return getDescriptor()->getSamplingIndex();
}

inline unsigned int EMBCell::getRegionIndex () const
{
  return getDescriptor()->getRegionIndex();
}

inline const EMBDetDescr * EMBCell::getDescriptor () const
{
  return m_embDetDescr;
}

inline double EMBCell::getRLocal (CELLPOS position) const
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

inline double EMBCell::getZMinLocal (CELLPOS position) const
{
  return getRLocal(position)*sinh(getEtaMin());
}

inline double EMBCell::getZMaxLocal (CELLPOS position) const
{
  return getRLocal(position)*sinh(getEtaMax());
}

inline double EMBCell::getPhiLocalLower () const
{
  return getDescriptor()->getPhiBinning().binLower(getPhiIndex());
}

inline double EMBCell::getPhiLocalUpper () const
{
  return getDescriptor()->getPhiBinning().binUpper(getPhiIndex());
}

inline double EMBCell::getPhiMinNominal () const
{
  return getEndcapIndex()==0 ? M_PI-getPhiLocalLower() : getPhiLocalLower();
}

inline double EMBCell::getPhiMaxNominal () const
{
  return getEndcapIndex()==0 ? M_PI-getPhiLocalUpper() : getPhiLocalUpper();
}

inline double EMBCell::getEtaMin () const
{
  return getDescriptor()->getEtaBinning().binLower(getEtaIndex());
}

inline double EMBCell::getEtaMax () const
{
  return getDescriptor()->getEtaBinning().binUpper(getEtaIndex());
}




#endif
