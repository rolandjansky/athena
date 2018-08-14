/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigFTKBankGen/FTKPattGenRoot.h"
#include <fstream>
#include <cmath>
#include <algorithm>
#include <TFile.h>
#include <TDirectory.h>
#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/FTKRootFile.h"
#include <TFile.h>
#include "TrigFTKSim/FTKTrack.h"
// #include "TrigFTKSim/FTKPatternBySector.h"
// #include "TrigFTKSim/ftk_dcap.h"
#include "TrigFTKSim/ftk_dcap_helper.h"
#include "TrigFTKBankGen/decodeEtacode.h"

#ifdef DEBUG_BEAMSPOT
#include "TrkGeometry/MagneticFieldProperties.h"
#endif

/* class FTKPattGenRoot
 *
 * Generate patterns from constants.
 * authors: D. Britzger, after PattBankGenAlgo by L. Luongo, G. Volpi, ....
 *
 * Usage:
 *    FTKPattGenRoot pattgen(pmap,ssmap,sectorslice,rmap,m_curreg,constbank);
 *    pattgen.SetRootOutput("patterns.root");
 *  repeated times:
 *    pattgen.trackInvertion(m_trials,true);
 *    // patterns will be written when pattgen is destroyed
 * Alternative usage: Use pointers and 'delete pattgen'.
 *  
 * If desired write out also ascii format:
 *     (not yet implemented)
 *  
 */

using namespace std;

//================== class FTKPattGenRoot =================



FTKPattGenRoot::FTKPattGenRoot(int curreg, FTKSSMap* ssmap, FTKSectorSlice* slices, FTKConstantBank* constbank,int keep7of8,double tolerance7of8
#ifdef DEBUG_BEAMSPOT
                  ,ToolHandle<Trk::IPropagator> *propagator
#endif
                               )
   : FTKLogging("FTKPattGenRoot"), m_curreg(curreg), m_ssmap(ssmap),
     m_sectorslice(slices), m_constbank(constbank),
     m_RndGauss(0),
     m_RndFlat(0),
     m_output(0),
     m_patternWriter(0),
     m_keep7of8(keep7of8),m_tolerance7of8(tolerance7of8),
     m_d0_alpha(0.)
#ifdef DEBUG_BEAMSPOT
,m_propagator(propagator)
#endif
 {
   /************************************************
    *
    *  Create patterns from constants
    *  Input parameters are:  
    *    + Current region:          (int)curreg
    *    + SS map:                  ssmap (how many strips, pixels are in each layer's modules)
    *    + Sector slices:           slices
    *    + Geometry constants:      constbank
    *  
    ************************************************/

   SetSliceParameters(); // init slice parameters
   m_constbank->prepareInvConstants();

   m_select = ModuleGeometry; //FTKPattGenRoot::RndSector;

   ostream & out=
      (Info("FTKPattGenRoot")<<"keep 7/8 patterns for missing planes [");
   unsigned mask=keep7of8;
   for(int missing =0;mask;missing ++,mask>>=1) {
      if(mask & 1) {
         out<<" "<<missing;
      }
   }
   out<<"]\n";
}

void FTKPattGenRoot::SetRandomNumberGenerator(CLHEP::HepRandomEngine* RndEng ) {
   m_RndGauss=new CLHEP::RandGauss(RndEng);
   m_RndFlat=new CLHEP::RandFlat(RndEng);
}

//___________________________________________________________________________________________ //
FTKPattGenRoot::~FTKPattGenRoot() {
   //! Destructor
   //! Destroy pattern writer object to safe patterns to disk
   Info("~FTKPattGenRoot")<<"Deleting PatternWriter = write root-patterns to disk"<<endl;
   if ( m_patternWriter) delete m_patternWriter;
   if(m_output) delete m_output;
}


//___________________________________________________________________________________________ //
const FTKPlaneMap* FTKPattGenRoot::pmap() const { 
   return m_ssmap->getPlaneMap(); // same as m_rmap->getPlaneMap();
}


//___________________________________________________________________________________________ //
void FTKPattGenRoot::DeleteMaps() { 
   // delete pointers of input files
   m_ssmap=NULL;
   Info("DeleteMaps")<<"Delete m_sectorslice"<<endl;
   if (m_sectorslice) delete m_sectorslice;
   m_sectorslice=NULL;
   Info("DeleteMaps")<<"Delete m_constbank"<<endl;
   if (m_constbank) delete m_constbank;
   m_constbank=NULL;
}


