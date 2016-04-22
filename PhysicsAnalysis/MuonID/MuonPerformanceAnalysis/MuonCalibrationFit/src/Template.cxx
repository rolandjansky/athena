/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define Template_cxx
#include "MuonCalibrationFit/Template.h"

Template::Template( TString name, TString title, int splitting, std::string bkg ) {
  //:::
  m_Name = name;
  m_Title = title;
  m_Splitting = splitting;
  //:::
  m_VectorHistograms = NULL;
  //:::
  m_MassJpsiContentBuffer_Data = NULL;
  m_MassZContentBuffer_Data = NULL;
  m_RhoZContentBuffer_Data = NULL;
  m_MassJpsiContentBuffer_MonteCarlo = NULL;
  m_MassJpsiErrorBuffer_MonteCarlo = NULL;
  m_MassZContentBuffer_MonteCarlo = NULL;
  m_MassZErrorBuffer_MonteCarlo = NULL;
  m_RhoZContentBuffer_MonteCarlo = NULL;
  m_RhoZErrorBuffer_MonteCarlo = NULL;
  m_MassJpsiContentBuffer_Background = NULL;
  m_MassJpsiErrorBuffer_Background = NULL;
  m_MassZContentBuffer_Background = NULL;
  m_MassZErrorBuffer_Background = NULL;
  m_RhoZContentBuffer_Background = NULL;
  m_RhoZErrorBuffer_Background = NULL;
  m_MassJpsiAddWeightBuffer_MonteCarlo = NULL;
  m_MassZAddWeightBuffer_MonteCarlo = NULL;
  m_RhoZAddWeightBuffer_MonteCarlo = NULL;
  m_MassJpsi = NULL;
  m_MassZ = NULL;
  m_EtaZ = NULL;
  m_PtZ = NULL;
  m_RhoZ = NULL;
  m_MassJpsiPtBins = NULL;
  m_MassZPtBins = NULL;
  m_RhoZPtBins = NULL;
  m_Weights = new TH1F( m_Name + "_Weights_DummyHistogram", "", 1, 0, 1 );
  m_SetGlobalWeights = kFALSE;
  m_SetAverageWeight = kFALSE;
  m_EvaluatedBackground = kFALSE;
  for( int i = 0; i < Type::N; i++ ) {
    m_FitInfo[ i ] = new std::vector< Tools::FitInfo* >;
  }
  m_Bkg = bkg;
  m_AverageWeight = -1.;
  m_EmptyMonitoredPlots = true;
}

Template::~Template() {
  if( m_MassJpsiContentBuffer_Data )         Delete( m_MassJpsiContentBuffer_Data );
  if( m_MassZContentBuffer_Data )            Delete( m_MassZContentBuffer_Data );
  if( m_RhoZContentBuffer_Data )             Delete( m_RhoZContentBuffer_Data );
  if( m_MassJpsiContentBuffer_MonteCarlo )   Delete( m_MassJpsiContentBuffer_MonteCarlo );
  if( m_MassJpsiErrorBuffer_MonteCarlo )     Delete( m_MassJpsiErrorBuffer_MonteCarlo );
  if( m_MassZContentBuffer_MonteCarlo )      Delete( m_MassZContentBuffer_MonteCarlo );
  if( m_MassZErrorBuffer_MonteCarlo )        Delete( m_MassZErrorBuffer_MonteCarlo );
  if( m_RhoZContentBuffer_MonteCarlo )       Delete( m_RhoZContentBuffer_MonteCarlo );
  if( m_RhoZErrorBuffer_MonteCarlo )         Delete( m_RhoZErrorBuffer_MonteCarlo );
  if( m_MassJpsiContentBuffer_Background )   Delete( m_MassJpsiContentBuffer_Background );
  if( m_MassJpsiErrorBuffer_Background )     Delete( m_MassJpsiErrorBuffer_Background );
  if( m_MassZContentBuffer_Background )      Delete( m_MassZContentBuffer_Background );
  if( m_MassZErrorBuffer_Background )        Delete( m_MassZErrorBuffer_Background );
  if( m_RhoZContentBuffer_Background )       Delete( m_RhoZContentBuffer_Background );
  if( m_RhoZErrorBuffer_Background )         Delete( m_RhoZErrorBuffer_Background );
  if( m_MassJpsiAddWeightBuffer_MonteCarlo ) Delete( m_MassJpsiAddWeightBuffer_MonteCarlo );
  if( m_MassZAddWeightBuffer_MonteCarlo )    Delete( m_MassZAddWeightBuffer_MonteCarlo );
  if( m_RhoZAddWeightBuffer_MonteCarlo )     Delete( m_RhoZAddWeightBuffer_MonteCarlo );
  if( m_MassJpsi != NULL ) delete m_MassJpsi;
  if( m_MassZ != NULL ) delete m_MassZ;
  if( m_EtaZ != NULL ) delete m_EtaZ;
  if( m_PtZ != NULL ) delete m_PtZ;
  if( m_RhoZ != NULL ) delete m_RhoZ;
  if( m_MassJpsiPtBins != NULL ) delete m_MassJpsiPtBins;
  if( m_MassZPtBins != NULL ) delete m_MassZPtBins;
  if( m_RhoZPtBins != NULL ) delete m_RhoZPtBins;
  if( m_Weights != NULL ) delete m_Weights;
  for( int i = 0; i < Type::N; i++ ) {
    for( std::vector< Tools::FitInfo* >::iterator iter = m_FitInfo[ i ]->begin(); iter != m_FitInfo[ i ]->end(); ++iter ) {
      delete ( *iter );
    }
    delete m_FitInfo[ i ];
  }
}

void Template::Add( int nBins, float xMin, float xMax, std::vector< float >* ptBins, Tools::BoundsInfo* &dummyBin, Tools::FloatBoundsInfo* &dummyBinPt, float* &data_contentBuffer, float* &mc_contentBuffer, float* &mc_errorBuffer, float* &mc_addweightBuffer, float* &bkg_contentBuffer, float* &bkg_errorBuffer ) {
  dummyBin = new Tools::BoundsInfo( xMin, xMax, nBins, m_Splitting );
  dummyBinPt = new Tools::FloatBoundsInfo( ptBins->size() - 1, &( ptBins->at( 0 ) ) );
  data_contentBuffer = new float[ dummyBinPt->Nbins * dummyBin->NTotalBins() ];
  mc_contentBuffer   = new float[ dummyBinPt->Nbins * dummyBin->NTotalBins() ];
  mc_errorBuffer     = new float[ dummyBinPt->Nbins * dummyBin->NTotalBins() ];
  bkg_contentBuffer  = new float[ dummyBinPt->Nbins * dummyBin->NTotalBins() ];
  bkg_errorBuffer    = new float[ dummyBinPt->Nbins * dummyBin->NTotalBins() ];
  mc_addweightBuffer = new float[ dummyBinPt->Nbins * dummyBin->NTotalBins() ];
  for( int i = 0; i < dummyBinPt->Nbins * dummyBin->NTotalBins(); i++ ) {
    data_contentBuffer[ i ] = 0.;
    mc_contentBuffer  [ i ] = 0.;
    mc_errorBuffer    [ i ] = 0.;
    bkg_contentBuffer [ i ] = 0.;
    bkg_errorBuffer   [ i ] = 0.;
    mc_addweightBuffer[ i ] = 0.;
  }
}

void Template::AddMassJpsi( int nBins, float xMin, float xMax, std::vector< float >* ptBins ) {
  return Add( nBins, xMin, xMax, ptBins, m_MassJpsi, m_MassJpsiPtBins, m_MassJpsiContentBuffer_Data, m_MassJpsiContentBuffer_MonteCarlo, m_MassJpsiErrorBuffer_MonteCarlo, m_MassJpsiAddWeightBuffer_MonteCarlo, m_MassJpsiContentBuffer_Background, m_MassJpsiErrorBuffer_Background ); 
}

void Template::AddMassZ( int nBins, float xMin, float xMax, std::vector< float >* ptBins ) {
  return Add( nBins, xMin, xMax, ptBins, m_MassZ, m_MassZPtBins, m_MassZContentBuffer_Data, m_MassZContentBuffer_MonteCarlo, m_MassZErrorBuffer_MonteCarlo, m_MassZAddWeightBuffer_MonteCarlo, m_MassZContentBuffer_Background, m_MassZErrorBuffer_Background ); 
}

void Template::AddRhoZ( int nBins, float xMin, float xMax, std::vector< float >* ptBins ) {
  return Add( nBins, xMin, xMax, ptBins, m_RhoZ, m_RhoZPtBins, m_RhoZContentBuffer_Data, m_RhoZContentBuffer_MonteCarlo, m_RhoZErrorBuffer_MonteCarlo, m_RhoZAddWeightBuffer_MonteCarlo, m_RhoZContentBuffer_Background, m_RhoZErrorBuffer_Background ); 
}

