/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include  "InDetVKalPriVxFinderTool/InDetVKalPriVxFinderTool.h"
#include  "TrkToolInterfaces/ITrackSummaryTool.h"
#include  "TrkTrackSummary/TrackSummary.h"
//-------------------------------------------------
// Other stuff
#include <map>
//#include <iostream>



               /*  Technicalities */


namespace InDet {


   void InDetVKalPriVxFinderTool::inpSelector(std::vector<const Trk::TrackParticleBase*>   & ListParticles,
                                              std::vector<const Trk::Track*>               & ListTracks,
	                                      Amg::Vector3D                                   & IniVertex,
                                              std::vector<const Trk::TrackParticleBase*>   & SelectedParticles,
                                              std::vector<const Trk::Track*>               & SelectedTracks)
   {
     int Selector=2;   // Initial choice
     int NTrkInList = 0;
     SelectedParticles.clear(); SelectedTracks.clear();
     if( ListParticles.size() == 0 && ListTracks.size() == 0 )return;
     if( ListParticles.size() != 0 && ListTracks.size() == 0 ){ Selector =1; NTrkInList=ListParticles.size(); }
     if( ListParticles.size() == 0 && ListTracks.size() != 0 ){ Selector =2; NTrkInList=ListTracks.size();}
//
//  track selection for given vertex candidates
     std::vector<double>  Impact, ImpactError;
     double Signif=0.;
     for(int i=0; i < NTrkInList; i++) {
	  if(Selector==1)Signif=m_fitSvc->VKalGetImpact( ListParticles[i], IniVertex, 1, Impact, ImpactError);
	  if(Selector==2)Signif=m_fitSvc->VKalGetImpact( ListTracks[i],    IniVertex, 1, Impact, ImpactError);
	  if ( fabs(Impact[0])/sqrt(ImpactError[0])  > m_RImpSelCut)   continue;
	  if ( fabs(Impact[1])/sqrt(ImpactError[2])  > m_ZImpSelCut)   continue;
	  if (  Signif                               > m_SignifSelCut) continue;
	  if ( fabs(Impact[0])                 > m_RDistSelCut)       continue;
	  if ( fabs(Impact[1]*sin(Impact[2]))  > m_ZDistSelCut)       continue;
	  if(Selector==1)SelectedParticles.push_back( ListParticles[i] );
	  if(Selector==2)SelectedTracks.push_back( ListTracks[i] );
      }
      if(Selector==1 && msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Chosen for vertex fit="<<SelectedParticles.size()<<endmsg;
      if(Selector==2 && msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Chosen for vertex fit="<<SelectedTracks.size()<<endmsg;
      return;
   }




   int InDetVKalPriVxFinderTool::FindMin( std::vector<double>& Chi2PerTrk)
   { 
      double Chi2Ref=1.e12;
      int Position=0;
      if( Chi2PerTrk.size() < 1 ) return Position ;
      for (int i=0; i< (int)Chi2PerTrk.size(); i++){
         if( Chi2PerTrk[i] < Chi2Ref) { Chi2Ref=Chi2PerTrk[i]; Position=i;}
      }
      return Position;
   }      
   int InDetVKalPriVxFinderTool::FindMax( std::vector<double>& Chi2PerTrk)
   { 
      double Chi2Ref=0.;
      int Position=0;
      if( Chi2PerTrk.size() < 1 ) return Position ;
      for (int i=0; i< (int)Chi2PerTrk.size(); i++){
         if( Chi2PerTrk[i] > Chi2Ref) { Chi2Ref=Chi2PerTrk[i]; Position=i;}
      }
      return Position;
   }      
   int InDetVKalPriVxFinderTool::FindMaxSecond( std::vector<double>& Chi2PerTrk)
   { 
      double Chi2Ref=0.,Chi2RefS=0.;
      int Position=0;
      if( Chi2PerTrk.size() < 1 ) return Position ;
      for (int i=0; i< (int)Chi2PerTrk.size(); i++){
         if( Chi2PerTrk[i] > Chi2Ref) { Chi2Ref=Chi2PerTrk[i]; Position=i;}
      }
      for (int i=0; i< (int)Chi2PerTrk.size(); i++){
         if( Chi2PerTrk[i] == Chi2Ref ) continue;
         if( Chi2PerTrk[i] > Chi2RefS ) { Chi2RefS=Chi2PerTrk[i]; Position=i;}
      }
      return Position;
   }      
  
 


  const Trk::Perigee* InDetVKalPriVxFinderTool::GetPerigee( const Trk::TrackParticleBase* i_ntrk) 
  {
//
//-- Perigee in TrackParticle
//
//     return i_ntrk->Perigee();
     const Trk::Perigee* mPer;
     mPer = dynamic_cast<const Trk::Perigee*>( &(i_ntrk->definingParameters()) );
     return mPer;
  }

  const Trk::Perigee* InDetVKalPriVxFinderTool::GetPerigee( const Trk::Track* i_ntrk) 
  {
     return i_ntrk->perigeeParameters();
//
//-- Perigee in Trk::Track
//
//     const Trk::Perigee* mPer=NULL;
//     const DataVector<const Trk::TrackParameters>* AllTrkPar;
//     DataVector<const Trk::TrackParameters>::const_iterator i_apar;
//     AllTrkPar = i_ntrk->trackParameters();
//     i_apar=AllTrkPar->begin();
//     while( i_apar < AllTrkPar->end() ){
//       if( (mPer=dynamic_cast<const Trk::Perigee*>(*i_apar)) == 0 ){ i_apar++;}
//       else { break;}
//     }
//     return mPer;
  }
//-----------------------------------------------------------------------------------------------
  void InDetVKalPriVxFinderTool::RemoveEntryInList(std::vector<const Trk::Track*>& ListTracks, int Outlier)
  {
    if(Outlier < 0 ) return;
    if(Outlier >= (int)ListTracks.size() ) return;
    std::vector<const Trk::Track*>::iterator   TransfEnd;
    TransfEnd = remove( ListTracks.begin(), ListTracks.end(), ListTracks[Outlier]);
    ListTracks.erase( TransfEnd,ListTracks.end());
  }     

  void InDetVKalPriVxFinderTool::RemoveEntryInList(std::vector<const Trk::TrackParticleBase*>& ListTracks, int Outlier)
  {
    if(Outlier < 0 ) return;
    if(Outlier >= (int)ListTracks.size() ) return;
    std::vector<const Trk::TrackParticleBase*>::iterator   TransfEnd;
    TransfEnd = remove( ListTracks.begin(), ListTracks.end(), ListTracks[Outlier]);
    ListTracks.erase( TransfEnd,ListTracks.end());
  }     

  void InDetVKalPriVxFinderTool::RemoveEntryInList(std::vector<double>& List, int Outlier)
  {
    if(Outlier < 0 ) return;
    if(Outlier >= (int)List.size() ) return;
    std::vector<double>::iterator   TransfEnd;
    TransfEnd = remove( List.begin(), List.end(), List[Outlier]);
    List.erase( TransfEnd,List.end());
  }     
//-----------------------------------------------------------------------------------------------

  void InDetVKalPriVxFinderTool::RemoveUsedEntry(std::vector<const Trk::Track*>& List, 
                                       std::vector<const Trk::Track*>& ListUsed,
				       std::vector<double> & TrkWeights)
  {
    int UseWgt=0;
    if(ListUsed.size()   == 0 ) return;
    std::vector<const Trk::Track*>::iterator   TransfEnd = List.end();
    int NUsedTrk=ListUsed.size();
    if( (int)TrkWeights.size()>= NUsedTrk) UseWgt=1;
    for (int i=0; i < NUsedTrk; i++) {
       if(UseWgt && (TrkWeights[i]<m_WeightCut) && NUsedTrk>2 ) continue;    // Only well attached tracks are removed
       TransfEnd=remove(List.begin() , TransfEnd , ListUsed[i]); 
    }
    List.erase( TransfEnd,List.end());
  }

  void InDetVKalPriVxFinderTool::RemoveUsedEntry(std::vector<const Trk::TrackParticleBase*>& List, 
                                       std::vector<const Trk::TrackParticleBase*>& ListUsed,
				       std::vector<double> & TrkWeights)
  {
    int UseWgt=0;
    if(ListUsed.size() == 0 ) return;
    std::vector<const Trk::TrackParticleBase*>::iterator   TransfEnd = List.end();
    int NUsedTrk=ListUsed.size();
    if( (int)TrkWeights.size()>= NUsedTrk) UseWgt=1;
    for (int i=0; i < NUsedTrk; i++) {
       if(UseWgt && (TrkWeights[i]<m_WeightCut) && NUsedTrk>2 ) continue;    // Only well attached tracks are removed
       TransfEnd=remove(List.begin() , TransfEnd , ListUsed[i]); 
    }
    List.erase( TransfEnd,List.end());
  }







  double InDetVKalPriVxFinderTool::GetLimitAngle(double Phi){
  while ( Phi < 0.) { Phi += 2.*3.1415926536;}
  while ( Phi > 2.*3.1415926536) {Phi -=2.*3.1415926536;}
  return Phi;
  }

//  Track list cleaning


  void InDetVKalPriVxFinderTool::UniqList(std::vector<const Trk::Track*> & List)
  {
      std::vector<const Trk::Track*>::iterator   TransfEnd ;
      sort(List.begin(),List.end());
      TransfEnd =  unique(List.begin(),List.end());
      List.erase( TransfEnd, List.end());
  }
  void InDetVKalPriVxFinderTool::UniqList(std::vector<const Trk::TrackParticleBase*> & List)
  {
      std::vector<const Trk::TrackParticleBase*>::iterator   TransfEnd ;
      sort(List.begin(),List.end());
      TransfEnd =  unique(List.begin(),List.end());
      List.erase( TransfEnd, List.end());
  }


/*   const vector<const Trk::TrackParticleBase*> 
       InDetVKalPriVxFinderTool::Base(const vector<const Rec::TrackParticle*> & listPart)
   {
     vector <const Trk::TrackParticleBase*> listBase;
     for(int i=0; i<(int)listPart.size(); i++) {
        listBase.push_back( (const Trk::TrackParticleBase*)listPart[i]); 
     }
     return listBase;
    }

   const Trk::TrackParticleBase* InDetVKalPriVxFinderTool::Base(const Rec::TrackParticle* Part)
   {
       return (const Trk::TrackParticleBase*) Part;
    }
*/

  double** InDetVKalPriVxFinderTool::getWorkArr2(long int dim1,long int dim2)
  { 
     double **ppArr = new double*[dim1];
     for (int i = 0; i < dim1; i++) ppArr[i] = new double[dim2];
     return ppArr;
  }

  void InDetVKalPriVxFinderTool::removeWorkArr2(double **ppArr, long int dim1,long int )
  {  
     for (int i = 0; i < dim1; i++) delete[] ppArr[i];
     delete[] ppArr;
  }

  double*** InDetVKalPriVxFinderTool::getWorkArr3(long int dim1,long int dim2,long int dim3)
  { 
     int i,j; 
     double ***ppArr = new double**[dim1];
     for (i = 0; i < dim1; i++) ppArr[i] = new double*[dim2];
     for (i = 0; i < dim1; i++) { for (j = 0; j < dim2; j++) ppArr[i][j] = new double[dim3];}
     return ppArr;
  }

  void InDetVKalPriVxFinderTool::removeWorkArr3(double ***ppArr, long int dim1,long int dim2,long int )
  {  
     int i,j; 
     for (i = 0; i < dim1; i++){ for (j = 0; j < dim2; j++) delete[]ppArr[i][j]; }
     for (i = 0; i < dim1; i++) delete[]ppArr[i];
     delete[] ppArr;
  }


// Fills 5x5 matrix.  Input Matrix is a full covariance
  AmgSymMatrix(5)  InDetVKalPriVxFinderTool::FillCovMatrix(int iTrk, std::vector<double> & Matrix)
 {
    int iTmp=(iTrk+1)*3;
    int NContent = Matrix.size();
    AmgSymMatrix(5) CovMtx; CovMtx.setZero(); CovMtx(2,2)=1.; CovMtx(3,3)=1.; CovMtx(4,4)=1.;
    int pnt = (iTmp+1)*iTmp/2 + iTmp;   if( pnt   > NContent ) return CovMtx;
    CovMtx(2,2) =  Matrix[pnt];
    pnt = (iTmp+1+1)*(iTmp+1)/2 + iTmp; if( pnt+1 > NContent ){ return CovMtx; }
    CovMtx(2,3) =  Matrix[pnt];
    CovMtx(3,3) =  Matrix[pnt+1];
    pnt = (iTmp+2+1)*(iTmp+2)/2 + iTmp; if( pnt+2 > NContent ){ return CovMtx; }
    CovMtx(2,4) = Matrix[pnt];
    CovMtx(3,4) = Matrix[pnt+1];
    CovMtx(4,4) = Matrix[pnt+2];
    return CovMtx;
  }



  Amg::Vector3D InDetVKalPriVxFinderTool::findIniXY(const TrackCollection* trackTES)
  {
    //.............................................
    AmgVector(5) VectPerig; VectPerig<<0.,0.,0.,0.,0.;
    const Trk::Perigee* mPer=NULL;
    std::map<double, const Trk::Track*>  mapTracks;
    std::map<double, const Trk::Track*>::reverse_iterator  map_i;
//
    const DataVector<Trk::Track>*    newTrkCol = trackTES;
    DataVector<Trk::Track>::const_iterator    i_ntrk;
    for (i_ntrk = newTrkCol->begin(); i_ntrk < newTrkCol->end(); ++i_ntrk) {
       mPer=GetPerigee( (*i_ntrk) ); if( mPer == NULL )continue; 
       VectPerig = mPer->parameters(); 
       double pmom = sin(VectPerig[3])/fabs(VectPerig[4]);
//----------------------------------- Summary tools
       if(m_SummaryToolExist) {
          const Trk::TrackSummary* testSum = m_sumSvc->createSummary(*(*i_ntrk));
          if( testSum->get(Trk::numberOfInnermostPixelLayerHits) <= 0) continue;
       }
       mapTracks.insert( std::pair<double, const Trk::Track*>(pmom,(*i_ntrk)));
    }

    std::vector<const Trk::Track*>  SelectedTracks;

    int cnt=0;
    for(map_i=mapTracks.rbegin(); map_i != mapTracks.rend(); map_i++){
      SelectedTracks.push_back((*map_i).second);
      if(++cnt>20)break;
    }
    Amg::Vector3D Vertex(0.,0.,0.);
    if(cnt>=2){
      StatusCode sc=m_fitSvc->VKalVrtFitFast(SelectedTracks,Vertex);
      if(sc.isFailure())Vertex<<0.,0.,0.;
    }
    return Vertex;
  } 



  Amg::Vector3D InDetVKalPriVxFinderTool::findIniXY(const Trk::TrackParticleBaseCollection* newPrtCol)
  {
    //.............................................
    AmgVector(5) VectPerig; VectPerig<<0.,0.,0.,0.,0.;
    const Trk::Perigee* mPer=NULL;
    std::map<double, const Trk::TrackParticleBase*>  mapTracks;
    std::map<double, const Trk::TrackParticleBase*>::reverse_iterator  map_i;
//
    Trk::TrackParticleBaseCollection::const_iterator i_nprt  = newPrtCol->begin();
    for (i_nprt = newPrtCol->begin(); i_nprt < newPrtCol->end(); ++i_nprt) {
       mPer=GetPerigee( (*i_nprt) ); if( mPer == NULL )continue; 
       VectPerig = mPer->parameters(); 
       double pmom = sin(VectPerig[3])/fabs(VectPerig[4]);
//----------------------------------- Summary tools
       const Trk::TrackSummary* testSum = (*i_nprt)->trackSummary();
       if( testSum->get(Trk::numberOfInnermostPixelLayerHits) <=0 ) continue;
       mapTracks.insert( std::pair<double, const Trk::TrackParticleBase*>(pmom,(*i_nprt)));
    }

    std::vector<const Trk::TrackParticleBase*>  SelectedTrackParticles;

    int cnt=0;
    for(map_i=mapTracks.rbegin(); map_i != mapTracks.rend(); map_i++){
      //std::cout<<(*map_i).first<<'\n';
      SelectedTrackParticles.push_back((*map_i).second);
      if(++cnt>20)break;
    }
    Amg::Vector3D Vertex(0.,0.,0.);
    if(cnt>=2){
      StatusCode sc= m_fitSvc->VKalVrtFitFast(SelectedTrackParticles,Vertex);
      if(sc.isFailure())Vertex<<0.,0.,0.;
    }
    return Vertex;
  } 


}
