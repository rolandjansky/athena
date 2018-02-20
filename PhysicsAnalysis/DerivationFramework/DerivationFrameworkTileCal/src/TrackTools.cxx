/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * File:   TrackTools.cxx
 * Author: Marco van Woerden <mvanwoer@cern.ch>, Archil Durglishvili <archil.durglishvili@cern.ch>
 * Description: Track tools.
 *
 * Created in February 2013.
 * Updated in November 2014
 */
#include "TrackTools.h"

//=============================================
TrackTools::TrackTools(const std::string&type, const std::string&name, const IInterface*parent) 
  : AthAlgTool(type, name, parent)
  , m_caloExtensionTool("Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool")
{
//============================================
    // INTERFACING
    declareInterface<ITrackTools>(this);        

    declareProperty("ParticleCaloExtensionTool",m_caloExtensionTool ); 
} // TRACKTOOLS::TRACKTOOLS

//========================
TrackTools::~TrackTools(){
//=======================
} // TRACKTOOLS::~TRACKTOOLS

//==================================
StatusCode TrackTools::initialize(){
//=================================
    
    ATH_MSG_INFO( "Initializing TrackTools" );

    ATH_CHECK(m_caloExtensionTool.retrieve());
        
    return StatusCode::SUCCESS;
} // TrackTools::initialize

//================================
StatusCode TrackTools::finalize(){
//===============================
    ATH_MSG_INFO( "Finalizing TrackTools" );
    return StatusCode::SUCCESS;
} // TRACKTOOLS::FINALIZE

//==================================================================================================
const Trk::TrackParameters* TrackTools::getTrackInCellSampling(const TRACK* track, CaloSampling::CaloSample sampling){
//==================================================================================================
  if( !m_caloExtensionTool.empty() ){
    const Trk::CaloExtension* extension = 0;
    m_caloExtensionTool->caloExtension(*track, extension);
    if(!extension) return 0;

    Trk::TrackParametersIdHelper  parsIdHelper;

    std::map<CaloSampling::CaloSample,const Trk::TrackParameters*> Samplings;

    // loop over calo layers, keep track of previous layer
    auto cur = extension->caloLayerIntersections().begin();
    auto prev = cur;
    for( ; cur != extension->caloLayerIntersections().end() ; ++cur ){
      // check that prev and cur are not the same, if not fill if the previous was an entry layer
      if( prev != cur && parsIdHelper.isEntryToVolume((*prev)->cIdentifier()) )
      {
       	TrackParametersIdentifier id =  (*prev)->cIdentifier();
        CaloSampling::CaloSample sample = parsIdHelper.caloSample(id);
        Samplings[sample] = *prev;
      }
      prev=cur;
    }
    if(!Samplings[sampling])  return 0 ;
    else return Samplings[sampling];
  }
  return 0;
}

//==================================================================================================
std::vector< double > TrackTools::getXYZEtaPhiInCellSampling(const TRACK* track, CaloSampling::CaloSample sampling){
//==================================================================================================
  std::vector<double> coordinates;
  if( !m_caloExtensionTool.empty() ){
    const Trk::CaloExtension* extension = 0;
    m_caloExtensionTool->caloExtension(*track, extension);
    if(!extension) return coordinates;

    Trk::TrackParametersIdHelper  parsIdHelper;

    std::map<CaloSampling::CaloSample,const Trk::TrackParameters*> Samplings;

    // loop over calo layers, keep track of previous layer
    auto cur = extension->caloLayerIntersections().begin();
    auto prev = cur;
    for( ; cur != extension->caloLayerIntersections().end() ; ++cur ){
      // check that prev and cur are not the same, if not fill if the previous was an entry layer
      if( prev != cur && parsIdHelper.isEntryToVolume((*prev)->cIdentifier()) ) 
      {
        TrackParametersIdentifier id =  (*prev)->cIdentifier();
        CaloSampling::CaloSample sample = parsIdHelper.caloSample(id);
        Samplings[sample] = *prev;
      }
      prev=cur;
    }
    if(!(Samplings[sampling])) return coordinates;
    coordinates.push_back(Samplings[sampling]->position().x());
    coordinates.push_back(Samplings[sampling]->position().y());
    coordinates.push_back(Samplings[sampling]->position().z());
    coordinates.push_back(Samplings[sampling]->position().eta());
    coordinates.push_back(Samplings[sampling]->position().phi());
  }
  return coordinates;
} // getXYZEtaPhiInCellSampling


