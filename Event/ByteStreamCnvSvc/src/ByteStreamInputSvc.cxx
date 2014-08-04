/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ByteStreamCnvSvc/ByteStreamInputSvc.h"

// Constructor.
ByteStreamInputSvc::ByteStreamInputSvc(const std::string& name, ISvcLocator* svcloc) :
	::AthService(name, svcloc) {
}

// Destructor.
ByteStreamInputSvc::~ByteStreamInputSvc() {}
