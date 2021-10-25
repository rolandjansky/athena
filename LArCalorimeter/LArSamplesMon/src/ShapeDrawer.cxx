/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSamplesMon/ShapeDrawer.h"

#include "TH1.h"
#include "TGraphErrors.h"
#include "TPaveText.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TLegendEntry.h"
#include "TList.h"

#include "LArCafJobs/AbsShape.h"
#include "LArCafJobs/SimpleShape.h"
#include "LArSamplesMon/Chi2Calc.h"

#include <iostream>
using std::cout;
using std::endl;

using namespace LArSamples;


TGraphErrors* ShapeDrawer::graph(const AbsShape& shape, const char* title) const
{
  bool samplingTimeUnits = (m_pars & SamplingTimeUnits) != 0;
  TGraphErrors* graph = shape.graph(samplingTimeUnits);
  graph->SetTitle(title);
  graph->SetMarkerStyle(20);
  graph->SetMarkerSize(0.4);
  graph->GetXaxis()->SetTitle(samplingTimeUnits ? "Sample Index" : "Time [ns]");
  graph->GetYaxis()->SetTitle("ADC counts");
  graph->GetYaxis()->SetTitleOffset(1.25);
  return graph;
}


TGraphErrors* ShapeDrawer::draw(const AbsShape* shape, const char* title, bool drawAxes, const char* gopt) const
{
  TGraphErrors* g = graph(*shape, title);
  g->Draw(TString(drawAxes ? "A" : "") + gopt); // X=no errors
  return g;
}


bool ShapeDrawer::draw(const char* title, const std::vector<const AbsShape*>& shapes, 
                                   const AbsShape* reference, const AbsShape* refSamples) const
{
  if (shapes.empty()) return false;
  
  if (m_pars & AtlasStyle) {
    gStyle->SetTitleXOffset(1.0);
    gStyle->SetPadTopMargin(0.13);  
    gStyle->SetPadBottomMargin(0.12);  
    gStyle->SetPadRightMargin(0.05);
    gStyle->SetPadLeftMargin(0.12);
  }
  
  TLegend* legend = nullptr;
  if (m_pars & AtlasStyle) {
    legend = new TLegend(0.45, 0.7, 0.8, 0.5, "", "NDC");
  }
  else if ((m_pars & Legend) && shapes.size() <= 4){
    legend = new TLegend(0.6, 0.85, 0.85, 0.85 - 0.1*shapes.size(), "", "NDC");
  }
    
  if (legend) {
    legend->SetFillColor(0);
    legend->SetLineColor(0);
    legend->SetShadowColor(0);
    legend->SetTextSize(0.06);
    legend->SetMargin(0.12);
  }

  if (m_pars & DataFirst) {
    if (!drawData(title, shapes, reference, true, legend)) return false;
    if (reference && !drawRef(title, reference, refSamples, false, legend)) return false;
    if (m_pars & AtlasStyle) {
      if (!drawData(title, shapes, reference, false)) return false;
      TLatex l;
      l.SetNDC();
      l.SetTextSize(0.06);
      l.DrawLatex(0.12,0.89, title);
    }
  }
  else {
    if (reference && !drawRef(title, reference, refSamples, true)) return false;
    if (!drawData(title, shapes, reference, false)) return false;
  }
  if (legend && shapes.size() > 1) legend->Draw();
  return true;
}


bool ShapeDrawer::drawRef(const char* title, const AbsShape* reference, const AbsShape* refSamples, bool first, TLegend* legend) const
{
  if (m_pars & AtlasStyle) {
    //SimpleShape* overSampledRef = reference->resample(125);
    //TGraphErrors* ref = graph(*overSampledRef);
    TGraphErrors* ref = graph(*reference);
    ref->SetName("reference");
    ref->SetMarkerStyle(21);
    ref->SetMarkerSize(0.3);
    ref->SetMarkerColor(4);
    ref->SetLineColor(4);
    ref->SetLineWidth(3);
    ref->Draw("XC");
    if (legend) {
      TGraphErrors* refBigMarkers = new TGraphErrors(*ref);
      refBigMarkers->SetMarkerSize(1);
      legend->AddEntry(refBigMarkers, "Prediction", "L");
      ((TLegendEntry*)legend->GetListOfPrimitives()->Last())->SetTextColor(ref->GetMarkerColor());
    }
    return true;
  }
  
  TGraphErrors* ref = draw(reference, title, first, "CPX");
  ref->SetName("reference");
  
  if (refSamples) {
    TGraphErrors* ref2 = graph(*refSamples);
    ref->SetName("reference_samples");
    ref2->SetMarkerSize(1);
    ref2->Draw("P");
  }
  return true;
}


