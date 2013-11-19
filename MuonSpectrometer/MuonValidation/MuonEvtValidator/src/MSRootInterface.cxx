/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////
//
//   July 2006
//   Authors: Matthias Schott (LMU)
//
/////////////////////////////////////////////////////////////////////
#include "TF1.h"
#include "TH2.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TLegendEntry.h"
#include "TColor.h"
#include "TDirectory.h"
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "MuonEvtValidator/MSRootInterface.h"

#include "MuonSimValUtils/MuonChambersRange.h"
#include "MuonSimValUtils/MuonChamberNameConverter.h"
#include "MuonSimValUtils/MuonIntToStrName.h"

#include "TAxis.h"

MSRootInterface::MSRootInterface()
{
	clear();
}


void MSRootInterface::setDirectory(string directory)
{
	m_Directory = directory;
}


void MSRootInterface::clear()
{
	m_Directory="";
	map<string, TH1F*>::const_iterator itr;
	for(itr = map1DHistos.begin(); itr != map1DHistos.end(); ++itr)
	{
		delete (*itr).second;
	}
	mapGraph.clear();
	map1DHistos.clear();
}


void MSRootInterface::setNewGraph(string id, string name, string xAxis, string yAxis)
{
	MSOutputDescription output={0.,0.,0,"","",""};
	output.name=name;
	output.xAxis=xAxis;
	output.yAxis=yAxis;
	mapOutputDescription[id]=output;
}


void MSRootInterface::setNewHistrogram(string id, string name, string xAxis, string yAxis, int bins, float start, float end)
{
	MSOutputDescription output;
	output.name=name;
	output.xAxis=xAxis;
	output.yAxis=yAxis;
	output.bins=bins;
	output.start=start;
	output.end=end;
	map1DHistos[id] 	= new TH1F(id.c_str(), id.c_str(), bins, start, end);
	mapOutputDescription[id]=output;
}


void MSRootInterface::fill(string id, float value)
{
	if (map1DHistos.find(id) == map1DHistos.end())	return;
	map1DHistos[id]->Fill(value);
}


void MSRootInterface::fill(string id, float value, float weight)
{
	if (map1DHistos.find(id) == map1DHistos.end())	return;
	map1DHistos[id]->Fill(value, weight);
}


void MSRootInterface::writeHistogram(string id1)
{
	if (m_Directory!="")
	{
		TDirectory tDir;
		tDir.cd(m_Directory.c_str());
	}

	if (mapOutputDescription.find(id1) == mapOutputDescription.end())	return;
	if (map1DHistos.find(id1) == map1DHistos.end())				return;

	TCanvas *histcanvas = new TCanvas("histcanvas", "Histogram",1,40,797,640);
	histcanvas->ToggleEventStatus();
	histcanvas->SetHighLightColor(2);
	histcanvas->SetFillColor(0);
	histcanvas->SetBorderMode(0);
	histcanvas->SetBorderSize(1);
	histcanvas->SetGridx();
	histcanvas->SetGridy();
	histcanvas->SetLeftMargin(0.1);
	histcanvas->SetRightMargin(0.05);
	histcanvas->SetTopMargin(0.05);
	histcanvas->SetBottomMargin(0.12);
	histcanvas->SetFrameFillStyle(0);
	histcanvas->SetFrameLineStyle(0);
	histcanvas->SetFrameLineWidth(2);
	histcanvas->SetFrameBorderMode(0);
	histcanvas->SetFrameFillStyle(0);
	histcanvas->SetFrameLineStyle(0);
	histcanvas->SetFrameLineWidth(2);
	histcanvas->SetFrameBorderMode(0);

	map1DHistos[id1]->SetFillColor(7);
	map1DHistos[id1]->SetFillStyle(1001);
	map1DHistos[id1]->SetMarkerStyle(7);
	map1DHistos[id1]->GetXaxis()->SetTitle(mapOutputDescription[id1].xAxis.c_str());
	map1DHistos[id1]->GetXaxis()->SetLabelFont(42);
	map1DHistos[id1]->GetXaxis()->SetLabelSize(0.05);
	map1DHistos[id1]->GetXaxis()->SetTitleSize(0.06);
	map1DHistos[id1]->GetXaxis()->SetTitleOffset(0.85);
	map1DHistos[id1]->GetXaxis()->SetTitleFont(52);
	map1DHistos[id1]->GetYaxis()->SetTitle(mapOutputDescription[id1].yAxis.c_str());
	map1DHistos[id1]->GetYaxis()->SetLabelFont(42);
	map1DHistos[id1]->GetYaxis()->SetLabelSize(0.05);
	map1DHistos[id1]->GetYaxis()->SetTitleSize(0.06);
	map1DHistos[id1]->GetYaxis()->SetTitleOffset(0.85);
	map1DHistos[id1]->GetYaxis()->SetTitleFont(52);
	map1DHistos[id1]->GetZaxis()->SetLabelSize(0.03);
	map1DHistos[id1]->GetZaxis()->SetTitleSize(0.03);
	map1DHistos[id1]->Draw("");

	histcanvas->Modified();
	histcanvas->cd();
	histcanvas->SetSelected(histcanvas);
	histcanvas->Write(mapOutputDescription[id1].name.c_str());
	histcanvas->Draw(mapOutputDescription[id1].name.c_str());
}


