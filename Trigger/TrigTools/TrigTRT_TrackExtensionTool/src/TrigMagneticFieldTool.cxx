/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"


#include "TrigTRT_TrackExtensionTool/TrigMagneticFieldTool.h"

static const InterfaceID IID_TrigMagneticFieldTool("TrigMagneticFieldTool", 1, 0);

const InterfaceID& TrigMagneticFieldTool::interfaceID() { 
  return IID_TrigMagneticFieldTool; 
}

TrigMagneticFieldTool::TrigMagneticFieldTool( const std::string& type, 
					    const std::string& name, 
					    const IInterface* parent )
  : AthAlgTool(type, name, parent), 
    m_MagFieldSvc("AtlasFieldSvc",this->name()) {

  declareInterface< TrigMagneticFieldTool>( this );
  declareProperty( "AthenaFieldService", m_MagFieldSvc, "AtlasFieldSvc");
}

StatusCode TrigMagneticFieldTool::initialize()  {

  ATH_MSG_INFO(" in initialize");


  ATH_MSG_INFO("Using Athena magnetic field service");
  StatusCode sc = m_MagFieldSvc.retrieve();
  if(sc.isFailure()) 
  {
    ATH_MSG_ERROR("Unable to retrieve Athena MagFieldService");
    return StatusCode::FAILURE;
  }
  return sc;
}

StatusCode TrigMagneticFieldTool::finalize() 
{
  StatusCode sc = AthAlgTool::finalize(); 
  return sc;
}

TrigMagneticFieldTool::~TrigMagneticFieldTool()
{

}

void TrigMagneticFieldTool::m_getMagneticField(double r[3],double* B)
{
  B[0]=0.0;B[1]=0.0;B[2]=0.0;
	double field[3];
	m_MagFieldSvc->getField(r,field);//field is returned in kT
	for(int i=0;i<3;i++) B[i]=field[i]/Gaudi::Units::kilogauss;//convert to kG
}

