/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrkMaterialProviderTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TrkMaterialProviderTool_H
#define TrkMaterialProviderTool_H

#define protected public
#include "TrkTrack/Track.h"

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkSurfaces/Surface.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/TrackStateOnSurface.h"

#include "TrkToolInterfaces/ITrkMaterialProviderTool.h"
#include "TrkDetDescrInterfaces/ITrackingVolumesSvc.h"
#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"
#include "TrkExInterfaces/IMultipleScatteringUpdator.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/MagneticFieldProperties.h"

namespace MagField {
  class IMagFieldSvc;
}

namespace Trk {
  class IExtrapolator; 
  class IEnergyLossUpdator;
  class Surface;
}

namespace Rec {
  class CaloMeas;
  class IMuidCaloEnergyMeas;
  class IMuidCaloEnergyParam;
  class IMuidTrackIsolation;
  class IMuonCaloEnergyTool;
}

class AtlasDetectorID;

  /** @class TrkMaterialProviderTool
   */
namespace Trk{  
  
  class TrkMaterialProviderTool : public AthAlgTool,
    virtual public Trk::ITrkMaterialProviderTool {
      
  public:
      
    /** AlgTool like constructor */
    TrkMaterialProviderTool(const std::string&,const std::string&,const IInterface*);
    
    /**Virtual destructor*/
    virtual ~TrkMaterialProviderTool();
    
    /** AlgTool initailize method.*/
    StatusCode initialize();
    /** AlgTool finalize method */
    StatusCode finalize();

    /** Update Calorimeter TSOS from input ID and MS tracks*/
    void updateCaloTSOS(Trk::Track& idTrack, Trk::Track& extrapolatedTrack);

    /** Update Calorimeter TSOS from input MS/CB track*/
    void updateCaloTSOS(Trk::Track& track, const Trk::TrackParameters* startParamaters=0);

    /** Get Calorimeter MEOT*/
    void getCaloMEOT(const Trk::Track& idTrack, const Trk::Track& msTrack, std::vector<MaterialEffectsOnTrack>& calomeots);

    /** Retrieve Calorimeter TSOS from TG and apply corrections*/
    std::vector<const Trk::TrackStateOnSurface*>*
      getCaloTSOS (const Trk::TrackParameters&	parm, const Trk::Track &muonTrack, const Trk::TrackParameters* parms=0) const;
    
    /** Retrieve a clone of the parametrised energy loss*/
    CaloEnergy* getParamCaloELoss(Trk::Track* track);
  
  private:      

    /** Internal method to retrieve Calorimeter TSOS from TG and apply corrections*/
    DataVector<const Trk::TrackStateOnSurface>* 
      getCaloTSOS (const Trk::TrackParameters&	parm,
		   const Trk::Track&            muonTrack,
		   const Trk::Surface&		surf,
		   Trk::PropDirection		dir,
		   Trk::ParticleHypothesis	mateffects,
                   double&                      Eloss,
                   double&                      X0ScaleMS,
                   double&                      ElossScaleMS,
		   const Trk::TrackParameters*	parms=0,
		   bool                         boundaryCheck=false,
		   bool                         removeOoC=false)     const;

    /**Helper to get last calo TSOS with TP*/
    const Trk::TrackStateOnSurface* getCaloExitTSOS(const std::vector<const Trk::TrackStateOnSurface*>* caloTSOS, Trk::PropDirection dir) const;

    /**Helper to get first calo TSOS with TP*/
    const Trk::TrackStateOnSurface* getCaloEntryTSOS(const std::vector<const Trk::TrackStateOnSurface*>* caloTSOS, Trk::PropDirection dir) const;

    /** Helper to remove MS and ID TSOS */
    void removeOutOfCalo(std::vector<const Trk::TrackStateOnSurface*>* caloTSOS) const;
     /** Helper to remove only MS TSOS */
    void removeMS(std::vector<const Trk::TrackStateOnSurface*>* caloTSOS) const;

    /** Helper to update entries in the vector*/
    void updateVector(DataVector<const Trk::TrackStateOnSurface>* inputTSOS, 
		      DataVector<const Trk::TrackStateOnSurface>::iterator lastID, 
		      DataVector<const Trk::TrackStateOnSurface>::iterator firstMS, 
		      DataVector<const Trk::TrackStateOnSurface>* caloTSOS);

    /** update the TSOS vector for the Muon Spectrometer applying X0 and Eloss scaling*/
    void updateVectorMS(DataVector<const Trk::TrackStateOnSurface>* inputTSOS,
                                                  DataVector<const Trk::TrackStateOnSurface>::iterator firstMS,
                                                  double X0ScaleMS, double ElossScaleMS);

    //* Helper to indentify detector volume**/
    unsigned int getVolumeByGeo(const Trk::TrackStateOnSurface* m) const;

    /* Helper to delete TSOS (data)vectors 
       Note that DataVector ownership is taken and elements deleted!*/
    void deleteTSOS(const std::vector<const Trk::TrackStateOnSurface*>* vecTSOS) const;
    void deleteTSOS(DataVector<const Trk::TrackStateOnSurface>* vecTSOS) const;

    //** Helper to printout TSOS details*/
    void printTSOS(const Trk::TrackStateOnSurface* m, std::string tag) const;
           
    /** Function to modify TSOS doing repositioning, aggregation and corrections*/
    DataVector<const Trk::TrackStateOnSurface>* 
      modifyTSOSvector(const std::vector<const Trk::TrackStateOnSurface*>* matvec, 
		       double scaleX0, 
		       double scaleEloss, 
		       bool reposition, 
		       bool aggregate, 
		       bool updateEloss, 
		       double caloEnergy, 
		       double caloEnergyError, 
		       double fsrCaloEnergy,
		       double pCaloEntry, 
		       double momentumError,
		       double& Eloss_tot,
                       bool useMeasuredEnergy=true,
                       double mopEloss=0., double meanElossIoni=0., double sigmaElossIoni=0.) const;

    /** Function to get mop and mean (ionization) energy loss*/
    void getMopAndIoniEnergyLoss(const std::vector<const Trk::TrackStateOnSurface*>* matvec,
				 double pCaloEntry,
				 double scaleEloss,
				 double& mopEloss,
				 double& meanElossIoni,
				 double& sigmaElossIoni) const;

    /** Function to check isolation*/
    bool isIsolatedTrack(double eta, double phi) const;

    /** Function to get calorimeter measured energy loss*/
    double getCaloMeasuredEnergy(double eta, double phi, 
				 double mopEloss,
				 double meanElossIoni,
				 double& fsrCaloEnergy) const;

    /** Function to get corrected (final) calorimeter measured energy loss*/
    double getFinalMeasuredEnergy(Rec::CaloMeas* caloMeas, 
				  double mopEloss,
				  double meanElossIoni,
				  double eta,
				  double& fsrCaloEnergy) const;
    
    
    ToolHandle<Trk::IExtrapolator>                m_muonExtrapolator;
    ToolHandle<Trk::IEnergyLossUpdator>           m_elossupdator;
    ServiceHandle<Trk::ITrackingVolumesSvc>       m_trackingVolumesSvc;
    ServiceHandle<Trk::ITrackingGeometrySvc>      m_trackingGeometrySvc;
    ToolHandle< Trk::IMultipleScatteringUpdator > m_scattool;

    ToolHandle<Rec::IMuidCaloEnergyMeas>  m_caloMeasTool;
    ToolHandle<Rec::IMuidCaloEnergyParam> m_caloParamTool;
    ToolHandle<Rec::IMuidTrackIsolation>  m_trackIsolationTool;
    ToolHandle<Rec::IMuonCaloEnergyTool>  m_muonCaloEnergyTool;

    const AtlasDetectorID *m_DetID;
    const Trk::Volume* m_calorimeterVolume;
    const Trk::Volume* m_indetVolume;

    ServiceHandle<MagField::IMagFieldSvc>           m_magFieldSvc;

    bool m_applyTGScaling;
    bool m_repositionTSOS;
    bool m_aggregateTSOS;
    bool m_updateTSOS;

    int m_maxNTracksIso;
    double m_paramPtCut;
    bool m_useCaloEnergyMeasurement;
    bool m_useMuonCaloEnergyTool;
    bool m_overwriteElossParam;
    bool m_infoExtrapolation; 

  };
} // end namespace

#endif // TrkMaterialProviderTool_H
