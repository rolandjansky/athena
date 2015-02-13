/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void newtab() {
  TFile* file = new TFile( "output_test.root", "recreate" );
  TH2F* means = new TH2F( "means", "", 100, 0, 0.1, 100, 0, 0.1 );
  TH1F* matrix[ 100 ][ 100 ];
  int index = 0;
  for( int bin_x = 1; bin_x < means->GetNbinsX() + 1; bin_x++ ) {
    for( int bin_y = bin_x; bin_y < means->GetNbinsY() + 1; bin_y++ ) {
      cout << "Iteration n." << ++index << "  ( bin_x, bin_y ) = ( " << bin_x << ", " << bin_y << " )" << endl;
      matrix[ bin_x - 1 ][ bin_y - 1 ] = func( means->GetBin( bin_x, bin_y ), means->GetXaxis()->GetBinCenter( bin_x ), means->GetYaxis()->GetBinCenter( bin_y ) );
    }
  }
  file->Write();
  file->Close();
}

TH1F* func( int index, float x1, float x2 ) {
  TH1F* h = new TH1F( Form( "h_%d", index ), "", 500, 0.7, 1.3 );
  int i = 0;
  while( i++ < 1000000 ) {
    float rnd1 = gRandom->Gaus( 1, x1 );
    float rnd2 = gRandom->Gaus( 1, x2 );
    float val =  1. / ( rnd1 * rnd2 ); 
    h->Fill( val );
  }
  h->ComputeIntegral();
  Double_t *integral = h->GetIntegral();
  h->SetContent(integral);
  return h;
}

