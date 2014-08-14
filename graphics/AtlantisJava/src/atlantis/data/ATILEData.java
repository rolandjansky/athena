package atlantis.data;

import atlantis.event.*;
import java.io.IOException;
import java.io.InputStream;
import java.io.StringReader;
import java.io.FileNotFoundException;

import com.Ostermiller.util.CSVParser;

import atlantis.geometry.ACalorimeterDetector;
import atlantis.globals.AGlobals;
import atlantis.graphics.ACoord;
import atlantis.output.ALogInterface;
import atlantis.output.AOutput;
import atlantis.projection.AProjection2D;
import atlantis.projection.AProjectionFR;
import atlantis.projection.AProjectionRZ;
import atlantis.projection.AProjectionYX;
import atlantis.utils.AAtlantisException;
import atlantis.utils.AHashMap;
import atlantis.utils.AIdHelper;
import atlantis.utils.AMath;
import atlantis.utils.AUtilities;
import atlantis.utils.ALogger;


/**
 * The TILE hadronic calorimeter.
 *
 * @author Eric Jansen
 */
public class ATILEData extends ACalorimeterData
{
    private static ALogger logger = ALogger.getLogger(ATILEData.class);
    
    // data for real pulse shapes plots
    private float[] pmt1Energy = null;
    private float[] pmt2Energy = null;
    private float[] pmt1Chi2 = null;
    private float[] pmt2Chi2 = null;
    private float[] pmt1Time = null;
    private float[] pmt2Time = null;
    private float[] pmt1RawTime = null;
    private float[] pmt2RawTime = null;
    private float[] pmt1RawAmplitude = null;
    private float[] pmt2RawAmplitude = null;
    private int[][] adcCounts1 = null;
    private int[][] adcCounts2 = null;
    private int[] pmt1Gain = null;
    private int[] pmt2Gain = null;
    private float[] pmt1Pedestal = null;
    private float[] pmt2Pedestal = null;
    private int[] pmt1Number = null;
    private int[] pmt2Number = null;    
    private int[] pmt1ADCStatus = null;
    private int[] pmt2ADCStatus = null;
    private static boolean pulseShapesDataAvailable = false;
    private static final String LOOKUP_TABLE_FILE =
    		AGlobals.instance().getHomeDirectory() + "configuration" + 
        		System.getProperty("file.separator") +
        		"rpsplt_tile.csv";
    // number of lookup table values for real pulse shapes plots calculation
    private static final short NUMBER_OF_LOOKUP_VALUES = 401;


    ATILEData(AHashMap p, AEvent e)
    {
        super(p,e);


        for (int i = 0; i < numData; i++)
        {
            try
            {
                side[i] = (byte) AIdHelper.tileSide(id[i]);
                etaIndex[i] = (short) AIdHelper.tileTower(id[i]);
                phiIndex[i] = (short) AIdHelper.tileModule(id[i]);
                sampling[i] = AIdHelper.tileSampling(id[i]);
            }
            catch (AAtlantisException ex)
            {
                System.out.println("Problem decoding ID " + id[i] + " in " +
                                   CALORIMETER_NAME + ": " + ex.getMessage());
                side[i] = 0;
                etaIndex[i] = -1;
                phiIndex[i] = -1;
                sampling[i] = -1;
            }
        }

        makeHitToGeometryMapping();
        for (int i = 0; i < et.length; ++i)
        {
            et[i] = Math.abs(energy[i] / (float) Math.cosh(eta[i]));
        }

        // Collect some constants needed for the histograms.
        for (int i = 0; i < ACalorimeterDetector.count(); i++)
        {
            if (ACalorimeterDetector.get(i).getName().indexOf(CALORIMETER_NAME) >= 0)
            {
                if (innerR == 0.0 || ACalorimeterDetector.get(i).getRMin() < innerR)
                {
                    innerR = ACalorimeterDetector.get(i).getRMin();
                }
                if (phiGranularity == 0.0 || ACalorimeterDetector.get(i).getDeltaPhi() < phiGranularity)
                {
                    phiGranularity = ACalorimeterDetector.get(i).getDeltaPhi();
                }
                if (etaGranularity == 0.0 || ACalorimeterDetector.get(i).getDeltaEta() < etaGranularity)
                {
                    etaGranularity = ACalorimeterDetector.get(i).getDeltaEta();
                }
                if (outerEta == 0.0 || ACalorimeterDetector.get(i).getEtaMax() > outerEta)
                {
                    outerEta = ACalorimeterDetector.get(i).getEtaMax();
                }
            }

            if (outerR == 0.0 || ACalorimeterDetector.get(i).getRMax() > outerR)
            {
                outerR = ACalorimeterDetector.get(i).getRMax();
            }
            if (outerZ == 0.0 || ACalorimeterDetector.get(i).getZMax() > outerZ)
            {
                outerZ = ACalorimeterDetector.get(i).getZMax();
            }
        }

        // Add a little bit of extra margin to prevent binning errors due to
        // rounding of numbers.
        outerEta += etaGranularity;

        readPulseShapePlotData(p);
        
        pmt1ADCStatus = (p.get("pmt1ADCStatus") != null) ? p.getIntArray("pmt1ADCStatus") : null;
        pmt2ADCStatus = (p.get("pmt2ADCStatus") != null) ? p.getIntArray("pmt2ADCStatus") : null;
        

    } // ATILEData() --------------------------------------------------------



