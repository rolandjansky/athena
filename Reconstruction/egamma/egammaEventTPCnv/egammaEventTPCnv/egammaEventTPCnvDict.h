///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// File: egammaEventTPCnv/egammaEventTPCnvDict.h
/// Dictionary file for persistent representation(s) of AOD containers
/// Author: Sebastien Binet <binet@cern.ch>
/// Date:   December 2006

#ifndef EGAMMAEVENTTPCNV_EGAMMAEVENTTPCNVDICT_H
#define EGAMMAEVENTTPCNV_EGAMMAEVENTTPCNVDICT_H

#include "egammaEventTPCnv/egamma_p1.h"
#include "egammaEventTPCnv/egammaContainer_p1.h"
#include "egammaEventTPCnv/egammaContainerCnv_p1.h"
#include "egammaEventTPCnv/Electron_p1.h"
#include "egammaEventTPCnv/ElectronContainer_p1.h"
#include "egammaEventTPCnv/ElectronContainerCnv_p1.h"
#include "egammaEventTPCnv/Photon_p1.h"
#include "egammaEventTPCnv/PhotonContainer_p1.h"
#include "egammaEventTPCnv/PhotonContainerCnv_p1.h"
#include "egammaEventTPCnv/egDetail_p1.h"
#include "egammaEventTPCnv/egDetailContainer_p1.h"
#include "egammaEventTPCnv/egDetailContainerCnv_p1.h"
#include "egammaEventTPCnv/egamma_p2.h"
#include "egammaEventTPCnv/egammaContainer_p2.h"
#include "egammaEventTPCnv/egammaContainerCnv_p2.h"
#include "egammaEventTPCnv/Electron_p2.h"
#include "egammaEventTPCnv/ElectronContainer_p2.h"
#include "egammaEventTPCnv/ElectronContainerCnv_p2.h"
#include "egammaEventTPCnv/Photon_p2.h"
#include "egammaEventTPCnv/PhotonContainer_p2.h"
#include "egammaEventTPCnv/PhotonContainerCnv_p2.h"
#include "egammaEventTPCnv/egDetail_p2.h"
#include "egammaEventTPCnv/egDetailContainer_p2.h"
#include "egammaEventTPCnv/egDetailContainerCnv_p2.h"
#include "egammaEventTPCnv/egamma_p3.h"
#include "egammaEventTPCnv/egammaContainer_p3.h"
#include "egammaEventTPCnv/egammaContainerCnv_p3.h"
#include "egammaEventTPCnv/Electron_p3.h"
#include "egammaEventTPCnv/ElectronContainer_p3.h"
#include "egammaEventTPCnv/ElectronContainerCnv_p3.h"
#include "egammaEventTPCnv/Photon_p3.h"
#include "egammaEventTPCnv/PhotonContainer_p3.h"
#include "egammaEventTPCnv/PhotonContainerCnv_p3.h"
#include "egammaEventTPCnv/egamma_p4.h"
#include "egammaEventTPCnv/egammaContainer_p4.h"
#include "egammaEventTPCnv/egammaContainerCnv_p4.h"
#include "egammaEventTPCnv/Electron_p4.h"
#include "egammaEventTPCnv/ElectronContainer_p4.h"
#include "egammaEventTPCnv/ElectronContainerCnv_p4.h"
#include "egammaEventTPCnv/Photon_p4.h"
#include "egammaEventTPCnv/PhotonContainer_p4.h"
#include "egammaEventTPCnv/PhotonContainerCnv_p4.h"
#include "egammaEventTPCnv/egamma_p5.h"
#include "egammaEventTPCnv/egammaContainer_p5.h"
#include "egammaEventTPCnv/egammaContainerCnv_p5.h"
#include "egammaEventTPCnv/Electron_p5.h"
#include "egammaEventTPCnv/ElectronContainer_p5.h"
#include "egammaEventTPCnv/ElectronContainerCnv_p5.h"
#include "egammaEventTPCnv/Photon_p5.h"
#include "egammaEventTPCnv/PhotonContainer_p5.h"
#include "egammaEventTPCnv/PhotonContainerCnv_p5.h"

#include "egammaEventTPCnv/CaloRings_p1.h"
#include "egammaEventTPCnv/CaloRingsContainer_p1.h"
#include "egammaEventTPCnv/CaloRingsContainerCnv_p1.h"

struct GCCXML_DUMMY_INSTANTIATION_EGAMMAEVENTTPCNV {
  T_TPCnv<ElectronContainer, ElectronContainer_p1> m_elecnv;
  T_TPCnv<PhotonContainer, PhotonContainer_p1> m_phocnv;
  T_TPCnv<ElectronContainer, ElectronContainer_p2> m_elecnv_2;
  T_TPCnv<PhotonContainer, PhotonContainer_p2> m_phocnv_2;
  T_TPCnv<ElectronContainer, ElectronContainer_p3> m_elecnv_3;
  T_TPCnv<PhotonContainer, PhotonContainer_p3> m_phocnv_3;
  T_TPCnv<ElectronContainer, ElectronContainer_p4> m_elecnv_4;
  T_TPCnv<PhotonContainer, PhotonContainer_p4> m_phocnv_4;
  T_TPCnv<ElectronContainer, ElectronContainer_p5> m_elecnv_5;
  T_TPCnv<PhotonContainer, PhotonContainer_p5> m_phocnv_5;

  T_TPCnv<egDetailContainer, egDetailContainer_p1> m_egDetailContainercnv;
  T_TPCnv<egDetailContainer, egDetailContainer_p2> m_egDetailContainercnv_2;

  T_TPCnv<CaloRingsContainer, CaloRingsContainer_p1> m_caloRingsContainercnv;
};

#endif //EGAMMAEVENTTPCNV_EGAMMAEVENTTPCNVDICT_H
