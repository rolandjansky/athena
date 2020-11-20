/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/* Athena includes */
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IPartPropSvc.h"

/* Header include */
#include "FastCaloSimCaloExtrapolation.h"

/* ISF includes */
#include "ISF_FastCaloSimEvent/FastCaloSim_CaloCell_ID.h"
#include "ISF_FastCaloSimParametrization/IFastCaloSimGeometryHelper.h"
#include "ISF_FastCaloSimEvent/TFCSTruthState.h"

/* Tracking includes */
#include "TrkGeometry/TrackingGeometry.h"
#include "CaloTrackingGeometry/ICaloSurfaceHelper.h"

/* Geometry primitives */
#include "GeoPrimitives/GeoPrimitivesHelpers.h"

/* Particle data */
#include "HepPDT/ParticleDataTable.hh"

/* Preprocessor macro to use
   -- DEBUG   if CONDITION is True
   -- WARNING if CONDITION is False
 */
#define ATH_MSG_COND(MSG, CONDITION) \
do {                                 \
    if (CONDITION) {                 \
      ATH_MSG_DEBUG(MSG);            \
    } else {                         \
      ATH_MSG_WARNING(MSG);          \
    }                                \
} while (0)


FastCaloSimCaloExtrapolation::FastCaloSimCaloExtrapolation(const std::string& t, const std::string& n, const IInterface* p)
  : AthAlgTool(t,n,p)
  , m_extrapolator("TimedExtrapolator")
  , m_caloSurfaceHelper("CaloSurfaceHelper")
  , m_CaloGeometryHelper("FastCaloSimGeometryHelper")
{
  declareInterface<IFastCaloSimCaloExtrapolation>(this);

  m_surfacelist.resize(0);
  m_surfacelist.push_back(CaloCell_ID_FCS::PreSamplerB);
  m_surfacelist.push_back(CaloCell_ID_FCS::PreSamplerE);
  m_surfacelist.push_back(CaloCell_ID_FCS::EME1);
  m_surfacelist.push_back(CaloCell_ID_FCS::EME2);
  m_surfacelist.push_back(CaloCell_ID_FCS::FCAL0);
  
  declareProperty("CaloBoundaryR",      m_CaloBoundaryR);
  declareProperty("CaloBoundaryZ",      m_CaloBoundaryZ);
  declareProperty("CaloMargin",         m_calomargin);
  declareProperty("Surfacelist",        m_surfacelist);
  declareProperty("Extrapolator",       m_extrapolator);
  declareProperty("CaloEntrance",       m_caloEntranceName);
  declareProperty("CaloSurfaceHelper",  m_caloSurfaceHelper);
  declareProperty("CaloGeometryHelper", m_CaloGeometryHelper);
}

FastCaloSimCaloExtrapolation::~FastCaloSimCaloExtrapolation()
{
}

StatusCode FastCaloSimCaloExtrapolation::initialize(){

  ATH_MSG_INFO( "Initializing FastCaloSimCaloExtrapolation" );

  // Get CaloGeometryHelper
  ATH_CHECK(m_CaloGeometryHelper.retrieve());

  // Get PDG table
  IPartPropSvc* p_PartPropSvc = nullptr;

  ATH_CHECK(service("PartPropSvc", p_PartPropSvc));
  if(!p_PartPropSvc){
      ATH_MSG_ERROR("could not find PartPropService");
      return StatusCode::FAILURE;
  }

  m_particleDataTable = (HepPDT::ParticleDataTable*) p_PartPropSvc->PDT();
  if(!m_particleDataTable){
      ATH_MSG_ERROR("PDG table not found");
      return StatusCode::FAILURE;
  }
  //#########################

  // Get TimedExtrapolator
  if(!m_extrapolator.empty()){
      ATH_CHECK(m_extrapolator.retrieve());
      ATH_MSG_INFO("Extrapolator retrieved "<< m_extrapolator);
  }

  // Get CaloSurfaceHelper
  ATH_CHECK(m_caloSurfaceHelper.retrieve());

  ATH_MSG_INFO("m_CaloBoundaryR="<<m_CaloBoundaryR<<" m_CaloBoundaryZ="<<m_CaloBoundaryZ<<" m_caloEntranceName "<<m_caloEntranceName);

  return StatusCode::SUCCESS;

}

StatusCode FastCaloSimCaloExtrapolation::finalize(){
  ATH_MSG_INFO( "Finalizing FastCaloSimCaloExtrapolation" );
  return StatusCode::SUCCESS;
}

bool FastCaloSimCaloExtrapolation::getCaloSurface(TFCSExtrapolationState& result, std::vector<Trk::HitInfo>* hitVector) const{
  ATH_MSG_DEBUG("Start getCaloSurface()");

  //used to identify ID-Calo boundary in tracking tools
  int IDCaloBoundary = 3000;
  
  result.set_CaloSurface_sample(CaloCell_ID_FCS::noSample);
  result.set_CaloSurface_eta(-999);
  result.set_CaloSurface_phi(-999);
  result.set_CaloSurface_r(0);
  result.set_CaloSurface_z(0);
  double min_calo_surf_dist=1000;

  for(unsigned int i=0;i<m_surfacelist.size();++i){

    int sample=m_surfacelist[i];
    std::vector<Trk::HitInfo>::iterator it = hitVector->begin();

    while (it != hitVector->end() && it->detID != (IDCaloBoundary+sample)) it++;

    if(it==hitVector->end()) continue;

    Amg::Vector3D hitPos = (*it).trackParms->position();

    //double offset = 0.;
    double etaCalo = hitPos.eta();

    if(std::abs(etaCalo)<900){
        
        double phiCalo = hitPos.phi();
        double distsamp =deta(sample,etaCalo);

        if(distsamp<min_calo_surf_dist && min_calo_surf_dist>=0){
            
            //hitVector is ordered in r, so if first surface was hit, keep it
            result.set_CaloSurface_sample(sample);
            result.set_CaloSurface_eta(etaCalo);
            result.set_CaloSurface_phi(phiCalo);
            double rcalo = rent(sample,etaCalo);
            double zcalo = zent(sample,etaCalo);
            result.set_CaloSurface_r(rcalo);
            result.set_CaloSurface_z(zcalo);
            min_calo_surf_dist = distsamp;
            msg(MSG::DEBUG)<<" r="<<rcalo<<" z="<<zcalo;

            if(distsamp<0){
              msg(MSG::DEBUG)<<endreq;
              break;
            }
          }
        msg(MSG::DEBUG)<<endreq;
      }
    else
      msg(MSG::DEBUG)<<": eta > 900, not using this"<<endreq;
    }

  if(result.CaloSurface_sample() == CaloCell_ID_FCS::noSample){
      // first intersection with sensitive calo layer
      std::vector<Trk::HitInfo>::iterator it = hitVector->begin();

      while(it < hitVector->end() && (*it).detID != 3) it++;   // to be updated

      if (it == hitVector->end()) return false;  // no calo intersection, abort

      Amg::Vector3D surface_hitPos = (*it).trackParms->position();

      result.set_CaloSurface_eta(surface_hitPos.eta());
      result.set_CaloSurface_phi(surface_hitPos.phi());
      result.set_CaloSurface_r(surface_hitPos.perp());
      result.set_CaloSurface_z(surface_hitPos[Amg::z]);

      double pT=(*it).trackParms->momentum().perp();

      ATH_MSG_COND("only entrance to calo entrance layer found, no surface : eta="<<result.CaloSurface_eta()<<" phi="<<result.CaloSurface_phi()<<" r="<<result.CaloSurface_r()<<" z="<<result.CaloSurface_z()<<" pT="<<pT, std::abs(result.CaloSurface_eta())>4.9 || pT<500 || (std::abs(result.CaloSurface_eta())>4 && pT<1000));
    
    } //sample
  else ATH_MSG_DEBUG("entrance to calo surface : sample="<<result.CaloSurface_sample()<<" eta="<<result.CaloSurface_eta()<<" phi="<<result.CaloSurface_phi()<<" r="<<result.CaloSurface_r()<<" z="<<result.CaloSurface_z()<<" deta="<<min_calo_surf_dist);
    
  ATH_MSG_DEBUG("End getCaloSurface()");
  return true;
}