    /**
     * readLookupTableFile() reads in comma separated values (CSV) file
     * with TILE real pulse shapes plots time and amplitude lookup values
     * @throws AAtlantisException
     */
    protected static void readLookupTableFile() throws AAtlantisException
    {
        try
        {
            InputStream is = AUtilities.getFileAsStream(LOOKUP_TABLE_FILE);
            CSVParser parser = new CSVParser(is);
            parser.setCommentStart("#");
            String arrayName =  null;

            while((arrayName = parser.nextValue()) != null)
            {
                String valueArray = parser.nextValue(); // shall now contain all values
                CSVParser parserArray = new CSVParser(new StringReader(valueArray));
                String[][] s = parserArray.getAllValues();
                if("TILE_AMPLITUDE".equals(arrayName))
                {
                    ACalorimeterRPSPLT.TILE_AMPLITUDE =
                        getLookupTableArray(s, NUMBER_OF_LOOKUP_VALUES);
                }
                else if("TILE_TIME".equals(arrayName))
                {
                    ACalorimeterRPSPLT.TILE_TIME =
                        getLookupTableArray(s, NUMBER_OF_LOOKUP_VALUES);
                }
            }
        }
        catch(FileNotFoundException e)
        {
            throw new AAtlantisException("could not find file: " +
                                         LOOKUP_TABLE_FILE);
        }
        catch(IOException e)
        {
            throw new AAtlantisException("exception while reading file: " +
                                         LOOKUP_TABLE_FILE);
        }
        catch(AAtlantisException e)
        {
            throw e;
        }

    } // readLookupTableFile() ----------------------------------------------