void Template::AddMonitoredVariable( std::string name, std::string title, std::string axistitle, int jpsi_n_bins, float jpsi_min, float jpsi_max, int z_n_bins, float z_min, float z_max ) {
  Tools::MonitoredVariable Var( m_Name, m_Title, name, title, axistitle, jpsi_n_bins, jpsi_min, jpsi_max, z_n_bins, z_min, z_max, m_MassJpsiPtBins, m_MassZPtBins ); 
  m_MonVars[ name ] = Var;
}


void Template::FillMass( int type, float mass, float pt, float weight ) {
  m_Weights->Fill( 0.5, weight );
  //::: Jpsi Mass
  if( m_MassJpsi != NULL ) { 
    int Bin = m_MassJpsi->GetIndex( mass );
    if( Bin > 0 && Bin < m_MassJpsi->NTotalBins() + 1 ) {
      int PtBin = m_MassJpsiPtBins->GetIndex( pt );
      if( PtBin > 0 && PtBin < m_MassJpsiPtBins->Nbins + 1 ) {
        switch( type ) {
          //::: Data without smoothing
          case Type::Data:
            m_MassJpsiContentBuffer_Data[ ( PtBin - 1 ) * m_MassJpsi->NTotalBins() + Bin - 1 ] += 1;
            break;
          //::: MC without smoothing
          case Type::MC:
            m_MassJpsiContentBuffer_MonteCarlo[ ( PtBin - 1 ) * m_MassJpsi->NTotalBins() + Bin - 1 ] += weight; 
            m_MassJpsiErrorBuffer_MonteCarlo[ ( PtBin - 1 ) * m_MassJpsi->NTotalBins() + Bin - 1 ] += ( weight * weight ); 
            break;
          //::: End of switch
        }
      }
    }
  }
  //::: Z Mass
  if( m_MassZ != NULL ) { 
    int Bin = m_MassZ->GetIndex( mass );
    if( Bin > 0 && Bin < m_MassZ->NTotalBins() + 1 ) {
      int PtBin = m_MassZPtBins->GetIndex( pt );
      if( PtBin > 0 && PtBin < m_MassZPtBins->Nbins + 1 ) {
        switch( type ) {
          //::: Data without smoothing
          case Type::Data:
            m_MassZContentBuffer_Data[ ( PtBin - 1 ) * m_MassZ->NTotalBins() + Bin - 1 ] += 1;
            break;
          //::: MC without smoothing
          case Type::MC:
            //std::cout << "  ---  " << Bin << "  ---  " << PtBin << "  ---  " << mass << "  ---  " << weight << std::endl;
            //std::cout << "  ---  " << mass << "  ---  " << std::endl;
            m_MassZContentBuffer_MonteCarlo[ ( PtBin - 1 ) * m_MassZ->NTotalBins() + Bin - 1 ] += weight; 
            m_MassZErrorBuffer_MonteCarlo[ ( PtBin - 1 ) * m_MassZ->NTotalBins() + Bin - 1 ] += ( weight * weight ); 
            break;
          //::: MC background without smoothing
          case Type::Background:
            m_MassZContentBuffer_Background[ ( PtBin - 1 ) * m_MassZ->NTotalBins() + Bin - 1 ] += weight; 
            m_MassZErrorBuffer_Background[ ( PtBin - 1 ) * m_MassZ->NTotalBins() + Bin - 1 ] += ( weight * weight ); 
            break;
          //::: End of switch
        }
      }
    }
  }
}

void Template::FillRho( int type, float rho, float pt, float weight ) {
  m_Weights->Fill( 0.5, weight );
  //::: Z Rho
  if( m_RhoZ != NULL ) { 
    int Bin = m_RhoZ->GetIndex( rho );
    if( Bin > 0 && Bin < m_RhoZ->NTotalBins() + 1 ) {
      int PtBin = m_RhoZPtBins->GetIndex( pt );
      if( PtBin > 0 && PtBin < m_RhoZPtBins->Nbins + 1 ) {
        switch( type ) {
          //::: Data without smoothing
          case Type::Data:
            m_RhoZContentBuffer_Data[ ( PtBin - 1 ) * m_RhoZ->NTotalBins() + Bin - 1 ] += 1;
            break;
          //::: MC without smoothing
          case Type::MC:
            m_RhoZContentBuffer_MonteCarlo[ ( PtBin - 1 ) * m_RhoZ->NTotalBins() + Bin - 1 ] += weight; 
            m_RhoZErrorBuffer_MonteCarlo[ ( PtBin - 1 ) * m_RhoZ->NTotalBins() + Bin - 1 ] += ( weight * weight ); 
            break;
          //::: MC background without smoothing
          case Type::Background:
            m_RhoZContentBuffer_Background[ ( PtBin - 1 ) * m_RhoZ->NTotalBins() + Bin - 1 ] += weight; 
            m_RhoZErrorBuffer_Background[ ( PtBin - 1 ) * m_RhoZ->NTotalBins() + Bin - 1 ] += ( weight * weight ); 
            break;
          //::: End of switch
        }
      }
    }
  }
}

void Template::FillMonitoredVariable( int type, std::string name, float var, float pt, float mass, bool os, float weight, bool inRoF ) {
  int local_type = type;
  bool is_jpsi = ( mass < 50 );
  bool within_mass_range = false;
  if( is_jpsi && ( mass >= m_MassJpsi->Min ) && ( mass <= m_MassJpsi->Max ) ) within_mass_range = true;
  if( !is_jpsi && ( mass >= m_MassZ->Min ) && ( mass <= m_MassZ->Max ) ) within_mass_range = true;
  if( !within_mass_range ) return;
  if( is_jpsi && !os ) { 
    local_type = Type::Background;
  }
  if( os || ( is_jpsi && local_type == Type::Background ) ) {
    m_MonVars[ name ].Fill( local_type, is_jpsi, inRoF, var, pt, weight );
  }
}

void Template::Print() {
  /*
  std::cout << " ::: " << std::setw( 30 ) << std::right << "m_MassJpsiContentBuffer_Data" << "    " << std::left << m_MassJpsiContentBuffer_Data << std::endl;
  Print( m_MassJpsiContentBuffer_Data, m_MassJpsiPtBins->Nbins * m_MassJpsi->NTotalBins() );
  std::cout << " ::: " << std::setw( 30 ) << std::right << "m_MassZContentBuffer_Data" << "    " << std::left << m_MassZContentBuffer_Data << std::endl;
  Print( m_MassZContentBuffer_Data, m_MassZPtBins->Nbins * m_MassZ->NTotalBins() );
  std::cout << " ::: " << std::setw( 30 ) << std::right << "m_MassJpsiContentBuffer_MonteCarlo" << "    " << std::left << m_MassJpsiContentBuffer_MonteCarlo << std::endl;
  Print( m_MassJpsiContentBuffer_MonteCarlo, m_MassJpsiPtBins->Nbins * m_MassJpsi->NTotalBins() );
  std::cout << " ::: " << std::setw( 30 ) << std::right << "m_MassJpsiErrorBuffer_MonteCarlo" << "    " << std::left << m_MassJpsiErrorBuffer_MonteCarlo << std::endl;
  Print( m_MassJpsiErrorBuffer_MonteCarlo, m_MassJpsiPtBins->Nbins * m_MassJpsi->NTotalBins() );
  std::cout << " ::: " << std::setw( 30 ) << std::right << "m_MassJpsiAddWeightBuffer_MonteCarlo" << "    " << std::left << m_MassJpsiAddWeightBuffer_MonteCarlo << std::endl;
  Print( m_MassJpsiAddWeightBuffer_MonteCarlo, m_MassJpsiPtBins->Nbins * m_MassJpsi->NTotalBins() );
  std::cout << " ::: " << std::setw( 30 ) << std::right << "m_MassZContentBuffer_MonteCarlo" << "    " << std::left << m_MassZContentBuffer_MonteCarlo << std::endl;
  Print( m_MassZContentBuffer_MonteCarlo, m_MassZPtBins->Nbins * m_MassZ->NTotalBins() );
  std::cout << " ::: " << std::setw( 30 ) << std::right << "m_MassZErrorBuffer_MonteCarlo" << "    " << std::left << m_MassZErrorBuffer_MonteCarlo << std::endl;
  Print( m_MassZErrorBuffer_MonteCarlo, m_MassZPtBins->Nbins * m_MassZ->NTotalBins() );
  std::cout << " ::: " << std::setw( 30 ) << std::right << "m_MassZAddWeightBuffer_MonteCarlo" << "    " << std::left << m_MassZAddWeightBuffer_MonteCarlo << std::endl;
  Print( m_MassZAddWeightBuffer_MonteCarlo, m_MassZPtBins->Nbins * m_MassZ->NTotalBins() );
  std::cout << " ::: " << std::setw( 30 ) << std::right << "m_MassJpsiContentBuffer_Background" << "    " << std::left << m_MassJpsiContentBuffer_Background << std::endl;
  Print( m_MassJpsiContentBuffer_Background, m_MassJpsiPtBins->Nbins * m_MassJpsi->NTotalBins() );
  std::cout << " ::: " << std::setw( 30 ) << std::right << "m_MassJpsiErrorBuffer_Background" << "    " << std::left << m_MassJpsiErrorBuffer_Background << std::endl;
  Print( m_MassJpsiErrorBuffer_Background, m_MassJpsiPtBins->Nbins * m_MassJpsi->NTotalBins() );
  std::cout << " ::: " << std::setw( 30 ) << std::right << "m_MassZContentBuffer_Background" << "    " << std::left << m_MassZContentBuffer_Background << std::endl;
  Print( m_MassZContentBuffer_Background, m_MassZPtBins->Nbins * m_MassZ->NTotalBins() );
  std::cout << " ::: " << std::setw( 30 ) << std::right << "m_MassZErrorBuffer_Background" << "    " << std::left << m_MassZErrorBuffer_Background << std::endl;
  Print( m_MassZErrorBuffer_Background, m_MassZPtBins->Nbins * m_MassZ->NTotalBins() );
  std::cout << " ::: " << std::setw( 30 ) << std::right << "m_MassJpsi" << "    " << std::left << m_MassJpsi << std::endl;
  std::cout << " ::: " << std::setw( 30 ) << std::right << "m_MassZ" << "    " << std::left << m_MassZ << std::endl;
  std::cout << " ::: " << std::setw( 30 ) << std::right << "m_MassJpsiPtBins" << "    " << std::left << m_MassJpsiPtBins << std::endl;
  m_MassJpsiPtBins->Print();
  std::cout << " ::: " << std::setw( 30 ) << std::right << "m_MassZPtBins" << "    " << std::left << m_MassZPtBins << std::endl;
  m_MassZPtBins->Print();
  std::cout << " ::: " << std::setw( 30 ) << std::right << "m_Weights" << "    " << std::left << m_Weights << std::endl;
  */
}

