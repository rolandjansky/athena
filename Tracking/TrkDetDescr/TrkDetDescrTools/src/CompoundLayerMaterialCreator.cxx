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

const Trk::LayerMaterialProperties* Trk::CompoundLayerMaterialCreator::createLayerMaterial(const Trk::LayerMaterialRecord& lmr) const
{
    // get the material matrix
    const Trk::MaterialPropertiesMatrix& materialMatrix = lmr.associatedLayerMaterial();
    // get the bin untility
    const Trk::BinUtility* binUtility = lmr.binUtility();
    // if there's no bin utility - bail out
    if (!binUtility){
        ATH_MSG_WARNING( "No BinUtility given - Bailing out." );
        return nullptr;
    } else 
        ATH_MSG_DEBUG( "BinUtility provided, creating binned array in dimensions " << binUtility->max(0)+1 << " x " << binUtility->max(1)+1 );
    // return the created compound material
    return createCompoundLayerMaterial(materialMatrix,*binUtility);
}    

const Trk::LayerMaterialProperties* Trk::CompoundLayerMaterialCreator::convertLayerMaterial(const Trk::LayerMaterialProperties& lmProperties) const
{
    
    // the return object
    const Trk::LayerMaterialProperties* bLayerMaterial = nullptr;
    // get the binUtility of the LayerMaterialProperties
    const Trk::BinUtility* bUtility = lmProperties.binUtility();
    // we have a bin utility, get the matrix and fill it
    if (bUtility){
        // prepare the matrix
        // nF x nS
        size_t nBins0  = bUtility->max(0)+1;
        size_t nBins1  = bUtility->max(1)+1;
        // create the MaterialMatrix
        Trk::MaterialPropertiesMatrix materialMatrix;
        materialMatrix.reserve(nBins1);
        // fill the matrix
        for (size_t ibin1 = 0; ibin1 < nBins1; ++ibin1) {
            // create the vector first
            Trk::MaterialPropertiesVector materialVector;
            materialVector.reserve(nBins0);
            // loop over local 1 bins
            for (size_t ibin0 = 0; ibin0 < nBins0; ++ibin0) {
                // get the material from the properties and push them into the matrix (no cloning !)
                const Trk::MaterialProperties* mProperties = lmProperties.material(ibin0,ibin1);
                materialVector.push_back(mProperties);
            }
            // now pus the vector into the matrix
            materialMatrix.push_back(materialVector);
        }
        
        // create the material
        ATH_MSG_VERBOSE("Converting the MaterialPropertiesMatrix into a CompressedLayerMaterial.");
        bLayerMaterial = createCompoundLayerMaterial(materialMatrix,*bUtility);
        
    } else {
        // must be homogenous material, can be transformed into a 0-bin material, would be silly though
        ATH_MSG_DEBUG("No BinUtility provided - return a simple clone.");
        bLayerMaterial = lmProperties.clone();
    }
    // 
    return bLayerMaterial;
}    