//___________________________________________________________________________________________ //
bool FTKPattGenRoot::GoodTrack(const FTKTrack& track, int isector, FTKPatternWithSector* tmp_curpatt,int *missingPlanesPtr,double *distancePtr) {
   /**
   *   Check if track is in sector
   *   returns true or false
   *   tmp_curpatt is filled coordinates
   */
   int missing=0;
   double distance=0.0;
   for (int p=0; p<pmap()->getNPlanes(); ++p) {
      int ndim = pmap()->getDim(p,1)==-1 ? 1 : 2;
      FTKHit hit(ndim);
      int ssid=-1;
      // potential BUG with HWMODEID=2
      // the section number and module number are not known
      int section=0;
      int phimod=0;
      int etamod=0;
      if(FTKSetup::getFTKSetup().getHWModeSS()==0) {
         hit.setSector(m_sectorbank[isector].GetHit(p));
	 hit.setITkMode(FTKSetup::getFTKSetup().getITkMode());
         section=hit.getSection();
         phimod=hit.getPhiModule();
         etamod=hit.getEtaModule();
      }
      double x_width=m_ssmap->getMap(p,section,phimod).m_phiwidth;
      int ix = pmap()->getDim(p,0);
      double x_track = track.getCoord(ix);
      double x_ssid=x_track;
      if(x_ssid<0) {
         x_ssid=0.;
         missing |= (1<<p);
      } else if(x_ssid>=x_width) {
         x_ssid=x_width-1.;
         missing |= (1<<p);
      }
      double y_width=0.,y_track=0.,y_ssid=0.;
      if(ndim==2) {
         y_width=m_ssmap->getMap(p,section,etamod).m_etawidth;
         int iy = pmap()->getDim(p,1);
         y_track = track.getCoord(iy);
         y_ssid=y_track;
         if(y_ssid<0.) {
            y_ssid=0.;
            missing |= (1<<p);
         } else if(y_ssid>=y_width) {
            y_ssid=y_width-1;
            missing |= (1<<p);
         }
      }
      if (FTKSetup::getFTKSetup().getHWModeSS()==0) {
         hit.setCoord(0, x_ssid);
         if ( ndim==2) {
            hit.setCoord(1, y_ssid);
         }
         // build hit
         hit.setPlane(p);
         hit.setEtaWidth(1);
         hit.setPhiWidth(1);	 
         ssid=m_ssmap->getSSGlobal(hit);
      } else { 
         // HWMIDEID==2
         int globalModuleId=m_sectorbank[isector].GetHit(p);
         int localModuleID=m_ssmap->getRegionMap()
            ->getLocalId(m_curreg,p,globalModuleId);
         if(localModuleID<0) {
            cout<<"globalModuleId="<<globalModuleId
                <<" -> local="<<localModuleID<<"\n";
         }
         if(ndim==2) {
            ssid=m_ssmap->compressed_ssid_word_pixel
               (localModuleID,p,section,x_ssid,y_ssid);
         } else {
            ssid=m_ssmap->compressed_ssid_word_strip
               (localModuleID,p,section,x_ssid);
         }
      }
      tmp_curpatt->SetHit(p,ssid);
      if(missing & (1<<p)) {
         double d=(x_ssid-x_track)/x_width;
         if(fabs(d)>distance) distance=d;
         if(ndim==2) {
            d=(y_ssid-y_track)/y_width;
            if(fabs(d)>distance) distance=d;
         }
      }
   } // for n-planes
   if(missingPlanesPtr) *missingPlanesPtr=missing;
   if(distancePtr) *distancePtr=distance;
   return !missing;

}


