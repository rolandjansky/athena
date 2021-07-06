// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetTileCorrectionTool.h 2015-03-10 tripiana $
#ifndef JETTILECORRECTION_JETTILECORRECTIONTOOL_H
#define JETTILECORRECTION_JETTILECORRECTIONTOOL_H

// Framework include(s):
#include "AsgTools/AsgMetadataTool.h"

// Tool dependencies

// Local include(s):
#include "JetCPInterfaces/IJetTileCorrectionTool.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetConstituentVector.h"

#include "TFile.h"
#include "TSystem.h"
#include "TProfile.h"
#include <iostream>
#include <stdlib.h>
#include <set>
#include <vector>
#include <boost/unordered_map.hpp>

//--- config ---
#define PIXWIDTH  0.1
#define RINGWIDTH 0.1
#define RJET      0.4
#define NPIX      16
#define OUT 99
#define Pix_eta 17
#define Pix_phi 6
//--------------



namespace CP {
  
  class JetTileCorrectionTool : public virtual IJetTileCorrectionTool,
                                public asg::AsgMetadataTool
  {
    /// Create a proper constructor for Athena
    ASG_TOOL_CLASS3( JetTileCorrectionTool, CP::IJetTileCorrectionTool, CP::ISystematicsTool, CP::IReentrantSystematicsTool )
    
    public:
    /// Create a constructor for standalone usage
    JetTileCorrectionTool( const std::string& name );

    /// Destructor: 
    virtual ~JetTileCorrectionTool(); 
    
    /// Function initialising the tool
    virtual StatusCode initialize();

    /// Apply the correction on a modifyable object
    virtual CorrectionCode applyCorrection( xAOD::Jet& object );

    /// Create a corrected copy from a constant jet
    virtual CorrectionCode correctedCopy( const xAOD::Jet& input,  xAOD::Jet*& output );


    /// returns: whether this tool is affected by the given systematic
    virtual bool
    isAffectedBySystematic( const SystematicVariation& systematic ) const;

    /// returns: the list of all systematics this tool can be affected by
    virtual SystematicSet
    affectingSystematics() const;

    /// returns: the list of all systematics this tool recommends to use
    virtual SystematicSet
    recommendedSystematics() const;

    /// returns: the currently applied systematics
    const CP::SystematicSet& appliedSystematics() const {
      return *m_appliedSystematics;
    }

    virtual StatusCode applySystematicVariation
    ( const SystematicSet& systConfig );

    StatusCode registerSystematics();


    //status helpers
    JTC::TS getTileStatus(const xAOD::Jet& jet);
    StatusCode addTileStatus(const xAOD::Jet& jet);

    //DR helper
    void setRJET(float r); //Set Jet Radius. NOTE: Only for checking overlap! The correction is only derived for EMTopo R=0.4 jets at the moment.

  protected:
    // Arguments and tool dependencies
    bool m_isData;
    
  private:

    // Load dead regions DB
    void loadDeadUser(); //user-defined
    void loadDeadDB();   //DB-defined

    std::vector<float> getCorrections(const xAOD::Jet& j); //, bool constScale);

    JTC::TS overlap(const xAOD::Jet& j, JTC::Hole region);

    int getPtBin(float pt); //in MeV
    
    bool inIOV(JTC::Hole region, int run);

    bool inHole(const xAOD::Jet& j, JTC::Hole rdead);
    bool inHole(float eta, float phi, JTC::Hole rdead);
      
    IPair getModulePosition(const xAOD::Jet& jet, JTC::Hole module);

    void loadModulesFromMap(const xAOD::Jet& jet, JTC::TS& status, std::map<std::string,JTC::Hole> hmap, JTC::PART part=JTC::PART::LB, JTC::TYPE type=JTC::TYPE::DB);
    StatusCode loadAllModules(const xAOD::Jet& jet, JTC::TS& status);

    JTC::Hole partModToHole(int part, int mod); //translate to eta-phi coordinates (+dummy IOV)

    bool m_constScale = false; //Get correction using constituent pt (for testing purposes)
    bool m_isMC = false;       //check if it is simulation

    int m_current_run = -1;  //keep current run number
    //int m_current_lb;   //keep current lumiblock

    int  m_NbinsPt = -1;   //number of pt-bins in parametrization histograms

    std::vector<std::string> m_v_user_dead; //book dead regions (apart from those coming from detector db)   
    
    TFile* m_rootFile = 0;               //correction file
    std::string m_rootFileName;          //correction filename

    std::string m_bd_dead_mapFile;       //masked region DB filename    

    std::vector<JTC::Region> m_position_masked; //!    //needs to be transient!

    //***
    TH1F* m_core_sys_LB = 0;
    TH1F* m_core_sys_EB = 0;
    //***

    std::map<int,TH1F*> m_pars_LB;
    std::map<int,TH1F*> m_pars_EB;
    std::map<std::string,JTC::Hole> m_db_dead_LB;
    std::map<std::string,JTC::Hole> m_db_dead_EB;
    std::map<std::string,JTC::Hole> m_user_dead_LB;
    std::map<std::string,JTC::Hole> m_user_dead_EB;

    float m_RJET;

    /// Systematics filter map
    boost::unordered_map<CP::SystematicSet, CP::SystematicSet> m_systFilter;
    
    /// Currently applied systematics
    CP::SystematicSet* m_appliedSystematics = 0;

  }; // class JetTileCorrectionTool
  
} // namespace CP

#endif // JETTILECORRECTION_JETTILECORRECTIONTOOL_H
