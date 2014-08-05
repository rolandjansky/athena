/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//  ***************************************************************************
//  *                                                                         *
//  *   Author: John Morris (john.morris@cern.ch)                             *
//  *           Queen Mary University of London                               *
//  *                                                                         *
//  *   See EisoTool.h for Documentation                                      *
//  *                                                                         *
//  ***************************************************************************/

#include "egammaAnalysisUtils/EisoTool.h"

EisoTool::EisoTool() :
  m_GeV(1000.0),
  m_isMC(false),
  m_rel16False_rel17True(true),
  m_sumEtFalse_isEMTrue(true)
{
  this->Init();
}

EisoTool::EisoTool(const bool& isMC) :
  m_GeV(1000.0),
  m_isMC(isMC),
  m_rel16False_rel17True(true),
  m_sumEtFalse_isEMTrue(true)
{
  this->Init();
}

EisoTool::~EisoTool()
{
  delete m_etBins;
  delete m_etaBins;
  
  delete m_etcone20_iso90;
  delete m_etcone20_iso95;
  delete m_etcone20_iso98;

  delete m_etcone30_iso90;
  delete m_etcone30_iso95;
  delete m_etcone30_iso98;
  
  delete m_ptcone20_iso90;  
  delete m_ptcone20_iso97;
  delete m_ptcone20_iso98;
  delete m_ptcone20_iso99;

  delete m_ptcone30_iso90;  
  delete m_ptcone30_iso97;
  delete m_ptcone30_iso98;
  delete m_ptcone30_iso99;

  delete m_ptcone40_iso90;  
  delete m_ptcone40_iso97;
  delete m_ptcone40_iso98;
  delete m_ptcone40_iso99;

  delete m_etcone20DivEt_iso90;
  delete m_etcone20DivEt_iso95;
  delete m_etcone20DivEt_iso98;

  delete m_etcone30DivEt_iso90;
  delete m_etcone30DivEt_iso95;
  delete m_etcone30DivEt_iso98;

  delete m_ptcone20DivEt_iso90;
  delete m_ptcone20DivEt_iso97;
  delete m_ptcone20DivEt_iso98;
  delete m_ptcone20DivEt_iso99;

  delete m_ptcone30DivEt_iso90;  
  delete m_ptcone30DivEt_iso97;
  delete m_ptcone30DivEt_iso98;
  delete m_ptcone30DivEt_iso99;

  delete m_ptcone40DivEt_iso90;  
  delete m_ptcone40DivEt_iso97;
  delete m_ptcone40DivEt_iso98;
  delete m_ptcone40DivEt_iso99;

  delete m_etcone20_iso90_rel17;
  delete m_etcone20_iso95_rel17;
  delete m_etcone20_iso98_rel17;

  delete m_etcone30_iso90_rel17;
  delete m_etcone30_iso95_rel17;
  delete m_etcone30_iso98_rel17;

  delete m_etcone20DivEt_iso90_rel17;
  delete m_etcone20DivEt_iso95_rel17;
  delete m_etcone20DivEt_iso98_rel17;

  delete m_etcone30DivEt_iso90_rel17;
  delete m_etcone30DivEt_iso95_rel17;
  delete m_etcone30DivEt_iso98_rel17;


  delete m_etcone20_iso90_rel17_sumEt;
  delete m_etcone20_iso95_rel17_sumEt;
  delete m_etcone20_iso98_rel17_sumEt;

  delete m_etcone30_iso90_rel17_sumEt;
  delete m_etcone30_iso95_rel17_sumEt;
  delete m_etcone30_iso98_rel17_sumEt;

  delete m_ptcone20_iso90_sumEt;
  delete m_ptcone20_iso97_sumEt;
  delete m_ptcone20_iso98_sumEt;
  delete m_ptcone20_iso99_sumEt;

  delete m_ptcone30_iso90_sumEt;
  delete m_ptcone30_iso97_sumEt;
  delete m_ptcone30_iso98_sumEt;
  delete m_ptcone30_iso99_sumEt;

  delete m_ptcone40_iso90_sumEt;
  delete m_ptcone40_iso97_sumEt;
  delete m_ptcone40_iso98_sumEt;
  delete m_ptcone40_iso99_sumEt;

  delete m_etcone20DivEt_iso90_rel17_sumEt;
  delete m_etcone20DivEt_iso95_rel17_sumEt;
  delete m_etcone20DivEt_iso98_rel17_sumEt;

  delete m_etcone30DivEt_iso90_rel17_sumEt;
  delete m_etcone30DivEt_iso95_rel17_sumEt;
  delete m_etcone30DivEt_iso98_rel17_sumEt;

  delete m_ptcone20DivEt_iso90_sumEt;
  delete m_ptcone20DivEt_iso97_sumEt;
  delete m_ptcone20DivEt_iso98_sumEt;
  delete m_ptcone20DivEt_iso99_sumEt;

  delete m_ptcone30DivEt_iso90_sumEt;
  delete m_ptcone30DivEt_iso97_sumEt;
  delete m_ptcone30DivEt_iso98_sumEt;
  delete m_ptcone30DivEt_iso99_sumEt;

  delete m_ptcone40DivEt_iso90_sumEt;
  delete m_ptcone40DivEt_iso97_sumEt;
  delete m_ptcone40DivEt_iso98_sumEt;
  delete m_ptcone40DivEt_iso99_sumEt;
}

void EisoTool::Init()
{
  this->Init_Bins();
  
  this->Init_Etcone20_iso90();
  this->Init_Etcone20_iso95();
  this->Init_Etcone20_iso98();

  this->Init_Etcone30_iso90();
  this->Init_Etcone30_iso95();
  this->Init_Etcone30_iso98();

  this->Init_Etcone20_iso90_rel17();
  this->Init_Etcone20_iso95_rel17();
  this->Init_Etcone20_iso98_rel17();

  this->Init_Etcone30_iso90_rel17();
  this->Init_Etcone30_iso95_rel17();
  this->Init_Etcone30_iso98_rel17();
  
  this->Init_Ptcone20_iso90();
  this->Init_Ptcone20_iso97();
  this->Init_Ptcone20_iso98();
  this->Init_Ptcone20_iso99();

  this->Init_Ptcone30_iso90();
  this->Init_Ptcone30_iso97();
  this->Init_Ptcone30_iso98();
  this->Init_Ptcone30_iso99();

  this->Init_Ptcone40_iso90();
  this->Init_Ptcone40_iso97();
  this->Init_Ptcone40_iso98();
  this->Init_Ptcone40_iso99();


  this->Init_Etcone20DivEt_iso90();
  this->Init_Etcone20DivEt_iso95();
  this->Init_Etcone20DivEt_iso98();

  this->Init_Etcone30DivEt_iso90();
  this->Init_Etcone30DivEt_iso95();
  this->Init_Etcone30DivEt_iso98();

  this->Init_Etcone20DivEt_iso90_rel17();
  this->Init_Etcone20DivEt_iso95_rel17();
  this->Init_Etcone20DivEt_iso98_rel17();

  this->Init_Etcone30DivEt_iso90_rel17();
  this->Init_Etcone30DivEt_iso95_rel17();
  this->Init_Etcone30DivEt_iso98_rel17();

  this->Init_Ptcone20DivEt_iso90();
  this->Init_Ptcone20DivEt_iso97();
  this->Init_Ptcone20DivEt_iso98();
  this->Init_Ptcone20DivEt_iso99();

  this->Init_Ptcone30DivEt_iso90();
  this->Init_Ptcone30DivEt_iso97();
  this->Init_Ptcone30DivEt_iso98();
  this->Init_Ptcone30DivEt_iso99();

  this->Init_Ptcone40DivEt_iso90();
  this->Init_Ptcone40DivEt_iso97();
  this->Init_Ptcone40DivEt_iso98();
  this->Init_Ptcone40DivEt_iso99();

  this->Init_Etcone20_iso90_rel17_sumEt();
  this->Init_Etcone20_iso95_rel17_sumEt();
  this->Init_Etcone20_iso98_rel17_sumEt();

  this->Init_Etcone30_iso90_rel17_sumEt();
  this->Init_Etcone30_iso95_rel17_sumEt();
  this->Init_Etcone30_iso98_rel17_sumEt();

  this->Init_Ptcone20_iso90_sumEt();
  this->Init_Ptcone20_iso97_sumEt();
  this->Init_Ptcone20_iso98_sumEt();
  this->Init_Ptcone20_iso99_sumEt();

  this->Init_Ptcone30_iso90_sumEt();
  this->Init_Ptcone30_iso97_sumEt();
  this->Init_Ptcone30_iso98_sumEt();
  this->Init_Ptcone30_iso99_sumEt();

  this->Init_Ptcone40_iso90_sumEt();
  this->Init_Ptcone40_iso97_sumEt();
  this->Init_Ptcone40_iso98_sumEt();
  this->Init_Ptcone40_iso99_sumEt();

  this->Init_Etcone20DivEt_iso90_rel17_sumEt();
  this->Init_Etcone20DivEt_iso95_rel17_sumEt();
  this->Init_Etcone20DivEt_iso98_rel17_sumEt();

  this->Init_Etcone30DivEt_iso90_rel17_sumEt();
  this->Init_Etcone30DivEt_iso95_rel17_sumEt();
  this->Init_Etcone30DivEt_iso98_rel17_sumEt();

  this->Init_Ptcone20DivEt_iso90_sumEt();
  this->Init_Ptcone20DivEt_iso97_sumEt();
  this->Init_Ptcone20DivEt_iso98_sumEt();
  this->Init_Ptcone20DivEt_iso99_sumEt();

  this->Init_Ptcone30DivEt_iso90_sumEt();
  this->Init_Ptcone30DivEt_iso97_sumEt();
  this->Init_Ptcone30DivEt_iso98_sumEt();
  this->Init_Ptcone30DivEt_iso99_sumEt();

  this->Init_Ptcone40DivEt_iso90_sumEt();
  this->Init_Ptcone40DivEt_iso97_sumEt();
  this->Init_Ptcone40DivEt_iso98_sumEt();
  this->Init_Ptcone40DivEt_iso99_sumEt();
}

void EisoTool::Init_Bins()
{
  // Binning is based on isEM binning
  
  m_etBins = new std::vector<double>;
  m_etBins->push_back(10.0 * m_GeV);
  m_etBins->push_back(15.0 * m_GeV);
  m_etBins->push_back(20.0 * m_GeV);
  m_etBins->push_back(30.0 * m_GeV);
  m_etBins->push_back(40.0 * m_GeV);
  m_etBins->push_back(50.0 * m_GeV);
  m_etBins->push_back(60.0 * m_GeV);
  m_etBins->push_back(70.0 * m_GeV);
  m_etBins->push_back(80.0 * m_GeV);
  m_etBins->push_back(120.0 * m_GeV);

  m_etaBins = new std::vector<double>;
  m_etaBins->push_back(0.0);
  m_etaBins->push_back(0.6);
  m_etaBins->push_back(0.8);
  m_etaBins->push_back(1.15);
  m_etaBins->push_back(1.37);
  m_etaBins->push_back(1.52);
  m_etaBins->push_back(1.81);
  m_etaBins->push_back(2.01);
  m_etaBins->push_back(2.37);
  m_etaBins->push_back(2.47);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////   Etcone 20 Isolation Initilization ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////// 
void EisoTool::Init_Etcone20_iso90()
{

  // Note that some cut values are -100.0
  // This means that in that bin, the efficieny is not possible
  // This will result in the cut returning a false value
  
  m_etcone20_iso90 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 2.0  ,  2.0  ,  2.0  ,  2.2  ,  -100 ,  1.9  ,  2.0  ,  1.5  ,  1.55  }, // Et : 10 -> 15  GeV
    { 1.95 ,  1.9  ,  2.2  ,  2.4  ,  -100 ,  1.95 ,  1.75 ,  1.45 ,  1.4   }, // Et : 15 -> 20  GeV
    { 1.7  ,  1.9  ,  2.25 ,  2.55 ,  -100 ,  2.1  ,  1.75 ,  1.45 ,  1.5   }, // Et : 20 -> 30  GeV
    { 1.75 ,  1.8  ,  2.2  ,  2.65 ,  -100 ,  2.1  ,  1.8  ,  1.45 ,  1.55  }, // Et : 30 -> 40  GeV
    { 1.55 ,  1.6  ,  2.0  ,  2.45 ,  -100 ,  1.75 ,  1.6  ,  1.35 ,  1.6   }, // Et : 40 -> 50  GeV
    { 1.6  ,  1.65 ,  2.0  ,  2.55 ,  -100 ,  1.7  ,  1.65 ,  1.35 ,  1.6   }, // Et : 50 -> 60  GeV
    { 1.6  ,  1.8  ,  2.1  ,  2.95 ,  -100 ,  1.8  ,  1.75 ,  1.35 ,  2.0   }, // Et : 60 -> 70  GeV
    { 1.75 ,  1.7  ,  2.15 ,  2.8  ,  -100 ,  1.9  ,  1.5  ,  1.55 ,  2.35  }, // Et : 70 -> 80  GeV
    { 1.7  ,  1.65 ,  2.5  ,  3.15 ,  -100 ,  1.9  ,  1.75 ,  1.45 ,  2.25  }  // Et : 80 -> 120  GeV
  };
  
    for(unsigned int i=0;i<9;++i){
      std::vector<double> v;
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] );
      }
      m_etcone20_iso90->push_back(v);
    }     
}

void EisoTool::Init_Etcone20_iso95()
{
  m_etcone20_iso95 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 2.8  ,  2.8  ,  2.75 ,  3.0  ,  -100 ,  2.55 ,  2.6  ,  2.1  ,  2.25  }, // Et : 10 -> 15  GeV
    { 2.7  ,  2.65 ,  3.0  ,  3.2  ,  -100 ,  2.55 ,  2.35 ,  2.0  ,  2.2   }, // Et : 15 -> 20  GeV
    { 2.35 ,  2.7  ,  3.0  ,  3.25 ,  -100 ,  2.7  ,  2.35 ,  1.95 ,  1.95  }, // Et : 20 -> 30  GeV
    { 2.45 ,  2.5  ,  2.9  ,  3.4  ,  -100 ,  2.75 ,  2.4  ,  1.95 ,  1.9   }, // Et : 30 -> 40  GeV
    { 2.15 ,  2.25 ,  2.6  ,  3.15 ,  -100 ,  2.35 ,  2.15 ,  1.8  ,  2.0   }, // Et : 40 -> 50  GeV
    { 2.25 ,  2.3  ,  2.6  ,  3.3  ,  -100 ,  2.4  ,  2.2  ,  1.85 ,  1.95  }, // Et : 50 -> 60  GeV
    { 2.25 ,  2.8  ,  2.65 ,  3.8  ,  -100 ,  2.35 ,  2.25 ,  1.85 ,  2.3   }, // Et : 60 -> 70  GeV
    { 2.45 ,  2.55 ,  2.85 ,  4.05 ,  -100 ,  2.4  ,  2.0  ,  2.2  ,  2.85  }, // Et : 70 -> 80  GeV
    { 2.45 ,  2.4  ,  3.2  ,  4.25 ,  -100 ,  2.2  ,  2.3  ,  2.4  ,  2.9   }  // Et : 80 -> 120  GeV
  };


    for(unsigned int i=0;i<9;++i){
      std::vector<double> v;
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] );
      }
      m_etcone20_iso95->push_back(v);
    }     
}

void EisoTool::Init_Etcone20_iso98()
{
  m_etcone20_iso98 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 4.2  ,  3.65 ,  4.3  ,  3.9  ,  -100 ,  3.8  ,  3.7  ,  2.85 ,  2.85  }, // Et : 10 -> 15  GeV
    { 4.05 ,  3.85 ,  4.3  ,  4.4  ,  -100 ,  3.65 ,  3.25 ,  2.7  ,  3.25  }, // Et : 15 -> 20  GeV
    { 3.35 ,  3.7  ,  4.0  ,  4.2  ,  -100 ,  3.6  ,  3.05 ,  2.6  ,  2.6   }, // Et : 20 -> 30  GeV
    { 3.5  ,  3.55 ,  3.8  ,  4.4  ,  -100 ,  3.6  ,  3.2  ,  2.65 ,  2.55  }, // Et : 30 -> 40  GeV
    { 3.0  ,  3.15 ,  3.4  ,  4.05 ,  -100 ,  3.2  ,  2.85 ,  2.45 ,  2.55  }, // Et : 40 -> 50  GeV
    { 3.1  ,  3.15 ,  3.5  ,  4.15 ,  -100 ,  3.25 ,  2.95 ,  2.5  ,  2.5   }, // Et : 50 -> 60  GeV
    { 3.3  ,  3.65 ,  3.5  ,  4.85 ,  -100 ,  3.3  ,  2.95 ,  2.65 ,  2.8   }, // Et : 60 -> 70  GeV
    { 3.4  ,  3.7  ,  3.7  ,  5.35 ,  -100 ,  4.2  ,  3.05 ,  2.7  ,  3.35  }, // Et : 70 -> 80  GeV
    { 3.75 ,  4.0  ,  4.45 ,  5.65 ,  -100 ,  2.85 ,  3.3  ,  3.3  ,  6.0   }  // Et : 80 -> 120  GeV
  };

  for(unsigned int i=0;i<9;++i){
      std::vector<double> v; // Et Bins
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] ); // Fill Eta values in Et vector
      }
      m_etcone20_iso98->push_back(v);
    }  
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////   Etcone 30 Isolation Initilization ////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void EisoTool::Init_Etcone30_iso90()
{

  m_etcone30_iso90 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 3.45 ,  3.3  ,  3.35 ,  3.65 ,  -100 ,  3.45 ,  3.45 ,  2.8  ,  2.85  }, // Et : 10 -> 15  GeV
    { 3.55 ,  3.2  ,  3.6  ,  3.7  ,  -100 ,  3.35 ,  2.9  ,  2.5  ,  2.6   }, // Et : 15 -> 20  GeV
    { 2.85 ,  3.1  ,  3.3  ,  3.65 ,  -100 ,  3.25 ,  2.75 ,  2.4  ,  2.5   }, // Et : 20 -> 30  GeV
    { 2.8  ,  2.75 ,  3.15 ,  3.6  ,  -100 ,  3.25 ,  2.75 ,  2.45 ,  2.55  }, // Et : 30 -> 40  GeV
    { 2.55 ,  2.45 ,  2.85 ,  3.2  ,  -100 ,  2.75 ,  2.5  ,  2.25 ,  2.55  }, // Et : 40 -> 50  GeV
    { 2.6  ,  2.6  ,  2.85 ,  3.3  ,  -100 ,  2.75 ,  2.55 ,  2.3  ,  2.45  }, // Et : 50 -> 60  GeV
    { 2.7  ,  2.8  ,  3.05 ,  3.85 ,  -100 ,  2.7  ,  2.6  ,  2.2  ,  2.65  }, // Et : 60 -> 70  GeV
    { 2.8  ,  2.75 ,  3.0  ,  3.85 ,  -100 ,  2.7  ,  2.7  ,  2.5  ,  3.3   }, // Et : 70 -> 80  GeV
    { 2.65 ,  2.75 ,  3.4  ,  3.9  ,  -100 ,  2.8  ,  2.85 ,  2.45 ,  3.6   }  // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v;
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] );
      }
      m_etcone30_iso90->push_back(v);
    }
}

void EisoTool::Init_Etcone30_iso95()
{
  m_etcone30_iso95 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 5.0  ,  4.35 ,  4.7  ,  4.65 ,  -100 ,  5.15 ,  4.65 ,  3.85 ,  4.35  }, // Et : 10 -> 15  GeV
    { 5.05 ,  4.55 ,  5.0  ,  5.15 ,  -100 ,  4.4  ,  4.0  ,  3.55 ,  3.85  }, // Et : 15 -> 20  GeV
    { 3.95 ,  4.2  ,  4.5  ,  4.8  ,  -100 ,  4.2  ,  3.7  ,  3.25 ,  3.45  }, // Et : 20 -> 30  GeV
    { 3.85 ,  3.8  ,  4.1  ,  4.65 ,  -100 ,  4.2  ,  3.8  ,  3.35 ,  3.35  }, // Et : 30 -> 40  GeV
    { 3.5  ,  3.4  ,  3.8  ,  4.15 ,  -100 ,  3.65 ,  3.4  ,  3.1  ,  3.3   }, // Et : 40 -> 50  GeV
    { 3.6  ,  3.5  ,  3.7  ,  4.35 ,  -100 ,  3.7  ,  3.45 ,  3.25 ,  3.35  }, // Et : 50 -> 60  GeV
    { 3.7  ,  3.95 ,  3.85 ,  4.9  ,  -100 ,  3.95 ,  3.4  ,  3.25 ,  3.4   }, // Et : 60 -> 70  GeV
    { 3.65 ,  3.85 ,  4.05 ,  4.8  ,  -100 ,  3.65 ,  3.45 ,  3.8  ,  4.85  }, // Et : 70 -> 80  GeV
    { 3.9  ,  3.85 ,  4.65 ,  4.85 ,  -100 ,  3.75 ,  3.95 ,  3.1  ,  4.4   }  // Et : 80 -> 120  GeV
  };


    for(unsigned int i=0;i<9;++i){
      std::vector<double> v;
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] );
      }
      m_etcone30_iso95->push_back(v);
    }
}

void EisoTool::Init_Etcone30_iso98()
{
  m_etcone30_iso98 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 9.55 ,  7.1  ,  8.0  ,  6.9  ,  -100 ,  7.9  ,  7.8  ,  6.15 ,  7.55  }, // Et : 10 -> 15  GeV
    { 8.0  ,  6.65 ,  8.15 ,  7.45 ,  -100 ,  6.25 ,  6.05 ,  5.35 ,  5.65  }, // Et : 15 -> 20  GeV
    { 5.65 ,  6.05 ,  6.3  ,  6.4  ,  -100 ,  5.55 ,  5.25 ,  4.5  ,  4.85  }, // Et : 20 -> 30  GeV
    { 5.4  ,  5.2  ,  5.4  ,  6.0  ,  -100 ,  5.4  ,  5.0  ,  4.6  ,  4.35  }, // Et : 30 -> 40  GeV
    { 4.75 ,  4.6  ,  4.95 ,  5.45 ,  -100 ,  4.9  ,  4.55 ,  4.2  ,  4.4   }, // Et : 40 -> 50  GeV
    { 4.95 ,  4.75 ,  5.0  ,  5.75 ,  -100 ,  5.05 ,  4.7  ,  4.35 ,  4.3   }, // Et : 50 -> 60  GeV
    { 5.2  ,  5.65 ,  5.0  ,  6.2  ,  -100 ,  5.35 ,  4.4  ,  4.4  ,  4.55  }, // Et : 60 -> 70  GeV
    { 5.2  ,  5.65 ,  5.95 ,  6.3  ,  -100 ,  5.55 ,  5.7  ,  4.85 ,  6.85  }, // Et : 70 -> 80  GeV
    { 6.35 ,  5.35 ,  6.1  ,  7.2  ,  -100 ,  4.4  ,  7.1  ,  5.0  ,  6.85  }  // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v; // Et Bins
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] ); // Fill Eta values in Et vector
      }
      m_etcone30_iso98->push_back(v);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////   Etcone 20 Isolation Initilization  -- rel17 CaloIsoCorrection/////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////// 
void EisoTool::Init_Etcone20_iso90_rel17()
{
  m_etcone20_iso90_rel17 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 1.95 ,  1.85 ,  1.75 ,  1.8  ,  -100 ,  1.4  ,  1.9  ,  1.45 ,  1.3  }, // Et : 10 -> 15  GeV
    { 1.85 ,  1.75 ,  1.95 ,  2.1  ,  -100 ,  1.5  ,  1.7  ,  1.45 ,  1.25 }, // Et : 15 -> 20  GeV
    { 1.65 ,  1.85 ,  2.05 ,  2.3  ,  -100 ,  1.7  ,  1.8  ,  1.45 ,  1.4  }, // Et : 20 -> 30  GeV
    { 1.7  ,  1.8  ,  2.05 ,  2.5  ,  -100 ,  1.8  ,  1.85 ,  1.5  ,  1.45 }, // Et : 30 -> 40  GeV
    { 1.55 ,  1.65 ,  1.95 ,  2.3  ,  -100 ,  1.55 ,  1.75 ,  1.5  ,  1.55 }, // Et : 40 -> 50  GeV
    { 1.6  ,  1.8  ,  2.0  ,  2.55 ,  -100 ,  1.65 ,  1.8  ,  1.55 ,  1.6  }, // Et : 50 -> 60  GeV
    { 1.6  ,  2.0  ,  2.2  ,  3.05 ,  -100 ,  1.85 ,  2.0  ,  1.55 ,  2.05 }, // Et : 60 -> 70  GeV
    { 1.8  ,  1.95 ,  2.25 ,  2.95 ,  -100 ,  2.05 ,  1.85 ,  1.85 ,  2.5  }, // Et : 70 -> 80  GeV
    { 1.85 ,  2.2  ,  2.7  ,  3.7  ,  -100 ,  2.3  ,  2.2  ,  2.0  ,  2.25 }  // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v; // Et Bins
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] ); // Fill Eta values in Et vector
      }
      m_etcone20_iso90_rel17->push_back(v);
    }
}