//___________________________________________________________________________________________ //
uint64_t FTKPattGenRoot::trackInvertion(u_int64_t ntrials, bool smear) {
   //!
   //! 'main' function ot do the track inversion
   //! and generate patterns
   //!

   if (m_sectorbank.empty()) {
      Error("trackInvertion")<<"No sectors given. Please call ReadSectorFile(filename) first."<<endl;
      return 0;
   }
   FTKTrack track;
   track.setNCoords(pmap()->getTotalDim());
   static FTKPatternWithSector* tmp_curpatt = new FTKPatternWithSector(pmap()->getNPlanes());
   static vector<double> constr(m_constbank->getNConstr()); // filled with 0

   uint64_t nFound=0;
   uint64_t nFound7of8=0;
   uint64_t nSuccTrials=0;

   ofstream *gap_monitoring=0;
   ofstream *overlap_monitoring=0;
   ofstream *trackpar_monitoring=0;
#ifdef DEBUG_GAP_MONITORING
   ostringstream gap_file_name;
   gap_file_name<<"gap_monitoring_reg"<<m_curreg<<".txt";
   gap_monitoring=new ofstream(gap_file_name.str().c_str());
#endif
#ifdef DEBUG_OVERLAP
   ostringstream overlap_file_name;
   overlap_file_name<<"overlap_monitoring_reg"<<m_curreg<<".txt";
   overlap_monitoring=new ofstream(overlap_file_name.str().c_str());
#endif
#ifdef DEBUG_BEAMSPOT
   ostringstream trackpar_file_name;
   trackpar_file_name<<"trackpar_monitoring_reg"<<m_curreg<<".txt";
   trackpar_monitoring=new ofstream(trackpar_file_name.str().c_str());
#endif

   for (u_int64_t t=0; t<ntrials; ++t) {
      nSuccTrials++;
      if ( nSuccTrials%1000000==0 )
	 Info("trackInvertion()")
            <<"Number of all trials: "<<nSuccTrials
            <<". Number of successful trials: "<<nFound
            <<" with 7/8: "<<nFound7of8<<endl; //nFound==t
      // build track with random numbers
      GetRandomTrack(track); 
      // Convert phi to the format used in the constants calculations
      ConvertPhi(track);
     
      vector<int>* sectors =  m_sectorslice->searchSectors(track);
      //m_sectorHist->Fill((Double_t)sectors->size());
      if (sectors->size() == 0) {
	 t--; // we want to have ntrials patterns (also below)
	 delete sectors;
	 continue;
      } 

      // Generate constraints (random numbers from the standard Gaussian Distribution or 0)
      if (smear) 
	 for (unsigned int i=0; i<constr.size(); ++i) 
	    constr[i] = getRandomGaus();

      // loop over found sectors
      std::vector<FTKPatternWithSector> found_patterns,found_patterns7of8;
      std::map<int,pair<int,double> > info7of8;
      for ( auto isec : *sectors ) {
	 tmp_curpatt->SetSectorID(isec); // not available in FTKPatternWithCoverage
	 tmp_curpatt->SetCoverage(1);
	 m_constbank->invlinfit(isec, track, &constr[0]); // set: track.setCoord
         int missingPlanes=0;
         double distance=0.;
	 if ( GoodTrack(track,isec,tmp_curpatt,&missingPlanes,&distance) ) {
	    found_patterns.push_back(*tmp_curpatt);
	 } else {
            for(int missingPlane=0;missingPlane<pmap()->getNPlanes();
                missingPlane++) {
               // check whetrher 7/8 is allowed for this plane
               if(!(m_keep7of8 & (1<<missingPlane))) continue;
               // check whether there is exactly one hit missing in this plane
               if(missingPlanes != (1<<missingPlane)) continue;
               // check wthere distance is small enough
               if(fabs(distance)>m_tolerance7of8) continue;

               // check whether this is the first 7/8 pattern
               //   for the given missing plane
               std::map<int,pair<int,double> >::iterator infoPtr=
                  info7of8.find(missingPlane);
               if(infoPtr!=info7of8.end()) {
                  // previous pattern exists
                  if( fabs(distance)<fabs((*infoPtr).second.second)) {
                     // new hit has a smaller distance, replace
                     // old pattern
                     (*infoPtr).second.second= distance;
                     found_patterns7of8[(*infoPtr).second.first]=
                        *tmp_curpatt;
                  }
               } else {
                  // first 7/8 pattern for this plane, always store it
                  info7of8[missingPlane]=
                     make_pair(found_patterns7of8.size(),distance);
                  found_patterns7of8.push_back(*tmp_curpatt);
               }
            }
         }
      }

      // if there is no 8/8 pattern, try 7/8
      bool is7of8=false;
      if(found_patterns.empty() && m_keep7of8) {
         found_patterns=found_patterns7of8;
         is7of8=true;
      } else {
         if(overlap_monitoring && found_patterns.size()>1) {
            (*overlap_monitoring)<<found_patterns.size();
            for(unsigned i=0;i<found_patterns.size();i++) {
               (*overlap_monitoring)
                  <<" "<<found_patterns[i].GetSectorID();
            }
            (*overlap_monitoring)<<"\n";
         }
      }
      if(gap_monitoring && is7of8 && info7of8.size()) {
         (*gap_monitoring)<< info7of8.size()<<"\n";
         for(std::map<int,pair<int,double> >::const_iterator infoPtr=
                info7of8.begin();infoPtr!=info7of8.end();infoPtr++) {
            int isector=found_patterns[(*infoPtr).second.first].GetSectorID();
            int moduleID=m_sectorbank[isector].GetHit((*infoPtr).first);
            (*gap_monitoring)
               <<(*infoPtr).first
               <<" "<<moduleID
               <<" "<<(*infoPtr).second.second<<"\n";
            gap_monitoring->flush();
         }
      }

      OverlapRemoval(found_patterns ); 

      // patterns found? keep it following the selection criterion
      if ( !found_patterns.empty() ) {
	 // sector selection
         unsigned patt=0;
	 if ( m_select == FTKPattGenRoot::RndSector ) {
	    // prepare to iterate on the list of sector IDs
	    patt = (unsigned int)(found_patterns.size() * getRandom());//m_pEng->flat());
            if(patt>=found_patterns.size()) patt=found_patterns.size()-1;
	 } else if((m_select==ModuleGeometry)||
                   (m_select==ModuleGeometrySmallestSector)||
                   (m_select==ModuleGeometryBestMatch)) {
            static int print=30;
            if(FTKSetup::getFTKSetup().getHWModeSS()!=2) {
               Fatal("trackInvertion")
                  <<"Selector="<<m_select<<" only implemented for HDMODEID==2\n";
            }
            // for each candidate, calculate a weight 0<w<1
            vector<double> weight(found_patterns.size());
            unsigned nFound=0;
            vector<unsigned> exactMatch;
            static double const B_FACTOR=-0.3*2.083;// conversion factor 1/Mev -> 1/mm
            double rho=B_FACTOR*track.getHalfInvPt() *(2.0);
            for(unsigned ipatt=0;ipatt<found_patterns.size();ipatt++) {
               double w=1.0;
               bool found=true;
               int isector=found_patterns[ipatt].GetSectorID();
               // the weight w is the a product of the weights from each plane
               for (int p=0; p<pmap()->getNPlanes(); ++p) {
                  int id=m_sectorbank[isector].GetHit(p);
                  map<int,RZminmax_t>::const_iterator irz;
                  if(pmap()->getDim(p,1)!=-1) {
                     // lookup geometry by module ID
                     irz=m_moduleBoundsPixel.find(id);
                     found = (irz!=m_moduleBoundsPixel.end());
                  } else {
                     irz=m_moduleBoundsSCT.find(id);
                     found = (irz!=m_moduleBoundsSCT.end());
                  }
                  if(found) {
                     RZminmax_t const &rz=(*irz).second;
                     // rMin,rMax and zMin,zMax are the 
                     // module bounds in r,z
                     float dz=rz.zMax-rz.zMin;
                     float dr=rz.rMax-rz.rMin;
                     // track parameters in r,z
                     double cotTh=track.getCotTheta();
                     double z0=track.getZ0();
                     // d is the distance from the border
                     // in units of module size
                     // if the track is inside bounds, d=0
                     double d=0.0;
                     if(dz>dr) {
                        // barrel: module dimension in z is larger than in r
                        double r=0.5*(rz.rMax+rz.rMin);
                        // sagitta correction
                        double rrho=r*rho;
                        double s=r*(1.+rrho*rrho/24.);
                        double z=s*cotTh+z0;
                        if(z<rz.zMin) d=(rz.zMin-z)/dz;
                        if(z>rz.zMax) d=(z-rz.zMax)/dz;
                        if(print) {
                           cout<<"barr"<<p<<" sector="<<isector
                               <<" z0="<<z0<<" cotTh="<<cotTh
                               <<" r="<<r
                               <<" z="<<z
                               <<" zMin="<<rz.zMin<<" zMax="<<rz.zMax
                               <<" d="<<d<<"\n";
                        }
                     } else {
                        // endcap: module is smaller in z than in r
                        double z=0.5*(rz.zMax+rz.zMin);
                        double s=(z-z0)/cotTh;
                        // sagitta correction
                        double srho=s*rho;
                        double r=s*(1.-srho*srho/24.);
                        if(r<rz.rMin) d=(rz.rMin-r)/dr;
                        if(r>rz.rMax) d=(r-rz.rMax)/dr;
                        if(print) {
                           cout<<"endc"<<p<<" sector="<<isector
                               <<" z0="<<z0<<" cotTh="<<cotTh
                               <<" z="<<z
                               <<" r="<<r<<" s="<<s
                               <<" rMin="<<rz.rMin<<" rMax="<<rz.rMax
                               <<" d="<<d<<"\n";
                        }
                     }
                     if(d>0.0) {
                        w *=TMath::Exp(-d*d*50.); 
                     }
                  }
               }
               if(found) {
                  nFound++;
               }
               weight[ipatt]=w;
               if(w>=1.0) exactMatch.push_back(ipatt);
            }
            // here: sectors where track is within all modules bounds
            //       have weight=1
            //     the indices are stored in: exactMatch
            // other sectors have 0<weight<1
            //     the weight is stored in weight[]
            if(nFound!=found_patterns.size()) {
               Fatal("trackInvertion")<<"some modules not found\n";
            }
            // throw a random number here, even if it is not used
            // -> algorithms can be compared event by event

            double rndForSector=getRandom();
            if(exactMatch.size()>0) {
               // have an exact match
               if((m_select==ModuleGeometrySmallestSector)||
                  (m_select==ModuleGeometryBestMatch)) {
                  // use smallest sector ID
                  patt=exactMatch[0];
               } else {
                  // random selection
                  unsigned i= (unsigned)(exactMatch.size() * rndForSector);
                  if(i>=exactMatch.size()) i=exactMatch.size()-1;
                  patt=exactMatch[i];
               }
            } else {
               // no exact match
               if(m_select==ModuleGeometryBestMatch) {
                  // find sector with largest weight and use it
                  double maxw=-1.;
                  patt=0;
                  for(unsigned ipatt=0;ipatt<found_patterns.size();ipatt++) {
                     double w=weight[ipatt];
                     if(w>maxw) {
                        maxw=w;
                        patt=ipatt;
                     }
                  }
               } else {
                  // random selection                  
                  // select a pattern according to probability
                  //   given by the pattern weight
                  double wSum=0.0;
                  for(unsigned ipatt=0;ipatt<found_patterns.size();ipatt++) {
                     wSum+=weight[ipatt];
                  }
                  double threshold= wSum*rndForSector;
                  for(patt=0;patt<found_patterns.size()-1;patt++) {
                     threshold -=weight[patt];
                     if(threshold<=0.0) break;
                  }
               }
            }
            if(print) {
               cout<<"candidates: [";
               for(unsigned ipatt=0;ipatt<found_patterns.size();ipatt++) {
                  cout<<" "<<weight[ipatt];
               }
               cout<<"] selected: "<<patt<<" w="<<weight[patt]<<"\n";
               print--;
               //if(!print) exit(0);
            }
	 } else {
	    Fatal("trackInvertion")<<"Selector not yet implemented."<<endl;
	 }

         if ( m_patternWriter ) m_patternWriter->AppendPattern(found_patterns[patt]);
         if(trackpar_monitoring) {
#ifdef DEBUG_BEAMSPOT
            Amg::Vector3D nominal(0.,0.,0.);
            double theta=atan2(1.,track.getCotTheta());
            Trk::Perigee trackpar
               (track.getIP(),track.getZ0(),track.getPhi(),
                theta,
                2.*track.getHalfInvPt()*sin(theta),
                Trk::PerigeeSurface(nominal));
            
            Trk::PerigeeSurface beamspot
               (Amg::Vector3D(m_beamspot[0],m_beamspot[1],0.));

            Trk::MagneticFieldProperties magnet
               =Trk::MagneticFieldProperties(Trk::FullField);;
            const Trk::TrackParameters* tP=
               (*m_propagator)->propagate
               (trackpar,beamspot,Trk::anyDirection, false,magnet);
            double sinth=sin(tP->parameters()[Trk::theta]);
#endif
            
            (*trackpar_monitoring)
               <<track.getPhi()
               <<" "<<2.*track.getHalfInvPt()
               <<" "<<track.getIP()
               <<" "<<track.getZ0()
               <<" "<<track.getCotTheta()
#ifdef DEBUG_BEAMSPOT
               <<" "<< tP->parameters()[Trk::phi]
               <<" "<< tP->parameters()[Trk::qOverP]/sinth
               <<" "<< tP->parameters()[Trk::d0]
               <<" "<< tP->parameters()[Trk::z0]
               <<" "<< cos(tP->parameters()[Trk::theta])/sinth
               <<" "<<m_d0BS<<" "<<m_z0BS
#endif
               <<"\n";
#ifdef DEBUG_BEAMSPOT
            delete tP;
#endif
         }
         nFound++;
         if(is7of8) nFound7of8++;
      }
      else {
	 t--; // we want to have ntrials patterns (also above)
      }
      delete sectors;
      
   } // for ntrials
   if(gap_monitoring) delete gap_monitoring;
   if(overlap_monitoring) delete overlap_monitoring;
   if(trackpar_monitoring) delete trackpar_monitoring;

   return nSuccTrials;
}


