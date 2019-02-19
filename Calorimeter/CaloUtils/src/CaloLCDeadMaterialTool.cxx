/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#undef DEBUG_DMTHINGS
#undef MAKE_MOMENTS
#include "CaloUtils/CaloLCDeadMaterialTool.h"
#include "CaloUtils/CaloLCCoeffHelper.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "CaloEvent/CaloPrefetch.h"
#include "CaloEvent/CaloRecoStatus.h"
#include "CaloConditions/CaloLocalHadDefs.h"
#include "CxxUtils/prefetch.h"

#include "xAODCaloEvent/CaloClusterKineHelper.h"

#include "CLHEP/Units/SystemOfUnits.h"

using CLHEP::MeV;
using xAOD::CaloCluster;

#ifdef MAKE_MOMENTS
void set_zero_moments(CaloCluster *theCluster);
#endif

/* ****************************************************************************

**************************************************************************** */
CaloLCDeadMaterialTool::CaloLCDeadMaterialTool(const std::string& type,
					       const std::string& name,
					       const IInterface* parent)
  : AthAlgTool(type,name,parent),
    m_key("HadDMCoeff2"),
    m_recoStatus(CaloRecoStatus::UNKNOWNSTATUS),
    m_weightModeDM(1),
    m_MinClusterEnergyToDeal(200.0*MeV),
    m_MinLookupBinNentry(40),
    m_MinCellEnergyToDeal(0.0),
    m_MaxChangeInCellWeight(30.0),
    m_useHadProbability(false),
    m_interpolate(false),
    m_absOpt(false)
{
  declareInterface<IClusterCellWeightTool>(this);
  declareProperty("HadDMCoeffKey",m_key);
  declareProperty("ClusterRecoStatus",m_recoStatus);
  declareProperty("WeightModeDM",m_weightModeDM) ;
  declareProperty("MinClusterEnergyToDeal", m_MinClusterEnergyToDeal);
  declareProperty("MinLookupBinNentry", m_MinLookupBinNentry);
  declareProperty("MinCellEnergyToDeal", m_MinCellEnergyToDeal);
  declareProperty("MaxChangeInCellWeight", m_MaxChangeInCellWeight);
  declareProperty("UseHadProbability",m_useHadProbability);
  // Use Interpolation or not
  declareProperty("Interpolate",m_interpolate);
  // list of dimensions to interpolate trought in 3 different type of areas
  std::vector<std::string > vstr;
  vstr.resize(2);
  vstr[0] = "DIMD_ETA";
  vstr[1] = "DIMD_ENER";
  m_interpolateDimensionNames[std::string("AREA_DMFIT")] = vstr;
  vstr.resize(3);
  vstr[0] = "DIMD_ETA";
  vstr[1] = "DIMD_ENER";
  vstr[2] = "DIMD_LAMBDA";
  m_interpolateDimensionNames[std::string("AREA_DMLOOKUP")] = vstr;
  vstr.resize(2);
  vstr[0] = "DIMD_ETA";
  vstr[1] = "DIMD_LAMBDA";
  m_interpolateDimensionNames[std::string("AREA_DMSMPW")] = vstr;
  declareProperty("InterpolateDimensionNames", m_interpolateDimensionNames);
  declareProperty("UpdateSamplingVars",m_updateSamplingVars=false);
  //Use weighting of negative clusters?
  declareProperty("WeightingOfNegClusters",m_absOpt);
}


/* ****************************************************************************

**************************************************************************** */
CaloLCDeadMaterialTool::~CaloLCDeadMaterialTool()
{

}


