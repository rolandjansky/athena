/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TileConditions/TileWienerFilterWeights.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include "PathResolver/PathResolver.h"

using std::memset;

TileWienerFilterWeights::TileWienerFilterWeights()
  : m_Luminosity(40)
  , m_loaded(false)
  , m_weights(nullptr)
{
}

TileWienerFilterWeights::~TileWienerFilterWeights()
{
  if (m_weights) delete m_weights; 
}

void TileWienerFilterWeights::loadWeights(MsgStream &log)
{
  log<<MSG::DEBUG<<"TileWienerFilterWeights::loadWeights"<<endmsg;

  m_weights = new TileWienerFilterWeightsStruct;
  memset(m_weights,0,sizeof(TileWienerFilterWeightsStruct));

  // set luminosity
  m_weights->luminosity = m_Luminosity;
  log<<MSG::DEBUG<<" Luminosity="<<m_Luminosity<<endmsg;

  /////////////////////////////////////////////////////////////////////////////
  // GENERAL WEIGHTS
  std::string file_name_gn = "wiener_General_mu"+std::to_string(m_Luminosity)+".dat";
  std::string file_path_gn = PathResolver::find_file(file_name_gn, "DATAPATH");
  std::ifstream file_gn(file_path_gn.c_str());
  std::string line_gn;

  if (file_gn.is_open()) {
    log<<MSG::DEBUG<<file_name_gn<<" opened"<<endmsg;

    int partition, channel;

    while (std::getline(file_gn, line_gn)) {
      // skip empty or comment lines
      if (line_gn.size() == 0 || line_gn[0] == '#') continue;

      // read values
      std::istringstream iss(line_gn);
      iss>>partition;
      iss>>channel;
      if (partition > -1 && partition < 4 && channel > -1 && channel < 48) {
        for (int i=0; i<8; i++) {
          iss>>m_weights->generalWeights[partition][channel][i];
        }
      }
    }

    file_gn.close();
  } else {
    log<<MSG::FATAL<<file_name_gn<<" not found"<<endmsg;
    return;
  }
  /////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////
  // OPTIMAL WEIGHTS
  std::string file_name_op = "wiener_Optimal_mu"+std::to_string(m_Luminosity)+".dat";
  std::string file_path_op = PathResolver::find_file(file_name_op, "DATAPATH");
  std::ifstream file_op(file_path_op.c_str());
  std::string line_op;

  if (file_op.is_open()) {
    log<<MSG::DEBUG<<file_name_op<<" opened"<<endmsg;

    int bcidIndex = 0;

    while (std::getline(file_op, line_op)) {
      // skip empty or comment lines
      if (line_op.size() == 0 || line_op[0] == '#') continue;

      // read values
      std::istringstream iss(line_op);
      for (int i=0; i<8; i++) {
        iss>>m_weights->optimalWeights[bcidIndex][i];
      }
      bcidIndex++;
    }

    file_op.close();
  } else {
    log<<MSG::FATAL<<file_name_op<<" not found"<<endmsg;
    return;
  }
  /////////////////////////////////////////////////////////////////////////////

  m_loaded = true;
  log<<MSG::DEBUG<<" WienerFilter Weights loaded"<<endmsg;
}
