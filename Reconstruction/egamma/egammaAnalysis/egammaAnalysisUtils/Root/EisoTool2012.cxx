/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaAnalysisUtils/EisoTool2012.h"

unsigned int EisoTool2012::Etcone(const double& cone,
                                  const unsigned int& Efficiency_required,
                                  const unsigned int& NPV,
                                  const double& Electron_Cluster_E,
                                  const double& Electron_Track_Eta,
                                  const double& Electron_Cluster_Eta,
                                  const double& Electron_Etas2,
                                  const double& Electron_Etap,
                                  const double& Electron_Etcone_Uncorrected,
                                  const bool& isMC)
{
  m_isMC = isMC;
  return this->Etcone(cone,Efficiency_required,NPV,Electron_Cluster_E,Electron_Track_Eta,
                      Electron_Cluster_Eta,Electron_Etas2,Electron_Etap,Electron_Etcone_Uncorrected);
}

unsigned int EisoTool2012::Etcone(const double& cone,
                                  const unsigned int& Efficiency_required,
                                  const unsigned int& NPV,
                                  const double& Electron_Cluster_E,
                                  const double& Electron_Track_Eta,
                                  const double& Electron_Cluster_Eta,
                                  const double& Electron_Etas2,
                                  const double& Electron_Etap,
                                  const double& Electron_Etcone_Uncorrected) const
{
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  double Value = this->EtconeCorrected(NPV,Electron_Cluster_E,Electron_Cluster_Eta,Electron_Etas2,Electron_Etap,Electron_Etcone_Uncorrected,cone);

  if( cone == 0.2 || cone == 20 ){
    return this->PassOrFail(Et,Electron_Cluster_Eta,Efficiency_required,Value,h_etconeCor20);
  }
  if( cone == 0.3 || cone == 30 ){
    return this->PassOrFail(Et,Electron_Cluster_Eta,Efficiency_required,Value,h_etconeCor30);
  }
  return 0; 
}

unsigned int EisoTool2012::Etcone_DivEt(const double& cone,
                                        const unsigned int& Efficiency_required,
                                        const unsigned int& NPV,
                                        const double& Electron_Cluster_E,
                                        const double& Electron_Track_Eta,
                                        const double& Electron_Cluster_Eta,
                                        const double& Electron_Etas2,
                                        const double& Electron_Etap,
                                        const double& Electron_Etcone_Uncorrected,
                                        const bool& isMC)
{
  m_isMC = isMC;
  return this->Etcone_DivEt(cone,Efficiency_required,NPV,Electron_Cluster_E,Electron_Track_Eta,
                            Electron_Cluster_Eta,Electron_Etas2,Electron_Etap,Electron_Etcone_Uncorrected);
}

unsigned int EisoTool2012::Etcone_DivEt(const double& cone,
                                        const unsigned int& Efficiency_required,
                                        const unsigned int& NPV,
                                        const double& Electron_Cluster_E,
                                        const double& Electron_Track_Eta,
                                        const double& Electron_Cluster_Eta,
                                        const double& Electron_Etas2,
                                        const double& Electron_Etap,
                                        const double& Electron_Etcone_Uncorrected) const
{
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  double Value = this->EtconeCorrected(NPV,Electron_Cluster_E,Electron_Cluster_Eta,Electron_Etas2,Electron_Etap,Electron_Etcone_Uncorrected,cone);

  if( cone == 0.2 || cone == 20 ){
    return this->PassOrFail_DivEt(Et,Electron_Cluster_Eta,Efficiency_required,Value,h_etconeCor20_DivEt);
  }
  if( cone == 0.3 || cone == 30 ){
    return this->PassOrFail_DivEt(Et,Electron_Cluster_Eta,Efficiency_required,Value,h_etconeCor30_DivEt);
  }
  return 0; 
}

unsigned int EisoTool2012::Ptcone(const double& cone,
                                  const unsigned int& Efficiency_required,
                                  const double& Electron_Cluster_E,
                                  const double& Electron_Track_Eta,
                                  const double& Electron_Cluster_Eta,
                                  const double& Electron_Ptcone) const
{
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  if( cone == 0.2 || cone == 20 ){
    return this->PassOrFail(Et,Electron_Cluster_Eta,Efficiency_required,Electron_Ptcone,h_ptcone20);
  }
  if( cone == 0.3 || cone == 30 ){
    return this->PassOrFail(Et,Electron_Cluster_Eta,Efficiency_required,Electron_Ptcone,h_ptcone30);
  }
  return 0;   
}

unsigned int EisoTool2012::Ptcone_DivEt(const double& cone,
                                        const unsigned int& Efficiency_required,
                                        const double& Electron_Cluster_E,
                                        const double& Electron_Track_Eta,
                                        const double& Electron_Cluster_Eta,
                                        const double& Electron_Ptcone) const
{
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  if( cone == 0.2 || cone == 20 ){
    return this->PassOrFail_DivEt(Et,Electron_Cluster_Eta,Efficiency_required,Electron_Ptcone,h_ptcone20_DivEt);
  }
  if( cone == 0.3 || cone == 30 ){
    return this->PassOrFail_DivEt(Et,Electron_Cluster_Eta,Efficiency_required,Electron_Ptcone,h_ptcone30_DivEt);
  }
  return 0;   
}

unsigned int EisoTool2012::Topocone(const double& cone,
                                    const unsigned int& Efficiency_required,
                                    const double& Electron_Cluster_E,
                                    const double& Electron_Track_Eta,
                                    const double& Electron_Cluster_Eta,
                                    const double& Electron_Etas2,
                                    const double& Electron_Etap,
                                    const double& Electron_ED_median,
                                    const double& Electron_topocone,
                                    const bool& isMC)
{
  m_isMC = isMC;
  return this->Topocone(cone,Efficiency_required,Electron_Cluster_E,Electron_Track_Eta,
                        Electron_Cluster_Eta,Electron_Etas2,Electron_Etap,Electron_ED_median,Electron_topocone);
}

unsigned int EisoTool2012::Topocone(const double& cone,
                                    const unsigned int& Efficiency_required,
                                    const double& Electron_Cluster_E,
                                    const double& Electron_Track_Eta,
                                    const double& Electron_Cluster_Eta,
                                    const double& Electron_Etas2,
                                    const double& Electron_Etap,
                                    const double& Electron_ED_median,
                                    const double& Electron_topocone) const
{
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  double Value = this->TopoconeCorrected(Electron_Cluster_E,Electron_Cluster_Eta,Electron_Etas2,Electron_Etap,
                                         Electron_ED_median,Electron_topocone,cone);

  if( cone == 0.2 || cone == 20 ){
    return this->PassOrFail(Et,Electron_Cluster_Eta,Efficiency_required,Value,h_topoconeCor20);
  }
  if( cone == 0.3 || cone == 30 ){
    return this->PassOrFail(Et,Electron_Cluster_Eta,Efficiency_required,Value,h_topoconeCor30);
  }
  return 0;
}



