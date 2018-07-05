#include "TrigFTKBankGen/FTKGhostHitCalculator.h"
#include "TrigFTKSim/FTKRootFile.h"
#include "TrigFTKSim/FTKRegionMap.h"
#include "TrigFTKSim/ftk_dcap.h"
#include "TrigFTKSim/FTKTruthTrack.h"
#include "TrigFTKSim/FTKSetup.h"
#include <TTree.h>


bool FTK_ModuleGeometry::extrapolate
(double rho,double cotTh,double z0,double d0,double sinPhi0,double cosPhi0,
 TVector3 const *trackVertex,double *xLocPtr,TVector2 *xPtr,double *sPtr) 
   const {
   // rough track extrapolation in phi
   TVector3 d(-d0*sinPhi0,d0*cosPhi0,z0);
   if(trackVertex) d += *trackVertex;
   TVector3 cd(m_center-d);
   double r=TMath::Hypot(cd.X(),cd.Y());
   double product=(cd.X()*cosPhi0+cd.Y()*sinPhi0)/r;
   // skip modules if track is not within +/-25 degree in phi
   if(product<0.5) {
      /*std::cout<<"id="<<m_id<<"/"<<m_plane<<" rho="<<rho<<" cotTh="<<cotTh<<" z0="<<z0<<" d0="<<d0
          <<" sinPhi="<<sinPhi0<<" cosPhi="<<cosPhi0
          <<" center="<<m_center.X()<<","<<m_center.Y()<<","<<m_center.Z()
          <<"\n";
          //exit(0); */
      return false;
   }
   TVector2 x;
   TVector3 const &dir0(m_phiAxis);
   TVector3 const &dir1(m_etaAxis);
   // iterate to get accurate extrapolation to module plane
   double s;
   bool haveS=false;
   for(int iter=0;iter<3;iter++) {
      // 1/rho may be large (or infinite)
      // but sin(s*rho)/rho  and  (1-cos(s*rho))/rho are finite
      // further complication:
      // 1-cos(s*rho) may be small -> calculation using cos() is inaccurate
      //  -> calculate using sin(s*rho/2)
      double sRho;
      if(haveS) {
         sRho=s*rho;
      } else {
         double rRho=r*rho;
         if(fabs(rRho)>1.E-10) {
            sRho=2.*asin(0.5*rRho);
            s=sRho/rho;
         } else {
            // 2*arcsin(x/2) ~= x*(1+x^2/24) 
            double corr=(1.+rRho*rRho/24.);
            s=r*corr;
            sRho=rRho*corr;
         }
      }
      double sinSrho_byRho;
      double one_minus_cosSrho_byRho;
      if(fabs(sRho)>1.E-10) {
         sinSrho_byRho=sin(sRho)/rho;
         double h=sin(0.5*sRho); 
         // h*h = sin^2(0.5*sRho)
         //     = 1/2* (sin^2(0.5*sRho)+1-cos^2(0.5*sRho))
         //     = 1/2* (1 - cos(sRho))
         one_minus_cosSrho_byRho = 2.*h*h/rho; //(1-cos(sRho))/rho;
      } else {
         // sin(x) ~= x*(1-x^2/6)
         sinSrho_byRho=s-sRho*sRho*s/6.;
         // 1-cos(x) ~= x^2*(1-x^2/24)
         one_minus_cosSrho_byRho=s*sRho*(0.5-sRho*sRho/24.);
      }
      // h is the difference vector
      // of the extrapolated helix and the module centre
      TVector3 h(cosPhi0*sinSrho_byRho-sinPhi0*one_minus_cosSrho_byRho,
                 sinPhi0*sinSrho_byRho+cosPhi0*one_minus_cosSrho_byRho,
                 s*cotTh);
      h-=cd;
      // projection on strip phi axis
      // projection on strip eta axis
      x=TVector2(dir0.Dot(h),dir1.Dot(h));
      // updated estimate of impact point on module plane minus
      TVector3 impact=cd+dir0*x.X()+dir1*x.Y();
      if(fabs(dir1.Z())>0.5) {
         // barrel module
         r=TMath::Hypot(impact.X(),impact.Y());
         haveS=false;
      } else {
         // endcap module
         s=impact.Z()/cotTh;
         haveS=true;
      }
   }
   if(xLocPtr) {
      xLocPtr[0]=(x.X()+m_width.X()*0.5)/m_pitch.X();
      xLocPtr[1]=(x.Y()+m_width.Y()*0.5)/m_pitch.Y();
   }
   if(xPtr) {
      *xPtr = x;
   }
   if(sPtr) {
      *sPtr =s;
   }
   return true;
}

