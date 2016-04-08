/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void DrawPIXECModules(TH2F *histo, float RangeMin=0., float RangeMax=0. ){
//////////////////////////////////////////////////////
  //cout << " <DrawPIXECModules>("<< histo->GetTitle()<<", " << RangeMin << ", " << RangeMax << ") " << endl;

  const int npixmods=48;  // 48 modules per wheel 

  TPolyLine *p[npixmods];
  double x[5], y[5];
  double u[5], v[5];

  const float hxl = 1;
  const float hxh = 3;
  const float hy  = 4;
  const float phistep = 2*TMath::Pi()/npixmods;
  const float inr =  5; 
  const float our = 10; 

  for(int i=0; i<npixmods; i++){
    for(int j=0; j<5; j++){
      x[j]=0;
      y[j]=0;
    }

    float phi = -i*phistep;

    x[0] = inr*cos(phi-phistep/2);
    x[1] = inr*cos(phi+phistep/2);
    x[2] = our*cos(phi+phistep/2);
    x[3] = our*cos(phi-phistep/2);
    x[4] = inr*cos(phi-phistep/2);

    y[0] = inr*sin(phi-phistep/2);
    y[1] = inr*sin(phi+phistep/2);
    y[2] = our*sin(phi+phistep/2);
    y[3] = our*sin(phi-phistep/2);
    y[4] = inr*sin(phi-phistep/2);

    // Hitogram Z Range
    if (RangeMin>=RangeMax) {
      // Range are not user defined, so let's take the 
      // self histogram range
      RangeMin = histo->GetMinimum();
      RangeMax = histo->GetMaximum();
    } 

    int col_index = locateColor(gStyle->GetNumberOfColors(), 
				RangeMin, RangeMax, 
				histo->GetBinContent(1,i+1));
    if (1==0) {
      cout << " << col_index >> bin " << i 
	   << " res " << histo->GetBinContent(1,i+1)
	   << " Min->Max " << RangeMin << "->" << RangeMax
	   << " col index " << col_index
	   << endl;
    }

    p[i] = new TPolyLine(5,x,y);
    if( histo->GetBinContent(1,i+1)> -1000000) p[i]->SetFillColor(gStyle->GetColorPalette(col_index));
    else  p[i]->SetFillColor(kBlack);
  }

  for(int i=0; i<npixmods; i++)
    p[i]->Draw("Fsame");

}
