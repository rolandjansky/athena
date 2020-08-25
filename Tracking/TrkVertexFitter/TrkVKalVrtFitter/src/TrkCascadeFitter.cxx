/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrkVKalVrtFitter/VKalVrtAtlas.h"
#include "TrkVKalVrtFitter/VxCascadeInfo.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "VxVertex/ExtendedVxCandidate.h"
#include "TrkVKalVrtCore/TrkVKalUtils.h"
#include "TrkVKalVrtCore/TrkVKalVrtCoreBase.h"
//-------------------------------------------------
// Other stuff
#include<iostream>

namespace Trk {
    extern int makeCascade(VKalVrtControl & FitCONTROL, long int NTRK, long int *ich, double *wm, double *inp_Trk5, double *inp_CovTrk5,
                   const std::vector< std::vector<int> > &vertexDefinition,
                   const std::vector< std::vector<int> > &cascadeDefinition,
		   double definedCnstAccuracy=1.e-4);
    extern int processCascade(CascadeEvent &);
    extern int processCascade(CascadeEvent &, double *);
    extern int processCascade(CascadeEvent &, double *, double *);
    extern int processCascadePV(CascadeEvent &, double *, double *);
    extern void getFittedCascade( CascadeEvent &,
                                  std::vector< Vect3DF > &, 
		                  std::vector<std::vector<double> >& ,
                                  std::vector< std::vector< VectMOM> > & ,
		                  std::vector< std::vector<double> > & ,
		                  std::vector<double> & ,
		                  std::vector<double> & );
    extern int setCascadeMassConstraint(CascadeEvent &, long int IV, double Mass);
    extern int setCascadeMassConstraint(CascadeEvent &, long int IV, std::vector<int> &, std::vector<int> &, double Mass);



