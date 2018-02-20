/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TauHistUtils/Migration.h"

namespace Tau{

Migration::Migration(PlotBase* pParent, std::string sDir, std::string sTauJetContainerName):
   PlotBase(pParent, sDir),
   m_migration_panTau(nullptr),
   m_migration_panTauProto(nullptr),
   m_migration_cellBased(nullptr),
   m_sTauJetContainerName(sTauJetContainerName)
{
}
 
Migration::~Migration()
{
}

void Migration::initializePlots()
{
   m_migration_panTau = Book1D("panTau_migration",m_sTauJetContainerName + " panTau migration",DECAYSIZE,0,DECAYSIZE);
   m_migration_panTauProto = Book1D("panTauProto_migration",m_sTauJetContainerName + " panTau proto migration",DECAYSIZE,0,DECAYSIZE);
   m_migration_cellBased = Book1D("cellBased_migration",m_sTauJetContainerName + " cellBased migration",DECAYSIZE,0,DECAYSIZE);
   m_migration_panTau->GetXaxis()->SetLabelSize(0.05);
   m_migration_panTauProto->GetXaxis()->SetLabelSize(0.05);
   m_migration_cellBased->GetXaxis()->SetLabelSize(0.05);
   for(int i=1; i<= DECAYSIZE;i++){
      m_migration_panTauProto->GetXaxis()->SetBinLabel(i,m_lable[i-1]);
      m_migration_panTau->GetXaxis()->SetBinLabel(i,m_lable[i-1]);
      m_migration_cellBased->GetXaxis()->SetBinLabel(i,m_lable[i-1]);
   }
}
   
void Migration::fill(const xAOD::TauJet& thisTau,int nProng, int nNeu) {

   // panTau
   int isPanTauCandidate = 0;
   bool foundDetail = thisTau.panTauDetail(xAOD::TauJetParameters::PanTauDetails::PanTau_isPanTauCandidate, isPanTauCandidate);
   if ( !foundDetail || !isPanTauCandidate ) return;
   
   int decayMode = xAOD::TauJetParameters::DecayMode::Mode_Error;
   foundDetail = thisTau.panTauDetail(xAOD::TauJetParameters::PanTauDetails::PanTau_DecayMode, decayMode);
   if ( foundDetail ) decayModeFill(decayMode, nProng, nNeu, m_migration_panTau);	

   decayMode = xAOD::TauJetParameters::DecayMode::Mode_Error;
   foundDetail = thisTau.panTauDetail(xAOD::TauJetParameters::PanTauDetails::PanTau_DecayModeProto, decayMode);
   if ( foundDetail ) decayModeFill(decayMode, nProng, nNeu, m_migration_panTauProto);	


//RecTau
   
	
   
   
   /*+++++++++++++++++++++++++++++++++++++++++++++++++
     +++++++++++++++++++CellBased+++++++++++++++++++++
     +++++++++++++++++++++++++++++++++++++++++++++++++*/

   
   //Charged Pions
   std::vector< ElementLink< xAOD::PFOContainer > > cellBased_chargedPFO = thisTau.protoChargedPFOLinks();
   int cellP = cellBased_chargedPFO.size();
	
   
   //Neutral Pions
   int nPi0_tau = 0;
   std::vector< ElementLink< xAOD::PFOContainer > > cellBased_neutralPFO = thisTau.protoNeutralPFOLinks();
   std::vector< ElementLink< xAOD::PFOContainer > >::iterator first_cellBased_neutralPFO = cellBased_neutralPFO.begin();
   std::vector< ElementLink< xAOD::PFOContainer > >::iterator last_cellBased_neutralPFO = cellBased_neutralPFO.end();
   for (; first_cellBased_neutralPFO != last_cellBased_neutralPFO; ++first_cellBased_neutralPFO){
      ElementLink<xAOD::PFOContainer> thisLink = *first_cellBased_neutralPFO;
      const xAOD::PFO* thisPFO = *thisLink;
      int myNPi0Proto = 0;
      if(thisPFO->attribute(xAOD::PFODetails::nPi0Proto, myNPi0Proto)) nPi0_tau+=myNPi0Proto;
      
   } 
   decayModeFill(nProng,nNeu,cellP,nPi0_tau,m_migration_cellBased);	
   
   
   
   /*+++++++++++++++++++++++++++++++++++++++++++++++++
     +++++++++++++++++++Pantau+++++++++++++++++++++
     +++++++++++++++++++++++++++++++++++++++++++++++++*/
   
}


void Migration::decayModeFill(int trueMode, int recP, int recN,TH1 *histo)
{
   switch (trueMode) {
   case xAOD::TauJetParameters::DecayMode::Mode_1p0n:
      if      ( recP == 1 && recN == 0 ) histo->Fill(t10r10 + 0.5);
      else if ( recP == 1 && recN == 1 ) histo->Fill(t10r11 + 0.5);
      else if ( recP == 1 && recN >  1 ) histo->Fill(t10r1x + 0.5);
      else if ( recP == 3 ) histo->Fill(t1r3 + 0.5);
      break;
   case xAOD::TauJetParameters::DecayMode::Mode_1p1n:
      if      ( recP == 1 && recN == 0 ) histo->Fill(t11r10 + 0.5);
      else if ( recP == 1 && recN == 1 ) histo->Fill(t11r11 + 0.5);
      else if ( recP == 1 && recN >  1 ) histo->Fill(t11r1x + 0.5);
      else if ( recP == 3 ) histo->Fill(t1r3 + 0.5);
      break;
   case xAOD::TauJetParameters::DecayMode::Mode_1pXn:
      if      ( recP == 1 && recN == 0 ) histo->Fill(t1xr10 + 0.5);
      else if ( recP == 1 && recN == 1 ) histo->Fill(t1xr11 + 0.5);
      else if ( recP == 1 && recN >  1 ) histo->Fill(t1xr1x + 0.5);
      else if ( recP == 3 ) histo->Fill(t1r3 + 0.5);
      break;
   case xAOD::TauJetParameters::DecayMode::Mode_3p0n:
      if      ( recP == 3 && recN == 0 ) histo->Fill(t30r30 + 0.5);
      else if ( recP == 3 && recN >= 1 ) histo->Fill(t30r3x + 0.5);
      else if ( recP == 1 ) histo->Fill(t3r1 + 0.5);
      break;
   case xAOD::TauJetParameters::DecayMode::Mode_3pXn:
      if      ( recP == 3 && recN == 0 ) histo->Fill(t3xr30 + 0.5);
      else if ( recP == 3 && recN >= 1 ) histo->Fill(t3xr3x + 0.5);
      else if ( recP == 1 ) histo->Fill(t3r1 + 0.5);
      break;
   }
   return;
}


void Migration::decayModeFill(int trueP,int trueN,int recP, int recN,TH1 *histo){
   if(recP == 1 && recN== 0){
      if(trueP == 1 && trueN== 0) histo->Fill(t10r10 + 0.5);
      if(trueP == 1 && trueN== 1) histo->Fill(t11r10 + 0.5);
      if(trueP == 1 && trueN > 1) histo->Fill(t1xr10 + 0.5);
      if(trueP == 3) histo->Fill(t3r1 + 0.5);
   }
   if(recP == 1 && recN== 1){
      if(trueP == 1 && trueN== 0) histo->Fill(t10r11 + 0.5);
      if(trueP == 1 && trueN== 1) histo->Fill(t11r11 + 0.5);
      if(trueP == 1 && trueN > 1) histo->Fill(t1xr11 + 0.5);
      if(trueP == 3 ) histo->Fill(t3r1 + 0.5);
   }
   if(recP == 1 && recN > 1){
      if(trueP == 1 && trueN== 0) histo->Fill(t10r1x + 0.5);
      if(trueP == 1 && trueN== 1) histo->Fill(t11r1x + 0.5);
      if(trueP == 1 && trueN > 1) histo->Fill(t1xr1x + 0.5);
      if(trueP == 3 ) histo->Fill(t3r1 + 0.5);
   }
   if(recP == 3 && recN== 0){
      if(trueP == 1 ) histo->Fill(t1r3 + 0.5);
      if(trueP == 3 && trueN== 0) histo->Fill(t30r30 + 0.5);
      if(trueP == 3 && trueN>= 1) histo->Fill(t3xr30 + 0.5);
   }
   if(recP == 3 && recN>= 1){
      if(trueP == 1 ) histo->Fill(t1r3 + 0.5);
      if(trueP == 3 && trueN== 0) histo->Fill(t30r3x + 0.5);
      if(trueP == 3 && trueN>= 1) histo->Fill(t3xr3x + 0.5);
   }
   return;
}

}
