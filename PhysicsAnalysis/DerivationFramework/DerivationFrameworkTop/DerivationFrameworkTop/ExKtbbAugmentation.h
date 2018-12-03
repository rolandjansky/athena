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
    float m_radius;
    std::string m_largeJetCollectionName;
    std::string m_smallJetCollectionName;
    bool m_isMC;
  }; /// class

} /// namespace


#endif
