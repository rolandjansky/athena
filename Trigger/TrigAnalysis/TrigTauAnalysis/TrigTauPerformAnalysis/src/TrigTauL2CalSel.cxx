/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauL2CalSel.cxx
/// Package : TrigTauPerformAnalysis
/// Author  : Eric Torrence (University of Oregon)
/// Created : February 2007
///
/// DESCRIPTION:
///           Class to apply L2 selection to performance ntuple objects
///
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "TrigTauPerformAnalysis/TrigTauL2CalSel.h"
#include "TrigTauPerformNtuple/TrigTauL2Object.h"

#include <math.h>
#include <iostream>
using namespace std;

TrigTauL2CalSel::TrigTauL2CalSel()
 : cut_Et              (0),           
   cut_EtCalib        (0),                            
   cut_EtNar          (0),                            
   cut_EtNor          (0),                            
   cut_nStrips        (0),    
   cut_stripWidth     (0),      
   cut_EMRadius       (0),        
   cut_IsoFrac        (0),           
   cut_EMEnergyWidth2 (0),
   cut_nStripsMultiProng        (0),    
   cut_stripWidthMultiProng     (0),      
   cut_EMRadiusMultiProng       (0),        
   cut_IsoFracMultiProng        (0),           
   cut_EMEnergyWidth2MultiProng (0),
   _debug(false),
   menu(TrigTauFlags::SECONDATTEMPT),
   m_histManager(0)
{}

TrigTauL2CalSel::~TrigTauL2CalSel()
{
  if( m_histManager ) { delete m_histManager; m_histManager=0;}
}

TrigTauL2CalSel::TrigTauL2CalSel(const TrigTauL2CalSel& other)
  : cut_Et             (other.cut_Et            ),            
    cut_EtCalib        (other.cut_EtCalib       ),                            
    cut_EtNar          (other.cut_EtNar         ),                            
    cut_EtNor          (other.cut_EtNor         ),                            
    cut_nStrips        (other.cut_nStrips       ),    
    cut_stripWidth     (other.cut_stripWidth    ),      
    cut_EMRadius       (other.cut_EMRadius      ),        
    cut_IsoFrac        (other.cut_IsoFrac       ),            
    cut_EMEnergyWidth2 (other.cut_EMEnergyWidth2) ,
    cut_nStripsMultiProng        (other.cut_nStripsMultiProng       ),    
    cut_stripWidthMultiProng     (other.cut_stripWidthMultiProng    ),      
    cut_EMRadiusMultiProng       (other.cut_EMRadiusMultiProng      ),        
    cut_IsoFracMultiProng        (other.cut_IsoFracMultiProng       ),            
    cut_EMEnergyWidth2MultiProng (other.cut_EMEnergyWidth2MultiProng) ,
    _debug(other._debug),
    menu(other.menu),
    m_histManager (other.m_histManager)
{}

void TrigTauL2CalSel::setHistDebug(TrigTauHistogram::HISTTYPE type)
{
  if( type != TrigTauHistogram::NONE ){
    //gROOT->cd();
    if( m_histManager ) delete m_histManager;
    m_histManager = new TrigTauHistogram(type);
    m_histManager->book("baseL2Cal","Before any cut");
    m_histManager->book("et","Cut on raw Et of cluster");
    m_histManager->book("etCalib","After Cut on EtCalib of cluster");
    m_histManager->book("nStrips","After Cut on nStrips of cluster");
    m_histManager->book("stripWidth","After Cut on stripWidth of cluster");
    m_histManager->book("emrad","After Cut on EMRadius of cluster");
    m_histManager->book("isoFrac","After Cut on IsoFrac of cluster");
    m_histManager->book("EmEnWidth","After Cut on EMEnergyWidth2 of cluster");
    m_histManager->book("EtRawNor","After Cut on EtRawNor");
  }else{
      if( m_histManager ) { delete m_histManager; m_histManager=0;}
  } 
}



