#ifndef InDetRegionSelector_InDetRegionSelectorLUT_h
#define InDetRegionSelector_InDetRegionSelectorLUT_h

// #include "GaudiKernel/Algorithm.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "RegSelLUT/IRegionIDLUT_Creator.h"
 

class RegSelSiLUT;

/////////////////////////////////////////////////////////////////////////////

namespace InDetDD {

  class InDetRegionSelectorLUT : public AthAlgorithm {
  public:
    InDetRegionSelectorLUT (const std::string& name, ISvcLocator* pSvcLocator);
    StatusCode initialize();
    StatusCode execute()  {return StatusCode::SUCCESS; };
    StatusCode finalize() {return StatusCode::SUCCESS; };
    
    void printTable(const RegSelSiLUT*);
  
  private:
    
    ToolHandle<IRegionIDLUT_Creator> m_lutCreatorToolPixel;
    ToolHandle<IRegionIDLUT_Creator> m_lutCreatorToolSCT;
    ToolHandle<IRegionIDLUT_Creator> m_lutCreatorToolTRT;
  
    bool m_doTests;

  };

}
#endif
