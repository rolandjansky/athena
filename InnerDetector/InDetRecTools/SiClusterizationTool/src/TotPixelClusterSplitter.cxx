/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SiClusterizationTool/TotPixelClusterSplitter.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"

#include "InDetPrepRawData/PixelCluster.h"

#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "PixelConditionsServices/IPixelCalibSvc.h"

InDet::TotPixelClusterSplitter::TotPixelClusterSplitter(const std::string & type,
  const std::string & name,
  const IInterface * parent) :
  AthAlgTool(type, name, parent),
  m_calibSvc("PixelCalibSvc", name),
  m_minPixels(3),
  m_maxPixels(25),
  m_doLongPixels(true)
{
  declareInterface<IPixelClusterSplitter>(this);
  declareProperty("PixelCalibSvc", m_calibSvc);
}

InDet::TotPixelClusterSplitter::~TotPixelClusterSplitter()
{}

StatusCode InDet::TotPixelClusterSplitter::initialize()
{
  CHECK(m_calibSvc.retrieve());

  return StatusCode::SUCCESS;
}

StatusCode InDet::TotPixelClusterSplitter::finalize()
{
//  ATH_MSG_INFO("finalize()");
  return StatusCode::SUCCESS;
}

std::vector<InDet::PixelClusterParts> InDet::TotPixelClusterSplitter::splitCluster(
  const InDet::PixelCluster & OrigCluster) const
{
  std::vector<InDet::PixelClusterParts> Parts;
//  ATH_MSG_INFO("splitCluster() called!");

  const std::vector<Identifier> & Rdos = OrigCluster.rdoList();
  const unsigned int NumPixels = static_cast<unsigned int>(Rdos.size());
  const InDetDD::SiDetectorElement * Element = OrigCluster.detectorElement();

  // Check cluster-size bounds.
  if (NumPixels < m_minPixels || NumPixels > m_maxPixels) return Parts;

  InDetDD::SiCellId * CellIds = new InDetDD::SiCellId[NumPixels];

  // Detect special pixels and exclude them if necessary.
  // Veto removed on 28-7-2011 by K. Barry - residual-level 
  // studies show RMS improvement when ganged-pixel containing 
  // clusters are split.
  for (unsigned int i = 0; i < NumPixels; i++)
  {
    CellIds[i] = Element->cellIdFromIdentifier(Rdos[i]);
/*
    int PixType = pixelType(CellIds[i].phiIndex(), CellIds[i].etaIndex());
    if (PixType != 0)
    {
      if (PixType == 1 && m_doLongPixels);
      else
      {
         delete CellIds;
         return Parts;
      }
    }
*/
  }

  // Determine maximum and minimum phi and eta indices.
  int TempVar[4] = { 0, 0, 0, 0 };
  for (unsigned int i = 0; i < NumPixels; i++)
  {
    const int phiIdx = CellIds[i].phiIndex();
    const int etaIdx = CellIds[i].etaIndex();
    if (i == 0)
    {
      TempVar[0] = phiIdx;
      TempVar[1] = phiIdx;
      TempVar[2] = etaIdx;
      TempVar[3] = etaIdx;
    }
    else
    {
      if (phiIdx < TempVar[0]) TempVar[0] = phiIdx;
      else if (phiIdx > TempVar[1]) TempVar[1] = phiIdx;

      if (etaIdx < TempVar[2]) TempVar[2] = etaIdx;
      else if (etaIdx > TempVar[3]) TempVar[3] = etaIdx;
    }
  }
  const int MinPhiIdx = TempVar[0];
  const int MaxPhiIdx = TempVar[1];
  const int MinEtaIdx = TempVar[2];
  const int MaxEtaIdx = TempVar[3];
  const int PhiWidth = MaxPhiIdx - MinPhiIdx + 1;
  const int EtaWidth = MaxEtaIdx - MinEtaIdx + 1;
  
  // In the future, might want to add a check here to see if the charge list is empty...
  const std::vector<float> & Charges = OrigCluster.chargeList();

  std::vector<float> PhiChargeHist(PhiWidth, 0.);
  unsigned int NumPhiMinima = 0;
  std::vector<unsigned int> PhiMinimaLoc(PhiWidth/2);
  std::vector<float> PhiMinimaQuality(PhiWidth/2);
  if (PhiWidth > 2)
  {
    // Fill up the phi-charge histogram. The values were initialized to zero by the vector constructor.
    for (unsigned int i = 0; i < NumPixels; i++)
      PhiChargeHist[CellIds[i].phiIndex() - MinPhiIdx] += Charges[i];

    // Minima search
    for (int i = 1; i < PhiWidth - 1; i++)
    {
      if (PhiChargeHist[i-1] <= PhiChargeHist[i] || PhiChargeHist[i] >= PhiChargeHist[i+1]) continue;

      PhiMinimaLoc[NumPhiMinima] = static_cast<unsigned int>(i);
      PhiMinimaQuality[NumPhiMinima++] = (PhiChargeHist[i+1] + PhiChargeHist[i-1] - 2.*PhiChargeHist[i]) /
                                      (PhiChargeHist[i+1] + PhiChargeHist[i-1]);
    }
  }
    
  std::vector<float> EtaChargeHist(EtaWidth, 0.);
  unsigned int NumEtaMinima = 0;
  std::vector<unsigned int> EtaMinimaLoc(EtaWidth/2);
  std::vector<float> EtaMinimaQuality(EtaWidth/2);
  const float LongPixScale = 2./3.;
  if (EtaWidth > 2)
  {
    float Scale;
    // Fill up the eta-charge histogram. The values were initialized to zero by the vector constructor.
    for (unsigned int i = 0; i < NumPixels; i++)
    {
      const int PixType = pixelType(CellIds[i].phiIndex(), CellIds[i].etaIndex());
      if ( (PixType % 2) == 1 )
        Scale = LongPixScale;
      else
        Scale = 1.;
      EtaChargeHist[CellIds[i].etaIndex() - MinEtaIdx] += Scale*Charges[i];
    }

    // Minima search
    for (int i = 1; i < EtaWidth - 1; i++)
    {
      if (EtaChargeHist[i-1] <= EtaChargeHist[i] || EtaChargeHist[i] >= EtaChargeHist[i+1]) continue;

      EtaMinimaLoc[NumEtaMinima] = static_cast<unsigned int>(i);
      EtaMinimaQuality[NumEtaMinima++] = (EtaChargeHist[i+1] + EtaChargeHist[i-1] - 2.*EtaChargeHist[i]) /
                                         (EtaChargeHist[i+1] + EtaChargeHist[i-1]);
    }
  }

  // Select the "best" minimum for splitting
  unsigned int bestMin = 0;
  float bestQual = -1.;
  SplitType Type = NoSplit;
  for (unsigned int i = 0; i < NumPhiMinima; i++)
  {
    if (PhiMinimaQuality[i] <= bestQual) continue;
    Type = PhiSplit;
    bestMin = PhiMinimaLoc[i];
    bestQual = PhiMinimaQuality[i];
  }
  for (unsigned int i = 0; i < NumEtaMinima; i++)
  {
    if (EtaMinimaQuality[i] <= bestQual) continue;
    Type = EtaSplit;
    bestMin = EtaMinimaLoc[i];
    bestQual = EtaMinimaQuality[i];
  }
  const int SplitIndex = static_cast<int>(bestMin);

  // Now it's time to do the split!
  if (Type == NoSplit)
  {
    delete [] CellIds;
    return Parts;
  }
  if (Type != PhiSplit && Type != EtaSplit)
  {
    // Should never happen...major bug someplace if it does
    ATH_MSG_ERROR("<InDet::TotPixelClusterSplitter::splitCluster> : Unrecognized SplitType!");
    delete [] CellIds;
    return Parts;
  }

  int Idx;
  int (InDetDD::SiCellId:: * const IndexFunc) () const = (Type == PhiSplit) ? &InDetDD::SiCellId::phiIndex
                                                                            : &InDetDD::SiCellId::etaIndex;
  const int LowIdx = (Type == PhiSplit) ? MinPhiIdx : MinEtaIdx;

  // Variables for PixelClusterParts
  std::vector<Identifier> SplitRdos[2];
  std::vector<int> Totgroups[2];
  std::vector<int> Lvl1groups[2];

  const std::vector<int> OrigTots = OrigCluster.totList();
  const int Lvl1a = OrigCluster.LVL1A();
  
  const AtlasDetectorID* aid = Element->getIdHelper();
  if (aid==0)
  {
    ATH_MSG_ERROR("Could not get ATLASDetectorID");
  }
  const PixelID* pixelIDp=dynamic_cast<const PixelID*>(aid);
  if (!pixelIDp){
    ATH_MSG_ERROR("Could not get PixelID pointer");
  } 
  //  const PixelID& pixelID = *pixelIDp;

  
  for (unsigned int i = 0; i < NumPixels; i++)
  {
    Idx = (CellIds[i].*IndexFunc)() - LowIdx;
    if (Idx < SplitIndex)
    {
      SplitRdos[0].push_back(Rdos[i]);
      Totgroups[0].push_back(OrigTots[i]);
      Lvl1groups[0].push_back(Lvl1a);
    }
    else if (Idx > SplitIndex)
    {
      SplitRdos[1].push_back(Rdos[i]);
      Totgroups[1].push_back(OrigTots[i]);
      Lvl1groups[1].push_back(Lvl1a);
    }
    else // only == remains
    {
      for (int j = 0; j < 2; j++)
      {
        SplitRdos[j].push_back(Rdos[i]);
        Totgroups[j].push_back(static_cast<int>(m_calibSvc->getTotMean(Rdos[i],Charges[i]/2.)));
        Lvl1groups[j].push_back(Lvl1a);
      }
    }
  }

  Parts.push_back(InDet::PixelClusterParts(SplitRdos[0], Totgroups[0], Lvl1groups[0]));
  Parts.push_back(InDet::PixelClusterParts(SplitRdos[1], Totgroups[1], Lvl1groups[1]));
    
  delete [] CellIds;
  return Parts;
}

