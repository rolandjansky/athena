/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGGING_IJETBTAGGERTOOL_HH
#define BTAGGING_IJETBTAGGERTOOL_HH
///////////////////////////////////////////
///
/// \class JetBTaggerTool
/// jet modfier tool to run and add btagging information.
////////////////////////////////////////////

#include "GaudiKernel/IAlgTool.h"
#include "xAODJet/Jet.h"


namespace Analysis{

static const InterfaceID IID_IJetBTaggerTool("IJetBTaggerTool", 1, 0);


class  IJetBTaggerTool: virtual public IAlgTool { 
  public:

	/** Virtual destructor */
        virtual ~IJetBTaggerTool(){};

        /** AlgTool interface methods */
        static const InterfaceID& interfaceID() { return IID_IJetBTaggerTool; };
  
        virtual StatusCode initialize() = 0;
        virtual int modify(xAOD::JetContainer& jets) const = 0;


};

} // End namespace
#endif
