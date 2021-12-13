/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "SiHitsTestTool.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthenaKernel/errorcheck.h"

#include "GeoAdaptors/GeoSiHit.h"
#include "InDetSimEvent/SiHitCollection.h"
#include "AtlasHepMC/GenParticle.h"

#include <TH2D.h>
#include <TProfile.h>

SiHitsTestTool::SiHitsTestTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SimTestToolBase(type, name, parent),
    m_collection("PixelHits"),
    m_indetBarrel(0), m_indetLongView(0),
    m_hits_time(0), m_hits_edep(0), m_hits_log_edep(0),
    m_hits_xy(0), m_hits_zr(0),
    m_hits_edep_zr(0),
    m_hits_edep_z(0), m_hits_edep_r(0),
    m_hits_log_barcode(0),
    m_hits_step_length(0),
    m_hits_log_step_length(0)
{  
    declareProperty("CollectionName",  m_collection="PixelHits");
}

StatusCode SiHitsTestTool::initialize() 
{
  // initialise general (if not done yet)
  m_path+="InDet/";
  _TH2D(m_indetBarrel,"idet_xy",100,-1200.,1200.,100,-1200.,1200.);
  _SET_TITLE(m_indetBarrel, "indet hit distribution","x [mm]","y [mm]");
  // perhaps better make r-phi?
  _TH2D(m_indetLongView,"idet_zr",100,-3200.,3200.,100,0.,1200.);
  _SET_TITLE(m_indetLongView, "indet hit distribution","z [mm]","r [mm]");

  std::string detName("Pixel"); std::string detpathName("Pixel");
  // initialise pixel or SCT variables:
  if (m_collection=="PixelHits") {
    detName = "Pixel"; detpathName = "Pixel";
  }
  else if (m_collection=="PileupPixelHits") {
    detName = "PileupPixel"; detpathName = "Pixel";
  }
  else if (m_collection=="SCT_Hits") {
    detName = "SCT"; detpathName = "SCT";
  }
  else if (m_collection=="PileupSCT_Hits") {
    detName = "PileupSCT"; detpathName = "SCT";  
  }
  else {
    ATH_MSG_ERROR("collection: "<<m_collection<<", SiHitsTestTool for "<<name()<<" not supported !!!\n");
    return StatusCode::FAILURE;
  }
  m_path+=detpathName+"/";
  _TH2D(m_hits_xy,(detName+"_xy").c_str(),100,-1200.,1200.,100,-1200.,1200.);
  _SET_TITLE(m_hits_xy, "hit distribution","x [mm]","y [mm]");
  _TH2D(m_hits_zr,(detName+"_zr").c_str(),100,-3200.,3200.,100,0.,1200.);
  _SET_TITLE(m_hits_zr, "hit distribution","z [mm]","r [mm]");
  _TH1D_WEIGHTED(m_hits_time,(detName+"_edep_time").c_str(),100,0, 25);
  _SET_TITLE(m_hits_time, "energy weighted hit distribution","t [ns]","dE/dt");
  _TH1D(m_hits_edep,(detName+"_edep").c_str(),100,0.,1.);
  _SET_TITLE(m_hits_edep, "hit distribution","edep [MeV]","dN/dE [1/MeV]");
  _TH1D(m_hits_log_edep,(detName+"_log_edep").c_str(),100,-15.,2.);
  _SET_TITLE(m_hits_log_edep, "log hit distribution","log(edep [MeV])","dN/dlog(E) [1/log(MeV)]");

  _TH2D_WEIGHTED(m_hits_edep_zr,(detName+"_edep_zr").c_str(),100,-3200.,3200.,100,0.,1200.);
  _SET_TITLE(m_hits_edep_zr, "energy weighted indet hit distribution","z [mm]","r [mm]");

  _TH1D_WEIGHTED(m_hits_edep_z,(detName+"_edep_z").c_str(),100,-3200.,3200.);
  _SET_TITLE(m_hits_edep_z, "energy weighted indet hit distribution (barrel only)","z [mm]","dE/dz [MeV/mm]");
  _TH1D_WEIGHTED(m_hits_edep_r,(detName+"_edep_r").c_str(),100,0.,1200.);
  _SET_TITLE(m_hits_edep_r, "energy weighted indet hit distribution (endcap only)","r [mm]","dE/dr [MeV/mm]");

  _TH1D(m_hits_log_barcode,(detName+"_log_barcode").c_str(),50,0.,16.);
  _SET_TITLE(m_hits_log_barcode, "log(barcode)","log(barcode)","dN/dlog(barcode)");

  _TH1D(m_hits_step_length,(detName+"_step_length").c_str(),100,0.,2.);
  _SET_TITLE(m_hits_step_length, "Step Length","Step Length l","dN/dl");
  _TH1D(m_hits_log_step_length,(detName+"_log_step_length").c_str(), 100, -13, 2.);
  _SET_TITLE(m_hits_log_step_length, "Logarithm of Step Length","log(l [mm])","dN/dlog(l)");

  return StatusCode::SUCCESS;
}

StatusCode SiHitsTestTool::processEvent() {

  const DataHandle<SiHitCollection> p_collection;
  if (evtStore()->retrieve(p_collection,m_collection).isSuccess())  {
    for (SiHitConstIterator i_hit = p_collection->begin(); 
	                    i_hit != p_collection->end(); ++i_hit) {
      GeoSiHit ghit(*i_hit);
      HepGeom::Point3D<double> u = ghit.getGlobalPosition();
      if (m_hits_xy) {
	m_hits_xy->Fill(u.x(),u.y());
	m_hits_zr->Fill(u.z(),u.perp());
      }
      m_indetBarrel->Fill(u.x(),u.y());
      m_indetLongView->Fill(u.z(),u.perp());

      m_hits_time->Fill(i_hit->meanTime(),i_hit->energyLoss());
      m_hits_edep->Fill(i_hit->energyLoss());
      m_hits_log_edep->Fill( log(i_hit->energyLoss()) );
      m_hits_edep_zr->Fill(u.z(),u.perp(),i_hit->energyLoss());
      int barcode = i_hit->particleLink().barcode();
      m_hits_log_barcode->Fill( barcode > 0 ? log(barcode) : -1 );
      double step_length = ( i_hit->localStartPosition()-i_hit->localEndPosition() ).mag();
      m_hits_step_length->Fill(step_length);
      m_hits_log_step_length->Fill( log(step_length));

      if (i_hit->getBarrelEndcap()==0) {
	// barrel
	m_hits_edep_z->Fill(u.z(),i_hit->energyLoss());
      }
      else {
	// discs
	m_hits_edep_r->Fill(u.perp(),i_hit->energyLoss());
      }

      // debuging only:
      //i_hit->print();
      /*
          *** SCT Hit 
          Barrel/ EndCap Number -2
          Layer/Disk Number     5
          Eta Number            0
          Phi Number            43
          Side Number            1
       */

    }
  }

  return StatusCode::SUCCESS;
}