    private void readPulseShapePlotData(AHashMap p)
    {
        pmt1Energy = (p.get("pmt1Energy") != null) ? p.getFloatArray("pmt1Energy") : null;
        pmt2Energy = (p.get("pmt2Energy") != null) ? p.getFloatArray("pmt2Energy") : null;
        pmt1Chi2 = (p.get("pmt1Chi2") != null) ? p.getFloatArray("pmt1Chi2") : null;
        pmt2Chi2 = (p.get("pmt2Chi2") != null) ? p.getFloatArray("pmt2Chi2") : null;
        pmt1Time = (p.get("pmt1Time") != null) ? p.getFloatArray("pmt1Time") : null;
        pmt2Time = (p.get("pmt2Time") != null) ? p.getFloatArray("pmt2Time") : null;
        
        pmt1Gain = (p.get("pmt1Gain") != null) ? p.getIntArray("pmt1Gain") : null;
        pmt2Gain = (p.get("pmt2Gain") != null) ? p.getIntArray("pmt2Gain") : null;

        pmt1Pedestal = (p.get("pmt1Pedestal") != null) ? p.getFloatArray("pmt1Pedestal") : null;
        pmt2Pedestal = (p.get("pmt2Pedestal") != null) ? p.getFloatArray("pmt2Pedestal") : null;

        pmt1Number = (p.get("pmt1Number") != null) ? p.getIntArray("pmt1Number") : null;
        pmt2Number = (p.get("pmt2Number") != null) ? p.getIntArray("pmt2Number") : null;

        int[] adc1 = (p.get("adcCounts1") != null) ? p.getIntArray("adcCounts1") : null;
        int[] adc2 = (p.get("adcCounts2") != null) ? p.getIntArray("adcCounts2") : null;

        // by zdenek (2008-09-15):        
        // raw time: later additions to tilecal pulse shapes, doens't necessarily
        // have to be in the event files, will not be in the old ones
        pmt1RawTime = (p.get("pmt1RawTime") != null) ? p.getFloatArray("pmt1RawTime") : null;
        pmt2RawTime = (p.get("pmt2RawTime") != null) ? p.getFloatArray("pmt2RawTime") : null;
 
        // by zdenek (2008-11-24):
        // pmt1RawAmplitude, pmt2RawAmplitude now used to calculate pulse shape
        // this is made now obligatory, without this data, the pulse shapes will
        // not be available
        pmt1RawAmplitude = (p.get("pmt1RawAmplitude") != null) ? p.getFloatArray("pmt1RawAmplitude") : null;
        pmt2RawAmplitude = (p.get("pmt2RawAmplitude") != null) ? p.getFloatArray("pmt2RawAmplitude") : null;
        
        // read in ADCCounts
        if(adc1 != null && adc2 != null)
        {
            adcCounts1 = new int[numData][0];
            adcCounts2 = new int[numData][0];
            // amount of numbers associated with each cells (i.e. with each data item)
            int multiple = adc1.length / numData;
            int num = 0;
            for (int i = 0; i < numData; i++)
            {
                adcCounts1[i] = new int[multiple];
                adcCounts2[i] = new int[multiple];
                for (int j = 0; j < multiple; j++)
                {
                    adcCounts1[i][j] = adc1[num]; // fill in array for each cell
                    adcCounts2[i][j] = adc2[num]; // fill in array for each cell
                    num++;
                }
            }
        }

        pulseShapesDataAvailable = false;
        if(adcCounts1 != null && adcCounts2 != null && adcCounts1.length != 0 &&
                adcCounts2.length != 0 && adcCounts1[0].length != 0 &&
                adcCounts2[0].length != 0 && pmt1Number != null &&
                pmt2Number != null && pmt1Energy != null && pmt2Energy != null &&
                pmt1Time != null && pmt2Time != null && pmt1Pedestal != null &&
                pmt2Pedestal != null && pmt1Gain != null && pmt2Gain != null &&
                pmt1RawAmplitude != null && pmt2RawAmplitude != null)
        {
            pulseShapesDataAvailable = true;
  
            logger.debug(CALORIMETER_NAME +
                    ": data for real pulse shape plots available");

            if(ACalorimeterRPSPLT.areTileLookupTablesInitialized())
            {
                logger.debug(CALORIMETER_NAME +
                        ": lookup tables have already been read in");
            }
            else
            {
                
                logger.debug(CALORIMETER_NAME +
                        ": lookup table values have not been read in yet\n" +
                        "  trying to read file: " + LOOKUP_TABLE_FILE);

                try
                {
                    readLookupTableFile();
                    logger.debug(CALORIMETER_NAME +
                            ": values from " + LOOKUP_TABLE_FILE +
                            " successfully read in");
                }
                catch(AAtlantisException ex)
                {
                    logger.error(CALORIMETER_NAME +
                            ": reading " + LOOKUP_TABLE_FILE +
                            " failed, real pulse shapes plots will not " +
                            "be available, reason: " + ex.getMessage(), ex);
                    pulseShapesDataAvailable = false;
                }
            }
        }

    } // readPulseShapePlotData() -------------------------------------------



    protected void applyCuts()
    {
        super.applyCuts();
        cut("CutsCalo", "TileET", "TileET", et);
        cut("CutsCalo", "TileEnergyBottom", "TileEnergyBottom", energy);
        cut("CutsCalo", "TileEnergyUp", "TileEnergyUp", energy);
        
        // following cut is based on pmt1ADCStatus, resp. pmt2ADCStatus
        if(pmt1ADCStatus != null && pmt2ADCStatus != null)
        {
        	cut("CutsCalo", "TilePMTADC", "TilePMTADC", pmt1ADCStatus);
        	cut("CutsCalo", "TilePMTADC", "TilePMTADC", pmt2ADCStatus);
        }
    }

    
    
    /**
     * Returns the name of the parameter group.
     * @return String parameter group
     */
    public String getParameterGroup()
    {
        return "TILE";
    }