//___________________________________________________________________________________________ // 
void FTKPattGenRoot::OverlapRemoval(std::vector<FTKPatternWithSector>& patterns){
   set<unsigned int> remove;
   for ( unsigned int i = 0 ; i<patterns.size() ; i++ ) {
      int s1 = patterns[i].GetSectorID();
      for ( unsigned int j = i+1 ; j<patterns.size() ; j++ ) {
	 int s2 = patterns[j].GetSectorID();
	 int compsect = 0;
	 if ( !remove.count(s1) && !remove.count(s2) ) { // one of the two is already listed
	    if (m_overlap==0 ) compsect = 0 ; // no overlap removal
	    else if (m_overlap==1 ) compsect = OverlapSectorBarrel( s1, s2);
	    else if (m_overlap==2 ) compsect = OverlapSector( s1, s2);
	    else printf("Warning. cannot understand m_overlap. Allowed values are 0,1,2 but is %d\n",m_overlap);
	    //printf("(s1=%d,s2=%d)\tCOMPSECT=%d \n",s1,s2,compsect); 
	    if ( compsect ==  1 ) remove.insert(i);
	    if ( compsect == -1 ) remove.insert(j);
	 }
      }
   }

   if (!remove.empty() ) {
      for (set<unsigned int>::iterator it=remove.end(); it!=remove.begin(); ) {
	 it--;
      }
   }
}


