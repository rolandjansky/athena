/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelMonitoring/PixelAthMonitoringBase.h"

//////////////////////////////////////////////
///
/// filling 2D(Prof) per-layer histogram, one of ["ECA","ECC","B0","B1","B2","IBL","DBMA","DBMC"]
///
StatusCode PixelAthMonitoringBase::fill2DProfLayer( std::string prof2Dname, Identifier& id, const PixelID* pid, float value, bool copy2DFEval) const {
  ATH_MSG_VERBOSE( "in fill2DProfLayer()" );

  // Define the monitored variables
  auto pm  = Monitored::Scalar<int>(   Form("%s_%s", prof2Dname.c_str(), "pm"), pid->phi_module(id));
  auto val = Monitored::Scalar<float>( Form("%s_%s", prof2Dname.c_str(), "val"), value);

  int ld  = pid->layer_disk(id);
  auto em  = Monitored::Scalar<int>(   Form("%s_%s", prof2Dname.c_str(), "em"), ld);
  bool copy = false;
  if (pid->barrel_ec(id) == 0) {
    em = pid->eta_module(id);
    if (ld == 0) {
      int feid = 0;
      int emf = 0;
      if (em < -6) {
        emf = em - 6;
      } else if (em > -7 && em < 6) {
        if (pid->eta_index(id) >= 80) feid = 1;
        emf = 2 * em + feid;
        copy = true;
      } else {
        emf = em + 6;
      }
      em = emf;
    }
  }
  fill(prof2Dname, em, pm, val);
  // next line is only for IBL: fill the (per-FE) histogram again to avoid holes in the histogram in case 
  // the filled information is available per module (and not per FE)
  if ( copy2DFEval && copy ) {
    em++;
    fill(prof2Dname, em, pm, val);
  }
  return StatusCode::SUCCESS;
}
//////////////////////////////////////////////

///
/// filling 1DProf per-lumi per-layer histograms ["ECA","ECC","B0","B1","B2","IBL","DBMA","DBMC"]
///
StatusCode PixelAthMonitoringBase::fill1DProfLumiLayers( std::string prof1Dname, int lumiblock, float* values) const {
  ATH_MSG_VERBOSE( "in fill1DProfLumiLayers()" );

  // Define the monitored variables
  auto lb = Monitored::Scalar<int>( Form("%s_%s", prof1Dname.c_str(), "lb"), lumiblock );
  auto val = Monitored::Scalar<float>( Form("%s_%s", prof1Dname.c_str(), "val"), 1.0);

  for (int i = 0; i < PixLayers::COUNT; i++) {
    val = values[i];
    fill( prof1Dname + "_" + pixLayersLabel[i], lb, val);
  }

  return StatusCode::SUCCESS;
}
//////////////////////////////////////////////

///
/// filling 1D per-layer histograms ["ECA","ECC","B0","B1","B2","IBL","DBMA","DBMC"]
///
StatusCode PixelAthMonitoringBase::fill1DProfLayers( std::string name, float* values) const {
  ATH_MSG_VERBOSE( "in fill1DProfLayers()" );

  // Define the monitored variables
  auto val = Monitored::Scalar<float>( Form("%s_%s", name.c_str(), "val"), -999. );
  for (int i = 0; i < PixLayers::COUNT; i++) {
    val = values[i];
    fill(name + "_" + pixLayersLabel[i], val);
  }
  return StatusCode::SUCCESS;
}
//////////////////////////////////////////////

///
/// filling 1DProfile per-pp0(ROD) histograms for ["ECA","ECC","B0","B1","B2","IBLA","IBLC"]
///
StatusCode PixelAthMonitoringBase::fillPP0Histos( std::string name, int(&D_A)[PixMon::kNumModulesDisk][PixMon::kNumLayersDisk], int(&D_C)[PixMon::kNumModulesDisk][PixMon::kNumLayersDisk], int(&B0)[PixMon::kNumStavesL0][PixMon::kNumModulesBarrel], int(&B1)[PixMon::kNumStavesL1][PixMon::kNumModulesBarrel], int(&B2)[PixMon::kNumStavesL2][PixMon::kNumModulesBarrel], int(&IBL)[PixMon::kNumStavesIBL][PixMon::kNumFEsIBL]) const {
  ATH_MSG_VERBOSE( "in fillPP0Histos()" );


  std::string posvar = Form("%s_%s", name.c_str(), "pos");
  std::string valvar = Form("%s_%s", name.c_str(), "val");

  for (unsigned int x = 0; x < PixMon::kNumModulesDisk; ++x) {
    for (unsigned int y = 0; y < PixMon::kNumLayersDisk; ++y) {
      auto pos = Monitored::Scalar<int>( posvar, (y-1)*8 + (x-1)/6 + 1);
      auto val = Monitored::Scalar<float>( valvar, D_A[x][y]);
      if (D_A[x][y]>-1) {
	fill(name + "_ECA", pos, val);
	D_A[x][y] = 0;
      }
    }
  }
  for (unsigned int x = 0; x < PixMon::kNumModulesDisk; ++x) {
    for (unsigned int y = 0; y < PixMon::kNumLayersDisk; ++y) {
      auto pos = Monitored::Scalar<int>( posvar, (y-1)*8 + (x-1)/6 + 1);
      auto val = Monitored::Scalar<float>( valvar, D_C[x][y]);
      if (D_C[x][y]>-1) {
	fill(name + "_ECC", pos, val);
	D_C[x][y] = 0;
      }
    }
  }
  for (unsigned int x = 0; x < PixMon::kNumStavesL0; ++x) {
    auto pos = Monitored::Scalar<int>( posvar, x);
    for (unsigned int y = 0; y < PixMon::kNumModulesBarrel; ++y) {
      auto val = Monitored::Scalar<float>( valvar, B0[x][y]);
      if (B0[x][y]>-1) {
	fill(name + "_B0", pos, val);
	B0[x][y] = 0;
      }
    }
  }
  for (unsigned int x = 0; x < PixMon::kNumStavesL1; ++x) {
    auto pos = Monitored::Scalar<int>( posvar, x);
    for (unsigned int y = 0; y < PixMon::kNumModulesBarrel; ++y) {
      auto val = Monitored::Scalar<float>( valvar, B1[x][y]);
      if (B1[x][y]>-1) {
	fill(name + "_B1", pos, val);
	B1[x][y] = 0;
      }
    }
  }
  for (unsigned int x = 0; x < PixMon::kNumStavesL2; ++x) {
    auto pos = Monitored::Scalar<int>( posvar, x);
    for (unsigned int y = 0; y < PixMon::kNumModulesBarrel; ++y) {
      auto val = Monitored::Scalar<float>( valvar, B2[x][y]);
      if (B2[x][y]>-1) {
	fill(name + "_B2", pos, val);
	B2[x][y] = 0;
      }
    }
  }
  unsigned int nbinx = PixMon::kNumStavesIBL;
  unsigned int nbiny = PixMon::kNumFEsIBL;
  for (unsigned int x = 0; x < nbinx; ++x) {
    auto pos = Monitored::Scalar<int>( posvar, x);
    for (unsigned int y = 0; y < nbiny; ++y) {
      auto val = Monitored::Scalar<float>( valvar, IBL[x][y]);
      if (IBL[x][y]>-1) {
	if (y>0.5*nbiny) {
	  fill(name + "_IBLA", pos, val);
	} else {
	  fill(name + "_IBLC", pos, val);
	}
	IBL[x][y] = 0;
      }
    }
  }
  return StatusCode::SUCCESS;
}
//////////////////////////////////////////////