void EisoTool::Init_Etcone20_iso95_rel17()
{
  m_etcone20_iso95_rel17 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 2.7  ,  2.6  ,  2.5  ,  2.65 ,  -100 ,  2.1  ,  2.55 ,  2.05 ,  2.0  }, // Et : 10 -> 15  GeV
    { 2.65 ,  2.55 ,  2.8  ,  2.9  ,  -100 ,  2.1  ,  2.35 ,  2.0  ,  2.05 }, // Et : 15 -> 20  GeV
    { 2.3  ,  2.65 ,  2.8  ,  3.05 ,  -100 ,  2.35 ,  2.35 ,  1.95 ,  1.85 }, // Et : 20 -> 30  GeV
    { 2.4  ,  2.5  ,  2.75 ,  3.25 ,  -100 ,  2.5  ,  2.45 ,  2.0  ,  1.8  }, // Et : 30 -> 40  GeV
    { 2.15 ,  2.3  ,  2.55 ,  3.05 ,  -100 ,  2.2  ,  2.25 ,  1.95 ,  1.95 }, // Et : 40 -> 50  GeV
    { 2.25 ,  2.4  ,  2.6  ,  3.3  ,  -100 ,  2.3  ,  2.4  ,  2.0  ,  1.95 }, // Et : 50 -> 60  GeV
    { 2.3  ,  2.95 ,  2.75 ,  3.85 ,  -100 ,  2.4  ,  2.5  ,  2.05 ,  2.35 }, // Et : 60 -> 70  GeV
    { 2.5  ,  2.8  ,  2.9  ,  4.15 ,  -100 ,  2.55 ,  2.25 ,  2.35 ,  3.0  }, // Et : 70 -> 80  GeV
    { 2.6  ,  3.05 ,  3.5  ,  5.25 ,  -100 ,  2.8  ,  3.05 ,  3.1  ,  2.85 }  // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v; // Et Bins
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] ); // Fill Eta values in Et vector
      }
      m_etcone20_iso95_rel17->push_back(v);
    }
}

void EisoTool::Init_Etcone20_iso98_rel17()
{
  m_etcone20_iso98_rel17 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 4.1  ,  3.5  ,  4.05 ,  3.5  ,  -100 ,  3.35 ,  3.6  ,  2.75 ,  2.7  }, // Et : 10 -> 15  GeV
    { 4.0  ,  3.65 ,  4.05 ,  4.0  ,  -100 ,  3.2  ,  3.25 ,  2.65 ,  3.1  }, // Et : 15 -> 20  GeV
    { 3.3  ,  3.7  ,  3.85 ,  3.95 ,  -100 ,  3.25 ,  3.1  ,  2.65 ,  2.45 }, // Et : 20 -> 30  GeV
    { 3.45 ,  3.55 ,  3.7  ,  4.25 ,  -100 ,  3.35 ,  3.3  ,  2.75 ,  2.45 }, // Et : 30 -> 40  GeV
    { 3.0  ,  3.25 ,  3.35 ,  4.0  ,  -100 ,  3.0  ,  2.95 ,  2.6  ,  2.5  }, // Et : 40 -> 50  GeV
    { 3.1  ,  3.35 ,  3.45 ,  4.15 ,  -100 ,  3.2  ,  3.15 ,  2.7  ,  2.5  }, // Et : 50 -> 60  GeV
    { 3.35 ,  3.8  ,  3.55 ,  4.9  ,  -100 ,  3.35 ,  3.2  ,  2.85 ,  2.9  }, // Et : 60 -> 70  GeV
    { 3.45 ,  3.95 ,  3.75 ,  5.5  ,  -100 ,  4.4  ,  3.45 ,  3.05 ,  3.5  }, // Et : 70 -> 80  GeV
    { 3.9  ,  4.7  ,  4.75 ,  6.45 ,  -100 ,  4.1  ,  3.6  ,  3.9  ,  6.35 }  // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v; // Et Bins
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] ); // Fill Eta values in Et vector
      }
      m_etcone20_iso98_rel17->push_back(v);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////   Etcone 30 Isolation Initilization  -- rel17 CaloIsoCorrection/////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////// 
void EisoTool::Init_Etcone30_iso90_rel17()
{
  m_etcone30_iso90_rel17 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 3.25 ,  3.05 ,  2.95 ,  3.1  ,  -100 ,  2.65 ,  3.25 ,  2.8  ,  2.55 }, // Et : 10 -> 15  GeV
    { 3.35 ,  3.0  ,  3.3  ,  3.25 ,  -100 ,  2.6  ,  2.85 ,  2.45 ,  2.35 }, // Et : 15 -> 20  GeV
    { 2.75 ,  3.0  ,  3.05 ,  3.35 ,  -100 ,  2.6  ,  2.75 ,  2.45 ,  2.3  }, // Et : 20 -> 30  GeV
    { 2.75 ,  2.8  ,  3.0  ,  3.35 ,  -100 ,  2.7  ,  2.85 ,  2.5  ,  2.4  }, // Et : 30 -> 40  GeV
    { 2.5  ,  2.55 ,  2.75 ,  3.05 ,  -100 ,  2.3  ,  2.6  ,  2.4  ,  2.5  }, // Et : 40 -> 50  GeV
    { 2.6  ,  2.8  ,  2.8  ,  3.25 ,  -100 ,  2.45 ,  2.75 ,  2.45 ,  2.4  }, // Et : 50 -> 60  GeV
    { 2.7  ,  3.05 ,  3.15 ,  3.95 ,  -100 ,  2.55 ,  2.85 ,  2.4  ,  2.65 }, // Et : 60 -> 70  GeV
    { 2.85 ,  3.1  ,  3.15 ,  4.15 ,  -100 ,  2.65 ,  2.95 ,  2.85 ,  3.45 }, // Et : 70 -> 80  GeV
    { 2.9  ,  3.45 ,  3.75 ,  4.85 ,  -100 ,  3.15 ,  3.45 ,  3.0  ,  3.5  }  // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v; // Et Bins
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] ); // Fill Eta values in Et vector
      }
      m_etcone30_iso90_rel17->push_back(v);
    }
}

void EisoTool::Init_Etcone30_iso95_rel17()
{
  m_etcone30_iso95_rel17 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 4.85 ,  4.15 ,  4.35 ,  4.1  ,  -100 ,  4.4  ,  4.5  ,  3.8  ,  4.1  }, // Et : 10 -> 15  GeV
    { 4.9  ,  4.3  ,  4.7  ,  4.65 ,  -100 ,  3.7  ,  3.9  ,  3.5  ,  3.6  }, // Et : 15 -> 20  GeV
    { 3.8  ,  4.15 ,  4.25 ,  4.4  ,  -100 ,  3.55 ,  3.65 ,  3.3  ,  3.3  }, // Et : 20 -> 30  GeV
    { 3.8  ,  3.8  ,  3.95 ,  4.4  ,  -100 ,  3.65 ,  3.85 ,  3.4  ,  3.2  }, // Et : 30 -> 40  GeV
    { 3.45 ,  3.5  ,  3.7  ,  4.0  ,  -100 ,  3.25 ,  3.5  ,  3.25 ,  3.2  }, // Et : 40 -> 50  GeV
    { 3.55 ,  3.65 ,  3.7  ,  4.35 ,  -100 ,  3.4  ,  3.65 ,  3.4  ,  3.35 }, // Et : 50 -> 60  GeV
    { 3.75 ,  4.2  ,  4.0  ,  4.95 ,  -100 ,  3.75 ,  3.7  ,  3.45 ,  3.5  }, // Et : 60 -> 70  GeV
    { 3.75 ,  4.3  ,  4.1  ,  5.0  ,  -100 ,  3.6  ,  3.8  ,  3.95 ,  4.95 }, // Et : 70 -> 80  GeV
    { 4.3  ,  4.8  ,  5.05 ,  5.9  ,  -100 ,  4.2  ,  4.65 ,  4.0  ,  4.45 }  // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v; // Et Bins
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] ); // Fill Eta values in Et vector
      }
      m_etcone30_iso95_rel17->push_back(v);
    }
}

void EisoTool::Init_Etcone30_iso98_rel17()
{
  m_etcone30_iso98_rel17 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 9.45 ,  6.7  ,  7.45 ,  6.3  ,  -100 ,  7.2  ,  7.55 ,  6.1  ,  7.35 }, // Et : 10 -> 15  GeV
    { 7.9  ,  6.45 ,  7.85 ,  6.9  ,  -100 ,  5.45 ,  6.0  ,  5.3  ,  5.4  }, // Et : 15 -> 20  GeV
    { 5.5  ,  6.0  ,  6.0  ,  6.05 ,  -100 ,  4.9  ,  5.2  ,  4.55 ,  4.65 }, // Et : 20 -> 30  GeV
    { 5.3  ,  5.25 ,  5.2  ,  5.75 ,  -100 ,  4.85 ,  5.05 ,  4.65 ,  4.15 }, // Et : 30 -> 40  GeV
    { 4.65 ,  4.7  ,  4.85 ,  5.3  ,  -100 ,  4.45 ,  4.65 ,  4.35 ,  4.25 }, // Et : 40 -> 50  GeV
    { 4.95 ,  4.9  ,  5.0  ,  5.75 ,  -100 ,  4.75 ,  4.85 ,  4.5  ,  4.3  }, // Et : 50 -> 60  GeV
    { 5.1  ,  5.95 ,  5.05 ,  6.4  ,  -100 ,  4.75 ,  4.65 ,  4.3  ,  4.55 }, // Et : 60 -> 70  GeV
    { 5.35 ,  5.8  ,  5.9  ,  6.45 ,  -100 ,  5.4  ,  6.1  ,  4.85 ,  6.9  }, // Et : 70 -> 80  GeV
    { 7.5  ,  6.8  ,  6.45 ,  8.15 ,  -100 ,  6.45 ,  7.4  ,  6.9  ,  7.2  }  // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v; // Et Bins
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] ); // Fill Eta values in Et vector
      }
      m_etcone30_iso98_rel17->push_back(v);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////   Ptcone 20 Isolation Initilization ////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void EisoTool::Init_Ptcone20_iso90()
{
  m_ptcone20_iso90 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 1 ,  1 ,  1 ,  1 ,  -100 ,  1 ,  1 ,  1 ,  1  }, // Et : 10 -> 15  GeV
    { 1 ,  1 ,  1 ,  1 ,  -100 ,  1 ,  1 ,  1 ,  1  }, // Et : 15 -> 20  GeV
    { 1 ,  1 ,  1 ,  1 ,  -100 ,  1 ,  1 ,  1 ,  1  }, // Et : 20 -> 30  GeV
    { 1 ,  1 ,  1 ,  1 ,  -100 ,  1 ,  1 ,  1 ,  1  }, // Et : 30 -> 40  GeV
    { 1 ,  1 ,  1 ,  1 ,  -100 ,  1 ,  1 ,  1 ,  1  }, // Et : 40 -> 50  GeV
    { 1 ,  1 ,  1 ,  1 ,  -100 ,  1 ,  1 ,  1 ,  1  }, // Et : 50 -> 60  GeV
    { 1 ,  1 ,  1 ,  1 ,  -100 ,  1 ,  1 ,  1 ,  1  }, // Et : 60 -> 70  GeV
    { 1 ,  1 ,  1 ,  1 ,  -100 ,  1 ,  1 ,  1 ,  1  }, // Et : 70 -> 80  GeV
    { 1 ,  1 ,  1 ,  1 ,  -100 ,  1 ,  1 ,  1 ,  1  }  // Et : 80 -> 120  GeV
  };

  // Please note that this function is included for completeness
  // It has no discriminating power whatsoever

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v; // Et Bins
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] ); // Fill Eta values in Et vector
      }
      m_ptcone20_iso90->push_back(v);
    }
}

void EisoTool::Init_Ptcone20_iso97()
{
  m_ptcone20_iso97 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 1.8  ,  2.65 ,  2.3  ,  1.6  ,  -100 ,  2.15 ,  2.3  ,  1.4  ,  1.2   }, // Et : 10 -> 15  GeV
    { 1.75 ,  1.5  ,  1.8  ,  1.75 ,  -100 ,  1.5  ,  1.25 ,  1.45 ,  1.6   }, // Et : 15 -> 20  GeV
    { 1.25 ,  1.45 ,  1.4  ,  1.4  ,  -100 ,  1.45 ,  1.35 ,  1.2  ,  1.2   }, // Et : 20 -> 30  GeV
    { 1.2  ,  1.15 ,  1.2  ,  1.3  ,  -100 ,  1.25 ,  1.4  ,  1.25 ,  1.3   }, // Et : 30 -> 40  GeV
    { 1.1  ,  1.1  ,  1.1  ,  1.1  ,  -100 ,  1.2  ,  1.25 ,  1.2  ,  1.15  }, // Et : 40 -> 50  GeV
    { 1.05 ,  1.0  ,  1.1  ,  1.15 ,  -100 ,  1.25 ,  1.3  ,  1.25 ,  1.1   }, // Et : 50 -> 60  GeV
    { 1.1  ,  1.3  ,  1.05 ,  1.15 ,  -100 ,  1.65 ,  1.1  ,  1.1  ,  1.0   }, // Et : 60 -> 70  GeV
    { 1.1  ,  1.25 ,  1.1  ,  1.25 ,  -100 ,  1.15 ,  2.8  ,  1.5  ,  1.75  }, // Et : 70 -> 80  GeV
    { 1.15 ,  1.15 ,  1.15 ,  1.0  ,  -100 ,  2.0  ,  1.65 ,  1.2  ,  1.0   }  // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v; // Et Bins
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] ); // Fill Eta values in Et vector
      }
      m_ptcone20_iso97->push_back(v);
    }
}

void EisoTool::Init_Ptcone20_iso98()
{
  m_ptcone20_iso98 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 2.6  ,  3.7  ,  3.2  ,  1.95 ,  -100 ,  3.1  ,  2.95 ,  1.9  ,  1.65  }, // Et : 10 -> 15  GeV
    { 2.4  ,  2.05 ,  2.3  ,  2.35 ,  -100 ,  2.0  ,  1.65 ,  2.0  ,  2.05  }, // Et : 15 -> 20  GeV
    { 1.6  ,  1.85 ,  1.9  ,  1.9  ,  -100 ,  1.9  ,  1.75 ,  1.65 ,  1.75  }, // Et : 20 -> 30  GeV
    { 1.5  ,  1.55 ,  1.55 ,  1.65 ,  -100 ,  1.65 ,  1.85 ,  1.65 ,  1.85  }, // Et : 30 -> 40  GeV
    { 1.35 ,  1.4  ,  1.4  ,  1.4  ,  -100 ,  1.55 ,  1.8  ,  1.55 ,  1.6   }, // Et : 40 -> 50  GeV
    { 1.3  ,  1.3  ,  1.35 ,  1.5  ,  -100 ,  1.7  ,  1.7  ,  1.75 ,  1.5   }, // Et : 50 -> 60  GeV
    { 1.4  ,  2.1  ,  1.25 ,  1.5  ,  -100 ,  2.4  ,  1.3  ,  1.85 ,  1.2   }, // Et : 60 -> 70  GeV
    { 1.35 ,  1.35 ,  1.25 ,  1.35 ,  -100 ,  2.95 ,  4.4  ,  1.85 ,  1.75  }, // Et : 70 -> 80  GeV
    { 1.5  ,  1.45 ,  1.35 ,  1.3  ,  -100 ,  2.3  ,  3.05 ,  1.75 ,  1.0   }  // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v; // Et Bins
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] ); // Fill Eta values in Et vector
      }
      m_ptcone20_iso98->push_back(v);
    }
}

void EisoTool::Init_Ptcone20_iso99()
{
  m_ptcone20_iso99 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 4.15 ,  7.45 ,  6.3  ,  3.2  ,  -100 ,  5.9  ,  4.6  ,  3.4  ,  3.85  }, // Et : 10 -> 15  GeV
    { 4.5  ,  3.85 ,  4.0  ,  5.45 ,  -100 ,  3.2  ,  2.55 ,  3.15 ,  3.45  }, // Et : 15 -> 20  GeV
    { 2.45 ,  2.9  ,  2.7  ,  2.95 ,  -100 ,  3.0  ,  3.3  ,  2.7  ,  3.05  }, // Et : 20 -> 30  GeV
    { 2.2  ,  2.25 ,  2.25 ,  2.55 ,  -100 ,  2.9  ,  3.45 ,  2.65 ,  4.25  }, // Et : 30 -> 40  GeV
    { 1.9  ,  1.95 ,  1.95 ,  2.1  ,  -100 ,  2.85 ,  3.25 ,  2.65 ,  2.65  }, // Et : 40 -> 50  GeV
    { 1.85 ,  1.95 ,  1.95 ,  2.25 ,  -100 ,  3.45 ,  3.4  ,  2.8  ,  3.4   }, // Et : 50 -> 60  GeV
    { 2.2  ,  3.15 ,  1.7  ,  4.05 ,  -100 ,  3.9  ,  1.9  ,  2.6  ,  2.2   }, // Et : 60 -> 70  GeV
    { 2.0  ,  2.25 ,  2.8  ,  2.05 ,  -100 ,  5.9  ,  5.35 ,  2.25 ,  2.2   }, // Et : 70 -> 80  GeV
    { 2.25 ,  2.9  ,  1.9  ,  2.65 ,  -100 ,  5.9  ,  13.05,  4.55 ,  3.8   }  // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v; // Et Bins
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] ); // Fill Eta values in Et vector
      }
      m_ptcone20_iso99->push_back(v);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////   Ptcone 30 Isolation Initilization ////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void EisoTool::Init_Ptcone30_iso90()
{
  m_ptcone30_iso90 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 1.3  ,  1.35 ,  1.2  ,  1.1  ,  -100 ,  1.2  ,  1.15 ,  1.0  ,  1.0   }, // Et : 10 -> 15  GeV
    { 1.15 ,  1.15 ,  1.25 ,  1.2  ,  -100 ,  1.0  ,  1.0  ,  1.0  ,  1.0   }, // Et : 15 -> 20  GeV
    { 1.0  ,  1.05 ,  1.05 ,  1.0  ,  -100 ,  1.0  ,  1.0  ,  1.0  ,  1.0   }, // Et : 20 -> 30  GeV
    { 1.0  ,  1.0  ,  1.0  ,  1.0  ,  -100 ,  1.0  ,  1.0  ,  1.0  ,  1.0   }, // Et : 30 -> 40  GeV
    { 1.0  ,  1.0  ,  1.0  ,  1.0  ,  -100 ,  1.0  ,  1.0  ,  1.0  ,  1.0   }, // Et : 40 -> 50  GeV
    { 1.0  ,  1.0  ,  1.0  ,  1.0  ,  -100 ,  1.0  ,  1.0  ,  1.0  ,  1.0   }, // Et : 50 -> 60  GeV
    { 1.0  ,  1.0  ,  1.0  ,  1.0  ,  -100 ,  1.0  ,  1.0  ,  1.0  ,  1.0   }, // Et : 60 -> 70  GeV
    { 1.0  ,  1.0  ,  1.0  ,  1.0  ,  -100 ,  1.0  ,  1.0  ,  1.0  ,  1.0   }, // Et : 70 -> 80  GeV
    { 1.0  ,  1.0  ,  1.0  ,  1.0  ,  -100 ,  1.0  ,  1.05 ,  1.0  ,  1.0   }  // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v; // Et Bins
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] ); // Fill Eta values in Et vector
      }
      m_ptcone30_iso90->push_back(v);
    }
}

void EisoTool::Init_Ptcone30_iso97()
{
  m_ptcone30_iso97 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 4.5  ,  3.7  ,  4.6  ,  3.2  ,  -100 ,  4.75 ,  3.0  ,  2.75 ,  3.0   }, // Et : 10 -> 15  GeV
    { 3.6  ,  2.8  ,  3.75 ,  3.75 ,  -100 ,  2.5  ,  2.4  ,  2.25 ,  2.2   }, // Et : 15 -> 20  GeV
    { 2.05 ,  2.6  ,  2.5  ,  2.4  ,  -100 ,  2.15 ,  2.15 ,  1.95 ,  1.7   }, // Et : 20 -> 30  GeV
    { 1.85 ,  1.9  ,  1.85 ,  1.9  ,  -100 ,  1.85 ,  2.15 ,  1.8  ,  1.9   }, // Et : 30 -> 40  GeV
    { 1.6  ,  1.65 ,  1.6  ,  1.6  ,  -100 ,  1.7  ,  1.85 ,  1.65 ,  1.6   }, // Et : 40 -> 50  GeV
    { 1.6  ,  1.7  ,  1.7  ,  1.75 ,  -100 ,  1.85 ,  1.95 ,  1.75 ,  1.4   }, // Et : 50 -> 60  GeV
    { 1.75 ,  2.3  ,  1.65 ,  1.6  ,  -100 ,  2.25 ,  1.35 ,  1.75 ,  1.0   }, // Et : 60 -> 70  GeV
    { 1.75 ,  2.25 ,  1.9  ,  2.25 ,  -100 ,  2.0  ,  2.8  ,  1.8  ,  1.75  }, // Et : 70 -> 80  GeV
    { 2.25 ,  2.05 ,  1.6  ,  1.7  ,  -100 ,  2.75 ,  5.15 ,  1.65 ,  3.3   }  // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v; // Et Bins
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] ); // Fill Eta values in Et vector
      }
      m_ptcone30_iso97->push_back(v);
    }
}

void EisoTool::Init_Ptcone30_iso98()
{
  m_ptcone30_iso98 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 6.3  ,  7.45 ,  6.5  ,  4.65 ,  -100 ,  6.25 ,  5.35 ,  3.85 ,  4.45  }, // Et : 10 -> 15  GeV
    { 5.5  ,  4.0  ,  5.35 ,  4.8  ,  -100 ,  3.35 ,  3.6  ,  3.05 ,  3.05  }, // Et : 15 -> 20  GeV
    { 2.75 ,  3.5  ,  3.15 ,  3.35 ,  -100 ,  2.95 ,  3.0  ,  2.6  ,  2.3   }, // Et : 20 -> 30  GeV
    { 2.4  ,  2.45 ,  2.35 ,  2.4  ,  -100 ,  2.55 ,  2.85 ,  2.4  ,  2.45  }, // Et : 30 -> 40  GeV
    { 1.95 ,  2.05 ,  2.05 ,  2.0  ,  -100 ,  2.3  ,  2.45 ,  2.15 ,  1.95  }, // Et : 40 -> 50  GeV
    { 2.05 ,  2.1  ,  2.1  ,  2.2  ,  -100 ,  2.55 ,  2.7  ,  2.3  ,  1.8   }, // Et : 50 -> 60  GeV
    { 2.25 ,  3.1  ,  2.15 ,  2.5  ,  -100 ,  2.75 ,  1.9  ,  2.2  ,  1.2   }, // Et : 60 -> 70  GeV
    { 2.5  ,  2.8  ,  2.5  ,  2.7  ,  -100 ,  3.1  ,  4.4  ,  2.0  ,  1.75  }, // Et : 70 -> 80  GeV
    { 2.95 ,  2.45 ,  2.4  ,  2.35 ,  -100 ,  3.45 ,  8.25 ,  2.2  ,  3.3  }  // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v; // Et Bins
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] ); // Fill Eta values in Et vector
      }
      m_ptcone30_iso98->push_back(v);
    }
}

void EisoTool::Init_Ptcone30_iso99()
{
  m_ptcone30_iso99 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 9.75 ,  13.25,  11.25,  8.85 ,  -100 ,  10.55,  9.45 ,  5.35 ,  7.75  }, // Et : 10 -> 15  GeV
    { 9.7  ,  6.55 ,  9.75 ,  8.25 ,  -100 ,  5.9  ,  5.35 ,  4.8  ,  4.25  }, // Et : 15 -> 20  GeV
    { 4.45 ,  5.9  ,  5.25 ,  4.85 ,  -100 ,  5.0  ,  4.85 ,  4.0  ,  3.8   }, // Et : 20 -> 30  GeV
    { 3.55 ,  3.55 ,  3.4  ,  3.7  ,  -100 ,  4.0  ,  4.65 ,  3.6  ,  4.95  }, // Et : 30 -> 40  GeV
    { 2.75 ,  2.95 ,  2.9  ,  2.95 ,  -100 ,  3.55 ,  4.25 ,  3.45 ,  3.1   }, // Et : 40 -> 50  GeV
    { 2.85 ,  3.05 ,  3.0  ,  3.4  ,  -100 ,  4.55 ,  5.15 ,  3.9  ,  4.9   }, // Et : 50 -> 60  GeV
    { 3.5  ,  4.55 ,  2.95 ,  5.6  ,  -100 ,  4.55 ,  2.9  ,  2.95 ,  2.2   }, // Et : 60 -> 70  GeV
    { 3.3  ,  3.75 ,  3.9  ,  3.2  ,  -100 ,  10.35,  5.8  ,  3.95 ,  2.2   }, // Et : 70 -> 80  GeV
    { 4.55 ,  4.75 ,  3.05 ,  3.1  ,  -100 ,  8.15 ,  13.05,  6.4  ,  3.8   }  // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v; // Et Bins
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] ); // Fill Eta values in Et vector
      }
      m_ptcone30_iso99->push_back(v);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////   Ptcone 40 Isolation Initilization ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////// 