//___________________________________________________________________________________________ // 
void FTKPattGenRoot::RemovePattern(std::vector<FTKPatternWithSector>& patterns, unsigned int id) {
   if ( patterns.empty() ) return; // nothing todo
   patterns.erase(patterns.begin()+id);
}


//___________________________________________________________________________________________ // 
int FTKPattGenRoot::OverlapSectorBarrel(int s1, int s2){
   //! adapted from pe2sector.c
   const FTKHitPattern& m1 = m_sectorbank[s1];
   const FTKHitPattern& m2 = m_sectorbank[s2];
   int score = 0;
   int zero = 0;
   
   if(pmap()->getNPlanes()==4){
      Fatal("OverlapSectorBarrel")<<"NPLANES=4 Not implemented.\n";
   }
   if(FTKSetup::getFTKSetup().getHWModeSS()!=0) {
      Fatal("OverlapSectorBarrel")
         <<"HWMODEID="<<FTKSetup::getFTKSetup().getHWModeSS()
         <<" not implemented.\n";
   }

   for(int i = 0; i < pmap()->getNPlanes(); ++i) {
      int section = SECTION_INT(m1.GetHit(i));
      int ireg = m_ssmap->getRegionMap()->getNumRegions()-1;
      int rmap_maxplane = m_ssmap->getRegionMap()->getRegionMapItem(ireg,i,section).getPhiTot()-1; //  crmap.map[crmap.regions-1][i][section].phi_tot-1; 

      if(m1.GetHit(i)%1000 == m2.GetHit(i)%1000) {
	 int diff = m1.GetHit(i)/1000 - m2.GetHit(i)/1000;
        if(diff == 0)
          zero++;
        else if (diff == 1 || diff == -rmap_maxplane)
          score++;
        else if (diff == -1 || diff == rmap_maxplane)
          score--;
        else
          return 0; // |diff|>1, no overlap

      } else {
        return 0; // diff eta, no overlap
      }
    }
 
   if(score+zero == pmap()->getNPlanes() ) // complete overlap
      return 1;
   else if (zero-score == pmap()->getNPlanes()) // complete overlap reversed
      return -1;
   else
      return 0; // no overlap
}