    /**
     * Returns the name of the datatype.
     * @return String datatype
     */
    public String getName()
    {
        return "TILE";
    }

    /**
     * Returns the displayed name of datatype
     * @return String screen name
     */
    public String getNameScreenName()
    {
        return "TILE";
    }

    /**
     * Returns the type of calorimeter (ECAL/HCAL) for a hit.
     * @param index int hit index
     * @return String calorimeter type
     */
    public String getCalorimeterType(int index)
    {
        return "HCAL";
    }

    @Override //ACalorimeterData
    //Gives the hit time for this Tile cell based on the PMT times
    protected double getTime(int hit)
    {
        if (pmt1Time == null) return 0.0;
        else if (pmt2Time == null) return 0.0;
        else {
        if (pmt1Time[hit]!=0.0 && pmt2Time[hit]!=0.0){
            return (pmt1Time[hit]+pmt2Time[hit])/2.;
        }
        if (pmt1Time[hit]==0.0 && pmt2Time[hit]!=0.0){
            return pmt2Time[hit];
        }
        if (pmt1Time[hit]!=0.0 && pmt2Time[hit]==0.0){
            return pmt1Time[hit];
        }
        }
        return 0.0;
    }

    /**
     * Returns calo hit info, most of the parameters are taken from from
     * (mother) ACalorimeterData class.
     * @param index int
     * @return String
     */
    public String getHitInfo(int index)
    {
    	int simpleOutput = AGlobals.instance().getSimpleOutput();
    	if(simpleOutput>0){
    		String output = getNameScreenName()+" index: " + index;
    		if(simpleOutput==1 || simpleOutput==3) 
    			output+= "\n ET="+String.format("%.3f",et[index])+" GeV\n "+
    			AMath.ETA+" = "+String.format("%.3f",eta[index])+"\n "+
    			AMath.PHI+" = "+String.format("%.3f",Math.toDegrees(phi[index]))+AMath.DEGREES;
        	if(simpleOutput==2 || simpleOutput==3) 
        		output+= "\n Ex="+String.format("%.3f",et[index]*Math.cos(phi[index]))+" GeV "+
        		"\n Ey="+String.format("%.3f",et[index]*Math.sin(phi[index]))+" GeV "+
        		"\n Ez="+String.format("%.3f",et[index]*Math.sinh(eta[index]))+" GeV ";
        	return output;
    	}
    	
        String t = "";
        String pmt1Info = "";
        String pmt2Info = "";

        String decodedId = AIdHelper.getDecodedTileIndentifier(id[index]);
        t = decodedId + "  " + super.getHitInfoNoDecode(index);

        
        pmt1Info += "\n PMT1 ADC status = ";
        pmt1Info += (pmt1ADCStatus != null) ? Integer.toString(pmt1ADCStatus[index]) : "n/a";
        
        pmt1Info += "\n PMT1 energy = ";
        pmt1Info += (pmt1Energy != null) ? Float.toString(pmt1Energy[index]) +
                                           " GeV" : "n/a";
        pmt1Info += "\n PMT1 chi2 = ";
        pmt1Info += (pmt1Chi2 != null) ? Float.toString(pmt1Chi2[index]) : "n/a";
        pmt1Info += "\n PMT1 time = ";
        pmt1Info += (pmt1Time != null) ? Float.toString(pmt1Time[index]) +
                                         " ns" : "n/a";
        pmt1Info += "\n PMT1 gain = ";
        pmt1Info += (pmt1Gain != null) ? Integer.toString(pmt1Gain[index]) : "n/a";
        pmt1Info += "\n PMT1 pedestal = ";
        pmt1Info += (pmt1Pedestal != null) ? Float.toString(pmt1Pedestal[index]) +
                                             " ADC counts" : "n/a";


        pmt2Info += "\n PMT2 ADC status = ";
        pmt2Info += (pmt2ADCStatus != null) ? Integer.toString(pmt2ADCStatus[index]) : "n/a";
        
        pmt2Info += "\n PMT2 energy = ";
        pmt2Info += (pmt2Energy != null) ? Float.toString(pmt2Energy[index]) +
                                           " GeV" : "n/a";
        pmt2Info += "\n PMT2 chi2 = ";
        pmt2Info += (pmt2Chi2 != null) ? Float.toString(pmt2Chi2[index]) : "n/a";
        pmt2Info += "\n PMT2 time = ";
        pmt2Info += (pmt2Time != null) ? Float.toString(pmt2Time[index]) +
                                         " ns" : "n/a";
        pmt2Info += "\n PMT2 gain = ";
        pmt2Info += (pmt2Gain != null) ? Integer.toString(pmt2Gain[index]) : "n/a";
        pmt2Info += "\n PMT2 pedestal = ";
        pmt2Info += (pmt2Pedestal != null) ? Float.toString(pmt2Pedestal[index]) +
                                             " ADC counts" : "n/a";

        pmt2Info += pulseShapesDataAvailable ? "" :
                    "\n data for real pulse shapes plot n/a";

        pmt2Info += "\n calc time = "+getTime(index);

        return t + pmt1Info + pmt2Info;

    } // getHitInfo() -------------------------------------------------------



