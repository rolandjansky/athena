/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOEVENT_CALOSAMPLINGHELPER_H
#define CALOEVENT_CALOSAMPLINGHELPER_H

#include "CaloGeoHelpers/CaloSampling.h"

/**
   @class CaloSamplingHelper
   @brief algorithmic methods of the legacy CaloSampling.h class used by the legacy CaloCluster object 
*/

class CaloCell;

#include <vector>

class CaloSamplingHelper: public CaloSampling {
public:

   //* Constructors, destructors
   //**

   CaloSamplingHelper( );
   ~CaloSamplingHelper( ) {}

   //**
   //* Accessors
   //**

   /*! \brief Retrieves the sampling indicator for a cell
    *
    *  \param c reference to a non-modifiable \a CaloCell
    *
    *  Returns \a CaloSampling::Unknown if sampling cannot be found.
    */
   static CaloSample getSampling( const CaloCell& c );

   /*! \brief Return a unique bit set for a given sampling
    *
    *  \param rSample reference to a non-modifiable sampling indicator
    *
    *  Returns 0 for \a CaloSampling::Unknown .
    */
   static unsigned int getSamplingBit(const CaloSample& rSample);
   /*! \brief Return a unique bit set for a given sampling
    *
    *  \overload
    *
    *  \param rCell const reference to a \a CaloCell object
    *
    *  Returns 0 if cell has \a CaloSampling::Unknown indicator.
    */
   static unsigned int getSamplingBit(const CaloCell& rCell);

   /*! \brief Get number of available samplings */
   static unsigned int getNumberOfSamplings();

   /*! \brief Get list of available samplings */
   static bool getSamplingList(std::vector<CaloSampling::CaloSample>& vSamp);

   /*! \brief Get sampling characteristics */
   static bool isEMSampling(const CaloSample& rSample);
   static bool isHADSampling(const CaloSample& rSample);
   static bool isBarrelSampling(const CaloSample& rSample);
   static bool isEndCapSampling(const CaloSample& rSample);
   static bool isLArSampling(const CaloSample& rSample);
   static bool isTileSampling(const CaloSample& rSample);

   //**
   //* Private/Implementation
   //**
private:
   CaloSamplingHelper( const CaloSamplingHelper& rhs);
   CaloSamplingHelper& operator =( const CaloSamplingHelper& rhs);

   static unsigned int m_EMMask;
   static unsigned int m_HADMask;
   static unsigned int m_BarrelMask;
   static unsigned int m_EndCapMask;
   static unsigned int m_LArMask;
   static unsigned int m_TileMask;

   static bool matchPattern(const CaloSample& rSample, 
			    const unsigned int& mask);   

};

inline
unsigned int
CaloSamplingHelper::getNumberOfSamplings()
{
  return (unsigned int)Unknown;
}

#endif  /* CALOSAMPLING_H */
