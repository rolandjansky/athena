#include "TrigFTKBankGen/FTKDataMatrixFromMCAlgo.h"
#include "TrigFTKSim/FTKRegionMap.h"
#include "TrigFTKSim/FTKSetup.h"
#include <TTree.h>
#include <TH2F.h>
#include <TVectorD.h>
#include <TMatrixD.h>
#include <TMatrixDSymEigen.h>
#include <TError.h>
#include <vector>

using namespace std;

#define INCLUDE_SHORT_VECTOR

FTKDataMatrixFromMCAlgo::FTKDataMatrixFromMCAlgo
(const std::string& name, ISvcLocator* pSvcLocator) :
   AthAlgorithm(name,pSvcLocator),
   m_HWMODEID(2),
   m_tower(-1),
   m_invertIBLphiMatrix(1),
   m_invertIBLphiData(1),
   m_matrixVtxX(0),
   m_matrixVtxY(0),
   m_matrixVtxZ(0),
   m_rmap(0),
   m_MCmodulePositionCalculator(0),
   m_DATAmodulePositionCalculator(0),
   m_MCmatrixFile(0),
   m_DATAmatrixFile(0),
   m_debugFile(0)
{
   SetLogger(this);
   declareProperty("MCmatrixFileName",m_MCmatrixFileName);
   declareProperty("DATAmatrixFileName",m_DATAmatrixFileName);
   declareProperty("debugFileName",m_debugFileName);
   declareProperty("MCmodulePositionFileName",m_MCmodulePositionFileName);
   declareProperty("DATAmodulePositionFileName",m_DATAmodulePositionFileName);
   declareProperty("pmap_path", m_pmap_path);
   declareProperty("rmap_path",m_rmap_path);
   declareProperty("hwmodeid", m_HWMODEID);
   declareProperty("tower", m_tower);
   /* declareProperty("IBLMode",m_IBLMode);
   declareProperty("FixEndcapL0",m_fixEndcapL0);
   declareProperty("ITkMode",m_ITkMode); */
   declareProperty("ModuleLUTPath", m_modulelut_path);
   declareProperty("matrixVtxX",m_matrixVtxX);
   declareProperty("matrixVtxY",m_matrixVtxY);
   declareProperty("matrixVtxZ",m_matrixVtxZ);
   declareProperty("dataVtxX",m_dataVtxX);
   declareProperty("dataVtxY",m_dataVtxY);
   declareProperty("dataVtxZ",m_dataVtxZ);
   declareProperty("invertIBLphiMatrix",m_invertIBLphiMatrix);
   declareProperty("invertIBLphiData",m_invertIBLphiData);
}

FTKDataMatrixFromMCAlgo::~FTKDataMatrixFromMCAlgo() {
   if(m_DATAmatrixFile) delete m_DATAmatrixFile;
   if(m_MCmatrixFile) delete m_MCmatrixFile;
   if(m_debugFile) delete m_debugFile;
   if(m_MCmodulePositionCalculator) delete m_MCmodulePositionCalculator;
   if(m_DATAmodulePositionCalculator) delete m_DATAmodulePositionCalculator;
   if(m_rmap) delete m_rmap;
}

void FTKDataMatrixFromMCAlgo::PostMessage(void) {
   if     (FTKLogger::m_type==0)  ATH_MSG_FATAL(m_buffer->str());
   else if(FTKLogger::m_type==1)  ATH_MSG_ERROR(m_buffer->str());
   else if(FTKLogger::m_type==2)  ATH_MSG_WARNING(m_buffer->str());
   else if(FTKLogger::m_type==3)  ATH_MSG_INFO(m_buffer->str());
   else if(FTKLogger::m_type==4)  ATH_MSG_DEBUG(m_buffer->str());
}

