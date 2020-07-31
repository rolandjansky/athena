/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <cassert>
#include <string>
#include <map>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <limits>

#include <TFile.h>
#include <TH1F.h>
#include <TObjString.h>

#include "PathResolver/PathResolver.h"

#include "egammaLayerRecalibTool/egammaLayerRecalibTool.h"

const float VALUE_OVERFLOW = std::numeric_limits<float>::max();

float GetAmountHisto1D::operator()(const StdCalibrationInputs & input) const {
  const int bin = m_histo->FindFixBin(input.eta);
  if (m_histo->IsBinUnderflow(bin) or m_histo->IsBinOverflow(bin)) return VALUE_OVERFLOW;
  return m_histo->GetBinContent(bin);
}

float GetAmountHisto1DUp::operator()(const StdCalibrationInputs & input) const {
  const int bin = m_histo->FindFixBin(input.eta);
  if (m_histo->IsBinUnderflow(bin) or m_histo->IsBinOverflow(bin)) return VALUE_OVERFLOW;
  return m_histo->GetBinContent(bin) + m_histo->GetBinError(bin);
}

float GetAmountHisto1DDown::operator()(const StdCalibrationInputs & input) const {
  const int bin = m_histo->FindFixBin(input.eta);
  if (m_histo->IsBinUnderflow(bin) or m_histo->IsBinOverflow(bin)) return VALUE_OVERFLOW;
  return m_histo->GetBinContent(bin) - m_histo->GetBinError(bin);
}

float GetAmountHisto1DErrorUp::operator()(const StdCalibrationInputs & input) const {
  const int bin = m_histo->FindFixBin(input.eta);
  if (m_histo->IsBinUnderflow(bin) or m_histo->IsBinOverflow(bin)) return VALUE_OVERFLOW;
  return m_histo->GetBinError(bin);
}

float GetAmountHisto1DErrorDown::operator()(const StdCalibrationInputs & input) const {
  const int bin = m_histo->FindFixBin(input.eta);
  if (m_histo->IsBinUnderflow(bin) or m_histo->IsBinOverflow(bin)) return VALUE_OVERFLOW;
  return -m_histo->GetBinError(bin);
}

float GetAmountHisto2D::operator()(const StdCalibrationInputs & input) const {
  const int bin = m_histo.FindFixBin(input.eta, input.phi);
  if (m_histo.IsBinUnderflow(bin) or m_histo.IsBinOverflow(bin)) return VALUE_OVERFLOW;
  return m_histo.GetBinContent(bin);
}

float GetAmountFixed::operator()(const StdCalibrationInputs & /*input*/ ) const {
  return m_amount;
}

float GetAmountFormula::operator()(const StdCalibrationInputs & input) const {
  return m_formula.Eval(input.eta, input.phi, input.RunNumber);
}


float GetAmountHVPSGuillaume::operator()(const StdCalibrationInputs & input) const {
  return m_tool.getCorr(input.RunNumber, input.eta, input.phi);
}


CP::CorrectionCode InputModifier::operator()(StdCalibrationInputs& inputs, float amount) const
{
  if (amount == VALUE_OVERFLOW) return CP::CorrectionCode::OutOfValidityRange;
  switch (m_base)
  {
    case SHIFT: scale_inputs(inputs, amount); return CP::CorrectionCode::Ok;
    case ZEROBASED: scale_inputs(inputs, 1. + amount); return CP::CorrectionCode::Ok;
    case ONEBASED: scale_inputs(inputs, amount); return CP::CorrectionCode::Ok;
    case ONEBASED_ALPHA: scale_inputs(inputs, 1. / amount); return CP::CorrectionCode::Ok;
    case ZEROBASED_ALPHA: scale_inputs(inputs, 1. / (1. + amount)); return CP::CorrectionCode::Ok;
    default: return CP::CorrectionCode::Error;
  };
}

void ScaleE0::scale_inputs(StdCalibrationInputs & inputs, float amount) const
{
  inputs.E0raw *= amount;
}

void ScaleE1::scale_inputs(StdCalibrationInputs & inputs, float amount) const
{
  inputs.E1raw *= amount;
}

void ScaleE2::scale_inputs(StdCalibrationInputs & inputs, float amount) const
{
  inputs.E2raw *= amount;
}

void ScaleE3::scale_inputs(StdCalibrationInputs & inputs, float amount) const
{
  inputs.E3raw *= amount;
}

void ScaleE1overE2::scale_inputs(StdCalibrationInputs & inputs, float amount) const
{
  const double Es1 = inputs.E1raw;
  const double Es2 = inputs.E2raw;
  if (Es1 == 0 and Es2 == 0) {
    inputs.E1raw = -999;
    inputs.E2raw = -999;
    return;
  }
  const double sum = Es1 + Es2;
  const double alpha = amount;
  const double den = (alpha * Es1 + Es2);
  inputs.E1raw = alpha * Es1 * sum / den;
  inputs.E2raw = Es2 * sum / den;
}