//==========================================================================================================
std::vector< double > TrackTools::getXYZEtaPhiInCellSampling(const TRACK* track, const CaloCell *cell){
//==========================================================================================================
  std::vector<double> coordinates;

  if(!cell || !track) return coordinates;
  // GET CELL DESCRIPTOR AND SAMPLING
  const CaloDetDescrElement* dde = cell->caloDDE();
  if(!dde) return coordinates;
  CaloSampling::CaloSample sampling = dde->getSampling();

  return getXYZEtaPhiInCellSampling(track,sampling);
} // getXYZEtaPhiInCellSampling


//=====================================================================================================
std::vector< std::vector<double> > TrackTools::getXYZEtaPhiPerLayer(const TRACK* track){
//====================================================================================================
  std::vector< std::vector<double> > coordinates(11);

  for(unsigned int sample=0 ; sample<21; ++sample) //Samplings:http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Calorimeter/CaloGeoHelpers/CaloGeoHelpers/CaloSampling.def
  {
    std::vector<double> TrkPars(5);
    std::vector<double> XYZEtaPhi = getXYZEtaPhiInCellSampling( track, (CaloSampling::CaloSample)sample );
    TrkPars[0] = ( XYZEtaPhi.size()!=5 ) ? -9999. : XYZEtaPhi[0] ;
    TrkPars[1] = ( XYZEtaPhi.size()!=5 ) ? -9999. : XYZEtaPhi[1] ;
    TrkPars[2] = ( XYZEtaPhi.size()!=5 ) ? -9999. : XYZEtaPhi[2] ;
    TrkPars[3] = ( XYZEtaPhi.size()!=5 ) ? -9999. : XYZEtaPhi[3] ;
    TrkPars[4] = ( XYZEtaPhi.size()!=5 ) ? -9999. : XYZEtaPhi[4] ;
    int lay=-1;
    if     (sample==0  || sample==4 ){lay=0;}
    else if(sample==1  || sample==5 ){lay=1;}
    else if(sample==2  || sample==6 ){lay=2;}
    else if(sample==3  || sample==7 ){lay=3;}
    else if(sample==12 || sample==18){lay=4;}
    else if(sample==13 || sample==19){lay=5;}
    else if(sample==15 || sample==17){lay=5;}
    else if(sample==14 || sample==20){lay=6;}
    else if(sample==16)                {lay=6;}
    else if(sample==8                 ){lay=7;}
    else if(sample==9                 ){lay=8;}
    else if(sample==10                ){lay=9;}
    else if(sample==11                ){lay=10;}
    if(lay!=-1) coordinates[lay] = TrkPars;
  } // FOR

  return coordinates;
} // TrackTools::getXYZEtaPhiPerLayer

//=====================================================================================================
std::vector< std::vector<double> > TrackTools::getXYZEtaPhiPerSampling(const TRACK* track){
//====================================================================================================
  std::vector< std::vector<double> > coordinates;

  for(unsigned int s=0 ; s<21; ++s) //Samplings: http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Calorimeter/CaloGeoHelpers/CaloGeoHelpers/CaloSampling.def
  {
    std::vector<double> TrkPars(5);
    std::vector<double> XYZEtaPhi = getXYZEtaPhiInCellSampling( track, (CaloSampling::CaloSample)s ); 
    TrkPars[0] = ( XYZEtaPhi.size()!=5 ) ? -9999. : XYZEtaPhi[0] ;
    TrkPars[1] = ( XYZEtaPhi.size()!=5 ) ? -9999. : XYZEtaPhi[1] ;
    TrkPars[2] = ( XYZEtaPhi.size()!=5 ) ? -9999. : XYZEtaPhi[2] ;
    TrkPars[3] = ( XYZEtaPhi.size()!=5 ) ? -9999. : XYZEtaPhi[3] ;
    TrkPars[4] = ( XYZEtaPhi.size()!=5 ) ? -9999. : XYZEtaPhi[4] ;
    coordinates.push_back(TrkPars);
  } // FOR
 
  return coordinates;
} // TrackTools::getXYZEtaPhiPerSampling


//======================================================================
void TrackTools::getCellsWithinConeAroundTrack(const TRACK* track, 
                                               const CaloCellContainer* input,
                                               ConstDataVector<CaloCellContainer>* output, 
                                               double cone,
                                               bool includelar){
//======================================================================
  // CELLS WITHIN CONE AROUND TRACK
  CaloCellContainer::const_iterator icell = input->begin();
  CaloCellContainer::const_iterator end   = input->end();
  for(;icell!=end;++icell){
    const CaloCell* cell = (*icell);
    const CaloDetDescrElement* dde = cell->caloDDE();

    // REASONS TO SKIP THIS CELL OR BREAK THE LOOP
    if(!dde) continue;
    if(!includelar && dde->getSubCalo() == CaloCell_ID::LAREM) continue;
    if(dde->getSubCalo() != CaloCell_ID::LAREM && dde->getSubCalo() != CaloCell_ID::TILE) continue;

    std::vector<double> coordinates = getXYZEtaPhiInCellSampling(track,dde->getSampling());
    if(coordinates.size()!=5) continue;

    double deltaR = KinematicUtils::deltaR( cell->eta(),coordinates[3], cell->phi(),coordinates[4]);

    if(deltaR<=cone)
    {
      unsigned int i=0;
      while(i!=output->size()){if(output->at(i)==cell){break;}i++;}
      if(i==output->size())
      {
        output->push_back(cell);
      }
    }
  } // FOR
} // TrackTools::getCellsWithinConeAroundTrack

