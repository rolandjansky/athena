package atlantis.data;

import atlantis.event.*;
import atlantis.geometry.ACalorimeterDetector;
import atlantis.globals.AGlobals;
import atlantis.graphics.ACoord;
import atlantis.output.ALogInterface;
import atlantis.output.AOutput;
import atlantis.projection.AProjection2D;
import atlantis.projection.AProjectionFZ;
import atlantis.projection.AProjectionRZ;
import atlantis.projection.AProjectionYX;
import atlantis.utils.AAtlantisException;
import atlantis.utils.AHashMap;
import atlantis.utils.AIdHelper;
import atlantis.utils.AMath;
import atlantis.utils.AUtilities;
import atlantis.utils.ALogger;
import java.io.IOException;
import com.Ostermiller.util.CSVParser;

import java.io.InputStream;
import java.io.StringReader;
import java.io.FileNotFoundException;


/**
 * The Hadronic End Cap calorimeter.
 *
 * @author Eric Jansen
 */
public class AHECData extends ACalorimeterData
{
    private static ALogger logger = ALogger.getLogger(AHECData.class);
    
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
        		"rpsplt_hec.csv";
    // number of lookup table values for real pulse shapes plots calculation
    private static final short NUMBER_OF_LOOKUP_VALUES = 800;



    AHECData(AHashMap p, AEvent e)
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
            catch (AAtlantisException ex)
            {
                logger.error("Problem decoding ID " + id[i] + " in " + CALORIMETER_NAME + ": " + ex.getMessage());
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
                if (innerR == 0.0 || ACalorimeterDetector.get(i).getRMin() < innerR)
                {
                    innerR = ACalorimeterDetector.get(i).getRMin();
                }
                if (innerZ == 0.0 || ACalorimeterDetector.get(i).getZMin() < innerZ)
                {
                    innerZ = ACalorimeterDetector.get(i).getZMin();
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

        // read in HEC data from real pulse shapes plots
        readPulseShapePlotData(p);

    }



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

            if(ACalorimeterRPSPLT.areHECLookupTablesInitialized())
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
                    logger.debug(CALORIMETER_NAME +
                                  ": reading " + LOOKUP_TABLE_FILE +
                                  " failed, real pulse shapes plots will not " +
                                  "be available, reason: " + ex.getMessage(), ex);
                    pulseShapesDataAvailable = false;
                }
            }
        }

    } // readPulseShapePlotData() -------------------------------------------



    /**
     * Returns info about a selected hit (for picking)
     *
     * @param index int hit index
     * @return String info
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
                // see comment at the getADC2MevFactorPredefined() method
                //, using predefined " +
                //    Float.toString(getADC2MevFactorPredefined());
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
    private float getADC2MevFactorPredefined()
    {

        float r = 13.9f;
        return r;

    } // getADC2MevFactorPredefined() ---------------------------------------
    */



    /**
     * readLookupTableFile() reads in comma separated values (CSV) file
     * with HEC real pulse shapes plots time and amplitude lookup values
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
                if("HEC_AMPLITUDE".equals(arrayName))
                {
                    ACalorimeterRPSPLT.HEC_AMPLITUDE =
                        getLookupTableArray(s, NUMBER_OF_LOOKUP_VALUES);
                }
                else if("HEC_TIME".equals(arrayName))
                {
                    ACalorimeterRPSPLT.HEC_TIME =
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



    protected void applyCuts()
    {
        super.applyCuts();
        cut("CutsCalo", "HECET", "HECET", et);
        cut("CutsCalo", "HECEnergy", "HECEnergy", energy);

        int cutSub = parameterStore.get("CutsCalo", "HEC").getI();
        if(cutSub != -1)
        {
            cutArray(side, cutSub, "Endcap");
        }

    }

    /**
     * Returns the name of the parameter group.
     *
     * @return String parameter group
     */
    public String getParameterGroup()
    {
        return "HEC";
    }

    /**
     * Returns the name of the datatype.
     *
     * @return String datatype
     */
    public String getName()
    {
        return "HEC";
    }

    /**
     * Returns the displayed name of datatype
     *
     * @return String screen name
     */
    public String getNameScreenName()
    {
        return "HEC";
    }

    /**
     * Returns the type of calorimeter (ECAL/HCAL) for a hit.
     *
     * @param index int hit index
     * @return String calorimeter type
     */
    public String getCalorimeterType(int index)
    {
        return "HCAL";
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
        if (projection instanceof AProjectionRZ)
            data = getRZHistograms();
        else if (projection instanceof AProjectionFZ)
            data = getFZHistograms();
        else if ((projection instanceof AProjectionYX) && (mode == AProjectionYX.MODE_HEC_1 ||
                mode == AProjectionYX.MODE_HEC_2 || mode == AProjectionYX.MODE_HEC_3 ||
                mode == AProjectionYX.MODE_HEC_4 || mode == AProjectionYX.MODE_HEC_SUMMED))
            data = getYXHistograms();
        return projection.nonLinearTransform(data);
    }



//    /**
//    * Call util class which plots cell pulse shapes provided that
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
                AOutput.append("\nADC counts are not available for this cell, " +
                               "can't plot pulse shapes.", ALogPane.WARNING);
                return;
            }
            

            float[] time = ACalorimeterRPSPLT.HEC_TIME; // lookup tables
            float[] amplitude = ACalorimeterRPSPLT.HEC_AMPLITUDE; // lookup tables

            // step - starting from 1, need to get step numbers within
            // number of ADC samples (adcCounts[0].length)
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

    }*/ // plotPulseShapes() --------------------------------------------------

    @Override
    protected int[][] getADCCounts(int index) {
        
        if (pulseShapesDataAvailable) {
            return new int[][]{adcCounts[index]};
        } else {
            return null;
        }
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
        
        float[] time = ACalorimeterRPSPLT.HEC_TIME; // lookup tables
        float[] amplitude = ACalorimeterRPSPLT.HEC_AMPLITUDE; // lookup tables

        // step - starting from 1, need to get step numbers within
        // number of ADC samples (adcCounts[0].length)
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
        try {
            if (factor != -1) {
                for (int i = 0; i < NUMBER_OF_LOOKUP_VALUES; i++) {
                    d += step;
                    double localTime = getLocalTime(d, cellTime[index], this.numSamplings);
                    realPulse[0][i] = super.getPhysicsPulseShape(localTime,
                            cellPedestal[index], energyLocal,
                            amplitude, time, NUMBER_OF_LOOKUP_VALUES);
                }
            }
        } catch (AAtlantisException aaex) {
            AOutput.append(aaex.getMessage(), ALogInterface.WARNING);
            return null;
        }

        if (factor != -1) {
            logger.debug("adc2Mev factor available, plotting full plot.");
            return realPulse;
        } else {
            logger.debug("adc2Mev factor not available, plotting just adc counts.");
            return null;
        }
    }

    @Override
    protected double getPulseStep(int index) {
        return (adcCounts[0].length - 1) / (float) NUMBER_OF_LOOKUP_VALUES;
    }
    
}