    // calculate real pulse shapes values based on the values in the lookup tables
    private double getPhysicsPulseShape(double xTime, double pmtTime,
                                        double pmtPedestal, double pmtEnergy,
                                        float[] amplitude, float[] time)
                                        throws AAtlantisException
    {
        double tdiv = 0.0;
        int lookup = 0;
        double localTime = 0.0;
        double xpulse = 0.0;

        // need to get "multiple" parameter ( = number of values in adcCounts
        // for each cell - adcCounts1[0].length), shall be the same for all the
        // cells - taking the length of the first array (for the first cell)
        // [currently (2007-05-23) is this multiple 9 but to change in real data]
        // no NullPointer or ArrayIndexBound checks are necessary here as this
        // method shall only be called when the relevant data is available
        double centerOfSamples = (adcCounts1[0].length + 1.0) / 2.0;

        tdiv = time[1] - time[0];
        localTime = (xTime - centerOfSamples) * 25.0 - pmtTime;
        lookup = (int) ((localTime - time[0]) / tdiv);

        if(lookup < 0)
        {
            lookup = 0;
        }
        if(lookup >= NUMBER_OF_LOOKUP_VALUES - 1)
        {
            lookup = NUMBER_OF_LOOKUP_VALUES - 2; // -1 was off by 1
        }

        try
        {
            if(lookup == 0 || lookup == NUMBER_OF_LOOKUP_VALUES - 2)
            {
                xpulse = amplitude[lookup];
            }
            else
            {
                xpulse = amplitude[lookup] + ((amplitude[lookup + 1] -
                         amplitude[lookup]) / tdiv) * (localTime - time[lookup]);
            }
        }
        catch(ArrayIndexOutOfBoundsException ex)
        {
            String m = "ATILEData.getPhysicsPulseShape():\n" +
                       "  lookup index out of bound: lookup = " + lookup;
            throw new AAtlantisException(m);
        }
        return (xpulse * pmtEnergy) + pmtPedestal;

    } // getPhysicsPulseShape() ---------------------------------------------



//    /**
//     * Call util class which plots cell pulse shapes provided that
//     * all real pulse shapes data is available
//     * Functions calculates values of real pulse shape calculated in the method
//     * getPhysicsPulseShape().
//     * This method is called from pick interaction.
//     *
//     * @param index int
//     */
    /*public void plotPulseShapes(int index)
    {

        if(pulseShapesDataAvailable)
        {
            String title = getPulseTitleString(index);

            // two channels for a TILE cell (two arrays of ADC counts)
            int[][] adcCounts = new int[][] { adcCounts1[index], adcCounts2[index] };
            
            if(super.checkADCCountsAvailability(adcCounts))
            {
                // adc counts are available
                logger.debug(CALORIMETER_NAME + " adc counts (digits) are " +
                             "available for pulse shapes plots for this cell.");
            }
            else
            {
                AOutput.append("\nADC counts are not available for this cell, " +
                               "can't plot pulse shapes.", ALogPane.WARNING);
                return;
            }
            
            
            String cap1 = "Real pulse shape " + "PMT " + pmt1Number[index] +
                          " gain " + pmt1Gain[index];
            String cap2 = "Real pulse shape " + "PMT " + pmt2Number[index] +
                          " gain " + pmt2Gain[index];
            String[] subTitle = new String[] { cap1, cap2 };
            
            // by zdenek (2008-11-24):
            // energy calculations (lowGainFactor, highGainFactor) and related
            // magic constants removed - now using pmt1RawAmplitude, pmt2RawAmplitude
            // instead of pmt[1,2]Energy, resp. former local variables
            // energy1, energy2 when calling getPhysicsPulseShape() further down
            
            // 1 .. 9 range (9 values of ADC counts, 9 samples (starts from 1!)),
            // need to get 401 values within this range. number of samples will
            // likely decrease in real data from 9 to 7
            double step = (adcCounts1[index].length - 1) / (float) NUMBER_OF_LOOKUP_VALUES;
            double[][] realPulse = new double[2][NUMBER_OF_LOOKUP_VALUES]; // 2 channels
            double[][] realPulseForRawTime = null;
            double d = 1.0;
            
        	// if pmt1RawTime and pmt2RawTime are present, use those for
        	// calculating data for real pulse shape plot based on these time values
        	boolean showPlotForRawTime = false;
        	if(pmt1RawTime != null && pmt2RawTime != null)
        	{
        		logger.debug("TILE pmt[1,2]RawTime are present, showing another plot.");
        		showPlotForRawTime = true;
        		realPulseForRawTime = new double[2][NUMBER_OF_LOOKUP_VALUES]; // 2 channels
        	}
        	
        	
        	// by zdenek 2008-11-25
        	// DPD (slimed version of ESD) sometimes don't have pmt[1,2]Pedestal
        	// for a cell (the tag exists, values are there, but are 0). check
        	// if pedestal is 0 and if so, take as pedestal value the first
        	// adc count digit
        	float pmt1PedestalLocal = pmt1Pedestal[index]; 
        	float pmt2PedestalLocal = pmt2Pedestal[index];
        	if(pmt1PedestalLocal == 0.0f && pmt2PedestalLocal == 0.0f)
        	{
        		logger.debug("TILE pmt[1,2]Pedestal are not available (i.e. " +
        				     "are 0.0f), using minimum of first and last " +
                                             "adcCount digit as pedestal.");
                        float last1 = adcCounts1[index][adcCounts1[index].length - 1];
                        float last2 = adcCounts2[index][adcCounts2[index].length - 1];
        		pmt1PedestalLocal = Math.min(adcCounts1[index][0], last1);
        		pmt2PedestalLocal = Math.min(adcCounts2[index][0], last2);
        	}
            
            
            try
            {
                for(int i = 0; i < NUMBER_OF_LOOKUP_VALUES; i++)
                {
                    d += step;
                    realPulse[0][i] =
                        getPhysicsPulseShape(d, pmt1Time[index], pmt1PedestalLocal,
                        		             pmt1RawAmplitude[index],
                                             ACalorimeterRPSPLT.TILE_AMPLITUDE,
                                             ACalorimeterRPSPLT.TILE_TIME);
                    realPulse[1][i] =
                        getPhysicsPulseShape(d, pmt2Time[index], pmt2PedestalLocal,
                        		             pmt2RawAmplitude[index],
                                             ACalorimeterRPSPLT.TILE_AMPLITUDE,
                                             ACalorimeterRPSPLT.TILE_TIME);
                    if(showPlotForRawTime)
                    {
                    	realPulseForRawTime[0][i] =
                    		getPhysicsPulseShape(d, pmt1RawTime[index], pmt1PedestalLocal,
                    				             pmt1RawAmplitude[index],
                    				             ACalorimeterRPSPLT.TILE_AMPLITUDE,
                    				             ACalorimeterRPSPLT.TILE_TIME);       		
                    	realPulseForRawTime[1][i] =
                            getPhysicsPulseShape(d, pmt2RawTime[index], pmt2PedestalLocal,
                            		             pmt2RawAmplitude[index],
				                                 ACalorimeterRPSPLT.TILE_AMPLITUDE,
				                                 ACalorimeterRPSPLT.TILE_TIME);
                    }
                }
            }
            catch(AAtlantisException aaex)
            {
                AOutput.append(aaex.getMessage(), ALogPane.WARNING);
                return;
            }

            if(realPulseForRawTime != null)
            {
                APulseShapePlot.plotRealPulseShapes(adcCounts, realPulse,
                        step, subTitle, title + " (cell time)");
                APulseShapePlot.plotRealPulseShapes(adcCounts, realPulseForRawTime,
                        step, subTitle, title + " (raw time)");
            }
            else
            {
            	// if RawTime stuff is to be removed, there will remain only
            	// one such call to plot real pulse shapes - this one            	
                APulseShapePlot.plotRealPulseShapes(adcCounts, realPulse,
                        step, subTitle, title);
            }            
        }
        else
        {
        	logger.warn(CALORIMETER_NAME + " plotPulseShapes() method called, " +
		                "but data is not available.");
            return;
        }

    } */// plotPulseShapes() --------------------------------------------------



