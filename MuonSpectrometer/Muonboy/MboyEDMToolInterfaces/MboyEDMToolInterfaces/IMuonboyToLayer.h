/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMuonboyToLayer_H
#define IMuonboyToLayer_H

#include "GaudiKernel/IAlgTool.h" 

//Can not be forwarded due to typedef
#include "TrkParameters/TrackParameters.h"

namespace Trk {
  class Layer;
}

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMToolInterfaces/IMboyEDMToolMisc.h"


class MbMuSc;

static const InterfaceID IID_IMuonboyToLayer("IMuonboyToLayer", 1, 0);

  /**
   @class IMuonboyToLayer

   This class convert MbMuSc into Layer

  @author samusog@cern.ch
  
  */

class IMuonboyToLayer:virtual public IAlgTool {

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();

   /** Convert MbMuSc in Layer **/
   virtual std::pair<const Trk::TrackParameters*, const Trk::Layer*> ToLayer(
                             const MbMuSc* pMbMuSc,
                             double sign
                            ) = 0 ;

};

inline const InterfaceID& IMuonboyToLayer::interfaceID() { return IID_IMuonboyToLayer;}

#endif