unsigned int EisoTool2012::Topocone_DivEt(const double& cone,
                                          const unsigned int& Efficiency_required,
                                          const double& Electron_Cluster_E,
                                          const double& Electron_Track_Eta,
                                          const double& Electron_Cluster_Eta,
                                          const double& Electron_Etas2,
                                          const double& Electron_Etap,
                                          const double& Electron_ED_median,
                                          const double& Electron_topocone,
                                          const bool& isMC)
{
  m_isMC = isMC;
  return this->Topocone_DivEt(cone,Efficiency_required,Electron_Cluster_E,Electron_Track_Eta,
                              Electron_Cluster_Eta,Electron_Etas2,Electron_Etap,Electron_ED_median,Electron_topocone);
}

unsigned int EisoTool2012::Topocone_DivEt(const double& cone,
                                          const unsigned int& Efficiency_required,
                                          const double& Electron_Cluster_E,
                                          const double& Electron_Track_Eta,
                                          const double& Electron_Cluster_Eta,
                                          const double& Electron_Etas2,
                                          const double& Electron_Etap,
                                          const double& Electron_ED_median,
                                          const double& Electron_topocone) const
{
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  double Value = this->TopoconeCorrected(Electron_Cluster_E,Electron_Cluster_Eta,Electron_Etas2,Electron_Etap,
                                         Electron_ED_median,Electron_topocone,cone);

  if( cone == 0.2 || cone == 20 ){
    return this->PassOrFail_DivEt(Et,Electron_Cluster_Eta,Efficiency_required,Value,h_topoconeCor20_DivEt);
  }
  if( cone == 0.3 || cone == 30 ){
    return this->PassOrFail_DivEt(Et,Electron_Cluster_Eta,Efficiency_required,Value,h_topoconeCor30_DivEt);
  }
  return 0;
}

unsigned int EisoTool2012::MiniIsolation(const unsigned int& MI,
                                         const unsigned int& max,
                                         const unsigned int& Efficiency_required,
                                         const double& Electron_Cluster_E,
                                         const double& Electron_Track_Eta,
                                         const double& Electron_Cluster_Eta,
                                         const double& MiniIsolationVariable) const
{
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta ); 
  if( MI == 10 ){
    if( max == 30 ){
      return this->PassOrFail(Et,Electron_Cluster_Eta,Efficiency_required,MiniIsolationVariable,h_MI10_max30_ptsum);
    }
    if( max == 40 ){
      return this->PassOrFail(Et,Electron_Cluster_Eta,Efficiency_required,MiniIsolationVariable,h_MI10_max40_ptsum);  
    }
  }
  if( MI == 15 ){
    if( max == 30 ){
      return this->PassOrFail(Et,Electron_Cluster_Eta,Efficiency_required,MiniIsolationVariable,h_MI15_max30_ptsum);
    }
    if( max == 40 ){
      return this->PassOrFail(Et,Electron_Cluster_Eta,Efficiency_required,MiniIsolationVariable,h_MI15_max40_ptsum);  
    }
  }  
  return 0;
}

unsigned int EisoTool2012::MiniIsolation_DivEt(const unsigned int& MI,
                                               const unsigned int& max,
                                               const unsigned int& Efficiency_required,
                                               const double& Electron_Cluster_E,
                                               const double& Electron_Track_Eta,
                                               const double& Electron_Cluster_Eta,
                                               const double& MiniIsolationVariable) const
{
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta ); 
  if( MI == 10 ){
    if( max == 30 ){
      return this->PassOrFail_DivEt(Et,Electron_Cluster_Eta,Efficiency_required,MiniIsolationVariable,h_MI10_max30_ptsum_DivEt);
    }
    if( max == 40 ){
      return this->PassOrFail_DivEt(Et,Electron_Cluster_Eta,Efficiency_required,MiniIsolationVariable,h_MI10_max40_ptsum_DivEt);  
    }
  }
  if( MI == 15 ){
    if( max == 30 ){
      return this->PassOrFail_DivEt(Et,Electron_Cluster_Eta,Efficiency_required,MiniIsolationVariable,h_MI15_max30_ptsum_DivEt);
    }
    if( max == 40 ){
      return this->PassOrFail_DivEt(Et,Electron_Cluster_Eta,Efficiency_required,MiniIsolationVariable,h_MI15_max40_ptsum_DivEt);  
    }
  }  
  return 0;
}

/////////////////////////////////////////////////////////
// Get Cut Values                                      //
/////////////////////////////////////////////////////////
double EisoTool2012::getCutValueEtcone(const double& cone,
                                       const unsigned int& Efficiency_required,
                                       const double& Electron_Cluster_E,
                                       const double& Electron_Track_Eta,
                                       const double& Electron_Cluster_Eta) const
{
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  if( cone == 0.2 || cone == 20 ){
    return this->getCutValue(Et,Electron_Cluster_Eta,Efficiency_required,h_etconeCor20);
  }
  if( cone == 0.3 || cone == 30 ){
    return this->getCutValue(Et,Electron_Cluster_Eta,Efficiency_required,h_etconeCor30);
  }
  return 0;   
}

double EisoTool2012::getCutValueEtcone_DivEt(const double& cone,
                                             const unsigned int& Efficiency_required,
                                             const double& Electron_Cluster_E,
                                             const double& Electron_Track_Eta,
                                             const double& Electron_Cluster_Eta) const
{
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  if( cone == 0.2 || cone == 20 ){
    return this->getCutValue(Et,Electron_Cluster_Eta,Efficiency_required,h_etconeCor20_DivEt);
  }
  if( cone == 0.3 || cone == 30 ){
    return this->getCutValue(Et,Electron_Cluster_Eta,Efficiency_required,h_etconeCor30_DivEt);
  }
  return 0;   
}

