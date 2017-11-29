// Dear emacs, this is -*- c++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file  CaloCellFactor.h
 * @author Walter Lampl <walter.lampl@cern.ch>
 * @date Dec 21, 2006
 * @brief Container for a cell-level rescaling-factors, typically obtained by Z->ee intercalibration
 */


#ifndef CALOREC_CALOCELLSCALE_H
#define CALOREC_CALOCELLSCALE_H


#include "AthenaKernel/CLASS_DEF.h" 
#include <vector>

namespace CaloRec {

/**
 * @class CaloCellFactor
 * @brief Container for a cell-level rescaling-factors, typically obtained by Z->ee intercalibration
 *
 */
class CaloCellFactor
  {
  public:
    typedef std::vector<float> contType;
    typedef contType::size_type size_type;

    //** Default Constructor
    CaloCellFactor() {};

    //** Standard constructor, resize vector and set elements on 1.0
    CaloCellFactor(const size_type& size); 


    //** non-const access
    inline float& operator[](const size_type& hashID)
      {return m_cellFactor[hashID];}


    //** const access
    inline const float& operator[](const size_type& hashID) const 
      {return m_cellFactor[hashID];}

    //** resize underlying vector (new elements will be 1.0)
    void resize(const size_type& size);
        
    size_type size() const
    {return m_cellFactor.size();}

  private:
    contType m_cellFactor;


  };//end class
}// end namespace

CLASS_DEF(CaloRec::CaloCellFactor,206274732,1)

#endif
