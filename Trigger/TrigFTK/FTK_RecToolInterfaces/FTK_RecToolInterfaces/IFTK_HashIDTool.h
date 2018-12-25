/*
  Copyright (C) 2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IFTK_HASHIDTOOL_H
#define IFTK_HASHIDTOOL_H


#include "GaudiKernel/IAlgTool.h"
#include "TrigFTK_RawData/FTK_RawTrackContainer.h"

static const InterfaceID IID_IFTK_HashIDTool("IFTK_HashIDTool",1,0);

class IFTK_HashIDTool: virtual public IAlgTool {
  
 public:

  static const InterfaceID& interfaceID ()   //!< the Tool's interface
  {  return IID_IFTK_HashIDTool; }  	

  virtual unsigned int getHash(unsigned int tower, unsigned int sector,  unsigned int plane)=0;

  virtual unsigned int getHashFromAuxSector(unsigned int tower, unsigned int sector,  unsigned int plane)=0;
  
  virtual bool findHash(unsigned int hash, bool isSCT, unsigned int& tower, unsigned int& sector, unsigned int& plane)=0;

  virtual FTK_RawTrackContainer* processTracks(const FTK_RawTrackContainer& inputTracks, const bool reverseIBL)=0;
};

#endif // IFTK_HASHIDTOOL_H