double EisoTool2012::getCutValuePtcone(const double& cone,
                                       const unsigned int& Efficiency_required,
                                       const double& Electron_Cluster_E,
                                       const double& Electron_Track_Eta,
                                       const double& Electron_Cluster_Eta) const
{
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  if( cone == 0.2 || cone == 20 ){
    return this->getCutValue(Et,Electron_Cluster_Eta,Efficiency_required,h_ptcone20);
  }
  if( cone == 0.3 || cone == 30 ){
    return this->getCutValue(Et,Electron_Cluster_Eta,Efficiency_required,h_ptcone30);
  }
  return 0;    
}

double EisoTool2012::getCutValuePtcone_DivEt(const double& cone,
                                             const unsigned int& Efficiency_required,
                                             const double& Electron_Cluster_E,
                                             const double& Electron_Track_Eta,
                                             const double& Electron_Cluster_Eta) const
{
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  if( cone == 0.2 || cone == 20 ){
    return this->getCutValue(Et,Electron_Cluster_Eta,Efficiency_required,h_ptcone20_DivEt);
  }
  if( cone == 0.3 || cone == 30 ){
    return this->getCutValue(Et,Electron_Cluster_Eta,Efficiency_required,h_ptcone30_DivEt);
  }
  return 0;    
}

double EisoTool2012::getCutValueTopocone(const double& cone,
                                         const unsigned int& Efficiency_required,
                                         const double& Electron_Cluster_E,
                                         const double& Electron_Track_Eta,
                                         const double& Electron_Cluster_Eta) const
{
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  if( cone == 0.2 || cone == 20 ){
    return this->getCutValue(Et,Electron_Cluster_Eta,Efficiency_required,h_topoconeCor20);
  }
  if( cone == 0.3 || cone == 30 ){
    return this->getCutValue(Et,Electron_Cluster_Eta,Efficiency_required,h_topoconeCor30);
  }
  return 0;  
}

double EisoTool2012::getCutValueTopocone_DivEt(const double& cone,
                                               const unsigned int& Efficiency_required,
                                               const double& Electron_Cluster_E,
                                               const double& Electron_Track_Eta,
                                               const double& Electron_Cluster_Eta) const
{
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta );
  if( cone == 0.2 || cone == 20 ){
    return this->getCutValue(Et,Electron_Cluster_Eta,Efficiency_required,h_topoconeCor20_DivEt);
  }
  if( cone == 0.3 || cone == 30 ){
    return this->getCutValue(Et,Electron_Cluster_Eta,Efficiency_required,h_topoconeCor30_DivEt);
  }
  return 0;  
}

double EisoTool2012::getCutValueMiniIsolation(const unsigned int& MI,
                                              const unsigned int& max,
                                              const unsigned int& Efficiency_required,
                                              const double& Electron_Cluster_E,
                                              const double& Electron_Track_Eta,
                                              const double& Electron_Cluster_Eta) const
{
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta ); 
  if( MI == 10 ){
    if( max == 30 ){
      return this->getCutValue(Et,Electron_Cluster_Eta,Efficiency_required,h_MI10_max30_ptsum);
    }
    if( max == 40 ){
      return this->getCutValue(Et,Electron_Cluster_Eta,Efficiency_required,h_MI10_max40_ptsum);  
    }
  }
  if( MI == 15 ){
    if( max == 30 ){
      return this->getCutValue(Et,Electron_Cluster_Eta,Efficiency_required,h_MI15_max30_ptsum);
    }
    if( max == 40 ){
      return this->getCutValue(Et,Electron_Cluster_Eta,Efficiency_required,h_MI15_max40_ptsum);  
    }
  } 
  return 0;
}

double EisoTool2012::getCutValueMiniIsolation_DivEt(const unsigned int& MI,
                                                    const unsigned int& max,
                                                    const unsigned int& Efficiency_required,
                                                    const double& Electron_Cluster_E,
                                                    const double& Electron_Track_Eta,
                                                    const double& Electron_Cluster_Eta) const
{
  double Et = this->ElectronEt( Electron_Cluster_E , Electron_Track_Eta ); 
  if( MI == 10 ){
    if( max == 30 ){
      return this->getCutValue(Et,Electron_Cluster_Eta,Efficiency_required,h_MI10_max30_ptsum_DivEt);
    }
    if( max == 40 ){
      return this->getCutValue(Et,Electron_Cluster_Eta,Efficiency_required,h_MI10_max40_ptsum_DivEt);  
    }
  }
  if( MI == 15 ){
    if( max == 30 ){
      return this->getCutValue(Et,Electron_Cluster_Eta,Efficiency_required,h_MI15_max30_ptsum_DivEt);
    }
    if( max == 40 ){
      return this->getCutValue(Et,Electron_Cluster_Eta,Efficiency_required,h_MI15_max40_ptsum_DivEt);  
    }
  } 
  return 0;
}

/////////////////////////////////////////////////////////
// Correction tools                                    //
/////////////////////////////////////////////////////////
double EisoTool2012::EtconeCorrected(const unsigned int& NPV,
                                     const double& Electron_Cluster_E,
                                     const double& Electron_Cluster_Eta,
                                     const double& Electron_Etas2,
                                     const double& Electron_Etap,
                                     const double& Electron_Etcone_Uncorrected,
                                     const double& cone) const
{
  return CaloIsoCorrection::GetPtNPVCorrectedIsolation(
                            NPV,
                            Electron_Cluster_E,
                            Electron_Etas2,
                            Electron_Etap,
                            Electron_Cluster_Eta,
                            cone,
                            m_isMC,
                            Electron_Etcone_Uncorrected,
                            false,
                            CaloIsoCorrection::ELECTRON,
                            CaloIsoCorrection::REL17);
}

double EisoTool2012::TopoconeCorrected(const double& Electron_Cluster_E,
                                       const double& Electron_Cluster_Eta,
                                       const double& Electron_Etas2,
                                       const double& Electron_Etap,
                                       const double& Electron_ED_median,
                                       const double& Electron_topocone,
                                       const double& cone) const
{
  return CaloIsoCorrection::GetPtEDCorrectedTopoIsolation(
                Electron_ED_median,
                Electron_Cluster_E,
                Electron_Etas2,
                Electron_Etap,
                Electron_Cluster_Eta,
                cone,
                m_isMC,
                Electron_topocone,
                false,
                CaloIsoCorrection::ELECTRON,
                CaloIsoCorrection::REL17);

}


/////////////////////////////////////////////////////////
// Internal tools                                      //
/////////////////////////////////////////////////////////
double EisoTool2012::ElectronEt(const double& Electron_Cluster_E, const double& Electron_Track_Eta) const
{
  return Electron_Cluster_E / cosh( Electron_Track_Eta );
}

