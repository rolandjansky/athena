/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SURVEYCONSTRAINTTOOLS_SURVEYCONSTRAINT_IH
#define SURVEYCONSTRAINTTOOLS_SURVEYCONSTRAINT_IH

#include "GaudiKernel/IAlgTool.h"
#include "InDetSurveyConstraintTool/SurveyConstraintPoint.h"
#include "InDetSurveyConstraintTool/SurveyConstraintModule.h"

#include <vector>
#include <map>

#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"
class Identifier;



/** Must declare this, with name of interface*/
static const InterfaceID IID_ISurveyConstraint("ISurveyConstraint", 1, 0);

class ISurveyConstraint : virtual public IAlgTool {
        public:
        /**Declared here, and defined below*/
        static const InterfaceID& interfaceID();
        
        /**Pure virtual*/

	virtual StatusCode computeConstraint(const Identifier&,
					     Amg::VectorX&,
					     double&,
					     Amg::VectorX &,
					     Amg::MatrixX&) =0;
	virtual void setup_SurveyConstraintModules() =0;

	virtual void MMap(std::map<Identifier, SurveyConstraintModule*, std::less<Identifier> >&) =0; 

	virtual int getWeightPixEC(//const Identifier&,
				   Amg::MatrixX&) =0;
	virtual int getWeightPixB(//const Identifier&,
				  Amg::MatrixX&) =0;
	virtual int getWeightSCTEC(//const Identifier&,
				   Amg::MatrixX&) =0;
	virtual int getWeightSCTB(//const Identifier&,
				  Amg::MatrixX&) =0;
	virtual void getSurveyCoordsPixEC(//const Identifier&, 
					  std::vector< Amg::Vector3D > &) =0;
	virtual void getSurveyCoordsPixB(//const Identifier&, 
					 std::vector< Amg::Vector3D > &) =0;
	virtual void getSurveyCoordsSCTEC(//const Identifier&, 
					  std::vector< Amg::Vector3D > &) =0;
	virtual void getSurveyCoordsSCTB(//const Identifier&, 
					 std::vector< Amg::Vector3D > &) =0;
	virtual void GlobalToLocal(SurveyConstraintModule* ,std::vector<SurveyConstraintPoint>&) =0;
	virtual int SectorNumber(int) =0;
	virtual double PhiModuleToSector(int) =0;
	virtual void TransformSector(Identifier,
				     SurveyConstraintModule*,
				     Amg::Vector3D&,
				     Amg::Transform3D) =0;
};

inline const InterfaceID& ISurveyConstraint::interfaceID()
{ 
        return IID_ISurveyConstraint; 
}

#endif // SURVEYCONSTRAINTTOOLS_SURVEYCONSTRAINT_IH
