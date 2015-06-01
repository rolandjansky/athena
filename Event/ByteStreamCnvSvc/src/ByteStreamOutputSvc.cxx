/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Include files.
#include "ByteStreamCnvSvc/ByteStreamOutputSvc.h"

// Constructor.
ByteStreamOutputSvc::ByteStreamOutputSvc(const std::string& name, ISvcLocator* svcloc)
	: ::AthService(name, svcloc) {
}

// Destructor.
ByteStreamOutputSvc::~ByteStreamOutputSvc() {}
