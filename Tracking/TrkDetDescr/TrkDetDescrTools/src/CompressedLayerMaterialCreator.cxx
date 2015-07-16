/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CompressedLayerMaterialCreator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// STL
#include <sstream>
// Trk include
#include "TrkDetDescrTools/CompressedLayerMaterialCreator.h"
#include "TrkGeometry/LayerMaterialRecord.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometry/CompressedLayerMaterial.h"
    

// constructor
Trk::CompressedLayerMaterialCreator::CompressedLayerMaterialCreator(const std::string& t, const std::string& n, const IInterface* p)
: AthAlgTool(t,n,p),
  m_compressedMaterialThickness(1.),
  m_compressedMaterialX0Bins(256),
  m_compressedMaterialZARhoBins(256)
{
    declareInterface<Trk::ILayerMaterialCreator>(this);
    
    // give the map a name
    declareProperty("LayerMaterialName"                 , m_layerMaterialName);
    declareProperty("LayerMaterialDirectory"            , m_layerMaterialDirectory);
    // setup for compressed layer creation
    declareProperty("MaterialThickness"                 , m_compressedMaterialThickness);
    declareProperty("MaterialBinsX0"                    , m_compressedMaterialX0Bins);
    declareProperty("MaterialBinsZARho"                 , m_compressedMaterialZARhoBins);
    
}

// destructor
Trk::CompressedLayerMaterialCreator::~CompressedLayerMaterialCreator()
{}


// initialize
StatusCode Trk::CompressedLayerMaterialCreator::initialize() 
{
    return StatusCode::SUCCESS;
}
    
// finalize
StatusCode Trk::CompressedLayerMaterialCreator::finalize() 
{
    return StatusCode::SUCCESS;
}

const Trk::LayerMaterialProperties* Trk::CompressedLayerMaterialCreator::createLayerMaterial(const Trk::LayerMaterialRecord& lmr) const
{
  // get the material matrix
  const Trk::MaterialPropertiesMatrix& materialProperties = lmr.associatedLayerMaterial();
  // get the bin utility
  const Trk::BinUtility* lBinUtility = lmr.binUtility();
  if (lBinUtility){
      ATH_MSG_VERBOSE("Converting the MaterialPropertiesMatrix into a CompressedLayerMaterial.");
      return createCompressedLayerMaterial(materialProperties,*lBinUtility);
  }
  // we can not do anything without a bin utility
  ATH_MSG_DEBUG("No BinUtility provided - return 0");
  return nullptr; 
}    
    
const Trk::LayerMaterialProperties* Trk::CompressedLayerMaterialCreator::convertLayerMaterial(const Trk::LayerMaterialProperties& lmProperties) const
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
        bLayerMaterial = createCompressedLayerMaterial(materialMatrix,*bUtility);
        
    } else {
        // must be homogenous material, can be transformed into a 0-bin material, would be silly though
        ATH_MSG_DEBUG("No BinUtility provided - return a simple clone.");
        bLayerMaterial = lmProperties.clone();
    }
    // 
    return bLayerMaterial;
}

