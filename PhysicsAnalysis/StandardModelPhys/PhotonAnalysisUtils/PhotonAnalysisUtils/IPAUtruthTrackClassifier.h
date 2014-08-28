/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////
// IPAUtruthTrackClassifier.h (c) H. Abreu 01/10/2009                //
///////////////////////////////////////////////////////////////////////

#ifndef PHOTONANALYSISUTILS_IPAUTRUTHTRACKCLASSIFIER_H
#define PHOTONANALYSISUTILS_IPAUTRUTHTRACKCLASSIFIER_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "PhotonAnalysisUtils/PAUegamma.h" //to define PAU::gamma


static const InterfaceID IID_IPAUtruthTrackClassifier("IPAUtruthTrackClassifier", 1, 0);
        
class IPAUtruthTrackClassifier : virtual public IAlgTool 
{
 public:
  
  virtual void ConvPhotClassifier( PAU::egamma* g )=0;

  virtual ~IPAUtruthTrackClassifier(){};
  
  /** AlgTool interface methods */
  static const InterfaceID& interfaceID() 
    { 
      return IID_IPAUtruthTrackClassifier;
    };
  
};

#endif // PHOTONANALYSISUTILS_IPAUTRUTHTRACKCLASSIFIER_H