void Template::Print( float* Vec, int n ) {
  for( int i = 0; i < n; i++ ) {
    std::cout << "  " << Vec[ i ];
  }
}

void Template::Write( TString PreFix ) {
  //:::
  std::vector< TH1F* >* hists = GetAllHistograms( PreFix, true );
  for( std::vector< TH1F* >::iterator It = hists->begin(); It != hists->end(); ++It ) {
    ( *It )->Write();
  }
  //::: Monitoring Plots: ( Rescale, Save and Clear )
  if( ! m_EmptyMonitoredPlots ) {
    for( auto& KeyValue: m_MonVars ) {
      ( KeyValue.second ).ScalePlots( /* Arguments to be found :D */ );
      ( KeyValue.second ).WritePlots( PreFix );
      ( KeyValue.second ).ClearPlots();
    }
    m_EmptyMonitoredPlots = true;
  }
}

std::vector< TH1F* >* Template::GetAllHistograms( TString PreFix, bool recalculate ) {
  if( m_VectorHistograms != NULL && !recalculate ) return m_VectorHistograms;
  //:::
  int TotalSize = 0;
  //:::
  std::vector< TH1F* > t_MassJpsiVec_MC;
  if( m_MassJpsiContentBuffer_MonteCarlo != NULL ) {
    for( int ptBin = 1; ptBin < m_MassJpsiPtBins->Nbins + 1; ptBin++ ) {
      char name[100], title[100];
      sprintf( name, "%s~%s~MassJpsi~MC~%.1f_pt_%.1f", PreFix.Data(), m_Name.Data(), m_MassJpsiPtBins->GetLowEdge( ptBin ), m_MassJpsiPtBins->GetHighEdge( ptBin ) );
      sprintf( title, "%s:::J/#psi Mass:::%.1f<p_{T}[GeV]<%.1f;m_{#mu#mu} [GeV];Entries", m_Title.Data(), m_MassJpsiPtBins->GetLowEdge( ptBin ), m_MassJpsiPtBins->GetHighEdge( ptBin ) );
      TString name_with_dots = name;
      TH1F* temp = new TH1F( name_with_dots.ReplaceAll( ".", "_" ), title, m_MassJpsi->Nbins, m_MassJpsi->Min, m_MassJpsi->Max );
      t_MassJpsiVec_MC.push_back( temp ); 
      ++TotalSize;
      for( int Bin = 1; Bin < m_MassJpsi->Nbins + 1; Bin++ ) {
        float val( 0. ), err( 0. );
        for( int IntBin = 0; IntBin < m_MassJpsi->Split; IntBin++ ) {
          int FinalBin = ( Bin - 1 ) * m_MassJpsi->Split + IntBin;
          int theBin = ( ptBin - 1 ) * m_MassJpsi->NTotalBins() + FinalBin;
          val += m_MassJpsiContentBuffer_MonteCarlo[ theBin ] * m_MassJpsiAddWeightBuffer_MonteCarlo[ theBin ]; 
          err += m_MassJpsiErrorBuffer_MonteCarlo[ theBin ] * TMath::Power( m_MassJpsiAddWeightBuffer_MonteCarlo[ theBin ], 2 ); 
        }
        t_MassJpsiVec_MC[ ptBin - 1 ]->SetBinContent( Bin, val ); 
        t_MassJpsiVec_MC[ ptBin - 1 ]->SetBinError( Bin, TMath::Sqrt( err ) ); 
      }
    }
  }
  //:::
  std::vector< TH1F* > t_MassJpsiVec_Bkg;
  if( m_MassJpsiContentBuffer_Background != NULL ) {
    for( int ptBin = 1; ptBin < m_MassJpsiPtBins->Nbins + 1; ptBin++ ) {
      char name[100], title[100];
      sprintf( name, "%s~%s~MassJpsi~Bkg~%.1f_pt_%.1f", PreFix.Data(), m_Name.Data(), m_MassJpsiPtBins->GetLowEdge( ptBin ), m_MassJpsiPtBins->GetHighEdge( ptBin ) );
      sprintf( title, "%s:::J/#psi Mass:::%.1f<p_{T}[GeV]<%.1f;m_{#mu#mu} [GeV];Entries", m_Title.Data(), m_MassJpsiPtBins->GetLowEdge( ptBin ), m_MassJpsiPtBins->GetHighEdge( ptBin ) );
      TString name_with_dots = name;
      TH1F* temp = new TH1F( name_with_dots.ReplaceAll( ".", "_" ), title, m_MassJpsi->Nbins, m_MassJpsi->Min, m_MassJpsi->Max );
      t_MassJpsiVec_Bkg.push_back( temp ); 
      ++TotalSize;
      for( int Bin = 1; Bin < m_MassJpsi->Nbins + 1; Bin++ ) {
        float val( 0. ), err( 0. );
        for( int IntBin = 0; IntBin < m_MassJpsi->Split; IntBin++ ) {
          int FinalBin = ( Bin - 1 ) * m_MassJpsi->Split + IntBin;
          int theBin = ( ptBin - 1 ) * m_MassJpsi->NTotalBins() + FinalBin;
          val += m_MassJpsiContentBuffer_Background[ theBin ];
          err += m_MassJpsiErrorBuffer_Background[ theBin ];
        }
        t_MassJpsiVec_Bkg[ ptBin - 1 ]->SetBinContent( Bin, val ); 
        t_MassJpsiVec_Bkg[ ptBin - 1 ]->SetBinError( Bin, TMath::Sqrt( err ) ); 
      }
    }
  }
  //:::
  std::vector< TH1F* > t_MassJpsiVec_Data;
  if( m_MassJpsiContentBuffer_Data != NULL ) {
    for( int ptBin = 1; ptBin < m_MassJpsiPtBins->Nbins + 1; ptBin++ ) {
      char name[100], title[100];
      sprintf( name, "%s~%s~MassJpsi~Data~%.1f_pt_%.1f", PreFix.Data(), m_Name.Data(), m_MassJpsiPtBins->GetLowEdge( ptBin ), m_MassJpsiPtBins->GetHighEdge( ptBin ) );
      sprintf( title, "%s:::J/#psi Mass:::%.1f<p_{T}[GeV]<%.1f;m_{#mu#mu} [GeV];Entries", m_Title.Data(), m_MassJpsiPtBins->GetLowEdge( ptBin ), m_MassJpsiPtBins->GetHighEdge( ptBin ) );
      TString name_with_dots = name;
      TH1F* temp = new TH1F( name_with_dots.ReplaceAll( ".", "_" ), title, m_MassJpsi->Nbins, m_MassJpsi->Min, m_MassJpsi->Max );
      t_MassJpsiVec_Data.push_back( temp );
      ++TotalSize;
      for( int Bin = 1; Bin < m_MassJpsi->Nbins + 1; Bin++ ) {
        float val( 0. );
        for( int IntBin = 0; IntBin < m_MassJpsi->Split; IntBin++ ) {
          int FinalBin = ( Bin - 1 ) * m_MassJpsi->Split + IntBin;
          int theBin = ( ptBin - 1 ) * m_MassJpsi->NTotalBins() + FinalBin;
          val += m_MassJpsiContentBuffer_Data[ theBin ]; 
        }
        t_MassJpsiVec_Data[ ptBin - 1 ]->SetBinContent( Bin, val ); 
        t_MassJpsiVec_Data[ ptBin - 1 ]->SetBinError( Bin, TMath::Sqrt( val ) ); 
      }
    }
  }
  //:::
  std::vector< TH1F* > t_MassZVec_MC;
  if( m_MassZContentBuffer_MonteCarlo != NULL ) {
    for( int ptBin = 1; ptBin < m_MassZPtBins->Nbins + 1; ptBin++ ) {
      char name[100], title[100];
      sprintf( name, "%s~%s~MassZ~MC~%.1f_pt_%.1f", PreFix.Data(), m_Name.Data(), m_MassZPtBins->GetLowEdge( ptBin ), m_MassZPtBins->GetHighEdge( ptBin ) );
      sprintf( title, "%s:::Z Mass:::%.1f<p_{T}[GeV]<%.1f;m_{#mu#mu} [GeV];Entries", m_Title.Data(), m_MassZPtBins->GetLowEdge( ptBin ), m_MassZPtBins->GetHighEdge( ptBin ) );
      TString name_with_dots = name;
      TH1F* temp = new TH1F( name_with_dots.ReplaceAll( ".", "_" ), title, m_MassZ->Nbins, m_MassZ->Min, m_MassZ->Max );
      t_MassZVec_MC.push_back( temp );
      ++TotalSize;
      for( int Bin = 1; Bin < m_MassZ->Nbins + 1; Bin++ ) {
        float val( 0. ), err( 0. );
        for( int IntBin = 0; IntBin < m_MassZ->Split; IntBin++ ) {
          int FinalBin = ( Bin - 1 ) * m_MassZ->Split + IntBin;
          int theBin = ( ptBin - 1 ) * m_MassZ->NTotalBins() + FinalBin;
          val += m_MassZContentBuffer_MonteCarlo[ theBin ] * m_MassZAddWeightBuffer_MonteCarlo[ theBin ]; 
          err += m_MassZErrorBuffer_MonteCarlo[ theBin ] * TMath::Power( m_MassZAddWeightBuffer_MonteCarlo[ theBin ], 2 ); 
        }
        t_MassZVec_MC[ ptBin - 1 ]->SetBinContent( Bin, val ); 
        t_MassZVec_MC[ ptBin - 1 ]->SetBinError( Bin, TMath::Sqrt( err ) ); 
      }
    }
  }
  //:::
  std::vector< TH1F* > t_MassZVec_Bkg;
  if( m_MassZContentBuffer_Background != NULL ) {
    for( int ptBin = 1; ptBin < m_MassZPtBins->Nbins + 1; ptBin++ ) {
      char name[100], title[100];
      sprintf( name, "%s~%s~MassZ~Bkg~%.1f_pt_%.1f", PreFix.Data(), m_Name.Data(), m_MassZPtBins->GetLowEdge( ptBin ), m_MassZPtBins->GetHighEdge( ptBin ) );
      sprintf( title, "%s:::Z Mass:::%.1f<p_{T}[GeV]<%.1f;m_{#mu#mu} [GeV];Entries", m_Title.Data(), m_MassZPtBins->GetLowEdge( ptBin ), m_MassZPtBins->GetHighEdge( ptBin ) );
      TString name_with_dots = name;
      TH1F* temp = new TH1F( name_with_dots.ReplaceAll( ".", "_" ), title, m_MassZ->Nbins, m_MassZ->Min, m_MassZ->Max );
      t_MassZVec_Bkg.push_back( temp );
      ++TotalSize;
      for( int Bin = 1; Bin < m_MassZ->Nbins + 1; Bin++ ) {
        float val( 0. ), err( 0. );
        for( int IntBin = 0; IntBin < m_MassZ->Split; IntBin++ ) {
          int FinalBin = ( Bin - 1 ) * m_MassZ->Split + IntBin;
          int theBin = ( ptBin - 1 ) * m_MassZ->NTotalBins() + FinalBin;
          val += m_MassZContentBuffer_Background[ theBin ];
          err += m_MassZErrorBuffer_Background[ theBin ];
        }
        t_MassZVec_Bkg[ ptBin - 1 ]->SetBinContent( Bin, val ); 
        t_MassZVec_Bkg[ ptBin - 1 ]->SetBinError( Bin, TMath::Sqrt( err ) ); 
      }
    }
  }
  //:::
  std::vector< TH1F* > t_MassZVec_Data;
  if( m_MassZContentBuffer_Data != NULL ) {
    for( int ptBin = 1; ptBin < m_MassZPtBins->Nbins + 1; ptBin++ ) {
      char name[100], title[100];
      sprintf( name, "%s~%s~MassZ~Data~%.1f_pt_%.1f", PreFix.Data(), m_Name.Data(), m_MassZPtBins->GetLowEdge( ptBin ), m_MassZPtBins->GetHighEdge( ptBin ) );
      sprintf( title, "%s:::Z Mass:::%.1f<p_{T}[GeV]<%.1f;m_{#mu#mu} [GeV];Entries", m_Title.Data(), m_MassZPtBins->GetLowEdge( ptBin ), m_MassZPtBins->GetHighEdge( ptBin ) );
      TString name_with_dots = name;
      TH1F* temp = new TH1F( name_with_dots.ReplaceAll( ".", "_" ), title, m_MassZ->Nbins, m_MassZ->Min, m_MassZ->Max );
      t_MassZVec_Data.push_back( temp );
      ++TotalSize;
      for( int Bin = 1; Bin < m_MassZ->Nbins + 1; Bin++ ) {
        float val( 0. );
        for( int IntBin = 0; IntBin < m_MassZ->Split; IntBin++ ) {
          int FinalBin = ( Bin - 1 ) * m_MassZ->Split + IntBin;
          int theBin = ( ptBin - 1 ) * m_MassZ->NTotalBins() + FinalBin;
          val += m_MassZContentBuffer_Data[ theBin ]; 
        }
        t_MassZVec_Data[ ptBin - 1 ]->SetBinContent( Bin, val ); 
        t_MassZVec_Data[ ptBin - 1 ]->SetBinError( Bin, TMath::Sqrt( val ) ); 
      }
    }
  }
  //:::
  std::vector< TH1F* > t_RhoZVec_MC;
  if( m_RhoZContentBuffer_MonteCarlo != NULL ) {
    for( int ptBin = 1; ptBin < m_RhoZPtBins->Nbins + 1; ptBin++ ) {
      char name[100], title[100];
      sprintf( name, "%s~%s~RhoZ~MC~%.1f_pt_%.1f", PreFix.Data(), m_Name.Data(), m_RhoZPtBins->GetLowEdge( ptBin ), m_RhoZPtBins->GetHighEdge( ptBin ) );
      sprintf( title, "%s:::Z Rho:::%.1f<p_{T}[GeV]<%.1f;m_{#mu#mu} [GeV];Entries", m_Title.Data(), m_RhoZPtBins->GetLowEdge( ptBin ), m_RhoZPtBins->GetHighEdge( ptBin ) );
      TString name_with_dots = name;
      TH1F* temp = new TH1F( name_with_dots.ReplaceAll( ".", "_" ), title, m_RhoZ->Nbins, m_RhoZ->Min, m_RhoZ->Max );
      t_RhoZVec_MC.push_back( temp );
      ++TotalSize;
      for( int Bin = 1; Bin < m_RhoZ->Nbins + 1; Bin++ ) {
        float val( 0. ), err( 0. );
        for( int IntBin = 0; IntBin < m_RhoZ->Split; IntBin++ ) {
          int FinalBin = ( Bin - 1 ) * m_RhoZ->Split + IntBin;
          int theBin = ( ptBin - 1 ) * m_RhoZ->NTotalBins() + FinalBin;
          val += m_RhoZContentBuffer_MonteCarlo[ theBin ] * m_RhoZAddWeightBuffer_MonteCarlo[ theBin ]; 
          err += m_RhoZErrorBuffer_MonteCarlo[ theBin ] * TMath::Power( m_RhoZAddWeightBuffer_MonteCarlo[ theBin ], 2 ); 
        }
        t_RhoZVec_MC[ ptBin - 1 ]->SetBinContent( Bin, val ); 
        t_RhoZVec_MC[ ptBin - 1 ]->SetBinError( Bin, TMath::Sqrt( err ) ); 
      }
    }
  }
  //:::
  std::vector< TH1F* > t_RhoZVec_Bkg;
  if( m_RhoZContentBuffer_Background != NULL ) {
    for( int ptBin = 1; ptBin < m_RhoZPtBins->Nbins + 1; ptBin++ ) {
      char name[100], title[100];
      sprintf( name, "%s~%s~RhoZ~Bkg~%.1f_pt_%.1f", PreFix.Data(), m_Name.Data(), m_RhoZPtBins->GetLowEdge( ptBin ), m_RhoZPtBins->GetHighEdge( ptBin ) );
      sprintf( title, "%s:::Z Rho:::%.1f<p_{T}[GeV]<%.1f;m_{#mu#mu} [GeV];Entries", m_Title.Data(), m_RhoZPtBins->GetLowEdge( ptBin ), m_RhoZPtBins->GetHighEdge( ptBin ) );
      TString name_with_dots = name;
      TH1F* temp = new TH1F( name_with_dots.ReplaceAll( ".", "_" ), title, m_RhoZ->Nbins, m_RhoZ->Min, m_RhoZ->Max );
      t_RhoZVec_Bkg.push_back( temp );
      ++TotalSize;
      for( int Bin = 1; Bin < m_RhoZ->Nbins + 1; Bin++ ) {
        float val( 0. ), err( 0. );
        for( int IntBin = 0; IntBin < m_RhoZ->Split; IntBin++ ) {
          int FinalBin = ( Bin - 1 ) * m_RhoZ->Split + IntBin;
          int theBin = ( ptBin - 1 ) * m_RhoZ->NTotalBins() + FinalBin;
          val += m_RhoZContentBuffer_Background[ theBin ];
          err += m_RhoZErrorBuffer_Background[ theBin ];
        }
        t_RhoZVec_Bkg[ ptBin - 1 ]->SetBinContent( Bin, val ); 
        t_RhoZVec_Bkg[ ptBin - 1 ]->SetBinError( Bin, TMath::Sqrt( err ) ); 
      }
    }
  }
  //:::
  std::vector< TH1F* > t_RhoZVec_Data;
  if( m_RhoZContentBuffer_Data != NULL ) {
    for( int ptBin = 1; ptBin < m_RhoZPtBins->Nbins + 1; ptBin++ ) {
      char name[100], title[100];
      sprintf( name, "%s~%s~RhoZ~Data~%.1f_pt_%.1f", PreFix.Data(), m_Name.Data(), m_RhoZPtBins->GetLowEdge( ptBin ), m_RhoZPtBins->GetHighEdge( ptBin ) );
      sprintf( title, "%s:::Z Rho:::%.1f<p_{T}[GeV]<%.1f;m_{#mu#mu} [GeV];Entries", m_Title.Data(), m_RhoZPtBins->GetLowEdge( ptBin ), m_RhoZPtBins->GetHighEdge( ptBin ) );
      TString name_with_dots = name;
      TH1F* temp = new TH1F( name_with_dots.ReplaceAll( ".", "_" ), title, m_RhoZ->Nbins, m_RhoZ->Min, m_RhoZ->Max );
      t_RhoZVec_Data.push_back( temp );
      ++TotalSize;
      for( int Bin = 1; Bin < m_RhoZ->Nbins + 1; Bin++ ) {
        float val( 0. );
        for( int IntBin = 0; IntBin < m_RhoZ->Split; IntBin++ ) {
          int FinalBin = ( Bin - 1 ) * m_RhoZ->Split + IntBin;
          int theBin = ( ptBin - 1 ) * m_RhoZ->NTotalBins() + FinalBin;
          val += m_RhoZContentBuffer_Data[ theBin ]; 
        }
        t_RhoZVec_Data[ ptBin - 1 ]->SetBinContent( Bin, val ); 
        t_RhoZVec_Data[ ptBin - 1 ]->SetBinError( Bin, TMath::Sqrt( val ) ); 
      }
    }
  }
  std::vector< TH1F* > t_RhoWVec_MC;
  std::vector< TH1F* > t_RhoWVec_Bkg;
  std::vector< TH1F* > t_RhoWVec_Data;
  //:::
  m_VectorHistograms = new std::vector< TH1F* >;
  m_VectorHistograms->reserve( TotalSize );
  if( !t_MassJpsiVec_MC.empty() )    m_VectorHistograms->insert( m_VectorHistograms->end(), t_MassJpsiVec_MC.begin(), t_MassJpsiVec_MC.end() );
  if( !t_MassZVec_MC.empty() )       m_VectorHistograms->insert( m_VectorHistograms->end(), t_MassZVec_MC.begin(),    t_MassZVec_MC.end() );
  if( !t_RhoZVec_MC.empty() )        m_VectorHistograms->insert( m_VectorHistograms->end(), t_RhoZVec_MC.begin(),     t_RhoZVec_MC.end() );
  if( !t_MassJpsiVec_Bkg.empty() )   m_VectorHistograms->insert( m_VectorHistograms->end(), t_MassJpsiVec_Bkg.begin(), t_MassJpsiVec_Bkg.end() );
  if( !t_MassZVec_Bkg.empty() )      m_VectorHistograms->insert( m_VectorHistograms->end(), t_MassZVec_Bkg.begin(),    t_MassZVec_Bkg.end() );
  if( !t_RhoZVec_Bkg.empty() )       m_VectorHistograms->insert( m_VectorHistograms->end(), t_RhoZVec_Bkg.begin(),     t_RhoZVec_Bkg.end() );
  if( !t_MassJpsiVec_Data.empty() )  m_VectorHistograms->insert( m_VectorHistograms->end(), t_MassJpsiVec_Data.begin(), t_MassJpsiVec_Data.end() );
  if( !t_MassZVec_Data.empty() )     m_VectorHistograms->insert( m_VectorHistograms->end(), t_MassZVec_Data.begin(),    t_MassZVec_Data.end() );
  if( !t_RhoZVec_Data.empty() )      m_VectorHistograms->insert( m_VectorHistograms->end(), t_RhoZVec_Data.begin(),     t_RhoZVec_Data.end() );
  return m_VectorHistograms;
}