void ScaleEaccordion::scale_inputs(StdCalibrationInputs & inputs, float amount) const
{
  inputs.E1raw *= amount;
  inputs.E2raw *= amount;
  inputs.E3raw *= amount;
}

void ScaleEcalorimeter::scale_inputs(StdCalibrationInputs & inputs, float amount) const
{
  inputs.E0raw *= amount;
  inputs.E1raw *= amount;
  inputs.E2raw *= amount;
  inputs.E3raw *= amount;
}


std::string egammaLayerRecalibTool::resolve_alias(const std::string& tune) const {

  if ("layer1_2012" == tune)         return "layer1_2012_v5";
  if ("layer1_alt_2012" == tune)     return "layer1_alt_2012_v5";
  if ("layer1_2011" == tune)         return "layer1_2011_v5";
  if ("layer1_alt_2011" == tune)     return "layer1_alt_2011_v5";
  if ("layer1_2010" == tune)         return "layer1_2010_v5";
  if ("ps_2012" == tune)             return "ps_2012_v3";
  if ("ps_2011" == tune)             return "ps_2011_v3";
  if ("ps_2010" == tune)             return "ps_2010_v3";
  if ("layer1_2012_up" == tune)      return "layer1_2012_v5_up";
  if ("layer1_2012_down" == tune)    return "layer1_2012_v5_down";
  if ("layer1_2012_errup" == tune)   return "layer1_2012_v5_errup";
  if ("layer1_2012_errdown" == tune) return "layer1_2012_v5_errdown";
  if ("layer1_2011_up" == tune)      return "layer1_2011_v5_up";
  if ("layer1_2011_down" == tune)    return "layer1_2011_v5_down";
  if ("layer1_2011_errup" == tune)   return "layer1_2011_v5_errup";
  if ("layer1_2011_errdown" == tune) return "layer1_2011_v5_errdown";
  if ("layer1_2010_up" == tune)      return "layer1_2010_v5_up";
  if ("layer1_2010_down" == tune)    return "layer1_2010_v5_down";
  if ("layer1_2010_errup" == tune)   return "layer1_2010_v5_errup";
  if ("layer1_2010_errdown" == tune) return "layer1_2010_v5_errdown";
  if ("ps_2012_up" == tune)          return "ps_2012_v3_up";
  if ("ps_2012_down" == tune)        return "ps_2012_v3_down";
  if ("ps_2012_errup" == tune)       return "ps_2012_v3_errup";
  if ("ps_2012_errdown" == tune)     return "ps_2012_v3_errdown";
  if ("ps_2011_up" == tune)          return "ps_2011_v3_up";
  if ("ps_2011_down" == tune)        return "ps_2011_v3_down";
  if ("ps_2011_errup" == tune)       return "ps_2011_v3_errup";
  if ("ps_2011_errdown" == tune)     return "ps_2011_v3_errdown";
  if ("ps_2010_up" == tune)          return "ps_2010_v3_up";
  if ("ps_2010_down" == tune)        return "ps_2010_v3_down";
  if ("ps_2010_errup" == tune)       return "ps_2010_v3_errup";
  if ("ps_2010_errdown" == tune)     return "ps_2010_v3_errdown";

  return tune;
}

