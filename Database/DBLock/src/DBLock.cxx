/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file DBLock/src/DBLock.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2021
 * @brief Common database lock.
 */


#include "DBLock/DBLock.h"


namespace Athena {


/// Global mutex to protect database access.
std::recursive_mutex DBLock::m_mutex;


/** 
 * @brief Constructor.
 */
DBLock::DBLock()
  : m_lockPtr (std::make_shared<lock_t> (m_mutex))
{
}


/** 
 * @brief Destructor.
 */
DBLock::~DBLock()
{
}


} // namespace Athena
