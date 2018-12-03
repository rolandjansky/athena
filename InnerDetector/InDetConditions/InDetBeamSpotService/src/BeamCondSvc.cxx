/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// BeamCondSvc.cxx - service giving beamspot data
// Richard Hawkings, started 16/6/05

#include "GaudiKernel/SvcFactory.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoralBase/AttributeListException.h"
#include "EventPrimitives/EventPrimitives.h"
#include "BeamCondSvc.h"

// name of the folder for beamspot information
#define INDET_BEAMPOS "/Indet/Beampos"


BeamCondSvc::BeamCondSvc(const std::string& name, ISvcLocator* svc) :
  AthService(name,svc),
  p_detstore("DetectorStore",name),
  m_par_usedb(true),
  m_par_status(1),
  m_par_posx(0.),
  m_par_posy(0.),
  m_par_posz(0.),
  m_par_sigx(0.015),
  m_par_sigy(0.015),
  m_par_sigz(53.),
  m_par_sigxy(0.),
  m_par_tiltx(0.),
  m_par_tilty(0.),
  m_status(0)
{
  // declare properties
  declareProperty("useDB",m_par_usedb);
  declareProperty("status",m_par_status);
  declareProperty("posX",m_par_posx);
  declareProperty("posY",m_par_posy);
  declareProperty("posZ",m_par_posz);
  declareProperty("sigmaX",m_par_sigx);
  declareProperty("sigmaY",m_par_sigy);
  declareProperty("sigmaZ",m_par_sigz);
  declareProperty("sigmaXY",m_par_sigxy);
  declareProperty("tiltX",m_par_tiltx);
  declareProperty("tiltY",m_par_tilty);
}

BeamCondSvc::~BeamCondSvc() {}

const InterfaceID& BeamCondSvc::type() const
{ 
return IBeamCondSvc::interfaceID();
}

StatusCode BeamCondSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if (IBeamCondSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface=(IBeamCondSvc*)this;
  } else {
    return AthService::queryInterface(riid,ppvInterface);
  }
  return StatusCode::SUCCESS;
}

StatusCode BeamCondSvc::initialize()
{
  // service initialisation - get parameters, setup default cache
  // and register for condDB callbacks if needed
  msg(MSG::INFO) << "in initialize()" << endmsg;

  // get detector store
  if (p_detstore.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Detector store not found" << endmsg; 
    return StatusCode::FAILURE;
  }
  // always init cache to joboption values in case CondDB read fails
  initCache(m_par_status,m_par_posx,m_par_posy,m_par_posz,m_par_sigx,m_par_sigy,m_par_sigz,
	      m_par_sigxy,m_par_tiltx,m_par_tilty);
  if (m_par_usedb) {
    // register callback function for cache updates
    const DataHandle<AthenaAttributeList> aptr;
    if (StatusCode::SUCCESS==p_detstore->regFcn(&BeamCondSvc::update,this,
						aptr, INDET_BEAMPOS )) {
      msg(MSG::DEBUG) << "Registered callback for beam position" << endmsg;
    } else {
      msg(MSG::ERROR) << "Callback registration failed" << endmsg;
    }
  } else {
    msg(MSG::INFO) << 
     "Default beamspot parameters will be used (from jobopt) " << 
      endmsg << "Beamspot status " << m_par_status << 
      endmsg << "Beamspot position  (" << m_par_posx << "," << m_par_posy << "," <<
      m_par_posz << ")" << endmsg << "RMS size (" << m_par_sigx << "," << m_par_sigy
	<< "," << m_par_sigz << ")" << endmsg << "Tilt xz yz/radian (" << 
      m_par_tiltx << "," << m_par_tilty << ")" << endmsg;
  }
  return StatusCode::SUCCESS;
}

StatusCode BeamCondSvc::finalize() {
  return StatusCode::SUCCESS;
}

void BeamCondSvc::initCache(int status, float x, float y, float z, 
			    float sx, float sy, float sz, float sxy,
			    float tx, float ty) {
  // set cache values from given parameters (comes from jobopt or condDB)
  m_status=status;
  m_beampos=Amg::Vector3D(x,y,z);
  m_errpar[0]=sx;
  m_errpar[1]=sy;
  m_errpar[2]=sz;
  m_errpar[3]=tx;
  m_errpar[4]=ty;
  m_errpar[5]=sxy;
  // calculate corresponding error matrix
  float xx=tan(tx)*sz;
  float yy=tan(ty)*sz;
  float six=sqrt(sx*sx+xx*xx);
  float siy=sqrt(sy*sy+yy*yy);
  Amg::MatrixX beamerr(3,3);
  beamerr.setZero();
  beamerr.fillSymmetric(0,0,six*six);
  beamerr.fillSymmetric(0,1,sxy);
  beamerr.fillSymmetric(1,1,siy*siy);
  beamerr.fillSymmetric(0,2,tan(tx)*sz*sz);
  beamerr.fillSymmetric(1,2,tan(ty)*sz*sz);
  beamerr.fillSymmetric(2,2,sz*sz);

#ifndef SIMULATIONBASE
  // fill in RecVertex information, ndof and chisq are set to 0 by default
  m_vertex=Trk::RecVertex(m_beampos,beamerr);
#endif
}

