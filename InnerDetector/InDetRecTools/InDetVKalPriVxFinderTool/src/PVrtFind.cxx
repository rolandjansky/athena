/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "InDetVKalPriVxFinderTool/InDetVKalPriVxFinderTool.h"
//-------------------------------------------------
// Other stuff
#include<iostream>

 
//
//--------------------------------------------------------
//   Routine for global secondary vertex fitting
//
namespace InDet {

  int InDetVKalPriVxFinderTool::PVrtListFind(std::vector<const Trk::TrackParticleBase*>   & ListParticles,
                                             std::vector<const Trk::Track*>               & ListTracks,
	                                     std::vector< Amg::Vector3D >                 & PVrtList,
                                             std::vector< AmgSymMatrix(3) >               & ErrorMatrixPerVrt,
			           	     std::vector<double>                          & Chi2PerVrt,
			           	     std::vector<double>                          & ControlVariablePerVrt,
			            	     std::vector< std::vector<const Trk::TrackParticleBase*> > & PrtPerVrt,
			            	     std::vector< std::vector<const Trk::Track*> >             & TrkPerVrt,
			           	     std::vector< std::vector<double> >                & TrkWgtPerVrt )
  {
     if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Primary Vertex Finder called " <<endmsg;

//  Select track type for vertex finding.

     int Selector=2;   // Initial choice
     int NTracksVrt = 0;
     if( ListParticles.size() == 0 && ListTracks.size() == 0 ){return 0; }
     if( ListParticles.size() != 0 && ListTracks.size() == 0 ){ Selector =1; NTracksVrt=ListParticles.size(); }
     if( ListParticles.size() == 0 && ListTracks.size() != 0 ){ Selector =2; NTracksVrt=ListTracks.size();}
     if( NTracksVrt == 0) return 0;
     if( NTracksVrt == 1 && !m_BeamConstraint) return 0;
     if(Selector==1 && msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << " Trk::TrackParticlesBase are used for vertex search!!!="<<NTracksVrt<<endmsg;
     if(Selector==2 && msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << " Trk::Tracks are used for vertex search!!!="<<NTracksVrt<<endmsg;
//
     int NFoundVertices = 0;
     std::vector<const Trk::TrackParticleBase*> SelectedParticles;
     std::vector<const Trk::Track*>             SelectedTracks;

     PVrtList.clear();ErrorMatrixPerVrt.clear();Chi2PerVrt.clear();ControlVariablePerVrt.clear();
     TrkPerVrt.clear(); PrtPerVrt.clear(); TrkWgtPerVrt.clear();
//
// Fit preparation
//
     double ZStartFit=0., ControlVariable, Chi2 ;
     std::vector<double>  VertexErrorMatrix;
     std::vector<double>  TrkWeights;
     AmgSymMatrix(3)     CovMtx;
     Amg::Vector3D FitVertex(0.,0.,0.);

//
// Start of fit
//

     while ( 1 ) {
       if(Selector==1) ZStartFit = FindZPosTrk( ListParticles, ControlVariable);
       if(Selector==2) ZStartFit = FindZPosTrk( ListTracks,    ControlVariable);
       if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< " Z pos estimation= " <<ZStartFit<<" for vertex="<<NFoundVertices<< endmsg;
       if(ZStartFit < -10000.) return NFoundVertices;                  // No more candidates
       if(NFoundVertices > m_NPVertexMax) return NFoundVertices;         // Too many candidates
       Amg::Vector3D IniVertex(m_BeamCnst[0], m_BeamCnst[1], ZStartFit);
//
//  track selection for given vertex candidates
//
       inpSelector(ListParticles,ListTracks,IniVertex,SelectedParticles,SelectedTracks);
//
       if(Selector==1){
           if( SelectedParticles.size() == 0)  break;      // 0 tracks selected for fit. Break.
           if( SelectedParticles.size() < 2 && NFoundVertices>0 )  break;   // One vertex is found already. Break.
           if( SelectedParticles.size() == 1 && (!m_BeamConstraint)) break; // 1 track selected for fit. Break.
//VK now after fit     RemoveUsedEntry(ListParticles,SelectedParticles);
       }
       if(Selector==2){
           if( SelectedTracks.size() == 0)  break;      // 0 tracks selected for fit. Break.
           if( SelectedTracks.size() < 2 && NFoundVertices>0 )      break; // One vertex is found already. Break.
           if( SelectedTracks.size() == 1 && (!m_BeamConstraint) )  break; // 1 track selected for fit. Break.
//VK now after fit     RemoveUsedEntry(ListTracks,SelectedTracks);
       }

// Now common vertex fit. Also saves results for current vertex, so each vertex may be fitted only ONCE!!!

       Chi2 = FitCommonVrt(SelectedParticles,SelectedTracks,ZStartFit,FitVertex,VertexErrorMatrix,TrkWeights);
//       if( Chi2 < 0.) break;    //Bad fit. Break vertex search  !!VK 6.02.2008 Wrong!!! 
       if( Chi2 > 0. ){
         int NTrkInVrt=0;
         if(Selector==1)NTrkInVrt=SelectedParticles.size();
         if(Selector==2)NTrkInVrt=SelectedTracks.size();
         if(fabs(FitVertex.z()-IniVertex.z())>m_ZDistSelCut/2. && NTrkInVrt>4){ 
	    ZStartFit=FitVertex.z(); 
	    IniVertex<<FitVertex.x(),FitVertex.y(),FitVertex.z();
            int lastVrt=m_savedTrkFittedPerigees.size()-1;
            removeWorkArr2(m_savedTrkFittedPerigees[lastVrt],NTrkInVrt,3);     // Remove previous fit results
            m_savedTrkFittedPerigees.pop_back();    m_fittedTrkCov.pop_back(); // Remove previous fit results!!!See above
            inpSelector(ListParticles,ListTracks,IniVertex,SelectedParticles,SelectedTracks);
            Chi2 = FitCommonVrt(SelectedParticles,SelectedTracks,ZStartFit,FitVertex,VertexErrorMatrix,TrkWeights);
	    if(Chi2 > 0){
              if(Selector==1)NTrkInVrt=SelectedParticles.size();
              if(Selector==2)NTrkInVrt=SelectedTracks.size();
            }else{ NTrkInVrt=0; }
	 }
         if( NTrkInVrt && (NTrkInVrt>1 || NFoundVertices==0) ){ 
           NFoundVertices++;
           Chi2PerVrt.push_back(Chi2);
           ControlVariablePerVrt.push_back(ControlVariable);
           PVrtList.push_back(FitVertex);
           if(Selector==1)PrtPerVrt.push_back(SelectedParticles);
           if(Selector==2)TrkPerVrt.push_back(SelectedTracks);
           TrkWgtPerVrt.push_back(TrkWeights);
//
           CovMtx(0,0)=               VertexErrorMatrix[0];
           CovMtx(1,0)= CovMtx(0,1) = VertexErrorMatrix[1];
           CovMtx(1,1)=               VertexErrorMatrix[2];
           CovMtx(2,0)= CovMtx(0,2) = VertexErrorMatrix[3];
           CovMtx(2,1)= CovMtx(1,2) = VertexErrorMatrix[4];
           CovMtx(2,2)=               VertexErrorMatrix[5];
           ErrorMatrixPerVrt.push_back( CovMtx );
         }
       }
//
//  VK now only tracks attached to vertex are excluded (with weight > m_WeightCut)

       if(Selector==1){ RemoveUsedEntry(ListParticles,SelectedParticles,TrkWeights);}
       if(Selector==2){ RemoveUsedEntry(ListTracks,SelectedTracks,TrkWeights);}
       if(Selector==1 && ListParticles.size() < 2)  break;      // Less than 2 tracks left
       if(Selector==2 && ListTracks.size()    < 2)  break;      // Less than 2 tracks left

     }         // Next beam vertex

     return NFoundVertices;
   }



}
