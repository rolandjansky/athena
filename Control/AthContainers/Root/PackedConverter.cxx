/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/PackedConverter.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2014
 * @brief Helper for packing/unpacking a @c PackedContainer to/from a stream.
 */


#include "AthContainers/PackedConverter.h"


namespace SG {


/** 
 * @brief Constructor.
 * @param parms The parameters describing the packed data.
 */
PackedConverter::PackedConverter (const PackedParameters& parms)
  : m_parms (parms),
    m_packer (parms.nbits(),
              parms.nmantissa() + (parms.isSigned()?1:0),
              parms.scale(),
              parms.isSigned(),
              parms.rounding())
{
}


} // namespace SG
