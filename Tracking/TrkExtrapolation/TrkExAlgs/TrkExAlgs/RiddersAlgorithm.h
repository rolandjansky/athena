/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
      Amg::Transform3D* createTransform(double x, double y, double z, double phi=0., double theta=0., double alphaZ=0.);      
     
      /** Langrange-parabolic interpolation */
      double parabolicInterpolation(double x0, double x1, double x2,
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

      mutable int               m_steps;                           
      mutable float             m_loc1loc1[RIDDLERSSTEPS];         
      mutable float             m_loc1loc2[RIDDLERSSTEPS];         
      mutable float             m_loc1phi[RIDDLERSSTEPS];          
      mutable float             m_loc1theta[RIDDLERSSTEPS];        
      mutable float             m_loc1qop[RIDDLERSSTEPS];          
      mutable float             m_loc1steps[RIDDLERSSTEPS];        

      mutable float             m_loc2loc1[RIDDLERSSTEPS];         
      mutable float             m_loc2loc2[RIDDLERSSTEPS];         
      mutable float             m_loc2phi[RIDDLERSSTEPS];          
      mutable float             m_loc2theta[RIDDLERSSTEPS];        
      mutable float             m_loc2qop[RIDDLERSSTEPS];          
      mutable float             m_loc2steps[RIDDLERSSTEPS];        

      mutable float             m_philoc1[RIDDLERSSTEPS];          
      mutable float             m_philoc2[RIDDLERSSTEPS];          
      mutable float             m_phiphi[RIDDLERSSTEPS];           
      mutable float             m_phitheta[RIDDLERSSTEPS];         
      mutable float             m_phiqop[RIDDLERSSTEPS];           
      mutable float             m_phisteps[RIDDLERSSTEPS];         

      mutable float             m_thetaloc1[RIDDLERSSTEPS];        
      mutable float             m_thetaloc2[RIDDLERSSTEPS];        
      mutable float             m_thetaphi[RIDDLERSSTEPS];         
      mutable float             m_thetatheta[RIDDLERSSTEPS];       
      mutable float             m_thetaqop[RIDDLERSSTEPS];         
      mutable float             m_thetasteps[RIDDLERSSTEPS];       

      mutable float             m_qoploc1[RIDDLERSSTEPS];          
      mutable float             m_qoploc2[RIDDLERSSTEPS];          
      mutable float             m_qopphi[RIDDLERSSTEPS];           
      mutable float             m_qoptheta[RIDDLERSSTEPS];         
      mutable float             m_qopqop[RIDDLERSSTEPS];           
      mutable float             m_qopsteps[RIDDLERSSTEPS];         


      /** Random Number setup */
      Rndm::Numbers*                    m_gaussDist;
      Rndm::Numbers*                    m_flatDist;

    }; 
} // end of namespace

#endif 
