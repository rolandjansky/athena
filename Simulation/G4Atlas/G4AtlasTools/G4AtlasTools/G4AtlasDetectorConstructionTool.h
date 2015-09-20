/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AtlasDetectorConstructionTool_H
#define G4AtlasDetectorConstructionTool_H

// Include files
// from Gaudi
#include "AthenaBaseComps/AthAlgTool.h"

#include "G4AtlasInterfaces/IDetectorConstructionTool.h"

#include "G4VUserDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"

/** @class G4AtlasDetectorConstructionTool G4AtlasDetectorConstructionTool.h "G4AtlasTools/G4AtlasDetectorConstructionTool.h"
*  
*  Tool for the concrete implementation of a G4Atlas-based detector construction
* 
*  @author Andrea Dell'Acqua
*  @date   2015-03-06
*/
class G4AtlasDetectorConstructionTool final : public AthAlgTool, virtual public IDetectorConstructionTool {

public:
/// Standard constructor
	G4AtlasDetectorConstructionTool( const std::string& type , const std::string& name,
				     const IInterface* parent ) ;
                        
	~G4AtlasDetectorConstructionTool( ); ///< Destructor

/// Initialize method
	StatusCode initialize( ) ;
	
	G4VUserDetectorConstruction* GetDetectorConstruction() {return this;}
                                           

/** Implements 
                                              */
											  
	void SetWorld(G4VPhysicalVolume*);
	G4VPhysicalVolume* Construct();
		
protected:
	
	G4VPhysicalVolume* world;
	
};

#endif
