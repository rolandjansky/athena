#include <boost/tokenizer.hpp>
#include <cmath>
#include <string>
#include <vector>
#include <iostream>
#include "TrigInDetPattRecoTools/TrigTrackSeedGenerator.h"
#include "TrigInDetPattRecoTools/TrigCombinatorialSettings.h"
#include "TrigInDetPattRecoEvent/TrigInDetTriplet.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "PathResolver/PathResolver.h"
#include <fstream>
#include <chrono>


//Use spacepoints from a real event, run triplet making outside Athena
int main()
{
  typedef boost::tokenizer<boost::escaped_list_separator<char>> tokenizer;
  std::string file_name = "TrigInDetPattRecoTools/spacepoints.csv";
  std::ifstream in(PathResolver::find_file(file_name, "DATAPATH"));
  if (!in.is_open()) {
    std::cout << "ERROR: Cannot find " << file_name << std::endl;
    return 1;
  }
  std::vector<TrigSiSpacePointBase> convertedSpacePoints;

  std::string fline;
  while (getline(in,fline)) {
    std::vector<std::string> line_sep;
    tokenizer tok{fline};
    for (const auto &t : tok) {
      line_sep.push_back(std::string(t));
    }
    if (line_sep.size() == 5) {
      int layer = std::stoi(line_sep[0]);
      float r = std::stof(line_sep[1]);
      float phi = std::stof(line_sep[2]);
      float z = std::stof(line_sep[3]);
      bool isPixel = std::stoi(line_sep[4]);
      convertedSpacePoints.push_back(TrigSiSpacePointBase(layer, r, phi, z, isPixel));
    }
  }
  TrigCombinatorialSettings tcs;
  tcs.m_tripletPtMin = 0.3*1000.0;
  tcs.m_doubletFilterRZ = true;
  tcs.m_doublet_dR_Max = 270.0;
  tcs.m_seedRadBinWidth = 2.0;
  tcs.m_tripletD0Max      = 4.0;
  tcs.m_tripletD0_PPS_Max = 1.7;
  tcs.m_nMaxPhiSlice = 53;
  tcs.m_maxTripletBufferLength = 3;
  tcs.m_tripletDoPSS = false;
  tcs.m_tripletDtCut      = 10.0;
  tcs.m_zvError = 10.0;
  tcs.m_maxBarrelPix    = -1;
  tcs.m_minEndcapPix    = 8;
  tcs.m_maxEndcapPix    = -1;
  tcs.m_maxSiliconLayer = 32;

  std::vector<TRIG_INDET_SI_LAYER> pVL;
  pVL.resize(32);
  pVL.at(0).m_subdet = 1;
  pVL.at(0).m_type = 0;
  pVL.at(0).m_refCoord = 33.2443;
  pVL.at(0).m_minBound = -333.323;
  pVL.at(0).m_maxBound = 329.323;
  pVL.at(1).m_subdet = 1;
  pVL.at(1).m_type = 0;
  pVL.at(1).m_refCoord = 50.4567;
  pVL.at(1).m_minBound = -400.702;
  pVL.at(1).m_maxBound = 400.702;
  pVL.at(2).m_subdet = 1;
  pVL.at(2).m_type = 0;
  pVL.at(2).m_refCoord = 88.4566;
  pVL.at(2).m_minBound = -400.702;
  pVL.at(2).m_maxBound = 400.702;
  pVL.at(3).m_subdet = 1;
  pVL.at(3).m_type = 0;
  pVL.at(3).m_refCoord = 122.457;
  pVL.at(3).m_minBound = -400.702;
  pVL.at(3).m_maxBound = 400.702;
  pVL.at(4).m_subdet = 2;
  pVL.at(4).m_type = 0;
  pVL.at(4).m_refCoord = 299.501;
  pVL.at(4).m_minBound = -743.273;
  pVL.at(4).m_maxBound = 743.273;
  pVL.at(5).m_subdet = 2;
  pVL.at(5).m_type = 0;
  pVL.at(5).m_refCoord = 371;
  pVL.at(5).m_minBound = -743.273;
  pVL.at(5).m_maxBound = 743.273;
  pVL.at(6).m_subdet = 2;
  pVL.at(6).m_type = 0;
  pVL.at(6).m_refCoord = 443;
  pVL.at(6).m_minBound = -743.273;
  pVL.at(6).m_maxBound = 743.273;
  pVL.at(7).m_subdet = 2;
  pVL.at(7).m_type = 0;
  pVL.at(7).m_refCoord = 514;
  pVL.at(7).m_minBound = -743.273;
  pVL.at(7).m_maxBound = 743.273;
  pVL.at(8).m_subdet = 1;
  pVL.at(8).m_type = -2;
  pVL.at(8).m_refCoord = -495;
  pVL.at(8).m_minBound = 89.1479;
  pVL.at(8).m_maxBound = 149.795;
  pVL.at(9).m_subdet = 1;
  pVL.at(9).m_type = -2;
  pVL.at(9).m_refCoord = -580;
  pVL.at(9).m_minBound = 89.1479;
  pVL.at(9).m_maxBound = 149.795;
  pVL.at(10).m_subdet = 1;
  pVL.at(10).m_type = -2;
  pVL.at(10).m_refCoord = -650;
  pVL.at(10).m_minBound = 89.1479;
  pVL.at(10).m_maxBound = 149.795;
  pVL.at(11).m_subdet = 2;
  pVL.at(11).m_type = -2;
  pVL.at(11).m_refCoord = -856.843;
  pVL.at(11).m_minBound = 337.464;
  pVL.at(11).m_maxBound = 562.272;
  pVL.at(12).m_subdet = 2;
  pVL.at(12).m_type = -2;
  pVL.at(12).m_refCoord = -933.091;
  pVL.at(12).m_minBound = 274.928;
  pVL.at(12).m_maxBound = 562.272;
  pVL.at(13).m_subdet = 2;
  pVL.at(13).m_type = -2;
  pVL.at(13).m_refCoord = -1090.59;
  pVL.at(13).m_minBound = 274.928;
  pVL.at(13).m_maxBound = 562.272;
  pVL.at(14).m_subdet = 2;
  pVL.at(14).m_type = -2;
  pVL.at(14).m_refCoord = -1298.99;
  pVL.at(14).m_minBound = 274.928;
  pVL.at(14).m_maxBound = 562.272;
  pVL.at(15).m_subdet = 2;
  pVL.at(15).m_type = -2;
  pVL.at(15).m_refCoord = -1398.79;
  pVL.at(15).m_minBound = 274.928;
  pVL.at(15).m_maxBound = 562.272;
  pVL.at(16).m_subdet = 2;
  pVL.at(16).m_type = -2;
  pVL.at(16).m_refCoord = -1770.49;
  pVL.at(16).m_minBound = 274.928;
  pVL.at(16).m_maxBound = 562.272;
  pVL.at(17).m_subdet = 2;
  pVL.at(17).m_type = -2;
  pVL.at(17).m_refCoord = -2118.25;
  pVL.at(17).m_minBound = 337.464;
  pVL.at(17).m_maxBound = 562.272;
  pVL.at(18).m_subdet = 2;
  pVL.at(18).m_type = -2;
  pVL.at(18).m_refCoord = -2508.04;
  pVL.at(18).m_minBound = 402.836;
  pVL.at(18).m_maxBound = 562.272;
  pVL.at(19).m_subdet = 2;
  pVL.at(19).m_type = -2;
  pVL.at(19).m_refCoord = -2740.2;
  pVL.at(19).m_minBound = 438.426;
  pVL.at(19).m_maxBound = 562.272;
  pVL.at(20).m_subdet = 1;
  pVL.at(20).m_type = 2;
  pVL.at(20).m_refCoord = 495;
  pVL.at(20).m_minBound = 89.1479;
  pVL.at(20).m_maxBound = 149.795;
  pVL.at(21).m_subdet = 1;
  pVL.at(21).m_type = 2;
  pVL.at(21).m_refCoord = 580;
  pVL.at(21).m_minBound = 89.1479;
  pVL.at(21).m_maxBound = 149.795;
  pVL.at(22).m_subdet = 1;
  pVL.at(22).m_type = 2;
  pVL.at(22).m_refCoord = 650;
  pVL.at(22).m_minBound = 89.1479;
  pVL.at(22).m_maxBound = 149.795;
  pVL.at(23).m_subdet = 2;
  pVL.at(23).m_type = 2;
  pVL.at(23).m_refCoord = 856.843;
  pVL.at(23).m_minBound = 337.464;
  pVL.at(23).m_maxBound = 562.272;
  pVL.at(24).m_subdet = 2;
  pVL.at(24).m_type = 2;
  pVL.at(24).m_refCoord = 933.091;
  pVL.at(24).m_minBound = 274.928;
  pVL.at(24).m_maxBound = 562.272;
  pVL.at(25).m_subdet = 2;
  pVL.at(25).m_type = 2;
  pVL.at(25).m_refCoord = 1090.59;
  pVL.at(25).m_minBound = 274.928;
  pVL.at(25).m_maxBound = 562.272;
  pVL.at(26).m_subdet = 2;
  pVL.at(26).m_type = 2;
  pVL.at(26).m_refCoord = 1298.99;
  pVL.at(26).m_minBound = 274.928;
  pVL.at(26).m_maxBound = 562.272;
  pVL.at(27).m_subdet = 2;
  pVL.at(27).m_type = 2;
  pVL.at(27).m_refCoord = 1398.79;
  pVL.at(27).m_minBound = 274.928;
  pVL.at(27).m_maxBound = 562.272;
  pVL.at(28).m_subdet = 2;
  pVL.at(28).m_type = 2;
  pVL.at(28).m_refCoord = 1770.49;
  pVL.at(28).m_minBound = 274.928;
  pVL.at(28).m_maxBound = 562.272;
  pVL.at(29).m_subdet = 2;
  pVL.at(29).m_type = 2;
  pVL.at(29).m_refCoord = 2118.25;
  pVL.at(29).m_minBound = 337.464;
  pVL.at(29).m_maxBound = 562.272;
  pVL.at(30).m_subdet = 2;
  pVL.at(30).m_type = 2;
  pVL.at(30).m_refCoord = 2508.04;
  pVL.at(30).m_minBound = 402.836;
  pVL.at(30).m_maxBound = 562.272;
  pVL.at(31).m_subdet = 2;
  pVL.at(31).m_type = 2;
  pVL.at(31).m_refCoord = 2740.2;
  pVL.at(31).m_minBound = 438.426;
  pVL.at(31).m_maxBound = 562.272;
  tcs.m_layerGeometry = pVL;

  std::vector<int> times;
  for (unsigned int i = 0; i < 10; ++i) {
    auto t0 = std::chrono::high_resolution_clock::now();
    TRIG_TRACK_SEED_GENERATOR seedGen(tcs);

    seedGen.loadSpacePoints(convertedSpacePoints);

    std::unique_ptr<TrigRoiDescriptor> tmpRoi = std::make_unique<TrigRoiDescriptor>(true);
    seedGen.createSeeds(tmpRoi.get());
    std::vector<TrigInDetTriplet> triplets;
    seedGen.getSeeds(triplets);
    if (triplets.size() != 24511) {
      std::cout << "ERROR: change in number of triplets created" << std::endl;
      std::cout << "ERROR: expected: 24511, actual " << triplets.size() << std::endl;
      return 1;
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    times.push_back(std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count());
  }
  float mean_time = std::accumulate(times.begin(), times.end(), 0)/float(times.size());
  
  float dev_time = 0;
  for (unsigned int i = 0; i < times.size(); i++) {
    dev_time += (times[i] - mean_time)*(times[i] - mean_time);
  }
  dev_time = std::sqrt(dev_time/times.size());
    
  std::cout << "Seed making time: " << mean_time  << " +/- " << dev_time << " microseconds" << std::endl;
  
  return 0;
}

