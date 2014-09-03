/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOEVENT_CALOCOMPACTCELL_H
#define CALOEVENT_CALOCOMPACTCELL_H

/**
 * @class CaloCompactCell
 * @version \$Id: CaloCompactCell.h,v 1.2 2005-04-05 17:19:30 menke Exp $
 * @author Sven Menke <menke@mppmu.mpg.de>
 * @date 03-August-2004
 * @brief holds compactified @c CaloCell information
 *
 * This class is used to store @c CaloCell objects in an compressed
 * manner. The actual data is stored as a @c vector<unsigned short>.
 * Each compact cell therefor must provide its data in
 * multiples of 16bit @c shorts. 
 */

#include "CLIDSvc/CLASS_DEF.h"
#include <vector>

class CaloCompactCell 
{
  
 public:
  
  /** @brief value type for the compact @c CaloCell data 
   *
   *  Each CompactCell holds a multiple of this data type. This type
   *  must have the length of 16 bits. 
   */
  typedef unsigned short  value_type; 
  
  /** @brief mask to project all bits of the value_type defined above.*/
  enum MASKS {WORDMASK = 0xFFFF}; 
  
 private:
  /** @brief vector containing the compactified CaloCell information.*/
  std::vector<value_type> m_compactData;
  
 public:
  /** 
   * @brief Constructor.
   * @param theCompactData The vector holding the compact @c CaloCell data
   */
   CaloCompactCell(const std::vector<value_type> & theCompactData):
    m_compactData(theCompactData)
  { };
  
  virtual ~CaloCompactCell() { };
  
  /** 
   * @brief returns the vector of compactified CaloCell data.
   * @return a const reference to the vector holding the compact data 
   */
  inline const std::vector<value_type> & getData() const {
    return m_compactData;
  };
  
};

CLASS_DEF(CaloCompactCell, 107078417, 1)
#endif



