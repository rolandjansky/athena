/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __Tools_h__
#define __Tools_h__

#include <iostream>
#include <iomanip>
#include <vector>

#include <TH1.h>
#include <TFile.h>
#include <TString.h>
#include <TVectorT.h>
#include <TLorentzVector.h>

#define MuonMass 0.10565837 
#define WMass 80.385
#define WWidth 2.085 
#define ZMass 91.1876
#define ZWidth 2.4952
#define JpsiMass 3.096916

namespace Type { enum { Data, MC, Background, N }; }

namespace Par { enum { s0, s1, p0, p1, p2, N }; }

namespace Bkg { enum { Exp, Lin, N }; }

namespace Tools {

  float GetWeight( int chNum );

  float GetNormPhi( float phi, float eta ); //::: Return 0.5 for Large sectors and 1.5 for Small ones

  struct BoundsInfo {
    float Min;
    float Max;
    int   Nbins;
    int   Split;
    float Step;
    float OneOverStep;
    //::://:::
    BoundsInfo( float min, float max, int bins, int split ) : Min( min ), Max( max ), Nbins( bins ), Split( split ), Step( ( max - min ) / ( float ) ( bins * split ) ), OneOverStep( split * bins / ( max - min ) ) {}
    //::://:::
    int GetIndex( float value ) { return ceil( ( value - Min ) * OneOverStep ); }
    //::://:::
    float GetLowEdge( int bin ) { return Min + ( bin - 1 ) * Step; }
    //::://:::
    float GetHighEdge( int bin ) { return Min + bin * Step; }
    //::://:::
    int NTotalBins() { return Nbins * Split; }
    //::://:::
    void Print() { std::cout << "   Min: " << Min << " Max: " << Max << " Nbins: " << Nbins << std::endl; }
  };

  struct FloatBoundsInfo {
    float   Min;
    float   Max;
    int     Nbins;
    float*  Bins;
    //::://:::
    FloatBoundsInfo( int nbins, float* bins ) : Min( bins[ 0 ] ), Max( bins[ nbins - 1 ] ), Nbins( nbins ), Bins( bins ) { }
    //::://:::
    ~FloatBoundsInfo() { delete [] Bins; }
    //::://:::
    int GetIndex( float value ) { 
      return 1 + TMath::BinarySearch( Nbins, Bins, value );
    }
    //::://:::
    float GetLowEdge( int bin ) { return Bins[ bin - 1 ]; }
    //::://:::
    float GetHighEdge( int bin ) { return Bins[ bin ]; }
    //::://:::
    void Print() { 
      std::cout << "   Min: " << Min << " Max: " << Max << " Nbins: " << Nbins; 
      for( int i = 0; i < Nbins; i++ ) std::cout << " Bins[" << i << "]: " << Bins[ i ];
      std::cout << std::endl;
    }
  };

  typedef struct {
    float Mean;
    float MeanError;
    float Sigma;
    float SigmaError;
    float Pt;
    //::://:::
    void Print() { 
      std::cout << "   Mean: " << Mean << " +/- " << MeanError << "   Sigma: " << Sigma << " +/- " << SigmaError << "   Average Pt: " << Pt << std::endl;
    }
  } FitInfo;

  typedef struct {
    TString Name;
    float  Min;
    float  Max;
  } VariableInfo;

  struct RandomValues {
    float g1;
    float g2;
    float g3;
    float g4;
    //::://:::
    RandomValues( float _g1_, float _g2_, float _g3_, float _g4_ ) : g1( _g1_ ), g2( _g2_ ), g3( _g3_ ), g4( _g4_ ) {}
  };
  