std::unique_ptr<std::vector<Trk::HitInfo>> FastCaloSimCaloExtrapolation::caloHits(const TFCSTruthState* truth, bool forceNeutral) const{
  // Start calo extrapolation
  ATH_MSG_DEBUG ("[ fastCaloSim transport ] processing particle "<<truth->pdgid() );

  auto hitVector = std::make_unique<std::vector<Trk::HitInfo>>();

  int     pdgId    = truth->pdgid();
  double  charge   = HepPDT::ParticleID(pdgId).charge();
  if (forceNeutral) charge   = 0.;

  // particle Hypothesis for the extrapolation
  Trk::ParticleHypothesis pHypothesis = m_pdgToParticleHypothesis.convert(pdgId, charge);

  ATH_MSG_DEBUG ("particle hypothesis "<< pHypothesis);

  // geantinos not handled by PdgToParticleHypothesis - fix there
  if (pdgId == 999) pHypothesis = Trk::geantino;

  Amg::Vector3D pos = Amg::Vector3D(truth->vertex().X(), truth->vertex().Y(), truth->vertex().Z());

  Amg::Vector3D mom(truth->X(), truth->Y(), truth->Z());

  ATH_MSG_DEBUG( "[ fastCaloSim transport ] x from position eta="<<pos.eta()<<" phi="<<pos.phi()<<" d="<<pos.mag()<<" pT="<<mom.perp() );

  // input parameters : curvilinear parameters
  Trk::CurvilinearParameters inputPar(pos,mom,charge);

  // stable vs. unstable check : ADAPT for FASTCALOSIM
  //double freepath = ( !m_particleDecayHelper.empty()) ? m_particleDecayHelper->freePath(isp) : - 1.;
  double freepath = -1.;
  //ATH_MSG_VERBOSE( "[ fatras transport ] Particle free path : " << freepath);
  // path limit -> time limit  ( TODO : extract life-time directly from decay helper )
  double tDec = freepath > 0. ? freepath : -1.;
  int decayProc = 0;

  /* uncomment if unstable particles used by FastCaloSim
  // beta calculated here for further use in validation
  double mass = m_particleMasses.mass[pHypothesis];
  double mom = isp.momentum().mag();
  double beta = mom/sqrt(mom*mom+mass*mass);

  if ( tDec>0.)
  {
  tDec = tDec/beta/CLHEP::c_light + isp.timeStamp();
  decayProc = 201;
  }
  */

  Trk::TimeLimit timeLim(tDec, 0., decayProc);        // TODO: set vertex time info

  // prompt decay ( uncomment if unstable particles used )
  //if ( freepath>0. && freepath<0.01 ) {
  //  if (!m_particleDecayHelper.empty()) {
  //    ATH_MSG_VERBOSE( "[ fatras transport ] Decay is triggered for input particle.");
  //    m_particleDecayHelper->decay(isp);
  //  }
  //  return 0;
  //}

  // presample interactions - ADAPT FOR FASTCALOSIM
  Trk::PathLimit pathLim(-1., 0);

  Trk::GeometrySignature nextGeoID=Trk::Calo;

  // first extrapolation to reach the ID boundary
  ATH_MSG_DEBUG( "[ fastCaloSim transport ] before calo entrance ");

  // get CaloEntrance if not done already
  if(!m_caloEntrance){
    m_caloEntrance = m_extrapolator->trackingGeometry()->trackingVolume(m_caloEntranceName);
    if(!m_caloEntrance)
      ATH_MSG_WARNING("CaloEntrance not found");
    else
      ATH_MSG_DEBUG("CaloEntrance found");
  }

  ATH_MSG_DEBUG( "[ fastCaloSim transport ] after calo entrance ");

  const Trk::TrackParameters* caloEntry = 0;

  if(m_caloEntrance && m_caloEntrance->inside(pos, 0.001) && !m_extrapolator->trackingGeometry()->atVolumeBoundary(pos,m_caloEntrance, 0.001)){
      std::vector<Trk::HitInfo>* dummyHitVector = 0;
      if(charge==0) caloEntry = m_extrapolator->transportNeutralsWithPathLimit(inputPar,pathLim, timeLim, Trk::alongMomentum, pHypothesis, dummyHitVector, nextGeoID, m_caloEntrance);
      else caloEntry = m_extrapolator->extrapolateWithPathLimit(inputPar, pathLim, timeLim, Trk::alongMomentum, pHypothesis, dummyHitVector, nextGeoID, m_caloEntrance);    
  }
  else caloEntry=&inputPar;

  ATH_MSG_DEBUG( "[ fastCaloSim transport ] after calo caloEntry ");

  if(caloEntry){
      const Trk::TrackParameters* eParameters = 0;

      // save Calo entry hit (fallback info)
      hitVector->push_back(Trk::HitInfo(caloEntry->clone(), timeLim.time, nextGeoID, 0.));

      ATH_MSG_DEBUG( "[ fastCaloSim transport ] starting Calo transport from position eta="<<caloEntry->position().eta()<<" phi="<<caloEntry->position().phi()<<" d="<<caloEntry->position().mag() );
      
      std::vector<Trk::HitInfo>* rawHitVector =  hitVector.get();
      if(charge == 0) eParameters = m_extrapolator->transportNeutralsWithPathLimit(*caloEntry,pathLim,timeLim, Trk::alongMomentum, pHypothesis, rawHitVector, nextGeoID);
      else eParameters = m_extrapolator->extrapolateWithPathLimit(*caloEntry, pathLim, timeLim, Trk::alongMomentum, pHypothesis, rawHitVector, nextGeoID);

      // save Calo exit hit (fallback info)
      if(eParameters) hitVector->push_back(Trk::HitInfo(eParameters, timeLim.time, nextGeoID, 0.));
    } //if caloEntry

  //used to identify ID-Calo boundary in tracking tools
  int IDCaloBoundary = 3000;

  if(msgLvl(MSG::DEBUG)){
    std::vector<Trk::HitInfo>::iterator it = hitVector->begin();
    while (it < hitVector->end()){
        int sample=(*it).detID;
        Amg::Vector3D hitPos = (*it).trackParms->position();
        ATH_MSG_DEBUG(" HIT: layer="<<sample<<" sample="<<sample-IDCaloBoundary<<" eta="<<hitPos.eta()<<" phi="<<hitPos.phi()<<" d="<<hitPos.mag());
        it++;
    }
  }

  std::vector<Trk::HitInfo>::iterator it2 = hitVector->begin();
  while(it2 < hitVector->end()){
    int sample=(*it2).detID;
    Amg::Vector3D hitPos = (*it2).trackParms->position();
    ATH_MSG_DEBUG(" HIT: layer="<<sample<<" sample="<<sample-IDCaloBoundary<<" eta="<<hitPos.eta()<<" phi="<<hitPos.phi()<<" r="<<hitPos.perp()<<" z="<<hitPos[Amg::z]);
    it2++;
  }

  // Extrapolation may fail for very low pT charged particles. Enforce charge 0 to prevent this 
  if (!forceNeutral && hitVector->empty()){
      ATH_MSG_DEBUG("forcing neutral charge in FastCaloSimCaloExtrapolation::caloHits");
      return caloHits(truth, true);
  }
  // Don't expect this ever to happen. Nevertheless, error handling should be improved. 
  // This may require changes in periphery (adjustments after setting function type to StatusCode)
  else if(hitVector->empty()) ATH_MSG_ERROR("Empty hitVector even after forcing neutral charge. This may cause a segfault soon.");

  return hitVector;
}


