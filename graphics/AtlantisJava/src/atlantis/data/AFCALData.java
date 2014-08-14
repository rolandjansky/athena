package atlantis.data;

import atlantis.canvas.ACanvas;
import atlantis.event.AEvent;
import atlantis.geometry.ACalorimeterDetector;
import atlantis.globals.AGlobals;
import atlantis.graphics.ACoord;
import atlantis.output.ALogInterface;
import atlantis.output.AOutput;
import atlantis.parameters.AParameterUtilities;
import atlantis.projection.AProjection;
import atlantis.projection.AProjection2D;
import atlantis.projection.AProjectionRZ;
import atlantis.projection.AProjectionVP;
import atlantis.projection.AProjectionYX;
import atlantis.utils.AAtlantisException;
import atlantis.utils.AHashMap;
import atlantis.utils.AIdHelper;
import atlantis.utils.AMath;
import atlantis.utils.AUtilities;
import atlantis.utils.ALogger;
import java.io.IOException;
import com.Ostermiller.util.CSVParser;
import java.io.StringReader;
import java.io.InputStream;
import java.io.FileNotFoundException;

/**
 * The Forward calorimeter implementation.
 *
 * @author Eric Jansen
 */
public class AFCALData extends ACalorimeterData
{
    private static ALogger logger = ALogger.getLogger(AFCALData.class);
    
    protected float[] x;
    protected float[] y;
    protected float[] z;
    protected float[] r;
    protected float[] dx;
    protected float[] dy;
    protected float[] dz;
    protected float[] dr;

    private int[][] cellTable;

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
        		"rpsplt_fcal.csv";
    // number of lookup table values for real pulse shapes plots calculation
    private static final short NUMBER_OF_LOOKUP_VALUES = 600;