  typedef struct {
    std::vector< VariableInfo > VarInfo;
    TString Name;
    int Order;
    double par[ Par::N ];
    double err_up[ Par::N ];
    double err_low[ Par::N ];
    bool Corrected;
    //::://:::
    bool HasMuon( double eta, double phi, int sector ) { 
      for( std::vector< VariableInfo >::iterator It = VarInfo.begin(); It != VarInfo.end(); ++It ) {
        double cut_var = -666.;
        if     ( ( *It ).Name == "eta"    ) cut_var = eta;
        else if( ( *It ).Name == "abseta" ) cut_var = TMath::Abs( eta );
        else if( ( *It ).Name == "phi" )    cut_var = phi;
        else if( ( *It ).Name == "absphi" ) cut_var = TMath::Abs( phi );
        else if( ( *It ).Name == "normphi" ) cut_var = GetNormPhi( phi, eta );
        else if( ( *It ).Name == "sector" ) cut_var = sector % 2;
        else {
          std::cerr << "Unknown variable in Regions file!" << std::endl; 
          return kFALSE;
        }
        if( ( *It ).Min > cut_var || cut_var > ( *It ).Max ) return kFALSE;
      }
      return kTRUE;
    }
    //::://:::
    void Print() { std::cout << "\nRegion n." << Order << ", called " << Name << std::endl; }
    //::://:::
    TString GetInfo() { TString res; res.Form( "Region --> Name: %s, Order: %d, s0: %.10f, s1: %.10f, p0: %.10f, p1: %.10f, p2: %.10f, corrected: %d", Name.Data(), Order, par[ Par::s0 ], par[ Par::s1 ], par[ Par::p0 ], par[ Par::p1 ], par[ Par::p2 ], ( int ) Corrected ); return res;  }
    //::://:::
    TString GetString() { TString res; res.Form( "\nRegion n.%d, called %s", Order, Name.Data() ); return res; }
    //::://:::
    void SaveToFile( TFile* file, float chi2, float nll, int ndof ) { 
      file->cd(); 
      TVectorD temp_vector( 3 * Par::N + 3 ); 
      for( int index = 0; index < Par::N; index++ ) {
        temp_vector[ 3 * index ]     = par[ index ];
        temp_vector[ 3 * index + 1 ] = err_up[ index ];
        temp_vector[ 3 * index + 2 ] = err_low[ index ];
      }
      temp_vector[ 3 * Par::N ] = chi2;
      temp_vector[ 3 * Par::N + 1 ] = nll;
      temp_vector[ 3 * Par::N + 2 ] = ndof;
      temp_vector.Write( "FinalResults___" + Name ); 
      TString res = Name;
      res.ReplaceAll( "___", ":::" );
      for( std::vector< VariableInfo >::iterator It = VarInfo.begin(); It != VarInfo.end(); ++It ) {
        TString cut_var = "";
        if( ( *It ).Name == "sector" ) {
          if     ( ( *It ).Min == 0 && ( *It ).Max == 0 ) res += ":::Small sectors"; 
          else if( ( *It ).Min == 1 && ( *It ).Max == 1 ) res += ":::Large sectors"; 
          else if( ( *It ).Min == 0 && ( *It ).Max == 1 ) res += ":::All sectors"; 
          continue;
        }
        if( ( *It ).Name == "normphi" ) {
          if     ( ( *It ).Min == 1 && ( *It ).Max == 2 ) res += ":::Small sectors"; 
          else if( ( *It ).Min == 0 && ( *It ).Max == 1 ) res += ":::Large sectors"; 
          else if( ( *It ).Min == 0 && ( *It ).Max == 2 ) res += ":::All sectors"; 
          continue;
        }
        else if( ( *It ).Name == "eta"    ) cut_var = "#eta";
        else if( ( *It ).Name == "abseta" ) cut_var = "|#eta|";
        else if( ( *It ).Name == "phi" )    cut_var = "#phi";
        else if( ( *It ).Name == "absphi" ) cut_var = "|#phi|";
        else {
          std::cerr << "Unknown variable in Regions file!" << std::endl; 
        }
        res += Form( ":::%.2f < %s < %.2f", ( *It ).Min, cut_var.Data(), ( *It ).Max );
      }
      TNamed temp( Name, res );
      temp.Write();
    }
    //::://:::
    Float_t GetEta() {
      for( std::vector< VariableInfo >::iterator It = VarInfo.begin(); It != VarInfo.end(); ++It ) {
        if ( ( *It ).Name == "eta" || ( *It ).Name == "abseta" ) return 0.5 * ( ( *It ).Min + ( *It ).Max ); 
      }
      return -999.;
    }
    //::://:::
    Float_t Correct( Float_t pt, Float_t r0, Float_t add ) {
      return ( par[ Par::s0 ] + ( 1. + par[ Par::s1 ] ) * pt ) / ( 1 + r0 * TMath::Sqrt( TMath::Power( par[ Par::p0 ] / pt, 2 ) + TMath::Power( par[ Par::p1 ], 2 ) + TMath::Power( par[ Par::p2 ] * pt * add * add, 2 ) ) );
    }
  } RegionInfo;
  
