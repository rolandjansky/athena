
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
#include "BoostedJetTaggers/DeepsetXbbTagger.h"


namespace DerivationFramework {

  
  class ExKtbbAugmentation : public AthAlgTool, public IAugmentationTool {


  public:
    ExKtbbAugmentation(const std::string& t, const std::string& n, const IInterface* p);
    ~ExKtbbAugmentation();
    StatusCode initialize();
    StatusCode finalize();
    virtual StatusCode addBranches() const;
    int getDeXTerLabel(const int ghostBFinalCount,const int ghostCFinalCount) const;

   private:
    std::string m_eventInfoName;    
    std::string m_secvtxName;
    std::string m_largeJetCollectionName;
    std::string m_smallJetCollectionName;
    std::string m_exktJetCollectionName;
    std::string m_primaryVerticesCollectionName;
    bool m_isMC;

    ToolHandle<DeepsetXbbTagger> m_dexter;

    const SG::AuxElement::Decorator<std::vector<float> > secvtx_pt;
    const SG::AuxElement::Decorator<std::vector<float> > secvtx_eta;
    const SG::AuxElement::Decorator<std::vector<float> > secvtx_phi;
    const SG::AuxElement::Decorator<std::vector<float> > secvtx_mass;
    const SG::AuxElement::Decorator<std::vector<float> > secvtx_vtxx;
    const SG::AuxElement::Decorator<std::vector<float> > secvtx_vtxy;
    const SG::AuxElement::Decorator<std::vector<float> > secvtx_vtxz;
    const SG::AuxElement::Decorator<std::vector<float> > secvtx_lxy;
    const SG::AuxElement::Decorator<std::vector<float> > secvtx_lxysig;
    const SG::AuxElement::Decorator<std::vector<float> > secvtx_lz;
    const SG::AuxElement::Decorator<std::vector<float> > secvtx_lzsig;
    const SG::AuxElement::Decorator<std::vector<float> > secvtx_3dsig;
    const SG::AuxElement::Decorator<float> jet_maxsd0;
    const SG::AuxElement::Decorator<float> jet_av3sd0;
    const SG::AuxElement::Decorator<float> jet_dexter_pbb;
    const SG::AuxElement::Decorator<float> jet_dexter_pb;
    const SG::AuxElement::Decorator<float> jet_dexter_pl;
    const SG::AuxElement::Decorator<float> jet_dexter_pbb_trksflip;
    const SG::AuxElement::Decorator<float> jet_dexter_pb_trksflip;
    const SG::AuxElement::Decorator<float> jet_dexter_pl_trksflip;
    const SG::AuxElement::Decorator<float> jet_dexter_pbb_trksflip_svmassflip;
    const SG::AuxElement::Decorator<float> jet_dexter_pb_trksflip_svmassflip;
    const SG::AuxElement::Decorator<float> jet_dexter_pl_trksflip_svmassflip;
    const SG::AuxElement::Decorator<float> jet_dexter_pbb_negtrksflip;
    const SG::AuxElement::Decorator<float> jet_dexter_pb_negtrksflip;
    const SG::AuxElement::Decorator<float> jet_dexter_pl_negtrksflip;
    const SG::AuxElement::Decorator<float> jet_dexter_pbb_negtrksflip_svmassflip;
    const SG::AuxElement::Decorator<float> jet_dexter_pb_negtrksflip_svmassflip;
    const SG::AuxElement::Decorator<float> jet_dexter_pl_negtrksflip_svmassflip;
    const SG::AuxElement::Decorator<int> smalljet_largeJetLabel;
    const SG::AuxElement::Decorator<int> jet_dexter_ghostBhadronCount;
    const SG::AuxElement::Decorator<int> jet_dexter_ghostChadronCount;
    const SG::AuxElement::Decorator<int> jet_dexter_TruthLabel;

  }; /// class

} /// namespace


#endif
