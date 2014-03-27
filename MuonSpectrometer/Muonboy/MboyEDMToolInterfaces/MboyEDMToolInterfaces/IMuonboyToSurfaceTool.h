/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMuonboyToSurfaceTool_H
#define IMuonboyToSurfaceTool_H

#include "GaudiKernel/IAlgTool.h" 

namespace Trk {
  class  Surface;
}

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMToolInterfaces/IMboyEDMToolMisc.h"

class MbIOObject   ;

class MbIDTagMDT   ;
class MbIDTagRPC   ;
class MbIDTagTGC   ;
class MbIDTagCSC   ;
class MbIDTagCSCclu;

static const InterfaceID IID_IMuonboyToSurfaceTool("Trk::IMuonboyToSurfaceTool", 1, 0);

namespace Trk {

  /**
   @class IMuonboyToSurfaceTool

    This tool returns surface for a MbIOObject object
   
  @author samusog@cern.ch
  
  */

class IMuonboyToSurfaceTool:virtual public IAlgTool {

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();

   /** Return surface for a  MbIOObject */
   virtual const Trk::Surface* GetSurfaceForMbIOObject   (const  MbIOObject*    pMbIOObject ) = 0 ;

   /** Return surface for a  MbIDTag */
   virtual const Trk::Surface* GetSurfaceForMbIDTagMDT   (const  MbIDTagMDT*    pMbIDTag ) = 0 ;
   virtual const Trk::Surface* GetSurfaceForMbIDTagRPC   (const  MbIDTagRPC*    pMbIDTag ) = 0 ;
   virtual const Trk::Surface* GetSurfaceForMbIDTagTGC   (const  MbIDTagTGC*    pMbIDTag ) = 0 ;
   virtual const Trk::Surface* GetSurfaceForMbIDTagCSC   (const  MbIDTagCSC*    pMbIDTag ) = 0 ;
   virtual const Trk::Surface* GetSurfaceForMbIDTagCSCclu(const  MbIDTagCSCclu* pMbIDTag ) = 0 ;

};

inline const InterfaceID& Trk::IMuonboyToSurfaceTool::interfaceID() { return IID_IMuonboyToSurfaceTool;}

}

#endif

