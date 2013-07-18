/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EMBBasicReadoutNumbers_h
#define EMBBasicReadoutNumbers_h 1
#include <vector>
/**
 *	@brief This is a collection of numbers used to create the cell
 *	shapes.
 */
class EMBBasicReadoutNumbers 
{

  public:
  /**
   * @brief Constructor
   */
  EMBBasicReadoutNumbers();
  
  /**
   * @brief Destructor
   */
  ~EMBBasicReadoutNumbers();
  
  
  /**
   * @brief Gets the presampler active radius (Presampler Geometry.RACTIVE)
   */
  double getPresamplerRadius () const;
  
  /**
   * @brief	Gets the accordeon inner Radius (BarrelGeometry.RIN_AC)
   */
  double getRinAc () const;
  
  /**
   * @brief	Gets the accordeon outer Radius (BarrelGeometry.ROUT_AC)
   */
  double getRoutAc () const;
  
  /**
   * @brief     Sampling Separation between Layer 1 & 2 vs eta index (reg 0) 
   **/
  double getEMBSamplingSepInnerRMax(unsigned int i) const;

  /**
   * @brief	Rapidity Nodes for 1-2 Separation (BarrelLongDiv.EE)
   */
  double getEE (unsigned int i) const;
  
  /**
   * @brief	Gets the Sampling1-2 separation radii array (BarrelLong	Div.RMX12)
   */
  double getRMX12 (unsigned int i) const;
  
  /**
   * @brief	Gets the Sampling2-3 separation radii array (BarrelLong	Div.RMX23)
   */
  double getRMX23 (unsigned int i) const;
  
  /**
   * @brief     Number of values in the EMBSamplingSepInnerRMax array
   **/
  unsigned int getNumEMBSamplingSepInnerRMax() const;

  /**
   * @brief	Number of values in the EE array
   */
  unsigned int getNumEE () const;
  
  /**
   * @brief	Number of values in the RMX12 Array
   */
  unsigned int getNumRMX12 () const;
  
  /**
   * @brief	Number of values in the RMX23 Array
   */
  unsigned int getNumRMX23 () const;
  
 protected:
  
 private:

  EMBBasicReadoutNumbers(const EMBBasicReadoutNumbers &right);
  
  EMBBasicReadoutNumbers & operator=(const EMBBasicReadoutNumbers &right);
  
  double m_presamplerRadius;
  double m_rInAc;
  double m_rOutAc;
  std::vector<double> m_EMBSamplingSepInnerRMax;
  std::vector<double> m_EE;
  std::vector<double> m_RMX12;
  std::vector<double> m_RMX23;
};



inline double EMBBasicReadoutNumbers::getPresamplerRadius () const
{
  return m_presamplerRadius;
}

inline double EMBBasicReadoutNumbers::getRinAc () const
{
  return m_rInAc;
}

inline double EMBBasicReadoutNumbers::getRoutAc () const
{
  return m_rOutAc;
}

inline double EMBBasicReadoutNumbers::getEMBSamplingSepInnerRMax (unsigned int i) const
{
  return m_EMBSamplingSepInnerRMax[i];
}

inline double EMBBasicReadoutNumbers::getEE (unsigned int i) const
{
  return m_EE[i];
}

inline double EMBBasicReadoutNumbers::getRMX12 (unsigned int i) const
{
  return m_RMX12[i];
}

inline double EMBBasicReadoutNumbers::getRMX23 (unsigned int i) const
{
  return m_RMX23[i];
}

inline unsigned int EMBBasicReadoutNumbers::getNumEE () const
{
  return m_EE.size();
}

inline unsigned int EMBBasicReadoutNumbers::getNumRMX12 () const
{
  return m_RMX12.size();
}

inline unsigned int EMBBasicReadoutNumbers::getNumRMX23 () const
{
  return m_RMX23.size();
}

inline unsigned int EMBBasicReadoutNumbers::getNumEMBSamplingSepInnerRMax () const
{
  return m_EMBSamplingSepInnerRMax.size();
}



#endif
