/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDIIONS_EXCEPTION_H
#define TILECONDIIONS_EXCEPTION_H

#include "TileCalibBlobObjs/Exception.h"
#include "TileIdentifier/TileRawChannelUnit.h"
#include "CaloIdentifier/CaloGain.h"

namespace TileCalib {

  //
  //__________________________________________________________________________________
  /** 
      @brief Signals invalid use of TileRawChannelUnit
      @author Nils Gollub <nils.gollub@cern.ch>
  */
  class InvalidRawChanUnit : public TileCalib::Exception {
  public:
    explicit InvalidRawChanUnit( const std::string& domain, 
				 TileRawChannelUnit::UNIT unit)
      : Exception( domain, "" ) 
      {
	std::ostringstream msg;
	msg << "Invalid TileRawChannelUnit: " << unit;
	setMessage(msg.str());
      }
    virtual ~InvalidRawChanUnit() throw() {}
  };
  
  //
  //__________________________________________________________________________________
  /** 
      @brief Signals invalid use of gain
      @author Nils Gollub <nils.gollub@cern.ch>
  */
  class InvalidCaloGain : public TileCalib::Exception {
  public:
    explicit InvalidCaloGain( const std::string& domain, 
			      CaloGain::CaloGain caloGain)
      : Exception( domain, "" ) 
      {
	std::ostringstream msg;
	msg << "Invalid CaloGain: " << caloGain;
	setMessage(msg.str());
      }
    virtual ~InvalidCaloGain() throw() {}
  };
  
}

#endif
