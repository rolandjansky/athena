/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IEntryLayerTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_INTERFACES_IENTRYLAYERTOOL_H
#define ISF_INTERFACES_IENTRYLAYERTOOL_H 1

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// ISF
#include "ISF_Event/EntryLayer.h"
 
namespace ISF {

  class ISFParticle;
 
  static const InterfaceID IID_IEntryLayerTool("IEntryLayerTool", 1, 0);
   
  /**
   @class IEntryLayerTool
       
   @author Elmar.Ritsch -at- cern.ch
   */
     
  class IEntryLayerTool : virtual public IAlgTool {
     public:
     
       /** Virtual destructor */
       virtual ~IEntryLayerTool(){}

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_IEntryLayerTool; }
       
       /** Add the given particle to the corresponding Entry/Exit layer if applicable */
       virtual ISF::EntryLayer registerParticle(const ISFParticle& isp, EntryLayer layer=fUnsetEntryLayer) = 0;
  };

} // end of namespace

#endif // ISF_INTERFACES_IENTRYLAYERTOOL_H