    AFCALData(AHashMap p, AEvent e)
    {
        super(p,e);

        x = p.getFloatArray("x");
        dx = p.getFloatArray("dx");
        y = p.getFloatArray("y");
        dy = p.getFloatArray("dy");

        r = new float[numData];
        dr = new float[numData];

        cellTable = new int[MAX_HIT_INDEX][MAX_HIT_INDEX];

        for (int i = 0; i < numData; i++)
        {
            try
            {
                side[i] = (byte) AIdHelper.larBarrelEndcap(id[i]);
                etaIndex[i] = (short) AIdHelper.larEta(id[i]);
                phiIndex[i] = (short) AIdHelper.larPhi(id[i]);
                sampling[i] = 0; //AIdHelper.larSampling(id[i]);
            }
            catch (AAtlantisException ex)
            {
                logger.error("Problem decoding ID " + id[i] + " in " +
                        CALORIMETER_NAME + ": " + ex.getMessage());
                side[i] = 0;
                etaIndex[i] = -1;
                phiIndex[i] = -1;
                sampling[i] = -1;
            }
        }

        // Events produced by older JiveXML versions don't have z and dz yet.
        // Use the old hardcoded values in that case for backwards
        // compatibility.
        if (p.get("z") == null || p.get("dz") == null)
        {
            z = new float[numData];
            dz = new float[numData];
            for (int i = 0; i < numData; i++)
            {
                switch (sampling[i])
                {
                    case 0:
                        z[i] = (side[i] / Math.abs(side[i])) * 489.4f;
                        dz[i] = 45.2f;
                        break;
                    case 1:
                        z[i] = (side[i] / Math.abs(side[i])) * 534.8f;
                        dz[i] = 45.0f;
                        break;
                    case 2:
                        z[i] = (side[i] / Math.abs(side[i])) * 582.2f;
                        dz[i] = 45.0f;
                        break;
                }
            }
        }
        else
        {
            z = p.getFloatArray("z");
            dz = p.getFloatArray("dz");
        }

        float drMin = 0.0f, zMin = 0.0f, zMax = 0.0f, rMin = 0.0f, rMax = 0.0f;
        for (int i = 0; i < numData; i++)
        {

            if (side[i] < 0)
            {
                sub[i] = 0;
            }
            else
            {
                sub[i] = 1;
            }

            float etaMin = (float) AMath.etaAbs(z[i], Math.sqrt(Math.pow(Math.abs(x[i]) - dx[i], 2) +
                    Math.pow(Math.abs(y[i]) - dy[i], 2)));
            float etaMax = (float) AMath.etaAbs(z[i], Math.sqrt(Math.pow(Math.abs(x[i]) + dx[i], 2) +
                    Math.pow(Math.abs(y[i]) + dy[i], 2)));
            eta[i] = (etaMin + etaMax) / 2.f;
            //TODO check calculation original: deta[i] = etaMax - etaMin;
            deta[i] = Math.abs(etaMax - etaMin);
            phi[i] = (float) Math.atan2(y[i], x[i]);
            dphi[i] = (float) Math.abs(Math.atan2(y[i] + dy[i], x[i] - dx[i]) -
                    Math.atan2(y[i] - dy[i], x[i] + dx[i]));
            if (phi[i] < 0)
            {
                phi[i] += 2. * Math.PI;
            }

            r[i] = (float) Math.sqrt(x[i] * x[i] + y[i] * y[i]);
            dr[i] = (float) Math.sqrt(dx[i] * dx[i] + dy[i] * dy[i]);

            // Collect some constants needed for binning the histograms.
            if (rMin == 0.0 || Math.abs(r[i]) - dr[i] / 2. < rMin)
            {
                rMin = Math.abs(r[i]) - dr[i] / 2.f;
            }
            if (rMax == 0.0 || Math.abs(r[i]) + dr[i] / 2. > rMax)
            {
                rMax = Math.abs(r[i]) + dr[i] / 2.f;
            }
            if (zMax == 0.0 || Math.abs(z[i]) + dz[i] / 2. > zMax)
            {
                zMax = Math.abs(z[i]) + dz[i] / 2.f;
            }
            if (zMin == 0.0 || Math.abs(z[i]) - dz[i] / 2. < zMin)
            {
                zMin = Math.abs(z[i]) - dz[i] / 2.f;
            }
            if (drMin == 0.0 || dr[i] < drMin)
            {
                drMin = dr[i];
            }
        }

        etaGranularity = AParameterUtilities.eta(zMin, rMax - drMin) - AParameterUtilities.eta(zMin, rMax);
        outerEta = AParameterUtilities.eta(zMax, rMin);

        // Collect some constants needed for drawing the histograms.
        for (int i = 0; i < ACalorimeterDetector.count(); i++)
        {
            if (outerR == 0.0 || ACalorimeterDetector.get(i).getRMax() > outerR)
            {
                outerR = ACalorimeterDetector.get(i).getRMax();
            }
            if (outerZ == 0.0 || ACalorimeterDetector.get(i).getZMax() > outerZ)
            {
                outerZ = ACalorimeterDetector.get(i).getZMax();
            }
        }

        for (int i = 0; i < et.length; ++i)
        {
            et[i] = Math.abs(energy[i] / (float) Math.cosh(eta[i]));
        }

        // read in FCAL data from real pulse shapes plots
        readPulseShapePlotData(p);

    }



    /**
     * readLookupTableFile() reads in comma separated values (CSV) file
     * with FCAL real pulse shapes plots time and amplitude lookup values
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

                if("FCAL_LAYER0_AMPLITUDE".equals(arrayName))
                {
                    ACalorimeterRPSPLT.FCAL_LAYER0_AMPLITUDE =
                        getLookupTableArray(s, NUMBER_OF_LOOKUP_VALUES);
                }
                else if("FCAL_LAYER1_AMPLITUDE".equals(arrayName))
                {
                    ACalorimeterRPSPLT.FCAL_LAYER1_AMPLITUDE =
                        getLookupTableArray(s, NUMBER_OF_LOOKUP_VALUES);
                }
                else if("FCAL_LAYER2_AMPLITUDE".equals(arrayName))
                {
                    ACalorimeterRPSPLT.FCAL_LAYER2_AMPLITUDE =
                        getLookupTableArray(s, NUMBER_OF_LOOKUP_VALUES);
                }
                else if("FCAL_TIME".equals(arrayName))
                {
                    ACalorimeterRPSPLT.FCAL_TIME =
                        getLookupTableArray(s, NUMBER_OF_LOOKUP_VALUES);
                }
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


            if(ACalorimeterRPSPLT.areFCALLookupTablesInitialized())
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
     * Returns the name of the parameter group.
     *
     * @return String parameter group
     */
    public String getParameterGroup()
    {
        return "FCAL";
    }