const Trk::LayerMaterialProperties* Trk::CompoundLayerMaterialCreator::createCompoundLayerMaterial(const Trk::MaterialPropertiesMatrix& materialMatrix, const Trk::BinUtility& lBinUtility) const
{
    
    
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
    
    ATH_MSG_DEBUG( "Preparing the store: min/max values for t, x0, l0, a, z, rho estimated." );

    
    // the bin matrices in the store    
    Trk::ValueMatrix binMatrix( lBinUtility.max(1)+1, Trk::ValueVector(lBinUtility.max(0)+1, static_cast<unsigned char>(0) ) );
    // 255 bins, the 0 bin indicates empy
    Trk::ValueStore tStore, xStore, lStore, aStore, zStore, rStore;    
    // set the store min, max
    tStore.valueMin = tMin;  
    xStore.valueMin = xMin;  
    lStore.valueMin = lMin;  
    aStore.valueMin = aMin;  
    zStore.valueMin = zMin;  
    rStore.valueMin = rMin;  
    tStore.valueStep = fabs(tMin-tMax)<10e-8 ? 0. : (tMax-tMin)/double(static_cast<int>(UCHAR_MAX)-1);
    xStore.valueStep = (xMax-xMin)/double(static_cast<int>(UCHAR_MAX)-1);
    lStore.valueStep = (lMax-lMin)/double(static_cast<int>(UCHAR_MAX)-1);
    aStore.valueStep = (aMax-aMin)/double(static_cast<int>(UCHAR_MAX)-1);
    zStore.valueStep = (zMax-zMin)/double(static_cast<int>(UCHAR_MAX)-1);
    rStore.valueStep = (rMax-rMin)/double(static_cast<int>(UCHAR_MAX)-1);
    
    ATH_MSG_VERBOSE(" - t   [ min/max/step ] = "  << tMin  << " / " << tMax << " / " << tStore.valueStep );
    ATH_MSG_VERBOSE(" - x0  [ min/max/step ] = "  << xMin  << " / " << xMax << " / " << xStore.valueStep );
    ATH_MSG_VERBOSE(" - l0  [ min/max/step ] = "  << lMin  << " / " << lMax << " / " << lStore.valueStep );
    ATH_MSG_VERBOSE(" - a   [ min/max/step ] = "  << aMin  << " / " << aMax << " / " << aStore.valueStep );
    ATH_MSG_VERBOSE(" - z   [ min/max/step ] = "  << zMin  << " / " << zMax << " / " << zStore.valueStep );
    ATH_MSG_VERBOSE(" - rho [ min/max/step ] = "  << rMin  << " / " << rMax << " / " << rStore.valueStep );
    
    // set the initally empty - thickness store can be empty
    if (tStore.valueStep > 0.) 
        tStore.valueBinMatrix = binMatrix;
    else 
        ATH_MSG_VERBOSE("Thickness has been estimated to be constant - matrix is not prepared.");
    xStore.valueBinMatrix = binMatrix;
    lStore.valueBinMatrix = binMatrix;
    aStore.valueBinMatrix = binMatrix;
    zStore.valueBinMatrix = binMatrix;    
    rStore.valueBinMatrix = binMatrix;    
    
    ATH_MSG_VERBOSE( "Material stores prepared, now preparing composition matrix." );
    
    // the compound material 
    std::vector< std::vector< Trk::MaterialComposition > > compositionMatrix( lBinUtility.max(1)+1, std::vector< Trk::MaterialComposition >( lBinUtility.max(0)+1, Trk::MaterialComposition()) ); 
    
    ATH_MSG_VERBOSE( "Composition matrix created." );
    
    // second loop : assign the bins & and copy the material composition
    size_t obin = 0; 
    for (auto& mo : materialMatrix){   
        size_t ibin =0;
        for (auto& mi : mo) {
            if (mi){
              const Trk::MaterialProperties& mp = (*mi);
              if (tStore.valueStep > 0.) tStore.valueBinMatrix[obin][ibin] = static_cast<unsigned char>(lrint(mp.thickness()/tStore.valueStep)+1); 
              xStore.valueBinMatrix[obin][ibin] = static_cast<unsigned char>(lrint(mp.x0()/xStore.valueStep)+1); 
              lStore.valueBinMatrix[obin][ibin] = static_cast<unsigned char>(lrint(mp.l0()/lStore.valueStep)+1); 
              aStore.valueBinMatrix[obin][ibin] = static_cast<unsigned char>(lrint(mp.averageA()/aStore.valueStep)+1); 
              zStore.valueBinMatrix[obin][ibin] = static_cast<unsigned char>(lrint(mp.averageZ()/zStore.valueStep)+1); 
              rStore.valueBinMatrix[obin][ibin] = static_cast<unsigned char>(lrint(mp.averageRho()/rStore.valueStep)+1); 
              // set the material composition 
              if (mp.material().composition) {
                  compositionMatrix[obin][ibin] = Trk::MaterialComposition(*(mp.material().composition));
                  ATH_MSG_VERBOSE(" - composition is copied.");
              }
            }
            ++ibin;
       }
       ++obin;      
   }
   ATH_MSG_VERBOSE( "Returning the new compound material." );  
   
   // now create the compound material propertis 
   return new Trk::CompoundLayerMaterial(lBinUtility, tStore, xStore, lStore, aStore, zStore, rStore, compositionMatrix, m_fullCompoundCalculation);

}


   