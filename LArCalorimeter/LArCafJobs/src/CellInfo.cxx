/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCafJobs/CellInfo.h"

#include "LArCafJobs/ShapeInfo.h"
#include "LArCafJobs/Geometry.h"
#include "LArCafJobs/ClassCounts.h"
#include <sstream>
#include <iostream>

using std::cout;
using std::endl;

using namespace LArSamples;

// Add raw here:--
CellInfo::CellInfo()
  : m_calo(UNKNOWN_CALO), m_layer(-1),
    m_iEta(-1), m_iPhi(-1), m_feedThrough(-1), 
    m_slot(-1), m_channel(-1), m_shapeL(0), m_shapeM(0), m_shapeH(0),
    m_rt(0), m_eta(0), m_phi(0), m_onlid(-1) 
{ 
  ClassCounts::incrementInstanceCount("CellInfo"); 
}


CellInfo::CellInfo(CaloId calo, short layer, 
                   short iEta, short iPhi, short feedThrough, short slot, short channel,
                   ShapeInfo* shapeL, ShapeInfo* shapeM, ShapeInfo* shapeH, 
                   const TVector3& position, ULong64_t onlid)
  : m_calo(calo),  m_layer(layer), 
    m_iEta(iEta), m_iPhi(iPhi), m_feedThrough(feedThrough), 
    m_slot(slot), m_channel(channel), 
    m_shapeL(shapeL), m_shapeM(shapeM), m_shapeH(shapeH), 
    m_rt(position.Pt()), m_eta(position.Eta()), m_phi(position.Phi()), m_onlid(onlid)
{ 
  ClassCounts::incrementInstanceCount("CellInfo"); 
}

CellInfo::CellInfo(const CellInfo& other, bool withShapes) 
  : m_calo(other.m_calo), m_layer(other.m_layer), 
    m_iEta(other.m_iEta), m_iPhi(other.m_iPhi), 
    m_feedThrough(other.m_feedThrough), m_slot(other.m_slot), m_channel(other.m_channel),
    m_shapeL(0), m_shapeM(0), m_shapeH(0), 
    m_rt(other.m_rt), m_eta(other.m_eta), m_phi(other.m_phi),  m_onlid(other.m_onlid)
{ 
  ClassCounts::incrementInstanceCount("CellInfo"); 
  if (withShapes && other.m_shapeL) m_shapeL = new ShapeInfo(*other.m_shapeL); 
  if (withShapes && other.m_shapeM) m_shapeM = new ShapeInfo(*other.m_shapeM); 
  if (withShapes && other.m_shapeH) m_shapeH = new ShapeInfo(*other.m_shapeH); 
}

