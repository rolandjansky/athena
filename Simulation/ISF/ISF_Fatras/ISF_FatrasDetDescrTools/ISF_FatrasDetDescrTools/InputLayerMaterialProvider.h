/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InputLayerMaterialProvider.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FATRASDETDESCRTOOLS_INPUTLAYERMATERIALPROVIDER_H
#define ISF_FATRASDETDESCRTOOLS_INPUTLAYERMATERIALPROVIDER_H

// Gaudi & Athena
#include "AthenaBaseComps/AthAlgTool.h"

//ISF
#include "ISF_FatrasDetDescrInterfaces/IInputLayerMaterialProvider.h"

namespace Trk {
  class LayerMaterialProperties;    
}

namespace iFatras {
  
  /** @class InputLayerMaterialProvider
      
      AlgTool that creates the LayerMaterial to load into the layers
      
      @author Noemi.Calace@cern.ch
  */
  
  class InputLayerMaterialProvider :  public AthAlgTool, virtual public IInputLayerMaterialProvider {
    
  public:
    /** Constructor */
    InputLayerMaterialProvider(const std::string&,const std::string&,const IInterface*);
    
    /** Destructor */
    virtual ~InputLayerMaterialProvider();
       
    /** Action to work on Layers */
    virtual Trk::LayerMaterialProperties * handleMaterial();
            
  private:
    
    double       m_layerLengthX;
    double       m_layerLengthY;
    
    unsigned int m_NbinsX;
    unsigned int m_NbinsY;
    unsigned int m_NbinsSupport;
    double       m_sensorThickness;
    double       m_supportThickness;
    
    double       m_sensorPathX0;
    double       m_supportPathX0;
    
    double       m_materialL0;
    double       m_materialA;
    double       m_materialZ;
    double       m_materialRho;
    
      
  };

} // end of namespace

#endif // ISF_FATRASDETDESCRTOOLS_INPUTLAYERMATERIALPROVIDER_H