FTKGhostHitCalculator::FTKGhostHitCalculator
(const FTKRegionMap *rmap,std::string const &name)
   : FTKLogging(name),m_rmap(rmap) {
   // extract module to plane mapping
   //  m_idByPlane gives for each plane the set of modules 
   m_planeFromPIXid.clear();
   m_plane_From_SCTid.clear();
   for(int tower=0;tower<m_rmap->getNumRegions();tower++) {
      for(int plane=0;plane<m_rmap->getNPlanes();plane++) {
         std::map<int,int> &planeByID=m_rmap->getPlaneMap()->isPixel(plane) ?
            m_planeFromPIXid : m_plane_From_SCTid;
         std::map<unsigned int,unsigned int> tpMap=
            m_rmap->getGlobalToLocalMapping(tower,plane);
         for(std::map<unsigned int,unsigned int>::const_iterator
                iID=tpMap.begin();iID!=tpMap.end();iID++) {
            planeByID[(*iID).first]=plane;
         }
      }
   }
}

int FTKGhostHitCalculator::loadModuleGeometry
(std::string const &name,bool markAllBad,bool invertIBLphi) {
   TDirectory *file=FTKRootFile::Instance()->OpenRootFileReadonly(name.c_str());
   if(file) {
      Info("LoadModueGeoemetry")
         <<"reading \""<<name<<"\""<<" markAllBad="<<markAllBad
         <<" invertIBLphi="<<invertIBLphi<<"\n";
      int r=loadModuleGeometry(file,markAllBad,invertIBLphi);
      delete file;
      if(!r) {
         Error("loadModuleGeometry")
            <<"failed to load module geometry from "<<name<<"\n";
      }
      return r;
   } else {
      Error("loadModuleGeometry")<<"failed to open "<<name<<" as root file\n";
      return 0;
   }
}

int FTKGhostHitCalculator::addBadModules(std::string const &name) {
   ftk_dcap::istream moduleList;
   const bool ok_read = ftk_dcap::open_for_read(name,moduleList);
   if (!moduleList || !ok_read) {
      Error("addBadModules")<<"failed to open file "<<name<<" for reading\n";
      return 0;
   } else {
      int r=addBadModules(moduleList);
      if(!r) {
         Error("addBadModules")<<"failed to read bad modules from "<<name<<"\n";
      }
      return r;
   }
}

int FTKGhostHitCalculator::addBadModules(std::istream &in) {
   int r=1;
   std::set<int> SCTmodules,PIXmodules;
   std::string line;
   int nline=0;
   while(getline(in,line)) {
      nline++;
      std::istringstream lineRead(line);
      int tmpisPixel,tmpBEC,tmpSector,tmpPlane,tmpEtaModule,tmpPhiModule,
         tmpSection,tmpidhash;
      lineRead >>tmpisPixel >>tmpBEC >>tmpSector >>tmpPlane >>tmpEtaModule
               >>tmpPhiModule >>tmpSection>>tmpidhash;
      if(lineRead.fail()) {
         Error("addBadModules")
            <<"problem to read bad modules, line="<<nline
            <<" : "<<line<<"\n";
         r=0;
         continue;
      }
      std::set<int> &moduleSet=(tmpisPixel ? PIXmodules : SCTmodules);
      if(!moduleSet.insert(tmpidhash).second) {
         Warning("addBadModules")
            <<"skipping duplicate module isPixel="<<tmpisPixel<<" id="
            <<tmpidhash<<"\n";
      }
   }
   int nPixRead=PIXmodules.size();
   int nPix=addBadPixelModules(PIXmodules);
   int nSCTRead=SCTmodules.size();
   int nSCT=addBadSCTModules(SCTmodules);
   if(nPixRead==nPix) {
      Info("addBadModules")<<"Using "<<nPix<<" dead pixel modules\n";
   } else {
      Info("addBadModules")
         <<"Using "<<nPix<<" dead pixel modules ("<<nPixRead<<" requested)\n";
   }
   if(nSCTRead==nSCT) {
      Info("addBadModules")<<"Using "<<nSCT<<" dead SCT modules\n";
   } else {
      Info("addBadModules")
         <<"Using "<<nSCT<<" dead SCT modules ("<<nSCTRead<<" requested)\n";
   }
   return r;
}

