/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONALIGNGENTOOLS_MUONALIGNMODULETOOL_H
#define MUONALIGNGENTOOLS_MUONALIGNMODULETOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkAlignInterfaces/IGeometryManagerTool.h"
#include "TrkAlignEvent/AlignParList.h"
#include "TrkAlignEvent/AlignModuleList.h"
#include "TrkAlignGenTools/AlignModuleTool.h"

/**
   @file MuonAlignModuleTool.h
   @class MuonAlignModuleTool

   @brief MDT-specific implementation of AlignModuleTool, primarily needed 
   to shift and restore MdtAlignModules.

   @author Robert Harrington <roberth@bu.edu>, Steffen Kaiser <Steffen.Kaiser@cern.ch>
   @date 3/18/2009
*/

namespace MuonGM {
  class MuonStation;
  class MuonDetectorManager;
}

namespace Trk {
  class AlignTSOS;
  class Track;
}

namespace Muon {

  class MuonAlignModuleTool : public Trk::AlignModuleTool {
    
  public:
    
    MuonAlignModuleTool(const std::string& type, const std::string& name,
		       const IInterface* parent);
    ~MuonAlignModuleTool();
    
    StatusCode initialize();
    StatusCode finalize();
    
    /** methods used for numerical derivative to shift module before refitting track and restore to normal position, to be inherited by detector-specific align modules */
    void shiftModule(const Trk::AlignModule* module, 
		     const Trk::Track* track, 
		     Trk::AlignModule::TransformParameters dimension, 
		     double value) const;
    /*
    void shiftMdt(const Trk::AlignModule* module, 
		  Trk::AlignModule::TransformParameters dimension, 
		  double value) const;
    void shiftTgc(const Trk::AlignModule* module, 
		  Trk::AlignModule::TransformParameters dimension, 
		  double value) const;
    void shiftRpc(const Trk::AlignModule* module, 
		  Trk::AlignModule::TransformParameters dimension, 
		  double value) const;
    */

    void shiftDetElements(Trk::AlignModule::DetectorType detType,
			  const std::vector<Identifier>& readoutElementIDs,
			  const Trk::AlignModule* module, 
			  const Trk::Track* track,
			  Trk::AlignModule::TransformParameters dimension, 
			  double value) const;
    
    void restoreModule(const Trk::AlignModule* module) const;

    void setMeasurementProperties(Trk::AlignTSOS* atsos) const;

    int subDetElementIDHash(Identifier id) const;

  private:
    const MuonGM::MuonDetectorManager* p_muonMgr;

    void shiftSurface(const Trk::AlignModule* module, 
		      const Trk::TrkDetElementBase* detelement,
		      Identifier id) const;
    
    /** stores original transforms for chambers when chambers are shifted so they can be used to restore chambers when restoreModule called */
    mutable std::vector<std::vector<Amg::Transform3D*>*> m_origTransforms; 

    /** stores original transforms for TGC chamber when chamber is shifted so it can be used to restore chamber when restoreModule called */
    mutable std::vector<MuonGM::MuonStation*> m_shiftedMuonStations;

  }; // end class
  
} // end namespace

#endif // MUONALIGNGENTOOLS_MUONALIGNMODULETOOL_H