void Template::Clear() {
  ClearData();
  ClearMonteCarlo();
  ClearBackground();
}

void Template::ClearData() {
  if( m_MassJpsiContentBuffer_Data ) ClearVector( m_MassJpsiContentBuffer_Data, m_MassJpsiPtBins->Nbins * m_MassJpsi->NTotalBins() );
  if( m_MassZContentBuffer_Data ) ClearVector( m_MassZContentBuffer_Data, m_MassZPtBins->Nbins * m_MassZ->NTotalBins() );
  if( m_RhoZContentBuffer_Data ) ClearVector( m_RhoZContentBuffer_Data, m_RhoZPtBins->Nbins * m_RhoZ->NTotalBins() );
}

void Template::ClearMonteCarlo() {
  if( m_MassJpsiContentBuffer_MonteCarlo )   ClearVector( m_MassJpsiContentBuffer_MonteCarlo,   m_MassJpsiPtBins->Nbins * m_MassJpsi->NTotalBins() );
  if( m_MassJpsiErrorBuffer_MonteCarlo )     ClearVector( m_MassJpsiErrorBuffer_MonteCarlo,     m_MassJpsiPtBins->Nbins * m_MassJpsi->NTotalBins() );
  //if( m_MassJpsiAddWeightBuffer_MonteCarlo ) ClearVector( m_MassJpsiAddWeightBuffer_MonteCarlo, m_MassJpsiPtBins->Nbins * m_MassJpsi->NTotalBins() );
  if( m_MassZContentBuffer_MonteCarlo )      ClearVector( m_MassZContentBuffer_MonteCarlo,      m_MassZPtBins->Nbins * m_MassZ->NTotalBins() );
  if( m_MassZErrorBuffer_MonteCarlo )        ClearVector( m_MassZErrorBuffer_MonteCarlo,        m_MassZPtBins->Nbins * m_MassZ->NTotalBins() );
  //if( m_MassZAddWeightBuffer_MonteCarlo )    ClearVector( m_MassZAddWeightBuffer_MonteCarlo,    m_MassZPtBins->Nbins * m_MassZ->NTotalBins() );
  if( m_RhoZContentBuffer_MonteCarlo )       ClearVector( m_RhoZContentBuffer_MonteCarlo,       m_RhoZPtBins->Nbins * m_RhoZ->NTotalBins() );
  if( m_RhoZErrorBuffer_MonteCarlo )         ClearVector( m_RhoZErrorBuffer_MonteCarlo,         m_RhoZPtBins->Nbins * m_RhoZ->NTotalBins() );
  //if( m_RhoZAddWeightBuffer_MonteCarlo )     ClearVector( m_RhoZAddWeightBuffer_MonteCarlo,     m_RhoZPtBins->Nbins * m_RhoZ->NTotalBins() );
  if( m_Weights != NULL ) m_Weights->Reset();
}

