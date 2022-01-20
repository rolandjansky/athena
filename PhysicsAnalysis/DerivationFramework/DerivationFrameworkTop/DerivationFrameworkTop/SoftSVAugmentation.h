
/**
 *  * @file DerivationFrameworkTop/SoftSVAugmentation.h
 *   * @author Judith Höfer
 *    * @date Jan. 2021
 *     * @brief tool to add additional variables to the soft secondary vertices
 *     */


#ifndef DerivationFramework_SoftSVAugmentation_H
#define DerivationFramework_SoftSVAugmentation_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthContainers/AuxElement.h"  
#include "GaudiKernel/ToolHandle.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"


namespace DerivationFramework {

  
  class SoftSVAugmentation : public AthAlgTool, public IAugmentationTool {


  public:
    SoftSVAugmentation(const std::string& t, const std::string& n, const IInterface* p);
    StatusCode initialize();
    virtual StatusCode addBranches() const;
    
  private:
    std::string m_eventInfoName;    
    std::string m_SoftSVCollectionName_tc_lvt_Loose;
    std::string m_SoftSVCollectionName_tc_lvt_Medium;
    std::string m_SoftSVCollectionName_tc_lvt_Tight;
    std::string m_SoftSVCollectionName_t_lvt;
    std::string m_primaryVerticesCollectionName;
    bool m_isMC;

    const SG::AuxElement::Decorator<float> secvtx_lxy;
    const SG::AuxElement::Decorator<float> secvtx_lxysig;
    const SG::AuxElement::Decorator<float> secvtx_lz;
    const SG::AuxElement::Decorator<float> secvtx_lzsig;
    const SG::AuxElement::Decorator<float> secvtx_l3d;
    const SG::AuxElement::Decorator<float> secvtx_l3dsig;

  }; /// class

} /// namespace


#endif
