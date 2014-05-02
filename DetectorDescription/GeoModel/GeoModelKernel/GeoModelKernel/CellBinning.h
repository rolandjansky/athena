/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CellBinning_h
#define CellBinning_h 1
/**
 *   @brief A simple class to specify a range, and a number of bins.
 */

/** 
 *  The binning is specified as one would specify bins of a histogram.
 *  In addition to the usual start-of-range, end-of-range, and number
 *  of divisions this class also has an index for the first bin number.
 *  In this package the cell binning is used to set up regions of homogenous
 *  granularity (or just "regions").  
 */ 
class CellBinning 
{

  public:
  
  /**
   * @brief Constructor
   */
  CellBinning (double startVal, double endVal, unsigned int nBins, unsigned int firstDivisionNumber = 0);


  /**
   *     @brief Gets the ith bin lower edge
   */
  double binLower (unsigned int i) const;
  
  /**
   *	 @brief gets the ith bin upper edge
   */
  double binUpper (unsigned int i) const;
  
  /**
   *	 @brief gets the ith bin center
   */
  double binCenter (unsigned int i) const;
  
  /**
   *	 @brief Starting value
   */
  double getStart () const;
  
  /**
   *	 @brief Ending Value
   */
  double getEnd () const;
  
  /**
   *	 @brief Number of Bins
   */
  unsigned int getNumDivisions () const;
  
  /**
   *	 @brief gets the bin width
   */
  double getDelta () const;
  
  /**
   *	 @brief Number for the first division.  Optional, default=0.
   */
  unsigned int getFirstDivisionNumber () const;
  
  
 private: 
  
  double start;
  double end;
  unsigned int numDivisions;
  double delta;
  unsigned int firstDivisionNumber;
};

inline double CellBinning::binLower (unsigned int i) const
{
  return start + (i-firstDivisionNumber)*delta;
}

inline double CellBinning::binUpper (unsigned int i) const
{
  return start + (i-firstDivisionNumber+1)*delta;
}

inline double CellBinning::binCenter (unsigned int i) const
{
  return start + (i-firstDivisionNumber+0.5)*delta;
}

inline double CellBinning::getStart () const
{
  return start;
}

inline double CellBinning::getEnd () const
{
  return end;
}

inline unsigned int CellBinning::getNumDivisions () const
{
  return numDivisions;
}

inline double CellBinning::getDelta () const
{
  return delta;
}

inline unsigned int CellBinning::getFirstDivisionNumber () const
{
  return firstDivisionNumber;
}

#endif
