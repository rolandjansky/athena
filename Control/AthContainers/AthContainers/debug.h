// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/debug.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2015
 * @brief Helper functions intended to be called from the debugger.
 */


#ifndef ATHCONTAINERS_DEBUG_H
#define ATHCONTAINERS_DEBUG_H


#include "AthContainersInterfaces/IConstAuxStore.h"
#include "AthContainers/AuxElement.h"
#include "AthContainers/AuxVectorData.h"


namespace SGdebug {


/**
 * @brief Return the name corresponding to a given aux id.
 * @param id The aux id to look up.
 */
std::string aux_var_name (SG::auxid_t id);


/**
 * @brief Print the name corresponding to a given aux id.
 * @param id The aux id to print.
 */
void print_aux_var_name (SG::auxid_t id);


/******************************************************************************
 * Print the list of aux variables given some object.
 */


/**
 * @brief Print the list of aux variables in a set.
 * @param auxids The set to print.
 */
void print_aux_vars (const SG::auxid_set_t& auxids);


/**
 * @brief Print the list of aux variables handled by a store.
 * @param store The store to dump.
 */
void print_aux_vars (const SG::IConstAuxStore& store);


/**
 * @brief Print the list of aux variables handled by a store.
 * @param store The store to dump.
 */
void print_aux_vars (const SG::IConstAuxStore* store);


/**
 * @brief Print the list of aux variables associated with a container.
 * @param vec The container to dump.
 */
void print_aux_vars (const SG::AuxVectorData& vec);


/**
 * @brief Print the list of aux variables associated with a container.
 * @param vec The container to dump.
 */
void print_aux_vars (const SG::AuxVectorData* vec);


/**
 * @brief Print the list of aux variables associated with an element.
 * @param elt The element to dump.
 */
void print_aux_vars (const SG::AuxElement& elt);


/**
 * @brief Print the list of aux variables associated with an element.
 * @param elt The element to dump.
 */
void print_aux_vars (const SG::AuxElement* elt);


/******************************************************************************
 * Dump out aux variables.
 */


/**
 * @brief Convert an aux variable to a string.
 * @param auxid The id of the variable.
 * @param p Pointer to the location of the variable.
 */
std::string aux_var_as_string (SG::auxid_t auxid, const void* p);


/**
 * @brief Dump aux variables from a store for a single element.
 * @param store The store from which to dump.
 * @param i The index of the element to dump.
 */
void dump_aux_vars (const SG::IConstAuxStore& store, size_t i);


/**
 * @brief Dump aux variables from a store for a single element.
 * @param store The store from which to dump.
 * @param i The index of the element to dump.
 */
void dump_aux_vars (const SG::IConstAuxStore* store, size_t i);


/**
 * @brief Dump aux variables from a store for all elements.
 * @param store The store from which to dump.
 */
void dump_aux_vars (const SG::IConstAuxStore& store);


/**
 * @brief Dump aux variables from a store for all elements.
 * @param store The store from which to dump.
 */
void dump_aux_vars (const SG::IConstAuxStore* store);


/**
 * @brief Dump aux variables from a vector for a single element.
 * @param vec The vector from which to dump.
 * @param i The index of the element to dump.
 */
void dump_aux_vars (const SG::AuxVectorData& vec, size_t i);


/**
 * @brief Dump aux variables from a vector for a single element.
 * @param vec The vector from which to dump.
 * @param i The index of the element to dump.
 */
void dump_aux_vars (const SG::AuxVectorData* vec, size_t i);


/**
 * @brief Dump aux variables from a vector for all elements.
 * @param vec The vector from which to dump.
 */
void dump_aux_vars (const SG::AuxVectorData& vec);


/**
 * @brief Dump aux variables from a vector for all elements.
 * @param vec The vector from which to dump.
 */
void dump_aux_vars (const SG::AuxVectorData* vec);


/**
 * @brief Dump aux variables for an element.
 * @param elt The element from which to dump.
 */
void dump_aux_vars (const SG::AuxElement& elt);


/**
 * @brief Dump aux variables for an element.
 * @param elt The element from which to dump.
 */
void dump_aux_vars (const SG::AuxElement* elt);


} // namespace SGdebug


#endif // not ATHCONTAINERS_DEBUG_H