void egammaLayerRecalibTool::add_scale(const std::string& tuneIn)
{
  ATH_MSG_INFO("using scale " << tuneIn);
  std::string tune = resolve_alias(tuneIn);

  if (tune.empty()) { }
  // 2012
  else if ("2012" == tune) {
    add_scale("ps_HV1");
    add_scale("layer1_2012");
    add_scale("ps_2012");
  }
  else if("2012_with_layer2" == tune) {
    add_scale("ps_HV1");
    add_scale("layer2_2012_v5");
    add_scale("ps_2012");
  }
  else if ("2012_alt" == tune) {
    add_scale("ps_HV1");
    add_scale("layer1_alt_2012");
    add_scale("ps_2012");
  }
  else if("2012_alt_with_layer2" == tune) {
    add_scale("ps_HV1");
    add_scale("layer2_alt_2012_v5");
    add_scale("ps_2012");
  }
  else if("2010_with_layer2" == tune) {
    add_scale("layer2_2010_v5");
    add_scale("ps_2010");
  }
  else if ("2012_layer1_up" == tune) {
    add_scale("ps_HV1");
    add_scale("layer1_2012_up");
    add_scale("ps_2012");
  }
  else if ("2012_layer1_down" == tune) {
    add_scale("ps_HV1");
    add_scale("layer1_2012_down");
    add_scale("ps_2012");
  }
  else if ("2012_layer1_errup" == tune) {
    add_scale("layer1_2012_errup");
  }
  else if ("2012_layer1_errdown" == tune) {
    add_scale("layer1_2012_errdown");
  }
  else if ("2012_ps_down" == tune) {
    add_scale("ps_HV1");
    add_scale("layer1_2012");
    add_scale("ps_2012_down");
  }
  else if ("2012_ps_up" == tune) {
    add_scale("ps_HV1");
    add_scale("layer1_2012");
    add_scale("ps_2012_up");
  }
  else if ("2012_ps_errdown" == tune) {
    add_scale("ps_2012_errdown");
  }
  else if ("2012_ps_errup" == tune) {
    add_scale("ps_2012_errup");
  }
  else if ("2012_up" == tune) {
    add_scale("ps_HV1");
    add_scale("layer1_2012_up");
    add_scale("ps_2012_up");
  }
  else if ("2012_down" == tune) {
    add_scale("ps_HV1");
    add_scale("layer1_2012_down");
    add_scale("ps_2012_down");
  }
  else if ("2012_errup" == tune) {
    add_scale("layer1_2012_errup");
    add_scale("ps_2012_errup");
  }
  else if ("2012_errdown" == tune) {
    add_scale("layer1_2012_errdown");
    add_scale("ps_2012_errdown");
  }
  // 2011
  else if ("2011" == tune) {
    add_scale("layer1_2011");
    add_scale("ps_2011");
  }
  else if("2011_with_layer2" == tune) {
    add_scale("layer2_2011_v5");
    add_scale("ps_2011");
  }
  else if ("2011_alt" == tune) {
    add_scale("layer1_alt_2011");
    add_scale("ps_2011");
  }
  else if("2011_alt_with_layer2" == tune) {
    add_scale("layer2_alt_2011_v5");
    add_scale("ps_2011");
  }
  else if ("2011_layer1_up" == tune) {
    add_scale("layer1_2011_up");
    add_scale("ps_2011");
  }
  else if ("2011_layer1_down" == tune) {
    add_scale("layer1_2011_down");
    add_scale("ps_2011");
  }
  else if ("2011_layer1_errup" == tune) {
    add_scale("layer1_2011_errup");
  }
  else if ("2011_layer1_errdown" == tune) {
    add_scale("layer1_2011_errdown");
  }
  else if ("2011_ps_down" == tune) {
    add_scale("layer1_2011");
    add_scale("ps_2011_down");
  }
  else if ("2011_ps_up" == tune) {
    add_scale("layer1_2011");
    add_scale("ps_2011_up");
  }
  else if ("2011_ps_errdown" == tune) {
    add_scale("ps_2011_errdown");
  }
  else if ("2011_ps_errup" == tune) {
    add_scale("ps_2011_errup");
  }
  else if ("2011_up" == tune) {
    add_scale("layer1_2011_up");
    add_scale("ps_2011_up");
  }
  else if ("2011_down" == tune) {
    add_scale("layer1_2011_down");
    add_scale("ps_2011_down");
  }
  else if ("2011_errup" == tune) {
    add_scale("layer1_2011_errup");
    add_scale("ps_2011_errup");
  }
  else if ("2011_errdown" == tune) {
    add_scale("layer1_2011_errdown");
    add_scale("ps_2011_errdown");
  }
  // 2010
  else if ("2010" == tune) {
    add_scale("layer1_2010");
    add_scale("ps_2010");
  }
  else if ("2010_layer1_up" == tune) {
    add_scale("layer1_2010_up");
    add_scale("ps_2010");
  }
  else if ("2010_layer1_down" == tune) {
    add_scale("layer1_2010_down");
    add_scale("ps_2010");
  }
  else if ("2010_layer1_errup" == tune) {
    add_scale("layer1_2010_errup");
  }
  else if ("2010_layer1_errdown" == tune) {
    add_scale("layer1_2010_errdown");
  }
  else if ("2010_ps_down" == tune) {
    add_scale("layer1_2010");
    add_scale("ps_2010_down");
  }
  else if ("2010_ps_up" == tune) {
    add_scale("layer1_2010");
    add_scale("ps_2010_up");
  }
  else if ("2010_ps_errdown" == tune) {
    add_scale("ps_2010_errdown");
  }
  else if ("2010_ps_errup" == tune) {
    add_scale("ps_2010_errup");
  }
  else if ("2010_up" == tune) {
    add_scale("layer1_2010_up");
    add_scale("ps_2010_up");
  }
  else if ("2010_down" == tune) {
    add_scale("layer1_2010_down");
    add_scale("ps_2010_down");
  }
  else if ("2010_errup" == tune) {
    add_scale("layer1_2010_errup");
    add_scale("ps_2010_errup");
  }
  else if ("2010_errdown" == tune) {
    add_scale("layer1_2010_errdown");
    add_scale("ps_2010_errdown");
  }
  else if ("ps_HV1" == tune) {
    add_scale(new ScaleE0(InputModifier::ONEBASED), new GetAmountHVPSGuillaume());
  }
  else if ("test1" == tune) {
    TH1F h_presampler("h_presampler", "h_presampler", 10, -2.5, 2.5);
    // just as an example, correct E0 by 0.1 * sign(eta)
    // and E1 by 1%
    for (int ibin = 1; ibin <= 5; ++ibin) {
      h_presampler.SetBinContent(ibin, -0.1);
      h_presampler.SetBinContent(ibin + 5, 0.1);
    }
    add_scale(new ScaleE0(InputModifier::ZEROBASED), new GetAmountHisto1D(h_presampler));
    add_scale(new ScaleE1(InputModifier::ZEROBASED), new GetAmountFixed(0.01));
  }
  else if ("layer1_1" == tune) {
    TFormula f("formula_layer1_1", "(abs(x)<1.425) ? 0.97 : 1");
    add_scale(new ScaleE1(InputModifier::ONEBASED_ALPHA), new GetAmountFormula(f));
  }
  else if ("layer1_2" == tune) {
    TFormula f("formula_layer1_2", "(abs(x)<1.425) ? 0.97 : 1.05");
    add_scale(new ScaleE1(InputModifier::ONEBASED_ALPHA), new GetAmountFormula(f));
  }
  else if ("layer1_alt_2012_v5" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1D* histo = static_cast<TH1D*>(f.Get("hE1E2ave_alt_2012"));
    assert(histo);
    add_scale(new ScaleE1(InputModifier::ONEBASED_ALPHA),
	      new GetAmountHisto1D(*histo));
  }
  else if ("layer1_2012_v5" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1D* histo = static_cast<TH1D*>(f.Get("hE1E2ave_2012"));
    assert(histo);
    add_scale(new ScaleE1(InputModifier::ONEBASED_ALPHA),
	      new GetAmountHisto1D(*histo));
  }
  else if ("layer1_2012_v5_down" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1D* histo = static_cast<TH1D*>(f.Get("hE1E2ave_2012"));
    assert(histo);
    add_scale(new ScaleE1(InputModifier::ONEBASED_ALPHA),
	      new GetAmountHisto1DUp(*histo));
  }
  else if ("layer1_2012_v5_up" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1D* histo = static_cast<TH1D*>(f.Get("hE1E2ave_2012"));
    assert(histo);
    add_scale(new ScaleE1(InputModifier::ONEBASED_ALPHA),
	      new GetAmountHisto1DDown(*histo));
  }
  else if ("layer1_2012_v5_errdown" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1D* histo = static_cast<TH1D*>(f.Get("hE1E2ave_2012"));
    assert(histo);
    add_scale(new ScaleE1(InputModifier::ZEROBASED_ALPHA),
	      new GetAmountHisto1DErrorUp(*histo));
  }
  else if ("layer1_2012_v5_errup" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1D* histo = static_cast<TH1D*>(f.Get("hE1E2ave_2012"));
    assert(histo);
    add_scale(new ScaleE1(InputModifier::ZEROBASED_ALPHA),
	      new GetAmountHisto1DErrorDown(*histo));
  }
  else if ("layer1_alt_2011_v5" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1D* histo = static_cast<TH1D*>(f.Get("hE1E2ave_alt_2011"));
    assert(histo);
    add_scale(new ScaleE1(InputModifier::ONEBASED_ALPHA),
	      new GetAmountHisto1D(*histo));
  }
  else if ("layer1_2011_v5" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1D* histo = static_cast<TH1D*>(f.Get("hE1E2ave_2011"));
    assert(histo);
    add_scale(new ScaleE1(InputModifier::ONEBASED_ALPHA),
	      new GetAmountHisto1D(*histo));
  }
  else if ("layer1_2011_v5_down" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1D* histo = static_cast<TH1D*>(f.Get("hE1E2ave_2011"));
    assert(histo);
    add_scale(new ScaleE1(InputModifier::ONEBASED_ALPHA),
	      new GetAmountHisto1DUp(*histo));
  }
  else if ("layer1_2011_v5_up" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1D* histo = static_cast<TH1D*>(f.Get("hE1E2ave_2011"));
    assert(histo);
    add_scale(new ScaleE1(InputModifier::ONEBASED_ALPHA),
	      new GetAmountHisto1DDown(*histo));
  }
  else if ("layer1_2011_v5_errdown" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1D* histo = static_cast<TH1D*>(f.Get("hE1E2ave_2011"));
    assert(histo);
    add_scale(new ScaleE1(InputModifier::ZEROBASED_ALPHA),
	      new GetAmountHisto1DErrorUp(*histo));
  }
  else if ("layer1_2011_v5_errup" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1D* histo = static_cast<TH1D*>(f.Get("hE1E2ave_2011"));
    assert(histo);
    add_scale(new ScaleE1(InputModifier::ZEROBASED_ALPHA),
	      new GetAmountHisto1DErrorDown(*histo));
  }
  else if ("layer1_2010_v5" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1D* histo = static_cast<TH1D*>(f.Get("hE1E2ave_2010"));
    assert(histo);
    add_scale(new ScaleE1(InputModifier::ONEBASED_ALPHA),
	      new GetAmountHisto1D(*histo));
  }
  else if ("layer1_2010_v5_down" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1D* histo = static_cast<TH1D*>(f.Get("hE1E2ave_2010"));
    assert(histo);
    add_scale(new ScaleE1(InputModifier::ONEBASED_ALPHA),
	      new GetAmountHisto1DUp(*histo));
  }
  else if ("layer1_2010_v5_up" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1D* histo = static_cast<TH1D*>(f.Get("hE1E2ave_2010"));
    assert(histo);
    add_scale(new ScaleE1(InputModifier::ONEBASED_ALPHA),
	      new GetAmountHisto1DDown(*histo));
  }
  else if ("layer1_2010_v5_errdown" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1D* histo = static_cast<TH1D*>(f.Get("hE1E2ave_2010"));
    assert(histo);
    add_scale(new ScaleE1(InputModifier::ZEROBASED_ALPHA),
	      new GetAmountHisto1DErrorUp(*histo));
  }
  else if ("layer1_2010_v5_errup" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1D* histo = static_cast<TH1D*>(f.Get("hE1E2ave_2010"));
    assert(histo);
    add_scale(new ScaleE1(InputModifier::ZEROBASED_ALPHA),
	      new GetAmountHisto1DErrorDown(*histo));
  }
  else if("layer2_alt_2012_v5" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1D* histo = static_cast<TH1D*>(f.Get("hE1E2ave_alt_2012"));
    assert(histo);
    add_scale(new ScaleE2(InputModifier::ONEBASED),
	      new GetAmountHisto1D(*histo));
  }
  else if("layer2_2012_v5" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1D* histo = static_cast<TH1D*>(f.Get("hE1E2ave_2012"));
    assert(histo);
    add_scale(new ScaleE2(InputModifier::ONEBASED),
	      new GetAmountHisto1D(*histo));
  }
  else if("layer2_2012_v5_down" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1D* histo = static_cast<TH1D*>(f.Get("hE1E2ave_2012"));
    assert(histo);
    add_scale(new ScaleE2(InputModifier::ONEBASED),
	      new GetAmountHisto1DDown(*histo));
  }
  else if("layer2_2012_v5_up" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1D* histo = static_cast<TH1D*>(f.Get("hE1E2ave_2012"));
    assert(histo);
    add_scale(new ScaleE2(InputModifier::ONEBASED),
	      new GetAmountHisto1DUp(*histo));
  }
  else if ("layer2_2012_v5_errdown" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1D* histo = static_cast<TH1D*>(f.Get("hE1E2ave_2012"));
    assert(histo);
    add_scale(new ScaleE2(InputModifier::ONEBASED),
	      new GetAmountHisto1DErrorDown(*histo));
  }
  else if ("layer2_2012_v5_errup" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1D* histo = static_cast<TH1D*>(f.Get("hE1E2ave_2012"));
    assert(histo);
    add_scale(new ScaleE2(InputModifier::ONEBASED),
	      new GetAmountHisto1DErrorUp(*histo));
  }
  else if("layer2_alt_2011_v5" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1D* histo = static_cast<TH1D*>(f.Get("hE1E2ave_alt_2011"));
    assert(histo);
    add_scale(new ScaleE2(InputModifier::ONEBASED),
	      new GetAmountHisto1D(*histo));
  }
  else if("layer2_2011_v5" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1D* histo = static_cast<TH1D*>(f.Get("hE1E2ave_2011"));
    assert(histo);
    add_scale(new ScaleE2(InputModifier::ONEBASED),
	      new GetAmountHisto1D(*histo));
  }
  else if("layer2_2011_v5_down" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1D* histo = static_cast<TH1D*>(f.Get("hE1E2ave_2011"));
    assert(histo);
    add_scale(new ScaleE2(InputModifier::ONEBASED),
	      new GetAmountHisto1DDown(*histo));
  }
  else if("layer2_2011_v5_up" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1D* histo = static_cast<TH1D*>(f.Get("hE1E2ave_2011"));
    assert(histo);
    add_scale(new ScaleE2(InputModifier::ONEBASED),
	      new GetAmountHisto1DUp(*histo));
  }
  else if ("layer2_2011_v5_errdown" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1D* histo = static_cast<TH1D*>(f.Get("hE1E2ave_2011"));
    assert(histo);
    add_scale(new ScaleE2(InputModifier::ONEBASED),
	      new GetAmountHisto1DErrorDown(*histo));
  }
  else if ("layer2_2011_v5_errup" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1D* histo = static_cast<TH1D*>(f.Get("hE1E2ave_2011"));
    assert(histo);
    add_scale(new ScaleE2(InputModifier::ONEBASED),
	      new GetAmountHisto1DErrorUp(*histo));
  }
  else if("layer2_2010_v5" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1D* histo = static_cast<TH1D*>(f.Get("hE1E2ave_2010"));
    assert(histo);
    add_scale(new ScaleE2(InputModifier::ONEBASED),
	      new GetAmountHisto1D(*histo));
  }
  else if("layer2_2010_v5_down" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1D* histo = static_cast<TH1D*>(f.Get("hE1E2ave_2010"));
    assert(histo);
    add_scale(new ScaleE2(InputModifier::ONEBASED),
	      new GetAmountHisto1DDown(*histo));
  }
  else if("layer2_2010_v5_up" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1D* histo = static_cast<TH1D*>(f.Get("hE1E2ave_2010"));
    assert(histo);
    add_scale(new ScaleE2(InputModifier::ONEBASED),
	      new GetAmountHisto1DUp(*histo));
  }
  else if ("layer2_2010_v5_errdown" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1D* histo = static_cast<TH1D*>(f.Get("hE1E2ave_2010"));
    assert(histo);
    add_scale(new ScaleE2(InputModifier::ONEBASED),
	      new GetAmountHisto1DErrorDown(*histo));
  }
  else if ("layer2_2010_v5_errup" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1D* histo = static_cast<TH1D*>(f.Get("hE1E2ave_2010"));
    assert(histo);
    add_scale(new ScaleE2(InputModifier::ONEBASED),
	      new GetAmountHisto1DErrorUp(*histo));
  }
  else if ("ps_2012_v3" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1D* histo_ps_tot_error = static_cast<TH1D*>(f.Get("hPS_2012"));
    assert(histo_ps_tot_error);
    add_scale(new ScaleE0(InputModifier::ONEBASED_ALPHA),
	      new GetAmountHisto1D(*histo_ps_tot_error));
  }
  else if ("ps_2012_v3_down" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1F* histo_ps_tot_error = static_cast<TH1F*>(f.Get("hPS_2012"));
    assert(histo_ps_tot_error);
    add_scale(new ScaleE0(InputModifier::ONEBASED_ALPHA),
	      new GetAmountHisto1DUp(*histo_ps_tot_error));
  }
  else if ("ps_2012_v3_up" == tune){
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1F* histo_ps_tot_error = static_cast<TH1F*>(f.Get("hPS_2012"));
    assert(histo_ps_tot_error);
    add_scale(new ScaleE0(InputModifier::ONEBASED_ALPHA),
	      new GetAmountHisto1DDown(*histo_ps_tot_error));
  }
  else if ("ps_2012_v3_errdown" == tune){
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1F* histo_ps_tot_error = static_cast<TH1F*>(f.Get("hPS_2012"));
    assert(histo_ps_tot_error);
    add_scale(new ScaleE0(InputModifier::ZEROBASED_ALPHA),
	      new GetAmountHisto1DErrorUp(*histo_ps_tot_error));
  }
  else if ("ps_2012_v3_errup" == tune){
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1F* histo_ps_tot_error = static_cast<TH1F*>(f.Get("hPS_2012"));
    assert(histo_ps_tot_error);
    add_scale(new ScaleE0(InputModifier::ZEROBASED_ALPHA),
	      new GetAmountHisto1DErrorDown(*histo_ps_tot_error));
  }
  else if ("ps_2011_v3" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1F* histo_ps_tot_error = static_cast<TH1F*>(f.Get("hPS_2011"));
    assert(histo_ps_tot_error);
    add_scale(new ScaleE0(InputModifier::ONEBASED_ALPHA),
	      new GetAmountHisto1D(*histo_ps_tot_error));
  }
  else if ("ps_2011_v3_down" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1F* histo_ps_tot_error = static_cast<TH1F*>(f.Get("hPS_2011"));
    assert(histo_ps_tot_error);
    add_scale(new ScaleE0(InputModifier::ONEBASED_ALPHA),
	      new GetAmountHisto1DUp(*histo_ps_tot_error));
  }
  else if ("ps_2011_v3_up" == tune){
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1F* histo_ps_tot_error = static_cast<TH1F*>(f.Get("hPS_2011"));
    assert(histo_ps_tot_error);
    add_scale(new ScaleE0(InputModifier::ONEBASED_ALPHA),
	      new GetAmountHisto1DDown(*histo_ps_tot_error));
  }
  else if ("ps_2011_v3_errdown" == tune){
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1F* histo_ps_tot_error = static_cast<TH1F*>(f.Get("hPS_2011"));
    assert(histo_ps_tot_error);
    add_scale(new ScaleE0(InputModifier::ZEROBASED_ALPHA),
	      new GetAmountHisto1DErrorUp(*histo_ps_tot_error));
  }
  else if ("ps_2011_v3_errup" == tune){
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1F* histo_ps_tot_error = static_cast<TH1F*>(f.Get("hPS_2011"));
    assert(histo_ps_tot_error);
    add_scale(new ScaleE0(InputModifier::ZEROBASED_ALPHA),
	      new GetAmountHisto1DErrorDown(*histo_ps_tot_error));
  }
  // 2010
  else if ("ps_2010_v3" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1F* histo_ps_tot_error = static_cast<TH1F*>(f.Get("hPS_2010"));
    assert(histo_ps_tot_error);
    add_scale(new ScaleE0(InputModifier::ONEBASED_ALPHA),
	      new GetAmountHisto1D(*histo_ps_tot_error));
  }
  else if ("ps_2010_v3_down" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1F* histo_ps_tot_error = static_cast<TH1F*>(f.Get("hPS_2010"));
    assert(histo_ps_tot_error);
    add_scale(new ScaleE0(InputModifier::ONEBASED_ALPHA),
	      new GetAmountHisto1DUp(*histo_ps_tot_error));
  }
  else if ("ps_2010_v3_up" == tune) {
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1F* histo_ps_tot_error = static_cast<TH1F*>(f.Get("hPS_2010"));
    assert(histo_ps_tot_error);
    add_scale(new ScaleE0(InputModifier::ONEBASED_ALPHA),
	      new GetAmountHisto1DDown(*histo_ps_tot_error));
  }
  else if ("ps_2010_v3_errdown" == tune){
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1F* histo_ps_tot_error = static_cast<TH1F*>(f.Get("hPS_2010"));
    assert(histo_ps_tot_error);
    add_scale(new ScaleE0(InputModifier::ZEROBASED_ALPHA),
	      new GetAmountHisto1DErrorUp(*histo_ps_tot_error));
  }
  else if ("ps_2010_v3_errup" == tune){
    const std::string file = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/egammaLayerRecalibTunes.root");
    TFile f(file.c_str());
    TH1F* histo_ps_tot_error = static_cast<TH1F*>(f.Get("hPS_2010"));
    assert(histo_ps_tot_error);
    add_scale(new ScaleE0(InputModifier::ZEROBASED_ALPHA),
	      new GetAmountHisto1DErrorDown(*histo_ps_tot_error));
  }
  else {
    std::cerr << tune << " is not a valid tune" << std::endl;
    assert(false);
  }
}

