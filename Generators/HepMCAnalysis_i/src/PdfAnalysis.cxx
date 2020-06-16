/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ----------------------------------------------------------------------

#include <iostream>
#include "TH1.h"
#include "TMath.h"
#include "MCUtils/HepMCEventUtils.h"
#include "../HepMCAnalysis_i/PdfAnalysis.h"
namespace MC {
using MCUtils::get_weight;
}

// ----------------------------------------------------------------------
PdfAnalysis::PdfAnalysis()
{
  InitPlots();
}

// ----------------------------------------------------------------------
PdfAnalysis::~PdfAnalysis()
{}

// ----------------------------------------------------------------------
void PdfAnalysis::InitPlots()
{

  m_Qscale =  new TH1D( "Qscale", "Q", 10000, 0, 10000 );

  m_id_1pos_x = new TH1D( "x_id 1pos", "x_id 1pos", 100, 0.0, 1.0 );
  m_id_2pos_x = new TH1D( "x_id 2pos", "x_id 2pos", 100, 0.0, 1.0 );
  m_id_3pos_x = new TH1D( "x_id 3pos", "x_id 3pos", 100, 0.0, 1.0 );
  m_id_4pos_x = new TH1D( "x_id 4pos", "x_id 4pos", 100, 0.0, 1.0 );
  m_id_5pos_x = new TH1D( "x_id 5pos", "x_id 5pos", 100, 0.0, 1.0 );

  m_id_1neg_x = new TH1D( "x_id 1neg", "x_id 1neg", 100, 0.0, 1.0 );
  m_id_2neg_x = new TH1D( "x_id 2neg", "x_id 2neg", 100, 0.0, 1.0 );
  m_id_3neg_x = new TH1D( "x_id 3neg", "x_id 3neg", 100, 0.0, 1.0 );
  m_id_4neg_x = new TH1D( "x_id 4neg", "x_id 4neg", 100, 0.0, 1.0 );
  m_id_5neg_x = new TH1D( "x_id 5neg", "x_id 5neg", 100, 0.0, 1.0 );

  m_id_21_x = new TH1D( "x_id 21", "x_id 21", 100, 0.0, 1.0 );

  m_Qscale -> Sumw2();

  m_id_1pos_x -> Sumw2();
  m_id_2pos_x -> Sumw2();
  m_id_3pos_x -> Sumw2();
  m_id_4pos_x -> Sumw2();
  m_id_5pos_x -> Sumw2();

  m_id_1neg_x -> Sumw2();
  m_id_2neg_x -> Sumw2();
  m_id_3neg_x -> Sumw2();
  m_id_4neg_x -> Sumw2();
  m_id_5neg_x -> Sumw2();

  m_id_21_x -> Sumw2();

}

// ----------------------------------------------------------------------
int PdfAnalysis::Init(double maxEta, double minPt)
{
  m_max_eta = maxEta;
  m_min_pt = minPt;

  m_outputFileName = "PdfAnalysis.root";
  m_outputRootDir = "PdfAnalysis";

  m_Qscale =  initHist( "Qscale", "Q","Q", 10000, 0, 10000 );

  m_id_1pos_x = initHist( "x_id 1pos", "x_id 1pos", "x", 100, 0.0, 1.0 );
  m_id_2pos_x = initHist( "x_id 2pos", "x_id 2pos", "x", 100, 0.0, 1.0 );
  m_id_3pos_x = initHist( "x_id 3pos", "x_id 3pos", "x", 100, 0.0, 1.0 );
  m_id_4pos_x = initHist( "x_id 4pos", "x_id 4pos", "x", 100, 0.0, 1.0 );
  m_id_5pos_x = initHist( "x_id 5pos", "x_id 5pos", "x", 100, 0.0, 1.0 );

  m_id_1neg_x = initHist( "x_id 1neg", "x_id 1neg", "x", 100, 0.0, 1.0 );
  m_id_2neg_x = initHist( "x_id 2neg", "x_id 2neg", "x", 100, 0.0, 1.0 );
  m_id_3neg_x = initHist( "x_id 3neg", "x_id 3neg", "x", 100, 0.0, 1.0 );
  m_id_4neg_x = initHist( "x_id 4neg", "x_id 4neg", "x", 100, 0.0, 1.0 );
  m_id_5neg_x = initHist( "x_id 5neg", "x_id 5neg", "x", 100, 0.0, 1.0 );

  m_id_21_x = initHist( "x_id 21", "x_id 21", "x", 100, 0.0, 1.0 );

  m_Qscale -> Sumw2();

  m_id_1pos_x -> Sumw2();
  m_id_2pos_x -> Sumw2();
  m_id_3pos_x -> Sumw2();
  m_id_4pos_x -> Sumw2();
  m_id_5pos_x -> Sumw2();

  m_id_1neg_x -> Sumw2();
  m_id_2neg_x -> Sumw2();
  m_id_3neg_x -> Sumw2();
  m_id_4neg_x -> Sumw2();
  m_id_5neg_x -> Sumw2();

  m_id_21_x -> Sumw2();

  return true;
}