//___________________________________________________________________________________________ // 
int FTKPattGenRoot::OverlapSector(int s1, int s2){
   //! adapted from pe2sector.c
   const FTKHitPattern& m1 = m_sectorbank[s1];
   const FTKHitPattern& m2 = m_sectorbank[s2];

   int score=0;
   int zero=0;
 
   if(pmap()->getNPlanes()==4){
      Fatal("OverlapSector")<<"NPLANES=4 Not implemented.\n";
   }
   if(FTKSetup::getFTKSetup().getHWModeSS()!=0) {
      Fatal("OverlapSector")
         <<"HWMODEID="<<FTKSetup::getFTKSetup().getHWModeSS()
         <<" not implemented.\n";
   }
   
   // code adapted from pe2sector.c from stanalone code
  for(int i = 0; i < pmap()->getNPlanes(); ++i) {
     if( m1.GetHit(i) == m2.GetHit(i) ) { // Same module
	zero++;
     }
     else { // Different module
	int sdiff = SECTION_INT(m1.GetHit(i)) - SECTION_INT(m2.GetHit(i));
	int pdiff = PHI_MOD_INT(m1.GetHit(i)) - PHI_MOD_INT(m2.GetHit(i));
	int ediff = ETA_MOD_INT(m1.GetHit(i)) - ETA_MOD_INT(m2.GetHit(i));

	if( ediff==0 && sdiff==0 ) { // Same eta and section, check phi
	   int section = SECTION_INT(m1.GetHit(i));
	   int ireg = m_ssmap->getRegionMap()->getNumRegions()-1;
	   int rmap_maxplane = m_ssmap->getRegionMap()->getRegionMapItem(ireg,i,section).getPhiTot()-1; //  crmap.map[crmap.regions-1][i][section].phi_tot-1; 
	   if (pdiff == 1 || pdiff == -rmap_maxplane)
	      score++; // Phi overlap in favor of s2
	   else if (pdiff == -1 || pdiff == rmap_maxplane)
	      score--; // Phi overlap in favor of s1
	   else
	      return 0; // |diff|>1, no overlap
	}
	else if (sdiff != 0) { // Section overlap is ok for disks
	   // If one is barrel and one endcap, take the barrel hit

	        
	   if (pmap()->getPlane(i,SECTION_INT(m1.GetHit(i))).getIsEndcap()
	       && !pmap()->getPlane(i,SECTION_INT(m2.GetHit(i))).getIsEndcap()) {
	      // Section overlap in favor of s2
	      score++;
	   }
	   else if (!pmap()->getPlane(i,SECTION_INT(m1.GetHit(i))).getIsEndcap()
		    && pmap()->getPlane(i,SECTION_INT(m2.GetHit(i))).getIsEndcap()) {
	      // Section overlap in favor of s1
	      score--;
	   }
	   else if (pmap()->getPlane(i,SECTION_INT(m1.GetHit(i))).getPDisk()
		    == pmap()->getPlane(i,SECTION_INT(m2.GetHit(i))).getPDisk() ) {
	      // Same physical disk, different section -- no overlap
	      return 0;
	   }
	   else if ((CSIDE_INT(m1.GetHit(i)) && ASIDE_INT(m2.GetHit(i))) 
		    || (ASIDE_INT(m1.GetHit(i)) && CSIDE_INT(m2.GetHit(i)))) {
	      // Opposite sides of the detector -- no overlap
	      return 0;
	   }
	   else {
	      // Now we have 2 endcap hits on the same side. Take the lower-z one.
	      if (pmap()->getPlane(i,SECTION_INT(m1.GetHit(i))).getPDisk()
		  > pmap()->getPlane(i,SECTION_INT(m2.GetHit(i))).getPDisk()) {
		 score++; // Section overlap in favor of s2
	      }
	      else {
		 score--; // Section overlap in favor of s1
	      }
	   }

	}
	else {
	   // Now we have ediff != 0 -- no overlap
	   return 0;
	}
     } 
  }

  if(zero+score == pmap()->getNPlanes()) // complete overlap
     return 1;
  else if (zero-score == pmap()->getNPlanes()) // complete overlap reversed
     return -1;
  else
     return 0; // no overlap  
}


//___________________________________________________________________________________________ // 
void FTKPattGenRoot::SetRootOutput(const string& outfilename){
   //! Set filename for output root file
   if (m_patternWriter){
      Warning("SetOutputFilename")<<"Pattern writer already initialized with a filename."<<endl;
      delete m_patternWriter;
   }
   if(m_output) delete m_output;
   m_output =
      FTKRootFile::Instance()->CreateRootFile(outfilename.c_str());
   if(!m_output) {
      Fatal("SetRootOutput")<<" can not open file "<<outfilename<<"\n";
   } else {
      m_patternWriter = FTKPatternBySectorWriter::Factory(*m_output);
      m_patternWriter->SetNLayers(pmap()->getNPlanes());
   }
   return;
}


//___________________________________________________________________________________________ //
void FTKPattGenRoot::WriteMapsToFile() {
   //! Write pmap to root-file
   // check if filename is set
   if ( !m_patternWriter ) {
      Fatal("WriteMapsToFile") << "RootOutput not yet set."<<endl;
   }
   if ( !m_ssmap ) {
      Fatal("WriteMapsToFile") << "ssmap not set."<<endl;
   }
   m_ssmap->WriteMapToRootFile(m_output, "TSP" );
}


//___________________________________________________________________________________________ //
void FTKPattGenRoot::SetSliceParameters(double phi_min,double phi_max, double c_min,double c_max, 
					double d0_min,double d0_max, double z0_min,double z0_max, 
					double eta_min, double eta_max){
   //! set slice parameters
   //! Use +/- 999 for autotune parameters.
   if ( !m_sectorslice ) {
      Fatal("SetSliceParameters") << "*** Sector file must be specified first."<< endl;
      exit(1);
   }
   m_phi_min = phi_min==-999. ? m_sectorslice->getAutotunePhiMin() : phi_min;
   m_phi_max = phi_max== 999. ? m_sectorslice->getAutotunePhiMax() : phi_max;

   m_c_min   = c_min==-999.   ? m_sectorslice->getAutotuneCMin() : c_min;//-0.000625;
   m_c_max   = c_max== 999.   ? m_sectorslice->getAutotuneCMax() : c_max;// 0.000625;

   m_d0_min  = d0_min==-999.  ? m_sectorslice->getAutotuneDMin() : d0_min;
   m_d0_max  = d0_max== 999.  ? m_sectorslice->getAutotuneDMax() : d0_max;

   m_z0_min  = z0_min==-999.  ? m_sectorslice->getAutotuneZ0Min() : z0_min;
   m_z0_max  = z0_max== 999.  ? m_sectorslice->getAutotuneZ0Max() : z0_max;

   m_eta_min = eta_min==-999. ? m_sectorslice->getAutotuneEtaMin() : eta_min;
   m_eta_max = eta_max== 999. ? m_sectorslice->getAutotuneEtaMax() : eta_max;

   // JW (2016.9.14) - Update the FTKSectorSlice object with these parameter ranges
   if( phi_min != -999 ) m_sectorslice->setPhiMin( phi_min );
   if( phi_max !=  999 ) m_sectorslice->setPhiMax( phi_max );
   if( c_min != -999 ) m_sectorslice->setCMin( c_min );
   if( c_max !=  999 ) m_sectorslice->setCMax( c_max );
   if( d0_min != -999 ) m_sectorslice->setD0Min( d0_min );
   if( d0_max !=  999 ) m_sectorslice->setD0Max( d0_max );
   if( z0_min != -999 ) m_sectorslice->setZ0Min( z0_min );
   if( z0_max !=  999 ) m_sectorslice->setZ0Max( z0_max );
   if( eta_min != -999 ) m_sectorslice->setEtaMin( eta_min );
   if( eta_max !=  999 ) m_sectorslice->setEtaMax( eta_max );
   m_sectorslice->resetStepSizes();

   Info("SetSliceParameters")<<"Phi: "<<m_phi_min<<" "<<m_phi_max<<"\n";
   Info("SetSliceParameters")<<"Eta: "<<m_eta_min<<" "<<m_eta_max<<"\n";
   Info("SetSliceParameters")<<"C:   "<<m_c_min<<" "<<m_c_max<<"\n";
   Info("SetSliceParameters")<<"D0:  "<<m_d0_min<<" "<<m_d0_max<<"\n";
   Info("SetSliceParameters")<<"Z0:  "<<m_z0_min<<" "<<m_z0_max<<"\n";

}


