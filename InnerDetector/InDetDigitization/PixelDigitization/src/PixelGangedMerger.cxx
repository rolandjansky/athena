/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelGangedMerger.cxx
//   Implementation file for class PixelGangedMerger
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 24/03/2007 Fredrik Tegenfeldt <fredrik.tegenfeldt@cern.ch>
///////////////////////////////////////////////////////////////////

#include "PixelGangedMerger.h"
#include "SiDigitization/SiChargedDiodeCollection.h"
#include "SiDigitization/SiHelper.h"
#include "InDetIdentifier/PixelID.h"

using namespace InDetDD;

static const InterfaceID IID_IPixelGangedMerger("PixelGangedMerger", 1, 0);
const InterfaceID& PixelGangedMerger::interfaceID( ){ return IID_IPixelGangedMerger; }

// Constructor with parameters:
PixelGangedMerger::PixelGangedMerger(const std::string& type, const std::string& name,const IInterface* parent):
  AthAlgTool(type,name,parent),
  m_pixelID(0)
{
  declareInterface< PixelGangedMerger >( this );
}

// Destructor:
PixelGangedMerger::~PixelGangedMerger()
{}


//----------------------------------------------------------------------
// Initialize
//----------------------------------------------------------------------
StatusCode PixelGangedMerger::initialize() {
  StatusCode sc = AthAlgTool::initialize(); 
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "PixelGangedMerger::initialize() failed");
    return sc ;
  }
  ATH_MSG_DEBUG ( "PixelGangedMerger::initialize()");
  //  
  // get PixelID
  //
  if ( detStore()->retrieve(m_pixelID,"PixelID").isFailure() ) {
    // if this fails, it's probably a bug -> FATAL!
    ATH_MSG_FATAL ( "Could not get Pixel ID helper" );
    return StatusCode::FAILURE;
  }
  return sc ;
}

//----------------------------------------------------------------------
// finalize
//----------------------------------------------------------------------
StatusCode PixelGangedMerger::finalize() {
  StatusCode sc = AthAlgTool::finalize();
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "PixelGangedMerger::finalize() failed");
    return sc ;
  }
  ATH_MSG_DEBUG ( "PixelGangedMerger::finalize()");
  return sc ;
}

// process the collection of diode collection
void PixelGangedMerger::process(SiChargedDiodeCollection &collection) const
{
  // loop over the collection and check if the diode is a ganged pixel
  // if ganged, find its ganged partner and merge/copy charge
  // we need:
  //   * check if ID is ganged
  //   * given an ID, find its ganged partner
  //   * create new charge
  //
  Identifier gangedID;
  for(SiChargedDiodeIterator i_chargedDiode=collection.begin();
      i_chargedDiode!=collection.end();
      ++i_chargedDiode) {

    if (isGanged(collection.getId((*i_chargedDiode).first),collection.element(),gangedID)) {
      SiTotalCharge thisTotalCharge = (*i_chargedDiode).second.totalCharge();
      SiChargedDiode *gangedChargeDiode = collection.find(gangedID);
      int phiGanged = m_pixelID->phi_index(gangedID);
      int phiThis   = m_pixelID->phi_index(collection.getId((*i_chargedDiode).first));
      //
      if (gangedChargeDiode) { // merge charges
        bool maskGanged = ((phiGanged>159) && (phiGanged<168));
        bool maskThis   = ((phiThis>159) && (phiThis<168));
        //
        // mask the one ganged pixel that does not correspond to the readout electronics.
        // not really sure this is needed
        //
        if (maskGanged && maskThis) {
          ATH_MSG_ERROR ( "PixelGangedMerger: both ganged pixels are in the mask out region -> BUG!" );
        }
        if (maskGanged) {
          (*i_chargedDiode).second.add( gangedChargeDiode->totalCharge() ); // merged org pixel
          //        gangedChargeDiode->add(thisTotalCharge); // merged ganged pixel
          SiHelper::maskOut(*gangedChargeDiode,true);
        } else {
          gangedChargeDiode->add( (*i_chargedDiode).second.totalCharge() ); // merged org pixel
          //        gangedChargeDiode->add(thisTotalCharge); // merged ganged pixel
          SiHelper::maskOut((*i_chargedDiode).second,true);
        }
      }
    }
  }
}

bool PixelGangedMerger::isGanged(const Identifier& rdoID,
                                 const InDetDD::SiDetectorElement* element,
                                 Identifier& gangedID) const
{
  InDetDD::SiCellId cellID = element->cellIdFromIdentifier(rdoID);
  // gangedCell will be invalid if cell was not ganged.
  InDetDD::SiCellId gangedCell = element->gangedCell(cellID);
  // Convert to identifier
  // The following will create an invalid ID if gangedCell is invalid.
  gangedID = element->identifierFromCellId(gangedCell);
  return gangedCell.isValid();
}


