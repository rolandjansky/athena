/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef D2PDMAKER_CREATECONTROLPLOTS_H
#define D2PDMAKER_CREATECONTROLPLOTS_H
//===========================================================================
// Name    : CreateControlPlots.h
/**
   @class CreateControlPlots

   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>

   @date February 2011

   @brief Make some histograms.
*/
//===========================================================================

#include "AthenaBaseComps/AthAlgorithm.h"

// STL includes
#include <string>
#include <vector>

// Gaudi includes
#include "GaudiKernel/ServiceHandle.h"

class ITHistSvc;
class TH1F;

namespace Trk
  {
    class VxCandidate;
    class VxTrackAtVertex;
  }




class CreateControlPlots : public AthAlgorithm {

public:

  /** Default constructor */
  CreateControlPlots(const std::string& name, ISvcLocator* pSvcLocator);

  /* Default destructor */
  ~CreateControlPlots();


  /** Gaudi Service Interface method implementations - initialize */
  StatusCode initialize();

  /** Gaudi Service Interface method implementations - execute */
  StatusCode finalize();

  /** Gaudi Service Interface method implementations - finalize */
  StatusCode execute();

 
private:

  /** a handle on the Hist/TTree registration service */
  //ITHistSvc * m_thistSvc;
  ServiceHandle<ITHistSvc> m_thistSvc;
  
  /** name of the ITHistSvc */
  //std::string m_thistSvcName;

  /** name of the vertex container to retrieve from StoreGate */
  std::string m_vtxCollName;

  /** Name of the output stream to which to save the histograms */
  std::string m_outStreamName;

  /** Name of the output directory inside the root file */
  std::string m_rootDirName;

  /** Prefix for histogram names */
  std::string m_histPreName;


  /** Internal event counter */
  unsigned long m_nEventsProcessed;


  //------------------------------------------------------
  // Selections
  //------------------------------------------------------
  
  /** minimum number of tracks at vertex */
  unsigned int m_minNumberVxTracks;

  /** maximum number of tracks at vertex */
  unsigned int m_maxNumberVxTracks;


  /** Pointer to VertexCandidate */
  const Trk::VxCandidate* m_vxcand;

  /** Pointer to vector of tracks at vertex */
  const std::vector<Trk::VxTrackAtVertex*>* m_trackAtVertex;

  /** VertexType list for accept (logical OR combination) */
  std::vector<int> m_acceptVertexTypeList;

  /** VertexType list for require (logical AND combination) */
  std::vector<int> m_requireVertexTypeList;

  /** VertexType list for veto (logical NOT combination) */
  std::vector<int> m_vetoVertexTypeList;

        
  /** minimum global x-poition of the vertex */
  double m_minVertexX;

  /** maximum global x-poition of the vertex */
  double m_maxVertexX;
  
   /** minimum global y-poition of the vertex */
  double m_minVertexY;

  /** maximum global y-poition of the vertex */
  double m_maxVertexY;
          
  /** minimum global z-poition of the vertex */
  double m_minVertexZ;

  /** maximum global z-poition of the vertex */
  double m_maxVertexZ;



  /** create the histograms */
  TH1F* m_vtx_number;

//   TH1F* m_vtx_mass;
//   TH1F* m_vtx_pt;
//   TH1F* m_vtx_eta;



};

#endif // CREATECONTROLPLOTS_H