void FastCaloSimCaloExtrapolation::extrapolateToID(TFCSExtrapolationState& result, std::vector<Trk::HitInfo>* hitVector, const TFCSTruthState* truth) const{

  ATH_MSG_DEBUG("Start extrapolateToID()");

  //pT threshold of truth particles over which extrapolation failures will be printed as warnings
  const float transverseMomWarningLimit = 500;

  //initialize values
  result.set_IDCaloBoundary_eta(-999.);
  result.set_IDCaloBoundary_phi(-999.);
  result.set_IDCaloBoundary_r(0);
  result.set_IDCaloBoundary_z(0);
  result.set_IDCaloBoundary_AngleEta(-999.);
  result.set_IDCaloBoundary_Angle3D(-999.);
  
  //magnitude of extrapolated position
  double extPosDist = -1;

  for (unsigned int surfID = 0; surfID<3; surfID++){

    double R = m_CaloBoundaryR.at(surfID);
    double Z = m_CaloBoundaryZ.at(surfID);

    ATH_MSG_DEBUG("[ExtrapolateToID] Extrapolating to ID-Calo boundary with ID="<<surfID<<" R="<<R<<" Z="<<Z);

    //extrapolated position and momentum direction at IDCaloBoundary
    Amg::Vector3D extPos, momDir;
    
    //main extrapolation call
    if(!extrapolateToCylinder(hitVector, R, Z, extPos, momDir)) continue;
    
    double tolerance = 0.001;
    //test if z inside previous cylinder within some tolerance
    if(surfID > 0 && std::abs(extPos[Amg::z]) < m_CaloBoundaryZ[surfID-1] - tolerance) continue;

    //test if r inside next cylinder within some tolerance
    if(surfID < m_CaloBoundaryR.size()-1 && extPos.perp() < m_CaloBoundaryR[surfID + 1] - tolerance) continue;

    if(extPosDist >= 0 && extPos.mag() > extPosDist) continue;
    
    extPosDist = extPos.mag();

    result.set_IDCaloBoundary_eta(extPos.eta());
    result.set_IDCaloBoundary_phi(extPos.phi());
    result.set_IDCaloBoundary_r(extPos.perp());
    result.set_IDCaloBoundary_z(extPos[Amg::z]);
    
    //compute angle between extrapolated position vector and momentum at IDCaloBoundary
    //can be used to correct shower shapes for particles which do not originate from {0,0,0}
    double Angle3D  = Amg::angle(extPos, momDir);
    double AngleEta = extPos.theta() - momDir.theta();
    result.set_IDCaloBoundary_AngleEta(AngleEta);
    result.set_IDCaloBoundary_Angle3D(Angle3D);

  } //end of loop over surfaces
  
  if(result.IDCaloBoundary_eta() == -999) ATH_MSG_COND("[ExtrapolateToID] Failed extrapolation to ID-Calo boundary. \n[ExtrapolateToID] Particle with truth vertex at (" << truth->vertex().X() <<","<<truth->vertex().Y()<<","<<truth->vertex().Z()<<")"<<" with"<<" PdgId="<<truth->pdgid()<<" pT="<<truth->Pt()<<" eta="<<truth->Eta()<<" phi="<<truth->Phi()<<" E="<<truth->E()<<" Ekin_off="<<truth->Ekin_off(), truth->Pt() < transverseMomWarningLimit);
  
  ATH_MSG_DEBUG("[ExtrapolateToID] End extrapolateToID()");

} 

void FastCaloSimCaloExtrapolation::extrapolate(TFCSExtrapolationState& result, const TFCSTruthState* truth) const{ 
  
  ATH_MSG_DEBUG("Start FastCaloSimCaloExtrapolation::extrapolate");
  auto hitVector = caloHits(truth);
  
  ATH_MSG_DEBUG("Done FastCaloSimCaloExtrapolation::extrapolate: caloHits");

  ATH_MSG_DEBUG("FastCaloSimCaloExtrapolation::extrapolate:*** Get calo surface ***");
  getCaloSurface(result, hitVector.get());

  ATH_MSG_DEBUG("FastCaloSimCaloExtrapolation::extrapolate:*** Do extrapolation to ID-calo boundary ***");
  extrapolateToID(result, hitVector.get(), truth);
 
  ATH_MSG_DEBUG("FastCaloSimCaloExtrapolation::extrapolate:*** Do extrapolation ***");
  extrapolateToLayers(result, hitVector.get(), truth);

  ATH_MSG_DEBUG("FastCaloSimCaloExtrapolation::extrapolate: Truth extrapolation done");

  ATH_MSG_DEBUG("Done FastCaloSimCaloExtrapolation::extrapolate");

  //trackParms needs to be manually deleted as ownership is given to FastCaloSimCaloExtrapolation   
  for(std::vector<Trk::HitInfo>::iterator it = hitVector.get()->begin(); it != hitVector.get()->end(); ++it) {    
    auto ptr = (*it).trackParms;    
    if(ptr) delete ptr;   
  }

}


void FastCaloSimCaloExtrapolation::extrapolateToLayers(TFCSExtrapolationState& result, std::vector<Trk::HitInfo>* hitVector, const TFCSTruthState* truth) const
{
  ATH_MSG_DEBUG("[extrapolateToLayers] Start extrapolate");

  //pT threshold of truth particles over which extrapolation failures will be printed as warnings
  const float transverseMomWarningLimit = 500;

  //////////////////////////////////////
  // Start calo extrapolation
  //////////////////////////////////////

  //only continue if inside the calo
  if(std::abs(result.IDCaloBoundary_eta()) < 6){
      //now try to extrapolate to all calo layers that contain energy
      for(int sample=CaloCell_ID_FCS::FirstSample; sample<CaloCell_ID_FCS::MaxSample; ++sample){   
          for(int subpos=SUBPOS_MID; subpos<=SUBPOS_EXT; ++subpos){ 
            
            float cylR, cylZ;
            if(isCaloBarrel(sample)){
              cylR = std::abs(rpos(sample, result.CaloSurface_eta(), subpos));
              //EMB0 - EMB3 use z position of EME1 front end surface for extrapolation
              //else extrapolate to cylinder with symmetrized maximum Z bounds
              //set eta to a dummy value of 1000 and -1000 to force detector side
              if(sample < 4) cylZ = result.CaloSurface_eta() > 0 ? std::abs(zpos(5, 1000, 1)) : std::abs(zpos(5, -1000, 1));
              else cylZ = 0.5*(std::abs(zpos(sample, 1000, subpos)) + std::abs(zpos(sample, -1000, subpos)));
            }
            else{
              //if we are not at barrel surface, extrapolate to cylinder with maximum R to reduce extrapolation length
              cylZ = std::abs(zpos(sample, result.CaloSurface_eta(), subpos));
              //calculate radius of cylinder we will extrapolate to
              double mineta, maxeta, eta;
              minmaxeta(sample, result.CaloSurface_eta(), mineta, maxeta);
              //get eta where we will look up the layer radius
              eta = result.CaloSurface_eta() > 0 ? mineta : maxeta;
              //calculate azimuthal angle from pseudorapidity
              double theta = 2*std::atan(std::exp(-eta));
              //calculate maximum R of last cell of layer from z and theta
              cylR = std::abs(cylZ*std::sqrt((1/(std::cos(theta)*std::cos(theta))) - 1));
            }

            Amg::Vector3D extPos, momDir;
            if(extrapolateToCylinder(hitVector, cylR, cylZ, extPos, momDir)){

              //scale the extrapolation to fit the radius of the cylinder in the case of barrel and scale extrapolation to fit z component in case of endcap layer
              //scale is only non-unitary in case we extrapolate to the endcaps of the cylinder for barrel and in case we extrapolate to cover for endcaps
              //this will keep phi, eta intact and only scale r and z to fit a sensible position on the cylinder
              double scale = isCaloBarrel(sample) ? cylR / extPos.perp(): cylZ / std::abs(extPos.z());
              extPos = scale * extPos; 

              result.set_OK(sample, subpos,  true);
              result.set_phi(sample, subpos, extPos.phi());
              result.set_z  (sample, subpos, extPos.z());
              result.set_eta(sample, subpos, extPos.eta());
              result.set_r  (sample, subpos, extPos.perp());
            }
            else{
              //in case that something goes wrong, use CaloSurface as extrapolation result
              //not expected to happen
              result.set_OK (sample, subpos, false);
              result.set_phi(sample, subpos, result.CaloSurface_phi());
              result.set_eta(sample, subpos, result.CaloSurface_eta());
              result.set_z  (sample, subpos, zpos(sample, result.CaloSurface_eta(), subpos));
              result.set_r  (sample, subpos, rpos(sample, result.CaloSurface_eta(), subpos));
            }   
          } //for pos
        } //for sample
    } //inside calo
  
  else ATH_MSG_COND("[extrapolateToLayers] Ups. Not inside calo. result.IDCaloBoundary_eta()="<<result.IDCaloBoundary_eta()<< "\n[extrapolateToLayers] Particle with truth vertex at (" << truth->vertex().X() <<","<<truth->vertex().Y()<<","<<truth->vertex().Z()<<")"<<" with"<<" PdgId="<<truth->pdgid()<<" pT="<<truth->Pt()<<" eta="<<truth->Eta()<<" phi="<<truth->Phi()<<" E="<<truth->E()<<" Ekin_off="<<truth->Ekin_off(), truth->Pt() < transverseMomWarningLimit);
  

  ATH_MSG_DEBUG("[extrapolateToLayers] End extrapolateToLayers()");
}

