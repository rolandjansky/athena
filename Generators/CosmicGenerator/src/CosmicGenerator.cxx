/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
// File: CosmicGenerator/CosmicGenerator.cxx
// Description:

//    The output will be stored in the transient event store so it can be
//    passed to the simulation.
//
// AuthorList:
//         W. Seligman: Initial Code 08-Nov-2002,
//         based on work by M. Shapiro and I. Hinchliffe
//

// Modification for increasing efficiency of muon hitting the detector:
//                     H. Ma.    March 17, 2006
//   Property: ExzCut:
//        if true, the method exzCut(...) will be called to apply a
//               energy dependent position cut on the surface.
//               This rejects low energy muons at large distance.
//   Property: RMax
//               Used by exzCut to reject non-projective muons, which are
//               too far out on the surface


// Modifications to accomodate Pixel EndCap C Cosmic Test needs
//      Marian Zdrazil   June 7, 2006   mzdrazil@lbl.gov
//
// Modifications to accomodate replacement of Pixel EndCap C by a Pixel EndCap A
//      Marian Zdrazil   November 24, 2006  mzdrazil@lbl.gov
//
// Description:
// ------------
// It is easier and actually more useful to leave the EndCap A
// in the vertical position (the way it is positioned in the ATLAS detector)
// instead of rotating it clockwise by 90deg which corresponds to the
// placement during the Pixel EndCap A cosmic test in SR1 in November 2006.
// This is why we will generate cosmic muons coming from the positive Z-axis
// direction better than rotating the whole setup in PixelGeoModel.

// Modifications July 3rd 2007, Rob McPherson
//     - Fix mu+/mu- bug (always present in Athena versions)
//     - Fix sign of Py (since tag CosmicGenerator-00-00-21, muons only upward-going)

// Optimize selection of events passed to Geant4 for full simulation:
// - cut on energy based on pathlength in rock
// - reweighting of generated cosmic rays
// - geometrical cut in plane of pixel detector
//
//      Juerg Beringer   November 2007      JBeringer@lgl.gov
//      Robert Cahn      November 2007      RNCahn@lbl.gov


#include "CosmicGenerator/CosmicGenerator.h"
#include "CosmicGenerator/CosmicGun.h"
#include "CosmicGenerator/CosmicEventParser.h"

#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Geometry/Normal3D.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Random/RandFlat.h"

#include <limits>
#include <cmath>
#include <vector>
#include <string>
#include <fstream>


// Pointer On AtRndmGenSvc
IAtRndmGenSvc*         CosmicGenerator::p_AtRndmGenSvc = 0;
extern "C" float cosmicrndm_(int* /*dummy*/)
{
  CLHEP::HepRandomEngine* engine = CosmicGenerator::p_AtRndmGenSvc->GetEngine("COSMICS");
  return CLHEP::RandFlat::shoot(engine);
}

//--------------------------------------------------------------------------
CosmicGenerator::CosmicGenerator(const std::string& name,
      ISvcLocator* pSvcLocator)
  : GenModule(name,pSvcLocator)
  , m_stopped_tminus(0.)
  , m_stopped_tplus(0.)
