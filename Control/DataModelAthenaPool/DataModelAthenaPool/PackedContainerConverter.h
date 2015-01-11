// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelAthenaPool/PackedContainerConverter.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2014
 * @brief Allow converting std::vector to SG::PackedContainer.
 */


#ifndef DATAMODELATHENAPOOL_PACKEDCONTAINERCONVERTER_H
#define DATAMODELATHENAPOOL_PACKEDCONTAINERCONVERTER_H


namespace DataModelAthenaPool {


/**
 * @brief Install converters for supported instantiations.
 */
void installPackedContainerConverters();


} // namespace DataModelAthenaPool


#endif // not DATAMODELATHENAPOOL_PACKEDCONTAINERCONVERTER_H
