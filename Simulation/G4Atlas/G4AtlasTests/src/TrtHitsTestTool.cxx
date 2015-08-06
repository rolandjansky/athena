/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrtHitsTestTool.h"

#include "GeoAdaptors/GeoTRTUncompressedHit.h"
#include "InDetSimEvent/TRTUncompressedHitCollection.h"
#include <TH1D.h>
#include <TH2D.h>


TrtHitsTestTool::TrtHitsTestTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SimTestToolBase(type, name, parent),
    m_collection("TRTUncompressedHits"),
    m_indetBarrel(0), m_indetLongView(0),
    m_hits_time_photons(0), m_hits_edep_photons(0), 
    m_hits_time_nonphotons(0), m_hits_edep_nonphotons(0),
    m_hits_xy(0), m_hits_zr(0),
    m_hits_edep_zr_photons(0), m_hits_edep_zr_nonphotons(0),
    m_hits_log_barcode(0)
{  
    declareProperty("CollectionName",  m_collection="TRTUncompressedHits");
}

StatusCode TrtHitsTestTool::initialize() 
{
  // initialise general (if not done yet)
  m_path+="InDet/";
  _TH2D(m_indetBarrel,"idet_xy",100,-1200.,1200.,100,-1200.,1200.);
  _SET_TITLE(m_indetBarrel, "indet hit distribution [All]","x [mm]","y [mm]");
  _TH2D(m_indetLongView,"idet_zr",100,-3200.,3200.,100,0.,1200.);
  _SET_TITLE(m_indetLongView, "indet hit distribution [All]","z [mm]","r [mm]");

  if (m_collection=="TRTUncompressedHits") m_path+="TRT/";
  else if (m_collection=="PileupTRTUncompressedHits") m_path+="PileupTRT/";
  else { std::cerr<<"unsupported CollectionName for "<< this->name()<<std::endl; }

  _TH2D(m_hits_xy,"trt_xy",100,-1200.,1200.,100,-1200.,1200.);
  _SET_TITLE(m_hits_xy, "hit distribution [All]","x [mm]","y [mm]");
  _TH2D(m_hits_zr,"trt_zr",100,-3200.,3200.,100,0.,1200.);
  _SET_TITLE(m_hits_zr, "hit distribution [All]","z [mm]","r [mm]");
  _TH1D(m_hits_time_photons,"trt_edep_time_photons",100,0,25);
  _SET_TITLE(m_hits_time_photons, "energy weighted hit distribution [Photons]","t [ns]","dE/dt");
  _TH1D(m_hits_edep_photons,"trt_edep_photons",100,0.,40.);
  _SET_TITLE(m_hits_edep_photons, "TRT hit distribution [Photons]","edep [MeV]","dN/dE [1/MeV]");
  _TH2D(m_hits_edep_zr_photons,"trt_edep_zr_photons",100,-3200.,3200.,100,0.,1200.);
  _SET_TITLE(m_hits_edep_zr_photons, "TRT energy weighted indet hit distribution [Photons]","z [mm]","r [mm]");
  _TH1D(m_hits_time_nonphotons,"trt_edep_time_nonphotons",100,0,25);
  _SET_TITLE(m_hits_time_nonphotons, "energy weighted hit distribution [Non-Photons]","t [ns]","dE/dt");
  _TH1D(m_hits_edep_nonphotons,"trt_edep_nonphotons",100,0.,5.);
  _SET_TITLE(m_hits_edep_nonphotons, "TRT hit distribution [Non-Photons]","edep [MeV]","dN/dE [1/MeV]");
  _TH2D(m_hits_edep_zr_nonphotons,"trt_edep_zr_nonphotons",100,-3200.,3200.,100,0.,1200.);
  _SET_TITLE(m_hits_edep_zr_nonphotons, "TRT energy weighted indet hit distribution [Non-Photons]","z [mm]","r [mm]");

  _TH1D(m_hits_log_barcode,"trt_log_barcode",50,0.,16.);
  _SET_TITLE(m_hits_log_barcode, "log(barcode)","log(barcode)","dN/dlog(barcode)");

  return StatusCode::SUCCESS;
}


StatusCode TrtHitsTestTool::processEvent() {
 
  const DataHandle<TRTUncompressedHitCollection> p_collection;
  if (evtStore()->retrieve(p_collection,m_collection).isSuccess()) {
    for (TRTUncompressedHitConstIter i_hit=p_collection->begin() ; i_hit!=p_collection->end() ; ++i_hit) {
      GeoTRTUncompressedHit ghit(*i_hit);
      HepGeom::Point3D<double> u = ghit.getGlobalPosition();
      m_indetBarrel->Fill(u.x(),u.y());
      m_indetLongView->Fill(u.z(),u.perp());
      m_hits_xy->Fill(u.x(),u.y());
      m_hits_zr->Fill(u.z(),u.perp());
      m_hits_log_barcode->Fill( log(i_hit->particleLink().barcode()) );
      int particleId(i_hit->GetParticleEncoding());
      if (particleId == 22 || static_cast<int>(abs(particleId)/100000) == 41 ||  static_cast<int>(abs(particleId)/10000000) == 1)
	{
	  //criteria set to match TRT_HitCollectionCnv_p3.cxx
	  //only photon energy is persistified properly.
	  m_hits_time_photons->Fill(i_hit->GetGlobalTime(),i_hit->GetEnergyDeposit());
	  m_hits_edep_photons->Fill(i_hit->GetEnergyDeposit());
	  m_hits_edep_zr_photons->Fill(u.z(),u.perp(),i_hit->GetEnergyDeposit());
	}
      else
	{
	  m_hits_time_nonphotons->Fill(i_hit->GetGlobalTime(),i_hit->GetEnergyDeposit());
	  m_hits_edep_nonphotons->Fill(i_hit->GetEnergyDeposit());
	  m_hits_edep_zr_nonphotons->Fill(u.z(),u.perp(),i_hit->GetEnergyDeposit());
	}
    }
  }
  else {
    ATH_MSG_WARNING("TRT Hit collection not found!");
  }

  return StatusCode::SUCCESS;
}
