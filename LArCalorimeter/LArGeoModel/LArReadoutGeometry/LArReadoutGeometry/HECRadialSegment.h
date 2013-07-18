/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HECRadialSegment_h
#define HECRadialSegment_h 1

/**
 *      @brief Description of a radial segment of a HEC module.
 */

/**
 *	The HEC Radial Segment class describes the radial
 *	segmentation of one of the 7 compartments of the HEC.
 *	This essentially just consists of a minimum and maximum
 *	radial value.
 */

class HECRadialSegment 
{
  
  

 public:
    
  /**
   * @brief Copy Constructor
   */
  HECRadialSegment(const HECRadialSegment &right);
      
  /**
   * @brief Constructor.
   */
  HECRadialSegment (double lower, double upper);

  /**
   * @brief Destructor
   */
  ~HECRadialSegment();

  /**
   * @brief Assignment 
   */
  HECRadialSegment & operator=(const HECRadialSegment &right);
      
  /**
   * @brief Minimum Range Value
   */
  double getMinVal () const;
      
  /**
   * @brief Maximum Range Value
   */
  double getMaxVal () const;

 private: 

  double minVal;
  double maxVal;

};





inline HECRadialSegment::HECRadialSegment(const HECRadialSegment &right)
  
  
  
  :minVal(right.minVal),maxVal(right.maxVal)
  
{
  
  
}

inline HECRadialSegment::HECRadialSegment (double lower, double upper)
  
  
  
  :minVal(lower),maxVal(upper)
  
{
  
  
}


inline HECRadialSegment::~HECRadialSegment()
{
  
  
}


inline HECRadialSegment & HECRadialSegment::operator=(const HECRadialSegment &right)
{
  
  if (this!=&right) {
    minVal=right.minVal;
    maxVal=right.maxVal;
  }
  return *this;
  
}




inline double HECRadialSegment::getMinVal () const
{
  
  return minVal;
  
}

inline double HECRadialSegment::getMaxVal () const
{
  
  return maxVal;
  
}





#endif
