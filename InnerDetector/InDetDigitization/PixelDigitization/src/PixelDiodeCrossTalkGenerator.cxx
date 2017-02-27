/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelDiodeCrossTalkGenerator.cxx
//   Implementation file for class PixelDiodeCrossTalkGenerator
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "PixelDiodeCrossTalkGenerator.h"
#include "SiDigitization/SiChargedDiodeCollection.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetReadoutGeometry/SiCellId.h"
#include "InDetIdentifier/PixelID.h"

using namespace InDetDD;

PixelDiodeCrossTalkGenerator::PixelDiodeCrossTalkGenerator(const std::string& type, const std::string& name,const IInterface* parent):
  PixelProcessorTool(type,name,parent),
  m_diodeCrossTalk(.06)
{  
  declareProperty("DiodeCrossTalk",m_diodeCrossTalk,"Diode cross talk factor");
}

PixelDiodeCrossTalkGenerator::~PixelDiodeCrossTalkGenerator() {}

StatusCode PixelDiodeCrossTalkGenerator::initialize() {
  CHECK(PixelProcessorTool::initialize());
  ATH_MSG_DEBUG("PixelDiodeCrossTalkGenerator::initialize()");
	return StatusCode::SUCCESS;
}

StatusCode PixelDiodeCrossTalkGenerator::finalize() {
  ATH_MSG_DEBUG("PixelDiodeCrossTalkGenerator::finalize()");
	return StatusCode::SUCCESS;
}

void PixelDiodeCrossTalkGenerator::process(SiChargedDiodeCollection &collection) {

  // if the cross talk factor if 0, do nothing
  if (0==m_diodeCrossTalk) return;

  // get pixel module design and check it
  const PixelModuleDesign *p_design= static_cast<const PixelModuleDesign *>(&(collection.design()));
  if (!p_design) return;

  // create a local copy of the current collection
  // (cross-talk must be added only to the current SiChargedDiodes)
  // -ME fix- std::map<Identifier32,SiChargedDiode> oldChargedDiodes=collection.chargedDiodes();
  SiChargedDiodeMap oldChargedDiodes=collection.chargedDiodes();

  // loop on all old charged diodes
  // -ME fix- for(std::map<Identifier32,SiChargedDiode>::const_iterator p_chargedDiode=
  for (SiChargedDiodeMap::const_iterator p_chargedDiode=oldChargedDiodes.begin(); p_chargedDiode!=oldChargedDiodes.end(); ++p_chargedDiode) {

    // current diode
    SiCellId diode=(*p_chargedDiode).second.diode();

    // get the list of neighbours for this diode
    std::vector<SiCellId> neighbours;
    p_design->neighboursOfCell(diode,neighbours);

    // loop on all neighbours
    for (std::vector<SiCellId>::const_iterator p_neighbour=neighbours.begin(); p_neighbour!=neighbours.end(); ++p_neighbour) {

      // get the intersection length between the two diodes
      const double intersection=p_design->intersectionLength(diode,*p_neighbour);

      // add cross talk only if the intersection is non-zero
      // if the original pixel is at (col,row) then the intersection length is
      // (col+-1, row+-1) : 0 -> diagonal
      // (col   , row+-1) : 0.4 mm (or 0.6 if long pixel) pixel width  = 400um or 600um
      // (col+-1, row   ) : 0.05 mm , pixel height = 50um
      // intersection length is just the length of the contact surface between
      // the two pixels
      if (intersection>0) {
        //
        // create a new charge:
        // Q(new) = Q*L*X
        //   Q = charge of source pixel
        //   L = intersection length [mm]
        //   X = crosstalk factor    [mm-1]
        //
        const SiChargedDiode & chargedDiode = (*p_chargedDiode).second;

        SiCharge charge(chargedDiode.charge()*intersection*m_diodeCrossTalk, chargedDiode.totalCharge().time(), SiCharge::diodeX_Talk, chargedDiode.totalCharge().particleLink());
        // add this new charge
        collection.add(*p_neighbour,charge);
      }
    }
  }
}