bool FastCaloSimCaloExtrapolation::extrapolateToCylinder(std::vector<Trk::HitInfo> * hitVector, float cylR, float cylZ, Amg::Vector3D& extPos, Amg::Vector3D& momDir) const{

  if(hitVector->size() == 1){
    Amg::Vector3D hitPos = hitVector->at(0).trackParms->position();
    ATH_MSG_DEBUG("[extrapolateWithPCA(R="<<cylR<<",Z="<<cylZ<<")] Extrapolating single hit position to surface.");
    extPos = projectOnCylinder(cylR, cylZ, hitPos);
    momDir = hitVector->at(0).trackParms->momentum();
    return true;
  }

  //if we do not find any good intersections, extrapolate to closest point on surface 
  bool foundHit = extrapolateWithIntersection(hitVector, cylR, cylZ, extPos, momDir) ? true : extrapolateWithPCA(hitVector, cylR, cylZ, extPos, momDir);

  if(foundHit){
    ATH_MSG_DEBUG("[extrapolateToCylinder(R="<<cylR<<",Z="<<cylZ<<")::END] Extrapolated to cylinder with R="<<cylR<<" and Z="<<cylZ<<" at ("<< extPos[Amg::x]<<","<<extPos[Amg::y]<<","<<extPos[Amg::z]<<")");
  }
  else{ 
    //this is not expected to ever happen
    ATH_MSG_DEBUG("(R="<<cylR<<", Z="<<cylZ<<"::END) Extrapolation to cylinder surface failed!"); 
  }


  return foundHit;

}


bool FastCaloSimCaloExtrapolation::extrapolateWithIntersection(std::vector<Trk::HitInfo> * hitVector, float cylR, float cylZ, Amg::Vector3D& extPos, Amg::Vector3D& momDir) const{

  ATH_MSG_DEBUG("[extrapolateWithIntersection(R="<<cylR<<",Z="<<cylZ<<")] Checking for cylinder intersections of line segments.");
  
  //counter for number of computed extrapolations, does not count cases of rejected extrapolations due to close by hit positions
  unsigned int nExtrapolations = 0;
  for (size_t hitID = 1; hitID < hitVector->size(); hitID++){   
    //initialize intersection result variables
    //get current and consecutive hit position and build hitLine
    Amg::Vector3D hitPos1 = hitVector->at(hitID-1).trackParms->position();
    Amg::Vector3D hitPos2 = hitVector->at(hitID).trackParms->position();
    Amg::Vector3D hitDir  = hitPos2 - hitPos1; 

    ATH_MSG_DEBUG("[extrapolateWithIntersection(R="<<cylR<<",Z="<<cylZ<<")] Considering line segment between ("<<hitPos1[Amg::x]<<","<<hitPos1[Amg::y]<<","<<hitPos1[Amg::z]<<") and ("
                                                                                                         <<hitPos2[Amg::x]<<","<<hitPos2[Amg::y]<<","<<hitPos2[Amg::z]<<")");
    //get position of the hit positions on the cylinder
    HITPOSITION cylPosHit1 = whereOnCylinder(cylR, cylZ, hitPos1);    
    HITPOSITION cylPosHit2 = whereOnCylinder(cylR, cylZ, hitPos2);    
    
    //check if one of the hit positions already lays on the cylinder surface    
    if(cylPosHit1 == ON || cylPosHit2 == ON){   
      extPos = cylPosHit1 == ON ? hitPos1 : hitPos2;    
      momDir = cylPosHit1 == ON ? hitVector->at(hitID-1).trackParms->momentum() : hitVector->at(hitID).trackParms->momentum();     
      ATH_MSG_DEBUG("[extrapolateWithIntersection(R="<<cylR<<",Z="<<cylZ<<")] Hit position already on cylinder surface.");    
      return true;    
    }

    //do not try to extrapolate with intersections if the hit position are very close together
    if(hitDir.norm() < 0.01) continue;
    
    //get intersections through cylinder
    CylinderIntersections intersections = getCylinderIntersections(cylR, cylZ, hitPos1, hitPos2);
    nExtrapolations++;
    
    Amg::Vector3D selectedIntersection(0, 0, 0); 

    //select the best intersection
    if(intersections.number == 1)      selectedIntersection = intersections.first;
    else if(intersections.number > 1)  selectedIntersection = whichIntersection(cylR, cylZ, hitPos1, hitPos2, intersections.first, intersections.second) == 0 ? 
                                                              intersections.first : intersections.second; 
                                     
    if(intersections.number > 0){
      
      bool isForwardExtrapolation = (selectedIntersection[Amg::x] - hitPos1[Amg::x]) /  (hitPos2[Amg::x] -  hitPos1[Amg::x]) >= 0;
      bool travelThroughSurface = doesTravelThroughSurface(cylR, cylZ, hitPos1, hitPos2);
      
      //do not allow for backward extrapolation except in the case of first two (distinguishable) hit positions outside cylinder 
      //and in the case we detect a travel though the surface
      if(nExtrapolations > 1 && !isForwardExtrapolation && !travelThroughSurface) continue;
      
      //check if the intersection between infinite line and cylinder lays on segment spanned by hit positions
      bool intersectionOnSegment = isOnSegment(selectedIntersection, hitPos1, hitPos2);
      //check if both hit positions lay outside of the cylinder
      bool hitPosOutside = cylPosHit1 == OUTSIDE && cylPosHit2 == OUTSIDE;
      
      //we found our extrapolated hit position in case that either
      //we detect that the line segment crosses the surface of the cylinder
      //the intersection between the infinite lines and the cylinder lays on the line segment
      //both hit positions are outside of the cylinder and there is a backwards extrapolation for the first two hit positions
      //if this is not the case for any of the hit position pairs we will use the last two hit position for the linear extrapolation
      //if these do not have any intersection, then we will pass back to extrapolateWithPCA
      if(travelThroughSurface || intersectionOnSegment || (hitPosOutside && !isForwardExtrapolation && nExtrapolations == 1) || hitVector->size()-1 == hitID){
        //take momentum direction of hit position closest to cylinder surface
        //alternatively one could also take the extrapolated direction normDir = hitPos2 - hitPos1
        double distHitPos1 = (hitPos1 - projectOnCylinder(cylR, cylZ, hitPos1)).norm(); 
        double distHitPos2 = (hitPos2 - projectOnCylinder(cylR, cylZ, hitPos2)).norm(); 
        momDir = distHitPos1 < distHitPos2 ? hitVector->at(hitID-1).trackParms->momentum() : hitVector->at(hitID).trackParms->momentum();
        extPos = selectedIntersection; 
        return true;
      } 
      ATH_MSG_DEBUG("[extrapolateWithIntersection(R="<<cylR<<",Z="<<cylZ<<")] Extrapolated position at ("<<selectedIntersection[Amg::x]<<","<<selectedIntersection[Amg::y]<<","<<selectedIntersection[Amg::z]<<")");
    }
  } //end of loop over hit positions

  return false;
}