//--------------------------------------------------------------------------
{
  //
  // Migration to MeV and mm units: all conversions are done in this interface
  // to the CosmicGun. The CosmicGun itself uses GeV units internally - to call
  // the fortran code.
  //

  m_GeV = 1000;
  m_mm  = 10;
  m_readfile = false;
  m_activeStore = 0;

  m_events = 0;
  m_rejected = 0;
  m_accepted = 0;
  m_selection = 0;

  declareProperty("eventfile",  m_infile = "NONE" );
  declareProperty("emin",       m_emin =10.*m_GeV );
  declareProperty("emax",       m_emax =100*m_GeV );
  declareProperty("xvert_low",  m_xlow =0. *m_mm);
  declareProperty("xvert_hig",  m_xhig =10.*m_mm );
  declareProperty("zvert_low",  m_zlow =0. *m_mm);
  declareProperty("zvert_hig",  m_zhig =10.*m_mm );
  declareProperty("yvert_val",  m_yval = 81*m_mm );
  declareProperty("tmin",       m_tmin =0. );
  declareProperty("tmax",       m_tmax =0. );

  declareProperty("IPx",  m_IPx =0. );
  declareProperty("IPy",  m_IPy =0. );
  declareProperty("IPz",  m_IPz =0. );
  declareProperty("Radius",  m_radius =0. );
  declareProperty("ExzCut",  m_exzCut = false );
  declareProperty("OptimizeForCavern",  m_cavOpt = false );
  declareProperty("OptimizeForSR1", m_srOneOpt = 0);
  declareProperty("OptimizeForSR1PixelEndCap", m_srOnePixECOpt = false);
  declareProperty("SwapYZAxis", m_swapYZAxis = false);
  declareProperty("OptimizeForMuonEndCap", m_muonECOpt = false);
  declareProperty("ctcut",      m_ctcut =0.35 );
  declareProperty("PrintEvent", m_printEvent=10);
  declareProperty("PrintMod",   m_printMod=100);
  declareProperty("RMax",       m_rmax = 10000000. );
  declareProperty("ThetaMin", m_thetamin = 0.);
  declareProperty("ThetaMax", m_thetamax = 1.);
  declareProperty("PhiMin", m_phimin = -1*M_PI);
  declareProperty("PhiMax", m_phimax = M_PI);
  declareProperty("Zposition", m_zpos = 14500);

  // Job options for new optimzation options (November 2007)
  declareProperty("doPathLengthCut",m_doPathlengthCut = false);
  declareProperty("doAimedAtPixelsCut",m_doAimedAtPixelsCut = false);
  declareProperty("doReweighting",m_doReweighting = false);
  declareProperty("energyCutThreshold",m_energyCutThreshold = 1.0);
  declareProperty("ysurface",m_ysurface = 81*m_mm);
  declareProperty("rvert_max",m_rvertmax = 300*m_mm);   // replaces rectangle in case of reweighting
  declareProperty("pixelplane_maxx",m_pixelplanemaxx = 1150);
  declareProperty("pixelplane_maxz",m_pixelplanemaxz = 1650);

}

//--------------------------------------------------------------------------
 CosmicGenerator::~CosmicGenerator()
//--------------------------------------------------------------------------
{}

//---------------------------------------------------------------------------
StatusCode CosmicGenerator::genInitialize() {
//---------------------------------------------------------------------------

  // Initialize event count.

  m_events = 0;

  m_accepted=0;
  m_rejected=0;

  if(m_infile=="NONE")

    {
      // Get the random number service
      CosmicGenerator::p_AtRndmGenSvc = &(GenModule::atRndmGenSvc());
      CosmicGun* gun = CosmicGun::GetCosmicGun();

      gun->SetEnergyRange(m_emin/m_GeV,m_emax/m_GeV);
      gun->SetCosCut(m_ctcut);
      gun->PrintLevel(m_printEvent, m_printMod);
      float flux_withCT = gun->InitializeGenerator();

      ATH_MSG_INFO( "Initialisation cosmic gun done." );
      ATH_MSG_INFO( "Accepted diff  flux after E and cos(theta) cuts = " << flux_withCT << " /cm^2/s" );
      if (! m_doReweighting) {
        // The following is only correct w/o reweighting
        ATH_MSG_INFO( "Accepted total flux after E and cos(theta) cuts = " <<
          flux_withCT*(m_xhig-m_xlow)/m_mm*(m_zhig-m_zlow)/m_mm << " /s" );
      }

    }
  else
    {
      ATH_MSG_INFO( "Cosmics are read from file " << m_infile );
      m_ffile.open(m_infile.c_str());
      if(!m_ffile)
        {
          ATH_MSG_FATAL( "Could not open input file - stop! " );
          return StatusCode::FAILURE;
        }
      m_readfile = true;
    }

  m_center=CLHEP::Hep3Vector(m_IPx, m_IPy, m_IPz);

  return StatusCode::SUCCESS;

}

CLHEP::HepLorentzVector CosmicGenerator::generateVertex(void) {

  // Get the pointer to the engine of the stream named SINGLE. If the
  // stream does not exist is created automaticaly
  CLHEP::HepRandomEngine* engine = CosmicGenerator::p_AtRndmGenSvc->GetEngine("COSMICS");

  // Generate a random number according to the distribution.

  float x_val = CLHEP::RandFlat::shoot(engine, m_xlow, m_xhig);
  float z_val = CLHEP::RandFlat::shoot(engine, m_zlow, m_zhig);

  // Generate a random number for time offset
  float t_val = m_tmin;  // Assign defined value
  if(m_tmin < m_tmax){
    t_val = CLHEP::RandFlat::shoot(engine, m_tmin, m_tmax);
  }
  else if(m_tmin == m_tmax){
    t_val = m_tmin;
  }
  else ATH_MSG_FATAL("You specified m_tmin = " << m_tmin << " and m_tmax " << m_tmax);
  CLHEP::HepLorentzVector p(x_val,m_yval,z_val, t_val*CLHEP::c_light);

  return p;

}

