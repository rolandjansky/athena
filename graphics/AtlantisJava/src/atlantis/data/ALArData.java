package atlantis.data;

import atlantis.event.*;
import com.Ostermiller.util.CSVParser;

import java.io.IOException;
import java.io.InputStream;
import java.io.StringReader;
import java.io.FileNotFoundException;

import atlantis.geometry.ABarrelCalorimeterDetector;
import atlantis.geometry.ACalorimeterDetector;
import atlantis.globals.AGlobals;
import atlantis.graphics.ACoord;
import atlantis.output.ALogInterface;
import atlantis.output.AOutput;
import atlantis.projection.AProjection2D;
import atlantis.projection.AProjectionFR;
import atlantis.projection.AProjectionFZ;
import atlantis.projection.AProjectionRZ;
import atlantis.projection.AProjectionYX;
import atlantis.utils.AAtlantisException;
import atlantis.utils.AHashMap;
import atlantis.utils.AIdHelper;
import atlantis.utils.AMath;
import atlantis.utils.AUtilities;
import atlantis.parameters.AParameter;
import atlantis.utils.ALogger;


/**
 * The Liquid Argon ElectroMagnetic Calorimeter
 *
 * @author Eric Jansen
 */
public class ALArData extends ACalorimeterData
{
    private static ALogger logger = ALogger.getLogger(ALArData.class);
    
    // data for real pulse shapes plots
    private int numSamplings = 0;
    private int[][] adcCounts = null;
    private float[] cellTime = null;
    private int[] cellGain = null;
    private float[] cellPedestal = null;
    private float[] adc2Mev = null;
    private static boolean pulseShapesDataAvailable = false;
    private static final String LOOKUP_TABLE_FILE =
    		AGlobals.instance().getHomeDirectory() + "configuration" + 
        		System.getProperty("file.separator") +
        		"rpsplt_lar.csv";
    // number of lookup table values for real pulse shapes plots calculation
    private static final short NUMBER_OF_LOOKUP_VALUES = 799;