bool FastCaloSimCaloExtrapolation::extrapolateWithPCA(std::vector<Trk::HitInfo> * hitVector, float cylR, float cylZ, Amg::Vector3D& extPos, Amg::Vector3D& momDir) const{

  bool foundHit = false;
  ATH_MSG_DEBUG("[extrapolateWithPCA(R="<<cylR<<",Z="<<cylZ<<")] No forward intersections with cylinder surface. Extrapolating to closest point on surface.");

  //here we also need to consider distances from line segments to the cylinder 
  double minDistToSurface = 100000;
  for (size_t hitID = 1; hitID < hitVector->size(); hitID++){   
   
    Amg::Vector3D hitPos1 = hitVector->at(hitID-1).trackParms->position();
    Amg::Vector3D hitPos2 = hitVector->at(hitID).trackParms->position();

    ATH_MSG_DEBUG("[extrapolateWithPCA(R="<<cylR<<",Z="<<cylZ<<")] Considering line segment between ("<<hitPos1[Amg::x]<<","<<hitPos1[Amg::y]<<","<<hitPos1[Amg::z]<<") and ("<<hitPos2[Amg::x]<<","<<hitPos2[Amg::y]<<","<<hitPos2[Amg::z]<<")");

    Amg::Vector3D PCA;
    //find the point of closest approach (PCA) to the cylinder on the line segment 
    findPCA(cylR, cylZ, hitPos1, hitPos2, PCA);
    //compute distance between PCA and cylinder
    Amg::Vector3D cylinderSurfacePCA = projectOnCylinder(cylR, cylZ, PCA); 
    double tmpMinDistToSurface = (PCA - cylinderSurfacePCA).norm();
    
    ATH_MSG_DEBUG("[extrapolateWithPCA(R="<<cylR<<",Z="<<cylZ<<")] Extrapolated line segment to ("<<cylinderSurfacePCA[Amg::x]<<","<<cylinderSurfacePCA[Amg::y]<<","<<cylinderSurfacePCA[Amg::z]<<") with distance "<<tmpMinDistToSurface);
    
    if(tmpMinDistToSurface < minDistToSurface){
      foundHit = true;
      extPos = cylinderSurfacePCA;
      //take momentum direction of hit position closest to cylinder surface
      //alternatively one could also take the extrapolated direction normDir = hitPos2 - hitPos1
      double distHitPos1 = (hitPos1 - projectOnCylinder(cylR, cylZ, hitPos1)).norm(); 
      double distHitPos2 = (hitPos2 - projectOnCylinder(cylR, cylZ, hitPos2)).norm(); 
      momDir = distHitPos1 < distHitPos2 ? hitVector->at(hitID-1).trackParms->momentum() : hitVector->at(hitID).trackParms->momentum();

      minDistToSurface = tmpMinDistToSurface;
    }
  } //end over loop of hit postions

  return foundHit;
} 


void FastCaloSimCaloExtrapolation::findPCA(float cylR, float cylZ, Amg::Vector3D& hitPos1, Amg::Vector3D& hitPos2, Amg::Vector3D& PCA) const{
  //in the following we will try to find the closest point-of-approach (PCA) to the cylinder on the line segment
  //hit direction
  Amg::Vector3D hitDir = hitPos2 - hitPos1;

  //project both hit positions onto the cylinder
  Amg::Vector3D projCylinderHitPos1 = projectOnCylinder(cylR, cylZ, hitPos1);
  Amg::Vector3D projCylinderHitPos2 = projectOnCylinder(cylR, cylZ, hitPos2);
  //direction of line spanned by the two projected points on the cylinder surface
  Amg::Vector3D cylinderProjDir = projCylinderHitPos2 - projCylinderHitPos1;

  //CASE A: projections on the cylinder are close enough, take one of the hit positions as PCA 
  if(cylinderProjDir.norm() < 0.0001) {PCA = hitPos1; return;};   

  //CASE B: we are outside the Z bounds of the cylinder
  if((hitPos1[Amg::z] > cylZ || hitPos1[Amg::z] < -cylZ) || (hitPos2[Amg::z] > cylZ || hitPos2[Amg::z] < -cylZ)){
           
    //calculate PCA to point on endcap
    Amg::Vector3D cylZEndcap(0, 0, cylZ);
    bool isParallelToEndcap = std::abs(hitPos1[Amg::z] - hitPos2[Amg::z]) < 0.00001;

    //Check if parallel to endcap plane
    if(isParallelToEndcap){

      //if both inside there are infinite solutions take one in the middle
      Amg::Vector3D intersectA, intersectB;
      intersectA.setZero();
      intersectB.setZero();
      int nIntersections = circleLineIntersection2D(cylR, hitPos1, hitPos2, intersectA, intersectB);
      
      if(nIntersections == 2){
        
        bool IntAOnSegment = isOnSegment(intersectA, hitPos1, hitPos2);
        bool IntBOnSegment = isOnSegment(intersectB, hitPos1, hitPos2);

        if(IntAOnSegment && IntBOnSegment) PCA = intersectA + 0.5*(intersectB-intersectA);
        else if(IntAOnSegment) PCA = hitPos1.perp() <= cylR ?  intersectA + 0.5*(hitPos1 - intersectA) : intersectA + 0.5*(hitPos2 - intersectA);
        else if(IntBOnSegment) PCA = hitPos1.perp() <= cylR ?  intersectB + 0.5*(hitPos1 - intersectB) : intersectB + 0.5*(hitPos2 - intersectB);
        //intersections are not on line segment, i.e. line segment is within extended cylinder
        else PCA = hitPos1 + 0.5*hitDir;
      
      }
      else if(!intersectA.isZero() || !intersectB.isZero()){
        //this can only happen if the extended line is tangetial to the cylinder
        //if intersection lays on segment PCA will be intersection, if not it will be the corresponding end points
        Amg::Vector3D intersect = intersectA.isZero() ? intersectB : intersectA;
        Amg::Vector3D hitPos = (hitPos1 - intersect).norm() < (hitPos2 - intersect).norm() ? hitPos1 : hitPos2;
        bool IntOnSegment = isOnSegment(intersectA, hitPos1, hitPos2);
        PCA = IntOnSegment ? intersect : hitPos;
      
      }
      else{
        //line segment is outside extended cylinder
        //PCA corresponds to closest distance to center {0, 0, cylZ}
        Amg::Vector3D infLinePCA = hitPos1 + ((cylZEndcap-hitPos1).dot(hitDir)/hitDir.dot(hitDir))*(hitDir);

        if(isOnSegment(infLinePCA, hitPos1, hitPos2)) PCA = infLinePCA;
        else PCA = (hitPos1 - infLinePCA).norm() < (hitPos2 - infLinePCA).norm() ? hitPos1 : hitPos2;    
        
      }
    }

    else{

      //figure out all other cases iteratively beginning with BoundA and BoundB
      Amg::Vector3D BoundA, BoundB;
      //this is point on line closest to {0, 0, cylZ}, always on segment
      double t =  ((cylZEndcap-hitPos1).dot(hitDir)/hitDir.dot(hitDir));
      BoundA = t <= 0 ? hitPos1 : (t >= 1 ? hitPos2 : hitPos1 + t*hitDir);
      
      //calculate intersection point of line segment and endcap plane and project intersection onto cylinder
      //check if t is between 0 and 1, if not, take hitpos as starting bound
      t = (cylZ-hitPos1[Amg::z]) / hitDir[Amg::z];
      BoundB = t <= 0 ? hitPos1 : (t >= 1 ? hitPos2 : hitPos1 + t*hitDir);
      //looks for the PCA iteratively in cases there is no easy analytical solution
      getIterativePCA(cylR, cylZ, BoundA, BoundB, PCA);
      
    }

    return;
  }

  //CASE C: we are inside the Z bounds of the cylinder
  //construct Z axis as straight line surface
  Trk::StraightLineSurface line(new Amg::Transform3D(Trk::s_idTransform), 0, cylZ);
  //compute point of closest approach to z axis
  //this is analogous to finding the PCA of two 3D lines
  Trk::Intersection PCACylBounds = line.straightLineIntersection(hitPos1, hitDir.unit(), false, true);

  double distSurfHit1 = (projCylinderHitPos1 - hitPos1).norm();
  double distSurfHit2 = (projCylinderHitPos2 - hitPos2).norm();
  
  //take PCA on line in case it lays on segment, otherwise take closest hit position to surface
  PCA = isOnSegment(PCACylBounds.position, hitPos1, hitPos2) ? PCACylBounds.position : (distSurfHit1 < distSurfHit2 ? hitPos1 : hitPos2);
  
}