//======================================================================================
double TrackTools::getPathInsideCell(const TRACK *track, const CaloCell *cell){
//======================================================================================

  ATH_MSG_DEBUG("in TrackInCaloTools::getPathInsideCell" );

  // GET CELL DESCRIPTOR AND SAMPLING
  const CaloDetDescrElement* dde = cell->caloDDE();
  if(!dde) return 0.;
  int sampling = dde->getSampling();
  int sampling_entrance = 0;
  int sampling_exit     = 0;
  switch(sampling){
      case 12: sampling_entrance = 12; sampling_exit = 14; break;
      case 13: sampling_entrance = 12; sampling_exit = 14; break;
      case 14: sampling_entrance = 12; sampling_exit = 14; break;
      case 15: sampling_entrance = 13; sampling_exit = 14; break;
      case 16: sampling_entrance = 12; sampling_exit = 13; break;
      case 17: sampling_entrance = 17; sampling_exit = 19; break;
      case 18: sampling_entrance = 18; sampling_exit = 20; break;
      case 19: sampling_entrance = 18; sampling_exit = 20; break;
      case 20: sampling_entrance = 18; sampling_exit = 20; break;
      default: return 0.;
  } // SWITCH

  const Trk::TrackParameters *pars_entrance = getTrackInCellSampling(track, (CaloSampling::CaloSample)sampling_entrance);
  const Trk::TrackParameters *pars_exit     = getTrackInCellSampling(track, (CaloSampling::CaloSample)sampling_exit);

  if( !pars_entrance || !pars_exit ) return 0.;

  return getPath(cell, pars_entrance, pars_exit);
} // TrackTools::getPathInsideCell

