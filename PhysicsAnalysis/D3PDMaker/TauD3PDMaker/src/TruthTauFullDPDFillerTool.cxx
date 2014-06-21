/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TruthTauFullDPDFillerTool.cxx 550729 2013-06-12 09:47:32Z ssnyder $
/**
 * @file TauD3PDMaker/src/TruthTauDPDFillerTool.cxx
 * @author Sebastian Fleischmann / modified: Christian Limbach
 * @date Jul, 2010 / May 2012
 * @brief Block filler tool for PanTau details.
 */


#include "TruthTauFullDPDFillerTool.h"
#include "TauTrackEvent/TruthTau.h"
#include "AthenaKernel/errorcheck.h"
 
//helper function to sort 4 momenta of neutral pions in tau by Et
bool        sortPionsByEt(CLHEP::HepLorentzVector momA, CLHEP::HepLorentzVector momB) {
  return momA.et() > momB.et();
}

namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TruthTauFullDPDFillerTool::TruthTauFullDPDFillerTool (const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent)
  : BlockFillerTool<TruthParticle> (type, name, parent),
  m_tauClassUtil(),
  m_TauTruthType(0),
  m_resonancePdgId(0),
  //extra
  m_nPi0(0),
  m_nPhotonConversions(0),
  m_nChargedProducts(0),
  m_nNeutralProducts(0),
  //mark's variables
  m_ChargedEt(0),
  m_NeutralEt(0),
  m_VisEt(0)
{
  book().ignore(); // Avoid coverity warnings.
}


/**
 * @brief Book variables for this block.
 */
