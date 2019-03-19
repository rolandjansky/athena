/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMULTIVARHYPO_RINGERREADER_H
#define TRIGMULTIVARHYPO_RINGERREADER_H

#include "TrigMultiVarHypo/tools/procedures/IModel.h"
#include "TrigMultiVarHypo/tools/procedures/IThresholds.h"
#include "TrigMultiVarHypo/tools/procedures/INormalization.h"
#include "TrigMultiVarHypo/tools/procedures/Thresholds.h"
#include "TrigMultiVarHypo/tools/procedures/MultiLayerPerceptron.h"

#include "PathResolver/PathResolver.h"
#include <vector>
#include <string>
#include "TTree.h"
#include "TFile.h"
#include <TROOT.h>
#include <memory>


#include "AsgTools/AsgMessaging.h"

namespace Ringer{

  
  class RingerReader: public asg::AsgMessaging{
  
    public:
      /* constructor */
      RingerReader( std::string );
      /* destructor */
      ~RingerReader();
      
      /* use this methods to retrieve the tools from the archieve */
      bool retrieve(std::string &, std::vector<std::shared_ptr<Ringer::IModel>>      &);
      bool retrieve(std::string &, std::vector<std::shared_ptr<Ringer::IThresholds>> &);
      


      int   lumiCut()               const {return m_lumiCut;};
      bool  doPileupCorrection()    const {return m_doPileupCorrection;};
      bool  removeOutputTansigTF()  const {return m_removeOutputTansigTF;};
      bool  useTrack()              const {return m_useTrack;};
      bool  useCaloRings()          const {return m_useCaloRings;};
      bool  useShowerShape()        const {return m_useShowerShape;};
      bool  useTileCal()            const {return m_useTileCal;};
      
      std::string name()            const {return m_name;};
 
      
 

  
    private:
      // Use this to retrieve all branch values
      template <class T>
      void InitBranch(TTree* fChain, std::string branch_name, T* param);
     
      
      std::string m_name;
      int m_version;
      
      // archieve variables holder
      int  m_lumiCut;
      bool m_useCaloRings;
      bool m_useTrack;
      bool m_useShowerShape;
      bool m_useTileCal;
      bool m_removeOutputTansigTF;
      bool m_doPileupCorrection;
  
      // Dense Model
      std::vector<unsigned int>   *m_nodes;
      std::vector<double>         *m_weights;
      std::vector<double>         *m_bias;
      std::vector<double>         *m_thresholds;
      std::vector<std::string>    *m_tfnames;
  
      // Convolutional Model
      std::vector<bool>           *m_useConvLayer;
      std::vector<unsigned int>   *m_conv_nodes;
      std::vector<unsigned int>   *m_conv_kernel_i;
      std::vector<unsigned int>   *m_conv_kernel_j;
      std::vector<double>         *m_conv_kernel;
      std::vector<double>         *m_conv_bias;
      std::vector<unsigned int>   *m_conv_input_i;
      std::vector<unsigned int>   *m_conv_input_j;
      std::vector<std::string>    *m_conv_tfnames;
      std::vector<unsigned int>   *m_conv_frame;
  
      // Common parans
      std::vector<double>         *m_etaBins;
      std::vector<double>         *m_etBins;
      std::vector<double>         *m_muBins;
   


  };//End of classi

}
#endif
