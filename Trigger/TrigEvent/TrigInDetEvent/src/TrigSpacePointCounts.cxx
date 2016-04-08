/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigSpacePointCounts.h"

//---------------------------------------------------------------

TrigSpacePointCounts::TrigSpacePointCounts(): m_pixelClusEndcapC(),
					      m_pixelClusBarrel(),
					      m_pixelClusEndcapA(),
					      m_droppedPixelModules(),
					      m_sctSpEndcapC(0),
					      m_sctSpBarrel(0),
					      m_sctSpEndcapA(0),
					      m_droppedSctModules() {
}

//---------------------------------------------------------------

TrigSpacePointCounts::TrigSpacePointCounts(const TrigHisto2D& pixelClusEndcapC,
					   const TrigHisto2D& pixelClusBarrel,
					   const TrigHisto2D& pixelClusEndcapA,
					   const std::vector<Identifier>& droppedPixelModules,
					   unsigned int sctSpEndcapC,
					   unsigned int sctSpBarrel,
					   unsigned int sctSpEndcapA,
					   const std::vector<Identifier>& droppedSctModules)
  : m_pixelClusEndcapC(pixelClusEndcapC),
    m_pixelClusBarrel(pixelClusBarrel),
    m_pixelClusEndcapA(pixelClusEndcapA),
    m_droppedPixelModules(droppedPixelModules),
  m_sctSpEndcapC(sctSpEndcapC),
  m_sctSpBarrel(sctSpBarrel),
  m_sctSpEndcapA(sctSpEndcapA),
  m_droppedSctModules(droppedSctModules)
{
}

//---------------------------------------------------------------

TrigSpacePointCounts::TrigSpacePointCounts(TrigHisto2D&& pixelClusEndcapC,
					   TrigHisto2D&& pixelClusBarrel,
					   TrigHisto2D&& pixelClusEndcapA,
					   std::vector<Identifier>&& droppedPixelModules,
					   unsigned int sctSpEndcapC,
					   unsigned int sctSpBarrel,
					   unsigned int sctSpEndcapA,
					   std::vector<Identifier>&& droppedSctModules)
  : m_pixelClusEndcapC(std::move(pixelClusEndcapC)),
    m_pixelClusBarrel(std::move(pixelClusBarrel)),
    m_pixelClusEndcapA(std::move(pixelClusEndcapA)),
    m_droppedPixelModules(std::move(droppedPixelModules)),
  m_sctSpEndcapC(sctSpEndcapC),
  m_sctSpBarrel(sctSpBarrel),
  m_sctSpEndcapA(sctSpEndcapA),
  m_droppedSctModules(std::move(droppedSctModules))
{
}

//---------------------------------------------------------------

TrigSpacePointCounts::TrigSpacePointCounts(const TrigSpacePointCounts& trigSpacePointCounts) {
  m_pixelClusEndcapC = trigSpacePointCounts.m_pixelClusEndcapC;
  m_pixelClusBarrel = trigSpacePointCounts.m_pixelClusBarrel;
  m_pixelClusEndcapA = trigSpacePointCounts.m_pixelClusEndcapA;
  m_sctSpEndcapC = trigSpacePointCounts.m_sctSpEndcapC;
  m_sctSpBarrel = trigSpacePointCounts.m_sctSpBarrel;
  m_sctSpEndcapA = trigSpacePointCounts.m_sctSpEndcapA;
}

//---------------------------------------------------------------

TrigSpacePointCounts::~TrigSpacePointCounts(void) {
}

//---------------------------------------------------------------

const TrigHisto2D& TrigSpacePointCounts::pixelClusEndcapC(void) const {
  return m_pixelClusEndcapC;
}

const TrigHisto2D& TrigSpacePointCounts::pixelClusBarrel(void) const {
  return m_pixelClusBarrel;
}

const TrigHisto2D& TrigSpacePointCounts::pixelClusEndcapA(void) const {
  return m_pixelClusEndcapA;
}

const std::vector<Identifier>& TrigSpacePointCounts::droppedPixelModules(void) const {
  return m_droppedPixelModules;
} 

unsigned int TrigSpacePointCounts::sctSpEndcapC(void) const {
  return m_sctSpEndcapC;
}

unsigned int TrigSpacePointCounts::sctSpBarrel(void) const {
  return m_sctSpBarrel;
}

unsigned int TrigSpacePointCounts::sctSpEndcapA(void) const {
  return m_sctSpEndcapA;
}

const std::vector<Identifier>& TrigSpacePointCounts::droppedSctModules(void) const {
  return m_droppedSctModules;
}

//---------------------------------------------------------------
