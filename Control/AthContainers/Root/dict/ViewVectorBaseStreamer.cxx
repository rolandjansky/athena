/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/Root/ViewVectorBaseStreamer.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2016
 * @brief Streamer code for I/O with SG::ViewVectorBase.
 *
 * This sets up a custom streamer function to ensure that toPersistent()
 * is called when ViewVectorBase is written.  The toTransient() method
 * is called from the root read rule (or from the pool/bytestream converter
 * in the case of schema evolution).
 */


#include "AthContainers/ViewVectorBase.h"
#include "TClass.h"
#include "TBuffer.h"


namespace SG {


void ViewVectorBaseStreamerFunc (TBuffer& b, void* p)
{
  static TClass* const cl = TClass::GetClass ("SG::ViewVectorBase");
  if (b.IsReading()) {
    cl->ReadBuffer (b, p);
  } else {
    SG::ViewVectorBase* vvb = reinterpret_cast<SG::ViewVectorBase*>(p);
    vvb->toPersistent();
    cl->WriteBuffer(b, p);
  }
}


} // namespace SG


namespace ROOT {
TGenericClassInfo* GenerateInitInstance(const SG::ViewVectorBase*);
}

namespace SG {
struct InstallViewVectorBaseStreamer {
  InstallViewVectorBaseStreamer() {
    ROOT::GenerateInitInstance((SG::ViewVectorBase*)nullptr)->SetStreamerFunc (ViewVectorBaseStreamerFunc);
  }
};
InstallViewVectorBaseStreamer viewVectorBaseStreamerInstance;
} // namespace SG

