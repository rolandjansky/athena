/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IBLParameterSvc.h
//   Header file for class IBLParameterSvc
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
//

#ifndef PIXELGEOMODEL_IBLPARAMETERSVC_H
#define PIXELGEOMODEL_IBLPARAMETERSVC_H

#include "PixelGeoModel/IIBLParameterSvc.h" 
#include "GaudiKernel/ToolHandle.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/Service.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IInterface.h" 

class IGeoDbTagSvc;
class IRDBAccessSvc;

static const InterfaceID IID_IIBLParameterSvc("IBLParameterSvc",1,0); 

class IBLParameterSvc
  : public extends<AthService, IIBLParameterSvc>
{
public:
 // Standard Constructor
    IBLParameterSvc(const std::string& name, ISvcLocator* svc);
   // Standard Destructor
    virtual ~IBLParameterSvc();
  
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    // Can get rid of these once all clients are using IIBLParameterSvc
    // rather than IBLParameterSvc.
    virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvIf ) override;
    static const InterfaceID& interfaceID();
    bool containsIBL();
    bool contains3D();
    bool containsDBM();

    std::string setStringParameters(const std::string& param,const std::string& paramName) {
        if (m_IBLpresent) {
	      if (m_disableAllClusterSplitting && paramName=="clusterSplitter") return "";
        }
        return param;
     }

    void setCablingParameters(std::vector<int> &columnsPerFE,std::vector<int> &rowsPerFE,std::vector<std::vector<int> > &FEsPerHalfModule,int *DBMColumnsPerFE=NULL,int *DBMRowsPerFE=NULL,int *DBMFEsPerHalfModule=NULL) {
	if (m_IBLpresent) {
		columnsPerFE=std::vector<int>(1);
		rowsPerFE=std::vector<int>(1);
		columnsPerFE[0]=m_LayerColumnsPerFE;
		rowsPerFE[0]=m_LayerRowsPerFE;
		FEsPerHalfModule.clear();
		FEsPerHalfModule.push_back(m_LayerFEsPerHalfModule);
		if (m_DBMpresent) {
			if (DBMColumnsPerFE) *DBMColumnsPerFE=m_LayerColumnsPerFE;
			if (DBMRowsPerFE) *DBMRowsPerFE=m_LayerRowsPerFE;
			if (DBMFEsPerHalfModule) *DBMFEsPerHalfModule=m_LayerFEsPerHalfModule_3d ? m_LayerFEsPerHalfModule_3d : m_LayerFEsPerHalfModule_planar;
		}
	}
    }

    virtual
    void setBoolParameters(bool& param, const std::string& paramName) override
    {
	if (m_IBLpresent) {
	     if (m_disablePixMapCondDB && paramName=="UsePixMapCondDB") param=false;
	     if (m_disableSpecialPixels && paramName=="EnableSpecialPixels") param=false;
	     if (m_disableAlignable && paramName=="alignable") param=false;
	     if (m_disableAllClusterSplitting && paramName=="applyNNcorrection") param = false; 
	     if (m_disableAllClusterSplitting && paramName=="doPixelClusterSplitting") param = false;
	     if (m_disableDCS && paramName=="useDCS") param=false;
	     if (paramName=="IBLAbsent") param=false;
	}
    }
protected:
           
private:
  bool m_IBLpresent,m_DBMpresent;
  int m_LayerColumnsPerFE,m_LayerRowsPerFE,m_LayerFEsPerHalfModule_planar,m_LayerFEsPerHalfModule_3d,m_layout;
  std::vector<int> m_LayerFEsPerHalfModule;

  ServiceHandle< IGeoDbTagSvc > m_geoDbTagSvc;
  ServiceHandle< IRDBAccessSvc > m_rdbAccessSvc;  
  bool m_disablePixMapCondDB;
  bool m_disableSpecialPixels;
  bool m_disableAlignable;
  bool m_disableAllClusterSplitting;
  bool m_disableDCS;

  StatusCode setIblParameters(); 
  };

  inline bool IBLParameterSvc::containsIBL() {return m_IBLpresent;}
  inline bool IBLParameterSvc::contains3D() {return m_LayerFEsPerHalfModule_3d>0;}
  inline bool IBLParameterSvc::containsDBM() {return m_DBMpresent;}

  inline const InterfaceID& IBLParameterSvc::interfaceID(){
         return IID_IIBLParameterSvc;
}
#endif
