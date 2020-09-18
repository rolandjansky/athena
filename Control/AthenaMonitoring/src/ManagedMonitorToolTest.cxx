/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// $Id: ManagedMonitorToolTest.cxx,v 1.9 2009-05-04 12:48:51 sschaetz Exp $
// **********************************************************************

#include <sstream>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolTest.h"

#include "TH1.h"
#include "TH2.h"
#include "TGraph.h"
#include "TTree.h"
#include "TMath.h"
#include "LWHists/TH1D_LW.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"

const long ManagedMonitorToolTest::s_nEnsembles         = 100;
const double ManagedMonitorToolTest::s_ensembleGenMean  =   1.0;
const double ManagedMonitorToolTest::s_ensembleGenSigma =   2.0;


// *********************************************************************
// Public Methods
// *********************************************************************

ManagedMonitorToolTest::
ManagedMonitorToolTest( const std::string & type, const std::string & name,
      const IInterface* parent )
   : ManagedMonitorToolBase( type, name, parent )
   , m_generatorIsInit(false)
   , m_generatorSeedProp(65539)
   , m_ensembleMean(0)
   , m_ensembleRMS(0)
   , m_ensembleMeanPull(0)
   , m_ensembleMeanPullDist(0)
   , m_managedHist0_lowStat(0)
   , m_managedHist1_lowStat(0)
   , m_managedHist2_lowStat(0)
   , m_managedHist_lumiBlock(0)
   , m_managedHist_eventsBlock(0)
   , m_managedHist_run(0)
   , m_Lumi(0)
   , m_graph(0)
   , m_managedGraph(0)
   , m_ntuple(0)
   , m_managedNtuple(0)
   , m_counter(0)
   , m_variable1(0)
   , m_variable2(0)
   , m_hlw_1D(0)
   , m_hlw_1F(0)
   , m_hlw_2F(0)
   , m_offset(0)
   , m_gen(65539)
   , m_efficiency(0)
   , m_managedEfficiency(0)
   , m_managedEfficiencyXLB(0)
   , m_managedGraphXLB(0)
{
   declareProperty( "GeneratorSeed", m_generatorSeedProp );
}


ManagedMonitorToolTest::
~ManagedMonitorToolTest()
{
}

// Description: Used for rebooking unmanaged histograms       
StatusCode
ManagedMonitorToolTest::
bookHistogramsRecurrent( )
{

   if( m_environment == AthenaMonManager::online ) {
      // book histograms that are only made in the online environment...
   }

   if( m_dataType == AthenaMonManager::cosmics ) {
      // book histograms that are only relevant for cosmics data...
   }

   if( !m_generatorIsInit ) {
      m_gen.SetSeed( m_generatorSeedProp );
      m_generatorIsInit = true;
   }

   
   MonGroup gaussian_ensemble( this, "Stats/Gaussian", lumiBlock, ATTRIB_UNMANAGED );
   MonGroup gaussian_summary( this, "Stats/Gaussian", lumiBlock, ATTRIB_UNMANAGED );

   if( newLowStatIntervalFlag() ) { }

   if( newLumiBlockFlag() ) {
         m_ensembles.clear();

         for( int i = 0; i < s_nEnsembles; ++i ) {
            std::ostringstream hName;
            std::ostringstream hTitle;

            hName << "gaus_" << i;
            hTitle << "A Gaussian Distribution (" << i << ", "
               << s_ensembleGenMean << ", " << s_ensembleGenSigma << ")";
            TH1* h = new TH1F( hName.str().c_str(), hTitle.str().c_str(),
                  50, s_ensembleGenMean-5.0*s_ensembleGenSigma,
                  s_ensembleGenMean+5.0*s_ensembleGenSigma );
            gaussian_ensemble.regHist( h ).ignore();
            m_ensembles.push_back( h );
         }


         m_ensembleMean = new TH1F( "nthMean", "Means of the Ensembles",
               s_nEnsembles, 0.0, s_nEnsembles );
         gaussian_summary.regHist( m_ensembleMean ).ignore();


         m_ensembleRMS = new TH1F( "nthRMS", "RMSs of the Ensembles",
               s_nEnsembles, 0.0, s_nEnsembles );
         gaussian_summary.regHist( m_ensembleRMS ).ignore();


         m_ensembleMeanPull = new TH1F( "nthMeanPull", "Pulls of the Means of the Ensembles",
               s_nEnsembles, 0.0, s_nEnsembles );
         gaussian_summary.regHist( m_ensembleMeanPull ).ignore();


         m_ensembleMeanPullDist = new TH1F( "meanPull", "Pull Distribution of Means",
               50, -5.0, 5.0 );
         gaussian_summary.regHist( m_ensembleMeanPullDist ).ignore();

   }

   if( newRunFlag() ) {
      
         // Example for TGraph
         MonGroup space_point( this, "SpacePoint", run, ATTRIB_UNMANAGED );
         m_graph = new TGraph();
         m_graph->SetNameTitle("spacepointVsEvent","Space-point vs Event");
         space_point.regGraph( m_graph ).ignore();
 
         // Example for ntuple
         MonGroup group_ntuple( this, "Ntuple", run, ATTRIB_UNMANAGED );
         m_ntuple = new TTree("ntupleName","ntupleTitle");
         m_ntuple->Branch("var1",&m_variable1,"var1/I"); 
         m_ntuple->Branch("var2",&m_variable2,"var2/I");
         group_ntuple.regTree( m_ntuple ).ignore();

         // Example for TEfficiency
         MonGroup efficiency( this, "Efficiency", run, ATTRIB_UNMANAGED );
         m_efficiency = new TEfficiency("Efficiency","Efficiency (Unmanaged)",10,0,10);
         efficiency.regEfficiency( m_efficiency ).ignore();

   }   

   return StatusCode::SUCCESS;
}

