/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyToLayer_H
#define MuonboyToLayer_H

#include "AthenaBaseComps/AthAlgTool.h"

//Can not be forwarded due to typedef
#include "TrkParameters/TrackParameters.h"

namespace Trk {
  class Layer;
}

#include "MboyEDMToolInterfaces/IMuonboyToLayer.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyEDMToolMisc.h"

class MbMuSc;

  /**
   @class MuonboyToLayer

   This class convert MbMuSc into Layer

  @author samusog@cern.ch
  
  */

class MuonboyToLayer: public AthAlgTool, virtual public IMuonboyToLayer{
public:
    MuonboyToLayer(const std::string&,const std::string&,const IInterface*);
    virtual ~MuonboyToLayer 	 ();

public:
///////////////////////////////////

   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   /** Convert MbMuSc in Layer **/
   virtual std::pair<const Trk::TrackParameters*, const Trk::Layer*> ToLayer(
                             const MbMuSc* pMbMuSc,
                             double sign
                            );

};

#endif

