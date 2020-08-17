///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthPrescaler.h
// Header file for class AthPrescaler
// Author: S.Binet<binet@cern.ch>
// Stolen from Gaudi repository
///////////////////////////////////////////////////////////////////
#ifndef GAUDISEQUENCER_ATHPRESCALER_H
#define GAUDISEQUENCER_ATHPRESCALER_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "Gaudi/Property.h"

class AthPrescaler:public AthAlgorithm 
{

public:

  /**
   ** Constructor(s)
   **/
  AthPrescaler( const std::string& name, ISvcLocator* pSvcLocator );

  /**
   ** Destructor
   **/
  ~AthPrescaler( );

  /*****************************
   ** Public Function Members **
   *****************************/

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:

  /**************************
   ** Private Data Members **
   **************************/

  /**
   ** Percentage of events that should be passed
   **/
  Gaudi::CheckedProperty<double> m_percentPass;

  /**
   ** Number of events passed
   **/
  int m_pass;

  /**
   ** Number of events seen
   **/
  int m_seen;
};

#endif // GAUDISEQUENCER_ATHPRESCALER_H