    /**
     * Returns the histograms for this projection.
     *
     * @param projection AProjection2D current projection
     * @return ACoord[] polygons representing histograms
     */
    protected ACoord[] getUserHistograms(AProjection2D projection)
    {
        ACoord[] data = ACoord.NO_HISTOGRAMS;
        if (projection instanceof AProjectionYX && parameterStore.get("YX", "Mode").getI() == AProjectionYX.MODE_STANDARD)
            data = getYXHistograms();
        else if (projection instanceof AProjectionFR)
            data = getFRHistograms();
        else if (projection instanceof AProjectionRZ)
            data = getRZHistograms();
        return projection.nonLinearTransform(data);
    }

    @Override
    protected String getPulseTitleString(int index) {
        String[] decodedId = AIdHelper.getFullIdentifier(id[index]);

        String title = CALORIMETER_NAME + " cell: " + decodedId[0];
        
        return title;
    }

    @Override
    protected int[][] getADCCounts(int index) {

        if (pulseShapesDataAvailable) {
            // two channels for a TILE cell (two arrays of ADC counts)
            return new int[][]{adcCounts1[index], adcCounts2[index]};
        } else {
            return null;
        }
    }

    @Override
    protected double[][] getPulseShape(int index) {

        if (!pulseShapesDataAvailable) return null;

        // by zdenek (2008-11-24):
        // energy calculations (lowGainFactor, highGainFactor) and related
        // magic constants removed - now using pmt1RawAmplitude, pmt2RawAmplitude
        // instead of pmt[1,2]Energy, resp. former local variables
        // energy1, energy2 when calling getPhysicsPulseShape() further down

        // 1 .. 9 range (9 values of ADC counts, 9 samples (starts from 1!)),
        // need to get 401 values within this range. number of samples will
        // likely decrease in real data from 9 to 7
        double step = getPulseStep(index);
        double[][] realPulseRaw = new double[2][NUMBER_OF_LOOKUP_VALUES]; // 2 channels
        double d = 1.0;

        // if pmt1RawTime and pmt2RawTime are present, use those for
        // calculating data for real pulse shape plot based on these time values
        if (pmt1RawTime == null || pmt2RawTime == null) {
            logger.warn("No raw time values available");
            return null;
        }


        // by zdenek 2008-11-25
        // DPD (slimed version of ESD) sometimes don't have pmt[1,2]Pedestal
        // for a cell (the tag exists, values are there, but are 0). check
        // if pedestal is 0 and if so, take as pedestal value the first
        // adc count digit
        float pmt1PedestalLocal = pmt1Pedestal[index];
        float pmt2PedestalLocal = pmt2Pedestal[index];
        if (pmt1PedestalLocal == 0.0f && pmt2PedestalLocal == 0.0f) {
            logger.debug("TILE pmt[1,2]Pedestal are not available (i.e. " +
                    "are 0.0f), using minimum of first and last " +
                    "adcCount digit as pedestal.");
            float last1 = adcCounts1[index][adcCounts1[index].length - 1];
            float last2 = adcCounts2[index][adcCounts2[index].length - 1];
            pmt1PedestalLocal = Math.min(adcCounts1[index][0], last1);
            pmt2PedestalLocal = Math.min(adcCounts2[index][0], last2);
        }

        try {
            for (int i = 0; i < NUMBER_OF_LOOKUP_VALUES; i++) {
                d += step;
                realPulseRaw[0][i] =
                        getPhysicsPulseShape(d, pmt1RawTime[index], pmt1PedestalLocal,
                        pmt1RawAmplitude[index],
                        ACalorimeterRPSPLT.TILE_AMPLITUDE,
                        ACalorimeterRPSPLT.TILE_TIME);
                realPulseRaw[1][i] =
                        getPhysicsPulseShape(d, pmt2RawTime[index], pmt2PedestalLocal,
                        pmt2RawAmplitude[index],
                        ACalorimeterRPSPLT.TILE_AMPLITUDE,
                        ACalorimeterRPSPLT.TILE_TIME);
            }
        } catch (AAtlantisException aaex) {
            AOutput.append(aaex.getMessage(), ALogInterface.WARNING);
            return null;
        }

        return realPulseRaw;
    }

