/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALIGNINTERFACES_IJACOBIANMANAGERTOOL_H
#define TRKALIGNINTERFACES_IJACOBIANMANAGERTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include <map>

/**
   @file IJacobianManagerTool.h
   @class IJacobianManagerTool
   
   @brief Interface for tool used to calculate Jacobian terms.  Not yet implemented.
   
   Based on InDetAlignAlgs/SiGlobalChi2AlignAlgs.
   
   @author Robert Harrington <roberth@bu.edu>
   @date 1/5/08
*/

class AlMat;

namespace Trk {
  class IGeometryManagerTool;
}

namespace Trk {
  static const InterfaceID 
    IID_TRKALIGNINTERFACES_IJacobianManagerTool("IJacobianManagerTool",1,0);
  
  class IJacobianManagerTool : virtual public IAlgTool {
    
  public:
    static const InterfaceID& interfaceID();
    
    typedef std::map<int,int> ModuleIndexMap;

    virtual StatusCode initialize() = 0; // must retrieve IGeometryManagerTool

    virtual StatusCode BuildJacobian(AlMat*, int, bool, ModuleIndexMap) = 0;
    virtual StatusCode BuildJacobian(int, bool) = 0;
    
    virtual std::vector<HepVector3D> GetCenterOfGravity() const = 0;
    virtual std::vector<int> GetNumberOfModsPerLayer() const = 0;
    virtual std::vector<float> GetLayerRadius() const = 0;
    
  private:
    virtual StatusCode DumpJacobian(AlMat*) const = 0;
    virtual StatusCode TestJacobian(AlMat*, int, int) const = 0;
    virtual StatusCode FillJacobian(AlMat*, int, int, ModuleIndexMap, ModuleIndexMap) = 0;
    virtual StatusCode StoreTxt() = 0;
    virtual StatusCode ShowHitmap(ModuleIndexMap, ModuleIndexMap, bool, bool) = 0;
    virtual int FindModIndex(int, ModuleIndexMap) = 0;
    
    ToolHandle<IGeometryManagerTool> m_geometryManagerTool;
    
  }; // end class definition
  
  inline const InterfaceID& IJacobianManagerTool::interfaceID() {
    return   IID_TRKALIGNINTERFACES_IJacobianManagerTool;
  }
  
} // end namespace

#endif // TRKALIGNINTERFACES_IJACOBIANMANAGERTOOL_H
