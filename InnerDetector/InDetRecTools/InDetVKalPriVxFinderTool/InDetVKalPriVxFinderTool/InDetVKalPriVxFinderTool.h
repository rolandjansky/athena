/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*******************************************************
   InDetVKalPriVxFinderTool.h - description

 A tool to find primary vertex with or without pileup, with or without beam constraint.
 It works with TrackCollection of TrackParticleBaseCollection.
 Beam position if needed is provided via BeamCond service or
 jobOptions. 
 
   Tool used VKalVrt vertex fitter (TrkVKalVrtFitter tool).
 
   By default primary vertex finder user robust functional Rob=5 (see VKalVrt 
 descrption for details) which provides optimal pulls.

  Author: Vadim Kostyukhin
  e-mail: vadim.kostyukhin@cern.ch


********************************************************/



#ifndef _VKalVrt_InDetVKalPriVxFinderTool_H
#define _VKalVrt_InDetVKalPriVxFinderTool_H
// Normal STL and physical vectors
#include <vector>
// Gaudi includes
#include  "TrkTrack/TrackCollection.h"
#include  "TrkParameters/TrackParameters.h"
#include  "TrkParticleBase/TrackParticleBaseCollection.h"

// Gaudi includes
#include  "AthenaBaseComps/AthAlgTool.h"
#include  "GaudiKernel/ToolHandle.h"
#include  "GaudiKernel/ServiceHandle.h"
//
#include  "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include  "InDetRecToolInterfaces/IVertexFinder.h"
#include  "xAODTracking/Vertex.h"
#include  "xAODTracking/TrackParticle.h"
#include  "xAODTracking/VertexContainer.h"
#include  "xAODTracking/VertexAuxContainer.h"
//


class IBeamCondSvc;


namespace Trk {
   class ITrackSummaryTool;
   class ITrackSelectorTool;
}

namespace InDet {

        
  class InDetVKalPriVxFinderTool : public AthAlgTool, virtual public IVertexFinder
{
  public:

   InDetVKalPriVxFinderTool(const std::string& t, const std::string& n, const IInterface*  p);
   virtual ~InDetVKalPriVxFinderTool();
   StatusCode initialize();
   StatusCode finalize();
//
//    Tool interface
//
   std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> findVertex(const TrackCollection* trackTES);
   std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> findVertex(const Trk::TrackParticleBaseCollection* trackTES);
   std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> findVertex(const xAOD::TrackParticleContainer* trackTES);  
//
//Old   VxContainer* findVertex(const Rec::TrackParticleContainer* partTES);
				

  private:


/*  JobOption tunable parameters */

// Beam position
      double   m_BeamPositionX;
      double   m_BeamPositionY;
      long int m_BeamConstraint;
      long int m_TypeRobust;
      double m_RobustScale;

// Track selection
      long int m_CutSctHits;
      long int m_CutPixelHits;
      long int m_CutSiHits;
      long int m_CutBLayHits;
      long int m_CutSharedHits;
      double m_CutPt;
      double m_CutZVrt;
      double m_CutA0;
      double m_CutChi2;
      double m_A0TrkErrorCut;
      double m_ZTrkErrorCut;
// Cuts for track - initial vertex association 
      double m_RImpSelCut;
      double m_ZImpSelCut;
      double m_RDistSelCut;
      double m_ZDistSelCut;
      double m_SignifSelCut;
// Stop point for the common fit
      double m_SecTrkChi2Cut;
      double m_WeightCut;
// Maximal number of vertices
      long int m_NPVertexMax;


   ToolHandle< Trk::ITrkVKalVrtFitter > m_fitSvc;
   ToolHandle< Trk::ITrackSummaryTool > m_sumSvc; //!< Pointer to the track summary tool
   ServiceHandle< IBeamCondSvc >  m_iBeamCondSvc; //!< pointer to the beam condition service
   //Trk::ITrkVKalVrtFitter*      m_fitSvc;
   //IBeamCondSvc* m_iBeamCondSvc; 
   ToolHandle<Trk::ITrackSelectorTool>  m_trkSelector;

   Amg::Vector3D m_BeamCnst;
   std::vector<double> m_BeamCnstWid;
//
   int m_SummaryToolExist;
   int m_BeamCondSvcExist;
   int m_trkSelectorExist;
//
// To simplify life....
   StatusCode         m_sc;
   TLorentzVector   m_Momentum;
   long int           m_Charge;
//----------------------------------------------------------------------------------------------
//  Private functions
//
   void   CleanTrkSet(std::vector<const Trk::TrackParticleBase*>& ,std::vector<const Trk::Track*>&,
                      Amg::Vector3D &FitVertex, std::vector<double> &Chi2PerTrk,
                      std::vector<const Trk::TrackParticleBase*>& ,std::vector<const Trk::Track*>&);
   int    FindMax( std::vector<double>& );
   int    FindMin( std::vector<double>& );
   int    FindMaxSecond( std::vector<double>& );
   const Trk::Perigee* GetPerigee( const Trk::TrackParticleBase* List); 
   const Trk::Perigee* GetPerigee( const Trk::Track* List); 
   void   RemoveEntryInList(std::vector<const Trk::Track*>& ListTracks, int );
   void   RemoveEntryInList(std::vector<const Trk::TrackParticleBase*>& ListTracks, int );
   void   RemoveEntryInList(std::vector<double>& List, int Outlier);
   double GetLimitAngle(double);
   void UniqList(std::vector<const Trk::Track*> & List);
   void UniqList(std::vector<const Trk::TrackParticleBase*> & List);
   void RemoveUsedEntry(std::vector<const Trk::Track*>& ,std::vector<const Trk::Track*>& , std::vector<double> &); 
   void RemoveUsedEntry(std::vector<const Trk::TrackParticleBase*>& ,
                         std::vector<const Trk::TrackParticleBase*>&, std::vector<double> & );