  struct Info {
    float First_Mu_pt;
    float First_Mu_pt_id;
    float First_Mu_eta;
    float First_Mu_phi;
    float Second_Mu_pt;
    float Second_Mu_pt_id;
    float Second_Mu_eta;
    float Second_Mu_phi;
    float Weight;
    float Gamma;
    ULong64_t EvtNum;
    bool  IsOS;
    float FirstAdd = 1.;
    float SecondAdd = 1.;
    //::://:::
    Info( float pt1, float pt1_id, float eta1, float phi1, float pt2, float pt2_id, float eta2, float phi2, float weight, ULong64_t evt_num, bool is_os ) : First_Mu_pt( pt1 ), First_Mu_pt_id( pt1_id ), First_Mu_eta( eta1 ), First_Mu_phi( phi1 ), Second_Mu_pt( pt2 ), Second_Mu_pt_id( pt2_id ), Second_Mu_eta( eta2 ), Second_Mu_phi( phi2 ), Weight( weight ), EvtNum( evt_num ), IsOS( is_os ) {
      TLorentzVector a, b;
      a.SetPtEtaPhiM( pt1, eta1, phi1, MuonMass ); 
      b.SetPtEtaPhiM( pt2, eta2, phi2, MuonMass ); 
      float First_Mu_epsilon = TMath::Power( a.M() / a.P(), 2 );
      float Second_Mu_epsilon = TMath::Power( b.M() / b.P(), 2 );
      //:::
      float First_Mu_sintheta = TMath::Sin( a.Theta() );
      float Second_Mu_sintheta = TMath::Sin( b.Theta() );
      float First_Mu_Taylor = 1. + First_Mu_epsilon / 2. - First_Mu_epsilon * First_Mu_epsilon / 8.;
      float Second_Mu_Taylor = 1. + Second_Mu_epsilon / 2. - Second_Mu_epsilon * Second_Mu_epsilon / 8.;
      float Angle = a.Angle( b.Vect() );
      if( First_Mu_pt_id < 0.000001 && TMath::Abs( First_Mu_eta ) > 2 ) FirstAdd = TMath::Power( TMath::SinH( First_Mu_eta ), 2 );
      if( Second_Mu_pt_id < 0.000001 && TMath::Abs( Second_Mu_eta ) > 2 ) SecondAdd = TMath::Power( TMath::SinH( Second_Mu_eta ), 2 );
      //std::cout << "Just to check, First_Mu_eta=" << First_Mu_eta << " Second_Mu_eta=" << Second_Mu_eta << std::endl;
      //std::cout << "First_Mu_pt_id=" << First_Mu_pt_id << " First_Mu_pt_id < 0.000001=" << ( First_Mu_pt_id < 0.000001 ) << " First_Mu_eta=" << First_Mu_eta << " TMath::Abs( First_Mu_eta ) > 2=" << ( TMath::Abs( First_Mu_eta ) > 2 ) << " FirstAdd=" << FirstAdd << std::endl; 
      //std::cout << "Second_Mu_pt_id=" << Second_Mu_pt_id << " Second_Mu_pt_id < 0.000001=" << ( Second_Mu_pt_id < 0.000001 ) << " Second_Mu_eta=" << Second_Mu_eta << " TMath::Abs( Second_Mu_eta ) > 2=" << ( TMath::Abs( Second_Mu_eta ) > 2 ) << " SecondAdd=" << SecondAdd << std::endl; 
      Gamma = ( First_Mu_Taylor * Second_Mu_Taylor - TMath::Cos( Angle ) ) / ( First_Mu_sintheta * Second_Mu_sintheta );
    }
    //::://:::
    float GetFirst( TString name, const double* pars = NULL, float rnd1 = -999, float rnd2 = -999 ) {
      //::: Trick: variables which use BOTH muons are returned only by this method, so to avoid double-counting!
      if( name == "Eta" ) { 
        return First_Mu_eta;
      }
      if( name == "Pt" ) { 
        if( pars == NULL || rnd1 < -666 ) return First_Mu_pt;
        else                              return Corr_First_Mu_pt( pars, rnd1 ); 
      }
      if( name == "Phi" ) { 
        return First_Mu_phi;
      }
      if( name == "DeltaPhi" ) { 
        TLorentzVector one, two;
        one.SetPtEtaPhiM( GetFirst( "Pt", pars, rnd1, rnd2 ), First_Mu_eta, First_Mu_phi, MuonMass );
        two.SetPtEtaPhiM( GetSecond( "Pt", pars, rnd2 ), Second_Mu_eta, Second_Mu_phi, MuonMass );
        return one.DeltaPhi( two );
      }
      if( name == "DeltaEta" ) { 
        return TMath::Abs( First_Mu_eta - Second_Mu_eta );
      }
      if( name == "PairPt" ) { 
        TLorentzVector one, two;
        one.SetPtEtaPhiM( GetFirst( "Pt", pars, rnd1, rnd2 ), First_Mu_eta, First_Mu_phi, MuonMass );
        two.SetPtEtaPhiM( GetSecond( "Pt", pars, rnd2 ), Second_Mu_eta, Second_Mu_phi, MuonMass );
        return ( one + two ).Pt();
      }
      if( name == "PairEta" ) { 
        TLorentzVector one, two;
        one.SetPtEtaPhiM( GetFirst( "Pt", pars, rnd1, rnd2 ), First_Mu_eta, First_Mu_phi, MuonMass );
        two.SetPtEtaPhiM( GetSecond( "Pt", pars, rnd2 ), Second_Mu_eta, Second_Mu_phi, MuonMass );
        return ( one + two ).Eta();
      }
      if( name == "PairPhi" ) { 
        TLorentzVector one, two;
        one.SetPtEtaPhiM( GetFirst( "Pt", pars, rnd1, rnd2 ), First_Mu_eta, First_Mu_phi, MuonMass );
        two.SetPtEtaPhiM( GetSecond( "Pt", pars, rnd2 ), Second_Mu_eta, Second_Mu_phi, MuonMass );
        return ( one + two ).Phi();
      }
      return -9999999;
    }
    //::://:::
    float GetSecond( TString name, const double* pars = NULL, float rnd2 = -999 ) {
      //::: Trick: variables which use BOTH muons are NOT returned by this method, so to avoid double-counting!
      if( name == "Eta" ) { 
        return Second_Mu_eta;
      }
      if( name == "Pt" ) { 
        if( pars == NULL || rnd2 < -666 ) return Second_Mu_pt;
        else                              return Corr_Second_Mu_pt( pars, rnd2 ); 
      }
      if( name == "Phi" ) { 
        return Second_Mu_phi;
      }
      if( name == "DeltaPhi" ) { 
        return -9999999; 
      }
      if( name == "DeltaEta" ) { 
        return -9999999; 
      }
      if( name == "PairPt" ) { 
        return -9999999; 
      }
      if( name == "PairEta" ) { 
        return -9999999; 
      }
      if( name == "PairPhi" ) { 
        return -9999999; 
      }
      return -9999999;
    }
    //::://:::
    void Print( int type ) {
      std::cout << "FIND SIMILAR ::::: " << type << "\t" << EvtNum << "\t" << First_Mu_eta << "\t" << Second_Mu_eta << std::endl;
    }
    //::://:::
    float GetMass( float pt1, float pt2 ) {
      //TLorentzVector a, b;
      //a.SetPtEtaPhiM( pt1, First_Mu_eta, First_Mu_phi, MuonMass ); 
      //b.SetPtEtaPhiM( pt2, Second_Mu_eta, Second_Mu_phi, MuonMass ); 
      ////std::cout << "TLV    Output: " << ( a + b ).M() << std::endl;
      ////std::cout << "Taylor Output: " << TMath::Sqrt( 2. * ( MuonMass * MuonMass + pt1 * pt2 * Gamma ) ) << std::endl << std::endl;
      //float m1 = ( a + b ).M();
      //float m2 = TMath::Sqrt( 2. * ( MuonMass * MuonMass + pt1 * pt2 * Gamma ) );
      //if( fabs( m1 - m2 ) > 0.0001 ) std::cout << m1 - m2 << "   " << m1 << "   " << m2 << std::endl;
      return TMath::Sqrt( 2. * ( MuonMass * MuonMass + pt1 * pt2 * Gamma ) );
    }
    //::://:::
    float GetMass() {
      return GetMass( First_Mu_pt, Second_Mu_pt );
    }
    //::://:::
    float GetMassSingle( const double* par, double rnd ) {
      return GetMass( Corr_First_Mu_pt( par, rnd ), Second_Mu_pt );
    }
    //::://:::
    float GetMassBoth( const double* par, double rnd1, double rnd2 ) {
      return GetMass( Corr_First_Mu_pt( par, rnd1 ), Corr_Second_Mu_pt( par, rnd2 ) );
    }
    //::://:::
    std::pair< float, float > GetRho( float pt1, float pt2 ) {
      return std::make_pair( pt1 / First_Mu_pt_id - 1., pt2 / Second_Mu_pt_id - 1. );
    }
    //::://:::
    std::pair< float, float > GetRho() {
      return GetRho( First_Mu_pt, Second_Mu_pt );
    }
    //::://:::
    std::pair< float, float > GetRho( const double* par, double rnd ) {
      return GetRho( Corr_First_Mu_pt( par, rnd ), Corr_Second_Mu_pt( par, rnd ) );
    }
    //::://:::
    float Corr_First_Mu_pt( const double* par, double rnd ) {
      return Corr_Mu_pt( par, First_Mu_pt, FirstAdd, rnd ); 
    }
    //::://:::
    float Corr_Second_Mu_pt( const double* par, double rnd ) {
      return Corr_Mu_pt( par, Second_Mu_pt, SecondAdd, rnd ); 
    }
    //::://:::
    float Corr_Mu_pt( const double* par, float pt, float add, float rnd ) {
      //std::cout << " Checking ---> " << add << std::endl;
      return ( par[ Par::s0 ] + ( 1. + par[ Par::s1 ] ) * pt ) / ( 1 + rnd * TMath::Sqrt( TMath::Power( par[ Par::p0 ] / pt, 2 ) + TMath::Power( par[ Par::p1 ], 2 ) + TMath::Power( par[ Par::p2 ] * pt * add, 2 ) ) );
    }
  }; 

