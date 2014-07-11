/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelGangedMerger.h
//   Header file for class PixelGangedMerger
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to add/merge ganged pixels
///////////////////////////////////////////////////////////////////

/** @class PixelGangedMerger
 * @brief merges ganged pixels
 * @author Fredrik Tegenfeldt
 *
 * \b Description
 *
 * This class merges ganged pixels.
 * The pixel corresponding to the readout electronics is retained and obtains the merged charge.
 * The other pixel is masked out.
 *
 */

#ifndef PIXELDIGITIZATION_PIXELGANGEDMERGER_H
#define PIXELDIGITIZATION_PIXELGANGEDMERGER_H

#include "SiDigitization/ISiChargedDiodesProcessorTool.h"
#include "AthenaBaseComps/AthAlgTool.h"


#include "Identifier/Identifier.h"

namespace InDetDD {
	class SiDetectorElement;
}

class PixelID;

class PixelGangedMerger : public AthAlgTool, virtual public ISiChargedDiodesProcessorTool {

public:

  // Constructor:
  PixelGangedMerger( const std::string& type, const std::string& name,const IInterface* parent);

  /** Destructor */
  virtual ~PixelGangedMerger();

  /** AlgTool InterfaceID */
  static const InterfaceID& interfaceID() ;

  /** AlgTool initialize */
  virtual StatusCode initialize();

  /** AlgTool finalize */
  virtual StatusCode finalize();

  /** process the collection of pre digits */
  virtual void process(SiChargedDiodeCollection &collection) const;
  bool isGanged(const Identifier& rdoID,
                const InDetDD::SiDetectorElement* element,
                Identifier& gangedID) const; 

private:

  /** empty constructor mad private */
  PixelGangedMerger();

private:
  const PixelID            *m_pixelID;     /**< the ID helper */
};
#endif // PIXELDIGITIZATION_PIXELGANGEDMERGER_H
