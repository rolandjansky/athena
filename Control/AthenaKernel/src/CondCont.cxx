// $Id$
/**
 * @file AthenaKernel/src/CondCont.cpp
 * @author Vakho, Charles, Scott
 * @date Apr, 2018
 * @brief Hold mappings of ranges to condition objects.
 */


#include "AthenaKernel/CondCont.h"


/**
 * @brief Name of the category.
 */
const char* CondContBase::Category::name() const
{
  return "CondCont";
}


/**
 * @brief Description for code within this category.
 */
std::string CondContBase::Category::message (code_t code) const
{
  if (code == static_cast<code_t> (CondContStatusCode::DUPLICATE)) {
    return "DUPLICATE";
  }
  return StatusCode::Category::message (code);
}


/**
 * @brief Is code considered success?
 */
bool CondContBase::Category::isSuccess (code_t code) const
{
  return code == static_cast<code_t>( CondContStatusCode::DUPLICATE ) ||
    code == static_cast<code_t>( CondContStatusCode::SUCCESS );
}


/// Helper to test whether a code is DUPLICATE.
bool CondContBase::Category::isDuplicate (code_t code)
{
  return code == static_cast<code_t> (CondContStatusCode::DUPLICATE);
}


/// Helper to test whether a code is DUPLICATE.
bool CondContBase::Category::isDuplicate (StatusCode code)
{
  return isDuplicate (code.getCode());
}


STATUSCODE_ENUM_IMPL (CondContStatusCode, CondContBase::Category)