StatusCode FTKDataMatrixFromMCAlgo::initialize() {
   // open root files
   int error=0;
   FTKSetup &ftkset = FTKSetup::getFTKSetup();
   ftkset.setHWModeSS(m_HWMODEID);
   /*ftkset.setIBLMode(m_IBLMode);
   ftkset.setfixEndcapL0(m_fixEndcapL0);
   ftkset.setITkMode(m_ITkMode); */
   if(m_pmap_path.empty()) {
      ATH_MSG_FATAL("no plane map specified");
   }
   if(m_rmap_path.empty()) {
      ATH_MSG_FATAL("no region map specified");
   }

   ATH_MSG_INFO("Loading plane map: "+m_pmap_path);
   FTKPlaneMap* pmap = new FTKPlaneMap(m_pmap_path.c_str());
   if (!(*pmap)) {
      ATH_MSG_FATAL("Error using plane map: "+ m_pmap_path);
   }
   
   ATH_MSG_INFO("Loading region map: "+m_rmap_path);
   m_rmap = new FTKRegionMap(pmap, m_rmap_path.c_str());

   if(m_HWMODEID==2) {
      if(m_modulelut_path.empty()) {
         ATH_MSG_FATAL("A module LUT is required with HWMODEID==2");
	 return StatusCode::FAILURE;
      } else {
         m_rmap->loadModuleIDLUT(m_modulelut_path.c_str());
      }
   }

   m_MCmatrixFile=new TFile(m_MCmatrixFileName.c_str());
   m_DATAmatrixFile=new TFile(m_DATAmatrixFileName.c_str(),"recreate");
   m_MCmodulePositionCalculator=
      new FTKGhostHitCalculator(m_rmap,"FTKGhostHitCalculator_MC");
   m_DATAmodulePositionCalculator=
      new FTKGhostHitCalculator(m_rmap,"FTKGhostHitCalculator_data");
   if(!m_MCmatrixFile->IsOpen()) {
      ATH_MSG_FATAL("Can not open input matrix file \""+
                    m_MCmatrixFileName+"\" for reading");
      error++;
   } else {
      ATH_MSG_INFO("opening input matrix file \""+m_MCmatrixFileName+"\"");
   }
   if(!m_DATAmatrixFile->IsOpen()) {
      ATH_MSG_FATAL("Can not open DATA matrix file \""+
                    m_DATAmatrixFileName+"\" for writing");
      error++;
   } else {
      ATH_MSG_INFO("opening output matrix file \""+m_DATAmatrixFileName+"\"");
   }
   if(!m_MCmodulePositionCalculator->loadModuleGeometry
      (m_MCmodulePositionFileName,true,m_invertIBLphiMatrix)) {
      ATH_MSG_FATAL("Can not load MC module position file \""+
                    m_MCmodulePositionFileName+"\" for reading");
      error++;
   }
   if(!m_DATAmodulePositionCalculator->loadModuleGeometry
      (m_DATAmodulePositionFileName,true,m_invertIBLphiData)) {
      ATH_MSG_FATAL("Can not open DATA module position file \""+
                    m_DATAmodulePositionFileName+"\" for reading");
      error++;
   }
   if(!m_debugFileName.empty()) {
      m_debugFile=new TFile(m_debugFileName.c_str(),"recreate");
      if(m_debugFile->IsOpen()) {
         ATH_MSG_INFO("Debugging information is stored in "+
                      m_debugFileName);
      } else {
         ATH_MSG_WARNING("File \""+m_debugFileName+
                         "\"for storing debug information can not be opened");
         delete m_debugFile;
         m_debugFile=0;
      }
   }
   if(error) {
      return StatusCode::FAILURE;
   } else {
      return StatusCode::SUCCESS;
   }
}

