/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


/**
@class CosmicGenerator




@brief  Cosmic generator.  The output will be stored in the transient event store so it can be passed to the simulation.

@author 
   W. Seligman: Initial Code 08-Nov-2002,
        based on work by M. Shapiro and I. Hinchliffe


 Modification for increasing efficiency of muon hitting the detector:
                     H. Ma.    March 17, 2006 
 Property: ExzCut: 	
	if true, the method exzCut(...) will be called to apply a 
               energy dependent position cut on the surface.
               This rejects low energy muons at large distance. 
   Property: RMax
               Used by exzCut to reject non-projective muons, which are 
               too far out on the surface


 Modifications to accomodate Pixel EndCap C Cosmic Test needs
      Marian Zdrazil   June 7, 2006   mzdrazil@lbl.gov

Modifications to accomodate replacement of Pixel EndCap C by a Pixel EndCap A
      Marian Zdrazil   November 24, 2006  mzdrazil@lbl.gov

It is easier and actually more useful to leave the EndCap A
in the vertical position (the way it is positioned in the ATLAS detector)
 instead of rotating it clockwise by 90deg which corresponds to the
 placement during the Pixel EndCap A cosmic test in SR1 in November 2006.
 This is why we will generate cosmic muons coming from the positive Z-axis 
 direction better than rotating the whole setup in PixelGeoModel.

 Modifications July 3rd 2007, Rob McPherson
     - Fix mu+/mu- bug (always present in Athena versions)
     - Fix sign of Py (since tag CosmicGenerator-00-00-21, muons only upward-going) 

 Optimize selection of events passed to Geant4 for full simulation:
 - cut on energy based on pathlength in rock
 - reweighting of generated cosmic rays
 - geometrical cut in plane of pixel detector
      Juerg Beringer   November 2007      JBeringer@lgl.gov
      Robert Cahn      November 2007      RNCahn@lbl.gov

*/



#ifndef GENERATORMODULESCOSMICGEN_H
#define GENERATORMODULESCOSMICGEN_H

#include "GeneratorModules/GenModule.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include "CLHEP/Vector/LorentzVector.h"
#include "AtlasHepMC/Polarization.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>



class CosmicGun;

class CosmicGenerator:public GenModule {

public:
  CosmicGenerator(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~CosmicGenerator();
  virtual StatusCode genInitialize();
  virtual StatusCode callGenerator();
  virtual StatusCode genFinalize();
  virtual StatusCode fillEvt(HepMC::GenEvent* evt);

  CLHEP::HepLorentzVector generateVertex(void);
  CLHEP::HepLorentzVector generateVertexReweighted(void);

  static IAtRndmGenSvc*         p_AtRndmGenSvc;

private:
  // event counter, used for event ID
  int m_events, m_rejected,m_accepted;
  std::vector<int> m_pdgCode;
  float m_emin, m_emax;
  float m_ctcut;
  float m_xlow, m_xhig, m_zlow, m_zhig, m_yval, m_IPx, m_IPy, m_IPz, m_radius, m_zpos;
  float m_tmin, m_tmax;
  bool m_cavOpt;
  int m_srOneOpt;
  bool m_srOnePixECOpt;
  bool m_swapYZAxis;
  bool m_muonECOpt;
  int m_printEvent, m_printMod;
  int m_selection ; 

  float m_thetamin, m_thetamax, m_phimin, m_phimax;
  
  float m_GeV;
  float m_mm;

  bool m_readfile;
  std::string m_infile;
  std::ifstream    m_ffile;

  // Event scalars, three-vectors, and four-vectors:
  std::vector<CLHEP::HepLorentzVector> m_fourPos;
  std::vector<CLHEP::HepLorentzVector> m_fourMom;
  CLHEP::Hep3Vector m_center;
  std::vector<HepMC::Polarization> m_polarization;

  // Energy dependent position cut for muons to reach the detector.
  bool exzCut(const CLHEP::Hep3Vector& pos,const CLHEP::HepLorentzVector& p); 

  // property for calling exzCut
  bool m_exzCut ; 
  // maximum r used in exzCut 
  float m_rmax ; 

  // Calculation of pathlength in rock and whether cosmic ray is aimed towards
  // the pixel detector
  double pathLengthInRock(double xgen, double ygen, double zgen, double theta, double phi);
  bool pointsAtPixels(double xgen, double ygen, double zgen, double theta, double phi);

  // New optimization options
  bool m_doPathlengthCut;
  bool m_doAimedAtPixelsCut;
  bool m_doReweighting;
  double m_energyCutThreshold;
  double m_ysurface;
  double m_rvertmax;
  double m_pixelplanemaxx;
  double m_pixelplanemaxz;
};

#endif
