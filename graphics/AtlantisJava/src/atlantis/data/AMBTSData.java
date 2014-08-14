package atlantis.data;

import atlantis.event.*;
import atlantis.geometry.ACalorimeterDetector;
import atlantis.globals.AGlobals;
import atlantis.graphics.ACoord;
import atlantis.output.ALogInterface;
import atlantis.output.AOutput;
import atlantis.projection.AProjection2D;
import atlantis.projection.AProjectionYX;
import atlantis.utils.AAtlantisException;
import atlantis.utils.AHashMap;
import atlantis.utils.ALogger;


/**
 * The Minimum Bias Trigger Scintillator implementation.
 *
 * @author Mark Stockton
 */
public class AMBTSData extends ACalorimeterData
{
	private static ALogger logger = ALogger.getLogger(AMBTSData.class);

	
    /*
     * For Rz histograms may/will need:
     * 
     * private int[][] cellTable;
     * 
     * phiIndex[i]
     * etaIndex[i]
     * outerEta -> use -> detector.getEtaMax()
     * outerR   -> use -> detector.getRMax()
     * outerZ   -> use -> detector.getZMax() 
     * etaGranularity
     */
    
    protected float[] quality;
    protected float[] time;
    //label not needed at present as is just a string made 
    // from type, module and channel 
    //protected String[] label;
    
    // data for real pulse shapes plots
    private int[][] adcCounts = null;    
    private float[] cellPedestal = null;
    private float[] cellRawTime = null;
    private float[] cellRawAmplitude = null;
    private static boolean pulseShapesDataAvailable = false;
    private static final String LOOKUP_TABLE_FILE =
    		AGlobals.instance().getHomeDirectory() + "configuration" + 
        		System.getProperty("file.separator") +
        		"rpsplt_tile.csv";
    // number of lookup table values for real pulse shapes plots calculation
    private static final short NUMBER_OF_LOOKUP_VALUES = 401;
    
    

    
    protected ACalorimeterDetector[] MBTSlayers = new ACalorimeterDetector[2];

    AMBTSData(AHashMap p, AEvent e)
    {
        super(p,e);

        channel = p.getIntArray("channel");
        /*
         * this energy is in MeV not GeV
         */
        et = p.getFloatArray("energy");
        etSum = p.getFloatArray("energy");
        //label = p.getStringArray("label");
        quality = p.getFloatArray("quality");
        sampling = p.getIntArray("sampling");
        time = p.getFloatArray("time");
        
        /*
         * The phi and eta coordinates used here are actually not phi and eta
         * 
         * Instead: - phi refers to the module number 0-8
         *          - eta refers to which side of the detector
         *          
         * The actual phi and eta are then calculated from the geometry object
         */
        int[] inteta;
        int[] intphi;
        if( p.containsKey("module"))
        {
            //phi new version
            intphi = p.getIntArray("module");
        }
        else    
        {
            //phi first version
            float[] tempphi = p.getFloatArray("phi");
            intphi = new int[numData];
            for(int i=0; i<numData; i++)
            {
                intphi[i]=(int) (( (8*tempphi[i])/(2*Math.PI))-(1/2));
            }
        }
        
        if( p.containsKey("type"))
        {
            //eta new version
            inteta = p.getIntArray("type");
        }
        else
        {
            //eta first version
            float[] tempeta = p.getFloatArray("eta");
            inteta = new int[numData];
            for(int i=0; i<numData; i++)
            {
                if(tempeta[i]<0)
                { 
                    inteta[i] = -1;
                }
                else
                {
                    inteta[i] = 1;
                }
            }
        }
        eta = new float[numData];
        phi = new float[numData];
        for(int i=0; i<numData; i++)
        {
            eta[i]=inteta[i];
            phi[i]=intphi[i];
        }
        
        //now define sub and side depending on eta value
        for(int i=0; i<numData; i++)
        {
            if(eta[i]<0)
            { 
                sub[i] = 0;
                side[i] = -1;
            }
            else
            {
                sub[i] = 1;
                side[i] = 1;
            }
        }
        
        //store the two sampling of the MBTS geometry
        for (int i=0; i<ACalorimeterDetector.count(); i++) 
        {
            ACalorimeterDetector detector = ACalorimeterDetector.get(i);
            if(detector.getName().equals("Minimum Bias Trigger Scintillators"))
            {
                MBTSlayers[detector.getSampling()] = detector;
            }
        }
        
        // read in MBTS data for real pulse shapes plots
        readPulseShapePlotData(p);
        
    }
    
    
    
