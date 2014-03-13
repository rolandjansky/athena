/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "muonEvent/MuonSpShowerContainer.h"
#include "muonEvent/MuonSpShower.h"

#include "MissingETGoodness/MuonSpShowerHelper.h"

// Utility functions not depending on any class
namespace MuonSpShowerHelper {
   
// Get total number of hits 
   uint TotalSpHits(Rec::MuonSpShower const * const  MuonSpShower) {
      return (MuonSpShower->numberOfInnerHits()+
              MuonSpShower->numberOfMiddleHits()+
              MuonSpShower->numberOfOuterHits());
   }


   // Get index list, sorted by number of hits 
   std::vector<int> SortListSpShower(Rec::MuonSpShowerContainer const * const MuonSpShowers) {
      
      std::vector<std::pair<int,int> > sortvector;
      std::vector<int > resultvector;
      
      // Sorting by filling with negative number of hits.
      uint numshowers= MuonSpShowers->size();
      for (uint i=0;i<numshowers;i++){     
         sortvector.push_back( std::pair<int,int>(-TotalSpHits(MuonSpShowers->at(i)),i));
      }
      std::sort(sortvector.begin(), sortvector.end());
      
      for (std::vector< std::pair<int,int> >::const_iterator iter = sortvector.begin();
           iter != sortvector.end(); ++iter)
      {     
         resultvector.push_back(iter->second);
      }
      return resultvector;      
   }
   
// Find SpShower closest to some Phi,Eta
   bool FindSpShower(uint & FoundIndex,
                     Rec::MuonSpShowerContainer const * const MuonSpShowers,
                     double atEta,double atPhi,bool PhiOnly ) {

      double min_dR=1e9;
      int best=-1;

      int numshowers= MuonSpShowers->size();
      if(numshowers<1){
         return false;
      }
      for (int i=0;i<numshowers;i++){
         // Unfortunately phi and eta are not const, so we'll cast away const
         Rec::MuonSpShower * const mSP = const_cast<Rec::MuonSpShower *>(MuonSpShowers->at(i));
     
         if(!TotalSpHits(mSP)) continue;
         
         // calculate eta,phi radius
         double deta=0;      
         if (!PhiOnly){
            double eta=mSP->eta();
            deta = fabs(atEta - eta); 				  
         }
         double phi=mSP->phi();
         double dphi =MuonSpShowerHelper::DiffPhi(phi,atPhi);
         double dR = sqrt(pow(deta,2) + pow(dphi,2));
      
         if ((best<0)||(min_dR>dR)){
            min_dR=dR;
            best=i;
         }
      }
      if (best>=0){
         FoundIndex=best;
         return true;
      }
      return false;
   }
   
     
// Find SpShower closest to some Phi
   bool FindSpShower(uint & FoundIndex,
                     Rec::MuonSpShowerContainer const * const MuonSpShowers,
                     double atPhi) {
      return FindSpShower(FoundIndex, MuonSpShowers,0,atPhi,true);
   }


   
// Find SpShower with highest number of hits 
   bool FindSpShowerMaxHits(uint & FoundIndex,
                            Rec::MuonSpShowerContainer const * const MuonSpShowers) {
   
      double max_Hits=-1;
      int best=-1;
      int numshowers= MuonSpShowers->size();
      for (int i=0;i<numshowers;i++){     
         uint Hits = TotalSpHits(MuonSpShowers->at(i));      
         if ((best<0)||(Hits>max_Hits)){
            max_Hits=Hits;
            best=i;
         }
      }
      if (best>=0){
         FoundIndex=best;
         return true;
      }   
      return false;
   }


// Sum MuonSpShower vector. Eta,Phi becomes the Hit weighted mean values
// - returns false on empty vector , or if there are NO hits
   bool SumSpShowers(Rec::MuonSpShower & ResultSpShower,
                     Rec::MuonSpShowerContainer const * const MuonSpShowers) {
 
      uint numshowers= MuonSpShowers->size();
      if(numshowers<1){
         return false;
      }

      Rec::MuonSpShower R; 
   
      double sum_sinphi=0;
      double sum_cosphi=0;
      double eta=0;
   
      uint iH=0;
      uint mH=0;
      uint oH=0;
      uint tH=0;   
      uint iS=0;
      uint mS=0;
      uint oS=0;
   
      for (uint i=0;i<numshowers;i++){

         // Unfortunately phi and eta are not const, so we'll cast away const
         Rec::MuonSpShower * mSP = const_cast<Rec::MuonSpShower *>(MuonSpShowers->at(i));

         uint tmp_iH= mSP->numberOfInnerHits(); 
         uint tmp_mH= mSP->numberOfMiddleHits();
         uint tmp_oH= mSP->numberOfOuterHits();
      
         iH+=tmp_iH;
         mH+=tmp_mH;
         oH+=tmp_oH;
         iS+=mSP->numberOfInnerSegments();
         mS+=mSP->numberOfMiddleSegments();
         oS+=mSP->numberOfOuterSegments();
         tH+=mSP->numberOfTriggerHits();

         uint tmp_tothits= (tmp_iH+tmp_mH+tmp_oH);
         if(tmp_tothits>0){
            eta+=tmp_tothits*mSP->eta();
            sum_cosphi+=tmp_tothits*cos(mSP->phi());
            sum_sinphi+=tmp_tothits*sin(mSP->phi());      
         }
      }

      int tothits= iH+mH+oH;
      if (tothits==0){
         return false;
      }
   
      R.set_numberOfInnerHits(iH);
      R.set_numberOfMiddleHits(mH);
      R.set_numberOfOuterHits(oH);
      R.set_numberOfInnerSegments(iS);
      R.set_numberOfMiddleSegments(mS);
      R.set_numberOfOuterSegments(oS);
      R.set_numberOfTriggerHits(tH);
      // weighted mean of phi (wrapping correctly) in vector notation:
      //
      // x=sum(cos), y=sum(sin), result=atan2(y,x)
      R.set_phi(atan2(sum_sinphi,sum_cosphi));
      // for eta we just do a flat mean.
      R.set_eta(eta/tothits);
   
      ResultSpShower=R;

      return true;
   }

   
   // GetSpShower by Index
  bool GetSpShower(Rec::MuonSpShower & ResultSpShower,
                   Rec::MuonSpShowerContainer const * const MuonSpShowers,uint index) {
     if(index>=MuonSpShowers->size()){
        return false;
     }
     ResultSpShower=*MuonSpShowers->at(index);
     return true;
  }
}