   /** Interface for  cascade fit*/
//-----------------------------------------------------------------------------------------
// First vertex in cascade
//
//
//   Main structure description objects
//
//  cascadeV -  structure with cascade vertex description. It contains
//               vID                            - VertexID of the vertex
//               vector<int>      trkInVrt      - used tracks
//               vector<VertexID> inPointingV   - used vertices (pseudo-tracks from them really) or vertices pointing to the current one. 
//               VertexID         outPointingV  - vertex to which the current vertex points
//
//  PartialMassConstraint - structure for mass constraint description. It contains
//               VRT         -  VertexID of corresponding vertex
//               trkInVrt    -  list of real tracks. It's vector<int> of indices to m_partListForCascade
//               pseudoInVrt -  list of VertexID of the participating vertices
//
//  vector<cascadeV> m_cascadeVList  - main cascade defining vector with cascade vertices. Filled by cascade interface.
//                                     Should not be touched during fit.
//
//  vector< TrackParticleBase* >    m_partListForCascade      - list of all REAL tracks used in cascade
//  vector< double >                m_partMassForCascade      - their masses
//  vector< PartialMassConstraint > m_partMassCnstForCascade  - list of all mass constraints in cascade
//
//
//---------- makeSimpleCascade transforms the full cascade definition m_cascadeVList into simplified structure with merging---------
//----------   SimpleCascade may have LESS vertices than Cascade itself due to merging                                     ---------
//----------   SimpleCascade is defined by vertexDefinition  and cascadeDefinition vectors.                            ---------
//----------   Their sizes are SimpleCascade sise.                                                                         ---------
//
//  vector< vector <int> > vertexDefinition   -  list of pointers to REAL tracks (in m_partListForConstraints ) for SimpleCascade
//  vector< vector <int> > cascadeDefinition  -  simple vertices pointing to the current one. Contains indices (integer) to itself!  
//
//
//----------------------------------------------------------------------------------------

VertexID TrkVKalVrtFitter::startVertex(const  std::vector<const xAOD::TrackParticle*> & list,
                                       const  std::vector<double>& particleMass,
                                       IVKalState& istate,
	  			       const  double massConstraint) const
{
    assert(dynamic_cast<State*> (&istate)!=nullptr);
    State& state = static_cast<State&> (istate);
    state.m_cascadeState = std::make_unique<CascadeState>();
    state.m_vkalFitControl.renewCascadeEvent(new CascadeEvent());

    return nextVertex (list, particleMass, istate, massConstraint);
}


//
// Calculate total number of degrees of freedom for cascade WITHOUT pointing to primary vertex
//
int TrkVKalVrtFitter::getCascadeNDoF (const CascadeState& cstate) const 
{

// get Tracks, Vertices and Pointings in cascade
//
  int nTrack    = cstate.m_partListForCascade.size();
  int nVertex   = cstate.m_cascadeVList.size();

  int nPointing = 0;
  for( int iv=0; iv<nVertex; iv++) nPointing += cstate.m_cascadeVList[iv].inPointingV.size();

  int nMassCnst = cstate.m_partMassCnstForCascade.size();  //  mass cnsts

  return 2*nTrack - 3*nVertex + 2*nPointing + nMassCnst;
}
//
// Next vertex in cascade
//
VertexID TrkVKalVrtFitter::nextVertex(const  std::vector<const xAOD::TrackParticle*> & list,
                                      const  std::vector<double>& particleMass,
                                      IVKalState& istate,
	  		              const  double massConstraint) const
{
    assert(dynamic_cast<State*> (&istate)!=nullptr);
    State& state = static_cast<State&> (istate);
    CascadeState& cstate = *state.m_cascadeState;

//----
    int NV = cstate.m_cascadeSize++;
    VertexID new_vID=10000+NV;
//----
    int NTRK = list.size();
    int presentNT = cstate.m_partListForCascade.size();
//----

    double totMass=0;
    for(int it=0; it<NTRK; it++){
       cstate.m_partListForCascade.push_back(list[it]);
       cstate.m_partMassForCascade.push_back(particleMass[it]);
       totMass += particleMass[it];
    }
//---------------------- Fill complete vertex mass constraint
    if(totMass < massConstraint) {
      PartialMassConstraint  tmpMcnst;
      tmpMcnst.Mass     =  massConstraint;
      tmpMcnst.VRT      =   new_vID;
      for(int it=0; it<NTRK; it++)tmpMcnst.trkInVrt.push_back(it+presentNT);
      cstate.m_partMassCnstForCascade.push_back(std::move(tmpMcnst));
    }
//
//
//--   New vertex structure-----------------------------------
    cascadeV newV; newV.vID=new_vID;
    for(int it=0; it<NTRK; it++){
       newV.trkInVrt.push_back(it+presentNT);
    }
    cstate.m_cascadeVList.push_back(std::move(newV));
//--------------------------------------------------------------  
    return new_vID;
}


//
// Next vertex in cascade
//
VertexID TrkVKalVrtFitter::nextVertex(const  std::vector<const xAOD::TrackParticle*> & list,
                                      const  std::vector<double>& particleMass,
		                      const  std::vector<VertexID> &precedingVertices,
                                      IVKalState& istate,
	  		              const  double massConstraint) const
{
    assert(dynamic_cast<State*> (&istate)!=nullptr);
    State& state = static_cast<State&> (istate);
    CascadeState& cstate = *state.m_cascadeState;

    VertexID vID=nextVertex( list, particleMass, istate, massConstraint);
//
    int lastC=cstate.m_partMassCnstForCascade.size()-1;   // Check if full vertex mass constraint exist
    if( lastC>=0 ){    if(  cstate.m_partMassCnstForCascade[lastC].VRT == vID ){ 
                         for(int iv=0; iv<(int)precedingVertices.size(); iv++){
                            cstate.m_partMassCnstForCascade[lastC].pseudoInVrt.push_back(precedingVertices[iv]); }
                       }
    }
//
//--   New vertex structure-----------------------------------
    int lastV=cstate.m_cascadeVList.size()-1;
    for(int iv=0; iv<(int)precedingVertices.size(); iv++){
       cstate.m_cascadeVList[lastV].inPointingV.push_back(precedingVertices[iv]); // fill preceding vertices list
    }
//--
    return vID;     
}


//--------------------------------------------------------------------------------
//   Convert complex (with vertex merging) structure into simple one for fitter
//     vrtDef[iv][it]      -  list of real tracks in vertex IV in cascade
//     cascadeDef[iv][ipv] -  list of previous vertices pointing to vertex IV in cascade
//
//   Vectors are filled with the indices (NOT VertexID!!!)
//------------------------
void TrkVKalVrtFitter::makeSimpleCascade(std::vector< std::vector<int> > & vrtDef,
                                         std::vector< std::vector<int> > & cascadeDef,
                                         CascadeState& cstate) const
{
    int iv,ip,it, nVAdd, iva;
    vrtDef.clear();
    cascadeDef.clear();
    int NVC=cstate.m_cascadeVList.size();
    vrtDef.resize(NVC);
    cascadeDef.resize(NVC);
//
//----  First set up position of each vertex in simple structure with merging(!!!)
//
    int vCounter=0;
    for(iv=0; iv<NVC; iv++){
       cascadeV &vrt=cstate.m_cascadeVList[iv];
       vrt.indexInSimpleCascade=-1;              // set to -1 for merged vertices not present in simple list
       if(vrt.mergedTO) continue;                // vertex is merged with another one;
       vrt.indexInSimpleCascade=vCounter;        // vertex position in simple cascade structure
       vCounter++;
    }
//----  Fill vertices in simple structure
    vCounter=0;
    for(iv=0; iv<NVC; iv++){
       const cascadeV &vrt=cstate.m_cascadeVList[iv];
       if(vrt.mergedTO) continue;                                // vertex is merged with another one;
       for(it=0; it<(int)vrt.trkInVrt.size(); it++) vrtDef[vCounter].push_back(vrt.trkInVrt[it]);       //copy real tracks
       for(ip=0; ip<(int)vrt.inPointingV.size(); ip++) {
          //int indInFull=vrt.inPointingV[ip];                                 // pointing vertex in full list  WRONG!!!
          int indInFull=indexInV(vrt.inPointingV[ip], cstate);                         // pointing vertex in full list
          int indInSimple=cstate.m_cascadeVList[indInFull].indexInSimpleCascade;      // its index in simple structure
	  if(indInSimple<0) continue;                            // merged out vertex. Will be added as tracks
          cascadeDef[vCounter].push_back(indInSimple);
       }
       nVAdd=vrt.mergedIN.size();
       if( nVAdd ) {         //----------------------------- mergedIN(added) vertices exist
          for(iva=0; iva<nVAdd; iva++){
	     const cascadeV &vrtM=cstate.m_cascadeVList[vrt.mergedIN[iva]];    // merged/added vertex itself
             for(it=0; it<(int)vrtM.trkInVrt.size(); it++) vrtDef[vCounter].push_back(vrtM.trkInVrt[it]);
             for(ip=0; ip<(int)vrtM.inPointingV.size(); ip++) {
                //int indInFull=vrtM.inPointingV[ip];                                // pointing vertex in full list  WRONG!!!
                int indInFull=indexInV(vrtM.inPointingV[ip], cstate);                        // pointing vertex in full list
                int indInSimple=cstate.m_cascadeVList[indInFull].indexInSimpleCascade;      // its index in simple structure
	        if(indInSimple<0) continue;                            // merged out vertex. Will be added as tracks
	        cascadeDef[vCounter].push_back(indInSimple);
	     }
          }
       }

       vCounter++;
    }
    vrtDef.resize(vCounter);
    cascadeDef.resize(vCounter);
}
//--------------------------------------------------------------------------------
//   Printing of cascade structure
//
void TrkVKalVrtFitter::printSimpleCascade(std::vector< std::vector<int> > & vrtDef, 
                                          std::vector< std::vector<int> > & cascadeDef,
                                          const CascadeState& cstate) const
{
      int kk,kkk;
      for(kk=0; kk<(int)vrtDef.size(); kk++){
         std::cout<<" Vertex("<<kk<<"):: trk=";
         for(kkk=0; kkk<(int)vrtDef[kk].size(); kkk++){
	                 std::cout<<vrtDef[kk][kkk]<<", ";}  std::cout<<" pseu=";
         for(kkk=0; kkk<(int)cascadeDef[kk].size(); kkk++){
	                 std::cout<<cascadeDef[kk][kkk]<<", ";}  
      } std::cout<<'\n';
//---      
      for(kk=0; kk<(int)vrtDef.size(); kk++){
         std::cout<<" Vertex("<<kk<<"):: trkM=";
         for(kkk=0; kkk<(int)vrtDef[kk].size(); kkk++){
             std::cout<<cstate.m_partMassForCascade[vrtDef[kk][kkk]]<<", ";}  
      }std::cout<<'\n';
//--
      for (const PartialMassConstraint& c : cstate.m_partMassCnstForCascade) {
          std::cout<<" MCnst vID=";
          std::cout<<c.VRT<<" m="<<c.Mass<<" trk=";
          for(int idx : c.trkInVrt) {
             std::cout<<idx<<", ";
          }
          std::cout<<" pseudo=";
          for (VertexID id : c.pseudoInVrt) {
            std::cout<<id<<", ";
          }
          std::cout<<'\n';
      }
}


inline int SymIndex(int it, int i, int j) {  return (3*it+3+i)*(3*it+3+i+1)/2 + (3*it+3+j);}
#define CLEANCASCADE()  state.m_vkalFitControl.renewCascadeEvent(nullptr)

VxCascadeInfo * TrkVKalVrtFitter::fitCascade(IVKalState& istate,
                                             const Vertex* primVrt, bool FirstDecayAtPV ) const
{
    assert(dynamic_cast<State*> (&istate)!=nullptr);
    State& state = static_cast<State&> (istate);
    CascadeState& cstate = *state.m_cascadeState;

    int iv,it,jt;
    std::vector< Vect3DF >               cVertices;
    std::vector< std::vector<double> >   covVertices;
    std::vector< std::vector< VectMOM> > fittedParticles;
    std::vector< std::vector<double> >   fittedCovariance;
    std::vector<double>  fitFullCovariance;
    std::vector<double>  particleChi2;
//
    int ntrk=0;
    StatusCode sc; sc.setChecked();
    std::vector<const TrackParameters*> baseInpTrk;
    if(m_firstMeasuredPoint){               //First measured point strategy
       std::vector<const xAOD::TrackParticle*>::const_iterator   i_ntrk;
       //for (i_ntrk = cstate.m_partListForCascade.begin(); i_ntrk < cstate.m_partListForCascade.end(); ++i_ntrk) baseInpTrk.push_back(GetFirstPoint(*i_ntrk));
       unsigned int indexFMP;
       for (i_ntrk = cstate.m_partListForCascade.begin(); i_ntrk < cstate.m_partListForCascade.end(); ++i_ntrk) {
	  if ((*i_ntrk)->indexOfParameterAtPosition(indexFMP, xAOD::FirstMeasurement)){
            ATH_MSG_DEBUG("FirstMeasuredPoint on track is discovered. Use it.");
	    baseInpTrk.push_back(new CurvilinearParameters((*i_ntrk)->curvilinearParameters(indexFMP)));
          }else{
            ATH_MSG_DEBUG("No FirstMeasuredPoint on track in CascadeFitter. Stop fit");
            { CLEANCASCADE();  return nullptr; }
          }	      
       }
       sc=CvtTrackParameters(baseInpTrk,ntrk,state);
       if(sc.isFailure()){ntrk=0; sc=CvtTrackParticle(cstate.m_partListForCascade,ntrk,state);}
    }else{
       sc=CvtTrackParticle(cstate.m_partListForCascade,ntrk,state);
    }
    if(sc.isFailure()){ CLEANCASCADE(); return nullptr; }

    VKalVrtConfigureFitterCore(ntrk, state);

    std::vector< std::vector<int> >    vertexDefinition;             // track indices for vertex;
    std::vector< std::vector<int> >     cascadeDefinition;           // cascade structure
    makeSimpleCascade(vertexDefinition, cascadeDefinition, cstate);

    double * partMass=new double[ntrk];
    for(int i=0; i<ntrk; i++) partMass[i]  = cstate.m_partMassForCascade[i];
    int IERR = makeCascade(state.m_vkalFitControl, ntrk, state.m_ich, partMass, &state.m_apar[0][0], &state.m_awgt[0][0],
                            vertexDefinition,
                            cascadeDefinition,
			    m_cascadeCnstPrecision);  delete[] partMass; if(IERR){ CLEANCASCADE(); return nullptr;}
    CascadeEvent & refCascadeEvent=*(state.m_vkalFitControl.getCascadeEvent());
//
// Then set vertex mass constraints
//
    std::vector<int>  indexT,indexV,indexTT,indexVV,tmpInd;                  // track indices for vertex;
    for (const PartialMassConstraint& c : cstate.m_partMassCnstForCascade) {
      //int index=c.VRT;                // vertex position in simple structure
      int index=getSimpleVIndex(c.VRT, cstate);  // vertex position in simple structure
      IERR = findPositions(c.trkInVrt,    vertexDefinition[index],  indexT);  if(IERR)break;
      tmpInd.clear();
      for (int idx : c.pseudoInVrt)
        tmpInd.push_back( getSimpleVIndex(idx, cstate) );
      IERR = findPositions(             tmpInd,                      cascadeDefinition[index], indexV);  if(IERR)break;
    //IERR = findPositions(c.pseudoInVrt, cascadeDefinition[index], indexV);  if(IERR)break; //VK 31.10.2011 ERROR!!!
      IERR = setCascadeMassConstraint(refCascadeEvent,index, indexT, indexV, c.Mass);
      if(IERR)break;
    }
    if(IERR){ CLEANCASCADE(); return nullptr;}
    if(msgLvl(MSG::DEBUG)){
       msg(MSG::DEBUG)<<"Standard cascade fit" << endmsg;
       printSimpleCascade(vertexDefinition,cascadeDefinition, cstate);
    }
//
//     At last fit of cascade  
//   primVrt == 0            - no primary vertex
//   primVrt is <Vertex*>    - exact pointing to primary vertex
//   primVrt is <RecVertex*> - summary track pass near primary vertex
//
    if(primVrt){
       double vertex[3] = {primVrt->position().x()-state.m_refFrameX, primVrt->position().y()-state.m_refFrameY,primVrt->position().z()-state.m_refFrameZ};
       const RecVertex* primVrtRec=dynamic_cast< const RecVertex* > (primVrt);
       if(primVrtRec){
         double covari[6] = {primVrtRec->covariancePosition()(0,0),primVrtRec->covariancePosition()(0,1),
                             primVrtRec->covariancePosition()(1,1),primVrtRec->covariancePosition()(0,2),
                             primVrtRec->covariancePosition()(1,2),primVrtRec->covariancePosition()(2,2)};
	 if(FirstDecayAtPV) { IERR = processCascadePV(refCascadeEvent,vertex,covari);}
         else               { IERR = processCascade(refCascadeEvent,vertex,covari);}
       }else{
         IERR = processCascade(refCascadeEvent,vertex);
       }
    }else{
        IERR = processCascade(refCascadeEvent);
    }
    if(IERR){ CLEANCASCADE(); return nullptr;}
    getFittedCascade(refCascadeEvent, cVertices, covVertices, fittedParticles, fittedCovariance, particleChi2, fitFullCovariance );

//    for(int iv=0; iv<(int)cVertices.size(); iv++){       std::cout<<"iv="<<iv<<" masses=";
//       for(int it=0; it<(int)fittedParticles[iv].size(); it++){
//          double m=sqrt( fittedParticles[iv][it].E *fittedParticles[iv][it].E
//	                -fittedParticles[iv][it].Pz*fittedParticles[iv][it].Pz
//	                -fittedParticles[iv][it].Py*fittedParticles[iv][it].Py
//	                -fittedParticles[iv][it].Px*fittedParticles[iv][it].Px);
//          std::cout<<m<<", ";    }        std::cout<<'\n'; }
//-----------------------------------------------------------------------------
//
//  Check cascade correctness
//
    int ip,ivFrom=0,ivTo;
    double px,py,pz,Sign=10.;
    for( ivTo=0; ivTo<(int)vertexDefinition.size(); ivTo++){      //Vertex to check
       if(cascadeDefinition[ivTo].empty()) continue;          //no pointing to it
       for( ip=0; ip<(int)cascadeDefinition[ivTo].size(); ip++){
          ivFrom=cascadeDefinition[ivTo][ip];                     //pointing vertex
          px=py=pz=0;
          for(it=0; it<(int)fittedParticles[ivFrom].size(); it++){
	     px += fittedParticles[ivFrom][it].Px;
	     py += fittedParticles[ivFrom][it].Py;
	     pz += fittedParticles[ivFrom][it].Pz;
	  }
	  Sign=  (cVertices[ivFrom].X-cVertices[ivTo].X)*px
	        +(cVertices[ivFrom].Y-cVertices[ivTo].Y)*py
	        +(cVertices[ivFrom].Z-cVertices[ivTo].Z)*pz;
          if(Sign<0) break;
       }  
       if(Sign<0) break;
    }
//
//--------------- Wrong vertices in cascade precedence. Squeeze cascade and refit-----------
//
    int NDOFsqueezed=0;
    if(Sign<0.){
       int index,itmp;
       std::vector< std::vector<int> >  new_vertexDefinition;                         // track indices for vertex;
       std::vector< std::vector<int> >  new_cascadeDefinition;                        // cascade structure
       cstate.m_cascadeVList[ivFrom].mergedTO=cstate.m_cascadeVList[ivTo].vID;
       cstate.m_cascadeVList[ivTo].mergedIN.push_back(ivFrom);
       makeSimpleCascade(new_vertexDefinition, new_cascadeDefinition, cstate);
       if(msgLvl(MSG::DEBUG)){
          msg(MSG::DEBUG)<<"Compressed cascade fit" << endmsg;
          printSimpleCascade(new_vertexDefinition,new_cascadeDefinition, cstate);
       }
//-----------------------------------------------------------------------------------------
       state.m_vkalFitControl.renewCascadeEvent(new CascadeEvent());
       partMass=new double[ntrk];
       for(int i=0; i<ntrk; i++) partMass[i]  = cstate.m_partMassForCascade[i];
       int IERR = makeCascade(state.m_vkalFitControl, ntrk, state.m_ich, partMass, &state.m_apar[0][0], &state.m_awgt[0][0],
                               new_vertexDefinition,
                               new_cascadeDefinition);        delete[] partMass;  if(IERR){ CLEANCASCADE(); return nullptr;}
//------Set up mass constraints
       for (const PartialMassConstraint& c : cstate.m_partMassCnstForCascade) {
         indexT.clear(); indexV.clear();
         index=getSimpleVIndex( c.VRT, cstate);
         IERR = findPositions(c.trkInVrt,    new_vertexDefinition[index],  indexT);  if(IERR)break;
         for (VertexID inV : c.pseudoInVrt) {       //cycle over pseudotracks
	     int icv=indexInV(inV, cstate);  if(icv<0) break;
	     if(cstate.m_cascadeVList[icv].mergedTO == c.VRT){
	        IERR = findPositions(cstate.m_cascadeVList[icv].trkInVrt,    new_vertexDefinition[index],  indexTT);
	        if(IERR)break;
                indexT.insert (indexT.end(), indexTT.begin(), indexTT.end());
	     }else{
	        std::vector<int> tmpI(1); tmpI[0]=inV;
                IERR = findPositions(tmpI, new_cascadeDefinition[index], indexVV);
		if(IERR)break;
                indexV.insert (indexV.end(), indexVV.begin(), indexVV.end());
             }							 
         }   if(IERR)break;
         //std::cout<<"trk2="; for(int I=0; I<(int)indexT.size(); I++)std::cout<<indexT[I]; std::cout<<'\n';
         //std::cout<<"pse="; for(int I=0; I<(int)indexV.size(); I++)std::cout<<indexV[I]; std::cout<<'\n';
         IERR = setCascadeMassConstraint(*(state.m_vkalFitControl.getCascadeEvent()), index , indexT, indexV, c.Mass); if(IERR)break;
       }
       ATH_MSG_DEBUG("Setting compressed mass constraints ierr="<<IERR);
       if(IERR){ CLEANCASCADE(); return nullptr;}
//
//--------------------------- Refit
//
       if(primVrt){
          double vertex[3] = {primVrt->position().x()-state.m_refFrameX, primVrt->position().y()-state.m_refFrameY,primVrt->position().z()-state.m_refFrameZ};
          const RecVertex* primVrtRec=dynamic_cast< const RecVertex* > (primVrt);
          if(primVrtRec){
            double covari[6] = {primVrtRec->covariancePosition()(0,0),primVrtRec->covariancePosition()(0,1),
                                primVrtRec->covariancePosition()(1,1),primVrtRec->covariancePosition()(0,2),
                                primVrtRec->covariancePosition()(1,2),primVrtRec->covariancePosition()(2,2)};
            IERR = processCascade(*(state.m_vkalFitControl.getCascadeEvent()),vertex,covari);
          }else{
            IERR = processCascade(*(state.m_vkalFitControl.getCascadeEvent()),vertex);
          }
       }else{
          IERR = processCascade(*(state.m_vkalFitControl.getCascadeEvent()));
       }
       if(IERR){ CLEANCASCADE(); return nullptr;}
       NDOFsqueezed=getCascadeNDoF(cstate)+3-2;   // Remove vertex (+3 ndf) and this vertex pointing (-2 ndf)
//
//-------------------- Get information according to old cascade structure
//
       std::vector< Vect3DF >               t_cVertices;
       std::vector< std::vector<double> >   t_covVertices;
       std::vector< std::vector< VectMOM> > t_fittedParticles;
       std::vector< std::vector<double> >   t_fittedCovariance;
       std::vector<double>   t_fitFullCovariance;
       getFittedCascade(*(state.m_vkalFitControl.getCascadeEvent()), t_cVertices, t_covVertices, t_fittedParticles, 
                         t_fittedCovariance, particleChi2, t_fitFullCovariance);
       cVertices.clear(); covVertices.clear();
//
//------------------------- Real tracks
//
       if(msgLvl(MSG::DEBUG)){
         msg(MSG::DEBUG)<<"Initial cascade momenta"<<endmsg;
         for(int kv=0; kv<(int)fittedParticles.size(); kv++){
           for(int kt=0; kt<(int)fittedParticles[kv].size(); kt++)
             std::cout<<
	         " Px="<<fittedParticles[kv][kt].Px<<" Py="<<fittedParticles[kv][kt].Py<<";";
           std::cout<<'\n';
         }
         msg(MSG::DEBUG)<<"Squized cascade momenta"<<endmsg;
         for(int kv=0; kv<(int)t_fittedParticles.size(); kv++){
           for(int kt=0; kt<(int)t_fittedParticles[kv].size(); kt++)
             std::cout<<
	         " Px="<<t_fittedParticles[kv][kt].Px<<" Py="<<t_fittedParticles[kv][kt].Py<<";";
           std::cout<<'\n';
         }
       }
       for(iv=0; iv<(int)cstate.m_cascadeVList.size(); iv++){
         index=getSimpleVIndex( cstate.m_cascadeVList[iv].vID, cstate );               //index of vertex in simplified structure
         cVertices.push_back(t_cVertices[index]);
         covVertices.push_back(t_covVertices[index]);
         for(it=0; it<(int)cstate.m_cascadeVList[iv].trkInVrt.size(); it++){
	    int numTrk=cstate.m_cascadeVList[iv].trkInVrt[it];                 //track itself
            for(itmp=0; itmp<(int)new_vertexDefinition[index].size(); itmp++) if(numTrk==new_vertexDefinition[index][itmp])break;
            fittedParticles[iv][it]=t_fittedParticles[index][itmp];    
//Update only particle covariance. Cross particle covariance remains old.
            fittedCovariance[iv][SymIndex(it,0,0)]=t_fittedCovariance[index][SymIndex(itmp,0,0)];
            fittedCovariance[iv][SymIndex(it,1,0)]=t_fittedCovariance[index][SymIndex(itmp,1,0)];
            fittedCovariance[iv][SymIndex(it,1,1)]=t_fittedCovariance[index][SymIndex(itmp,1,1)];
            fittedCovariance[iv][SymIndex(it,2,0)]=t_fittedCovariance[index][SymIndex(itmp,2,0)];
            fittedCovariance[iv][SymIndex(it,2,1)]=t_fittedCovariance[index][SymIndex(itmp,2,1)];
            fittedCovariance[iv][SymIndex(it,2,2)]=t_fittedCovariance[index][SymIndex(itmp,2,2)];
         }
         fittedCovariance[iv][SymIndex(0,0,0)]=t_fittedCovariance[index][SymIndex(0,0,0)];    // Update also vertex 
         fittedCovariance[iv][SymIndex(0,1,0)]=t_fittedCovariance[index][SymIndex(0,1,0)];    // covarinace
         fittedCovariance[iv][SymIndex(0,1,1)]=t_fittedCovariance[index][SymIndex(0,1,1)];
         fittedCovariance[iv][SymIndex(0,2,0)]=t_fittedCovariance[index][SymIndex(0,2,0)];
         fittedCovariance[iv][SymIndex(0,2,1)]=t_fittedCovariance[index][SymIndex(0,2,1)];
         fittedCovariance[iv][SymIndex(0,2,2)]=t_fittedCovariance[index][SymIndex(0,2,2)];
       }
// Pseudo-tracks. They are filled based on fitted results for nonmerged vertices
//             or as sum for merged vertices
       VectMOM tmpMom;
       for(iv=0; iv<(int)cstate.m_cascadeVList.size(); iv++){
         index=getSimpleVIndex( cstate.m_cascadeVList[iv].vID, cstate );               //index of current vertex in simplified structure
         int NTrkInVrt=cstate.m_cascadeVList[iv].trkInVrt.size();
         for(ip=0; ip<(int)cstate.m_cascadeVList[iv].inPointingV.size(); ip++){    //inPointing verties
            int tmpIndexV=indexInV( cstate.m_cascadeVList[iv].inPointingV[ip], cstate);    //index of inPointing vertex in full structure
            if(cstate.m_cascadeVList[tmpIndexV].mergedTO){           //vertex is merged, so take pseudo-track as a sum
              tmpMom.Px=tmpMom.Py=tmpMom.Pz=tmpMom.E=0.;
              for(it=0; it<(int)(cstate.m_cascadeVList[tmpIndexV].trkInVrt.size()+
	                         cstate.m_cascadeVList[tmpIndexV].inPointingV.size()); it++){
                tmpMom.Px += fittedParticles[tmpIndexV][it].Px; tmpMom.Py += fittedParticles[tmpIndexV][it].Py;
                tmpMom.Pz += fittedParticles[tmpIndexV][it].Pz; tmpMom.E  += fittedParticles[tmpIndexV][it].E;
              }
              fittedParticles[iv][ip+NTrkInVrt]=tmpMom;	    
            }else{
              int indexS=getSimpleVIndex( cstate.m_cascadeVList[iv].inPointingV[ip], cstate );   //index of inPointing vertex in simplified structure
              for(itmp=0; itmp<(int)new_cascadeDefinition[index].size(); itmp++) if(indexS==new_cascadeDefinition[index][itmp])break;
              fittedParticles[iv][ip+NTrkInVrt]=t_fittedParticles[index][itmp+new_vertexDefinition[index].size()];	    
            }
         }
       }
       if(msgLvl(MSG::DEBUG)){
         msg(MSG::DEBUG)<<"Refit cascade momenta"<<endmsg;
          for(int kv=0; kv<(int)fittedParticles.size(); kv++){
            for(int kt=0; kt<(int)fittedParticles[kv].size(); kt++)
              std::cout<<
	          " Px="<<fittedParticles[kv][kt].Px<<" Py="<<fittedParticles[kv][kt].Py<<";";
            std::cout<<'\n';
          }
       }
// Covariance matrix for nonmerged vertices is updated.
//   For merged vertices (both IN and TO ) it's taken from old fit

       for(iv=0; iv<(int)cstate.m_cascadeVList.size(); iv++){
         bool isMerged=false;
         if(cstate.m_cascadeVList[iv].mergedTO)isMerged=true;                 //vertex is merged
         index=getSimpleVIndex( cstate.m_cascadeVList[iv].vID, cstate );                   //index of current vertex in simplified structure
         for(ip=0; ip<(int)cstate.m_cascadeVList[iv].inPointingV.size(); ip++){    //inPointing verties
            int tmpIndexV=indexInV( cstate.m_cascadeVList[iv].inPointingV[ip], cstate);    //index of inPointing vertex in full structure
            if(cstate.m_cascadeVList[tmpIndexV].mergedTO)isMerged=true;       //vertex is merged
         }
         if(!isMerged){
           fittedCovariance[iv]=t_fittedCovariance[index];    //copy complete covarinace matrix for nonmerged vertices
         }
       }
    }
//
//-------------------------------------Saving
//
    ATH_MSG_DEBUG("Now save results");
    Amg::MatrixX VrtCovMtx(3,3);
    Trk::Perigee * measPerigee;
    std::vector<xAOD::Vertex*> xaodVrtList(0);
    double phi, theta, invP, mom, fullChi2=0.;

    int NDOF=getCascadeNDoF(cstate); if(NDOFsqueezed) NDOF=NDOFsqueezed; 
    if(primVrt){ if(FirstDecayAtPV){ NDOF+=3; }else{ NDOF+=2; } }

    for(iv=0; iv<(int)cVertices.size(); iv++){
      Amg::Vector3D FitVertex(cVertices[iv].X+state.m_refFrameX,cVertices[iv].Y+state.m_refFrameY,cVertices[iv].Z+state.m_refFrameZ);
      VrtCovMtx(0,0) = covVertices[iv][0]; VrtCovMtx(0,1) = covVertices[iv][1];
      VrtCovMtx(1,1) = covVertices[iv][2]; VrtCovMtx(0,2) = covVertices[iv][3];
      VrtCovMtx(1,2) = covVertices[iv][4]; VrtCovMtx(2,2) = covVertices[iv][5];
      VrtCovMtx(1,0) = VrtCovMtx(0,1);
      VrtCovMtx(2,0) = VrtCovMtx(0,2);
      VrtCovMtx(2,1) = VrtCovMtx(1,2);
      double Chi2=0; int NTrInV=0;
      for(it=0; it<(int)vertexDefinition[iv].size(); it++) { Chi2 += particleChi2[vertexDefinition[iv][it]]; NTrInV++ ;};
      fullChi2+=Chi2;

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=  xAOD::Vertex creation
      xAOD::Vertex * tmpXAODVertex=new xAOD::Vertex();
      tmpXAODVertex->makePrivateStore();
      tmpXAODVertex->setPosition(FitVertex);
      tmpXAODVertex->setFitQuality(Chi2, (float)NDOF);
      std::vector<VxTrackAtVertex> & tmpVTAV=tmpXAODVertex->vxTrackAtVertex();
      tmpVTAV.clear();

      int NRealT=vertexDefinition[iv].size();
      Amg::MatrixX genCOV( NRealT*3+3, NRealT*3+3 );     // Fill cov. matrix for vertex
      for( it=0; it<NRealT*3+3; it++){                                 // (X,Y,Z,px1,py1,....pxn,pyn,pzn)
        for( jt=0; jt<=it; jt++){                                      // 
           genCOV(it,jt) = genCOV(jt,it) = fittedCovariance[iv][it*(it+1)/2+jt];      // for real tracks only
      } }                                                              // (first in the list)
      Amg::MatrixX *fullDeriv=nullptr;
      if( m_makeExtendedVertex ){
         //VK fullDeriv=new CLHEP::HepMatrix( NRealT*3+3, NRealT*3+3, 0); // matrix is filled by zeros
         fullDeriv=new Amg::MatrixX( NRealT*3+3, NRealT*3+3);       
         (*fullDeriv)=Amg::MatrixX::Zero(NRealT*3+3, NRealT*3+3);    // matrix is filled by zeros
	 (*fullDeriv)(0,0)=(*fullDeriv)(1,1)=(*fullDeriv)(2,2)=1.;
      }
      for( it=0; it<NRealT; it++) {  
         mom= sqrt(  fittedParticles[iv][it].Pz*fittedParticles[iv][it].Pz
                    +fittedParticles[iv][it].Py*fittedParticles[iv][it].Py
                    +fittedParticles[iv][it].Px*fittedParticles[iv][it].Px);
         double Px=fittedParticles[iv][it].Px;
         double Py=fittedParticles[iv][it].Py;
         double Pz=fittedParticles[iv][it].Pz;
         double Pt= sqrt(Px*Px + Py*Py) ;
         phi=atan2( Py, Px);
         theta=acos( Pz/mom );
         invP = - state.m_ich[vertexDefinition[iv][it]] / mom;   // Change charge sign according to ATLAS
//  d(Phi,Theta,InvP)/d(Px,Py,Pz)  -  Perigee vs summary momentum
         Amg::MatrixX tmpDeriv( 5, NRealT*3+3);
	 tmpDeriv.setZero();                            // matrix is filled by zeros
         tmpDeriv(0,1) = -sin(phi);                     // Space derivatives
         tmpDeriv(0,2) =  cos(phi);
         tmpDeriv(1,1) = -cos(phi)/tan(theta);
         tmpDeriv(1,2) = -sin(phi)/tan(theta);
         tmpDeriv(1,3) =  1.;
         tmpDeriv(2+0,3*it+3+0) = -Py/Pt/Pt;             //dPhi/dPx
         tmpDeriv(2+0,3*it+3+1) =  Px/Pt/Pt;             //dPhi/dPy
         tmpDeriv(2+0,3*it+3+2) =  0;                    //dPhi/dPz
         tmpDeriv(2+1,3*it+3+0) =  Px*Pz/(Pt*mom*mom);   //dTheta/dPx
         tmpDeriv(2+1,3*it+3+1) =  Py*Pz/(Pt*mom*mom);   //dTheta/dPy
         tmpDeriv(2+1,3*it+3+2) = -Pt/(mom*mom);         //dTheta/dPz
         tmpDeriv(2+2,3*it+3+0) = -Px/(mom*mom) * invP;  //dInvP/dPx
         tmpDeriv(2+2,3*it+3+1) = -Py/(mom*mom) * invP;  //dInvP/dPy
         tmpDeriv(2+2,3*it+3+2) = -Pz/(mom*mom) * invP;  //dInvP/dPz
//----------  Here for Eigen block(startrow,startcol,sizerow,sizecol)
         if( m_makeExtendedVertex )(*fullDeriv).block(3*it+3+0,3*it+3+0,3,3) = tmpDeriv.block(2,3*it+3+0,3,3);
//----------
	 AmgSymMatrix(5) *tmpCovMtx = new AmgSymMatrix(5);                      // New Eigen based EDM
	 (*tmpCovMtx) = genCOV.similarity(tmpDeriv);                            // New Eigen based EDM
         measPerigee =  new Perigee( 0.,0., phi, theta, invP, PerigeeSurface(FitVertex), tmpCovMtx );  // New Eigen based EDM
         tmpVTAV.emplace_back( particleChi2[vertexDefinition[iv][it]] , measPerigee ) ;
      }
      std::vector<float> floatErrMtx;
      if( m_makeExtendedVertex ) {
  	 Amg::MatrixX tmpCovMtx(NRealT*3+3,NRealT*3+3);                      // New Eigen based EDM
         tmpCovMtx=genCOV.similarity(*fullDeriv);
         floatErrMtx.resize((NRealT*3+3)*(NRealT*3+3+1)/2);
         int ivk=0;
         for(int i=0;i<NRealT*3+3;i++){
           for(int j=0;j<=i;j++){
               floatErrMtx.at(ivk++)=tmpCovMtx(i,j);
           }
         }
      }else{
         floatErrMtx.resize(6);
         for(int i=0; i<6; i++) floatErrMtx[i]=covVertices[iv][i];
      }
      tmpXAODVertex->setCovariance(floatErrMtx);
      for(int itvk=0; itvk<NRealT; itvk++) {
          ElementLink<xAOD::TrackParticleContainer> TEL;
          if(itvk < (int)cstate.m_cascadeVList[iv].trkInVrt.size()){
            TEL.setElement( cstate.m_partListForCascade[ cstate.m_cascadeVList[iv].trkInVrt[itvk] ] );
          }else{
            TEL.setElement( nullptr );
          }
          tmpXAODVertex->addTrackAtVertex(TEL,1.);
      }
      xaodVrtList.push_back(tmpXAODVertex);              //VK Save xAOD::Vertex
//
//---- Save and clean
      delete fullDeriv;   //Mandatory cleaning
    }
//
//  Save momenta of all particles including combined at vertex positions
//
    std::vector<TLorentzVector>                     tmpMoms;   
    std::vector<std::vector<TLorentzVector> >       particleMoms;   
    std::vector<Amg::MatrixX>                       particleCovs;   
    int allFitPrt=0;
    for(iv=0; iv<(int)cVertices.size(); iv++){
      tmpMoms.clear();
      int NTrkF=fittedParticles[iv].size();
      for(it=0; it< NTrkF; it++) { 
         tmpMoms.emplace_back( fittedParticles[iv][it].Px, fittedParticles[iv][it].Py, 
                                            fittedParticles[iv][it].Pz, fittedParticles[iv][it].E   );            
      }
      //CLHEP::HepSymMatrix COV( NTrkF*3+3, 0 );
      Amg::MatrixX COV(NTrkF*3+3,NTrkF*3+3); COV=Amg::MatrixX::Zero(NTrkF*3+3,NTrkF*3+3);
      for( it=0; it<NTrkF*3+3; it++){
        for( jt=0; jt<=it; jt++){
           COV(it,jt) = COV(jt,it) = fittedCovariance[iv][it*(it+1)/2+jt];
      } }
      particleMoms.push_back( std::move(tmpMoms) );
      particleCovs.push_back( std::move(COV) );
      allFitPrt += NTrkF;
    }
//
    int NAPAR=(allFitPrt+cVertices.size())*3; //Full size of complete covariance matrix
    //CLHEP::HepSymMatrix FULL( NAPAR, 0 ); 
    Amg::MatrixX FULL(NAPAR,NAPAR); FULL.setZero();
    if( !NDOFsqueezed ){                //normal cascade
      for( it=0; it<NAPAR; it++){
        for( jt=0; jt<=it; jt++){
           FULL(it,jt) = FULL(jt,it) = fitFullCovariance[it*(it+1)/2+jt];
      } }
    }else{    //squeezed cascade
     //int mcount=1;                                          //Indexing in SUB starts from 1 !!!! 
       int mcount=0;                                          //Indexing in BLOCK starts from 0 !!!! 
       for(iv=0; iv<(int)cstate.m_cascadeVList.size(); iv++){
        //FULL.sub(mcount,particleCovs[iv]); mcount += particleCovs[iv].num_col();
          FULL.block(mcount,mcount,particleCovs[iv].rows(),particleCovs[iv].cols())=particleCovs[iv];
	  mcount += particleCovs[iv].rows();
       }
    }    
//
//
//    VxCascadeInfo * recCascade= new VxCascadeInfo(vxVrtList,particleMoms,particleCovs, NDOF ,fullChi2);
    VxCascadeInfo * recCascade= new VxCascadeInfo(std::move(xaodVrtList),std::move(particleMoms),std::move(particleCovs), NDOF ,fullChi2);
    recCascade->setFullCascadeCovariance(FULL);
    CLEANCASCADE();
    return recCascade;
}

#undef CLEANCASCADE

 
StatusCode  TrkVKalVrtFitter::addMassConstraint(VertexID Vertex,
                                 const std::vector<const xAOD::TrackParticle*> & tracksInConstraint,
                                 const std::vector<VertexID> &pseudotracksInConstraint, 
                                 IVKalState& istate,
				 double massConstraint ) const
{
    assert(dynamic_cast<State*> (&istate)!=nullptr);
    State& state = static_cast<State&> (istate);
    CascadeState& cstate = *state.m_cascadeState;

    int ivc, it, itc;
    //int NV=m_cstate.cascadeVList.size();              // cascade size
//----
    if(Vertex <   0) return StatusCode::FAILURE;   
    //if(Vertex >= NV) return StatusCode::FAILURE;  //Now this check is WRONG. Use indexInV(..) instead
//
//---- real tracks 
//
    int cnstNTRK=tracksInConstraint.size();                  // number of real tracks in constraints
    int indexV = indexInV(Vertex, cstate);                   // index of vertex in cascade structure
    if(indexV<0) return StatusCode::FAILURE;
    int NTRK    = cstate.m_cascadeVList[indexV].trkInVrt.size();    // number of real tracks in chosen vertex
    int totNTRK = cstate.m_partListForCascade.size();               // total number of real tracks 
    if( cnstNTRK > NTRK ) return StatusCode::FAILURE;
//-
    PartialMassConstraint  tmpMcnst;
    tmpMcnst.Mass         = massConstraint;
    tmpMcnst.VRT          = Vertex;
//
    double totMass=0;
    for(itc=0; itc<cnstNTRK; itc++) { 
       for(it=0; it<totNTRK; it++) if(tracksInConstraint[itc]==cstate.m_partListForCascade[it]) break;  
       if(it==totNTRK) return StatusCode::FAILURE;  //track in constraint doesn't correspond to any track in vertex
       tmpMcnst.trkInVrt.push_back(it);
       totMass += cstate.m_partMassForCascade[it];
    }
    if(totMass > massConstraint)return StatusCode::FAILURE;
//
//---- pseudo tracks 
//
    int cnstNVP = pseudotracksInConstraint.size();               // number of pseudo-tracks in constraints
    int NVP     = cstate.m_cascadeVList[indexV].inPointingV.size();     // number of pseudo-tracks in chosen vertex
    if( cnstNVP > NVP ) return StatusCode::FAILURE;
//-
    for(ivc=0; ivc<cnstNVP; ivc++) { 
       int tmpV = indexInV(pseudotracksInConstraint[ivc], cstate);                           // index of vertex in cascade structure
       if( tmpV< 0) return StatusCode::FAILURE;  //pseudotrack in constraint doesn't correspond to any pseudotrack in vertex
       tmpMcnst.pseudoInVrt.push_back( pseudotracksInConstraint[ivc] );
    }

    cstate.m_partMassCnstForCascade.push_back(std::move(tmpMcnst));
  
    return StatusCode::SUCCESS;
}


//----------------------------------------------------------------------------------------
//   Looks for index of each element of inputList in refList
//
int TrkVKalVrtFitter::findPositions(const std::vector<int> &inputList,const std::vector<int> &refList, std::vector<int> &index) const
{   
   int R,I;
   index.clear();
   int nI=inputList.size();   if(nI==0) return 0;        //all ok
   int nR=refList.size();     if(nR==0) return 0;        //all ok
   //std::cout<<"inp="; for(I=0; I<nI; I++)std::cout<<inputList[I]; std::cout<<'\n';
   //std::cout<<"ref="; for(R=0; R<nR; R++)std::cout<<refList[R]; std::cout<<'\n';
   for(I=0; I<nI; I++){
      for(R=0; R<nR; R++) if(inputList[I]==refList[R]){index.push_back(R); break;}
      if(R==nR) return -1;                              //input element not found in reference list
   }   
   return 0;
} 
//-----------------------------------------------------------------
//   Get index of given vertex in simplified cascade structure
//
int TrkVKalVrtFitter::getSimpleVIndex( const VertexID & vrt,
                                       const CascadeState& cstate) const
{
    int NVRT=cstate.m_cascadeVList.size();

    int iv=indexInV(vrt, cstate);
    if(iv<0) return -1;  //not found

    int ivv=0;
    if(cstate.m_cascadeVList[iv].mergedTO){ 
       for(ivv=0; ivv<NVRT; ivv++) if(cstate.m_cascadeVList[iv].mergedTO == cstate.m_cascadeVList[ivv].vID) break; 
       if(iv==NVRT) return -1;  //not found
       iv=ivv;
    }
    return cstate.m_cascadeVList[iv].indexInSimpleCascade;
} 
//-----------------------------------------------------------------
//   Get index of given vertex in full cascade structure
//
int TrkVKalVrtFitter::indexInV( const VertexID & vrt,
                                const CascadeState& cstate) const
{  int icv;   int NVRT=cstate.m_cascadeVList.size();
   for(icv=0; icv<NVRT; icv++)  if(vrt==cstate.m_cascadeVList[icv].vID)break;
   if(icv==NVRT)return -1;
   return icv;
}

} // End Of Namespace
