
/**
 *  * @file DerivationFrameworkTop/ExKtbbAugmentation.h
 *   * @author Mazin Khader
 *    * @date Feb. 2017
 *     * @brief tool to add variables for the double b-tagger for low mass h->2a->4b search
 *     */


#ifndef DerivationFramework_ExKtbbAugmentation_H
#define DerivationFramework_ExKtbbAugmentation_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthContainers/AuxElement.h"  
#include "GaudiKernel/ToolHandle.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"


namespace DerivationFramework {

  
  class ExKtbbAugmentation : public AthAlgTool, public IAugmentationTool {


  public:
    ExKtbbAugmentation(const std::string& t, const std::string& n, const IInterface* p);
    ~ExKtbbAugmentation();
    StatusCode initialize();
    StatusCode finalize();
    virtual StatusCode addBranches() const;
    
  private:
    std::string m_eventInfoName;    
    std::string m_secvtxName;
    std::string m_largeJetCollectionName;
    std::string m_smallJetCollectionName;
    std::string m_exktJetCollectionName;
    std::string m_primaryVerticesCollectionName;
    bool m_isMC;

    struct ExKtbbDecorators {

      static SG::AuxElement::Decorator<std::vector<float> > secvtx_pt;
      static SG::AuxElement::Decorator<std::vector<float> > secvtx_eta;
      static SG::AuxElement::Decorator<std::vector<float> > secvtx_phi;
      static SG::AuxElement::Decorator<std::vector<float> > secvtx_mass;
      static SG::AuxElement::Decorator<std::vector<float> > secvtx_vtxx;
      static SG::AuxElement::Decorator<std::vector<float> > secvtx_vtxy;
      static SG::AuxElement::Decorator<std::vector<float> > secvtx_vtxz;
      static SG::AuxElement::Decorator<std::vector<float> > secvtx_lxy;
      static SG::AuxElement::Decorator<std::vector<float> > secvtx_lxysig;
      static SG::AuxElement::Decorator<std::vector<float> > secvtx_lz;
      static SG::AuxElement::Decorator<std::vector<float> > secvtx_lzsig;
      static SG::AuxElement::Decorator<std::vector<float> > secvtx_3dsig;
      static SG::AuxElement::Decorator<float> jet_maxsd0;
      static SG::AuxElement::Decorator<float> jet_av3sd0;
      static SG::AuxElement::Decorator<int> smalljet_largeJetLabel;

    };
    
    ExKtbbDecorators tj_decorators;    

  }; /// class

} /// namespace


#endif