/* ****************************************************************************
- CaloLCDeadMaterialTool::initialize
**************************************************************************** */
StatusCode CaloLCDeadMaterialTool::initialize()
{
  if(m_interpolate) {
    msg(MSG::INFO) << "Interpolation is ON, dimensions: ";
    for(std::map<std::string, std::vector<std::string> >::iterator it=m_interpolateDimensionNames.begin(); it!=m_interpolateDimensionNames.end(); it++){
      msg(MSG::INFO) << " " << (*it).first << " (";
      for(std::vector<std::string >::iterator it2 = (*it).second.begin(); it2!=(*it).second.end(); it2++) {
        msg() << (*it2) << " ";
      }
      msg() << ")";
    }
    msg() << endmsg;
    for(std::map<std::string, std::vector<std::string> >::iterator it=m_interpolateDimensionNames.begin(); it!=m_interpolateDimensionNames.end(); it++){
      std::vector<int > *vtmp=0;
      if((*it).first == "AREA_DMFIT") {
        vtmp = &m_interpolateDimensionsFit;
      }else if((*it).first == "AREA_DMLOOKUP") {
        vtmp = &m_interpolateDimensionsLookup;
      }else if((*it).first == "AREA_DMSMPW") {
        vtmp = &m_interpolateDimensionsSampling;
      }else{
        msg(MSG::WARNING) << "Unkown dead material area type '" << (*it).first << "'" << std::endl;
        continue;
      }
      for(std::vector<std::string >::iterator it2 = (*it).second.begin(); it2!=(*it).second.end(); it2++) {
        CaloLocalHadDefs::LocalHadDimensionId id = CaloLCCoeffHelper::getDimensionId( (*it2) );
        if(id!=CaloLocalHadDefs::DIMU_UNKNOWN) {
          vtmp->push_back(int(id));
        }else{
          ATH_MSG_WARNING( "Dimension '" << (*it2) << "' is invalid and will be excluded."  );
        }
      }
    }
  } // m_interpolate

  ATH_MSG_INFO( "Initializing " << name()  );


  ATH_CHECK(m_key.initialize());

  return StatusCode::SUCCESS;
}



