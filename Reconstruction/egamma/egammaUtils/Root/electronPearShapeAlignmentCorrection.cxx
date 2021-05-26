/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaUtils/electronPearShapeAlignmentCorrection.h"
#include "PathResolver/PathResolver.h"
#include "TFile.h"

namespace {
char const * const s_histoName = "sdeta";
}

electronPearShapeAlignmentCorrection::electronPearShapeAlignmentCorrection(
  const std::string& infile)
{
  const std::string path = "EGammaVariableCorrection/" + infile;
  std::unique_ptr<TFile> file =
    std::make_unique<TFile>(PathResolverFindCalibFile(path).c_str());
  if (!file) {
    throw std::runtime_error(
      "cannot find input file for pear shape correction");
  }

  m_sdeta = std::unique_ptr<TH2>(dynamic_cast<TH2*>(file->Get(s_histoName)));
  if (!m_sdeta) {
    throw std::runtime_error("cannot find histogram for pear shape correction");
  }
  m_sdeta->SetDirectory(nullptr);
}

double
electronPearShapeAlignmentCorrection::getDeltaEtaDistortion(
  const float eta,
  const float phi) const
{
  const int binEta = m_sdeta->GetXaxis()->FindBin(eta);
  const int binPhi = m_sdeta->GetYaxis()->FindBin(phi);
  const double dEtaDistortion = m_sdeta->GetBinContent(binEta, binPhi);
  return dEtaDistortion;
}

double
electronPearShapeAlignmentCorrection::getDeltaEtaCorrection(
  const float eta,
  const float phi) const
{
  return (-1. * getDeltaEtaDistortion(eta, phi));
}