//=====================================================================================================================================
double TrackTools::getPath(const CaloCell* cell, const Trk::TrackParameters *entrance, const Trk::TrackParameters *exit){
//====================================================================================================================================
    ATH_MSG_DEBUG("IN getPath...");

    // OBTAIN LAYER INDICES FOR LINEAR INTERPOLATION
    unsigned int SampleID = cell->caloDDE()->getSampling();
    
    // OBTAIN TRACK AND CELL PARAMETERS
    double pathl = 0.;
    double Layer1X(exit->position().x()),Layer1Y(exit->position().y()),Layer1Z(exit->position().z());
    double Layer2X(entrance->position().x()),Layer2Y(entrance->position().y()),Layer2Z(entrance->position().z());

    double CellPhi(cell->caloDDE()->phi()),CellDPhi(cell->caloDDE()->dphi());
    double CellPhimin = CellPhi-CellDPhi/2.;
    double CellPhimax = CellPhi+CellDPhi/2.;
    double CellEta(cell->caloDDE()->eta());
    double CellZ(cell->caloDDE()->z()),CellDZ(cell->caloDDE()->dz());
    double CellZmin = CellZ-CellDZ/2.;
    double CellZmax = CellZ+CellDZ/2.;
    double CellR(cell->caloDDE()->r()),CellDR(cell->caloDDE()->dr());
    double CellRmin = CellR-CellDR/2.;
    double CellRmax = CellR+CellDR/2.;
    
    // TILECAL GEOMETRY
    double RLBAmin = 2300;
    double RLBAmax = 2600;
    double RLBBmin = 2600;
    double RLBBmax = 2990;
    double RLBCmin = 2990;
    double RLBCmax = 3440;
    double RLBDmin = 3440;
    double RLBDmax = 3820;
    
    double REBAmin = 2300;
    double REBAmax = 2600;
    double REBBmin = 2600;
    double REBBmax = 3140;
    double REBDmin = 3140;
    double REBDmax = 3820;
    
    double RITC_D4_min  = 3630 - 380./2.;
    double RITC_C10_min = 3215 - 450./2.;
    double RITC_E1_min  = 2803 - 313./2.;
    double RITC_E2_min  = 2476 - 341./2.;
    double RITC_E3_min  = 2066 - 478./2.;
    double RITC_E4_min  = 1646 - 362./2.;
    
    double RITC_D4_max  = 3630 + 380./2.;
    double RITC_C10_max = 3215 + 450./2.;
    double RITC_E1_max  = 2803 + 313./2.;
    double RITC_E2_max  = 2476 + 341./2.;
    double RITC_E3_max  = 2066 + 478./2.;
    double RITC_E4_max  = 1646 + 362./2.;
    
    double ZITC_D4_a  = 3405;
    double ZITC_C10_a = 3511;
    double ZITC_E1_a  = 3552;
    //     double ZITC_E2_a  = 3552;
    double ZITC_E3_a  = 3536;
    //     double ZITC_E4_a  = 3536;
    
    double ZITC_D4_c  = 3395;
    double ZITC_C10_c = 3501;
    double ZITC_E1_c  = 3542;
    //     double ZITC_E2_c  = 3542;
    double ZITC_E3_c  = 3526;
    //     double ZITC_E4_c  = 3526;
    
    double ZDITC_D4  = 309.;
    double ZDITC_C10 = 95.;
    double ZDITC_E1  = 12.;
    //     double ZDITC_E2  = 12.;
    double ZDITC_E3  = 6.;
    //     double ZDITC_E4  = 6.;
    
    double CellZB[9]  = {141.495, 424.49, 707.485, 999.605, 1300.855, 1615.8, 1949., 2300.46, 2651.52};
    double CellDZB[9] = {282.99, 283., 282.99, 301.25, 301.25, 328.64, 337.76, 365.16, 336.96};
    double CellZC[9]  = {159.755, 483.83, 812.465, 1150.23, 1497.125, 1857.71, 2241.12, 2628.695,0};
    double CellDZC[9] = {319.51, 328.64, 328.63, 346.9, 346.89, 374.28, 392.54, 382.61,0};
    
    double CellXimp[2], CellYimp[2], CellZimp[2];
    //     double Sxz(0), Syz(0), Sxy(0);
    double X(0), Y(0), Z(0), R(0), Phi(0);
    double DeltaPhi;
    
    int etaInd(-1);

    float samplecellmap[81] = {
        12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
        13, 13, 13, 13, 13, 13, 13, 13, 13,
        14, 14, 14, 14,
        16, 15, 17, 17, 17, 17,            //** ITC
        18, 18, 18, 18, 18,
        19, 19, 19, 19, 19,
        20, 20,
        12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
        13, 13, 13, 13, 13, 13, 13, 13, 13,
        14, 14, 14,
        16, 15, 17, 17, 17, 17,            //** ITC
        18, 18, 18, 18, 18,
        19, 19, 19, 19, 19,
        20, 20
    };
    float etacellmap[81] = { 0.05,0.15,0.25,0.35,0.45,0.55,0.65,0.75,0.85,0.95,
        0.05,0.15,0.25,0.35,0.45,0.55,0.65,0.75,0.85,
        0.0, 0.2,0.4,0.6,
        0.8579205,0.9583722,1.0589020,1.1593041,1.3098471,1.5103633,
        1.1594202,1.2586680,1.3579534,1.4572804,1.556651,
        1.0586925,1.1580252,1.2573844,1.3567756,1.4562022,
        1.0074122,1.2063241,
        -0.05,-0.15,-0.25,-0.35,-0.45,-0.55,-0.65,-0.75,-0.85,-0.95,
        -0.05,-0.15,-0.25,-0.35,-0.45,-0.55,-0.65,-0.75,-0.85,
        -0.2,-0.4,-0.6,
        -0.855940, -0.956279, -1.056676, -1.156978,-1.307385,-1.507772,
        -1.157065,-1.256501,-1.355965,-1.455460,-1.554988,
        -1.056519,-1.156018,-1.255538,-1.355081,-1.454651,
        -1.005559,-1.204743
    };
    
    for(int i=0; i<81; i++){
        if(SampleID==samplecellmap[i] && fabs(CellEta-etacellmap[i])<0.001) etaInd = i;
    } // FOR
    
    // COMPUTE PATH
    bool compute = true;
    int lBC(0);
    while(compute){
        if(lBC==1 && (etaInd==18 || etaInd==59) ) break;
        int Np = 0;
        if(sqrt((Layer1X-Layer2X)*(Layer1X-Layer2X)+(Layer1Y-Layer2Y)*(Layer1Y-Layer2Y)) < 3818.5){
            if(SampleID == 15){
                CellRmin = RITC_C10_min;
                CellRmax = RITC_C10_max;
                if(CellEta > 0){
                    if(CellZmin < ZITC_C10_a-ZDITC_C10/2.) CellZmin = ZITC_C10_a-ZDITC_C10/2.;
                    if(CellZmax > ZITC_C10_a+ZDITC_C10/2.) CellZmax = ZITC_C10_a+ZDITC_C10/2.;
                } // IF
                else if(CellEta < 0){
                    if(CellZmin < -ZITC_C10_c-ZDITC_C10/2.) CellZmin = -ZITC_C10_c-ZDITC_C10/2.;
                    if(CellZmax > -ZITC_C10_c+ZDITC_C10/2.) CellZmax = -ZITC_C10_c+ZDITC_C10/2.;
                } // ELSE IF
            } // IF
            else if(SampleID == 16){
                CellRmin=RITC_D4_min;
                CellRmax=RITC_D4_max;
                if(CellEta > 0){
                    if(CellZmin < ZITC_D4_a-ZDITC_D4/2.) CellZmin = ZITC_D4_a-ZDITC_D4/2.;
                    if(CellZmax > ZITC_D4_a+ZDITC_D4/2.) CellZmax = ZITC_D4_a+ZDITC_D4/2.;
                } // IF
                else if(CellEta < 0){
                    if(CellZmin < -ZITC_D4_c-ZDITC_D4/2.) CellZmin = -ZITC_D4_c-ZDITC_D4/2.;
                    if(CellZmax > -ZITC_D4_c+ZDITC_D4/2.) CellZmax = -ZITC_D4_c+ZDITC_D4/2.;
                } // ELSE IF
            } // ELSE IF
            else if(SampleID == 17){
                if( etaInd==25 || etaInd==65 ){
                    CellRmax = RITC_E1_max;
                    CellRmin = RITC_E1_min;
                } // IF
                else if( etaInd==26 || etaInd==66 ){
                    CellRmax = RITC_E2_max;
                    CellRmin = RITC_E2_min;
                } // ELSE IF
                else if( etaInd==27 || etaInd==67 ){
                    CellRmax = RITC_E3_max;
                    CellRmin = RITC_E3_min;
                } // ELSE IF
                else if( etaInd==28 || etaInd==68 ){
                    CellRmax = RITC_E4_max;
                    CellRmin = RITC_E4_min;
                } // ELSE IF
                if( (etaInd>=25 && etaInd<=26) || (etaInd>=65 && etaInd<=66) ){
                    if(CellEta > 0){
                        if(CellZmin < ZITC_E1_a-ZDITC_E1/2.) CellZmin=ZITC_E1_a-ZDITC_E1/2.;
                        if(CellZmax > ZITC_E1_a+ZDITC_E1/2.) CellZmax=ZITC_E1_a+ZDITC_E1/2.;
                    } // IF
                    else if(CellEta < 0){
                        if(CellZmin < -ZITC_E1_c-ZDITC_E1/2.) CellZmin= -ZITC_E1_c-ZDITC_E1/2.;
                        if(CellZmax > -ZITC_E1_c+ZDITC_E1/2.) CellZmax= -ZITC_E1_c+ZDITC_E1/2.;
                    } // ELSE
                } // IF
                else if( (etaInd>=27 && etaInd<=28) || (etaInd>=67 && etaInd<=68) ){
                    if(CellEta > 0){
                        if(CellZmin < ZITC_E3_a-ZDITC_E3/2.) CellZmin=ZITC_E3_a-ZDITC_E3/2.;
                        if(CellZmax > ZITC_E3_a+ZDITC_E3/2.) CellZmax=ZITC_E3_a+ZDITC_E3/2.;
                    } // IF
                    else if(CellEta < 0){
                        if(CellZmin < -ZITC_E3_c-ZDITC_E3/2.) CellZmin= -ZITC_E3_c-ZDITC_E3/2.;
                        if(CellZmax > -ZITC_E3_c+ZDITC_E3/2.) CellZmax= -ZITC_E3_c+ZDITC_E3/2.;
                    } // ELSE IF
                } // ELSE
            } // ELSE IF
            else if(SampleID == 12){
                CellRmin = RLBAmin;
                CellRmax = RLBAmax;
                
                if(CellZmin <-2802.5439) CellZmin=-2802.5439;
                if(CellZmax > 2802.5454) CellZmax=2802.5454;
            } // ELSE IF
            else if(SampleID == 13 && lBC == 0){
                CellRmin = RLBBmin;
                CellRmax = RLBBmax;
                int cpos = fabs(CellEta)/0.1;
                CellZ = CellZB[cpos];
                if(CellEta < 0) CellZ=-CellZ;
                CellDZ = CellDZB[cpos];
                CellZmin = CellZ-CellDZ/2;
                CellZmax = CellZ+CellDZ/2;
                if(CellZmin <-2802.5439) CellZmin=-2802.5439;
                if(CellZmax > 2802.5454) CellZmax=2802.5454;
            } // ELSE IF
            else if(SampleID == 13 && lBC == 1){
                CellRmin = RLBCmin;
                CellRmax = RLBCmax;
                int cpos = fabs(CellEta)/0.1;
                if(cpos >= 8) return 0;
                CellZ = CellZC[cpos];
                if(CellEta < 0) CellZ=-CellZ;
                CellDZ = CellDZC[cpos];
                CellZmin = CellZ-CellDZ/2;
                CellZmax = CellZ+CellDZ/2;
                if(CellZmin <-2802.5439) CellZmin=-2802.5439;
                if(CellZmax >2802.5454) CellZmax=2802.5454;
            } // ELSE IF
            else if(SampleID == 14){
                CellRmin = RLBDmin;
                CellRmax = RLBDmax;
                if(CellZmin <-2802.5439) CellZmin=-2802.5439;
                if(CellZmax >2802.5454) CellZmax=2802.5454;
            } // ELSE IF
            if(SampleID == 18){
                CellRmin = REBAmin;
                CellRmax = REBAmax;
                if(CellEta > 0){
                    if(CellZmin <3574.5027) CellZmin=3574.4978;
                    if(CellZmax >6130.0039) CellZmax=6130.0039;
                } // IF
                else if(CellEta < 0){
                    if(CellZmin <-6120.0018) CellZmin=-6120.0018;
                    if(CellZmax >-3564.5006) CellZmax=-3564.5006;
                } // IF
            } // IF
            if(SampleID == 19){
                CellRmin = REBBmin;
                CellRmax = REBBmax;
                if(CellEta > 0){
                    if(CellZmin <3574.5027) CellZmin=3574.4978;
                    if(CellZmax >6130.0039) CellZmax=6130.0039;
                } // IF
                else if(CellEta < 0){
                    if(CellZmin <-6120.0018) CellZmin=-6120.0018;
                    if(CellZmax >-3564.5006) CellZmax=-3564.5006;
                } // ELSE IF
            } // IF
            if(SampleID == 20){
                CellRmin = REBDmin;
                CellRmax = REBDmax;
                if(CellEta > 0){
                    if(CellZmin <3574.5027) CellZmin=3574.4978;
                    if(CellZmax >6130.0039) CellZmax=6130.0039;
                } // IF
                else if(CellEta < 0){
                    if(CellZmin <-6120.0018) CellZmin=-6120.0018;
                    if(CellZmax >-3564.5006) CellZmax=-3564.5006;
                } // ELSE IF
            } // IF
            
            // CALCULATE POINTS OF INTERSECTION
            // INTERSECTIONS R PLANES
            double Radius(CellRmin);
            
            double x0int(exit->position().x()), x1int(entrance->position().x()),
            y0int(exit->position().y()), y1int(entrance->position().y()),
            z0int(exit->position().z()), z1int(entrance->position().z());
            double S((y1int-y0int)/(x1int-x0int));
            double a(1+S*S), b(2*S*y0int-2*S*S*x0int), c(y0int*y0int-Radius*Radius+S*S*x0int*x0int-2*y0int*S*x0int);
            double x1((-b+sqrt(b*b-4*a*c))/(2*a)), x2((-b-sqrt(b*b-4*a*c))/(2*a));
            double y1(y0int+S*(x1-x0int)), y2(y0int+S*(x2-x0int));
            double S1 = ((z1int-z0int)/(x1int-x0int));//, R1(sqrt(x1*x1+y1*y1)), R2(sqrt(x2*x2+y2*y2));
            double z1(z0int+S1*(x1-x0int)), z2(z0int+S1*(x2-x0int));
            
            X = x1;
            Y = y1;
            Z = z1;
            
            if( ((x1-x0int)*(x1-x0int)+(y1-y0int)*(y1-y0int)+(z1-z0int)*(z1-z0int)) >
               ((x2-x0int)*(x2-x0int)+(y2-y0int)*(y2-y0int)+(z2-z0int)*(z2-z0int)) ){
                X=x2;
                Y=y2;
                Z=z2;
            } // IF

            Phi = acos(X/sqrt(X*X+Y*Y));
            if(Y <= 0) Phi = -Phi;
            R = CellRmin;
            
            if(Z>=CellZmin && Z<=CellZmax && Phi>=CellPhimin && Phi<=CellPhimax){
                CellXimp[Np]=X;
                CellYimp[Np]=Y;
                CellZimp[Np]=Z;
                Np=Np+1;
               
            } // IF
            
            Radius = CellRmax;
            
            c  = y0int*y0int-Radius*Radius+S*S*x0int*x0int-2*y0int*S*x0int;
            x1 = ((-b+sqrt(b*b-4*a*c))/(2*a));
            x2 = ((-b-sqrt(b*b-4*a*c))/(2*a));
            y1 = (y0int+S*(x1-x0int));
            y2 = (y0int+S*(x2-x0int));
            z1 = (z0int+S1*(x1-x0int));
            z2 = (z0int+S1*(x2-x0int));
            S1 = ((z1int-z0int)/(x1int-x0int));

            X = x1;
            Y = y1;
            Z = z1;

            if( ((x1-x0int)*(x1-x0int)+(y1-y0int)*(y1-y0int)+(z1-z0int)*(z1-z0int)) >
               ((x2-x0int)*(x2-x0int)+(y2-y0int)*(y2-y0int)+(z2-z0int)*(z2-z0int)) ){
                X = x2;
                Y = y2;
                Z = z2;
            } // IF

            Phi=std::acos(X/sqrt(X*X+Y*Y));
            if (Y <= 0) Phi=-Phi;
            R=CellRmax;
            
            if(Z>=CellZmin && Z<=CellZmax && Phi>=CellPhimin && Phi<=CellPhimax){
                CellXimp[Np]=X; CellYimp[Np]=Y; CellZimp[Np]=Z;
                Np=Np+1;
               
            } // IF
            
            // INTERSECTIONS Z PLANES
            if(Np < 2){
                double Sxz = (Layer2X-Layer1X)/(Layer2Z-Layer1Z);
                double Syz = (Layer2Y-Layer1Y)/(Layer2Z-Layer1Z);
                Z = CellZmin;
                X = Layer1X+Sxz*(Z-Layer1Z);
                Y = Layer1Y+Syz*(Z-Layer1Z);
                R = sqrt(X*X+Y*Y);
                Phi = std::acos(X/R);
                if(Y <= 0) Phi=-Phi;
                if(R>=CellRmin && R<=CellRmax && Phi>=CellPhimin && Phi<=CellPhimax){
                    CellXimp[Np]=X; CellYimp[Np]=Y; CellZimp[Np]=Z;
                    Np=Np+1;
                   
                } // IF
            } // IF

            if(Np < 2){
                double Sxz = (Layer2X-Layer1X)/(Layer2Z-Layer1Z);
                double Syz = (Layer2Y-Layer1Y)/(Layer2Z-Layer1Z);
                Z = CellZmax;
                X = Layer1X+Sxz*(Z-Layer1Z);
                Y = Layer1Y+Syz*(Z-Layer1Z);
                R = sqrt(X*X+Y*Y);
                Phi = std::acos(X/R);
                if(Y <= 0) Phi=-Phi;
                if(R>=CellRmin && R<=CellRmax && Phi>=CellPhimin && Phi<=CellPhimax){
                    CellXimp[Np]=X; CellYimp[Np]=Y; CellZimp[Np]=Z;
                    Np=Np+1;
                   
                } // IF
            } // IF

            // INTERSECTIONS PHI PLANES
            if(Np < 2){
                double Sxy=(Layer2X-Layer1X)/(Layer2Y-Layer1Y);
                double Sxz=(Layer2X-Layer1X)/(Layer2Z-Layer1Z);
                X = (Layer1X-Sxy*Layer1Y)/(1-Sxy*tan(CellPhimin));
                Y = X*std::tan(CellPhimin);
                Z = Layer1Z+(1/Sxz)*(X-Layer1X);
                R = sqrt(X*X+Y*Y);
                Phi = std::acos(X/R);
                if(Y <= 0) Phi=-Phi;
                DeltaPhi=fabs(Phi-CellPhimin);
                if(DeltaPhi > 3.141593) DeltaPhi=fabs(Phi+CellPhimin);
                if(R>=CellRmin && R<=CellRmax && Z>=CellZmin && Z<=CellZmax && DeltaPhi<0.0001){
                    CellXimp[Np]=X; CellYimp[Np]=Y; CellZimp[Np]=Z;
                    Np=Np+1;                  
                } // IF
            } // IF
            if(Np < 2){
                double Sxy=(Layer2X-Layer1X)/(Layer2Y-Layer1Y);
                double Sxz=(Layer2X-Layer1X)/(Layer2Z-Layer1Z);
                X = (Layer1X-Sxy*Layer1Y)/(1-Sxy*tan(CellPhimax));
                Y = X*std::tan(CellPhimax);
                Z = Layer1Z+(1/Sxz)*(X-Layer1X);
                R = sqrt(X*X+Y*Y);
                Phi=acos(X/R);
                if(Y <= 0) Phi=-Phi;
                DeltaPhi=fabs(Phi-CellPhimax);
                if(DeltaPhi > 3.141593) DeltaPhi=fabs(Phi+CellPhimax);
                if(R>=CellRmin && R<=CellRmax && Z>=CellZmin && Z<=CellZmax && DeltaPhi<0.0001){
                    CellXimp[Np]=X; CellYimp[Np]=Y; CellZimp[Np]=Z;
                    Np=Np+1;
                } // IF
            } // IF

            // CALCULATE PATH IF TWO INTERSECTIONS WERE FOUND
            if(Np == 2){
                
                pathl += sqrt( (CellXimp[0]-CellXimp[1])*(CellXimp[0]-CellXimp[1]) +
                               (CellYimp[0]-CellYimp[1])*(CellYimp[0]-CellYimp[1]) +
                               (CellZimp[0]-CellZimp[1])*(CellZimp[0]-CellZimp[1]) );
            } // IF
        } // IF
        if(SampleID == 13 && lBC == 0) ++lBC;
        else compute = false;
    } // WHILE (FOR LBBC LAYER)

    return pathl;
} // TrackTools::getPath