// ----------------------------------------------------------------------
int PdfAnalysis::Process(HepMC::GenEvent *event)
{
  double weight;
  weight = MC::get_weight(event, 0 );

  auto pdf = event->pdf_info();

  if(pdf != 0){
    m_Qscale -> Fill(pdf -> scalePDF(), weight);

    if (pdf -> id1() == 21){
      m_id_21_x -> Fill(pdf -> x1(), weight);
    }
    if (pdf -> id2() == 21){
      m_id_21_x -> Fill(pdf -> x2(), weight);
    }

    if (pdf -> id1() == 1){
      m_id_1pos_x -> Fill(pdf -> x1(), weight);
    }
    if (pdf -> id2() == 1){
      m_id_1pos_x -> Fill(pdf -> x2(), weight);
    }

    if (pdf -> id1() == 2){
      m_id_2pos_x -> Fill(pdf -> x1(), weight);
    }
    if (pdf -> id2() == 2){
      m_id_2pos_x -> Fill(pdf -> x2(), weight);
    }

    if (pdf -> id1() == 3){
      m_id_3pos_x -> Fill(pdf -> x1(), weight);
    }
    if (pdf -> id2() == 3){
      m_id_3pos_x -> Fill(pdf -> x2(), weight);
    }

    if (pdf -> id1() == 4){
      m_id_4pos_x -> Fill(pdf -> x1(), weight);
    }
    if (pdf -> id2() == 4){
      m_id_4pos_x -> Fill(pdf -> x2(), weight);
    }

    if (pdf -> id1() == 5){
      m_id_5pos_x -> Fill(pdf -> x1(), weight);
    }
    if (pdf -> id2() == 5){
      m_id_5pos_x -> Fill(pdf -> x2(), weight);
    }



    if (pdf -> id1() == -1){
      m_id_1neg_x -> Fill(pdf -> x1(), weight);
    }
    if (pdf -> id2() == -1){
      m_id_1neg_x -> Fill(pdf -> x2(), weight);
    }

    if (pdf -> id1() == -2){
      m_id_2neg_x -> Fill(pdf -> x1(), weight);
    }
    if (pdf -> id2() == -2){
      m_id_2neg_x -> Fill(pdf -> x2(), weight);
    }

    if (pdf -> id1() == -3){
      m_id_3neg_x -> Fill(pdf -> x1(), weight);
    }
    if (pdf -> id2() == -3){
      m_id_3neg_x -> Fill(pdf -> x2(), weight);
    }

    if (pdf -> id1() == -4){
      m_id_4neg_x -> Fill(pdf -> x1(), weight);
    }
    if (pdf -> id2() == -4){
      m_id_4neg_x -> Fill(pdf -> x2(), weight);
    }

    if (pdf -> id1() == -5){
      m_id_5neg_x -> Fill(pdf -> x1(), weight);
    }
    if (pdf -> id2() == -5){
      m_id_5neg_x -> Fill(pdf -> x2(), weight);
    }

  }


  return true;
}
