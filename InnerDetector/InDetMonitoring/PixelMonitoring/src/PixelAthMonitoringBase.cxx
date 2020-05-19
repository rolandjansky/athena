/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelAthMonitoringBase.h"


//////////////////////////////////////////////
///
/// helper class to accumulate points to fill a 2D plot with
///
void PixelAthMonitoringBase::VecAccumulator2DMap::add( const int layer, const Identifier& id,
						  const PixelID* pid, float value ) {
  m_pm[layer].push_back(pid->phi_module(id));
  int ld = pid->layer_disk(id);
  int em = ld;
  m_val[layer].push_back(value);

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
  m_em[layer].push_back(em);
  
  if (m_copy2DFEval && copy) {
    ++em;
    m_pm[layer].push_back(pid->phi_module(id));
    m_em[layer].push_back(em);
    m_val[layer].push_back(value);
  }   
}

//////////////////////////////////////////////
///
/// take VecAccumulator2DMap and fill the corresponding group
///
void PixelAthMonitoringBase::fill2DProfLayerAccum( const VecAccumulator2DMap& accumulator ) const {
  // iterate over all actually filled layers
  for ( const auto& itr : accumulator.m_pm ) {
    // Define the monitored variables
    int layer = itr.first;
    auto pm  = Monitored::Collection(accumulator.m_prof2Dname + "_pm", accumulator.m_pm.at(layer));
    auto val = Monitored::Collection(accumulator.m_prof2Dname + "_val", accumulator.m_val.at(layer));
    auto em  = Monitored::Collection(accumulator.m_prof2Dname + "_em", accumulator.m_em.at(layer));
    fill(pixLayersLabel[layer], pm, em, val);
  }
}
  
///
/// filling 1DProf per-lumi per-layer histograms ["ECA","ECC","B0","B1","B2","IBL","DBMA","DBMC"]
///
void PixelAthMonitoringBase::fill1DProfLumiLayers( const std::string& prof1Dname, int lumiblock, float* values) const {
  ATH_MSG_VERBOSE( "in fill1DProfLumiLayers()" );

  // Define the monitored variables
  auto lb = Monitored::Scalar<int>( prof1Dname + "_lb", lumiblock );
  auto val = Monitored::Scalar<float>( prof1Dname + "_val", 1.0);

  for (int i = 0; i < PixLayers::COUNT; i++) {
    val = values[i];
    fill( pixLayersLabel[i], lb, val);
  }
}
//////////////////////////////////////////////

///
/// filling 2DProf per-lumi per-layer histograms ["ECA","ECC","B0","B1","B2","IBL","DBMA","DBMC"]
///
void PixelAthMonitoringBase::fill2DProfLumiLayers( const std::string& prof2Dname, int lumiblock, float(*values)[PixLayers::COUNT], const int* nCategories) const {
  ATH_MSG_VERBOSE( "in fill2DProfLumiLayers()" );

  // Define the monitored variables
  auto lb  = Monitored::Scalar<int>( prof2Dname + "_lb", lumiblock );
  auto val = Monitored::Scalar<float>( prof2Dname + "_val", 1.0);
  auto cat = Monitored::Scalar<int>( prof2Dname + "_cat");

  for (int i = 0; i < PixLayers::COUNT; i++) {
    for (cat = 0; cat < nCategories[i]; cat++) {
      val = values[cat][i];
      fill( pixLayersLabel[i], lb, cat, val);
    }
  }
}
//////////////////////////////////////////////


