// emacs, this file is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>
#undef NDEBUG

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
  
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "TFile.h"
#include "TTree.h"

struct ClusterMomentSetter {
  static TTree* tree ;
  
  ClusterMomentSetter(std::string name, xAOD::CaloCluster::MomentType m) : m_data(0), m_momtype(m) {
    tree->SetBranchAddress(name.c_str(), &m_data);
  }

  void fillCluster(size_t i, xAOD::CaloCluster* cl){
    cl->insertMoment( m_momtype, (*m_data)[i]);
  }

  std::vector< float > *m_data;
  xAOD::CaloCluster::MomentType m_momtype;
};

TTree * ClusterMomentSetter::tree = 0;

#define CREATEMOMSETTER( n ) ClusterMomentSetter( "CaloCalTopoClusterAuxDyn."#n , xAOD::CaloCluster::n )

namespace ClusterData {

  xAOD::CaloClusterContainer* clusterCont = new  xAOD::CaloClusterContainer();
  xAOD::JetContainer* jetCont = new xAOD::JetContainer();


  int testEL(){
    //const xAOD::IParticleContainer *cont = dynamic_cast<const xAOD::IParticleContainer*>( ClusterData::clusterCont[0]->container() ); 
    //std::cout << " testEL : "<< cont << "  "<< &ClusterData::clusterCont << std::endl;
    ElementLink<xAOD::IParticleContainer> el(*ClusterData::clusterCont,0);
    return 0;
  }



  void fillContainer(const std::string &fname ){
    static bool auxStoreAdded = false;
    if( ! auxStoreAdded ){
      xAOD::CaloClusterAuxContainer* aux = new xAOD::CaloClusterAuxContainer();
      clusterCont->setStore(aux);
      jetCont->setStore(    new xAOD::JetAuxContainer() ) ;
      auxStoreAdded = true;
    }
    
    TFile * input = TFile::Open(fname.c_str());
    
    TTree *tree = (TTree*) input->Get("CollectionTree");
    ClusterMomentSetter::tree = tree;
    
    std::vector<ClusterMomentSetter> momsetters = {
      CREATEMOMSETTER( AVG_TILE_Q), 
      CREATEMOMSETTER( AVG_LAR_Q), 
      CREATEMOMSETTER( BADLARQ_FRAC), 
      CREATEMOMSETTER( ENG_POS), 
      CREATEMOMSETTER( SIGNIFICANCE),
      // CREATEMOMSETTER( CENTER_X ),
      // CREATEMOMSETTER( CENTER_Y ),
      // CREATEMOMSETTER( CENTER_Z )
    };


    // horrible hack ?
    size_t ncl = tree->Draw("CaloCalTopoClusterAux.calE:CaloCalTopoClusterAux.calEta:CaloCalTopoClusterAux.calPhi:CaloCalTopoClusterAux.rawE","","goff",1);
    double *e =   tree->GetV1();
    double *eta = tree->GetV2();
    double *phi = tree->GetV3();
    double *rawe =   tree->GetV4();

    tree->SetBranchStatus("*",0);
    tree->SetBranchStatus("CaloCalTopoClusterAux*",1);
    tree->GetEntry(0);

    for(size_t i=0;i<ncl; i++){
      xAOD::CaloCluster *cl = new xAOD::CaloCluster();
#define SETCLUSTERMOM( E, eta, phi ) cl->setE(E);cl->setEta(eta);cl->setPhi(phi);cl->setM(0)
#define SETCLUSTERRAWMOM( E, eta, phi ) cl->setRawE(E);cl->setRawEta(eta);cl->setRawPhi(phi);cl->setRawM(0)
      clusterCont->push_back(cl);

      SETCLUSTERMOM( e[i], eta[i], phi[i] );
      SETCLUSTERRAWMOM( rawe[i], eta[i], phi[i] );

      for( auto & setter : momsetters) setter.fillCluster(i, cl );
      if(i < 3 ){
        double sig;
        cl->retrieveMoment(xAOD::CaloCluster::SIGNIFICANCE, sig);
        std::cout << " cluster e="<< cl->e() << " eta="<< cl->eta() << "  sig="<<sig<<std::endl; 
      }
    }

    // redo a loop for time
    ncl = tree->Draw("CaloCalTopoClusterAux.time", "","goff",1);
    e =   tree->GetV1();
    for(size_t i=0;i<ncl; i++){ (*clusterCont)[i]->setTime( e[i] ); }

    // std::cout << " testing EL "<< std::endl;
    // testEL();
    // std::cout << " tested EL "<< std::endl;


    // ***************************************************
    /// Fill jets
    // ***************************************************
    tree->SetBranchStatus("*",1);
    for(size_t i=0;i<3;i++){
      size_t nconst = tree->Draw(TString::Format("AntiKt4LCTopoJetsAux.constituentLinks.ElementLinkBase.m_persIndex[%d]",int(i)) ,"","goff",1);
      double *ind = tree->GetV1();
      jetCont->push_back(new xAOD::Jet());
      xAOD::Jet *jet = jetCont->back();
      
      TLorentzVector sum;
      for(size_t c=0; c<nconst; c++) {
        xAOD::CaloCluster* cl = (*clusterCont)[ size_t(ind[c]) ];
        jet->addConstituent( cl );
        sum += cl->p4();
      }
      jet->setJetP4( xAOD::JetFourMom_t( sum.Pt(), sum.Eta(), sum.Phi(), sum.M() ) );
      std::cout << " jet  e="<< jet->e() << " eta="<< jet->eta() <<std::endl; 
    }

    delete tree;
    delete input;
  }
    



  
}



