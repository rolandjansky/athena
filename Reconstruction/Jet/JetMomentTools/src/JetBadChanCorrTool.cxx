/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

//#include "GaudiKernel/Service.h"
#include "GaudiKernel/ITHistSvc.h"

#include "PathResolver/PathResolver.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloEvent/CaloCellContainer.h"
//#include "CaloEvent/CaloCellPrefetchIterator.h"
//#include "CaloInterface/IHadronicCalibrationTool.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "AthenaKernel/errorcheck.h"


#include "TFile.h"
#include "TH1.h"

#include "JetMomentTools/JetBadChanCorrTool.h"
#include "JetUtils/JetCellAccessor.h"
#include "JetUtils/JetDistances.h"
#include "AthenaKernel/Units.h"


using Athena::Units::GeV;


JetBadChanCorrTool::JetBadChanCorrTool(	const std::string& name) :
  JetModifierBase(name),
  m_thistSvc( "THistSvc", name ),
  // m_caloDDM(0),
  // m_calo_id(0),
  //m_calibTool(this),
  //    m_missingCellToolHandle("MissingCellListTool"),
  // m_missingCellMapName(),
  m_forceMissingCellCheck(false),
  m_useClusters(false)
{

  // limit to calculate the moments, #bad ch is typically 3000 out of 187616
  declareProperty("NBadCellLimit", m_nBadCellLimit = 10000);
 

  // for jet-level correction
  declareProperty("StreamName", m_streamName = "/JetBadChanCorrTool/");
  declareProperty("ProfileName", m_profileName = "JetBadChanCorrTool.root");
  declareProperty("ProfileTag", m_profileTag = "");

  declareProperty("UseCone", m_useCone = true);
  //declareProperty("ConeDr", m_coneDr = 0.4);

  // for calibration
  declareProperty("UseCalibScale", m_useCalibScale = false);
  //  declareProperty("CellCalibrator", m_calibTool);

  //declareProperty("MissingCellTool", m_missingCellToolHandle);
  // declareProperty("MissingCellMap", m_missingCellMapName= "MissingCaloCellsMap");
  declareProperty("ForceMissingCellCheck", m_forceMissingCellCheck=false);
  declareProperty("UseClusters",m_useClusters= false);

  declareProperty("MissingCellMap", m_badCellMap_key="MissingCaloCellsMap");
  
}

JetBadChanCorrTool::~JetBadChanCorrTool()
{ }

