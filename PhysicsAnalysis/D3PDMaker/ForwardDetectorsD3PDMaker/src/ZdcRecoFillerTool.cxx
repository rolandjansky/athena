/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ZdcRecoFillerTool.cxx 538890 2013-02-25 04:49:38Z leite $
/**
 * @file ForwardDetectorsD3PDMaker/ZdcRecoFillerTool.h
 * @author Marco Leite <leite@cern.ch>
 * @date Nov 2011
 * @brief Block filler tool for ZDC reconstruected information.
 */
#include <algorithm>
#include <numeric>
// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"
#include "Identifier/Identifier.h"

// ZDC include(s):
#include "ZdcIdentifier/ZdcID.h"
#include "ZdcConditions/ZdcCablingService.h"
#include "ZdcEvent/ZdcDigits.h"

// Local include(s):
#include "ZdcRecoFillerTool.h"

namespace D3PD {

	//=================================================================================================
   /**
    * @brief Standard Gaudi tool constructor.
    * @param type The name of the tool type.
    * @param name The tool name.
    * @param parent The tool's Gaudi parent.
    */
   ZdcRecoFillerTool::ZdcRecoFillerTool( const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent )
      : ZdcDigitsCollectionFillerBase( type, name, parent ) {

      declareProperty( "CalibrateEnergy",      m_calibrateEnergy = false );
      declareProperty( "CalibrateTime",        m_calibrateTime = false );
      declareProperty( "NoiseThreshold",       m_threshold = 5 );
      declareProperty( "ReconstructionMethod", m_recoMethod = "DirectSample" );

      book().ignore(); // Avoid coverity warnings
   }

   /**
    * Initialize: informs the type container to D3PDMaker
    *             retrieves the mapping database
    *             prepares the coefficients for the calibration
    *
    * @return Success or failure (will stop the processing)
    */
   StatusCode ZdcRecoFillerTool::initialize() {

      // Initialize the base class:
      CHECK( ZdcDigitsCollectionFillerBase::initialize() );


      //Get mapping from detector store
      CHECK( detStore()->retrieve( m_zdcID ) );

      //Prepare the calibration objects
      fillCalibrationConstants();

      ATH_MSG_INFO( "ZdcRecoFillerTool---> Initialized" );

      return StatusCode::SUCCESS;
   }
   //=================================================================================================
   //This is necessary to avoid a memeory leak.
   //Probably it should be placed in all filler tools
   //(it is safe as it checks for it before deleting it in deleteInstance)
   StatusCode ZdcRecoFillerTool::finalize() {
	   ZdcCablingService::deleteInstance();
	   ATH_MSG_INFO( "ZdcRecoFillerTool---> Finalized" );
	   return StatusCode::SUCCESS;
   }


   //=================================================================================================
   StatusCode ZdcRecoFillerTool::book() {

      ATH_MSG_DEBUG( "ZdcRecoFillerTool---> Booking Histograms" );


      CHECK( addVariable( "ESumA", m_energySumA,
                          "Sum of modules energy in side A (auto gain)" ) );
      CHECK( addVariable( "ESumC", m_energySumC,
                          "Sum of modules energy in side C (auto gain)" ) );

      CHECK( addVariable( "EA", m_energyA,
                          "Modules energy in side A (auto gain)" ) );
      CHECK( addVariable( "EC", m_energyC,
                          "Modules energy in side C (auto gain)" ) );

      CHECK( addVariable( "qA", m_qualityA,
                          "Flag for the reason of the auto gain selection in side A" ) );
      CHECK( addVariable( "qC", m_qualityC,
                          "Flag for the reason of the auto gain selection in side C" ) );



      CHECK( addVariable( "ESumA_LG", m_energySumA_Lg,
                          "Sum of modules energy in side A LG" ) );
      CHECK( addVariable( "ESumA_HG", m_energySumA_Hg,
                          "Sum of modules energy in side A HG" ) );
      CHECK( addVariable( "ESumC_LG", m_energySumC_Lg,
                          "Sum of modules energy in side C LG" ) );
      CHECK( addVariable( "ESumC_HG", m_energySumC_Hg,
                          "Sum of modules energy in side C HG" ) );

      return StatusCode::SUCCESS;
   }
   //=================================================================================================

