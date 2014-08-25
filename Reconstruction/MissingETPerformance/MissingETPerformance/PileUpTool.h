/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PILEUP_TOOL_H 
#define PILEUP_TOOL_H 


#include "AthenaBaseComps/AthAlgTool.h"
#include <string>

class StoreGateSvc;
class MsgStream;
class ITHistSvc;
class TH1F;
class TH2F;
class TProfile;
class MissingETData;


static const InterfaceID IID_PileUpTool("PileUpTool", 1, 0);


class PileUpTool : public AthAlgTool
{

  public:
    
    PileUpTool(const std::string& type, const std::string& name, const IInterface* parent);
    
    // AlgTool and IAlgTool interface methods
    static const InterfaceID& interfaceID( ) { return IID_PileUpTool; };
    
    StatusCode CBNT_initialize();
    StatusCode CBNT_finalize();
    
    // Overriding initialize, finalize and execute
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode execute(MissingETData *data);
    
  inline void SetFolderName(const std::string& folderName) { m_folderName=folderName; }

  protected:
    
    // Standard destructor
    virtual ~PileUpTool( );
    
  private:
    
    StatusCode pileUp(MissingETData *data);
    bool getProfile( TH2F *hIn, TH1F *hOut, bool doMean );
    bool getMeanProfile( TH2F *hIn, TH1F *hOut );
    bool getResolutionProfile( TH2F *hIn, TH1F *hOut );
    
    std::string m_folderName;

    // Handle on the Hist/TTree registration service
    ITHistSvc *m_thistSvc;
    
    // Parameters for histograms
    std::string  m_vtxContName;
    unsigned int m_minTracksPerVertex;
    int          m_vtxBins, m_vtxRange;
    int          m_rtBins, m_etBins, m_stBins;
    float        m_rtRange, m_etRange, m_stRange;
    bool         m_doFit;
    
    // Histograms
    TH1F *h1_Nvtx;
    
    TH2F *h2_RefFinal_Nvtx_RatioEt;
    TH2F *h2_RefFinal_Nvtx_DeltaEt;
    TH2F *h2_RefFinal_Nvtx_DeltaEx;
    TH2F *h2_RefFinal_Nvtx_DeltaEy;
    
    TH1F *h1_RefFinal_Nvtx_ResponseEt;
    TH1F *h1_RefFinal_Nvtx_OffsetEt;
    TH1F *h1_RefFinal_Nvtx_ResolutionEt;
    TH1F *h1_RefFinal_Nvtx_ResolutionEx;
    TH1F *h1_RefFinal_Nvtx_ResolutionEy;
    
    TH2F **h2_RefFinal_SumEt_RatioEt_vtx;
    TH2F **h2_RefFinal_SumEt_DeltaEt_vtx;
    TH2F **h2_RefFinal_SumEt_DeltaEx_vtx;
    TH2F **h2_RefFinal_SumEt_DeltaEy_vtx;
    
    TH1F **h1_RefFinal_SumEt_ResponseEt_vtx;
    TH1F **h1_RefFinal_SumEt_OffsetEt_vtx;
    TH1F **h1_RefFinal_SumEt_ResolutionEt_vtx;
    TH1F **h1_RefFinal_SumEt_ResolutionEx_vtx;
    TH1F **h1_RefFinal_SumEt_ResolutionEy_vtx;
    
    TProfile *hp_RefFinal_Nvtx_SumEt;
};


inline bool PileUpTool::getMeanProfile( TH2F *hIn, TH1F *hOut )
{
  return getProfile( hIn, hOut, true );
}


inline bool PileUpTool::getResolutionProfile( TH2F *hIn, TH1F *hOut )
{
  return getProfile( hIn, hOut, false );
}


#endif // PILEUP_TOOL_H 