void EisoTool::Init_Ptcone40_iso90()
{
  m_ptcone40_iso90 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 2.2  ,  2.3  ,  2.0  ,  1.65 ,  -100 ,  1.85 ,  1.75 ,  1.45 ,  1.3   }, // Et : 10 -> 15  GeV
    { 1.85 ,  1.75 ,  1.95 ,  1.65 ,  -100 ,  1.5  ,  1.55 ,  1.35 ,  1.15  }, // Et : 15 -> 20  GeV
    { 1.35 ,  1.65 ,  1.55 ,  1.45 ,  -100 ,  1.35 ,  1.3  ,  1.15 ,  1.0   }, // Et : 20 -> 30  GeV
    { 1.25 ,  1.2  ,  1.2  ,  1.25 ,  -100 ,  1.2  ,  1.25 ,  1.15 ,  1.0   }, // Et : 30 -> 40  GeV
    { 1.1  ,  1.1  ,  1.1  ,  1.1  ,  -100 ,  1.1  ,  1.1  ,  1.05 ,  1.0   }, // Et : 40 -> 50  GeV
    { 1.1  ,  1.1  ,  1.1  ,  1.1  ,  -100 ,  1.1  ,  1.15 ,  1.05 ,  1.0   }, // Et : 50 -> 60  GeV
    { 1.15 ,  1.35 ,  1.15 ,  1.1  ,  -100 ,  1.2  ,  1.05 ,  1.0  ,  1.0   }, // Et : 60 -> 70  GeV
    { 1.2  ,  1.15 ,  1.1  ,  1.25 ,  -100 ,  1.1  ,  1.15 ,  1.0  ,  1.0   }, // Et : 70 -> 80  GeV
    { 1.25 ,  1.25 ,  1.1  ,  1.0  ,  -100 ,  1.1  ,  1.6  ,  1.15 ,  1.0   }  // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v; // Et Bins
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] ); // Fill Eta values in Et vector
      }
      m_ptcone40_iso90->push_back(v);
    }
}

void EisoTool::Init_Ptcone40_iso97()
{
  m_ptcone40_iso97 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 8.0  ,  7.45 ,  6.5  ,  5.55 ,  -100 ,  8.1  ,  6.95 ,  3.9  ,  4.9   }, // Et : 10 -> 15  GeV
    { 6.7  ,  5.2  ,  6.95 ,  5.5  ,  -100 ,  4.45 ,  4.5  ,  3.65 ,  3.15  }, // Et : 15 -> 20  GeV
    { 3.5  ,  4.85 ,  3.95 ,  4.0  ,  -100 ,  3.65 ,  3.2  ,  2.9  ,  2.3   }, // Et : 20 -> 30  GeV
    { 2.85 ,  2.7  ,  2.65 ,  2.75 ,  -100 ,  2.75 ,  2.9  ,  2.55 ,  2.3   }, // Et : 30 -> 40  GeV
    { 2.25 ,  2.3  ,  2.3  ,  2.25 ,  -100 ,  2.4  ,  2.45 ,  2.2  ,  1.95  }, // Et : 40 -> 50  GeV
    { 2.35 ,  2.2  ,  2.4  ,  2.4  ,  -100 ,  2.65 ,  2.8  ,  2.25 ,  1.6   }, // Et : 50 -> 60  GeV
    { 2.55 ,  3.35 ,  2.6  ,  2.5  ,  -100 ,  2.6  ,  2.25 ,  1.9  ,  1.85  }, // Et : 60 -> 70  GeV
    { 2.85 ,  3.1  ,  2.25 ,  2.55 ,  -100 ,  3.1  ,  4.4  ,  2.0  ,  1.75  }, // Et : 70 -> 80  GeV
    { 3.75 ,  4.0  ,  2.45 ,  2.65 ,  -100 ,  3.45 ,  8.95 ,  2.65 ,  3.3   }  // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v; // Et Bins
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] ); // Fill Eta values in Et vector
      }
      m_ptcone40_iso97->push_back(v);
    }  
}

void EisoTool::Init_Ptcone40_iso98()
{
  m_ptcone40_iso98 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 13.15,  10.9 ,  8.05 ,  8.4  ,  -100 ,  11.15,  10.2 ,  5.05 ,  6.65  }, // Et : 10 -> 15  GeV
    { 10.7 ,  8.5  ,  11.1 ,  9.2  ,  -100 ,  6.7  ,  6.0  ,  4.95 ,  4.05  }, // Et : 15 -> 20  GeV
    { 4.75 ,  6.85 ,  5.7  ,  5.4  ,  -100 ,  5.0  ,  4.3  ,  3.85 ,  3.35  }, // Et : 20 -> 30  GeV
    { 3.65 ,  3.45 ,  3.35 ,  3.6  ,  -100 ,  3.55 ,  3.95 ,  3.2  ,  3.0   }, // Et : 30 -> 40  GeV
    { 2.75 ,  2.85 ,  2.9  ,  2.85 ,  -100 ,  3.05 ,  3.2  ,  2.85 ,  2.5   }, // Et : 40 -> 50  GeV
    { 2.9  ,  2.85 ,  3.05 ,  3.3  ,  -100 ,  3.75 ,  3.95 ,  2.8  ,  2.45  }, // Et : 50 -> 60  GeV
    { 3.35 ,  4.55 ,  3.15 ,  3.6  ,  -100 ,  3.75 ,  3.25 ,  2.4  ,  2.2   }, // Et : 60 -> 70  GeV
    { 3.4  ,  4.9  ,  2.95 ,  3.15 ,  -100 ,  5.1  ,  5.8  ,  3.6  ,  1.75  }, // Et : 70 -> 80  GeV
    { 5.8  ,  6.4  ,  3.05 ,  3.3  ,  -100 ,  4.55 ,  9.8  ,  3.5  ,  3.3   }  // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v; // Et Bins
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] ); // Fill Eta values in Et vector
      }
      m_ptcone40_iso98->push_back(v);
    }    
}

void EisoTool::Init_Ptcone40_iso99()
{
  m_ptcone40_iso99 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { -100 ,  16.0 ,  17.25,  14.8 ,  -100 ,  17.55,  17.0 ,  7.55 ,  10.05 }, // Et : 10 -> 15  GeV
    { 19.25,  14.7 ,  20.6 ,  13.8 ,  -100 ,  10.65,  10.35,  8.1  ,  6.55  }, // Et : 15 -> 20  GeV
    { 8.3  ,  12.7 ,  9.05 ,  8.5  ,  -100 ,  7.95 ,  6.9  ,  5.6  ,  5.3   }, // Et : 20 -> 30  GeV
    { 5.45 ,  5.05 ,  4.85 ,  5.45 ,  -100 ,  5.5  ,  6.15 ,  4.9  ,  5.95  }, // Et : 30 -> 40  GeV
    { 3.8  ,  3.9  ,  3.95 ,  4.0  ,  -100 ,  4.6  ,  5.4  ,  4.2  ,  3.7   }, // Et : 40 -> 50  GeV
    { 4.05 ,  3.95 ,  4.25 ,  4.85 ,  -100 ,  6.05 ,  6.2  ,  4.4  ,  6.1   }, // Et : 50 -> 60  GeV
    { 5.05 ,  6.1  ,  5.0  ,  7.6  ,  -100 ,  5.4  ,  4.35 ,  5.25 ,  3.4   }, // Et : 60 -> 70  GeV
    { 4.75 ,  8.8  ,  4.2  ,  4.6  ,  -100 ,  11.05,  6.75 ,  4.2  ,  2.2   }, // Et : 70 -> 80  GeV
    { 11.05,  10.85,  4.4  ,  9.95 ,  -100 ,  8.15 ,  13.05,  6.4  ,  3.8   }  // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v; // Et Bins
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] ); // Fill Eta values in Et vector
      }
      m_ptcone40_iso99->push_back(v);
    }      
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////   Etcone 20 / Et Isolation Initilization ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void EisoTool::Init_Etcone20DivEt_iso90()
{
  m_etcone20DivEt_iso90 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 0.155 ,  0.1525 ,  0.16   ,  0.17   ,  -100 ,  0.145  ,  0.155  ,  0.1175 ,  0.1175  }, // Et : 10 -> 15  GeV
    { 0.11  ,  0.1075 ,  0.125  ,  0.1325 ,  -100 ,  0.1125 ,  0.1    ,  0.085  ,  0.085   }, // Et : 15 -> 20  GeV
    { 0.065 ,  0.0725 ,  0.0875 ,  0.0975 ,  -100 ,  0.08   ,  0.07   ,  0.055  ,  0.06    }, // Et : 20 -> 30  GeV
    { 0.05  ,  0.05   ,  0.0625 ,  0.075  ,  -100 ,  0.06   ,  0.0525 ,  0.0425 ,  0.045   }, // Et : 30 -> 40  GeV
    { 0.035 ,  0.0375 ,  0.045  ,  0.055  ,  -100 ,  0.04   ,  0.0375 ,  0.0325 ,  0.0375  }, // Et : 40 -> 50  GeV
    { 0.03  ,  0.03   ,  0.0375 ,  0.05   ,  -100 ,  0.0325 ,  0.0325 ,  0.025  ,  0.03    }, // Et : 50 -> 60  GeV
    { 0.025 ,  0.03   ,  0.035  ,  0.0475 ,  -100 ,  0.03   ,  0.0275 ,  0.0225 ,  0.0325  }, // Et : 60 -> 70  GeV
    { 0.025 ,  0.0225 ,  0.03   ,  0.04   ,  -100 ,  0.0275 ,  0.02   ,  0.0225 ,  0.0325  }, // Et : 70 -> 80  GeV
    { 0.02  ,  0.02   ,  0.0275 ,  0.035  ,  -100 ,  0.02   ,  0.0225 ,  0.0175 ,  0.025   }  // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v;
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] );
      }
      m_etcone20DivEt_iso90->push_back(v);
    }
}

void EisoTool::Init_Etcone20DivEt_iso95()
{
  m_etcone20DivEt_iso95 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 0.2225 ,  0.205  ,  0.2175 ,  0.23   ,  -100 ,  0.2    ,  0.2025 ,  0.16   ,  0.17    }, // Et : 10 -> 15  GeV
    { 0.155  ,  0.15   ,  0.17   ,  0.1825 ,  -100 ,  0.14   ,  0.135  ,  0.11   ,  0.1225  }, // Et : 15 -> 20  GeV
    { 0.09   ,  0.1025 ,  0.1175 ,  0.1275 ,  -100 ,  0.105  ,  0.0925 ,  0.0775 ,  0.0775  }, // Et : 20 -> 30  GeV
    { 0.07   ,  0.0725 ,  0.0825 ,  0.0975 ,  -100 ,  0.08   ,  0.07   ,  0.055  ,  0.055   }, // Et : 30 -> 40  GeV
    { 0.05   ,  0.0525 ,  0.06   ,  0.0725 ,  -100 ,  0.055  ,  0.05   ,  0.0425 ,  0.045   }, // Et : 40 -> 50  GeV
    { 0.0425 ,  0.0425 ,  0.05   ,  0.0625 ,  -100 ,  0.045  ,  0.0425 ,  0.035  ,  0.0375  }, // Et : 50 -> 60  GeV
    { 0.0375 ,  0.045  ,  0.0425 ,  0.06   ,  -100 ,  0.0375 ,  0.0375 ,  0.0275 ,  0.0375  }, // Et : 60 -> 70  GeV
    { 0.0325 ,  0.035  ,  0.04   ,  0.055  ,  -100 ,  0.0325 ,  0.0275 ,  0.03   ,  0.04    }, // Et : 70 -> 80  GeV
    { 0.0275 ,  0.025  ,  0.0375 ,  0.0475 ,  -100 ,  0.025  ,  0.025  ,  0.025  ,  0.035   }  // Et : 80 -> 120  GeV
  };  

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v;
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] );
      }
      m_etcone20DivEt_iso95->push_back(v);
    }
}

void EisoTool::Init_Etcone20DivEt_iso98()
{
  m_etcone20DivEt_iso98 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 0.31   ,  0.285  ,  0.3575 ,  0.3025 ,  -100 ,  0.2975 ,  0.3    ,  0.2375 ,  0.24    }, // Et : 10 -> 15  GeV
    { 0.2275 ,  0.2175 ,  0.245  ,  0.245  ,  -100 ,  0.205  ,  0.19   ,  0.1525 ,  0.18    }, // Et : 15 -> 20  GeV
    { 0.1325 ,  0.1425 ,  0.1575 ,  0.1625 ,  -100 ,  0.1375 ,  0.1225 ,  0.105  ,  0.105   }, // Et : 20 -> 30  GeV
    { 0.1    ,  0.1    ,  0.1075 ,  0.125  ,  -100 ,  0.105  ,  0.0925 ,  0.0775 ,  0.0725  }, // Et : 30 -> 40  GeV
    { 0.07   ,  0.0725 ,  0.08   ,  0.0925 ,  -100 ,  0.0725 ,  0.065  ,  0.0575 ,  0.06    }, // Et : 40 -> 50  GeV
    { 0.0575 ,  0.06   ,  0.065  ,  0.0775 ,  -100 ,  0.0625 ,  0.0575 ,  0.0475 ,  0.0475  }, // Et : 50 -> 60  GeV
    { 0.0525 ,  0.06   ,  0.055  ,  0.075  ,  -100 ,  0.055  ,  0.0475 ,  0.045  ,  0.0475  }, // Et : 60 -> 70  GeV
    { 0.045  ,  0.05   ,  0.0525 ,  0.0725 ,  -100 ,  0.055  ,  0.0425 ,  0.04   ,  0.045   }, // Et : 70 -> 80  GeV
    { 0.04   ,  0.045  ,  0.045  ,  0.0625 ,  -100 ,  0.035  ,  0.04   ,  0.0375 ,  0.055   }  // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v;
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] );
      }
      m_etcone20DivEt_iso98->push_back(v);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////   Etcone 30 / Et Isolation Initilization ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void EisoTool::Init_Etcone30DivEt_iso90()
{
  m_etcone30DivEt_iso90 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 0.26   ,  0.25   ,  0.2575 ,  0.275  ,  -100 ,  0.2825 ,  0.26   ,  0.22   ,  0.2375  }, // Et : 10 -> 15  GeV
    { 0.1975 ,  0.18   ,  0.2025 ,  0.2125 ,  -100 ,  0.185  ,  0.165  ,  0.1425 ,  0.1475  }, // Et : 15 -> 20  GeV
    { 0.1125 ,  0.12   ,  0.1275 ,  0.1425 ,  -100 ,  0.1275 ,  0.1075 ,  0.095  ,  0.1     }, // Et : 20 -> 30  GeV
    { 0.08   ,  0.08   ,  0.09   ,  0.1025 ,  -100 ,  0.0925 ,  0.08   ,  0.07   ,  0.0725  }, // Et : 30 -> 40  GeV
    { 0.06   ,  0.055  ,  0.065  ,  0.075  ,  -100 ,  0.0625 ,  0.0575 ,  0.0525 ,  0.06    }, // Et : 40 -> 50  GeV
    { 0.05   ,  0.05   ,  0.055  ,  0.0625 ,  -100 ,  0.0525 ,  0.0475 ,  0.0425 ,  0.0475  }, // Et : 50 -> 60  GeV
    { 0.0425 ,  0.045  ,  0.0475 ,  0.06   ,  -100 ,  0.045  ,  0.04   ,  0.035  ,  0.04    }, // Et : 60 -> 70  GeV
    { 0.0375 ,  0.0375 ,  0.04   ,  0.0525 ,  -100 ,  0.0375 ,  0.0375 ,  0.035  ,  0.045   }, // Et : 70 -> 80  GeV
    { 0.03   ,  0.03   ,  0.0375 ,  0.0425 ,  -100 ,  0.0325 ,  0.03   ,  0.03   ,  0.04    }  // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v;
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] );
      }
      m_etcone30DivEt_iso90->push_back(v);
    }
}

void EisoTool::Init_Etcone30DivEt_iso95()
{
  m_etcone30DivEt_iso95 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 0.39   ,  0.3525 ,  0.3725 ,  0.3625 ,  -100 ,  0.39   ,  0.405  ,  0.31   ,  0.335   }, // Et : 10 -> 15  GeV
    { 0.2875 ,  0.26   ,  0.2825 ,  0.28   ,  -100 ,  0.25   ,  0.225  ,  0.2025 ,  0.215   }, // Et : 15 -> 20  GeV
    { 0.155  ,  0.1625 ,  0.175  ,  0.1825 ,  -100 ,  0.165  ,  0.145  ,  0.13   ,  0.14    }, // Et : 20 -> 30  GeV
    { 0.11   ,  0.1075 ,  0.1175 ,  0.1325 ,  -100 ,  0.12   ,  0.11   ,  0.0975 ,  0.095   }, // Et : 30 -> 40  GeV
    { 0.08   ,  0.0775 ,  0.0875 ,  0.095  ,  -100 ,  0.085  ,  0.0775 ,  0.07   ,  0.075   }, // Et : 40 -> 50  GeV
    { 0.0675 ,  0.0675 ,  0.07   ,  0.0825 ,  -100 ,  0.07   ,  0.065  ,  0.06   ,  0.065   }, // Et : 50 -> 60  GeV
    { 0.06   ,  0.0625 ,  0.0625 ,  0.0775 ,  -100 ,  0.0625 ,  0.055  ,  0.05   ,  0.055   }, // Et : 60 -> 70  GeV
    { 0.05   ,  0.0525 ,  0.055  ,  0.065  ,  -100 ,  0.05   ,  0.0475 ,  0.0525 ,  0.0675  }, // Et : 70 -> 80  GeV
    { 0.0425 ,  0.0425 ,  0.05   ,  0.0525 ,  -100 ,  0.04   ,  0.0475 ,  0.035  ,  0.05    }  // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v;
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] );
      }
      m_etcone30DivEt_iso95->push_back(v);
    }
}

void EisoTool::Init_Etcone30DivEt_iso98()
{
  m_etcone30DivEt_iso98 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 0.6975 ,  0.5275 ,  0.69   ,  0.535  ,  -100 ,  0.675  ,  0.6125 ,  0.5    ,  0.5475  }, // Et : 10 -> 15  GeV
    { 0.45   ,  0.3875 ,  0.455  ,  0.41   ,  -100 ,  0.3575 ,  0.35   ,  0.305  ,  0.3275  }, // Et : 15 -> 20  GeV
    { 0.2175 ,  0.2375 ,  0.245  ,  0.2525 ,  -100 ,  0.215  ,  0.205  ,  0.1825 ,  0.1975  }, // Et : 20 -> 30  GeV
    { 0.1525 ,  0.1475 ,  0.155  ,  0.1725 ,  -100 ,  0.155  ,  0.145  ,  0.1325 ,  0.125   }, // Et : 30 -> 40  GeV
    { 0.1075 ,  0.105  ,  0.115  ,  0.1275 ,  -100 ,  0.1125 ,  0.1025 ,  0.0975 ,  0.1025  }, // Et : 40 -> 50  GeV
    { 0.0925 ,  0.09   ,  0.095  ,  0.1075 ,  -100 ,  0.095  ,  0.09   ,  0.0825 ,  0.0825  }, // Et : 50 -> 60  GeV
    { 0.0825 ,  0.09   ,  0.08   ,  0.1    ,  -100 ,  0.0775 ,  0.0725 ,  0.07   ,  0.075   }, // Et : 60 -> 70  GeV
    { 0.07   ,  0.0725 ,  0.08   ,  0.0825 ,  -100 ,  0.08   ,  0.075  ,  0.07   ,  0.095   }, // Et : 70 -> 80  GeV
    { 0.075  ,  0.0625 ,  0.0675 ,  0.07   ,  -100 ,  0.0525 ,  0.0825 ,  0.0625 ,  0.0625  }  // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v;
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] );
      }
      m_etcone30DivEt_iso98->push_back(v);
    }
}


void EisoTool::Init_Etcone20DivEt_iso90_rel17()
{
  m_etcone20DivEt_iso90_rel17 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 0.15   ,  0.1425 ,  0.1375 ,  0.1425 ,  -100 ,  0.1075 ,  0.1475 ,  0.115  ,  0.1025  }, // Et : 10 -> 15  GeV
    { 0.105  ,  0.1    ,  0.11   ,  0.115  ,  -100 ,  0.085  ,  0.0975 ,  0.0825 ,  0.0725  }, // Et : 15 -> 20  GeV
    { 0.0625 ,  0.07   ,  0.08   ,  0.09   ,  -100 ,  0.0675 ,  0.07   ,  0.0575 ,  0.055   }, // Et : 20 -> 30  GeV
    { 0.0475 ,  0.05   ,  0.06   ,  0.07   ,  -100 ,  0.0525 ,  0.0525 ,  0.0425 ,  0.0425  }, // Et : 30 -> 40  GeV
    { 0.035  ,  0.0375 ,  0.045  ,  0.0525 ,  -100 ,  0.0375 ,  0.04   ,  0.035  ,  0.035   }, // Et : 40 -> 50  GeV
    { 0.03   ,  0.035  ,  0.0375 ,  0.0475 ,  -100 ,  0.0325 ,  0.035  ,  0.03   ,  0.03    }, // Et : 50 -> 60  GeV
    { 0.025  ,  0.0325 ,  0.035  ,  0.0475 ,  -100 ,  0.03   ,  0.0325 ,  0.025  ,  0.0325  }, // Et : 60 -> 70  GeV
    { 0.025  ,  0.0275 ,  0.0325 ,  0.04   ,  -100 ,  0.0275 ,  0.025  ,  0.025  ,  0.035   }, // Et : 70 -> 80  GeV
    { 0.02   ,  0.025  ,  0.0275 ,  0.0375 ,  -100 ,  0.0225 ,  0.025  ,  0.02   ,  0.0275  } // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v;
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] );
      }
      m_etcone20DivEt_iso90_rel17->push_back(v);
    }
}

void EisoTool::Init_Etcone20DivEt_iso95_rel17()
{
  m_etcone20DivEt_iso95_rel17 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 0.2175 ,  0.195  ,  0.195  ,  0.2    ,  -100 ,  0.16   ,  0.2    ,  0.155  ,  0.155   }, // Et : 10 -> 15  GeV
    { 0.15   ,  0.1425 ,  0.1575 ,  0.1625 ,  -100 ,  0.1175 ,  0.13   ,  0.11   ,  0.115   }, // Et : 15 -> 20  GeV
    { 0.0875 ,  0.1    ,  0.11   ,  0.1175 ,  -100 ,  0.09   ,  0.0925 ,  0.0775 ,  0.0725  }, // Et : 20 -> 30  GeV
    { 0.0675 ,  0.0725 ,  0.08   ,  0.0925 ,  -100 ,  0.07   ,  0.07   ,  0.0575 ,  0.0525  }, // Et : 30 -> 40  GeV
    { 0.05   ,  0.0525 ,  0.06   ,  0.07   ,  -100 ,  0.05   ,  0.0525 ,  0.045  ,  0.045   }, // Et : 40 -> 50  GeV
    { 0.0425 ,  0.045  ,  0.05   ,  0.0625 ,  -100 ,  0.045  ,  0.045  ,  0.04   ,  0.0375  }, // Et : 50 -> 60  GeV
    { 0.0375 ,  0.0475 ,  0.045  ,  0.06   ,  -100 ,  0.0375 ,  0.04   ,  0.0325 ,  0.0375  }, // Et : 60 -> 70  GeV
    { 0.035  ,  0.04   ,  0.04   ,  0.055  ,  -100 ,  0.035  ,  0.0325 ,  0.0325 ,  0.0425  }, // Et : 70 -> 80  GeV
    { 0.0275 ,  0.03   ,  0.035  ,  0.045  ,  -100 ,  0.0275 ,  0.03   ,  0.03   ,  0.0325  } // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v;
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] );
      }
      m_etcone20DivEt_iso95_rel17->push_back(v);
    }
}

