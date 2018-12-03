/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetBeamSpotService_BEAMCOND_H
#define InDetBeamSpotService_BEAMCOND_H
// BeamCondSvc.h - concrete implementation of service giving beamspot data
// Richard Hawkings, started 16/6/05

#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGate.h"

#ifndef SIMULATIONBASE
namespace Trk {
  class RecVertex;
}
#endif

class BeamCondSvc : public virtual IBeamCondSvc, public virtual AthService
{
  // template <class TYPE> class SvcFactory;

public:
  BeamCondSvc(const std::string& name, ISvcLocator* svc);
  virtual ~BeamCondSvc();

  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

  virtual const InterfaceID& type() const;

  virtual const Amg::Vector3D& beamPos() const;
  virtual float beamSigma(int i) const;
  virtual float beamSigmaXY() const;
  virtual float beamTilt(int i) const;
  virtual int beamStatus() const;
#ifndef SIMULATIONBASE
  virtual const Trk::RecVertex& beamVtx() const;
#endif
  virtual bool fillRec() const;

  virtual StatusCode initialize();
  virtual StatusCode finalize();

private:
  // methods
  void initCache(int status, float x,float y, float z, 
		 float sx, float sy, float sz,float sxy,
		 float tx, float ty);
  StatusCode update( IOVSVC_CALLBACK_ARGS );
  // services
  ServiceHandle<StoreGateSvc> p_detstore;

  // properties
  bool m_par_usedb;
  int m_par_status;
  float m_par_posx;
  float m_par_posy;
  float m_par_posz;
  float m_par_sigx;
  float m_par_sigy;
  float m_par_sigz;
  float m_par_sigxy;
  float m_par_tiltx;
  float m_par_tilty;
  // beamspot data cache - 3D point, error parameters (sigma and tilt) 
  // and RecVertex information
  int m_status;
  Amg::Vector3D m_beampos;
  float m_errpar[6];
#ifndef SIMULATIONBASE
  Trk::RecVertex m_vertex;
#endif
};

// inline implementations of access functions

inline const Amg::Vector3D& BeamCondSvc::beamPos() const { return m_beampos; }

inline float BeamCondSvc::beamSigma(int i) const { return m_errpar[i]; }

inline float BeamCondSvc::beamSigmaXY() const { return m_errpar[5]; }

inline float BeamCondSvc::beamTilt(int i) const { return m_errpar[3+i]; }

inline int BeamCondSvc::beamStatus() const { return m_status; }
#ifndef SIMULATIONBASE
inline const Trk::RecVertex& BeamCondSvc::beamVtx() const { return m_vertex; }
#endif
#endif // InDetBeamSpotService_BEAMCOND_H