///
/// helper function to get layers ID
///
int PixelAthMonitoringBase::getPixLayersID(int ec, int ld) const {
  int layer = 99;
  if (ec == 2) {
    layer = PixLayers::kECA;
  } else if (ec == -2) {
    layer = PixLayers::kECC;
  } else if (ec == 0) {
    if (ld == 0)  layer = PixLayers::kIBL;
    if (ld == 1)  layer = PixLayers::kB0;
    if (ld == 2)  layer = PixLayers::kB1;
    if (ld == 3)  layer = PixLayers::kB2;
  } else {
    if (ec == 4)  layer = PixLayers::kDBMA;
    if (ec == -4) layer = PixLayers::kDBMC;
  }
  return layer;
}
//////////////////////////////////////////////

///
/// helper function to (optionally) append histogram title
///
std::string PixelAthMonitoringBase::addTxt(std::string title, bool ontrack) const {
  if (ontrack) {
    title+="_OnTrack";
  }
  return title;
}
//////////////////////////////////////////////

///
/// helper function to get eta phi coordinates of per-layer arrays
///
void PixelAthMonitoringBase::getPhiEtaMod(const PixelID* pid, Identifier& id, int& phiMod, int& etaMod, bool& copyFE) const {
  
  phiMod = pid->phi_module(id);
  
  int layerDisk = pid->layer_disk(id);
  etaMod = layerDisk;
  copyFE = false;
  if (pid->barrel_ec(id) == 0) {
    etaMod = pid->eta_module(id);
    if (layerDisk == 0) {
      if (etaMod < -6) {
	etaMod = etaMod - 6;
      } else if (etaMod > -7 && etaMod < 6) {
	int feid = 0;
	if (pid->eta_index(id) >= 80) feid = 1;
	etaMod = 2 * etaMod + feid;
	copyFE = true;
      } else {
	etaMod = etaMod + 6;
      }
      etaMod = etaMod + 16;
    } else etaMod = etaMod + 6;
  }  
}

//////////////////////////////////////////////
///
/// checks if hit is on track
///

bool PixelAthMonitoringBase::isHitOnTrack(Identifier id, std::vector<Identifier> const &RDOIDs) const {
  return binary_search(RDOIDs.begin(), RDOIDs.end(), id);;
}
//////////////////////////////////////////////


///
/// checks if cluster is on track
///

bool PixelAthMonitoringBase::isClusterOnTrack(Identifier id, std::vector<std::pair<Identifier, double> > const &ClusterIDs) const {
  bool onTrack = false;
  std::pair<Identifier, double> searchVal = std::make_pair(id, -1.0);
  onTrack = std::binary_search(ClusterIDs.begin(), ClusterIDs.end(), searchVal,
			       [](std::pair<Identifier, double> l, std::pair<Identifier, double> r) -> bool { return l.first < r.first; });
  return onTrack;
}
//////////////////////////////////////////////

///
/// checks if cluster is on track and returns its cosalpha
///
bool PixelAthMonitoringBase::isClusterOnTrack(Identifier id, std::vector<std::pair<Identifier, double> > const &ClusterIDs, double& cosalpha) const {
  bool onTrack(false);
  std::pair<Identifier, double> searchVal = std::make_pair(id, -1.0);
  auto it = std::lower_bound(ClusterIDs.begin(), ClusterIDs.end(), searchVal,
                             [](std::pair<Identifier, double> l, std::pair<Identifier, double> r) -> bool { return l.first < r.first; });

  if (it != ClusterIDs.end() && !(id < (*it).first)) {
    onTrack = true;
    cosalpha = (*it).second;
  }
  return onTrack;
}
//////////////////////////////////////////////