void MSRootInterface::writeOverlayHistograms(string id1, string id2, string outputName)
{
	if (mapOutputDescription.find(id1) == mapOutputDescription.end())	return;
	if (map1DHistos.find(id1) == map1DHistos.end())				return;
	if (mapOutputDescription.find(id2) == mapOutputDescription.end())	return;
	if (map1DHistos.find(id2) == map1DHistos.end())				return;

	if (m_Directory!="")
	{
		TDirectory tDir;
		tDir.cd(m_Directory.c_str()); //"MVCommon.root:/MDT/"
	}

	TH1F *Hist1	= new TH1F(*map1DHistos[id1]);
	TH1F *Hist2	= new TH1F(*map1DHistos[id2]);

	TCanvas *histcanvas = new TCanvas("histcanvas", "Histogram",1,40,797,640);
	histcanvas->ToggleEventStatus();
	histcanvas->SetHighLightColor(2);
	histcanvas->SetFillColor(0);
	histcanvas->SetBorderMode(0);
	histcanvas->SetBorderSize(1);
	histcanvas->SetGridx();
	histcanvas->SetGridy();
	histcanvas->SetLeftMargin(0.1);
	histcanvas->SetRightMargin(0.05);
	histcanvas->SetTopMargin(0.05);
	histcanvas->SetBottomMargin(0.12);
	histcanvas->SetFrameFillStyle(0);
	histcanvas->SetFrameLineStyle(0);
	histcanvas->SetFrameLineWidth(2);
	histcanvas->SetFrameBorderMode(0);
	histcanvas->SetFrameFillStyle(0);
	histcanvas->SetFrameLineStyle(0);
	histcanvas->SetFrameLineWidth(2);
	histcanvas->SetFrameBorderMode(0);

	Hist1->SetFillColor(7);
	Hist1->SetFillStyle(1001);
	Hist1->SetMarkerStyle(7);
	Hist1->GetXaxis()->SetTitle(mapOutputDescription[id1].xAxis.c_str());
	Hist1->GetXaxis()->SetLabelFont(42);
	Hist1->GetXaxis()->SetLabelSize(0.05);
	Hist1->GetXaxis()->SetTitleSize(0.06);
	Hist1->GetXaxis()->SetTitleOffset(0.85);
	Hist1->GetXaxis()->SetTitleFont(52);
	Hist1->GetYaxis()->SetTitle(mapOutputDescription[id1].yAxis.c_str());
	Hist1->GetYaxis()->SetLabelFont(42);
	Hist1->GetYaxis()->SetLabelSize(0.05);
	Hist1->GetYaxis()->SetTitleSize(0.06);
	Hist1->GetYaxis()->SetTitleOffset(0.85);
	Hist1->GetYaxis()->SetTitleFont(52);
	Hist1->GetZaxis()->SetLabelSize(0.03);
	Hist1->GetZaxis()->SetTitleSize(0.03);
	Hist1->Draw("");

	Hist2->SetFillColor(8);
	Hist2->SetFillStyle(3001);
	Hist2->SetMarkerStyle(7);
	Hist2->GetXaxis()->SetTitle(mapOutputDescription[id2].xAxis.c_str());
	Hist2->GetXaxis()->SetLabelFont(42);
	Hist2->GetXaxis()->SetLabelSize(0.05);
	Hist2->GetXaxis()->SetTitleSize(0.06);
	Hist2->GetXaxis()->SetTitleOffset(0.85);
	Hist2->GetXaxis()->SetTitleFont(52);
	Hist2->GetYaxis()->SetTitle(mapOutputDescription[id2].yAxis.c_str());
	Hist2->GetYaxis()->SetLabelFont(42);
	Hist2->GetYaxis()->SetLabelSize(0.05);
	Hist2->GetYaxis()->SetTitleSize(0.06);
	Hist2->GetYaxis()->SetTitleOffset(0.85);
	Hist2->GetYaxis()->SetTitleFont(52);
	Hist2->GetZaxis()->SetLabelSize(0.03);
	Hist2->GetZaxis()->SetTitleSize(0.03);
	Hist2->Draw("same");

	TLegend *leg = new TLegend(0.7,0.7,0.98,0.98,NULL,"brNDC");
	leg->SetLineColor(1);
	leg->SetLineStyle(1);
	leg->SetLineWidth(1);
	leg->SetFillColor(0);
	leg->SetFillStyle(1001);
	TLegendEntry *entry=leg->AddEntry(id1.c_str(),mapOutputDescription[id1].name.c_str(),"f");
	entry->SetFillColor(1);
	entry->SetFillStyle(3005);
	entry->SetLineColor(1);
	entry->SetLineStyle(1);
	entry->SetLineWidth(1);
	entry->SetMarkerColor(1);
	entry->SetMarkerStyle(21);
	entry->SetMarkerSize(2);
	entry->SetTextAlign(12);
	entry->SetTextColor(1);
	entry=leg->AddEntry(id2.c_str(),mapOutputDescription[id2].name.c_str(),"f");
	entry->SetFillStyle(1001);
	entry->SetLineColor(1);
	entry->SetLineStyle(1);
	entry->SetLineWidth(1);
	entry->SetMarkerColor(1);
	entry->SetMarkerStyle(21);
	entry->SetMarkerSize(1);
	entry->SetTextAlign(12);
	entry->SetTextColor(1);
	leg->Draw();

	histcanvas->Modified();
	histcanvas->cd();
	histcanvas->SetSelected(histcanvas);
	histcanvas->Write(outputName.c_str());
	histcanvas->Draw(outputName.c_str());
	delete Hist1;
	delete Hist2;
	delete histcanvas;
}