unsigned int EisoTool2012::PassOrFail(const double& Et,const double& Eta,const unsigned int& Efficiency_required, const double& value,TH3D* histo) const
{
  if( (value / m_GeV) < histo->GetBinContent(this->FindEtBin(Et),this->FindEtaBin(Eta),Efficiency_required) ){
    return 1;
  }
  return 0;
}

unsigned int EisoTool2012::PassOrFail_DivEt(const double& Et,const double& Eta,const unsigned int& Efficiency_required, const double& value,TH3D* histo) const
{
  if( (value / Et) < histo->GetBinContent(this->FindEtBin(Et),this->FindEtaBin(Eta),Efficiency_required) ){
    return 1;
  }
  return 0;
}

double EisoTool2012::getCutValue(const double& Et,const double& Eta,const unsigned int& Efficiency_required,TH3D* histo) const
{
  return histo->GetBinContent(this->FindEtBin(Et),this->FindEtaBin(Eta),Efficiency_required);
}

int EisoTool2012::FindEtBin(const double& Et) const
{
  int bin = h_etBins->FindBin(Et / m_GeV);
  if( bin > 13 ){bin = 13;}
  return bin;
}

int EisoTool2012::FindEtaBin(const double& Eta) const
{
  return h_etaBins->FindBin(Eta);
}

/////////////////////////////////////////////////////////
// Constructors, destructor and operator overloading   //
/////////////////////////////////////////////////////////


EisoTool2012::EisoTool2012() :
  m_GeV(1000.0),
  m_isMC(false),
  m_inputFileName("NULL"),
  
  s_PID_MediumPP("MediumPP"),
  s_PID_TightPP("TightPP"),
  s_PID_LH_Medium("LH_Medium"),
  s_PID_LH_Tight("LH_Tight"),
  s_PID_LH_VeryTight("LH_VeryTight"),
  
  s_etconeCor20("etconeCor20"),
  s_etconeCor30("etconeCor30"),
  s_ptcone20("ptcone20"),
  s_ptcone30("ptcone30"),
  s_topoconeCor20("topoconeCor20"),
  s_topoconeCor30("topoconeCor30"),
  s_MI10_max30_ptsum("MI10_max30_ptsum"),
  s_MI10_max40_ptsum("MI10_max40_ptsum"), 
  s_MI15_max30_ptsum("MI15_max30_ptsum"),
  s_MI15_max40_ptsum("MI15_max40_ptsum"),   
  
  s_etconeCor20_DivEt("etconeCor20_DivEt"),
  s_etconeCor30_DivEt("etconeCor30_DivEt"),
  s_ptcone20_DivEt("ptcone20_DivEt"),
  s_ptcone30_DivEt("ptcone30_DivEt"),
  s_topoconeCor20_DivEt("topoconeCor20_DivEt"),
  s_topoconeCor30_DivEt("topoconeCor30_DivEt"),   
  s_MI10_max30_ptsum_DivEt("MI10_max30_ptsum_DivEt"),
  s_MI10_max40_ptsum_DivEt("MI10_max40_ptsum_DivEt"), 
  s_MI15_max30_ptsum_DivEt("MI15_max30_ptsum_DivEt"),
  s_MI15_max40_ptsum_DivEt("MI15_max40_ptsum_DivEt"),   

  h_etconeCor20(NULL),
  h_etconeCor30(NULL),
  h_ptcone20(NULL),
  h_ptcone30(NULL),
  h_topoconeCor20(NULL),
  h_topoconeCor30(NULL),
  h_MI10_max30_ptsum(NULL),
  h_MI10_max40_ptsum(NULL),
  h_MI15_max30_ptsum(NULL),
  h_MI15_max40_ptsum(NULL),
  
  h_etconeCor20_DivEt(NULL),
  h_etconeCor30_DivEt(NULL),
  h_ptcone20_DivEt(NULL),
  h_ptcone30_DivEt(NULL),
  h_topoconeCor20_DivEt(NULL),
  h_topoconeCor30_DivEt(NULL),
  h_MI10_max30_ptsum_DivEt(NULL),
  h_MI10_max40_ptsum_DivEt(NULL),
  h_MI15_max30_ptsum_DivEt(NULL),
  h_MI15_max40_ptsum_DivEt(NULL),  
  
  h_etBins(NULL),
  h_etaBins(NULL)
{
  this->SetupBinningHistos();
}

EisoTool2012::EisoTool2012(const TString& inputFileName) :
  m_GeV(1000.0),
  m_isMC(false),
  m_inputFileName(inputFileName), 
  
  s_PID_MediumPP("MediumPP"),
  s_PID_TightPP("TightPP"),
  s_PID_LH_Medium("LH_Medium"),
  s_PID_LH_Tight("LH_Tight"),
  s_PID_LH_VeryTight("LH_VeryTight"),
    
  s_etconeCor20("etconeCor20"),
  s_etconeCor30("etconeCor30"),
  s_ptcone20("ptcone20"),
  s_ptcone30("ptcone30"),
  s_topoconeCor20("topoconeCor20"),
  s_topoconeCor30("topoconeCor30"),
  s_MI10_max30_ptsum("MI10_max30_ptsum"),
  s_MI10_max40_ptsum("MI10_max40_ptsum"), 
  s_MI15_max30_ptsum("MI15_max30_ptsum"),
  s_MI15_max40_ptsum("MI15_max40_ptsum"),   
  
  s_etconeCor20_DivEt("etconeCor20_DivEt"),
  s_etconeCor30_DivEt("etconeCor30_DivEt"),
  s_ptcone20_DivEt("ptcone20_DivEt"),
  s_ptcone30_DivEt("ptcone30_DivEt"),
  s_topoconeCor20_DivEt("topoconeCor20_DivEt"),
  s_topoconeCor30_DivEt("topoconeCor30_DivEt"),   
  s_MI10_max30_ptsum_DivEt("MI10_max30_ptsum_DivEt"),
  s_MI10_max40_ptsum_DivEt("MI10_max40_ptsum_DivEt"), 
  s_MI15_max30_ptsum_DivEt("MI15_max30_ptsum_DivEt"),
  s_MI15_max40_ptsum_DivEt("MI15_max40_ptsum_DivEt"),
  
  h_etconeCor20(NULL),
  h_etconeCor30(NULL),
  h_ptcone20(NULL),
  h_ptcone30(NULL),
  h_topoconeCor20(NULL),
  h_topoconeCor30(NULL),
  h_MI10_max30_ptsum(NULL),
  h_MI10_max40_ptsum(NULL),
  h_MI15_max30_ptsum(NULL),
  h_MI15_max40_ptsum(NULL),
  
  h_etconeCor20_DivEt(NULL),
  h_etconeCor30_DivEt(NULL),
  h_ptcone20_DivEt(NULL),
  h_ptcone30_DivEt(NULL),
  h_topoconeCor20_DivEt(NULL),
  h_topoconeCor30_DivEt(NULL),
  h_MI10_max30_ptsum_DivEt(NULL),
  h_MI10_max40_ptsum_DivEt(NULL),
  h_MI15_max30_ptsum_DivEt(NULL),
  h_MI15_max40_ptsum_DivEt(NULL),   
  
  h_etBins(NULL),
  h_etaBins(NULL) 
{
  this->SetupBinningHistos();  
  this->SetPID_TightPP();
}

