/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Package for TestBeam Detector Description converter
 -------------------------------------------------
 Copyright (C) 2004 by ATLAS Collaboration
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include "TBDetDescrCnv/TBDetDescrCnv.h"

#include "DetDescrCnvSvc/DetDescrConverter.h"
#include "DetDescrCnvSvc/DetDescrAddress.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGate.h" 

#include "TBDetDescr/TBDetDescrManager.h"

#include "GeoPrimitives/GeoPrimitives.h"

//needed to access G4 geometry 
//#include "FadsGeometry/DetectorFacility.h"
#include "FadsGeometry/DetectorFacilityCatalog.h"
//#include "FadsGeometry/GeometryManager.h"
using namespace FADS;

#include <string>

//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

StatusCode 
TBDetDescrCnv::initialize()
{
    // First call parent init
    StatusCode sc = DetDescrConverter::initialize();

    MsgStream log(messageService(), "TBDetDescrCnv");
    log << MSG::INFO << "in initialize" << endreq;

    if (sc.isFailure()) {
        log << MSG::ERROR << "DetDescrConverter::initialize failed" << endreq;
	return sc;
    }
    
    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode 
TBDetDescrCnv::finalize()
{
    MsgStream log(messageService(), "TBDetDescrCnv");
    log << MSG::INFO << "in finalize" << endreq;

    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode
TBDetDescrCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{
  //StatusCode sc = StatusCode::SUCCESS;
  MsgStream log(messageService(), "TBDetDescrCnv");
  log << MSG::INFO 
      << "in createObj: creating a TBDetDescrManager object in the detector store" 
      << endreq;

  // Create a new TBDetDescrManager

  DetDescrAddress* ddAddr;
  ddAddr = dynamic_cast<DetDescrAddress*> (pAddr);
  if(!ddAddr) {
    log << MSG::FATAL << "Could not cast to DetDescrAddress." << endreq;
    return StatusCode::FAILURE;
  }

  // Get the StoreGate key of this container.
  std::string mgrKey  = *( ddAddr->par() );
  if ("" == mgrKey) {
    log << MSG::DEBUG << "No Manager key " << endreq;
  }
  else {
    log << MSG::DEBUG << "Manager key is " << mgrKey << endreq;
  }
    
  // Create the manager
  TBDetDescrManager* TBmgr = new TBDetDescrManager(); 

  // Pass a pointer to the container to the Persistency service by reference.
  pObj = StoreGateSvc::asStorable(TBmgr);

  // Initialize the TBDetDescr manager from FADS

  DetectorFacilityCatalog *dfs=DetectorFacilityCatalog::GetInstance();
  //GeometryManager *gm=GeometryManager::GetGeometryManager();
  //gm->PrintSolids();

  for (unsigned int iid=0; iid<TBElementID::TotalSize; ++iid) {

    TBElementID::TBElementID id = (TBElementID::TBElementID)iid;
    std::string name=TBmgr->getElement(id).name();

    if (name.length() > 0) {
      
      G4String det = name;

      if (dfs->GetDetector(det)) {

        HepGeom::Vector3D<double> position = dfs->GetDetector(det)->GetEnvelope().thePosition;
        CLHEP::HepRotation rotation;
        if(G4RotationMatrix *r=dfs->GetDetector(det)->GetEnvelope().theRotation)
          rotation = (*r);

        Amg::Vector3D positionEigen=Amg::Vector3D(position.x(), position.y(), position.z());
        Amg::RotationMatrix3D rotationEigen=Amg::RotationMatrix3D(3,3);
        rotationEigen<<rotation.xx(),rotation.xy(),rotation.xz(),
                       rotation.yx(),rotation.yy(),rotation.yz(),
                       rotation.zx(),rotation.zy(),rotation.zz();
        TBmgr->setElement(TBElement(id,name,positionEigen,rotationEigen));
        log << MSG::DEBUG
            << (std::string)TBmgr->getElement(id)
            << endreq;
      
      } else {
        log << MSG::DEBUG 
            << "TBElement '" << name << "' not found among G4 volumes"
            << endreq;
      } 
    } 
  }

  return StatusCode::SUCCESS; 

}

//--------------------------------------------------------------------

long
TBDetDescrCnv::storageType()
{
    return DetDescr_StorageType;
}

long
TBDetDescrCnv::repSvcType() const
{
    return DetDescr_StorageType;
}

//--------------------------------------------------------------------
const CLID& 
TBDetDescrCnv::classID() { 
    return ClassID_traits<TBDetDescrManager>::ID(); 
}

//--------------------------------------------------------------------
TBDetDescrCnv::TBDetDescrCnv(ISvcLocator* svcloc) 
    :
    DetDescrConverter(ClassID_traits<TBDetDescrManager>::ID(), svcloc)
{
  MsgStream log(messageService(), "TBDetDescrCnv");
  log << MSG::DEBUG 
      << "in TBDetDescrCnv::TBDetDescrCnv "
      << endreq;
}