CLHEP::HepLorentzVector CosmicGenerator::generateVertexReweighted(void) {

  // Get the pointer to the engine of the stream named SINGLE. If the
  // stream does not exist is created automaticaly
  CLHEP::HepRandomEngine* engine = CosmicGenerator::p_AtRndmGenSvc->GetEngine("COSMICS");

  // Generate non-uniform distribution of vertices to reflect azimuthal
  // angle subtended by the sphere of radiusm m_radius
  // Inside m_radius, the density of vertices is proportional to 2 pi r dr
  // Outside m_radius, the density is proportional to 2r arcsin (m_radius/r)
  // We approximate the latter by its maximum: m_radius * pi
  // We generate vertices out to m_rvertmax.
  // Integrating the approximated distribution gives
  // pi r**2 for r < m_radius and pi m_radius r for r> m_radius
  // So with ran in (0,1) we take r=max_r * ran for ran>m_radius/max_r
  // and r= sqrt(m_radius*max_r*ran) for ran<m_radius/max_r
  // for r>m_radius we use acceptance/rejection by comparing
  // m_radius * pi * new_ran with 2r arcsin (m_radius/r)
  int accept=0;
  float max_r = m_rvertmax;
  float r_val = 0.;
  while(accept==0){
    float ran_one = CLHEP::RandFlat::shoot(engine,0.,1.);
    if(ran_one>(m_radius/max_r)){
      r_val = ran_one*max_r;
      float ran_two = CLHEP::RandFlat::shoot(engine,0.,1.);
      if(m_radius*M_PI*ran_two<2*r_val*asin(m_radius/r_val)){
        accept=1;
      }
    }
    else
      {
        r_val = sqrt(m_radius*max_r*ran_one);
        accept=1;
      }
  }
  float ran_three= CLHEP::RandFlat::shoot(engine, 0.,2*M_PI);
  float x_val = r_val*cos(ran_three);
  float z_val = r_val*sin(ran_three);

  // Generate a random number for time offset
  float t_val = m_tmin;  // Assign defined value
  if(m_tmin < m_tmax){
    t_val = CLHEP::RandFlat::shoot(engine, m_tmin, m_tmax);
  }
  else if(m_tmin == m_tmax){
    t_val = m_tmin;
  }
  else ATH_MSG_FATAL( " You specified m_tmin = " << m_tmin << " and m_tmax " << m_tmax );
  CLHEP::HepLorentzVector p(x_val,m_yval,z_val, t_val*CLHEP::c_light);

  return p;
}