void Template::ClearBackground() {
  if( m_MassJpsiContentBuffer_Background ) ClearVector( m_MassJpsiContentBuffer_Background, m_MassJpsiPtBins->Nbins * m_MassJpsi->NTotalBins() );
  if( m_MassJpsiErrorBuffer_Background ) ClearVector( m_MassJpsiErrorBuffer_Background, m_MassJpsiPtBins->Nbins * m_MassJpsi->NTotalBins() );
  if( m_MassZContentBuffer_Background ) ClearVector( m_MassZContentBuffer_Background, m_MassZPtBins->Nbins * m_MassZ->NTotalBins() );
  if( m_MassZErrorBuffer_Background ) ClearVector( m_MassZErrorBuffer_Background, m_MassZPtBins->Nbins * m_MassZ->NTotalBins() );
  if( m_RhoZContentBuffer_Background ) ClearVector( m_RhoZContentBuffer_Background, m_RhoZPtBins->Nbins * m_RhoZ->NTotalBins() );
  if( m_RhoZErrorBuffer_Background ) ClearVector( m_RhoZErrorBuffer_Background, m_RhoZPtBins->Nbins * m_RhoZ->NTotalBins() );
}

void Template::ClearVector( float* Vec, int n ) {
  for( int i = 0; i < n; i++ ) {
    Vec[ i ] = 0.;
  }
}