void MSRootInterface::setGraphPoint(string id, float x, float y, float dx, float dy)
{
	MSGraphPointDescription point;

	point.X=x;
	point.Y=y;
	point.DX=dx;
	point.DY=dy;
	mapGraph[id].push_back(point);
}


void MSRootInterface::getRangeForGraph(string id, float &x1, float &y1, float &x2, float &y2)
{
	if (mapOutputDescription.find(id) == mapOutputDescription.end())	return;

	int nNumberOfEntries=(int)mapGraph[id].size();

	x1=0.0;
	y1=0.0;
	x2=1.0;
	y2=1.0;

	if (nNumberOfEntries<2) return;

	x1	= mapGraph[id][0].X;
	y1	= mapGraph[id][0].Y;
	x2	= mapGraph[id][0].X;
	y2	= mapGraph[id][0].Y;

	for (int i=0; i<nNumberOfEntries; i++)
	{
		if (x1>mapGraph[id][i].X) x1=mapGraph[id][i].X;
		if (x2<mapGraph[id][i].X) x2=mapGraph[id][i].X;
		if (y1>mapGraph[id][i].Y) y1=mapGraph[id][i].Y;
		if (y2<mapGraph[id][i].Y) y2=mapGraph[id][i].Y;
	}
	if (x1<0.0) x1*=1.2; else x1*=0.8;
	if (y1<0.0) y1*=1.2; else y1*=0.8;
	if (x2<0.0) x2*=0.8; else x2*=1.2;
	if (y2<0.0) y2*=0.8; else y2*=1.2;
}


