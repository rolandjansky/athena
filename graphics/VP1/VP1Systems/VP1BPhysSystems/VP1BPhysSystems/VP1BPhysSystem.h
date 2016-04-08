/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1BPhysSystem_H
#define VP1BPhysSystem_H

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Header file for class VP1BPhysSystem                               //
//                                                                     //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>              //
//                                                                     //
//  Initial version: June 2007                                         //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include "VP1Base/IVP13DSystemSimple.h"
#include "VP1TrackSystems/TrackPropagationHelper.h"
//#include "TrkParametersBase/ParametersBase.h"

#include "GeoPrimitives/GeoPrimitives.h"


class SoSwitch;
class SoLineSet;

namespace Trk {
  class Track;
//   class IExtrapolator;
}

namespace Rec {
  class TrackParticle;
  class TrackParticleContainer;
}

class VP1ToolAccessHelper;
// class TrackPropagationHelper;
class TFile;
class TTree;


class Br {
    
  public:
    
    bool init(TTree* tree);
    int GetEntry(int i);
    
    TTree* vp1Filter;

    int evtNum; //event number
    int runNum; //run number

  //vertices
    double              vtx_x; //vtx position
    double              vtx_y;
    double              vtx_z;
    double              vtx_xx; //covariance matrix
    double              vtx_yy;
    double              vtx_zz;
    double              vtx_xy;
    double              vtx_xz;
    double              vtx_yz;
    int                 vtx_mother; //index of mother vertex
    unsigned long       vtx_color;  //color of the vertex (for drawing)
    std::vector<int>*   vtx_daughters;  //daughter vertices

  //tracks
    std::vector<double>* track_pt ;  //track parameters
    std::vector<double>* track_eta;
    std::vector<double>* track_phi;
    std::vector<double>* track_d0 ;
    std::vector<double>* track_z0 ;
    std::vector<double>* track_charge;
    std::vector<double>* track_refitted_px;  //refitted momentum
    std::vector<double>* track_refitted_py;
    std::vector<double>* track_refitted_pz;
    std::vector<unsigned long>*    track_color;   //track color
    std::vector<unsigned long>*    track_refitted_color;   //refitted momentum color
  
  //neutral tracks
    std::vector<double>* neutral_refitted_px;  //neutral track momentum
    std::vector<double>* neutral_refitted_py;
    std::vector<double>* neutral_refitted_pz;
    std::vector<double>* neutral_length;  //length of the track
    std::vector<int>*    neutral_decay;  //decay vertex
    std::vector<unsigned long>*    neutral_color;   //track color


};
//________________________________________________________________

class VP1BPhysSystem : public IVP13DSystemSimple {

  Q_OBJECT

public:
  VP1BPhysSystem();
  virtual ~VP1BPhysSystem(){};
	void buildEventSceneGraph(StoreGateSvc* sg, SoSeparator *root);

  QWidget * buildController();

private:
  
  //drawing methods
	void actualBuild();

	void filterTrack(SoSeparator *root, const Rec::TrackParticleContainer* partCont,
                   double pt, double eta, double phi, double d0, double z0,
                   double x, double y, double z, unsigned long color, std::vector<const Rec::TrackParticle*>* selectedParticles);
  
  void drawAllTrackParticles(StoreGateSvc* sg, SoSeparator *root, std::vector<const Rec::TrackParticle*>* selectedParticles);

  void drawTrackParticle(SoSwitch* trackSwitch, const Rec::TrackParticle* part, unsigned long color);
  
  void drawCutoffTrackParticle(SoSeparator *root, const Rec::TrackParticle* part, double x, double y, double z, unsigned long color);
  
  void drawNeutralTrack(SoSeparator* root, double x, double y, double z, double px, double py, double pz, double length, unsigned long color);

  void drawRefittedTrack(SoSeparator* root, double x, double y, double z, double px, double py, double pz, double charge, unsigned long color);

  void drawVertex(SoSeparator *root, double x, double y, double z, double radius,
                  double xx, double xy, double xz, double yy, double yz, double zz, unsigned long color,
                  std::vector<SoSwitch*>& vertexSwitches);

  void drawPoints(SoSwitch* trackSwitch, std::vector<Amg::Vector3D >* points, unsigned long color, double width, bool isNeutral);

  std::vector<Amg::Vector3D >* getPoints(const Trk::Track* track);
  std::vector<Amg::Vector3D >* getPoints(const Rec::TrackParticle* part);

  std::vector<Amg::Vector3D >* findClosestApproach(std::vector<Amg::Vector3D >* points, double x, double y, double z);
      
  //utils
  const Trk::Track* getTrack(const Rec::TrackParticle* trackparticle);
  const Trk::Track* getRefittedTrack(Amg::Vector3D pos, Amg::Vector3D mom, double charge);
  
  void getColor(unsigned long icolor, double& r, double& g, double& b);
  SoLineSet* createCross(double extent );

  //member attributes
        
  TFile* m_rootFile;
  TTree* m_tree;
  QString m_fileName;

	StoreGateSvc* m_sg;
	SoSeparator* m_root;
	
  int m_vertexStyle;
  bool m_showVertices;
  std::vector<SoSwitch*> m_vertexSwitches;

  bool m_showAll;
  bool m_showSignal;
  bool m_showNeutral;
  bool m_showRefitted;

  std::vector<SoSwitch*> m_trackSwitches;
  std::vector<SoSwitch*> m_overlapSwitches;
  std::vector<SoSwitch*> m_signalSwitches;
  std::vector<SoSwitch*> m_neutralSwitches;
  std::vector<SoSwitch*> m_refittedSwitches;

  Br* m_br;

	class Clockwork;
	Clockwork* c;

private slots:

  void loadFile();
  void displayVerticesChanged ( int state );
  void sphereToggled ( bool checked );
  void crossToggled ( bool checked );
  void ellipsoidToggled ( bool checked );

  void displayAllTracksChanged ( int state );
  void displayOrigSignalChanged ( int state );
  void displayRefTracksChanged ( int state );
  void displayNeutralChanged ( int state );

      
};

#endif
