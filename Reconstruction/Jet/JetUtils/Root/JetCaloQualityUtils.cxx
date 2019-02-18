/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUtils/JetCaloQualityUtils.h"

#include "xAODJet/Jet.h"

#include "xAODJet/JetAccessorMap.h"
#include "xAODCaloEvent/CaloCluster.h"

#include "CaloGeoHelpers/CaloSampling.h"


#include "xAODCaloEvent/CaloCluster.h"


using xAOD::Jet;
using xAOD::CaloCluster;



namespace {

int em_calosample[]  = { CaloSampling::PreSamplerB, CaloSampling::EMB1, CaloSampling::EMB2, CaloSampling::EMB3,
                           CaloSampling::PreSamplerE, CaloSampling::EME1, CaloSampling::EME2, CaloSampling::EME3,  
                           CaloSampling::FCAL0};
  int had_calosample[] = { CaloSampling::HEC0, CaloSampling::HEC1, CaloSampling::HEC2, CaloSampling::HEC3,          
                           CaloSampling::TileBar0, CaloSampling::TileBar1, CaloSampling::TileBar2,
                           CaloSampling::TileGap1, CaloSampling::TileGap2, CaloSampling::TileGap3,    
                           CaloSampling::TileExt0, CaloSampling::TileExt1, CaloSampling::TileExt2,    
                           CaloSampling::FCAL1, CaloSampling::FCAL2  };
  
//const double GeV = 1000.0;
}

namespace jet {


  int JetCaloQualityUtils::compute_nLeading(std::vector<double> &cell_energies,  const float& e, const float& frac)
  {  
    std::sort(cell_energies.rbegin(),cell_energies.rend());
    int counter =0;
    float sum = 0;
    for(unsigned int i=0;i<cell_energies.size();i++)
      {
        sum += cell_energies[i];
        counter++;
        if(sum>frac*e) break;
      }
    return counter;
  }



  static const xAOD::JetAttributeAccessor::AccessorWrapper< std::vector<float> >  &eperSamplAcc = 
    *xAOD::JetAttributeAccessor::accessor< std::vector<float> >(xAOD::JetAttribute::EnergyPerSampling);

  double JetCaloQualityUtils::emFraction(const std::vector<float>& e_sampling){   
    double e_EM = 0;
    for(int i=0; i<9; i++) e_EM += e_sampling[ ::em_calosample[i] ];
    
    double e_HAD = 0;
    for(int i=0; i<15; i++) e_HAD += e_sampling[ ::had_calosample[i] ];
    if( (e_EM==0) || ((e_EM+e_HAD)==0) ) return 0.;
    return (e_EM / (e_EM+e_HAD));    
  }
  

  double JetCaloQualityUtils::hecF(const Jet* jet)
  {
    
    const std::vector<float> & einsampling = eperSamplAcc(*jet);
    
    double e_hec =einsampling[CaloSampling::HEC0]
      +einsampling[CaloSampling::HEC1]
      +einsampling[CaloSampling::HEC2]
      +einsampling[CaloSampling::HEC3];
    
    double e_jet = jet->jetP4(xAOD::JetEMScaleMomentum).E();
  
    if(e_jet!=0) return  e_hec/e_jet;
    else return -999;
  }

  double JetCaloQualityUtils::presamplerFraction(const Jet* jet)
  {
    const std::vector<float> einsampling = eperSamplAcc(*jet);

    double e_pres = einsampling[CaloSampling::PreSamplerB] + einsampling[CaloSampling::PreSamplerE];

    double e_jet = jet->jetP4(xAOD::JetEMScaleMomentum).E();
  
    if(e_jet!=0) return  e_pres/e_jet;
    else return -999;
  }

  double JetCaloQualityUtils::tileGap3F(const Jet* jet)
  {
    const std::vector<float> & einsampling = eperSamplAcc(*jet);

    double e_tileGap3 =einsampling[CaloSampling::TileGap3];
    double e_jet = jet->jetP4(xAOD::JetEMScaleMomentum).E();

    if(e_jet!=0) return  e_tileGap3/e_jet;
    else return -999;
  }