///
/// filling 1DProfile per-pp0(ROD) histograms for ["ECA","ECC","B0","B1","B2","IBLA","IBLC"]
///
void PixelAthMonitoringBase::fillFromArrays( const std::string& namePP0, AccumulatorArrays& pixarrays, const std::string& name2DMap) const {
  ATH_MSG_VERBOSE( "in fillFromArrays()" );

  const float weightPix  = 1.0 / 46080.0;
  const float weightIBL  = 1.0 / 26880.0;

  bool fillPP0only(name2DMap == "");
  std::string pospp0varx = namePP0 + "_pospp0x";
  std::string valvarp    = namePP0 + "_val";
  std::string posvarx    = name2DMap + "_em";
  std::string posvary    = name2DMap + "_pm";
  std::string valvarm    = name2DMap + "_val";

  for (unsigned int a = 0; a < PixMon::kNumModulesDisk; ++a) {
    auto posy = Monitored::Scalar<int>( posvary, a);
    for (unsigned int b = 0; b < PixMon::kNumLayersDisk; ++b) {
      // to find out (and fill together into one PP0-histogram bin)
      // array content of the modules belonging to the same sector (or PP0)
      // the translation (a-1)/6 is used
      // to show PP0 values from other disks of the same endcap
      // in the same plot 
      // the shift (b-1)*8 applies per disk counter b
      // (there are in total 8 sectors/disk)
      auto pospp0x = Monitored::Scalar<int>( pospp0varx, (a-1)/6 + (b-1)*8 + 1);
      auto posx    = Monitored::Scalar<int>( posvarx, b);
      auto valp    = Monitored::Scalar<float>( valvarp, pixarrays.DA[a][b]);
      auto valm    = Monitored::Scalar<float>( valvarm, pixarrays.DA[a][b]*weightPix);
      if (pixarrays.DA[a][b]>-1) {
	fill("ECA", pospp0x, valp);
	if (!fillPP0only) fill("ECA", posx, posy, valm);
      }
      valp = pixarrays.DC[a][b];
      valm = pixarrays.DC[a][b]*weightPix;
      if (pixarrays.DC[a][b]>-1) {
	fill("ECC", pospp0x, valp);
	if (!fillPP0only) fill("ECC", posx, posy, valm);
      } 
    }
  }

  for (unsigned int b = 0; b < PixMon::kNumModulesBarrel; ++b) {
    // translating array index into old Pixel module eta on a stave
    // i.e. 0..12 into -6..6 so that standard per-layer histograms 
    // declared by define2DProfHist method can be filled
    auto posx  = Monitored::Scalar<int>( posvarx, b-6);

    for (unsigned int a = 0; a < PixMon::kNumStavesL0; ++a) {
      auto posy    = Monitored::Scalar<int>( posvary, a);
      auto pospp0x = Monitored::Scalar<int>( pospp0varx, a);
      auto valp  = Monitored::Scalar<float>( valvarp, pixarrays.B0[a][b]);
      auto valm  = Monitored::Scalar<float>( valvarm, pixarrays.B0[a][b]*weightPix);
      if (pixarrays.B0[a][b]>-1) {
	fill("B0", pospp0x, valp);
	if (!fillPP0only) fill("B0", posx, posy, valm);
      }
    }
    for (unsigned int a = 0; a < PixMon::kNumStavesL1; ++a) {
      auto posy    = Monitored::Scalar<int>( posvary, a);
      auto pospp0x = Monitored::Scalar<int>( pospp0varx, a);
      auto valp  = Monitored::Scalar<float>( valvarp, pixarrays.B1[a][b]);
      auto valm  = Monitored::Scalar<float>( valvarm, pixarrays.B1[a][b]*weightPix);
      if (pixarrays.B1[a][b]>-1) {
	fill("B1", pospp0x, valp);
	if (!fillPP0only) fill("B1", posx, posy, valm);
      }
    }
    for (unsigned int a = 0; a < PixMon::kNumStavesL2; ++a) {
      auto posy    = Monitored::Scalar<int>( posvary, a);
      auto pospp0x = Monitored::Scalar<int>( pospp0varx, a);
      auto valp  = Monitored::Scalar<float>( valvarp, pixarrays.B2[a][b]);
      auto valm  = Monitored::Scalar<float>( valvarm, pixarrays.B2[a][b]*weightPix);
      if (pixarrays.B2[a][b]>-1) {
	fill("B2", pospp0x, valp);
	if (!fillPP0only) fill("B2", posx, posy, valm);
      }
    }
  }
  unsigned int nbina = PixMon::kNumStavesIBL;
  unsigned int nbinb = PixMon::kNumFEsIBL;
  for (unsigned int a = 0; a < nbina; ++a) {
    auto posy    = Monitored::Scalar<int>( posvary, a);
    auto pospp0x = Monitored::Scalar<int>( pospp0varx, a);
    for (unsigned int b = 0; b < nbinb; ++b) {
      // translating array index into IBL frontend eta on a stave
      // i.e. 0..31 into -16..15 so that standard per-layer histograms 
      // declared by define2DProfHist method can be filled 
      auto posx  = Monitored::Scalar<int>( posvarx, b-16);
      auto valp  = Monitored::Scalar<float>( valvarp, pixarrays.IBL[a][b]);
      auto valm  = Monitored::Scalar<float>( valvarm, pixarrays.IBL[a][b]*weightIBL);
      if (pixarrays.IBL[a][b]>-1) {
	if (b>0.5*nbinb) {
	  fill("IBLA", pospp0x, valp);
	} else {
	  fill("IBLC", pospp0x, valp);
	}
	if (!fillPP0only) fill("IBL", posx, posy, valm);
      }
    }
  }
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