EisoTool2012::EisoTool2012(const TString& inputFileName,const bool isMC) :
  m_GeV(1000.0),
  m_isMC(isMC),
  m_inputFileName(inputFileName),
  
  s_PID_MediumPP("MediumPP"),
  s_PID_TightPP("TightPP"),
  s_PID_LH_Medium("LH_Medium"),
  s_PID_LH_Tight("LH_Tight"),
  s_PID_LH_VeryTight("LH_VeryTight"),
    
  s_etconeCor20("etconeCor20"),
  s_etconeCor30("etconeCor30"),
  s_ptcone20("ptcone20"),
  s_ptcone30("ptcone30"),
  s_topoconeCor20("topoconeCor20"), 
  s_topoconeCor30("topoconeCor30"),   
  s_MI10_max30_ptsum("MI10_max30_ptsum"),
  s_MI10_max40_ptsum("MI10_max40_ptsum"), 
  s_MI15_max30_ptsum("MI15_max30_ptsum"),
  s_MI15_max40_ptsum("MI15_max40_ptsum"),   
  
  s_etconeCor20_DivEt("etconeCor20_DivEt"),
  s_etconeCor30_DivEt("etconeCor30_DivEt"),
  s_ptcone20_DivEt("ptcone20_DivEt"),
  s_ptcone30_DivEt("ptcone30_DivEt"),
  s_topoconeCor20_DivEt("topoconeCor20_DivEt"),
  s_topoconeCor30_DivEt("topoconeCor30_DivEt"),   
  s_MI10_max30_ptsum_DivEt("MI10_max30_ptsum_DivEt"),
  s_MI10_max40_ptsum_DivEt("MI10_max40_ptsum_DivEt"), 
  s_MI15_max30_ptsum_DivEt("MI15_max30_ptsum_DivEt"),
  s_MI15_max40_ptsum_DivEt("MI15_max40_ptsum_DivEt"),
  
  h_etconeCor20(NULL),
  h_etconeCor30(NULL),
  h_ptcone20(NULL),
  h_ptcone30(NULL),
  h_topoconeCor20(NULL),
  h_topoconeCor30(NULL),
  h_MI10_max30_ptsum(NULL),
  h_MI10_max40_ptsum(NULL),
  h_MI15_max30_ptsum(NULL),
  h_MI15_max40_ptsum(NULL),
  
  h_etconeCor20_DivEt(NULL),
  h_etconeCor30_DivEt(NULL),
  h_ptcone20_DivEt(NULL),
  h_ptcone30_DivEt(NULL),
  h_topoconeCor20_DivEt(NULL),
  h_topoconeCor30_DivEt(NULL),
  h_MI10_max30_ptsum_DivEt(NULL),
  h_MI10_max40_ptsum_DivEt(NULL),
  h_MI15_max30_ptsum_DivEt(NULL),
  h_MI15_max40_ptsum_DivEt(NULL),  
  
  h_etBins(NULL),
  h_etaBins(NULL) 
{
  this->SetupBinningHistos();  
  this->SetPID_TightPP();
}


