/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIGINDETBREMDETECTIONTOOL_H__
#define __TRIGINDETBREMDETECTIONTOOL_H__

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/MsgStream.h"

#include "TrigInDetToolInterfaces/ITrigInDetBremDetectionTool.h"
#include <vector>

#define MAX_RES_SIZE 100
#define MAX_INP_SIZE 20


class LSMSolution  
{
public:
  void m_report();
  void m_fixVariable(int);
  bool m_isOnConstraint(int);
  double& m_Significance();
  double& m_Cov(int,int);
  LSMSolution(int);
  LSMSolution();
  virtual ~LSMSolution();
  double& operator[] (int);

private:
  int m_fixedVariables[MAX_INP_SIZE];
  double m_Chi2;
  double m_C[MAX_INP_SIZE][MAX_INP_SIZE];
  int m_size;
  double m_u[MAX_INP_SIZE];
};

class TrigInDetBremDetectionTool : virtual public ITrigInDetBremDetectionTool, public AthAlgTool {
 public:
      
  // standard AlgTool methods
  TrigInDetBremDetectionTool(const std::string&,const std::string&,const IInterface*);
  virtual ~TrigInDetBremDetectionTool();
		
  // standard Athena methods
  StatusCode initialize();
  StatusCode finalize();
  virtual void reset(); 
  virtual bool addNewPoint(Trk::TrkTrackState*,Trk::TrkBaseNode*, Trk::TrkPlanarSurface*, double a[5][5],double);
  virtual bool solve(int);
  virtual void modifySurfaces(int);
  virtual void report(int);
    
  private:

    void m_mixSolutions(LSMSolution*, LSMSolution*);
    bool m_checkFeasibility(LSMSolution*);
    int m_findBestDirection(double*);
    bool m_goodGradient(double*);
    bool m_isZempty();
    double m_getCriterionValue(LSMSolution*);
    LSMSolution* m_solveLSM();
    void m_fixVariables(const int *);
    void m_precomputeGain();
    void m_report();
    void m_getGradient(LSMSolution*,double[]);
    bool CholeskyDecompositionNxN(double*,int);
    bool invertMatrixNxN(double*, int);
    double m_K[MAX_INP_SIZE][MAX_RES_SIZE];
    double m_W[MAX_INP_SIZE][MAX_INP_SIZE];
    int m_size;
    
    double m_A[MAX_RES_SIZE][MAX_INP_SIZE];
    double m_MG[5][MAX_INP_SIZE];
    double m_jX[MAX_INP_SIZE];
    double m_jY[MAX_INP_SIZE];
    double m_jZ[MAX_INP_SIZE];
    double m_S[MAX_RES_SIZE][2];
    mutable MsgStream  m_log;         //!< msgstream as private member (-> speed)
    int m_outputLevel; //!< private member to control debug messages
    double m_totalPath;
    double m_minDistance;
    double m_P0;
    double m_SignificanceCut;
    int m_lsmSize,m_resSize;
    std::vector<int> m_resSizes;
    double m_R[MAX_INP_SIZE];
    
    int m_Parray[MAX_INP_SIZE];
    int m_Zarray[MAX_INP_SIZE];
    int m_sign;
    double m_Threshold;
    LSMSolution* m_pLS;
    Trk::TrkPlanarSurface* m_surfArray[MAX_INP_SIZE];
  };

#endif 
