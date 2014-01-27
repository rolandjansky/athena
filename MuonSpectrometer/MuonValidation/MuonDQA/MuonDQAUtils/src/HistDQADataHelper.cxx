/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonDQAUtils/HistDQADataHelper.h"

#include "TH1.h"
#include "TH2.h"
#include <iostream>

namespace MuonDQAHist {

  //  HistDQADataHelper* HistDQADataHelper::m_helperInstance = 0;
  HistDQADataHelper::HistDQADataMap HistDQADataHelper::m_histDQADataMap;

  HistDQADataHelper* HistDQADataHelper::getHelper() {
    // https://savannah.cern.ch/bugs/?45013
    // Sebastian Binet suggestion
    static HistDQADataHelper m_instance;
    static bool first = true;
    if (first) {
      first = false;
      //     m_helperInstance = &m_instance;
      //      m_helperInstance->loadDefaults();
      m_instance.loadDefaults();
    }
    //    return  m_helperInstance;
    return  &m_instance;
  }

  HistDQADataHelper::~HistDQADataHelper()
  {
    HistDQADataIt itdel_beg = m_histDQADataMap.begin();
    HistDQADataIt itdel_end = m_histDQADataMap.end();
    for(; itdel_beg!=itdel_end; ++itdel_beg) 
      {
	if (itdel_beg->second) delete itdel_beg->second; 
	itdel_beg->second=0;  
      }
    m_histDQADataMap.clear();
  }

  HistDQAData* HistDQADataHelper::histData( std::string name )  
  {
    HistDQADataIt it = m_histDQADataMap.find(name);
    if( it == m_histDQADataMap.end() ){
      return 0;
    }
    return it->second; 
  }

  bool HistDQADataHelper::addHistData( std::string name, HistDQAData* data ) 
  {
    if( !data ) return false;

    HistDQADataIt myIter = m_histDQADataMap.find(name);
    if( myIter != m_histDQADataMap.end() ) {
      delete data;
      return false;
    }

    m_histDQADataMap.insert( std::make_pair(name,data) );
    return true;
  }

  TH1* HistDQADataHelper::createHist( std::string name, std::string prefix )  
  {
    HistDQADataIt iter1 = m_histDQADataMap.find(name);
    if( iter1 == m_histDQADataMap.end() ) {
      //  std::cout << " Histogram not found " << name << std::endl;
      return 0;
    }
    else return createHist(*iter1->second,prefix);
  }
  

  TH2* HistDQADataHelper::createHist2D( std::string name, std::string prefix )  {
    HistDQADataIt iter2 = m_histDQADataMap.find(name);
    if( iter2 == m_histDQADataMap.end() ) {
      return 0;
    }
    if( iter2->second->dimension() != 2 ) return 0;
    HistDQAData2D* data2D = dynamic_cast<HistDQAData2D*>(iter2->second);
    if( !data2D ) return 0;
    return createHist2D( *data2D, prefix );
  }
  
  TProfile* HistDQADataHelper::createHistProf( std::string name, std::string prefix ) {
    HistDQADataIt iterP = m_histDQADataMap.find(name);
    if( iterP == m_histDQADataMap.end() ) {
      //      std::cout << " Histogram not found " << name << std::endl;
      return 0;
    }
    return createHistProf(*iterP->second,prefix);
  }

  TH1* HistDQADataHelper::createHist( HistDQAData& data, std::string prefix )  
  {
    if( data.dimension() == 1 ) return createHist1D(data,prefix);
    if( data.dimension() == 2 ) {
      HistDQAData2D* data2D = dynamic_cast< HistDQAData2D*>(&data);
      if( !data2D ) return 0;
      return createHist2D(*data2D, prefix);
    }
    return 0;
  }

  TH2* HistDQADataHelper::createHist2D(  HistDQAData2D& data, std::string prefix )  {
    prefix += data.title();
    TH2F* hist = new TH2F(prefix.c_str(),data.title().c_str(),data.xBins(),data.xMin(),data.xMax(),data.yBins(),data.yMin(),data.yMax());
    hist->SetXTitle(data.xAxisTitle().c_str());
    hist->SetYTitle(data.yAxisTitle().c_str());
    return hist;
  }