FTK_ModuleGeometry const *FTKGhostHitCalculator::findModule(size_t plane,int moduleID) const {
   FTK_ModuleGeometry const *r=0;
   if(plane<m_geometryData.size()) {
      auto modulePtr=m_geometryData[plane].find(moduleID);
      if(modulePtr!=m_geometryData[plane].end()) {
         r= &(*modulePtr).second;
      }
   }
   return r;
}

int FTKGhostHitCalculator::extrapolateTo
(size_t plane,int moduleID,double rho,double cotTh,double z0,double d0,
 double sinPhi0,double cosPhi0,TVector3 const *trackVertex,double *x) const {
   int r=0;
   if(plane<m_geometryData.size()) {
      auto modulePtr=m_geometryData[plane].find(moduleID);
      if(modulePtr!=m_geometryData[plane].end()) {
         if(!(*modulePtr).second.extrapolate
            (rho,cotTh,z0,d0,sinPhi0,cosPhi0,trackVertex,x,0,0)) {
            r=3;
         }
      } else {
         r=2;
         //Warning("extrapolateTo")
         //   <<"module "<<moduleID<<" not found in plane "<<plane<<"\n";
      }
   } else {
      r=1;
      Error("extrapolateTo")
         <<"bad plane number "<<plane<<" max="<<m_geometryData.size()<<"\n";
   }
   return r;
}

