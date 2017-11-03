/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ForwardTransportSvc_H
#define ForwardTransportSvc_H

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ToolHandle.h"
#include "ForwardTransportSvc/IForwardTransportSvc.h"

class ITHistSvc;
class StoreGateSvc;
class TTree;

template <class TYPE> class SvcFactory;

class ForwardTransportSvc: public AthService, virtual public IForwardTransportSvc {

 public:
  
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
  virtual StatusCode initialize();

  virtual ForwardTracker::ConfigData getConfigData()    const { return m_cData; }
  virtual bool                       getTransportFlag() const { return m_transportFlag; }
  virtual double                     getEtaCut()        const { return m_etaCut; }
  virtual double                     getXiCut()         const { return m_xiCut; }
  virtual bool                       getFillRootTree()  const { return m_fillRootTree; }
  virtual std::string                getRootFilePath()  const { return m_rootFilePath; }
  virtual std::string                getMCkey()         const { return m_MCkey; }
  
  virtual bool selectedParticle(G4ThreeVector mom, int pid);
  virtual void fillMcInfo      (G4ThreeVector pos, double tim, G4ThreeVector mom, double ene);
  virtual void addMcVertex     (G4ThreeVector pos, double tim, G4ThreeVector mom, double ene, int pid, bool isTransported, HepMC::GenEvent*);

  virtual HepMC::GenEvent* getEvent();

  friend class SvcFactory<ForwardTransportSvc>;
  
  ForwardTransportSvc(const std::string& name, ISvcLocator* svc);
  
 private:

  ForwardTracker::ConfigData m_cData;
  bool                       m_transportFlag;
  double                     m_etaCut;
  double                     m_xiCut;
  bool                       m_fillRootTree;
  std::string                m_rootFilePath;
  std::string                m_MCkey;

  ServiceHandle<StoreGateSvc> m_StoreGate;
  ServiceHandle<ITHistSvc>    m_thistSvc;
  
  TTree* m_tree; 
  
  int    m_evt;
  int    m_pid; // Particle PDG code.
  double m_x0;  // Position and Momentum where transport code starts.
  double m_y0;
  double m_z0;
  double m_t0;
  double m_px0;
  double m_py0;
  double m_pz0;
  double m_e0;
  double m_x1;  // Position calculated at Cavern.
  double m_y1;
  double m_z1;
  double m_t1;
  double m_x2;  // Position calculated at Detector front face.
  double m_y2;
  double m_z2;
  double m_t2;
  double m_x;   // Position and Momentum where transport code ends.
  double m_y;  
  double m_z;
  double m_t;
  double m_px;
  double m_py;
  double m_pz;
  double m_e;
  bool   m_ist; // isTransported flag (0:lost, 1:transported). If particle is lost, store (Position, Momentum) where it was lost.
};

#endif