void EisoTool::Init_Etcone20DivEt_iso98_rel17()
{
  m_etcone20DivEt_iso98_rel17 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 0.3025 ,  0.2775 ,  0.335  ,  0.275  ,  -100 ,  0.255  ,  0.29   ,  0.2375 ,  0.22    }, // Et : 10 -> 15  GeV
    { 0.225  ,  0.21   ,  0.2325 ,  0.2275 ,  -100 ,  0.18   ,  0.185  ,  0.155  ,  0.1725  }, // Et : 15 -> 20  GeV
    { 0.13   ,  0.14   ,  0.15   ,  0.1525 ,  -100 ,  0.125  ,  0.1225 ,  0.1075 ,  0.1     }, // Et : 20 -> 30  GeV
    { 0.0975 ,  0.1    ,  0.105  ,  0.12   ,  -100 ,  0.095  ,  0.095  ,  0.08   ,  0.07    }, // Et : 30 -> 40  GeV
    { 0.0675 ,  0.075  ,  0.0775 ,  0.09   ,  -100 ,  0.07   ,  0.0675 ,  0.06   ,  0.0575  }, // Et : 40 -> 50  GeV
    { 0.06   ,  0.0625 ,  0.065  ,  0.0775 ,  -100 ,  0.06   ,  0.06   ,  0.0525 ,  0.0475  }, // Et : 50 -> 60  GeV
    { 0.0525 ,  0.06   ,  0.0575 ,  0.0775 ,  -100 ,  0.055  ,  0.05   ,  0.045  ,  0.0475  }, // Et : 60 -> 70  GeV
    { 0.0475 ,  0.0525 ,  0.055  ,  0.0725 ,  -100 ,  0.0575 ,  0.0475 ,  0.0425 ,  0.0475  }, // Et : 70 -> 80  GeV
    { 0.04   ,  0.0475 ,  0.045  ,  0.0625 ,  -100 ,  0.04   ,  0.045  ,  0.0425 ,  0.0575  } // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v;
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] );
      }
      m_etcone20DivEt_iso98_rel17->push_back(v);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////   Etcone 30 / Et Isolation Initilization ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void EisoTool::Init_Etcone30DivEt_iso90_rel17()
{
  m_etcone30DivEt_iso90_rel17 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 0.2475 ,  0.23   ,  0.2275 ,  0.235  ,  -100 ,  0.2225 ,  0.2425 ,  0.2175 ,  0.2125  }, // Et : 10 -> 15  GeV
    { 0.19   ,  0.17   ,  0.1825 ,  0.1875 ,  -100 ,  0.1425 ,  0.1575 ,  0.14   ,  0.1325  }, // Et : 15 -> 20  GeV
    { 0.1075 ,  0.115  ,  0.12   ,  0.1275 ,  -100 ,  0.1    ,  0.1075 ,  0.095  ,  0.0925  }, // Et : 20 -> 30  GeV
    { 0.0775 ,  0.08   ,  0.085  ,  0.095  ,  -100 ,  0.0775 ,  0.08   ,  0.0725 ,  0.0675  }, // Et : 30 -> 40  GeV
    { 0.0575 ,  0.0575 ,  0.065  ,  0.07   ,  -100 ,  0.0525 ,  0.06   ,  0.055  ,  0.0575  }, // Et : 40 -> 50  GeV
    { 0.05   ,  0.0525 ,  0.0525 ,  0.06   ,  -100 ,  0.0475 ,  0.0525 ,  0.0475 ,  0.045   }, // Et : 50 -> 60  GeV
    { 0.0425 ,  0.0475 ,  0.05   ,  0.0625 ,  -100 ,  0.0425 ,  0.045  ,  0.04   ,  0.0425  }, // Et : 60 -> 70  GeV
    { 0.04   ,  0.0425 ,  0.0425 ,  0.055  ,  -100 ,  0.0375 ,  0.04   ,  0.04   ,  0.0475  }, // Et : 70 -> 80  GeV
    { 0.03   ,  0.035  ,  0.0375 ,  0.0475 ,  -100 ,  0.035  ,  0.035  ,  0.0325 ,  0.04    } // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v;
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] );
      }
      m_etcone30DivEt_iso90_rel17->push_back(v);
    }
}

void EisoTool::Init_Etcone30DivEt_iso95_rel17()
{
  m_etcone30DivEt_iso95_rel17 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 0.38   ,  0.33   ,  0.3375 ,  0.3225 ,  -100 ,  0.3325 ,  0.3825 ,  0.305  ,  0.305   }, // Et : 10 -> 15  GeV
    { 0.2775 ,  0.2475 ,  0.26   ,  0.255  ,  -100 ,  0.2075 ,  0.22   ,  0.2    ,  0.2025  }, // Et : 15 -> 20  GeV
    { 0.15   ,  0.16   ,  0.165  ,  0.1675 ,  -100 ,  0.1375 ,  0.145  ,  0.13   ,  0.1325  }, // Et : 20 -> 30  GeV
    { 0.1075 ,  0.1075 ,  0.11   ,  0.125  ,  -100 ,  0.105  ,  0.11   ,  0.0975 ,  0.0925  }, // Et : 30 -> 40  GeV
    { 0.08   ,  0.08   ,  0.085  ,  0.0925 ,  -100 ,  0.075  ,  0.08   ,  0.075  ,  0.075   }, // Et : 40 -> 50  GeV
    { 0.0675 ,  0.07   ,  0.07   ,  0.08   ,  -100 ,  0.0625 ,  0.0675 ,  0.065  ,  0.065   }, // Et : 50 -> 60  GeV
    { 0.06   ,  0.065  ,  0.0625 ,  0.08   ,  -100 ,  0.06   ,  0.06   ,  0.055  ,  0.055   }, // Et : 60 -> 70  GeV
    { 0.0525 ,  0.06   ,  0.0575 ,  0.0675 ,  -100 ,  0.05   ,  0.0525 ,  0.0525 ,  0.0675  }, // Et : 70 -> 80  GeV
    { 0.045  ,  0.05   ,  0.05   ,  0.0575 ,  -100 ,  0.045  ,  0.05   ,  0.0375 ,  0.045   } // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v;
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] );
      }
      m_etcone30DivEt_iso95_rel17->push_back(v);
    }
}

void EisoTool::Init_Etcone30DivEt_iso98_rel17()
{
  m_etcone30DivEt_iso98_rel17 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 0.69   ,  0.5    ,  0.655  ,  0.495  ,  -100 ,  0.6075 ,  0.605  ,  0.49   ,  0.5175  }, // Et : 10 -> 15  GeV
    { 0.4425 ,  0.3725 ,  0.44   ,  0.38   ,  -100 ,  0.315  ,  0.3475 ,  0.295  ,  0.3125  }, // Et : 15 -> 20  GeV
    { 0.2125 ,  0.235  ,  0.235  ,  0.2375 ,  -100 ,  0.19   ,  0.205  ,  0.1825 ,  0.19    }, // Et : 20 -> 30  GeV
    { 0.15   ,  0.1475 ,  0.15   ,  0.1625 ,  -100 ,  0.14   ,  0.145  ,  0.1325 ,  0.1175  }, // Et : 30 -> 40  GeV
    { 0.1075 ,  0.1075 ,  0.1125 ,  0.1225 ,  -100 ,  0.1025 ,  0.105  ,  0.1    ,  0.0975  }, // Et : 40 -> 50  GeV
    { 0.0925 ,  0.0925 ,  0.095  ,  0.1075 ,  -100 ,  0.09   ,  0.09   ,  0.0875 ,  0.0825  }, // Et : 50 -> 60  GeV
    { 0.0825 ,  0.095  ,  0.0825 ,  0.1    ,  -100 ,  0.075  ,  0.0725 ,  0.07   ,  0.0725  }, // Et : 60 -> 70  GeV
    { 0.0725 ,  0.0775 ,  0.0825 ,  0.0875 ,  -100 ,  0.075  ,  0.08   ,  0.07   ,  0.0975  }, // Et : 70 -> 80  GeV
    { 0.08   ,  0.07   ,  0.065  ,  0.075  ,  -100 ,  0.0525 ,  0.0875 ,  0.0675 ,  0.0675  } // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v;
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] );
      }
      m_etcone30DivEt_iso98_rel17->push_back(v);
    }
}



///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////   Ptcone 20 / Et Isolation Initilization ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void EisoTool::Init_Ptcone20DivEt_iso90()
{
  m_ptcone20DivEt_iso90 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 0.0025 ,  0.0025 ,  0.0025 ,  0.0025 ,  -100 ,  0.0025 ,  0.0025 ,  0.0025 ,  0.0025  }, // Et : 10 -> 15  GeV
    { 0.0025 ,  0.0025 ,  0.0025 ,  0.0025 ,  -100 ,  0.0025 ,  0.0025 ,  0.0025 ,  0.0025  }, // Et : 15 -> 20  GeV
    { 0.0025 ,  0.0025 ,  0.0025 ,  0.0025 ,  -100 ,  0.0025 ,  0.0025 ,  0.0025 ,  0.0025  }, // Et : 20 -> 30  GeV
    { 0.0025 ,  0.0025 ,  0.0025 ,  0.0025 ,  -100 ,  0.0025 ,  0.0025 ,  0.0025 ,  0.0025  }, // Et : 30 -> 40  GeV
    { 0.0025 ,  0.0025 ,  0.0025 ,  0.0025 ,  -100 ,  0.0025 ,  0.0025 ,  0.0025 ,  0.0025  }, // Et : 40 -> 50  GeV
    { 0.0025 ,  0.0025 ,  0.0025 ,  0.0025 ,  -100 ,  0.0025 ,  0.0025 ,  0.0025 ,  0.0025  }, // Et : 50 -> 60  GeV
    { 0.0025 ,  0.0025 ,  0.0025 ,  0.0025 ,  -100 ,  0.0025 ,  0.0025 ,  0.0025 ,  0.0025  }, // Et : 60 -> 70  GeV
    { 0.0025 ,  0.0025 ,  0.0025 ,  0.0025 ,  -100 ,  0.0025 ,  0.0025 ,  0.0025 ,  0.0025  }, // Et : 70 -> 80  GeV
    { 0.0025 ,  0.0025 ,  0.0025 ,  0.0025 ,  -100 ,  0.0025 ,  0.0025 ,  0.0025 ,  0.0025  }  // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v; // Et Bins
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] ); // Fill Eta values in Et vector
      }
      m_ptcone20DivEt_iso90->push_back(v);
    }
}

void EisoTool::Init_Ptcone20DivEt_iso97()
{
  m_ptcone20DivEt_iso97 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 0.1425 ,  0.18   ,  0.1875 ,  0.1275 ,  -100 ,  0.1625 ,  0.195  ,  0.105  ,  0.095   }, // Et : 10 -> 15  GeV
    { 0.1    ,  0.0875 ,  0.105  ,  0.1025 ,  -100 ,  0.085  ,  0.0725 ,  0.08   ,  0.0875  }, // Et : 15 -> 20  GeV
    { 0.05   ,  0.06   ,  0.055  ,  0.055  ,  -100 ,  0.055  ,  0.055  ,  0.0475 ,  0.05    }, // Et : 20 -> 30  GeV
    { 0.035  ,  0.035  ,  0.035  ,  0.0375 ,  -100 ,  0.0375 ,  0.04   ,  0.0375 ,  0.0375  }, // Et : 30 -> 40  GeV
    { 0.025  ,  0.025  ,  0.025  ,  0.025  ,  -100 ,  0.0275 ,  0.03   ,  0.0275 ,  0.0275  }, // Et : 40 -> 50  GeV
    { 0.02   ,  0.0025 ,  0.02   ,  0.0225 ,  -100 ,  0.0225 ,  0.025  ,  0.025  ,  0.02    }, // Et : 50 -> 60  GeV
    { 0.0175 ,  0.02   ,  0.0175 ,  0.02   ,  -100 ,  0.025  ,  0.0175 ,  0.0175 ,  0.0025  }, // Et : 60 -> 70  GeV
    { 0.015  ,  0.0175 ,  0.015  ,  0.0175 ,  -100 ,  0.0175 ,  0.0375 ,  0.02   ,  0.0225  }, // Et : 70 -> 80  GeV
    { 0.0125 ,  0.0125 ,  0.0125 ,  0.0025 ,  -100 ,  0.02   ,  0.015  ,  0.0125 ,  0.0025  }  // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v; // Et Bins
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] ); // Fill Eta values in Et vector
      }
      m_ptcone20DivEt_iso97->push_back(v);
    }
}

void EisoTool::Init_Ptcone20DivEt_iso98()
{
  m_ptcone20DivEt_iso98 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 0.1925 ,  0.3075 ,  0.2575 ,  0.15   ,  -100 ,  0.265  ,  0.265  ,  0.15   ,  0.13    }, // Et : 10 -> 15  GeV
    { 0.135  ,  0.1075 ,  0.135  ,  0.1475 ,  -100 ,  0.1075 ,  0.0875 ,  0.1125 ,  0.1225  }, // Et : 15 -> 20  GeV
    { 0.0625 ,  0.0725 ,  0.0725 ,  0.075  ,  -100 ,  0.07   ,  0.0725 ,  0.065  ,  0.0675  }, // Et : 20 -> 30  GeV
    { 0.0425 ,  0.045  ,  0.045  ,  0.0475 ,  -100 ,  0.0475 ,  0.0525 ,  0.0475 ,  0.0525  }, // Et : 30 -> 40  GeV
    { 0.03   ,  0.0325 ,  0.0325 ,  0.0325 ,  -100 ,  0.035  ,  0.0425 ,  0.035  ,  0.0375  }, // Et : 40 -> 50  GeV
    { 0.025  ,  0.025  ,  0.025  ,  0.03   ,  -100 ,  0.0325 ,  0.035  ,  0.0325 ,  0.03    }, // Et : 50 -> 60  GeV
    { 0.0225 ,  0.035  ,  0.02   ,  0.025  ,  -100 ,  0.04   ,  0.02   ,  0.03   ,  0.02    }, // Et : 60 -> 70  GeV
    { 0.02   ,  0.02   ,  0.0175 ,  0.02   ,  -100 ,  0.0375 ,  0.0575 ,  0.0275 ,  0.0225  }, // Et : 70 -> 80  GeV
    { 0.0175 ,  0.015  ,  0.0175 ,  0.0125 ,  -100 ,  0.0275 ,  0.035  ,  0.02   ,  0.0025  }  // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v; // Et Bins
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] ); // Fill Eta values in Et vector
      }
      m_ptcone20DivEt_iso98->push_back(v);
    }
}

void EisoTool::Init_Ptcone20DivEt_iso99()
{
  m_ptcone20DivEt_iso99 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 0.32   ,  0.5825 ,  0.465  ,  0.2325 ,  -100 ,  0.4675 ,  0.3425 ,  0.255  ,  0.32    }, // Et : 10 -> 15  GeV
    { 0.245  ,  0.2075 ,  0.225  ,  0.3075 ,  -100 ,  0.18   ,  0.1325 ,  0.1775 ,  0.205   }, // Et : 15 -> 20  GeV
    { 0.095  ,  0.115  ,  0.105  ,  0.1175 ,  -100 ,  0.1175 ,  0.13   ,  0.105  ,  0.1175  }, // Et : 20 -> 30  GeV
    { 0.0625 ,  0.065  ,  0.065  ,  0.0725 ,  -100 ,  0.0825 ,  0.095  ,  0.075  ,  0.125   }, // Et : 30 -> 40  GeV
    { 0.0425 ,  0.045  ,  0.045  ,  0.0475 ,  -100 ,  0.065  ,  0.075  ,  0.06   ,  0.065   }, // Et : 40 -> 50  GeV
    { 0.035  ,  0.0375 ,  0.0375 ,  0.0425 ,  -100 ,  0.06   ,  0.0675 ,  0.0525 ,  0.0625  }, // Et : 50 -> 60  GeV
    { 0.035  ,  0.05   ,  0.0275 ,  0.06   ,  -100 ,  0.0625 ,  0.0325 ,  0.04   ,  0.035   }, // Et : 60 -> 70  GeV
    { 0.0275 ,  0.03   ,  0.0375 ,  0.0275 ,  -100 ,  0.0775 ,  0.075  ,  0.0325 ,  0.03    }, // Et : 70 -> 80  GeV
    { 0.0225 ,  0.035  ,  0.0225 ,  0.0325 ,  -100 ,  0.05   ,  0.1275 ,  0.03   ,  0.045   }  // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v; // Et Bins
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] ); // Fill Eta values in Et vector
      }
      m_ptcone20DivEt_iso99->push_back(v);
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////   Ptcone 30 / Et Isolation Initilization ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void EisoTool::Init_Ptcone30DivEt_iso90()
{
  m_ptcone30DivEt_iso90 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 0.0975 ,  0.1025 ,  0.095  ,  0.0975 ,  -100 ,  0.0925 ,  0.09   ,  0.0025 ,  0.0025  }, // Et : 10 -> 15  GeV
    { 0.065  ,  0.065  ,  0.07   ,  0.065  ,  -100 ,  0.0025 ,  0.0025 ,  0.0025 ,  0.0025  }, // Et : 15 -> 20  GeV
    { 0.0025 ,  0.04   ,  0.035  ,  0.0025 ,  -100 ,  0.0025 ,  0.0025 ,  0.0025 ,  0.0025  }, // Et : 20 -> 30  GeV
    { 0.0025 ,  0.0025 ,  0.0025 ,  0.0025 ,  -100 ,  0.0025 ,  0.0025 ,  0.0025 ,  0.0025  }, // Et : 30 -> 40  GeV
    { 0.0025 ,  0.0025 ,  0.0025 ,  0.0025 ,  -100 ,  0.0025 ,  0.0025 ,  0.0025 ,  0.0025  }, // Et : 40 -> 50  GeV
    { 0.0025 ,  0.0025 ,  0.0025 ,  0.0025 ,  -100 ,  0.0025 ,  0.0025 ,  0.0025 ,  0.0025  }, // Et : 50 -> 60  GeV
    { 0.0025 ,  0.0025 ,  0.0025 ,  0.0025 ,  -100 ,  0.0025 ,  0.0025 ,  0.0025 ,  0.0025  }, // Et : 60 -> 70  GeV
    { 0.0025 ,  0.0025 ,  0.0025 ,  0.0025 ,  -100 ,  0.0025 ,  0.0025 ,  0.0025 ,  0.0025  }, // Et : 70 -> 80  GeV
    { 0.0025 ,  0.0025 ,  0.0025 ,  0.0025 ,  -100 ,  0.0025 ,  0.0125 ,  0.0025 ,  0.0025  }  // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v; // Et Bins
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] ); // Fill Eta values in Et vector
      }
      m_ptcone30DivEt_iso90->push_back(v);
    }
}

void EisoTool::Init_Ptcone30DivEt_iso97()
{
  m_ptcone30DivEt_iso97 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 0.355  ,  0.32   ,  0.37   ,  0.29   ,  -100 ,  0.3775 ,  0.265  ,  0.205  ,  0.28    }, // Et : 10 -> 15  GeV
    { 0.1975 ,  0.165  ,  0.21   ,  0.2075 ,  -100 ,  0.1375 ,  0.1375 ,  0.1325 ,  0.1225  }, // Et : 15 -> 20  GeV
    { 0.08   ,  0.1025 ,  0.0975 ,  0.095  ,  -100 ,  0.0875 ,  0.085  ,  0.0775 ,  0.07    }, // Et : 20 -> 30  GeV
    { 0.0525 ,  0.055  ,  0.0525 ,  0.055  ,  -100 ,  0.0525 ,  0.06   ,  0.0525 ,  0.055   }, // Et : 30 -> 40  GeV
    { 0.0375 ,  0.0375 ,  0.0375 ,  0.0375 ,  -100 ,  0.04   ,  0.0425 ,  0.0375 ,  0.0375  }, // Et : 40 -> 50  GeV
    { 0.03   ,  0.0325 ,  0.0325 ,  0.035  ,  -100 ,  0.0375 ,  0.0375 ,  0.035  ,  0.025   }, // Et : 50 -> 60  GeV
    { 0.0275 ,  0.035  ,  0.0275 ,  0.025  ,  -100 ,  0.035  ,  0.0225 ,  0.0275 ,  0.0025  }, // Et : 60 -> 70  GeV
    { 0.025  ,  0.03   ,  0.0275 ,  0.0325 ,  -100 ,  0.0275 ,  0.0375 ,  0.025  ,  0.0225  }, // Et : 70 -> 80  GeV
    { 0.0225 ,  0.0225 ,  0.0175 ,  0.0175 ,  -100 ,  0.0325 ,  0.055  ,  0.02   ,  0.03    }  // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v; // Et Bins
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] ); // Fill Eta values in Et vector
      }
      m_ptcone30DivEt_iso97->push_back(v);
    }
}

void EisoTool::Init_Ptcone30DivEt_iso98()
{
  m_ptcone30DivEt_iso98 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 0.45   ,  0.52   ,  0.49   ,  0.395  ,  -100 ,  0.54   ,  0.4275 ,  0.295  ,  0.3575  }, // Et : 10 -> 15  GeV
    { 0.31   ,  0.22   ,  0.315  ,  0.275  ,  -100 ,  0.19   ,  0.2    ,  0.175  ,  0.1925  }, // Et : 15 -> 20  GeV
    { 0.11   ,  0.135  ,  0.125  ,  0.13   ,  -100 ,  0.1175 ,  0.1175 ,  0.105  ,  0.09    }, // Et : 20 -> 30  GeV
    { 0.0675 ,  0.0675 ,  0.0675 ,  0.0675 ,  -100 ,  0.07   ,  0.0825 ,  0.07   ,  0.0725  }, // Et : 30 -> 40  GeV
    { 0.045  ,  0.0475 ,  0.0475 ,  0.0475 ,  -100 ,  0.0525 ,  0.0575 ,  0.05   ,  0.045   }, // Et : 40 -> 50  GeV
    { 0.04   ,  0.04   ,  0.04   ,  0.0425 ,  -100 ,  0.0475 ,  0.05   ,  0.0425 ,  0.035   }, // Et : 50 -> 60  GeV
    { 0.035  ,  0.05   ,  0.035  ,  0.0375 ,  -100 ,  0.045  ,  0.0325 ,  0.035  ,  0.02    }, // Et : 60 -> 70  GeV
    { 0.0325 ,  0.04   ,  0.035  ,  0.0375 ,  -100 ,  0.0425 ,  0.0575 ,  0.0275 ,  0.0225  }, // Et : 70 -> 80  GeV
    { 0.03   ,  0.0225 ,  0.0225 ,  0.025  ,  -100 ,  0.04   ,  0.0875 ,  0.025  ,  0.03    }  // Et : 80 -> 120  GeV
  }; 

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v; // Et Bins
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] ); // Fill Eta values in Et vector
      }
      m_ptcone30DivEt_iso98->push_back(v);
    }
}

void EisoTool::Init_Ptcone30DivEt_iso99()
{
  m_ptcone30DivEt_iso99 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 0.72   ,  0.9875 ,  0.9275 ,  0.6475 ,  -100 ,  0.7675 ,  0.725  ,  0.435  ,  0.615   }, // Et : 10 -> 15  GeV
    { 0.55   ,  0.37   ,  0.5625 ,  0.5    ,  -100 ,  0.3125 ,  0.3075 ,  0.2675 ,  0.23    }, // Et : 15 -> 20  GeV
    { 0.18   ,  0.225  ,  0.2075 ,  0.2    ,  -100 ,  0.195  ,  0.1975 ,  0.1575 ,  0.1475  }, // Et : 20 -> 30  GeV
    { 0.1    ,  0.1025 ,  0.0975 ,  0.1075 ,  -100 ,  0.115  ,  0.1325 ,  0.1025 ,  0.15    }, // Et : 30 -> 40  GeV
    { 0.0625 ,  0.0675 ,  0.0675 ,  0.0675 ,  -100 ,  0.085  ,  0.0975 ,  0.0775 ,  0.07    }, // Et : 40 -> 50  GeV
    { 0.055  ,  0.0575 ,  0.0575 ,  0.0675 ,  -100 ,  0.0825 ,  0.095  ,  0.07   ,  0.09    }, // Et : 50 -> 60  GeV
    { 0.0525 ,  0.075  ,  0.045  ,  0.09   ,  -100 ,  0.0675 ,  0.0425 ,  0.045  ,  0.035   }, // Et : 60 -> 70  GeV
    { 0.045  ,  0.0525 ,  0.0525 ,  0.045  ,  -100 ,  0.1425 ,  0.0775 ,  0.0575 ,  0.03    }, // Et : 70 -> 80  GeV
    { 0.045  ,  0.06   ,  0.03   ,  0.0325 ,  -100 ,  0.065  ,  0.1275 ,  0.05   ,  0.045   }  // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v; // Et Bins
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] ); // Fill Eta values in Et vector
      }
      m_ptcone30DivEt_iso99->push_back(v);
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////   Ptcone 40 / Et Isolation Initilization ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void EisoTool::Init_Ptcone40DivEt_iso90()
{
  m_ptcone40DivEt_iso90 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 0.1675 ,  0.1775 ,  0.1625 ,  0.1225 ,  -100 ,  0.15   ,  0.1375 ,  0.1125 ,  0.1025  }, // Et : 10 -> 15  GeV
    { 0.105  ,  0.1025 ,  0.1125 ,  0.09   ,  -100 ,  0.0875 ,  0.085  ,  0.075  ,  0.065   }, // Et : 15 -> 20  GeV
    { 0.0525 ,  0.0625 ,  0.06   ,  0.0575 ,  -100 ,  0.0525 ,  0.05   ,  0.045  ,  0.0025  }, // Et : 20 -> 30  GeV
    { 0.035  ,  0.035  ,  0.035  ,  0.035  ,  -100 ,  0.035  ,  0.0375 ,  0.0325 ,  0.0025  }, // Et : 30 -> 40  GeV
    { 0.025  ,  0.025  ,  0.025  ,  0.025  ,  -100 ,  0.025  ,  0.025  ,  0.0225 ,  0.0025  }, // Et : 40 -> 50  GeV
    { 0.02   ,  0.0225 ,  0.0225 ,  0.02   ,  -100 ,  0.0225 ,  0.0225 ,  0.02   ,  0.0025  }, // Et : 50 -> 60  GeV
    { 0.02   ,  0.0225 ,  0.0175 ,  0.0175 ,  -100 ,  0.02   ,  0.0175 ,  0.0025 ,  0.0025  }, // Et : 60 -> 70  GeV
    { 0.0175 ,  0.0175 ,  0.0175 ,  0.0175 ,  -100 ,  0.015  ,  0.015  ,  0.0025 ,  0.0025  }, // Et : 70 -> 80  GeV
    { 0.0125 ,  0.0125 ,  0.01   ,  0.0025 ,  -100 ,  0.0125 ,  0.02   ,  0.0125 ,  0.0025  }  // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v; // Et Bins
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] ); // Fill Eta values in Et vector
      }
      m_ptcone40DivEt_iso90->push_back(v);
    }
}

