// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGTOOL_JETFITTERVARIABLESFACTORY_C
#define BTAGTOOL_JETFITTERVARIABLESFACTORY_C

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
#include "JetTagTools/IJetFitterVariablesFactory.h"
#include <utility> 


namespace Analysis {


  //static const InterfaceID IID_JetFitterVariablesFactory("Analysis::JetFitterVariablesFactory", 1, 0);


  class JetFitterVariablesFactory : public AthAlgTool , virtual public IJetFitterVariablesFactory  {
    
  public:
    
    
    JetFitterVariablesFactory(const std::string& name,
			      const std::string& n, const IInterface* p);
    virtual ~JetFitterVariablesFactory();
    
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    
    virtual StatusCode fillJetFitterVariables(const xAOD::Jet &, xAOD::BTagging* BTag, const Trk::VxJetFitterVertexInfo* myJetFitterInfo, std::string basename) const;

 
    //  static const InterfaceID& interfaceID() { return IID_JetFitterVariablesFactory; };

   
  private:

    std::string m_secVxFinderName; 
    std::string m_jetFitterInstance;
    bool m_addNegativeTracksToPrimaryVertex;
    bool m_usePtCorrectedEnergy;
    bool m_useSingleTracksAlsoForMass;
    mutable bool m_revertFromPositiveToNegativeTags;
    
    void fill(xAOD::BTagging* BTag, const std::string& basename, float mass_uncorr,
              int nVTX, int nSingleTracks, int nTracksAtVtx, float mass, float energyFraction,
              float significance3d, float deltaeta, float deltaphi, float chi2, int ndof, float deltaRFlightDir) const;

  };
  
}//end Analysis namespace

#endif
