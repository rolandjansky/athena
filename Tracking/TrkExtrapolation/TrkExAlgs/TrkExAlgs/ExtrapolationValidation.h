/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ExtrapolationValidation.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXALGS_EXTRAPOLATIONVALIDATION_H
#define TRKEXALGS_EXTRAPOLATIONVALIDATION_H

// Gaudi includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"
#include <string>


class TTree;

#ifndef TRKEXALGS_MAXPARAMETERS
#define TRKEXALGS_MAXPARAMETERS 10
#endif

namespace Trk 
{

  class IExtrapolator;
  class Surface;
  class TrackingGeometry;
  class TrackingVolume;

  /** @class ExtrapolationValidation

     The ExtrapolationValidation Algorithm runs a number of n test extrapolations
     from randomly distributed Track Parameters to randombly distributed
     reference surfcas within
    
     - a) the Inner Detector if DetFlags.ID_On()
     - b) the Calorimeter if DetFlags.Calo_On()
     - c) the Muon System if DetFlags.Muon_On()

     and backwards.

     It is the TestAlgorithm for 'the' Extrapolator instance provided 
     to client algorithms.

      @author  Andreas Salzburger <Andreas.Salzburger@cern.ch>
  */  

  class ExtrapolationValidation : public AthAlgorithm
    {
    public:

       /** Standard Athena-Algorithm Constructor */
       ExtrapolationValidation(const std::string& name, ISvcLocator* pSvcLocator);
       /** Default Destructor */
       ~ExtrapolationValidation();

       /** standard Athena-Algorithm method */
       StatusCode          initialize();
       /** standard Athena-Algorithm method */
       StatusCode          execute();
       /** standard Athena-Algorithm method */
       StatusCode          finalize();

    private:
      /** private helper method to create a HepTransform */
      static Amg::Transform3D createTransform(double x, double y, double z, double phi=0., double theta=0., double alphaZ=0.);      

      /** the highest volume */
      const TrackingVolume*     m_highestVolume;

      /** The Extrapolator to be retrieved */
      ToolHandle<IExtrapolator> m_extrapolator;

      /** Random Number setup */
      Rndm::Numbers*            m_gaussDist;
      Rndm::Numbers*            m_flatDist;

      bool                      m_materialCollectionValidation; //!< run the material collection

      bool                      m_direct; //extrapolate directly

      TTree*                    m_validationTree;            //!< Root Validation Tree

      std::string               m_validationTreeName;        //!< validation tree name - to be acessed by this from root
      std::string               m_validationTreeDescription; //!< validation tree description - second argument in TTree
      std::string               m_validationTreeFolder;      //!< stream/folder to for the TTree to be written out

      double                    m_maximumR;                  //!< maximum R of the highest
      double                    m_maximumZ;                  //!< maximum halfZ of the highest tracking volume

      double                    m_sigmaLoc;       //!< local sigma of start value
      double                    m_sigmaR;         //!< r sigma of start value
      double                    m_sigmaZ;         //!< Z sigma of start value
      double                    m_minEta;         //!< Minimal eta value
      double                    m_maxEta;         //!< Maximal eta value
      double                    m_minP;           //!< Minimal p value 
      double                    m_maxP;           //!< Maximal p value
 
      int                       m_particleType;   //!< the particle typre for the extrap.

      mutable int               m_parameters;          //!< maximum 3 : start - destination - backward
      mutable float             m_parameterLoc1[TRKEXALGS_MAXPARAMETERS];    //!< start local 1 
      mutable float             m_parameterLoc2[TRKEXALGS_MAXPARAMETERS];    //!< start local 2 
      mutable float             m_parameterPhi[TRKEXALGS_MAXPARAMETERS];     //!< start phi 
      mutable float             m_parameterTheta[TRKEXALGS_MAXPARAMETERS];   //!< start theta
      mutable float             m_parameterEta[TRKEXALGS_MAXPARAMETERS];     //!< start eta
      mutable float             m_parameterQoverP[TRKEXALGS_MAXPARAMETERS];  //!< start qOverP

      mutable float             m_covarianceLoc1[TRKEXALGS_MAXPARAMETERS];    //!< start local 1 
      mutable float             m_covarianceLoc2[TRKEXALGS_MAXPARAMETERS];    //!< start local 2 
      mutable float             m_covariancePhi[TRKEXALGS_MAXPARAMETERS];     //!< start phi 
      mutable float             m_covarianceTheta[TRKEXALGS_MAXPARAMETERS];   //!< start theta
      mutable float             m_covarianceQoverP[TRKEXALGS_MAXPARAMETERS];  //!< start qOverP
      mutable float             m_covarianceDeterminant[TRKEXALGS_MAXPARAMETERS];  //!< start qOverP

      mutable int               m_destinationSurfaceType;  //!< destination surface type 
      mutable float             m_startX;      //!< startX
      mutable float             m_startY;      //!< startX
      mutable float             m_startR;      //!< startX
      mutable float             m_startZ;      //!< startX
      mutable float             m_startP;      //!< startP
 
      mutable float             m_estimationX;      //!< estimation in X
      mutable float             m_estimationY;      //!< estimation in Y
      mutable float             m_estimationR;      //!< estimation in R
      mutable float             m_estimationZ;      //!< estimation in Z
 
      mutable float             m_destinationX;      //!< destination in X
      mutable float             m_destinationY;      //!< destination in Y
      mutable float             m_destinationR;      //!< destination in R
      mutable float             m_destinationZ;      //!< destination in Z
      
      // ---- output statistics
      mutable unsigned int     m_triesFront;    //!< events front
      mutable unsigned int     m_breaksFront;  //!< breaks front
      mutable unsigned int     m_triesBack;    //!< events back
      mutable unsigned int     m_breaksBack;   //!< breaks

      mutable unsigned int     m_collectedLayerFront;    //!< collected material layers forward
      mutable unsigned int     m_collectedLayerBack;    //!< collected material layers backwards

      //AmgSymMatrix(5) * m_covariance; 
      
      
    }; 
} // end of namespace

#endif
