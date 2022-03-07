/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include "TgcHit.h"
#include "TString.h"

TGC::TgcHit::TgcHit(double x,double y,double z,
		    double shortWidth,double longWidth,double length,
		    bool isStrip,int gasGap,int channel,int stationEta,int stationPhi,int stationName, int bunch) :
  m_x{x},
  m_y{y},
  m_z{z},
  m_shortWidth{shortWidth},
  m_longWidth{longWidth},
  m_length{length},
  m_isStrip{isStrip},
  m_gasGap{gasGap},
  m_channel{channel},
  m_stationEta{stationEta},
  m_stationPhi{stationPhi},
  m_stationName{stationName},
  m_bunch{bunch},
  m_residual{-99999}
{
  initChamber(stationEta,stationPhi,stationName),
  m_gap_name = Form("%sL%02d%s",cham_name().data(),m_gasGap,(m_isStrip)?("S"):("W"));
}