void Template::Delete( float* Vec ) {
  delete [] Vec;
}

void Template::SetAverageWeight() {
  m_AverageWeight = m_Weights->Integral() / ( float ) m_Weights->GetEntries();
  m_SetAverageWeight = kTRUE;
  //std::cout << "m_AverageWeight ---> " << m_AverageWeight << std::endl;
}

void Template::SetGlobalWeights() {
  if( m_MassJpsiPtBins != NULL ) {
    for( int ptBin = 1; ptBin < m_MassJpsiPtBins->Nbins + 1; ptBin++ ) {
      float DataTot( 0. ), MonteCarloTot( 0. ), BackgroundTot( 0. );
      for( int varBin = 1; varBin < m_MassJpsi->NTotalBins() + 1; varBin++ ) {
        int theBin = ( ptBin - 1 ) * m_MassJpsi->NTotalBins() + varBin - 1;
        DataTot += m_MassJpsiContentBuffer_Data[ theBin ]; 
        BackgroundTot += m_MassJpsiContentBuffer_Background[ theBin ]; 
        MonteCarloTot += m_MassJpsiContentBuffer_MonteCarlo[ theBin ]; 
      }
      std::cout << "m_GlobalWeights ( MassJpsi, ptbin #" << ptBin << " ) ---> " << ( DataTot - BackgroundTot ) / MonteCarloTot << std::endl;
      for( int varBin = 1; varBin < m_MassJpsi->NTotalBins() + 1; varBin++ ) {
        int theBin = ( ptBin - 1 ) * m_MassJpsi->NTotalBins() + varBin - 1;
        m_MassJpsiAddWeightBuffer_MonteCarlo[ theBin ] = ( DataTot - BackgroundTot ) / MonteCarloTot;
      }
    }
  }
  if( m_MassZPtBins != NULL ) {
    for( int ptBin = 1; ptBin < m_MassZPtBins->Nbins + 1; ptBin++ ) {
      float DataTot( 0. ), MonteCarloTot( 0. ), BackgroundTot( 0. );
      for( int varBin = 1; varBin < m_MassZ->NTotalBins() + 1; varBin++ ) {
        int theBin = ( ptBin - 1 ) * m_MassZ->NTotalBins() + varBin - 1;
        DataTot += m_MassZContentBuffer_Data[ theBin ]; 
        BackgroundTot += m_MassZContentBuffer_Background[ theBin ]; 
        MonteCarloTot += m_MassZContentBuffer_MonteCarlo[ theBin ]; 
      }
      std::cout << "m_GlobalWeights ( MassZ, ptbin #" << ptBin << " ) ---> " << ( DataTot - BackgroundTot ) / MonteCarloTot << std::endl;
      for( int varBin = 1; varBin < m_MassZ->NTotalBins() + 1; varBin++ ) {
        int theBin = ( ptBin - 1 ) * m_MassZ->NTotalBins() + varBin - 1;
        m_MassZAddWeightBuffer_MonteCarlo[ theBin ] = ( DataTot - BackgroundTot ) / MonteCarloTot;
      }
    }
  }
  if( m_RhoZPtBins != NULL ) {
    for( int ptBin = 1; ptBin < m_RhoZPtBins->Nbins + 1; ptBin++ ) {
      float DataTot( 0. ), MonteCarloTot( 0. ), BackgroundTot( 0. );
      for( int varBin = 1; varBin < m_RhoZ->NTotalBins() + 1; varBin++ ) {
        int theBin = ( ptBin - 1 ) * m_RhoZ->NTotalBins() + varBin - 1;
        DataTot += m_RhoZContentBuffer_Data[ theBin ]; 
        BackgroundTot += m_RhoZContentBuffer_Background[ theBin ]; 
        MonteCarloTot += m_RhoZContentBuffer_MonteCarlo[ theBin ]; 
      }
      std::cout << "m_GlobalWeights ( RhoZ, ptbin #" << ptBin << " ) ---> " << ( DataTot - BackgroundTot ) / MonteCarloTot << std::endl;
      for( int varBin = 1; varBin < m_RhoZ->NTotalBins() + 1; varBin++ ) {
        int theBin = ( ptBin - 1 ) * m_RhoZ->NTotalBins() + varBin - 1;
        m_RhoZAddWeightBuffer_MonteCarlo[ theBin ] = ( DataTot - BackgroundTot ) / MonteCarloTot;
      }
    }
  }
  m_SetGlobalWeights = kTRUE;
}

void Template::EvaluateBackground() {
  if( m_MassJpsiPtBins ) {
    //::: J/psi fit 
    for( int ptBin = 0; ptBin < m_MassJpsiPtBins->Nbins; ptBin++ ) {
      TString wsName = Form( "%s~BkgFit~MassJpsi~%.1f_pt_%.1f", m_Name.Data(), m_MassJpsiPtBins->GetLowEdge( ptBin + 1 ), m_MassJpsiPtBins->GetHighEdge( ptBin + 1 ) );
      TString wsTitle = Form( "%s:::BkgFit:::MassJpsi:::%.1f<p_{T}[GeV]<%.1f", m_Title.Data(), m_MassJpsiPtBins->GetLowEdge( ptBin + 1 ), m_MassJpsiPtBins->GetHighEdge( ptBin + 1 ) );
      RooWorkspace ws( wsName, wsTitle );
      RooRealVar x( "x", "m_{#mu#mu}", m_MassJpsi->Min, m_MassJpsi->Max, "GeV" );
      //::: Let's fit MC first
      TH1F tempHist_MC( "tempHist_MC", "", m_MassJpsi->NTotalBins(), m_MassJpsi->Min, m_MassJpsi->Max ); 
      for( int varBin = 0; varBin < m_MassJpsi->NTotalBins(); varBin++ ) {
        tempHist_MC.SetBinContent( varBin + 1, m_MassJpsiContentBuffer_MonteCarlo[ ptBin * m_MassJpsi->NTotalBins() + varBin ] );
        tempHist_MC.SetBinError( varBin + 1, TMath::Sqrt( m_MassJpsiErrorBuffer_MonteCarlo[ ptBin * m_MassJpsi->NTotalBins() + varBin ] ) );
      }
      tempHist_MC.Rebin( m_MassJpsi->Split );
      RooDataHist tempDataSet_MC( "tempDataSet_MC", "", RooArgList( x ), &tempHist_MC );
      //::: Writing fit as in the validation case
      RooRealVar mean( "mean", "#mu", 3.1, 3.05, 3.15 );
      RooRealVar sigma( "ssigma", "#sigma", 0.05, 0.0001, 0.5 );
      RooRealVar alpha( "a", "#alpha", 2, 0.5, 4 );
      RooRealVar eta( "e", "#eta", 10, 0, 100 );
      RooCBShape tempCB_MC( "tempCB_MC", "Crystal-Ball", x, mean, sigma, alpha, eta );

      RooRealVar sigma_gauss( "sigma_gauss", "#sigma_{gauss}", 1, 0.05, 1 );
      RooGaussian tempGauss_MC( "tempGauss_MC", "Gauss", x, mean, sigma_gauss );
      RooRealVar frac_1( "frac_1", "frac_1", 0.5, 0., 1. );

      RooAddPdf tempModel_MC( "tempModel_MC", "ModelMC", tempCB_MC, tempGauss_MC, frac_1 );
      tempModel_MC.fitTo( tempDataSet_MC );
      //ws.import( tempModel_MC );
      ws.import( tempDataSet_MC );
      frac_1.setConstant( kTRUE );
      sigma_gauss.setConstant( kTRUE );
      alpha.setConstant( kTRUE );
      eta.setConstant( kTRUE );

      //::: Move on to data!
      TH1F tempHist_Data( "tempHist_Data", "", m_MassJpsi->NTotalBins(), m_MassJpsi->Min, m_MassJpsi->Max ); 
      for( int varBin = 0; varBin < m_MassJpsi->NTotalBins(); varBin++ ) {
        tempHist_Data.SetBinContent( varBin + 1, m_MassJpsiContentBuffer_Data[ ptBin * m_MassJpsi->NTotalBins() + varBin ] );
      }
      tempHist_Data.Rebin( m_MassJpsi->Split );
      float Est_BkgNorm = 0.5 * ( tempHist_Data.GetBinContent( 1 ) + tempHist_Data.GetBinContent( m_MassJpsi->Nbins ) ) * m_MassJpsi->Nbins; 
      float Est_SigNorm = tempHist_Data.Integral() - Est_BkgNorm;
      RooDataHist tempDataSet_Data( "tempDataSet_Data", "", RooArgList( x ), &tempHist_Data );
      RooRealVar tempBkgNorm( "tempBkgNorm", "Exponential Coefficient", Est_BkgNorm, 0, 10000000 );
      RooRealVar tempSigNorm( "tempSigNorm", "Signal Coefficient", Est_SigNorm, 0, 10000000 );
      //::: In case we fit the background with an exponential p.d.f.
      RooRealVar t( "t", "#tau", -1, -10, 0, "" );
      RooExponential tempExpo( "tempExpo", "Exponential", x, t );
      //::: In case we fit the background with a linear p.d.f.
      RooRealVar x1( "x1", "x_{1}", 0, -10, 10, "" );
      RooRealVar x2( "x2", "x_{2}", 0, -10, 10, "" );
      RooChebychev tempCheby( "tempCheby", "Chebychev polynomial", x, RooArgList( x1, x2 ) );
      //:::
      if( m_Bkg == "Exponential" ) {
        RooAddPdf tempModel( "tempModel", "Complete Model", RooArgList( tempExpo, tempModel_MC ), RooArgList( tempBkgNorm, tempSigNorm ) );
        tempModel.fitTo( tempDataSet_Data );
        ws.import( tempDataSet_Data );
        ws.import( tempModel );
        ws.Write( wsName.ReplaceAll( ".", "_" ) );
      }
      else {
        RooAddPdf tempModel( "tempModel", "Complete Model", RooArgList( tempCheby, tempModel_MC ), RooArgList( tempBkgNorm, tempSigNorm ) );
        tempModel.fitTo( tempDataSet_Data );
        ws.import( tempDataSet_Data );
        ws.import( tempModel );
        ws.Write( wsName.ReplaceAll( ".", "_" ) );
      }
      //:::
      TH1F* nominalHist = x.createHistogram( Form( "nominalHist_%d", ptBin ), "", m_MassJpsi->Min, m_MassJpsi->Max, m_MassJpsi->NTotalBins() ); 
      if( m_Bkg == "Exponential" ) tempExpo.fillHistogram( nominalHist, RooArgList( x ), tempBkgNorm.getVal() );
      else                         tempCheby.fillHistogram( nominalHist, RooArgList( x ), tempBkgNorm.getVal() );
      for( int Bin = 0; Bin < m_MassJpsi->NTotalBins(); Bin++ ) {
        m_MassJpsiContentBuffer_Background[ ptBin * m_MassJpsi->NTotalBins() + Bin ] = nominalHist->GetBinContent( Bin + 1 );
        m_MassJpsiErrorBuffer_Background[ ptBin * m_MassJpsi->NTotalBins() + Bin ] = nominalHist->GetBinError( Bin + 1 );
      }
    }
  }
  m_EvaluatedBackground = kTRUE;
}