EisoTool2012::EisoTool2012(const EisoTool2012& rhs) :
  m_GeV(1000.0),
  m_isMC(false),
  m_inputFileName("NULL"),
  
  s_PID_MediumPP("MediumPP"),
  s_PID_TightPP("TightPP"),
  s_PID_LH_Medium("LH_Medium"),
  s_PID_LH_Tight("LH_Tight"),
  s_PID_LH_VeryTight("LH_VeryTight"),
    
  s_etconeCor20("etconeCor20"),
  s_etconeCor30("etconeCor30"),
  s_ptcone20("ptcone20"),
  s_ptcone30("ptcone30"),
  s_topoconeCor20("topoconeCor20"),
  s_topoconeCor30("topoconeCor30"),   
  s_MI10_max30_ptsum("MI10_max30_ptsum"),
  s_MI10_max40_ptsum("MI10_max40_ptsum"), 
  s_MI15_max30_ptsum("MI15_max30_ptsum"),
  s_MI15_max40_ptsum("MI15_max40_ptsum"),   
  
  s_etconeCor20_DivEt("etconeCor20_DivEt"),
  s_etconeCor30_DivEt("etconeCor30_DivEt"),
  s_ptcone20_DivEt("ptcone20_DivEt"),
  s_ptcone30_DivEt("ptcone30_DivEt"),
  s_topoconeCor20_DivEt("topoconeCor20_DivEt"),
  s_topoconeCor30_DivEt("topoconeCor30_DivEt"),   
  s_MI10_max30_ptsum_DivEt("MI10_max30_ptsum_DivEt"),
  s_MI10_max40_ptsum_DivEt("MI10_max40_ptsum_DivEt"), 
  s_MI15_max30_ptsum_DivEt("MI15_max30_ptsum_DivEt"),
  s_MI15_max40_ptsum_DivEt("MI15_max40_ptsum_DivEt"),
  
  h_etconeCor20(NULL),
  h_etconeCor30(NULL),
  h_ptcone20(NULL),
  h_ptcone30(NULL),
  h_topoconeCor20(NULL),
  h_topoconeCor30(NULL),
  h_MI10_max30_ptsum(NULL),
  h_MI10_max40_ptsum(NULL),
  h_MI15_max30_ptsum(NULL),
  h_MI15_max40_ptsum(NULL),
  
  h_etconeCor20_DivEt(NULL),
  h_etconeCor30_DivEt(NULL),
  h_ptcone20_DivEt(NULL),
  h_ptcone30_DivEt(NULL),
  h_topoconeCor20_DivEt(NULL),
  h_topoconeCor30_DivEt(NULL),
  h_MI10_max30_ptsum_DivEt(NULL),
  h_MI10_max40_ptsum_DivEt(NULL),
  h_MI15_max30_ptsum_DivEt(NULL),
  h_MI15_max40_ptsum_DivEt(NULL),  
  
  h_etBins(NULL),
  h_etaBins(NULL) 
{
  m_GeV = rhs.m_GeV;
  m_isMC = rhs.m_isMC;

  m_inputFileName = rhs.m_inputFileName;
  
  s_PID_MediumPP = rhs.s_PID_MediumPP;
  s_PID_TightPP = rhs.s_PID_TightPP;
  s_PID_LH_Medium = rhs.s_PID_LH_Medium;
  s_PID_LH_Tight = rhs.s_PID_LH_Tight;
  s_PID_LH_VeryTight = rhs.s_PID_LH_VeryTight;  

  s_etconeCor20 = rhs.s_etconeCor20;
  s_etconeCor30 = rhs.s_etconeCor30;
  s_ptcone20 = rhs.s_ptcone20;
  s_ptcone30 = rhs.s_ptcone30;
  s_topoconeCor20 = rhs.s_topoconeCor20;
  s_topoconeCor30 = rhs.s_topoconeCor30;
  s_MI10_max30_ptsum = rhs.s_MI10_max30_ptsum;
  s_MI10_max40_ptsum = rhs.s_MI10_max40_ptsum;  
  s_MI15_max30_ptsum = rhs.s_MI15_max30_ptsum;
  s_MI15_max40_ptsum = rhs.s_MI15_max40_ptsum;   
  
  s_etconeCor20_DivEt = rhs.s_etconeCor20_DivEt;
  s_etconeCor30_DivEt = rhs.s_etconeCor30_DivEt;
  s_ptcone20_DivEt = rhs.s_ptcone20_DivEt;
  s_ptcone30_DivEt = rhs.s_ptcone30_DivEt;
  s_topoconeCor20_DivEt = rhs.s_topoconeCor20_DivEt;
  s_topoconeCor30_DivEt = rhs.s_topoconeCor30_DivEt;
  s_MI10_max30_ptsum_DivEt = rhs.s_MI10_max30_ptsum_DivEt;
  s_MI10_max40_ptsum_DivEt = rhs.s_MI10_max40_ptsum_DivEt;  
  s_MI15_max30_ptsum_DivEt = rhs.s_MI15_max30_ptsum_DivEt;
  s_MI15_max40_ptsum_DivEt = rhs.s_MI15_max40_ptsum_DivEt;   
  
  h_etconeCor20 = (TH3D*)rhs.h_etconeCor20->Clone();
  h_etconeCor30 = (TH3D*)rhs.h_etconeCor30->Clone();
  h_ptcone20 = (TH3D*)rhs.h_ptcone20->Clone();
  h_ptcone30 = (TH3D*)rhs.h_ptcone30->Clone();
  h_topoconeCor20 = (TH3D*)rhs.h_topoconeCor20->Clone();
  h_topoconeCor30 = (TH3D*)rhs.h_topoconeCor30->Clone();
  h_MI10_max30_ptsum = (TH3D*)rhs.h_MI10_max30_ptsum->Clone();
  h_MI10_max40_ptsum = (TH3D*)rhs.h_MI10_max40_ptsum->Clone();  
  h_MI15_max30_ptsum = (TH3D*)rhs.h_MI15_max30_ptsum->Clone();
  h_MI15_max40_ptsum = (TH3D*)rhs.h_MI15_max40_ptsum->Clone();   
  
  h_etconeCor20_DivEt = (TH3D*)rhs.h_etconeCor20_DivEt->Clone();
  h_etconeCor30_DivEt = (TH3D*)rhs.h_etconeCor30_DivEt->Clone();
  h_ptcone20_DivEt = (TH3D*)rhs.h_ptcone20_DivEt->Clone();
  h_ptcone30_DivEt = (TH3D*)rhs.h_ptcone30_DivEt->Clone();
  h_topoconeCor20_DivEt = (TH3D*)rhs.h_topoconeCor20_DivEt->Clone();
  h_topoconeCor30_DivEt = (TH3D*)rhs.h_topoconeCor30_DivEt->Clone();
  h_MI10_max30_ptsum_DivEt = (TH3D*)rhs.h_MI10_max30_ptsum_DivEt->Clone();
  h_MI10_max40_ptsum_DivEt = (TH3D*)rhs.h_MI10_max40_ptsum_DivEt->Clone();  
  h_MI15_max30_ptsum_DivEt = (TH3D*)rhs.h_MI15_max30_ptsum_DivEt->Clone();
  h_MI15_max40_ptsum_DivEt = (TH3D*)rhs.h_MI15_max40_ptsum_DivEt->Clone();  
  
  h_etBins = (TH1D*)rhs.h_etBins->Clone();
  h_etaBins = (TH1D*)rhs.h_etaBins->Clone();
}