// // Values read from 1st event in 
// // /afs/cern.ch/atlas/groups/JetEtmiss/ReferenceFiles/RTT/MC/AOD/mc12_8TeV.117050.PowhegPythia_P2011C_ttbar.simul.AOD.e1728_s1581.19_1_1_5.039772.pool.root.1
// std::vector< std::vector<float> > clustersKin = { 
//   /// e, eta , phi (mass are 0)
//   { 26832.550 , -0.927390 , -0.037607 } };
//   { 21128.115 , -0.974543 , -0.129331 },
//   { 16133.963 , 0.6404343 , 1.5678019 },
//   { 24615.232 , -0.847577 , -0.096929 },
//   { 21233.148 , 0.6236842 , 1.5764616 },
//   { 23889.429 , -1.318650 , -2.001048 },
//   { 10709.591 , 0.2633796 , -2.914640 },
//   { 11407.520 , 0.5576553 , 1.5582702 },
//   { 10282.594 , 0.1783299 , -2.932228 },
//   { 8559.1162 , 0.2289998 , -2.813697 },
//   { 12512.816 , -0.467060 , -2.213232 },
//   { 8694.8271 , 0.6143872 ,  1.499318 },
//   { 13862.853 , 1.2454798 , -1.383631 },
//   { 7633.9887 , 0.7124757 , 1.6031537 },
//   { 10279.566 , -1.027477 , 0.0442401 },
//   { 4710.3632 , 0.0869960 , -2.890529 },
//   { 9425.7832 , -1.240470 , -2.087149 },
//   { 4974.9497 , 0.1030960 , 3.0754895 },
//   { 3011.0690 , 0.2345674 , -2.636965 },
//   { 52374.367 , -3.406755 , -0.175965 },
//   { 6298.2880 , 1.2256604 , -1.627055 },
//   { 5391.7578 , 0.6943342 , 1.4846857 },
//   { 3740.2099 , 0.5919813 , -2.996334 },
//   { 3656.1218 , 0.1702851 , 3.1053152 },
//   { 2906.7036 , 0.1156901 , -1.146971 },
//   { 3366.6894 , 0.3325673 , 3.1040334 },
//   { 6731.7949 , 0.7328419 , 1.4803007 },
//   { 2315.1855 , 0.3868140 , -2.762619 },
//   { 1966.4317 , -0.247496 , -0.626801 },
//   { 2321.8823 , -0.179334 , -2.248436 },
//   { 2184.7873 , -0.462879 , -3.117369 },
//   { 6048.0371 , -1.846866 , 2.4149701 },
//   { 2619.1496 , -0.790691 , 0.1326492 },
//   { 3415.1042 , -0.912595 , 0.3786779 },
//   { 39987.242 , -3.625080 , -0.445425 },
//   { 1765.4661 , 0.1200857 , 2.5403938 },
//   { 1901.0018 , 0.1737768 , -2.025693 },
//   { 28364.599 , -3.000594 , -0.880366 },
//   { 1666.1423 , -0.412515 , 2.3158392 },
//   { 1379.2968 , 0.1491050 , -1.728081 },
//   { 24318.703 , 3.0791435 , -1.102431 },
//   { 1790.1508 , -0.539304 , -1.991287 },
//   { 2585.6386 , 1.3079475 , -2.207916 },
//   { 1208.0795 , -0.016053 , -2.983653 },
//   { 4264.6201 , 1.7590383 , -2.018674 },
//   {    9862.5 , 2.4786906 , -2.179607 },
//   { 3869.4834 , 0.8289600 , 1.5724415 },
//   { 2466.9939 , 1.1677756 , -1.718844 },
//   { 6024.6323 , -1.985751 , 2.2821452 },
//   { 4181.3588 , 0.7406598 , 1.7162662 },
//   { 3080.5119 , -1.377711 , 2.7981212 },
//   { 4032.3955 , -1.751309 , 1.3349471 },
//   { 1452.8774 , -0.699657 , -1.904385 },
//   { 7521.0688 , 2.7695200 , 2.9697728 },
//   { 1266.6035 , -0.299054 , -2.793029 },
//   { 21301.679 , 3.3826663 , 0.7025466 },
//   { 18705.076 , -3.384880 , -0.319797 },
//   { 5495.6269 , -2.184863 , -0.288976 },
//   { 2623.8503 , -1.199948 , -2.972216 },
//   { 1435.1069 , 0.3628215 , 0.4421064 },
//   { 3518.9316 , -1.699695 , 1.3566939 },
//   { 5330.7334 , 1.7943863 , 0.1412324 },
//   { 4603.1816 , -2.093743 , -0.242667 },
//   { 4630.0097 , -1.862495 , 1.3002706 },
//   { 44073.980 , -4.533745 , -2.442840 },
//   { 11146.236 , -3.018893 , -0.582349 },
//   { 4032.0510 , -1.976584 , 2.6637611 },
//   { 1094.8365 , 0.0829946 , -1.420733 },
//   { 1084.5499 , 0.6038413 , 2.0948739 },
//   { 1010.5720 , -0.609398 , 1.1016989 },
//   { 976.47930 , -0.565925 , -1.412239 },
//   { 8096.5288 , 2.5779559 , 0.3346863 },
//   { 1042.6391 , -0.533139 , 0.6393038 },
//   { 2074.5253 , -1.245137 , 0.3447599 },
//   { 1512.1879 , 0.9284248 , 1.9448241 },
//   { 986.40673 , -0.430380 , 2.0307126 },
//   { 756.27227 , 0.2885856 , -2.509693 },
//   { 721.37445 , 0.3396269 , -2.897682 },
//   { 1455.1937 , -0.886376 , -1.291688 },
//   { 4533.4506 , -2.149812 , -0.534796 },
//   { 989.76477 , -0.499282 , -3.090538 },
//   { 6262.7661 , -2.214601 , 0.9383000 },
//   { 19256.517 , -3.394595 , -2.595745 },
//   { 1184.1771 , 0.8461242 , -0.930921 },
//   { 949.22943 , 0.7341871 , -2.672912 },
//   { 933.14929 , 0.7401811 , 1.1079863 },
//   { 710.58868 , -0.451216 , -2.409091 },
//   { 5014.9873 , 2.2572906 , -0.103761 },
//   { 3257.4689 , 2.0033330 , -2.012591 },
//   { 742.31994 , -0.380724 , 0.2579212 },
//   { 771.79632 , 0.6179624 , 1.5230137 },
//   { 738.75787 , -0.436029 , 1.9206278 },
//   { 4415.5449 , 2.3193347 , -1.301353 },
//   { 762.92053 , 0.6715003 , -0.944551 },
//   { 511.99951 , -0.465039 , 1.9197158 },
//   { 9292.8115 , -2.985261 , -0.987171 },
//   { 756.56079 , -0.451685 , 1.9150447 },
//   { 1208.4456 , 1.1545267 , -1.629757 },
//   { 737.42944 , 0.4508003 , 2.7924311 },
//   { 717.17108 , 0.5629454 , -2.980958 },
//   { 1046.2543 , -0.927631 , 1.2718305 },
//   { 2000.0448 , -1.673090 , 1.4260014 },
//   { 356.86972 , -0.360469 , -1.910773 },
//   { 1131.7016 , -1.040958 , -1.244756 },
//   { 2124.4797 , 1.7213376 , -1.402537 },
//   { 2715.2922 , 2.0704062 , 2.7120437 },
//   { 1001.4078 , -1.118915 , -2.123357 },
//   { 555.41314 , -1.063079 , 1.0330293 },
//   { 552.92248 , -0.636054 , -2.200307 },
//   { 634.86267 , -0.568686 , -2.041829 },
//   { 988.84570 , -1.501420 , 1.5547539 },
//   { 727.20269 , -0.840355 , -2.110203 },
//   { 3881.3671 , -2.243576 , -0.855293 },
//   { 1113.9769 , -1.207514 , -3.047214 },
//   { 647.24957 , 1.2380378 , 0.3311821 },
//   { 1108.5311 , -1.445739 , 0.4502757 },
//   { 694.15460 , 1.2167913 , 0.3362919 },
//   { 2683.6025 , -1.888324 , 1.0870648 },
//   { 835.54864 , -0.918291 , 0.9346773 },
//   { 1723.4925 , 1.6385462 , 0.9181974 },
//   { 504.97766 , -0.782656 , 0.3404427 },
//   { 539.67517 , -0.687225 , -2.301231 },
//   { 507.49905 , -0.414276 , 2.9705309 },
//   { 346.47183 , -0.010755 , 1.5156707 },
//   { 10057.526 , 2.9771006 , 3.0640301 },
//   { 580.95898 , 1.3359664 , -1.427866 },
//   { 1392.4084 , -1.724515 , 1.8063898 },
//   { 1626.7291 , -1.741171 , -0.461990 },
//   { 1536.3057 , -1.611561 , -0.933147 },
//   { 605.42907 , 1.3358080 , -1.329933 },
//   { 398.39822 , -0.411102 , 2.0913531 },
//   { 403.98336 , -0.406422 , -1.800289 },
//   { 239.26789 , -0.360310 , -1.518729 },
//   { 303.51147 , 0.6865302 , -1.721587 },
//   { 492.45669 , 0.4409164 , -2.725038 },
//   { 2071.7968 , -2.030476 , -0.632959 },
//   { 1525.0993 , -1.550519 , -2.130198 },
//   { 1210.3986 , 1.6226664 , 1.3253227 },
//   { 309.24527 , 0.9625922 , -2.896802 },
//   { 1562.7738 , -1.907213 , 0.8406773 },
//   { 354.11071 , -0.507514 , 0.0401076 },
//   { 668.85321 , 0.9140877 , 2.0732674 },
//   { 2997.0405 , 2.5137083 , 2.8203089 },
//   { 1653.5875 , -1.919928 , 2.8433103 },
//   { 1552.2690 , -1.869057 , -0.011060 },
//   { 2866.1276 , 2.2337443 , -0.737913 },
//   { 404.40789 , -0.253425 ,  2.706743 },
//   { 638.26794 , 0.9302623 , -2.049738 },
//   { 1570.8397 , -1.713388 , -0.652719 },
//   { 492.43655 , -0.846000 , -1.423901 },
//   { 269.76116 , 0.2955519 , -2.589722 },
//   { 1523.9704 , 1.5944942 , -0.341417 },
//   { 6792.7993 , -3.124882 , -0.906324 },
//   { 919.19976 , 1.3494231 , -1.920055 },
//   { 312.11419 , -0.480875 , -2.498152 },
//   { 404.81610 , 0.6176764 , 1.3516434 },
//   { 613.95965 , -1.110993 , 1.9270501 },
//   { 533.60327 , -0.857227 , 0.4433113 },
//   { 446.32623 , 1.3608663 , -1.329935 },
//   { 675.38537 , 0.8360574 , -0.544501 },
//   { 280.78704 , 0.2201948 , 2.7006921 },
//   { 2887.9580 , 2.0294282 , 2.2222929 },
//   { 579.16424 , -1.460472 , -2.201072 },
//   { 519.01647 , -0.884441 , -3.086962 },
//   { 221.89711 , 0.8141200 , 2.4048137 },
//   { 620.47705 , -1.507305 , 0.1481916 },
//   { 279.25524 , 0.2472449 , 2.3036842 },
//   { 207.32861 , 0.7643108 , 1.4149115 },
//   { 2356.8388 , -2.198385 , -0.154645 },
//   { 237.54243 , -0.360902 , -2.740587 },
//   { 1496.6348 , -1.863296 , -2.962850 },
//   { 1394.5179 , 1.8463050 , -0.120632 },
//   { 900.67578 , 1.5644705 , -1.361557 },
//   { 288.38037 , -1.293179 , 2.8016543 },
//   { 300.06658 , 1.2903020 , 0.7318146 },
//   { 1128.5855 , -1.794713 , 1.5290404 },
//   { 316.60559 , 0.4078592 , -2.398869 },
//   { 1188.7457 , 1.6513104 , 1.4710537 },
//   { 549.19244 , -1.431118 , 0.3864392 },
//   { 1152.5954 , 1.7486407 , -2.241467 },
//   { 354.89334 , -0.989414 , 2.0212192 },
//   { 273.61468 , -0.758692 , 0.3394872 },
//   { 1128.5800 , -1.853737 , -0.199410 },
//   { 251.48349 , 0.6629966 , 2.8973932 },
//   { 272.49823 , 0.7949233 , 1.3231451 },
//   { 720.68737 , 1.7028000 , -1.432785 },
//   { 257.32525 , -0.947365 , -2.402587 },
//   { 962.89398 , 1.7970228 , -0.436719 },
//   { 1226.5229 , -2.148675 , 1.4878343 },
//   { 1840.6778 , 2.2985074 , -3.032544 },
//   { 2233.0468 , 2.5213942 , 1.2612670 },
//   { 1600.4366 , -2.176522 , 0.6641689 },
//   { 244.16929 , -1.480031 , -2.108826 },
//   {  241.3909 , -1.507772 , 0.4470171 },
//   { 639.68127 , 1.7748405 , 2.5098025 },
//   { 311.51190 , -1.092209 , -0.147522 },
//   { 341.90466 , 1.1230706 , -1.329961 },
//   { 435.89788 , 1.3924759 , -2.048287 },
//   { 244.86798 , 0.7167282 , -2.703305 },
//   { 616.36090 , 1.7985919 , 2.4992313 },
//   { 1003.7953 , 1.8755983 , -0.043975 },
//   { 1011.0583 , -1.933215 , 2.7173271 },
//   { 365.49169 , -2.026548 ,  1.816432 },
//   { 265.22656 , -0.867343 , -1.970670 },
//   { 218.83763 , 1.5103633 , -1.316848 },
//   { 1043.7188 , -2.082054 , -0.615414 },
//   { 307.46612 , 1.1921824 , -2.208534 },
//   { 505.72546 , -1.948837 , 2.3436749 },
//   { 176.55197 , -1.426060 , 2.4090273 },
//   { 298.72326 , -1.112437 , -0.252640 },
//   { 237.81263 , -1.414396 , 2.7079856 },
//   { 1251.1693 ,  2.290344 , -0.335326 },
//   { 415.05133 , -1.441094 , 2.8213560 },
//   { 1561.1165 , 2.4406096 , -2.876875 },
//   { 506.62039 , -1.896385 , -1.521202 },
//   { 146.38749 , 0.6780560 , 1.2940630 },
//   { 213.28025 , -1.088483 , -0.136521 },
//   { 406.74173 , -1.841101 , 1.5398056 },
//   { 347.56463 , 1.8022007 , -1.124924 },
//   { 128.58009 , -1.504471 , -0.148700 },
//   { 402.43734 , 1.9105185 , 0.1434464 },
//   { 351.67889 , -1.744752 , 2.5081632 },
//   { 123.77259 , 1.5043976 , 2.6052258 },
//   { 227.55468 , -1.498618 , 0.7296782 },
//   { 98.312683 , 1.4896421 , -0.336421 },
//   { 85.632263 , -0.111530 , -1.515059 },
//   { 157.73030 , -1.211035 , 1.9145700 },
//   { 222.50627 , 1.7017273 , -1.422166 },
//   { 65.207702 , -0.332923 , -2.205075 },
//   { 187.20732 , -1.418334 , 1.8581798 },
//   { 395.78970 , 2.2436540 , 0.4089814 },
//   { 318.99472 , -2.180621 , -0.699335 },
//   { 364.12820 , 2.4742693 , 0.2330551 },
//   { 178.74490 , -1.972572 , -0.839460 },
//   { 74.402107 , -0.677721 , -2.878207 },
//   { 50.737182 , 1.2665880 , -2.441089 },
//   { 15.522075 , -0.561219 , -2.790420 },
//   { 326.83215 , 1.6022548 , 2.4080691 },
//   { 89.218177 , 1.2897139 , -1.914717 },
//   { -10.18428 , -0.355148 , -2.899181 },
//   { -28.47536 , -0.958540 , 0.5392282 },
//   { -35.20274 , -1.181015 , -2.118641 },
//   { -163.6096 , -2.074751 , 1.8307694 },
//   { -66.76682 , 1.0759987 , -1.429083 },
//   { -761.8370 , -3.461989 , -2.663778 },
//   { -105.0239 , 0.7829098 , -2.706809 },
//   { -167.4234 , 0.8392444 , -2.499046 }
// };
