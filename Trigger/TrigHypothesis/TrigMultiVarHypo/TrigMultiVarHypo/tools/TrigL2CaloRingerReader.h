/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TOOLS_TRIGL2CALORINGERREADER_H
#define TOOLS_TRIGL2CALORINGERREADER_H

//#include "TrigMultiVarHypo/preproc/TrigRingerPreprocessor.h"
#include "TrigMultiVarHypo/tools/TrigRingerHelper.h"
#include "TrigMultiVarHypo/tools/MultiLayerPerceptron.h"
#include "PathResolver/PathResolver.h"
#include "GaudiKernel/MsgStream.h"
#include <vector>
#include <string>
#include "TTree.h"
#include "TFile.h"

class TrigL2CaloRingerReader{

  public:
    /* constructor */
    TrigL2CaloRingerReader( std::string );
    /* destructor */
    ~TrigL2CaloRingerReader();
    /* use this methods to retrieve the tools from the archieve */
    bool retrieve(std::string &, std::vector<MultiLayerPerceptron*>               &);
    bool retrieve(std::string &, std::vector<TrigCaloRingsHelper::CutDefsHelper*> &);
    /* return the current class name */
    std::string name(){return m_name;};
    /* return luminosity flag */
    bool  useLumiVar(){return m_useLumiVar;};
    /* return eta var flag */
    bool  useEtaVar(){return m_useEtaVar;};
    /* if true, we will no use the activation function in the last layer */
    bool  useNoActivationFunctionInTheLastLayer(){return m_useNoActivationFunctionInTheLastLayer;};
    /* if true, apply the pileup correction */
    bool  doPileupCorrection(){return m_doPileupCorrection;};
    /* return the threshold lumi */
    int lumiCut(){return m_lumiCut;};
    /* use this to set the messenger stream output */
    void setMsgStream( MsgStream &msg ){
       m_log = new MsgStream(msg);
    }


  private:
    // Use this to retrieve all branch values
    template <class T>
    void InitBranch(TTree* fChain, std::string branch_name, T* param);
    // msg output 
    MsgStream& msg() const{
       return (*m_log);
    }
    
    std::string m_name;
    
    int m_version;
    
    MsgStream *m_log;

    // archieve variables holder
    int  m_lumiCut;
    bool m_useEtaVar;
    bool m_useLumiVar;
    bool m_useNoActivationFunctionInTheLastLayer;
    bool m_doPileupCorrection;

    std::vector<unsigned int>   *m_nodes;
    std::vector<double>         *m_weights;
    std::vector<double>         *m_bias;
    std::vector<double>         *m_thresholds;
    std::vector<double>         *m_etaBins;
    std::vector<double>         *m_etBins;
    std::vector<double>         *m_muBins;
  
};//End of classi
#endif