StatusCode TruthTauFullDPDFillerTool::book()
{
    CHECK( addVariable ("DecayType", m_TauTruthType) );
    CHECK( addVariable ("ResonancePdgId", m_resonancePdgId) );
    
    CHECK( addVariable ("nPi0", m_nPi0) );
    CHECK( addVariable ("nPhotonConversions", m_nPhotonConversions) );
    CHECK( addVariable ("nChargedProducts", m_nChargedProducts) );
    CHECK( addVariable ("nNeutralProducts", m_nNeutralProducts) );
    
    CHECK( addVariable ("ChargedEt", m_ChargedEt) );
    CHECK( addVariable ("NeutralEt", m_NeutralEt) );
    CHECK( addVariable ("VisibleEt", m_VisEt) );
     
    CHECK( addVariable ("Pi0_1st_E",    m_Pi0_1st_E) );
    CHECK( addVariable ("Pi0_1st_Eta",  m_Pi0_1st_Eta) );
    CHECK( addVariable ("Pi0_1st_Phi",  m_Pi0_1st_Phi) );
    CHECK( addVariable ("Pi0_1st_Pt",   m_Pi0_1st_Pt) );
    
    CHECK( addVariable ("Pi0_2nd_E",    m_Pi0_2nd_E) );
    CHECK( addVariable ("Pi0_2nd_Eta",  m_Pi0_2nd_Eta) );
    CHECK( addVariable ("Pi0_2nd_Phi",  m_Pi0_2nd_Phi) );
    CHECK( addVariable ("Pi0_2nd_Pt",   m_Pi0_2nd_Pt) );
    
    CHECK( addVariable ("Pi0_3rd_E",    m_Pi0_3rd_E) );
    CHECK( addVariable ("Pi0_3rd_Eta",  m_Pi0_3rd_Eta) );
    CHECK( addVariable ("Pi0_3rd_Phi",  m_Pi0_3rd_Phi) );
    CHECK( addVariable ("Pi0_3rd_Pt",   m_Pi0_3rd_Pt) );    
    
    return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
StatusCode TruthTauFullDPDFillerTool::fill (const TruthParticle& p)
{
    const TauID::TruthTau* truthTau = dynamic_cast<const TauID::TruthTau*>(&p);
    if (!truthTau) {
        ATH_MSG_WARNING( "Dyn cast from TruthParticle to TruthTau did not succeed!");
        *m_TauTruthType=999;
        *m_resonancePdgId=999;
        *m_nPi0 = -1;
        *m_nPhotonConversions = -1;
        *m_nChargedProducts = -1;
        *m_nNeutralProducts = -1;
        *m_ChargedEt = -999999.;
        *m_NeutralEt = -999999.;
        *m_VisEt = -999999.;

        *m_Pi0_1st_E = -9999;
        *m_Pi0_1st_Eta = -19;
        *m_Pi0_1st_Phi = -19;
        *m_Pi0_1st_Pt = -9999;

        *m_Pi0_2nd_E = -9999;
        *m_Pi0_2nd_Eta = -19;
        *m_Pi0_2nd_Phi = -19;
        *m_Pi0_2nd_Pt = -9999;

        *m_Pi0_3rd_E = -9999;
        *m_Pi0_3rd_Eta = -19;
        *m_Pi0_3rd_Phi = -19;
        *m_Pi0_3rd_Pt = -9999;
        return StatusCode::SUCCESS;
    }
    
    // fill basic properties
    *m_TauTruthType             = m_tauClassUtil.getTruthTauType(*truthTau, msg());
    *m_resonancePdgId           = truthTau->resonancePdgId();
    *m_nPi0                     = truthTau->nPi0();
    *m_nPhotonConversions       = truthTau->nPhotonConversions();
    *m_nChargedProducts         = truthTau->chargedProducts().size();
    *m_nNeutralProducts         = truthTau->neutralProducts().size();
    
    
    // fill energies
    double chargedET    = 0.0;
    double neutralET    = 0.0;
    double visibleET    = 0.0;
    
    const std::vector<const TruthParticle*> chargedProducts = truthTau->chargedProducts();
    const std::vector<const TruthParticle*> neutralProducts = truthTau->neutralProducts();

    std::vector<const TruthParticle*>::const_iterator firstCharged = chargedProducts.begin();
    std::vector<const TruthParticle*>::const_iterator lastCharged = chargedProducts.end();

    for (; firstCharged != lastCharged; ++firstCharged) {
        visibleET += (*firstCharged)->et();
        chargedET += (*firstCharged)->et();
    }

    //list to hold the 4 momenta of the neutral pions
    std::vector<CLHEP::HepLorentzVector> pionsMomenta = std::vector<CLHEP::HepLorentzVector>(0);

    std::vector<const TruthParticle*>::const_iterator firstNeutral = neutralProducts.begin();
    std::vector<const TruthParticle*>::const_iterator lastNeutral = neutralProducts.end();

    for (; firstNeutral != lastNeutral; ++firstNeutral) {
        visibleET += (*firstNeutral)->et();
        neutralET += (*firstNeutral)->et();
        
        if((*firstNeutral)->pdgId() == 111) {
            pionsMomenta.push_back( CLHEP::HepLorentzVector( (*firstNeutral)->px(),
                                                             (*firstNeutral)->py(),
                                                             (*firstNeutral)->pz(),
                                                             (*firstNeutral)->e()  ) );
        }
    }

    *m_VisEt        = visibleET;
    *m_ChargedEt    = chargedET;
    *m_NeutralEt    = neutralET;
     
    //sort and fill the neutral pions
    std::sort( pionsMomenta.begin(), pionsMomenta.end(), sortPionsByEt );
     
    if(pionsMomenta.size() >= 1) {
        *m_Pi0_1st_E    = pionsMomenta[0].et();
        *m_Pi0_1st_Eta  = pionsMomenta[0].eta();
        *m_Pi0_1st_Phi  = pionsMomenta[0].phi();
        *m_Pi0_1st_Pt   = pionsMomenta[0].perp();
    }
    
    if(pionsMomenta.size() >= 2) {
        *m_Pi0_2nd_E    = pionsMomenta[1].et();
        *m_Pi0_2nd_Eta  = pionsMomenta[1].eta();
        *m_Pi0_2nd_Phi  = pionsMomenta[1].phi();
        *m_Pi0_2nd_Pt   = pionsMomenta[1].perp();
    }
    
    if(pionsMomenta.size() >= 3) {
        *m_Pi0_3rd_E    = pionsMomenta[2].et();
        *m_Pi0_3rd_Eta  = pionsMomenta[2].eta();
        *m_Pi0_3rd_Phi  = pionsMomenta[2].phi();
        *m_Pi0_3rd_Pt   = pionsMomenta[2].perp();
    }
            
    return StatusCode::SUCCESS;
}

StatusCode TruthTauFullDPDFillerTool::finalize() {
    return StatusCode::SUCCESS;
}

} // namespace D3PD
