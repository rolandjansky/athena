// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file AthenaServices/SelectionVetoes.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2020
 * @brief Communicate between AthenaOuptutStream and ThinningCacheTool.
 *
 * AthenaOutputStream processes ItemList entries, including the selections
 * of which dynamic variables to write.  This last information is then used
 * by ThinningCacheTool to populate the ThinningCache.  To convey
 * this information, AthenaOutputStream records in the event store
 * a SelectionVetoes object with a key of `SelectionVetoes_STREAM',
 * for stream STREAM.  SelectionVetoes then contains a
 * map from object names to sets of vetoed variables.
 *
 * We record the list of vetoed
 * variables, rather than selected, since that leads to a natural handling
 * of the common case where there is no selection made; we can just
 * omit any entry in that case.
 *
 * But note that the selection mechanism only applies to dynamic variables,
 * so static variables will be flagged as vetoed.
 */


#ifndef ATHENASERVICES_SELECTIONVETOES_H
#define ATHENASERVICES_SELECTIONVETOES_H


#include "AthContainersInterfaces/AuxTypes.h"
#include "AthenaKernel/sgkey_t.h"
#include "AthenaKernel/CLASS_DEF.h"
#include <vector>
#include <unordered_map>
#include <tuple>
#include <string>


namespace SG {


/**
 * @brief Map of vetoed variables.
 *
 * Map from object SG names to a mask of vetoed variables for that object.
 */
using SelectionVetoes = std::unordered_map<std::string, SG::auxid_set_t>;


} // namespace SG


CLASS_DEF( SG::SelectionVetoes, 3789442, 1 )


#endif // not ATHENASERVICES_SELECTIONVETOES_H
