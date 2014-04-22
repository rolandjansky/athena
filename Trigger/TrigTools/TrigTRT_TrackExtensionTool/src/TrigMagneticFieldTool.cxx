/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGate.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "GaudiKernel/SystemOfUnits.h"


#include "TrigTRT_TrackExtensionTool/TrigMagneticFieldTool.h"

static const InterfaceID IID_TrigMagneticFieldTool("TrigMagneticFieldTool", 1, 0);

const InterfaceID& TrigMagneticFieldTool::interfaceID() { 
  return IID_TrigMagneticFieldTool; 
}

TrigMagneticFieldTool::TrigMagneticFieldTool( const std::string& type, 
					    const std::string& name, 
					    const IInterface* parent )
  : AlgTool(type, name, parent), 
    m_MagFieldSvc("AtlasFieldSvc",this->name()) {

  declareInterface< TrigMagneticFieldTool>( this );
  declareProperty( "AthenaFieldService", m_MagFieldSvc, "AtlasFieldSvc");
}

StatusCode TrigMagneticFieldTool::initialize()  {
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << name() << " in initialize" << endreq;

  StoreGateSvc* detStore; 
  StatusCode sc=service("DetectorStore",detStore);
  if (sc.isFailure()) 
    { 
      log << MSG::ERROR << name() <<" failed to get detStore" << endreq;
      return sc;
    }

      log << MSG::INFO <<"Using Athena magnetic field service"<<endreq;
      sc = m_MagFieldSvc.retrieve();
      if(sc.isFailure()) 
	{
	  log << MSG::ERROR << "Unable to retrieve Athena MagFieldService" << endreq;
	  return StatusCode::FAILURE;
	}
  return sc;
}

StatusCode TrigMagneticFieldTool::finalize() 
{
  StatusCode sc = AlgTool::finalize(); 
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