// Description: Used for re-booking managed histograms       
StatusCode
ManagedMonitorToolTest::
bookHistograms( )
{

   if( m_environment == AthenaMonManager::online ) {
      // book histograms that are only made in the online environment...
   }

   if( m_dataType == AthenaMonManager::cosmics ) {
      // book histograms that are only relevant for cosmics data...
   }

   if( !m_generatorIsInit ) {
      m_gen.SetSeed( m_generatorSeedProp );
      m_generatorIsInit = true;
   }

   // Example for histograms
   // Create managed histograms ()
   m_managedHist0_lowStat  = new TH1F( "managedHist0_lowStat", "Managed Histogram, regHist Example 0; interval: lowStat", 5, 0.0, 5.0 );
   m_managedHist1_lowStat = new TH1F ( "managedHist1_lowStat", "Managed Histogram, regHist Example 1; interval: lowStat", 5, 0.0, 5.0 );
   m_managedHist2_lowStat = new TH1F ( "managedHist2_lowStat", "Managed Histogram, regHist Example 2; interval: lowStat", 5, 0.0, 5.0 );

   m_managedHist_lumiBlock  = new TH1F( "managedHist_lumiBlock", "Managed Histogram; interval: lumiBlock", 5, 0.0, 5.0 );
   m_managedHist_eventsBlock  = new TH1F( "managedHist_eventsBlock", "Managed Histogram; interval: eventBlock", 5, 0.0, 5.0 );
   m_managedHist_run  = new TH1F( "managedHist0_run", "Managed Histogram; interval: run", 5, 0.0, 5.0 );

   // Create luminosity histogram 
   m_Lumi = new TH1F( "Avg_mu", "Average number of interactions", 100, 0, 100 );

   // Create MonGroup object for managing managed histograns from same group
   MonGroup managed_booking_lowStat( this, "Managed/Histograms", lowStat );   // to be re-booked every lowStat interval

   // Case 1. Use the tool regHist method and pass MonGroup instance.
   regHist( m_managedHist0_lowStat, managed_booking_lowStat ).ignore();

   // Case 2. Use the tool regHist method and pass parameters manually.
   regHist( m_managedHist1_lowStat, "Managed/Histograms", lowStat ).ignore();

   // Case 3. Use regHist method of the MonGroup instance.
   managed_booking_lowStat.regHist(m_managedHist2_lowStat).ignore();


   // Some more histogram examples
   MonGroup managed_booking_lumiBlock( this, "Managed/Histograms", lumiBlock);   // to re-booked every luminosity block
   regHist( m_managedHist_lumiBlock, managed_booking_lumiBlock ).ignore();

   MonGroup managed_eventsBlock(this, "Managed/Histograms", eventsBlock);    // to re-booked every eventsBlock
   regHist( m_managedHist_eventsBlock, managed_eventsBlock ).ignore();

   // Register luminosity histogram
   regHist( m_Lumi, managed_eventsBlock).ignore();

   MonGroup managed_booking_run( this, "Managed/Histograms", run);   // to be re-booked every run 
   regHist( m_managedHist_run, managed_booking_run ).ignore();

   // LW Histograms
   // Register managed LW histogram. Case 1: use the tool regHist method and pass parameters manually.
   m_hlw_1D = TH1D_LW::create("hLW_1D", "Simple 1D LW histograms", 100, 0, 100); 
   regHist( m_hlw_1D, "Managed/LW", lowStat ).ignore();   // to be re-booked ever lowStat interval

   // Register managed LW histogram (case 2). Use the tool regHist method and pass parameters manually.
   m_hlw_1F = TH1F_LW::create("hLW_1F", "Simple 1F LW histograms", 100, 0, 100); 
   regHist( m_hlw_1F, "Managed/Histograms", eventsBlock ).ignore();

   // Register managed LW histogram. Case 3: use regHist method of the MonGroup instance.
   MonGroup LWcontent_lumiBlock( this, "Managed/LW", lumiBlock );   // to be re-booked every luminosity block
   m_hlw_2F = TH2F_LW::create("hLW_2F","content comparison LW", 100, 0, 100, 50, 75, 125);
   LWcontent_lumiBlock.regHist( m_hlw_2F ).ignore();
   
   // Example for the managed TGraph
   MonGroup managed_space_point( this, "Managed/SpacePoint", lumiBlock );
   m_managedGraph = new TGraph();
   m_managedGraph->SetNameTitle("spacepointVsEvent_managed","Space-point vs Event (Managed)");
   managed_space_point.regGraph( m_managedGraph ).ignore();

   // Example for the managed ntuple
   MonGroup managed_group_ntuple( this, "Managed/managed_Ntuple", lumiBlock );
   m_managedNtuple = new TTree("managedNtupleName","managedNtupleTitle");
   m_managedNtuple->Branch("var1",&m_variable1,"var1/I"); 
   m_managedNtuple->Branch("var2",&m_variable2,"var2/I");
   managed_group_ntuple.regTree( m_managedNtuple ).ignore();

   // Example for the managed TEffficiency
   MonGroup managed_efficiency( this, "Managed/Efficiency", lumiBlock );
   m_managedEfficiency = new TEfficiency("ManagedEfficiency","Efficiency (Managed)",10,0,10);
   managed_efficiency.regEfficiency( m_managedEfficiency ).ignore();

   // Example for the managed TEfficiency across lumiblocks
   MonGroup managed_efficiency_xlb( this, "Managed/EfficiencyXLB", run, ATTRIB_X_VS_LB, "", "merge");
   m_managedEfficiencyXLB = new TEfficiency("ManagedEfficiencyXLB","EfficiencyXLB (Managed)",1200,0,1200);
   managed_efficiency_xlb.regEfficiency( m_managedEfficiencyXLB ).ignore();

   // Example for the managed TGraph across lumiblocks
   MonGroup managed_graph_xlb( this, "Managed/GraphXLB", run, ATTRIB_X_VS_LB, "", "merge");
   m_managedGraphXLB = new TGraph();
   m_managedGraphXLB->SetNameTitle("ManagedGraphXLB","GraphXLB (Managed)");
   managed_graph_xlb.regGraph( m_managedGraphXLB ).ignore();
   
   return StatusCode::SUCCESS;
}