/* ****************************************************************************
- DeadMaterialCorrectionTool2::execute
**************************************************************************** */
StatusCode  CaloLCDeadMaterialTool::weight(CaloCluster* theCluster) const
{
  CaloLCCoeffHelper hp;
  CaloLocalHadCoeff::LocalHadCoeff parint;

  float wrong_dm_energy = 0.0;
  float dm_total = 0.0;

   
  double weightMom (1);
  if (!theCluster->retrieveMoment(CaloCluster::DM_WEIGHT,weightMom)) {
    ATH_MSG_WARNING ("Cannot find cluster moment DM_WEIGHT");
  }
  /* WTF?? re-setting the same moment??
  theCluster->insertMoment(CaloClusterMoment::DM_WEIGHT,CaloClusterMoment(new_weight),true); 
  */

  double eWeighted = theCluster->e();

//   // check desired reco status
//   float cls_em_fraction = 0.0;
//   if ( theCluster->checkRecoStatus(CaloRecoStatus::TAGGEDHAD)) {
//      cls_em_fraction = 0.25;
//   } else if( theCluster->checkRecoStatus(CaloRecoStatus::TAGGEDEM)) {
//     cls_em_fraction = 0.99;
//   }else {
//     log << MSG::DEBUG<<"Cluster was not selected for local DM calibration. Cluster reco status differs from TAGGEDHAD or TAGGEDEM."
//         << " Cluster energy " << theCluster->e() << " remains the same." << endmsg;
// #ifdef MAKE_MOMENTS
//     set_zero_moments(theCluster);
// #endif
//     return StatusCode::SUCCESS;
//   }

  if ( theCluster->e() <= m_MinClusterEnergyToDeal) {
    ATH_MSG_DEBUG("Cluster was not selected for local DM calibration, ecls < m_MinClusterEnergyToDeal (=" <<m_MinClusterEnergyToDeal << ").");
#ifdef MAKE_MOMENTS
    set_zero_moments(theCluster);
#endif
    return StatusCode::SUCCESS;
  }

  double pi0Prob = 0;
  if ( m_useHadProbability) {
    if (!theCluster->retrieveMoment(CaloCluster::EM_PROBABILITY,pi0Prob)) {
      ATH_MSG_ERROR("Cannot retrieve EM_PROBABILITY cluster moment, "
                    << " cluster energy " << theCluster->e() << " remains the same."  );
      return StatusCode::FAILURE;
    }
    if ( pi0Prob < 0 ) {
      pi0Prob = 0;
    } else if ( pi0Prob > 1 ) {
      pi0Prob = 1;
    }
  } else if (theCluster->recoStatus().checkStatus(CaloRecoStatus::TAGGEDEM)) {
    pi0Prob = 1;
  }

  double center_lambda = 0;
  if ( !theCluster->retrieveMoment(CaloCluster::CENTER_LAMBDA, center_lambda) ){
    ATH_MSG_ERROR("Cannot retrieve CENTER_LAMBDA cluster moment, "
                  << " cluster energy " << theCluster->e() << " remains the same."  );
    return StatusCode::FAILURE;
  }

  const CaloLocalHadCoeff* data(0);
  SG::ReadCondHandle<CaloLocalHadCoeff> rch(m_key);
  data = *rch;
  if(data==0) {
    ATH_MSG_ERROR("Unable to access conditions object");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("Cluster is selected for local DM calibration."
		<< " Old cluster energy:" << theCluster->e() 
		<< " m_weightModeDM:" << m_weightModeDM);

  // calculation of specific cluster quantities for DM correction (i.e. core of procedure)
  std::vector<Area> areas;
  std::vector<Cell> cells;
  float smp_energy[CaloSampling::Unknown];
  float cls_unweighted_energy = 0;
  StatusCode sc = prepare_for_cluster(theCluster, areas, cells,
                                      smp_energy, cls_unweighted_energy, data);
  if ( !sc.isSuccess() ) {
#ifdef MAKE_MOMENTS
    set_zero_moments(theCluster);
#endif
    ATH_MSG_ERROR(  "prepare for cluster failed!"  );
    return sc;
  }

  float cls_initial_energy =  theCluster->e();
  float cls_side = (theCluster->eta()<0?-1.0:1.0);
  float cls_eta = fabs(theCluster->eta());
  float cls_phi = theCluster->phi();
  float log10ener = 0.0;
  if(cls_unweighted_energy > 0.0) {
    log10ener = log10(cls_unweighted_energy);
  }
  float log10lambda;
  if(center_lambda > 0.0) {
    log10lambda = log10(center_lambda);
    if(log10lambda >=4.0) log10lambda = 3.9999;
  }else{
    log10lambda = 0.0;
  }
#ifdef DEBUG_DMTHINGS
  std::cout << " cls_initial_energy: " << cls_initial_energy << " cls_eta: " << cls_eta << std::endl;
  std::cout << " log10ener: " << log10ener << " log10lambda: " << log10lambda << std::endl;
  for(int i_smp=0; i_smp<CaloSampling::Unknown; i_smp++){
    std::cout << " i_smp: " << i_smp << " smp_energy: " << smp_energy[i_smp] << std::endl;
  }
#endif

  std::vector<float > vars;
  vars.resize(6, 0.0);
  vars[CaloLocalHadDefs::DIMD_EMFRAC] = pi0Prob;
  vars[CaloLocalHadDefs::DIMD_SIDE] = cls_side;
  vars[CaloLocalHadDefs::DIMD_ETA] = cls_eta;
  vars[CaloLocalHadDefs::DIMD_PHI] = cls_phi;
  vars[CaloLocalHadDefs::DIMD_ENER] = log10ener;
  vars[CaloLocalHadDefs::DIMD_LAMBDA] = log10lambda;

  size_t n_dm = data->getSizeAreaSet();

  // loop over HAD/EM possibilities for mixing different correction types
  for(int i_mix=0; i_mix<2; i_mix++){ // 0 - pure HAD case, 1-pure EM case
    float mixWeight = (i_mix==0?(1-pi0Prob):pi0Prob);
    if(mixWeight == 0) continue;

    vars[CaloLocalHadDefs::DIMD_EMFRAC] = float(i_mix);

    // loop over DM areas defined and calculation of cluster DM energy deposited in these areas
    for(size_t i_dm=0; i_dm < n_dm; i_dm++){
      if(areas[i_dm].eprep <= 0.0) continue; // no appropriate signal to reconstruct dm energy in this area

      const CaloLocalHadCoeff::LocalHadArea *area = data->getArea(i_dm);
      float emax = area->getDimension(CaloLocalHadDefs::DIMD_ENER)->getXmax();
      if(log10ener > emax) log10ener = emax - 0.0001;
      vars[CaloLocalHadDefs::DIMD_ENER] = log10ener;

      const CaloLocalHadCoeff::LocalHadCoeff *pars = data->getCoeff(i_dm, vars);
      if( !pars ) continue;

      float edm = 0.0;
      // if dm area is reconstructed using polynom fit approach
      if(area->getType() == CaloLocalHadDefs::AREA_DMFIT) {
        edm = (*pars)[CaloLocalHadDefs::BIN_P0] + (*pars)[CaloLocalHadDefs::BIN_P1]*areas[i_dm].eprep;
        if(m_interpolate) {
          bool isa = hp.Interpolate(data, i_dm, vars, parint, m_interpolateDimensionsFit, areas[i_dm].eprep);
          // calculation of fitted values is done already in the interpolator
          if(isa) edm = parint[CaloLocalHadDefs::BIN_P0];
        }

      // if dm area is reconstructed using lookup table approach
      }else if(area->getType() == CaloLocalHadDefs::AREA_DMLOOKUP){
        if( (*pars)[CaloLocalHadDefs::BIN_ENTRIES] > m_MinLookupBinNentry) edm = cls_unweighted_energy*((*pars)[CaloLocalHadDefs::BIN_WEIGHT] - 1.0 );
        if(m_interpolate) {
          bool isa = hp.Interpolate(data, i_dm, vars, parint, m_interpolateDimensionsLookup);
          if(isa && parint[CaloLocalHadDefs::BIN_ENTRIES] > m_MinLookupBinNentry) edm = cls_unweighted_energy*(parint[CaloLocalHadDefs::BIN_WEIGHT] - 1.0 );
        }

      // if dm area is reconstructed using new sampling weights
      }else if(area->getType() == CaloLocalHadDefs::AREA_DMSMPW){
	const int nSmp=pars->size()-1;
	//std::cout << "size=" << nSmp << ", unkown value=" << CaloSampling::Unknown << ", minifcal=" << CaloSampling::MINIFCAL0 << std::endl;
        double ecalonew = 0.0;
        double ecaloold = 0.0;
        for(int i_smp=0; i_smp<nSmp; i_smp++){
          ecaloold += smp_energy[i_smp];
          ecalonew += smp_energy[i_smp] * (*pars)[i_smp];
        }
        ecalonew += (*pars)[nSmp]; // const Used to be CaloSampling::Unknown but the value of this enum has changed to include the miniFCAL. 
        edm = ecalonew - ecaloold;

//         if(m_interpolate) {
//           bool isa = hp.Interpolate(data, i_dm, vars, parint, m_interpolateDimensionsSampling);
//           if(isa) {
//             ecalonew = 0.0;
//             ecaloold = 0.0;
//             for(int i_smp=0; i_smp<CaloSampling::Unknown; i_smp++){
//               ecaloold += smp_energy[i_smp];
//               ecalonew += smp_energy[i_smp] * parint[i_smp];
//             }
//             ecalonew += parint[CaloSampling::Unknown]; // const
//             edm = ecalonew - ecaloold;
//           }
//         }

      } else{
        std::cout << "CaloLCDeadMaterialTool -> Error! Unknown correction type " << area->getType() 
          << " with number of parameters " << area->getNpars() << std::endl;
      }
      wrong_dm_energy += areas[i_dm].edm_wrong;

#ifdef DEBUG_DMTHINGS
      std::cout << "   result> edm: " << edm << " edm_wrong:" << edm_wrong << std::endl;
#endif
      edm -= areas[i_dm].edm_wrong;
      if(edm > 0.0) {
        areas[i_dm].erec = areas[i_dm].erec + edm*mixWeight;
        dm_total += edm;
      }
    } // i_dm
  } // i_mix

  // giving of calculated DM energy to cluster by one of 3 methods
  // m_weightModeDM=0 - simple setting of cluster energy to new value
  // m_weightModeDM=1 - changing (increasing) of weights of cluster cells
  // m_weightModeDM=2 - changing (increasing) of weights of cluster cells (which were invloved into DM calculation)

#ifdef DEBUG_DMTHINGS
  std::cout << "wc> calculation of weights" << std::endl;
#endif

  if(dm_total > 0.0) {
    if (m_weightModeDM == 0) {
      // method 0: setting cluster energy to new value without changing of cells weights
      theCluster->setE(cls_initial_energy + dm_total - wrong_dm_energy);

    } else if (m_weightModeDM == 1) {
      // method1: calculation of weights of all cluster cells to treat DM energy.
      // Weights of all cluster cells will be changed proportionally
      float sub_ener_old = 0.0;
      for(size_t i_c = 0; i_c < cells.size(); i_c++)
      {
        const Cell& cell = cells[i_c];
        if (cell.energy <= m_MinCellEnergyToDeal) continue;
        sub_ener_old += cell.weight*cell.energy;
      }
      if(sub_ener_old > 0.0) {
        float corr = (sub_ener_old + dm_total)/sub_ener_old;
        for(size_t i_c = 0; i_c < cells.size(); i_c++)
        {
          if(cells[i_c].energy <= m_MinCellEnergyToDeal) continue;
          cells[i_c].weight *= corr;
        }
      }

    } else if (m_weightModeDM == 2) {
      // method2: calculation of weights of all cluster cells to treat DM energy.
      // only weights for cells involved into DM calculation will be changed

      for (size_t cell_index = 0; cell_index < cells.size(); ++cell_index)
      {
        const Cell& cell = cells[cell_index];
        if (cell.energy <= m_MinCellEnergyToDeal) continue;
        float we = cell.weight * cell.energy;
        if (cell.dm != sDM)
          areas[cell.dm].sub_ener_old += we;
        areas[sDM_FCAL].sub_ener_old += we;
        areas[sDM_LEAKAGE].sub_ener_old += we;
        areas[sDM_UNCLASS].sub_ener_old += we;
      }

      for(size_t i_dm=0; i_dm < areas.size(); i_dm++) {
        Area& dma = areas[i_dm];
        float corrfac = 1;
        if (i_dm >= n_dm || dma.eprep <= 0)
          corrfac = 1;
        else if (dma.sub_ener_old > 0)
          corrfac = (dma.sub_ener_old + dma.erec) / dma.sub_ener_old;

        if (i_dm < sDM_FCAL)
          areas[sDM_FCAL].sub_ener_old += (corrfac-1)*dma.sub_ener_old;
        if (i_dm < sDM_LEAKAGE)
          areas[sDM_LEAKAGE].sub_ener_old += (corrfac-1)*dma.sub_ener_old;
        if (i_dm < sDM_UNCLASS)
          areas[sDM_UNCLASS].sub_ener_old += (corrfac-1)*dma.sub_ener_old;

        dma.corr = corrfac;
      }

      float gblfac = areas[sDM_FCAL].corr * areas[sDM_LEAKAGE].corr * areas[sDM_UNCLASS].corr;
      for (size_t cell_index = 0; cell_index < cells.size(); ++cell_index)
      {
        Cell& cell = cells[cell_index];
        if (cell.energy <= m_MinCellEnergyToDeal) continue;
        if (cell.dm != sDM)
          cell.weight *= areas[cell.dm].corr;
        cell.weight *= gblfac;
      }
    } else {
      ATH_MSG_ERROR( "Unknown m_weightModeDM " << m_weightModeDM  );
    }

    // setting of calculated weights to the cluster cells
    if (m_weightModeDM == 1 || m_weightModeDM == 2){
      int cell_index = 0;
      CaloCluster::cell_iterator itrCell = theCluster->cell_begin();
      CaloCluster::cell_iterator itrCellEnd = theCluster->cell_end();
      for (;itrCell!=itrCellEnd; ++itrCell) {
        //CaloPrefetch::nextDDE(itrCell, itrCellEnd); no DDE info needed
        const float old_weight = itrCell.weight();
        float new_weight = cells[cell_index].weight;
        if( new_weight < m_MaxChangeInCellWeight*old_weight ){
          theCluster->reweightCell(itrCell, new_weight);
        }else{
          new_weight = old_weight; //Why that? Just for the printout below?
        }
#ifdef DEBUG_DMTHINGS
        std::cout << "    cells> " << cell_index << " ener: " << cells[cell_index].energy << " old_w: " << old_weight << " new_w:" << new_weight << std::endl;
        if( new_weight > 100) std::cout << "DeadMaterialCorrectionTool2 -> Panic! Too large weight" << std::endl;
#endif
        cell_index++;
      }//end loop over cells
      CaloClusterKineHelper::calculateKine(theCluster,true,m_updateSamplingVars);
    }//end if method 1 or 2

  } // dm_total > 0.0

  ATH_MSG_DEBUG("cls_initial_energy: " << cls_initial_energy 
		<< " (contains wrongly estimated DM energy: " << wrong_dm_energy << ")"
		<< " calculated DM energy (to be added):" << dm_total
		<< " new cluster energy:" << theCluster->e());

#ifdef MAKE_MOMENTS
  // to save reconstructed energy in different DM areas as cluster moments
  CaloCluster::MomentType xtype = CaloCluster::ENG_CALIB_DEAD_TOT;
  double xmom;
  bool result = theCluster->retrieveMoment(xtype, xmom, false);

  if(result) {
    double x;
    bool useLink = true;

    x = (double)smp_energy[CaloSampling::PreSamplerB];
    theCluster->insertMoment(CaloCluster::ENG_RECO_EMB0, x);

    x = (double)smp_energy[CaloSampling::PreSamplerE];
    theCluster->insertMoment(CaloCluster::ENG_RECO_EME0, x);

    x = (double)smp_energy[CaloSampling::TileGap3];
    theCluster->insertMoment(CaloCluster::ENG_RECO_TILEG3, x);

    x = (double)(dm_total+wrong_dm_energy);
    theCluster->insertMoment(CaloCluster::ENG_RECO_DEAD_TOT, x);

    for(size_t i_dm=0; i_dm < areas.size(); i_dm++) {
      Area& dma = areas[i_dm];
      CaloCluster::MomentType m_type = (CaloCluster::MomentType) (int(CaloCluster::ENG_RECO_DEAD_EMB0)+i_dm);
      x = (double)dma.erec;
      if(i_dm == sDM_EMB0_EMB1 && smp_energy[CaloSampling::PreSamplerB]>0.0) x+= smp_energy[CaloSampling::PreSamplerB];
      if(i_dm == sDM_EME0_EME12 && smp_energy[CaloSampling::PreSamplerE] > 0.0) x+= smp_energy[CaloSampling::PreSamplerE];
      if(i_dm == sDM_SCN && smp_energy[CaloSampling::TileGap3] > 0.0) x+= smp_energy[CaloSampling::TileGap3];
      theCluster->insertMoment(m_type, CaloClusterMoment(x));
    }
  }

#endif

  // assume that the weighting could be called more than once. In that
  // case eWeighted is the result of the previous step and the current
  // e/eWeighted ratio should be multiplied with the existing
  // DM_WEIGHT moment

  if ( eWeighted > 0 || eWeighted < 0 ) {
    weightMom *= theCluster->e()/eWeighted;
  }
  theCluster->insertMoment(CaloCluster::DM_WEIGHT,weightMom); 
  
  return StatusCode::SUCCESS;
}


#ifdef MAKE_MOMENTS
void set_zero_moments(CaloCluster *theCluster) {
  CaloCluster::MomentType xtype = CaloCluster::ENG_CALIB_DEAD_TOT;
  double xmom;
  bool result = theCluster->retrieveMoment(xtype, xmom);
  if(result) {
    theCluster->insertMoment(CaloCluster::ENG_RECO_EMB0, 0.0);
    theCluster->insertMoment(CaloCluster::ENG_RECO_EME0, 0.0);
    theCluster->insertMoment(CaloCluster::ENG_RECO_TILEG3, 0.0);
    theCluster->insertMoment(CaloCluster::ENG_RECO_DEAD_TOT, 0.0);
    for(size_t i_dm=0; i_dm < DeadMaterialCorrectionTool2::sDM; i_dm++) {
      CaloCluster::MomentType m_type = (CaloCluster::MomentType) (int(CaloCluster::ENG_RECO_DEAD_EMB0)+i_dm);
      theCluster->insertMoment(m_type, 0.0);
    }
  }
}
#endif



/* ****************************************************************************
- Finding energy and noise in different "areas" of cluster. Some of these areas
  correspond to CaloSamplings, others are special.
- Filling array m_eprep[sDM], it will be used later for reconstruction of DM
  energy in different zones as m_dmrec[i] = fun(m_eprep[i]);
**************************************************************************** */
StatusCode
CaloLCDeadMaterialTool::prepare_for_cluster
  (const CaloCluster* theCluster,
   std::vector<Area>& areas,
   std::vector<Cell>& cells,
   float* smp_energy,
   float& cls_unweighted_energy, const CaloLocalHadCoeff* data) const
{
  areas.resize(data->getSizeAreaSet());
  cells.reserve (theCluster->size());

  bzero(smp_energy, CaloSampling::Unknown*sizeof(float));

  // Finding of energy and noise in different "areas" of cluster. Some of these
  // areas correspond to CaloSamplings, others are special.
  int cell_index = 0;
  CaloCluster::const_cell_iterator itrCell = theCluster->cell_begin();
  CaloCluster::const_cell_iterator itrCellEnd = theCluster->cell_end();
  cls_unweighted_energy = 0;
  for (;itrCell!=itrCellEnd; ++itrCell) {
    CxxUtils::prefetchNext(itrCell, itrCellEnd);
    const CaloCell* thisCell = *itrCell;
    //Identifier id = thisCell->ID();
    CaloSampling::CaloSample nsmp=thisCell->caloDDE()->getSampling();
    //CaloSampling::CaloSample nsmp = CaloSampling::CaloSample(m_calo_id->calo_sample(id));

    float energy = thisCell->e();
    float weight = itrCell.weight();
    //float sigma =  m_noiseTool->getNoise(thisCell,ICalorimeterNoiseTool::ELECTRONICNOISE);
    cls_unweighted_energy += energy;

    Cell cell;
    cell.weight = weight;
    cell.energy = energy;
    cell.dm = sDM;

    smp_energy[nsmp] += energy; // unweighted energy in samplings for given cluster

    if(energy > m_MinCellEnergyToDeal) {

      if ( nsmp == CaloSampling::PreSamplerB  || nsmp == CaloSampling::EMB1) {
        cell.dm = sDM_EMB0_EMB1;

      } else if( nsmp == CaloSampling::TileGap3 ) {
        cell.dm = sDM_SCN;

      } else if ( nsmp == CaloSampling::PreSamplerE || nsmp == CaloSampling::EME1 ) {
        cell.dm = sDM_EME0_EME12;

      } else if (nsmp == CaloSampling::EME3) {
        cell.dm = sDM_EME3_HEC0;

      } else if (nsmp == CaloSampling::HEC0) {
        cell.dm = sDM_EME3_HEC0;

      } else if (nsmp == CaloSampling::EMB3) {
        cell.dm = sDM_EMB3_TILE0;

      } else if (nsmp == CaloSampling::TileBar0) {
        cell.dm = sDM_EMB3_TILE0;
      }

    } // cell_ener > cell_min_energy
    cells.push_back (cell);
    cell_index++;
  } // itrCell

//   Realculate sampling energy as the abs value of the original cluster, if you summed up energies, fluctuations wouldn't cancel and sample energy would be huge   
 if(m_absOpt){
    
    for(unsigned int i=0; i != CaloSampling::Unknown; ++ i)   smp_energy[i] = fabs(theCluster->eSample((CaloSampling::CaloSample)i));
    
  } 
  
  
  // calculation of 'signal' which will be used for DM energy estimation
  float x(0), y(0);

  // sDM_EMB0_EMB1: energy before EMB0 and between EMB0 and EMB1
  x = smp_energy[CaloSampling::PreSamplerB];
  if ( x > 0.) {
    areas[sDM_EMB0_EMB1].eprep = x;
    areas[sDM_EMB0_EMB1].edm_wrong = x;
  }

  // sDM_EMB3_TILE0: to correct energy between barrel and tile
  x = smp_energy[CaloSampling::EMB3];
  y = smp_energy[CaloSampling::TileBar0];
  if ( x > 0. && y>0.) areas[sDM_EMB3_TILE0].eprep = sqrt(x*y);

  // sDM_SCN: to correct energy before scintillator
  x = smp_energy[CaloSampling::TileGap3];
  if ( x > 0.) {
    areas[sDM_SCN].eprep = x;
    areas[sDM_SCN].edm_wrong = x;
  }

  // sDM_EME0_EME12: sum of energy before EME0 and between EME0 and EME1
  x = smp_energy[CaloSampling::PreSamplerE];
  if ( x > 0.) {
    areas[sDM_EME0_EME12].eprep = x;
    areas[sDM_EME0_EME12].edm_wrong = x;
  }

  // sDM_EME3_HEC0: to correct energy between EMEC and HEC
  x = smp_energy[CaloSampling::EME3];
  y = smp_energy[CaloSampling::HEC0];
  if ( x > 0. && y>0.) areas[sDM_EME3_HEC0].eprep = sqrt(x*y);

  areas[sDM_FCAL].eprep = cls_unweighted_energy;
  areas[sDM_LEAKAGE].eprep = cls_unweighted_energy;
  areas[sDM_UNCLASS].eprep = cls_unweighted_energy;

 return StatusCode::SUCCESS;
}