void EisoTool::Init_Ptcone40DivEt_iso97()
{
  m_ptcone40DivEt_iso97 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 0.625  ,  0.5225 ,  0.5    ,  0.445  ,  -100 ,  0.6175 ,  0.5325 ,  0.295  ,  0.4125  }, // Et : 10 -> 15  GeV
    { 0.365  ,  0.3    ,  0.4025 ,  0.32   ,  -100 ,  0.2475 ,  0.2525 ,  0.205  ,  0.18    }, // Et : 15 -> 20  GeV
    { 0.1375 ,  0.1925 ,  0.1525 ,  0.16   ,  -100 ,  0.1475 ,  0.13   ,  0.115  ,  0.0925  }, // Et : 20 -> 30  GeV
    { 0.08   ,  0.0775 ,  0.075  ,  0.0775 ,  -100 ,  0.08   ,  0.085  ,  0.0725 ,  0.065   }, // Et : 30 -> 40  GeV
    { 0.0525 ,  0.0525 ,  0.0525 ,  0.0525 ,  -100 ,  0.055  ,  0.0575 ,  0.05   ,  0.045   }, // Et : 40 -> 50  GeV
    { 0.045  ,  0.0425 ,  0.045  ,  0.045  ,  -100 ,  0.05   ,  0.0525 ,  0.0425 ,  0.03    }, // Et : 50 -> 60  GeV
    { 0.04   ,  0.0525 ,  0.0425 ,  0.0375 ,  -100 ,  0.0425 ,  0.0375 ,  0.03   ,  0.03    }, // Et : 60 -> 70  GeV
    { 0.0375 ,  0.0425 ,  0.0325 ,  0.0375 ,  -100 ,  0.04   ,  0.0575 ,  0.0275 ,  0.0225  }, // Et : 70 -> 80  GeV
    { 0.04   ,  0.0425 ,  0.025  ,  0.0275 ,  -100 ,  0.04   ,  0.1    ,  0.03   ,  0.03    }  // Et : 80 -> 120  GeV
  };  

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v; // Et Bins
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] ); // Fill Eta values in Et vector
      }
      m_ptcone40DivEt_iso97->push_back(v);
    }
}

void EisoTool::Init_Ptcone40DivEt_iso98()
{
  m_ptcone40DivEt_iso98 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { 0.9975 ,  0.81   ,  0.6775 ,  0.585  ,  -100 ,  0.945  ,  0.7825 ,  0.3975 ,  0.5375  }, // Et : 10 -> 15  GeV
    { 0.6025 ,  0.465  ,  0.61   ,  0.5325 ,  -100 ,  0.3725 ,  0.3425 ,  0.27   ,  0.21    }, // Et : 15 -> 20  GeV
    { 0.19   ,  0.27   ,  0.2125 ,  0.21   ,  -100 ,  0.2    ,  0.175  ,  0.1525 ,  0.13    }, // Et : 20 -> 30  GeV
    { 0.1025 ,  0.1    ,  0.0975 ,  0.1025 ,  -100 ,  0.1025 ,  0.1125 ,  0.0925 ,  0.0875  }, // Et : 30 -> 40  GeV
    { 0.0625 ,  0.065  ,  0.0675 ,  0.065  ,  -100 ,  0.07   ,  0.0725 ,  0.065  ,  0.0575  }, // Et : 40 -> 50  GeV
    { 0.055  ,  0.055  ,  0.0575 ,  0.0625 ,  -100 ,  0.0725 ,  0.0725 ,  0.0525 ,  0.0475  }, // Et : 50 -> 60  GeV
    { 0.0525 ,  0.07   ,  0.05   ,  0.0575 ,  -100 ,  0.06   ,  0.05   ,  0.0375 ,  0.035   }, // Et : 60 -> 70  GeV
    { 0.0475 ,  0.0675 ,  0.04   ,  0.04   ,  -100 ,  0.065  ,  0.0775 ,  0.0525 ,  0.0225  }, // Et : 70 -> 80  GeV
    { 0.06   ,  0.0725 ,  0.0325 ,  0.035  ,  -100 ,  0.0525 ,  0.11   ,  0.0375 ,  0.03    }  // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v; // Et Bins
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] ); // Fill Eta values in Et vector
      }
      m_ptcone40DivEt_iso98->push_back(v);
    }
}

void EisoTool::Init_Ptcone40DivEt_iso99()
{
  m_ptcone40DivEt_iso99 = new std::vector<std::vector<double> >;

  const double m[9][9] = {
    { -100   ,  -100   ,  -100   ,  -100   ,  -100 ,  -100   ,  -100   ,  0.5925 ,  0.6925  }, // Et : 10 -> 15  GeV
    { 1.085  ,  0.8075 ,  1.1325 ,  0.82   ,  -100 ,  0.6425 ,  0.595  ,  0.4725 ,  0.33    }, // Et : 15 -> 20  GeV
    { 0.3275 ,  0.48   ,  0.3825 ,  0.335  ,  -100 ,  0.3275 ,  0.275  ,  0.23   ,  0.23    }, // Et : 20 -> 30  GeV
    { 0.155  ,  0.1475 ,  0.1375 ,  0.155  ,  -100 ,  0.16   ,  0.1825 ,  0.1425 ,  0.1625  }, // Et : 30 -> 40  GeV
    { 0.0875 ,  0.09   ,  0.09   ,  0.0925 ,  -100 ,  0.105  ,  0.125  ,  0.0975 ,  0.085   }, // Et : 40 -> 50  GeV
    { 0.0775 ,  0.0725 ,  0.08   ,  0.095  ,  -100 ,  0.1125 ,  0.115  ,  0.08   ,  0.12    }, // Et : 50 -> 60  GeV
    { 0.08   ,  0.0975 ,  0.075  ,  0.115  ,  -100 ,  0.09   ,  0.0675 ,  0.08   ,  0.0575  }, // Et : 60 -> 70  GeV
    { 0.065  ,  0.125  ,  0.0575 ,  0.065  ,  -100 ,  0.1425 ,  0.0925 ,  0.0575 ,  0.03    }, // Et : 70 -> 80  GeV
    { 0.1175 ,  0.105  ,  0.0425 ,  0.0825 ,  -100 ,  0.0675 ,  0.1275 ,  0.05   ,  0.045   }  // Et : 80 -> 120  GeV
  };

    for(unsigned int i=0;i<9;++i){
      std::vector<double> v; // Et Bins
      for(unsigned int j=0;j<9;++j){
        v.push_back( m[i][j] ); // Fill Eta values in Et vector
      }
      m_ptcone40DivEt_iso99->push_back(v);
    }
}

void EisoTool::Init_Etcone20_iso90_rel17_sumEt()
{
  m_etcone20_iso90_rel17_sumEt = new std::vector<double>;
  m_etcone20_iso90_rel17_sumEt->push_back( 1.65 );
  m_etcone20_iso90_rel17_sumEt->push_back( 1.75 );
  m_etcone20_iso90_rel17_sumEt->push_back( 2 );
  m_etcone20_iso90_rel17_sumEt->push_back( 2.45 );
  m_etcone20_iso90_rel17_sumEt->push_back( -100 );
  m_etcone20_iso90_rel17_sumEt->push_back( 1.7 );
  m_etcone20_iso90_rel17_sumEt->push_back( 1.8 );
  m_etcone20_iso90_rel17_sumEt->push_back( 1.5 );
  m_etcone20_iso90_rel17_sumEt->push_back( 1.5 );
}

void EisoTool::Init_Etcone20_iso95_rel17_sumEt()
{
  m_etcone20_iso95_rel17_sumEt = new std::vector<double>;
  m_etcone20_iso95_rel17_sumEt->push_back( 2.3 );
  m_etcone20_iso95_rel17_sumEt->push_back( 2.45 );
  m_etcone20_iso95_rel17_sumEt->push_back( 2.7 );
  m_etcone20_iso95_rel17_sumEt->push_back( 3.2 );
  m_etcone20_iso95_rel17_sumEt->push_back( -100 );
  m_etcone20_iso95_rel17_sumEt->push_back( 2.35 );
  m_etcone20_iso95_rel17_sumEt->push_back( 2.35 );
  m_etcone20_iso95_rel17_sumEt->push_back( 2 );
  m_etcone20_iso95_rel17_sumEt->push_back( 1.95 );  
}

void EisoTool::Init_Etcone20_iso98_rel17_sumEt()
{
  m_etcone20_iso98_rel17_sumEt = new std::vector<double>;
  m_etcone20_iso98_rel17_sumEt->push_back( 3.25 );
  m_etcone20_iso98_rel17_sumEt->push_back( 3.45 );
  m_etcone20_iso98_rel17_sumEt->push_back( 3.6 );
  m_etcone20_iso98_rel17_sumEt->push_back( 4.15 );
  m_etcone20_iso98_rel17_sumEt->push_back( -100 );
  m_etcone20_iso98_rel17_sumEt->push_back( 3.2 );
  m_etcone20_iso98_rel17_sumEt->push_back( 3.15 );
  m_etcone20_iso98_rel17_sumEt->push_back( 2.65 );
  m_etcone20_iso98_rel17_sumEt->push_back( 2.55 );  
}


void EisoTool::Init_Etcone30_iso90_rel17_sumEt()
{
  m_etcone30_iso90_rel17_sumEt = new std::vector<double>;
  m_etcone30_iso90_rel17_sumEt->push_back( 2.65 );
  m_etcone30_iso90_rel17_sumEt->push_back( 2.75 );
  m_etcone30_iso90_rel17_sumEt->push_back( 2.9 );
  m_etcone30_iso90_rel17_sumEt->push_back( 3.25 );
  m_etcone30_iso90_rel17_sumEt->push_back( -100 );
  m_etcone30_iso90_rel17_sumEt->push_back( 2.55 );
  m_etcone30_iso90_rel17_sumEt->push_back( 2.75 );
  m_etcone30_iso90_rel17_sumEt->push_back( 2.45 );
  m_etcone30_iso90_rel17_sumEt->push_back( 2.45 );  
}

void EisoTool::Init_Etcone30_iso95_rel17_sumEt()
{
  m_etcone30_iso95_rel17_sumEt = new std::vector<double>;
  m_etcone30_iso95_rel17_sumEt->push_back( 3.65 );
  m_etcone30_iso95_rel17_sumEt->push_back( 3.75 );
  m_etcone30_iso95_rel17_sumEt->push_back( 3.9 );
  m_etcone30_iso95_rel17_sumEt->push_back( 4.3 );
  m_etcone30_iso95_rel17_sumEt->push_back( -100 );
  m_etcone30_iso95_rel17_sumEt->push_back( 3.5 );
  m_etcone30_iso95_rel17_sumEt->push_back( 3.7 );
  m_etcone30_iso95_rel17_sumEt->push_back( 3.35 );
  m_etcone30_iso95_rel17_sumEt->push_back( 3.3 );
}

void EisoTool::Init_Etcone30_iso98_rel17_sumEt()
{
  m_etcone30_iso98_rel17_sumEt = new std::vector<double>;
  m_etcone30_iso98_rel17_sumEt->push_back( 5.15 );
  m_etcone30_iso98_rel17_sumEt->push_back( 5.15 );
  m_etcone30_iso98_rel17_sumEt->push_back( 5.2 );
  m_etcone30_iso98_rel17_sumEt->push_back( 5.7 );
  m_etcone30_iso98_rel17_sumEt->push_back( -100 );
  m_etcone30_iso98_rel17_sumEt->push_back( 4.75 );
  m_etcone30_iso98_rel17_sumEt->push_back( 5 );
  m_etcone30_iso98_rel17_sumEt->push_back( 4.55 );
  m_etcone30_iso98_rel17_sumEt->push_back( 4.5 );
}


void EisoTool::Init_Ptcone20_iso90_sumEt()
{
  m_ptcone20_iso90_sumEt = new std::vector<double>;
  m_ptcone20_iso90_sumEt->push_back( 1 );
  m_ptcone20_iso90_sumEt->push_back( 1 );
  m_ptcone20_iso90_sumEt->push_back( 1 );
  m_ptcone20_iso90_sumEt->push_back( 1 );
  m_ptcone20_iso90_sumEt->push_back( -100 );
  m_ptcone20_iso90_sumEt->push_back( 1 );
  m_ptcone20_iso90_sumEt->push_back( 1 );
  m_ptcone20_iso90_sumEt->push_back( 1 );
  m_ptcone20_iso90_sumEt->push_back( 1 );  
}

void EisoTool::Init_Ptcone20_iso97_sumEt()
{
  m_ptcone20_iso97_sumEt = new std::vector<double>;
  m_ptcone20_iso97_sumEt->push_back( 1.15 );
  m_ptcone20_iso97_sumEt->push_back( 1.2 );
  m_ptcone20_iso97_sumEt->push_back( 1.2 );
  m_ptcone20_iso97_sumEt->push_back( 1.25 );
  m_ptcone20_iso97_sumEt->push_back( -100 );
  m_ptcone20_iso97_sumEt->push_back( 1.3 );
  m_ptcone20_iso97_sumEt->push_back( 1.35 );
  m_ptcone20_iso97_sumEt->push_back( 1.25 );
  m_ptcone20_iso97_sumEt->push_back( 1.2 );  
}

void EisoTool::Init_Ptcone20_iso98_sumEt()
{
  m_ptcone20_iso98_sumEt = new std::vector<double>;
  m_ptcone20_iso98_sumEt->push_back( 1.45 );
  m_ptcone20_iso98_sumEt->push_back( 1.55 );
  m_ptcone20_iso98_sumEt->push_back( 1.55 );
  m_ptcone20_iso98_sumEt->push_back( 1.6 );
  m_ptcone20_iso98_sumEt->push_back( -100 );
  m_ptcone20_iso98_sumEt->push_back( 1.7 );
  m_ptcone20_iso98_sumEt->push_back( 1.85 );
  m_ptcone20_iso98_sumEt->push_back( 1.65 );
  m_ptcone20_iso98_sumEt->push_back( 1.75 );  
}

void EisoTool::Init_Ptcone20_iso99_sumEt()
{
  m_ptcone20_iso99_sumEt = new std::vector<double>;
  m_ptcone20_iso99_sumEt->push_back( 2.1 );
  m_ptcone20_iso99_sumEt->push_back( 2.25 );
  m_ptcone20_iso99_sumEt->push_back( 2.25 );
  m_ptcone20_iso99_sumEt->push_back( 2.45 );
  m_ptcone20_iso99_sumEt->push_back( -100 );
  m_ptcone20_iso99_sumEt->push_back( 3 );
  m_ptcone20_iso99_sumEt->push_back( 3.4 );
  m_ptcone20_iso99_sumEt->push_back( 2.7 );
  m_ptcone20_iso99_sumEt->push_back( 3.25 );  
}


void EisoTool::Init_Ptcone30_iso90_sumEt()
{
  m_ptcone30_iso90_sumEt = new std::vector<double>;
  m_ptcone30_iso90_sumEt->push_back( 1 );
  m_ptcone30_iso90_sumEt->push_back( 1 );
  m_ptcone30_iso90_sumEt->push_back( 1 );
  m_ptcone30_iso90_sumEt->push_back( 1 );
  m_ptcone30_iso90_sumEt->push_back( -100 );
  m_ptcone30_iso90_sumEt->push_back( 1 );
  m_ptcone30_iso90_sumEt->push_back( 1 );
  m_ptcone30_iso90_sumEt->push_back( 1 );
  m_ptcone30_iso90_sumEt->push_back( 1 );  
}

void EisoTool::Init_Ptcone30_iso97_sumEt()
{
  m_ptcone30_iso97_sumEt = new std::vector<double>;
  m_ptcone30_iso97_sumEt->push_back( 1.8 );
  m_ptcone30_iso97_sumEt->push_back( 1.9 );
  m_ptcone30_iso97_sumEt->push_back( 1.85 );
  m_ptcone30_iso97_sumEt->push_back( 1.85 );
  m_ptcone30_iso97_sumEt->push_back( -100 );
  m_ptcone30_iso97_sumEt->push_back( 1.95 );
  m_ptcone30_iso97_sumEt->push_back( 2.05 );
  m_ptcone30_iso97_sumEt->push_back( 1.8 );
  m_ptcone30_iso97_sumEt->push_back( 1.75 );  
}

void EisoTool::Init_Ptcone30_iso98_sumEt()
{
  m_ptcone30_iso98_sumEt = new std::vector<double>;
  m_ptcone30_iso98_sumEt->push_back( 2.3 );
  m_ptcone30_iso98_sumEt->push_back( 2.45 );
  m_ptcone30_iso98_sumEt->push_back( 2.4 );
  m_ptcone30_iso98_sumEt->push_back( 2.4 );
  m_ptcone30_iso98_sumEt->push_back( -100 );
  m_ptcone30_iso98_sumEt->push_back( 2.65 );
  m_ptcone30_iso98_sumEt->push_back( 2.8 );
  m_ptcone30_iso98_sumEt->push_back( 2.4 );
  m_ptcone30_iso98_sumEt->push_back( 2.35 );  
}

void EisoTool::Init_Ptcone30_iso99_sumEt()
{
  m_ptcone30_iso99_sumEt = new std::vector<double>;
  m_ptcone30_iso99_sumEt->push_back( 3.45 );
  m_ptcone30_iso99_sumEt->push_back( 3.55 );
  m_ptcone30_iso99_sumEt->push_back( 3.55 );
  m_ptcone30_iso99_sumEt->push_back( 3.8 );
  m_ptcone30_iso99_sumEt->push_back( -100 );
  m_ptcone30_iso99_sumEt->push_back( 4.25 );
  m_ptcone30_iso99_sumEt->push_back( 4.75 );
  m_ptcone30_iso99_sumEt->push_back( 3.8 );
  m_ptcone30_iso99_sumEt->push_back( 3.9 );  
}


void EisoTool::Init_Ptcone40_iso90_sumEt()
{
  m_ptcone40_iso90_sumEt = new std::vector<double>;
  m_ptcone40_iso90_sumEt->push_back( 1.2 );
  m_ptcone40_iso90_sumEt->push_back( 1.2 );
  m_ptcone40_iso90_sumEt->push_back( 1.2 );
  m_ptcone40_iso90_sumEt->push_back( 1.2 );
  m_ptcone40_iso90_sumEt->push_back( -100 );
  m_ptcone40_iso90_sumEt->push_back( 1.2 );
  m_ptcone40_iso90_sumEt->push_back( 1.2 );
  m_ptcone40_iso90_sumEt->push_back( 1.1 );
  m_ptcone40_iso90_sumEt->push_back( 1 );  
}

void EisoTool::Init_Ptcone40_iso97_sumEt()
{
  m_ptcone40_iso97_sumEt = new std::vector<double>;
  m_ptcone40_iso97_sumEt->push_back( 2.7 );
  m_ptcone40_iso97_sumEt->push_back( 2.8 );
  m_ptcone40_iso97_sumEt->push_back( 2.75 );
  m_ptcone40_iso97_sumEt->push_back( 2.75 );
  m_ptcone40_iso97_sumEt->push_back( -100 );
  m_ptcone40_iso97_sumEt->push_back( 2.85 );
  m_ptcone40_iso97_sumEt->push_back( 2.9 );
  m_ptcone40_iso97_sumEt->push_back( 2.55 );
  m_ptcone40_iso97_sumEt->push_back( 2.25 );  
}

void EisoTool::Init_Ptcone40_iso98_sumEt()
{
  m_ptcone40_iso98_sumEt = new std::vector<double>;
  m_ptcone40_iso98_sumEt->push_back( 3.5 );
  m_ptcone40_iso98_sumEt->push_back( 3.6 );
  m_ptcone40_iso98_sumEt->push_back( 3.5 );
  m_ptcone40_iso98_sumEt->push_back( 3.55 );
  m_ptcone40_iso98_sumEt->push_back( -100 );
  m_ptcone40_iso98_sumEt->push_back( 3.85 );
  m_ptcone40_iso98_sumEt->push_back( 4 );
  m_ptcone40_iso98_sumEt->push_back( 3.3 );
  m_ptcone40_iso98_sumEt->push_back( 3.05 );  
}

void EisoTool::Init_Ptcone40_iso99_sumEt()
{
  m_ptcone40_iso99_sumEt = new std::vector<double>;
  m_ptcone40_iso99_sumEt->push_back( 5.35 );
  m_ptcone40_iso99_sumEt->push_back( 5.35 );
  m_ptcone40_iso99_sumEt->push_back( 5.25 );
  m_ptcone40_iso99_sumEt->push_back( 5.6 );
  m_ptcone40_iso99_sumEt->push_back( -100 );
  m_ptcone40_iso99_sumEt->push_back( 6.15 );
  m_ptcone40_iso99_sumEt->push_back( 6.65 );
  m_ptcone40_iso99_sumEt->push_back( 5 );
  m_ptcone40_iso99_sumEt->push_back( 5.15 );  
}


void EisoTool::Init_Etcone20DivEt_iso90_rel17_sumEt()
{
  m_etcone20DivEt_iso90_rel17_sumEt = new std::vector<double>;
  m_etcone20DivEt_iso90_rel17_sumEt->push_back( 0.045 );
  m_etcone20DivEt_iso90_rel17_sumEt->push_back( 0.0475 );
  m_etcone20DivEt_iso90_rel17_sumEt->push_back( 0.055 );
  m_etcone20DivEt_iso90_rel17_sumEt->push_back( 0.065 );
  m_etcone20DivEt_iso90_rel17_sumEt->push_back( -100 );
  m_etcone20DivEt_iso90_rel17_sumEt->push_back( 0.0475 );
  m_etcone20DivEt_iso90_rel17_sumEt->push_back( 0.0525 );
  m_etcone20DivEt_iso90_rel17_sumEt->push_back( 0.0425 );
  m_etcone20DivEt_iso90_rel17_sumEt->push_back( 0.045 );  
}

void EisoTool::Init_Etcone20DivEt_iso95_rel17_sumEt()
{
  m_etcone20DivEt_iso95_rel17_sumEt = new std::vector<double>;
  m_etcone20DivEt_iso95_rel17_sumEt->push_back( 0.065 );
  m_etcone20DivEt_iso95_rel17_sumEt->push_back( 0.0675 );
  m_etcone20DivEt_iso95_rel17_sumEt->push_back( 0.075 );
  m_etcone20DivEt_iso95_rel17_sumEt->push_back( 0.0875 );
  m_etcone20DivEt_iso95_rel17_sumEt->push_back( -100 );
  m_etcone20DivEt_iso95_rel17_sumEt->push_back( 0.07 );
  m_etcone20DivEt_iso95_rel17_sumEt->push_back( 0.0725 );
  m_etcone20DivEt_iso95_rel17_sumEt->push_back( 0.0625 );
  m_etcone20DivEt_iso95_rel17_sumEt->push_back( 0.06 ); 
}

void EisoTool::Init_Etcone20DivEt_iso98_rel17_sumEt()
{
  m_etcone20DivEt_iso98_rel17_sumEt = new std::vector<double>;
  m_etcone20DivEt_iso98_rel17_sumEt->push_back( 0.0975 );
  m_etcone20DivEt_iso98_rel17_sumEt->push_back( 0.1 );
  m_etcone20DivEt_iso98_rel17_sumEt->push_back( 0.1075 );
  m_etcone20DivEt_iso98_rel17_sumEt->push_back( 0.12 );
  m_etcone20DivEt_iso98_rel17_sumEt->push_back( -100 );
  m_etcone20DivEt_iso98_rel17_sumEt->push_back( 0.1 );
  m_etcone20DivEt_iso98_rel17_sumEt->push_back( 0.1025 );
  m_etcone20DivEt_iso98_rel17_sumEt->push_back( 0.09 );
  m_etcone20DivEt_iso98_rel17_sumEt->push_back( 0.085 );  
}