    /**
     * Returns the name of the datatype.
     *
     * @return String datatype
     */
    public String getName()
    {
        return "FCAL";
    }

    /**
     * Returns the name of the datatype.
     *
     * @return String datatype
     */
    public String getNameScreenName()
    {
        return "FCAL";
    }

    /**
     * Returns the type of calorimeter (ECAL/HCAL) for a hit.
     *
     * @param index int hit index
     * @return String calorimeter type
     */
    public String getCalorimeterType(int index)
    {
        if (sampling[index] == 0)
        {
            return "ECAL";
        }
        else
        {
            return "HCAL";
        }
    }

    /**
     * Calorimeter cells can be drawn either explicitly or implicitly. In most
     * views cells that are located behind eachother are consolidated and only
     * the most energetic one is drawn explicitly. The other cells are drawn
     * implicitly, since their energy value is still added to the energy sum of
     * the explicitly drawn cell. The list manager does not know about the
     * implicitly drawn cells though, so this poses a problem when the user
     * requests information about a certain area of the canvas. This is solved
     * using this method. This function receives the list of explicitly drawn
     * cells and then adds the ones that were implicitly drawn in this
     * selection.
     *
     * @param drawn boolean[] initial drawn list containing only the explicitly
     *        drawn cells
     * @return boolean[] drawn list containing the explicitly and the implicitly
     *         drawn cells.
     */
    public boolean[] expandDrawn(boolean[] drawn)
    {
        AProjection projection = ACanvas.getCanvas().getCurrentWindow().getProjection();

        if (projection instanceof AProjectionYX)
        {
            return expandDrawnYX(drawn);
        }
        else if (projection instanceof AProjectionRZ)
        {
            return expandDrawnRZ(drawn);
        }
        else
        {
            return drawn;
        }
    }

    /**
     * Filters the drawList for the y-x projection. Hits from both endcaps are
     * combined.
     */
    protected void filterDrawListYX()
    {
        float etTable[][] = new float[MAX_HIT_INDEX][MAX_HIT_INDEX];
        for (int i = 0; i < MAX_HIT_INDEX; i++)
        {
            for (int j = 0; j < MAX_HIT_INDEX; j++)
            {
                cellTable[i][j] = -1;
                etTable[i][j] = 0.0f;
            }
        }

        for (int i = 0; i < numDraw; i++)
        {
            int j = listdl[i];
            int eta = etaIndex[j];
            int phi = phiIndex[j];
            etTable[eta][phi] += et[j];
            if (cellTable[eta][phi] < 0 || et[cellTable[eta][phi]] < et[j])
                cellTable[eta][phi] = j;
        }

        numDraw = 0;
        for (int i = 0; i < MAX_HIT_INDEX; i++)
        {
            for (int j = 0; j < MAX_HIT_INDEX; j++)
            {
                if (cellTable[i][j] > 0)
                {
                    listdl[numDraw] = cellTable[i][j];
                    etSum[listdl[numDraw]] = etTable[i][j];
                    numDraw++;
                }
            }
        }
    }

    /**
     * See expandDrawn.
     *
     * @param drawn boolean[] initial drawn list containing only the explicitly
     *        drawn cells
     * @return boolean[] drawn list containing the explicitly and the implicitly
     *         drawn cells.
     */
    private boolean[] expandDrawnYX(boolean[] drawn)
    {
        makeDrawList();

        int mode = parameterStore.get("YX", "Mode").getI();
        if (parameterStore.get("YX", "DrawFCAL").getStatus() && mode >= AProjectionYX.MODE_HEC_2 &&
                mode <= AProjectionYX.MODE_HEC_4)
        {
            cutArray(sampling, mode - AProjectionYX.MODE_HEC_2, "Layer");
        }
        else if (mode >= AProjectionYX.MODE_FCAL_EM || mode <= AProjectionYX.MODE_FCAL_HAD_2)
        {
            cutArray(sampling, mode - AProjectionYX.MODE_FCAL_EM, "Layer");
        }
        else
        {
            return drawn;
        }

        for (int i = 0; i < numDraw; i++)
        {
            int j = listdl[i];

            if (drawn[j])
                continue;

            int eta = etaIndex[j];
            int phi = phiIndex[j];

            if (eta >= 0 && phi >= 0)
            {
                int mappedTo = cellTable[eta][phi];

                if (mappedTo >= 0 && drawn[mappedTo] == true)
                {
                    drawn[j] = true;
                }
            }
        }

        return drawn;
    }