void FastCaloSimCaloExtrapolation::getIterativePCA(float cylR, float cylZ, Amg::Vector3D& BoundA, Amg::Vector3D& BoundB, Amg::Vector3D& PCA) const{

    ATH_MSG_DEBUG("[getIterativePCA] Finding PCA iteratively.");
    
    Amg::Vector3D boundDir = BoundB - BoundA;
    double distBounds = boundDir.norm();

    //if bounds are close enough together, there is nothing to do.
    if (distBounds < 0.001){ PCA = BoundA; return;} 

    //this sets the precision of the iterative finding procedure
    double stepSize = 0.01;

    Amg::Vector3D tmpBoundA, tmpBoundB, tmpOnCylinderBoundA, tmpOnCylinderBoundB;
    Amg::Vector3D resBoundA, resBoundB, resOnCylinderBoundA, resOnCylinderBoundB;
    
    //initial positions on cylinder and distance to line segment
    Amg::Vector3D OnCylinderBoundA = projectOnCylinder(cylR, cylZ, BoundA);
    Amg::Vector3D OnCylinderBoundB = projectOnCylinder(cylR, cylZ, BoundB);

    double minDistA = (BoundA - OnCylinderBoundA).norm();
    double minDistB = (BoundB - OnCylinderBoundB).norm();

    double tmpMinDistA, tmpMinDistB;
    unsigned int nHalfDivisions = (distBounds/stepSize)/2;
    
    for(unsigned int step = 0; step < nHalfDivisions; step++){

      //temporary bounds on line segment
      tmpBoundA = BoundA + (step+1)*stepSize*(boundDir/distBounds);
      tmpBoundB = BoundB - (step+1)*stepSize*(boundDir/distBounds);

      //temporary projected bounds on cylinder
      tmpOnCylinderBoundA = projectOnCylinder(cylR, cylZ, tmpBoundA);
      tmpOnCylinderBoundB = projectOnCylinder(cylR, cylZ, tmpBoundB);
      
      //temporary minimum distance between bound on segment and bound on cylinder
      tmpMinDistA = (tmpBoundA - tmpOnCylinderBoundA).norm();
      tmpMinDistB = (tmpBoundB - tmpOnCylinderBoundB).norm();
      
      if(minDistA >= tmpMinDistA){
        minDistA = tmpMinDistA;
      }
      else{
        double t = (step*stepSize)/distBounds;
        resBoundA = BoundA + t*boundDir;
        resBoundB = tmpBoundA;
        break;
      }

      if(minDistB >= tmpMinDistB){
        minDistB = tmpMinDistB;
      }
      else{
        double t = (step*stepSize)/distBounds;
        resBoundB = BoundB - t*boundDir;
        resBoundA = tmpBoundB;
        break;
      }
    }

    //return middle of best bounds
    PCA = resBoundA + 0.5*(resBoundB - resBoundA);

}


int FastCaloSimCaloExtrapolation::circleLineIntersection2D(float circR, Amg::Vector3D& pointA, Amg::Vector3D& pointB, Amg::Vector3D& intersectA, Amg::Vector3D& intersectB) const{
    //find intersections intA and intB with line spanned by pointA and pointB
    //returns number of intersections
    //assumes circle lays in xy plane

    double dx, dy, A, B, C, det, t;

    dx = pointB[Amg::x] - pointA[Amg::x];
    dy = pointB[Amg::y] - pointA[Amg::y];

    A = dx * dx + dy * dy;
    B = 2 * (dx * pointA[Amg::x] + dy * pointA[Amg::y]);
    C = pointA[Amg::x] * pointA[Amg::x] + pointA[Amg::y] * pointA[Amg::y] - circR * circR;

    det = B * B - 4 * A * C;
    
    if (A <= 0.0000001 || det < 0){   
        ATH_MSG_DEBUG("[circleLineIntersection2D] No intersections.");
        return 0;
    }
    else if (std::abs(det) < 0.00001){
        //one solution, tangential case.
        t = -B / (2 * A);
        intersectA = {pointA[Amg::x] + t * dx, pointA[Amg::y] + t * dy, pointA[Amg::z]};
        ATH_MSG_DEBUG("[circleLineIntersection2D] One intersection at ("<<intersectA[Amg::x]<<","<<intersectA[Amg::y]<<","<<intersectA[Amg::z]<<").");
        return 1;
    }
    else{
        // two solutions
        t = (-B + std::sqrt(det)) / (2 * A);
        intersectA = {pointA[Amg::x] + t * dx, pointA[Amg::y] + t * dy, pointA[Amg::z]};
        t = (-B - std::sqrt(det)) / (2 * A);
        intersectB = {pointA[Amg::x] + t * dx, pointA[Amg::y] + t * dy, pointB[Amg::z]};
        ATH_MSG_DEBUG("[circleLineIntersection2D] Two intersections at ("<<intersectA[Amg::x]<<","<<intersectA[Amg::y]<<","<<intersectA[Amg::z]<<") and at ("<<intersectB[Amg::x]<<","<<intersectB[Amg::y]<<","<<intersectB[Amg::z]<<").");
        return 2;
    }


}


