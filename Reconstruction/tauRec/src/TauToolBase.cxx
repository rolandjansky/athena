/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:        TauBuilderHelper.cxx
// package:     Reconstruction/tauEvent
// authors:     Lukasz Janyst
// date:        2007-02-13
//
//  MODIFICATIONS
// 2008-04-22: (nicom) moved setObject()/getObject() to TauCandidateData
//-----------------------------------------------------------------------------

#include "tauRec/TauToolBase.h"

static const InterfaceID TauToolBaseID( "TauToolBase", 1, 0 );

const InterfaceID& TauToolBase::interfaceID() {
    return TauToolBaseID;
}


//-------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------
TauToolBase :: TauToolBase( const std::string &type,
        const std::string &name,
        const IInterface  *parent ):
    AthAlgTool(type, name, parent)
{
}

//-------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------
TauToolBase :: ~TauToolBase()
{
}

//-------------------------------------------------------------------------
// Initializer
//-------------------------------------------------------------------------
StatusCode TauToolBase :: initialize()
{
    return StatusCode :: SUCCESS;
}

//-------------------------------------------------------------------------
// Initializer
//-------------------------------------------------------------------------
StatusCode TauToolBase :: eventInitialize( TauCandidateData *)
{
    return StatusCode :: SUCCESS;
}

//-------------------------------------------------------------------------
// Execute
//-------------------------------------------------------------------------
StatusCode TauToolBase :: execute( TauCandidateData * )
{
    return StatusCode :: SUCCESS;
}

//-------------------------------------------------------------------------
// Cleanup
//-------------------------------------------------------------------------
void TauToolBase :: cleanup( TauCandidateData * )
{
}

//-------------------------------------------------------------------------
// Finalizer
//-------------------------------------------------------------------------
StatusCode TauToolBase :: eventFinalize( TauCandidateData * )
{
    return StatusCode :: SUCCESS;
}

//-------------------------------------------------------------------------
// Finalizer
//-------------------------------------------------------------------------
StatusCode TauToolBase :: finalize()
{
    return StatusCode :: SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// Helpers
template <class T>
bool TauToolBase::openContainer(T* &container, std::string containerName, bool printFATAL) {
    StatusCode sc = evtStore()->retrieve(container, containerName);
    if (sc.isFailure() || !container) {
      if (printFATAL) ATH_MSG_FATAL("Container (" << containerName << ") not found in StoreGate");
      return 0;
    }
    return container;
}

template <class T>
bool TauToolBase::retrieveTool(T & tool) {
    if (tool.retrieve().isFailure()) {
        ATH_MSG_FATAL("Failed to retrieve tool " << tool);
        return false;
    } else {
        ATH_MSG_VERBOSE("Retrieved tool " << tool);
    }
    return true;
}

