/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArGeoRAL/RAL.h"
#include "LArGeoCode/VDetectorParameters.h"

#include "LArGeoRAL/RALExperimentalHall.h"
#include "LArGeoRAL/RALEmb.h"
#include "LArGeoRAL/RALEmec.h"
#include "LArGeoRAL/RALHec.h"

#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdexcept>

namespace LArGeo {

  RAL::RAL()
      : m_ExpHall(new RALExperimentalHall())
      , m_Emec(new RALEmec())
      , m_Emb(new RALEmb())
      , m_Hec(new RALHec())
  {
  }


  RAL::~RAL()
  {
  }

  double RAL::GetValue(const std::string& a_name,
			   const int a0,
			   const int a1,
			   const int a2,
			   const int a3,
			   const int a4 ) const
  {

    // Call the appropriate GetValue() based on the contents of the
    // string in a_name.
    
    // Note: "npos" is defined in the <string> header.  If it's the
    // result of a find() method, then the sub-string was not found.

    if ( a_name.find("ExpHall") != std::string::npos )
      return m_ExpHall->GetValue(a_name,a0,a1,a2,a3,a4);

    if ( a_name.find("EMEC") != std::string::npos )
      return m_Emec->GetValue(a_name,a0,a1,a2,a3,a4);

    if ( a_name.find("EMB") != std::string::npos )
      return m_Emb->GetValue(a_name,a0,a1,a2,a3,a4);

    if ( a_name.find("HEC") != std::string::npos )
      return m_Hec->GetValue(a_name,a0,a1,a2,a3,a4);


    // We didn't find a match.  Throw an exception
    std::string errMessage = "RAL::GetValue: could not find a match for the key '" + a_name;
    std::cerr << errMessage << std::endl;
    throw std::runtime_error(errMessage.c_str());

    // Unreached.
  }

} // namespace LArGeo