    ALArData(AHashMap p, AEvent e)
    {
        super(p,e);

        for (int i = 0; i < numData; i++)
        {
            try
            {
                side[i] = (byte) AIdHelper.larBarrelEndcap(id[i]);
                etaIndex[i] = (short) AIdHelper.larEta(id[i]);
                phiIndex[i] = (short) AIdHelper.larPhi(id[i]);
                sampling[i] = AIdHelper.larSampling(id[i]);
            }
            catch (AAtlantisException aex)
            {
                logger.warn("Problem decoding ID " + id[i] + " in " +
                               CALORIMETER_NAME + ": " + aex.getMessage(), aex);
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

        // Collect some constants we need for the histograms.
        for (int i = 0; i < ACalorimeterDetector.count(); i++)
        {

            if (ACalorimeterDetector.get(i).getName().indexOf(CALORIMETER_NAME) >= 0)
            {
                if (ACalorimeterDetector.get(i) instanceof ABarrelCalorimeterDetector)
                {
                    if (innerR == 0.0 || ACalorimeterDetector.get(i).getRMin() < innerR)
                    {
                        innerR = ACalorimeterDetector.get(i).getRMin();
                    }
                }
                else
                {
                    if (innerZ == 0.0 || ACalorimeterDetector.get(i).getZMin() < innerZ)
                    {
                        innerZ = ACalorimeterDetector.get(i).getZMin();
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
                if (outerEta == 0.0 || ACalorimeterDetector.get(i).getEtaMax() > outerEta)
                {
                    outerEta = ACalorimeterDetector.get(i).getEtaMax();
                }
                if (phiGranularity == 0.0 || ACalorimeterDetector.get(i).getDeltaPhi() < phiGranularity)
                {
                    phiGranularity = ACalorimeterDetector.get(i).getDeltaPhi();
                }
                if (etaGranularity == 0.0 || ACalorimeterDetector.get(i).getDeltaEta() < etaGranularity)
                {
                    etaGranularity = ACalorimeterDetector.get(i).getDeltaEta();
                }
            }
        }

        // Add a little bit of extra margin to prevent binning errors due to
        // rounding of numbers.
        outerEta += etaGranularity;

        readPulseShapePlotData(p);

    } // ALArData() ---------------------------------------------------------



    private void readPulseShapePlotData(AHashMap p)
    {
        // read ADCCounts and cell data for real pulse shapes plots
        adcCounts = super.getADCCountsData(p);

        // read LAr digits (cell data) for real pulse shapes plots
        cellTime = (p.get("cellTime") != null) ? p.getFloatArray("cellTime") : null;
        cellGain = (p.get("cellGain") != null) ? p.getIntArray("cellGain") : null;
        cellPedestal = (p.get("cellPedestal") != null) ? p.getFloatArray("cellPedestal") : null;
        adc2Mev = (p.get("adc2Mev") != null) ? p.getFloatArray("adc2Mev") : null;

        pulseShapesDataAvailable = false;
        if(adcCounts != null && cellTime != null && cellGain != null &&
           cellPedestal != null && adc2Mev != null)
        {
            pulseShapesDataAvailable = true;
            numSamplings = adcCounts[0].length;
            
            logger.debug(CALORIMETER_NAME +
                    ": data for real pulse shape plots available");

            if(ACalorimeterRPSPLT.areLarLookupTablesInitialized())
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



    /**
     * readLookupTableFile() reads in comma separated values (CSV) file
     * with LAr real pulse shapes plots time and amplitude lookup values
     * @param fileName String
     * @throws AAtlantisException
     */
    private void readLookupTableFile() throws AAtlantisException
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
                
                float[] array = getLookupTableArray(s, NUMBER_OF_LOOKUP_VALUES);
                ACalorimeterRPSPLT.setLarTable(arrayName, array); 
            } // while
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



    /**
     *
     * @param index int
     * @param lookupTablesInfo String
     * Debug info method in order to make sure that correct lookup table
     * is used given barrel/endcap, sampling and region of LAr.
     */
    private void pulseShapePlotsDebugInfo(int index, String lookupTableName)
    {
        String msg = null;
        try {
            msg = "\n    barrel/endcap value: " +
                    AIdHelper.larBarrelEndcap(id[index]);
            msg += " is ";
            msg += AIdHelper.larIsBarrel(id[index]) ? "barrel" : "endcap";
            msg += "\n    sampling/layer: " + sampling[index];
            msg += "\n    region: " + AIdHelper.larRegion(id[index]);
            msg += "\n    lookup table used: " + lookupTableName;
            logger.debug(msg + "\n");
        } catch (AAtlantisException aaex) {
            logger.debug("\n  exception: " + aaex.getMessage(), aaex);
        }
    } // pulseShapePlotsDebugInfo() -----------------------------------------


    
    /**
     * This method is used when calculating real pulse shapes plots.
     * It is called from getLookupTableForPulseShapesPlots()
     * It calculates index of the 0.1 eta bin the argument eta
     * (eta of a cell) falls into. Thus it determines which eta
     * range a cell belongs to (eta ranges are last level of cells
     * division from barrel/endcap, layer, region.
     * startIndex argument tells which eta range we start counting from
     * (given the eta ranges division it may not necessarily be from 0).
     * If eta doesn't fall into a specified interval, empty String
     * is returned - then missing ETAx suffix will cause retrieving
     * of the float[] will fail.
     * 
     * @param etaFrom
     * @param etaTo
     * @return
     */
    private static String getEtaBinIndex(float etaFrom, float etaTo,
                                         float cellEta, int startIndex)
    {
        float step = 0.1f; // step, size of the eta bins
        float absEta = Math.abs(cellEta);
        int index = startIndex;
        
        for(float f = etaFrom; f < etaTo; f += step, index++)
        {
            if(absEta > f && absEta < f + step)
            {
                logger.debug("eta range calculation: cell eta = " + cellEta + " " +
                             "etaFrom = " + etaFrom + " etaTo = " + etaTo);
                logger.debug("eta range index result: " + index +
                             " (index offset was: " + startIndex + ")");
                return "ETA" + index + "_" ;
            }
        }
        
        // calculating bin failed, retrieving the array will fail later
        return "";
        
    } // calculateEtaBinIndex() ---------------------------------------------
    
    

    /**
     * Method returns correct float[] array for amplitude lookup table
     * depending whether a cell (determined by index, resp. id[index]) is in
     * barrel, endcap, which layer and which region or which eta range.
     * This method accesses ACalorimeterRPSPLT
     * Methods contains constant value defining parts of LAr detector
     * (as used in AIdHelper) and also eta range constants provided by
     * the LAr community.
     *
     * @param index int
     * @return float[]
     */
    private float[] getLookupTableForPulseShapesPlots(int index) throws AAtlantisException
    {
        float[] amplitude =  null; // lookup table amplitude values
        
        // helper variable - absolute value of eta - for checking of the eta range
        float absEta = Math.abs(eta[index]);
        
        // the key for ACalorimeterRPSPLT.getLarTable(key) will gradually be
        // constructed depending on barrel/endcap, layer, region and eta range
        String lookupTableKey = "LAR_"; // start of the name all LAR_ tables
        

        if(AIdHelper.larIsBarrel(id[index]))
        {
            // it is barrel
            lookupTableKey += "BARREL_";
            switch(sampling[index])
            {
                case 0: // layer 0
                    // plot ADC counts only for this layer
                    String[] decodedId = AIdHelper.getFullIdentifier(id[index]);
                    String title = "LAr cell: " + decodedId[0];
                    int[][] data = { adcCounts[index] };
                    APulseShapePlot.plotADCCounts(data, title, null);
                    throw new AAtlantisException("ADC counts plot only, pulse shape plot not " +
                                                 "implemented for LAr barrel, layer 0");
                case 1:
                    lookupTableKey += "LAYER1_"; // layer 1
                    switch(AIdHelper.larRegion(id[index]))
                    {
                        case 0:
                            lookupTableKey += "REGION0_";
                            break;
                        case 1:
                            lookupTableKey += "REGION1_";
                            break;
                        default:
                            AOutput.append("\npulse shape plots - LAr barrel, " +
                                           "layer 1, region > 1. Using layer 1, " +
                                           "region 1 lookup table\n", ALogInterface.WARNING);
                            lookupTableKey += "REGION1_";
                    }
                    break;
                case 2: // layer 2
                    lookupTableKey += "LAYER2_"; // layer 2
                    switch(AIdHelper.larRegion(id[index]))
                    {
                        case 0:
                            lookupTableKey += "REGION0_";
                            break;
                        case 1:
                            lookupTableKey += "REGION1_";
                            break;
                        default:
                            AOutput.append("\npulse shape plots - LAr barrel, " +
                                           "layer 2, region > 1. Using layer 2, " +
                                           "region 1 lookup table\n", ALogInterface.WARNING);
                            lookupTableKey += "REGION1_";
                    }
                    break;
                case 3: // layer 3
                    lookupTableKey += "LAYER3_REGION0_";
                    break;
            }
        } // if(AIdHelper.larIsBarrel(id[index]))
        else
        {
            // we are in the endcap
            lookupTableKey += "ENDCAP_";
            switch(sampling[index])
            {
                case 0: // layer 0
                    // only one region and one eta range on this layer
                    lookupTableKey += "LAYER0_REGION0_ETA0_";
                    break;
                case 1: // layer 1
                    lookupTableKey += "LAYER1_";
                    switch(AIdHelper.larRegion(id[index]))
                    {
                        case 0:
                            lookupTableKey += "REGION0_";
                            if(absEta > 1.375f && absEta < 1.425f)
                            {
                                lookupTableKey += "ETA0_"; // first eta range
                            }
                            else
                            {
                                // eta ranges with step 0.1 provided by Andre
                                // start index counting from 1, 0 range is used above
                                lookupTableKey += getEtaBinIndex(2.5f, 3.2f, eta[index], 1);                              
                            }
                            break;
                        case 1:
                            lookupTableKey += "REGION1_ETA0_"; // no eta subdivision
                            break;
                        case 2:
                            lookupTableKey += "REGION2_";
                            // starting from first range, startIndex 0
                            lookupTableKey += getEtaBinIndex(1.5f, 1.8f, eta[index], 0);
                            break;
                        case 3:
                            lookupTableKey += "REGION3_";
                            lookupTableKey += getEtaBinIndex(1.8f, 2.0f, eta[index], 0);
                            break;
                        case 4:
                            lookupTableKey += "REGION4_";
                            lookupTableKey += getEtaBinIndex(2.0f, 2.4f, eta[index], 0);
                            break;
                        case 5:
                            lookupTableKey += "REGION5_";
                            if(absEta > 2.4f && absEta < 2.5f)
                            {
                                lookupTableKey += "ETA0_";
                            }
                            break;
                        default:
                            AOutput.append("\npulse shape plots - LAr endcap, " +
                                           "layer 1, region > 5. Using layer 1, " +
                                           "region 5, eta range 0 lookup table\n",
                                           ALogInterface.WARNING);
                            lookupTableKey += "REGION5_ETA0_";
                    }
                    break;
                case 2: // layer 2
                    lookupTableKey += "LAYER2_";
                    switch(AIdHelper.larRegion(id[index]))
                    {
                        case 0:
                            lookupTableKey += "REGION0_";
                            if(absEta > 1.375f && absEta < 1.425f)
                            {
                                lookupTableKey += "ETA0_"; // first eta range
                            }
                            else
                            {
                                // eta ranges with step 0.1 provided by Andre
                                // start index counting from 1, 0 range is used above
                                lookupTableKey += getEtaBinIndex(2.5f, 3.2f, eta[index], 1);                              
                            }
                            break;
                        case 1:
                            lookupTableKey += "REGION1_";
                            lookupTableKey += getEtaBinIndex(1.425f, 2.5f, eta[index], 0);
                            break;
                    }
                    break;
                case 3: // layer 3
                    lookupTableKey += "LAYER3_";
                    lookupTableKey += "REGION0_";
                    lookupTableKey += getEtaBinIndex(1.5f, 2.5f, eta[index], 0);
                    break;
            }
        } // else (endcap)

        lookupTableKey += "AMPLITUDE";
        pulseShapePlotsDebugInfo(index, lookupTableKey);
        amplitude = ACalorimeterRPSPLT.getLarTable(lookupTableKey);
        
        return amplitude;

    } // getLookupTableForPulseShapesPlots() -------------------------------


    
    /**
     * This method plots only the ADC counts and no pulse shape plot for a
     * given cell. This is useful when the pulse shape plot is not correct
     * but ADC count plot is still needed. The method is called from pick
     * interaction with D (digits only), see APickInteraction.pressed()
     * It is supposedly a temporary solution until LAr real pulse shapes
     * lookup tables are corrected / finalized and propagated to Atlantis.
     * @param index
     */
    public void plotADCCounts(int index)
    {
        if(pulseShapesDataAvailable)
        {
            String title = getPulseTitleString(index);
            
            int[][] adcCountsLocal = new int[][] { adcCounts[index] };
            
            logger.debug("Plotting only ADC counts plot ... ");
            APulseShapePlot.plotADCCounts(adcCountsLocal, title, null);
            
        } // if(pulseShapesDataAvailable)
        else
        {
            return;
        }
                
    } // plotADCCounts() ----------------------------------------------------

    
    
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
            
            int[][] adcCountsLocal = new int[][] { adcCounts[index] };

            
            if(super.checkADCCountsAvailability(adcCountsLocal))
            {
                // adc counts are available
                logger.debug(CALORIMETER_NAME + " adc counts (digits) are " +
                             "available for pulse shapes plots for this cell.");
            }
            else
            {
                String m = "ADC counts are not available for this cell, " +
                           "can't plot pulse shapes.";
                AOutput.append("\n" + m, ALogPane.WARNING);
                logger.warn(m);
                return;
            }
            
            
            float[] amplitude = null; // amplitude lookup table
            float[] time = null; // time lookup table
            try
            {
                time = ACalorimeterRPSPLT.getLarTable("LAR_TIME");
                amplitude = getLookupTableForPulseShapesPlots(index);
            }
            catch(AAtlantisException ex)
            {
                AOutput.append("\n" + ex.getMessage(), ALogPane.WARNING);
                logger.error(ex.getMessage());
                return;
            }

            // 1 .. 32 range (32 values of ADC counts, i.e. 32 samples), need to
            // get NUMBER_OF_LOOKUP_VALUES (769) within this range (starts from 1!).
            // Real data will have (probably) only 5 samples per cell
            double step = (adcCounts[0].length - 1) / (float) NUMBER_OF_LOOKUP_VALUES;
            double[][] realPulse = new double[1][NUMBER_OF_LOOKUP_VALUES]; // 1 channel
            double d = 1.0;
            // if adc2Mev != -1 -> use value from event file, otherwise predefined value
            // see comment at the getADC2MevFactorPredefined() method
            // factor variable is used as a flag whether or not to plot pulse shape,
            // if is -1 (adc2Mev not available), then want to see only the adc counts
            // and don't calculate the pulse shapes
            // getADC2MevFactorPredefined(index) is no longer in use, see comment at it
            float factor = adc2Mev[index];

            double energyLocal = energy[index] * 1000.0 / factor;
            try
            {
            	if(factor != -1)
            	{
	                for(int i = 0; i < NUMBER_OF_LOOKUP_VALUES; i++)
	                {
	                    d += step;
	                    realPulse[0][i] = super.getPhysicsPulseShape(d, cellTime[index],
	                                            cellPedestal[index], energyLocal,
	                                            amplitude, time, NUMBER_OF_LOOKUP_VALUES);
	                }
            	}
            }
            catch(AAtlantisException aaex)
            {
                AOutput.append(aaex.getMessage(), ALogPane.WARNING);
                logger.error(aaex.getMessage());
                return;
            }

            if(factor != -1)
            {
            	logger.debug("adc2Mev factor available, plotting full plot.");
            	APulseShapePlot.plotRealPulseShapes(adcCountsLocal, realPulse,
                                                    step, null, title);
            }
            else
            {
            	logger.debug("adc2Mev factor not available, plotting just adc counts.");
            	APulseShapePlot.plotADCCounts(adcCountsLocal, title, null);
            }

        } // if(pulseShapesDataAvailable)
        else
        {
            return;
        }

    } */// plotPulseShapes() --------------------------------------------------

    @Override
    protected int[][] getADCCounts(int index) {
        if (pulseShapesDataAvailable) {
            for (int adc : adcCounts[index]) {
                // Only return something if it is non-zero
                if (adc != 0) return new int[][] { adcCounts[index] };
            }
        }
        return null;
    }

    /**
     * Calculate local time for pulse shape plots
     */
    protected double getLocalTime(double xTime, double cellTime, int numSamplings) {
        int nominalPeakSample = (int)(numSamplings / 2.);
        return ((xTime - nominalPeakSample + 3) * 25.0) - cellTime;
    }

    @Override
    protected double[][] getPulseShape(int index) {

            float[] amplitude = null; // amplitude lookup table
            float[] time = null; // time lookup table
            try
            {
                time = ACalorimeterRPSPLT.getLarTable("LAR_TIME");
                amplitude = getLookupTableForPulseShapesPlots(index);
            }
            catch(AAtlantisException ex)
            {
                AOutput.append("\n" + ex.getMessage(), ALogInterface.WARNING);
                logger.error(ex.getMessage());
                return null;
            }

            // 1 .. 32 range (32 values of ADC counts, i.e. 32 samples), need to
            // get NUMBER_OF_LOOKUP_VALUES (769) within this range (starts from 1!).
            // Real data will have (probably) only 5 samples per cell
            double step = getPulseStep(index);
            double[][] realPulse = new double[1][NUMBER_OF_LOOKUP_VALUES]; // 1 channel
            double d = 1.0;
            // if adc2Mev != -1 -> use value from event file, otherwise predefined value
            // see comment at the getADC2MevFactorPredefined() method
            // factor variable is used as a flag whether or not to plot pulse shape,
            // if is -1 (adc2Mev not available), then want to see only the adc counts
            // and don't calculate the pulse shapes
            // getADC2MevFactorPredefined(index) is no longer in use, see comment at it
            float factor = adc2Mev[index];

            double energyLocal = energy[index] * 1000.0 / factor;
            try
            {
            	if(factor != -1)
            	{
	                for(int i = 0; i < NUMBER_OF_LOOKUP_VALUES; i++)
	                {
	                    d += step;
                            double localTime = getLocalTime(d, cellTime[index], this.numSamplings);
	                    realPulse[0][i] = super.getPhysicsPulseShape(localTime,
	                                            cellPedestal[index], energyLocal,
	                                            amplitude, time, NUMBER_OF_LOOKUP_VALUES);
	                }
            	}
            }
            catch(AAtlantisException aaex)
            {
                AOutput.append(aaex.getMessage(), ALogInterface.WARNING);
                logger.error(aaex.getMessage());
                return null;
            }

            if(factor != -1)
            {
            	logger.debug("adc2Mev factor available, plotting full plot.");
                return realPulse;
            }
            else
            {
                return null;
            }
    }

    @Override
    protected double getPulseStep(int index) {
        return (adcCounts[0].length - 1) / (float) NUMBER_OF_LOOKUP_VALUES;
    }

//    /**
//     * Determines adc2Mev factor value for a cell. The number either comes
//     * with the event file (adc2Mev subtag), but sometimes is not available
//     * in Athena (in the database) which is indicated by -1 in the event file
//     * for a particular cell. In this case, use predefined constant in this
//     * method.
//     *
//     * zdenek (2008-09-15):
//     * These predefined values (which are used if adc2Mev factor is -1) should
//     * no longer be used. if the factor is not present, do not plot the pulse
//     * shape, just the ADC counts as these values are non-sense. The method
//     * and references to it should be removed after some time (if calo people
//     * don't change their mind to pre-define some other values and use those)
//
//     * @param index int
//     * @return String
//     */
    /*
    private float getADC2MevFactorPredefined(int index)
    {
        boolean barrel = false;
        int layer = sampling[index];

        try
        {
            barrel = AIdHelper.larIsBarrel(id[index]);
        }
        catch (AAtlantisException e)
        {
            AOutput.append("Problem decoding ID " + id[index] +
                           " in " + CALORIMETER_NAME + ": " + e.getMessage(),
                           ALogPane.WARNING);
            return Float.NEGATIVE_INFINITY;
        }

        float r = Float.NEGATIVE_INFINITY;
        if(barrel)
        {
            switch(layer)
            {
                case 0: r = 7.0f;
                        break;
                case 1: r = 2.5f;
                        break;
                case 2: r = Math.abs(this.eta[index]) < 0.8 ? 10.0f : 18.0f;
                        break;
                case 3: r = 9.0f;
                        break;
            }
        }
        else
        {
            r = 16.0f;
        }

        return r;

    } // getADC2MevFactor() -------------------------------------------------
     */


    /**
     * Returns calo hit info
     *
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
        String cellInfo = "";

        // if decoded ID is required and implemented for LAr ...
        // String decodedId = AIdHelper.getDecodedTileIndentifier(id[index]);
        // t = decodedId + " " + super.getHitInfo(index);
        t = super.getHitInfo(index);

        cellInfo += "\n cell time = ";
        cellInfo += (cellTime != null) ? Float.toString(cellTime[index]) +
                                         " ns" : "n/a";
        cellInfo += "\n cell gain = ";
        cellInfo += (cellGain != null) ? Integer.toString(cellGain[index]) : "n/a";
        cellInfo += "\n cell pedestal = ";
        cellInfo += (cellPedestal != null) ? Float.toString(cellPedestal[index]) +
                                             " ADC counts" : "n/a";

        cellInfo += "\n ADC 2 MeV = ";
        if(adc2Mev == null)
        {
            cellInfo += "n/a";
        }
        else
        {
            // subtag adc2Mev was available in the event file
            if(adc2Mev[index] != -1)
            {
                // other than -1: the factor was available in
                // Athena (database) for this particular cell
                cellInfo += Float.toString(adc2Mev[index]) + " (from database)";
            }
            else
            {
                // -1 -> the factor wasn't available in Athena (database),
                // use hardcoded constant for this particular cell
                cellInfo += "-1 (n/a in database)";
                // see comment at getADC2MevFactorPredefined() method
                // using predefined " +
                // Float.toString(getADC2MevFactorPredefined(index));
            }
        }

        cellInfo += pulseShapesDataAvailable ? "" :
                    "\n data for real pulse shapes plot n/a";

        return t + cellInfo;

    } // getHitInfo() -------------------------------------------------------

    @Override //ACalorimeterData
    //Gives the hit time for this cell
    protected double getTime(int hit)
    {
        if (cellTime == null) return 0.0;
        else return cellTime[hit];
    }

    private void cutEnergyByLayer(String groupName, String parameterName,
                                  String text, float[] array, int[] sampling, int currSampling)
    {
        AParameter par = parameterStore.get(groupName, parameterName);

        if(par.getStatus() && array != null)
        {
            double value = par.getD();
            String operator = par.getOperator();
            int num = 0;

            // this special cut shall have isMod="YES" attribute set, so
            // par.isModulus() is true, thus only Math.abs() values are considered
            if(operator.equals("<"))
            {
                for(int i = 0; i < numDraw; i++)
                {
                    if(sampling[listdl[i]] == currSampling)
                    {
                        if(Math.abs(array[listdl[i]]) < value)
                        {
                            listdl[num++] = listdl[i];
                        }
                    }
                    else
                    {
                        // don't check energy for other layers and don't cut
                        listdl[num++] = listdl[i];
                    }
                }
            }
            else if(operator.equals(">"))
            {
                // '>' really means '>=' for reals since they are real in
                // from ascii file and don't have full precison
                for(int i = 0; i < numDraw; i++)
                {
                    if(sampling[listdl[i]] == currSampling)
                    {
                        if(Math.abs(array[listdl[i]]) >= value)
                        {
                            listdl[num++] = listdl[i];
                        }
                    }
                    else
                    {
                        // don't check energy for other layers and don't cut
                        listdl[num++] = listdl[i];
                    }
                }
            }
            else
            {
                throw new Error(operator + " operator not sensible for floats");
            }

            numDraw = num;
        }

    } // cutEnergyByLayer() -------------------------------------------------



    protected void applyCuts()
    {
        super.applyCuts();
        cut("CutsCalo", "LArET", "LArET", et);
        cut("CutsCalo", "LArEnergy", "LArEnergy", energy);

        // energy cut (E) per layers
        cutEnergyByLayer("CutsCalo", "LArEnergyLayer0", "LArEnergyLayer0",
                         energy, sampling, 0);
        cutEnergyByLayer("CutsCalo", "LArEnergyLayer1", "LArEnergyLayer1",
                         energy, sampling, 1);
        cutEnergyByLayer("CutsCalo", "LArEnergyLayer2", "LArEnergyLayer2",
                         energy, sampling, 2);
        cutEnergyByLayer("CutsCalo", "LArEnergyLayer3", "LArEnergyLayer3",
                         energy, sampling, 3);

        // cut by LAr region (+- EndCap, All)
        int cutSub = parameterStore.get("CutsCalo", "LAr").getI();
        if(cutSub != -1)
        {
	    
            int num = 0;
            for(int i = 0; i < numDraw; i++)
            {

		switch(cutSub){
		case 0: 
		    if(side[listdl[i]] == -2 || side[listdl[i]] == -3){
		    listdl[num++] = listdl[i];
		    }
		    break;
		case 1: 
		    if(side[listdl[i]] == 2 || side[listdl[i]] == 3){
			listdl[num++] = listdl[i];
		    }
		    break;
	  	case 2: 
		    if(Math.abs(side[listdl[i]]) != 1){
			listdl[num++] = listdl[i];
		    }
		    break;
		case 3: 
		    if(Math.abs(side[listdl[i]]) == 1){
			listdl[num++] = listdl[i];
		    }
		    break;
		default:
		    break;
		    
			   
		}
	    }
            numDraw = num;
	    
	}
    } // applyCuts() --------------------------------------------------------


    /**
     * Returns the name of the parameter group.
     *
     * @return String parameter group
     */
    public String getParameterGroup()
    {
        return "LAr";
    }

    /**
     * Returns the name of the datatype.
     *
     * @return String datatype
     */
    public String getName()
    {
        return "LAr";
    }

    /**
     * Returns the name of the datatype.
     *
     * @return String datatype
     */
    public String getNameScreenName()
    {
        return "LAr";
    }

    /**
     * Returns the type of calorimeter (ECAL/HCAL) for a hit.
     *
     * @param index int hit index
     * @return String calorimeter type
     */
    public String getCalorimeterType(int index)
    {
        return "ECAL";
    }

    /**
     * Returns the histograms for this projection.
     *
     * @param projection AProjection2D current projection
     * @return ACoord[] polygons representing histograms
     */
    protected ACoord[] getUserHistograms(AProjection2D projection)
    {
        ACoord[] data = ACoord.NO_HISTOGRAMS;
        int mode = parameterStore.get("YX", "Mode").getI();
        if ((projection instanceof AProjectionYX) && (mode == AProjectionYX.MODE_STANDARD ||
                mode == AProjectionYX.MODE_LAR_ENDCAP_PRESAMPLER ||
                mode == AProjectionYX.MODE_LAR_ENDCAP_1 ||
                mode == AProjectionYX.MODE_LAR_ENDCAP_2 ||
                mode == AProjectionYX.MODE_LAR_ENDCAP_3 ||
                mode == AProjectionYX.MODE_LAR_ENDCAP_SUMMED))
            data = getYXHistograms();
        else if (projection instanceof AProjectionFR)
            data = getFRHistograms();
        else if (projection instanceof AProjectionRZ)
            data = getRZHistograms();
        else if (projection instanceof AProjectionFZ)
            data = getFZHistograms();
        return projection.nonLinearTransform(data);
    }
}
