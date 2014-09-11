/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*- header for JetFitterVariablesFactory
#ifndef BTAGTOOL_JETFITTERCHARMVARIABLESFACTORY_C
#define BTAGTOOL_JETFITTERCHARMVARIABLESFACTORY_C

/******************************************************
    @class  JetFitterVariableFactory
    Package : JetTagTools 
    Created : March 2007
    DESCRIPTION:
    This class is a "variable factory". It generates, starting from the VxJetCandidate, all 
    the variables actually used by the JetFitterTag.
    @author Giacinto Piacquadio (giacinto.piacquadio AT physik.uni-freiburg.de)
    @author2 Christian Weiser (christian.weiser AT physik.uni-freiburg.de) (University of Freiburg)
********************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "JetTagTools/IJetFitterVariablesFactory.h"
#include <vector>

//class Jet;
// class Hep3Vector; 

namespace Analysis {


  static const InterfaceID IID_JetFitterCharmVariablesFactory
  ("Analysis::JetFitterCharmVariablesFactory", 1, 0);


  class JetFitterGenericTagInfo;
  
  class JetFitterCharmVariablesFactory : 
    public AthAlgTool, 
    public IJetFitterVariablesFactory 
  {
    
  public:
    
    
    JetFitterCharmVariablesFactory(const std::string& name,
				   const std::string& n, const IInterface* p);
    virtual ~JetFitterCharmVariablesFactory();
    
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    
    IJetFitterTagInfo* getITagInfoObject(const xAOD::Jet &) const;
 
    static const InterfaceID& interfaceID() { 
      return IID_JetFitterCharmVariablesFactory; 
    };

   
  private:
    typedef std::pair<const Trk::SelectedTracksInJet*,
		      const Trk::VxJetCandidate*> JFInfo;
    JFInfo get_jf_info(const xAOD::Jet&) const; 

    std::string m_secVxFinderName; 
    std::string m_jetFitterInstance;
    bool m_addNegativeTracksToPrimaryVertex;
    bool m_usePtCorrectedEnergy;
    bool m_usePtCorrectedMass;
    bool m_useSingleTracksAlsoForMass;
    bool m_revertFromPositiveToNegativeTags;
    bool m_buildOldInfoObject; 

    bool is_vertex_positive(double vertex_position) const ; 

    class SecVxDisplacement
    {
    public: 
      SecVxDisplacement(const Trk::VxJetCandidate&, int index); 
      SecVxDisplacement(); 
      bool operator<(const SecVxDisplacement&) const;
      Amg::Vector3D m_vector3;
    }; 

    typedef std::vector<CLHEP::HepLorentzVector> FourMomenta; 
    void add_rap_ptrel(JetFitterGenericTagInfo* info, const Amg::Vector3D& direction,
		       const FourMomenta& tracks) const; 

    typedef std::vector<SecVxDisplacement> SecVertices; 
    void add_vertex_info(JetFitterGenericTagInfo* generic_tag_info, 
			 const SecVertices& secondary_vertices) const; 

  };

  bool has_lower_pt(const CLHEP::HepLorentzVector&, const CLHEP::HepLorentzVector&); 
  
}//end Analysis namespace

#endif