egammaLayerRecalibTool::egammaLayerRecalibTool(const std::string& name, const std::string& tune)
  : asg::AsgMessaging(name), m_tune(tune)
{
  add_scale(tune);
}

egammaLayerRecalibTool::egammaLayerRecalibTool(const std::string& tune)
  : egammaLayerRecalibTool("egammaLayerRecalibTool", tune) { }


void egammaLayerRecalibTool::add_scale(InputModifier* modifier, GetAmountBase* amount)
{
  m_modifiers.push_back(std::make_pair(modifier, amount));
}

CP::CorrectionCode egammaLayerRecalibTool::scale_inputs(StdCalibrationInputs & inputs) const
{
  CP::CorrectionCode status = CP::CorrectionCode::Ok;
  for (const auto& modifier : m_modifiers)  {
    const float amount = (*modifier.second)(inputs);
    const auto s = (*modifier.first)(inputs, amount);
    if (s != CP::CorrectionCode::Ok) {
      if (status != CP::CorrectionCode::Error) { status = s; }
    }
  }
  return status;
}

CP::CorrectionCode egammaLayerRecalibTool::applyCorrection(xAOD::Egamma& particle, const xAOD::EventInfo& event_info) const
{
  const xAOD::CaloCluster* cluster = particle.caloCluster();
  if (!cluster) {
    ATH_MSG_ERROR("egamma particle without CaloCluster");
    return CP::CorrectionCode::Error;
  }

  StdCalibrationInputs inputs{
    event_info.runNumber(),
      cluster->eta(),
      cluster->phi(),
      cluster->energyBE(0),
      cluster->energyBE(1),
      cluster->energyBE(2),
      cluster->energyBE(3),};

  const CP::CorrectionCode status = scale_inputs(inputs);

  static const SG::AuxElement::Decorator<double> deco_E0("correctedcl_Es0");
  static const SG::AuxElement::Decorator<double> deco_E1("correctedcl_Es1");
  static const SG::AuxElement::Decorator<double> deco_E2("correctedcl_Es2");
  static const SG::AuxElement::Decorator<double> deco_E3("correctedcl_Es3");
  static const SG::AuxElement::Decorator<std::string> deco_layer_correction("layer_correction");

  if (status == CP::CorrectionCode::Ok) {
    ATH_MSG_DEBUG("decorating cluster with corrected layer energies");
    deco_E0(*cluster) = inputs.E0raw;
    deco_E1(*cluster) = inputs.E1raw;
    deco_E2(*cluster) = inputs.E2raw;
    deco_E3(*cluster) = inputs.E3raw;
    deco_layer_correction(*cluster) = m_tune;
    return status;
  }
  
    ATH_MSG_DEBUG("cannot correct layer energies: decorating particle with non-corrected layer energies");
    // this is done for safety, since when a particle is decorated
    // all the particle in the container are decorated
    // it is not possible to distinguish between decorated / non-decorated
    // since all are decorated
    deco_E0(*cluster) = cluster->energyBE(0);
    deco_E1(*cluster) = cluster->energyBE(1);
    deco_E2(*cluster) = cluster->energyBE(2);
    deco_E3(*cluster) = cluster->energyBE(3);
    deco_layer_correction(*cluster) = m_tune;
    return status;
  
}