const Trk::LayerMaterialProperties* Trk::CompressedLayerMaterialCreator::createCompressedLayerMaterial(const Trk::MaterialPropertiesMatrix& materialProperties, const BinUtility& lBinUtility) const
{
    // the vector to be created and reserve the maximum
    Trk::MaterialPropertiesVector materialVector;
    materialVector.reserve(m_compressedMaterialZARhoBins*m_compressedMaterialX0Bins);
    // nF x nS
    size_t nFirstBins  = lBinUtility.max(0)+1;
    size_t nSecondBins = lBinUtility.max(1)+1;
    // low, high boundaries
    double x0min        = 10e10;
    double x0max        = 0.;
    double avZArhoMin   = 10e10;
    double avZArhoMax   = 0.;
    // create two maps, the compression map and the index map
    std::vector< std::vector<unsigned short int> > materialBins;
    // (1) FIRST LOOP, get boundaries
    materialBins.reserve(nSecondBins);
    for (size_t isec = 0; isec < nSecondBins; ++isec) {
        std::vector<unsigned short int> firstbins(nFirstBins,0);
        materialBins.push_back(firstbins);
        // loop over the bins
        for (size_t ifir = 0; ifir < nFirstBins; ++ifir) {
            // get the current material properties
            const Trk::MaterialProperties* matProp = materialProperties[isec][ifir];
            if (matProp) {
                double tinX0   = matProp->thicknessInX0();
                double avZArho = matProp->zOverAtimesRho();
                x0min = tinX0 < x0min ? tinX0 : x0min;
                x0max = tinX0 > x0max ? tinX0 : x0max;
                avZArhoMin = avZArho < avZArhoMin ? avZArho : avZArhoMin;
                avZArhoMax = avZArho > avZArhoMax ? avZArho : avZArhoMax;
            }
        }
    }
    // min / max is defined, find step size
    double stepX0    = (x0max-x0min)/m_compressedMaterialX0Bins;
    double stepZArho = (avZArhoMax-avZArhoMin)/m_compressedMaterialZARhoBins;
    // get the material histogram
    std::vector< std::vector< std::vector< Trk::IndexedMaterial> > > materialHistogram;
    materialHistogram.reserve(m_compressedMaterialZARhoBins);
    // prepare the histogram
    for (size_t izarho = 0; izarho < m_compressedMaterialZARhoBins; ++izarho) {
        std::vector< std::vector < Trk::IndexedMaterial > > x0materialbins;
        x0materialbins.reserve(m_compressedMaterialX0Bins);
        for (size_t ix0 = 0; ix0 < m_compressedMaterialX0Bins; ++ix0) {
            std::vector < Trk::IndexedMaterial > materialBin;
            x0materialbins.push_back( materialBin );
        }
        materialHistogram.push_back(x0materialbins);
    }
    // fill the histogram
    for (size_t isec = 0; isec < nSecondBins; ++isec) {
        for (size_t ifir = 0; ifir < nFirstBins; ++ifir) {
            // get the material properties
            const Trk::MaterialProperties* matProp = dynamic_cast<const Trk::MaterialProperties*>(materialProperties[isec][ifir]);
            if (matProp) {
                // calculate the bins of the material histogram
                double tinX0   = matProp->thicknessInX0();
                double avZArho = matProp->zOverAtimesRho();
                int x0bin    =  int(  (tinX0-x0min)/stepX0 );
                int zarhobin =  int(  (avZArho-avZArhoMin)/stepZArho );
                // range protection
                x0bin    = ( (size_t)x0bin >= m_compressedMaterialX0Bins) ? m_compressedMaterialX0Bins-1 : x0bin;
                x0bin    = x0bin < 0 ? 0 : x0bin;
                zarhobin = ( (size_t)zarhobin >= m_compressedMaterialZARhoBins) ? m_compressedMaterialZARhoBins-1 : zarhobin;
                zarhobin = zarhobin < 0 ? 0 : zarhobin;
                // create indexed material
                Trk::IndexedMaterial idxMaterial;
                idxMaterial.materialProperties = matProp;
                idxMaterial.firstBin  = ifir;
                idxMaterial.secondBin = isec;
                // fill into the material histogram
                materialHistogram[zarhobin][x0bin].push_back(idxMaterial);
            }
        }
    }
    // merge the bins and ready
    materialVector.push_back(0);
    // prepare the histogram
    for (size_t izarho = 0; izarho < m_compressedMaterialZARhoBins; ++izarho) {
        for (size_t ix0 = 0; ix0 < m_compressedMaterialX0Bins; ++ix0) {
            // get the indexed material properties
            std::vector< Trk::IndexedMaterial > indexedMaterial = materialHistogram[izarho][ix0];
            if (indexedMaterial.size()) {
                double avT          = 0.; // thickness: by default on one layer it should be the same !
                double tinX0        = 0.;
                double tinL0        = 0.;
                double avA          = 0.;
                double avZ          = 0.;
                double avRho        = 0.;
                std::vector< Trk::IndexedMaterial >::iterator idmIter = indexedMaterial.begin();
                std::vector< Trk::IndexedMaterial >::iterator idmIterEnd = indexedMaterial.end();
                for ( ; idmIter != idmIterEnd; ++idmIter ) {
                    tinX0 += (*idmIter).materialProperties->thicknessInX0();
                    tinL0 += (*idmIter).materialProperties->thicknessInL0();
                    avA   += (*idmIter).materialProperties->averageA();
                    avZ   += (*idmIter).materialProperties->averageZ();
                    avRho += (*idmIter).materialProperties->averageRho();
                }
                double measure = 1./(indexedMaterial.size());
                // average it
                tinX0 *= measure;
                tinL0 *= measure;
                avA   *= measure;
                avZ   *= measure;
                avRho *= measure;
                avT   *= measure;
                // compress to a model thickness [ rho affected ]
                avRho *= avT/m_compressedMaterialThickness;
                materialVector.push_back(new Trk::MaterialProperties(m_compressedMaterialThickness,
                                                                     m_compressedMaterialThickness/tinX0,
                                                                     m_compressedMaterialThickness/tinL0,
                                                                     avA,
                                                                     avZ,
                                                                     avRho));
                // now set the index
                int matindex = int(materialVector.size()-1);
                idmIter = indexedMaterial.begin();
                for ( ; idmIter != idmIterEnd; ++idmIter )
                    materialBins[(*idmIter).secondBin][(*idmIter).firstBin] = matindex;
            }
        }
    }
 
    // change the 2bin matrix to a 1bin vector (better for persistency)
    std::vector<unsigned short int> materialBinsVector;
    materialBinsVector.reserve( (lBinUtility.max(0)+1)*(lBinUtility.max(1)+1) );
    std::vector< std::vector<unsigned short int> >::iterator binVecIter    = materialBins.begin();
    std::vector< std::vector<unsigned short int> >::iterator binVecIterEnd = materialBins.end();
    for ( ; binVecIter != binVecIterEnd; ++binVecIter) {
        std::vector<unsigned short int>::iterator binIter    = (*binVecIter).begin();
        std::vector<unsigned short int>::iterator binIterEnd = (*binVecIter).end();
        for ( ; binIter != binIterEnd; ++binIter )
            materialBinsVector.push_back(*binIter);
    }
    
    //!< @TODO check if cleanup is needed
    
    // create the compressed material
    return new Trk::CompressedLayerMaterial(lBinUtility,materialVector,materialBinsVector);
}



