/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ZDCHitsTestTool.h"

/*
#include "GeoAdaptors/GeoLArHit.h"
#include "LArSimEvent/LArHit.h"
#include "LArSimEvent/LArHitContainer.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "HepMC/GenParticle.h"
*/
#include "ZDC_SimEvent/ZDC_SimStripHit.h"
#include "ZDC_SimEvent/ZDC_SimStripHit_Collection.h"


#include <TH2D.h>
#include <TH1.h>
#include <TProfile.h>

ZDCHitsTestTool::ZDCHitsTestTool(const std::string& type, const std::string& name, const IInterface* parent)
    : SimTestToolBase(type, name, parent),
      m_zdc_sidea_0(0),m_zdc_sidea_1(0),m_zdc_sidea_2(0),m_zdc_sidea_3(0),
      m_zdc_sidec_0(0),m_zdc_sidec_1(0),m_zdc_sidec_2(0),m_zdc_sidec_3(0)
{  
  
}

StatusCode ZDCHitsTestTool::initialize()
{
  m_path+="ZDC/";
  // Modules - Side A
  _TH1D(m_zdc_sidea_0,"zdc_sidea_0",100,0.,1.e6);
  _SET_TITLE(m_zdc_sidea_0, "Energy deposit in ZDC - Side A - Module 0 (EM)","E (eV)","N/10keV");
  _TH1D(m_zdc_sidea_1,"zdc_sidea_1",100,0.,1.e6);
  _SET_TITLE(m_zdc_sidea_1, "Energy deposit in ZDC - Side A - Module 1 (Had1)","E (eV)","N/10keV");
  _TH1D(m_zdc_sidea_2,"zdc_sidea_2",100,0.,1.e6);
  _SET_TITLE(m_zdc_sidea_2, "Energy deposit in ZDC - Side A - Module 2 (Had2)","E (eV)","N/10keV");
  _TH1D(m_zdc_sidea_3,"zdc_sidea_3",100,0.,1.e6);
  _SET_TITLE(m_zdc_sidea_3, "Energy deposit in ZDC - Side A - Module 3 (Had3)","E (eV)","N/10keV");
  // Modules - Side C
  _TH1D(m_zdc_sidec_0,"zdc_sidec_0",100,0.,1.e6);
  _SET_TITLE(m_zdc_sidec_0, "Energy deposit in ZDC - Side C - Module 0 (EM)","E (eV)","N/10keV");
  _TH1D(m_zdc_sidec_1,"zdc_sidec_1",100,0.,1.e6);
  _SET_TITLE(m_zdc_sidec_1, "Energy deposit in ZDC - Side C - Module 1 (Had1)","E (eV)","N/10keV");
  _TH1D(m_zdc_sidec_2,"zdc_sidec_2",100,0.,1.e6);
  _SET_TITLE(m_zdc_sidec_2, "Energy deposit in ZDC - Side C - Module 2 (Had2)","E (eV)","N/10keV");
  _TH1D(m_zdc_sidec_3,"zdc_sidec_3",100,0.,1.e6);
  _SET_TITLE(m_zdc_sidec_3, "Energy deposit in ZDC - Side C - Module 3 (Had3)","E (eV)","N/10keV");

  
  return StatusCode::SUCCESS;
}

StatusCode ZDCHitsTestTool::processEvent() {

  ZDC_SimStripHit_ConstIterator hi;

  const DataHandle<ZDC_SimStripHit_Collection> iter;
  CHECK( evtStore()->retrieve(iter,"ZDC_SimStripHit_Collection") );
  
  for (hi=(*iter).begin(); hi != (*iter).end(); ++hi) {
    ZDC_SimStripHit ghit(*hi);
    
    double ene = ghit.GetEdep();
    int side = ghit.GetSide();
    int mod = ghit.GetMod();
    
    if (side==1) {
      switch (mod) {
        case 0: 
	  m_zdc_sidea_0->Fill(ene);
	  break;
        case 1: 
	  m_zdc_sidea_1->Fill(ene);
	  break;
        case 2: 
	  m_zdc_sidea_2->Fill(ene);
	  break;
        case 3: 
	  m_zdc_sidea_3->Fill(ene);
	  break;
      }
    } else {
      switch (mod) {
        case 0: 
	  m_zdc_sidec_0->Fill(ene);
	  break;
        case 1: 
	  m_zdc_sidec_1->Fill(ene);
	  break;
        case 2: 
	  m_zdc_sidec_2->Fill(ene);
	  break;
        case 3: 
	  m_zdc_sidec_3->Fill(ene);
	  break;
      }
    }
  }


  return StatusCode::SUCCESS;
}