void egammaLayerRecalibTool::clear_corrections()
{
  for (auto modifier :  m_modifiers) {
    delete modifier.first;
    delete modifier.second;
  }
  m_modifiers.clear();
}


// helper
std::map<std::string, std::string> parse(const std::string& list)
{
  std::cout << "list: '" << list << "'" << std::endl;
  std::map<std::string, std::string> result;
  TIter next(TString(list).Tokenize(","));
  while (TObjString* sObj = (TObjString*) next())
    {
      const TString& item(sObj->GetString());
      std::cout << "item: '" << item << "'" << std::endl;
      TObjArray* item_list = TString(item).Tokenize(":");
      std::string key;
      std::string value;
      if (item_list->GetEntries() == 1) {
	key = "amount";
	value = static_cast<TObjString*>(item_list->At(0))->GetString().Data();
      }
      else if (item_list->GetEntries() == 2) {
	key = static_cast<TObjString*>(item_list->At(0))->GetString().Data();
	value = static_cast<TObjString*>(item_list->At(1))->GetString().Data();
      }
      else {
	std::cerr << "invalid string " << item << std::endl;
      }
      if (result.find(key) != result.end()) {
	std::cerr << "trying to insert two times key " << key << std::endl;
	assert(false);
      }
      result.insert(std::make_pair(key, value));
    }
  return result;
}


