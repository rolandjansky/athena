/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_DETECTORTOOL_H
#define ALFA_DETECTORTOOL_H

#include "GeoModelUtilities/GeoModelTool.h"
#include "ALFA_DetectorFactory.h"
#include "AthenaKernel/IIOVDbSvc.h"
#include "AthenaKernel/IIOVSvc.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#define COOLFOLDER_DETSWCORR "/FWD/ALFA/position_calibration"

typedef struct _USERTRANSFORM
{
	int iRPot;
	double fAngle;
	CLHEP::Hep3Vector vecRotation;
	CLHEP::Hep3Vector vecTranslation;
} USERTRANSFORM, *PUSERTRANSFORM;


class ALFA_DetectorTool : public GeoModelTool 
{
	private:
		CONFIGURATION m_Config;
		ALFA_DetectorFactory* m_pALFADetectorFactory;
		ServiceHandle< IIOVDbSvc > m_iovSvc;

	public:
		// Standard Constructor
		ALFA_DetectorTool( const std::string& type, const std::string& name, const IInterface* parent );

		// Standard Destructor
		virtual ~ALFA_DetectorTool() override final;

		// Build geometry and store Manager to the TDS
		virtual StatusCode create() override final;

private:
		virtual StatusCode registerCallback() override final;
		virtual StatusCode align(IOVSVC_CALLBACK_ARGS) override final;
};

#endif