    private void readPulseShapePlotData(AHashMap p)
    {
        // read ADCCounts and cell data for real pulse shapes plots
        adcCounts = super.getADCCountsData(p);

        // read other MBTS data for real pulse shapes plots
        cellPedestal = (p.get("cellPedestal") != null) ? p.getFloatArray("cellPedestal") : null;
        cellRawTime = (p.get("cellRawTime") != null) ? p.getFloatArray("cellRawTime") : null;
        cellRawAmplitude = (p.get("cellRawAmplitude") != null) ? p.getFloatArray("cellRawAmplitude") : null;
        
        pulseShapesDataAvailable = false;
        if(adcCounts != null && cellPedestal != null && cellRawTime != null &&
           cellRawAmplitude != null)
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
                    ATILEData.readLookupTableFile();
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
     * Returns the name of the parameter group.
     *
     * @return String parameter group
     */
    public String getParameterGroup()
    {
        return "TILE";
    }

    /**
     * Returns the name of the datatype.
     *
     * @return String datatype
     */
    public String getName()
    {
        return "MBTS";
    }

    /**
     * Returns the name of the datatype.
     *
     * @return String datatype
     */
    public String getNameScreenName()
    {
        return "MBTS";
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
     * Draws the MBTS data in the y-x projection.
     *
     * @return ACoord cell geometry polygons
     */
    protected ACoord getYXUser()
    {
        makeDrawList();

        int mode = parameterStore.get("YX", "Mode").getI();
        if (mode != AProjectionYX.MODE_MBTS)
        {
            return ACoord.NO_DATA;
        }
        
        double[][][] hv = new double[2][numDraw][];
        int[] index = new int[numDraw];
        for (int i = 0; i < numDraw; i++)
        {
            int j = listdl[i];
            double rMax = MBTSlayers[sampling[j]].getRMax();
            double rMin = MBTSlayers[sampling[j]].getRMin();
            int numPhi = MBTSlayers[sampling[j]].getNumPhi();
            double phiMin = Math.PI*2*phi[j]/numPhi;
            double phiMax = Math.PI*2*(1+phi[j])/numPhi;
            hv[0][i] = new double[] {rMax*Math.cos(phiMin), rMax*Math.cos(phiMax),
                                  rMin*Math.cos(phiMax), rMin*Math.cos(phiMin)};
            hv[1][i] = new double[] {rMax*Math.sin(phiMin), rMax*Math.sin(phiMax),
                                  rMin*Math.sin(phiMax), rMin*Math.sin(phiMin)};
            index[i] = j;
        }

        return new ACoord(hv, index, this);
    }

    /**
     * Draws the MBTS data in the rho-z projection. See the description of
     * filterDrawListRZ() for an explanation of the drawing method.
     *
     * @return ACoord cell geometry polygons
     */
    protected ACoord getRZUser()
    {
        makeDrawList();

        double[][][] hv = new double[2][numDraw][];
        int[] index = new int[numDraw];
        
        for (int i = 0; i < numDraw; i++)
        {
            int j = listdl[i];
            int sign = MBTSlayers[sampling[j]].getRSign((int) phi[j]);
            double zMax = MBTSlayers[sampling[j]].getZMax() * side[j];
            double zMin = MBTSlayers[sampling[j]].getZMin() * side[j];
            double rMax = MBTSlayers[sampling[j]].getRMax() * sign;
            double rMin = MBTSlayers[sampling[j]].getRMin() * sign;            
            hv[0][i] = new double[] {zMax, zMin, zMin, zMax};
            hv[1][i] = new double[] {rMax, rMax, rMin, rMin};
            index[i] = j;
        }

        return new ACoord(hv, index, this);
    }
        
    /**
     * Applies cuts to the data.
     */
    protected void applyCuts()
    {
        super.applyCuts();
        
        cut("CutsCalo", "MBTSEnergy", "MBTSEnergy", et);
        int cutSub = parameterStore.get("CutsCalo", "MBTS").getI();
        if(cutSub != -1)
        {
            cutArray(sub, cutSub, "Endcap");
        }
    }

    /**
     * Returns the data in the phi-rho projection.
     *
     * @return ACoord polygons representing calorimeter cells
     */
    public ACoord getFRUser()
    {
        return getYXUser().convertYXToFR().includePhiWrapAround("FR");
    }
        
    /**
     * Returns info about a selected hit (for picking)
     *
     * @param index int hit index
     * @return String info
     */
    public String getHitInfo(int index)
    {
    	int simpleOutput = AGlobals.instance().getSimpleOutput();
    	if(simpleOutput>0) return getNameScreenName()+" index: " + index+
        "\n Energy="+correctEnergy(energy[index])+" GeV\n ";
        
        //find the cell energy on the opposite MBTS layer
        String cellminus, cellplus;
        double tempEnergy=0.0;
        for(int i=0; i<energy.length; i++)
        {
            if(eta[i]==(-1.0*eta[index]) && phi[i]==phi[index] && sampling[i]==sampling[index])
            {
                tempEnergy=energy[i];
            }
        }
            
        if(eta[index]<0)
        {
            cellminus = correctEnergy(energy[index]);
            cellplus = correctEnergy(tempEnergy);
        }
        else
        {
            cellplus = correctEnergy(energy[index]);
            cellminus = correctEnergy(tempEnergy);
        }
        StringBuffer msg = new StringBuffer(CALORIMETER_NAME + " cell ");
        
        // calling AIdHelper.getFullIdentifier(id[index])[0] - does not work,
        // id array is not present in the event data. it is arbitrarily created
        // in Atlantis, however, but contains only indices, thus currently
        // (2008-11-25) index and id[index] are the same values
        msg.append(" (id: " + id[index] + " index: " + index + ")");
        
        msg.append("\n Energy = " + correctEnergy(energy[index]));
        msg.append("\n (MBTS + = " + cellplus + " MBTS - = " + cellminus + ")");
        msg.append("\n time = " + time[index]);
        msg.append("\n quality = " + quality[index]);
        msg.append("\n type = " + String.format("%.4f",eta[index]));
        msg.append("\n module = " + String.format("%.1f",phi[index]));
        msg.append("\n sampling = " + sampling[index]);
        msg.append("\n channel = " + channel[index]);

        // real pulse shapes plots
        msg.append("\n cell pedestal = ");
        String m = (cellPedestal != null) ? Float.toString(cellPedestal[index]) +
                   " ADC counts" : "n/a";
        msg.append(m);
        
        m = pulseShapesDataAvailable ? "" : "\n data for real pulse shapes plot n/a";
        msg.append(m);
        
        return msg.toString();

    } // getHitInfo() -------------------------------------------------------

    private String correctEnergy(double energy)
    {
        String energyOutput;
        if(energy>1.)
        {
            energyOutput = String.format("%.2f",energy) + " MeV";
        }
        else if(energy>0.001)
        {
            energy*=1000.;
            energyOutput = String.format("%.2f",energy) + " KeV";
        }
        else
        {
            energy*=1000.*1000.;
            energyOutput = String.format("%.2f",energy) + " eV";
        }
        return energyOutput;
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
        //TODO add MBTS RZ Histograms option turned off in gui
        //if (projection instanceof AProjectionRZ)
        //    data = getRZHistograms();
        return projection.nonLinearTransform(data);
    }


    
    // calculate real pulse shapes values based on the values in the lookup tables
    // taken from ATILEData. This function can't be shared from ACalorimeterData
    // (shared by LAr, FCAL, HEC) since the calculation is different and can't
    // call it directly from TILE since there are two channels.
    // Should likely be refactored (so that method is somewhat shared) - after
    // discussion with someone from TILE/MBTS/LAr ...
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
        // for each cell - adcCounts.length), shall be the same for all the
        // cells - taking the length of the first array (for the first cell)
        // no NullPointer or ArrayIndexBound checks are necessary here as this
        // method shall only be called when the relevant data is available
        double centerOfSamples = (adcCounts[0].length + 1.0) / 2.0;

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
            String m = "AMBTSData.getPhysicsPulseShape():\n" +
                       "  lookup index out of bound: lookup = " + lookup;
            throw new AAtlantisException(m);
        }
        return (xpulse * pmtEnergy) + pmtPedestal;

    } // getPhysicsPulseShape() ---------------------------------------------
   
   
    