Amg::Vector3D FastCaloSimCaloExtrapolation::projectOnCylinder(float cylR, float cylZ, Amg::Vector3D& hitPos) const {
        
  Amg::Vector3D closestPointOnCylinder;
  Amg::Vector3D cylAxis(0, 0, cylZ);

  //positive side
  if(hitPos[Amg::z] >= cylZ){
    //project hit position on x-y plane at positive side
    Amg::Vector3D projHitPos(hitPos[Amg::x], hitPos[Amg::y], cylZ);
    
    //if r of hit position outside cylinder, closest hit is always on edge  
    if(hitPos.perp() > cylR) closestPointOnCylinder = cylAxis + cylR * (projHitPos - cylAxis).unit();
    else closestPointOnCylinder = cylAxis + hitPos.perp() * (projHitPos - cylAxis).unit();

  } 
  //negative side
  else if (hitPos[Amg::z] <= -cylZ){
    //project hit position on x-y plane at negative side
    Amg::Vector3D projHitPos(hitPos[Amg::x], hitPos[Amg::y], -cylZ);
    
    if(hitPos.perp() > cylR) closestPointOnCylinder = -cylAxis + cylR * (projHitPos + cylAxis).unit();
    else closestPointOnCylinder = -cylAxis + hitPos.perp() * (projHitPos + cylAxis).unit();

  }
  else{
    Amg::Vector3D hitPosZ(0, 0, hitPos[Amg::z]);
    closestPointOnCylinder = hitPosZ + cylR * (hitPos - hitPosZ).unit(); 
  }
  
  return closestPointOnCylinder;

}



CylinderIntersections FastCaloSimCaloExtrapolation::getCylinderIntersections(float cylR, float cylZ, Amg::Vector3D& hitPos1, Amg::Vector3D& hitPos2) const{
  //calculates intersection of infinite line with cylinder --> can have 0 or 2 intersections
  CylinderIntersections intersections;

  //look for intersections with the cover of the cylinder
  unsigned int nCoverIntersections = cylinderLineIntersection(cylR, cylZ, hitPos1, hitPos2, intersections.first, intersections.second);
  if(nCoverIntersections == 2){
    ATH_MSG_DEBUG("[getCylinderIntersections(R="<<cylR<<",Z="<<cylZ<<")] Found two cylinder intersections through cylinder cover.");
    intersections.number = 2;
    return intersections;
  }
  else if (nCoverIntersections == 1){
    
    Amg::Vector3D positiveEndcapIntersection, negativeEndcapIntersection;
    bool IsPositiveEndcapIntersection = cylinderEndcapIntersection(cylR, cylZ, true, hitPos1, hitPos2, positiveEndcapIntersection);
    bool IsNegativeEndcapIntersection = cylinderEndcapIntersection(cylR, cylZ, false, hitPos1, hitPos2, negativeEndcapIntersection);

    if(IsPositiveEndcapIntersection && IsNegativeEndcapIntersection){
      //if we have a cover intersection we only expect one additional endcap intersection
      //both endcap intersections can be valid in case the intersection is at the edge of the cylinder cover and endcap
      //in that case take the endcap intersection which is further away from the cylinder cover intersection to prevent taking the same intersection twice
      ATH_MSG_DEBUG("[getCylinderIntersections(R="<<cylR<<",Z="<<cylZ<<")] Found intersection through cylinder cover and both endcaps. Intersection seems to be at edge of cover and endcap.");
      intersections.second = (positiveEndcapIntersection - intersections.first).norm() > (negativeEndcapIntersection - intersections.first).norm() ? positiveEndcapIntersection : negativeEndcapIntersection;
      intersections.number = 2;
    }
    else if(IsPositiveEndcapIntersection) {
      ATH_MSG_DEBUG("[getCylinderIntersections(R="<<cylR<<",Z="<<cylZ<<")] Found intersection through cylinder cover and positive endcap.");
      intersections.second = positiveEndcapIntersection;
      intersections.number = 2;
    }
    else if(IsNegativeEndcapIntersection) {
      ATH_MSG_DEBUG("[getCylinderIntersections(R="<<cylR<<",Z="<<cylZ<<")] Found intersection through cylinder cover and negative endcap.");
      intersections.second = negativeEndcapIntersection;
      intersections.number = 2;
    }
    else{
      //line is tangential to cylinder cover
      ATH_MSG_DEBUG("[getCylinderIntersections(R="<<cylR<<",Z="<<cylZ<<")] Found single intersection through cylinder cover.");
      intersections.number = 1;
    }

  }
  else{
    //no cylinder cover intersections 
    Amg::Vector3D positiveEndcapIntersection, negativeEndcapIntersection;
    bool IsPositiveEndcapIntersection = cylinderEndcapIntersection(cylR, cylZ, true, hitPos1, hitPos2, positiveEndcapIntersection);
    bool IsNegativeEndcapIntersection = cylinderEndcapIntersection(cylR, cylZ, false, hitPos1, hitPos2, negativeEndcapIntersection);

    if(IsPositiveEndcapIntersection && IsNegativeEndcapIntersection){
      ATH_MSG_DEBUG("[getCylinderIntersections(R="<<cylR<<",Z="<<cylZ<<")] Found intersections through both endcaps.");
      intersections.first = positiveEndcapIntersection;
      intersections.second = negativeEndcapIntersection;
      intersections.number = 2;
    }
    else if(IsPositiveEndcapIntersection) {
      //dont expect this to ever happen
      ATH_MSG_DEBUG("[getCylinderIntersections(R="<<cylR<<",Z="<<cylZ<<")] Found single intersection through positive endcap. This should not happen.");
      intersections.first = positiveEndcapIntersection;
      intersections.number = 1;
    }
    else if(IsNegativeEndcapIntersection) {
      //dont expect this to ever happen
      ATH_MSG_DEBUG("[getCylinderIntersections(R="<<cylR<<",Z="<<cylZ<<")] Found single intersection through negative endcap. This should not happen.");
      intersections.first = negativeEndcapIntersection;
      intersections.number = 1;
    }
    else{
      ATH_MSG_DEBUG("[getCylinderIntersections(R="<<cylR<<",Z="<<cylZ<<")] Found no cylinder intersections.");
      //no intersections at all
      intersections.number = 0;

    }
  }

  return intersections;


}


//calculates the intersection between the line defined by pointA and pointB and the cylinder cover definded by cylR and cylZ
int FastCaloSimCaloExtrapolation::cylinderLineIntersection(float cylR, float cylZ, Amg::Vector3D& pointA, Amg::Vector3D& pointB, Amg::Vector3D& intersectA, Amg::Vector3D& intersectB) const{

  //projections of points spanning the line onto the xy plane
  Amg::Vector3D projPointA(pointA[Amg::x], pointA[Amg::y], 0);
  Amg::Vector3D projPointB(pointB[Amg::x], pointB[Amg::y], 0);
  Amg::Vector3D projDiff = projPointA - projPointB;

  //calculate distance from (0,0,0) to line spanned by projPointA and projPointB
  double t = (projPointA.dot(projDiff))/(projDiff).dot(projDiff);
  double d = std::sqrt(projPointA.dot(projPointA) - t*t*(projDiff).dot(projDiff));

  //if distance larger than cylinder radius then there are no intersection and we are done
  if(d > cylR) return 0;

  double k = std::sqrt((cylR*cylR - d*d)/(projDiff.dot(projDiff)));

  intersectA = pointA + (t+k)*(pointB - pointA);
  intersectB = pointA + (t-k)*(pointB - pointA);

  //check if intersection is outside z bounds
  bool IntAisValid = (intersectA[Amg::z] <= cylZ && intersectA[Amg::z] >= -cylZ);
  bool IntBisValid = (intersectB[Amg::z] <= cylZ && intersectB[Amg::z] >= -cylZ);


  if(IntAisValid && IntBisValid) return 2;
  else if(IntAisValid) return 1;
  else if(IntBisValid){
    intersectA = intersectB;
    return 1;
  }


  return 0;

}