double Template::GetChiSquare( double nRandom ) {
  double res = 0.;
  if( ! m_EvaluatedBackground ) {
    EvaluateBackground();
  }
  if( ! m_SetAverageWeight ) SetAverageWeight();
  if( ! m_SetGlobalWeights ) SetGlobalWeights();
  for( int Offset = 0; Offset < m_Splitting; Offset++ ) {
    //::: Jpsi
    if( m_MassJpsiPtBins ) {
      for( int ptBin = 0; ptBin < m_MassJpsiPtBins->Nbins; ptBin++ ) {
        for( int varBin = 0; varBin < m_MassJpsi->Nbins; varBin++ ) {
          float mc_val( 0. ), mc_err_sq( 0. ), data_val( 0. );
          for( int IntBin = 0; IntBin < m_MassJpsi->Split; IntBin++ ) {
            int FinalBin = varBin * m_MassJpsi->Split + IntBin + Offset;
            if( FinalBin < m_MassJpsi->NTotalBins() ) {
              mc_val += m_MassJpsiContentBuffer_MonteCarlo[ ptBin * m_MassJpsi->NTotalBins() + FinalBin ] * m_MassJpsiAddWeightBuffer_MonteCarlo[ ptBin * m_MassJpsi->NTotalBins() + FinalBin ]; 
              mc_val += m_MassJpsiContentBuffer_Background[ ptBin * m_MassJpsi->NTotalBins() + FinalBin ];
              mc_err_sq += nRandom * m_MassJpsiErrorBuffer_MonteCarlo[ ptBin * m_MassJpsi->NTotalBins() + FinalBin ] * TMath::Power( m_MassJpsiAddWeightBuffer_MonteCarlo[ ptBin * m_MassJpsi->NTotalBins() + FinalBin ], 2 ); 
              mc_err_sq += m_MassJpsiErrorBuffer_Background[ ptBin * m_MassJpsi->NTotalBins() + FinalBin ];
              data_val += m_MassJpsiContentBuffer_Data[ ptBin * m_MassJpsi->NTotalBins() + FinalBin ];
            }
          }
          double diff = mc_val - data_val;
          if( data_val + mc_err_sq > 0 ) {
            res += diff * diff / ( data_val + mc_err_sq );
          }
        }
      }
    }
    //::: Z (mass)
    if( m_MassZPtBins ) {
      for( int ptBin = 0; ptBin < m_MassZPtBins->Nbins; ptBin++ ) {
        for( int varBin = 0; varBin < m_MassZ->Nbins; varBin++ ) {
          float mc_val( 0. ), mc_err_sq( 0. ), data_val( 0. );
          for( int IntBin = 0; IntBin < m_MassZ->Split; IntBin++ ) {
            int FinalBin = varBin * m_MassZ->Split + IntBin + Offset;
            if( FinalBin < m_MassZ->NTotalBins() ) {
              mc_val += m_MassZContentBuffer_MonteCarlo[ ptBin * m_MassZ->NTotalBins() + FinalBin ] * m_MassZAddWeightBuffer_MonteCarlo[ ptBin * m_MassZ->NTotalBins() + FinalBin ]; 
              mc_val += m_MassZContentBuffer_Background[ ptBin * m_MassZ->NTotalBins() + FinalBin ];
              mc_err_sq += nRandom * m_MassZErrorBuffer_MonteCarlo[ ptBin * m_MassZ->NTotalBins() + FinalBin ] * TMath::Power( m_MassZAddWeightBuffer_MonteCarlo[ ptBin * m_MassZ->NTotalBins() + FinalBin ], 2 ); 
              mc_err_sq += m_MassZErrorBuffer_Background[ ptBin * m_MassZ->NTotalBins() + FinalBin ];
              data_val += m_MassZContentBuffer_Data[ ptBin * m_MassZ->NTotalBins() + FinalBin ];
            }
          }
          double diff = mc_val - data_val;
          if( data_val + mc_err_sq > 0 ) {
            res += diff * diff / ( data_val + mc_err_sq );
          }
        }
      }
    }
    //::: Z (rho)
    if( m_RhoZPtBins ) {
      for( int ptBin = 0; ptBin < m_RhoZPtBins->Nbins; ptBin++ ) {
        for( int varBin = 0; varBin < m_RhoZ->Nbins; varBin++ ) {
          float mc_val( 0. ), mc_err_sq( 0. ), data_val( 0. );
          for( int IntBin = 0; IntBin < m_RhoZ->Split; IntBin++ ) {
            int FinalBin = varBin * m_RhoZ->Split + IntBin + Offset;
            if( FinalBin < m_RhoZ->NTotalBins() ) {
              mc_val += m_RhoZContentBuffer_MonteCarlo[ ptBin * m_RhoZ->NTotalBins() + FinalBin ] * m_RhoZAddWeightBuffer_MonteCarlo[ ptBin * m_RhoZ->NTotalBins() + FinalBin ]; 
              mc_val += m_RhoZContentBuffer_Background[ ptBin * m_RhoZ->NTotalBins() + FinalBin ];
              mc_err_sq += nRandom * m_RhoZErrorBuffer_MonteCarlo[ ptBin * m_RhoZ->NTotalBins() + FinalBin ] * TMath::Power( m_RhoZAddWeightBuffer_MonteCarlo[ ptBin * m_RhoZ->NTotalBins() + FinalBin ], 2 ); 
              mc_err_sq += m_RhoZErrorBuffer_Background[ ptBin * m_RhoZ->NTotalBins() + FinalBin ];
              data_val += m_RhoZContentBuffer_Data[ ptBin * m_RhoZ->NTotalBins() + FinalBin ];
            }
          }
          double diff = mc_val - data_val;
          if( data_val + mc_err_sq > 0 ) {
            res += diff * diff / ( data_val + mc_err_sq );
          }
        }
      }
    }
  }
  return res / ( double ) m_Splitting;
}