    protected double[][] getPulseShapeCellTime(int index) {

        if (!pulseShapesDataAvailable) return null;

        double step = getPulseStep(index);
        double[][] realPulse = new double[2][NUMBER_OF_LOOKUP_VALUES]; // 2 channels
        double d = 1.0;

        // by zdenek 2008-11-25
        // DPD (slimed version of ESD) sometimes don't have pmt[1,2]Pedestal
        // for a cell (the tag exists, values are there, but are 0). check
        // if pedestal is 0 and if so, take as pedestal value the first
        // adc count digit
        float pmt1PedestalLocal = pmt1Pedestal[index];
        float pmt2PedestalLocal = pmt2Pedestal[index];
        if (pmt1PedestalLocal == 0.0f && pmt2PedestalLocal == 0.0f) {
            logger.debug("TILE pmt[1,2]Pedestal are not available (i.e. " +
                    "are 0.0f), using minimum of first and last " +
                    "adcCount digit as pedestal.");
            float last1 = adcCounts1[index][adcCounts1[index].length - 1];
            float last2 = adcCounts2[index][adcCounts2[index].length - 1];
            pmt1PedestalLocal = Math.min(adcCounts1[index][0], last1);
            pmt2PedestalLocal = Math.min(adcCounts2[index][0], last2);
        }

        try {
            for (int i = 0; i < NUMBER_OF_LOOKUP_VALUES; i++) {
                d += step;
                realPulse[0][i] =
                        getPhysicsPulseShape(d, pmt1Time[index], pmt1PedestalLocal,
                        pmt1RawAmplitude[index],
                        ACalorimeterRPSPLT.TILE_AMPLITUDE,
                        ACalorimeterRPSPLT.TILE_TIME);
                realPulse[1][i] =
                        getPhysicsPulseShape(d, pmt2Time[index], pmt2PedestalLocal,
                        pmt2RawAmplitude[index],
                        ACalorimeterRPSPLT.TILE_AMPLITUDE,
                        ACalorimeterRPSPLT.TILE_TIME);
            }
        } catch (AAtlantisException aaex) {
            AOutput.append(aaex.getMessage(), ALogInterface.WARNING);
            return null;
        }

        return realPulse;
    }


