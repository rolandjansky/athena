/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LArRawConditions/LArShapeComplete.h" 

const std::vector<float> LArShapeComplete::m_empty;

LArShapeComplete::LArShapeComplete(){ 
  m_cache.gain=-1;
}

LArShapeComplete::~LArShapeComplete() {}

/* Fill transient object in ATHENA *****************************************
 */
void LArShapeComplete::set(const HWIdentifier& CellID, int gain,
			   const std::vector<std::vector<float> >& vShape,
			   const std::vector<std::vector<float> >& vShapeDer,
			   float timeOffset, float timeBinWidth)  
{
  LArShapeP2 t (timeOffset, timeBinWidth, vShape, vShapeDer);
  setPdata(CellID, t, gain) ; 
}

/* retrieve Shape for a given tbin using online ID ****************************
 */
LArShapeComplete::ShapeRef_t
LArShapeComplete::Shape(const HWIdentifier& CellID,
                        int gain,
                        int tbin,
                        int /*mode*/) const 
{ 
  // 'mode' setting is ignored
  CONTAINER::ConstReference t = get(CellID,gain) ; 
  if (!t.isValid() || tbin<0 || tbin>=(int)t.shapeSize())
    return m_empty;
  return t.shape (tbin);
}

/* retrieve Shape derivative for a given tbin using online ID ****************************
 */
LArShapeComplete::ShapeRef_t
LArShapeComplete::ShapeDer(const HWIdentifier& CellID,
                           int gain,
                           int tbin,
                           int /*mode*/) const 
{
  // 'mode' setting is ignored
  CONTAINER::ConstReference t = get(CellID,gain) ; 
  if (!t.isValid() || tbin<0 || tbin>=(int)t.shapeDerSize())
    return m_empty;
  return t.shapeDer (tbin);
}

/* retrieve Shape for a given tbin using offline ID ****************************
 */
LArShapeComplete::ShapeRef_t
LArShapeComplete::Shape(const Identifier&  CellID,
                        int gain,
                        int tbin,
                        int /*mode*/) const
{
  // 'mode' setting is ignored
  const HWIdentifier OnId = larCablingSvc()->createSignalChannelID(CellID); 
  return Shape(OnId, gain, tbin);
} 

/* retrieve Shape derivative for a given tbin using offline ID ****************************
 */
LArShapeComplete::ShapeRef_t
LArShapeComplete::ShapeDer(const Identifier&  CellID,
                           int gain,
                           int tbin,
                           int /*mode*/) const
{
  // 'mode' setting is ignored
  const HWIdentifier OnId = larCablingSvc()->createSignalChannelID(CellID); 
  return ShapeDer(OnId, gain, tbin);
} 


/*   retrieving time offset using online/offline ID
 */

float LArShapeComplete::timeOffset(const Identifier&  CellID, int gain) const {
  const HWIdentifier OnId = larCablingSvc()->createSignalChannelID(CellID); 
  return timeOffset(OnId,gain);
}

float LArShapeComplete::timeOffset(const HWIdentifier&  CellID, int gain) const {
  if (gain<0 || gain>3) {
    std::cout << "ERROR! LArShapeComplete::timeOffset time offset called for gain " << gain << std::endl;
    std::cout.flush();
    return 0;
  }

  CONTAINER::ConstReference t = get(CellID,gain) ; 
  if (!t.isValid()) return 0;
  return  t.timeOffset();
}

//For the TB / cosmic case: retrieve the number of time-bins (aka "phases")
unsigned LArShapeComplete::nTimeBins(const HWIdentifier&  CellID, int gain) const {
  if (gain<0 || gain>3) {
    std::cout << "ERROR! LArShapeComplete::nTimeBins time offset called for gain " << gain << std::endl;
    std::cout.flush();
  }
  CONTAINER::ConstReference t = get(CellID,gain) ; 
  if (!t.isValid()) return 0;
  return t.shapeSize(); //Check also b-coefficient?
}

unsigned LArShapeComplete::nTimeBins(const Identifier&  CellID, int gain) const {
  const HWIdentifier OnId = larCablingSvc()->createSignalChannelID(CellID); 
  return nTimeBins(OnId,gain);
} 

//For the TB / cosmic case: retrieve the width of the time bin (default 24 bins in 25 ns)
float LArShapeComplete::timeBinWidth(const HWIdentifier&  CellID, int gain) const {
  if (gain<0 || gain>3) {
    std::cout << "ERROR! LArShapeComplete::timeBinWidth time offset called for gain " << gain << std::endl;
    std::cout.flush();
  }
  CONTAINER::ConstReference t = get(CellID,gain) ; 
  if (!t.isValid()) return 0;
  return t.timeBinWidth();
}

float LArShapeComplete::timeBinWidth(const Identifier&  CellID, int gain) const {
  const HWIdentifier OnId = larCablingSvc()->createSignalChannelID(CellID); 
  return timeBinWidth(OnId,gain);
}