//---------------------------------------------------------------------------
StatusCode CosmicGenerator::callGenerator() {
//---------------------------------------------------------------------------

  ++m_events;
  ATH_MSG_DEBUG( "Event #" << m_events);

  CLHEP::HepRandomEngine* engine = CosmicGenerator::p_AtRndmGenSvc->GetEngine("COSMICS");

  // clear up the vectors
  m_fourPos.clear();
  m_fourMom.clear();
  m_polarization.clear();
  m_pdgCode.clear();


  if(m_readfile)
    {
      if(!m_ffile.eof())
        {
          CosmicEventParser evt;
          m_ffile >> evt;

          ATH_MSG_VERBOSE( evt );

          double polx = 0;
          double poly = 0;
          double polz = 0;
          HepMC::Polarization thePolarization;
          thePolarization.set_normal3d(HepGeom::Normal3D<double>(polx,poly,polz));
          m_polarization.push_back(thePolarization);

          //
          // units are already converted to MeV's and mm.
          //
          m_fourPos.push_back(evt.Vertex());
          m_fourMom.push_back(evt.Momentum());
          m_pdgCode.push_back(evt.pdgID());

        }
      else
        {
          ATH_MSG_FATAL( "End of file reached - stop " );
          exit(1);
          return StatusCode::FAILURE;
        }
    }
  else
    {

      bool accepted=false;
      CLHEP::HepLorentzVector pp;
      CosmicGun* gun = CosmicGun::GetCosmicGun();
      CLHEP::HepLorentzVector vert;
      CLHEP::Hep3Vector vert3;
      double theta1;
      double phi1;
      double mag1;

      while(!accepted){

        if (m_doReweighting && m_cavOpt) {
          // The code here doesn't make sense without the sphere cut in the
          // cavern optimization that is selected by OptimizeForCavern=True
          vert = generateVertexReweighted();
          vert3 = CLHEP::Hep3Vector(vert.x(),vert.y(),vert.z());

          double vert_radius=sqrt(vert3.x()*vert3.x() + vert3.z()*vert3.z());

          pp = gun->GenerateEvent();

          theta1=pp.theta();
          phi1=pp.phi();
          mag1=pp.rho();

          if (vert_radius>m_radius) {
            phi1=atan2(vert.z(),vert.x())+M_PI;
            float delta_phi=asin(m_radius/vert_radius);
            phi1=phi1+CLHEP::RandFlat::shoot(engine, -delta_phi, delta_phi);
          }
          pp.setX(mag1*sin(theta1)*cos(phi1));
          pp.setY(mag1*sin(theta1)*sin(phi1));

        } else {
          vert = generateVertex();
          vert3 = CLHEP::Hep3Vector(vert.x(),vert.y(),vert.z());

          pp = gun->GenerateEvent();

          theta1=pp.theta();
          phi1=pp.phi();
          mag1=pp.rho();
        }

        CLHEP::Hep3Vector pp_corr(mag1*sin(theta1)*cos(phi1),
                           -mag1*cos(theta1),
                           mag1*sin(theta1)*sin(phi1));
        CLHEP::Hep3Vector direction(pp_corr.x(),pp_corr.y(), pp_corr.z());

        // if optimization activated, check for the direction of the generated muon
        if(m_cavOpt) {

          CLHEP::Hep3Vector center_dir=m_center-vert3;
          double beta=direction.angle(center_dir);
          double alpha=asin(m_radius/center_dir.r());

          if(fabs(beta)<alpha) {

            if(m_exzCut) {
              // Old optimization code - is it still useful?
              CLHEP::HepLorentzVector pp2(pp_corr.x(),pp_corr.y(), pp_corr.z(), pp.e());
              if( exzCut(vert3,pp2) ) {
                accepted=true;
              }
            } else {

              accepted = true;

              ATH_MSG_DEBUG( "x0 = " << vert3.x()
                  << ", y0 = " << vert3.y()
                  << ", z0 = " << vert3.z()
                  << ", theta = " << pp.theta()
                  << ", phi = " << pp.phi()
                  << ", energy = " << pp.e()*m_GeV );

              if (m_doPathlengthCut) {
                double path = pathLengthInRock(vert3.x(),vert3.y(),vert3.z(),pp.theta(),pp.phi());
                double energyLoss =  2.33e-3 * 244. * path; //FIXME Hardcoded values!
                ATH_MSG_DEBUG( "Energy loss is " << energyLoss
                    << " --> " << (energyLoss>pp.e()*m_GeV ? "REJECTED" : "ACCEPTED") << " by pathlength cut");
                if (energyLoss-m_energyCutThreshold > pp.e()*m_GeV) accepted = false;
              }

              if (m_doAimedAtPixelsCut) {
                bool aimedAtPixels = pointsAtPixels(vert3.x(),vert3.y(),vert3.z(),pp.theta(),pp.phi());
                ATH_MSG_DEBUG( (aimedAtPixels ? "AIMED AT PIXELS" : "NOT AIMED AT PIXELS") );
                if (!aimedAtPixels) accepted = false;
              }

              // FOR DEBUGGING ONLY
              if (accepted) {
                ATH_MSG_VERBOSE("The following event has been accepted for simulation:");
                ATH_MSG_VERBOSE( "x0 = " << vert3.x() << ", y0 = " << vert3.y() << ", z0 = " << vert3.z()
                              << ", theta = " << pp.theta() << ", phi = " << pp.phi() << ", energy = " << pp.e()*m_GeV );

                if (m_doPathlengthCut) {
                  double path = pathLengthInRock(vert3.x(),vert3.y(),vert3.z(),pp.theta(),pp.phi());
                  double energyLoss =  2.33e-3 * 244. * path;
                  ATH_MSG_VERBOSE( "Energy loss is " << energyLoss
                      << " --> " << (energyLoss>pp.e()*m_GeV ? "REJECTED" : "ACCEPTED") << " by pathlength cut" );
                }
              
                if (m_doAimedAtPixelsCut) {
                  bool aimedAtPixels = pointsAtPixels(vert3.x(),vert3.y(),vert3.z(),pp.theta(),pp.phi());
                  ATH_MSG_VERBOSE( (aimedAtPixels ? "AIMED AT PIXELS" : "NOT AIMED AT PIXELS") );
                }

              }

            }
          }

          if(accepted) {
            m_accepted++;
          } else {
            ATH_MSG_VERBOSE("Rejected muon due to cavern optimization request!");
            m_rejected++;
          }
        }
        else if(m_srOneOpt == 1) {
          CLHEP::Hep3Vector srOneVec(direction.x(), direction.y(), direction.z());
          if(mag1 < 0) // Check if momentum vector is flipped.
            srOneVec *= -1;

          if( (srOneVec.phi() >= -2.25) && (srOneVec.phi() <= -1.7) &&
              (srOneVec.theta() >= 0.85) && (srOneVec.theta() <= 2.25) ) { //FIXME Hardcoded values!
            accepted = true;
            m_accepted++;
            ATH_MSG_DEBUG("Muon accepted by SR1 SCT/TRT optimization!");
          } else {
            ATH_MSG_DEBUG("Rejected muon due to SR1 SCT/TRT optimization request!");
            m_rejected++;
          }
        }
        else if(m_srOneOpt == 2) {
          CLHEP::Hep3Vector srOneVec(direction.x(), direction.y(), direction.z());
          if(mag1 < 0) // Check if momentum vector is flipped.
            srOneVec *= -1;

          if( (srOneVec.phi() >= -1.68) && (srOneVec.phi() <= -1.08) &&
              (srOneVec.theta() >= 0.29) && (srOneVec.theta() <= 0.72) ) { //FIXME Hardcoded values!
            accepted = true;
            m_accepted++;
            ATH_MSG_DEBUG("Muon accepted by SR1 SCT/TRT EndCapC optimization!");
          } else {
            ATH_MSG_DEBUG("Rejected muon due to SR1 SCT/TRT EndcapC optimization request!");
            m_rejected++;
          }
        }

        else if(m_srOnePixECOpt) {
          CLHEP::Hep3Vector srOneVec(direction.x(), direction.y(), direction.z());
          if(mag1 < 0) // Check if momentum vector is flipped.
            srOneVec *= -1;

          if( (srOneVec.phi() >= m_phimin) && (srOneVec.phi() <= m_phimax) &&
              (srOneVec.theta() >= m_thetamin) && (srOneVec.theta() <= m_thetamax) ) {
            accepted = true;
            m_accepted++;
            ATH_MSG_DEBUG("Muon accepted by SR1 Pixel EndCap optimization!");
          } else {
            ATH_MSG_DEBUG("Rejected muon due to SR1 Pixel EndCap optimization request!");
            m_rejected++;
          }
        }

        else if (m_muonECOpt) {
          double coor_x, coor_y, coor_z;
          coor_z = m_zpos; // defined in jobOpt.
          coor_x = direction.x()*(coor_z - vert.z())/direction.z() +vert.x();
          coor_y = direction.y()*(coor_z - vert.z())/direction.z() +vert.y();
          if( ((coor_x)*(coor_x) + (coor_y)*(coor_y)) <= m_radius*m_radius ) {
            accepted = true;
            m_accepted++;
          } else {
            coor_z = -m_zpos;
            coor_x = direction.x()*(coor_z - vert.z())/direction.z() +vert.x();
            coor_y = direction.y()*(coor_z - vert.z())/direction.z() +vert.y();
            if( ((coor_x)*(coor_x) + (coor_y)*(coor_y)) <= m_radius*m_radius ) {
              accepted = true;
              m_accepted++;
            } else {
              ATH_MSG_DEBUG("Rejected muon due to Muon EndCap optimization request!");
              m_rejected++;
            }
          }
        }

        else accepted=true; // if no opt required accept the first muon
      }

      pp.setX(pp.x()*m_GeV);
      pp.setY(pp.y()*m_GeV);
      pp.setZ(pp.z()*m_GeV);
      pp.setT(pp.t()*m_GeV);

      // Get the mass of the particle to be generated
      int charge = gun->GetMuonCharge();
      // m_pdgCode.push_back(charge*13);
      m_pdgCode.push_back(charge*-13);

      const HepPDT::ParticleData* particle = particleData(abs(m_pdgCode.back()));
      if (particle==nullptr){
        ATH_MSG_FATAL( "Particle with PDG ID=" << abs(m_pdgCode.back()) << " returned a nullptr" );
        return StatusCode::FAILURE;
      }

      double mass = particle->mass().value();

      // Compute the kinematic values.  First, the vertex 4-vector:
      double x = vert.x();
      double y = vert.y();
      double z = vert.z();
      double t = vert.t();

      // Do we need to swap Y- and Z-axis for the PixelEndCap A Cosmic test ?
      // if not...do nothing...if so, invert position of y- and z- coordinate
      //
      // but not only that...change also the direction of the incoming cosmic muon(s),
      // they must go towards the pixel endcap A, i.e. y -> -y
      //
      if(!m_swapYZAxis)
        m_fourPos.push_back(CLHEP::HepLorentzVector(x,y,z,t));
      else
        m_fourPos.push_back(CLHEP::HepLorentzVector(x,z,y,t));

      // Set the polarization.  Realistically, this is going to be zero
      // for most studies, but you never know...
      double polx = 0;
      double poly = 0;
      double polz = 0;
      //m_polarization.set_normal3d(HepGeom::Normal3D<double>(polx,poly,polz));
      HepMC::Polarization thePolarization;

      // Do we need to swap Y- and Z-axis for the PixelEndCap C Cosmic test ?
      // if not...do nothing...if so, invert position of y- and z- coordinate
      //
      // well and don't forget about the direction of the incoming cosmic muon(s) either
      // that means:  y -> -y
      //
      if(!m_swapYZAxis){
        // thePolarization.set_normal3d(HepGeom::Normal3D<double>(polx,-poly,polz));
        thePolarization.set_normal3d(HepGeom::Normal3D<double>(polx,poly,polz));
      }
      else
        thePolarization.set_normal3d(HepGeom::Normal3D<double>(polx,polz,-poly));

      m_polarization.push_back(thePolarization);


      // The method of calculating e, theta, and phi depends on the user's
      // commands.  Let the KinematicManager handle it.
      double e     = pp.e();
      double theta = pp.theta();
      double phi   = pp.phi();

      // At this point, we have e, theta, and phi.  Put them together to
      // get the four-momentum.

      double p2 = e*e - mass*mass;
      if ( p2 < 0 )
        {
          ATH_MSG_ERROR( "Event #" << m_events
              << " E=" << e << ", mass=" << mass
              << " -- you have generated a tachyon! Increase energy or change particle ID." );
          return StatusCode::FAILURE;
        }

      double p = sqrt(p2);
      double px = p*sin(theta)*cos(phi);
      double pz = p*sin(theta)*sin(phi);
      double py = -p*cos(theta);

      // Do we need to swap Y- and Z-axis for the PixelEndCap C Cosmic test ?
      // if not...do nothing...if so, invert position of y- and z- coordinate
      //
      // well and don't forget about the direction of the incoming cosmic muon(s) either
      // that means:  y -> -y
      //
      if(!m_swapYZAxis) {
        // Line below corrupted py sign and forces muons to be upwards, not downwards.
        // m_fourMom.push_back(CLHEP::HepLorentzVector(px,-py,pz,pp.e()));
        m_fourMom.push_back(CLHEP::HepLorentzVector(px,py,pz,pp.e()));
      }
      else
        m_fourMom.push_back(CLHEP::HepLorentzVector(px,pz,-py,pp.e()));

      ATH_MSG_DEBUG(
             "  (x,y,z,t) = ("
          << m_fourPos.back().x() << ","
          << m_fourPos.back().y() << ","
          << m_fourPos.back().z() << ","
          << m_fourPos.back().t() << "), (Px,Py,Pz,E) = ("
          << m_fourMom.back().px() << ","
          << m_fourMom.back().py() << ","
          << m_fourMom.back().pz() << ","
          << m_fourMom.back().e()  << ")" );
      ATH_MSG_DEBUG(
             "  (theta,phi) = (" << theta << "," << phi << "), "
          << "polarization(x,y,z) = ("
          << m_polarization.back().normal3d().x() << ","
          << m_polarization.back().normal3d().y() << ","
          << m_polarization.back().normal3d().z() << ")" );
    }
  return StatusCode::SUCCESS;

}


