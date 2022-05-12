///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// ElectronCnv_p3.cxx 
// Implementation file for class ElectronCnv_p3
/////////////////////////////////////////////////////////////////// 


// STL includes

// egammaEvent includes
#include "egammaEvent/Electron.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/ElectronCnv_p3.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

// Destructor
///////////////

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

void ElectronCnv_p3::persToTrans( const Electron_p3* pers,
				  Analysis::Electron* trans, 
				  MsgStream& msg ) 
{
//   msg << MSG::DEBUG << "Loading Electron from persistent state..."
//       << endmsg;

  // base class
  m_egammaCnv.persToTrans( &pers->m_egamma, trans, msg );
}

void ElectronCnv_p3::transToPers( const Analysis::Electron* trans, 
				  Electron_p3* pers, 
				  MsgStream& msg ) 
{
//   msg << MSG::DEBUG << "Creating persistent state of Electron..."
//       << endmsg;

  // base class
  m_egammaCnv.transToPers( trans, &pers->m_egamma, msg );
}