EisoTool2012& EisoTool2012::operator=(const EisoTool2012& rhs)
{
  if( this != &rhs ){
    m_GeV = rhs.m_GeV;
    m_isMC = rhs.m_isMC;

    m_inputFileName = rhs.m_inputFileName;
    
    s_PID_MediumPP = rhs.s_PID_MediumPP;
    s_PID_TightPP = rhs.s_PID_TightPP;
    s_PID_LH_Medium = rhs.s_PID_LH_Medium;
    s_PID_LH_Tight = rhs.s_PID_LH_Tight;
    s_PID_LH_VeryTight = rhs.s_PID_LH_VeryTight;  

    s_etconeCor20 = rhs.s_etconeCor20;
    s_etconeCor30 = rhs.s_etconeCor30;
    s_ptcone20 = rhs.s_ptcone20;
    s_ptcone30 = rhs.s_ptcone30;
    s_topoconeCor20 = rhs.s_topoconeCor20;
    s_topoconeCor30 = rhs.s_topoconeCor30;
    s_MI10_max30_ptsum = rhs.s_MI10_max30_ptsum;
    s_MI10_max40_ptsum = rhs.s_MI10_max40_ptsum;  
    s_MI15_max30_ptsum = rhs.s_MI15_max30_ptsum;
    s_MI15_max40_ptsum = rhs.s_MI15_max40_ptsum;   
    
    s_etconeCor20_DivEt = rhs.s_etconeCor20_DivEt;
    s_etconeCor30_DivEt = rhs.s_etconeCor30_DivEt;
    s_ptcone20_DivEt = rhs.s_ptcone20_DivEt;
    s_ptcone30_DivEt = rhs.s_ptcone30_DivEt;
    s_topoconeCor20_DivEt = rhs.s_topoconeCor20_DivEt;
    s_topoconeCor30_DivEt = rhs.s_topoconeCor30_DivEt;
    s_MI10_max30_ptsum_DivEt = rhs.s_MI10_max30_ptsum_DivEt;
    s_MI10_max40_ptsum_DivEt = rhs.s_MI10_max40_ptsum_DivEt;  
    s_MI15_max30_ptsum_DivEt = rhs.s_MI15_max30_ptsum_DivEt;
    s_MI15_max40_ptsum_DivEt = rhs.s_MI15_max40_ptsum_DivEt;  
  
    TH3D* tmp_etconeCor20 = 0;
    TH3D* tmp_etconeCor30 = 0;
    TH3D* tmp_ptcone20 = 0;
    TH3D* tmp_ptcone30 = 0;
    TH3D* tmp_topoconeCor20 = 0;
    TH3D* tmp_topoconeCor30 = 0;
    TH3D* tmp_MI10_max30_ptsum = 0;
    TH3D* tmp_MI10_max40_ptsum = 0;    
    TH3D* tmp_MI15_max30_ptsum = 0;
    TH3D* tmp_MI15_max40_ptsum = 0;
    
    TH3D* tmp_etconeCor20_DivEt = 0;
    TH3D* tmp_etconeCor30_DivEt = 0;
    TH3D* tmp_ptcone20_DivEt = 0;
    TH3D* tmp_ptcone30_DivEt = 0;
    TH3D* tmp_topoconeCor20_DivEt = 0;
    TH3D* tmp_topoconeCor30_DivEt = 0;
    TH3D* tmp_MI10_max30_ptsum_DivEt = 0;
    TH3D* tmp_MI10_max40_ptsum_DivEt = 0;    
    TH3D* tmp_MI15_max30_ptsum_DivEt = 0;
    TH3D* tmp_MI15_max40_ptsum_DivEt = 0;    
    
    TH1D* tmp_etBins = 0;
    TH1D* tmp_etaBins = 0;

    try{
      tmp_etconeCor20 = (TH3D*)rhs.h_etconeCor20->Clone();
      tmp_etconeCor30 = (TH3D*)rhs.h_etconeCor30->Clone();
      tmp_ptcone20 = (TH3D*)rhs.h_ptcone20->Clone();
      tmp_ptcone30 = (TH3D*)rhs.h_ptcone30->Clone();
      tmp_topoconeCor20 = (TH3D*)rhs.h_topoconeCor20->Clone();
      tmp_topoconeCor30 = (TH3D*)rhs.h_topoconeCor30->Clone();
      tmp_MI10_max30_ptsum = (TH3D*)rhs.h_MI10_max30_ptsum->Clone();
      tmp_MI10_max40_ptsum = (TH3D*)rhs.h_MI10_max40_ptsum->Clone();      
      tmp_MI15_max30_ptsum = (TH3D*)rhs.h_MI15_max30_ptsum->Clone();
      tmp_MI15_max40_ptsum = (TH3D*)rhs.h_MI15_max40_ptsum->Clone(); 
      
      tmp_etconeCor20_DivEt = (TH3D*)rhs.h_etconeCor20_DivEt->Clone();
      tmp_etconeCor30_DivEt = (TH3D*)rhs.h_etconeCor30_DivEt->Clone();
      tmp_ptcone20_DivEt = (TH3D*)rhs.h_ptcone20_DivEt->Clone();
      tmp_ptcone30_DivEt = (TH3D*)rhs.h_ptcone30_DivEt->Clone();
      tmp_topoconeCor20_DivEt = (TH3D*)rhs.h_topoconeCor20_DivEt->Clone();
      tmp_topoconeCor30_DivEt = (TH3D*)rhs.h_topoconeCor30_DivEt->Clone();
      tmp_MI10_max30_ptsum_DivEt = (TH3D*)rhs.h_MI10_max30_ptsum_DivEt->Clone();
      tmp_MI10_max40_ptsum_DivEt = (TH3D*)rhs.h_MI10_max40_ptsum_DivEt->Clone();      
      tmp_MI15_max30_ptsum_DivEt = (TH3D*)rhs.h_MI15_max30_ptsum_DivEt->Clone();
      tmp_MI15_max40_ptsum_DivEt = (TH3D*)rhs.h_MI15_max40_ptsum_DivEt->Clone();      
      
      tmp_etBins = (TH1D*)rhs.h_etBins->Clone();
      tmp_etaBins = (TH1D*)rhs.h_etaBins->Clone();
    }
    catch(...){
      delete tmp_etconeCor20;
      delete tmp_etconeCor30;
      delete tmp_ptcone20;
      delete tmp_ptcone30;
      delete tmp_topoconeCor20;
      delete tmp_topoconeCor30;
      delete tmp_MI10_max30_ptsum;
      delete tmp_MI10_max40_ptsum;
      delete tmp_MI15_max30_ptsum;
      delete tmp_MI15_max40_ptsum;
      
      delete tmp_etconeCor20_DivEt;
      delete tmp_etconeCor30_DivEt;
      delete tmp_ptcone20_DivEt;
      delete tmp_ptcone30_DivEt;
      delete tmp_topoconeCor20_DivEt;
      delete tmp_topoconeCor30_DivEt;
      delete tmp_MI10_max30_ptsum_DivEt;
      delete tmp_MI10_max40_ptsum_DivEt;
      delete tmp_MI15_max30_ptsum_DivEt;
      delete tmp_MI15_max40_ptsum_DivEt;      
      
      delete tmp_etBins;
      delete tmp_etaBins;
    }

    delete h_etconeCor20;
    delete h_etconeCor30;
    delete h_ptcone20;
    delete h_ptcone30;
    delete h_topoconeCor20;
    delete h_topoconeCor30;
    delete h_MI10_max30_ptsum;
    delete h_MI10_max40_ptsum;
    delete h_MI15_max30_ptsum;
    delete h_MI15_max40_ptsum;
    
    delete h_etconeCor20_DivEt;
    delete h_etconeCor30_DivEt;
    delete h_ptcone20_DivEt;
    delete h_ptcone30_DivEt;
    delete h_topoconeCor20_DivEt;
    delete h_topoconeCor30_DivEt;
    delete h_MI10_max30_ptsum_DivEt;
    delete h_MI10_max40_ptsum_DivEt;
    delete h_MI15_max30_ptsum_DivEt;
    delete h_MI15_max40_ptsum_DivEt;    
    
    delete h_etBins;
    delete h_etaBins;

    h_etconeCor20 = tmp_etconeCor20;
    h_etconeCor30 = tmp_etconeCor30;
    h_ptcone20 = tmp_ptcone20;
    h_ptcone30 = tmp_ptcone30;
    h_topoconeCor20 = tmp_topoconeCor20;
    h_topoconeCor30 = tmp_topoconeCor30;
    h_MI10_max30_ptsum = tmp_MI10_max30_ptsum;
    h_MI10_max40_ptsum = tmp_MI10_max40_ptsum;    
    h_MI15_max30_ptsum = tmp_MI15_max30_ptsum;
    h_MI15_max40_ptsum = tmp_MI15_max40_ptsum;    
    
    h_etconeCor20_DivEt = tmp_etconeCor20_DivEt;
    h_etconeCor30_DivEt = tmp_etconeCor30_DivEt;
    h_ptcone20_DivEt = tmp_ptcone20_DivEt;
    h_ptcone30_DivEt = tmp_ptcone30_DivEt;
    h_topoconeCor20_DivEt = tmp_topoconeCor20_DivEt;
    h_topoconeCor30_DivEt = tmp_topoconeCor30_DivEt;
    h_MI10_max30_ptsum_DivEt = tmp_MI10_max30_ptsum_DivEt;
    h_MI10_max40_ptsum_DivEt = tmp_MI10_max40_ptsum_DivEt;    
    h_MI15_max30_ptsum_DivEt = tmp_MI15_max30_ptsum_DivEt;
    h_MI15_max40_ptsum_DivEt = tmp_MI15_max40_ptsum_DivEt;       
    
    h_etBins = tmp_etBins;
    h_etaBins = tmp_etaBins;
  }
  return *this;
}

