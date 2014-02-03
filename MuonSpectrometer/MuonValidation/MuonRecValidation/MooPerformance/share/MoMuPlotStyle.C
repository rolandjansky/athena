{
  
  TStyle* MoMuPlotStyle = new TStyle("MoMuPlotStyle", "The style for MoMu/Muid plots");
//TStyle* MoMuPlotStyle = new TStyle("MoMuPlotStyle","custom style");

  std::cout << "Setting up MoMuPlotStyle" << std::endl;

  //Colors of objects
  MoMuPlotStyle->SetPadColor(0);
  MoMuPlotStyle->SetCanvasColor(0);
  MoMuPlotStyle->SetStatColor(0);
  MoMuPlotStyle->SetPadBorderSize(0);
  MoMuPlotStyle->SetPadBorderMode(0);
  MoMuPlotStyle->SetCanvasBorderSize(0);
  MoMuPlotStyle->SetCanvasBorderMode(0);
  MoMuPlotStyle->SetFrameBorderSize(0);
  MoMuPlotStyle->SetFrameBorderMode(0);
  MoMuPlotStyle->SetPalette(1);

  // set the paper & margin sizes
  MoMuPlotStyle->SetPaperSize(21,27.9);
  MoMuPlotStyle->SetPadTopMargin(0.05);
  MoMuPlotStyle->SetPadRightMargin(0.05);
  MoMuPlotStyle->SetPadLeftMargin(0.15);
  MoMuPlotStyle->SetPadBottomMargin(0.15);

  // use helvetica-bold-r-normal, precision 2 (rotatable)
  //Int_t atlasFont = 132;
  Int_t atlasFont = 62;
  Double_t atlasWidth = 2.00;  // line thickness needed for publisher

  // use large fonts
  MoMuPlotStyle->SetTextFont(atlasFont);
  MoMuPlotStyle->SetTextSize(0.06);
  MoMuPlotStyle->SetLabelFont(atlasFont,"xyz");
  MoMuPlotStyle->SetLabelSize(0.05,"xyz");
  MoMuPlotStyle->SetTitleFont(atlasFont);
  MoMuPlotStyle->SetTitleFont(atlasFont,"xyz");
  MoMuPlotStyle->SetTitleSize(0.06,"xyz");

  // use bold lines and markers
  MoMuPlotStyle->SetHistLineWidth(atlasWidth);
  //MoMuPlotStyle->SetLineWidth(atlasWidth);
  //MoMuPlotStyle->SetFrameLineWidth(atlasWidth);
  //MoMuPlotStyle->SetFuncWidth(atlasWidth);
  //MoMuPlotStyle->SetGridWidth(atlasWidth);
  MoMuPlotStyle->SetLineStyleString(2,"[12 12]"); // postscript dashes
  MoMuPlotStyle->SetLineColor(1);
  MoMuPlotStyle->SetMarkerStyle(8);
  MoMuPlotStyle->SetMarkerSize(1.0);
  MoMuPlotStyle->SetMarkerColor(1);

  // label offsets
  MoMuPlotStyle->SetLabelOffset(0.015);
  MoMuPlotStyle->SetTitleOffset(1.2,"y");
  MoMuPlotStyle->SetTitleOffset(1.2,"z");

  //Title and stat/fit
//  MoMuPlotStyle->SetOptTitle(0);
//  MoMuPlotStyle->SetOptStat(0);
  MoMuPlotStyle->SetOptFit(1111);
  //MoMuPlotStyle->SetOptFit(00101);
  //MoMuPlotStyle->SetFitFormat("3.2f"); 

  // look of the statistics box
  MoMuPlotStyle->SetStatX(1.00 - MoMuPlotStyle->GetPadRightMargin());
  MoMuPlotStyle->SetStatY(1.00 - MoMuPlotStyle->GetPadTopMargin());
  MoMuPlotStyle->SetStatW(0.15);
  MoMuPlotStyle->SetStatH(0.05);
  MoMuPlotStyle->SetStatBorderSize(1);
  MoMuPlotStyle->SetStatBorderSize(1);
  MoMuPlotStyle->SetStatFont(atlasFont);
  MoMuPlotStyle->SetStatFontSize(0.05);

  // histogram divisions: only 5 in x to avoid label overlaps
  MoMuPlotStyle->SetNdivisions(505,"x");
  MoMuPlotStyle->SetNdivisions(510,"y");

  TPaveText *atlasName = new TPaveText(0.65,0.8,0.9,0.9,"BRNDC");
  atlasName->SetTextFont(atlasFont);
  atlasName->SetFillColor(0);
  atlasName->SetTextAlign(12);
  atlasName->SetBorderSize(0);
  atlasName->AddText("Atlas");
  
  TText *atlasLabel = new TText();
  atlasLabel->SetTextFont(atlasFont);
  atlasLabel->SetTextColor(1);
  atlasLabel->SetTextSize(0.04);
  atlasLabel->SetTextAlign(12);

  TLatex *atlasLatex = new TLatex();
  atlasLatex->SetTextFont(atlasFont);
  atlasLatex->SetTextColor(1);
  atlasLatex->SetTextSize(0.04);
  atlasLatex->SetTextAlign(12);

  //Legend!!
  TLegend* thesisleg = new TLegend(0.64,0.84,0.85,0.95);
  thesisleg->SetName("thesisleg");
  thesisleg->SetMargin(.5);
  thesisleg->SetBorderSize(.5);
  thesisleg->SetTextSize(.06);
  thesisleg->SetTextFont(atlasFont);
  thesisleg->SetLineStyle(0);
  thesisleg->SetFillColor(0);
  thesisleg->SetFillStyle(0);

  //Functions!!
  TObject* obj = gROOT->GetFunction("gaus");
  TF1* gaus;
  if(obj){
    gaus= dynamic_cast<TF1*>(obj);
  }
  gaus->SetParName(0, "C");
  gaus->SetParName(1, "#mu");
  gaus->SetParName(2, "#sigma");

  TF1* tFDoubleGaus = new TF1("tFDoubleGaus", "gaus(0) + gaus(3)", -10, 10);
  tFDoubleGaus->SetParName(0, "C_{1}");
  tFDoubleGaus->SetParName(1, "#mu_{1}");
  tFDoubleGaus->SetParName(2, "#sigma_{1}");
  tFDoubleGaus->SetParName(3, "C_{2}");
  tFDoubleGaus->SetParName(4, "#mu_{2}");
  tFDoubleGaus->SetParName(5, "#sigma_{2}");
  //tFDoubleGaus->SetParLimits(0, 0., 1000000000000.);
  //tFDoubleGaus->SetParLimits(2, 0., 1000000000000.);
  //tFDoubleGaus->SetParLimits(3, 0., 1000000000000.);
  //tFDoubleGaus->SetParLimits(5, 0., 1000000000000.);
  tFDoubleGaus->SetParameters(1000.0, 0.0, 0.2, 200.0, 0.0, 0.5);
 
  gROOT->SetStyle("MoMuPlotStyle");
  gROOT->ForceStyle();

}
