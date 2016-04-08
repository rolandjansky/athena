/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void utils() {
  cout << " -> utils loaded!" << endl;
}

void setTileFont(int font=62) {
  if (!AtlasStyle) {
    gPad->Update();
    TPaveText *pt = (TPaveText*)(gPad->GetPrimitive("title"));
    pt->SetTextFont(font);
    gPad->Modified();
  }
}

void ScalePlots() {
  if (normalizePlots) {
    TLatex *t2 = new TLatex();
    t2->SetNDC();
    t2->SetTextFont(font);
    t2->SetTextSize(0.08);
    t2->SetTextAlign(12);
    t2->SetTextAngle(270);
    t2->SetTextSize(0.035);
    t2->SetTextColor(kRed);
    sprintf(name,"Normalized plots");
    t2->DrawLatex(0.92,0.37,name);
  }
}