CellInfo::CellInfo(CellInfo&& other) noexcept : m_calo(other.m_calo), m_layer(other.m_layer), 
    m_iEta(other.m_iEta), m_iPhi(other.m_iPhi), 
    m_feedThrough(other.m_feedThrough), m_slot(other.m_slot), m_channel(other.m_channel),
    m_shapeL(0), m_shapeM(0), m_shapeH(0), 
    m_rt(other.m_rt), m_eta(other.m_eta), m_phi(other.m_phi),  m_onlid(other.m_onlid)
{
    ClassCounts::incrementInstanceCount("CellInfo");
    std::swap(m_shapeL, other.m_shapeL);
    std::swap(m_shapeM, other.m_shapeM);
    std::swap(m_shapeH, other.m_shapeH);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

CellInfo::~CellInfo() 
{
  ClassCounts::decrementInstanceCount("CellInfo"); 
  if (m_shapeL) delete m_shapeL;
  if (m_shapeM) delete m_shapeM;
  if (m_shapeH) delete m_shapeH;
}


const ShapeInfo* CellInfo::shape(CaloGain::CaloGain gain) const
{
  switch (gain) {
    case CaloGain::LARLOWGAIN    : return m_shapeL;
    case CaloGain::LARMEDIUMGAIN : return m_shapeM;
    case CaloGain::LARHIGHGAIN   : return m_shapeH;
    default : return 0;
  }
  return 0;
}


bool CellInfo::setShape(CaloGain::CaloGain gain, ShapeInfo* shape)
{
  switch (gain) {
    case CaloGain::LARLOWGAIN    : m_shapeL = shape; return true;
    case CaloGain::LARMEDIUMGAIN : m_shapeM = shape; return true;
    case CaloGain::LARHIGHGAIN   : m_shapeH = shape; return true;
    default : return false;
  }
  return false;
}


short CellInfo::feb() const
{
  // slot starts at 1!
  return slot() - 1 + Geo::nSlots(partition())*feedThrough();
}


short CellInfo::globalFeb() const
{
  short offset = 0;
  PartitionId part;
  part = EMB_A_PARTITION;  if (partition() == part) return offset + feb(); offset += Geo::nSlots(part);
  part = EMB_C_PARTITION;  if (partition() == part) return offset + feb(); offset += Geo::nSlots(part);
  part = EMEC_A_PARTITION; if (partition() == part) return offset + feb(); offset += Geo::nSlots(part);
  part = EMEC_C_PARTITION; if (partition() == part) return offset + feb(); offset += Geo::nSlots(part);
  part = HEC_A_PARTITION;  if (partition() == part) return offset + feb(); offset += Geo::nSlots(part);
  part = HEC_C_PARTITION;  if (partition() == part) return offset + feb(); offset += Geo::nSlots(part);
  part = FCAL_A_PARTITION; if (partition() == part) return offset + feb(); offset += Geo::nSlots(part);
  part = FCAL_C_PARTITION; if (partition() == part) return offset + feb();
  return -1;
}


short CellInfo::globalPhiRing() const
{
  short offset = 0;
  CaloId cal[10] = { EMB_A, EMB_C, EMEC_OUTER_A, EMEC_OUTER_C, EMEC_INNER_A, EMEC_INNER_C, HEC_A, HEC_C, FCAL_A, FCAL_C };
  for (unsigned int c = 0; c < 10; c++)
    for (unsigned short l = Geo::firstLayer(cal[c]); l < Geo::firstLayer(cal[c]) + Geo::nLayers(cal[c]); l++)
      for (unsigned short r = 0; r < Geo::nRegions(cal[c], l); r++) {
        if (calo() == cal[c] && layer() == l && region() == r) return offset + (iEta() - Geo::firstEta(calo(), layer(), region()));
        offset += Geo::nEta(cal[c], l, r);
      }

  return -1;
}

TString CellInfo::location(int verbose) const
{
  TString loc = Id::str(calo());
  if (loc == Id::str(UNKNOWN_CALO)) return loc;
  
  if (verbose == 0) {
    std::ostringstream ss;
    ss << "(" << m_layer << ")";
    loc += " " + ss.str();
    return loc;
  }
  
  if (verbose == 1) {
    std::ostringstream ss;
    ss << "(" << layer() << ")" << " " << feedThrough() << "/" << slot() << "/" << channel();
    loc += " " + ss.str();
    return loc;
  }
  
  std::ostringstream ss;
  ss << "layer " << layer() << ", FT " << feedThrough()
     << ", slot " << slot() << ", channel " << channel()
     << ", (eta, phi) = " << Form("(%4.2f, %4.2f)", eta(), phi());

  if (verbose == 2) {
    loc += " " + ss.str();
    return loc;
  }
  ss  << ", (iEta, iPhi, region) = (" << iEta() << ", " << iPhi() << ", " << region() << ")";
  loc += " " + ss.str();
  return loc;
}


bool CellInfo::isValid() const
{
  //if (!m_shape || !m_shape->isValid()) return false;
  
  if (m_calo == UNKNOWN_CALO) return false;
  if (m_layer < 0) return false;
  if (m_iEta < 0) return false;
  if (m_iPhi < 0) return false;
  if (m_feedThrough < 0) return false;
  if (m_slot < 0) return false;
  if (m_channel < 0) return false;

  return true;
}


double CellInfo::footprint() const 
{ 
  return sizeof(*this) 
    + (m_shapeL ? m_shapeL->footprint() : 0)
    + (m_shapeM ? m_shapeM->footprint() : 0)
    + (m_shapeH ? m_shapeH->footprint() : 0);
}


 PartitionId CellInfo::partition() const
{
  return Id::partition(calo());
}


TVector3 CellInfo::position() const 
{ 
  TVector3 v;
  v.SetPtEtaPhi(m_rt, m_eta, m_phi); 
  return v;
}