   //=================================================================================================
   /**
    *
    * @param p pointer  the collection
    * @return always SUCCESS
    */
   StatusCode ZdcRecoFillerTool::fill( const ZdcDigitsCollection& zdcCol ) {

	  std::vector< std::vector<int>  > zdcA_Lg(4);
	  std::vector< std::vector<int>  > zdcA_Hg(4);

	  std::vector< std::vector<int>  > zdcC_Lg(4);
	  std::vector< std::vector<int>  > zdcC_Hg(4);


	  int module = 0;
	  int side = 0;

	  m_energyA->clear();
	  m_energyC->clear();

	  m_qualityA->clear();
	  m_qualityC->clear();

	  m_energyA->resize(4);
	  m_energyC->resize(4);

	  m_qualityA->resize(4);
	  m_qualityC->resize(4);


	  *m_energySumA = 0;
	  *m_energySumC = 0;

      *m_energySumA_Lg = 0;
      *m_energySumA_Hg = 0;

      *m_energySumC_Lg = 0;
      *m_energySumC_Hg = 0;

      //Try to get digits from here

      ZdcDigitsCollection::const_iterator itr     = zdcCol.begin();
      ZdcDigitsCollection::const_iterator itrLast = zdcCol.end();
      for( ; itr !=itrLast; ++itr ) {

         const ZdcDigits* p = ( *itr );
         const Identifier id = p->identify();


         if( ( m_zdcID->type(id) == 0 ) && ( m_zdcID->channel(id) == 0) ) {
        	 module = m_zdcID->module(id);
        	 side   = m_zdcID->side(id);

        	 if (side == 1) {
        		 zdcA_Lg[module] = p->get_digits_gain0_delay0();
        		 zdcA_Hg[module] = p->get_digits_gain1_delay0();
        	 }
        	 else if (side == -1) {
        		 zdcC_Lg[module] = p->get_digits_gain0_delay0();
        		 zdcC_Hg[module] = p->get_digits_gain1_delay0();
        	 }
         }
      }

      // Get the energy sum in auto-gain

      getEnergySum(zdcA_Lg, zdcA_Hg, 1);
      getEnergySum(zdcC_Lg, zdcC_Hg,-1);


      return StatusCode::SUCCESS;
}