void
TrigTauL2CalSel::setCuts(
			 float Et, 
			 float EtCalib, 
			 int nStrips, 
			 float stripWidth, 
			 float EMRadius, 
			 float isoFrac, 
			 float EMEnWidth2, 
			 float EtNar, 
			 int nStripsMultiProng, 
			 float stripWidthMultiProng, 
			 float EMRadiusMultiProng, 
			 float isoFracMultiProng, 
			 float EMEnWidth2MultiProng) {
  cut_Et             =Et;             
  cut_EtCalib        =EtCalib;                          
  cut_EtNar          =EtNar;                          
  cut_nStrips        =nStrips;  
  cut_stripWidth     =stripWidth;    
  cut_EMRadius       =EMRadius;      
  cut_IsoFrac        =isoFrac;        
  cut_EMEnergyWidth2 =EMEnWidth2;  
  
  // this will switch between the old and the new cuts
  cut_EtNor = 0;
  
  if( nStripsMultiProng<0) cut_nStripsMultiProng        =nStrips;  
  else                     cut_nStripsMultiProng        =nStripsMultiProng;  
   if( stripWidthMultiProng<0) cut_stripWidthMultiProng     =stripWidth;  
  else                         cut_stripWidthMultiProng     =stripWidthMultiProng;     
   if( EMRadiusMultiProng<0) cut_EMRadiusMultiProng       =EMRadius;   
  else                       cut_EMRadiusMultiProng       =EMRadiusMultiProng;       
   if( isoFracMultiProng<0) cut_IsoFracMultiProng        =isoFrac;   
  else                      cut_IsoFracMultiProng        =isoFracMultiProng;     
   if( EMEnWidth2MultiProng<0) cut_EMEnergyWidth2MultiProng =EMEnWidth2;   
  else                         cut_EMEnergyWidth2MultiProng =EMEnWidth2MultiProng;    



}

void
TrigTauL2CalSel::setNewCuts( float RawEtNor,
                             float EMRadius, 
                             float EMRadiusMultiProng)
{
  cut_EtNor    = RawEtNor;                          
  cut_EMRadius = EMRadius;      

  if( EMRadiusMultiProng<0)
      cut_EMRadiusMultiProng = EMRadius;   
  else
      cut_EMRadiusMultiProng = EMRadiusMultiProng;       
}

void
TrigTauL2CalSel::print() const {

  if (menu != TrigTauFlags::SECONDATTEMPT) {
    cout << "LVL2 selection based on trigger decisision bits for " 
	 << TrigTauFlags::menuName(menu) << endl;
    return;
  }


  cout << " L2Calo cuts :" << endl;

  if (cut_Et > 1.e-10) {
    cout << " Et             > " << cut_Et            << endl;
  } else {
    cout << " Et:             not applied" << endl;
  }

  if (cut_EtCalib > 1.e-10) {
    cout << " EtCalib        > " << cut_EtCalib        << endl;
  } else {
    cout << " EtCalib:        not applied" << endl;
  }

  if (cut_EtNar > 1.e-10) {
    cout << " EtNar        > " << cut_EtNar        << endl;
  } else {
    cout << " EtNar:        not applied" << endl;
  }
  if (cut_nStrips > 1.e-10) {
    cout << " nStrips        <= " << cut_nStrips     <<" (MultiProng : "<< cut_nStripsMultiProng<< " )" << endl;
  } else {
    cout << " nStrips:        not applied" << endl;
  }

  if (cut_stripWidth > 1.e-10) {
    cout << " stripWidth     <= " << cut_stripWidth    <<" (MultiProng : "<< cut_stripWidthMultiProng << " )" << endl;
  } else {
    cout << " stripWidth:     not applied" << endl;
  }

  if (cut_EMRadius > 1.e-10) {
    cout << " EMRadius       <= " << cut_EMRadius        <<" (MultiProng : "<<cut_EMRadiusMultiProng << " )" << endl;
  } else {
    cout << " EMRadius:       not applied" << endl;
  }

  if (cut_IsoFrac > 1.e-10) {
    cout << " IsoFrac        <= " << cut_IsoFrac        <<" (MultiProng : "<< cut_IsoFracMultiProng << " )"<< endl;
  } else {
    cout << " IsoFrac:        not applied" << endl;
  }

  if (cut_EMEnergyWidth2 > 1.e-10) {
    cout << " EMEnergyWidth2 <= " << cut_EMEnergyWidth2 <<" (MultiProng : "<<cut_EMEnergyWidth2MultiProng << " )"<< endl;
  } else {
    cout << " EMEnergyWidth2: not applied" << endl;
  }

  if (cut_EtNor > 0) {
    cout << " RawEtNor <= " << cut_EtNor << endl;
  } else {
    cout << " RawEtNor: not applied" << endl;
  }

}

bool
TrigTauL2CalSel::passed(const TrigTauL2Object& tau) const {
  if (menu != TrigTauFlags::SECONDATTEMPT) return tau.m_tags.passedL2(menu);

  if( cut_EtNor > 0) return cutWordNew(tau) == 0;
  
  return (cutWord(tau) == 0);
}