std::pair<std::string, egammaLayerRecalibTool*>
egammaLayerRecalibTool::create(const std::string& type, const std::string& args)
{
  std::map<std::string, std::string> args_map = parse(args);
  egammaLayerRecalibTool* tool = new egammaLayerRecalibTool("egammaLayerRecalibTool", "");
  std::string name = "";
  std::string amount_name = "";
  std::string type_name = "";

  GetAmountBase* amount_getter = nullptr;
  InputModifier* modifier = nullptr;

  if (args_map.find("amount") != args_map.end()) {
    std::string amount_str = args_map["amount"];
    bool perc = false;
    if (std::string("%") == amount_str.substr(amount_str.size()-1, amount_str.size())) {
      perc = true;
      amount_str = amount_str.substr(0, amount_str.size()-1);
    }
    const float amount = TString(amount_str).Atof() * (perc ? 0.01 : 1);

    amount_getter = new GetAmountFixed(amount);
    std::stringstream amount_stream;
    amount_stream << amount;
    amount_name = amount_stream.str();
    std::replace(amount_name.begin(), amount_name.end(), '-', 'n');
    std::replace(amount_name.begin(), amount_name.end(), '.', 'p');
  }
  else if (args_map.find("name") != args_map.end()) {
    name = args_map["name"];
  }
  else if (args_map.find("histo") != args_map.end()) {
    int dim = 0;
    if (args_map.find("file") == args_map.end()) {
      std::cerr << "with histo you must specify file" << std::endl;
      assert(false);
    }
    if (args_map.find("formulax") != args_map.end()) dim = 1;

    if (dim == 0)
      {
	std::cerr << "with histo you must specify formulax" << std::endl;
	assert(false);
      }
    if (dim == 1) {
      TFile f(args_map["file"].c_str());
      std::cout << "opening histo " << args_map["histo"] << " from file " << args_map["file"] << std::endl;
      TH1F* histo = dynamic_cast<TH1F*>(f.Get(args_map["histo"].c_str()));

      if(histo){
	histo->SetDirectory(nullptr);
	amount_getter = new GetAmountHisto1D(*histo);
      }
      else{assert(false); }
    }
    else { assert(false); }
  }
  else {
    std::cerr << "cannot understand argument " << args << std::endl;
    assert(false);
  }

  if ("bias-E0" == type)                { modifier = new ScaleE0(InputModifier::ZEROBASED); type_name = "E0"; }
  else if ("bias-E1" == type)           { modifier = new ScaleE1(InputModifier::ZEROBASED); type_name = "E1"; }
  else if ("bias-E2" == type)           { modifier = new ScaleE2(InputModifier::ZEROBASED); type_name = "E2"; }
  else if ("bias-E3" == type)           { modifier = new ScaleE3(InputModifier::ZEROBASED); type_name = "E3"; }
  else if ("bias-E1overE2" == type)     { modifier = new ScaleE1overE2(InputModifier::ZEROBASED); type_name = "E1overE2"; }
  else if ("bias-Eaccordion" == type)   { modifier = new ScaleEaccordion(InputModifier::ZEROBASED); type_name = "Eaccordion"; }
  else if ("bias-Ecalorimeter" == type) { modifier = new ScaleEcalorimeter(InputModifier::ZEROBASED); type_name = "Ecalorimeter"; }

  if (not type_name.empty() and not amount_name.empty()) {
    name = type_name + "_" + amount_name;
  }

  if (name.empty()) {
    std::cerr << "you need to specify a name for the bias with type " << type << std::endl;
  }

  if (modifier and amount_getter) {
    tool->add_scale(modifier, amount_getter);
  }
  else{
    tool->add_scale(type);
    //release resources, if modifier false need to release amount_getter and vice versa
    //since they are not passed to the tool
    if(modifier) delete modifier;
    if(amount_getter) delete  amount_getter;
  }

  return std::pair<std::string, egammaLayerRecalibTool*>(name, tool);
}
