/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/JetFilter.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "TruthUtils/HepMCHelpers.h"
#include <vector>
#include <functional>
#include <algorithm>

JetFilter::JetFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name, pSvcLocator)
{
   declareProperty("EtaRange", m_UserEta=2.7);
   declareProperty("JetThreshold", m_UserThresh=17000.); // note CLHEP::MeV units
   declareProperty("SeedThreshold", m_Stop=1000.);    //note CLHEP::MeV units
   declareProperty("ConeSize", m_Cone=0.4);
   declareProperty("GridSizeEta", m_Gride=2);
   declareProperty("GridSizePhi", m_Gridp=2);
   declareProperty("JetNumber", m_UserNumber=1);
   declareProperty("JetType", m_Type=true); // true if its a cone

   m_EtaRange = 0.;
   m_emaxeta = 0.;
   m_edeta = 0.;
   m_edphi = 0.;
   m_twopi = 0.;
   m_nphicell = 0;
   m_netacell = 0;
   m_nphicell2 = 0;
   m_netacell2 = 0;
}


StatusCode JetFilter::filterInitialize() {
  m_emaxeta = 6.0;
  m_twopi = 4*asin(1.);
  m_edphi = m_twopi/m_grphi; // cell size
  m_edeta = 2.*m_emaxeta/m_greta; // cell size
  // How many cells in the jet cluster
  if (!m_Type) { // it's the rectangular grid
    m_nphicell = m_Gridp/2;
    m_netacell = m_Gride/2;
    m_nphicell2 = m_Gridp;
    m_netacell2 = m_Gride;
  } else {
    m_nphicell = (int) (m_Cone/m_edphi); // number of cells inside cone
    m_netacell = (int) (m_Cone/m_edeta);  //number of cells inside come
    m_nphicell2 = 2*m_nphicell+1; // guaranteed to be odd so that the highest cell is in middle
    m_netacell2 = 2*m_netacell+1;
  }
  ATH_MSG_INFO("Parameters are \n ");
  if (m_Type) {
    ATH_MSG_INFO("  Cone algorithm: " <<
                 " Pt cut  = " << m_UserThresh  << ", Number= " <<  m_UserNumber <<
                 ", Cone size=" << m_Cone << ", Rapidity range " << m_UserEta);
  } else {
    ATH_MSG_INFO("  GridAlgorithm: " <<
                 " Pt cut  = " << m_UserThresh << ", Number= " <<  m_UserNumber <<
                 ", eta size (units of 0.06) =" << m_Gride <<
                 ", phi size (units of 0.06) =" << m_Gridp << ", Rapidity range " << m_UserEta);
  }
  return StatusCode::SUCCESS;
}