  double JetCaloQualityUtils::fracSamplingMax(const Jet* jet, int& SamplingMax)
  {
    const std::vector<float> & einsampling = eperSamplAcc(*jet);

    double fracSamplingMax=-999999999.;    
    double sumE_samplings=0.;
    for ( unsigned int i(0); i < einsampling.size(); ++i )
      {
        double e = einsampling[i];
        sumE_samplings+=e;
        if (e>fracSamplingMax)
          {
            fracSamplingMax=e;
            SamplingMax=i;
          }
      }

    if(sumE_samplings!=0)
      fracSamplingMax/=sumE_samplings;
    else fracSamplingMax=0;
  
    return fracSamplingMax;
  }







  double JetCaloQualityUtils::jetNegativeEnergy(const Jet* jet)
  {
    JetCalcNegativeEnergy negE;
    return negE(jet);
  }
  
  
  bool JetCaloQualityUtils::isUgly(const Jet* jet,const bool /*recalculateQuantities*/){
    double fcor = jet->getAttribute<float>(xAOD::JetAttribute::BchCorrCell);
    double tileGap3f = JetCaloQualityUtils::tileGap3F(jet);
    
    if (fcor>0.5) return true;
    if (tileGap3f >0.5) return true;
    
    return false;
  }




  // ****************************************************************
  // JetCalcnLeadingCells *************************************************
  // ****************************************************************

  bool JetCalcnLeadingCells::setupJet(const Jet* j){
    m_sumE_cells=0;
    m_cell_energies.clear();
    m_cell_energies.reserve(j->numConstituents());
    return true;
  }


  bool JetCalcnLeadingCells::processConstituent(xAOD::JetConstituentVector::iterator& iter){

    double e = iter->e();
    m_cell_energies.push_back(e);
    m_sumE_cells+=e;
    return true;
  }

  double JetCalcnLeadingCells::jetCalculation() const {  
    std::vector<double> nc_cell_energies(m_cell_energies);
    return JetCaloQualityUtils::compute_nLeading(nc_cell_energies,m_sumE_cells,m_threshold);
  }


  // ****************************************************************
  // JetCalcOutOfTimeEnergyFraction *************************************************
  // ****************************************************************
  bool JetCalcOutOfTimeEnergyFraction::setupJet(const Jet*  ){
    m_sumE=0;
    m_sumE_OOT=0;
    return true;
  }


  double JetCalcOutOfTimeEnergyFraction::jetCalculation() const {  
    if( m_sumE== 0.) return -1;
    return m_sumE_OOT/m_sumE ;
  }

  bool JetCalcOutOfTimeEnergyFraction::processConstituent(xAOD::JetConstituentVector::iterator& iter){

    //double sum_all(0), sum_time(0);
    
    double aClusterE    = iter->e();
    double aClusterTime = m_constitExtractor->time(iter);//;static_cast<const xAOD::CaloCluster*>(iter->rawConstituent())->time();
    
    
    if(onlyPosEnergy && aClusterE<0) return true;

    m_sumE += aClusterE;
    if(fabs(aClusterTime) > timecut) m_sumE_OOT += aClusterE;

    return true;
  }

  // ****************************************************************
  // JetCalcTimeCells *************************************************
  // ****************************************************************
  bool JetCalcTimeCells::setupJet(const Jet* ){
    m_time = 0; m_norm = 0;
    return true;
  }



  bool JetCalcTimeCells::processConstituent(xAOD::JetConstituentVector::iterator& iter){

    double thisNorm = iter->e()* iter->e();
    m_time += m_constitExtractor->time(iter) * thisNorm; //theClus->time() * thisNorm ;
    m_norm += thisNorm;

    return true;
  }

  double JetCalcTimeCells::jetCalculation() const {  
    if (m_norm==0) return 0;
    return m_time/m_norm ;
  }


  // ****************************************************************
  // JetCalcAverageLArQualityF *************************************************
  // ****************************************************************
  bool JetCalcAverageLArQualityF::setupJet(const Jet* ){
    m_qf = 0; m_norm = 0;   return true;
  }


  double JetCalcAverageLArQualityF::jetCalculation() const {  
    if(m_norm==0) return 0;
    return  m_qf/m_norm;
  }