void MSRootInterface::writeComparisionGraph(string id1, string id2, string outputName, string Type)
{

	if (m_Directory!="")
	{
		TDirectory tDir;
		tDir.cd(m_Directory.c_str());
	}

	if (mapGraph.find(id1) == mapGraph.end()) 				return;
	if (mapOutputDescription.find(id1) == mapOutputDescription.end())	return;
	if (mapGraph.find(id2) == mapGraph.end()) 				return;
	if (mapOutputDescription.find(id2) == mapOutputDescription.end())	return;
	//Int_t ci;   // for color index setting

	float	rx1, rx2, ry1, ry2;

	int nNumberOfEntries1 = (int)mapGraph[id1].size();
	int nNumberOfEntries2 = (int)mapGraph[id2].size();

	if (nNumberOfEntries1==0) return;
	if (nNumberOfEntries2==0) return;

	getRangeForGraph(id1, rx1, ry1, rx2, ry2);

	TCanvas *m_canvas = new TCanvas("m_canvas", "m_canvas",194,194,784,574);
	m_canvas->SetHighLightColor(2);
	m_canvas->Range(rx1, ry1, rx2, ry2);
	m_canvas->SetFillColor(0);
	m_canvas->SetBorderMode(0);
	m_canvas->SetBorderSize(1);
	m_canvas->SetGridx();
	m_canvas->SetGridy();
	m_canvas->SetLeftMargin(0.15);
	m_canvas->SetRightMargin(0.05);
	m_canvas->SetTopMargin(.1);
	m_canvas->SetBottomMargin(0.15);
	m_canvas->SetFrameFillStyle(0);
	m_canvas->SetFrameLineStyle(0);
	m_canvas->SetFrameBorderMode(0);
	m_canvas->SetFrameFillStyle(0);
	m_canvas->SetFrameLineStyle(0);
	m_canvas->SetFrameBorderMode(0);

	TGraphErrors *gre1 = new TGraphErrors(nNumberOfEntries1);
	gre1->SetName(id1.c_str());
	gre1->SetTitle(id1.c_str());
	gre1->SetFillColor(7);
	gre1->SetLineColor(7);
	gre1->SetMarkerColor(7);
	gre1->SetMarkerStyle(22);
	gre1->SetMarkerSize(1.6);
	for (int i=0; i<nNumberOfEntries1; i++)
	{
		gre1->SetPoint(i,mapGraph[id1][i].X,mapGraph[id1][i].Y);
		gre1->SetPointError(i,mapGraph[id1][i].DX,mapGraph[id1][i].DY);
	}

	TGraphErrors *gre2 = new TGraphErrors(nNumberOfEntries2);
	gre2->SetName(id2.c_str());
	gre2->SetTitle(id2.c_str());
	gre2->SetFillColor(8);
	gre2->SetLineColor(8);
	gre2->SetMarkerStyle(23);
	gre2->SetMarkerSize(1.6);
	gre2->SetMarkerColor(8);
	gre2->SetFillColor(8);
	gre2->SetLineColor(8);

	for (int i=0; i<nNumberOfEntries2; i++)
	{
		gre2->SetPoint(i,mapGraph[id2][i].X,mapGraph[id2][i].Y);
		gre2->SetPointError(i,mapGraph[id2][i].DX,mapGraph[id2][i].DY);
	}

	TH1F *UnderLyingHisto1 = new TH1F(outputName.c_str(),outputName.c_str(),100,rx1,rx2);
	UnderLyingHisto1->SetMinimum(ry1);
	UnderLyingHisto1->SetMaximum(ry2);
	UnderLyingHisto1->SetDirectory(0);
	UnderLyingHisto1->SetStats(0);
	UnderLyingHisto1->SetFillStyle(0);
	UnderLyingHisto1->SetLineWidth(2);
	UnderLyingHisto1->SetMarkerStyle(7);
	UnderLyingHisto1->GetXaxis()->SetTitle(mapOutputDescription[id1].xAxis.c_str());
	UnderLyingHisto1->GetXaxis()->SetLabelFont(42);
	UnderLyingHisto1->GetXaxis()->SetLabelSize(0.05);
	UnderLyingHisto1->GetXaxis()->SetTitleSize(0.06);
	UnderLyingHisto1->GetXaxis()->SetTitleFont(52);
	UnderLyingHisto1->GetYaxis()->SetTitle(mapOutputDescription[id1].yAxis.c_str());
	UnderLyingHisto1->GetYaxis()->SetLabelFont(42);
	UnderLyingHisto1->GetYaxis()->SetLabelSize(0.05);
	UnderLyingHisto1->GetYaxis()->SetTitleSize(0.06);
	UnderLyingHisto1->GetYaxis()->SetTitleOffset(0.9);
	UnderLyingHisto1->GetYaxis()->SetTitleFont(52);
	UnderLyingHisto1->GetZaxis()->SetLabelSize(0.05);
	UnderLyingHisto1->GetZaxis()->SetTitleSize(0.06);
	if(Type=="StationName"){
	UnderLyingHisto1->GetXaxis()->Set((int)(((rx2-rx1)/1.2)+1), (rx1/1.2)-0.5, (rx2/1.2)+0.5);
	for (int i = 1;  i<(int)(((rx2-rx1)/1.2)+2); i++){
		if(i<0 || i>53) continue;
		std::string ChamberName = convertIntToStrName(i-1+(int)(rx1/1.2),"MDT");
		UnderLyingHisto1->GetXaxis()->SetBinLabel(i,ChamberName.c_str());
	}
		UnderLyingHisto1->GetXaxis()->SetLabelSize(0.04);
	}
	UnderLyingHisto1->Draw("");
	gre1->SetHistogram(UnderLyingHisto1);
	gre1->Draw("ap");


	TH1F *UnderLyingHisto2 = new TH1F("UnderLyingHisto2","UnderLyingHisto2",100,rx1,rx2);
	UnderLyingHisto2->SetMinimum(ry1);
	UnderLyingHisto2->SetMaximum(ry2);
	UnderLyingHisto2->SetDirectory(0);
	UnderLyingHisto2->SetStats(0);
	UnderLyingHisto2->SetFillStyle(0);
	UnderLyingHisto2->SetLineWidth(2);
	UnderLyingHisto2->SetMarkerStyle(7);
	UnderLyingHisto2->GetXaxis()->SetTitle(mapOutputDescription[id1].xAxis.c_str());
	UnderLyingHisto2->GetXaxis()->SetLabelFont(42);
	UnderLyingHisto2->GetXaxis()->SetLabelSize(0.05);
	UnderLyingHisto2->GetXaxis()->SetTitleSize(0.06);
	UnderLyingHisto2->GetXaxis()->SetTitleFont(52);
	UnderLyingHisto2->GetYaxis()->SetTitle(mapOutputDescription[id1].yAxis.c_str());
	UnderLyingHisto2->GetYaxis()->SetLabelFont(42);
	UnderLyingHisto2->GetYaxis()->SetLabelSize(0.05);
	UnderLyingHisto2->GetYaxis()->SetTitleSize(0.06);
	UnderLyingHisto2->GetYaxis()->SetTitleOffset(0.9);
	UnderLyingHisto2->GetYaxis()->SetTitleFont(52);
	UnderLyingHisto2->GetZaxis()->SetLabelSize(0.05);
	UnderLyingHisto2->GetZaxis()->SetTitleSize(0.06);
	if(Type=="StationName"){
	UnderLyingHisto2->GetXaxis()->Set((int)(((rx2-rx1)/1.2)+1), (rx1/1.2)-0.5, (rx2/1.2)+0.5);
	for (int i = 1;  i<(int)(((rx2-rx1)/1.2)+2); i++){
		if(i<0 || i>53) continue;
		std::string ChamberName = convertIntToStrName(i-1+(int)(rx1/1.2),"MDT");
		UnderLyingHisto2->GetXaxis()->SetBinLabel(i,ChamberName.c_str());
	}
		UnderLyingHisto2->GetYaxis()->SetLabelSize(0.04);
	}
	gre2->SetHistogram(UnderLyingHisto2);
	gre2->Draw("psame");

	TLegend *leg = new TLegend(0.55,0.75,0.95,0.95,NULL,"brNDC");
	leg->SetTextFont(52);
	leg->SetTextSize(0.0579151);
	leg->SetLineColor(1);
	leg->SetLineStyle(1);
	leg->SetLineWidth(1);
	leg->SetFillColor(0);
	leg->SetFillStyle(1001);
	TLegendEntry *entry1=leg->AddEntry(id1.c_str(),mapOutputDescription[id1].name.c_str(),"p");
	entry1->SetLineColor(1);
	entry1->SetLineStyle(1);
	entry1->SetLineWidth(2);
	entry1->SetMarkerColor(7);
	entry1->SetMarkerStyle(22);
	entry1->SetMarkerSize(4);
	entry1->SetTextAlign(12);
	entry1->SetTextColor(1);
	TLegendEntry *entry2=leg->AddEntry(id2.c_str(),mapOutputDescription[id2].name.c_str(),"p");
	entry2->SetLineColor(1);
	entry2->SetLineStyle(1);
	entry2->SetLineWidth(2);
	entry2->SetMarkerColor(8);
	entry2->SetMarkerStyle(22);
	entry2->SetMarkerSize(4);
	entry2->SetTextAlign(12);
	entry2->SetTextColor(1);
	leg->Draw();

	m_canvas->Modified();
	m_canvas->cd();
	m_canvas->SetSelected(m_canvas);
	m_canvas->Write(outputName.c_str());
	m_canvas->Draw(outputName.c_str());
}