    /**
     * The rho-z projection is a bit difficult for FCAL. Because of the y-x
     * geometry of the cells there isn't really a nice way to project them in
     * rho-z. This prodedure takes the most energetic cell and the consolidates
     * it with all cells that overlap it. This is repeated until all cells are
     * assigned to such a most energetic cell. We are then left with a series of
     * "important" cells that do not overlap in rho-z. These can be drawn
     * nicely.
     */
    protected void filterDrawListRZ()
    {
        boolean available[] = new boolean[numDraw];
        int numCells = numDraw;
        int newNumDraw = 0;

        // Initialize the variables.
        for (int i = 0; i < numDraw; i++)
        {
            available[i] = true;
        }
        for (int i = 0; i < numData; i++)
        {
            etSum[i] = 0.0f;
        }

        while (numCells > 0)
        {

            // Loop through all available cells and find the highest energy.
            int max = -1;
            for (int i = 0; i < numDraw; i++)
            {
                if (available[i])
                {
                    if (max < 0 || et[listdl[i]] > et[listdl[max]])
                        max = i;
                }
            }

            // This is a cell we want to draw, store it and remove it from the
            // available cells list.
            available[max] = false;
            numCells--;
            listdl[newNumDraw++] = listdl[max];

            // Now find all available cells that overlap this one. This loop
            // will be processed
            // quite a lot, so it is very important to do the "easy" rejects
            // first.
            for (int i = 0; i < numDraw; i++)
            {
                if (available[i])
                {
                    int j = listdl[i];

                    double zDist = Math.abs(z[j] - z[listdl[max]]);
                    if (zDist >= (dz[j] + dz[listdl[max]]) / 2.)
                        continue;

                    double rDist = Math.abs(r[j] - r[listdl[max]]);
                    if (rDist >= (dr[j] + dr[listdl[max]]) / 2.)
                        continue;

                    if (AParameterUtilities.getRhoSign(x[j], y[j]) != AParameterUtilities.getRhoSign(x[listdl[max]], y[listdl[max]]))
                        continue;

                    available[i] = false;
                    etSum[listdl[max]] += et[j];
                    numCells--;
                }
            }
        }

        numDraw = newNumDraw;
    }

    /**
     * See expandDrawn.
     *
     * @param drawn boolean[] initial drawn list containing only the explicitly
     *        drawn cells
     * @return boolean[] drawn list containing the explicitly and the implicitly
     *         drawn cells.
     */
    private boolean[] expandDrawnRZ(boolean[] drawn)
    {
        makeDrawList();

        boolean available[] = new boolean[numDraw];
        int numCells = numDraw;

        // Initialize the variables.
        for (int i = 0; i < numDraw; i++)
        {
            available[i] = true;
        }

        while (numCells > 0)
        {

            // Loop through all available cells and find the highest energy.
            int max = -1;
            for (int i = 0; i < numDraw; i++)
            {
                if (available[i])
                {
                    if (max < 0 || et[listdl[i]] > et[listdl[max]])
                        max = i;
                }
            }

            // This is a cell we want to draw, store it and remove it from the
            // available cells list.
            available[max] = false;
            numCells--;

            // Now find all available cells that overlap this one. This loop
            // will be processed
            // quite a lot, so it is very important to do the "easy" rejects
            // first.
            for (int i = 0; i < numDraw; i++)
            {
                if (available[i])
                {
                    int j = listdl[i];

                    double zDist = Math.abs(z[j] - z[listdl[max]]);
                    if (zDist >= (dz[j] + dz[listdl[max]]) / 2.)
                        continue;

                    double rDist = Math.abs(r[j] - r[listdl[max]]);
                    if (rDist >= (dr[j] + dr[listdl[max]]) / 2.)
                        continue;

                    if (AParameterUtilities.getRhoSign(x[j], y[j]) != AParameterUtilities.getRhoSign(x[listdl[max]], y[listdl[max]]))
                        continue;

                    available[i] = false;
                    if (drawn[listdl[max]])
                        drawn[j] = true;
                    numCells--;
                }
            }
        }

        return drawn;
    }