  struct MonitoredVariable {
    TString TempName, TempTitle, Name, Title, AxisTitle;
    int JpsiNbins;
    float JpsiMin, JpsiMax;
    int ZNbins;
    float ZMin, ZMax;
    FloatBoundsInfo *JpsiPtBins, *ZPtBins;
    //::://:::
    std::vector< TH1F* > hJpsi[ 2 ][ Type::N ];
    std::vector< TH1F* > hZ[ 2 ] [ Type::N ];
    //::://:::
    MonitoredVariable() : TempName( "" ), TempTitle( "" ), Name( "" ), Title( "" ), AxisTitle( "" ), 
                          JpsiNbins( 1 ), JpsiMin( 0 ), JpsiMax( 0 ), 
                          ZNbins( 1 ), ZMin( 0 ), ZMax( 0 ), 
                          JpsiPtBins( NULL ), ZPtBins( NULL ) {} 
    //::://:::
    MonitoredVariable( TString nameTemp, TString titleTemp, TString name, TString title, TString axistitle, 
                       int jpsinbins, float jpsimin, float jpsimax, 
                       int znbins, float zmin, float zmax, 
                       FloatBoundsInfo* jpsiptbins, FloatBoundsInfo* zptbins ) : 

                       TempName( nameTemp ), TempTitle( titleTemp ), Name( name ), Title( title ), AxisTitle( axistitle ), 
                       JpsiNbins( jpsinbins ), JpsiMin( jpsimin ), JpsiMax( jpsimax ), 
                       ZNbins( znbins ), ZMin( zmin ), ZMax( zmax ), 
                       JpsiPtBins( jpsiptbins ), ZPtBins( zptbins ) { Init(); } 
    //::://:::
    void ScalePlots( /* Arguments to be found :D */ ) {
      // for( int iRoF = 0; iRoF < 2; iRoF++ ) {
      //   for( int ptBin = 1; ptBin < JpsiPtBins->Nbins + 1; ptBin++ ) {
      //     hJpsi[ iRoF ][ Type::MC ].at( ptBin - 1 )->Scale( 
      //       
      //   }
      //   for( int ptBin = 1; ptBin < ZPtBins->Nbins + 1; ptBin++ ) {
      //     hZ[ iRoF ][ iType ].at( ptBin - 1 )->
      //   }
      // }
    }
    //::://:::
    void WritePlots( TString prefix ) {
      for( int iRoF = 0; iRoF < 2; iRoF++ ) {
        for( int iType = 0; iType < Type::N; iType++ ) {
          for( int ptBin = 1; ptBin < JpsiPtBins->Nbins + 1; ptBin++ ) {
            hJpsi[ iRoF ][ iType ].at( ptBin - 1 )->Write( Form( "%s%s", prefix.Data(), hJpsi[ iRoF ][ iType ].at( ptBin - 1 )->GetName() ) );
          }
          for( int ptBin = 1; ptBin < ZPtBins->Nbins + 1; ptBin++ ) {
            hZ[ iRoF ][ iType ].at( ptBin - 1 )->Write( Form( "%s%s", prefix.Data(), hZ[ iRoF ][ iType ].at( ptBin - 1 )->GetName() ) );
          }
        }
      }
    }
    //::://:::
    void ClearPlots() { 
      for( int iRoF = 0; iRoF < 2; iRoF++ ) {
        for( int iType = 0; iType < Type::N; iType++ ) {
          for( int ptBin = 1; ptBin < JpsiPtBins->Nbins + 1; ptBin++ ) {
            hJpsi[ iRoF ][ iType ].at( ptBin - 1 )->Reset();
          }
          for( int ptBin = 1; ptBin < ZPtBins->Nbins + 1; ptBin++ ) {
            hZ[ iRoF ][ iType ].at( ptBin - 1 )->Reset();
          }
        }
      }
    }
    //::://:::
    void Fill( int type, bool isJpsi, bool isRoF, float var, float pt, float weight ) {
      int indexRoF = ( isRoF ) ? 0 : 1; 
      int PtBin = ( isJpsi ) ? JpsiPtBins->GetIndex( pt ) : ZPtBins->GetIndex( pt );
      bool validPtBin = ( isJpsi ) ? ( PtBin < JpsiPtBins->Nbins + 1 ) : ( PtBin < ZPtBins->Nbins + 1 );
      validPtBin = ( validPtBin && ( PtBin > 0 ) );
      if( validPtBin ) {
        if( isJpsi ) hJpsi[ indexRoF ][ type ].at( PtBin - 1 )->Fill( var, weight ); 
        else hZ[ indexRoF ][ type ].at( PtBin - 1 )->Fill( var, weight ); 
      }
    }
    //::://:::
    void Init() {
      TString RoF_Name[ 2 ] = { "InRoF", "OutRoF" };
      TString LocalType[ Type::N ] = { "Data", "MC", "Bkg" };
      for( int iRoF = 0; iRoF < 2; iRoF++ ) {
        for( int iType = 0; iType < Type::N; iType++ ) {
          //::: Jpsi Plots
          for( int ptBin = 1; ptBin < JpsiPtBins->Nbins + 1; ptBin++ ) {
            char name[ 100 ], title[ 100 ];
            sprintf( name, "~%s~Jpsi%s%s~%s~%.1f_pt_%.1f", TempName.Data(), Name.Data(), RoF_Name[ iRoF ].Data(), LocalType[ iType ].Data(), JpsiPtBins->GetLowEdge( ptBin ), JpsiPtBins->GetHighEdge( ptBin ) );
            sprintf( title, "%s:::J/#psi %s:::%.1f<p_{T}[GeV]<%.1f;%s;Entries", TempTitle.Data(), Title.Data(), JpsiPtBins->GetLowEdge( ptBin ), JpsiPtBins->GetHighEdge( ptBin ), AxisTitle.Data() );
            TString name_with_dots = name;
            TH1F* temp = new TH1F( name_with_dots.ReplaceAll( ".", "_" ), title, JpsiNbins, JpsiMin, JpsiMax );
            hJpsi[ iRoF ][ iType ].push_back( temp ); 
          }
          //::: Z Plots
          for( int ptBin = 1; ptBin < ZPtBins->Nbins + 1; ptBin++ ) {
            char name[ 100 ], title[ 100 ];
            sprintf( name, "~%s~Z%s%s~%s~%.1f_pt_%.1f", TempName.Data(), Name.Data(), RoF_Name[ iRoF ].Data(), LocalType[ iType ].Data(), ZPtBins->GetLowEdge( ptBin ), ZPtBins->GetHighEdge( ptBin ) );
            sprintf( title, "%s:::J/#psi %s:::%.1f<p_{T}[GeV]<%.1f;%s;Entries", TempTitle.Data(), Title.Data(), ZPtBins->GetLowEdge( ptBin ), ZPtBins->GetHighEdge( ptBin ), AxisTitle.Data() );
            TString name_with_dots = name;
            TH1F* temp = new TH1F( name_with_dots.ReplaceAll( ".", "_" ), title, ZNbins, ZMin, ZMax );
            hZ[ iRoF ][ iType ].push_back( temp ); 
          }
        }
      }
    }
  }; 

  bool RegionsSorter( RegionInfo* a, RegionInfo* b );

}

#endif