StatusCode
ManagedMonitorToolTest::
fillHistograms()
{
      // Fill average mu per bunch crossing
      double lumiPerBCID = lbAverageInteractionsPerCrossing();
      if (lumiPerBCID < 0) {
          // no luminosity information or EnableLumi is set to False in the config
          ATH_MSG_INFO("No luminosity information available or EnableLumi = False");
      } else {
         m_Lumi->Fill(lumiPerBCID);
      }

   
      std::vector<TH1*>::const_iterator ensembleEnd = m_ensembles.end();
      for( std::vector<TH1*>::const_iterator i = m_ensembles.begin(); i != ensembleEnd; ++i ) {
         TH1* h = *i;
         h->Fill( m_gen.Gaus( s_ensembleGenMean, s_ensembleGenSigma ) );
      }

      // TGraph
      m_counter++;
      m_event.push_back(m_counter);
      m_spacepoint.push_back(m_counter%10);

      // TGraph (managed-rebooking)
      // Use GetN() method. Otherwise need to handle manually in procHistograms
      m_managedGraph->SetPoint(m_managedGraph->GetN(), m_counter, m_counter);

      // Ntuple
      m_variable1 = m_counter;
      m_variable2 = m_counter*2;
      m_ntuple->Fill();
      
      // Ntuple (managed-rebooking)
      m_managedNtuple->SetBranchAddress("var1",&m_variable1);
      m_managedNtuple->SetBranchAddress("var2",&m_variable2);
      m_managedNtuple->Fill(); 

      // Fill managed-rebooking histograms
      m_managedHist0_lowStat->Fill(0);
      m_managedHist1_lowStat->Fill(1);
      m_managedHist2_lowStat->Fill(2);
       
      // newEventsBlock is a static variable.
      // It is true when eventsBlock changed.
      if (newEventsBlockFlag())        
          m_offset += 1;

      // Fill out different histograms bins 
      m_managedHist_eventsBlock->Fill(m_offset);

      m_managedHist_lumiBlock->Fill(0);
      m_managedHist_run->Fill(0);
      
      // Fill managed LW histogram
      float x = m_gen.Gaus( 50., 15. );
      float y = m_gen.Gaus( 100., 10. );
      m_hlw_1D->Fill( x );      
      m_hlw_1F->Fill( m_offset );      
      m_hlw_2F->Fill( x, y );    

      double a = m_gen.Uniform(10);
      bool b = m_gen.Rndm() < TMath::Gaus(a,5,4);
      m_efficiency->Fill(b,a);
      m_managedEfficiency->Fill(b,a);

      double lb = AthenaMonManager::lumiBlockNumber();
      m_managedEfficiencyXLB->Fill(b,lb);

      m_managedGraphXLB->SetPoint(m_managedGraphXLB->GetN(), lb, lb);

   return StatusCode::SUCCESS;
}