int FTKGhostHitCalculator::loadModuleGeometry
(TDirectory *source,bool markAllBad,bool invertIBLphi) {
   // initialize empty list of modules
   m_geometryData.resize(0);
   m_geometryData.resize(m_rmap->getPlaneMap()->getNPlanes());

   // read modules from TTree
   TTree *tree;
   source->GetObject("modulePositions",tree);
   int r=1;
   
   std::set<int> SCTmodules,PIXmodules;
   if(tree) {
      Int_t idhash;
      Int_t isbad,isPixel,hitSector,isBLayer;
      Int_t swapPhi,swapEta;
      Float_t center[3],phiAxis[3],etaAxis[3],width,length,phiPitch,etaPitch;
      Float_t sinTilt;
      tree->SetBranchAddress("id",&idhash);
      tree->SetBranchAddress("isPixel",&isPixel);
      tree->SetBranchAddress("isBLayer",&isBLayer);
      tree->SetBranchAddress("hitSector",&hitSector);
      tree->SetBranchAddress("width",&width);
      tree->SetBranchAddress("length",&length);
      tree->SetBranchAddress("phiPitch",&phiPitch);
      tree->SetBranchAddress("etaPitch",&etaPitch);
      tree->SetBranchAddress("phiAxis",phiAxis);
      tree->SetBranchAddress("etaAxis",etaAxis);
      tree->SetBranchAddress("center",center);
      tree->SetBranchAddress("isbad",&isbad);
      tree->SetBranchAddress("swapPhi",&swapPhi);
      tree->SetBranchAddress("swapEta",&swapEta);
      tree->SetBranchAddress("sinTilt",&sinTilt);
      m_geometryData.resize(m_rmap->getNPlanes());
      int nLoad[2],nFTK[2],nDuplicate[2];
      std::vector<int> rejected(4);
      for(int i=0;i<2;i++) {
         nLoad[i]=0;
         nFTK[i]=0;
         nDuplicate[i]=0;
      }
      for(int i=0;i<tree->GetEntries();i++) {
         tree->GetEntry(i);
         nLoad[isPixel ? 1 : 0] ++;
         int plane=-1;
         std::map<int,int> const &id2plane=
            (isPixel ? m_planeFromPIXid : m_plane_From_SCTid);
         std::map<int,int>::const_iterator iID=id2plane.find(idhash);
         if(iID != id2plane.end()) {
            plane=(*iID).second;
         } else {
            plane=isPixel ? -2 : -1;
         }
         if((plane>=0)&&(plane<(int)m_geometryData.size())) {
            nFTK[isPixel ? 1 : 0] ++;  
            std::map<int,FTK_ModuleGeometry>::iterator iModule=
               m_geometryData[plane].find(idhash);
            if(iModule!=m_geometryData[plane].end()) {
               nDuplicate[isPixel ? 1 : 0] ++;
               Warning("loadModuleGeometry")
                  <<"skipping duplicate module isPixel="
                  <<isPixel<<" id="<<idhash<<"\n";
            } else {
               FTK_ModuleGeometry &m=m_geometryData[plane][idhash];
               m.m_center.SetXYZ(center[0],center[1],center[2]);
               m.m_phiAxis.SetXYZ(phiAxis[0],phiAxis[1],phiAxis[2]);
               m.m_etaAxis.SetXYZ(etaAxis[0],etaAxis[1],etaAxis[2]);

               // fix for IBL inverted phi axis
               if(isBLayer && invertIBLphi) {
                  m.m_phiAxis *= -1.0;
               }
               //m.m_phiAxis.SetMag(1.0);
               //m.m_etaAxis.SetMag(1.0);
               double test=m.m_phiAxis.Dot(m.m_etaAxis);
               if(TMath::Abs(test)>1.E-5) {
                  Error("loadModuleGeometry")
                     <<"phi axis ["<<m.m_phiAxis.X()<<","<<m.m_phiAxis.Y()
                     <<","<<m.m_phiAxis.Z()<<"] and eta axis ["
                     <<m.m_etaAxis.X()<<","<<m.m_etaAxis.Y()
                     <<","<<m.m_etaAxis.Z()<<"] not orthogonal "<<test<<"\n";
               }
               m.m_pitch.Set(phiPitch,etaPitch);
               m.m_width.Set(width,length);
               m.m_isPixel=isPixel;
               m.m_id=idhash;
               m.m_plane=plane;
               m.m_hitSector=hitSector;
               if(isbad || markAllBad) {
                  if(isPixel) {
                     PIXmodules.insert(idhash);
                  } else {
                     SCTmodules.insert(idhash);
                  }
               }
            }
         } else {
            int iReject=isPixel ? 0 : 1;
            if(center[2]>0.) iReject+=2;
            rejected[iReject]++;
         }
      }
      delete tree;
      Info("loadModuleGeometry")
         <<"Modules found: SCT "<<nFTK[0]<<"/"<<nLoad[0]
         <<" rejected: "<<rejected[0]<<"/"<<rejected[1]
         <<"/"<<rejected[2]<<"/"<<rejected[3]
         <<" Pixel "<<nFTK[1]<<"/"<<nLoad[1]
         <<" Duplicates: SCT="<<nDuplicate[0]<<" Pixel="<<nDuplicate[1]<<"\n";
   } else {
      Error("loadModuleGeometry")<<"unable to load TTree \"modulePositions\"\n";
      r=0;
   }
   addBadPixelModules(PIXmodules);
   addBadSCTModules(SCTmodules);
   /* for(size_t plane=0;plane<m_geometryData.size();plane++) {
      std::cout<<"=========== modulelocator for plane="<<plane<<"\n";
      FTK_ModuleLocator &moduleLocator=m_moduleLocatorByPlane[plane];
      moduleLocator.print();
      } */
   return r;
}

void  FTKGhostHitCalculator::clearBadModuleList(void) {
   Info("clearBadModuleList")<<"reset bad modules, pixel "<<m_badPixel.size()
                             <<" SCT "<<m_badSCT.size()<<"\n";
   m_badPixel.clear();
   m_badSCT.clear();
   updateCalculator();
}