bool BeamCondSvc::fillRec() const {
  // fill beamspot information from jobOptions into TDS AttributeList
  // for registration into conditions database
  // for use in updating database only

 // create spec for Beampos object
  coral::AttributeListSpecification* aspec=
    new coral::AttributeListSpecification();
  aspec->extend("status","int");
  aspec->extend("posX","float");
  aspec->extend("posY","float");
  aspec->extend("posZ","float");
  aspec->extend("sigmaX","float");
  aspec->extend("sigmaY","float");
  aspec->extend("sigmaZ","float");
  aspec->extend("tiltX","float");
  aspec->extend("tiltY","float");
  aspec->extend("sigmaXY","float");
  if (!aspec->size()) msg(MSG::ERROR) << 
			"Attribute list specification is empty!" << endmsg;
  AthenaAttributeList* alist=new AthenaAttributeList(*aspec);
  // set status to 1 for now - for future use
  (*alist)["status"].setValue(m_par_status);
  (*alist)["posX"].setValue(m_par_posx);
  (*alist)["posY"].setValue(m_par_posy);
  (*alist)["posZ"].setValue(m_par_posz);
  (*alist)["sigmaX"].setValue(m_par_sigx);
  (*alist)["sigmaY"].setValue(m_par_sigy);
  (*alist)["sigmaZ"].setValue(m_par_sigz);
  (*alist)["tiltX"].setValue(m_par_tiltx);
  (*alist)["tiltY"].setValue(m_par_tilty);
  (*alist)["sigmaXY"].setValue(m_par_sigxy);
  // record Beampos object in TDS
  if (StatusCode::SUCCESS==p_detstore->record(alist, INDET_BEAMPOS )) {
    if (msgLvl(MSG::INFO)) msg() << "Recorded Beampos object in TDS" << endmsg;
  } else {
    msg(MSG::ERROR) << "Could not record Beampos object" << endmsg;
    return false;
  }
  return true;
}

StatusCode BeamCondSvc::update( IOVSVC_CALLBACK_ARGS_P(I,keys) ) {
  // callback function to update beamspot cache when condDB data changes
  if (msgLvl(MSG::DEBUG)) 
    msg() << "update callback invoked for I=" << I << " keys: ";
  for (std::list<std::string>::const_iterator itr=keys.begin();
       itr!=keys.end(); ++itr) msg() << " " << *itr;
  msg() << endmsg;
  // read the Beampos object
  const AthenaAttributeList* atrlist=0;
  if (StatusCode::SUCCESS==p_detstore->retrieve(atrlist, INDET_BEAMPOS ) &&
    atrlist!=0) {
    int status;
    float posx,posy,posz,sigx,sigy,sigz,sigxy,tiltx,tilty;
    status=(*atrlist)["status"].data<int>();
    posx=(*atrlist)["posX"].data<float>();
    posy=(*atrlist)["posY"].data<float>();
    posz=(*atrlist)["posZ"].data<float>();
    sigx=(*atrlist)["sigmaX"].data<float>();
    sigy=(*atrlist)["sigmaY"].data<float>();
    sigz=(*atrlist)["sigmaZ"].data<float>();
    tiltx=(*atrlist)["tiltX"].data<float>();
    tilty=(*atrlist)["tiltY"].data<float>();
    // sigmaXY was added later - use zero if not present in DB
    try {
      sigxy=(*atrlist)["sigmaXY"].data<float>();
    }
    catch (coral::AttributeListException& e) {
      sigxy=0.;
      if (msgLvl(MSG::DEBUG))
	msg() << "No sigmaXY retrieved from conditions DB, assume 0" << endmsg;
    }
    if (msgLvl(MSG::INFO))
      msg() << "Read from condDB"
            << " status " << status
            << " pos (" << posx << "," << posy << "," << posz << ")"
            << " sigma (" << sigx << "," << sigy << "," << sigz << ")"
            << " tilt (" << tiltx << "," << tilty << ")"
            << " sigmaXY " << sigxy
            << endmsg;
    initCache(status,posx,posy,posz,sigx,sigy,sigz,sigxy,tiltx,tilty);
  } else {
    msg(MSG::ERROR) << "Problem reading condDB object" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