void MSRootInterface::writeComparisionGraph(string id1, string id2, string outputName)
{
	writeComparisionGraph(id1, id2, outputName, "StationIDName");
}

void MSRootInterface::writeGraph(string id1)
{
	if (m_Directory!="")
	{
		TDirectory tDir;
		tDir.cd(m_Directory.c_str());
	}

	if (mapGraph.find(id1) == mapGraph.end()) 				return;
	if (mapOutputDescription.find(id1) == mapOutputDescription.end())	return;

	float	rx1, rx2, ry1, ry2;

	int nNumberOfEntries1=(int)mapGraph[id1].size();

	if (nNumberOfEntries1==0) return;

	getRangeForGraph(id1, rx1, ry1, rx2, ry2);

	TCanvas *m_canvas = new TCanvas("m_canvas", "m_canvas",194,194,784,574);
	m_canvas->SetHighLightColor(2);
	m_canvas->Range(rx1, ry1, rx2, ry2);
	m_canvas->SetFillColor(0);
	m_canvas->SetBorderMode(0);
	m_canvas->SetBorderSize(1);
	m_canvas->SetGridx();
	m_canvas->SetGridy();
	m_canvas->SetLeftMargin(0.15);
	m_canvas->SetRightMargin(0.05);
	m_canvas->SetTopMargin(.1);
	m_canvas->SetBottomMargin(0.15);
	m_canvas->SetFrameFillStyle(0);
	m_canvas->SetFrameLineStyle(0);
	m_canvas->SetFrameBorderMode(0);
	m_canvas->SetFrameFillStyle(0);
	m_canvas->SetFrameLineStyle(0);
	m_canvas->SetFrameBorderMode(0);

	TGraphErrors *gre1 = new TGraphErrors(nNumberOfEntries1);
	gre1->SetName(id1.c_str());
	gre1->SetTitle(id1.c_str());
	gre1->SetFillColor(8);
	gre1->SetLineColor(15);
	gre1->SetMarkerColor(8);
	gre1->SetMarkerStyle(22);
	gre1->SetMarkerSize(1.6);
	for (int i=0; i<nNumberOfEntries1; i++)
	{
		gre1->SetPoint(i,mapGraph[id1][i].X,mapGraph[id1][i].Y);
		gre1->SetPointError(i,mapGraph[id1][i].DY,mapGraph[id1][i].DY);
	}
	TH1F *UnderLyingHisto1 = new TH1F(mapOutputDescription[id1].name.c_str(),mapOutputDescription[id1].name.c_str(),100,rx1,rx2);
	UnderLyingHisto1->SetMinimum(ry1);
	UnderLyingHisto1->SetMaximum(ry2);
	UnderLyingHisto1->SetDirectory(0);
	UnderLyingHisto1->SetStats(0);
	UnderLyingHisto1->SetFillStyle(0);
	UnderLyingHisto1->SetLineWidth(2);
	UnderLyingHisto1->SetMarkerStyle(7);
	UnderLyingHisto1->GetXaxis()->SetTitle(mapOutputDescription[id1].xAxis.c_str());
	UnderLyingHisto1->GetXaxis()->SetLabelFont(42);
	UnderLyingHisto1->GetXaxis()->SetLabelSize(0.05);
	UnderLyingHisto1->GetXaxis()->SetTitleSize(0.06);
	UnderLyingHisto1->GetXaxis()->SetTitleFont(52);
	UnderLyingHisto1->GetYaxis()->SetTitle(mapOutputDescription[id1].yAxis.c_str());
	UnderLyingHisto1->GetYaxis()->SetLabelFont(42);
	UnderLyingHisto1->GetYaxis()->SetLabelSize(0.05);
	UnderLyingHisto1->GetYaxis()->SetTitleSize(0.06);
	UnderLyingHisto1->GetYaxis()->SetTitleOffset(0.9);
	UnderLyingHisto1->GetYaxis()->SetTitleFont(52);
	UnderLyingHisto1->GetZaxis()->SetLabelSize(0.05);
	UnderLyingHisto1->GetZaxis()->SetTitleSize(0.06);
	gre1->SetHistogram(UnderLyingHisto1);
	gre1->Draw("ap");

	m_canvas->Modified();
	m_canvas->cd();
	m_canvas->SetSelected(m_canvas);
	m_canvas->Write(mapOutputDescription[id1].name.c_str());
	m_canvas->Draw(mapOutputDescription[id1].name.c_str());
}

