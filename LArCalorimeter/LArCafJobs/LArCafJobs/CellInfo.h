/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
   @class LArCellInfo
   @brief Liquid Argon class for standalone storage of cell information
*/

#ifndef LArCellInfo_H
#define LArCellInfo_H

#include "TVector3.h"

#include "LArCafJobs/CaloId.h"

#include "CaloIdentifier/CaloGain.h"
//#include "LArCafJobs/TestObject2.h"

class TH2;
class TH2D;
class TProfile2D;

namespace LArSamples {

  class ShapeInfo;

  
  class CellInfo final{
  
   public:

    /** @brief Constructor  */
    CellInfo();

    CellInfo(CaloId calo, short layer = -1,
             short iEta = -1, short iPhi = -1, short feedThrough = -1, short slot = -1, short channel = -1, 
             ShapeInfo* shapeL = 0, ShapeInfo* shapeM = 0, ShapeInfo* shapeH = 0, const TVector3& position = TVector3(0,0,0), ULong64_t onlid = -1);
  
    CellInfo(const CellInfo& other, bool withShapes = true);  
 
    CellInfo(CellInfo&& other) noexcept;

    /** @brief Destructor */
    ~CellInfo();
  
    /** @return calo */
    CaloId calo() const { return (CaloId)m_calo; }
        
    /** @return layer */
    short layer() const { return m_layer; }
    
    /** @return iEta */
    short iEta() const { return (m_iEta < 0 ? -1 : (m_iEta & 0x3ff)); } // bottom 10 bits

    /** @return iEta */
    short region() const { return (m_iEta < 0 ? -1 : (m_iEta - iEta())>>10); } // top 5 bits (Actually 3 max)

    /** @return iPhi */
    short iPhi() const { return m_iPhi; }
    
    /** @return feedThrough */
    short feedThrough() const { return m_feedThrough; }
    
    /** @return slot -- starts at 1 ! */
    short slot() const { return m_slot; }
    
    /** @return feedThrough+slot position */ 
    short feb() const;
    short globalFeb() const;
    short globalPhiRing() const;

    /** @return channel */
    short channel() const { return m_channel; }
    
    /** @return location as a string */
    TString location(int verbose = 1) const;
    TVector3 position() const;
    
    bool isValid() const;
    
    const ShapeInfo* shape(CaloGain::CaloGain gain) const;
    
    double footprint() const;
    
    PartitionId partition() const;

    ULong64_t onlid() const { return m_onlid; }

    double rt()  const { return m_rt; }
    double eta() const { return m_eta; }
    double phi() const { return m_phi; }
    
    bool setShape(CaloGain::CaloGain gain, ShapeInfo* shape);
    
   private:
      
    unsigned int m_calo;
    short m_layer, m_iEta, m_iPhi, m_feedThrough, m_slot, m_channel;
    ShapeInfo* m_shapeL, *m_shapeM, *m_shapeH;
    double m_rt, m_eta, m_phi;
    ULong64_t m_onlid;

    CellInfo& operator= (const CellInfo&);
  };
}
#endif

