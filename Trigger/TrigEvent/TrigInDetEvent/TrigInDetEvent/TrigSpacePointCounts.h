/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINDETEVENT_SPACEPOINTCOUNTER_H
#define TRIGINDETEVENT_SPACEPOINTCOUNTER_H

#include "TrigInDetEvent/TrigHisto2D.h"
#include "Identifier/Identifier.h"

#include "AthenaKernel/CLASS_DEF.h"

/** @class TrigSpacePointCounts
 * @author Regina Kwee <regina.kwee@cern.ch>
 * 
 * A class to store Pixel cluster and SCT space point information for
 * the level 2 minbias trigger.
 *
 */
class TrigSpacePointCounts {
 public:
  
  /** Default constructor used by T/P converters. */
  TrigSpacePointCounts();

  /** Standard constructor used by FEX algorithms. */ 
  TrigSpacePointCounts(const TrigHisto2D& pixelClusEndcapC,
		       const TrigHisto2D& pixelClusBarrel,
		       const TrigHisto2D& pixelClusEndcapA,
		       const std::vector<Identifier>& droppedPixelModules,
		       unsigned int sctSpEndcapC,
		       unsigned int sctSpBarrel,
		       unsigned int sctSpEndcapA,
		       const std::vector<Identifier>& droppedSctModules);
  
  /** Constructor passing arguments by move. */
  TrigSpacePointCounts(TrigHisto2D&& pixelClusEndcapC,
		       TrigHisto2D&& pixelClusBarrel,
		       TrigHisto2D&& pixelClusEndcapA,
		       std::vector<Identifier>&& droppedPixelModules,
		       unsigned int sctSpEndcapC,
		       unsigned int sctSpBarrel,
		       unsigned int sctSpEndcapA,
		       std::vector<Identifier>&& droppedSctModules);
  
  /** Copy Constructor */
  TrigSpacePointCounts(const TrigSpacePointCounts& trigSpacePointCounts);

  /** Destructor */
  ~TrigSpacePointCounts(void);

  const TrigHisto2D& pixelClusEndcapC(void) const;

  const TrigHisto2D& pixelClusBarrel(void) const;

  const TrigHisto2D& pixelClusEndcapA(void) const;

  const std::vector<Identifier>& droppedPixelModules(void) const;

  unsigned int sctSpEndcapC(void) const;

  unsigned int sctSpBarrel(void) const;
 
  unsigned int sctSpEndcapA(void) const;

  const std::vector<Identifier>& droppedSctModules(void) const;

 private:
  
  /** A histogram of pixel cluster time over threshold against pixel
      cluster size for endcap C. */
  TrigHisto2D m_pixelClusEndcapC;
  
  /** A histogram of pixel cluster time over threshold against pixel
      cluster size for the barrel. */
  TrigHisto2D m_pixelClusBarrel;
  
  /** A histogram of pixel cluster time over threshold against pixel
      cluster size for endcap A. */
  TrigHisto2D m_pixelClusEndcapA;

  /** A vector of module identifiers dropped from the pixel cluster sum. */
  std::vector<Identifier> m_droppedPixelModules;

  /** The space point multiplicity for space points in the SCT endcap C */
  unsigned int m_sctSpEndcapC;

  /** The space point multiplicity for space points in the SCT barrel */
  unsigned int m_sctSpBarrel;

  /** The space point multiplicity for space points in the SCT endcap A */
  unsigned int m_sctSpEndcapA;

  /** A vector of modules identifiers dropped from the SCT space point sum. */
  std::vector<Identifier> m_droppedSctModules;
};
 
// obtained using clid -m TrigSpacePointCounts
CLASS_DEF( TrigSpacePointCounts , 180429846 , 1 )


#endif 