StatusCode JetBadChanCorrTool::initialize()
{
  ATH_MSG_DEBUG("initializing version with data handles");

  if(!m_useClusters){
    std::string fname = PathResolver::find_file(m_profileName, "DATAPATH");

    if(fname==""){
      ATH_MSG(ERROR) << "Could not get file " << m_profileName << endmsg;
      return StatusCode::FAILURE;
    }
    TFile tf(fname.c_str());
    if(tf.IsOpen()==false){
      ATH_MSG( ERROR ) << "Could not open file " << fname << endmsg;
      return StatusCode::FAILURE;
    }
    // already registered hists
    std::vector<std::string> histsInSvc = m_thistSvc->getHists();

    TListIter next(tf.GetListOfKeys());
    TObject *obj;
    while((obj = next())) {
      // assume hname=(tag)_sample(sample)_pt(ptMin)_(ptMax)_eta(etaMin)_(etaMax)_phi(phiMin)_(phiMax)
      std::string hname=obj->GetName();
      if(hname.find('_')==std::string::npos) continue;
      std::string tag = hname.substr(0,hname.find('_'));
      std::string para = hname.substr(hname.find('_')+1);
      std::string location = m_streamName+hname;

      if(m_profileTag=="" || m_profileTag==tag){
        // read in histo not registered yet
        if(find(histsInSvc.begin(),histsInSvc.end(),location)==histsInSvc.end()){
          StatusCode sc = m_thistSvc->regHist(location);
          if(sc.isFailure()){
            ATH_MSG( ERROR ) << "failed to read histo " << location << endmsg;
            return StatusCode::FAILURE;
          }
        }
        int sample=0;
        double ptMin=0;
        double ptMax=9999;
        double etaMin=0;
        double etaMax=5.0;
        double phiMin=-M_PI;
        double phiMax=M_PI;
        int ret = sscanf(para.c_str(),"sample%d_pt%lf_%lf_eta%lf_%lf_phi%lf_%lf",
                         &sample,&ptMin,&ptMax,&etaMin,&etaMax,&phiMin,&phiMax);

        if(ret<1 || sample<0 || sample>=CaloCell_ID::Unknown) {
          ATH_MSG( DEBUG ) << "Could not understand the name of hist " << obj->GetName() << endmsg;
          continue;
        }

        TH1* th=0;
        StatusCode sc = m_thistSvc->getHist(location,th);
        if(sc.isFailure()){
          ATH_MSG( ERROR ) << "failed to get histo " << location << endmsg;
          return StatusCode::FAILURE;
        }
        m_profileDatas[sample].push_back(ProfileData((TH1D*)th,sample,ptMin,ptMax,etaMin,etaMax,phiMin,phiMax));
        ATH_MSG( DEBUG ) << "read hist=" << th->GetName() 
                         << " tag=" << tag << " sample=" << sample 
                         << " ptMin=" << ptMin << " ptMax=" << ptMax 
                         << " etaMin=" << etaMin << " etaMax=" << etaMax
                         << " phiMin=" << phiMin << " phiMax=" << phiMax << endmsg;
      }
    }

    // if(m_useCalibScale){
    //   CHECK( m_calibTool.retrieve() ) ;
    //   ATH_MSG( DEBUG ) << "use calibration " << m_calibTool << endmsg;
    // } 
  
    if (  m_useCone) {
      // CHECK( detStore()->retrieve(m_caloDDM) );
      // m_calo_id = m_caloDDM->getCaloCell_ID();
      // ATH_MSG( DEBUG ) << "perform bad cells in cone calculations "  << endmsg;
      // if(m_calo_id==0){
      //   ATH_MSG( ERROR ) << "Could not get CaloCell_ID" << endmsg;
      //   return StatusCode::FAILURE;
      // }
    }


    //  CHECK( m_missingCellToolHandle.retrieve() );
    //m_missingCellTool = dynamic_cast<MissingCellListTool*>( &(*m_missingCellToolHandle));

  }
 
  ATH_CHECK(m_badCellMap_key.initialize());

  return StatusCode::SUCCESS;
}

StatusCode JetBadChanCorrTool::setupEvent()
{

  //  CHECK(m_missingCellTool->prepareCellList());
  
  return StatusCode::SUCCESS;
}


