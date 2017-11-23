/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GeantFollowerHelper.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef GeantFollowerHelper_H
#define GeantFollowerHelper_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
//#include "GaudiKernel/ServiceHandle.h"
#include "TrkParameters/TrackParameters.h" //typedef, can't fwd declare
#include "TrkG4UserActions/IGeantFollowerHelper.h"
#include "G4ThreeVector.hh" //typedef, can't fwd declare

#ifndef MAXPROBES
#define MAXPROBES 50000
#endif

class TTree;

namespace Trk {

  class IExtrapolator;
    
  class GeantFollowerHelper : public extends<AthAlgTool, IGeantFollowerHelper>  {

    public:
      // constructor, destructor
      GeantFollowerHelper(const std::string&,const std::string&,const IInterface*);
      virtual ~GeantFollowerHelper ();
    
      // Athena hooks
      virtual StatusCode initialize();
      virtual StatusCode finalize  ();

      // Follower interface
      // a) begin event - initialize follower process
      void beginEvent() const;
      // b) track the particle 
      void trackParticle(const G4ThreeVector& pos, const G4ThreeVector& mom, int pdg, double charge, float t, float X0) const;
      // c) end event - ntuple writing
      void endEvent() const;

   private:
       
        ToolHandle<IExtrapolator>      m_extrapolator;
        bool                           m_extrapolateDirectly;
        bool                           m_extrapolateIncrementally;
                                      
        mutable const TrackParameters* m_parameterCache;
        mutable float                  m_tX0Cache;
       
        // put some validation code is
        std::string                    m_validationTreeName;        //!< validation tree name - to be acessed by this from root
        std::string                    m_validationTreeDescription; //!< validation tree description - second argument in TTree
        std::string                    m_validationTreeFolder;      //!< stream/folder to for the TTree to be written out
                                       
        TTree*                         m_validationTree;            //!< Root Validation Tree        
        /** Ntuple variables : initial parameters*/      
        mutable float                  m_t_x;        
        mutable float                  m_t_y;
        mutable float                  m_t_z; 
        mutable float                  m_t_theta;
        mutable float                  m_t_eta;
        mutable float                  m_t_phi;
        mutable float                  m_t_p;
        mutable float                  m_t_charge;
        mutable int                    m_t_pdg;        
        /** Ntuple variables : g4 step parameters */
        mutable int                    m_g4_steps;
	    mutable float                  m_g4_p[MAXPROBES];
	    mutable float                  m_g4_eta[MAXPROBES];
	    mutable float                  m_g4_theta[MAXPROBES];
	    mutable float                  m_g4_phi[MAXPROBES];
	    mutable float                  m_g4_x[MAXPROBES];
	    mutable float                  m_g4_y[MAXPROBES];
	    mutable float                  m_g4_z[MAXPROBES];
        mutable float                  m_g4_tX0[MAXPROBES];
        mutable float                  m_g4_t[MAXPROBES];
        mutable float                  m_g4_X0[MAXPROBES];
        /** Ntuple variables : trk follow up parameters */
        mutable int                    m_trk_status[MAXPROBES];
	    mutable float                  m_trk_p[MAXPROBES];
	    mutable float                  m_trk_eta[MAXPROBES];
	    mutable float                  m_trk_theta[MAXPROBES];
	    mutable float                  m_trk_phi[MAXPROBES];
	    mutable float                  m_trk_x[MAXPROBES];
	    mutable float                  m_trk_y[MAXPROBES];
	    mutable float                  m_trk_z[MAXPROBES];   
	    mutable float                  m_trk_lx[MAXPROBES];
	    mutable float                  m_trk_ly[MAXPROBES];
	         
        
  };
    
}

#endif