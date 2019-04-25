/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/AthStatusCode.h"

namespace Athena {

struct StatusCategory : StatusCode::Category {
  const char* name() const override { return "Athena::Status"; }

  std::string message(StatusCode::code_t code) const override {
    switch (static_cast<Athena::Status>(code)) {
    case Status::TIMEOUT:
      return "Athena::Status::TIMEOUT";
    case Status::MISSING_DATA:
      return "Athena::Status::MISSING_DATA";
    default:
      return StatusCode::default_category().message(code);
    }
  }
};

} // namespace Athena

// Register the category
STATUSCODE_ENUM_IMPL(Athena::Status, Athena::StatusCategory)