void EisoTool::Init_Etcone30DivEt_iso90_rel17_sumEt()
{
  m_etcone30DivEt_iso90_rel17_sumEt = new std::vector<double>;
  m_etcone30DivEt_iso90_rel17_sumEt->push_back( 0.0725 );
  m_etcone30DivEt_iso90_rel17_sumEt->push_back( 0.0725 );
  m_etcone30DivEt_iso90_rel17_sumEt->push_back( 0.08 );
  m_etcone30DivEt_iso90_rel17_sumEt->push_back( 0.09 );
  m_etcone30DivEt_iso90_rel17_sumEt->push_back( -100 );
  m_etcone30DivEt_iso90_rel17_sumEt->push_back( 0.0725 );
  m_etcone30DivEt_iso90_rel17_sumEt->push_back( 0.08 );
  m_etcone30DivEt_iso90_rel17_sumEt->push_back( 0.0725 );
  m_etcone30DivEt_iso90_rel17_sumEt->push_back( 0.0725 );  
}

void EisoTool::Init_Etcone30DivEt_iso95_rel17_sumEt()
{
  m_etcone30DivEt_iso95_rel17_sumEt = new std::vector<double>;
  m_etcone30DivEt_iso95_rel17_sumEt->push_back( 0.105 );
  m_etcone30DivEt_iso95_rel17_sumEt->push_back( 0.105 );
  m_etcone30DivEt_iso95_rel17_sumEt->push_back( 0.11 );
  m_etcone30DivEt_iso95_rel17_sumEt->push_back( 0.1225 );
  m_etcone30DivEt_iso95_rel17_sumEt->push_back( -100 );
  m_etcone30DivEt_iso95_rel17_sumEt->push_back( 0.105 );
  m_etcone30DivEt_iso95_rel17_sumEt->push_back( 0.1125 );
  m_etcone30DivEt_iso95_rel17_sumEt->push_back( 0.105 );
  m_etcone30DivEt_iso95_rel17_sumEt->push_back( 0.105 );  
}

void EisoTool::Init_Etcone30DivEt_iso98_rel17_sumEt()
{
  m_etcone30DivEt_iso98_rel17_sumEt = new std::vector<double>;
  m_etcone30DivEt_iso98_rel17_sumEt->push_back( 0.1575 );
  m_etcone30DivEt_iso98_rel17_sumEt->push_back( 0.155 );
  m_etcone30DivEt_iso98_rel17_sumEt->push_back( 0.1575 );
  m_etcone30DivEt_iso98_rel17_sumEt->push_back( 0.17 );
  m_etcone30DivEt_iso98_rel17_sumEt->push_back( -100 );
  m_etcone30DivEt_iso98_rel17_sumEt->push_back( 0.1525 );
  m_etcone30DivEt_iso98_rel17_sumEt->push_back( 0.165 );
  m_etcone30DivEt_iso98_rel17_sumEt->push_back( 0.155 );
  m_etcone30DivEt_iso98_rel17_sumEt->push_back( 0.16 ); 
}


void EisoTool::Init_Ptcone20DivEt_iso90_sumEt()
{
  m_ptcone20DivEt_iso90_sumEt = new std::vector<double>;
  m_ptcone20DivEt_iso90_sumEt->push_back( 0.0025 );
  m_ptcone20DivEt_iso90_sumEt->push_back( 0.0025 );
  m_ptcone20DivEt_iso90_sumEt->push_back( 0.0025 );
  m_ptcone20DivEt_iso90_sumEt->push_back( 0.0025 );
  m_ptcone20DivEt_iso90_sumEt->push_back( -100 );
  m_ptcone20DivEt_iso90_sumEt->push_back( 0.0025 );
  m_ptcone20DivEt_iso90_sumEt->push_back( 0.0025 );
  m_ptcone20DivEt_iso90_sumEt->push_back( 0.0025 );
  m_ptcone20DivEt_iso90_sumEt->push_back( 0.0025 );  
}

void EisoTool::Init_Ptcone20DivEt_iso97_sumEt()
{
  m_ptcone20DivEt_iso97_sumEt = new std::vector<double>;
  m_ptcone20DivEt_iso97_sumEt->push_back( 0.0275 );
  m_ptcone20DivEt_iso97_sumEt->push_back( 0.03 );
  m_ptcone20DivEt_iso97_sumEt->push_back( 0.03 );
  m_ptcone20DivEt_iso97_sumEt->push_back( 0.0325 );
  m_ptcone20DivEt_iso97_sumEt->push_back( -100 );
  m_ptcone20DivEt_iso97_sumEt->push_back( 0.035 );
  m_ptcone20DivEt_iso97_sumEt->push_back( 0.0375 );
  m_ptcone20DivEt_iso97_sumEt->push_back( 0.035 );
  m_ptcone20DivEt_iso97_sumEt->push_back( 0.0375 );  
}

void EisoTool::Init_Ptcone20DivEt_iso98_sumEt()
{
  m_ptcone20DivEt_iso98_sumEt = new std::vector<double>;
  m_ptcone20DivEt_iso98_sumEt->push_back( 0.04 );
  m_ptcone20DivEt_iso98_sumEt->push_back( 0.0425 );
  m_ptcone20DivEt_iso98_sumEt->push_back( 0.0425 );
  m_ptcone20DivEt_iso98_sumEt->push_back( 0.045 );
  m_ptcone20DivEt_iso98_sumEt->push_back( -100 );
  m_ptcone20DivEt_iso98_sumEt->push_back( 0.0525 );
  m_ptcone20DivEt_iso98_sumEt->push_back( 0.055 );
  m_ptcone20DivEt_iso98_sumEt->push_back( 0.05 );
  m_ptcone20DivEt_iso98_sumEt->push_back( 0.0575 );  
}

void EisoTool::Init_Ptcone20DivEt_iso99_sumEt()
{
  m_ptcone20DivEt_iso99_sumEt = new std::vector<double>;
  m_ptcone20DivEt_iso99_sumEt->push_back( 0.0625 );
  m_ptcone20DivEt_iso99_sumEt->push_back( 0.0675 );
  m_ptcone20DivEt_iso99_sumEt->push_back( 0.07 );
  m_ptcone20DivEt_iso99_sumEt->push_back( 0.0775 );
  m_ptcone20DivEt_iso99_sumEt->push_back( -100 );
  m_ptcone20DivEt_iso99_sumEt->push_back( 0.095 );
  m_ptcone20DivEt_iso99_sumEt->push_back( 0.11 );
  m_ptcone20DivEt_iso99_sumEt->push_back( 0.09 );
  m_ptcone20DivEt_iso99_sumEt->push_back( 0.115 ); 
}

void EisoTool::Init_Ptcone30DivEt_iso90_sumEt()
{
  m_ptcone30DivEt_iso90_sumEt = new std::vector<double>;
  m_ptcone30DivEt_iso90_sumEt->push_back( 0.0025 );
  m_ptcone30DivEt_iso90_sumEt->push_back( 0.0025 );
  m_ptcone30DivEt_iso90_sumEt->push_back( 0.0025 );
  m_ptcone30DivEt_iso90_sumEt->push_back( 0.0025 );
  m_ptcone30DivEt_iso90_sumEt->push_back( -100 );
  m_ptcone30DivEt_iso90_sumEt->push_back( 0.0025 );
  m_ptcone30DivEt_iso90_sumEt->push_back( 0.0025 );
  m_ptcone30DivEt_iso90_sumEt->push_back( 0.0025 );
  m_ptcone30DivEt_iso90_sumEt->push_back( 0.0025 );  
}

void EisoTool::Init_Ptcone30DivEt_iso97_sumEt()
{
  m_ptcone30DivEt_iso97_sumEt = new std::vector<double>;
  m_ptcone30DivEt_iso97_sumEt->push_back( 0.0525 );
  m_ptcone30DivEt_iso97_sumEt->push_back( 0.055 );
  m_ptcone30DivEt_iso97_sumEt->push_back( 0.0525 );
  m_ptcone30DivEt_iso97_sumEt->push_back( 0.055 );
  m_ptcone30DivEt_iso97_sumEt->push_back( -100 );
  m_ptcone30DivEt_iso97_sumEt->push_back( 0.06 );
  m_ptcone30DivEt_iso97_sumEt->push_back( 0.065 );
  m_ptcone30DivEt_iso97_sumEt->push_back( 0.0575 );
  m_ptcone30DivEt_iso97_sumEt->push_back( 0.0575 ); 
}

void EisoTool::Init_Ptcone30DivEt_iso98_sumEt()
{
  m_ptcone30DivEt_iso98_sumEt = new std::vector<double>;
  m_ptcone30DivEt_iso98_sumEt->push_back( 0.0675 );
  m_ptcone30DivEt_iso98_sumEt->push_back( 0.0725 );
  m_ptcone30DivEt_iso98_sumEt->push_back( 0.07 );
  m_ptcone30DivEt_iso98_sumEt->push_back( 0.0725 );
  m_ptcone30DivEt_iso98_sumEt->push_back( -100 );
  m_ptcone30DivEt_iso98_sumEt->push_back( 0.0825 );
  m_ptcone30DivEt_iso98_sumEt->push_back( 0.0875 );
  m_ptcone30DivEt_iso98_sumEt->push_back( 0.08 );
  m_ptcone30DivEt_iso98_sumEt->push_back( 0.0775 );  
}

void EisoTool::Init_Ptcone30DivEt_iso99_sumEt()
{
  m_ptcone30DivEt_iso99_sumEt = new std::vector<double>;
  m_ptcone30DivEt_iso99_sumEt->push_back( 0.105 );
  m_ptcone30DivEt_iso99_sumEt->push_back( 0.11 );
  m_ptcone30DivEt_iso99_sumEt->push_back( 0.11 );
  m_ptcone30DivEt_iso99_sumEt->push_back( 0.1225 );
  m_ptcone30DivEt_iso99_sumEt->push_back( -100 );
  m_ptcone30DivEt_iso99_sumEt->push_back( 0.1375 );
  m_ptcone30DivEt_iso99_sumEt->push_back( 0.155 );
  m_ptcone30DivEt_iso99_sumEt->push_back( 0.1275 );
  m_ptcone30DivEt_iso99_sumEt->push_back( 0.14 );  
}


void EisoTool::Init_Ptcone40DivEt_iso90_sumEt()
{
  m_ptcone40DivEt_iso90_sumEt = new std::vector<double>;
  m_ptcone40DivEt_iso90_sumEt->push_back( 0.03 );
  m_ptcone40DivEt_iso90_sumEt->push_back( 0.0325 );
  m_ptcone40DivEt_iso90_sumEt->push_back( 0.0325 );
  m_ptcone40DivEt_iso90_sumEt->push_back( 0.03 );
  m_ptcone40DivEt_iso90_sumEt->push_back( -100 );
  m_ptcone40DivEt_iso90_sumEt->push_back( 0.0325 );
  m_ptcone40DivEt_iso90_sumEt->push_back( 0.0325 );
  m_ptcone40DivEt_iso90_sumEt->push_back( 0.0275 );
  m_ptcone40DivEt_iso90_sumEt->push_back( 0.0025 );  
}

void EisoTool::Init_Ptcone40DivEt_iso97_sumEt()
{
  m_ptcone40DivEt_iso97_sumEt = new std::vector<double>;
  m_ptcone40DivEt_iso97_sumEt->push_back( 0.0775 );
  m_ptcone40DivEt_iso97_sumEt->push_back( 0.0825 );
  m_ptcone40DivEt_iso97_sumEt->push_back( 0.0825 );
  m_ptcone40DivEt_iso97_sumEt->push_back( 0.0825 );
  m_ptcone40DivEt_iso97_sumEt->push_back( -100 );
  m_ptcone40DivEt_iso97_sumEt->push_back( 0.09 );
  m_ptcone40DivEt_iso97_sumEt->push_back( 0.0925 );
  m_ptcone40DivEt_iso97_sumEt->push_back( 0.0825 );
  m_ptcone40DivEt_iso97_sumEt->push_back( 0.075 );  
}

void EisoTool::Init_Ptcone40DivEt_iso98_sumEt()
{
  m_ptcone40DivEt_iso98_sumEt = new std::vector<double>;
  m_ptcone40DivEt_iso98_sumEt->push_back( 0.105 );
  m_ptcone40DivEt_iso98_sumEt->push_back( 0.1075 );
  m_ptcone40DivEt_iso98_sumEt->push_back( 0.1075 );
  m_ptcone40DivEt_iso98_sumEt->push_back( 0.1075 );
  m_ptcone40DivEt_iso98_sumEt->push_back( -100 );
  m_ptcone40DivEt_iso98_sumEt->push_back( 0.1225 );
  m_ptcone40DivEt_iso98_sumEt->push_back( 0.13 );
  m_ptcone40DivEt_iso98_sumEt->push_back( 0.11 );
  m_ptcone40DivEt_iso98_sumEt->push_back( 0.11 );  
}

