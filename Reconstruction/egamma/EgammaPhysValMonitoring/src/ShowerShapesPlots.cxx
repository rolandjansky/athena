/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ShowerShapesPlots.h"

namespace Egamma{


ShowerShapesPlots::ShowerShapesPlots(PlotBase* pParent, std::string sDir, std::string sParticleType):PlotBase(pParent, sDir), 
m_sParticleType(sParticleType)
{}	

void ShowerShapesPlots::initializePlots(){

  hadleak = Book1D("hadleak", "Hadronic leakage of " + m_sParticleType+"; E_{hadleak} (GeV);Entries", 100, -0.07, 0.13); 
  middlee = Book1D("middlee", "E_{middle} of "+ m_sParticleType+"; E_{277} (GeV); Entries" , 210,-10.,200.);
  reta37  = Book1D("reta37", "R_{#eta37} of "+ m_sParticleType+";R_{#eta37};Entries", 355, 0., 1.1005);
  reta33  = Book1D("reta33", "R_{#eta33} of "+ m_sParticleType+";R_{#eta33};Entries", 355, 0., 1.1005);
  weta1   = Book1D("weta1", "W_{#etas1} of "+ m_sParticleType+";W_{#etas1};Entries", 100, 0., 1.);
  weta2   = Book1D("weta2", "W_{#etas2} of "+ m_sParticleType+";W_{#etas2};Entries", 100, 0., 0.03);
  demax1  = Book1D("demax1", "#DeltaE_{maxs1} of "+ m_sParticleType+";#DeltaE_{maxs1};Entries" , 100, 0., 1.);
  demax2  = Book1D("demax2", "#DeltaE_{maxs2} of "+ m_sParticleType+";#DeltaE_{maxs2};Entries" , 80, 0., 0.8);
  de      = Book1D("de", "#DeltaE of "+ m_sParticleType+";#DeltaE (GeV);Entries", 250, 0., 0.5);
  fracs1  = Book1D("fracs1", "Fracs1 of "+ m_sParticleType+";Fracs1;Entries", 350, 0., 3.5);
  wtots1  = Book1D("wtots1", "W_{tots1} of "+ m_sParticleType+";W_{tots1};Entries", 100, 0., 10.);
  f1      = Book1D("f1", "f1 of "+ m_sParticleType+";f1;Entries" , 100, 0., 1.0);
  pos7    = Book1D("pos7", "pos7 of "+ m_sParticleType+";pos7;Entries", 80, -4., 4.);
  clusiso = Book1D("clusiso","Cluster Isolation of "+ m_sParticleType+"; E_{T}^{cone20}/E_{T};Entries", 100, 0., 1.);

  hadleakvset = Book2D("hadleakvset", "Hadronic leakage vs E_{T} of " + m_sParticleType+"; E_{hadleak} (GeV) ; E_{T} (GeV) ", 100, -0.07, 0.13, 200, 0., 200); 
  middleevset = Book2D("middleevset", "E_{middle} vs E_{T} of "+ m_sParticleType+"; E_{277} (GeV); E_{T} (GeV) " , 210,-10.,200., 200, 0., 200);
  reta37vset  = Book2D("reta37vset", "R_{#eta37} vs E_{T} of "+ m_sParticleType+";R_{#eta37}; E_{T} (GeV) ", 355, 0., 1.1005, 200, 0., 200);
  reta33vset  = Book2D("reta33vset", "R_{#eta33} vs E_{T} of "+ m_sParticleType+";R_{#eta33}; E_{T} (GeV) ", 355, 0., 1.1005, 200, 0., 200);
  weta1vset   = Book2D("weta1vset", "W_{#etas1} vs E_{T} of "+ m_sParticleType+";W_{#etas1}; E_{T} (GeV) ", 100, 0., 1., 200, 0., 200);
  weta2vset   = Book2D("weta2vset", "W_{#etas2} vs E_{T} of "+ m_sParticleType+";W_{#etas2}; E_{T} (GeV) ", 100, 0., 0.03, 200, 0., 200);
  demax1vset  = Book2D("demax1vset", "#DeltaE_{maxs1} vs E_{T} of "+ m_sParticleType+";#DeltaE_{maxs1}; E_{T} (GeV) " , 100, 0., 1., 200, 0., 200);
  demax2vset  = Book2D("demax2vset", "#DeltaE_{maxs2} vs E_{T} of "+ m_sParticleType+";#DeltaE_{maxs2}; E_{T} (GeV) " , 80, 0., 0.8, 200, 0., 200);
  devset      = Book2D("devset", "#DeltaE vs E_{T} of "+ m_sParticleType+";#DeltaE (GeV); E_{T} (GeV) ", 250, 0., 0.5, 200, 0., 200);
  fracs1vset  = Book2D("fracs1vset", "Fracs1 vs E_{T} of "+ m_sParticleType+";Fracs1; E_{T} (GeV) ", 350, 0., 3.5, 200, 0., 200);
  wtots1vset  = Book2D("wtots1vset", "W_{tots1} vsE_{T} of "+ m_sParticleType+";W_{tots1}; E_{T} (GeV) ", 100, 0., 10., 200, 0., 200);
  f1vset      = Book2D("f1vset", "f1 vs E_{T} of "+ m_sParticleType+";f1; E_{T} (GeV) " , 100, 0., 1.0, 200, 0., 200);
  pos7vset    = Book2D("pos7vset", "pos7 vs E_{T} of "+ m_sParticleType+";pos7; E_{T} (GeV) ", 80, -4., 4., 200, 0., 200);
  clusisovset = Book2D("clusisovset","Cluster Isolation vs E_{T} of "+ m_sParticleType+"; E_{T}^{cone20}/E_{T}; E_{T} (GeV) ", 100, 0., 1., 200, 0., 200);

}

void ShowerShapesPlots::fill(const xAOD::Egamma& egamma){


  float eta2 = fabs (egamma.caloCluster()->etaBE (2));  
  float et37 = egamma.caloCluster()->e() / cosh (eta2);
  float ethad,ethad1;
  egamma.showerShapeValue(ethad, xAOD::EgammaParameters::ethad );
  egamma.showerShapeValue(ethad1, xAOD::EgammaParameters::ethad1 );

  float raphad =  et37 > 0. ? ethad1 / et37 : 0.;
  float raphad1 = et37 > 0. ? ethad / et37 : 0.;
  float hadrleak = (eta2 >= 0.8 && eta2 < 1.37) ? raphad : raphad1;

  hadleak->Fill(hadrleak);

  hadleakvset->Fill(hadrleak, egamma.pt()*0.001);

  float e277,e237,e233;
  egamma.showerShapeValue(e277, xAOD::EgammaParameters::e277 );
  egamma.showerShapeValue(e237, xAOD::EgammaParameters::e237 );
  egamma.showerShapeValue(e233, xAOD::EgammaParameters::e233 );
  middlee->Fill(e277*0.001);
  middleevset->Fill(e277*0.001, egamma.pt()*0.001);

  float rateta37 =   e277!= 0. ? e237/e277:0.;
  float rateta33 =   e277!= 0. ? e233/e277:0.; 
  reta37->Fill(rateta37);
  reta33->Fill(rateta33);

  reta37vset->Fill(rateta37, egamma.pt()*0.001);
  reta33vset->Fill(rateta33, egamma.pt()*0.001);

  float shweta1,shweta2;
  egamma.showerShapeValue(shweta1, xAOD::EgammaParameters::weta1 );
  egamma.showerShapeValue(shweta2, xAOD::EgammaParameters::weta2 );
  weta1->Fill(shweta1);
  weta2->Fill(shweta2);

  weta1vset->Fill(shweta1, egamma.pt()*0.001);
  weta2vset->Fill(shweta2, egamma.pt()*0.001);

  float emax,emin,emax2;
  egamma.showerShapeValue(emax, xAOD::EgammaParameters::emaxs1 );
  egamma.showerShapeValue(emin, xAOD::EgammaParameters::emins1 );
  egamma.showerShapeValue(emax2, xAOD::EgammaParameters::e2tsts1 );

  float deltemax1 = fabs (emax + emax2) > 0. ? (emax - emax2) / (emax + emax2) : 0.; 
  demax1->Fill(deltemax1);
  demax2->Fill(emax2 / (1. + 0.009 * et37*0.001)*0.001); // use more official GeV
  de->Fill( (emax2 - emin) * 0.001); // use more official GeV

  demax1vset->Fill(deltemax1, egamma.pt()*0.001);
  demax2vset->Fill(emax2 / (1. + 0.009 * et37*0.001)*0.001, egamma.pt()*0.001); // use more official GeV
  devset->Fill( (emax2 - emin) * 0.001, egamma.pt()*0.001); // use more official GeV

  float shfracs1,shwtots1,fracf1,difpos7;
  egamma.showerShapeValue(shfracs1, xAOD::EgammaParameters::fracs1 );
  egamma.showerShapeValue(shwtots1, xAOD::EgammaParameters::wtots1 );
  egamma.showerShapeValue(fracf1, xAOD::EgammaParameters::f1 );
  egamma.showerShapeValue(difpos7, xAOD::EgammaParameters:: pos7);
  
  fracs1->Fill(shfracs1);
  wtots1->Fill(shwtots1);
  f1->Fill(fracf1);
  pos7->Fill(difpos7);

  fracs1vset->Fill(shfracs1, egamma.pt()*0.001);
  wtots1vset->Fill(shwtots1, egamma.pt()*0.001);
  f1vset->Fill(fracf1, egamma.pt()*0.001);
  pos7vset->Fill(difpos7, egamma.pt()*0.001);
  
  float EtconeIso, shetcone20;
  egamma.isolationValue(shetcone20,   xAOD::EgammaParameters::etcone20);
  EtconeIso = et37!=0. ? shetcone20/et37 : 0.; 
  clusiso->Fill(EtconeIso);
  clusisovset->Fill(EtconeIso, egamma.pt()*0.001);
}

}