StatusCode JetFilter::filterEvent() {
  // Init grid
  double etgrid[m_grphi][m_greta]; // clean it out before we start
  bool etgridused[m_grphi][m_greta]; //will use this to mark off cells after they are added to jets
  for (int ie = 0; ie < m_greta; ++ie) { //initialise everything to be safe
    for (int ip = 0; ip < m_grphi; ++ip) {
      etgrid[ip][ie] = 0.;
      etgridused[ip][ie] = false;
    }
  }

  // Loop over all particles in the event and build up the grid
  const HepMC::GenEvent* genEvt = event();
  for (auto part: *genEvt) {
    /// @todo Use MCPID to identify leptons
    // if ( part->status() == 1 ) {// stables only
      if(MC::isGenStable(part)) { //stables only
      if ( (part->pdg_id() != 13 ) &&  (part->pdg_id() != -13 ) &&
           (part->pdg_id() != 12 ) && (part->pdg_id() != -12 ) &&
           (part->pdg_id() != 14 ) && (part->pdg_id() != -14 ) &&
           (part->pdg_id() != 16 ) && (part->pdg_id() != -16 ) &&
           (std::abs(part->momentum().pseudoRapidity()) <= m_emaxeta) ) { // no neutrinos or muons and particles must be in active range
        int ip, ie;
        ip = (int) ((m_twopi/2.+ part->momentum().phi())/m_edphi); //phi is in range -CLHEP::pi to CLHEP::pi
        ie = (int) ((part->momentum().pseudoRapidity()+m_emaxeta)/m_edeta);
        if (ie < 0 || (ie >= m_greta)) { // its outside the ends so we should not be here
          ATH_MSG_FATAL("Jet too close to end");
          return StatusCode::FAILURE;
        }
        while (ip < 0) ip += m_grphi; //fix phi wrapping note that this is done after rr is calculated
        while (ip > m_grphi-1) ip -= m_grphi; //fix phi wrapping note that this is done after rr is calculated
        etgrid[ip][ie] = etgrid[ip][ie]+part->momentum().perp(); // fortran had pt here
      }
    }
  }

  // Find the highest cell; we loop here until we cannot find more jets
  double ethigh = 2.*m_Stop; // et of highest cell
  while (ethigh > m_Stop) { // stop looping when there are no cells left above threshold;
    ethigh = 0.;
    int etahigh = 0;
    int phihigh = 0;
    for (int ie0 = m_netacell; ie0 < m_greta-m_netacell; ++ie0) { // only look away from the edges
      for (int ip0 = 0; ip0 < m_grphi; ++ip0) {
        if (etgrid[ip0][ie0]>ethigh && !etgridused[ip0][ie0]) {
          ethigh = etgrid[ip0][ie0];
          etahigh = ie0;
          phihigh = ip0;
        }
      }
    }

    if (ethigh > m_Stop) { // this passes only if there is tower above threshold
      // new jet
      CLHEP::HepLorentzVector FoundJet;
      double jetpx = 0.;
      double jetpy = 0.;
      double jetpz = 0.;
      double jete = 0.;
      if (!m_Type) { //grid handle differantly if there are an even number of cells
        if (m_netacell2 % 2 == 0 && m_nphicell2 % 2 == 1) { // eta even
          double sum = 0.;
          double sum1 = 0.;
          for (int ip0 = 0; ip0 < m_nphicell2; ip0++) {
            int ip1 = ip0-m_nphicell+phihigh;
            sum = sum+etgrid[ip1][etahigh-1];
            sum1 = sum1+etgrid[ip1][etahigh+1];
          }
          if (sum < sum1) {
            etahigh += 1; //shift over by one
          }
        }
        if (m_netacell2 % 2 == 1 && m_nphicell2 % 2 == 0) { // phi even
          double sum = 0.;
          double sum1 = 0.;
          for (int ie0 = 0; ie0 < m_netacell2; ie0++) {
            int ie1 = ie0-m_netacell+etahigh;
            sum = sum+etgrid[(phihigh-1)%m_grphi][ie1];
            sum1 = sum1+etgrid[(phihigh+1)%m_grphi][ie1];
          }
          if (sum < sum1) {
            phihigh = (phihigh+1) % m_grphi; //shift over by one
          }
        }
        if (m_netacell2 % 2 == 0 && m_nphicell2 % 2 == 0) { // both even
          double sum = 0.;
          double sum1 = 0.;
          double sum2 = 0.;
          double sum3 = 0.;
          for (int ie0 = 0; ie0 < m_netacell2; ie0++) {
            for (int ip0 = 0; ip0 < m_nphicell2; ip0++) {
              int ip1 = ip0-m_nphicell+phihigh;
              int ie1 = ie0-m_netacell+etahigh;
              if (!etgridused[ip1][ie1]) sum=sum+etgrid[ip1][ie1];
              if (!etgridused[ip1][ie1+1]) sum1=sum1+etgrid[ip1][ie1+1];
              if (!etgridused[ip1+1][ie1]) sum2=sum2+etgrid[(ip1+1)%m_grphi][ie1];
              if (!etgridused[ip1+1][ie1+1]) sum3=sum3+etgrid[(ip1+1)%m_grphi][ie1+1];
            }
          }
          if (sum < sum1 && sum2 < sum1 && sum3 < sum1) etahigh=etahigh+1;
          if (sum < sum2 && sum1 <= sum2 && sum3 < sum2) phihigh=(phihigh+1)%m_grphi;
          if (sum < sum3 && sum2 <= sum3 && sum1 <= sum3) {
            etahigh = etahigh+1;
            phihigh = (phihigh+1)%m_grphi;
          }
        }
      }
      // Add up stuff around high cell
      for (int ie0 = 0; ie0 < m_netacell2; ++ie0) {
        int ie1 = ie0-m_netacell+etahigh;
        if ( (ie1<0) || (ie1>=  m_greta)) { // its outside the ends so we should not be here
          ATH_MSG_FATAL("  Jet too close to end");
          return StatusCode::FAILURE;
        }
        for (int ip0 = 0; ip0 < m_nphicell2; ++ip0) {
          int ip1 = ip0-m_nphicell+phihigh;
          // are we using a cone, if so check that its inside
          double rr = (ie1-etahigh)*(ie1-etahigh)*m_edeta*m_edeta+(ip1-phihigh)*(ip1-phihigh)*m_edphi*m_edphi;
          while ( ip1 < 0) ip1 += m_grphi; // fix phi wrapping note that this is done after rr is calculated
          while (ip1 > m_grphi-1) ip1 -= m_grphi; // fix phi wrapping note that this is done after rr is calculated
          if (rr<  m_Cone*m_Cone || !m_Type) { // make sure that its inside
            //check that the cell can be used and add energy to jet and mark the cell as used
            if (!etgridused[ip1][ie1]) {
              etgridused[ip1][ie1] = true;
              jetpx = jetpx+etgrid[ip1][ie1]*cos(-m_twopi/2.+(ip1+0.5)*m_edphi);
              jetpy = jetpy+etgrid[ip1][ie1]*sin(-m_twopi/2.+(ip1+0.5)*m_edphi);
              jetpz = jetpz+etgrid[ip1][ie1]*sinh((ie1+0.5)*m_edeta-m_emaxeta);
              jete = jete+etgrid[ip1][ie1]*cosh((ie1+0.5)*m_edeta-m_emaxeta);
            }
          }
        }
      }
      FoundJet.setPx(jetpx);
      FoundJet.setPy(jetpy);
      FoundJet.setPz(jetpz);
      FoundJet.setE(jete);
      if (std::abs(FoundJet.pseudoRapidity()) < m_UserEta) {
        m_Jets.push_back(FoundJet);   //OK we found one. add it to the list  if its inside the eta region
      }
    }
  }
  sort(m_Jets.begin(),m_Jets.end(),std::greater<McObj>());
  ATH_MSG_DEBUG("  Summary.  " << " Number of jets found   = " <<  m_Jets.size() <<  " \n ");
  if (m_Jets.size() > 0) {
    ATH_MSG_DEBUG(" Highest pt (in GeV)  " <<  (m_Jets[0].P().perp()/Gaudi::Units::GeV)  <<   "   Rapidity " <<m_Jets[0].P().pseudoRapidity()<<  "   Phi "<< m_Jets[0].P().phi() << "\n ");
    ATH_MSG_DEBUG(" Second Highest pt (in GeV)  " <<  (m_Jets[1].P().perp()/Gaudi::Units::GeV)  <<   "   Rapidity " <<m_Jets[1].P().pseudoRapidity()<<  "   Phi "<< m_Jets[1].P().phi() << "\n ");
    ATH_MSG_DEBUG(" Lowest pt (in GeV)  " <<  (m_Jets[m_Jets.size()-1].P().perp()/Gaudi::Units::GeV)  <<  "   Rapidity " <<m_Jets[m_Jets.size()-1].P().pseudoRapidity() <<  "   Phi " << m_Jets[m_Jets.size()-1].P().phi() << "\n ");
  }
  // now look at the jets and check the filter
  if (m_Jets.size() >= (unsigned) m_UserNumber) {
    if (m_Jets[m_UserNumber-1].P().perp() > m_UserThresh) {
      m_Jets.clear(); //clean it out
      return StatusCode::SUCCESS;
    }
  }
  setFilterPassed(false); // it failed to find any useful jets
  m_Jets.clear(); //clean out the found jets
  return StatusCode::SUCCESS;
}