unsigned long
TrigTauL2CalSel::cutWord(const TrigTauL2Object& tau) const {

  if(  cut_Et >1.e-10  &&  cut_EtCalib >1.e-10 )
    {
      cout << "TrigTauL2CalSel::WARNING both cuts on Et and EtCalib are used \n";
    }

  if (_debug) cout << "L2Cal:  eta="<<tau.m_caloEta <<", phi="<<tau.m_caloPhi << " RoIWord="
		   <<  std::hex <<tau.m_roiWord << std::dec <<endl;

  float varHist = 0;
  if( m_histManager)
    {
      if( m_histManager->type() == TrigTauHistogram::PTHIST ) varHist = tau.Pt();
      else if( m_histManager->type() == TrigTauHistogram::ETAHIST ) varHist = tau.Eta();
      else if( m_histManager->type() == TrigTauHistogram::PHIHIST ) varHist = tau.Phi();
    }

  if( m_histManager) m_histManager->fill(0, varHist);

  unsigned long word = 0;
  int nTrk = tau.nTracksInDR(0.1);

  if (_debug) cout << "Et="<<tau.etClus() << " ";
  if( cut_Et         > 1.e-10 && cut_Et          >= tau.etClus()) {
    if (_debug) cout << " - failed Et" << endl;
    word |= L2CUT_ET;
  }

  if (_debug) cout << "EtNar="<<tau.etNarrow() << " ";
  if( cut_EtNar         > 1.e-10 && cut_EtNar          >= tau.etNarrow()) {
    if (_debug) cout << " - failed EtNar" << endl;
    word |= L2CUT_ETNAR;
  }
  
  if( m_histManager && word==0) m_histManager->fill(1, varHist);

  if (_debug) cout << "EtCalib="<<tau.etClusCalib() << " ";
  if( cut_EtCalib    > 1.e-10 && cut_EtCalib     >= tau.etClusCalib() ) {
    if (_debug) cout << " - failed EtCalib" << endl;
    word |= L2CUT_ETCALIB;
  }

  if( m_histManager && word==0) m_histManager->fill(2, varHist);

  if (_debug) cout << "nStrips="<<tau.m_nStrips << " ";
  if( cut_nStrips    > 1.e-10 && nTrk<=1 && cut_nStrips     < tau.m_nStrips   ) {
    if (_debug) cout << " - failed nStrips" << endl;
    word |= L2CUT_NSTRIP;
  }
  if( cut_nStripsMultiProng    > 1.e-10 && nTrk > 1 && cut_nStripsMultiProng     < tau.m_nStrips   ) {
    if (_debug) cout << " - failed nStrips MultiProng" << endl;
    word |= L2CUT_NSTRIP;
  }

  if( m_histManager && word==0) m_histManager->fill(3, varHist);

  if (_debug) cout << "strpWidth="<<tau.m_stripWidth2 << " ";
  if( cut_stripWidth > 1.e-10  && nTrk<=1 && cut_stripWidth  < tau.m_stripWidth2     ) {
    if (_debug) cout << " - failed stripWidth" << endl;
    word |= L2CUT_STRWID;
  }
  if( cut_stripWidthMultiProng > 1.e-10 && nTrk > 1 && cut_stripWidthMultiProng  < tau.m_stripWidth2     ) {
    if (_debug) cout << " - failed stripWidth MultiProng " << endl;
    word |= L2CUT_STRWID;
  }

  if( m_histManager && word==0) m_histManager->fill(4, varHist);

  if (_debug) cout << "EMRad="<<tau.m_EMRadius << " ";
  if( cut_EMRadius   > 1.e-10  && nTrk<=1 && cut_EMRadius    < tau.m_EMRadius    ) {
    if (_debug) cout << " - failed EMRadius" << endl;
    word |= L2CUT_EMRAD;
  }
  if( cut_EMRadiusMultiProng   > 1.e-10  && nTrk > 1 && cut_EMRadiusMultiProng    < tau.m_EMRadius    ) {
    if (_debug) cout << " - failed EMRadius MultiProng" << endl;
    word |= L2CUT_EMRAD;
  }
  if( m_histManager && word==0) m_histManager->fill(5, varHist);

  if (_debug) cout << "IsoFrac="<<tau.m_isoFrac << " ";
  if( cut_IsoFrac    > 1.e-10 && nTrk<=1 && cut_IsoFrac     < tau.m_isoFrac        ) {
    if (_debug) cout << " - failed IsoFrac" << endl;
    word |= L2CUT_ISOFRAC;
  }
  if( cut_IsoFracMultiProng    > 1.e-10 && nTrk > 1 && cut_IsoFracMultiProng     < tau.m_isoFrac        ) {
    if (_debug) cout << " - failed IsoFrac MultiProng" << endl;
    word |= L2CUT_ISOFRAC;
  }
  if( m_histManager && word==0) m_histManager->fill(6, varHist);

  if (_debug) cout << "EmEnWidth="<<tau.m_EMEnergyWidth[2] << " ";
  if( cut_EMEnergyWidth2>1.e-10 && nTrk<=1 && cut_EMEnergyWidth2 < tau.m_EMEnergyWidth[2]) {
    if (_debug) cout << " - failed EMWidth2" << endl;
    word |= L2CUT_EMWID;
  }
  if( cut_EMEnergyWidth2MultiProng>1.e-10 && nTrk > 1 && cut_EMEnergyWidth2MultiProng < tau.m_EMEnergyWidth[2]) {
    if (_debug) cout << " - failed EMWidth2 MultiProng" << endl;
    word |= L2CUT_EMWID;
  }
  if( m_histManager && word==0) m_histManager->fill(7, varHist);

  if (_debug && (word==0)) cout << " accepted" << endl;
  return word;
}

