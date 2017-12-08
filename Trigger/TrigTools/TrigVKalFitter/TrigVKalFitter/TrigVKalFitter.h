/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrigVKalFittter.h
//
#ifndef _TrigVKalVrt_TrigVKalFitter_H
#define _TrigVKalVrt_TrigVKalFitter_H
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"
// Normal STL and physical vectors
#include <vector>
#include "CLHEP/Vector/LorentzVector.h"
// Gaudi includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
//


static const int m_NTrMaxTrig=300;

typedef std::vector<double> dvect;
class VTrack;
class TrigAtlasMagFld;


class TrigInDetTrack;


//------------------------------------------------------------------------
static const InterfaceID IID_ITrigVKalFitter("ITrigVKalFitter", 1, 0);

class ITrigVKalFitter : virtual public IAlgTool {
public:
   static const InterfaceID& interfaceID() { return IID_ITrigVKalFitter;}
   //---------------------------------------------------------------------------
   //Interface itself
   virtual StatusCode VKalVrtFit(const std::vector<const TrigInDetTrack*>     & list,
                                 dvect&         FitResult) =0;
   
   
   
   virtual StatusCode VKalVrtFit(const std::vector<const TrigInDetTrack*> & list,
                                 Amg::Vector3D&        Vertex,
                                 CLHEP::HepLorentzVector&   Momentum,
                                 long int&           Charge,
                                 dvect&              ErrorMatrix,
                                 dvect&              Chi2PerTrk,
                                 std::vector< std::vector<double> >& TrkAtVrt,
                                 double& Chi2 ) =0;



   virtual StatusCode VKalVrtCvtTool(const Amg::Vector3D& Vertex,
                                     const CLHEP::HepLorentzVector& Momentum,
                                     const dvect& CovVrtMom,
                                     const long int& Charge,
                                     dvect& Perigee,
                                     dvect& CovPerigee) =0;
   virtual TrigInDetTrack* CreateTrigTrack( const std::vector<double>& VKPerigee,
                                            const std::vector<double>& VKCov) =0;


   //.........................................................................................
   virtual StatusCode VKalVrtFitFast(const std::vector<const TrigInDetTrack*>& list,
                                     Amg::Vector3D& Vertex) =0;

   //.........................................................................................

   virtual StatusCode VKalGetTrkCov(const long int, const long int,
                                    dvect& CovMtx) =0;
   virtual StatusCode VKalGetMassError( std::vector<int> ListOfTracks , 
                                        double& Mass, double& MassError) =0;

   virtual void setApproximateVertex(double,double,double)=0;
   virtual void setMassForConstraint(double)=0;
   virtual void setMassForConstraint(double,std::vector<int>)=0;
   virtual void setRobustness(int)=0;
   virtual void setCnstType(int)=0;
   virtual void setMomCovCalc(int)=0;
   virtual void setIterations(int, double)=0;
   virtual void setVertexForConstraint(double,double,double)=0;
   virtual void setCovVrtForConstraint(double,double,double,
                                       double,double,double)=0;
   virtual void setMassInputParticles( std::vector<double>& )=0;
   virtual void setDefault() =0;
   virtual void setZeroCharge(int)=0;
   virtual void setAthenaField(MagField::IMagFieldSvc*)=0;
   //----
   virtual double VKalGetImpact(const TrigInDetTrack*,const Amg::Vector3D& Vertex, const long int Charge,
                                std::vector<double>& Impact, std::vector<double>& ImpactError)  = 0;
};


//------------------------------------------------------------------------
class TrigVKalFitter : public AthAlgTool, virtual public ITrigVKalFitter {
public:
   /* Constructor */
   TrigVKalFitter(const std::string& type, const std::string& name, 
                  const IInterface* parent);
   /* Destructor */
   virtual ~TrigVKalFitter();

   StatusCode VKalVrtFit(const std::vector<const TrigInDetTrack*>    &, dvect&  FitResult );



   StatusCode VKalVrtFit(const std::vector<const TrigInDetTrack*>&,
                         Amg::Vector3D&         Vertex,
                         CLHEP::HepLorentzVector&   Momentum,
                         long int&           Charge,
                         dvect&              ErrorMatrix,
                         dvect&              Chi2PerTrk,
                         std::vector< std::vector<double> >& TrkAtVrt,
                         double& Chi2 );
 
 

   StatusCode VKalVrtCvtTool(const Amg::Vector3D& Vertex,
                             const CLHEP::HepLorentzVector& Momentum,
                             const dvect& CovVrtMom,
                             const long int& Charge,
                             dvect& Perigee,
                             dvect& CovPerigee) ;

   TrigInDetTrack* CreateTrigTrack( const std::vector<double>& VKPerigee,
                                    const std::vector<double>& VKCov);

