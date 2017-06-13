/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************************
                      BremFind.h * description

completed   : Tuesday 18th November 2008
author      : Tony Shao
email       : Qi.Tao.Shao@cern.ch
description : Class for finding brem points in the inner detector using the GSF
**********************************************************************************/

#ifndef Trk_BremFind_H
#define Trk_BremFind_H

#include "TrkGaussianSumFilter/IBremsstrahlungFinder.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "DataModel/DataVector.h"
#include "TrkFitterUtils/FitterTypes.h"
#include "TrkParameters/TrackParameters.h"

#include "TrkExInterfaces/IPropagator.h"
#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"
#include "TrkGeometry/MagneticFieldProperties.h"

#include "GeoPrimitives/GeoPrimitives.h"

#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"

#include <vector>

//class ITrackingGeometrySvc;
class TTree;

namespace Trk {

class IMultiComponentStateCombiner;
class MultiComponentStateOnSurface;
class TrackStateOnSurface;
class EstimatedBremOnTrack;
class Layer;
class TrackingVolume;
class TrackingGeometry;
class EntryLayerProvider;
class AbstractVolume;
class Surface;
class QoverPBremFit;
    
struct GraphParameter {
  double constant;
  std::vector<double> coefficient{};
  std::vector<double> value{};
  std::vector<double> width{};
};
    
struct Element {
  double value{};
  double width{};
  bool sign{};
};
 
  
#define TRKFGSF_VALSURFACES 100
#define TRKGSF_VALSTATES 24
#define POTENTIAL_BREM_THRESHOLD 0.0001
#define MIN_KINK_THRESHOLD 0.005
#define BREM_AMPLITUDE 1.0 //in GeV
#define BREM_SEPARATION 0.005
#define COEFFICIENT_THRESHOLD 0.005
#define BOUND_THRESHOLD 20.0 //in percentage

class BremFind : public AthAlgTool, virtual public IBremsstrahlungFinder{
  public:
    
    BremFind(const std::string&, const std::string&, const IInterface*);

    virtual ~BremFind () {};

    StatusCode initialize();

    /** AlgTool finalise method */
    StatusCode finalize();

    //Find the brem points from a forward and smooth track fit
    virtual void BremFinder(const Trk::ForwardTrajectory&, const Trk::SmoothedTrajectory&);

    virtual void BremFinder(const Trk::ForwardTrajectory&, const Trk::SmoothedTrajectory&, bool);
    
    virtual void CombineParameters();

    virtual void GetBremLocations();
 
    virtual double GetBound(double, int);

    virtual double BremSignificance(double, QoverPBremFit*, QoverPBremFit*);

    virtual double GetPhiFromValue(double, QoverPBremFit*);
    
    virtual int ClosestMeasurement(double, QoverPBremFit*);

    virtual void CalibrateValue();
    
    virtual const Surface* ClosestSurface(double, double, double, const TrackParameters&); 
    
    virtual Amg::Vector3D SurfacePosition(const TrackParameters&, const Surface&, double, double, double);
    
    virtual StatusCode retrieveTrackingGeometry();
    
    virtual int GetNBrems();

    virtual void MakeTrackStateOnSurfaces ();
    
    virtual const TrackStateOnSurface* GetEstimatedBremOnTrack(int);
    

  private:
    
    double Graph_value(GraphParameter, double);
      
    double GetMaximumX(GraphParameter, double, double);

    double GetMaximumY(GraphParameter, double, double);

    double UniqueAngle(double);

    void SeparationQuality(std::vector<double>*, std::vector<double>*);

    std::pair<double,double> ProbabilityScore(double, double, double, double);

    //defining the tools used in this package
    ToolHandle<IMultiComponentStateCombiner>     m_stateCombiner;    
 
    ToolHandle<IPropagator> m_propagator;
    MagneticFieldProperties m_fieldProperties;
   
    ServiceHandle<ITrackingGeometrySvc> m_trackingGeometrySvc;   //!< Name of the TrackingGeometrySvc
    std::string                       m_trackingGeometryName;    //!< TrackingGeometry to be retrieved from DetectorStore
    mutable const TrackingGeometry*   m_trackingGeometry;        //!< The underlying TrackingGeometry
    
    //Validation variables
          
    bool m_validationMode;
    std::string m_validationTreeName;
    std::string m_validationTreeName2;
    std::string m_validationTreeDescription;
    std::string m_validationTreeFolder;
    std::string m_validationTreeFolder2;

    TTree* m_validationTree;
    TTree* m_validationTree2;

    //Calibration option
    bool m_useCalibration;
    
    //Propagation option
    bool m_usePropagate;
        
    //Class variables
    GraphParameter m_forwardparameters;
    GraphParameter m_smoothedparameters;
    GraphParameter m_combinedparameters;

    double m_perigee_1overP;
    double m_perigee_Phi;
    double m_perigee_Theta;
    double m_perigee_d0;
    double m_perigee_z0;

    std::vector<double> *m_brem_value; //could be R or Z;
    std::vector<double> *m_brem_phi;
    std::vector<double> *m_brem_theta;
    std::vector<double> *m_brem_energy;
    std::vector<double> *m_brem_UpperBound;
    std::vector<double> *m_brem_LowerBound;
    std::vector<double> *m_forward_kink;
    std::vector<double> *m_smoothed_kink;
    std::vector<double> *m_brem_significance;
    std::vector<double> *m_brem_valueCalibrated;
    std::vector<double> *m_surfaceX;
    std::vector<double> *m_surfaceY;
    std::vector<double> *m_surfaceZ;
  

    int m_nBrems;
    bool m_Z_mode;

    std::vector<const Trk::TrackStateOnSurface*> m_brem_trackStateOnSurface;
    std::vector<const Trk::TrackParameters*> m_brem_TrackParameters;
    std::vector<const Trk::EstimatedBremOnTrack*> m_EstimatedBremOnTrack;
    std::vector<const Trk::Surface*> m_brem_surfaces;
    std::vector<const Trk::Layer*> m_brem_layers;
    std::vector<Amg::Vector3D> m_surface_positions;

    int m_event_ID;
    double m_forwardparameter_constant;
    std::vector<double> *m_forwardparameter_coefficient;
    std::vector<double> *m_forwardparameter_value;
    std::vector<double> *m_forwardparameter_width;
    double m_smoothparameter_constant;
    std::vector<double> *m_smoothparameter_coefficient;
    std::vector<double> *m_smoothparameter_value;
    std::vector<double> *m_smoothparameter_width;

    std::vector<double> *m_forward_1overP;
    std::vector<double> *m_forward_1overPerr;
    std::vector<double> *m_forward_value;
    std::vector<double> *m_smooth_1overP;
    std::vector<double> *m_smooth_1overPerr;
    std::vector<double> *m_smooth_value;
    
    std::vector<double> *m_KinkSeparationScores;
    std::vector<double> *m_KinkSeparationScoresErr;

    QoverPBremFit *m_forwardBremFit;
    QoverPBremFit *m_smoothedBremFit;

    SG::ReadHandleKey<xAOD::EventInfo> m_readKey;

  };

}

#endif
