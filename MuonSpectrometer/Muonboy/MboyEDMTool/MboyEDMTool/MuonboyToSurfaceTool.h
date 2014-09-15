/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyToSurfaceTool_H
#define MuonboyToSurfaceTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "MboyEDMToolInterfaces/IMuonboyToSurfaceTool.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
//#include "MuonGeoModel/MuonDetectorManager.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyEDMToolMisc.h"

class IMuonboyToIdentifier;

namespace Trk {

  /**
   @class MuonboyToSurfaceTool

    This tool returns surface for a MbIOObject object
   
  @author samusog@cern.ch
  
  */

class MuonboyToSurfaceTool: public AthAlgTool, virtual public IMuonboyToSurfaceTool{
public:
    MuonboyToSurfaceTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MuonboyToSurfaceTool 	 ();

public:
///////////////////////////////////

   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   /** Return surface for a  MbIOObject */
   virtual const Trk::Surface* GetSurfaceForMbIOObject   (const  MbIOObject*    pMbIOObject ) ;

   /** Return surface for a  MbIDTag */
   virtual const Trk::Surface* GetSurfaceForMbIDTagMDT   (const  MbIDTagMDT*    pMbIDTag );
   virtual const Trk::Surface* GetSurfaceForMbIDTagRPC   (const  MbIDTagRPC*    pMbIDTag );
   virtual const Trk::Surface* GetSurfaceForMbIDTagTGC   (const  MbIDTagTGC*    pMbIDTag );
   virtual const Trk::Surface* GetSurfaceForMbIDTagCSC   (const  MbIDTagCSC*    pMbIDTag );
   virtual const Trk::Surface* GetSurfaceForMbIDTagCSCclu(const  MbIDTagCSCclu* pMbIDTag );

private:
///////////////////////////////////

//Data

   const MuonGM::MuonDetectorManager* p_MuonDetectorManager;  

   ToolHandle< IMuonboyToIdentifier > p_IMuonboyToIdentifier ; //!< Pointer On IMuonboyToIdentifier

};
}

#endif

