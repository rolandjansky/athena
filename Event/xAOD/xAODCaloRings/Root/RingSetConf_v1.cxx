/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s).
#include "xAODCaloRings/versions/RingSetConf_v1.h"
#include "xAODCaloRings/versions/RingSetConfContainer_v1.h"

// System include(s).
#include <iostream>

namespace xAOD {


//==============================================================================
/// Static methods:
/// @{

/// Layer determination
/// @{
bool RingSetConf_v1::isEMSection(
    const std::vector<CaloSampling::CaloSample> &layers)
{
  if (layers.empty()) return false;
  for (const auto &layer : layers){
    if ( (layer != CaloSampling::CaloSample::PreSamplerB ) &&
         (layer != CaloSampling::CaloSample::PreSamplerE ) &&
         (layer != CaloSampling::CaloSample::EMB1        ) &&
         (layer != CaloSampling::CaloSample::EMB2        ) &&
         (layer != CaloSampling::CaloSample::EMB3        ) &&
         (layer != CaloSampling::CaloSample::EME1        ) &&
         (layer != CaloSampling::CaloSample::EME2        ) &&
         (layer != CaloSampling::CaloSample::EME3        ) ){
      return false;
    }
  }
  return true;
}

//==============================================================================
bool RingSetConf_v1::isHADSection(
    const std::vector<CaloSampling::CaloSample> &layers) 
{
  if (layers.empty()) return false;
  for (const auto &layer : layers){
    if ( (layer != CaloSampling::CaloSample::HEC0 ) &&
         (layer != CaloSampling::CaloSample::HEC1 ) &&
         (layer != CaloSampling::CaloSample::HEC2 ) &&
         (layer != CaloSampling::CaloSample::HEC3 ) &&
         (layer != CaloSampling::CaloSample::TileBar0 ) &&
         (layer != CaloSampling::CaloSample::TileBar1 ) &&
         (layer != CaloSampling::CaloSample::TileBar2 ) &&
         (layer != CaloSampling::CaloSample::TileGap1 ) &&
         (layer != CaloSampling::CaloSample::TileGap2 ) &&
         (layer != CaloSampling::CaloSample::TileGap3 ) &&
         (layer != CaloSampling::CaloSample::TileExt0 ) &&
         (layer != CaloSampling::CaloSample::TileExt1 ) &&
         (layer != CaloSampling::CaloSample::TileExt2 ) ){
      return false;
    }
  }
  return true;
}

//==============================================================================
bool RingSetConf_v1::isPSLayer(
    const std::vector<CaloSampling::CaloSample> &layers) 
{
  if (layers.empty()) return false;
  for (const CaloSampling::CaloSample &layer : layers){
    if ( (layer != CaloSampling::CaloSample::PreSamplerB ) &&
         (layer != CaloSampling::CaloSample::PreSamplerE ) ){
      return false;
    }
  }
  return true;
}

//==============================================================================
bool RingSetConf_v1::isEM1Layer(
    const std::vector<CaloSampling::CaloSample> &layers) 
{
  if (layers.empty()) return false;
  for (const CaloSampling::CaloSample layer : layers){
    if ( (layer != CaloSampling::CaloSample::EMB1        ) &&
         (layer != CaloSampling::CaloSample::EME1        ) ){
      return false;
    }
  }
  return true;
}

//==============================================================================
bool RingSetConf_v1::isEM2Layer(
    const std::vector<CaloSampling::CaloSample> &layers) 
{
  if (layers.empty()) return false;
  for (const auto &layer : layers){
    if ( (layer != CaloSampling::CaloSample::EMB2        ) &&
         (layer != CaloSampling::CaloSample::EME2        ) ){
      return false;
    }
  }
  return true;
}

//==============================================================================
bool RingSetConf_v1::isEM3Layer(
    const std::vector<CaloSampling::CaloSample> &layers) 
{
  if (layers.empty()) return false;
  for (const auto &layer : layers){
    if ( (layer != CaloSampling::CaloSample::EMB3        ) &&
         (layer != CaloSampling::CaloSample::EME3        ) ){
      return false;
    }
  }
  return true;
}

//==============================================================================
bool RingSetConf_v1::isHAD1Layer(
    const std::vector<CaloSampling::CaloSample> &layers) 
{
  if (layers.empty()) return false;
  for (const auto &layer : layers){
    if ( (layer != CaloSampling::CaloSample::HEC0        ) &&
         (layer != CaloSampling::CaloSample::HEC1        ) && // Added for flexibility
         (layer != CaloSampling::CaloSample::TileBar0    ) &&
         (layer != CaloSampling::CaloSample::TileGap1    ) && // Added for flexibility
         (layer != CaloSampling::CaloSample::TileGap2    ) && // Added for flexibility
         (layer != CaloSampling::CaloSample::TileGap3    ) &&
         (layer != CaloSampling::CaloSample::TileExt0    ) ){
      return false;
    }
  }
  return true;
}

//==============================================================================
bool RingSetConf_v1::isHAD2Layer(
    const std::vector<CaloSampling::CaloSample> &layers) 
{
  if (layers.empty()) return false;
  for (const auto &layer : layers){
    if ( (layer != CaloSampling::CaloSample::HEC1        ) &&
         (layer != CaloSampling::CaloSample::HEC2        ) &&
         (layer != CaloSampling::CaloSample::TileBar1    ) &&
         (layer != CaloSampling::CaloSample::TileGap1    ) &&
         (layer != CaloSampling::CaloSample::TileGap2    ) && // Added for flexibility
         (layer != CaloSampling::CaloSample::TileGap3    ) && // Added for flexibility
         (layer != CaloSampling::CaloSample::TileExt1    ) ){
      return false;
    }
  }
  return true;
}

//==============================================================================
bool RingSetConf_v1::isHAD3Layer(
    const std::vector<CaloSampling::CaloSample> &layers) 
{
  if (layers.empty()) return false;
  for (const auto &layer : layers){
    if ( (layer != CaloSampling::CaloSample::HEC2        ) && // Added for flexibility
         (layer != CaloSampling::CaloSample::HEC3        ) &&
         (layer != CaloSampling::CaloSample::TileBar2    ) &&
         (layer != CaloSampling::CaloSample::TileGap1    ) && // Added for flexibility
         (layer != CaloSampling::CaloSample::TileGap2    ) && 
         (layer != CaloSampling::CaloSample::TileGap3    ) && // Added for flexibility
         (layer != CaloSampling::CaloSample::TileExt2    ) ){
      return false;
    }
  }
  return true;
}

//==============================================================================
Ringer::CalJointSection RingSetConf_v1::whichSection(
    const std::vector<CaloSampling::CaloSample> &layers) 
{
  if       (isEMSection (layers))  return Ringer::CalJointSection::EM;
  else if  (isHADSection(layers))  return Ringer::CalJointSection::HAD;
  else return Ringer::CalJointSection::UnknownJointSection;
}

//==============================================================================
Ringer::CalJointLayer RingSetConf_v1::whichLayer(
    const std::vector<CaloSampling::CaloSample> &layers) 
{
  if       (isPSLayer  (layers)) return Ringer::CalJointLayer::PS;
  else if  (isEM1Layer (layers)) return Ringer::CalJointLayer::EM1;
  else if  (isEM2Layer (layers)) return Ringer::CalJointLayer::EM2;
  else if  (isEM3Layer (layers)) return Ringer::CalJointLayer::EM3;
  else if  (isHAD1Layer(layers)) return Ringer::CalJointLayer::HAD1;
  else if  (isHAD2Layer(layers)) return Ringer::CalJointLayer::HAD2;
  else if  (isHAD3Layer(layers)) return Ringer::CalJointLayer::HAD3;
  else return Ringer::CalJointLayer::UnknownJointLayer;
}
/// @}

/// RawConf methods
/// @{

//==============================================================================
void RingSetConf_v1::print( const RawConf &raw, std::ostream &stream ) 
{
  stream << "| "; 

  stream << raw.nRings << " Rings, Layer: " << raw.calJointLayer <<
    " (" << raw.layerStartIdx << " -> " << raw.layerEndIdx <<
    ") Section: " << raw.calJointSection << 
    " (" << raw.sectionStartIdx << " -> " << raw.sectionEndIdx <<
    "), etaWidth = " << raw.etaWidth << ", phiWidth = " << raw.phiWidth <<
      ", cellMaxDEtaDist = " << raw.cellMaxDEtaDist <<
      ", cellMaxDPhiDist = " << raw.cellMaxDPhiDist <<
    ", doEtaAxesDivision = " << raw.doEtaAxesDivision <<
    ", doPhiAxesDivision = " << raw.doPhiAxesDivision;

  stream << "|" << std::endl;
}
/// @}

/// RawConfCollection Methods
/// @{
//==============================================================================
unsigned RingSetConf_v1::totalNumberOfRings( 
    const RawConfCollection &rawConfCol )
{
  unsigned totalNumberOfRings(0);
  for ( const auto &rsRawConfCol : rawConfCol )
  {
    totalNumberOfRings += rsRawConfCol.nRings;
  }
  return totalNumberOfRings;
}

//==============================================================================
void RingSetConf_v1::addRawConfColBounderies( RawConfCollection &clRingsConf )
{
  if ( !clRingsConf.size() ) {
    throw std::runtime_error(std::string(
          "Cannot add the JointLayer/JointSection bounderies "
          "into an empty RawConfCollection."));
  }

  unsigned startOfEMJointSection(0),
           endOfEMJointSection(0),
           startOfHADJointSection(0),
           endOfHADJointSection(0),
           currentRingIdx(0);

  bool isWithinEMJointSection(false), 
       isWithinHADJointSection(false);

  // Get the section start, end indexes:
  for ( const auto &rawConf : clRingsConf ){
    Ringer::CalJointSection secType = rawConf.calJointSection;
    bool isEM = secType == Ringer::CalJointSection::EM;
    bool isHAD = secType == Ringer::CalJointSection::HAD;
    // Check if we are entering a new section:
    if ( !isWithinEMJointSection && isEM ) {
      startOfEMJointSection = currentRingIdx;
      isWithinEMJointSection = true;
    }
    if ( !isWithinHADJointSection && isHAD ) {
      startOfHADJointSection = currentRingIdx;
      isWithinHADJointSection = true;
    }
    // Check if we left a section:
    if ( isWithinEMJointSection && !isEM ) {
      endOfEMJointSection = currentRingIdx;
      isWithinEMJointSection = false;
    }
    if ( isWithinHADJointSection && !isHAD ) {
      endOfHADJointSection = currentRingIdx;
      isWithinHADJointSection = false;
    }
    // Increment current index by the layer size:
    currentRingIdx += rawConf.nRings;
  }
  // We finished looping, close the contained section boundery with the last
  // ring index:
  if ( isWithinEMJointSection ) {
    endOfEMJointSection = currentRingIdx;
  } else if ( isWithinHADJointSection ) {
    endOfHADJointSection = currentRingIdx;
  }

  currentRingIdx = 0;
  // Now fill the indexes:
  for ( auto &rawConf : clRingsConf ){
    // Fill the layer indexes:
    rawConf.layerStartIdx = currentRingIdx;
    currentRingIdx += rawConf.nRings;
    rawConf.layerEndIdx   = currentRingIdx;
    // Fill the section indexes:
    switch (rawConf.calJointSection) {
      case Ringer::CalJointSection::EM:
      {
        rawConf.sectionStartIdx = startOfEMJointSection;
        rawConf.sectionEndIdx = endOfEMJointSection;
        break;
      }
      case Ringer::CalJointSection::HAD:
      {
        rawConf.sectionStartIdx = startOfHADJointSection;
        rawConf.sectionEndIdx = endOfHADJointSection;
        break;
      }
      default:
        throw std::runtime_error(std::string(
              "Found unknown JointSection type."));
    }
  }
}

//==============================================================================
void RingSetConf_v1::print( const RawConfCollection &rawCol, 
    std::ostream &stream ) 
{
  stream << "RawConfCollection is : " << std::endl;
  for (unsigned rsIdx = 0; rsIdx < rawCol.size(); ++rsIdx) {
    stream << "RawConf #" << rsIdx << " : ";
    RingSetConf_v1::print(rawCol.at(rsIdx),stream);
  }
}

//==============================================================================
void RingSetConf_v1::getEdges( const RawConfCollection &clRingsConf,
    const Ringer::CalJointSection sec,
    unsigned &startEdge,
    unsigned &endEdge)
{
  for( const auto &rsConf : clRingsConf ) {
    if ( rsConf.calJointSection == sec ) {
      startEdge = rsConf.sectionStartIdx;
      endEdge = rsConf.sectionEndIdx;
      return;
    }
  }
}

//==============================================================================
void RingSetConf_v1::getEdges( const RawConfCollection &clRingsConf,
    const Ringer::CalJointLayer layer,
    unsigned &startEdge,
    unsigned &endEdge)
{
  for( const auto &rsConf : clRingsConf ) {
    if ( rsConf.calJointLayer == layer ) {
      startEdge = rsConf.layerStartIdx;
      endEdge = rsConf.layerEndIdx;
      return;
    }
  }
}

//==============================================================================
void RingSetConf_v1::fillRingSetConfContainer(
    const RawConfCollection &rawConfCol,
    RingSetConfContainer_v1 *container)
{

  // Protect against bad pointers:
  if ( container == nullptr ) {
    container = new RingSetConfContainer_v1();
  }

  // Loop through RawConfCollection and build true RingSetConf_v1 objects from
  // them:
  for ( const auto& rsRawConf : rawConfCol ) {
    RingSetConf_v1 *rsConf =  new RingSetConf_v1( rsRawConf );
    container->push_back(rsConf);
  }

}

//==============================================================================
void RingSetConf_v1::getRawConfCol(
    RawConfCollection &rawConfCol,
    const RingSetConfContainer_v1 *container) 
{
  // Protect against bad pointers:
  if ( container == nullptr ) {
    std::runtime_error(std::string("Retrieved nullptr when trying to create"
        " RawConfCollection."));
  }

  // Clear previous collection
  rawConfCol.clear();
  // Push back new configuration
  for ( const auto &rsConf : *container ) {
    rawConfCol.push_back( rsConf->getRawConf() );
  }
}
/// @}

/// RingSetConf_v1 Ctors
/// @{
//==============================================================================
RingSetConf_v1::RingSetConf_v1(const unsigned nRings, 
    const std::vector<CaloSampling::CaloSample> &layers,
    const float etaWidth, const float phiWidth,
    const float cellMaxDEtaDist, const float cellMaxDPhiDist,
    const Ringer::CalJointLayer calJointLayer, 
    const Ringer::CalJointSection calJointSection,
    const bool doEtaAxesDivision, const bool doPhiAxesDivision,
    const unsigned layerStartIdx, const unsigned layerEndIdx,
    const unsigned sectionStartIdx, const unsigned sectionEndIdx){
  makePrivateStore();
  setNRings(nRings);
  setLayers(layers);
  setEtaWidth(etaWidth);
  setPhiWidth(phiWidth);
  setCellMaxDEtaDist(cellMaxDEtaDist);
  setCellMaxDPhiDist(cellMaxDPhiDist);
  setCalJointLayer(calJointLayer);
  setCalJointSection(calJointSection);
  setdoEtaAxesDivision(doEtaAxesDivision);
  setdoPhiAxesDivision(doPhiAxesDivision);
  setLayerStartIdx(layerStartIdx);
  setLayerEndIdx(layerEndIdx);
  setSectionStartIdx(sectionStartIdx);
  setSectionEndIdx(sectionEndIdx);
}

//==============================================================================
RingSetConf_v1::RingSetConf_v1(
    const RingSetConf_v1::RawConf &confStruct)
{
  makePrivateStore();
  setNRings(confStruct.nRings);
  setLayers(confStruct.layers);
  setEtaWidth(confStruct.etaWidth);
  setPhiWidth(confStruct.phiWidth);
  setCellMaxDEtaDist(confStruct.cellMaxDEtaDist);
  setCellMaxDPhiDist(confStruct.cellMaxDPhiDist);
  setCalJointLayer(confStruct.calJointLayer);
  setCalJointSection(confStruct.calJointSection);
  setdoEtaAxesDivision(confStruct.doEtaAxesDivision);
  setdoPhiAxesDivision(confStruct.doPhiAxesDivision);
  setLayerStartIdx(confStruct.layerStartIdx);
  setLayerEndIdx(confStruct.layerEndIdx);
  setSectionStartIdx(confStruct.sectionStartIdx);
  setSectionEndIdx(confStruct.sectionEndIdx);
}

//==============================================================================
RingSetConf_v1::RawConf::RawConf(
    const unsigned nRings, 
    const std::vector<CaloSampling::CaloSample> &layers,
    const float etaWidth, const float phiWidth,
    const float cellMaxDEtaDist, const float cellMaxDPhiDist,
    const Ringer::CalJointLayer calJointLayer,
    const Ringer::CalJointSection calJointSection,
    const bool doEtaAxesDivision,
    const bool doPhiAxesDivision):
  nRings(nRings),
  layers(layers),
  etaWidth(etaWidth),
  phiWidth(phiWidth),
  cellMaxDEtaDist(cellMaxDEtaDist),
  cellMaxDPhiDist(cellMaxDPhiDist),
  calJointLayer(calJointLayer),
  calJointSection(calJointSection),
  doEtaAxesDivision(doEtaAxesDivision),
  doPhiAxesDivision(doPhiAxesDivision),
  layerStartIdx(0),
  layerEndIdx(0),
  sectionStartIdx(0),
  sectionEndIdx(0){;}

//==============================================================================
RingSetConf_v1::RawConf::RawConf(
    const unsigned nRings, 
    const std::vector<CaloSampling::CaloSample> &layers,
    const float etaWidth, const float phiWidth,
    const float cellMaxDEtaDist, const float cellMaxDPhiDist,
    const Ringer::CalJointLayer calJointLayer,
    const Ringer::CalJointSection calJointSection,
    const bool doEtaAxesDivision,
    const bool doPhiAxesDivision,
    const unsigned layerStartIdx,
    const unsigned layerEndIdx,
    const unsigned sectionStartIdx,
    const unsigned sectionEndIdx):
  nRings(nRings),
  layers(layers),
  etaWidth(etaWidth),
  phiWidth(phiWidth),
  cellMaxDEtaDist(cellMaxDEtaDist),
  cellMaxDPhiDist(cellMaxDPhiDist),
  calJointLayer(calJointLayer),
  calJointSection(calJointSection),
  doEtaAxesDivision(doEtaAxesDivision),
  doPhiAxesDivision(doPhiAxesDivision),
  layerStartIdx(layerStartIdx),
  layerEndIdx(layerEndIdx),
  sectionStartIdx(sectionStartIdx),
  sectionEndIdx(sectionEndIdx){;}

//==============================================================================
RingSetConf_v1::RawConf::RawConf(
    const RingSetConf_v1 &rsConf):
  nRings(rsConf.nRings()),
  layers(rsConf.layers()),
  etaWidth(rsConf.etaWidth()),
  phiWidth(rsConf.phiWidth()),
  cellMaxDEtaDist(rsConf.cellMaxDEtaDist()),
  cellMaxDPhiDist(rsConf.cellMaxDPhiDist()),
  calJointLayer(rsConf.calJointLayer()),
  calJointSection(rsConf.calJointSection()),
  doEtaAxesDivision(rsConf.doEtaAxesDivision()),
  doPhiAxesDivision(rsConf.doPhiAxesDivision()),
  layerStartIdx(rsConf.layerStartIdx()),
  layerEndIdx(rsConf.layerEndIdx()),
  sectionStartIdx(rsConf.sectionStartIdx()),
  sectionEndIdx(rsConf.sectionEndIdx()){;}
/// @}


/// RingSetConf_v1 nRings set and get:
/// @{
//==============================================================================
AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(RingSetConf_v1,
    unsigned,
    nRings,
    setNRings)
/// @}
    
/// RingSetConf_v1 layer interation
/// @{
//==============================================================================
const std::vector<CaloSampling::CaloSample>& RingSetConf_v1::layers() const {
  static SG::AuxElement::ConstAccessor<std::vector<unsigned> > acc("layers");
  return reinterpret_cast<const std::vector<CaloSampling::CaloSample>& >( 
      acc( *this ) );
}

//==============================================================================
void RingSetConf_v1::setLayers(
    const std::vector<CaloSampling::CaloSample>& layers) {
  static SG::AuxElement::Accessor<std::vector<unsigned> > acc("layers");
  acc.set( *this , reinterpret_cast<const std::vector<unsigned>&>(layers) );
}

//==============================================================================
unsigned RingSetConf_v1::nLayers() const {
  static SG::AuxElement::ConstAccessor<std::vector<unsigned> > acc("layers");
  return acc( *this ).size();
}

//==============================================================================
CaloSampling::CaloSample RingSetConf_v1::layerAt(const unsigned idx) const {
  static SG::AuxElement::ConstAccessor< std::vector<unsigned> > acc("layers");
  return static_cast<CaloSampling::CaloSample>( acc( *this ).at(idx) );
}

//==============================================================================
void RingSetConf_v1::addLayer(const CaloSampling::CaloSample c){
  static SG::AuxElement::Accessor< std::vector<unsigned> > acc("layers");
  acc( *this ).push_back(c);
}

//==============================================================================
void RingSetConf_v1::clearLayers(){
  static SG::AuxElement::Accessor< std::vector<unsigned> > acc("layers");
  acc( *this ).clear();
}
/// @}

/// RingSetConf_v1 calJointLayer,calJointSection get and set:
/// @{
//==============================================================================
Ringer::CalJointLayer RingSetConf_v1::calJointLayer() const {
  static SG::AuxElement::ConstAccessor< unsigned > acc("calJointLayer");
  return static_cast<Ringer::CalJointLayer>( acc( *this ) );
}

//==============================================================================
void RingSetConf_v1::setCalJointLayer(const Ringer::CalJointLayer calJointLayer){
  static SG::AuxElement::Accessor< unsigned > acc("calJointLayer");
  acc.set( *this , calJointLayer );
}

//==============================================================================
Ringer::CalJointSection RingSetConf_v1::calJointSection() const {
  static SG::AuxElement::ConstAccessor< unsigned > acc("calJointSection");
  return static_cast<Ringer::CalJointSection>( acc( *this ) );
}

//==============================================================================
void RingSetConf_v1::setCalJointSection(
    const Ringer::CalJointSection calJointSection) {
  static SG::AuxElement::Accessor< unsigned > acc("calJointSection");
  acc.set( *this , calJointSection );
}
/// @}

/// RingSetConf_v1 etaWidth,phiWidth get and set:
/// @{
//==============================================================================
AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(RingSetConf_v1, 
    float, 
    etaWidth, 
    setEtaWidth)

//==============================================================================
AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(RingSetConf_v1, 
    float, 
    phiWidth, 
    setPhiWidth)

//==============================================================================
AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(RingSetConf_v1, 
    float, 
    cellMaxDEtaDist, 
    setCellMaxDEtaDist)

//==============================================================================
AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(RingSetConf_v1, 
    float, 
    cellMaxDPhiDist, 
    setCellMaxDPhiDist)

//==============================================================================
bool RingSetConf_v1::doEtaAxesDivision() const {
  static SG::AuxElement::ConstAccessor< char > acc("doEtaAxesDivision");
  return static_cast<bool>( acc( *this) );
}

//==============================================================================
void RingSetConf_v1::setdoEtaAxesDivision(bool doEtaAxesDivision){
  static SG::AuxElement::Accessor< char > acc("doEtaAxesDivision");
  acc( *this) = static_cast<char>(doEtaAxesDivision);
}

//==============================================================================
bool RingSetConf_v1::doPhiAxesDivision() const {
  static SG::AuxElement::ConstAccessor< char > acc("doPhiAxesDivision");
  return static_cast<bool>( acc( *this ) );
}

//==============================================================================
void RingSetConf_v1::setdoPhiAxesDivision(bool doPhiAxesDivision){
  static SG::AuxElement::Accessor< char > acc("doPhiAxesDivision");
  acc( *this ) = static_cast<char>(doPhiAxesDivision);
}
/// @}

/// RingSetConf_v1 layerStart, layerEnd, sectionStart, sectionEnd idx set and get:
/// @{
//==============================================================================
AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(RingSetConf_v1,
    unsigned,
    layerStartIdx,
    setLayerStartIdx)

//==============================================================================
AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(RingSetConf_v1,
    unsigned,
    layerEndIdx,
    setLayerEndIdx)

//==============================================================================
AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(RingSetConf_v1,
    unsigned,
    sectionStartIdx,
    setSectionStartIdx)

//==============================================================================
AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(RingSetConf_v1,
    unsigned,
    sectionEndIdx,
    setSectionEndIdx)
/// @}

/// @name RingSetConf_v1 utilities
/// @{
//==============================================================================
const RingSetConf_v1::RawConf RingSetConf_v1::getRawConf() const {
  return RawConf( *this );
}
/// @}

//==============================================================================
RingSetConf_v1& RingSetConf_v1::operator=(const RingSetConf_v1& rsConf ){
  if (this != &rsConf ){ // protect against invalid self-assignment
    if (!this->container() && !this->hasStore() ) {
      makePrivateStore();
    }
    this->SG::AuxElement::operator=( rsConf );
  }
  // by convention, always return *this
  return *this;
}

//==============================================================================
void RingSetConf_v1::print( std::ostream &stream ) const {
  stream << "| "; 

  stream << nRings() << " Rings, Layer: " << calJointLayer() <<
    " (" << layerStartIdx() << " -> " << layerEndIdx() <<
    ") Section: " << calJointSection() << 
    " (" << sectionStartIdx() << " -> " << sectionEndIdx() <<
    "), etaWidth = " << etaWidth() << ", phiWidth = " << phiWidth() <<
    ", cellMaxDEtaDist = " << cellMaxDEtaDist() <<
    ", cellMaxDPhiDist = " << cellMaxDPhiDist() <<
    ", doEtaAxesDivision = " << doEtaAxesDivision() << 
    ", doPhiAxesDivision = " << doPhiAxesDivision();

  stream << "|" << std::endl;
}

} // namespace xAOD

