/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// DESCRIPTION:
// Subject: 	This class is derivated from MSPhysics Object
//		The main idea is, that a MSTrackObject can have several
//		assoziated MSPhysicsObjects.
//		This makes it easy to study reconstruction efficiencies and
//		Tag&Probe approach since we can assoziate inner Tracks to
//		outer tracks
///////////////////////////////////////////////////////////////////////////

#ifndef Muon_MuonDQATrackObject_H
#define Muon_MuonDQATrackObject_H

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <math.h>
#include "MuonDQAUtils/MSPhysicsObject.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/MsgStream.h"

//______________________________________________________________________________________________
//
//	Global Variables
//______________________________________________________________________________________________


#define MUON	13
#define JET 	1
#define ZBOSON 	26
#define PI	3.14159265


using namespace std;
static const InterfaceID IID_MuonDQATrackObject("Muon::MuonDQATrackObject",1,0);

namespace Muon 
{
  class MuonDQATrackObject: public MSPhysicsObject, public AlgTool {
  public:
    
    /** @brief constructor */
    MuonDQATrackObject(const std::string&,const std::string&,const IInterface*);

    /** @brief destructor */
    virtual ~MuonDQATrackObject();
   
    /** @brief AlgTool initilize */
    StatusCode initialize();

    /** @brief access to tool interface */
    static const InterfaceID& interfaceID() { return IID_MuonDQATrackObject; }
    
    /** @brief AlgTool finalize */
    StatusCode finalize();

    // Basic Access Funktions
    void clear();
    
    // Particle Assoziation Funktions
    void  setAssoziatedTruthInformation(ToolHandle<Muon::MuonDQATrackObject> &ob); // Setting an MSPhysicsObject which should be assoziated as Truth
    bool  getAssoziatedTruthInformation(ToolHandle<Muon::MuonDQATrackObject> &ob); // Getting an MSPhysicsObject if there is a Truth-assoziated - otherwise FALSE
    
    void  setAssoziatedTrackInformation(string ContainerID, ToolHandle<Muon::MuonDQATrackObject> &ob);
    bool  getAssoziatedTrackInformation(string ContainerID, ToolHandle<Muon::MuonDQATrackObject> &ob);
    
    void  setAssoziatedTriggerInformation(string ContainerID, ToolHandle<Muon::MuonDQATrackObject> &ob);
    bool  getAssoziatedTriggerInformation(string ContainerID, ToolHandle<Muon::MuonDQATrackObject> &ob);
    
    bool  match(ToolHandle<Muon::MuonDQATrackObject> &ecTest, double max_Distance, double &distance);
    double distance(ToolHandle<Muon::MuonDQATrackObject> &ecTest);

    // Public Member Variables
  public:
    int	Type;			//	1 : Combined
    //	2 : Inner Track
    //	3 : Outer Track
    //	4 : Low PT
    //	0 : Truth
    //	-1: Something else
    int 	Hits;
    double	Chi2;
    double	Chi2oDOF;
    double	EnergyLoss;
   
    // Private Member Variables
  private:
    map< string,ToolHandle<Muon::MuonDQATrackObject> >	m_TruthAssoziation;
    map< string,ToolHandle<Muon::MuonDQATrackObject> >	m_TrackAssoziation;
    map< string,ToolHandle<Muon::MuonDQATrackObject> >	m_TriggerAssoziation;
    MsgStream*      m_log;                  //!< Gaudi message stream - msgstream as private member (-> speed)

  };
  
}

#endif
  
  
