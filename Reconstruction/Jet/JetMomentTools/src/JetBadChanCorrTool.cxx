/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

#include "GaudiKernel/ITHistSvc.h"
#include "StoreGate/WriteDecorHandle.h"
#include "PathResolver/PathResolver.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloEvent/CaloCellContainer.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "AthenaKernel/errorcheck.h"

#include "TFile.h"
#include "TH1.h"

#include "JetMomentTools/JetBadChanCorrTool.h"
#include "JetUtils/JetCellAccessor.h"
#include "JetUtils/JetDistances.h"
#include "AthenaKernel/Units.h"


using Athena::Units::GeV;


JetBadChanCorrTool::JetBadChanCorrTool(const std::string& name) :
  asg::AsgTool(name),
  m_thistSvc( "THistSvc", name )
{
  declareInterface<IJetDecorator>(this);
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
  }

  if(m_jetContainerName.empty()){
    ATH_MSG_ERROR("JetBadChanCorrTool needs to have its input jet container name configured!");
    return StatusCode::FAILURE;
  }

  ATH_CHECK(m_badCellMap_key.initialize());

  m_corrCellKey = m_jetContainerName + "." + m_corrCellKey.key();
  m_corrDotxKey       = m_jetContainerName + "." + m_corrDotxKey.key();
  m_corrJetKey        = m_jetContainerName + "." + m_corrJetKey.key();
  m_corrJetForCellKey = m_jetContainerName + "." + m_corrJetForCellKey.key();

  ATH_CHECK(m_corrCellKey.initialize());
  // These 3 aren't used if we're configured to use clusters
  ATH_CHECK(m_corrDotxKey.initialize(!m_useClusters));
  ATH_CHECK(m_corrJetKey.initialize(!m_useClusters));
  ATH_CHECK(m_corrJetForCellKey.initialize(!m_useClusters));

  return StatusCode::SUCCESS;
}

StatusCode JetBadChanCorrTool::setupEvent()
{
  return StatusCode::SUCCESS;
}


StatusCode JetBadChanCorrTool::decorate(const xAOD::JetContainer& jets) const
{

  if(m_useClusters) return correctionFromClustersBadCells(jets);

  auto handle = SG::makeHandle (m_badCellMap_key);
  if (!handle.isValid()){
    ATH_MSG_ERROR("Could not retieve bad cell map " << m_badCellMap_key.key());
    return StatusCode::FAILURE;
  }

  auto badCellMap = handle.cptr();

  // number of bad cells exceed the limit, set moments -1 and skip
  if((int) badCellMap->cells().size() >m_nBadCellLimit){
    SG::WriteDecorHandle<xAOD::JetContainer, float> corrCellHandle(m_corrCellKey);
    SG::WriteDecorHandle<xAOD::JetContainer, float> corrDotxHandle(m_corrDotxKey);
    SG::WriteDecorHandle<xAOD::JetContainer, float> corrJetHandle(m_corrJetKey);
    SG::WriteDecorHandle<xAOD::JetContainer, float> corrJetForCellHandle(m_corrJetForCellKey);
    for(const xAOD::Jet* jet : jets){
      corrCellHandle(*jet) = -1.;
      corrDotxHandle(*jet) = -1.;
      corrJetHandle(*jet) = -1.;
      corrJetForCellHandle(*jet) = -1.;
    }
    return StatusCode::SUCCESS;
  }

  return correctionFromCellsInJet(jets, badCellMap);
}

StatusCode JetBadChanCorrTool::correctionFromCellsInJet(const xAOD::JetContainer& jets, const jet::CaloCellFastMap* badCellMap) const {

  SG::WriteDecorHandle<xAOD::JetContainer, float> corrCellHandle(m_corrCellKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> corrDotxHandle(m_corrDotxKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> corrJetHandle(m_corrJetKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> corrJetForCellHandle(m_corrJetForCellKey);

  for(const xAOD::Jet* jet : jets){

    xAOD::JetFourMom_t p4 = jet->jetP4(xAOD::JetEMScaleMomentum);
    double rawPt = p4.Pt();
    double rawE  = p4.E();

    // jet moments, fraction
    double corr_jet_associate=0;
    double corr_jet_forcell=0;
    double corr_cell=0;
    double corr_dotx=0;

    jet::JetCellAccessor::const_iterator cellIt =  jet::JetCellAccessor::begin(jet);
    jet::JetCellAccessor::const_iterator cellItE = jet::JetCellAccessor::end(jet);

    for( ;cellIt!=cellItE; cellIt++) {

      const CaloDetDescrElement * dde = (*cellIt)->caloDDE();
      const CaloCell *cell = *cellIt;
      double cellWeight = cellIt.weight();

      double cell_energy = cell->e() * cellWeight;

      CaloCell_ID::CaloSample sampling = dde->getSampling();

      bool considerBad = cell->badcell();
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

        corr_jet_associate += frac;

        // corrected in cell-level
        if(cell_energy!=0){
          // for cryo.
          if(isOTX)
            corr_dotx += cell_energy;
          else
            corr_cell += cell_energy;

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

    corrCellHandle(*jet) = corr_cell;
    corrDotxHandle(*jet) = corr_dotx;
    corrJetForCellHandle(*jet) = corr_jet_forcell;
    if(m_useCone)
      corrJetHandle(*jet) = correctionFromCellsInCone(jet, badCellMap);
    else
      corrJetHandle(*jet) = corr_jet_associate;
  }
  return StatusCode::SUCCESS;
}

float JetBadChanCorrTool::correctionFromCellsInCone(const xAOD::Jet* jet, const jet::CaloCellFastMap* badCellMap) const {

  ATH_MSG_DEBUG( " Missing cells for cone search "<< badCellMap->size() << "  jet="<< jet->index() << "  R="
                 << jet->getSizeParameter() << "  input="<< jet->getInputType() << "  jet_eta="<<jet->eta() ) ;

  double corr_jet_cone=0;

  double rawPt = jet->jetP4(xAOD::JetEMScaleMomentum).Pt();

  double jeteta = jet->eta();
  double jetphi = jet->phi();
  std::vector<jet::CellPosition> closeCells = badCellMap->cellsInDeltaR(jeteta,jetphi, jet->getSizeParameter() );
  std::vector<jet::CellPosition>::iterator itr = closeCells.begin();
  std::vector<jet::CellPosition>::iterator itrE = closeCells.end();

  for(; itr!=itrE; itr++){

    double cell_eta = itr->x();
    double cell_phi = itr->phi();

    CaloCell_ID::CaloSample sampling = itr->sampling();

    double dr = jet::JetDistances::deltaR(jeteta,jetphi   , cell_eta, cell_phi);
    double frac_cell = getProfile(rawPt, dr, sampling, cell_eta, cell_phi);

    corr_jet_cone += frac_cell;
  }

  return corr_jet_cone;
}

StatusCode JetBadChanCorrTool::correctionFromClustersBadCells(const xAOD::JetContainer& jets) const {

  SG::WriteDecorHandle<xAOD::JetContainer, float> corrCellHandle(m_corrCellKey);

  for(const xAOD::Jet* jet : jets){

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
    if(rawE==0) corrCellHandle(*jet) = 0;
    else corrCellHandle(*jet) = corrCell / rawE;
  }
  return StatusCode::SUCCESS;
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
