/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDMaker/src/PAUGammaAssociationTool.cxx
 * @author Mike Hance <hance@hep.upenn.edu>, based on similar tools by scott snyder <snyder@bnl.gov>
 * @date March, 2010
 * @brief Associate photon to PAU egamma object
 */


#include "PAUGammaAssociationTool.h"
#include "EventKernel/INavigable4Momentum.h"
#include "AthenaKernel/errorcheck.h"
#include "FourMomUtils/P4Helpers.h"

#include "PhotonAnalysisUtils/PAUegamma.h"

namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
PAUGammaAssociationTool::PAUGammaAssociationTool (const std::string& type,
						  const std::string& name,
						  const IInterface* parent)
  : Base (type, name, parent),
    m_getter (this)
{
  declareProperty ("Getter", m_getter,
                   "ICollectionGetterTool instance defining "
                   "the target collection");

  declareProperty ("DRcut", m_dr_cut=.01,
		   "If the object can't be matched by pointer values, "
		   "check to see if there are any objects within DRcut.");
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode PAUGammaAssociationTool::initialize()
{
  // Retrieve the getter tool and configure it, saying that we want
  // to get @c PAU::egamma objects from it.
  CHECK( m_getter.retrieve() );
  CHECK( m_getter->configureD3PD<PAU::egamma>() );
  return Base::initialize();
}


/**
 * @brief Create any needed tuple variables.
 */
StatusCode PAUGammaAssociationTool::book ()
{
  return StatusCode::SUCCESS;
}


/**
 * @brief Return the target object.
 * @param p The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const PAU::egamma*
PAUGammaAssociationTool::get (const INavigable4Momentum& p)
{

  if (m_getter->reset().isFailure()) {
    REPORT_MESSAGE (MSG::ERROR) << "Can't reset input getter.";
    return 0;
  }

  const Analysis::Photon* p_orig = dynamic_cast<const Analysis::Photon*>(&p);
  const Analysis::Electron* e_orig = dynamic_cast<const Analysis::Electron*>(&p);

  if(p_orig == 0 && e_orig == 0){
    REPORT_MESSAGE (MSG::ERROR) << "Can't cast input object to an Analysis::Photon or an Analysis::Electron.";
    return 0;
  }
    

  const PAU::egamma* pout = 0;
  while ( const PAU::egamma* obj =
          m_getter->next<PAU::egamma>())
  {
    if (p_orig != 0 && p_orig == obj->photon()){
      pout = obj;
      break;
    }
    else if (e_orig != 0 && e_orig == obj->electron()){
      pout = obj;
      break;
    }
  }

  // now, we can have cases where the pointers don't match for some reason.
  // so, use a dR match to figure out which one should be kept.  We'll make
  // the dR match pretty tight, since the only reason we shouldn't find a match
  // is if, for instance, the PAU::gamma object is pointing to an object
  // in PhotonCollection while the Analysis::Photon object is from the
  // PhotonAODCollection.
  float 
    orig_phi=0, 
    orig_eta=0, 
    dphi=0, 
    deta=0, 
    dR=0;
  if(pout == 0){
    if(p_orig){
      orig_phi = p_orig->phi();
      orig_eta = p_orig->eta();
    }
    else if(e_orig){
      orig_phi = e_orig->phi();
      orig_eta = e_orig->eta();
    }

    if(m_getter->reset() != StatusCode::SUCCESS){
      REPORT_MESSAGE (MSG::ERROR) << "Could not reset photon getter!";
      return 0;
    }
    while ( const PAU::egamma* obj =
	    m_getter->next<PAU::egamma>())
      {
	dphi = P4Helpers::deltaPhi(obj->phi(),orig_phi);
	deta = obj->eta() - orig_eta;
	dR = std::sqrt(dphi*dphi + deta*deta);
	if(dR < m_dr_cut){
	  pout = obj;
	  break;
	}
      }
  }

  return pout;
}


} // namespace D3PD