//___________________________________________________________________________________________ //
double FTKPattGenRoot::getRandom(){
   double rndm = m_RndFlat->fire();
   return rndm;}


//___________________________________________________________________________________________ //
double FTKPattGenRoot::getRandomGaus(){
   double rndm = m_RndGauss->fire();
   return rndm;
}


//___________________________________________________________________________________________ //
void FTKPattGenRoot::GetRandomTrack(FTKTrack& track){
   //! Get random track using the sector parameters

   track.setHalfInvPt(0.5*((m_c_max - m_c_min) * getRandom() + m_c_min)); // diff!?

   track.setPhi( getRandom() * (m_phi_max - m_phi_min) + m_phi_min, false); //ok
   track.setPhiRaw(track.getPhi(),false);

   track.setCotTheta(TMath::SinH((m_eta_max - m_eta_min) * getRandom() + m_eta_min)); //ok

   double d0;
   if((m_d0_min!=m_d0_max)&&(m_d0_alpha!=0.)) {
      // generate density f(r)= k*abs(r)^n
      //  int_0^x dx' = int_r0^r f(r')dr'
      //   x = F(r)-F(r0)
      //   z=x+F(r0) = F(r)
      //   F^-1(z) = r
      //   F(r) = k*r*abs(r)^n
      //   1/k = r1*abs(r1)^n-r0*abs(r0)^n
      //   z/abs(z) * (abs(z)/k)^(1/(n+1)) = r

      double f0=(m_d0_min!=0.) ? m_d0_min*pow(fabs(m_d0_min),m_d0_alpha) : m_d0_min;
      double f1=(m_d0_max!=0.) ? m_d0_max*pow(fabs(m_d0_max),m_d0_alpha) : m_d0_max;
      double k=1./(f1-f0);
      if(k<0.0) {
         // swap f0,f1
         k=f0; f0=f1; f1=k;
         k=1./(f1-f0);
      }
      double z=getRandom()+k*f0;
      d0 = (z !=0.0) ? pow(fabs(z)/k,1./(1.+m_d0_alpha)) : 0.0;
      if(z<0.0) d0= -d0;

      // test: m_d0_alpha=0:
      //    f0=m_d0_min
      //    f1=m_d0_max
      //    k=1./(m_d0_max-m_d0_min)
      //    z = rnd + m_d0_min/(m_d0_max-m_d0_min)
      //   d0 = rnd*(m_d0_max-m_d0_min) + m_d0_min   -> OK
      //
   } else {
      d0=(m_d0_max - m_d0_min) * getRandom() + m_d0_min;
   }
   track.setIP(d0);
   track.setZ0( getRandom() * (m_z0_max - m_z0_min) + m_z0_min); //ok

#ifdef DEBUG_BEAMSPOT
   m_d0BS=track.getIP();
   m_z0BS=track.getZ0();
#endif
   if(!isBeamAtZero()) {
      // transform D0,Z0 (defined wrt beamspot) to origin
      transformToOrigin(track);
   }

   track.setIPRaw(track.getIP());
   track.setZ0Raw(track.getZ0());
}


//___________________________________________________________________________________________ //
bool FTKPattGenRoot::ReadSectorFile(const std::string& sectorfile){
   //! Read sector file in ascii format
   //! Return boolean if file was read successfully

   if (!m_sectorbank.empty()) {
      Warning("ReadSectorFile") << "*** Sector bank already initialized."<<endl;
      return false;
   }
   
      // ftk_dcap can read gzip and bzip2 files
      ftk_dcap::istream ifile;
      bool okread = ftk_dcap::open_for_read(sectorfile.c_str(), ifile);
      if (!ifile || !okread) {
	 Fatal("ReadSectorFile") << "*** Error loading the sector bank: " << sectorfile << endl;
      }
      if (!ifile || !okread) return false;
  
      // Retrieve the number of patterns and planes
      int tmpnplanes;
      int nsectors;
      ifile >> nsectors >> tmpnplanes;
      if (tmpnplanes != pmap()->getNPlanes() ) {
	 cerr << "Wrong planes number in the sectors file: " << sectorfile << endl;
	 return false;
      }
      cout << "Loading " << nsectors << " sectors..." << endl;
      if (nsectors > 0) {
	 m_sectorbank.resize(nsectors,FTKHitPattern(tmpnplanes));
	 for (int id=0; id<nsectors; id++) {
  	    int ssid, sID, pID, cov;
	    {
	       ifile >> pID;
	       for (int ip=0; ip<tmpnplanes;++ip) { // loop on the planes
		  // the m_nplnes+1 element is the number of the sector
		  // the other are the SS index
		  ifile >> ssid;// patt.m_ssid[iplane];
		  m_sectorbank[id].SetHit(ip,ssid);
	       } 
	       ifile >> sID;
	       ifile >> cov;
	    }
	 }
      }
      return true;
}

