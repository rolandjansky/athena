/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TrkVKalVrtCoreBase_VKalVrtCore_H
#define _TrkVKalVrtCoreBase_VKalVrtCore_H

#include "TrkVKalVrtCore/TrkVKalVrtCore.h"
#include "TrkVKalVrtCore/ForVrtClose.h"
#include "TrkVKalVrtCore/ForCFT.h"
#include "TrkVKalVrtCore/CommonPars.h"
#include <vector>
#include <iostream>

namespace Trk {

  
   class VKConstraintBase;
   class VKVertex;

   class CascadeEvent 
   {
     public:
       int cascadeNV;
       int nearPrmVertex;
       double getSCALE() const {return m_SCALE;};
       void   setSCALE(double S) {m_SCALE=S;};
       double getAccuracyConstraint() const { return m_accuracyConstraint; };
       void   setAccuracyConstraint(double C) { m_accuracyConstraint=C; };
       double *fullCovMatrix;
       std::vector< VKVertex *> cascadeVertexList; 
       std::vector<int> matrixPnt;
       CascadeEvent():cascadeNV(0), nearPrmVertex(0), fullCovMatrix(0), cascadeVertexList(0), 
                      matrixPnt(0), m_SCALE(1.), m_accuracyConstraint(1.e-4) {};
      ~CascadeEvent(){if(fullCovMatrix)delete[] fullCovMatrix;};

     private:
       double m_SCALE;
       double m_accuracyConstraint;


   };

   class TWRK       // collection of temporary arrays for 
   {   
    public:
      TWRK() = default;
     ~TWRK() = default;//default destructor allows compiler to optimize out method in this case.

     public:
       double   tt[3];    // U_i vector (see Billoir...)
       double   wb[9];
       double   wc[6]; 
       double  wci[6];
       double wbci[9];
       double drdp[2][3]; // for "pass near" constraint
       double parf0[3];   // parameter shifts during iteration
       double part[3];    // temporary array for fostfit optimization
   };

//
//   Main container for all track related information in vertex fit
//--------------------------------------------------------------------
   class VKTrack
   {
    public:
       VKTrack(long int, double[], double[], VKVertex *, double);
      ~VKTrack() = default; //default destructor allows compiler to optimize out method.
//       VKTrack(const VKTrack & src);  //copy
       friend std::ostream& operator<<( std::ostream& out, const VKTrack& track );

     public:
       long int Id;      //track ID number
       int Charge;       //track charge

               // Fitted track perameters at vertex VKVErtex->fitV
       double fitP[3];
       double Chi2;

               // Track perameters at vertex VKVErtex->cnstV for constraint calculations
       double cnstP[3];

               // ALL is estimated at VKVErtex->iniV posision - start position of the fit
       double iniP[3];     //perigee parameters assuming that track passes through iniXYZ 

               // ALL is estimated at VKVErtex->refIterV posision - iteration reference position 
       double Perig[5];         //perigee parameters prepared for vertex estimation
       double WgtM[15];         //symmetric weight matrix prepared for vertex estimation
       double WgtM_save[15];    //copy of weight matrix for speed optimisation

               // All for robustification
       double rmnd[5];     //remnants with respect to fitted vertex position
       double e[5];        //eigenvalues of weight matrix
       double v[5][5];     //corresponding eigenvectors of weight matrix

               // ALL is estimated at VKVErtex->refXYZ posision - reference position for vertex fit
       double refPerig[5];  //perigee parameters
       double refCovar[15]; //symmetric covariance matrix 

     public:
       double getMass() const  {return m_mass;}
       void   setMass(double M){ m_mass=M;}
       double a0()    const    { return Perig[0];}
       double z()     const    { return Perig[1];}
       double theta() const    { return Perig[2];}
       double phi()   const    { return Perig[3];}
       double invR()  const    { return Perig[4];}
       double r_a0()    const  { return refPerig[0];}
       double r_z()     const  { return refPerig[1];}
       double r_theta() const  { return refPerig[2];}
       double r_phi()   const  { return refPerig[3];}
       double r_invR()  const  { return refPerig[4];}

     public:
       void setCurrent  ( double[], double[]); // set iteration (current) track parameters 
       void setReference( double[], double[]); // set reference track parameters 
       void restoreCurrentWgt();               // restore track WGT from saved copy 

     private:

       VKVertex* m_originVertex;
       double m_mass;      //track mass

   };
  


   class VKVertex
   {   
    public:
      VKVertex(const VKalVrtControl &);
      VKVertex();
     ~VKVertex();
      VKVertex(const VKVertex & src);              //copy
      VKVertex& operator= (const VKVertex & src);  //assign

     public:        // Relative coordinates with respect to refIterV[]
       double Chi2;         // vertex Chi2
       double fitV[3];      //fitted vertex position on given iteration step
       double fitVcov[6];   //symmetric covariance matrix of fitted vertex
       double iniV[3];      //starting point for the fit.
       double cnstV[3];     //position for constraint values calculation. May be different from fitV or iniV

     public:        //Global coordinates
       double refIterV[3];  //initial point for iteration step and target for preparatory
                            //track extrapolation. At this point Perig[] and WgtM[] for each track are defined.
       double refV[3];      //reference point for given vertex. At this point refPerig[] and refCovar[]
                            //for each track are defined

       int useApriorVertex;   //for a priory vertex position knowledge usage
       double apriorV[3];     //global coordinates (the same as refV and refIterV)
       double apriorVWGT[6];  //weight matrix of a priori vertex   


       bool passNearVertex;       // needed for "passing near vertex" constraint
       bool passWithTrkCov;       //  Vertex, CovVertex, Charge and derivatives 
       double fitMom[3];          //   are in ForVrtClose structure
       double fitCovXYZMom[21];   //  Mom and CovMom are here because they are used also for other purposes
       ForVrtClose FVC;


       double T[3];           // save T(see Billoir) vector for futher use
       double wa[6];          // save WA matrix for futher use
       double dxyz0[3];       // unconstrained shift of vertex on current iteration. Needed for PostFit
      
       std::vector<VKTrack* > TrackList;
       std::vector<TWRK* >    tmpArr;
       std::vector<VKConstraintBase *> ConstraintList;


       void setRefV(double []);
       void setCnstV(double []);
       void setRefIterV(double []);
       void setIniV(double []);
       void setFitV(double []);


       VKVertex * nextCascadeVrt;
       std::vector<VKVertex*> includedVrt;  // these vertices are NOT owned by given object.
       double savedVrtMomCov[21]; // saved covariance WITHOUT pointing constraint
				  // for correct cascade error definition 
     public: 

       bool truncatedStep;
       int  existFullCov;
       double ader[(3*vkalNTrkM+3)*(3*vkalNTrkM+3)];  // was [903][903]

     public:        // Object with defining information for VKalVrtCore library.
                    // Each vertex has a copy of VKalVrtControl object what allows
                    // to fit it independently with individual set of constraints.
                    // See Cascade code. 
       std::unique_ptr<VKalVrtControl>  vk_fitterControl;
   };




} // end of namespace bracket

#endif