StatusCode FTKDataMatrixFromMCAlgo::execute() {
   if(!m_DATAmatrixFile) return StatusCode::FAILURE;
   // TTRee with debugging information
   TVector3 matrixVtx(m_matrixVtxX,m_matrixVtxY,m_matrixVtxZ);
   TVector3 dataVtx(m_dataVtxX,m_dataVtxY,m_dataVtxZ);
   ATH_MSG_INFO("matrix file assumed to be relative to "+
                TString::Format("(%lf,%lf,%lf)",
                                matrixVtx(0),matrixVtx(1),matrixVtx(2)));
   ATH_MSG_INFO("output matrix file will be relative to "+
                TString::Format("(%lf,%lf,%lf)",
                                dataVtx(0),dataVtx(1),dataVtx(2)));
#ifdef WRITE_DEBUG_TREE
   TTree *debugPix=0,*debugSCT=0;
   int debug_tower,debug_hash,debug_plane;
   float debug_track[5],debug_hit[2],debug_ghost[2],debug_data[2];
#endif
   vector<TH2 *> debug_coordHM(16),debug_coordMD(16),debug_resCurv(16);
   if(m_debugFile) {
      m_debugFile->cd();
#ifdef WRITE_DEBUG_TREE
      debugPix=new TTree("debugPix","debugPix");
      debugSCT=new TTree("debugSCT","debugSCT");
      debugPix->Branch("tower",&debug_tower,"tower/I");
      debugPix->Branch("plane",&debug_plane,"plane/I");
      debugPix->Branch("hash",&debug_hash,"hash/I");
      debugPix->Branch("track",debug_track,"track[5]/F");
      debugPix->Branch("hit",debug_hit,"hit[2]/F");
      debugPix->Branch("ghost",debug_ghost,"ghost[2]/F");
      debugPix->Branch("data",debug_data,"data[2]/F");
      debugSCT->Branch("tower",&debug_tower,"tower/I");
      debugSCT->Branch("plane",&debug_plane,"plane/I");
      debugSCT->Branch("hash",&debug_hash,"hash/I");
      debugSCT->Branch("track",debug_track,"track[5]/F");
      debugSCT->Branch("hit",debug_hit,"hit/F");
      debugSCT->Branch("ghost",debug_ghost,"ghost/F");
      debugSCT->Branch("data",debug_data,"data/F");
#endif
      for(size_t i=0;i<debug_coordHM.size();i++) {
         double w=800;
         if(i<8) {
            if(i&1) w=160;
            else w=360.;
         }
         double x0=-0.1*w;
         double x1=1.1*w;
         debug_coordHM[i]=new TH2F(TString::Format("coordHM%d",(int)i),
                                   ";hit;matrix",50,x0,x1,50,x0,x1);
         debug_coordMD[i]=new TH2F(TString::Format("coordMD%d",(int)i),
                                   ";matrix;output",50,x0,x1,50,x0,x1);
         debug_resCurv[i]=new TH2F(TString::Format("resCurv%d",(int)i),
                                   ";matrix;output",50,-0.001,0.001,50,-0.3*w,0.3*w);
      }
   }

   // output directory
   m_DATAmatrixFile->cd();
   // copy tree "slice"
   TTree *slice_tree;
   m_MCmatrixFile->GetObject("slice",slice_tree);
   if(slice_tree) {
        double tmp_cmax;
        double tmp_cmin;
        int tmp_cslices;
  
        double tmp_phimax;
        double tmp_phimin;
        int tmp_phislices;
  
        double tmp_d0max;
        double tmp_d0min;
        int tmp_d0slices;
  
        double tmp_z0max;
        double tmp_z0min;
        int tmp_z0slices;
  
        double tmp_etamax;
        double tmp_etamin;
        int tmp_etaslices;

        slice_tree->SetBranchAddress("c_max",&tmp_cmax);
        slice_tree->SetBranchAddress("c_min", &tmp_cmin);
        slice_tree->SetBranchAddress("c_slices", &tmp_cslices);
        slice_tree->SetBranchAddress("phi_max", &tmp_phimax);
        slice_tree->SetBranchAddress("phi_min", &tmp_phimin);
        slice_tree->SetBranchAddress("phi_slices", &tmp_phislices);
        slice_tree->SetBranchAddress("d0_max", &tmp_d0max);
        slice_tree->SetBranchAddress("d0_min", &tmp_d0min);
        slice_tree->SetBranchAddress("d0_slices", &tmp_d0slices);
        slice_tree->SetBranchAddress("z0_max", &tmp_z0max);
        slice_tree->SetBranchAddress("z0_min", &tmp_z0min);
        slice_tree->SetBranchAddress("z0_slices", &tmp_z0slices);
        slice_tree->SetBranchAddress("eta_max", &tmp_etamax);
        slice_tree->SetBranchAddress("eta_min", &tmp_etamin);
        slice_tree->SetBranchAddress("eta_slices", &tmp_etaslices);
        slice_tree->GetEntry(0); 
        delete slice_tree;
        slice_tree=0;

        m_DATAmatrixFile->cd();
        slice_tree=new TTree("slice","slice");
        slice_tree->Branch("c_max",&tmp_cmax);
        slice_tree->Branch("c_min", &tmp_cmin);
        slice_tree->Branch("c_slices", &tmp_cslices);
        slice_tree->Branch("phi_max", &tmp_phimax);
        slice_tree->Branch("phi_min", &tmp_phimin);
        slice_tree->Branch("phi_slices", &tmp_phislices);
        slice_tree->Branch("d0_max", &tmp_d0max);
        slice_tree->Branch("d0_min", &tmp_d0min);
        slice_tree->Branch("d0_slices", &tmp_d0slices);
        slice_tree->Branch("z0_max", &tmp_z0max);
        slice_tree->Branch("z0_min", &tmp_z0min);
        slice_tree->Branch("z0_slices", &tmp_z0slices);
        slice_tree->Branch("eta_max", &tmp_etamax);
        slice_tree->Branch("eta_min", &tmp_etamin);
        slice_tree->Branch("eta_slices", &tmp_etaslices);
        slice_tree->Fill();
        slice_tree->Write(0,TObject::kOverwrite);

   } else {
      ATH_MSG_ERROR("no slice tree found in \""+m_MCmatrixFileName+"\"");
   }
   // copy tree "montree"
   /*TTree *montree;
   m_MCmatrixFile->GetObject("montree",montree);
   if(montree) {
      montree->Write();
   } else {
      ATH_MSG_WARNING("no montree tree found in \""+m_MCmatrixFileName+"\"");
      } */
   gErrorAbortLevel=kError;
   // loop over all am trees
   for(int tower=0;tower<64;tower++) {
      if((m_tower>=0)&&(tower!=m_tower)) continue;
      TTree *mcTree;
      TString amName=TString::Format("am%d",tower); 
      m_MCmatrixFile->GetObject(amName,mcTree);
#ifdef WRITE_DEBUG_TREE
      debug_tower=tower;
#endif
      if(!mcTree) {
         ATH_MSG_ERROR("no tree \""+amName+"\" found in \""+
                       m_MCmatrixFileName+"\"");
      } else {
         ATH_MSG_INFO(TString::Format("Processing tower %d",tower) );
         vector<int> errorCountPlane(m_rmap->getPlaneMap()->getNPlanes());
         map<int,int> badModulePix,badModuleSCT;
         int errorCountE=0;
         int errorCountH=0;
         // rename input tree
         mcTree->SetName("tmptree");
         int ndim,ndim2,nplanes;
         mcTree->SetBranchAddress("ndim",&ndim);
         mcTree->SetBranchAddress("ndim2",&ndim2);
         mcTree->SetBranchAddress("nplanes",&nplanes);
         mcTree->GetEntry(0);
         ATH_MSG_INFO(TString::Format("ndim=%d ndim2=%d nplanes=%d",
                                      ndim,ndim2,nplanes) );
         double *Vec=new double[ndim];
         mcTree->SetBranchAddress("Vec",Vec);
         int *sectorID=new int[nplanes];
         mcTree->SetBranchAddress("sectorID",sectorID);
         int *hashID=new int[nplanes];
         mcTree->SetBranchAddress("hashID",hashID);
         float nhit;
         mcTree->SetBranchAddress("nhit",&nhit);
         double tmp_C_D_Phi_Coto_Z[5];
         mcTree->SetBranchAddress("tmpC",tmp_C_D_Phi_Coto_Z+0);
         mcTree->SetBranchAddress("tmpD",tmp_C_D_Phi_Coto_Z+1);
         mcTree->SetBranchAddress("tmpPhi",tmp_C_D_Phi_Coto_Z+2);
         mcTree->SetBranchAddress("tmpCoto",tmp_C_D_Phi_Coto_Z+3);
         mcTree->SetBranchAddress("tmpZ",tmp_C_D_Phi_Coto_Z+4);
         double *tmpx_C_D_Phi_Coto_Z=new double[5*ndim];
         mcTree->SetBranchAddress("tmpxC",tmpx_C_D_Phi_Coto_Z+0*ndim);
         mcTree->SetBranchAddress("tmpxD",tmpx_C_D_Phi_Coto_Z+1*ndim);
         mcTree->SetBranchAddress("tmpxPhi",tmpx_C_D_Phi_Coto_Z+2*ndim);
         mcTree->SetBranchAddress("tmpxCoto",tmpx_C_D_Phi_Coto_Z+3*ndim);
         mcTree->SetBranchAddress("tmpxZ",tmpx_C_D_Phi_Coto_Z+4*ndim);
         double *tmpcovx=new double[ndim2];
         mcTree->SetBranchAddress("tmpcovx",tmpcovx);
#ifdef INCLUDE_SHORT_VECTOR
         std::vector<short>
            *tmpintc=new std::vector<short>,
            *tmpintphi=new std::vector<short>,
            *tmpintd0=new std::vector<short>,
            *tmpintz0=new std::vector<short>,
            *tmpinteta=new std::vector<short>;
         mcTree->SetBranchAddress("tmpintc",&tmpintc);
         mcTree->SetBranchAddress("tmpintphi",&tmpintphi);
         mcTree->SetBranchAddress("tmpintd0",&tmpintd0);
         mcTree->SetBranchAddress("tmpintz0",&tmpintz0);
         mcTree->SetBranchAddress("tmpinteta",&tmpinteta);
#endif
         // create output tree
         m_DATAmatrixFile->cd();
         TTree *dataTree=new TTree
            (amName,TString::Format("Ambank %d para",tower));

         dataTree->Branch("ndim",&ndim,"ndim/I");
         dataTree->Branch("ndim2",&ndim2,"ndim2/I");
         dataTree->Branch("nplanes",&nplanes,"nplanes/I");
         dataTree->Branch("Vec", Vec,"Vec[ndim]/D");
         dataTree->Branch("sectorID",sectorID,"sectorID[nplanes]/I");
         dataTree->Branch("hashID",hashID,"hashID[nplanes]/I");
         dataTree->Branch("nhit", &nhit,"nhit/F");\
         dataTree->Branch("tmpC",tmp_C_D_Phi_Coto_Z+0,"tmpC/D");
         dataTree->Branch("tmpD",tmp_C_D_Phi_Coto_Z+1,"tmpD/D");
         dataTree->Branch("tmpPhi",tmp_C_D_Phi_Coto_Z+2,"tmpPhi/D");
         dataTree->Branch("tmpCoto",tmp_C_D_Phi_Coto_Z+3,"tmpCoto/D");
         dataTree->Branch("tmpZ",tmp_C_D_Phi_Coto_Z+4,"tmpZ/D");
         dataTree->Branch("tmpxC",tmpx_C_D_Phi_Coto_Z+0*ndim,"tmpxC[ndim]/D");
         dataTree->Branch("tmpxD",tmpx_C_D_Phi_Coto_Z+1*ndim,"tmpxD[ndim]/D");
         dataTree->Branch("tmpxPhi",tmpx_C_D_Phi_Coto_Z+2*ndim,"tmpxPhi[ndim]/D");
         dataTree->Branch("tmpxCoto",tmpx_C_D_Phi_Coto_Z+3*ndim,"tmpxCoto[ndim]/D");
         dataTree->Branch("tmpxZ",tmpx_C_D_Phi_Coto_Z+4*ndim,"tmpxZ[ndim]/D");
         dataTree->Branch("tmpcovx",tmpcovx,"tmpcovx[ndim2]/D");
         // drop these for tests
#ifdef INCLUDE_SHORT_VECTOR
         dataTree->Branch("tmpintc", &tmpintc);
         dataTree->Branch("tmpintphi", &tmpintphi);
         dataTree->Branch("tmpintd0", &tmpintd0);
         dataTree->Branch("tmpintz0", &tmpintz0);
         dataTree->Branch("tmpinteta", &tmpinteta);
#endif
         // loop over all entries (c.f. sectors)
         for(int iEnt=0;iEnt<mcTree->GetEntries(); iEnt++) {
            mcTree->GetEntry(iEnt);
            if(nhit<ndim) {
               errorCountH++;
               continue;
            }
            // transform here the coordinates to the new module geometry
            TVectorD avgT(5);
            TVectorD avgX(ndim);
            TMatrixD avgTX(5,ndim);
            TMatrixDSym avgXX(ndim);
            // <t>
            for(int i=0;i<5;i++) {
               avgT(i)=tmp_C_D_Phi_Coto_Z[i]/nhit;
            }
            // <x>
            for(int i=0;i<ndim;i++) {
               avgX(i)=Vec[i]/nhit;
            }
            // <tx>
            for(int i=0;i<5;i++) {
               for(int j=0;j<ndim;j++) {
                  avgTX(i,j)=tmpx_C_D_Phi_Coto_Z[j+ndim*i]/nhit-avgX(j)*avgT(i);
               }
            }
            // <xx>-<x>*<x>
            for(int j=0;j<ndim;j++) {
               for(int i=0;i<=j;i++) {
                  avgXX(i,j)=tmpcovx[i*ndim+j]/nhit-avgX(i)*avgX(j);
                  avgXX(j,i)=avgXX(i,j);
               }
            }
            TMatrixDSymEigen EXX(avgXX);
            TVectorD eval=EXX.GetEigenValues();
            TMatrixD evec=EXX.GetEigenVectors();
            TMatrixDSym H(ndim);
            int error=0;
            for(int i=0;i<ndim;i++) {
               for(int j=0;j<ndim;j++) {
                  for(int k=0;k<ndim;k++) {
                     if(eval(k)<=0.0) {
                        error++;
                        break;
                     }
                     H(i,j) += evec(i,k)*evec(j,k)/eval(k);
                  }
                  if(error) break;
               }
               if(error) break;
            }
            // if there is an error, skip further corrections
            if(error) {
               errorCountH++;
               /* cout<<"nhit="<<nhit<<"\n";
               cout<<"avgT\n";
               avgT.Print();
               cout<<"avgX\n";
               avgX.Print();
               cout<<"avgTX\n";
               avgTX.Print();
               cout<<"avgXX\n";
               avgXX.Print();
               exit(0); */
            } else {
               // matrix A:  t = A*(x-t0) + t0
               //   TMatrixD A(avgTX,TMatrixD::kMult,H);
               //
               // <tt>-<t>*<t>
               TMatrixDSym avgTT(H.Similarity(avgTX));
               // eigenvalue decomposition
               TMatrixDSymEigen ETT(avgTT);
               TMatrixD T=ETT.GetEigenVectors();
               TMatrixD Tinv(T);
               TVectorD evT=ETT.GetEigenValues();
               for(int i=0;i<5;i++) {
                  for(int j=0;j<5;j++) {
                     double sqrtEV=sqrt(evT(j));
                     T(i,j) *= sqrtEV;
                     Tinv(i,j) /= sqrtEV;
                  }
               }
               TMatrixD BTinv(avgTX,TMatrixD::kTransposeMult,Tinv);
               //
               // calculate alignment differences for DATA-MC
               // for central track positions and shifted 
               // by +/- eigenvectors
               TVectorD d_mcX(ndim);
               TMatrixD d_mcXT(ndim,5);
               TMatrixD d_mcXX(ndim,ndim);
               int planeError=0;
               for(int n=0;n<11;n++) {
                  // n=0:   <t>
                  // n=1,2  <t> +/- <T0>
                  // n=3,4  <t> +/- <T1>
                  //    ...
                  // n=9,10  <t> +/- <T4>
                  TVectorD track(avgT);
                  double sign= (n&1) ? 1. : -1.;
                  int iEV=(n-1)/2;
                  if(n) {
                     for(int j=0;j<5;j++) {
                        track(j) +=sign*T(j,iEV);
                     }
                  }
                  //cout<<"track\n";
                  //track.Print();
                  static double const B_FACTOR=-0.3*2.083;
                  double Q_over_pt=2.0*track(0);
                  double rho=B_FACTOR* Q_over_pt;
                  double d0=track(1);
                  double phi=track(2);
                  double cotTh=track(3);
                  double z0=track(4);
                  double sinPhi0=sin(phi);
                  double cosPhi0=cos(phi);
                  int icoord=0;
                  TVectorD d_mc(ndim);
                  for(int plane=0;plane<nplanes;plane++) {
                     int hash=hashID[plane];
                     double xData[2],xMC[2];
                     int errorMC=m_MCmodulePositionCalculator->
                        extrapolateTo(plane,hash,rho,cotTh,z0,d0,
                                      sinPhi0,cosPhi0,&matrixVtx,xMC);
                     int errorData=m_DATAmodulePositionCalculator->
                        extrapolateTo(plane,hash,rho,cotTh,z0,d0,
                                       sinPhi0,cosPhi0,&dataVtx,xData);
                     int ncoord=m_rmap->getPlaneMap()->isSCT(plane) ? 1 : 2;
                     if(errorMC||errorData) {
                        if(ncoord==2) {
                           badModulePix[hash]|=(1<<(errorMC+8))|(1<<errorData);
                        } else {
                           badModuleSCT[hash]|=(1<<(errorMC+8))|(1<<errorData);
                        }
                        planeError |= (1<<plane);
                        error++;
                     } else {
                        for(int j=0;j<ncoord;j++) {
                           d_mc(icoord+j) = xData[j]-xMC[j];
                        }
#ifdef WRITE_DEBUG_TREE
                        if((n==0) && (m_debugFile)) {
                           debug_hash=hash;
                           debug_plane=plane;
                           for(int j=0;j<ncoord;j++) {
                              debug_hit[j]=avgX(icoord+j);
                              debug_ghost[j]=xMC[j];
                              debug_data[j]=xData[j];
                           }
                           for(int i=0;i<5;i++) {
                              debug_track[i]=track(i);
                           }
                           /*FTK_ModuleGeometry const *module=
                              m_MCmodulePositionCalculator->
                              findModule(plane,hash); */
                           if(ncoord==2) {
                              debugPix->Fill();
                           } else {
                              debugSCT->Fill();
                           }
                        }
#endif
                        if(m_debugFile) {
                           // fill monitoring histograms
                           for(int j=0;j<ncoord;j++) {
                              if(debug_coordHM[icoord+j])
                                 debug_coordHM[icoord+j]->Fill
                                    (avgX(icoord+j),xMC[j]);
                              if(debug_coordMD[icoord+j])
                                 debug_coordMD[icoord+j]->Fill
                                    (xMC[j],xData[j]);
                              if(debug_resCurv[icoord+j])
                                 debug_resCurv[icoord+j]->Fill
                                    (xMC[j]-avgX(icoord+j),track(0));
                           }
                        }
                     }
                     icoord += ncoord;
                  }
                  if(n==0) {
                     // average difference data-mc
                     d_mcX=d_mc;
                  } else {
                     TVectorD deltaX(d_mc-d_mcX);
                     for(int i=0;i<ndim;i++) {
                        for(int k=0;k<5;k++) {
                           // sum of deltaX*deltaT
                           d_mcXT(i,k) += 0.5*deltaX(i)*sign*T(k,iEV);
                        }
                        for(int j=0;j<ndim;j++) {
                           // sums of deltaX*deltaX and correlations
                           // involving B=avgTX
                           d_mcXX(i,j) += 0.5*
                              (deltaX(i)*sign*BTinv(j,iEV)+
                               deltaX(j)*sign*BTinv(i,iEV)+
                               deltaX(i)*deltaX(j));
                        }
                     }
                  }
               }
               if(!error) {
                  // apply alignment changes to the coordinate sums

                  // coordinate shifts (vector d)
                  float nhit0=nhit;
                  //nhit=0;
                  for(int i=0;i<ndim;i++) {
                     Vec[i]+= d_mcX(i)*nhit;
                  }
                  // rotations (matrices M and E)
                  for(int i=0;i<5;i++) {
                     for(int j=0;j<ndim;j++) {
                        tmpx_C_D_Phi_Coto_Z[j+ndim*i]+=nhit*
                           (d_mcX(j)*avgT(i)+d_mcXT(j,i));
                     }
                  }
                  for(int j=0;j<ndim;j++) {
                     for(int i=0;i<=j;i++) {
                        tmpcovx[i*ndim+j]+= nhit*
                           (avgX(i)*d_mcX(j)+avgX(j)*d_mcX(i)+
                            d_mcX(i)*d_mcX(j)+d_mcXX(i,j));
                        tmpcovx[j*ndim+i]=tmpcovx[i*ndim+j];
                     }
                  }
                  nhit=nhit0;
               } else {
                  for(size_t k=0;k<errorCountPlane.size();k++) {
                     if(planeError & (1<<k)) errorCountPlane[k]++;
                     //cout<<"plane "<<k<<" error\n";
                  }
                  errorCountE++;
                  //exit(0);
               }
            }

            if((errorCountH |errorCountE)==0) {
               dataTree->Fill();
            }

            /*if(//(iEnt<10)||
               //((iEnt<100)&&(iEnt%10 ==0))||
               //((iEnt<1000)&&(iEnt%100 ==0))||
               ((iEnt<10000)&&(iEnt%1000 ==0))||
               ((iEnt%10000 ==0))) {
               ATH_MSG_INFO(TString::Format
                            ("sector %d nErrorE=%d nErrorH=%d",
                             iEnt,errorCountE,errorCountH));
                             } */
         }

         // save to output file
         m_DATAmatrixFile->cd();
         dataTree->Write(0,TObject::kOverwrite);

         if(errorCountE|errorCountH) {
            TString errorMsg=TString::Format
               ("tower=%d errors (%d+%d)/%lld sectors with error",
                tower,errorCountE,errorCountH,
                mcTree->GetEntries());
            if(errorCountE) {
               errorMsg+=" by plane:";
               for(size_t k=0;k<errorCountPlane.size();k++) {
                  errorMsg += TString::Format(" %d",errorCountPlane[k]);
               }
            }
            ATH_MSG_WARNING(errorMsg);
            if(badModulePix.size()) {
               TString pixErrors("pixel module errors");
               for(auto m=badModulePix.begin();m!=badModulePix.end();m++) {
                  pixErrors+=TString::Format(" %d:%x",(*m).first,(*m).second);
               }
               ATH_MSG_WARNING(pixErrors);
            }
            if(badModuleSCT.size()) {
               TString sctErrors("SCT module errors");
               for(auto m=badModuleSCT.begin();m!=badModuleSCT.end();m++) {
                  sctErrors+=TString::Format(" %d:%x",(*m).first,(*m).second);
               }
               ATH_MSG_WARNING(sctErrors);
            }
         }

         delete [] tmpcovx;
         delete [] tmpx_C_D_Phi_Coto_Z;
         delete [] hashID;
         delete [] sectorID;
         delete [] Vec;
      }
   }
   if(m_DATAmatrixFile) delete m_DATAmatrixFile;
   m_DATAmatrixFile=0;
#ifdef WRITE_DEBUG_TREE
   if(debugPix) {
      m_debugFile->cd();
      debugPix->Write(0,TObject::kOverwrite);
   }
   if(debugSCT) {
      m_debugFile->cd();
      debugSCT->Write(0,TObject::kOverwrite);
   }
#endif
   for(size_t i=0;i<debug_coordHM.size();i++) {
      if(m_debugFile) m_debugFile->cd();
      if(debug_coordHM[i]) debug_coordHM[i]->Write(0,TObject::kOverwrite);
      if(debug_coordMD[i]) debug_coordMD[i]->Write(0,TObject::kOverwrite);
      if(debug_resCurv[i]) debug_resCurv[i]->Write(0,TObject::kOverwrite);
   }
   if(m_debugFile) delete m_debugFile;
   m_debugFile=0;
   return StatusCode::SUCCESS;
}

StatusCode FTKDataMatrixFromMCAlgo::finalize() {
   return StatusCode::SUCCESS;
}
