#include "LArCalibTest/LArCablingChecker.h"
#include "LArCalibTest/LArFakeCellFactor.h"
#include "LArCalibTest/LArReadHadDMCoeffFile.h"
#include "LArCalibTest/LArReadHadDMCoeffFile2.h"
#include "LArCalibTest/DumpCaloBadChannels.h"
//#include "LArCalibTest/FixLArIdMap.h"
#include "LArCalibTest/LArFecLvTempDcsTest.h"
#include "LArCalibTest/FixLArElecCalib.h"
#include "LArCalibTest/LArIdMapConvert.h"
#include "LArCalibTest/LArFebRodMapConvert.h"
#include "LArCalibTest/LArShapeToSCShape.h"
#include "LArCalibTest/LArOFCtoOFC.h"




DECLARE_COMPONENT( LArCablingChecker )
DECLARE_COMPONENT( LArReadHadDMCoeffFile )
DECLARE_COMPONENT( LArReadHadDMCoeffFile2 )
DECLARE_COMPONENT( LArFakeCellFactor )
DECLARE_COMPONENT( DumpCaloBadChannels )
//DECLARE_COMPONENT( FixLArIdMap )
DECLARE_COMPONENT( LArFecLvTempDcsTest )
DECLARE_COMPONENT( FixLArElecCalib )
DECLARE_COMPONENT( LArIdMapConvert )
DECLARE_COMPONENT( LArFebRodMapConvert )
DECLARE_COMPONENT( LArShapeToSCShape )
DECLARE_COMPONENT( LArOFCtoOFC )