void EisoTool::Init_Ptcone40DivEt_iso99_sumEt()
{
  m_ptcone40DivEt_iso99_sumEt = new std::vector<double>;
  m_ptcone40DivEt_iso99_sumEt->push_back( 0.17 );
  m_ptcone40DivEt_iso99_sumEt->push_back( 0.1775 );
  m_ptcone40DivEt_iso99_sumEt->push_back( 0.1725 );
  m_ptcone40DivEt_iso99_sumEt->push_back( 0.185 );
  m_ptcone40DivEt_iso99_sumEt->push_back( -100 );
  m_ptcone40DivEt_iso99_sumEt->push_back( 0.205 );
  m_ptcone40DivEt_iso99_sumEt->push_back( 0.2175 );
  m_ptcone40DivEt_iso99_sumEt->push_back( 0.1775 );
  m_ptcone40DivEt_iso99_sumEt->push_back( 0.1875 );  
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////   Etcone 20 Isolation Functions ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////
//// Uniform 90% Etcone20 isolation efficiency  ////
////////////////////////////////////////////////////
unsigned int EisoTool::Eiso90Etcone20(const unsigned int& NPV,
                                      const double& Electron_Cluster_E,
                                      const double& Electron_Track_Eta,
                                      const double& Electron_Cluster_Eta,
                                      const double& Electron_Etas2,
                                      const double& Electron_Etap,
                                      const double& Electron_Etcone20_Uncorrected,
                                      const bool& isMC)
{
  m_isMC = isMC;
  return this->Eiso90Etcone20( NPV , Electron_Cluster_E , Electron_Track_Eta ,
                              Electron_Cluster_Eta , Electron_Etas2 , Electron_Etap , Electron_Etcone20_Uncorrected );
}


unsigned int EisoTool::Eiso90Etcone20(const unsigned int& NPV,
                                      const double& Electron_Cluster_E,
                                      const double& Electron_Track_Eta,
                                      const double& Electron_Cluster_Eta,
                                      const double& Electron_Etas2,
                                      const double& Electron_Etap,
                                      const double& Electron_Etcone20_Uncorrected) const
{
  unsigned int fail0_pass1(0);
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  double Etcone20Cor = this->Etcone20Corrected( NPV , Electron_Cluster_E ,
                                                Electron_Cluster_Eta , Electron_Etas2 ,
                                                Electron_Etap , Electron_Etcone20_Uncorrected );

  unsigned int EtBin = this->FindEtBin(Et);
  unsigned int EtaBin = this->FindEtaBin( fabs( Electron_Cluster_Eta ) );

  double cutVal(-999.9);
  if( !m_rel16False_rel17True ){
    cutVal = m_GeV * this->CheckValidAndReturn( m_etcone20_iso90 , EtBin , EtaBin );
  }
  if( m_rel16False_rel17True ){
    if( m_sumEtFalse_isEMTrue ){
      cutVal = m_GeV * this->CheckValidAndReturn( m_etcone20_iso90_rel17 , EtBin , EtaBin );
    }
    if( !m_sumEtFalse_isEMTrue ){
      cutVal = m_GeV * this->CheckValidAndReturn( m_etcone20_iso90_rel17_sumEt , EtaBin );
    }
  }    
  
  if( Etcone20Cor < cutVal ){
    fail0_pass1 = 1;
  }
  return fail0_pass1;
}

////////////////////////////////////////////////////
//// Uniform 95% Etcone20 isolation efficiency  ////
////////////////////////////////////////////////////
unsigned int EisoTool::Eiso95Etcone20(const unsigned int& NPV,
                                      const double& Electron_Cluster_E,
                                      const double& Electron_Track_Eta,
                                      const double& Electron_Cluster_Eta,
                                      const double& Electron_Etas2,
                                      const double& Electron_Etap,
                                      const double& Electron_Etcone20_Uncorrected,
                                      const bool& isMC)
{
  m_isMC = isMC;
  return this->Eiso95Etcone20( NPV , Electron_Cluster_E , Electron_Track_Eta ,
                              Electron_Cluster_Eta , Electron_Etas2 , Electron_Etap , Electron_Etcone20_Uncorrected );
}

unsigned int EisoTool::Eiso95Etcone20(const unsigned int& NPV,
                                      const double& Electron_Cluster_E,
                                      const double& Electron_Track_Eta,
                                      const double& Electron_Cluster_Eta,
                                      const double& Electron_Etas2,
                                      const double& Electron_Etap,
                                      const double& Electron_Etcone20_Uncorrected) const
{
  unsigned int fail0_pass1(0);
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  double Etcone20Cor = this->Etcone20Corrected( NPV , Electron_Cluster_E ,
                                                Electron_Cluster_Eta , Electron_Etas2 ,
                                                Electron_Etap , Electron_Etcone20_Uncorrected );

  unsigned int EtBin = this->FindEtBin(Et);
  unsigned int EtaBin = this->FindEtaBin( fabs( Electron_Cluster_Eta ) );

  double cutVal(-999.9);
  if( !m_rel16False_rel17True ){
    cutVal = m_GeV * this->CheckValidAndReturn( m_etcone20_iso95 , EtBin , EtaBin );
  }
  if( m_rel16False_rel17True ){
    if( m_sumEtFalse_isEMTrue ){
      cutVal = m_GeV * this->CheckValidAndReturn( m_etcone20_iso95_rel17 , EtBin , EtaBin );
    }
    if( !m_sumEtFalse_isEMTrue ){
      cutVal = m_GeV * this->CheckValidAndReturn( m_etcone20_iso95_rel17_sumEt , EtaBin );
    }    
  }   

  if( Etcone20Cor < cutVal ){
    fail0_pass1 = 1;
  }
  return fail0_pass1;
}

////////////////////////////////////////////////////
//// Uniform 98% Etcone20 isolation efficiency  ////
////////////////////////////////////////////////////
unsigned int EisoTool::Eiso98Etcone20(const unsigned int& NPV,
                                      const double& Electron_Cluster_E,
                                      const double& Electron_Track_Eta,
                                      const double& Electron_Cluster_Eta,
                                      const double& Electron_Etas2,
                                      const double& Electron_Etap,
                                      const double& Electron_Etcone20_Uncorrected,
                                      const bool& isMC)
{
  m_isMC = isMC;
  return this->Eiso98Etcone20( NPV , Electron_Cluster_E , Electron_Track_Eta ,
                              Electron_Cluster_Eta , Electron_Etas2 , Electron_Etap , Electron_Etcone20_Uncorrected );
}

unsigned int EisoTool::Eiso98Etcone20(const unsigned int& NPV,
                                      const double& Electron_Cluster_E,
                                      const double& Electron_Track_Eta,
                                      const double& Electron_Cluster_Eta,
                                      const double& Electron_Etas2,
                                      const double& Electron_Etap,
                                      const double& Electron_Etcone20_Uncorrected) const
{
  unsigned int fail0_pass1(0);
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  double Etcone20Cor = this->Etcone20Corrected( NPV , Electron_Cluster_E ,
                                                Electron_Cluster_Eta , Electron_Etas2 ,
                                                Electron_Etap , Electron_Etcone20_Uncorrected );

  unsigned int EtBin = this->FindEtBin(Et);
  unsigned int EtaBin = this->FindEtaBin( fabs( Electron_Cluster_Eta ) );

  double cutVal(-999.9);
  if( !m_rel16False_rel17True ){
    cutVal = m_GeV * this->CheckValidAndReturn( m_etcone20_iso98 , EtBin , EtaBin );
  }
  if( m_rel16False_rel17True ){
    if( m_sumEtFalse_isEMTrue ){
      cutVal = m_GeV * this->CheckValidAndReturn( m_etcone20_iso98_rel17 , EtBin , EtaBin );
    }
    if( !m_sumEtFalse_isEMTrue ){
      cutVal = m_GeV * this->CheckValidAndReturn( m_etcone20_iso98_rel17_sumEt , EtaBin );
    }    
  }   

  if( Etcone20Cor < cutVal ){
    fail0_pass1 = 1;
  }
  return fail0_pass1;
}


double EisoTool::getCutValueEiso90Etcone20(const double& Electron_Cluster_E,
                                           const double& Electron_Track_Eta,
                                           const double& Electron_Cluster_Eta) const
{
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );

  unsigned int EtBin = this->FindEtBin(Et);
  unsigned int EtaBin = this->FindEtaBin( fabs( Electron_Cluster_Eta ) );

  double cutVal(-999.9);
  if( !m_rel16False_rel17True ){
    cutVal = m_GeV * this->CheckValidAndReturn( m_etcone20_iso90 , EtBin , EtaBin );
  }
  if( m_rel16False_rel17True ){
    if( m_sumEtFalse_isEMTrue ){
      cutVal = m_GeV * this->CheckValidAndReturn( m_etcone20_iso90_rel17 , EtBin , EtaBin );
    }
    if( !m_sumEtFalse_isEMTrue ){
      cutVal = m_GeV * this->CheckValidAndReturn( m_etcone20_iso90_rel17_sumEt , EtaBin );
    }    
  }     

  return cutVal;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////   Etcone 30 Isolation Functions ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////
//// Uniform 90% Etcone30 isolation efficiency  ////
////////////////////////////////////////////////////
unsigned int EisoTool::Eiso90Etcone30(const unsigned int& NPV,
                                      const double& Electron_Cluster_E,
                                      const double& Electron_Track_Eta,
                                      const double& Electron_Cluster_Eta,
                                      const double& Electron_Etas2,
                                      const double& Electron_Etap,
                                      const double& Electron_Etcone30_Uncorrected,
                                      const bool& isMC)
{
  m_isMC = isMC;
  return this->Eiso90Etcone30( NPV , Electron_Cluster_E , Electron_Track_Eta ,
                              Electron_Cluster_Eta , Electron_Etas2 , Electron_Etap , Electron_Etcone30_Uncorrected );
}


unsigned int EisoTool::Eiso90Etcone30(const unsigned int& NPV,
                                      const double& Electron_Cluster_E,
                                      const double& Electron_Track_Eta,
                                      const double& Electron_Cluster_Eta,
                                      const double& Electron_Etas2,
                                      const double& Electron_Etap,
                                      const double& Electron_Etcone30_Uncorrected) const
{
  unsigned int fail0_pass1(0);
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  double Etcone30Cor = this->Etcone30Corrected( NPV , Electron_Cluster_E ,
                                                Electron_Cluster_Eta , Electron_Etas2 ,
                                                Electron_Etap , Electron_Etcone30_Uncorrected );

  unsigned int EtBin = this->FindEtBin(Et);
  unsigned int EtaBin = this->FindEtaBin( fabs( Electron_Cluster_Eta ) );

  double cutVal(-999.9);
  if( !m_rel16False_rel17True ){
    cutVal = m_GeV * this->CheckValidAndReturn( m_etcone30_iso90 , EtBin , EtaBin );
  }
  if( m_rel16False_rel17True ){
    if( m_sumEtFalse_isEMTrue ){
      cutVal = m_GeV * this->CheckValidAndReturn( m_etcone30_iso90_rel17 , EtBin , EtaBin );
    }
    if( !m_sumEtFalse_isEMTrue ){
      cutVal = m_GeV * this->CheckValidAndReturn( m_etcone30_iso90_rel17_sumEt , EtaBin );
    }    
  }   

  if( Etcone30Cor < cutVal ){
    fail0_pass1 = 1;
  }
  return fail0_pass1;                                               
}

////////////////////////////////////////////////////
//// Uniform 95% Etcone30 isolation efficiency  ////
////////////////////////////////////////////////////
unsigned int EisoTool::Eiso95Etcone30(const unsigned int& NPV,
                                      const double& Electron_Cluster_E,
                                      const double& Electron_Track_Eta,
                                      const double& Electron_Cluster_Eta,
                                      const double& Electron_Etas2,
                                      const double& Electron_Etap,
                                      const double& Electron_Etcone30_Uncorrected,
                                      const bool& isMC)
{
  m_isMC = isMC;
  return this->Eiso95Etcone30( NPV , Electron_Cluster_E , Electron_Track_Eta ,
                              Electron_Cluster_Eta , Electron_Etas2 , Electron_Etap , Electron_Etcone30_Uncorrected );
}

unsigned int EisoTool::Eiso95Etcone30(const unsigned int& NPV,
                                      const double& Electron_Cluster_E,
                                      const double& Electron_Track_Eta,
                                      const double& Electron_Cluster_Eta,
                                      const double& Electron_Etas2,
                                      const double& Electron_Etap,
                                      const double& Electron_Etcone30_Uncorrected) const
{
  unsigned int fail0_pass1(0);
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  double Etcone30Cor = this->Etcone30Corrected( NPV , Electron_Cluster_E ,
                                                Electron_Cluster_Eta , Electron_Etas2 ,
                                                Electron_Etap , Electron_Etcone30_Uncorrected );

  unsigned int EtBin = this->FindEtBin(Et);
  unsigned int EtaBin = this->FindEtaBin( fabs( Electron_Cluster_Eta ) );

  double cutVal(-999.9);
  if( !m_rel16False_rel17True ){
    cutVal = m_GeV * this->CheckValidAndReturn( m_etcone30_iso95 , EtBin , EtaBin );
  }
  if( m_rel16False_rel17True ){
    if( m_sumEtFalse_isEMTrue ){
      cutVal = m_GeV * this->CheckValidAndReturn( m_etcone30_iso95_rel17 , EtBin , EtaBin );
    }
    if( !m_sumEtFalse_isEMTrue ){
      cutVal = m_GeV * this->CheckValidAndReturn( m_etcone30_iso95_rel17_sumEt , EtaBin );
    }    
  }   

  if( Etcone30Cor < cutVal ){
    fail0_pass1 = 1;
  }
  return fail0_pass1;
}

////////////////////////////////////////////////////
//// Uniform 98% Etcone30 isolation efficiency  ////
////////////////////////////////////////////////////
unsigned int EisoTool::Eiso98Etcone30(const unsigned int& NPV,
                                      const double& Electron_Cluster_E,
                                      const double& Electron_Track_Eta,
                                      const double& Electron_Cluster_Eta,
                                      const double& Electron_Etas2,
                                      const double& Electron_Etap,
                                      const double& Electron_Etcone30_Uncorrected,
                                      const bool& isMC)
{
  m_isMC = isMC;
  return this->Eiso98Etcone30( NPV , Electron_Cluster_E , Electron_Track_Eta ,
                              Electron_Cluster_Eta , Electron_Etas2 , Electron_Etap , Electron_Etcone30_Uncorrected );
}

unsigned int EisoTool::Eiso98Etcone30(const unsigned int& NPV,
                                      const double& Electron_Cluster_E,
                                      const double& Electron_Track_Eta,
                                      const double& Electron_Cluster_Eta,
                                      const double& Electron_Etas2,
                                      const double& Electron_Etap,
                                      const double& Electron_Etcone30_Uncorrected) const
{
  unsigned int fail0_pass1(0);
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  double Etcone30Cor = this->Etcone30Corrected( NPV , Electron_Cluster_E ,
                                                Electron_Cluster_Eta , Electron_Etas2 ,
                                                Electron_Etap , Electron_Etcone30_Uncorrected );

  unsigned int EtBin = this->FindEtBin(Et);
  unsigned int EtaBin = this->FindEtaBin( fabs( Electron_Cluster_Eta ) );

  double cutVal(-999.9);
  if( !m_rel16False_rel17True ){
    cutVal = m_GeV * this->CheckValidAndReturn( m_etcone30_iso98 , EtBin , EtaBin );
  }
  if( m_rel16False_rel17True ){
    if( m_sumEtFalse_isEMTrue ){
      cutVal = m_GeV * this->CheckValidAndReturn( m_etcone30_iso98_rel17 , EtBin , EtaBin );
    }
    if( !m_sumEtFalse_isEMTrue ){
      cutVal = m_GeV * this->CheckValidAndReturn( m_etcone30_iso98_rel17_sumEt , EtaBin );
    }    
  }   

  if( Etcone30Cor < cutVal ){
    fail0_pass1 = 1;
  }
  return fail0_pass1;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////   Ptcone 20 Isolation Functions ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////
//// Uniform 90% Ptcone20 isolation efficiency  ////
////////////////////////////////////////////////////
unsigned int EisoTool::Eiso90Ptcone20(const double& Electron_Cluster_E,
                                      const double& Electron_Track_Eta,
                                      const double& Electron_Cluster_Eta,
                                      const double& Ptcone20) const
{
  unsigned int fail0_pass1(0);
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  unsigned int EtBin = this->FindEtBin(Et);
  unsigned int EtaBin = this->FindEtaBin( fabs( Electron_Cluster_Eta ) );

  double cutVal(-999.9);
  if( m_sumEtFalse_isEMTrue ){
    cutVal = m_GeV * this->CheckValidAndReturn( m_ptcone20_iso90 , EtBin , EtaBin );
  }
  if( !m_sumEtFalse_isEMTrue ){
    cutVal = m_GeV * this->CheckValidAndReturn( m_ptcone20_iso90_sumEt , EtaBin );
  }
  if( Ptcone20 < cutVal ){
    fail0_pass1 = 1;
  }
  return fail0_pass1;
}

////////////////////////////////////////////////////
//// Uniform 97% Ptcone20 isolation efficiency  ////
////////////////////////////////////////////////////
unsigned int EisoTool::Eiso97Ptcone20(const double& Electron_Cluster_E,
                                      const double& Electron_Track_Eta,
                                      const double& Electron_Cluster_Eta,
                                      const double& Ptcone20) const
{
  unsigned int fail0_pass1(0);
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  unsigned int EtBin = this->FindEtBin(Et);
  unsigned int EtaBin = this->FindEtaBin( fabs( Electron_Cluster_Eta ) );

  double cutVal(-999.9);
  if( m_sumEtFalse_isEMTrue ){
    cutVal = m_GeV * this->CheckValidAndReturn( m_ptcone20_iso97 , EtBin , EtaBin );
  }
  if( !m_sumEtFalse_isEMTrue ){
    cutVal = m_GeV * this->CheckValidAndReturn( m_ptcone20_iso97_sumEt , EtaBin );
  } 
  if( Ptcone20 < cutVal ){
    fail0_pass1 = 1;
  }
  return fail0_pass1;  
}

////////////////////////////////////////////////////
//// Uniform 98% Ptcone20 isolation efficiency  ////
////////////////////////////////////////////////////
unsigned int EisoTool::Eiso98Ptcone20(const double& Electron_Cluster_E,
                                      const double& Electron_Track_Eta,
                                      const double& Electron_Cluster_Eta,
                                      const double& Ptcone20) const
{
  unsigned int fail0_pass1(0);
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  unsigned int EtBin = this->FindEtBin(Et);
  unsigned int EtaBin = this->FindEtaBin( fabs( Electron_Cluster_Eta ) );
  double cutVal(-999.9);
  if( m_sumEtFalse_isEMTrue ){
    cutVal = m_GeV * this->CheckValidAndReturn( m_ptcone20_iso98 , EtBin , EtaBin );
  }
  if( !m_sumEtFalse_isEMTrue ){
    cutVal = m_GeV * this->CheckValidAndReturn( m_ptcone20_iso98_sumEt , EtaBin );
  }
  if( Ptcone20 < cutVal ){
    fail0_pass1 = 1;
  }
  return fail0_pass1;
}

////////////////////////////////////////////////////
//// Uniform 99% Ptcone20 isolation efficiency  ////
////////////////////////////////////////////////////
unsigned int EisoTool::Eiso99Ptcone20(const double& Electron_Cluster_E,
                                      const double& Electron_Track_Eta,
                                      const double& Electron_Cluster_Eta,
                                      const double& Ptcone20) const
{
  unsigned int fail0_pass1(0);
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  unsigned int EtBin = this->FindEtBin(Et);
  unsigned int EtaBin = this->FindEtaBin( fabs( Electron_Cluster_Eta ) );
  double cutVal(-999.9);
  if( m_sumEtFalse_isEMTrue ){
    cutVal = m_GeV * this->CheckValidAndReturn( m_ptcone20_iso99 , EtBin , EtaBin );
  }
  if( !m_sumEtFalse_isEMTrue ){
    cutVal = m_GeV * this->CheckValidAndReturn( m_ptcone20_iso99_sumEt , EtaBin );
  }
  if( Ptcone20 < cutVal ){
    fail0_pass1 = 1;
  }
  return fail0_pass1;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////   Ptcone 30 Isolation Functions ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////
//// Uniform 90% Ptcone30 isolation efficiency  ////
////////////////////////////////////////////////////
unsigned int EisoTool::Eiso90Ptcone30(const double& Electron_Cluster_E,
                                      const double& Electron_Track_Eta,
                                      const double& Electron_Cluster_Eta,
                                      const double& Ptcone30) const
{
  unsigned int fail0_pass1(0);
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  unsigned int EtBin = this->FindEtBin(Et);
  unsigned int EtaBin = this->FindEtaBin( fabs( Electron_Cluster_Eta ) );
  double cutVal(-999.9);
  if( m_sumEtFalse_isEMTrue ){
    cutVal = m_GeV * this->CheckValidAndReturn( m_ptcone30_iso90 , EtBin , EtaBin );
  }
  if( !m_sumEtFalse_isEMTrue ){
    cutVal = m_GeV * this->CheckValidAndReturn( m_ptcone30_iso90_sumEt , EtaBin );
  }
  if( Ptcone30 < cutVal ){
    fail0_pass1 = 1;
  }
  return fail0_pass1;
}

double EisoTool::getCutValueEiso90Ptcone30(const double& Electron_Cluster_E,
                                           const double& Electron_Track_Eta,
                                           const double& Electron_Cluster_Eta) const
{
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  unsigned int EtBin = this->FindEtBin(Et);
  unsigned int EtaBin = this->FindEtaBin( fabs( Electron_Cluster_Eta ) );
  double cutVal(-999.9);
  if( m_sumEtFalse_isEMTrue ){
    cutVal = m_GeV * this->CheckValidAndReturn( m_ptcone30_iso90 , EtBin , EtaBin );
  }
  if( !m_sumEtFalse_isEMTrue ){
    cutVal = m_GeV * this->CheckValidAndReturn( m_ptcone30_iso90_sumEt , EtaBin );
  }
  return cutVal;
}

////////////////////////////////////////////////////
//// Uniform 97% Ptcone30 isolation efficiency  ////
////////////////////////////////////////////////////
unsigned int EisoTool::Eiso97Ptcone30(const double& Electron_Cluster_E,
                                      const double& Electron_Track_Eta,
                                      const double& Electron_Cluster_Eta,
                                      const double& Ptcone30) const
{
  unsigned int fail0_pass1(0);
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  unsigned int EtBin = this->FindEtBin(Et);
  unsigned int EtaBin = this->FindEtaBin( fabs( Electron_Cluster_Eta ) );
  double cutVal(-999.9);
  if( m_sumEtFalse_isEMTrue ){
    cutVal = m_GeV * this->CheckValidAndReturn( m_ptcone30_iso97 , EtBin , EtaBin );
  }
  if( !m_sumEtFalse_isEMTrue ){
    cutVal = m_GeV * this->CheckValidAndReturn( m_ptcone30_iso97_sumEt , EtaBin );
  }
  if( Ptcone30 < cutVal ){
    fail0_pass1 = 1;
  }
  return fail0_pass1;
}

////////////////////////////////////////////////////
//// Uniform 98% Ptcone30 isolation efficiency  ////
////////////////////////////////////////////////////
unsigned int EisoTool::Eiso98Ptcone30(const double& Electron_Cluster_E,
                                      const double& Electron_Track_Eta,
                                      const double& Electron_Cluster_Eta,
                                      const double& Ptcone30) const
{
  unsigned int fail0_pass1(0);
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  unsigned int EtBin = this->FindEtBin(Et);
  unsigned int EtaBin = this->FindEtaBin( fabs( Electron_Cluster_Eta ) );
  double cutVal(-999.9);
  if( m_sumEtFalse_isEMTrue ){
    cutVal = m_GeV * this->CheckValidAndReturn( m_ptcone30_iso98 , EtBin , EtaBin );
  }
  if( !m_sumEtFalse_isEMTrue ){
    cutVal = m_GeV * this->CheckValidAndReturn( m_ptcone30_iso98_sumEt , EtaBin );
  }
  if( Ptcone30 < cutVal ){
    fail0_pass1 = 1;
  }
  return fail0_pass1;
}

////////////////////////////////////////////////////
//// Uniform 99% Ptcone30 isolation efficiency  ////
////////////////////////////////////////////////////
unsigned int EisoTool::Eiso99Ptcone30(const double& Electron_Cluster_E,
                                      const double& Electron_Track_Eta,
                                      const double& Electron_Cluster_Eta,
                                      const double& Ptcone30) const
{
  unsigned int fail0_pass1(0);
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  unsigned int EtBin = this->FindEtBin(Et);
  unsigned int EtaBin = this->FindEtaBin( fabs( Electron_Cluster_Eta ) );
  double cutVal(-999.9);
  if( m_sumEtFalse_isEMTrue ){
    cutVal = m_GeV * this->CheckValidAndReturn( m_ptcone30_iso99 , EtBin , EtaBin );
  }
  if( !m_sumEtFalse_isEMTrue ){
    cutVal = m_GeV * this->CheckValidAndReturn( m_ptcone30_iso99_sumEt , EtaBin );
  }
  if( Ptcone30 < cutVal ){
    fail0_pass1 = 1;
  }
  return fail0_pass1;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////   Ptcone 40 Isolation Functions ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////
//// Uniform 90% Ptcone40 isolation efficiency  ////
////////////////////////////////////////////////////
unsigned int EisoTool::Eiso90Ptcone40(const double& Electron_Cluster_E,
                                      const double& Electron_Track_Eta,
                                      const double& Electron_Cluster_Eta,
                                      const double& Ptcone40) const
{
  unsigned int fail0_pass1(0);
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  unsigned int EtBin = this->FindEtBin(Et);
  unsigned int EtaBin = this->FindEtaBin( fabs( Electron_Cluster_Eta ) );
  double cutVal(-999.9);
  if( m_sumEtFalse_isEMTrue ){
    cutVal = m_GeV * this->CheckValidAndReturn( m_ptcone40_iso90 , EtBin , EtaBin );
  }
  if( !m_sumEtFalse_isEMTrue ){
    cutVal = m_GeV * this->CheckValidAndReturn( m_ptcone40_iso90_sumEt , EtaBin );
  }
  if( Ptcone40 < cutVal ){
    fail0_pass1 = 1;
  }
  return fail0_pass1;
}

////////////////////////////////////////////////////
//// Uniform 97% Ptcone40 isolation efficiency  ////
////////////////////////////////////////////////////
unsigned int EisoTool::Eiso97Ptcone40(const double& Electron_Cluster_E,
                                      const double& Electron_Track_Eta,
                                      const double& Electron_Cluster_Eta,
                                      const double& Ptcone40) const
{
  unsigned int fail0_pass1(0);
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  unsigned int EtBin = this->FindEtBin(Et);
  unsigned int EtaBin = this->FindEtaBin( fabs( Electron_Cluster_Eta ) );
  double cutVal(-999.9);
  if( m_sumEtFalse_isEMTrue ){
    cutVal = m_GeV * this->CheckValidAndReturn( m_ptcone40_iso97 , EtBin , EtaBin );
  }
  if( !m_sumEtFalse_isEMTrue ){
    cutVal = m_GeV * this->CheckValidAndReturn( m_ptcone40_iso97_sumEt , EtaBin );
  }
  if( Ptcone40 < cutVal ){
    fail0_pass1 = 1;
  }
  return fail0_pass1;
}

////////////////////////////////////////////////////
//// Uniform 98% Ptcone40 isolation efficiency  ////
////////////////////////////////////////////////////
unsigned int EisoTool::Eiso98Ptcone40(const double& Electron_Cluster_E,
                                      const double& Electron_Track_Eta,
                                      const double& Electron_Cluster_Eta,
                                      const double& Ptcone40) const
{
  unsigned int fail0_pass1(0);
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  unsigned int EtBin = this->FindEtBin(Et);
  unsigned int EtaBin = this->FindEtaBin( fabs( Electron_Cluster_Eta ) );
  double cutVal(-999.9);
  if( m_sumEtFalse_isEMTrue ){
    cutVal = m_GeV * this->CheckValidAndReturn( m_ptcone40_iso98 , EtBin , EtaBin );
  }
  if( !m_sumEtFalse_isEMTrue ){
    cutVal = m_GeV * this->CheckValidAndReturn( m_ptcone40_iso98_sumEt , EtaBin );
  }
  if( Ptcone40 < cutVal ){
    fail0_pass1 = 1;
  }
  return fail0_pass1;
}

////////////////////////////////////////////////////
//// Uniform 99% Ptcone40 isolation efficiency  ////
////////////////////////////////////////////////////
unsigned int EisoTool::Eiso99Ptcone40(const double& Electron_Cluster_E,
                                      const double& Electron_Track_Eta,
                                      const double& Electron_Cluster_Eta,
                                      const double& Ptcone40) const
{
  unsigned int fail0_pass1(0);
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  unsigned int EtBin = this->FindEtBin(Et);
  unsigned int EtaBin = this->FindEtaBin( fabs( Electron_Cluster_Eta ) );
  double cutVal(-999.9);
  if( m_sumEtFalse_isEMTrue ){
    cutVal = m_GeV * this->CheckValidAndReturn( m_ptcone40_iso99 , EtBin , EtaBin );
  }
  if( !m_sumEtFalse_isEMTrue ){
    cutVal = m_GeV * this->CheckValidAndReturn( m_ptcone40_iso99_sumEt , EtaBin );
  }
  if( Ptcone40 < cutVal ){
    fail0_pass1 = 1;
  }
  return fail0_pass1;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////   Etcone 20 / Et Isolation Functions ////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
//// Uniform 90% Etcone20 / Et isolation efficiency  ////
/////////////////////////////////////////////////////////
unsigned int EisoTool::Eiso90Etcone20DivEt(const unsigned int& NPV,
                                           const double& Electron_Cluster_E,
                                           const double& Electron_Track_Eta,
                                           const double& Electron_Cluster_Eta,
                                           const double& Electron_Etas2,
                                           const double& Electron_Etap,
                                           const double& Electron_Etcone20_Uncorrected,
                                           const bool& isMC)
{
  m_isMC = isMC;
  return this->Eiso90Etcone20DivEt( NPV , Electron_Cluster_E , Electron_Track_Eta ,
                                    Electron_Cluster_Eta , Electron_Etas2 , Electron_Etap , Electron_Etcone20_Uncorrected );
}


unsigned int EisoTool::Eiso90Etcone20DivEt(const unsigned int& NPV,
                                           const double& Electron_Cluster_E,
                                           const double& Electron_Track_Eta,
                                           const double& Electron_Cluster_Eta,
                                           const double& Electron_Etas2,
                                           const double& Electron_Etap,
                                           const double& Electron_Etcone20_Uncorrected) const
{
  unsigned int fail0_pass1(0);
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  double Etcone20Cor = this->Etcone20Corrected( NPV , Electron_Cluster_E ,
                                                Electron_Cluster_Eta , Electron_Etas2 ,
                                                Electron_Etap , Electron_Etcone20_Uncorrected );

  unsigned int EtBin = this->FindEtBin(Et);
  unsigned int EtaBin = this->FindEtaBin( fabs( Electron_Cluster_Eta ) );

  double cutVal(-999.9);
  if( !m_rel16False_rel17True ){
    cutVal = this->CheckValidAndReturn( m_etcone20DivEt_iso90 , EtBin , EtaBin );
  }
  if( m_rel16False_rel17True ){
    if( m_sumEtFalse_isEMTrue ){
      cutVal =  this->CheckValidAndReturn( m_etcone20DivEt_iso90_rel17 , EtBin , EtaBin );
    }
    if( !m_sumEtFalse_isEMTrue ){
      cutVal =  this->CheckValidAndReturn( m_etcone20DivEt_iso90_rel17_sumEt , EtaBin );
    }     
  }   

  if( (Etcone20Cor / Et) < cutVal ){
    fail0_pass1 = 1;
  }
  return fail0_pass1;
}


/////////////////////////////////////////////////////////
//// Uniform 95% Etcone20 / Et isolation efficiency  ////
/////////////////////////////////////////////////////////
unsigned int EisoTool::Eiso95Etcone20DivEt(const unsigned int& NPV,
                                           const double& Electron_Cluster_E,
                                           const double& Electron_Track_Eta,
                                           const double& Electron_Cluster_Eta,
                                           const double& Electron_Etas2,
                                           const double& Electron_Etap,
                                           const double& Electron_Etcone20_Uncorrected,
                                           const bool& isMC)
{
  m_isMC = isMC;
  return this->Eiso95Etcone20DivEt( NPV , Electron_Cluster_E , Electron_Track_Eta ,
                                    Electron_Cluster_Eta , Electron_Etas2 , Electron_Etap , Electron_Etcone20_Uncorrected );
}


unsigned int EisoTool::Eiso95Etcone20DivEt(const unsigned int& NPV,
                                           const double& Electron_Cluster_E,
                                           const double& Electron_Track_Eta,
                                           const double& Electron_Cluster_Eta,
                                           const double& Electron_Etas2,
                                           const double& Electron_Etap,
                                           const double& Electron_Etcone20_Uncorrected) const
{
  unsigned int fail0_pass1(0);
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  double Etcone20Cor = this->Etcone20Corrected( NPV , Electron_Cluster_E ,
                                                Electron_Cluster_Eta , Electron_Etas2 ,
                                                Electron_Etap , Electron_Etcone20_Uncorrected );

  unsigned int EtBin = this->FindEtBin(Et);
  unsigned int EtaBin = this->FindEtaBin( fabs( Electron_Cluster_Eta ) );

  double cutVal(-999.9);
  if( !m_rel16False_rel17True ){
    cutVal =  this->CheckValidAndReturn( m_etcone20DivEt_iso95 , EtBin , EtaBin );
  }
  if( m_rel16False_rel17True ){
    if( m_sumEtFalse_isEMTrue ){
      cutVal = this->CheckValidAndReturn( m_etcone20DivEt_iso95_rel17 , EtBin , EtaBin );
    }
    if( !m_sumEtFalse_isEMTrue ){
      cutVal =  this->CheckValidAndReturn( m_etcone20DivEt_iso95_rel17_sumEt , EtaBin );
    }       
  }   

  if( (Etcone20Cor / Et) < cutVal ){
    fail0_pass1 = 1;
  }
  return fail0_pass1;
}

/////////////////////////////////////////////////////////
//// Uniform 98% Etcone20 / Et isolation efficiency  ////
/////////////////////////////////////////////////////////
unsigned int EisoTool::Eiso98Etcone20DivEt(const unsigned int& NPV,
                                           const double& Electron_Cluster_E,
                                           const double& Electron_Track_Eta,
                                           const double& Electron_Cluster_Eta,
                                           const double& Electron_Etas2,
                                           const double& Electron_Etap,
                                           const double& Electron_Etcone20_Uncorrected,
                                           const bool& isMC)
{
  m_isMC = isMC;
  return this->Eiso98Etcone20DivEt( NPV , Electron_Cluster_E , Electron_Track_Eta ,
                                    Electron_Cluster_Eta , Electron_Etas2 , Electron_Etap , Electron_Etcone20_Uncorrected );
}


unsigned int EisoTool::Eiso98Etcone20DivEt(const unsigned int& NPV,
                                           const double& Electron_Cluster_E,
                                           const double& Electron_Track_Eta,
                                           const double& Electron_Cluster_Eta,
                                           const double& Electron_Etas2,
                                           const double& Electron_Etap,
                                           const double& Electron_Etcone20_Uncorrected) const
{
  unsigned int fail0_pass1(0);
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  double Etcone20Cor = this->Etcone20Corrected( NPV , Electron_Cluster_E ,
                                                Electron_Cluster_Eta , Electron_Etas2 ,
                                                Electron_Etap , Electron_Etcone20_Uncorrected );

  unsigned int EtBin = this->FindEtBin(Et);
  unsigned int EtaBin = this->FindEtaBin( fabs( Electron_Cluster_Eta ) );

  double cutVal(-999.9);
  if( !m_rel16False_rel17True ){
    cutVal =  this->CheckValidAndReturn( m_etcone20DivEt_iso98 , EtBin , EtaBin );
  }
  if( m_rel16False_rel17True ){
    if( m_sumEtFalse_isEMTrue ){
      cutVal =  this->CheckValidAndReturn( m_etcone20DivEt_iso98_rel17 , EtBin , EtaBin );
    }
    if( !m_sumEtFalse_isEMTrue ){
      cutVal =  this->CheckValidAndReturn( m_etcone20DivEt_iso98_rel17_sumEt , EtaBin );
    }       
  }   

  if( (Etcone20Cor / Et) < cutVal ){
    fail0_pass1 = 1;
  }
  return fail0_pass1;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////   Etcone 30 / Et Isolation Functions ////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
//// Uniform 90% Etcone30 / Et isolation efficiency  ////
/////////////////////////////////////////////////////////
unsigned int EisoTool::Eiso90Etcone30DivEt(const unsigned int& NPV,
                                           const double& Electron_Cluster_E,
                                           const double& Electron_Track_Eta,
                                           const double& Electron_Cluster_Eta,
                                           const double& Electron_Etas2,
                                           const double& Electron_Etap,
                                           const double& Electron_Etcone30_Uncorrected,
                                           const bool& isMC)
{
  m_isMC = isMC;
  return this->Eiso90Etcone30DivEt( NPV , Electron_Cluster_E , Electron_Track_Eta ,
                                    Electron_Cluster_Eta , Electron_Etas2 , Electron_Etap , Electron_Etcone30_Uncorrected );
}


unsigned int EisoTool::Eiso90Etcone30DivEt(const unsigned int& NPV,
                                           const double& Electron_Cluster_E,
                                           const double& Electron_Track_Eta,
                                           const double& Electron_Cluster_Eta,
                                           const double& Electron_Etas2,
                                           const double& Electron_Etap,
                                           const double& Electron_Etcone30_Uncorrected) const
{
  unsigned int fail0_pass1(0);
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  double Etcone30Cor = this->Etcone30Corrected( NPV , Electron_Cluster_E ,
                                                Electron_Cluster_Eta , Electron_Etas2 ,
                                                Electron_Etap , Electron_Etcone30_Uncorrected );

  unsigned int EtBin = this->FindEtBin(Et);
  unsigned int EtaBin = this->FindEtaBin( fabs( Electron_Cluster_Eta ) );

  double cutVal(-999.9);
  if( !m_rel16False_rel17True ){
    cutVal =  this->CheckValidAndReturn( m_etcone30DivEt_iso90 , EtBin , EtaBin );
  }
  if( m_rel16False_rel17True ){
    if( m_sumEtFalse_isEMTrue ){
      cutVal =  this->CheckValidAndReturn( m_etcone30DivEt_iso90_rel17 , EtBin , EtaBin );
    }
    if( !m_sumEtFalse_isEMTrue ){
      cutVal =  this->CheckValidAndReturn( m_etcone30DivEt_iso90_rel17_sumEt , EtaBin );
    }       
  }   

  if( (Etcone30Cor / Et) < cutVal ){
    fail0_pass1 = 1;
  }
  return fail0_pass1;
}


/////////////////////////////////////////////////////////
//// Uniform 95% Etcone30 / Et isolation efficiency  ////
/////////////////////////////////////////////////////////
unsigned int EisoTool::Eiso95Etcone30DivEt(const unsigned int& NPV,
                                           const double& Electron_Cluster_E,
                                           const double& Electron_Track_Eta,
                                           const double& Electron_Cluster_Eta,
                                           const double& Electron_Etas2,
                                           const double& Electron_Etap,
                                           const double& Electron_Etcone30_Uncorrected,
                                           const bool& isMC)
{
  m_isMC = isMC;
  return this->Eiso95Etcone30DivEt( NPV , Electron_Cluster_E , Electron_Track_Eta ,
                                    Electron_Cluster_Eta , Electron_Etas2 , Electron_Etap , Electron_Etcone30_Uncorrected );
}


unsigned int EisoTool::Eiso95Etcone30DivEt(const unsigned int& NPV,
                                           const double& Electron_Cluster_E,
                                           const double& Electron_Track_Eta,
                                           const double& Electron_Cluster_Eta,
                                           const double& Electron_Etas2,
                                           const double& Electron_Etap,
                                           const double& Electron_Etcone30_Uncorrected) const
{
  unsigned int fail0_pass1(0);
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  double Etcone30Cor = this->Etcone30Corrected( NPV , Electron_Cluster_E ,
                                                Electron_Cluster_Eta , Electron_Etas2 ,
                                                Electron_Etap , Electron_Etcone30_Uncorrected );

  unsigned int EtBin = this->FindEtBin(Et);
  unsigned int EtaBin = this->FindEtaBin( fabs( Electron_Cluster_Eta ) );

  double cutVal(-999.9);
  if( !m_rel16False_rel17True ){
    cutVal =  this->CheckValidAndReturn( m_etcone30DivEt_iso95 , EtBin , EtaBin );
  }
  if( m_rel16False_rel17True ){
    if( m_sumEtFalse_isEMTrue ){
      cutVal =  this->CheckValidAndReturn( m_etcone30DivEt_iso95_rel17 , EtBin , EtaBin );
    }
    if( !m_sumEtFalse_isEMTrue ){
      cutVal =  this->CheckValidAndReturn( m_etcone30DivEt_iso95_rel17_sumEt , EtaBin );
    }       
  }   

  if( (Etcone30Cor / Et) < cutVal ){
    fail0_pass1 = 1;
  }
  return fail0_pass1;
}

/////////////////////////////////////////////////////////
//// Uniform 98% Etcone30 / Et isolation efficiency  ////
/////////////////////////////////////////////////////////
unsigned int EisoTool::Eiso98Etcone30DivEt(const unsigned int& NPV,
                                           const double& Electron_Cluster_E,
                                           const double& Electron_Track_Eta,
                                           const double& Electron_Cluster_Eta,
                                           const double& Electron_Etas2,
                                           const double& Electron_Etap,
                                           const double& Electron_Etcone30_Uncorrected,
                                           const bool& isMC)
{
  m_isMC = isMC;
  return this->Eiso98Etcone30DivEt( NPV , Electron_Cluster_E , Electron_Track_Eta ,
                                    Electron_Cluster_Eta , Electron_Etas2 , Electron_Etap , Electron_Etcone30_Uncorrected );
}


unsigned int EisoTool::Eiso98Etcone30DivEt(const unsigned int& NPV,
                                           const double& Electron_Cluster_E,
                                           const double& Electron_Track_Eta,
                                           const double& Electron_Cluster_Eta,
                                           const double& Electron_Etas2,
                                           const double& Electron_Etap,
                                           const double& Electron_Etcone30_Uncorrected) const
{
  unsigned int fail0_pass1(0);
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  double Etcone30Cor = this->Etcone30Corrected( NPV , Electron_Cluster_E ,
                                                Electron_Cluster_Eta , Electron_Etas2 ,
                                                Electron_Etap , Electron_Etcone30_Uncorrected );

  unsigned int EtBin = this->FindEtBin(Et);
  unsigned int EtaBin = this->FindEtaBin( fabs( Electron_Cluster_Eta ) );

  double cutVal(-999.9);
  if( !m_rel16False_rel17True ){
    cutVal =  this->CheckValidAndReturn( m_etcone30DivEt_iso98 , EtBin , EtaBin );
  }
  if( m_rel16False_rel17True ){
     if( m_sumEtFalse_isEMTrue ){
      cutVal =  this->CheckValidAndReturn( m_etcone30DivEt_iso98_rel17 , EtBin , EtaBin );
    }
    if( !m_sumEtFalse_isEMTrue ){
      cutVal =  this->CheckValidAndReturn( m_etcone30DivEt_iso98_rel17_sumEt , EtaBin );
    }      
  }  

  if( (Etcone30Cor / Et) < cutVal ){
    fail0_pass1 = 1;
  }
  return fail0_pass1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////   Ptcone 20 / Et Isolation Functions ///////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
//// Uniform 90% Ptcone20 / Et isolation efficiency  ////
/////////////////////////////////////////////////////////
unsigned int EisoTool::Eiso90Ptcone20DivEt(const double& Electron_Cluster_E,
                                           const double& Electron_Track_Eta,
                                           const double& Electron_Cluster_Eta,
                                           const double& Ptcone20) const
{
  unsigned int fail0_pass1(0);
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  unsigned int EtBin = this->FindEtBin(Et);
  unsigned int EtaBin = this->FindEtaBin( fabs( Electron_Cluster_Eta ) );
  double cutVal(-999.9);
  if( m_sumEtFalse_isEMTrue ){
    cutVal =  this->CheckValidAndReturn( m_ptcone20DivEt_iso90 , EtBin , EtaBin );
  }
  if( !m_sumEtFalse_isEMTrue ){
    cutVal =  this->CheckValidAndReturn( m_ptcone20DivEt_iso90_sumEt , EtaBin );
  }  
  if( (Ptcone20 / Et) < cutVal ){
    fail0_pass1 = 1;
  }
  return fail0_pass1;
}

/////////////////////////////////////////////////////////
//// Uniform 97% Ptcone20 / Et isolation efficiency  ////
/////////////////////////////////////////////////////////
unsigned int EisoTool::Eiso97Ptcone20DivEt(const double& Electron_Cluster_E,
                                           const double& Electron_Track_Eta,
                                           const double& Electron_Cluster_Eta,
                                           const double& Ptcone20) const
{
  unsigned int fail0_pass1(0);
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  unsigned int EtBin = this->FindEtBin(Et);
  unsigned int EtaBin = this->FindEtaBin( fabs( Electron_Cluster_Eta ) );
  double cutVal(-999.9);
  if( m_sumEtFalse_isEMTrue ){
    cutVal =  this->CheckValidAndReturn( m_ptcone20DivEt_iso97 , EtBin , EtaBin );
  }
  if( !m_sumEtFalse_isEMTrue ){
    cutVal =  this->CheckValidAndReturn( m_ptcone20DivEt_iso97_sumEt , EtaBin );
  }  
  if( (Ptcone20 / Et) < cutVal ){
    fail0_pass1 = 1;
  }
  return fail0_pass1;
}

/////////////////////////////////////////////////////////
//// Uniform 98% Ptcone20 / Et isolation efficiency  ////
/////////////////////////////////////////////////////////
unsigned int EisoTool::Eiso98Ptcone20DivEt(const double& Electron_Cluster_E,
                                           const double& Electron_Track_Eta,
                                           const double& Electron_Cluster_Eta,
                                           const double& Ptcone20) const
{
  unsigned int fail0_pass1(0);
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  unsigned int EtBin = this->FindEtBin(Et);
  unsigned int EtaBin = this->FindEtaBin( fabs( Electron_Cluster_Eta ) );
  double cutVal(-999.9);
  if( m_sumEtFalse_isEMTrue ){
    cutVal =  this->CheckValidAndReturn( m_ptcone20DivEt_iso98 , EtBin , EtaBin );
  }
  if( !m_sumEtFalse_isEMTrue ){
    cutVal =  this->CheckValidAndReturn( m_ptcone20DivEt_iso98_sumEt , EtaBin );
  }  
  if( (Ptcone20 / Et) < cutVal ){
    fail0_pass1 = 1;
  }
  return fail0_pass1;
}

/////////////////////////////////////////////////////////
//// Uniform 99% Ptcone20 / Et isolation efficiency  ////
/////////////////////////////////////////////////////////
unsigned int EisoTool::Eiso99Ptcone20DivEt(const double& Electron_Cluster_E,
                                           const double& Electron_Track_Eta,
                                           const double& Electron_Cluster_Eta,
                                           const double& Ptcone20) const
{
  unsigned int fail0_pass1(0);
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  unsigned int EtBin = this->FindEtBin(Et);
  unsigned int EtaBin = this->FindEtaBin( fabs( Electron_Cluster_Eta ) );
  double cutVal(-999.9);
  if( m_sumEtFalse_isEMTrue ){
    cutVal =  this->CheckValidAndReturn( m_ptcone20DivEt_iso99 , EtBin , EtaBin );
  }
  if( !m_sumEtFalse_isEMTrue ){
    cutVal =  this->CheckValidAndReturn( m_ptcone20DivEt_iso99_sumEt , EtaBin );
  }  
  if( (Ptcone20 / Et) < cutVal ){
    fail0_pass1 = 1;
  }
  return fail0_pass1;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////   Ptcone 30 / Et Isolation Functions ///////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
//// Uniform 90% Ptcone30 / Et isolation efficiency  ////
/////////////////////////////////////////////////////////
unsigned int EisoTool::Eiso90Ptcone30DivEt(const double& Electron_Cluster_E,
                                           const double& Electron_Track_Eta,
                                           const double& Electron_Cluster_Eta,
                                           const double& Ptcone30) const
{
  unsigned int fail0_pass1(0);
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  unsigned int EtBin = this->FindEtBin(Et);
  unsigned int EtaBin = this->FindEtaBin( fabs( Electron_Cluster_Eta ) );
  double cutVal(-999.9);
  if( m_sumEtFalse_isEMTrue ){
    cutVal =  this->CheckValidAndReturn( m_ptcone30DivEt_iso90 , EtBin , EtaBin );
  }
  if( !m_sumEtFalse_isEMTrue ){
    cutVal =  this->CheckValidAndReturn( m_ptcone30DivEt_iso90_sumEt , EtaBin );
  }  
  if( (Ptcone30 / Et) < cutVal ){
    fail0_pass1 = 1;
  }
  return fail0_pass1;
}

/////////////////////////////////////////////////////////
//// Uniform 97% Ptcone30 / Et isolation efficiency  ////
/////////////////////////////////////////////////////////
unsigned int EisoTool::Eiso97Ptcone30DivEt(const double& Electron_Cluster_E,
                                           const double& Electron_Track_Eta,
                                           const double& Electron_Cluster_Eta,
                                           const double& Ptcone30) const
{
  unsigned int fail0_pass1(0);
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  unsigned int EtBin = this->FindEtBin(Et);
  unsigned int EtaBin = this->FindEtaBin( fabs( Electron_Cluster_Eta ) );
  double cutVal(-999.9);
  if( m_sumEtFalse_isEMTrue ){
    cutVal =  this->CheckValidAndReturn( m_ptcone30DivEt_iso97 , EtBin , EtaBin );
  }
  if( !m_sumEtFalse_isEMTrue ){
    cutVal =  this->CheckValidAndReturn( m_ptcone30DivEt_iso97_sumEt , EtaBin );
  }  
  if( (Ptcone30 / Et) < cutVal ){
    fail0_pass1 = 1;
  }
  return fail0_pass1;
}

/////////////////////////////////////////////////////////
//// Uniform 98% Ptcone30 / Et isolation efficiency  ////
/////////////////////////////////////////////////////////
unsigned int EisoTool::Eiso98Ptcone30DivEt(const double& Electron_Cluster_E,
                                           const double& Electron_Track_Eta,
                                           const double& Electron_Cluster_Eta,
                                           const double& Ptcone30) const
{
  unsigned int fail0_pass1(0);
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  unsigned int EtBin = this->FindEtBin(Et);
  unsigned int EtaBin = this->FindEtaBin( fabs( Electron_Cluster_Eta ) );
  double cutVal(-999.9);
  if( m_sumEtFalse_isEMTrue ){
    cutVal =  this->CheckValidAndReturn( m_ptcone30DivEt_iso98 , EtBin , EtaBin );
  }
  if( !m_sumEtFalse_isEMTrue ){
    cutVal =  this->CheckValidAndReturn( m_ptcone30DivEt_iso98_sumEt , EtaBin );
  }  
  if( (Ptcone30 / Et) < cutVal ){
    fail0_pass1 = 1;
  }
  return fail0_pass1;
}

/////////////////////////////////////////////////////////
//// Uniform 99% Ptcone30 / Et isolation efficiency  ////
/////////////////////////////////////////////////////////
unsigned int EisoTool::Eiso99Ptcone30DivEt(const double& Electron_Cluster_E,
                                           const double& Electron_Track_Eta,
                                           const double& Electron_Cluster_Eta,
                                           const double& Ptcone30) const
{
  unsigned int fail0_pass1(0);
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  unsigned int EtBin = this->FindEtBin(Et);
  unsigned int EtaBin = this->FindEtaBin( fabs( Electron_Cluster_Eta ) );
  double cutVal(-999.9);
  if( m_sumEtFalse_isEMTrue ){
    cutVal =  this->CheckValidAndReturn( m_ptcone30DivEt_iso99 , EtBin , EtaBin );
  }
  if( !m_sumEtFalse_isEMTrue ){
    cutVal =  this->CheckValidAndReturn( m_ptcone30DivEt_iso99_sumEt , EtaBin );
  }  
  if( (Ptcone30 / Et) < cutVal ){
    fail0_pass1 = 1;
  }
  return fail0_pass1;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////   Ptcone 40 / Et Isolation Functions ///////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
//// Uniform 90% Ptcone40 / Et isolation efficiency  ////
/////////////////////////////////////////////////////////
unsigned int EisoTool::Eiso90Ptcone40DivEt(const double& Electron_Cluster_E,
                                           const double& Electron_Track_Eta,
                                           const double& Electron_Cluster_Eta,
                                           const double& Ptcone40) const
{
  unsigned int fail0_pass1(0);
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  unsigned int EtBin = this->FindEtBin(Et);
  unsigned int EtaBin = this->FindEtaBin( fabs( Electron_Cluster_Eta ) );
  double cutVal(-999.9);
  if( m_sumEtFalse_isEMTrue ){
    cutVal =  this->CheckValidAndReturn( m_ptcone40DivEt_iso90 , EtBin , EtaBin );
  }
  if( !m_sumEtFalse_isEMTrue ){
    cutVal =  this->CheckValidAndReturn( m_ptcone40DivEt_iso90_sumEt , EtaBin );
  }  
  if( (Ptcone40 / Et) < cutVal ){
    fail0_pass1 = 1;
  }
  return fail0_pass1;
}

/////////////////////////////////////////////////////////
//// Uniform 97% Ptcone40 / Et isolation efficiency  ////
/////////////////////////////////////////////////////////
unsigned int EisoTool::Eiso97Ptcone40DivEt(const double& Electron_Cluster_E,
                                           const double& Electron_Track_Eta,
                                           const double& Electron_Cluster_Eta,
                                           const double& Ptcone40) const
{
  unsigned int fail0_pass1(0);
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  unsigned int EtBin = this->FindEtBin(Et);
  unsigned int EtaBin = this->FindEtaBin( fabs( Electron_Cluster_Eta ) );
  double cutVal(-999.9);
  if( m_sumEtFalse_isEMTrue ){
    cutVal =  this->CheckValidAndReturn( m_ptcone40DivEt_iso97 , EtBin , EtaBin );
  }
  if( !m_sumEtFalse_isEMTrue ){
    cutVal =  this->CheckValidAndReturn( m_ptcone40DivEt_iso97_sumEt , EtaBin );
  }  
  if( (Ptcone40 / Et) < cutVal ){
    fail0_pass1 = 1;
  }
  return fail0_pass1;
}

/////////////////////////////////////////////////////////
//// Uniform 98% Ptcone40 / Et isolation efficiency  ////
/////////////////////////////////////////////////////////
unsigned int EisoTool::Eiso98Ptcone40DivEt(const double& Electron_Cluster_E,
                                           const double& Electron_Track_Eta,
                                           const double& Electron_Cluster_Eta,
                                           const double& Ptcone40) const
{
  unsigned int fail0_pass1(0);
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  unsigned int EtBin = this->FindEtBin(Et);
  unsigned int EtaBin = this->FindEtaBin( fabs( Electron_Cluster_Eta ) );
  double cutVal(-999.9);
  if( m_sumEtFalse_isEMTrue ){
    cutVal =  this->CheckValidAndReturn( m_ptcone40DivEt_iso98 , EtBin , EtaBin );
  }
  if( !m_sumEtFalse_isEMTrue ){
    cutVal =  this->CheckValidAndReturn( m_ptcone40DivEt_iso98_sumEt , EtaBin );
  }  
  if( (Ptcone40 / Et) < cutVal ){
    fail0_pass1 = 1;
  }
  return fail0_pass1;
}

/////////////////////////////////////////////////////////
//// Uniform 99% Ptcone40 / Et isolation efficiency  ////
/////////////////////////////////////////////////////////
unsigned int EisoTool::Eiso99Ptcone40DivEt(const double& Electron_Cluster_E,
                                           const double& Electron_Track_Eta,
                                           const double& Electron_Cluster_Eta,
                                           const double& Ptcone40) const
{
  unsigned int fail0_pass1(0);
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  unsigned int EtBin = this->FindEtBin(Et);
  unsigned int EtaBin = this->FindEtaBin( fabs( Electron_Cluster_Eta ) );
  double cutVal(-999.9);
  if( m_sumEtFalse_isEMTrue ){
    cutVal =  this->CheckValidAndReturn( m_ptcone40DivEt_iso99 , EtBin , EtaBin );
  }
  if( !m_sumEtFalse_isEMTrue ){
    cutVal =  this->CheckValidAndReturn( m_ptcone40DivEt_iso99_sumEt , EtaBin );
  }  
  if( (Ptcone40 / Et) < cutVal ){
    fail0_pass1 = 1;
  }
  return fail0_pass1;
}



/////////////////////////////////////////
//////// Internal Tools /////////////////
/////////////////////////////////////////

double EisoTool::CheckValidAndReturn(std::vector<std::vector<double> >* cutVec,const unsigned int et, const unsigned int eta) const
{
  double value(-999.9); // If you get this back, something somewhere has gone wrong
  if( cutVec->size() > et ){
    std::vector<double> v = cutVec->at(et);
    value = this->CheckValidAndReturn( v , eta );
  }
  return value;
}

double EisoTool::CheckValidAndReturn(std::vector<double>* cutVec,const unsigned int eta) const
{
  double value(-999.9); // If you get this back, something somewhere has gone wrong  
  if( cutVec->size() > eta ){
    value = cutVec->at(eta);
  }
  return value;
}

double EisoTool::CheckValidAndReturn(const std::vector<double>& cutVec,const unsigned int eta) const
{
  double value(-999.9); // If you get this back, something somewhere has gone wrong
  if( cutVec.size() > eta ){
    value = cutVec.at(eta);
  }
  return value;
}

double EisoTool::ElectronEt(const double& Electron_Cluster_E, const double& Electron_Track_Eta) const
{
  return Electron_Cluster_E / cosh( Electron_Track_Eta );
}

double EisoTool::Etcone20Corrected(const unsigned int& NPV,
                                   const double& Electron_Cluster_E,
                                   const double& Electron_Cluster_Eta,
                                   const double& Electron_Etas2,
                                   const double& Electron_Etap,
                                   const double& Electron_Etcone20_Uncorrected) const
{
  double etcone(-999.9);
  if( !m_rel16False_rel17True ){
    etcone = CaloIsoCorrection::GetPtNPVCorrectedIsolation(
                                NPV,
                                Electron_Cluster_E,
                                Electron_Etas2,
                                Electron_Etap,
                                Electron_Cluster_Eta,
                                0.2,
                                m_isMC,
                                Electron_Etcone20_Uncorrected,
                                false,
                                CaloIsoCorrection::ELECTRON,
                                CaloIsoCorrection::REL16);
  }
  if( m_rel16False_rel17True ){
    etcone = CaloIsoCorrection::GetPtNPVCorrectedIsolation(
                                NPV,
                                Electron_Cluster_E,
                                Electron_Etas2,
                                Electron_Etap,
                                Electron_Cluster_Eta,
                                0.2,
                                m_isMC,
                                Electron_Etcone20_Uncorrected,
                                false,
                                CaloIsoCorrection::ELECTRON,
                                CaloIsoCorrection::REL17);
  }
  return etcone;  
}


double EisoTool::Etcone30Corrected(const unsigned int& NPV,
                                   const double& Electron_Cluster_E,
                                   const double& Electron_Cluster_Eta,
                                   const double& Electron_Etas2,
                                   const double& Electron_Etap,
                                   const double& Electron_Etcone30_Uncorrected) const
{
  double etcone(-999.9);
  if( !m_rel16False_rel17True ){
    etcone = CaloIsoCorrection::GetPtNPVCorrectedIsolation(
                                NPV,
                                Electron_Cluster_E,
                                Electron_Etas2,
                                Electron_Etap,
                                Electron_Cluster_Eta,
                                0.3,
                                m_isMC,
                                Electron_Etcone30_Uncorrected,
                                false,
                                CaloIsoCorrection::ELECTRON,
                                CaloIsoCorrection::REL16);
  }
  if( m_rel16False_rel17True ){
    etcone = CaloIsoCorrection::GetPtNPVCorrectedIsolation(
                                NPV,
                                Electron_Cluster_E,
                                Electron_Etas2,
                                Electron_Etap,
                                Electron_Cluster_Eta,
                                0.3,
                                m_isMC,
                                Electron_Etcone30_Uncorrected,
                                false,
                                CaloIsoCorrection::ELECTRON,
                                CaloIsoCorrection::REL17);
  }
  return etcone;
}

unsigned int EisoTool::FindEtBin(const double Et) const
{
  // Out of range padding
  // If Et < 10 GeV - return error
  if( Et < 10.0 * m_GeV ){return 999;}
  // If Et > 120 GeV - use last bin
  if( Et > 120.0 * m_GeV ){return 8;}

  unsigned int bin(0);
  for(unsigned int i=0;i<m_etBins->size();++i){
    if( Et < m_etBins->at(i) ){
      bin = i - 1;
      break;
    }
  }
  return bin;
}

unsigned int EisoTool::FindEtaBin(const double Eta) const
{
  // Out of range padding
  // If |eta| > 2.47 - use last bin
  if( fabs( Eta ) > 2.47 ){return 8;}

  unsigned int bin(0);
  for(unsigned int i=0;i<m_etaBins->size();++i){
    if( fabs(Eta) < m_etaBins->at(i) ){
      bin = i - 1;
      break;
    }
  }
//   std::cout<<"Eta Bin used, value = "<<bin<<", "<<m_etaBins->at(bin)<<" -> "<<m_etaBins->at(bin+1)<<std::endl;
  return bin;
}