void FTKGhostHitCalculator::updateCalculator(void) {
   //
   m_moduleLocatorByPlane.resize(0);
   m_moduleLocatorByPlane.resize(m_geometryData.size());
   //
   for(size_t plane=0;plane<m_geometryData.size();plane++) {
      std::map<int,FTK_ModuleGeometry> const &planeData(m_geometryData[plane]);
      FTK_ModuleLocator &moduleLocator=m_moduleLocatorByPlane[plane];
      for(std::map<int,FTK_ModuleGeometry>::const_iterator im=planeData.begin();
          im!=planeData.end();im++) {
         int id=(*im).first;
         FTK_ModuleGeometry const *module=&((*im).second);
         std::set<int> const &badList=module->m_isPixel ? m_badPixel : m_badSCT;
         if(badList.find(id)!=badList.end()) {
            // insert this module into the module locator
            moduleLocator.insert(module);
         }
      }
   }
}

FTKHit *FTKGhostHitCalculator::addGhostHit
(FTKTruthTrack const &track,int plane, FTK_ModuleGeometry *module) const {
   return m_moduleLocatorByPlane[plane].locate(track,module);
}

bool FTKGhostHitCalculator::isBad(int plane,int moduleID) const {
   std::map<int,FTK_ModuleGeometry> const &planeData=m_geometryData[plane];
   std::map<int,FTK_ModuleGeometry>::const_iterator iID=
      planeData.find(moduleID);
   if(iID!=planeData.end()) {
      std::set<int> const &badSet=
         (*iID).second.m_isPixel ? m_badPixel : m_badSCT;
      return badSet.find(moduleID)!=badSet.end();
   }
   return false;
}

int FTKGhostHitCalculator::addBadPixelModules(std::set<int> const &bad) {
   int n0=m_badPixel.size();
   m_badPixel.insert(bad.begin(),bad.end());
   Info("addBadPixelModules")
      <<"inserting "<<bad.size()<<" bad modules, nBad increases from "
      <<n0<<" to "<< m_badPixel.size()<<"\n";
   updateCalculator();
   return 0;
}

int FTKGhostHitCalculator::addBadSCTModules(std::set<int> const &bad) {
   int n0=m_badSCT.size();
   m_badSCT.insert(bad.begin(),bad.end());
   Info("addBadSCTModules")
      <<"inserting "<<bad.size()<<" bad modules, nBad increases from "
      <<n0<<" to "<< m_badSCT.size()<<"\n";
   updateCalculator();
   return 0;
}

void FTK_ModuleLocator::insert(FTK_ModuleGeometry const *module) {
   std::vector<double> ri(2),zi(2);
   for(int ix=0;ix<2;ix++) {
      for(int iy=0;iy<2;iy++) {
         TVector3 pos=module->m_center
            +module->m_width.X()*(ix-0.5)*module->m_phiAxis
            +module->m_width.Y()*(iy-0.5)*module->m_etaAxis;
         double r=pos.Perp();
         double z=pos.Z();
         if((ix==0)&&(iy==0)) {
            ri[0]=r;
            ri[1]=r;
            zi[0]=z;
            zi[1]=z;
         } else {
            ri[0]=TMath::Min(r,ri[0]);
            ri[1]=TMath::Max(r,ri[1]);
            zi[0]=TMath::Min(z,zi[0]);
            zi[1]=TMath::Max(z,zi[1]);
         }
      }
   }
   int isBarrel;
   double x[2],y[2],dxMax;
   if(ri[1]-ri[0]<zi[1]-zi[0]) {
      // barrel
      isBarrel=1;
      x[0]=ri[0];
      x[1]=ri[1];
      y[0]=zi[0];
      y[1]=zi[1];
      dxMax=20.;
   } else {
      // endcap
      isBarrel=0;
      x[0]=zi[0];
      x[1]=zi[1];
      y[0]=ri[0];
      y[1]=ri[1];
      dxMax=10.;
   }
   size_t iMS;
   for(iMS=0;iMS<m_moduleSetVector.size();iMS++) {
      FTK_ModuleSet &moduleSet(m_moduleSetVector[iMS]);
      if(moduleSet.m_isBarrel != isBarrel) continue;
      double xmin=TMath::Min(x[0],moduleSet.m_x[0]);
      double xmax=TMath::Max(x[1],moduleSet.m_x[1]);
      if(xmax-xmin<=dxMax) {
         moduleSet.m_x[0]=xmin;
         moduleSet.m_x[1]=xmax;
         moduleSet.m_dy=
            TMath::Max(y[1]-y[0],moduleSet.m_dy);
         moduleSet.m_modules.insert
            (std::make_pair(0.5*(y[1]+y[0]),module));
         break;
      }
   }
   if(iMS==m_moduleSetVector.size()) {
      // start a new FTK_ModuleSet
      m_moduleSetVector.resize(iMS+1);
      FTK_ModuleSet &moduleSet(m_moduleSetVector[iMS]);
      moduleSet.m_isBarrel=isBarrel;
      moduleSet.m_x[0]=x[0];
      moduleSet.m_x[1]=x[1];
      moduleSet.m_dy=y[1]-y[0];
      moduleSet.m_modules
         .insert(std::make_pair(0.5*(y[1]+y[0]),module));
   }
}