    /**
     * Draws the FCAL data in the y-x projection.
     *
     * @return ACoord cell geometry polygons
     */
    protected ACoord getYXUser()
    {
        makeDrawList();

        int mode = parameterStore.get("YX", "Mode").getI();
        if (parameterStore.get("YX", "DrawFCAL").getStatus() && mode >= AProjectionYX.MODE_HEC_2 &&
                mode <= AProjectionYX.MODE_HEC_4)
        {
            cutArray(sampling, mode - AProjectionYX.MODE_HEC_2, "Layer");
        }
        else if (mode >= AProjectionYX.MODE_FCAL_EM || mode <= AProjectionYX.MODE_FCAL_HAD_2)
        {
            cutArray(sampling, mode - AProjectionYX.MODE_FCAL_EM, "Layer");
        }
        else
        {
            return ACoord.NO_DATA;
        }

        filterDrawListYX();

        double[][][] hv = new double[2][numDraw][];
        int[] index = new int[numDraw];

        for (int i = 0; i < numDraw; i++)
        {
            int j = listdl[i];
            double xMin = x[j] - dx[j] / 2.;
            double xMax = x[j] + dx[j] / 2.;
            double yMin = y[j] - dy[j] / 2.;
            double yMax = y[j] + dy[j] / 2.;

            hv[0][i] = new double[] { xMax, xMin, xMin, xMax };
            hv[1][i] = new double[] { yMax, yMax, yMin, yMin };

            index[i] = j;
        }

        return new ACoord(hv, index, this);
    }

