/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FastCaloSimCaloExtrapolation_H
#define FastCaloSimCaloExtrapolation_H

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace Trk {
  class TrackingVolume;
}
#include "TrkExInterfaces/ITimedExtrapolator.h" 

class ICaloSurfaceBuilder;
class ICaloSurfaceHelper;
class ICaloCoordinateTool;

namespace HepPDT {
  class ParticleDataTable;
}  

#include "ISF_FastCaloSimParametrization/IFastCaloSimCaloExtrapolation.h"
#include "ISF_FastCaloSimEvent/FastCaloSim_CaloCell_ID.h"
#include "ISF_FastCaloSimEvent/TFCSExtrapolationState.h"

class IFastCaloSimGeometryHelper;


class FastCaloSimCaloExtrapolation:public AthAlgTool, public IFastCaloSimCaloExtrapolation {
 public:
   FastCaloSimCaloExtrapolation( const std::string& t, const std::string& n, const IInterface* p );
   ~FastCaloSimCaloExtrapolation();

   virtual StatusCode initialize();
   virtual StatusCode finalize();

   IFastCaloSimGeometryHelper* GetCaloGeometry() const {return &(*m_CaloGeometryHelper);};

 protected:
   bool   isCaloBarrel(int sample) const;
   double deta(int sample,double eta) const;
   void   minmaxeta(int sample,double eta,double& mineta,double& maxeta) const;
   double rzmid(int sample,double eta) const;
   double rzent(int sample,double eta) const;
   double rzext(int sample,double eta) const;
   double rmid(int sample,double eta) const;
   double rent(int sample,double eta) const;
   double rext(int sample,double eta) const;
   double zmid(int sample,double eta) const;
   double zent(int sample,double eta) const;
   double zext(int sample,double eta) const;
   double rpos(int sample,double eta,int subpos = CaloSubPos::SUBPOS_MID) const;
   double zpos(int sample,double eta,int subpos = CaloSubPos::SUBPOS_MID) const;
   double rzpos(int sample,double eta,int subpos = CaloSubPos::SUBPOS_MID) const;
   
   HepPDT::ParticleDataTable*     m_particleDataTable;
   
   double m_CaloBoundaryR;
   double m_CaloBoundaryZ;
   double m_calomargin;

   /** The new Extrapolator setup */
   ToolHandle<Trk::ITimedExtrapolator> m_extrapolator;          
   ToolHandle<ICaloSurfaceHelper>      m_caloSurfaceHelper;
   mutable const Trk::TrackingVolume*  m_caloEntrance;
   std::string                         m_caloEntranceName; 

   /** The FastCaloSimGeometryHelper tool */
   ToolHandle<IFastCaloSimGeometryHelper> m_CaloGeometryHelper;
};

#endif // FastCaloSimCaloExtrapolation_H


