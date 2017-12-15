/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// MuidCaloMaterialParam
//  AlgTool containing a calorimeter material parametrization.
//  This parametrization represents the Coulomb scattering between the InDet
//  and MuonSpectrometer entrance.
// 
//  Parametrized positions and thicknesses (CaloLayers) are stored
//  as a function of eta assuming any track will 'see' 2 scattering centres,
//  taken from an inner and an outer layer.
//
//  Tabulated info taken from a geantino map
//
//  (c) ATLAS Combined Muon software
//////////////////////////////////////////////////////////////////////////////


//<<<<<< INCLUDES                                                       >>>>>>

#include <cmath>
#include <iomanip>
#include <iostream>
#include "GaudiKernel/SystemOfUnits.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "MuidCaloScatteringTools/MuidCaloMaterialParam.h"
#include "MuidEvent/CaloLayer.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/Surface.h"
#include "TrkTrack/TrackStateOnSurface.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

namespace Rec
{
 
MuidCaloMaterialParam::MuidCaloMaterialParam (const std::string&	type,
					      const std::string&	name, 
					      const IInterface*		parent)
    :	AthAlgTool			(type, name, parent),
	m_surfaceDisplayTool		("Trk::TrackingVolumeDisplayer/TrackingVolumeDisplayer"),
	m_produceSurfacesDisplay	(false),
	m_binSize                       (1),
	m_numberBins			(277)
{
    declareInterface<IMuidCaloMaterialParam>(this);
    declareProperty("SurfaceDisplayTool",	m_surfaceDisplayTool);
    declareProperty("ProduceSurfacesFile",	m_produceSurfacesDisplay);
}

MuidCaloMaterialParam::~MuidCaloMaterialParam (void) 
{}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

StatusCode
MuidCaloMaterialParam::initialize()
{
    ATH_MSG_INFO( "Initializing MuidCaloMaterialParam" );

    // get the display tool
    if (m_produceSurfacesDisplay)	
    {
	if (m_surfaceDisplayTool.retrieve().isFailure())
	{
	    ATH_MSG_FATAL( "Failed to retrieve tool " << m_surfaceDisplayTool );
	    return StatusCode::FAILURE;
	}
	else
	{
	    ATH_MSG_INFO( "Retrieved tool " << m_surfaceDisplayTool );
	}
    }
    else{
      m_surfaceDisplayTool.disable();
    }
	
    // setup up CaloMaterial
    m_caloInnerLayers.reserve(2*m_numberBins);
    m_caloOuterLayers.reserve(2*m_numberBins);
    m_innerBackwardSurfaces.reserve(m_numberBins);
    m_innerForwardSurfaces.reserve(m_numberBins);
    m_middleBackwardSurfaces.reserve(m_numberBins);
    m_middleForwardSurfaces.reserve(m_numberBins);
    m_outerBackwardSurfaces.reserve(m_numberBins);
    m_outerForwardSurfaces.reserve(m_numberBins);
    m_radiationThickness.reserve(m_numberBins);
    
    if (StatusCode::SUCCESS != defineCaloMaterial())
    {
	ATH_MSG_FATAL( " inconsistent eta binning - check setup " );
	return StatusCode::FAILURE;
    }

    if (msgLvl(MSG::DEBUG))
    {
	for (std::vector<const CaloLayer*>::const_iterator lay = m_caloOuterLayers.begin();
	     lay != m_caloOuterLayers.end();
	     ++lay)
	{    
	    //if((**lay).Zmax()>0.)
	    if ((**lay).Z()>0.)
	    {
		(**lay).print();
	    }
	}	    
    }

    return StatusCode::SUCCESS;
}

StatusCode
MuidCaloMaterialParam::finalize()
{
    ATH_MSG_DEBUG( "Finalizing MuidCaloMaterialParam" );
    
    for (std::vector<const Trk::Surface*>::iterator s = m_innerBackwardSurfaces.begin();
	 s != m_innerBackwardSurfaces.end();
	 ++s)
	delete *s;
    for (std::vector<const Trk::Surface*>::iterator s = m_innerForwardSurfaces.begin();
	 s != m_innerForwardSurfaces.end();
	 ++s)
	delete *s;
    for (std::vector<const Trk::Surface*>::iterator s = m_middleBackwardSurfaces.begin();
	 s != m_middleBackwardSurfaces.end();
	 ++s)
	delete *s;
    for (std::vector<const Trk::Surface*>::iterator s = m_middleForwardSurfaces.begin();
	 s != m_middleForwardSurfaces.end();
	 ++s)
	delete *s;
    for (std::vector<const Trk::Surface*>::iterator s = m_outerBackwardSurfaces.begin();
	 s != m_outerBackwardSurfaces.end();
	 ++s)
	delete *s;
    for (std::vector<const Trk::Surface*>::iterator s = m_outerForwardSurfaces.begin();
	 s != m_outerForwardSurfaces.end();
	 ++s)
	delete *s;

    for (std::vector<const CaloLayer*>::const_iterator lay = m_caloInnerLayers.begin();
	 lay != m_caloInnerLayers.end();
	 ++lay)
	delete *lay;
    for (std::vector<const CaloLayer*>::const_iterator lay = m_caloOuterLayers.begin();
	 lay != m_caloOuterLayers.end();
	 ++lay)
	delete *lay;
    
    return StatusCode::SUCCESS;
}

const Trk::Surface*
MuidCaloMaterialParam::innerSurface (double eta) const
{
    if (eta < 0.)
    {
	unsigned bin = static_cast<unsigned>(-eta/m_binSize);
	if (bin >= m_numberBins) bin = m_numberBins - 1;
	
	ATH_MSG_VERBOSE( std::setiosflags(std::ios::fixed)
			 << " innerSurface:  eta " << std::setw(7) << std::setprecision(3) << eta
			 << "   backwards bin " << bin );
	return m_innerBackwardSurfaces[bin];
    }
    else if (eta > 0.)
    {
	unsigned bin = static_cast<unsigned>(eta/m_binSize);
	if (bin >= m_numberBins) bin = m_numberBins - 1;
	
	ATH_MSG_VERBOSE( std::setiosflags(std::ios::fixed)
			 << " innerSurface:  eta " << std::setw(7) << std::setprecision(3) << eta
			 << "   forward bin " << bin );
	return m_innerForwardSurfaces[bin];
    }
    
    ATH_MSG_VERBOSE( std::setiosflags(std::ios::fixed)
		     << " innerSurface:  eta " << std::setw(7) << std::setprecision(3) << eta
		     << "   forward bin 0" );
    
    return m_innerForwardSurfaces[0];
}

const Trk::Surface*
MuidCaloMaterialParam::middleSurface (double eta) const
{
    if (eta < 0.)
    {
	unsigned bin = static_cast<unsigned>(-eta/m_binSize);
	if (bin >= m_numberBins) bin = m_numberBins - 1;
	
	ATH_MSG_VERBOSE( std::setiosflags(std::ios::fixed)
			 << " middleSurface:  eta " << std::setw(7) << std::setprecision(3) << eta
			 << "   backwards bin " << bin );
	return m_middleBackwardSurfaces[bin];
    }
    else if (eta > 0.)
    {
	unsigned bin = static_cast<unsigned>(eta/m_binSize);
	if (bin >= m_numberBins) bin = m_numberBins - 1;
	
	ATH_MSG_VERBOSE( std::setiosflags(std::ios::fixed)
			 << " middleSurface:  eta " << std::setw(7) << std::setprecision(3) << eta
			 << "   forward bin " << bin );
	return m_middleForwardSurfaces[bin];
    }
    
    
	ATH_MSG_VERBOSE( std::setiosflags(std::ios::fixed)
			  << " middleSurface:  eta " << std::setw(7) << std::setprecision(3) << eta
			  << "   forward bin 0" );
    
    return m_middleForwardSurfaces[0];
}

const Trk::Surface*
MuidCaloMaterialParam::outerSurface (double eta) const
{
    if (eta < 0.)
    {
	unsigned bin = static_cast<unsigned>(-eta/m_binSize);
	if (bin >= m_numberBins) bin = m_numberBins - 1;
	
	ATH_MSG_VERBOSE( std::setiosflags(std::ios::fixed)
			 << " outerSurface:  eta " << std::setw(7) << std::setprecision(3) << eta
			 << "   backwards bin " << bin );
	return m_outerBackwardSurfaces[bin];
    }
    else if (eta > 0.)
    {
	unsigned bin = static_cast<unsigned>(eta/m_binSize);
	if (bin >= m_numberBins) bin = m_numberBins - 1;
	
	ATH_MSG_VERBOSE( std::setiosflags(std::ios::fixed)
			 << " outerSurface:  eta " << std::setw(7) << std::setprecision(3) << eta
			 << "   forward bin " << bin );
	return m_outerForwardSurfaces[bin];
    }

    ATH_MSG_VERBOSE( std::setiosflags(std::ios::fixed)
		     << " outerSurface:  eta " << std::setw(7) << std::setprecision(3) << eta
		     << "   forward bin 0" );
    
    return m_outerForwardSurfaces[0];
}

double
MuidCaloMaterialParam::radiationThickness (double eta) const
{
    if (eta < 0.)
    {
	unsigned bin = static_cast<unsigned>(-eta/m_binSize);
	if (bin >= m_numberBins) bin = m_numberBins - 1;

	return m_radiationThickness[bin];
    }
    else if (eta > 0.)
    {
	unsigned bin = static_cast<unsigned>(eta/m_binSize);
	if (bin >= m_numberBins) bin = m_numberBins - 1;

	return m_radiationThickness[bin];
    }
    return m_radiationThickness[0];
}

const Trk::TrackStateOnSurface*
MuidCaloMaterialParam::trackStateOnSurface (const Trk::TrackParameters* trackParameters) const
{
    // find radiationThickness corresponding to surface
    // FIXME: use Surface* look-up method for execution speed
    double thickness = radiationThickness(trackParameters->position().eta());
    const Trk::MaterialEffectsBase*	materialEffects =
	new const Trk::MaterialEffectsOnTrack(thickness,trackParameters->associatedSurface());
    
    // create TSOS
    const Trk::FitQualityOnSurface*	fitQoS		= 0;
    const Trk::MeasurementBase*		measurementBase	= 0;
    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> pattern(0);
    pattern.set(Trk::TrackStateOnSurface::Scatterer);

    ATH_MSG_VERBOSE( " trackStateOnSurface::Scatterer with radiationThickness " << thickness );
    
    return new const Trk::TrackStateOnSurface(measurementBase,
					      trackParameters,
					      fitQoS,
					      materialEffects,
					      pattern);
}

//<<<<<< PRIVATE MEMBER FUNCTION DEFINITIONS                            >>>>>>

Trk::Surface*
MuidCaloMaterialParam::createSurface (double eta, double r, double z, double cotThetaWidth)
{
    Amg::Transform3D* transform		= new Amg::Transform3D;
    transform->setIdentity();
    (*transform)			= Amg::Translation3D(0.,0.,z);
    double halfZLength			= cotThetaWidth*r;
    if (std::abs(eta) < 1.4)
    {
	Trk::CylinderSurface* surf	= new Trk::CylinderSurface(transform,r,halfZLength);
	if (m_produceSurfacesDisplay) m_surfaceDisplayTool->process(*surf);
	return surf;
    }
    else
    {
	double halfRLength		= halfZLength*r/std::abs(z);
	Trk::DiscSurface* surf = new Trk::DiscSurface(transform, r-halfRLength, r+halfRLength);
	if (m_produceSurfacesDisplay) m_surfaceDisplayTool->process(*surf);
	return surf;
    }
}

StatusCode
MuidCaloMaterialParam::defineCaloMaterial (void)
{
    // careful - need const int declaration for array => check consistency
    const int N	= 277;
    if (static_cast<int>(m_numberBins) != N) return StatusCode::FAILURE;
    
    // In meters... be careful!!!
    //const double mean[N] = {2.90771,2.78248,2.83172,2.7975,2.76139,2.88316,2.83738,2.81438,2.83947,2.87006,2.8278,2.85738,2.86536,2.94242,2.83682,2.96257,2.96406,2.86074,2.89693,2.83767,2.85071,2.99601,3.01353,2.89679,2.90693,2.93785,2.93202,2.93902,2.95558,2.91682,2.96795,3.07245,2.99349,2.98052,3.11216,3.02759,3.12314,2.99859,3.14099,3.04129,3.17957,3.07913,3.20328,3.11009,3.12813,3.24812,3.26814,3.17714,3.16095,3.3125,3.32224,3.19366,3.21848,3.21796,3.25126,3.268,3.31347,3.42457,3.29278,3.33556,3.3819,3.39939,3.51691,3.36272,3.37978,3.39153,3.38534,3.50773,3.61067,3.30092,3.5418,3.49208,3.53835,3.53299,3.60777,3.32772,3.38532,3.48208,3.54515,3.77164,3.71523,3.91961,3.80365,3.84777,3.88416,4.06013,4.1111,3.94053,3.9722,3.96949,4.03478,4.09423,4.14198,4.38723,4.28485,4.33281,4.3333,4.41784,4.43311,4.45871,4.54202,4.64106,4.69047,4.65375,4.77979,4.78705,4.82244,4.883,4.89827,4.94015,4.92729,4.97583,4.97669,4.99448,5.25401,5.29072,5.10841,5.32148,5.22758,5.26328,5.28108,5.49924,5.49748,5.18736,5.18277,5.4169,5.31641,5.33159,5.20546,5.21829,5.21831,5.24387,5.25622,5.26771,5.29139,5.31036,5.33198,5.34383,5.34278,5.33719,5.34692,5.34394,5.34866,5.35532,5.38633,5.35792,5.40507,5.44075,5.43697,5.36688,5.3615,5.34395,5.351,5.33692,5.40554,5.32767,5.30937,5.27858,5.24888,5.22321,5.18993,5.16593,5.15314,5.19365,5.10601,5.09906,5.14761,5.11907,5.09005,5.08759,5.12649,5.09114,5.10094,5.11952,5.14246,5.15773,5.20285,5.23313,5.26284,5.29047,5.31104,5.31609,5.34698,5.32669,5.32644,5.33017,5.32103,5.3161,5.30679,5.29896,5.28471,5.29577,5.28692,5.28526,5.28018,5.25654,5.27276,5.26505,5.25626,5.25022,5.22271,5.23219,5.24233,5.23496,5.22445,5.22157,5.22442,5.21315,5.19474,5.20876,5.20789,5.18328,5.19939,5.19362,5.1882,5.17646,5.18578,5.21858,5.30511,5.28661,5.28584,5.3002,5.29541,5.28864,5.30157,5.28195,5.29587,5.28504,5.29397,5.29198,5.28954,5.28545,5.28473,5.28091,5.27896,5.281,5.28209,5.27643,5.27176,5.28034,5.26778,5.26428,5.27236,5.26271,5.25728,5.20309,5.26363,5.25403,5.26598,5.27713,5.2639,5.34616,5.34954,5.34928,5.34872,5.34628,5.34574,5.3471,5.35188,5.34754,5.34501,5.34445,5.34497,5.34499,5.34479,5.33511,5.34532,5.33912,5.3364,5.34464,5.34311,5.27807,5.31777,5.3388,5.33045,5.32491,5.33117,5.33274};

    const double mean[N] = {2.90896,2.85532,2.83093,2.8064,2.76147,2.77842,2.84819,2.85462,2.84207,2.8698,2.85196,2.85928,2.86565,2.85887,2.86074,2.86888,2.87614,2.88714,2.89461,2.88509,2.89893,2.90348,2.9149,2.92062,2.93808,2.93909,2.95316,2.96446,2.95808,2.9744,2.98252,2.99579,3.0031,3.00655,3.01533,3.02955,3.0375,3.04185,3.04363,3.06828,3.07989,3.08877,3.10764,3.11947,3.14111,3.14608,3.16545,3.1774,3.19129,3.20832,3.21966,3.2294,3.2477,3.26422,3.28213,3.29407,3.31355,3.31759,3.33572,3.36416,3.38337,3.40047,3.42178,3.44093,3.46112,3.469,3.48594,3.50981,3.49446,3.48858,3.47374,3.5025,3.53872,3.53273,3.47625,3.44053,3.44348,3.49941,3.54464,3.60332,3.71618,3.75569,3.80481,3.84988,3.88482,3.90896,3.93518,3.96212,3.98705,4.03085,4.0863,4.1386,4.18874,4.24235,4.28526,4.33191,4.37922,4.42264,4.47627,4.52819,4.58994,4.64149,4.69026,4.74336,4.78171,4.81408,4.84981,4.88344,4.9096,4.94122,4.96842,4.99079,5.02283,5.06858,5.08828,5.12193,5.15831,5.1959,5.22796,5.26302,5.28034,5.31236,5.30035,5.28938,5.30971,5.30201,5.3224,5.33088,5.27099,5.21831,5.23592,5.24828,5.26303,5.26884,5.29259,5.30454,5.32996,5.34636,5.34636,5.36077,5.36379,5.3704,5.37932,5.38896,5.39967,5.40373,5.41283,5.44188,5.42873,5.43097,5.4239,5.40874,5.41333,5.40596,5.39171,5.37567,5.35638,5.31926,5.29116,5.26111,5.23343,5.20337,5.17801,5.15636,5.14166,5.16448,5.16522,5.13663,5.11057,5.09843,5.14826,5.11023,5.11505,5.13523,5.15454,5.17912,5.21404,5.246,5.27176,5.29481,5.31911,5.33475,5.35133,5.34602,5.33701,5.3328,5.32425,5.32026,5.31533,5.31104,5.30473,5.30016,5.29413,5.28658,5.28047,5.27475,5.2677,5.26156,5.25584,5.24831,5.24262,5.23702,5.23091,5.22422,5.21841,5.21265,5.2077,5.20334,5.19879,5.19395,5.18991,5.18564,5.18073,5.17633,5.17276,5.16907,5.1658,5.29633,5.29401,5.29103,5.29047,5.2896,5.28996,5.2899,5.28791,5.28734,5.28593,5.28437,5.28506,5.28519,5.28436,5.2837,5.28259,5.28195,5.28128,5.28153,5.28072,5.28068,5.28069,5.27997,5.27685,5.27733,5.27693,5.27541,5.27407,5.27409,5.27443,5.27419,5.27407,5.27377,5.27264,5.34869,5.3483,5.34923,5.34963,5.34951,5.34925,5.34931,5.34959,5.34892,5.34872,5.34848,5.34837,5.34681,5.34619,5.34639,5.34711,5.34672,5.34638,5.34594,5.3456,5.34591,5.34566,5.34428,5.33676,5.3333,5.33278};


    //const double rms[N] = {0.824679,0.782446,0.818811,0.806885,0.803704,0.871136,0.813056,0.788195,0.814913,0.827358,0.803204,0.822611,0.823937,0.856411,0.809335,0.856246,0.863831,0.814316,0.837086,0.816152,0.827546,0.856583,0.868094,0.818259,0.827974,0.836275,0.827927,0.830695,0.841271,0.837476,0.853636,0.887113,0.852725,0.85005,0.889713,0.863394,0.906837,0.858824,0.905646,0.864854,0.916142,0.884857,0.92623,0.890286,0.893136,0.927233,0.93955,0.901352,0.910599,0.946087,0.953492,0.914146,0.919538,0.922878,0.926503,0.92716,0.941653,0.985742,0.947067,0.9437,0.968053,0.97057,1.01549,0.935575,0.936608,0.942302,0.937645,1.00356,1.05201,0.883722,1.04139,1.02598,1.04668,1.0511,1.09945,0.968244,1.01134,1.07265,1.10059,1.18976,1.13585,1.20976,1.17207,1.18968,1.2017,1.27592,1.28886,1.26559,1.28489,1.29385,1.30249,1.30601,1.31771,1.37203,1.34166,1.34777,1.3422,1.36425,1.35728,1.36912,1.38495,1.40415,1.40586,1.40536,1.40853,1.40943,1.4182,1.40153,1.40832,1.40528,1.3878,1.41426,1.39917,1.4051,1.46129,1.46377,1.41189,1.46455,1.44021,1.44527,1.44781,1.51096,1.50057,1.3753,1.34927,1.4508,1.40248,1.38909,1.31386,1.29765,1.29552,1.28221,1.27349,1.25879,1.25227,1.23533,1.22174,1.21283,1.19403,1.18947,1.16907,1.16221,1.14786,1.13494,1.13355,1.10559,1.09656,1.06811,1.06088,1.0577,1.04911,1.05024,1.02118,1.00554,0.989293,0.972336,0.961332,0.954117,0.955509,0.942485,0.927889,0.923071,0.910241,0.906156,0.895033,0.959418,0.948962,0.93796,0.926793,0.899814,0.911334,0.867638,0.867571,0.862463,0.854489,0.857443,0.866902,0.872196,0.86776,0.882162,0.898834,0.898431,0.900525,0.899913,0.892846,0.885463,0.896011,0.897224,0.898276,0.894747,0.891653,0.889951,0.889985,0.890034,0.889848,0.891503,0.881424,0.886063,0.883189,0.887374,0.889464,0.885434,0.880578,0.881069,0.879039,0.880486,0.877794,0.882369,0.879658,0.879052,0.875904,0.882218,0.869874,0.874634,0.873405,0.878323,0.864371,0.936041,0.982519,0.983783,0.984115,0.983673,0.987755,0.987389,0.989652,0.991566,0.982455,0.99419,0.990369,0.991263,0.984783,0.992409,0.994242,0.99489,0.996674,0.995799,0.994068,0.995591,1.00187,0.989916,1.00784,1.00462,1.00311,1.00495,1.00595,0.960289,1.00732,1.01066,1.00818,1.00171,1.01199,0.992116,0.990814,0.994797,0.995172,0.996153,0.996694,0.997064,0.986833,0.994606,0.998802,1.00438,1.00174,1.00119,1.00289,1.00228,1.00238,1.00678,1.00794,1.00604,1.00861,0.963929,1.01359,1.01218,1.01762,1.02796,1.02562,1.02};
    const double rms[N] = {0.823689,0.805241,0.817758,0.805648,0.802719,0.829411,0.808234,0.796944,0.813095,0.826435,0.811776,0.819873,0.823283,0.818004,0.819796,0.820846,0.82749,0.822357,0.818303,0.822916,0.834238,0.821518,0.835878,0.826969,0.83787,0.835485,0.837178,0.842465,0.839582,0.850781,0.847032,0.848794,0.849601,0.85978,0.85481,0.86164,0.866302,0.866074,0.869195,0.874239,0.880656,0.881667,0.888438,0.886809,0.89286,0.892054,0.903708,0.900372,0.904276,0.912044,0.917285,0.92219,0.927522,0.931037,0.935044,0.938616,0.941711,0.946958,0.950666,0.95474,0.967182,0.970128,0.975676,0.97931,0.984179,0.988279,0.995269,1.00143,0.999232,0.998867,0.999908,1.02241,1.0462,1.05131,1.03541,1.02535,1.03805,1.07367,1.09972,1.12079,1.13208,1.15175,1.17083,1.18699,1.20095,1.22169,1.24418,1.26595,1.28708,1.30303,1.31279,1.32175,1.329,1.33418,1.34,1.34675,1.35284,1.36086,1.368,1.37633,1.39387,1.40353,1.40547,1.40902,1.40658,1.40523,1.40309,1.40089,1.40122,1.40212,1.40281,1.40877,1.40972,1.41208,1.41789,1.42165,1.42453,1.42954,1.43856,1.44613,1.44894,1.45408,1.4347,1.41351,1.41201,1.40167,1.39811,1.38846,1.34956,1.29556,1.28353,1.27768,1.26644,1.25752,1.24987,1.23723,1.22353,1.20934,1.19558,1.18253,1.17099,1.15867,1.15074,1.13651,1.12275,1.10774,1.09431,1.06622,1.06609,1.06611,1.06505,1.05314,1.0261,1.00457,0.990423,0.968341,0.964148,0.959084,0.956413,0.943587,0.935617,0.923163,0.913453,0.903224,0.896578,0.925755,0.95124,0.939449,0.925327,0.898027,0.91426,0.875688,0.862338,0.858283,0.857438,0.86037,0.862203,0.870719,0.876149,0.887925,0.895045,0.898852,0.90329,0.895159,0.892505,0.893056,0.900081,0.896356,0.895693,0.891834,0.893339,0.888931,0.888282,0.890026,0.888641,0.886521,0.887356,0.885548,0.882484,0.88512,0.882618,0.88119,0.880995,0.88102,0.88134,0.88315,0.8824,0.880437,0.879697,0.879628,0.878431,0.877552,0.878481,0.878691,0.877373,0.876806,0.875456,0.978556,0.981988,0.985465,0.985556,0.986936,0.984079,0.982189,0.984073,0.984913,0.987486,0.989538,0.987139,0.985936,0.987119,0.988206,0.990297,0.991416,0.992575,0.991251,0.992607,0.992754,0.993668,0.994706,0.996367,0.996439,0.997143,1.0003,1.00234,1.0024,1.00091,1.00194,1.00287,1.00385,1.00475,0.986361,0.988892,0.989372,0.989529,0.989478,0.990144,0.9908,0.990212,0.991309,0.992435,0.993439,0.994017,0.997914,0.99955,0.999073,0.997491,0.99844,0.999279,1.00009,1.00086,1.0004,1.00093,1.00345,1.01321,1.01864,1.0196};
    //const double eta[N] = {0.005,0.015,0.025,0.035,0.045,0.055,0.065,0.075,0.085,0.095,0.105,0.115,0.125,0.135,0.145,0.155,0.165,0.175,0.185,0.195,0.205,0.215,0.225,0.235,0.245,0.255,0.265,0.275,0.285,0.295,0.305,0.315,0.325,0.335,0.345,0.355,0.365,0.375,0.385,0.395,0.405,0.415,0.425,0.435,0.445,0.455,0.465,0.475,0.485,0.495,0.505,0.515,0.525,0.535,0.545,0.555,0.565,0.575,0.585,0.595,0.605,0.615,0.625,0.635,0.645,0.655,0.665,0.675,0.685,0.695,0.705,0.715,0.725,0.735,0.745,0.755,0.765,0.775,0.785,0.795,0.805,0.815,0.825,0.835,0.845,0.855,0.865,0.875,0.885,0.895,0.905,0.915,0.925,0.935,0.945,0.955,0.965,0.975,0.985,0.995,1.005,1.015,1.025,1.035,1.045,1.055,1.065,1.075,1.085,1.095,1.105,1.115,1.125,1.135,1.145,1.155,1.165,1.175,1.185,1.195,1.205,1.215,1.225,1.235,1.245,1.255,1.265,1.275,1.285,1.295,1.305,1.315,1.325,1.335,1.345,1.355,1.365,1.375,1.385,1.395,1.405,1.415,1.425,1.435,1.445,1.455,1.465,1.475,1.485,1.495,1.505,1.515,1.525,1.535,1.545,1.555,1.565,1.575,1.585,1.595,1.605,1.615,1.625,1.635,1.645,1.655,1.665,1.675,1.685,1.695,1.705,1.715,1.725,1.735,1.745,1.755,1.765,1.775,1.785,1.795,1.805,1.815,1.825,1.835,1.845,1.855,1.865,1.875,1.885,1.895,1.905,1.915,1.925,1.935,1.945,1.955,1.965,1.975,1.985,1.995,2.005,2.015,2.025,2.035,2.045,2.055,2.065,2.075,2.085,2.095,2.105,2.115,2.125,2.135,2.145,2.155,2.165,2.175,2.185,2.195,2.205,2.215,2.225,2.235,2.245,2.255,2.265,2.275,2.285,2.295,2.305,2.315,2.325,2.335,2.345,2.355,2.365,2.375,2.385,2.395,2.405,2.415,2.425,2.435,2.445,2.455,2.465,2.475,2.485,2.495,2.505,2.515,2.525,2.535,2.545,2.555,2.565,2.575,2.585,2.595,2.605,2.615,2.625,2.635,2.645,2.655,2.665,2.675,2.685,2.695,2.705,2.715,2.725,2.735,2.745,2.755,2.765,2.775};
    const double eta[N] = {0.005,0.015,0.025,0.035,0.045,0.055,0.065,0.075,0.085,0.095,0.105,0.115,0.125,0.135,0.145,0.155,0.165,0.175,0.185,0.195,0.205,0.215,0.225,0.235,0.245,0.255,0.265,0.275,0.285,0.295,0.305,0.315,0.325,0.335,0.345,0.355,0.365,0.375,0.385,0.395,0.405,0.415,0.425,0.435,0.445,0.455,0.465,0.475,0.485,0.495,0.505,0.515,0.525,0.535,0.545,0.555,0.565,0.575,0.585,0.595,0.605,0.615,0.625,0.635,0.645,0.655,0.665,0.675,0.685,0.695,0.705,0.715,0.725,0.735,0.745,0.755,0.765,0.775,0.785,0.795,0.805,0.815,0.825,0.835,0.845,0.855,0.865,0.875,0.885,0.895,0.905,0.915,0.925,0.935,0.945,0.955,0.965,0.975,0.985,0.995,1.005,1.015,1.025,1.035,1.045,1.055,1.065,1.075,1.085,1.095,1.105,1.115,1.125,1.135,1.145,1.155,1.165,1.175,1.185,1.195,1.205,1.215,1.225,1.235,1.245,1.255,1.265,1.275,1.285,1.295,1.305,1.315,1.325,1.335,1.345,1.355,1.365,1.375,1.385,1.395,1.405,1.415,1.425,1.435,1.445,1.455,1.465,1.475,1.485,1.495,1.505,1.515,1.525,1.535,1.545,1.555,1.565,1.575,1.585,1.595,1.605,1.615,1.625,1.635,1.645,1.655,1.665,1.675,1.685,1.695,1.705,1.715,1.725,1.735,1.745,1.755,1.765,1.775,1.785,1.795,1.805,1.815,1.825,1.835,1.845,1.855,1.865,1.875,1.885,1.895,1.905,1.915,1.925,1.935,1.945,1.955,1.965,1.975,1.985,1.995,2.005,2.015,2.025,2.035,2.045,2.055,2.065,2.075,2.085,2.095,2.105,2.115,2.125,2.135,2.145,2.155,2.165,2.175,2.185,2.195,2.205,2.215,2.225,2.235,2.245,2.255,2.265,2.275,2.285,2.295,2.305,2.315,2.325,2.335,2.345,2.355,2.365,2.375,2.385,2.395,2.405,2.415,2.425,2.435,2.445,2.455,2.465,2.475,2.485,2.495,2.505,2.515,2.525,2.535,2.545,2.555,2.565,2.575,2.585,2.595,2.605,2.615,2.625,2.635,2.645,2.655,2.665,2.675,2.685,2.695,2.705,2.715,2.725,2.735,2.745,2.755,2.765};
    // X0tot/2.
    //const double X0CaloLayers[N] = {54.0718,53.6793,51.1026,52.7207,54.3121,54.5799,54.6933,57.3391,55.3929,52.7291,54.9498,54.8578,53.7012,59.4177,53.3716,59.0585,58.9992,54.2931,59.6594,57.9365,55.2779,61.2433,59.3145,55.276,54.9965,56.4419,55.5374,54.9552,57.0007,54.7047,57.678,63.3163,58.3533,55.4158,62.8817,57.7717,63.6192,58.9201,64.8709,58.0192,63.7163,59.1298,63.678,60.302,59.8701,65.7836,64.7294,60.9158,62.8989,66.3413,66.4773,60.4863,60.8472,61.9235,61.3218,62.1608,63.7831,70.1807,64.1836,64.124,64.3934,64.8063,71.6659,62.5457,63.0828,63.2448,62.6003,67.0684,72.1467,57.8225,67.3932,63.7332,63.74,62.1021,64.3551,55.0171,56.196,59.2418,58.633,64.674,55.9855,62.6046,57.0551,57.7426,57.8879,63.9515,62.4365,56.502,55.5581,53.9051,54.201,54.8809,55.8935,66.3435,59.7587,60.5284,59.6337,62.1275,60.5448,61.3072,59.5853,61.4811,62.1179,62.5012,64.0563,66.8948,68.5185,69.5734,71.4861,73.1542,73.3252,77.6957,77.3028,79.1056,90.128,92.1165,84.2486,94.0138,88.9091,90.6579,91.6065,102.636,102.664,88.2848,86.3781,94.2899,88.1281,85.8002,78.7165,77.144,76.019,73.3795,71.3756,69.5571,68.0864,66.248,64.2698,62.5635,61.8486,61.3565,60.0822,59.4924,58.6984,56.8871,54.5453,56.0838,52.6754,50.3197,49.6728,51.8083,50.7692,50.7799,50.0932,50.1331,45.6858,48.7889,48.6627,48.7291,49.0538,49.2277,49.8541,49.9372,49.3862,46.1696,50.63,52.8881,51.5254,52.3118,52.7285,52.5438,54.6988,54.5386,55.3539,57.0784,58.8203,61.5868,62.9939,65.5747,67.574,69.2886,71.3357,73.9579,74.7242,75.5187,75.1089,74.582,74.3894,74.3444,74.5447,74.7789,75.3804,74.2035,74.4863,74.2174,74.193,75.42,74.1402,74.1646,74.8089,74.4747,75.9973,75.1384,74.1878,74.4443,74.9275,74.7224,74.8561,74.693,75.8353,74.5733,74.4269,75.7665,74.5918,74.8829,74.8254,75.2599,74.9678,79.3404,80.8504,82.0182,82.0322,81.0368,81.3742,81.8718,81.3208,82.4715,81.6779,82.3247,81.8919,82.4235,82.3492,82.8106,82.9172,83.0532,83.3448,82.9899,83.4892,83.7382,83.9838,83.1928,84.5102,84.7175,84.3154,84.8548,85.3676,81.9878,85.0532,85.7296,85.0054,84.6196,85.3146,80.0937,80.1703,80.2248,80.288,80.3804,80.4301,80.5178,80.5528,80.6757,80.7319,80.9331,81.0151,80.9382,81.1191,81.7465,81.2769,81.8252,81.8298,81.5847,81.6739,78.9587,83.4391,81.9643,82.5875,82.835,82.5863,82.5761};
    //double X0CaloLayers[N] = {54.0318,57.0076,51.1641,51.9894,54.2876,49.952,53.8047,57.2497,55.2898,52.7514,55.8,54.8088,53.6844,54.3315,54.3281,54.3618,53.9736,55.5616,56.9004,57.4635,55.1762,56.5239,54.5526,56.377,55.1761,56.4038,56.7003,55.9741,56.9054,55.7233,57.0343,57.5817,57.8638,56.6075,58.0273,57.7163,58.1165,59.0785,59.8935,59.0176,58.7498,58.6666,58.3886,59.805,59.6712,60.6292,59.6653,60.9436,61.1729,61.1569,61.3458,61.1481,61.7262,62.1086,62.5146,63.3828,63.7797,64.8631,64.7619,65.4139,64.3393,64.7386,64.9614,65.6181,66.3964,66.1061,66.2985,66.9995,65.645,64.6098,63.1061,63.2137,63.7249,62.1394,59.6448,57.7733,57.1382,58.1615,58.6362,58.5288,56.0052,56.3635,57.0303,57.6919,57.8765,57.0406,56.0918,55.5172,54.763,54.9269,55.8817,56.6677,57.4943,58.3997,59.691,60.5222,61.3415,61.9216,61.8989,62.2833,61.2856,61.4755,62.0896,62.4886,64.0289,65.8034,67.7021,69.5661,71.0834,73.1107,75.0314,77.1124,79.0793,80.3072,82.1783,84.109,85.864,87.4737,88.7524,90.6428,91.6288,93.3277,93.5435,91.7726,91.8272,90.0656,87.8999,85.8715,81.4497,77.1833,75.3942,73.2785,71.9156,69.5589,68.0715,66.4536,64.2158,62.5124,61.4201,60.4425,59.1743,57.9759,56.9582,55.6219,54.3818,53.4652,52.348,50.246,49.5853,48.3385,47.7049,47.4402,46.9385,46.6683,46.3137,46.1782,46.2934,46.54,46.7983,47.1935,47.4539,47.6234,47.8758,48.2112,48.9842,50.2923,50.7137,51.236,51.8725,51.9032,53.3274,53.0254,54.5411,56.0885,57.9211,59.9838,62.3384,64.7229,66.7132,68.802,70.8444,72.5567,74.2853,74.1679,73.9891,74.025,74.0642,73.9976,73.9452,73.903,73.9035,73.9069,73.938,74.0291,74.1017,74.1701,74.2725,74.3826,74.4966,74.6262,74.7497,74.8589,74.9857,75.1326,75.2356,75.3198,75.3945,75.4574,75.5176,75.5841,75.6295,75.6865,75.7506,75.8005,75.8371,75.8714,75.8979,81.4955,81.6415,81.8159,81.9223,82.0275,82.1279,82.4254,82.7708,82.8566,82.8978,82.9502,83.0259,83.1228,83.2364,83.3429,83.4584,83.5643,83.6706,83.7648,83.8748,83.966,84.1007,84.156,84.0657,84.1141,84.2062,84.32,84.4591,84.5376,84.6284,84.6993,84.7651,84.8456,85.002,80.2595,80.3031,80.3048,80.3629,80.4524,80.5409,80.6004,80.6759,80.7758,80.8384,80.913,80.991,81.0944,81.183,81.26,81.3277,81.4144,81.501,81.5922,81.6799,81.754,81.8414,81.9578,82.287,82.469,82.5641};

    const double X0CaloLayers[N] = {54.1,57.1,51.25,52.1,54.4,50.05,53.9,57.4,55.4,52.9,55.95,54.95,53.85,54.45,54.45,54.5,54.1,55.7,56.7,57.3,54.95,56.3,54.65,56.5,55.25,56.5,56.8,56.05,57,55.8,57.1,57.65,57.95,56.7,58.1,57.85,58.25,59.2,60,59.15,58.85,58.75,58.5,59.9,59.75,60.7,59.75,61.05,61.3,61.3,61.45,61.25,61.85,62.25,62.65,63.5,63.9,65,64.9,65.55,64.5,64.9,65.1,65.75,66.5,66.2,66.4,67.15,65.75,64.7,63.2,63.3,63.85,62.25,59.75,57.9,57.25,58.25,58.75,58.65,56.15,56.7,58.25,59.85,60.9,61,60.8,59.95,58.75,58.45,58.95,59.3,59.9,60.75,61.9,62.65,63.35,63.8,63.7,64,62.9,63.05,63.55,63.85,65.55,67.6,69.65,71.55,72.95,75,76.9,79,81,82.2,84.05,85.95,87.75,89.35,90.65,92.5,93.5,95.15,95.35,93.6,93.65,91.85,89.65,87.65,83.25,78.95,77.2,75.05,73.65,71.3,69.85,68.2,65.95,64.25,63.1,62.15,60.85,59.65,58.55,57.25,56,55.1,53.95,51.85,51.15,49.96,49.29,49.08,48.515,48.24,47.92,47.765,47.875,48.095,48.355,48.78,49.015,49.14,48.98,48.77,49.03,49.515,49.245,49.78,50.45,50.45,51.9,51.65,53.15,54.65,56.5,58.55,60.9,63.3,65.25,67.35,69.4,71.15,72.85,72.75,72.55,72.6,72.6,72.55,72.55,72.5,72.55,72.55,72.6,72.65,72.75,72.8,72.9,73.1,73.25,73.4,73.5,73.65,73.75,73.9,74,74.1,74.2,74.35,74.4,74.45,74.5,74.7,74.95,75,75.05,75.05,75.1,75.1,75.15,75.25,75.2,75.25,75.05,75.05,75.3,75.3,75.3,75.25,75.25,75.25,75.3,75.35,75.4,75.4,75.4,75.4,75.45,75.45,75.5,75.45,75.3,75.3,75.3,75.35,75.4,75.4,75.45,75.45,75.5,75.5,75.6,70.8,70.75,70.7,70.7,70.7,70.75,70.75,70.7,70.75,70.7,70.75,70.85,71.05,71.1,71.1,71.15,71.15,71.2,71.25,71.25,71.25,71.3,71.3,71.6,71.65,71.7};


    // check regular eta look-up is setup OK
    m_binSize		= (eta[N-1]-eta[0])/(static_cast<double>(N-1));
    for (unsigned i = 0; i != m_numberBins; ++i)
    {
	double etaBin = m_binSize*(static_cast<double>(i) + 0.5);
	if (std::abs(eta[i] - etaBin) < 0.001*m_binSize) continue;
	ATH_MSG_ERROR( " have " << m_numberBins << " eta bins of width " << m_binSize 
			<< endmsg << " for bin " << i << ": expected/found eta "
			<< etaBin << "/" << eta[i]
			);
	
	return StatusCode::FAILURE;    
    }
  
    // populate radiationThickness and Trk::Surface vectors
    for (int i = 0; i != N; ++i)
    {
	// radiationThickness
	m_radiationThickness.push_back(X0CaloLayers[i]);

	// polar angle of bin centre
	double theta		= 2.* std::atan(std::exp(-eta[i]));
	double cosTheta		= std::cos(theta);
	double sinTheta		= std::sin(theta);
	double thetaNext	= 2.* std::atan(std::exp(-eta[i]-m_binSize));
	double cotThetaWidth	= 1./std::tan(thetaNext) - cosTheta/sinTheta;

	// inner surfaces
	double r	= sinTheta*(mean[i] - rms[i])*Gaudi::Units::meter;
	double z	= cosTheta*(mean[i] - rms[i])*Gaudi::Units::meter;
	m_innerBackwardSurfaces.push_back(createSurface(eta[i], r, -z, cotThetaWidth));
	m_innerForwardSurfaces.push_back(createSurface(eta[i], r, z, cotThetaWidth));
	
	// middle surfaces
	r	= sinTheta*mean[i]*Gaudi::Units::meter;
	z	= cosTheta*mean[i]*Gaudi::Units::meter;
	m_middleBackwardSurfaces.push_back(createSurface(eta[i], r, -z, cotThetaWidth));
	m_middleForwardSurfaces.push_back(createSurface(eta[i], r, z, cotThetaWidth));
	
	// outer surfaces
	r	= sinTheta*(mean[i] + rms[i])*Gaudi::Units::meter;
	z	= cosTheta*(mean[i] + rms[i])*Gaudi::Units::meter;
	m_outerBackwardSurfaces.push_back(createSurface(eta[i], r, -z, cotThetaWidth));
	m_outerForwardSurfaces.push_back(createSurface(eta[i], r, z, cotThetaWidth));
    }
  
    // fill Calo Layers with R,Z centre, surface, etc
    double innerR[N]	= {0.};	
    double innerZ[N]	= {0.};
    // double middleR[N]	= {0.};	
    // double middleZ[N]	= {0.};
    double outerR[N]	= {0.};
    double outerZ[N]	= {0.};	
    for (int i = 0; i != N; ++i)
    {
	double theta	= 2.* std::atan(std::exp(-eta[i]));
	double cosTheta	= std::cos(theta);
	double sinTheta	= std::sin(theta);
	innerR[i]	= sinTheta*(mean[i] - rms[i])*Gaudi::Units::meter;
	innerZ[i]	= cosTheta*(mean[i] - rms[i])*Gaudi::Units::meter;
	// middleR[i]	= sinTheta*mean[i]*Gaudi::Units::meter;
	// middleZ[i]	= cosTheta*mean[i]*Gaudi::Units::meter;
	outerR[i]	= sinTheta*(mean[i] + rms[i])*Gaudi::Units::meter;
	outerZ[i]	= cosTheta*(mean[i] + rms[i])*Gaudi::Units::meter;
	bool barrel 	= true;
	if (eta[i] > 1.4) barrel = false;

	// outer layer
	m_caloOuterLayers.push_back(new CaloLayer(outerSurface(-eta[i]),
						  i,
						  outerR[i],
						  -outerZ[i],
						  X0CaloLayers[i],
						  barrel));
	m_caloOuterLayers.push_back(new CaloLayer(outerSurface(eta[i]),
						  i+N,
						  outerR[i],
						  outerZ[i],
						  X0CaloLayers[i],
						  barrel));

	// same thing for inner layer
	m_caloInnerLayers.push_back(new CaloLayer(innerSurface(-eta[i]),
						  i,
						  innerR[i],
						  -innerZ[i],
						  X0CaloLayers[i],
						  barrel));
	m_caloInnerLayers.push_back(new CaloLayer(innerSurface(eta[i]),
						  i+N,
						  innerR[i],
						  innerZ[i],
						  X0CaloLayers[i],
						  barrel));

	// check
	const Trk::Surface* surfM = innerSurface(-eta[i]);
	if (std::abs(surfM->center().z() + innerZ[i]) > 0.001)
	{
	    ATH_MSG_ERROR( " wrong surface " << i << " at eta " << eta[i] << endmsg
			    << ": expected/found z " << -innerZ[i] << "/" << surfM->center().z()
			    );
	    return StatusCode::FAILURE;    
	}
	const Trk::Surface* surfP = innerSurface(eta[i]);
	if (std::abs(surfP->center().z() - innerZ[i]) > 0.001)
	{
	    ATH_MSG_ERROR( " wrong surface " << i << " at eta " << eta[i] << endmsg
			    << ": expected/found z " << innerZ[i] << "/" << surfP->center().z()
			    );
	    return StatusCode::FAILURE;    
	}
    }

    return StatusCode::SUCCESS;
}
 
}	// end of namespace