void FTK_ModuleLocator::print(void) const {
   std::cout<<"FTK_ModuleLocator::print "<<m_moduleSetVector.size()
            <<" module sets\n";
   for(size_t i=0;i<m_moduleSetVector.size();i++) {
      FTK_ModuleSet const &moduleSet=m_moduleSetVector[i];
      std::cout<<" moduleSet["<<i<<"] ";
      if(moduleSet.m_isBarrel) {
         std::cout<<"barrel R=[";
      } else {
         std::cout<<"endcap Z=[";
      }
      std::cout<<moduleSet.m_x[0]<<","<<moduleSet.m_x[1]
               <<"] n(modules)="<<moduleSet.m_modules.size()
               <<"\n";
      for(std::multimap<double,FTK_ModuleGeometry const *>
             ::const_iterator ix=moduleSet.m_modules.begin();
          ix!=moduleSet.m_modules.end();) {
         double z=(*ix).first;
         if(moduleSet.m_isBarrel) {
            std::cout<<"  Z=["<<z;
         } else {
            std::cout<<"  R=["<<z;
         }
         std::pair<
         std::multimap<double,FTK_ModuleGeometry const *>::const_iterator,
            std::multimap<double,FTK_ModuleGeometry const *>::const_iterator>
            er=moduleSet.m_modules.equal_range(z);
         for(ix=er.first;ix!=er.second;ix++) {
            std::cout<<" "<<(*ix).second->m_id;
         }
         std::cout<<" ]\n";
      }
   }
}

FTKHit *FTK_ModuleLocator::locate(FTKTruthTrack const &track,
                                  FTK_ModuleGeometry *ret_module) const {
   static double const B_FACTOR=-0.3*2.083;
   double rho=B_FACTOR/track.getPT()*track.getQ();
   double cotTh=track.getPZ()/track.getPT();
   double z0=track.getZ();
   double d0=track.getD0();
   double sinPhi=track.getPY()/track.getPT();
   double cosPhi=track.getPX()/track.getPT();

   std::multimap<double,std::pair<TVector2,FTK_ModuleGeometry const *> >
      candidates;

   for(size_t im=0;im<m_moduleSetVector.size();im++) {
      /* std::cout<<" getCandidate moduleset["<<im<<"] rho="<<rho
          <<" cotth="<<cotTh<<" z0="<<z0<<" d0="<<d0
          <<" sinPhi="<<sinPhi<<" cosPhi="<<cosPhi<<"\n"; */
      FTK_ModuleSet const &moduleSet=m_moduleSetVector[im];
      moduleSet.getCandidates(rho,cotTh,z0,d0,sinPhi,cosPhi,
                              candidates);
   }
   // select candidate with smallest s
   FTKHit *r=0;
   for(std::multimap<double,std::pair<TVector2,FTK_ModuleGeometry const *> >
          ::const_iterator ii=candidates.begin();ii!=candidates.end();ii++) {
      /*FTK_ModuleGeometry const *module=(*ii).second.second;
       double s=module->m_center.Pt();
       std::cout<<"  CAND hash="<<module->m_id
                <<" cosphi(cent)="<<module->m_center[0]/s
                <<" sinphi(cent)="<<module->m_center[1]/s
                <<" track: "<<cosPhi<<" "<<sinPhi<<"\n"; */
   }
   if(candidates.begin()!=candidates.end()) {
      TVector2 const &x=(*candidates.begin()).second.first;
      FTK_ModuleGeometry const *module=(*candidates.begin()).second.second;
      r=new FTKHit(module->m_isPixel ? 2 : 1);
      r->setSector(module->m_hitSector);
      r->setITkMode(FTKSetup::getFTKSetup().getITkMode());
      r->setIdentifierHash(module->m_id);
      r->setPhiWidth(1);
      r->setEtaWidth(1);
      r->setPlane(module->m_plane);
      r->setCoord(0,(x.X()+module->m_width.X()*0.5)/module->m_pitch.X());
      if(module->m_isPixel) {
         r->setCoord(1,(x.Y()+module->m_width.Y()*0.5)/module->m_pitch.Y());
      }
      if(ret_module) *ret_module=*module;
   }
   return r;
}