//=====================================================
int TrackTools::retrieveIndex(int sampling, float eta){
//====================================================
    // STORE ETA MAP
    float etamap[81] = { -0.95,-0.85,-0.75,-0.65,-0.55,-0.45,-0.35,-0.25,-0.15,-0.05,  // CELLS A-10 TO A-1  (SAMPLING 12) INDICES  0:9
        0.05, 0.15, 0.25, 0.35, 0.45, 0.55, 0.65, 0.75, 0.85, 0.95,  // CELLS A1 TO A10    (SAMPLING 12) INDICES 10:19
        -0.85,-0.75,-0.65,-0.55,-0.45,-0.35,-0.25,-0.15,-0.05,  // CELLS BC-9 TO BC-1 (SAMPLING 13) INDICES 20:28
        0.05, 0.15, 0.25, 0.35, 0.45, 0.55, 0.65, 0.75, 0.85,  // CELLS BC1 TO BC9   (SAMPLING 13) INDICES 29:37
        -0.60,-0.40,-0.20,  // CELLS D-3 TO D-1   (SAMPLING 14) INDICES 38:40
        0.00, 0.20, 0.40, 0.60,  // CELLS D0 TO D3     (SAMPLING 14) INDICES 41:44
        -0.956279,0.9583722,  // CELLS C-10 TO C10  (SAMPLING 15) INDICES 45:46
        -0.855940,0.8579205,  // CELLS D-4 TO D4    (SAMPLING 16) INDICES 47:48
        -1.507772,-1.307385,-1.156978,-1.056676,  // CELLS E-4 TO E-1   (SAMPLING 17) INDICES 49:52
        1.0589020,1.1593041,1.3098471,1.5103633,  // CELLS E1 TO E4     (SAMPLING 17) INDICES 53:56
        -1.554988,-1.455460,-1.355965,-1.256501,-1.157065,  // CELLS A-16 TO A-12 (SAMPLING 18) INDICES 57:61
        1.1594202,1.258668,1.3579534,1.4572804,1.5566510,  // CELLS A12 TO A16   (SAMPLING 18) INDICES 62:66
        -1.454651,-1.355081,-1.255538,-1.156018,-1.056519,  // CELLS B-15 TO B-11 (SAMPLING 19) INDICES 67:71
        1.0586925,1.1580252,1.2573844,1.3567756,1.4562022,  // CELLS B11 TO B15   (SAMPLING 19) INDICES 72:76
        -1.204743,-1.005559,  // CELLS D-6 TO D-5   (SAMPLING 20) INDICES 77:78
        1.0074122,1.2063241}; // CELLS D5 TO D6     (SAMPLING 20) INDICES 79:80
    // CALCULATE INDEX
    int index(999),i_start(999),i_end(999);
    switch(sampling){
        case 12: i_start = 0;  i_end = 19; break;
        case 13: i_start = 20; i_end = 37; break;
        case 14: i_start = 38; i_end = 44; break;
        case 15: i_start = 45; i_end = 46; break;
        case 16: i_start = 47; i_end = 48; break;
        case 17: i_start = 49; i_end = 56; break;
        case 18: i_start = 57; i_end = 66; break;
        case 19: i_start = 67; i_end = 76; break;
        case 20: i_start = 77; i_end = 80; break;
        default: break;
    } // SWITCH

    if(i_start==999 || i_end==999) return -1;
    
    index = i_start;
    
    for(int i=i_start;i<=i_end;++i) index = fabs(eta-etamap[i]) <= fabs(eta-etamap[index]) ? i : index;
    return index;  
} // TRACKTOOLS::RETRIEVEINDEX
