/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <utility>

#include "ShowerShapesPlots.h"

using CLHEP::GeV;

namespace Egamma{


  ShowerShapesPlots::ShowerShapesPlots(PlotBase* pParent, const std::string& sDir, std::string sParticleType):PlotBase(pParent, sDir), 
												       m_sParticleType(std::move(sParticleType)),
												       hadleak(nullptr), 
												       middlee(nullptr), 
												       reta37 (nullptr), 
												       reta33 (nullptr), 
												       weta1  (nullptr), 
												       weta2  (nullptr), 
												       demax1 (nullptr), 
												       demax2 (nullptr), 
												       de     (nullptr), 
												       fracs1 (nullptr), 
												       wtots1 (nullptr), 
												       f1     (nullptr),
												       f3     (nullptr),
												       pos7   (nullptr), 
												       clusiso(nullptr),
												       Eratio(nullptr),
												       E0(nullptr),
												       E1(nullptr),
												       E2(nullptr),
												       E3(nullptr),
												       Rhad(nullptr),
												       Rhad1(nullptr),
												       Reta(nullptr),
												       Rphi(nullptr),
												       hadleakvset(nullptr),  
												       middleevset(nullptr), 
												       reta37vset (nullptr), 
												       reta33vset (nullptr), 
												       weta1vset  (nullptr), 
												       weta2vset  (nullptr), 
												       demax1vset (nullptr), 
												       demax2vset (nullptr), 
												       devset     (nullptr), 
												       fracs1vset (nullptr), 
												       wtots1vset (nullptr), 
												       f1vset     (nullptr),
												       f3vset     (nullptr),
												       pos7vset   (nullptr), 
												       clusisovset(nullptr),
												       Eratiovset(nullptr),
												       Rhadvset(nullptr),
												       Rhad1vset(nullptr),
												       Retavset(nullptr),
												       Rphivset(nullptr),
												       hadleakvseta(nullptr),
												       middleevseta(nullptr),
												       reta37vseta(nullptr),
												       reta33vseta(nullptr),
												       weta1vseta(nullptr),
												       weta2vseta(nullptr),
												       demax1vseta(nullptr),
												       demax2vseta(nullptr),
												       devseta(nullptr),
												       fracs1vseta(nullptr),
												       wtots1vseta(nullptr),
												       f1vseta(nullptr),
												       f3vseta(nullptr),
												       pos7vseta(nullptr),
												       clusisovseta(nullptr),
												       Eratiovseta(nullptr),
												       Rhadvseta(nullptr),
												       Rhad1vseta(nullptr),
												       Retavseta(nullptr),
												       Rphivseta(nullptr)

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
    f1      = Book1D("f1", "f1 of "+ m_sParticleType+";f1;Entries" , 120, -0.2, 1.0);
    f3      = Book1D("f3", "f3 of "+ m_sParticleType+";f3;Entries" , 120, -0.2, 1.0);
    Eratio  = Book1D("Eratio", "Eratio of "+ m_sParticleType+";Eratio;Entries" , 100, 0., 1.0);
    pos7    = Book1D("pos7", "pos7 of "+ m_sParticleType+";pos7;Entries", 80, -4., 4.);
    clusiso = Book1D("clusiso","Cluster Isolation of "+ m_sParticleType+"; E_{T}^{cone20}/E_{T};Entries", 100, 0., 1.);
    E0      = Book1D("E0", "E0 of "+ m_sParticleType+"; E0 (GeV); Entries" , 210,-10.,200.);
    E1      = Book1D("E1", "E1 of "+ m_sParticleType+"; E1 (GeV); Entries" , 210,-10.,200.);
    E2      = Book1D("E2", "E2 of "+ m_sParticleType+"; E2 (GeV); Entries" , 210,-10.,200.);
    E3      = Book1D("E3", "E3 of "+ m_sParticleType+"; E3 (GeV); Entries" , 210,-10.,200.);
    Rhad    = Book1D("Rhad", "Rhad of "+ m_sParticleType+";Rhad;Entries", 100, -0.5, 2.);
    Rhad1   = Book1D("Rhad1", "Rhad1 of "+ m_sParticleType+";Rhad1;Entries", 100, -0.5, 1.5);
    Reta    = Book1D("Reta", "Reta of "+ m_sParticleType+";Reta;Entries", 100, -0., 3.);
    Rphi    = Book1D("Rphi", "Rphi of "+ m_sParticleType+";Rphi;Entries", 100, -1., 1.);

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
    f3vset      = Book2D("f3vset", "f3 vs E_{T} of "+ m_sParticleType+";f3; E_{T} (GeV) " , 120, -0.2, 1.0, 200, 0., 200);
    Eratiovset  = Book2D("Eratiovset", "Eratio vs E_{T} of "+ m_sParticleType+";Eratio; E_{T} (GeV) " , 100, 0., 1.0, 200, 0., 200);
    pos7vset    = Book2D("pos7vset", "pos7 vs E_{T} of "+ m_sParticleType+";pos7; E_{T} (GeV) ", 80, -4., 4., 200, 0., 200);
    clusisovset = Book2D("clusisovset","Cluster Isolation vs E_{T} of "+ m_sParticleType+"; E_{T}^{cone20}/E_{T}; E_{T} (GeV) ", 100, 0., 1., 200, 0., 200);
    Rhadvset    = Book2D("Rhadvset", "Rhad vs E_{T} of "+ m_sParticleType+";Rhad;E_{T} (GeV)", 100, -0.5, 2., 200, 0., 200);
    Rhad1vset   = Book2D("Rhad1vset", "Rhad1 vs E_{T} of "+ m_sParticleType+";Rhad1;E_{T} (GeV)", 100, -0.5, 1.5, 200, 0., 200);
    Retavset    = Book2D("Retavset", "Reta vs E_{T} of "+ m_sParticleType+";Reta;E_{T} (GeV)", 100, -0., 3., 200, 0., 200);
    Rphivset    = Book2D("Rphivset", "Rphi vs E_{T} of "+ m_sParticleType+";Rphi;E_{T} (GeV)", 100, -1., 1., 200, 0., 200);
    