int JetBadChanCorrTool::modifyJet( xAOD::Jet& jet) const
{

  int res = 0;
  if(m_useClusters){
    res = correctionFromClustersBadCells( &jet);
  }else{
    
    auto handle = SG::makeHandle (m_badCellMap_key);
    if (!handle.isValid()){
      ATH_MSG_ERROR("Could not retieve bad cell map "
                    << m_badCellMap_key.key());
      // << m_missingCellMapName);
      return 1;
    }

    auto badCellMap = handle.cptr();
    
    // number of bad cells exceed the limit, set -1 and skip    
    if((int) badCellMap->cells().size() >m_nBadCellLimit){
      jet.setAttribute<float>("BchCorrCell",-1.0);
      if(!m_useClusters){
        jet.setAttribute<float>("BchCorrDotx",-1.);
        jet.setAttribute<float>("BchCorrJet",-1.);
        jet.setAttribute<float>("BchCorrJetForCell",-1.);
      }
      return 0;
    }
    
    
    res = correctionFromCellsInJet(&jet, badCellMap);
    // cone-based boundary
    if(m_useCone) {
      ATH_MSG_DEBUG( " Missing cells for cone search "<< badCellMap->size() << "  jet="<< jet.index() << "  R="<< jet.getSizeParameter() << "  input="<< jet.getInputType() << "  jet_eta="<<jet.eta() ) ;

      res+=correctionFromCellsInCone(&jet, badCellMap);
    }
  }


  // // for cryo.
  // if(useCalibTool) {

  //   // without cell correction
  //   double emb3_raw = emb3-emb3_cell;
  //   double tile1_raw = tile1-tile1_cell;
  //   double cryoEt_raw = m_calibTool->etCryo(emb3_raw,tile1_raw);
  //   double emb3_rawotx = emb3-emb3_dotx;
  //   double tile1_rawotx = tile1-tile1_dotx;
  //   double cryoEt_rawotx = m_calibTool->etCryo(emb3_rawotx,tile1_rawotx);

  //   double cryoEt = m_calibTool->etCryo(emb3,tile1);
  //   corr_cell += (cryoEt-cryoEt_raw) / rawPt;
  //   corr_dotx += (cryoEt-cryoEt_rawotx) / rawPt;

  //   double cryoEt_jet_forcell = m_calibTool->etCryo(emb3_raw+emb3_jet_forcell,tile1_raw+tile1_jet_forcell);
  //   corr_jet_forcell += (cryoEt_jet_forcell-cryoEt_raw) / rawPt;

  //   double cryoEt_jet_associate = m_calibTool->etCryo(emb3_raw+emb3_jet_associate,tile1_raw+tile1_jet_associate);
  //   corr_jet_associate += (cryoEt_jet_associate-cryoEt_raw) / rawPt;

  //   double cryoEt_jet_cone = m_calibTool->etCryo(emb3_raw+emb3_jet_cone,tile1_raw+tile1_jet_cone);
  //   corr_jet_cone += (cryoEt_jet_cone-cryoEt_raw) / rawPt;

  //   total_energy += cryoEt*cosh(jet.eta());



  //   double calib_scale = total_energy/rawE;
  //   corr_cell /= calib_scale;
  //   corr_dotx /= calib_scale;
  //   corr_jet_associate /= calib_scale;
  //   corr_jet_cone /= calib_scale; 
  //   corr_jet_forcell /= calib_scale; 

  //   if(corr_cell         <1e-7) corr_cell         =0;
  //   if(corr_dotx         <1e-7) corr_dotx         =0;
  //   if(corr_jet_associate<1e-7) corr_jet_associate=0;
  //   if(corr_jet_cone     <1e-7) corr_jet_cone     =0;
  //   if(corr_jet_forcell  <1e-7) corr_jet_forcell  =0;
  // }
  
  
 
  return res;
}