unsigned long
TrigTauL2CalSel::cutWordNew(const TrigTauL2Object& tau) const
{
  if (_debug)
      cout << "L2Cal:  eta=" << tau.m_caloEta << ", phi=" << tau.m_caloPhi
           << " RoIWord=" << std::hex << tau.m_roiWord << std::dec <<endl;
  
  float varHist = 0;
  if( m_histManager)
  {
    if( m_histManager->type() == TrigTauHistogram::PTHIST)
        varHist = tau.Pt();
    else if( m_histManager->type() == TrigTauHistogram::ETAHIST)
        varHist = tau.Eta();
    else if( m_histManager->type() == TrigTauHistogram::PHIHIST)
        varHist = tau.Phi();
  }
  
  if( m_histManager) m_histManager->fill(0, varHist);
  
  unsigned long word = 0;
  int nTrk = tau.m_nCoreTracks + tau.m_nIsoTracks;
  
  // Cut1: EtNar NOT IMPLEMENTED
  if( m_histManager && word==0) m_histManager->fill(1, varHist);

  // Cut2: EtCalib NOT IMPLEMENTED
  if( m_histManager && word==0) m_histManager->fill(2, varHist);

  // Cut3: nStrips NOT IMPLEMENTED
  if( m_histManager && word==0) m_histManager->fill(3, varHist);

  // Cut4: StripWidth NOT IMPLEMENTED
  if( m_histManager && word==0) m_histManager->fill(4, varHist);
  
  // Cut5: EMRadius
  float emRadius3S = tau.m_EMRadius3S;
  if (_debug)
      cout << "EMRadius3S=" << emRadius3S << " ";
  if( cut_EMRadiusMultiProng > 1.e-10 && nTrk<=1 &&
      cut_EMRadiusMultiProng < emRadius3S)
  {
    if (_debug) cout << " - failed EMRadius3S" << endl;
    word |= L2CUT_EMRAD;
  }
  if( cut_EMRadius > 1.e-10 && nTrk == 1 && cut_EMRadius < emRadius3S)
  {
    if (_debug) cout << " - failed EMRadius3S OneProng" << endl;
    word |= L2CUT_EMRAD;
  }
  if( m_histManager && word==0) m_histManager->fill(5, varHist);

  // Cut6: IsoFrac NOT IMPLEMENTED
  if( m_histManager && word==0) m_histManager->fill(6, varHist);

  // Cut7: EMWidth2 NOT IMPLEMENTED
  if( m_histManager && word==0) m_histManager->fill(7, varHist);

  // Cut8: RawEtNor
  if(_debug) cout << "RawEtNor=" << tau.etWide() << " ";
  if( cut_EtNor > 0 && cut_EtNor >= tau.etWide())
  {
    if (_debug) cout << " - failed RawEtNor" << endl;
    word |= L2CUT_ETNOR;
  }
  
  if( m_histManager && word==0) m_histManager->fill(8, varHist);

  if (_debug && (word==0))
      cout << " accepted" << endl;
  
  return word;
}