   //=================================================================================================
   /*
    * Here we calculate the energy sum based on selection of  the best gain and
    * calibrated in the low gain scale. This takes into account only the electronic
    * gain variation. The module sampling weight is *NOT* included (and requires
    * run by run and maybe even lumiblock by lumiblock correction dur to response change
    * of the modules during the run
    */
   void ZdcRecoFillerTool::getEnergySum(const std::vector<std::vector<int> > & zdc_Lg,
		   	   	   	   	   	   	   	   	const std::vector<std::vector<int> > & zdc_Hg,
		   	   	   	   	   	   	   	   	const int & side) {

	   int module = 0;

	   int ped_Hg = 0;
	   int ped_Lg = 0;

	   int pp = 0;

	   std::vector<int> wfm_Lg;
	   std::vector<int> wfm_Hg;

	   std::vector<int>  pico_Lg(4,0);
	   std::vector<int>  pico_Hg(4,0);

	   std::vector<int>   qual(4,0);
	   std::vector<float> energy(4,0);

	   std::vector<int>   satLimit(4,0);

	   if (side ==  1) satLimit = m_saturationA;
	   if (side == -1) satLimit = m_saturationC;

	   for (module=0; module<4; module++) {

		   wfm_Lg = zdc_Lg[module];
		   wfm_Hg = zdc_Hg[module];

		   if (wfm_Lg.size() == 0) wfm_Lg.resize(7);
		   if (wfm_Hg.size() == 0) wfm_Hg.resize(7);

		   ped_Lg = wfm_Lg[0];
		   ped_Hg = wfm_Hg[0];

		   pico_Lg[module] = *(std::max_element(wfm_Lg.begin(), wfm_Lg.end()) ) - ped_Lg;
		   pico_Hg[module] = *(std::max_element(wfm_Hg.begin(), wfm_Hg.end()) ) - ped_Hg;

		   //Condition to discard High Gain
		   qual[module] = 0;
		   if (pico_Hg[module] > satLimit[module]) {
			   qual[module] =  qual[module] | 0x1;
		   }
		   if (ped_Hg < m_pedMin) {
			   qual[module] =  qual[module] | 0x2;
		   }
		   if (ped_Hg > m_pedMax) {
			   qual[module] = qual[module]  | 0x4;
		   }
		   //Select which gain to use
		   if (qual[module] != 0 ) {
			   energy[module] = pico_Lg[module];
		   }
		   else {
			   pp = pico_Hg[module];
			   energy[module] = convertToLowGain(pp, module, side);
		   }

	   }
	   if (side == 1) {
		   //The separated High and Low gain sums
		   *m_energySumA_Lg = std::accumulate(pico_Lg.begin(), pico_Lg.end(), 0);
		   *m_energySumA_Hg = std::accumulate(pico_Hg.begin(), pico_Hg.end(), 0);

		   *m_energyA = energy;
		   *m_qualityA = qual;
		   *m_energySumA =  std::accumulate(energy.begin(), energy.end(), 0.0) ;
	   }
	   if (side == -1) {
		   //The separated High and Low gain sums
		   *m_energySumC_Lg = std::accumulate(pico_Lg.begin(), pico_Lg.end(), 0);
		   *m_energySumC_Hg = std::accumulate(pico_Hg.begin(), pico_Hg.end(), 0);

		   *m_energyC = energy;
		   *m_qualityC = qual;
		   *m_energySumC =  std::accumulate(energy.begin(), energy.end(), 0.0) ;
	   }

     }
   //=================================================================================================

   float ZdcRecoFillerTool::convertToLowGain(const int & v, const int & module, const int & side) {

	   float y = 0;
	   //Side A
	   if (side == 1) {
		   y = v * m_calib_p1A[module] + m_calib_p0A[module];
	   }
	   if (side == -1) {
		   y = v * m_calib_p1C[module] + m_calib_p0C[module];
	   }
	   return y;
   }
   /**
    * This will prepare the vectors holding the coefficients for calibration.
    * Sometime  this should move to a database.
    */
   void ZdcRecoFillerTool::fillCalibrationConstants() {

      /**
       * Vector of constants coeficients for calibration
       * this is for 2013 analysis
       *
       * this considers *ONLY* the electronic gain
       * to convert the high gain to the low gain scale (y=p1*x + p0)
       */


	  static const float p1A[] = {0.10198,  0.11234,  0.11218,   0.11989};
	  static const float p1C[] = {0.10198,  0.12038,  0.09796,   0.11390};
	  //static const float p1C[] = {0.10198,  0.13304,  0.09796,   0.11390};

	  static const float p0A[] = {-0.07314, 0.61453,  0.91681,  0.39103};
	  static const float p0C[] = { 0.0,    -0.55202, -0.86575, -0.09950};
	  //static const float p0C[] = { 0.0,    -1.58776, -0.86575, -0.09950};

	  static const int satA[] = { 650, 650, 650, 700};
	  static const int satC[] = { 700, 480, 850, 650};

	  //Alternative constants
      //static const float p1A[] = {0.1104,  0.1079,  0.1080,   0.1111};
      //static const float p1C[] = {0.1104,  0.1077,  0.0995,   0.1143};

      //static const float p0A[] = {0.5595,  0.2165,  0.11322, -0.5599};
      //static const float p0C[] = {0.5595,  0.2332, -0.5520,   0.5571};

      m_calib_p1A.assign( p1A, p1A + 4 );
      m_calib_p1C.assign( p1C, p1C + 4 );

      m_calib_p0A.assign( p0A, p0A + 4 );
      m_calib_p0C.assign( p0C, p0C + 4 );

      m_saturationA.assign(satA, satA+4);
      m_saturationC.assign(satC, satC+4);

      m_pedMin = 10;
      m_pedMax = 160;

      return;
   }
   //=================================================================================================

} // namespace D3PD