    hadleakvseta = Book2D("hadleakvseta", "Hadronic leakage vs E_{T} of " + m_sParticleType+"; E_{hadleak} (GeV) ; #eta ", 100, -0.07, 0.13, 1000,-5.,5.); 
    middleevseta = Book2D("middleevseta", "E_{middle} vs E_{T} of "+ m_sParticleType+"; E_{277} (GeV); #eta " , 210,-10.,200., 1000,-5.,5.);
    reta37vseta  = Book2D("reta37vseta", "R_{#eta37} vs E_{T} of "+ m_sParticleType+";R_{#eta37}; #eta ", 355, 0., 1.1005, 1000,-5.,5.);
    reta33vseta  = Book2D("reta33vseta", "R_{#eta33} vs E_{T} of "+ m_sParticleType+";R_{#eta33}; #eta ", 355, 0., 1.1005, 1000,-5.,5.);
    weta1vseta   = Book2D("weta1vseta", "W_{#etas1} vs E_{T} of "+ m_sParticleType+";W_{#etas1}; #eta ", 100, 0., 1., 1000,-5.,5.);
    weta2vseta   = Book2D("weta2vseta", "W_{#etas2} vs E_{T} of "+ m_sParticleType+";W_{#etas2}; #eta ", 100, 0., 0.03, 1000,-5.,5.);
    demax1vseta  = Book2D("demax1vseta", "#DeltaE_{maxs1} vs E_{T} of "+ m_sParticleType+";#DeltaE_{maxs1}; #eta " , 100, 0., 1., 1000,-5.,5.);
    demax2vseta  = Book2D("demax2vseta", "#DeltaE_{maxs2} vs E_{T} of "+ m_sParticleType+";#DeltaE_{maxs2}; #eta " , 80, 0., 0.8, 1000,-5.,5.);
    devseta      = Book2D("devseta", "#DeltaE vs E_{T} of "+ m_sParticleType+";#DeltaE (GeV); #eta ", 250, 0., 0.5, 1000,-5.,5.);
    fracs1vseta  = Book2D("fracs1vseta", "Fracs1 vs E_{T} of "+ m_sParticleType+";Fracs1; #eta ", 350, 0., 3.5, 1000,-5.,5.);
    wtots1vseta  = Book2D("wtots1vseta", "W_{tots1} vsE_{T} of "+ m_sParticleType+";W_{tots1}; #eta ", 100, 0., 10., 1000,-5.,5.);
    f1vseta      = Book2D("f1vseta", "f1 vs E_{T} of "+ m_sParticleType+";f1; #eta " , 100, 0., 1.0, 1000,-5.,5.);
    f3vseta      = Book2D("f3vseta", "f3 vs #eta of "+ m_sParticleType+";f3; #eta " , 120, -0.2, 1.0, 1000,-5.,5.);
    Eratiovseta  = Book2D("Eratiovseta", "Eratio vs #eta of "+ m_sParticleType+";Eratio; #eta " , 100, 0., 1.0, 1000,-5.,5.);
    pos7vseta    = Book2D("pos7vseta", "pos7 vs #eta of "+ m_sParticleType+";pos7; #eta ", 80, -4., 4., 1000,-5.,5.);
    clusisovseta = Book2D("clusisovseta","Cluster Isolation vs #eta of "+ m_sParticleType+"; E_{T}^{topoEtcone20}/E_{T}; #eta ", 100, 0., 1., 1000,-5.,5.);
    Rhadvseta    = Book2D("Rhadvseta", "Rhad vs #eta of "+ m_sParticleType+";Rhad;#eta", 100, -0.5, 2., 1000,-5.,5.);
    Rhad1vseta   = Book2D("Rhad1vseta", "Rhad1 vs #eta of "+ m_sParticleType+";Rhad1;#eta", 100, -0.5, 1.5, 1000,-5.,5.);
    Retavseta    = Book2D("Retavseta", "Reta vs #eta of "+ m_sParticleType+";Reta;#eta", 100, -0., 3., 1000,-5.,5.);
    Rphivseta    = Book2D("Rphivseta", "Rphi vs #eta of "+ m_sParticleType+";Rphi;#eta", 100, -1., 1., 1000,-5.,5.);
    
  }

  void ShowerShapesPlots::fill(const xAOD::Egamma& egamma, const xAOD::EventInfo& eventInfo) const{

    float weight = 1.;
    weight = eventInfo.beamSpotWeight();

    float eta2 = fabs (egamma.caloCluster()->etaBE (2));  
    float et37 = egamma.caloCluster()->e() / cosh (eta2);
    float ethad(0);
    float ethad1(0);
    float raphad(0);
    float raphad1(0);
    if(egamma.showerShapeValue(ethad, xAOD::EgammaParameters::ethad )){
      raphad = et37 > 0. ? ethad / et37 : 0.;
    }
    if(egamma.showerShapeValue(ethad1, xAOD::EgammaParameters::ethad1 )){
      raphad1 =  et37 > 0. ? ethad1 / et37 : 0.;
    }
    float hadrleak = (eta2 >= 0.8 && eta2 < 1.37) ? raphad : raphad1;
    hadleak->Fill(hadrleak, weight);
    hadleakvset->Fill(hadrleak, egamma.pt()/GeV, weight);
    hadleakvseta->Fill(hadrleak, egamma.eta(), weight);

    float e277(0);
    float e237(0);
    float e233(0);

    if(egamma.showerShapeValue(e277, xAOD::EgammaParameters::e277 ) &&
       egamma.showerShapeValue(e237, xAOD::EgammaParameters::e237 ) &&
       egamma.showerShapeValue(e233, xAOD::EgammaParameters::e233 ) ){

      middlee->Fill(e277/GeV, weight);
      middleevset->Fill(e277/GeV, egamma.pt()/GeV, weight);
      middleevseta->Fill(e277/GeV, egamma.eta(), weight);
      float rateta37 =   e277!= 0. ? e237/e277:0.;
      float rateta33 =   e277!= 0. ? e233/e277:0.; 
      reta37->Fill(rateta37, weight);
      reta33->Fill(rateta33, weight);
      reta37vset->Fill(rateta37, egamma.pt()/GeV, weight);
      reta33vset->Fill(rateta33, egamma.pt()/GeV, weight);
      reta37vseta->Fill(rateta37, egamma.eta(), weight);
      reta33vseta->Fill(rateta33, egamma.eta(), weight);
    }

    float shweta1(0);
    float shweta2(0);
    if(egamma.showerShapeValue(shweta1, xAOD::EgammaParameters::weta1 )&&
       egamma.showerShapeValue(shweta2, xAOD::EgammaParameters::weta2 )){
      weta1->Fill(shweta1, weight);
      weta2->Fill(shweta2, weight);
      weta1vset->Fill(shweta1, egamma.pt()/GeV, weight);
      weta2vset->Fill(shweta2, egamma.pt()/GeV, weight);
      weta1vseta->Fill(shweta1, egamma.eta(), weight);
      weta2vseta->Fill(shweta2, egamma.eta(), weight);
    }
    
    float emax(0);
    float emin(0);
    float emax2(0);
    if(egamma.showerShapeValue(emax, xAOD::EgammaParameters::emaxs1 )&&
       egamma.showerShapeValue(emin, xAOD::EgammaParameters::emins1 )&&
       egamma.showerShapeValue(emax2, xAOD::EgammaParameters::e2tsts1 )){

      float deltemax1 = fabs (emax + emax2) > 0. ? (emax - emax2) / (emax + emax2) : 0.; 
      demax1->Fill(deltemax1, weight);
      demax2->Fill(emax2 / (1. + 0.009 * et37/GeV)/GeV, weight); 
      de->Fill( (emax2 - emin)/GeV, weight);
    
      demax1vset->Fill(deltemax1, egamma.pt()/GeV, weight);
      demax2vset->Fill(emax2 / (1. + 0.009 * et37/GeV)/GeV, egamma.pt()/GeV, weight);
      devset->Fill( (emax2 - emin)/GeV, egamma.pt()/GeV, weight);
      demax1vseta->Fill(deltemax1, egamma.eta(), weight);
      demax2vseta->Fill(emax2 / (1. + 0.009 * et37/GeV)/GeV, egamma.eta(), weight);
      devseta->Fill( (emax2 - emin)/GeV, egamma.eta(), weight); 
    }

    float shfracs1(0);
    float shwtots1(0);
    float fracf1(0);
    float fracf3(0);
    float eRatio(0);    
    float difpos7(0);
    float rhad(0);
    float rhad1(0);
    float reta(0);
    float rphi(0);

    if(egamma.showerShapeValue(shfracs1, xAOD::EgammaParameters::fracs1 )){
      fracs1->Fill(shfracs1, weight);
      fracs1vset->Fill(shfracs1, egamma.pt()/GeV, weight);
      fracs1vseta->Fill(shfracs1, egamma.eta(), weight);
    } 

    if(egamma.showerShapeValue(shwtots1, xAOD::EgammaParameters::wtots1 )){
      wtots1->Fill(shwtots1, weight);
      wtots1vset->Fill(shwtots1, egamma.pt()/GeV, weight);
      wtots1vseta->Fill(shwtots1, egamma.eta(), weight);
    }
    
    if(egamma.showerShapeValue(fracf1, xAOD::EgammaParameters::f1 )){
      f1->Fill(fracf1, weight);
      f1vset->Fill(fracf1, egamma.pt()/GeV, weight);
      f1vseta->Fill(fracf1, egamma.eta(), weight);
    }
    
    if(egamma.showerShapeValue(fracf3, xAOD::EgammaParameters::f3 )){
      f3->Fill(fracf3, weight);
      f3vset->Fill(fracf3, egamma.pt()/GeV, weight);
      f3vseta->Fill(fracf3, egamma.eta(), weight);
    }
    if(egamma.showerShapeValue(eRatio, xAOD::EgammaParameters::Eratio )){
      Eratio->Fill(eRatio, weight);
      Eratiovset->Fill(eRatio, egamma.pt()/GeV, weight);
      Eratiovseta->Fill(eRatio, egamma.eta(), weight);
    }
    
    if(egamma.showerShapeValue(difpos7, xAOD::EgammaParameters:: pos7)){
      pos7->Fill(difpos7, weight);
      pos7vset->Fill(difpos7, egamma.pt()/GeV, weight);
      pos7vseta->Fill(difpos7, egamma.eta(), weight);
    }
    
    if(egamma.showerShapeValue(rhad, xAOD::EgammaParameters::Rhad )){
      Rhad->Fill(rhad, weight);
      Rhadvset->Fill(rhad, egamma.pt()/GeV, weight);
      Rhadvseta->Fill(rhad, egamma.eta(), weight);
    }
   
    if(egamma.showerShapeValue(rhad1, xAOD::EgammaParameters::Rhad1 )){
      Rhad1->Fill(rhad1, weight);
      Rhad1vset->Fill(rhad1, egamma.pt()/GeV, weight);
      Rhad1vseta->Fill(rhad1, egamma.eta(), weight);
    }
    
    if(egamma.showerShapeValue(reta, xAOD::EgammaParameters::Reta )){
      Reta->Fill(reta, weight);
      Retavset->Fill(reta, egamma.pt()/GeV, weight);
      Retavseta->Fill(reta, egamma.eta(), weight);
    }
    
    if(egamma.showerShapeValue(rphi, xAOD::EgammaParameters::Rphi )){
      Rphi->Fill(rphi, weight);
      Rphivset->Fill(rphi, egamma.pt()/GeV, weight);
      Rphivseta->Fill(rphi, egamma.eta(), weight);
    }
  
    float topoEtconeIso(0);
    float shetcone20(0);
    if(egamma.isolationValue(shetcone20,   xAOD::Iso::topoetcone20)){ //rel20
//    if(egamma.isolationValue(shetcone20,   xAOD::EgammaParameters::topoetcone20)){//rel19
      topoEtconeIso = et37!=0. ? shetcone20/et37 : 0.; 
      clusiso->Fill(topoEtconeIso, weight);
      clusisovset->Fill(topoEtconeIso, egamma.pt()/GeV, weight);
      clusisovseta->Fill(topoEtconeIso, egamma.eta(), weight);
    }
    
    if(egamma.caloCluster()){
      const double e0 =  egamma.caloCluster()->energyBE(0);
      const double e1 =  egamma.caloCluster()->energyBE(1);
      const double e2 =  egamma.caloCluster()->energyBE(2);
      const double e3 =  egamma.caloCluster()->energyBE(3);
      
      E0->Fill(e0, weight);
      E1->Fill(e1, weight);
      E2->Fill(e2, weight);
      E3->Fill(e3, weight);
    }
  }

}
