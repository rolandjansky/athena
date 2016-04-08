/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////
void DrawSCTECModules(TH2F *histo, int disc) {
//////////////////////////////////////////////////////
  /*  Number of rings (i.e, module type) in each disk. 
      Values mean:
      1 : outers
      2 : outers + middles
      3 : outers + middles + inners       
  */

  const int nrings[9]={2,3,3,3,3,3,2,2,1};
  
  // number of modules in each ring
  const int nmods[3]={52,40,40};

  float inner_radius[3]={ 7.5, 4.9, 3.4};
  float outer_radius[3]={10.0, 7.4, 4.8};
  // the one before the last wheel has short middle modules
  if (disc==7) {
    inner_radius[1]=6.0;
  }

  TPolyLine *p[52];
  double x[5], y[5];
  double u[5], v[5];

  const float hxl = 1;
  const float hxh = 3;
  const float hy  = 4;

  const float min = histo->GetMinimum();
  const float max = histo->GetMaximum();

  // loop in rings within disk
  for(int ring=0; ring<nrings[disc]; ring++){    
    
    const float phistep = 2*TMath::Pi()/nmods[ring];
    const float inr = inner_radius[ring]; 
    const float our = outer_radius[ring]; 

    // loop in modules within ring
    for(int mod=0; mod<nmods[ring]; mod++){

      for(int j=0; j<5; j++){
	x[j]=0;
	y[j]=0;
      }

      // clockwise if looking z-axis into the page
      float phi = -mod*phistep; 
      
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

      int col_index = locateColor(gStyle->GetNumberOfColors(), 
      				  min, max, 
				  histo->GetBinContent(ring+1,mod+1));
      p[mod] = new TPolyLine(5,x,y);
      if( histo->GetBinContent(ring+1,mod+1)> -1000000) p[mod]->SetFillColor(gStyle->GetColorPalette(col_index));
      else  p[mod]->SetFillColor(kBlack);
     
      

    } // end loop in modules

    for(int mod=0; mod<nmods[ring]; mod++)
      p[mod]->Draw("Fsame");
      
  } // end loop in rings

  return;
}