    /**
     * Utility method APulseShapePlot.plotRealPulseShapes() is called from
     * here.
     * Physics pulse is calculated here: getPhysicsPulseShape().
     * This method is called from APickInteraction via ACalorimeterData
     * which defines abstract plotPulseShapes().
     * Method more or less copied & pasted from ATILEData, but here is 
     * for just one channel.
     *
     * @param index int
     */
    public void plotPulseShapes(int index)
    {
        if(pulseShapesDataAvailable)
        {
            String title = CALORIMETER_NAME + " cell index: " + index;

            // one channel for MBTS
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
                               "can't plot pulse shapes.", ALogInterface.WARNING);
                return;
            }
            

            // step - starting from 1, need to get step numbers within
            // number of ADC samples (adcCounts[0].length)
            double step = (adcCounts[0].length - 1) / (float) NUMBER_OF_LOOKUP_VALUES;
            double[][] realPulse = new double[1][NUMBER_OF_LOOKUP_VALUES]; // 1 channel
            double d = 1.0;
            
            // by zdenek 2008-12-02
            // DPD (slimed version of ESD) sometimes don't have cellPedestal
            // for a cell (the tag exists, values are there, but are 0). check
            // if pedestal is 0 and if so, take as pedestal value the first
            // adc count digit
            float cellPedestalLocal = cellPedestal[index]; 
            if(cellPedestalLocal == 0.0f)
            {
                logger.debug("MBTS cellPedestal is not available (i.e. " +
                             "are 0.0f), using first adcCount digit as pedestal.");
                cellPedestalLocal = adcCounts[index][0];
            }            
            
            try
            {
                for(int i = 0; i < NUMBER_OF_LOOKUP_VALUES; i++)
                {
                    d += step;
                    realPulse[0][i] =
                        this.getPhysicsPulseShape(d, cellRawTime[index],
                                             cellPedestalLocal,
                        		             cellRawAmplitude[index],
                                             ACalorimeterRPSPLT.TILE_AMPLITUDE,
                                             ACalorimeterRPSPLT.TILE_TIME);
                }
            }
            catch(AAtlantisException aaex)
            {
                AOutput.append(aaex.getMessage(), ALogInterface.WARNING);
                return;
            }

            APulseShapePlot.plotRealPulseShapes(adcCountsLocal, realPulse,
                                                step, null, title);
        }
        else
        {
        	logger.warn(CALORIMETER_NAME + " plotPulseShapes() method called, " +
        			    "but data is not available.");
            return;
        }

    } // plotPulseShapes() --------------------------------------------------
    
     

}