   void inpSelector(std::vector<const Trk::TrackParticleBase*>   & ListParticles,
                    std::vector<const Trk::Track*>               & ListTracks,
	            Amg::Vector3D                                   & IniVertex,
                    std::vector<const Trk::TrackParticleBase*>   & SelectedParticles,
                    std::vector<const Trk::Track*>               & SelectedTracks);

   double FitCommonVrt(std::vector<const Trk::TrackParticleBase*>& ListP,
                       std::vector<const Trk::Track*>& ListT,
		       double ZEstimation,
	               Amg::Vector3D           & FitVertex,
                       std::vector<double>  & ErrorMatrix,
		       std::vector<double>  & TrkWeights);

   double  FindZPosTrk(std::vector<const Trk::Track*>& ListTracks,double & ControlVariable);
   double  FindZPosTrk(std::vector<const Trk::TrackParticleBase*>& ListTracks,double & ControlVariable);

   double FindZPos( std::vector<double>   &ZTrk,
                    std::vector<double>   &PtTrk,
                    std::vector<double>   &PxTrk,
                    std::vector<double>   &PyTrk,
                    std::vector<double>   &PhiTrk,
		    double & ControlVar);

   void SetTrkParamVectors(std::vector<const Trk::TrackParticleBase*>& ListTracks,
                           std::vector<double>   &ZTrk,
                           std::vector<double>   &PtTrk,
                           std::vector<double>   &PxTrk,
                           std::vector<double>   &PyTrk,
			   std::vector<double>   &PhiTrk);

   void SetTrkParamVectors(std::vector<const Trk::Track*>& ListTracks,
                           std::vector<double>   &ZTrk,
                           std::vector<double>   &PtTrk,
                           std::vector<double>   &PxTrk,
                           std::vector<double>   &PyTrk,
			   std::vector<double>   &PhiTrk);

   int PVrtListFind(std::vector<const Trk::TrackParticleBase*>  & ListParticles,
                    std::vector<const Trk::Track*>              & ListTracks,
	            std::vector< Amg::Vector3D >                & PVrtList,
                    std::vector< AmgSymMatrix(3) >              & ErrorMatrixPerVrt,
		    std::vector<double>                         & Chi2PerVrt,
		    std::vector<double>                         & ControlVariablePerVrt,
		    std::vector< std::vector<const Trk::TrackParticleBase*> > & PrtPerVrt,
		    std::vector< std::vector<const Trk::Track*> >         & TrkPerVrt,
		    std::vector< std::vector<double> >          & TrkWgtPerVrt);

   std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> SaveResults( int NVrt,
	            std::vector< Amg::Vector3D >                & PVrtList,
                    std::vector< AmgSymMatrix(3) >              & ErrorMatrixPerVrt,
		    std::vector<double>                         & Chi2PerVrt,
                    std::vector<int>                            & NTrkPerVrt,
		    std::vector< std::vector<const Trk::TrackParticleBase*> > & PrtPerVrt,
		    std::vector< std::vector<const Trk::Track*> >         & TrkPerVrt,
		    std::vector< std::vector<double> >          & TrkWgtPerVrt,
                    const TrackCollection* trackTES = 0,
                    const Trk::TrackParticleBaseCollection* partTES = 0);

//   VxContainer* SaveResults( int NVrt,
//	            std::vector< Amg::Vector3D >                & PVrtList,
//                  std::vector< AmgSymMatrix(3) >              & ErrorMatrixPerVrt,
//		    std::vector<double>                         & Chi2PerVrt,
//                  std::vector<int>                            & NTrkPerVrt,
//		    std::vector< std::vector<const Trk::TrackParticleBase*> > & PrtPerVrt,
//		    std::vector< std::vector<const Trk::Track*> >         & TrkPerVrt,
//		    std::vector< std::vector<double> >          & TrkWgtPerVrt,
//                  const TrackCollection* trackTES = 0,
//                  const Trk::TrackParticleBaseCollection* partTES = 0);

   StatusCode CutTrk(double PInvVert, double ThetaVert, double A0Vert, double Chi2, 
           long int PixelHits,long int SctHits,long int SharedHits, long int BLayHits);

//   const vector<const Trk::TrackParticleBase*> Base(const vector<const Rec::TrackParticle*> & listPart);
//   const Trk::TrackParticleBase* Base(const Rec::TrackParticle* listPart);
     double**    getWorkArr2(long int dim1,long int dim2);
     void removeWorkArr2(double  **Arr,long int dim1,long int dim2);
     double***   getWorkArr3(long int dim1,long int dim2,long int dim3);
     void removeWorkArr3(double ***Arr,long int dim1,long int dim2,long int dim3);
     AmgSymMatrix(5)  FillCovMatrix(int iTrk, std::vector<double> & Matrix);
     Amg::Vector3D findIniXY(const Trk::TrackParticleBaseCollection* newPrtCol);
     Amg::Vector3D findIniXY(const TrackCollection* newPrtCol);
//---------------------------------------------------------------------------
//  Arrays for propagation of fit results to steering procedure
//   
     std::vector<double **> m_savedTrkFittedPerigees;   
     std::vector< std::vector< AmgSymMatrix(5) > > m_fittedTrkCov; 


   };

} // end of namespace bracket

#endif
