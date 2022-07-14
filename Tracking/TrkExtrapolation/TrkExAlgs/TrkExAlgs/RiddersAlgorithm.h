/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RiddersAlgorithm.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKRIDDERSALGORITHM_H
#define TRKRIDDERSALGORITHM_H

// Gaudi includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include <string>

class AtlasDetectorID;
class Identifier;
class TTree;

#ifndef RIDDLERSSTEPS
#define RIDDLERSSTEPS 10
#endif

namespace Trk 
{

  class IPropagator;
  class MagneticFieldProperties;

  /** @class RiddersAlgorithm

      This is a test algorithm for evaluating the jacobians for
      the transport of Track Parameters

      @author  Andreas Salzburger <Andreas.Salzburger@cern.ch>
  */  

  class RiddersAlgorithm : public AthAlgorithm
    {
    public:

       /** Standard Athena-Algorithm Constructor */
       RiddersAlgorithm(const std::string& name, ISvcLocator* pSvcLocator);
       /** Default Destructor */
       ~RiddersAlgorithm();

       /** standard Athena-Algorithm method */
       StatusCode          initialize();
       /** standard Athena-Algorithm method */
       StatusCode          execute();
       /** standard Athena-Algorithm method */
       StatusCode          finalize();

    private:
      /** private helper method to create a HepTransform */
      static Amg::Transform3D createTransform(double x, double y, double z, double phi=0., double theta=0., double alphaZ=0.);      
     
      /** Langrange-parabolic interpolation */
      static double parabolicInterpolation(double x0, double x1, double x2,
                                    double y0, double y1, double y2);

      /** member variables for algorithm properties: */
      ToolHandle<IPropagator>           m_propagator;       
      bool                              m_useCustomField;
      bool                              m_useAlignedSurfaces;
      double                            m_fieldValue;
      MagneticFieldProperties*          m_magFieldProperties;

      /** The smearing */
      double                    m_sigmaLoc;       //!< local sigma of start value
      double                    m_sigmaR;         //!< r sigma of start value
      double                    m_minPhi;         //!< Minimal phi value
      double                    m_maxPhi;         //!< Maximal phi value
      double                    m_minEta;         //!< Minimal eta value
      double                    m_maxEta;         //!< Maximal eta value
      double                    m_minP;           //!< Minimal p value 
      double                    m_maxP;           //!< Maximal p value

      /** To create the first extimations */
      double                            m_minimumR;
      double                            m_maximumR;

      /** variations */
      std::vector<double>               m_localVariations;
      std::vector<double>               m_angularVariations;
      std::vector<double>               m_qOpVariations;

      TTree*                    m_validationTree;            //!< Root Validation Tree

      std::string               m_validationTreeName;        //!< validation tree name - to be acessed by this from root
      std::string               m_validationTreeDescription; //!< validation tree description - second argument in TTree
      std::string               m_validationTreeFolder;      //!< stream/folder to for the TTree to be written out

      int               m_steps;
      float             m_loc1loc1[RIDDLERSSTEPS];
      float             m_loc1loc2[RIDDLERSSTEPS];
      float             m_loc1phi[RIDDLERSSTEPS];
      float             m_loc1theta[RIDDLERSSTEPS];
      float             m_loc1qop[RIDDLERSSTEPS];
      float             m_loc1steps[RIDDLERSSTEPS];

      float             m_loc2loc1[RIDDLERSSTEPS];
      float             m_loc2loc2[RIDDLERSSTEPS];
      float             m_loc2phi[RIDDLERSSTEPS];
      float             m_loc2theta[RIDDLERSSTEPS];
      float             m_loc2qop[RIDDLERSSTEPS];
      float             m_loc2steps[RIDDLERSSTEPS];

      float             m_philoc1[RIDDLERSSTEPS];
      float             m_philoc2[RIDDLERSSTEPS];
      float             m_phiphi[RIDDLERSSTEPS];
      float             m_phitheta[RIDDLERSSTEPS];
      float             m_phiqop[RIDDLERSSTEPS];
      float             m_phisteps[RIDDLERSSTEPS];

      float             m_thetaloc1[RIDDLERSSTEPS];
      float             m_thetaloc2[RIDDLERSSTEPS];
      float             m_thetaphi[RIDDLERSSTEPS];
      float             m_thetatheta[RIDDLERSSTEPS];
      float             m_thetaqop[RIDDLERSSTEPS];
      float             m_thetasteps[RIDDLERSSTEPS];

      float             m_qoploc1[RIDDLERSSTEPS];
      float             m_qoploc2[RIDDLERSSTEPS];
      float             m_qopphi[RIDDLERSSTEPS];
      float             m_qoptheta[RIDDLERSSTEPS];
      float             m_qopqop[RIDDLERSSTEPS];
      float             m_qopsteps[RIDDLERSSTEPS];


      /** Random Number setup */
      Rndm::Numbers*                    m_gaussDist;
      Rndm::Numbers*                    m_flatDist;

    }; 
} // end of namespace

#endif 