void FTKPattGenRoot::setBeamspot(double beamspotX,double beamspotY) {
   m_beamspot[0]=beamspotX;
   m_beamspot[1]=beamspotY;
}

bool  FTKPattGenRoot::isBeamAtZero(void) const {
   return hypot(m_beamspot[0],m_beamspot[1])<1.E-14;
}

void FTKPattGenRoot::transformToOrigin(FTKTrack &track) {
   static double const B_FACTOR=-0.3*2.083;// conversion factor 1/Mev -> 1/mm
   // parameters defined at the nominal vertex
   double phi=track.getPhi();
   double rho=B_FACTOR*track.getHalfInvPt() *(2.0);
   double cotTheta=track.getCotTheta();
   // parameters defined at beamspot
   double d1=track.getIP();
   double z1=track.getZ0();

   double sPhi=sin(phi);
   double cPhi=cos(phi);
   double R= -m_beamspot[0]*sPhi+m_beamspot[1]*cPhi;
   double Q=  m_beamspot[0]*cPhi+m_beamspot[1]*sPhi;

   double h1=2.*d1+rho*(d1*d1-Q*Q);
   double d0=R+h1/(sqrt(rho*h1+1.)+1.);

   double s= -Q/(rho*d1+1.);
   double alpha=rho*s;
   if(fabs(alpha)>1.E-7) {
      s=asin(alpha)/rho;
   }
   double z0=z1+cotTheta*s;

   track.setIP(d0);
   track.setZ0(z0);
}

//___________________________________________________________________________________________ //
void FTKPattGenRoot::ConvertPhi(FTKTrack& track){
   //! Convert phi to the format used in the constants calculations
   //! Requires 
   //!   + m_curreg
   //!   + m_rmap
   double local_phi(track.getPhi());
   track.setPhi(remainder(local_phi,2.*M_PI));
}

#ifdef REALLY_UNUSED
   if (m_ssmap->getRegionMap()->getNumRegions()==8) {
      if (m_curreg>=3 && m_curreg<=5) {
	 while (local_phi<0) local_phi += TMath::TwoPi();
	 track.setPhi(local_phi);
      }
   } 
   else {
      if (m_curreg==9 || m_curreg==10 || m_curreg==25 || m_curreg==26 || 
	  m_curreg==41 || m_curreg==42 || m_curreg==57 || m_curreg==58 ) {
	 while (local_phi>=TMath::TwoPi()) local_phi -= TMath::TwoPi();
	 while (local_phi<0) local_phi += TMath::TwoPi();
	 track.setPhi(local_phi);
      }
   }
}
#endif

void  FTKPattGenRoot::SetModuleGeometryCheck(const std::string &fileName,
                                             SectorSelection select) {
   m_select = select;
   if(m_select==RndSector) {
      Info("SetModuleGeometryCheck")
         <<"select="<<m_select<<": no boudary check, random sector selection\n";
   } else {
      // read module geometry
      TFile *f=new TFile(fileName.c_str());
      TTree *t=0;
      f->GetObject("modulePositions",t);
      if(t) {
         int id,isPixel;
         float r[2],z[2];
         t->SetBranchAddress("isPixel",&isPixel);
         t->SetBranchAddress("id",&id);
         t->SetBranchAddress("r",r);
         t->SetBranchAddress("z",z);
         for(int i=0;i<t->GetEntries();i++) {
            t->GetEntry(i);
            RZminmax_t *module;
            if(isPixel) {
               module=&m_moduleBoundsPixel[id];
            } else {
               module=&m_moduleBoundsSCT[id];
            }
            module->rMin=r[0];
            module->rMax=r[1];
            module->zMin=z[0];
            module->zMax=z[1];
         }
      }
      delete f;
      if(m_moduleBoundsPixel.size()+
         m_moduleBoundsSCT.size()) {
         Info("SetModuleGeometryCheck")
            <<"found "<<m_moduleBoundsPixel.size()
            <<"/"<<m_moduleBoundsSCT.size()
            <<" pixel/SCT modules\n";
         if(m_select==ModuleGeometry) {
            Info("SetModuleGeometryCheck")
               <<"select="<<m_select<<" random sector within module bounds, or weighted by distance\n";
         } else if(m_select==ModuleGeometrySmallestSector) {
            Info("SetModuleGeometryCheck")
               <<"select="<<m_select<<" smallest sector ID within module bounds, or weighted by distance\n";
         } else if(m_select==ModuleGeometryBestMatch) {
            Info("SetModuleGeometryCheck")
               <<"select="<<m_select<<" smallest sector ID within module bounds, or best distance\n";
         } else {
            Error("SetModuleGeometryCheck")
               <<"select="<<" (undefined selection algorithm)\n";
         }
      } else {
         m_select= RndSector;
         Error("SetModuleGeometryCheck")
            <<"module geometry "<<fileName<<" not found\n";
         Warning("SetModuleGeometryCheck")
            <<"revert to select="<<m_select
            <<": no boudary check, random sector selection\n";
      }
   }
}