    @Override
    protected double getPulseStep(int index) {
        return (adcCounts1[index].length - 1) / (float) NUMBER_OF_LOOKUP_VALUES;
    }

    @Override
    protected String[] getPulseSubtitle(int index) {
        String cap1 = "Real pulse shape " + "PMT " + pmt1Number[index] +
                " gain " + pmt1Gain[index];
        String cap2 = "Real pulse shape " + "PMT " + pmt2Number[index] +
                " gain " + pmt2Gain[index];
        return new String[]{cap1, cap2};
    }

   public void plotPulseShapesWithTiming(int index) {

       int[][] adc = getADCCounts(index);
       double[][] raw = getPulseShape(index);
       double[][] cell = getPulseShapeCellTime(index);
       
       if (adc == null || raw == null || cell == null) {
           logger.warn("Pulse shapes with raw/cell times requested but no " +
                   "data available, falling back to regular pulse shapes");
           plotPulseShapes(index, true);
       } else {       
           APulseShapePlot.plotRawCellPulseShapes(adc, raw, cell,
                            getPulseStep(index), getPulseSubtitle(index),
                            getPulseTitleString(index));
       }
        
        /*if (!withcurve) {
            APulseShapePlot.plotADCCounts(adc, getPulseTitleString(index), null);
        } else {
            double[][] ps = getPulseShape(index);
            if (ps == null) {
                logger.warn("No pulse shape information available, just plotting adc instead");
                APulseShapePlot.plotADCCounts(adc, getPulseTitleString(index), null);
            } else {
                APulseShapePlot.plotRealPulseShapes(adc, ps,
                        getPulseStep(index), getPulseSubtitle(index), getPulseTitleString(index));
            }
        }*/
    }
}
