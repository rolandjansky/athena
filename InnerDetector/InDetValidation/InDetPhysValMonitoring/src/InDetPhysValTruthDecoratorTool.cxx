/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPhysValTruthDecoratorTool.cxx
 * @author shaun roe
**/

#include "InDetPhysValTruthDecoratorTool.h"
#include <limits>

#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"
//#include "GeneratorUtils/PIDUtils.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include "TrkParameters/TrackParameters.h" //Contains typedef to Trk::CurvilinearParameters

//ref: ​https://svnweb.cern.ch/trac/atlasoff/browser/Tracking/TrkEvent/TrkParametersBase/trunk/TrkParametersBase/CurvilinearParametersT.h
namespace {
  template <class T>
	bool
	is_nan(const T & n){
		return (n!=n);
	}
	
	double chargeOnParticle(const int pid){
    	  if (pid == 1000010020) return 1.0; //deuteron
    	  if (pid == 1000010030) return 1.0; //triton
    	  double charge(std::numeric_limits<double>::quiet_NaN());
    	  TDatabasePDG p;
    	  TParticlePDG* ap = TDatabasePDG::Instance()->GetParticle (pid);
	  if (ap){
      	    charge=ap->Charge()/3.0; //see :http://root.cern.ch/root/html/TParticlePDG.html#TParticlePDG:fCharge
    	  }
    	  return charge;
	}
}





InDetPhysValTruthDecoratorTool::InDetPhysValTruthDecoratorTool(const std::string& type,const std::string& name,const IInterface* parent):
AthAlgTool(type,name,parent),
m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"){
declareInterface<IInDetPhysValDecoratorTool>(this);

}
InDetPhysValTruthDecoratorTool::~InDetPhysValTruthDecoratorTool (){
  //nop
}

StatusCode 
InDetPhysValTruthDecoratorTool::initialize(){
  StatusCode sc = AlgTool::initialize();
  if (sc.isFailure()) return sc;
  ATH_CHECK(m_extrapolator.retrieve());
  return sc;
}

StatusCode 
InDetPhysValTruthDecoratorTool::finalize  (){
  StatusCode sc(StatusCode::SUCCESS);
  return sc;
}

bool
InDetPhysValTruthDecoratorTool::decorateTruth(const xAOD::TruthParticle & particle){
		ATH_MSG_VERBOSE ("Decorate truth with d0 etc");
    const Amg::Vector3D momentum(particle.px(), particle.py(), particle.pz());
    const int pid(particle.pdgId());
    double charge = chargeOnParticle(pid);
    if (is_nan(charge)){
      ATH_MSG_WARNING("charge not found on particle with pid "<<pid);
      return false;
    } 

    static bool errorEmitted(false);
    const xAOD::TruthVertex * ptruthVertex(0);
    try{
      ptruthVertex=particle.prodVtx();
    } catch (std::exception e){
      if (not errorEmitted) ATH_MSG_WARNING("A non existent production vertex was requested in calculating the track parameters d0 etc");
      errorEmitted=true;
      return false;
    }
    if (!ptruthVertex){
      ATH_MSG_WARNING("A production vertex pointer was retrieved, but it is NULL");
      return false;
    }
    const Amg::Vector3D position(ptruthVertex->x(), ptruthVertex->y(), ptruthVertex->z());
    //delete ptruthVertex;ptruthVertex=0;
    const Trk::CurvilinearParameters cParameters(position, momentum, charge);
    const Trk::TrackParameters* tP = m_extrapolator->extrapolate(cParameters,Trk::PerigeeSurface(), Trk::anyDirection, false);
    if (tP){
      double d0_truth = tP->parameters()[Trk::d0];
      double theta_truth = tP->parameters()[Trk::theta];
      double z0_truth = tP->parameters()[Trk::z0];
      double phi_truth = tP->parameters()[Trk::phi];
      double qOverP_truth = tP->parameters()[Trk::qOverP]; //P or Pt ??
      double z0st_truth = z0_truth * std::sin(theta_truth);
      particle.auxdecor<float>("d0") = d0_truth;
      particle.auxdecor<float>("z0") = z0_truth;
      particle.auxdecor<float>("phi") = phi_truth;
      particle.auxdecor<float>("theta") = theta_truth;
      particle.auxdecor<float>("z0st") = z0st_truth;
      particle.auxdecor<float>("qopt") = qOverP_truth;
      delete tP;tP=0;
      return true;
    } else {
      ATH_MSG_WARNING("The TrackParameters pointer for this TruthParticle is NULL");
      return false;
    }
}

/** references:
https://svnweb.cern.ch/trac/atlasoff/browser/Event/xAOD/xAODTruth/trunk/Root/TruthParticle_v1.cxx
Typedefs for various TrackParameters:
https://svnweb.cern.ch/trac/atlasoff/browser/Tracking/TrkEvent/TrkParameters/trunk/TrkParameters/TrackParameters.h
TrackParameters is a typedef to:
https://svnweb.cern.ch/trac/atlasoff/browser/Tracking/TrkEvent/TrkParametersBase/trunk/TrkParametersBase
https://svnweb.cern.ch/trac/atlasoff/browser/Tracking/TrkEvent/TrkParametersBase/trunk/TrkParametersBase/CurvilinearParametersT.h
https://svnweb.cern.ch/trac/atlasoff/browser/Event/xAOD/xAODBase/trunk/xAODBase/IParticle.h
**/

/**references:
  // https://svnweb.cern.ch/trac/atlasoff/browser/Event/xAOD/xAODTracking/trunk/xAODTracking/versions/TrackParticle_v1.h
  // https://svnweb.cern.ch/trac/atlasoff/browser/Event/xAOD/xAODTruth/trunk/xAODTruth/versions/TruthParticle_v1.h
  //original code like this:
  
  float m_track_truth_eta  = generatedTrackPerigee->eta();
  int ieta = int(m_track_truth_eta/0.25 + 10.0);
    if (ieta < 0 || ieta>19){
      delete generatedTrackPerigee;
      continue;
    }
  m_hd0[ieta]->Fill(trkd0-m_track_truth_d0);
**/

/** Email from Andi Salzburger to Shaun Roe **

  the correct way is actually to extrapolate it.
  So you’d need to retrieve the extrapolator:
    TooHandle<IExtrapolator> m_extrapolator;
  
  -> retrieve it in your code and use the AtlasExtrapolator (which is fully configured, and can be imported in python as from TrkExTools.AtlasExtrapolator import AtlasExtrapolator).
  Then, in your code you need 3 lines:
  
    Trk::CurvilinearParameters cParameters(truthParticle->position(), truthParticle->momentum(), truthParticle->charge() ); // sorry for the little pseudo_code
    // for the moment express to (0,0,0), we may change this later
    const Trk::TrackParameters* tP = m_extrapolator->extrapolate(cParametrs,Trk::PerigeeSurface(), Trk::anyDirection, false);
    if (tP){
      double d0_truth = tP->parameters()[Trk::d0];
    }
  This will be super-transparent, because even if TrackingCP decides later on to change away from (0,0,0) you just need to change the Trk::PerigeeSurface() to the new one with different position.
  
**/