std::vector<InDet::PixelClusterParts> InDet::TotPixelClusterSplitter::splitCluster(
  const InDet::PixelCluster & OrigCluster, 
  const InDet::PixelClusterSplitProb &) const
{
  return splitCluster(OrigCluster);
}

int InDet::TotPixelClusterSplitter::pixelType(const int PhiIdx, const int EtaIdx) const
{
  if (!(EtaIdx%18 == 0 || EtaIdx%18 == 17))
  {
    if (PhiIdx==152 || PhiIdx==154 || PhiIdx==156 || PhiIdx==158 ||
        PhiIdx==169 || PhiIdx==171 || PhiIdx==173 || PhiIdx==175)
      return 4; // short inter-ganged pixel
    else if (PhiIdx==153 || PhiIdx==155 || PhiIdx==157 || PhiIdx==159 ||
             (PhiIdx > 159 && PhiIdx < 168) ||
             PhiIdx==168 || PhiIdx==170 || PhiIdx==172 || PhiIdx==174)
      return 2; // short ganged pixel
    else
      return 0; // short normal pixel
  }
  else
  {
    if (PhiIdx==152 || PhiIdx==154 || PhiIdx==156 || PhiIdx==158 ||
        PhiIdx==169 || PhiIdx==171 || PhiIdx==173 || PhiIdx==175)
      return 5; // long inter-ganged pixel
    else if (PhiIdx==153 || PhiIdx==155 || PhiIdx==157 || PhiIdx==159 ||
             (PhiIdx > 159 && PhiIdx < 168) ||
             PhiIdx==168 || PhiIdx==170 || PhiIdx==172 || PhiIdx==174)
      return 3; // long ganged pixel
    else
      return 1; // long normal pixel
  }
}