int JetBadChanCorrTool::correctionFromCellsInJet( xAOD::Jet* jet, const jet::CaloCellFastMap * ) const {

  //const jet::cellset_t & badcells = badCellMap.cells() ;

  xAOD::JetFourMom_t p4 = jet->jetP4(xAOD::JetEMScaleMomentum);
  double rawPt = p4.Pt();
  double rawE  = p4.E();

  // jet moments, fraction
  double corr_jet_associate=0;
  //double corr_jet_cone=0;
  double corr_jet_forcell=0;
  double corr_cell=0;
  double corr_dotx=0;

  // jet energy
  double total_energy=0;//for calib, with h1 weight but without jet-scale corr.

  // for cryo., et at em-scale
  // double emb3  = 0.;
  // double tile1 = 0.;
  // double emb3_cell  = 0.;
  // double tile1_cell = 0.;
  // double emb3_dotx  = 0.;
  // double tile1_dotx = 0.;
  // double emb3_jet_associate  = 0.;
  // double tile1_jet_associate = 0.;
  // double emb3_jet_cone  = 0.;
  // double tile1_jet_cone = 0.;
  // double emb3_jet_forcell  = 0.;
  // double tile1_jet_forcell = 0.;
 
  //  bool topoJet = dynamic_cast<const CaloCluster*>(*jet.begin()) != 0;

  ///bool useCalibTool = m_useCalibScale && jet.signalState()==P4SignalState::CALIBRATED;
  
  //  const IMissingCellListTool::list_t & missingCells = m_missingCellTool->extendedList();

  jet::JetCellAccessor::const_iterator cellIt =  jet::JetCellAccessor::begin(jet);
  jet::JetCellAccessor::const_iterator cellItE = jet::JetCellAccessor::end(jet);
    
  for( ;cellIt!=cellItE; cellIt++) {

    const CaloDetDescrElement * dde = (*cellIt)->caloDDE();
    const CaloCell *cell = *cellIt;
    double cellWeight = cellIt.weight();

    double cell_energy = cell->e() * cellWeight;
    // if(useCalibTool) {
    //   cell_energy *= m_calibTool->wtCell(cell);
    // }
    
    total_energy += cell_energy;    

    CaloCell_ID::CaloSample sampling = dde->getSampling();
    
    // // for cryo.
    // if (sampling == CaloCell_ID::EMB3) emb3 += cell->et();
    // if (sampling == CaloCell_ID::TileBar0) tile1 += cell->et();
    
    bool considerBad = cell->badcell();
    //if(considerBad) badCellList.push_back( cell->ID() );
    //    if(m_forceMissingCellCheck) considerBad = considerBad || ( missingCells.find( cell->ID() ) == missingCells.end() );
    // for bad channels in jet
    if(considerBad){

      // determine if this comes from a deadOTX
      bool isOTX = false;
      if(!(dde->is_tile()))
        if(cell->provenance() & 0x0200) 
          isOTX = true;
      
      double dr = jet::JetDistances::deltaR(jet->eta(),jet->phi(),dde->eta(), dde->phi());
      double frac_cell = getProfile(rawPt, dr, sampling, dde->eta(), dde->phi());

      double frac = frac_cell * cellWeight;
      // if(useCalibTool) {
      //   frac *= m_calibTool->wtCell(cell);
      // }
      
      // for cryo.
      // if (sampling == CaloCell_ID::EMB3) emb3_jet_associate += frac_cell * rawPt;
      // if (sampling == CaloCell_ID::TileBar0) tile1_jet_associate += frac_cell * rawPt;
      
      corr_jet_associate += frac;
      
      // corrected in cell-level
      if(cell_energy!=0){
        // for cryo.
        if(isOTX)
          {
            // if (sampling == CaloCell_ID::EMB3) emb3_dotx += cell->et();
            // if (sampling == CaloCell_ID::TileBar0) tile1_dotx += cell->et();
            corr_dotx += cell_energy ;
          } 
        else
          {
            // if (sampling == CaloCell_ID::EMB3) emb3_cell += cell->et();
            // if (sampling == CaloCell_ID::TileBar0) tile1_cell += cell->et();
            corr_cell += cell_energy ;
          }

        // if (sampling == CaloCell_ID::EMB3) emb3_jet_forcell += frac_cell * rawPt;
        // if (sampling == CaloCell_ID::TileBar0) tile1_jet_forcell += frac_cell * rawPt;
        
        corr_jet_forcell += frac;
        
      }
    }
  } // loop over cells

  const double inv_rawE = 1. / rawE;
  corr_cell *= inv_rawE;
  corr_dotx *= inv_rawE;
  ATH_MSG( DEBUG ) << "pt=" << jet->pt()/GeV << " eta=" << jet->eta() << " phi=" << jet->phi()
                   << " BCH_CORR_CELL=" << corr_cell
                   << " BCH_CORR_DOTX=" << corr_dotx
                   << " BCH_CORR_JET=" << corr_jet_associate
                   << " BCH_CORR_JET_FORCELL=" << corr_jet_forcell << endmsg;

  jet->setAttribute<float>("BchCorrCell",corr_cell);
  jet->setAttribute<float>("BchCorrDotx",corr_dotx);
  if(!m_useCone) jet->setAttribute<float>("BchCorrJet",  corr_jet_associate);
  jet->setAttribute<float>("BchCorrJetForCell",corr_jet_forcell);
  
  return 0;
}

