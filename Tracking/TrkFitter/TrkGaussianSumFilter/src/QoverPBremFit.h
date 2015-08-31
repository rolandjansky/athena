/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************************
  QoverPBremFit.h * description

  completed   : Tuesday 18th November 2008
  author      : Tony Shao
  email       : Qi.Tao.Shao@cern.ch
  description : Class called by BremFind to fit QoverP vs R plots using a sum of Tanh graphs
  **********************************************************************************/

#ifndef Trk_QoverPBremFit_H
#define Trk_QoverPBremFit_H

#include <vector>

#include "GaudiKernel/ToolHandle.h"
#include "DataModel/DataVector.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkGaussianSumFilter/BremFind.h"

namespace Trk {

  class IMultiComponentStateCombiner;
  class MultiComponentStateOnSurface; 
  class TrackStateOnSurface;


#define TRKFGSF_VALSURFACES 100
#define TRKGSF_VALSTATES 24
#define POTENTIAL_BREM_THRESHOLD 0.0001
 

  class QoverPBremFit {

    // forbid copying
    QoverPBremFit(const QoverPBremFit &) = delete ;
    QoverPBremFit &operator=(const QoverPBremFit &) = delete;

  public:
  
    QoverPBremFit(int, bool); //type=1 for forward fit || type=-1 for backward fit
    //bool for Z mode or not. True for Z mode.
   
    StatusCode initialize();
   
    ~QoverPBremFit();
   
    GraphParameter GetParameters(const DataVector <const Trk::TrackStateOnSurface>&);
   
    double GetPerigee_1overP();
    double GetPerigee_Phi();  
    double GetPerigee_Theta();
    double GetPerigee_d0();
    double GetPerigee_z0();
   
    int GetnSurfaces();
    double Get1overP(int);
    double GetPhi(int);
    double GetTheta(int);
    double GetRadius(int);
    double GetZ(int);
    double Get1overPerror(int);
    double GetPhierror(int);
    double GetThetaerror(int);
    double GetRadiuserror(int);
    double GetZerror(int);
    double GetValue(int);
    double GetCharge();
    double LastFitMeasurement();
    double FirstFitMeasurement();
    
    const Trk::TrackParameters* GetTrackParameters(int);
   
    int GetNMeasurements();
   
    void FillVariables(const DataVector <const Trk::TrackStateOnSurface>&);
  
    void FindPotentials();
   
    GraphParameter FindParameters();
   
    GraphParameter MergeParameters(GraphParameter);
   
  private:
   
    int nmeasurements (double, double);
   
    int npotentials (double, double); 
   
    double maxvalue (double, double);
  
    double coefficient_sum (std::vector<double>);
  
    double value_sum (std::vector<double>,std::vector<double>);
  
    double width_sum (std::vector<double>,std::vector<double>,std::vector<double>);

    //defining the tools used in this class
    ToolHandle<IMultiComponentStateCombiner> m_stateCombiner;

    int m_surfaceCounter;
    int m_type;
    bool m_Z_mode;

    mutable std::vector<double> *m_potentialvalue;
    mutable std::vector<double> *m_potentialwidth;

    double m_charge;
    std::vector<double> *m_1overPvalue;
    std::vector<double> *m_1overPvalueerror;
    std::vector<double> *m_PhiValue;
    std::vector<double> *m_PhiValueerror;
    std::vector<double> *m_ThetaValue;
    std::vector<double> *m_ThetaValueerror;
    std::vector<double> *m_RadiusValue;
    std::vector<double> *m_Zvalue;
    std::vector<double> *m_FitValue;
    std::vector<const Trk::TrackParameters*> *m_trackParameters;

    GraphParameter m_graphparameter;
    GraphParameter m_mergedparameter;
    
  };

}
#endif
