/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HIFlow.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef HIFLOW_H
#define HIFLOW_H

// Gaudi includes
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include <string>
#include "HIGlobal/HIFlowData.h"
#include "HIGlobal/HICaloUtil.h"

#include "InDetPrepRawData/SiClusterContainer.h"
#include "Particle/TrackParticleContainer.h"

#include "TFile.h"
#include "TH2D.h"

class AtlasDetectorID;
class Identifier;
class StoreGateSvc;
class CaloCellContainer;
class CaloTowerContainer;


/** @class HIFlow

    This is for the Doxygen-Documentation.  
    Please delete these lines and fill in information about
    the Algorithm!
    Please precede every member function declaration with a
    short Doxygen comment stating the purpose of this function.
    
    @author  Adam Trzupek <Adam.Trzupek@ifj.edu.pl>
*/  

class HIFlow : public Algorithm
  {
  public:

    static const bool CaloTagPsi[HICaloUtil::Unknown];
    static const bool CaloTagv2[HICaloUtil::Unknown];

     /** Standard Athena-Algorithm Constructor */
     HIFlow(const std::string& name, ISvcLocator* pSvcLocator);
     /** Default Destructor */
     ~HIFlow();

     void Getv2EtaDependentCaloCells(const CaloCellContainer * cCell);
     void GetPsiCaloCells(const CaloCellContainer * cCell);
     void GetPsiTowers(const CaloTowerContainer *cTower);
     void Getv2sil(const InDet::SiClusterContainer * prdContainer);
     void GetPsitrack(const Rec::TrackParticleContainer * TPcontainer);
     float Getv2track(const Rec::TrackParticleContainer * TPcontainer); 
     float get_weight(float eta, float phi, TH2D *hist); 
     
     /** standard Athena-Algorithm method */
     StatusCode          initialize();
     /** standard Athena-Algorithm method */
     StatusCode          execute();
     /** standard Athena-Algorithm method */
     StatusCode          finalize();

  private:
    
    /** class member version of retrieving MsgStream */
    mutable MsgStream                 m_log;
    /** class member version of retrieving StoreGate */
    StoreGateSvc*  m_sgSvc;

    HIFlowData *m_HIFlowData; 
    HICaloUtil *m_HICaloUtil;

    bool m_doCalo;
    bool m_doTrack;
    bool m_doSi;
    std::string m_HIFlowDataContainerName;

    /** indicates if we are processing Data */
    bool    m_isData;

    TFile  *m_weightfile;
    std::string m_HIFlowWeightFileName;
    
    TH2D *m_FCAL0_weight; 
    float m_harmonic;
    bool m_do_reweighting;
  }; 

#endif 
