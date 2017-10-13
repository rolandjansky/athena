/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Package for TestBeam Detector Description converter
 -------------------------------------------------
 ***************************************************************************/


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
#include "G4PhysicalVolumeStore.hh"
#include "G4VPhysicalVolume.hh"

#include <CLHEP/Geometry/Vector3D.h>

#include <string>

StatusCode
TBDetDescrCnv::initialize()
{
    // First call parent init
    StatusCode sc = DetDescrConverter::initialize();

    MsgStream log(msgSvc(), "TBDetDescrCnv");
    log << MSG::INFO << "in initialize" << endmsg;

    if (sc.isFailure()) {
        log << MSG::ERROR << "DetDescrConverter::initialize failed" << endmsg;
        return sc;
    }

    return StatusCode::SUCCESS;
}

//--------------------------------------------------------------------

StatusCode
TBDetDescrCnv::finalize()
{
    MsgStream log(msgSvc(), "TBDetDescrCnv");
    log << MSG::INFO << "in finalize" << endmsg;

    return StatusCode::SUCCESS;
}

//--------------------------------------------------------------------

StatusCode
TBDetDescrCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj)
{
  //StatusCode sc = StatusCode::SUCCESS;
  MsgStream log(msgSvc(), "TBDetDescrCnv");
  log << MSG::INFO
      << "in createObj: creating a TBDetDescrManager object in the detector store"
      << endmsg;

  // Create a new TBDetDescrManager

  DetDescrAddress* ddAddr;
  ddAddr = dynamic_cast<DetDescrAddress*> (pAddr);
  if(!ddAddr) {
    log << MSG::FATAL << "Could not cast to DetDescrAddress." << endmsg;
    return StatusCode::FAILURE;
  }

  // Get the StoreGate key of this container.
  std::string mgrKey  = *( ddAddr->par() );
  if ("" == mgrKey) {
    log << MSG::DEBUG << "No Manager key " << endmsg;
  }
  else {
    log << MSG::DEBUG << "Manager key is " << mgrKey << endmsg;
  }

  // Create the manager
  TBDetDescrManager* TBmgr = new TBDetDescrManager();

  // Pass a pointer to the container to the Persistency service by reference.
  pObj = StoreGateSvc::asStorable(TBmgr);

  // Initialize the TBDetDescr manager from FADS

  // Go through the physical volumes and hook the SDs up
  G4PhysicalVolumeStore *physicalVolumeStore = G4PhysicalVolumeStore::GetInstance();
  for (unsigned int iid=0; iid<TBElementID::TotalSize; ++iid) {

    TBElementID::TBElementID id = (TBElementID::TBElementID)iid;
    std::string name=TBmgr->getElement(id).name();

    if (name.length() > 0) {

      G4String det = name;
      G4VPhysicalVolume *thePhysicalVolume = physicalVolumeStore->GetVolume(det,false);
      //NB This call will just return the first one which matches,
      //there is no protection against multiple volumes with the same
      //name.
      if (thePhysicalVolume) {
        HepGeom::Vector3D<double> position = thePhysicalVolume->GetTranslation();//possibly GetFrameTranslation() or GetObjectTranslation()??
        CLHEP::HepRotation rotation;
        G4RotationMatrix *r=thePhysicalVolume->GetRotation(); //possibly GetFrameRotation() or GetObjectRotation()??
        if(r)
          {
            rotation = (*r);
          }
        Amg::Vector3D positionEigen=Amg::Vector3D(position.x(), position.y(), position.z());
        Amg::RotationMatrix3D rotationEigen=Amg::RotationMatrix3D(3,3);
        rotationEigen<<rotation.xx(),rotation.xy(),rotation.xz(),
                       rotation.yx(),rotation.yy(),rotation.yz(),
                       rotation.zx(),rotation.zy(),rotation.zz();
        TBmgr->setElement(TBElement(id,name,positionEigen,rotationEigen));
        log << MSG::DEBUG
            << (std::string)TBmgr->getElement(id)
            << endmsg;

      } else {
        log << MSG::DEBUG
            << "TBElement '" << name << "' not found among G4 volumes"
            << endmsg;
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
  MsgStream log(msgSvc(), "TBDetDescrCnv");
  log << MSG::DEBUG
      << "in TBDetDescrCnv::TBDetDescrCnv "
      << endmsg;
}