bool ShapeDrawer::drawData(const char* title, const std::vector<const AbsShape*>& shapes, const AbsShape* reference, 
				       bool first, TLegend* legend) const
{
  Chi2Calc c2c;
  double yMin = 0, yMax = 0;
  if (!yAxisLimits(shapes, yMin, yMax)) return false;
  cout << "Y limits = " << yMin << " " << yMax << endl;
  for (unsigned int i = 0; i < shapes.size(); i++) {
    if (!shapes[i]) return false;
    TGraphErrors* data = graph(*shapes[i], title);
    data->SetMinimum(yMin);
    data->SetMaximum(yMax);
    data->SetName(Form("data%d", i));
    if (m_pars & AtlasStyle) {
      data->GetXaxis()->SetLimits(-10, 800);
      data->SetMarkerColor(2);
      data->SetMarkerStyle(20);
      data->SetMarkerSize(1.3);
    }
    else 
      if (!colorize(data, i)) return false;
    data->Draw("P" + TString((first && i == 0) ? "A" : ""));
    if (!legend) continue;
    if (m_pars & AtlasStyle) {
      legend->AddEntry(data, "Data", "P");
      ((TLegendEntry*)legend->GetListOfPrimitives()->Last())->SetTextColor(data->GetMarkerColor());
    }
    else {
      if (!reference || i >= 4) continue; // no chi2 printout...
      double c2 = c2c.chi2(*shapes[i], *reference);
      legend->AddEntry(data, Form("#chi^{2}/n = %.2f/%-2d", c2, c2c.nDof()), "P");
      ((TLegendEntry*)legend->GetListOfPrimitives()->Last())->SetTextColor(data->GetMarkerColor());
    }
  }
  
  return true;
}


bool ShapeDrawer::draw(const char* title, const AbsShape* shape, const AbsShape* reference, const AbsShape* refSamples) const
{
  std::vector<const AbsShape*> shapes;
  shapes.push_back(shape);
  return draw(title, shapes, reference, refSamples);
}


bool ShapeDrawer::drawAndDelete(const char* title, const std::vector<const AbsShape*>& shapes, 
                   const AbsShape* reference, const AbsShape* refSamples) const
{
  bool result = draw(title, shapes, reference, refSamples);
  for (const AbsShape* shape : shapes)
    delete shape;
  delete reference;
  delete refSamples;
  return result;
}


bool ShapeDrawer::drawAndDelete(const char* title, const AbsShape* shape, const AbsShape* reference, const AbsShape* refSamples) const
{
  bool result = draw(title, shape, reference, refSamples);
  delete shape;
  delete reference;
  delete refSamples;
  return result;
}


bool ShapeDrawer::colorize(TGraphErrors* data, unsigned int index) const
{
  if (!data) return false;
  const int nColors = 5;
  int colors[nColors] = { 2,4,6,8,14 };

  data->SetMarkerColor(colors[index%nColors]); // use the 5 colors above
  data->SetMarkerStyle(20 + ((index/nColors) % 50));  // for more choices change marker style from 20->30. Afterwards, loop back...
  data->SetMarkerSize(1);
  return true;
}


bool ShapeDrawer::yAxisLimits(const std::vector<const AbsShape*>& shapes, double& yMin, double& yMax) const
{
  yMin = 0;
  yMax = 0;
  for (unsigned int i = 0; i < shapes.size(); i++) {
    double sMin = shapes[i]->minValue(true);
    double sMax = shapes[i]->maxValue(true);
    if (sMin < yMin) yMin = sMin;
    if (sMax > yMax) yMax = sMax;    
  }
  yMin *= 1.2;
  yMax *= 1.2;
  return true;
}

