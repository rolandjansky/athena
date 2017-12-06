// Dear emacs, this is -*- c++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file  CaloCellPositionShift.h
 * @author G.Unal
 * @date June 11, 2009
 * @brief Container for a cell-level position corrections to account for distorsion of calo shape in reconstruction geometry
 */


#ifndef CALOREC_CALOCELLPOSITIONSHIFT_H
#define CALOREC_CALOCELLPOSITIONSHIFT_H


#include "AthenaKernel/CLASS_DEF.h" 
#include <vector>

namespace CaloRec {

/**
 * @class CaloCellPositionShift
 * @brief Container for a cell-level rescaling-factors, typically obtained by Z->ee intercalibration
 *
 */
class CaloCellPositionShift
  {
  public:
    typedef std::vector<float> contType;
    typedef contType::size_type size_type;

    //** Default Constructor
    CaloCellPositionShift() {};

    //** Standard constructor, resize vector and set elements to 0.
    CaloCellPositionShift(const size_type& size); 


    //**  access to shift in X
    inline float deltaX(const size_type& hashID) const
      {return m_deltaX[hashID];}
    //**  access to shift in Y
    inline float deltaY(const size_type& hashID) const
      {return m_deltaY[hashID];}
    //**  access to shift in Z
    inline float deltaZ(const size_type& hashID) const
      {return m_deltaZ[hashID];}

    //** set dx shift
    inline void set_deltaX(const size_type& hashID,float dx)
      {if(hashID<m_deltaX.size()) m_deltaX[hashID]=dx;}
    //** set dy shift
    inline void set_deltaY(const size_type& hashID,float dy)
      {if(hashID<m_deltaY.size()) m_deltaY[hashID]=dy;}
    //** set dz shift
    inline void set_deltaZ(const size_type& hashID,float dz)
      {if(hashID<m_deltaZ.size()) m_deltaZ[hashID]=dz;}

    //** resize underlying vector (new elements will be 1.0)
    void resize(const size_type& size);
        
    size_type size() const
    {return m_deltaX.size();}

  private:
    contType m_deltaX;
    contType m_deltaY;
    contType m_deltaZ;


  };//end class
}// end namespace

CLASS_DEF(CaloRec::CaloCellPositionShift,96609121,1)

#endif
