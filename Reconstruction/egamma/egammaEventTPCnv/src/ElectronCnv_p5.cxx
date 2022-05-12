///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// ElectronCnv_p5.cxx 
// Implementation file for class ElectronCnv_p5
/////////////////////////////////////////////////////////////////// 


// STL includes

// egammaEvent includes
#include "egammaEvent/Electron.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/ElectronCnv_p5.h"

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

void ElectronCnv_p5::persToTrans( const Electron_p5* pers,
				  Analysis::Electron* trans, 
				  MsgStream& msg ) 
{
//   msg << MSG::DEBUG << "Loading Electron from persistent state..."
//       << endmsg;

  // base class
  m_egammaCnv.persToTrans( &pers->m_egamma, trans, msg );
}

void ElectronCnv_p5::transToPers( const Analysis::Electron* trans, 
				  Electron_p5* pers, 
				  MsgStream& msg ) 
{
//   msg << MSG::DEBUG << "Creating persistent state of Electron..."
//       << endmsg;

  // base class
  m_egammaCnv.transToPers( trans, &pers->m_egamma, msg );
}
