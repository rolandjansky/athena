/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs this is -*-c++-*-
//C++11 to be precise

#ifndef CALOUTILS_CALOSAMPLINGHELPER_H
#define CALOUTILS_CALOSAMPLINGHELPER_H

#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloGeoHelpers/CaloSampling.h"

//#include "CaloUtils/CaloSamplingId.h"

#include <string>
#include <map>

struct CaloSamplingHelper {

public:
  /*! \brief Returns a string (name) for each CaloSampling
   * @param[in] theSample \p CaloSampling::CaloSample enumerator value
   */
  static const std::string& getSamplingName(const CaloSampling::CaloSample theSample);

  /*! \brief Returns the \p CaloSampling::CaloSample enumerator value 
   *  @param[in] samplingName string with sampling name
   */
  static CaloSampling::CaloSample getSamplingId(const std::string& samplingName);

  /*! \brief Returns a list of samplings enumerators for a list of sub-calos
   *  @param[in]  theCalos      vector of calo enumerators
   *  @param[out] theSamplings  vector of sampling enumerators
   */
  static bool getSamplings(const std::vector<CaloCell_ID::SUBCALO>& theCalos,
			   std::vector<CaloSampling::CaloSample>& theSamplings);
   
  /*! \brief Returns a list of samplings enumerators for a list of sub-calos
   *  \overload
   *  @param[in] theCalo  calo enumerator
   */
  static bool getSamplings(const CaloCell_ID::SUBCALO& theCalo,
			   std::vector<CaloSampling::CaloSample>& theSamplings);
   

  /*! \brief Returns a list of sub-calos for a list of samplings
   *  @param[in]  theSamplings  vector of sampling enumerators
   *  @param[out] theCalos      vector of calo enumerators
   */
  static bool getCalos(const std::vector<CaloSampling::CaloSample>& theSamplings,
		       std::vector<CaloCell_ID::SUBCALO>& theCalos);

private:
  const static std::map<std::string,CaloSampling::CaloSample> m_lookUp; 
  const static std::string m_unknown;

};
#endif