  TH1* HistDQADataHelper::createHist1D(  HistDQAData& data, std::string prefix )  {
    if( data.dimension() != 1 ) return 0;
    std::string histo_title = data.xAxisTitle()+" "+prefix;
    prefix += data.title();
    //    TH1F* hist = new TH1F(prefix.c_str(),data.title().c_str(),data.xBins(),data.xMin(),data.xMax());
    TH1F* hist = new TH1F(prefix.c_str(),histo_title.c_str(),data.xBins(),data.xMin(),data.xMax());
    //    hist->SetXTitle(data.xAxisTitle().c_str());
    //    hist->SetYTitle(data.yAxisTitle().c_str());
    return hist;
  }

  TProfile* HistDQADataHelper::createHistProf( HistDQAData& data, std::string prefix ) {
    if( data.dimension() != 1 ) return 0;
    std::string histo_title = data.xAxisTitle()+" "+prefix;
    prefix += data.title();
    TProfile* hist = new TProfile(prefix.c_str(),histo_title.c_str(),1,0,1,data.xMin(),data.xMax());
    return hist;
  }

  void HistDQADataHelper::loadDefaults() {
    // add some useful default HistDQAData objects

    m_histDQADataMap["nsegm"]  = new HistDQAData(1,0.,1.,"n_segments","Number of segments","Entries");
    m_histDQADataMap["chi2Prof"]  = new HistDQAData(1,0.,1000.,"segm_chi2Prof","Chi2/ndof profile","Entries");

    m_histDQADataMap["mdtResProf"]  = new HistDQAData(1,-10.,10.,"mdt_ResProf","MDT hit residuals profile (mm)","Entries");
    m_histDQADataMap["mdtPullProf"] = new HistDQAData(1,-10.,10.,"mdt_PullProf","MDT hit pulls profile","Entries");
    m_histDQADataMap["nmdtProf"]    = new HistDQAData(1,1.,100.,"nmdt_Prof","Number of MDT hits profile","Entries");
 	    
    m_histDQADataMap["chi2"]        = new HistDQAData(600,0.,300.,"chi2_per_ndof","Chi2 per ndof","Entries");
    m_histDQADataMap["ndof"]        = new HistDQAData(51,-0.5,50.5,"ndof","Number of degrees of freedom","Entries");
    m_histDQADataMap["prob"]        = new HistDQAData(50,0.,1.,"chi2_probability","Chi2 probability","Entries");

    m_histDQADataMap["phi"]         = new HistDQAData(30,-3.2,3.2,"segm_position_phi","Segment crossing point #phi position","Entries");
    m_histDQADataMap["theta"]       = new HistDQAData(30,0.,3.2,"segm_position_theta","Segment crossing point #theta position","Entries");
    m_histDQADataMap["eta"]         = new HistDQAData(30,-3.2,3.2,"segm_position_eta","Segment crossing point #eta position","Entries");
    m_histDQADataMap["z_muon"]      = new HistDQAData(100,-25000.,25000,"segm_position_z","Segment crossing point z position (mm)","Entries");
    m_histDQADataMap["r_muon"]      = new HistDQAData(100,0.,12000,"segm_position_r","Segment crossing point r position (mm)","Entries");
    m_histDQADataMap["dirphi"]      = new HistDQAData(30,-3.2,3.2,"segm_direction_phi","Segment direction #phi","Entries");
    m_histDQADataMap["direta"]      = new HistDQAData(30,-3.2,3.2,"segm_direction_eta","Segment direction #eta","Entries");

    m_histDQADataMap["momentum"]    = new HistDQAData(10000,0.,1000,"momentum","Momentum (GeV)","Entries");
    m_histDQADataMap["pt"]          = new HistDQAData(10000,0.,1000,"pt","Transverse momentum (GeV)","Entries");

    m_histDQADataMap["layers"]      = new HistDQAData(10,0.,10,"n_layers_per_segment","Number of MDT layers hit per segment","Entries");
    m_histDQADataMap["mlayers"]     = new HistDQAData(5,0.,5,"n_mlayers_per_segment","Number of MDT multilayers hit per segment","Entries");

    m_histDQADataMap["nall"]        = new HistDQAData(31,-0.5,30.5,"n_total_hits","Total number of Hits","Entries");
    m_histDQADataMap["nallCh"]      = new HistDQAData(11,-0.5,10.5,"n_total_chambers","Total number of Chambers","Entries");
    m_histDQADataMap["nprec"]       = new HistDQAData(61,-0.5,60.5,"n_precision_hits","Number of precision hits","Entries");
    m_histDQADataMap["nprecCh"]     = new HistDQAData(16,-0.5,15.5,"n_precision_chambers","Number of precision Chambers","Entries");
    m_histDQADataMap["nphiHits"]    = new HistDQAData(31,-0.5,30.5,"n_phi_hits","Number of #phi hits","Entries");
    m_histDQADataMap["nphiCh"]      = new HistDQAData(11,-0.5,10.5,"n_phi_chambers","Number of #phi Chambers","Entries");
    m_histDQADataMap["netaTrigHits"]= new HistDQAData(31,-0.5,30.5,"n_eta_trig_hits","Number of #eta trigger hits","Entries");
    m_histDQADataMap["netaTrigCh"]  = new HistDQAData(11,-0.5,10.5,"n_eta_trig_chambers","Number of #eta trigger Chambers","Entries");

    m_histDQADataMap["nmdt"]        = new HistDQAData(61,-0.5,60.5,"n_mdt_hits","Number of MDT Hits","Entries");
    m_histDQADataMap["nmdtCh"]      = new HistDQAData(11,-0.5,10.5,"n_mdt_chambers","Number of MDT Chambers","Entries");

    m_histDQADataMap["ncsc"]        = new HistDQAData(21,-0.5,20.5,"n_csc_hits","Number of CSC Hits","Entries");
    m_histDQADataMap["ncscEta"]     = new HistDQAData(11,-0.5,10.5,"n_cscEta_hits","Number of CSC #eta-Hits","Entries");
    m_histDQADataMap["ncscPhi"]     = new HistDQAData(11,-0.5,10.5,"n_cscPhi_hits","Number of CSC #phi-Hits","Entries");
    m_histDQADataMap["ncscEtaCh"]   = new HistDQAData(11,-0.5,10.5,"n_cscEta_chambers","Number of CSC #eta Chambers","Entries");
    m_histDQADataMap["ncscPhiCh"]   = new HistDQAData(11,-0.5,10.5,"n_cscPhi_chambers","Number of CSC #phi Chambers","Entries");

    m_histDQADataMap["nrpc"]        = new HistDQAData(21,-0.5,20.5,"n_rpc_hits","Number of RPC Hits","Entries");
    m_histDQADataMap["nrpcEta"]     = new HistDQAData(11,-0.5,10.5,"n_rpcEta_hits","Number of RPC #eta-Hits","Entries");
    m_histDQADataMap["nrpcPhi"]     = new HistDQAData(11,-0.5,10.5,"n_rpcPhi_hits","Number of RPC #phi-Hits","Entries");
    m_histDQADataMap["nrpcEtaCh"]   = new HistDQAData(11,-0.5,10.5,"n_rpcEta_chambers","Number of RPC #eta Chambers","Entries");
    m_histDQADataMap["nrpcPhiCh"]   = new HistDQAData(11,-0.5,10.5,"n_rpcPhi_chambers","Number of RPC #phi Chambers","Entries");

    m_histDQADataMap["ntgc"]        = new HistDQAData(21,-0.5,20.5,"n_tgc_hits","Number of TGC Hits","Entries");
    m_histDQADataMap["ntgcEta"]     = new HistDQAData(16,-0.5,15.5,"n_tgcEta_hits","Number of TGC #eta-Hits","Entries");
    m_histDQADataMap["ntgcPhi"]     = new HistDQAData(16,-0.5,15.5,"n_tgcPhi_hits","Number of TGC #phi-Hits","Entries");
    m_histDQADataMap["ntgcEtaCh"]   = new HistDQAData(11,-0.5,10.5,"n_tgcEta_chambers","Number of TGC #eta Chambers","Entries");
    m_histDQADataMap["ntgcPhiCh"]   = new HistDQAData(11,-0.5,10.5,"n_tgcPhi_chambers","Number of TGC #phi Chambers","Entries");

    m_histDQADataMap["nmdtMis"]     = new HistDQAData(31,-0.5,30.5,"n_mdtMis","Number of missed  MDT Hits","Entries");
    m_histDQADataMap["nmdtChMis"]   = new HistDQAData(11,-0.5,10.5,"n_mdtChMis","Number of missed MDT Chambers","Entries");
    m_histDQADataMap["ncscMis"]     = new HistDQAData(21,-0.5,20.5,"n_cscMis","Number of missed CSC Hits","Entries");
    m_histDQADataMap["ncscPhiMis"]  = new HistDQAData(11,-0.5,10.5,"n_cscEtaMis","Number of missed CSC #eta-Hits","Entries");
    m_histDQADataMap["ncscEtaMis"]  = new HistDQAData(11,-0.5,10.5,"n_cscPhiMis","Number of missed CSC #phi-Hits","Entries");
    m_histDQADataMap["ncscChMis"]   = new HistDQAData(11,-0.5,10.5,"n_cscChMis","Number of missed CSC Chambers","Entries");
    m_histDQADataMap["nrpcMis"]     = new HistDQAData(21,-0.5,20.5,"n_rpcMis","Number of missed RPC Hits","Entries");
    m_histDQADataMap["nrpcPhiMis"]  = new HistDQAData(11,-0.5,10.5,"n_rpcEtaMis","Number of missed RPC #eta-Hits","Entries");
    m_histDQADataMap["nrpcEtaMis"]  = new HistDQAData(11,-0.5,10.5,"n_rpcPhiMis","Number of missed RPC #phi-Hits","Entries");
    m_histDQADataMap["nrpcChMis"]   = new HistDQAData(11,-0.5,10.5,"n_rpcChMis","Number of missed RPC Chambers","Entries");
    m_histDQADataMap["ntgcMis"]     = new HistDQAData(21,-0.5,20.5,"n_tgcMis","Number of missed TGC Hits","Entries");
    m_histDQADataMap["ntgcPhiMis"]  = new HistDQAData(11,-0.5,10.5,"n_tgcEtaMis","Number of missed TGC #eta-Hits","Entries");
    m_histDQADataMap["ntgcEtaMis"]  = new HistDQAData(11,-0.5,10.5,"n_tgcPhiMis","Number of missed TGC #phi-Hits","Entries");
    m_histDQADataMap["ntgcChMis"]   = new HistDQAData(11,-0.5,10.5,"n_tgcChMis","Number of missed TGC Chambers","Entries");
    m_histDQADataMap["nmdtMa"]      = new HistDQAData(31,-0.5,30.5,"n_mdtMatch","Number of matched  MDT Hits","Entries");
    m_histDQADataMap["nmdtChMa"]    = new HistDQAData(11,-0.5,10.5,"n_mdtChMatch","Number of matched MDT Chambers","Entries");
    m_histDQADataMap["ncscMa"]      = new HistDQAData(21,-0.5,20.5,"n_cscMatch","Number of matched CSC Hits","Entries");
    m_histDQADataMap["ncscPhiMa"]   = new HistDQAData(11,-0.5,10.5,"n_cscEtaMatch","Number of matched CSC #eta-Hits","Entries");
    m_histDQADataMap["ncscEtaMa"]   = new HistDQAData(11,-0.5,10.5,"n_cscPhiMatch","Number of matched CSC #phi-Hits","Entries");
    m_histDQADataMap["ncscChMa"]    = new HistDQAData(11,-0.5,10.5,"n_cscChMatch","Number of matched CSC Chambers","Entries");
    m_histDQADataMap["nrpcMa"]      = new HistDQAData(21,-0.5,20.5,"n_rpcMatch","Number of matched RPC Hits","Entries");
    m_histDQADataMap["nrpcPhiMa"]   = new HistDQAData(11,-0.5,10.5,"n_rpcEtaMatch","Number of matched RPC #eta-Hits","Entries");
    m_histDQADataMap["nrpcEtaMa"]   = new HistDQAData(11,-0.5,10.5,"n_rpcPhiMatch","Number of matched RPC #phi-Hits","Entries");
    m_histDQADataMap["nrpcChMa"]    = new HistDQAData(11,-0.5,10.5,"n_rpcChMatch","Number of matched RPC Chambers","Entries");
    m_histDQADataMap["ntgcMa"]      = new HistDQAData(21,-0.5,20.5,"n_tgcMatch","Number of matched TGC Hits","Entries");
    m_histDQADataMap["ntgcPhiMa"]   = new HistDQAData(11,-0.5,10.5,"n_tgcEtaMatch","Number of matched TGC #eta-Hits","Entries");
    m_histDQADataMap["ntgcEtaMa"]   = new HistDQAData(11,-0.5,10.5,"n_tgcPhiMatch","Number of matched TGC #phi-Hits","Entries");
    m_histDQADataMap["ntgcChMa"]    = new HistDQAData(11,-0.5,10.5,"n_tgcChMatch","Number of matched TGC Chambers","Entries");

    m_histDQADataMap["allRes"]      = new HistDQAData(100,-10.,10.,"all_technologies_Res","All technologies hit residual (mm)","Entries");
    m_histDQADataMap["mdtRes"]      = new HistDQAData(100,-10.,10.,"mdt_Res","MDT hit residual (mm)","Entries");
    m_histDQADataMap["cscResEta"]   = new HistDQAData(100,-10.,10.,"cscEta_Res","CSC #eta hit residual (mm)","Entries");
    m_histDQADataMap["cscResPhi"]   = new HistDQAData(100,-10.,10.,"cscPhi_Res","CSC #phi hit residual (mm)","Entries");
    m_histDQADataMap["rpcResEta"]   = new HistDQAData(100,-100.,100.,"rpcEta_Res","RPC #eta hit residual (mm)","Entries");
    m_histDQADataMap["rpcResPhi"]   = new HistDQAData(100,-100.,100.,"rpcPhi_Res","RPC #phi hit residual (mm)","Entries");
    m_histDQADataMap["tgcResEta"]   = new HistDQAData(100,-100.,100.,"tgcEta_Res","TGC #eta hit residual (mm)","Entries");
    m_histDQADataMap["tgcResPhi"]   = new HistDQAData(100,-100.,100.,"tgcPhi_Res","TGC #phi hit residual (mm)","Entries");

    m_histDQADataMap["allPull"]     = new HistDQAData(100,-10.,10.,"all_technologies_Pull","All technologies hit pull","Entries");
    m_histDQADataMap["mdtPull"]     = new HistDQAData(100,-10.,10.,"mdt_Pull","MDT hit pull","Entries");
    m_histDQADataMap["cscPullEta"]  = new HistDQAData(100,-10.,10.,"cscEta_Pull","CSC #eta hit pull","Entries");
    m_histDQADataMap["cscPullPhi"]  = new HistDQAData(100,-10.,10.,"cscPhi_Pull","CSC #phi hit pull","Entries");
    m_histDQADataMap["rpcPullEta"]  = new HistDQAData(100,-20.,20.,"rpcEta_Pull","RPC #eta hit pull","Entries");
    m_histDQADataMap["rpcPullPhi"]  = new HistDQAData(100,-20.,20.,"rpcPhi_Pull","RPC #phi hit pull","Entries");
    m_histDQADataMap["tgcPullEta"]  = new HistDQAData(100,-20.,20.,"tgcEta_Pull","TGC #eta hit pull","Entries");
    m_histDQADataMap["tgcPullPhi"]  = new HistDQAData(100,-20.,20.,"tgcPhi_Pull","TGC #phi hit pull","Entries");

    m_histDQADataMap["allError"]    = new HistDQAData(100,0.,10.,"all_technologies_hitError","All technologies hit error (mm)","Entries");
    m_histDQADataMap["mdtError"]    = new HistDQAData(100,0.,10.,"mdt_hitError","MDT hit error (mm)","Entries");
    m_histDQADataMap["cscErrorEta"] = new HistDQAData(100,0.,10.,"cscEta_hitError","CSC #eta hit error (mm)","Entries");
    m_histDQADataMap["cscErrorPhi"] = new HistDQAData(100,0.,10.,"cscPhi_hitError","CSC #phi hit error (mm)","Entries");
    m_histDQADataMap["rpcErrorEta"] = new HistDQAData(100,0.,10.,"rpcEta_hitError","RPC #eta hit error (mm)","Entries");
    m_histDQADataMap["rpcErrorPhi"] = new HistDQAData(100,0.,10.,"rpcPhi_hitError","RPC #phi hit error (mm)","Entries");
    m_histDQADataMap["tgcErrorEta"] = new HistDQAData(100,0.,10.,"tgcEta_hitError","TGC #eta hit error (mm)","Entries");
    m_histDQADataMap["tgcErrorPhi"] = new HistDQAData(100,0.,10.,"tgcPhi_hitError","TGC #phi hit error (mm)","Entries");

    m_histDQADataMap["allRelPosX"]    = new HistDQAData(100,-2000.,2000.,"all_technologies_hitRelPosX","All technologies hit relative x-position (mm)","Entries");
    m_histDQADataMap["mdtRelPosX"]    = new HistDQAData(100,0.,1.,"mdt_hitRelPosX","MDT hit relative x-position (mm)","Entries");
    m_histDQADataMap["cscRelPosXEta"] = new HistDQAData(100,0.,1.,"cscEta_hitRelPosX","CSC #eta hit relative x-position (mm)","Entries");
    m_histDQADataMap["cscRelPosXPhi"] = new HistDQAData(100,0.,1.,"cscPhi_hitRelPosX","CSC #phi hit relative x-position (mm)","Entries");
    m_histDQADataMap["rpcRelPosXEta"] = new HistDQAData(100,-2000.,2000.,"rpcEta_hitRelPosX","RPC #eta hit relative x-position (mm)","Entries"); //
    m_histDQADataMap["rpcRelPosXPhi"] = new HistDQAData(100,-2000.,2000.,"rpcPhi_hitRelPosX","RPC #phi hit relative x-position (mm)","Entries"); //
    m_histDQADataMap["tgcRelPosXEta"] = new HistDQAData(100,0.,1.,"tgcEta_hitRelPosX","TGC #eta hit relative x-position (mm)","Entries");
    m_histDQADataMap["tgcRelPosXPhi"] = new HistDQAData(100,0.,1.,"tgcPhi_hitRelPosX","TGC #phi hit relative x-position (mm)","Entries");

    m_histDQADataMap["allRelPosY"]    = new HistDQAData(100,-2000.,2000.,"all_technologies_hitRelPosY","All technologies hit relative y-position (mm)","Entries");
    m_histDQADataMap["mdtRelPosY"]    = new HistDQAData(101,-2000.,2000.,"mdt_hitRelPosY","MDT hit relative y-position (mm)","Entries"); //
    m_histDQADataMap["cscRelPosYEta"] = new HistDQAData(100,0.,1.,"cscEta_hitRelPosY","CSC #eta hit relative y-position (mm)","Entries");
    m_histDQADataMap["cscRelPosYPhi"] = new HistDQAData(100,0.,1.,"cscPhi_hitRelPosY","CSC #phi hit relative y-position (mm)","Entries");
    m_histDQADataMap["rpcRelPosYEta"] = new HistDQAData(100,-2000.,2000.,"rpcEta_hitRelPosY","RPC #eta hit relative y-position (mm)","Entries"); //
    m_histDQADataMap["rpcRelPosYPhi"] = new HistDQAData(100,-2000.,2000.,"rpcPhi_hitRelPosY","RPC #phi hit relative y-position (mm)","Entries"); //
    m_histDQADataMap["tgcRelPosYEta"] = new HistDQAData(100,0.,1.,"tgcEta_hitRelPosY","TGC #eta hit relative y-position (mm)","Entries");
    m_histDQADataMap["tgcRelPosYPhi"] = new HistDQAData(100,0.,1.,"tgcPhi_hitRelPosY","TGC #phi hit relative y-position (mm)","Entries");

    m_histDQADataMap["mdtSignedRadius"] = new HistDQAData(101,-17.,17.,"mdt_hitSignedRadius","Signed drift radius of MDT hit (mm)","Entries");
    m_histDQADataMap["mdtRadius"] = new HistDQAData(101,0.,17.,"mdt_hitRadius","Drift radius of MDT hit (mm)","Entries");

  }


}
