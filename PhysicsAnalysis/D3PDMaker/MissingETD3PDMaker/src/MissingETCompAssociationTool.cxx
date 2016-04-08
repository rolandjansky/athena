/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MissingETD3PDMaker/src/MissingETCompAssociationTool.cxx
 * @author Jet Goodson <jgoodson@cern.ch>, based on similar tools by scott snyder <snyder@bnl.gov>
 * @date September, 2010
 * @brief Associate electron to MET Composition object
 */

#include "MissingETD3PDMaker/MissingETCompAssociationTool.h"
#include "FourMomUtils/P4Helpers.h"
#include "AthenaKernel/errorcheck.h"
#include "egammaEvent/egamma.h"
#include "egammaEvent/egammaContainer.h"
#include "muonEvent/MuonContainer.h"
#include "muonEvent/Muon.h"
#include "JetEvent/Jet.h"
#include "tauEvent/TauJetContainer.h"
#include "tauEvent/TauJet.h"
#include "CaloEvent/CaloCluster.h"
#include "Particle/TrackParticle.h"

namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
MissingETCompAssociationTool::MissingETCompAssociationTool (const std::string& type,
						  const std::string& name,
						  const IInterface* parent)
  : Base (type, name, parent),
    m_getter (this)
{
  declareProperty ("Getter", m_getter,
                   "IObjectGetterTool instance defining "
                   "the target collection");
  declareProperty ("AllowMissing", m_allowMissing = false,
		   "If true, don't generate an error "
		   "if the target is missing.");
  declareProperty ("ObjectType", m_objectType = "egamma",
		   "what type of object is it "
		   "so METComp tools know what to get");

  book().ignore(); // Avoid coverity warning.
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode MissingETCompAssociationTool::initialize()
{
  // Retrieve the getter tool and configure it, saying that we want
  // to get @c PAU::gamma objects from it.
  CHECK( m_getter.retrieve() );
  CHECK( m_getter->configureD3PD<MissingETComposition>() );
  return Base::initialize();
}


/**
 * @brief Create any needed tuple variables.
 */
StatusCode MissingETCompAssociationTool::book ()
{

  CHECK( addVariable ("wpx", m_vec_weight_px) );
  CHECK( addVariable ("wpy", m_vec_weight_py) );
  CHECK( addVariable ("wet", m_vec_weight_ET) );
  CHECK( addVariable ("statusWord", m_vec_statusWord) );

  return StatusCode::SUCCESS;
}


/**
 * @brief Return the target object.
 * @param p The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const MissingETComposition*
MissingETCompAssociationTool::get (const INavigable4Momentum& p)
{

  const MissingETComposition* pout =
    m_getter->get<MissingETComposition>(m_allowMissing);
  if(pout == 0){
    if (!m_allowMissing)
      REPORT_MESSAGE (MSG::ERROR) << "In MissingETD3PDMaker::MissingETCompAssociationTool, MissingETComposition == 0.";
    return 0;
  }
  
  weighter (&p, pout);
#if 0
  if(m_objectType == "egamma"){
    const egamma* e_orig = dynamic_cast<const egamma*>(&p);
    if(e_orig == 0){
      REPORT_MESSAGE (MSG::ERROR) << "Couldn't cast inav4mom to physics object.";
      return 0;
    }
    weighter<egamma>(e_orig, pout);
  }
  else if (m_objectType == "Muon"){
    const Analysis::Muon* m_orig =  dynamic_cast<const Analysis::Muon*>(&p);
    if(m_orig == 0){
      REPORT_MESSAGE (MSG::ERROR) << "Couldn't cast inav4mom to physics object.";
      return 0;
    }
    weighter<Analysis::Muon>(m_orig, pout);
  }
  else if (m_objectType == "Jet"){
    const Jet* e_orig = dynamic_cast<const Jet*>(&p);
    if(e_orig == 0){
      REPORT_MESSAGE (MSG::ERROR) << "Couldn't cast inav4mom to physics object.";
      return 0;
    }
    weighter<Jet>(e_orig, pout);
  }
  else if (m_objectType == "TauJet"){
    const Analysis::TauJet* m_orig =  dynamic_cast<const Analysis::TauJet*>(&p);
    if(m_orig == 0){
      REPORT_MESSAGE (MSG::ERROR) << "Couldn't cast inav4mom to physics object.";
      return 0;
   }
    weighter<Analysis::TauJet>(m_orig, pout);
  }
  else if (m_objectType == "CaloCluster"){
    const CaloCluster* m_orig =  dynamic_cast<const CaloCluster*>(&p);
    if(m_orig == 0){
      REPORT_MESSAGE (MSG::ERROR) << "Couldn't cast inav4mom to physics object.";
      return 0;
   }
    weighter<CaloCluster>(m_orig, pout);
  }
  else if (m_objectType == "TrackParticle"){
    const Rec::TrackParticle* m_orig =  dynamic_cast<const Rec::TrackParticle*>(&p);
    if(m_orig == 0){
      REPORT_MESSAGE (MSG::ERROR) << "Couldn't cast inav4mom to physics object.";
      return 0;
   }
    weighter<Rec::TrackParticle>(m_orig, pout);
  }
  else{
    REPORT_MESSAGE (MSG::ERROR) << "Type not given in MissingETCompAssociationTool.";
    std::vector<float> temp_x;
    std::vector<float> temp_y;
    std::vector<float> temp_t;
    std::vector<unsigned int> temp_status;
    temp_x.push_back(0.0);
    temp_y.push_back(0.0);
    temp_t.push_back(0.0);
    temp_status.push_back(0);
    *m_vec_weight_px = temp_x;
    *m_vec_weight_py = temp_y;
    *m_vec_weight_ET = temp_t;
    *m_vec_statusWord = temp_status;
  }
#endif
  


  return pout;
  
}//end of mutant getter
  

 template <class T>
 void MissingETCompAssociationTool::weighter(const T* objecto, const MissingETComposition* compo){
   MissingETComposition::const_iterator<T> first(compo->beginIterator<T>());
   MissingETComposition::const_iterator<T> last(compo->endIterator<T>());
   bool isMatched = false;
   std::vector<float> temp_x;
   std::vector<float> temp_y;
   std::vector<float> temp_t;
   std::vector<unsigned int> temp_status;
   //*m_px = objecto->px();
   for ( ; first != last; first++ ){
     if((*first) == objecto){
       isMatched = true;
       temp_x.push_back(first.getParameter().wpx());
       temp_y.push_back(first.getParameter().wpy());
       temp_t.push_back(first.getParameter().wet());
       temp_status.push_back(first.getParameter().statusWord());
     }
   }
   if(!isMatched){
     temp_x.push_back(0.0);
     temp_y.push_back(0.0);
     temp_t.push_back(0.0);
     temp_status.push_back(0);
   }
   *m_vec_weight_px = temp_x;
   *m_vec_weight_py = temp_y;
   *m_vec_weight_ET = temp_t;
   *m_vec_statusWord = temp_status;
 }//end of weighter
  
  
void
MissingETCompAssociationTool::weighter(const INavigable4Momentum* objecto,
                                       const MissingETComposition* compo)
{
   MissingETComposition::object_iter first(compo->begin());
   MissingETComposition::object_iter last(compo->end());
   bool isMatched = false;
   std::vector<float> temp_x;
   std::vector<float> temp_y;
   std::vector<float> temp_t;
   std::vector<unsigned int> temp_status;
   //*m_px = objecto->px();
   for ( ; first != last; first++ ){
     if((*first) == objecto){
       isMatched = true;
       temp_x.push_back(first.getParameter().wpx());
       temp_y.push_back(first.getParameter().wpy());
       temp_t.push_back(first.getParameter().wet());
       temp_status.push_back(first.getParameter().statusWord());
     }
   }
   if(!isMatched){
     temp_x.push_back(0.0);
     temp_y.push_back(0.0);
     temp_t.push_back(0.0);
     temp_status.push_back(0);
   }
   *m_vec_weight_px = temp_x;
   *m_vec_weight_py = temp_y;
   *m_vec_weight_ET = temp_t;
   *m_vec_statusWord = temp_status;
 }//end of weighter
  
  
} // namespace D3PD