EisoTool2012::~EisoTool2012()
{
  delete h_etBins;
  delete h_etaBins;
}

/////////////////////////////////////////////////////////
// Setup Names and Load histograms from file           //
/////////////////////////////////////////////////////////


void EisoTool2012::LoadHistograms(const TString& pid)
{
  TFile* inputFile = new TFile(m_inputFileName,"READ");
  
  TString path = "/" + pid + "/" + pid + "_";
  
  h_etconeCor20 = (TH3D*)inputFile->Get( path + s_etconeCor20 )->Clone();
  h_etconeCor30 = (TH3D*)inputFile->Get( path + s_etconeCor30 )->Clone();
  h_ptcone20 = (TH3D*)inputFile->Get( path + s_ptcone20 )->Clone();
  h_ptcone30 = (TH3D*)inputFile->Get( path + s_ptcone30 )->Clone();
  h_topoconeCor20 = (TH3D*)inputFile->Get( path + s_topoconeCor20 )->Clone(); 
  h_topoconeCor30 = (TH3D*)inputFile->Get( path + s_topoconeCor30 )->Clone();   
  h_MI10_max30_ptsum = (TH3D*)inputFile->Get( path + s_MI10_max30_ptsum )->Clone(); 
  h_MI10_max40_ptsum = (TH3D*)inputFile->Get( path + s_MI10_max40_ptsum )->Clone();
  h_MI15_max30_ptsum = (TH3D*)inputFile->Get( path + s_MI15_max30_ptsum )->Clone();
  h_MI15_max40_ptsum = (TH3D*)inputFile->Get( path + s_MI15_max40_ptsum )->Clone();  
  h_etconeCor20_DivEt = (TH3D*)inputFile->Get( path + s_etconeCor20_DivEt )->Clone();
  h_etconeCor30_DivEt = (TH3D*)inputFile->Get( path + s_etconeCor30_DivEt )->Clone();
  h_ptcone20_DivEt = (TH3D*)inputFile->Get( path + s_ptcone20_DivEt )->Clone();
  h_ptcone30_DivEt = (TH3D*)inputFile->Get( path + s_ptcone30_DivEt )->Clone();
  h_topoconeCor20_DivEt = (TH3D*)inputFile->Get( path + s_topoconeCor20_DivEt )->Clone();
  h_topoconeCor30_DivEt = (TH3D*)inputFile->Get( path + s_topoconeCor30_DivEt )->Clone();   
  h_MI10_max30_ptsum_DivEt = (TH3D*)inputFile->Get( path + s_MI10_max30_ptsum_DivEt )->Clone();
  h_MI10_max40_ptsum_DivEt = (TH3D*)inputFile->Get( path + s_MI10_max40_ptsum_DivEt )->Clone();
  h_MI15_max30_ptsum_DivEt = (TH3D*)inputFile->Get( path + s_MI15_max30_ptsum_DivEt )->Clone();
  h_MI15_max40_ptsum_DivEt = (TH3D*)inputFile->Get( path + s_MI15_max40_ptsum_DivEt )->Clone();  

}

void EisoTool2012::SetPID_MediumPP()
{
  this->LoadHistograms(s_PID_MediumPP);
}
void EisoTool2012::SetPID_TightPP()
{
  this->LoadHistograms(s_PID_TightPP);
}
void EisoTool2012::SetPID_LH_Medium()
{
  this->LoadHistograms(s_PID_LH_Medium);
}
void EisoTool2012::SetPID_LH_Tight()
{
  this->LoadHistograms(s_PID_LH_Tight);
}
void EisoTool2012::SetPID_LH_VeryTight()
{
  this->LoadHistograms(s_PID_LH_VeryTight);
}

void EisoTool2012::SetupBinningHistos()
{

  const double ArrayEt[14]={0.0,7.0,10.0,15.0,20.0,25.0,30.0,35.0,40.0,45.0,50.0,60.0,80.0,8000.0};
                            

  const double ArrayEta[21]={-2.47,-2.37,-2.01,-1.81,-1.52,-1.37,-1.15,-0.8,-0.6,-0.1,
                             0.0,0.1,0.6,0.8,1.15,1.37,1.52,1.81,2.01,2.37,2.47};
  

  h_etBins = new TH1D("egammaAnalysisUtils_EisoTool2012_etBins","egammaAnalysisUtils_EisoTool2012_etBins",13,ArrayEt);
  h_etaBins = new TH1D("egammaAnalysisUtils_EisoTool2012_etaBins","egammaAnalysisUtils_EisoTool2012_etaBins",20,ArrayEta);
  h_etBins->SetDirectory(0);
  h_etaBins->SetDirectory(0);
}