bool FastCaloSimCaloExtrapolation::cylinderEndcapIntersection(float cylR, float cylZ, bool positiveEndcap, Amg::Vector3D& pointA, Amg::Vector3D& pointB, Amg::Vector3D& intersection) const{

  //normal and point on endcap defines the plane
  Amg::Vector3D pointOnEndcap;
  Amg::Vector3D normal(0, 0, 1);
  positiveEndcap ? pointOnEndcap = {0, 0, cylZ} : pointOnEndcap = {0, 0, -cylZ};
  Amg::Vector3D hitDir = (pointB - pointA);

  double denom = normal.dot(hitDir);
  if (std::abs(denom) > 1e-6) { 
    double t = normal.dot(pointOnEndcap - pointB)/denom;
    //compute intersection regardless of direction (t>0 or t<0) 
    intersection = pointB + t*hitDir;
    Amg::Vector3D v = intersection - pointOnEndcap; 

    //check if intersection is within cylR bounds 
    return std::sqrt(v.dot(v)) <= cylR;

  }

  return false;

  }

int FastCaloSimCaloExtrapolation::whichIntersection(float cylR, float cylZ, Amg::Vector3D& hitPos1, Amg::Vector3D& hitPos2, Amg::Vector3D& intersectionA, Amg::Vector3D intersectionB) const{
 
  //check if the hit positions are outside or inside the cylinder surface
  HITPOSITION cylPosHit1 = whereOnCylinder(cylR, cylZ, hitPos1);
  HITPOSITION cylPosHit2 = whereOnCylinder(cylR, cylZ, hitPos2);
  
  if((cylPosHit1 == INSIDE) ^ (cylPosHit2 == INSIDE)){
    /* CASE A: one hit position inside and one outside of the cylinder (travel through surface), 
    one intersection is on cylinder, take intersection closest to line segment */
    ATH_MSG_DEBUG("[whichIntersection] Travel through surface.");
    return getPointLineSegmentDistance(intersectionA, hitPos1, hitPos2) > getPointLineSegmentDistance(intersectionB, hitPos1, hitPos2);
  }
  else if(cylPosHit1 == INSIDE && cylPosHit2 == INSIDE){
    /* CASE B: both hit position inside, take intersection which points towards travel direction of particle */
    Amg::Vector3D directionA = intersectionA - hitPos2;
    Amg::Vector3D directionB = intersectionB - hitPos2;
    Amg::Vector3D hitDir = hitPos2 - hitPos1;
    ATH_MSG_DEBUG("[whichIntersection] Both hit positions inside.");
    return directionA.dot(hitDir) < directionB.dot(hitDir);
  }
  else{
    // /* CASE C: both hit position outside and the intersections lay on the segment, take intersection closest to second hit position */
    // /* CASE D: both hit positions are outside and the intersections are not on the line segment, take intersection closest to one of the hit positions */
    double distHitPosIntersectA = (hitPos2 - intersectionA).norm();
    double distHitPosIntersectB = (hitPos2 - intersectionB).norm();
    ATH_MSG_DEBUG("[whichIntersection] Both hit positions outside.");
    return distHitPosIntersectA > distHitPosIntersectB;
  }
} 

double FastCaloSimCaloExtrapolation::getPointLineSegmentDistance(Amg::Vector3D& point, Amg::Vector3D& hitPos1, Amg::Vector3D& hitPos2) const{

  Amg::Vector3D hitDir = hitPos2 - hitPos1;
  Amg::Vector3D w = point - hitPos1;

  double c1 = w.dot(hitDir);
  if(c1 <= 0) return Amg::distance(point, hitPos1);
  double c2 = hitDir.dot(hitDir);
  if(c2 <= c1) return Amg::distance(point, hitPos2);
  double t = c1/c2;
  Amg::Vector3D vec = hitPos1 + t*hitDir;
  return Amg::distance(point, vec);

}

enum FastCaloSimCaloExtrapolation::HITPOSITION FastCaloSimCaloExtrapolation::whereOnCylinder(float cylR, float cylZ, Amg::Vector3D& hitPos) const{
  //set a 1mm tolerance within which the hit position is considered to be on the cylinder surface
  //setting this higher can lead to extrapolation failures around truth particle eta ~4
  float tolerance = 1;

  bool isOnEndcap = hitPos.perp() <= cylR + tolerance && (hitPos[Amg::z] > 0 ? std::abs(hitPos[Amg::z] - cylZ) < tolerance : std::abs(hitPos[Amg::z] + cylZ) < tolerance);
  bool isOnCover  = std::abs(hitPos.perp() - cylR) < tolerance && hitPos[Amg::z] < cylZ && hitPos[Amg::z] > -cylZ;
  
  //check if hit position is on endcap or cover of cylinder
  if(isOnEndcap || isOnCover) return HITPOSITION::ON;
  
  //check if hit position is inside cover
  if(hitPos[Amg::z] < cylZ && hitPos[Amg::z] > -cylZ && hitPos.perp() < cylR) return HITPOSITION::INSIDE;
  
  return HITPOSITION::OUTSIDE;
}

bool FastCaloSimCaloExtrapolation::doesTravelThroughSurface(float cylR, float cylZ, Amg::Vector3D& hitPos1, Amg::Vector3D& hitPos2) const{
  //travel through surface in case one hit position is outside and the other outside of cylinder surface
  return (whereOnCylinder(cylR, cylZ, hitPos1) == INSIDE) ^ (whereOnCylinder(cylR, cylZ, hitPos2) == INSIDE);
}

bool FastCaloSimCaloExtrapolation::isOnSegment(Amg::Vector3D& point, Amg::Vector3D& hitPos1, Amg::Vector3D& hitPos2) const{
  return getPointLineSegmentDistance(point, hitPos1, hitPos2) < 0.001;
}

bool FastCaloSimCaloExtrapolation::isCaloBarrel(int sample) const
{
  return GetCaloGeometry()->isCaloBarrel(sample);
}

double FastCaloSimCaloExtrapolation::deta(int sample, double eta) const
{
  return GetCaloGeometry()->deta(sample, eta);
}

void FastCaloSimCaloExtrapolation::minmaxeta(int sample, double eta, double& mineta, double& maxeta) const
{
  GetCaloGeometry()->minmaxeta(sample, eta, mineta, maxeta);
}

double FastCaloSimCaloExtrapolation::rmid(int sample, double eta) const
{
  return GetCaloGeometry()->rmid(sample, eta);
}

double FastCaloSimCaloExtrapolation::zmid(int sample, double eta) const
{
  return GetCaloGeometry()->zmid(sample, eta);
}

double FastCaloSimCaloExtrapolation::rzmid(int sample, double eta) const
{
  return GetCaloGeometry()->rzmid(sample, eta);
}

double FastCaloSimCaloExtrapolation::rent(int sample, double eta) const
{
  return GetCaloGeometry()->rent(sample, eta);
}

double FastCaloSimCaloExtrapolation::zent(int sample, double eta) const
{
  return GetCaloGeometry()->zent(sample, eta);
}

double FastCaloSimCaloExtrapolation::rzent(int sample, double eta) const
{
  return GetCaloGeometry()->rzent(sample, eta);
}

double FastCaloSimCaloExtrapolation::rext(int sample, double eta) const
{
  return GetCaloGeometry()->rext(sample, eta);
}

double FastCaloSimCaloExtrapolation::zext(int sample, double eta) const
{
  return GetCaloGeometry()->zext(sample, eta);
}

double FastCaloSimCaloExtrapolation::rzext(int sample, double eta) const
{
  return GetCaloGeometry()->rzext(sample, eta);
}

double FastCaloSimCaloExtrapolation::rpos(int sample, double eta, int subpos) const
{
  return GetCaloGeometry()->rpos(sample, eta, subpos);
}

double FastCaloSimCaloExtrapolation::zpos(int sample, double eta, int subpos) const
{
  return GetCaloGeometry()->zpos(sample, eta, subpos);
}

double FastCaloSimCaloExtrapolation::rzpos(int sample, double eta, int subpos) const
{
  return GetCaloGeometry()->rzpos(sample, eta, subpos);
}