//---------------------------------------------------------------------------
StatusCode CosmicGenerator::genFinalize() {
//---------------------------------------------------------------------------
  // Get the KinematicManager.

  if(m_cavOpt){

    ATH_MSG_INFO("********************************************");
    ATH_MSG_INFO("** you have run CosmicGenerator with some ");
    ATH_MSG_INFO("** optimizations for cavern simulation");
    ATH_MSG_INFO("** "<<m_accepted<<" muons were accepted");
    ATH_MSG_INFO("** "<<m_rejected<<" muons were rejected");
    ATH_MSG_INFO("********************************************");

  }
  if(m_srOneOpt == 1){

    ATH_MSG_INFO("**********************************************");
    ATH_MSG_INFO("** you have run CosmicGenerator with some ");
    ATH_MSG_INFO("** optimizations for SR1 SCT/TRT simulation");
    ATH_MSG_INFO("** "<<m_accepted<<" muons were accepted");
    ATH_MSG_INFO("** "<<m_rejected<<" muons were rejected");
    ATH_MSG_INFO("**********************************************");

  }
  if(m_srOneOpt == 2){

    ATH_MSG_INFO("**********************************************");
    ATH_MSG_INFO("** you have run CosmicGenerator with some ");
    ATH_MSG_INFO("** optimizations for SR1 SCT/TRT EndcapC simulation");
    ATH_MSG_INFO("** "<<m_accepted<<" muons were accepted");
    ATH_MSG_INFO("** "<<m_rejected<<" muons were rejected");
    ATH_MSG_INFO("**********************************************");

  }

  if(m_srOnePixECOpt){

    ATH_MSG_INFO("***************************************************");
    ATH_MSG_INFO("** you have run CosmicGenerator with some ");
    ATH_MSG_INFO("** optimizations for SR1 Pixel EndCap simulation");
    ATH_MSG_INFO("** "<<m_accepted<<" muons were accepted");
    ATH_MSG_INFO("** "<<m_rejected<<" muons were rejected");
    ATH_MSG_INFO("***************************************************");

    if(m_swapYZAxis){
      ATH_MSG_INFO("***************************************************");
      ATH_MSG_INFO(" You have swapped Y- and Z-axis, i.e. muons are    ");
      ATH_MSG_INFO(" not coming from the top any more !!!              ");
      ATH_MSG_INFO("***************************************************");
    }

  }

  if(m_muonECOpt) {

    ATH_MSG_INFO("***************************************************");
    ATH_MSG_INFO("** you have run CosmicGenerator with some "        );
    ATH_MSG_INFO("** filters for cosmic muon simulation"             );
    ATH_MSG_INFO("** "<<m_accepted<<" muons were accepted"           );
    ATH_MSG_INFO("** "<<m_rejected<<" muons were rejected"           );
    ATH_MSG_INFO("***************************************************");
  }

  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------
StatusCode CosmicGenerator::fillEvt(HepMC::GenEvent* event) {
//---------------------------------------------------------------------------


  // loop over generated vertices
  if(m_fourMom.size()==m_fourPos.size()&&m_fourMom.size()==m_polarization.size()){

    for(std::size_t v=0;v<m_fourMom.size();++v){

      // Note: The vertex and particle are owned by the event, so the
      // event is responsible for those pointers.

      // Create the particle, and specify its polarization.

      HepMC::GenParticle* particle = new HepMC::GenParticle( m_fourMom[v], m_pdgCode[v], 1);
      particle->set_polarization( m_polarization[v] );

      // Create the vertex, and add the particle to the vertex.
      HepMC::GenVertex* vertex = new HepMC::GenVertex(m_fourPos[v]);
      vertex->add_particle_out( particle );

      // Add the vertex to the event.
      event->add_vertex( vertex );

    }

    event->set_event_number(m_events); // Set the event number
    if (event->weights().empty()){
      event->weights().push_back(1.0);
    }
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_ERROR("Wrong different number of vertexes/momenta/polaritazions!");
    return StatusCode::FAILURE;
  }

}


// Energy dependent position cut on the surface.
bool CosmicGenerator::exzCut(const CLHEP::Hep3Vector& pos,const CLHEP::HepLorentzVector& p)
{
// p is in GeV...

        double r =0;
        bool cut = false;
        if(pos.z()<0){
          r = sqrt((pow(pos.x(),2)+pow(pos.z()+28000,2))) ; //FIXME Hardcoded values!
          double e = 0.45238*r+5000 ;  //FIXME Hardcoded values!
          cut = p.e()*m_GeV>e;
        }
        else
        {
          r = sqrt((pow(pos.x(),2)+pow(pos.z()-20000,2))) ; //FIXME Hardcoded values!
          if(r<15000) { //FIXME Hardcoded values!
            cut = true;
          } else
          {
          double e = 0.461538*(r-15000)+10000 ;  //FIXME Hardcoded values!
          cut = p.e()*m_GeV>e;
          ATH_MSG_VERBOSE("z>0 r , e, p.e = "<<r <<" " <<e <<" " <<p.e()*m_GeV);
          }
        }

        cut = cut && r < m_rmax ;

        return cut;
}


// Estimate pathlength in rock towards the pixel detector, taking into account both
// the PX14 and PX16 shafts. The shaft positions are currently hard-coded.
double CosmicGenerator::pathLengthInRock(double xgen, double ygen, double zgen, double theta, double phi) {
  // y is vertical direction, z along beam, major shaft has z>0

  // Definition of shafts and cavern
  double p14_x = 1700; //FIXME Hardcoded values!
  double p14_z = 13500; //FIXME Hardcoded values!
  double p14_radius = 9000.; //FIXME Hardcoded values!
  double p16_x = 1700; //FIXME Hardcoded values!
  double p16_z = -20000; //FIXME Hardcoded values!
  double p16_radius = 6300.; //FIXME Hardcoded values!
  double y1 = 26400; //       ! mm cavern height above IP //FIXME Hardcoded values!

  // direction of trajectory
  // x=x0 - t sinth cosphi; y=y0 + t costh; z=z0 - t sinth sinphi
  double cosphi = cos(phi);
  double sinphi = sin(phi);
  double costh = cos(theta);
  double sinth = sin(theta);

  double y0 = m_ysurface;
  double t = (ygen-y0)/costh;
  double x0 = xgen + t*sinth*cosphi;   // x position at y=0
  double z0 = zgen + t*sinth*sinphi;   // z position at y=0

  // full path length ignoring shaft
  double full_distance = (y0-y1)/costh;

  // does trajectory intersect p14 cylinder?
  double z_mid14 = (x0-p14_x)*sinphi-(z0-p14_z)*cosphi;
  double min_dist14 = fabs(z_mid14);  //minimum distance of line from center
  double shaft_distance14 = 0.;
  if (min_dist14<p14_radius) {

    // z values at intersections
    double z_plus14  = -cosphi*z_mid14+sinphi*sqrt(pow(p14_radius,2.)-pow(z_mid14,2.)) + p14_z;
    double z_minus14 = -cosphi*z_mid14-sinphi*sqrt(pow(p14_radius,2.)-pow(z_mid14,2.)) + p14_z;

    // y values at intersections
    double y_plus14  = y0-costh*(z_plus14-z0)/sinth/sinphi;
    double y_minus14 = y0-costh*(z_minus14-z0)/sinth/sinphi;
    double y_great14 = y_plus14>y_minus14 ? y_plus14 : y_minus14;
    double y_less14  = y_plus14>y_minus14 ? y_minus14 : y_plus14;

    // top intersection must occur above bottom of shaft
    if ( (y_great14>y1) && (y_less14<y0) ) {
      double y_top14    = y_great14<y0 ? y_great14 : y0;
      double y_bottom14 = y_less14>y1  ? y_less14 : y1;
      shaft_distance14 = (y_top14-y_bottom14)/costh;
    }
  }

  // does trajectory intersect p16 cylinder?
  double z_mid16 = (x0-p16_x)*sinphi-(z0-p16_z)*cosphi;
  double min_dist16 = fabs(z_mid16);
  double shaft_distance16 = 0.;
  if (min_dist16<p16_radius) {

    // z values at intersections
    double z_plus16  = -cosphi*z_mid16+sinphi*sqrt(pow(p16_radius,2.)-pow(z_mid16,2.)) + p16_z;
    double z_minus16 = -cosphi*z_mid16-sinphi*sqrt(pow(p16_radius,2.)-pow(z_mid16,2.)) + p16_z;

    // determine y values at intersections
    double y_plus16  = y0-costh*(z_plus16-z0)/sinth/sinphi;
    double y_minus16 = y0-costh*(z_minus16-z0)/sinth/sinphi;
    double y_great16 = y_plus16>y_minus16 ? y_plus16 : y_minus16;
    double y_less16  = y_plus16>y_minus16 ? y_minus16 : y_plus16;

    // top intersection must occur above bottom of shaft
    if ( (y_great16>y1) && (y_less16<y0) ) {
      double y_top16    = y_great16<y0 ? y_great16 : y0;
      double y_bottom16 = y_less16>y1  ? y_less16 : y1;
      shaft_distance16 = (y_top16-y_bottom16)/costh;
    }
  }

  double rock_distance = full_distance - shaft_distance14-shaft_distance16;
  return rock_distance;
}


// Check if trajectory points towards a horizontal rectangle centered at the pixel detector
bool CosmicGenerator::pointsAtPixels(double xgen, double ygen, double zgen, double theta, double phi) {
  // y is vertical direction, z along beam, major shaft has z<0
  bool does = false;

  // direction of trajectory
  // x=xgen+ t sinth cosphi; y=ygen+t costh; z=zgen+t sinth sinphi
  double cosphi = cos(phi);
  double sinphi = sin(phi);
  double costh = cos(theta);
  double sinth = sin(theta);
  double t = ygen/costh;  //for parameterized trajectory
  double x_pos = xgen + t*sinth*cosphi; //x position at y=0
  double z_pos = zgen + t*sinth*sinphi; //z position at y=0

  ATH_MSG_VERBOSE("x_pos = " << x_pos << ", z_pos = " << z_pos);

  if((fabs(x_pos)<m_pixelplanemaxx)&&(fabs(z_pos)<m_pixelplanemaxz)){
    does=true;
  }

  return does;
}
