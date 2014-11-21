/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef particlejettools_jetpartontruthlabel_header
#define particlejettools_jetpartontruthlabel_header

#include "JetRec/JetModifierBase.h"
#include "xAODJet/Jet.h"

namespace Analysis {
	class JetPartonTruthLabel :
		public JetModifierBase {
			ASG_TOOL_CLASS(JetPartonTruthLabel, IJetModifier)

			public:
				// Constructor and destructor
				JetPartonTruthLabel(std::string name) : 
					JetModifierBase(name) { };

				int modifyJet(xAOD::Jet &jet) const;
	};
}

#endif