StatusCode
ManagedMonitorToolTest::
procHistograms( )
{
   
  if( endOfLowStatFlag() || endOfLumiBlockFlag() ) {
         for( int i = 0; i < s_nEnsembles; ++i ) {
            TH1* h = m_ensembles[i];

            double mean = h->GetMean();
            double mean_err = h->GetMeanError();
            double rms = h->GetRMS();
            double rms_err = h->GetRMSError();

            m_ensembleMean->SetBinContent( i+1, mean );
            m_ensembleMean->SetBinError( i+1, mean_err );

            m_ensembleRMS->SetBinContent( i+1, rms );
            m_ensembleRMS->SetBinError( i+1, rms_err );

            double meanPull = mean_err > 0 ? (mean-s_ensembleGenMean)/mean_err : 0;
            m_ensembleMeanPull->SetBinContent( i+1, meanPull );
            m_ensembleMeanPull->SetBinError( i+1, 1.0 );

            m_ensembleMeanPullDist->Fill( meanPull );
         }
   }


   if( endOfRunFlag() ) {
         // fill vectors to TGraph
         for(unsigned int i=0; i<m_event.size(); i++) 
            m_graph->SetPoint(i, m_event[i], m_spacepoint[i]);
   }

   return StatusCode::SUCCESS;
}