  bool JetCalcAverageLArQualityF::processConstituent(xAOD::JetConstituentVector::iterator& iter){

    double e2 = iter->e();
    e2 = e2*e2;

    m_norm+= e2;
    double qual(0);
    if(m_useTile) 
      qual = m_constitExtractor->moment(iter, xAOD::CaloCluster::AVG_TILE_Q);
    else
      qual = m_constitExtractor->moment(iter, xAOD::CaloCluster::AVG_LAR_Q);
    m_qf += qual*e2;
    return true;
  }

  // ****************************************************************
  // JetCalcQuality *************************************************
  // ****************************************************************
  bool JetCalcQuality::setupJet(const Jet* ){
    m_totE=0; m_badE=0;   return true;
  }


  double JetCalcQuality::jetCalculation() const {  
    if (m_totE==0) return 0;
    return m_badE/m_totE ;
  }


  bool JetCalcQuality::processConstituent(xAOD::JetConstituentVector::iterator& iter){
  
    m_totE += iter->e(); // using iter since it is set at the expected scale by the JetCaloCalculations instance
    double f = m_constitExtractor->moment(iter, xAOD::CaloCluster::BADLARQ_FRAC);

    m_badE += f * iter->e();
    return true;
  }


  // ****************************************************************
  // JetCalcQualityHEC *************************************************
  // ****************************************************************



  bool JetCalcQualityHEC::processConstituent(xAOD::JetConstituentVector::iterator& iter){
    double clustHEC = m_constitExtractor->energyHEC(iter);
    m_totE += clustHEC ;
    
    double f = m_constitExtractor->moment(iter, xAOD::CaloCluster::BADLARQ_FRAC);
    
    m_badE += f *clustHEC;
    return true;
  }

  // ****************************************************************
  // JetCalcNegativeEnergy *************************************************
  // ****************************************************************
  bool JetCalcNegativeEnergy::setupJet(const Jet* ){
    m_totE =0;
    return true;
  }


  double JetCalcNegativeEnergy::jetCalculation() const {  

    return m_totE;

  }

  bool JetCalcNegativeEnergy::processConstituent(xAOD::JetConstituentVector::iterator& iter){
 
   double e = iter->e() ;  // using iter since it is set at the expected scale by the JetCaloCalculations instance
     
    double epos= m_constitExtractor->moment(iter, xAOD::CaloCluster::ENG_POS);
    m_totE += (e - epos );

    return true;
  }

  // ****************************************************************
  // JetCalcCentroid *************************************************
  // ****************************************************************
  bool JetCalcCentroid::setupJet(const Jet* /*j*/){
    m_totE =0;
    m_centroid_x=0;  m_centroid_y=0;  m_centroid_z=0;
    return true;
  }


  double JetCalcCentroid::jetCalculation() const {  

    if (m_totE == 0) return 0;
  
    double c_x = m_centroid_x/ m_totE;
    double c_z = m_centroid_z/ m_totE;
    double c_y = m_centroid_y/ m_totE;

    return sqrt(c_x*c_x + c_y*c_y+ c_z*c_z);
  }

  bool JetCalcCentroid::processConstituent(xAOD::JetConstituentVector::iterator& iter){

    double e = iter->e() ;  // using iter since it is set at the expected scale by the JetCaloCalculations instance

    m_totE +=e;
    double x,y,z;

    x = m_constitExtractor->moment(iter, xAOD::CaloCluster::CENTER_X);
    y = m_constitExtractor->moment(iter, xAOD::CaloCluster::CENTER_Y);
    z = m_constitExtractor->moment(iter, xAOD::CaloCluster::CENTER_Z);

    m_centroid_x +=  x* e;
    m_centroid_y +=  y* e;
    m_centroid_z +=  z* e;

    return true;
  }

  // ****************************************************************
  // JetCalcBadCellsFrac *************************************************
  // ****************************************************************
  bool JetCalcBadCellsFrac::setupJet(const Jet* j){
    m_totE = j->jetP4(xAOD::JetEMScaleMomentum).E()  ; 
    m_badE = 0;
    return true;
  }


  bool JetCalcBadCellsFrac::processConstituent(xAOD::JetConstituentVector::iterator& iter){
    m_badE += m_constitExtractor->moment(iter, xAOD::CaloCluster::ENG_BAD_CELLS );
    return true;
  }

  double JetCalcBadCellsFrac::jetCalculation() const {  
    if (m_totE == 0) return 0;
    return m_badE / m_totE ;
  }


}