int JetBadChanCorrTool::correctionFromCellsInCone(xAOD::Jet* jet, const jet::CaloCellFastMap * badCellMap) const {

  double corr_jet_cone=0;
  
  double rawPt = jet->jetP4(xAOD::JetEMScaleMomentum).Pt();
  
  // badCellList.insert( badCellList.end(), m_missingCellTool->begin() , m_missingCellTool->end() );
    
  
  double jeteta = jet->eta();
  double jetphi = jet->phi();
  std::vector<jet::CellPosition> closeCells = badCellMap->cellsInDeltaR(jeteta,jetphi, jet->getSizeParameter() );
  std::vector<jet::CellPosition>::iterator itr = closeCells.begin();
  std::vector<jet::CellPosition>::iterator itrE = closeCells.end();

  for(; itr!=itrE; itr++){

    double cell_eta = itr->x();
    double cell_phi = itr->phi();

    // const CaloDetDescrElement * dde = m_caloDDM->get_element(itr->m_id);
    // CaloCell_ID::CaloSample sampling = dde->getSampling();
    CaloCell_ID::CaloSample sampling = itr->sampling();

    double dr = jet::JetDistances::deltaR(jeteta,jetphi   , cell_eta, cell_phi);
    double frac_cell = getProfile(rawPt, dr, sampling, cell_eta, cell_phi);
      
    //double frac = frac_cell;

    // if(useCalibTool) {

    //   CaloCell temp(dde,frac_cell*rawE,0,0,CaloGain::UNKNOWNGAIN);//to pick up weight 
    //   frac *= m_calibTool->wtCell(&temp);
    // }
      
    // if (sampling == CaloCell_ID::EMB3) emb3_jet_cone += frac_cell * rawPt;
    // if (sampling == CaloCell_ID::TileBar0) tile1_jet_cone += frac_cell * rawPt;
        
    corr_jet_cone += frac_cell;
  }
  
  jet->setAttribute<float>("BchCorrJet", corr_jet_cone );

  return 0;
}

int JetBadChanCorrTool::correctionFromClustersBadCells( xAOD::Jet* jet) const {

  double corrCell=0;

  size_t nconstit=jet->numConstituents();
  for(size_t i=0; i<nconstit; i++) {
    // use the raw constituents since we're interested only in one moment :
    const xAOD::IParticle* constit = jet->rawConstituent(i);
    if( constit->type() != xAOD::Type::CaloCluster ) continue;
    double badE; 
    bool v = static_cast<const xAOD::CaloCluster*>(constit)->retrieveMoment( xAOD::CaloCluster::ENG_BAD_CELLS, badE);
    if(v) corrCell += badE;
  }

  double rawE  = jet->jetP4(xAOD::JetEMScaleMomentum).E();
  if(rawE==0)jet->setAttribute<float>("BchCorrCell", 0. );
  else jet->setAttribute<float>("BchCorrCell", corrCell / rawE );
  return 0;  
}





double JetBadChanCorrTool::getProfile(double pt, double dr, int sample, double eta, double phi) const {
  std::vector<ProfileData>::const_iterator itr =m_profileDatas[sample].begin();
  std::vector<ProfileData>::const_iterator itrE =m_profileDatas[sample].end();
  for(; itr!=itrE; itr++){
    if((*itr).match(pt/GeV,sample,eta,phi)){
      return (*itr).frac(dr);
    }
  }
  return 0;
}



#endif
