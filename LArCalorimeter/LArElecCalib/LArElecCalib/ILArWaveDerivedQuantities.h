/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDITIONS_ILARWAVEDERIVEDQUANTITIES_H
#define LARCONDITIONS_ILARWAVEDERIVEDQUANTITIES_H

#include "Identifier/HWIdentifier.h"
#include "LArElecCalib/LArCalibErrorCode.h"
#include "CLIDSvc/CLASS_DEF.h" 

#include <vector> 

class ILArWaveDerivedQuantities {

/*
 * This class defines the interface for accessing LArWaveDerivedQuantities
 * @stereotype Interface
 * @author M. Delmastro
 * @version  0-0-1 , 28/11/2008
 */
  
 public: 
  virtual ~ILArWaveDerivedQuantities() {};

  // online ID
  virtual const float& BaseLine(const HWIdentifier& id , int gain )  const = 0 ;
  virtual const float& MaxAmp(const HWIdentifier& id , int gain )  const = 0 ;
  virtual const float& TMaxAmp(const HWIdentifier& id , int gain )  const = 0 ;
  virtual const float& Width(const HWIdentifier& id , int gain )  const = 0 ;
  virtual const float& rT0(const HWIdentifier& id , int gain )  const = 0 ;
  virtual const float& PosLobe(const HWIdentifier& id , int gain )  const = 0 ;
  virtual const unsigned& Flag(const HWIdentifier& CellID, int gain )  const = 0;
  /*
  // online ID
  virtual const float& BaseLine(const Identifier& id , int gain )  const = 0 ;
  virtual const float& MaxAmp(const Identifier& id , int gain )  const = 0 ;
  virtual const float& TMaxAmp(const Identifier& id , int gain )  const = 0 ;
  virtual const float& Width(const Identifier& id , int gain )  const = 0 ;
  virtual const float& rT0(const Identifier& id , int gain )  const = 0 ;
  virtual const float& PosLobe(const Identifier& id , int gain )  const = 0 ;
  virtual const unsigned& Flag(const Identifier& CellID, int gain )  const = 0;
  */

  enum {ERRORCODE = LArElecCalib::ERRORCODE};

} ;

CLASS_DEF( ILArWaveDerivedQuantities , 124991738 , 1 )

#endif 