    /**
     * Draws the FCAL data in the rho-z projection. See the description of
     * filterDrawListRZ() for an explanation of the drawing method.
     *
     * @return ACoord cell geometry polygons
     */
    protected ACoord getRZUser()
    {
        makeDrawList();
        filterDrawListRZ();

        double[][][] hv = new double[2][numDraw][];
        int[] index = new int[numDraw];

        for (int i = 0; i < numDraw; i++)
        {
            int j = listdl[i];

            double zMin = z[j] - dz[j] / 2.;
            double zMax = z[j] + dz[j] / 2.;
            double rMin = Math.sqrt(Math.pow(Math.abs(x[j]) - Math.abs(dx[j] / 2.), 2) + Math.pow(Math.abs(y[j]) - Math.abs(dy[j] / 2.), 2));
            double rMax = Math.sqrt(Math.pow(Math.abs(x[j]) + Math.abs(dx[j] / 2.), 2) + Math.pow(Math.abs(y[j]) + Math.abs(dy[j] / 2.), 2));

            double phiMid = Math.toRadians(parameterStore.get("RZ", "Phi").getD());
            double phiDiff = Math.abs(phi[j] - phiMid);
            if (phiDiff > Math.PI / 2. && phiDiff <= 3 * Math.PI / 2.)
            {
                rMin *= -1;
                rMax *= -1;
            }

            hv[0][i] = new double[] { zMax, zMin, zMin, zMax };
            hv[1][i] = new double[] { rMax, rMax, rMin, rMin };

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
        cut("CutsCalo", "FCALET", "FCALET", et);
        cut("CutsCalo", "FCALEnergy", "FCALEnergy", energy);

        int cutSub = parameterStore.get("CutsCalo", "FCAL").getI();
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
     * Returns the data in the phi-eta projection.
     *
     * @return ACoord polygons representing calorimeter cells
     */
    protected ACoord getVPUser()
    {
        makeDrawList();

        switch (parameterStore.get("VP", "Mode").getI())
        {
            case AProjectionVP.MODE_ECAL_LAYER_0:
            case AProjectionVP.MODE_ECAL_LAYER_1:
            case AProjectionVP.MODE_ECAL_LAYER_2:
            case AProjectionVP.MODE_ECAL_LAYER_3:
                cutArray(sampling, 0, "EM");
                break;
            case AProjectionVP.MODE_HCAL_LAYER_0:
            case AProjectionVP.MODE_HCAL_LAYER_1:
            case AProjectionVP.MODE_HCAL_LAYER_2:
            case AProjectionVP.MODE_HCAL_LAYER_3:
                cutArrayOR(sampling, 1, 2, "Hadronic");
                break;
            default:
                return ACoord.NO_DATA;
        }

        double[][][] hv = new double[2][numDraw][4];
        int[] index = new int[numDraw];

        for (int i = 0; i < numDraw; i++)
        {
            int j = listdl[i];
            etSum[j] = et[j];

            double eta0 = AMath.etaAbs(z[j], (float) Math.sqrt(Math.pow(x[j] + dx[j] / 2., 2) + Math.pow(y[j] + dy[j] / 2., 2)));
            double eta1 = AMath.etaAbs(z[j], (float) Math.sqrt(Math.pow(x[j] - dx[j] / 2., 2) + Math.pow(y[j] + dy[j] / 2., 2)));
            double eta2 = AMath.etaAbs(z[j], (float) Math.sqrt(Math.pow(x[j] - dx[j] / 2., 2) + Math.pow(y[j] - dy[j] / 2., 2)));
            double eta3 = AMath.etaAbs(z[j], (float) Math.sqrt(Math.pow(x[j] + dx[j] / 2., 2) + Math.pow(y[j] - dy[j] / 2., 2)));

            // Calculate phi between 0 and 360.
            double phi0 = Math.toDegrees(Math.atan2(y[j] + dy[j] / 2., x[j] + dx[j] / 2.) + 360.) % 360.;
            double phi1 = Math.toDegrees(Math.atan2(y[j] + dy[j] / 2., x[j] - dx[j] / 2.) + 360.) % 360.;
            double phi2 = Math.toDegrees(Math.atan2(y[j] - dy[j] / 2., x[j] - dx[j] / 2.) + 360.) % 360.;
            double phi3 = Math.toDegrees(Math.atan2(y[j] - dy[j] / 2., x[j] + dx[j] / 2.) + 360.) % 360.;

            // Some code to prevent problems with cells going across the 0/360
            // border.
            if (phi1 - phi0 > 180.)
                phi1 -= 360.;
            else if (phi0 - phi1 > 180.)
                phi1 += 360;
            if (phi2 - phi1 > 180.)
                phi2 -= 360.;
            else if (phi1 - phi2 > 180.)
                phi2 += 360;
            if (phi3 - phi2 > 180.)
                phi3 -= 360.;
            else if (phi2 - phi3 > 180.)
                phi3 += 360;

            hv[0][i] = new double[] { eta0, eta1, eta2, eta3 };
            hv[1][i] = new double[] { phi0, phi1, phi2, phi3 };

            index[i] = j;
        }

        return new ACoord(hv, index, this).includePhiWrapAround("VP");
    }

    /**
     * Returns the histograms in the RZ projection.
     *
     * @return ACoord[] histograms
     */
    public ACoord[] getRZHistograms()
    {
        AHistogram histUp = new AHistogram(-outerEta, outerEta, etaGranularity, histoScale.getD(), this);
        AHistogram histDown = new AHistogram(-outerEta, outerEta, etaGranularity, histoScale.getD(), this);

        for (int i = 0; i < numDraw; i++)
        {
            int j = listdl[i];
            int sign = AParameterUtilities.getRhoSign(x[j], y[j]);
            double etaMin;
            double etaMax;

            if (side[j] < 0)
            {
                etaMin = AParameterUtilities.eta(z[j] + dz[j] / 2.f, r[j] - dr[j] / 2.f);
                etaMax = AParameterUtilities.eta(z[j] - dz[j] / 2.f, r[j] + dr[j] / 2.f);
            }
            else
            {
                etaMin = AParameterUtilities.eta(z[j] - dz[j] / 2.f, r[j] + dr[j] / 2.f);
                etaMax = AParameterUtilities.eta(z[j] + dz[j] / 2.f, r[j] - dr[j] / 2.f);
            }

            if (sign > 0)
            {
                histUp.fill(etaMin, etaMax, etSum[j]);
            }
            else
            {
                histDown.fill(etaMin, etaMax, etSum[j]);
            }
        }

        ACoordArray c = new ACoordArray();
        c.add(histUp.getRZUser(outerZ, outerR, AHistogram.UP));
        c.add(histDown.getRZUser(outerZ, outerR, AHistogram.DOWN));
        return c.getArray();
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
//     *
//     * @param index int
//     * @return String
//     */
    /*
    private float getADC2MevFactorPredefined(int index)
    {
        int layer = sampling[index];
        float r = Float.NaN;
        switch(layer)
        {
            case 0: r = 87.0f;
                    break;
            case 1: r = 117.0f;
                    break;
            case 2: r = 193.0f;
                    break;
        }

        return r;

    } // getADC2MevFactorPredefined() ---------------------------------------
     */


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
        cellInfo += "\n x  = " + x[index] + " cm";
        cellInfo += "\n y  = " + y[index] + " cm";

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
                // (was printing the predefined value) using predefined " +
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

    /**
     * Recalculates eta from r and z in case the user wants to have their eta
     * with respect to the primary vertex instead of 0. The calculation is a
     * little different from the other calorimeter classes because this one
     * contains both barrel and endcap data.
     *
     * @param index int index of hit in data
     * @return double eta value
     */
    public double getEta(int index)
    {
        if (!parameterStore.get("VP", "EtaVertex").getStatus())
        {
            return this.eta[index];
        }

        return AParameterUtilities.eta(z[index], Math.sqrt(x[index] * x[index] + y[index] * y[index]));
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
        if (projection instanceof AProjectionRZ)
            data = getRZHistograms();
        return projection.nonLinearTransform(data);
    }



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
                AOutput.append("\nADC counts are not available for this cell, " +
                               "can't plot pulse shapes.", ALogPane.WARNING);
                return;
            }
            
            
            float[] time = ACalorimeterRPSPLT.FCAL_TIME; // lookup tables
            float[] amplitude = null; // lookup tables
            switch(sampling[index])
            {
                // use correct amplitude lookup table for this layer (sampling)
                case 0:  amplitude = ACalorimeterRPSPLT.FCAL_LAYER0_AMPLITUDE;
                         break;
                case 1:  amplitude = ACalorimeterRPSPLT.FCAL_LAYER1_AMPLITUDE;
                         break;
                case 2:  amplitude = ACalorimeterRPSPLT.FCAL_LAYER2_AMPLITUDE;
                         break;
                default: AOutput.append("\nADC counts plot only, pulse shape " +
                                        "plot not implemented for " +
                                        CALORIMETER_NAME + " layer " +
                                        sampling[index] + "\n", ALogPane.WARNING);
                         APulseShapePlot.plotADCCounts(adcCountsLocal, title, null);
                         return;
            }

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

        float[] time = ACalorimeterRPSPLT.FCAL_TIME; // lookup tables
            float[] amplitude = null; // lookup tables
            switch(sampling[index])
            {
                // use correct amplitude lookup table for this layer (sampling)
                case 0:  amplitude = ACalorimeterRPSPLT.FCAL_LAYER0_AMPLITUDE;
                         break;
                case 1:  amplitude = ACalorimeterRPSPLT.FCAL_LAYER1_AMPLITUDE;
                         break;
                case 2:  amplitude = ACalorimeterRPSPLT.FCAL_LAYER2_AMPLITUDE;
                         break;
                default: AOutput.append("\nADC counts plot only, pulse shape " +
                                        "plot not implemented for " +
                                        CALORIMETER_NAME + " layer " +
                                        sampling[index] + "\n", ALogInterface.WARNING);
                         return null;
            }

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
                return null;
            }

            if(factor != -1)
            {
            	logger.debug("adc2Mev factor available, plotting full plot.");
                return realPulse;
            }
            else
            {
            	logger.debug("adc2Mev factor not available, plotting just adc counts.");
                return null;          	
            }

    }

    @Override
    protected double getPulseStep(int index) {
        return (adcCounts[0].length - 1) / (float) NUMBER_OF_LOOKUP_VALUES;
    }
}