void FTK_ModuleLocator::FTK_ModuleSet::getCandidates
(double rho,double cotTh,double z0,double d0,double sinPhi0,double cosPhi0,
 std::multimap <double,std::pair<TVector2,FTK_ModuleGeometry const *> >
 &candidate) const {
   // calculate coordinates along module extent
   //   y = Z in barrel
   //   y = R im endcap
   double y[2];
   for(int k=0;k<2;k++) {
      if(m_isBarrel) {
         // barrel m_x are Rmin and Rmax
         double r=m_x[k];
         double rrho=r*rho;
         double s=r*(1.+rrho*rrho/24.);
         y[k]=s*cotTh+z0;
      } else {
         double z=m_x[k];
         double s=(z-z0)/cotTh;
         double srho=s*rho;
         y[k]=s*(1.-srho*srho/24.);
      }
   }
   if(y[0]>y[1]) {
      double yy=y[0];
      y[0]=y[1];
      y[1]=yy;
   }
   /*std::cout<<"  isBarrel="<<m_isBarrel<<" x[]="<<m_x[0]<<" "<<m_x[1]<<" y[]="<<y[0]<<" "<<y[1]<<"\n"; */

   // loop over a range of modules
   // start at module with smallest y where y>= y[0]-0.5*dy
   for(std::multimap<double,FTK_ModuleGeometry const *>
          ::const_iterator iModule=m_modules.lower_bound(y[0]-m_dy*0.5);
       iModule!=m_modules.end();iModule++) {
      // leave loop if
      //   y> y[1]+0.5*dy
      // because all following modules have even larger y
      /*std::cout<<" check: "<<(*iModule).first<<" >= "<<(y[0]-m_dy*0.5)
        <<" is: "<<(*iModule).first<<" ?<= "<<(y[1]+m_dy*0.5)<<"?"; */
      if((y[1]+m_dy*0.5)<(*iModule).first) {
         //std::cout<<" no\n";
         break;
      }
      //std::cout<<" yes\n";
      // here we have a module with 
      //   y[0]-d/2<=y and y<=y[1]+d/2
      // ->  y[0]<= y+d/2  and y[1]>=y-d/2
      // the estimated extrapolated track yt, which is between y[0] and y[1]
      // possibly is on the module [distance d/2 around the centre y].
      FTK_ModuleGeometry const *module=(*iModule).second;

      TVector2 x;
      double s;
      module->extrapolate(rho,cotTh,z0,d0,sinPhi0,cosPhi0,0,0,&x,&s);

      // check whether coordinates are on the module
      if(TMath::Abs(x.X())>0.5*module->m_width.X()) continue;
      if(TMath::Abs(x.Y())>0.5*module->m_width.Y()) continue;
      // add to the list of results
      candidate.insert(std::make_pair(s,std::make_pair(x,module)));
   }
}