   //............................................................................
   StatusCode VKalVrtFitFast(const std::vector<const TrigInDetTrack*>&,
                             Amg::Vector3D& Vertex);
   //............................................................................

   double VKalGetImpact( const TrigInDetTrack*,const Amg::Vector3D& Vertex, const long int Charge,
                         std::vector<double>& Impact, std::vector<double>& ImpactError);

   StatusCode VKalGetTrkCov(const long int, const long int,dvect& CovMtx);

   StatusCode VKalGetMassError( std::vector<int> ListOfTracks , 
                                double& Mass, double& MassError);
      
   void setApproximateVertex(double,double,double);
   void setMassForConstraint(double);
   void setMassForConstraint(double,std::vector<int>);
   void setRobustness(int);
   void setCnstType(int);
   void setIterations(int, double);
   void setVertexForConstraint(double,double,double);
   void setCovVrtForConstraint(double,double,double,
                               double,double,double);
   void setMassInputParticles( std::vector<double>& );
   void setMomCovCalc(int);
   void setDefault();
   void setZeroCharge(int);
   void setAthenaField(MagField::IMagFieldSvc *);
   void setAthenaField(const double);

   //
   //  Public functions not in interface
   // 

private:

   int m_FitStatus;    /* Fit Status flag*/
   /* =0 - no fit. All "after fit" routines fail*/
   /* >1 - good fit. "After fit" routines work*/

   int m_PropagatorType; /* type of propagator used for fit. VKalVrtCore definition */
   /* =0 - constant field propagator from VKalVrtCore         */
   /* =1 - Runge-Kutta propagator from VKalVrtCore            */
   /* =3 - external propagator accessed via VKalExtPropagator (not used)*/


   double m_BMAG;       /* Magnetic field*/
   double m_CNVMAG;     /* conversion constant for MeV and MM */
   long int m_ifcovv0;
   long int m_iflag;

   SimpleProperty<int>    m_Robustness;
   SimpleProperty<int>    m_Constraint;
   SimpleProperty<int>    m_CalcMomCovariance;
   SimpleProperty<double> m_MassForConstraint;
   SimpleProperty<int>    m_IterationNumber;
   SimpleProperty<double> m_IterationPrecision;
   SimpleProperty< std::vector< double > > mp_CovVrtForConstraint;
   SimpleProperty< std::vector< double > > mp_MassInputParticles;
   SimpleProperty< std::vector< double > > mp_ApproximateVertex;


   ServiceHandle<MagField::IMagFieldSvc> m_magFieldAthenaSvc;                //Athena magnetic field 
 

   std::vector<double>    m_MassInputParticles;
   std::vector<double>    m_VertexForConstraint;
   std::vector<double>    m_CovVrtForConstraint;
   std::vector<double>    m_ApproximateVertex;
   std::vector<int>       m_TrackCharge;
   std::vector<double>    m_PartMassCnst;
   std::vector< std::vector<int> >    m_PartMassCnstTrk;
   //
   // Arrays needed for fitting kernel
   //
   long int m_ich[m_NTrMaxTrig];
   double m_chi2tr[m_NTrMaxTrig];
   double m_par0[m_NTrMaxTrig][3];
   double m_apar[m_NTrMaxTrig][5];
   double m_awgt[m_NTrMaxTrig][15];
   double m_parfs[m_NTrMaxTrig][3];
   double m_wm[m_NTrMaxTrig];
   double m_VrtCst[3];
   double m_CovVrtCst[6];
   //
   //  Very local variables
   //
   //      double m_loc_xyzfit[3];
   //      double m_loc_ptot[4];
   //      double m_loc_covf[21];
   //      double m_loc_chi2f;


   TrigAtlasMagFld*  m_fitField;


   //      
   // Private functions (technicalities)
   //
   StatusCode CvtTrigTrack(const std::vector<const TrigInDetTrack*>& list,
                           long int& ntrk);

   void    VKalVrtSetOptions(long int NInputTracks);

   void    VKalToTrkTrack( double  , double  , double ,
                           double& , double& , double& );

   long int VKalVrtFit3( long int ntrk, Amg::Vector3D& Vertex, CLHEP::HepLorentzVector&   Momentum,
                         long int& Charge, dvect& ErrorMatrix, dvect& Chi2PerTrk, 
                         std::vector< std::vector<double> >& TrkAtVrt, double& Chi2 );

   long int VKalVrtFit2(long int ntrk, Amg::Vector3D& Vertex, dvect& ErrorMatrix,double& Chi2 );

   long int VKalVrtFit1(long int ntrk, dvect& FitResult); 

public:
   virtual StatusCode initialize();

};

#endif
