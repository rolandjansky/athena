/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
 // original file: TauBuilderHelper.cxx
 // package:     Reconstruction/tauEvent
 // authors:     Lukasz Janyst
 // date:        2007-02-13
 //
 //  MODIFICATIONS
 // 2008-04-22: (nicom) moved setObject()/getObject() to TauCandidateData
 //-----------------------------------------------------------------------------
 
 #include "DiTauRec/DiTauToolBase.h"
 
 static const InterfaceID DiTauToolBaseID( "DiTauToolBase", 1, 0 );
 
 const InterfaceID& DiTauToolBase::interfaceID() {
     return DiTauToolBaseID;
 }
 
 
 //-------------------------------------------------------------------------
 // Constructor
 //-------------------------------------------------------------------------
 DiTauToolBase :: DiTauToolBase( const std::string &type,
         const std::string &name,
         const IInterface  *parent ):
     AthAlgTool(type, name, parent)
 {
 }
 
 //-------------------------------------------------------------------------
 // Destructor
 //-------------------------------------------------------------------------
 DiTauToolBase :: ~DiTauToolBase()
 {
 }
 
 //-------------------------------------------------------------------------
 // Initializer
 //-------------------------------------------------------------------------
 StatusCode DiTauToolBase :: initialize()
 {
     return StatusCode :: SUCCESS;
 }
 
 //-------------------------------------------------------------------------
 // Initializer
 //-------------------------------------------------------------------------
 StatusCode DiTauToolBase :: eventInitialize( DiTauCandidateData *)
 {
     return StatusCode :: SUCCESS;
 }
 
 //-------------------------------------------------------------------------
 // Execute
 //-------------------------------------------------------------------------
 StatusCode DiTauToolBase :: execute( DiTauCandidateData *,
                                      const EventContext& /*ctx*/) const
 {
     return StatusCode :: SUCCESS;
 }
 
 //-------------------------------------------------------------------------
 // Cleanup
 //-------------------------------------------------------------------------
 void DiTauToolBase :: cleanup( DiTauCandidateData * )
 {
 }
 
 //-------------------------------------------------------------------------
 // Finalizer
 //-------------------------------------------------------------------------
 StatusCode DiTauToolBase :: finalize()
 {
     return StatusCode :: SUCCESS;
 }
 
 
 // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 // Helpers
 template <class T>
 bool DiTauToolBase::openContainer(T* &container, std::string containerName, bool printFATAL) {
     StatusCode sc = evtStore()->retrieve(container, containerName);
     if (sc.isFailure() || !container) {
       if (printFATAL) ATH_MSG_FATAL("Container (" << containerName << ") not found in StoreGate");
       return 0;
     }
     return container;
 }
 
 template <class T>
 bool DiTauToolBase::retrieveTool(T & tool) {
     if (tool.retrieve().isFailure()) {
         ATH_MSG_FATAL("Failed to retrieve tool " << tool);
         return false;
     } else {
         ATH_MSG_VERBOSE("Retrieved tool " << tool);
     }
     return true;
 }
