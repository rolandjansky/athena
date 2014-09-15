/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONALIGNGENTOOLS_TGCGEOMETRYMANAGERTOOL_H
#define MUONALIGNGENTOOLS_TGCGEOMETRYMANAGERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkAlignInterfaces/IGeometryManagerTool.h"
#include "MuonAlignGenTools/CommonGeometryManagerTool.h"

/**
   @file TgcGeometryManagerTool.h
   @class TgcGeometryManagerTool

   @brief Tool for creating AlignModules for MDT alignment.

   @author 
     Robert Harrington <roberth@cern.ch>

   @date 02/21/2011
*/
namespace Trk {
  class IAlignModuleTool;
}

namespace Muon {

  class TgcGeometryManagerTool : public CommonGeometryManagerTool, virtual public Trk::IGeometryManagerTool {

  public:

    TgcGeometryManagerTool(const std::string& type, const std::string& name,
			   const IInterface* parent);
    virtual ~TgcGeometryManagerTool();
    
    StatusCode initialize();
    StatusCode finalize();
    
    int ReadGeometry(int);
    
    void buildGeometry();

    void PrintModPositions() {}
    void StoreModPositions(std::string) {}

  private:

    ToolHandle<Trk::IAlignModuleTool> m_alignModuleTool; //!< Pointer to AlignModuleTool

    Trk::AlignModuleList  m_alignModuleList;    

    std::vector<Trk::AlignModuleList*> m_idHashToAlignModuleMaps; // uses det element id hash

    DataVector<DataVector<Trk::AlignPar> >* m_alignParList;
    DataVector<DataVector<Trk::AlignPar> >* m_fullAlignParList;

    std::vector<int> m_mdtAlignParameters;

    void buildL1();
    void buildL0();
    void buildL23();
    void buildL3();
  };
  
}
  
#endif // MUONALIGNGENTOOLS_TGCGEOMETRYMANAGERTOOL_H
