#ifndef CALOREC_CALOCELL_SIMPLEDETECTORINFO_H
#define CALOREC_CALOCELL_SIMPLEDETECTORINFO_H

/********************************************************************

NAME     :  CaloCell_SimpleDetectorInfo.h
AUTHORS  :  J.Tanaka <Junichi.Tanaka@cern.ch>
CREATED  :  23th April 2004
PURPOSE  :  handle simple detector information of cell.
            See Calorimeter/CaloRec/CaloRec-xx-yy-zz/src/CBNT_CaloCell.cxx

MODIFIED :
To use it in root:
root[0] #include "CaloCell_SimpleDetectorInfo.h"
root[1] CaloCell_SimpleDetectorInfo detInfo(1)
root[2] detInfo.isLArEM()

********************************************************************/

const unsigned CALOCELL_DETINFO_Atlas_Lar_EM   = (1<<0);
const unsigned CALOCELL_DETINFO_Atlas_Lar_HEC  = (1<<1);
const unsigned CALOCELL_DETINFO_Atlas_Lar_FCal = (1<<2);
const unsigned CALOCELL_DETINFO_Atlas_Tile     = (1<<3);
  
const unsigned CALOCELL_DETINFO_EM_sampling     = (1<<4)|(1<<5);
const unsigned CALOCELL_DETINFO_EM_sampling_SHIFT = 4;
const unsigned CALOCELL_DETINFO_EM_barrel       = (1<<6);
const unsigned CALOCELL_DETINFO_EM_endcap_outer = (1<<7);
const unsigned CALOCELL_DETINFO_EM_endcap_inner = (1<<8);

const unsigned CALOCELL_DETINFO_HEC_sampling    = (1<<9)|(1<<10);
const unsigned CALOCELL_DETINFO_HEC_sampling_SHIFT = 9;

const unsigned CALOCELL_DETINFO_FCal_sampling    = (1<<11)|(1<<12);
const unsigned CALOCELL_DETINFO_FCal_sampling_SHIFT = 11;
  
const unsigned CALOCELL_DETINFO_Tile_sampling  = (1<<13)|(1<<14)|(1<<15);
const unsigned CALOCELL_DETINFO_Tile_sampling_SHIFT = 13;
const unsigned CALOCELL_DETINFO_Tile_barrel    = (1<<16);
const unsigned CALOCELL_DETINFO_Tile_extbarrel = (1<<17);
const unsigned CALOCELL_DETINFO_Tile_gap       = (1<<18);
const unsigned CALOCELL_DETINFO_Tile_gapscin   = (1<<19);

class CaloCell_SimpleDetectorInfo
{
public:
  CaloCell_SimpleDetectorInfo() : m_det(0) {};
  virtual ~CaloCell_SimpleDetectorInfo() {};
  CaloCell_SimpleDetectorInfo(unsigned d) : m_det(d) {};

  inline void set(unsigned d) { m_det = d; }
  inline void reset() { m_det = 0; }
  inline unsigned get() const { return m_det; }

  inline bool isLArEM()   const { return (m_det & CALOCELL_DETINFO_Atlas_Lar_EM); }
  inline bool isLArHEC()  const { return (m_det & CALOCELL_DETINFO_Atlas_Lar_HEC); }
  inline bool isLArFCal() const { return (m_det & CALOCELL_DETINFO_Atlas_Lar_FCal); }
  inline bool isTile()    const { return (m_det & CALOCELL_DETINFO_Atlas_Tile); }

  inline bool isLArEMBarrel()      const { return (m_det & CALOCELL_DETINFO_EM_barrel); }
  inline bool isLArEMEndcap()      const { return (m_det & (CALOCELL_DETINFO_EM_endcap_inner | CALOCELL_DETINFO_EM_endcap_outer)); }
  inline bool isLArEMEndcapInner() const { return (m_det & CALOCELL_DETINFO_EM_endcap_inner); }
  inline bool isLArEMEndcapOuter() const { return (m_det & CALOCELL_DETINFO_EM_endcap_outer); }

  inline bool isTileBarrel()    const { return (m_det & CALOCELL_DETINFO_Tile_barrel); }
  inline bool isTileExtBarrel() const { return (m_det & CALOCELL_DETINFO_Tile_extbarrel); }
  inline bool isTileGap()       const { return (m_det & CALOCELL_DETINFO_Tile_gap); }
  inline bool isTileGapScin()   const { return (m_det & CALOCELL_DETINFO_Tile_gapscin); }
  
  inline unsigned getLArEMSampling()   const { return ((m_det & CALOCELL_DETINFO_EM_sampling  )>>CALOCELL_DETINFO_EM_sampling_SHIFT  ); }
  inline unsigned getLArHECSampling()  const { return ((m_det & CALOCELL_DETINFO_HEC_sampling )>>CALOCELL_DETINFO_HEC_sampling_SHIFT ); }
  inline unsigned getLArFCalSampling() const { return ((m_det & CALOCELL_DETINFO_FCal_sampling)>>CALOCELL_DETINFO_FCal_sampling_SHIFT); }
  inline unsigned getTileSampling()    const { return ((m_det & CALOCELL_DETINFO_Tile_sampling)>>CALOCELL_DETINFO_Tile_sampling_SHIFT); }

private:
  unsigned m_det;
};

#endif // CALOREC_CALOCELL_SIMPLEDETECTORINFO_H
