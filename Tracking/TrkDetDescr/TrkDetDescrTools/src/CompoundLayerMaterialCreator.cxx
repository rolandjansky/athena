/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CompoundLayerMaterialCreator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// STL
#include <sstream>
// Trk include
#include "TrkDetDescrTools/CompoundLayerMaterialCreator.h"
#include "TrkGeometry/LayerMaterialRecord.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometry/CompoundLayerMaterial.h"
#include "TrkDetDescrUtils/GeometryStatics.h"
#include "TrkDetDescrUtils/BinUtility.h"
    

// constructor
Trk::CompoundLayerMaterialCreator::CompoundLayerMaterialCreator(const std::string& t, const std::string& n, const IInterface* p)
: AthAlgTool(t,n,p),
  m_fullCompoundCalculation(false)
{
    declareInterface<Trk::ILayerMaterialCreator>(this);
    // give the map a name
    declareProperty("LayerMaterialName"                 , m_layerMaterialName);
    declareProperty("LayerMaterialDirectory"            , m_layerMaterialDirectory);
    declareProperty("FullCompoundCalculation"           , m_fullCompoundCalculation);
}

// destructor
Trk::CompoundLayerMaterialCreator::~CompoundLayerMaterialCreator()
{}


// initialize
StatusCode Trk::CompoundLayerMaterialCreator::initialize() 
{
    return StatusCode::SUCCESS;
}
    
// finalize
StatusCode Trk::CompoundLayerMaterialCreator::finalize() 
{
    return StatusCode::SUCCESS;
}

const Trk::LayerMaterialProperties* Trk::CompoundLayerMaterialCreator::createLayerMaterial(const Trk::Layer&, const Trk::LayerMaterialRecord& lmr) const
{
    // get the material matrix
    const Trk::MaterialPropertiesMatrix& materialMatrix = lmr.associatedLayerMaterial();
    // get the bin untility
    const Trk::BinUtility* cbinutil = lmr.binUtility();
    // if there's no bin utility - bail out
    if (!cbinutil){
        ATH_MSG_WARNING( "No BinUtility given - Bailing out." );
        return 0;
    } else 
        ATH_MSG_DEBUG( "BinUtility provided, creating binned array in dimensions " << cbinutil->max(0)+1 << " x " << cbinutil->max(1)+1 );
    
    // analyse the material bins
    double tMin   = 10e10, xMin  = 10e10, lMin  = 10e10, aMin   = 10e10, zMin   = 10e10, rMin = 10e10;
    double tMax   = 0., xMax  = 0., lMax  = 0., aMax   = 0., zMax   = 0., rMax = 0.;
    
    // first loop to get the min/max values
    for (auto& mo : materialMatrix)    
        for (auto& mi : mo) {
            if (mi){
              const Trk::MaterialProperties& mp = (*mi);
              // macro defined in GeometryStatics
              minMaxValue(tMin, tMax, mp.thickness());
              minMaxValue(xMin, xMax, mp.x0());
              minMaxValue(lMin, lMax, mp.l0());
              minMaxValue(aMin, aMax, mp.averageA());
              minMaxValue(zMin, zMax, mp.averageZ());
              minMaxValue(rMin, rMax, mp.averageRho());
            }
    }    
    
    ATH_MSG_VERBOSE( "Max values for t, x0, l0, a, z, rho estimated." );
    
    // the bin matrices in the store    
    Trk::ValueMatrix binMatrix( cbinutil->max(1)+1, Trk::ValueVector(cbinutil->max(0)+1, static_cast<unsigned char>(0) ) );
    // 255 bins, the 0 bin indicates empy
    Trk::ValueStore tStore, xStore, lStore, aStore, zStore, rStore;
    // set the store min, max
    tStore.valueMin = tMin;  
    xStore.valueMin = xMin;  
    lStore.valueMin = lMin;  
    aStore.valueMin = aMin;  
    zStore.valueMin = zMin;  
    rStore.valueMin = rMin;  
    tStore.valueStep = (tMax-tMin)/double(static_cast<int>(UCHAR_MAX)-1);
    xStore.valueStep = (xMax-xMin)/double(static_cast<int>(UCHAR_MAX)-1);
    lStore.valueStep = (lMax-lMin)/double(static_cast<int>(UCHAR_MAX)-1);
    aStore.valueStep = (aMax-aMin)/double(static_cast<int>(UCHAR_MAX)-1);
    zStore.valueStep = (zMax-zMin)/double(static_cast<int>(UCHAR_MAX)-1);
    rStore.valueStep = (rMax-rMin)/double(static_cast<int>(UCHAR_MAX)-1);
    // set the initally empty 
    tStore.valueBinMatrix = binMatrix;
    xStore.valueBinMatrix = binMatrix;
    lStore.valueBinMatrix = binMatrix;
    aStore.valueBinMatrix = binMatrix;
    zStore.valueBinMatrix = binMatrix;    
    rStore.valueBinMatrix = binMatrix;    
    
    ATH_MSG_VERBOSE( "Material stores prepared, now preparing composition matrix." );
    
    // the compound material 
    std::vector< std::vector< Trk::MaterialComposition > > compositionMatrix( cbinutil->max(1)+1, std::vector< Trk::MaterialComposition >( cbinutil->max(0)+1, Trk::MaterialComposition()) ); 
    
    ATH_MSG_VERBOSE( "Composition matrix created." );
    
    // second loop : assign the bins & and copy the material composition
    size_t obin = 0; 
    for (auto& mo : materialMatrix){   
        size_t ibin =0;
        for (auto& mi : mo) {
            if (mi){
              const Trk::MaterialProperties& mp = (*mi);
              tStore.valueBinMatrix[obin][ibin] = static_cast<unsigned char>(lrint(mp.thickness()/tStore.valueStep)+1); 
              xStore.valueBinMatrix[obin][ibin] = static_cast<unsigned char>(lrint(mp.x0()/xStore.valueStep)+1); 
              lStore.valueBinMatrix[obin][ibin] = static_cast<unsigned char>(lrint(mp.l0()/lStore.valueStep)+1); 
              aStore.valueBinMatrix[obin][ibin] = static_cast<unsigned char>(lrint(mp.averageA()/aStore.valueStep)+1); 
              zStore.valueBinMatrix[obin][ibin] = static_cast<unsigned char>(lrint(mp.averageZ()/zStore.valueStep)+1); 
              rStore.valueBinMatrix[obin][ibin] = static_cast<unsigned char>(lrint(mp.averageRho()/rStore.valueStep)+1); 
              // set the material composition 
              if (mp.material().composition)
                  compositionMatrix[obin][ibin] = Trk::MaterialComposition(*(mp.material().composition));
            }
            ++ibin;
       }
       ++obin;      
   }
   ATH_MSG_VERBOSE( "Returning the new compound material." );  
   
   // now create the compound material propertis 
   return new Trk::CompoundLayerMaterial(*cbinutil, tStore, xStore, lStore, aStore, zStore, rStore, compositionMatrix, m_fullCompoundCalculation);
}    
    