double Template::GetLogLikelihood( double nRandom ) {
  double res = 0.;
  if( ! m_EvaluatedBackground ) EvaluateBackground();
  if( ! m_SetAverageWeight ) SetAverageWeight();
  if( ! m_SetGlobalWeights ) SetGlobalWeights();
  for( int Offset = 0; Offset < m_MassJpsi->Split; Offset++ ) {
    //::: Jpsi
    if( m_MassJpsiPtBins ) {
      for( int ptBin = 0; ptBin < m_MassJpsiPtBins->Nbins; ptBin++ ) {
        for( int varBin = 0; varBin < m_MassJpsi->Nbins; varBin++ ) {
          float mc_val( 0. ), mc_val_sq( 0. ), mc_err_sq( 0. ), data_val( 0. );
          for( int IntBin = 0; IntBin < m_MassJpsi->Split; IntBin++ ) {
            int FinalBin = varBin * m_MassJpsi->Split + IntBin + Offset;
            if( FinalBin < m_MassJpsi->NTotalBins() ) {
              mc_val += m_MassJpsiContentBuffer_MonteCarlo[ ptBin * m_MassJpsi->NTotalBins() + FinalBin ] * m_MassJpsiAddWeightBuffer_MonteCarlo[ ptBin * m_MassJpsi->NTotalBins() + FinalBin ]; 
              mc_val += m_MassJpsiContentBuffer_Background[ ptBin * m_MassJpsi->NTotalBins() + FinalBin ];
              mc_err_sq += nRandom * m_MassJpsiErrorBuffer_MonteCarlo[ ptBin * m_MassJpsi->NTotalBins() + FinalBin ] * TMath::Power( m_MassJpsiAddWeightBuffer_MonteCarlo[ ptBin * m_MassJpsi->NTotalBins() + FinalBin ], 2 ); 
              mc_err_sq += m_MassJpsiErrorBuffer_Background[ ptBin * m_MassJpsi->NTotalBins() + FinalBin ];
              data_val += m_MassJpsiContentBuffer_Data[ ptBin * m_MassJpsi->NTotalBins() + FinalBin ];
            }
          }
          mc_val_sq = mc_val * mc_val;
          if( ( mc_err_sq * mc_err_sq + ( 4. * data_val - 2. * mc_val ) * mc_err_sq + mc_val_sq ) < 0 ) continue;
          //:::
          float x = 0.5 * ( mc_val - mc_err_sq + TMath::Sqrt( mc_err_sq * mc_err_sq + ( 4. * data_val - 2. * mc_val ) * mc_err_sq + mc_val_sq ) ); 
          //:::
          float log_fact = ( data_val > 0 ) ? data_val * TMath::Log( data_val ) - data_val + TMath::Log( data_val * ( 1. + 4 * data_val * ( 1 + 2 * data_val ) ) ) / 6. + TMath::Log( TMath::Pi() ) / 2. : 0.;
          //:::
          if( x > 0 ) res += ( data_val * TMath::Log( x ) - x - log_fact - TMath::Log( TMath::Sqrt( mc_err_sq ) ) - 0.5 * TMath::Power( ( x - mc_val ) / TMath::Sqrt( mc_err_sq ), 2 ) );
        }
      }
    }
    //::: Z (mass)
    if( m_MassZPtBins ) {
      for( int ptBin = 0; ptBin < m_MassZPtBins->Nbins; ptBin++ ) {
        for( int varBin = 0; varBin < m_MassZ->Nbins; varBin++ ) {
          float mc_val( 0. ), mc_val_sq( 0. ), mc_err_sq( 0. ), data_val( 0. );
          for( int IntBin = 0; IntBin < m_MassZ->Split; IntBin++ ) {
            int FinalBin = varBin * m_MassZ->Split + IntBin + Offset;
            if( FinalBin < m_MassZ->NTotalBins() ) {
              mc_val += m_MassZContentBuffer_MonteCarlo[ ptBin * m_MassZ->NTotalBins() + FinalBin ] * m_MassZAddWeightBuffer_MonteCarlo[ ptBin * m_MassZ->NTotalBins() + FinalBin ]; 
              mc_val += m_MassZContentBuffer_Background[ ptBin * m_MassZ->NTotalBins() + FinalBin ];
              mc_err_sq += nRandom * m_MassZErrorBuffer_MonteCarlo[ ptBin * m_MassZ->NTotalBins() + FinalBin ] * TMath::Power( m_MassZAddWeightBuffer_MonteCarlo[ ptBin * m_MassZ->NTotalBins() + FinalBin ], 2 ); 
              mc_err_sq += m_MassZErrorBuffer_Background[ ptBin * m_MassZ->NTotalBins() + FinalBin ];
              data_val += m_MassZContentBuffer_Data[ ptBin * m_MassZ->NTotalBins() + FinalBin ];
            }
          }
          mc_val_sq = mc_val * mc_val;
          if( ( mc_err_sq * mc_err_sq + ( 4. * data_val - 2. * mc_val ) * mc_err_sq + mc_val_sq ) < 0 ) continue;
          //:::
          float x = 0.5 * ( mc_val - mc_err_sq + TMath::Sqrt( mc_err_sq * mc_err_sq + ( 4. * data_val - 2. * mc_val ) * mc_err_sq + mc_val_sq ) ); 
          //:::
          float log_fact = ( data_val > 0 ) ? data_val * TMath::Log( data_val ) - data_val + TMath::Log( data_val * ( 1. + 4 * data_val * ( 1 + 2 * data_val ) ) ) / 6. + TMath::Log( TMath::Pi() ) / 2. : 0.;
          //:::
          if( x > 0 ) res += ( data_val * TMath::Log( x ) - x - log_fact - TMath::Log( TMath::Sqrt( mc_err_sq ) ) - 0.5 * TMath::Power( ( x - mc_val ) / TMath::Sqrt( mc_err_sq ), 2 ) );
        }
      }
    }
    //::: Z (rho)
    if( m_RhoZPtBins ) {
      for( int ptBin = 0; ptBin < m_RhoZPtBins->Nbins; ptBin++ ) {
        for( int varBin = 0; varBin < m_RhoZ->Nbins; varBin++ ) {
          float mc_val( 0. ), mc_val_sq( 0. ), mc_err_sq( 0. ), data_val( 0. );
          for( int IntBin = 0; IntBin < m_RhoZ->Split; IntBin++ ) {
            int FinalBin = varBin * m_RhoZ->Split + IntBin + Offset;
            if( FinalBin < m_RhoZ->NTotalBins() ) {
              mc_val += m_RhoZContentBuffer_MonteCarlo[ ptBin * m_RhoZ->NTotalBins() + FinalBin ] * m_RhoZAddWeightBuffer_MonteCarlo[ ptBin * m_RhoZ->NTotalBins() + FinalBin ]; 
              mc_val += m_RhoZContentBuffer_Background[ ptBin * m_RhoZ->NTotalBins() + FinalBin ];
              mc_err_sq += nRandom * m_RhoZErrorBuffer_MonteCarlo[ ptBin * m_RhoZ->NTotalBins() + FinalBin ] * TMath::Power( m_RhoZAddWeightBuffer_MonteCarlo[ ptBin * m_RhoZ->NTotalBins() + FinalBin ], 2 ); 
              mc_err_sq += m_RhoZErrorBuffer_Background[ ptBin * m_RhoZ->NTotalBins() + FinalBin ];
              data_val += m_RhoZContentBuffer_Data[ ptBin * m_RhoZ->NTotalBins() + FinalBin ];
            }
          }
          mc_val_sq = mc_val * mc_val;
          if( ( mc_err_sq * mc_err_sq + ( 4. * data_val - 2. * mc_val ) * mc_err_sq + mc_val_sq ) < 0 ) continue;
          //:::
          float x = 0.5 * ( mc_val - mc_err_sq + TMath::Sqrt( mc_err_sq * mc_err_sq + ( 4. * data_val - 2. * mc_val ) * mc_err_sq + mc_val_sq ) ); 
          //:::
          float log_fact = ( data_val > 0 ) ? data_val * TMath::Log( data_val ) - data_val + TMath::Log( data_val * ( 1. + 4 * data_val * ( 1 + 2 * data_val ) ) ) / 6. + TMath::Log( TMath::Pi() ) / 2. : 0.;
          //:::
          if( x > 0 ) res += ( data_val * TMath::Log( x ) - x - log_fact - TMath::Log( TMath::Sqrt( mc_err_sq ) ) - 0.5 * TMath::Power( ( x - mc_val ) / TMath::Sqrt( mc_err_sq ), 2 ) );
        }
      }
    }
  }
  return res / ( double ) m_MassJpsi->Split;
}

double Template::GetDegreesOfFreedom() {
  int count = 0;
  if( m_MassJpsi ) count += m_MassJpsi->Nbins * m_MassJpsiPtBins->Nbins; 
  if( m_MassZ )    count += m_MassZ   ->Nbins * m_MassZPtBins   ->Nbins; 
  if( m_RhoZ )     count += m_RhoZ    ->Nbins * m_RhoZPtBins    ->Nbins; 
  return count;
}
