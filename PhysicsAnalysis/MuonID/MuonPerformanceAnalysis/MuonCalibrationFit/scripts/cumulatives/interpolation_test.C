/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define N 100

TCanvas* interpolation() {
  gROOT->ProcessLine( ".L AtlasStyle.C" );
  SetAtlasStyle();
  TFile* file = new TFile( "output_test.root" );
  float min = 0;
  float max = 0.1;
  TH2F* means = new TH2F( "means", "", N, min, max, N, min, max );
  TH1F* matrix[ N ][ N ];
  for( int bin_x = 1; bin_x < means->GetNbinsX() + 1; bin_x++ ) {
    for( int bin_y = bin_x; bin_y < means->GetNbinsY() + 1; bin_y++ ) {
      matrix[ bin_x - 1 ][ bin_y - 1 ] = ( TH1F* ) file->Get( Form( "h_%d", means->GetBin( bin_x, bin_y ) ) );
      matrix[ bin_y - 1 ][ bin_x - 1 ] = matrix[ bin_x - 1 ][ bin_y - 1 ];
    }
  }
  float x = gRandom->Uniform( min, max );
  float y = gRandom->Uniform( min, max );
  cout << endl << " Random pair generated: " << x << "   " << y << endl;
  int x_first, x_second, y_first, y_second;
  GetIndeces( x, min, max, float( N ), x_first, x_second );
  GetIndeces( y, min, max, float( N ), y_first, y_second );
  cout << endl << " Indeces obtained (matrix notation): x( " << x_first << ", " << x_second << "), y( " << y_first << ", " << y_second << ")" << endl;
  if( x_first == -1 || y_first == -1 || x_second == N || y_second == N ) return;
  TCanvas* canvas = new TCanvas( "canvas_interpolation", "", 900, 600 );
  canvas->Divide( 1, 2 );
  canvas->cd( 1 );
  matrix[ x_first ][ y_first ]->SetLineColor( kViolet + 1 );
  matrix[ x_first ][ y_first ]->SetLineWidth( 2 );
  matrix[ x_first ][ y_first ]->Draw();
  matrix[ x_second ][ y_first ]->SetLineColor( kAzure + 1 );
  matrix[ x_second ][ y_first ]->SetLineWidth( 2 );
  matrix[ x_second ][ y_first ]->Draw( "same" );
  matrix[ x_first ][ y_second ]->SetLineColor( kOrange + 7 );
  matrix[ x_first ][ y_second ]->SetLineWidth( 2 );
  matrix[ x_first ][ y_second ]->Draw( "same" );
  matrix[ x_second ][ y_second ]->SetLineColor( kSpring + 5 );
  matrix[ x_second ][ y_second ]->SetLineWidth( 2 );
  matrix[ x_second ][ y_second ]->Draw( "same" );
  TH1F* true_h = GetCumulative( 1000, x, y );
  true_h->SetLineColor( kBlack );
  true_h->SetLineWidth( 2 );
  true_h->Draw( "same" );
  float d_1_1 = TMath::Power( x - means->GetXaxis()->GetBinCenter( x_first + 1 ), 2 ) + TMath::Power( y - means->GetYaxis()->GetBinCenter( y_first + 1 ), 2 );
  float d_2_1 = TMath::Power( x - means->GetXaxis()->GetBinCenter( x_second + 1 ), 2 ) + TMath::Power( y - means->GetYaxis()->GetBinCenter( y_first + 1 ), 2 );
  float d_1_2 = TMath::Power( x - means->GetXaxis()->GetBinCenter( x_first + 1 ), 2 ) + TMath::Power( y - means->GetYaxis()->GetBinCenter( y_second + 1 ), 2 );
  float d_2_2 = TMath::Power( x - means->GetXaxis()->GetBinCenter( x_second + 1 ), 2 ) + TMath::Power( y - means->GetYaxis()->GetBinCenter( y_second + 1 ), 2 );
  float f_1_1 = 1. / TMath::Power( d_1_1, 2 );
  float f_2_1 = 1. / TMath::Power( d_2_1, 2 );
  float f_1_2 = 1. / TMath::Power( d_1_2, 2 );
  float f_2_2 = 1. / TMath::Power( d_2_2, 2 );
  float tot = f_1_1 + f_2_1 + f_1_2 + f_2_2;
  TH1F* comb_h = GetCombination( matrix[ x_first ][ y_first ], matrix[ x_second ][ y_first ], matrix[ x_first ][ y_second ], matrix[ x_second ][ y_second ], f_1_1 / tot, f_2_1 / tot, f_1_2 / tot, f_2_2 / tot );
  comb_h->SetLineColor( kRed + 1 );
  comb_h->SetLineWidth( 2 );
  comb_h->SetLineStyle( 2 );
  comb_h->Draw( "same" );
  TLegend* leg = new TLegend( 0.2, 0.9, 0.4, 0.7 );
  leg->SetFillStyle( 0 );
  leg->SetFillColor( 0 );
  leg->SetBorderSize( 0 );
  leg->AddEntry( matrix[ x_first ][ y_first ], Form( "%d,%d bin", x_first, y_first ), "L" );
  leg->AddEntry( matrix[ x_second ][ y_first ], Form( "%d,%d bin", x_second, y_first ), "L" );
  leg->AddEntry( matrix[ x_first ][ y_second ], Form( "%d,%d bin", x_first, y_second ), "L" );
  leg->AddEntry( matrix[ x_second ][ y_second ], Form( "%d,%d bin", x_second, y_second ), "L" );
  leg->AddEntry( true_h, "true", "L" );
  leg->AddEntry( comb_h, "comb", "L" );
  leg->Draw();
  canvas->cd( 2 );
  TH1F* h_1_1 = ( TH1F* ) matrix[ x_first ][ y_first ]->Clone( "h_1_1" );
  TH1F* h_2_1 = ( TH1F* ) matrix[ x_second ][ y_first ]->Clone( "h_2_1" );
  TH1F* h_1_2 = ( TH1F* ) matrix[ x_first ][ y_second ]->Clone( "h_1_2" );
  TH1F* h_2_2 = ( TH1F* ) matrix[ x_second ][ y_second ]->Clone( "h_2_2" );
  TH1F* h_comb = ( TH1F* ) comb_h->Clone( "h_2_2" );
  h_1_1->Add( true_h, -1 );
  h_2_1->Add( true_h, -1 );
  h_1_2->Add( true_h, -1 );
  h_2_2->Add( true_h, -1 );
  h_comb->Add( true_h, -1 );
  // h_1_1->Divide( true_h );
  // h_2_1->Divide( true_h );
  // h_1_2->Divide( true_h );
  // h_2_2->Divide( true_h );
  // h_comb->Divide( true_h );
  h_1_1->SetMaximum( 0.01 );
  h_1_1->SetMinimum( -0.01 );
  h_1_1->Draw();
  h_2_1->Draw( "same" );
  h_1_2->Draw( "same" );
  h_2_2->Draw( "same" );
  h_comb->Draw( "same" );
  return canvas;
}

TH1F* GetCumulative( int index, float x1, float x2 ) {
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

void GetIndeces( float value, float min, float max, float bins, int& first, int& second ) {
  float step = ( max - min ) / bins;
  int temp = floor( ( value - min ) / step );
  if( ( value - min - step * temp ) < ( step * 0.5 ) ) {
    first = temp - 1;
    second = temp;
  }
  else { 
    first = temp;
    second = temp + 1;
  }
}

TH1F* GetCombination( TH1F* h_1_1, TH1F* h_2_1, TH1F* h_1_2, TH1F* h_2_2, float f_1_1, float f_2_1, float f_1_2, float f_2_2 ) {
  TH1F* res = ( TH1F* ) h_1_1->Clone( "res" );
  res->Reset();
  res->Add( h_1_1, f_1_1 );
  res->Add( h_2_1, f_2_1 );
  res->Add( h_1_2, f_1_2 );
  res->Add( h_2_2, f_2_2 );
  return res;
}
