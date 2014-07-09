/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerUtils/src/UDAlgBase.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2009
 * @brief Helper base class for algorithms filling UserData.
 */


#include "D3PDMakerUtils/UDAlgBase.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


UDAlgBase::UDAlgBase (const std::string& name,
                      ISvcLocator* svcloc)
  : AthAlgorithm (name, svcloc),
    m_userDataSvc ("UserDataSvc", name)
{
  declareProperty ("UserDataSvc", m_userDataSvc,
                   "The UserDataSvc.");
  declareProperty ("UDPrefix", m_udprefix = "",
                   "Prefix to add to UD labels.");
}


/**
 * @brief Standard Gaudi @c initialize method.
 */
StatusCode UDAlgBase::initialize()
{
  CHECK( AthAlgorithm::initialize() );
  CHECK( m_userDataSvc.retrieve() );
  return StatusCode::SUCCESS;
}


} // namespace D3PD
