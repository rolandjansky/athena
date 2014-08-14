package atlantis.data;

import atlantis.event.AEvent;
import java.awt.Color;
import java.util.Vector;

import atlantis.canvas.ACanvas;
import atlantis.canvas.ADrawCalorimeterSummedEndcaps;
import atlantis.canvas.ALegendWindow;
import atlantis.canvas.AWindow;
import atlantis.event.AData;
import atlantis.geometry.ABarrelCalorimeterDetector;
import atlantis.geometry.ACalorimeterDetector;
import atlantis.geometry.AEndcapCalorimeterDetector;
import atlantis.globals.AGlobals;
import atlantis.graphics.ACoord;
import atlantis.graphics.ADrawParameters;
import atlantis.graphics.ADrawnGraphics2D;
import atlantis.graphics.AGraphics;
import atlantis.graphics.colormap.AColorMap;
import atlantis.output.ALogInterface;
import atlantis.output.AOutput;
import atlantis.parameters.AParameter;
import atlantis.parameters.AParameterUtilities;
import atlantis.projection.AProjection;
import atlantis.projection.AProjection2D;
import atlantis.projection.AProjectionFR;
import atlantis.projection.AProjectionFZ;
import atlantis.projection.AProjectionRZ;
import atlantis.projection.AProjectionVP;
import atlantis.projection.AProjectionYX;
import atlantis.utils.A4Vector;
import atlantis.utils.AHashMap;
import atlantis.utils.AIdHelper;
import atlantis.utils.AMath;
import atlantis.utils.APolygon;
import atlantis.utils.AAtlantisException;
import atlantis.utils.ALogger;

/**
 * The superclass of all the ATLAS calorimeters. Contains also methods for
 * drawing groups of calorimeters together. Calorimeters can not be drawn
 * individually due to coupling of energy scale and the standard v-plot picture.
 * Each calorimeter also provides histograms.
 *
 * @author Eric Jansen
 */
public abstract class ACalorimeterData extends AHitData implements AHistogramData
{
    private static ALogger logger = ALogger.getLogger(ACalorimeterData.class);
    
    // Constants as defined in the configuration file.
    // Energy mode options.
    public static final int ENERGY_MODE_SUM_LIN = 0;
    public static final int ENERGY_MODE_MAX_LIN = 1;
    public static final int ENERGY_MODE_SUM_LOG = 2;
    public static final int ENERGY_MODE_MAX_LOG = 3;
    public static final int ENERGY_MODE_SUM_SQRT = 4;
    public static final int ENERGY_MODE_MAX_SQRT = 5;

    // Energy calibration options.
    public static final int ENERGY_CALIB_UNKNOWN = -1;
    public static final int ENERGY_CALIB_OVERALL = 0;
    public static final int ENERGY_CALIB_BY_TYPE = 1;
    public static final int ENERGY_CALIB_BY_CALO = 2;

    // Color functions.
    public static final int COLOR_FUNC_CONSTANT = 0;
    public static final int COLOR_FUNC_SUBDET = 1;
    public static final int COLOR_FUNC_CLUSTER = 2;
    public static final int COLOR_FUNC_SAMPLING = 3;
    public static final int COLOR_FUNC_ENERGY = 4;
    public static final int COLOR_FUNC_JET = 5;
    public static final int COLOR_FUNC_TYPE = 6;
    public static final int COLOR_FUNC_TIME = 7;

    private static final float SCALE_FACTOR_VPLOT = 20;

    public String CALORIMETER_NAME;

    // Properties needed for the histograms.
    protected double innerR;
    protected double outerR;
    protected double innerZ;
    protected double outerZ;
    protected double outerEta;

    protected double phiGranularity = 0.0;
    protected double etaGranularity = 0.0;
    protected AParameter histoScale;

    // This constant contains the highest phi or eta index for this data type.
    // Its value determines the amount of memory that is allocated for certain
    // data structures.
    protected short MAX_HIT_INDEX = 512;
    protected static final double NON_SCALING_FACTOR = 0.85;

    protected float[] phi;
    protected float[] eta;
    protected float[] dphi;
    protected float[] deta;
    protected float[] energy;
    protected float[] et;
    protected float[] etSum;

    // Et is more useful than pure Energy, so use Et to replace Energy
    // protected float[] energySum;

    protected byte[] side;
    protected short[] etaIndex;
    protected short[] phiIndex;
    protected int[] sampling;
    protected short[] detector;

    private int[][] cellTable;

    protected double minDensityECAL;
    protected double maxDensityECAL;
    protected double minDensityHCAL;
    protected double maxDensityHCAL;
    
    // following attributes occur at majority of calorimeter subdetectors
    // which inherit from here, but not at all of them (TILE is usually
    // the exception), checks are always done, so it's save ;
    // retrieving the data from XML is pre-checked so that not to spit out
    // unnecessary complaints
    protected int[] feedThrough = null;
    protected int[] channel = null;
    protected int[] slot = null;
    protected int[] BadCell = null;


    ACalorimeterData(AHashMap p, AEvent e)
    {
        super(p,e);

        CALORIMETER_NAME = getName();
        
        energy = p.getFloatArray("energy");
        et = new float[numData];

        phi = new float[numData];
        eta = new float[numData];
        deta = new float[numData];
        dphi = new float[numData];

        // energySum = new float[numData];
        etSum = new float[numData];

        side = new byte[numData];
        etaIndex = new short[numData];
        phiIndex = new short[numData];
        sampling = new int[numData];
        detector = new short[numData];

        histoScale = parameterStore.get(CALORIMETER_NAME, "HistoScale");
        
        // using AHashMap.getUnsureIntArray() does not complain if key is
        // not found - these parameters may not be present in all calorimetry
        // classes inherited from ACalorimeterData and using this call
        // also covers backwards compatibility, and is safe - null is returned
        // if key does not exist
        feedThrough = p.getUnsureIntArray("feedThrough");
        channel = p.getUnsureIntArray("channel");
        slot = p.getUnsureIntArray("slot");
        BadCell = p.getUnsureIntArray("BadCell");
        
    }

    /**
     * Get the name used for associating this datatype/collection. For the 
     * calorimeter cells no storegate key is used.
     * @return 
     */
    public String getFullName() {
        return getName();
    }

    /**
     * getADCCountsData()
     * reads in adcCounts array from XML data. This method is currently used
     * for LAr, FCAL, HEC and MBTS (TILE has two adcCounts arrays)
     * @param p AHashMap
     */
    protected int[][] getADCCountsData(AHashMap p)
    {
        int[][] adcCounts = null;
        int[] adc = (p.get("adcCounts") != null) ? p.getIntArray("adcCounts") : null;
        if(adc != null)
        {
            adcCounts = new int[numData][0];
            // multiple numbers are associated with each cell
            int multiple = adc.length / numData;
            int num = 0;
            for(int i = 0; i < numData; i++)
            {
                adcCounts[i] = new int[multiple];
                for (int j = 0; j < multiple; j++)
                {
                    adcCounts[i][j] = adc[num]; // fill in array for each cell
                    num++;
                }
            }
        } // if(adc != null)

        return adcCounts;

    } // getADCCountsData() -------------------------------------------------



    /**
     * Class used to merge ACoord[] objects.
     */
    public static class ACoordArray
    {
        private Vector<ACoord> v = new Vector<ACoord>();

        public void add(ACoord[] c)
        {
            for (int i = 0; i < c.length; i++)
            {
                v.addElement(c[i]);
            }
        }

        public ACoord[] getArray()
        {
            ACoord[] c = new ACoord[v.size()];
            for (int i = 0; i < v.size(); i++)
            {
                c[i] = (ACoord) v.elementAt(i);
            }
            return c;
        }
    }



    /**
     * Assigns the hits to a geometry object.
     */
    public void makeHitToGeometryMapping()
    {
        int numNotFound = 0;

        // In detector[] we store the hit-to-geometry object mapping.
        for (int i = 0; i < numData; i++)
        {
            detector[i] = (short) ACalorimeterDetector.getDetectorIndex(id[i]);

            if (detector[i] >= 0)
            {
                eta[i] = (float) ACalorimeterDetector.get(detector[i]).getEta(etaIndex[i], side[i]);
                deta[i] = (float) ACalorimeterDetector.get(detector[i]).getDeltaEta();
                phi[i] = (float) ACalorimeterDetector.get(detector[i]).getPhi(phiIndex[i]);
                dphi[i] = (float) ACalorimeterDetector.get(detector[i]).getDeltaPhi();
            }
            else if (!CALORIMETER_NAME.equals("FCAL"))
            {
                eta[i] = 0.0f;
                deta[i] = 0.0f;
                phi[i] = 0.0f;
                dphi[i] = 0.0f;
                numNotFound++;
            }
        }
        if (numNotFound > 0)
        {
            System.err.println(numNotFound + "/" + numData + " cells in " +
                    CALORIMETER_NAME + " not mapped to a geometry object. These will not be drawn.");
        }

        // This depends on the number of geometry objects.
        cellTable = new int[2 * ACalorimeterDetector.count()][2 * MAX_HIT_INDEX];
    }

    /**
     * Returns the name of the calorimeter this data object belongs to.
     *
     * @return calorimeter name
     */
    public String getCalorimeterName()
    {
        return CALORIMETER_NAME;
    }


    /**
     * Method is called from APickInteraction and plots pulse shapes plots
     * for a cell when picked.
     * @param index int
     */
    public void plotPulseShapes(int index, boolean withcurve) {
        int[][] adc = getADCCounts(index);
        if (adc == null) {
            logger.warn("Pulse shape plot requested but no adc data available");
            return;
        }
        
        if (!withcurve) {
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
        }
    }

    /**
     * 
     */
    protected int[][] getADCCounts(int index) { return null; }
    
    protected double[][] getPulseShape(int index) { return null; }

    protected double getPulseStep(int index) { return 1.0; }
    
    protected String[] getPulseSubtitle(int index) { return null; }
    
    /**
     * Returns calorimeter type (ECAL or HCAL) of a certain hit.
     *
     * @param index int hit index
     * @return String calorimeter type
     */
    public abstract String getCalorimeterType(int index);
    
    /**
     * Returns the ACalorimeterDetector of a certain hit.
     *
     * @param index int hit index
     * @return ACalorimeterDetector
     */
    public ACalorimeterDetector getDetector(int index) {
        return ACalorimeterDetector.get(detector[index]);
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
        double r, z;

        if (!parameterStore.get("VP", "EtaVertex").getStatus() || detector[index] < 0)
        {
            return this.eta[index];
        }

        if (ACalorimeterDetector.get(detector[index]) instanceof ABarrelCalorimeterDetector)
        {
            r = ACalorimeterDetector.get(detector[index]).getR();
            z = r * AMath.tanLambda(eta[index]);
        }
        else
        {
            z = ACalorimeterDetector.get(detector[index]).getZ();
            r = z / AMath.tanLambda(eta[index]);
        }

        return AParameterUtilities.eta(z, r);
    }

    /**
     * Returns the calorimeter sampling (or layer) of a certain hit.
     *
     * @param index int hit index
     * @return int sampling/layer
     */
    public int getSampling(int index)
    {
        return sampling[index];
    }
    
    /**
     * Returns the side of the calorimeter of a certain hit.
     * 
     * @param index int hit index
     * @return int side
     */
    public int getSide(int index)
    {
        return side[index];
    }

    /**
     * Returns the cell size in eta of a certain hit.
     *
     * @param index int hit index
     * @return double cell size in eta
     */
    public double getdEta(int index)
    {
        return deta[index];
    }

    /**
     * Returns the cell size in phi of a certain hit.
     *
     * @param index int hit index
     * @return double cell size in phi
     */
    public double getdPhi(int index)
    {
        return dphi[index];
    }

    /**
     * Returns the phi coordinate of a certain hit.
     *
     * @param index int hit index
     * @return double phi coordinate.
     */
    public double getPhi(int index)
    {
        return phi[index];
    }

    /**
     * Returns the transverse energy of a hit.
     *
     * @param index int hit index
     * @return float transverse energy
     */
    public float getET(int index)
    {
        return et[index];
    }

    /**
     * Returns the transverse energy of a list of hits.
     *
     * @param index int[] list of hit indices
     * @return float[] list of transverse energies
     */
    public float[] getET(int[] index)
    {
        float[] temp = new float[index.length];

        for (int i = 0; i < index.length; i++)
        {
            temp[i] = et[index[i]];
        }
        return temp;
    }

    /**
     * Returns the calorimeter sampling (or layer) of a list of hits.
     *
     * @param index int[] list of hit indices
     * @return int[] list of samplings
     */
    public int[] getSampling(int[] index)
    {
        int[] temp = new int[index.length];

        for (int i = 0; i < index.length; i++)
        {
            temp[i] = sampling[index[i]];
        }
        return temp;
    }

    /**
     * Returns the energy of a hit.
     *
     * @param index int hit index
     * @return float energy
     */
    public float getEnergy(int index)
    {
        return energy[index];
    }

    /**
     * Returns the depositied energy of a list of hits.
     *
     * @param index int[] list of hit indices
     * @return float[] list of energies
     */
    public float[] getEnergy(int[] index)
    {
        float[] temp = new float[index.length];

        for (int i = 0; i < index.length; i++)
        {
            temp[i] = energy[index[i]];
        }
        return temp;
    }

    public static double getNonScalingFactor()
    {
        return NON_SCALING_FACTOR;
    }

    /**
     * Returns the lowest density for the cell type of the specified hit.
     *
     * @param index int hit index
     * @return double minimum density
     */
    protected double getMinDensity(int index)
    {
        return getMinDensity(getCalorimeterType(index));
    }

    /**
     * Returns the lowest densityfor the specified calorimeter type.
     *
     * @param type String either "ECAL", "HCAL" or "ALL"
     * @return double minimum density
     */
    protected double getMinDensity(String type)
    {
        if (type.equals("ECAL"))
        {
            return minDensityECAL;
        }
        else if (type.equals("HCAL"))
        {
            return minDensityHCAL;
        }
        else if (type.equals("ALL"))
        {
            // Return the lowest non-zero value.
            if (minDensityECAL == 0.0 || minDensityHCAL == 0.0)
            {
                return Math.max(minDensityECAL, minDensityHCAL);
            }
            else
            {
                return Math.min(minDensityECAL, minDensityHCAL);
            }
        }
        else
        {
            return 0.0;
        }
    }

    /**
     * Returns the highest density for the cell type of the specified hit.
     *
     * @param index int hit index
     * @return double maximum density
     */
    protected double getMaxDensity(int index)
    {
        return getMaxDensity(getCalorimeterType(index));
    }

    /**
     * Returns the highest densityfor the specified calorimeter type.
     *
     * @param type String either "ECAL", "HCAL" or "ALL"
     * @return double maximum density
     */
    protected double getMaxDensity(String type)
    {
        if (type.equals("ECAL"))
        {
            return maxDensityECAL;
        }
        else if (type.equals("HCAL"))
        {
            return maxDensityHCAL;
        }
        else if (type.equals("ALL"))
        {
            return Math.max(maxDensityECAL, maxDensityHCAL);
        }
        else
        {
            return 0.0;
        }
    }

    /**
     * Sets the minimum density for the specified calorimeter type.
     *
     * @param type String either "ECAL", "HCAL" or "ALL"
     * @param density double minimum density to set
     */
    protected void setMinDensity(String type, double density)
    {
        if (type.equals("ECAL"))
        {
            minDensityECAL = density;
        }
        else if (type.equals("HCAL"))
        {
            minDensityHCAL = density;
        }
        else if (type.equals("ALL"))
        {
            minDensityECAL = density;
            minDensityHCAL = density;
        }
    }

    /**
     * Sets the maximum density for the specified calorimeter type.
     *
     * @param type String either "ECAL", "HCAL" or "ALL"
     * @param density double maximum density to set
     */
    protected void setMaxDensity(String type, double density)
    {
        if (type.equals("ECAL"))
        {
            maxDensityECAL = density;
        }
        else if (type.equals("HCAL"))
        {
            maxDensityHCAL = density;
        }
        else if (type.equals("ALL"))
        {
            maxDensityECAL = density;
            maxDensityHCAL = density;
        }

        parameterStore.get(CALORIMETER_NAME, "Area/Energy").setD(1. / Math.max(maxDensityECAL, maxDensityHCAL));
    }

    /**
     * Returns the data that needs to be drawn in the specificied projection.
     *
     * @param projection AProjection projection
     * @return ACoord data to be drawn
     */
    protected ACoord getUserNoTransform(AProjection projection)
    {
        if (CALORIMETER_NAME.equals("FCAL") || CALORIMETER_NAME.equals("MBTS"))
        {
            // FCAL and MBTS data are not linked to the geometry objects, so use the old method.
            return super.getUserNoTransform(projection);
        }
        else
        {
            // For all the other types we can simply get the geometry from the
            // geometry objects.
            makeDrawList();
            filterDrawList(projection);
            double[][][] hv = new double[2][numDraw][0];
            int[] index = new int[numDraw];

            for (int i = 0; i < numDraw; i++)
            {
                int j = listdl[i];
                if (detector[j] >= 0)
                {
                    ACoord coord = ACalorimeterDetector.get(detector[j]).getCell(projection, etaIndex[j], phiIndex[j], side[j]);
                    if (coord.hv[0].length > 0)
                    {
                        hv[0][i] = coord.hv[0][0];
                        hv[1][i] = coord.hv[1][0];
                    }
                }
                else
                {
                    // Alternative setups compatibility mode based on eta, deta,
                    // r or z, dr or dz, phi, dphi from event file could be
                    // implemented here.
                }
                index[i] = j;
            }

            // Apply the wrap arounds for phi in certain projections.
            if (projection instanceof AProjectionVP)
            {
                return new ACoord(hv, index, this).includePhiWrapAround("VP");
            }
            else if (projection instanceof AProjectionFR)
            {
                return new ACoord(hv, index, this).includePhiWrapAround("FR");
            }
            else if (projection instanceof AProjectionFZ)
            {
                return new ACoord(hv, index, this).includePhiWrapAround("FZ");
            }
            else
            {
                return new ACoord(hv, index, this);
            }
        }
    }

    /**
     * Returns the data points for the "standard" mode v-plot.
     *
     * @return ACoord v-plot points
     */
    public ACoord getVPPoints()
    {
        makeDrawList();
        double[][] hv = new double[2][numDraw];
        int[] index = new int[numDraw];
        for (int i = 0; i < numDraw; i++)
        {
            int j = listdl[i];
            hv[0][i] = getEta(j);
            hv[1][i] = Math.toDegrees(phi[j]);
            index[i] = j;
        }
        return new ACoord(hv, index, this, ACoord.SYMBOLS).includePhiWrapAround("VP");
    }

    /**
     * Cleans up the drawList. From all cells that are drawn at a certain place
     * only the one that has the highest deposited transverse energy will remain.
     * It also sums the transverse energy of the cells it removes. This is stored
     * in etSum[] at the index of the "most energetic" cell.
     *
     * @param projection AProjection projection to be drawn
     */
    protected void filterDrawList(AProjection projection)
    {
        // In the v-plot every cell is drawn, unless we're in one of the special
        // modes.
        if (projection instanceof AProjectionVP)
        {
            switch (parameterStore.get("VP", "Mode").getI())
            {
                case AProjectionVP.MODE_STANDARD:
                    // Everything is drawn in the standard mode v-plot.
                    return;
                case AProjectionVP.MODE_ECAL_LAYER_0:
                case AProjectionVP.MODE_ECAL_LAYER_1:
                case AProjectionVP.MODE_ECAL_LAYER_2:
                case AProjectionVP.MODE_ECAL_LAYER_3:
                    if (!CALORIMETER_NAME.equals("LAr"))
                    {
                        // Don't draw HCAL in ECAL mode.
                        numDraw = 0;
                    }
                    return;
                case AProjectionVP.MODE_HCAL_LAYER_0:
                case AProjectionVP.MODE_HCAL_LAYER_1:
                case AProjectionVP.MODE_HCAL_LAYER_2:
                case AProjectionVP.MODE_HCAL_LAYER_3:
                    if (!CALORIMETER_NAME.equals("TILE") && !CALORIMETER_NAME.equals("HEC"))
                    {
                        // Don't draw ECAL in HCAL mode.
                        numDraw = 0;
                    }
                    return;
                default:
                    numDraw = 0;
                    return;
            }
        }

        // The phi-z projection has special modes as well.
        if (projection instanceof AProjectionFZ)
        {
            if (parameterStore.get(projection.getName(), "Mode").getI() != AProjectionYX.MODE_STANDARD)
            {
                numDraw = 0;
                return;
            }
        }
        // Also the y-x and phi-rho projections have special modes (they share the same mode list).
        int mode = parameterStore.get("YX", "Mode").getI();
        if (projection instanceof AProjectionYX || projection instanceof AProjectionFR)
        {
            if (mode != AProjectionYX.MODE_STANDARD &&
                    (mode < AProjectionYX.MODE_LAR_ENDCAP_PRESAMPLER || mode > AProjectionYX.MODE_HEC_SUMMED))
            {
                numDraw = 0;
                return;
            }
            else if (mode >= AProjectionYX.MODE_LAR_ENDCAP_PRESAMPLER && mode <= AProjectionYX.MODE_HEC_SUMMED)
            {
                if ((mode >= AProjectionYX.MODE_HEC_1 && mode <= AProjectionYX.MODE_HEC_4 && !CALORIMETER_NAME.equals("HEC")) ||
                        (mode < AProjectionYX.MODE_HEC_1 && !CALORIMETER_NAME.equals("LAr")) ||
                        (mode == AProjectionYX.MODE_HEC_SUMMED && !CALORIMETER_NAME.equals("HEC")) ||
                        (mode == AProjectionYX.MODE_LAR_ENDCAP_SUMMED && !CALORIMETER_NAME.equals("LAr")))
                {
                    numDraw = 0;
                    return;
                }
            }
            else if(mode == AProjectionYX.MODE_MBTS && !CALORIMETER_NAME.equals("MBTS"))
            {
                numDraw = 0;
                return;
            }
        }

        float[][] etTable = new float[2 * ACalorimeterDetector.count()][2 * MAX_HIT_INDEX];
        for (int i = 0; i < 2 * ACalorimeterDetector.count(); i++)
        {
            for (int j = 0; j < 2 * MAX_HIT_INDEX; j++)
            {
                cellTable[i][j] = -1;
                etTable[i][j] = 0.0f;
            }
        }

        for (int i = 0; i < numDraw; i++)
        {
            int detIndex = getDetectorIndex(listdl[i]);
            int hitIndex = getHitIndex(listdl[i], projection);

            if (detIndex >= 0 && hitIndex >= 0)
            {
                etTable[detIndex][hitIndex] += et[listdl[i]];
                if (cellTable[detIndex][hitIndex] < 0 || et[cellTable[detIndex][hitIndex]] < et[listdl[i]])
                    cellTable[detIndex][hitIndex] = listdl[i];
            }
        }

        numDraw = 0;
        for (int i = 0; i < 2 * ACalorimeterDetector.count(); i++)
        {
            for (int j = 0; j < 2 * MAX_HIT_INDEX; j++)
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
     * This function maps a hit to an index. This index is such that two hits
     * get the same index when they are in the same detector and on the same
     * side.
     *
     * @param index int number of the hit in the current dataset
     * @return int detector index
     */
    protected int getDetectorIndex(int index)
    {
        if (detector[index] >= 0)
        {
            if (side[index] > 0)
            {
                return ACalorimeterDetector.count() + detector[index];
            }
            else
            {
                return detector[index];
            }
        }
        return -1;
    }

    /**
     * This function maps a hit to an index. This index is such that two hits in
     * the same detector will receive the same index if they will be drawn in
     * the same place.
     *
     * @param index int number of the hit in the current dataset
     * @param projection AProjection projection the hit index should be
     *        calculated for
     * @return int hit index
     */
    protected int getHitIndex(int index, AProjection projection)
    {
        if (detector[index] >= 0)
        {
            if (projection instanceof AProjectionYX)
            {
                return phiIndex[index];
            }
            else if (projection instanceof AProjectionRZ)
            {
                if (ACalorimeterDetector.get(detector[index]).getRSign(phiIndex[index]) > 0)
                {
                    return MAX_HIT_INDEX + etaIndex[index];
                }
                else
                {
                    return etaIndex[index];
                }
            }
            else if (projection instanceof AProjectionFR)
            {
                return phiIndex[index];
            }
            else if (projection instanceof AProjectionFZ)
            {
                return phiIndex[index];
            }
        }
        return -1;
    }

    /**
     * Returns the histograms for this projection.
     *
     * @param projection AProjection2D current projection
     * @return ACoord[] polygons representing histograms
     */
    protected ACoord[] getUserHistograms(AProjection2D projection)
    {
        return ACoord.NO_HISTOGRAMS;
    }

    /**
     * Returns the histograms in the YX projection.
     *
     * @return ACoord[] histograms
     */
    public ACoord[] getYXHistograms()
    {
        AHistogram hist = new AHistogram(0., 2. * Math.PI, phiGranularity, histoScale.getD(), this);
        int mode = parameterStore.get("YX", "Mode").getI();
        double newInnerR = innerR;
        boolean hasAddedToHistogram = false;
        for (int i = 0; i < numDraw; i++)
        {
            int j = listdl[i];
            if (detector[j] >= 0)
            {
                if ((mode == AProjectionYX.MODE_STANDARD && ACalorimeterDetector.get(detector[j]) instanceof ABarrelCalorimeterDetector) ||
                        (mode != AProjectionYX.MODE_STANDARD && ACalorimeterDetector.get(detector[j]) instanceof AEndcapCalorimeterDetector))
                {
                    String name = ACalorimeterDetector.get(detector[j]).getName();
                    if (mode == AProjectionYX.MODE_STANDARD || (mode == AProjectionYX.MODE_LAR_ENDCAP_PRESAMPLER && (name.equals("LAr Endcap Presampler")||name.equals("LAr_EC_Presampler"))) ||
                            (name.equals("LAr Outer Endcap") && getSampling(j) == mode - AProjectionYX.MODE_LAR_ENDCAP_PRESAMPLER) ||
                            (name.equals("LAr Inner Endcap") && getSampling(j) == mode - AProjectionYX.MODE_LAR_ENDCAP_PRESAMPLER - 1) ||
                            (name.equals("HEC") && getSampling(j) == mode - AProjectionYX.MODE_HEC_1) ||
                            (mode == AProjectionYX.MODE_LAR_ENDCAP_SUMMED && name.indexOf("LAr") >= 0) || (mode == AProjectionYX.MODE_HEC_SUMMED && name.equals("HEC")))
                    {
                        hasAddedToHistogram = true;
                        double phiMin = ACalorimeterDetector.get(detector[j]).getPhiMin(phiIndex[j]);
                        double phiMax = ACalorimeterDetector.get(detector[j]).getPhiMax(phiIndex[j]);

                        if (phiMin < -1e-9 && phiMax > 1e-9)
                        {
                            hist.fill(phiMin + 2. * Math.PI, 2. * Math.PI, etSum[j] / 2.);
                            hist.fill(0., phiMax, etSum[j] / 2.);
                        }
                        else
                        {
                            hist.fill(phiMin, phiMax, etSum[j]);
                        }
                        if (mode != AProjectionYX.MODE_STANDARD)
                        {
                            if (name.equals("HEC"))
                            {
                                if (mode != AProjectionYX.MODE_HEC_SUMMED)
                                {
                                    //can use the get RMin accurately for the HEC
                                    newInnerR = ACalorimeterDetector.get(detector[j]).getRMin();
                                }
                                else if (ACalorimeterDetector.get(detector[j]).getRMin() < newInnerR)
                                {
                                    //only want to assign HEC ALL minimum if is smaller
                                    newInnerR = ACalorimeterDetector.get(detector[j]).getRMin();
                                }
                            }
                            else
                            {
                                //calculate RMin as not accurate in geometry file
                                double etaMax = ACalorimeterDetector.get(detector[j]).getEtaMax();
                                double zMin = ACalorimeterDetector.get(detector[j]).getZMin();
                                double temp = zMin / Math.sinh(etaMax);
                                //save calculated if smallest
                                if (temp < newInnerR)
                                    newInnerR = temp;
                            }
                        }
                    }
                }
            }
        }
        if (hasAddedToHistogram)
            return new ACoord[] { hist.getYXUser(newInnerR) };
        else
            return ACoord.NO_HISTOGRAMS;
    }

    /**
     * Returns the histograms in the phi-rho projection.
     *
     * @return ACoord[] histograms
     */
    public ACoord[] getFRHistograms()
    {
        AHistogram hist = new AHistogram(0., 2. * Math.PI, phiGranularity, histoScale.getD(), this);

        for (int i = 0; i < numDraw; i++)
        {
            int j = listdl[i];
            if (detector[j] >= 0)
            {
                if (ACalorimeterDetector.get(detector[j]) instanceof ABarrelCalorimeterDetector)
                {
                    double phiMin = ACalorimeterDetector.get(detector[j]).getPhiMin(phiIndex[j]);
                    double phiMax = ACalorimeterDetector.get(detector[j]).getPhiMax(phiIndex[j]);

                    if (phiMin < -1e-9 && phiMax > 1e-9)
                    {
                        hist.fill(phiMin + 2. * Math.PI, 2. * Math.PI, etSum[j] / 2.);
                        hist.fill(0., phiMax, etSum[j] / 2.);
                    }
                    else
                    {
                        hist.fill(phiMin, phiMax, etSum[j]);
                    }
                }
            }
        }
        return new ACoord[] { hist.getFRUser(innerR).includePhiWrapAround("FR") };
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
            if (detector[j] >= 0)
            {
                int sign = ACalorimeterDetector.get(detector[j]).getRSign(phiIndex[j]);
                double etaMin = ACalorimeterDetector.get(detector[j]).getEtaMin(etaIndex[j], side[j]);
                double etaMax = ACalorimeterDetector.get(detector[j]).getEtaMax(etaIndex[j], side[j]);

                if (sign > 0)
                {
                    histUp.fill(etaMin, etaMax, etSum[j]);
                }
                else
                {
                    histDown.fill(etaMin, etaMax, etSum[j]);
                }
            }
        }

        ACoordArray c = new ACoordArray();
        c.add(histUp.getRZUser(outerZ, outerR, AHistogram.UP));
        c.add(histDown.getRZUser(outerZ, outerR, AHistogram.DOWN));
        return c.getArray();
    }

    /**
     * Returns the histograms in the phi-z projection.
     *
     * @return ACoord[] histograms
     */
    public ACoord[] getFZHistograms()
    {
        AHistogram hist;
        AHistogram histLeft = new AHistogram(0., 2. * Math.PI, phiGranularity, histoScale.getD(), this);
        AHistogram histRight = new AHistogram(0., 2. * Math.PI, phiGranularity, histoScale.getD(), this);

        for (int i = 0; i < numDraw; i++)
        {
            int j = listdl[i];
            if (detector[j] >= 0)
            {
                if (ACalorimeterDetector.get(detector[j]) instanceof AEndcapCalorimeterDetector)
                {
                    double phiMin = ACalorimeterDetector.get(detector[j]).getPhiMin(phiIndex[j]);
                    double phiMax = ACalorimeterDetector.get(detector[j]).getPhiMax(phiIndex[j]);

                    if (side[j] > 0)
                    {
                        hist = histRight;
                    }
                    else
                    {
                        hist = histLeft;
                    }

                    if (phiMin < -1e-9 && phiMax > 1e-9)
                    {
                        hist.fill(phiMin + 2. * Math.PI, 2. * Math.PI, etSum[j] / 2.);
                        hist.fill(0., phiMax, etSum[j] / 2.);
                    }
                    else
                    {
                        hist.fill(phiMin, phiMax, etSum[j]);
                    }
                }
            }
        }

        return new ACoord[] { histLeft.getFZUser(innerZ, AHistogram.LEFT).includePhiWrapAround("FZ"),
                histRight.getFZUser(innerZ, AHistogram.RIGHT).includePhiWrapAround("FZ") };
    }

    /**
     * Returns the histograms in the x'-z projection.
     *
     * @return ACoord[] histograms
     */
    public ACoord[] getXZHistograms()
    {
        return ACoord.NO_HISTOGRAMS;
    }

    /**
     * Returns the histograms in the y'-z projection.
     *
     * @return ACoord[] histograms
     */
    public ACoord[] getYZHistograms()
    {
        return ACoord.NO_HISTOGRAMS;
    }

    /**
     * Returns the histograms in the eta-phi projection.
     *
     * @return ACoord[] histograms
     */
    public ACoord[] getVPHistograms()
    {
        return ACoord.NO_HISTOGRAMS;
    }

    /**
     * Sets the type of the hits (associated, unassociated).
     */
    protected void setType()
    {
        int[][] cluster = event.getAssociationManager().get(getName(), "Cluster");
        for (int i = 0; i < numData; i++)
        {
            if (cluster != null && cluster[i] != null)
            {
                type[i] = 1;
            }
            else
            {
                type[i] = 0;
            }
        }
    }

    /**
     * Determines if the cells need to be scaled by energy or not. This will be
     * true for most cases, but not if the user has selected the color by energy
     * color function.
     *
     * @return boolean true for energy scaling, false for constant cell size
     */
    protected boolean energyScaling()
    {
        return parameterStore.get(PARAMETER_GROUP, "ColorFunction").getI() != COLOR_FUNC_ENERGY;
    }

    /**
     * Determines the color for each hit according to the color function set by
     * the user.
     *
     * @return int number of coloring options?
     */
    protected int internalColor()
    {
        int colorFunction = parameterStore.get(PARAMETER_GROUP, "ColorFunction").getI();

        switch (colorFunction)
        {
            case COLOR_FUNC_CONSTANT:
                colorByConstant();
                break;

            case COLOR_FUNC_SUBDET:
                colorBy(sub);
                break;

            case COLOR_FUNC_CLUSTER:
                colorBy(getClusters());
                break;

            case COLOR_FUNC_SAMPLING:
                colorBy(sampling);
                break;

            case COLOR_FUNC_ENERGY:
                colorByEnergy();
                break;

            case COLOR_FUNC_JET:
                if(parameterStore.get("Jet", "ColorFunction").getI()!=1)
                {
                    parameterStore.get("Jet", "ColorFunction").setI(1);
                    String msg = "Coloring Calo by jet so ";
                    msg += "ATLAS->Jet->Color Function changed to color by index";
                    AOutput.append("\n" + msg + "\n", ALogInterface.WARNING);
                }
                colorBy(getJets());
                break;

            case COLOR_FUNC_TYPE:
                colorByType();
                break;

            case COLOR_FUNC_TIME:
                colorByTime();
                break;
        }

        return colorFunction + 8;
    }

    /**
     * Colors cells by energy.
     */
    protected void colorByEnergy()
    {
        double min = 0.0, max = 0.0;
        boolean first = true;
        int numColors = 17;
        float[] en = null;

        switch (parameterStore.get(CALORIMETER_NAME, "EnergyMode").getI())
        {
            case ENERGY_MODE_SUM_LIN:
            case ENERGY_MODE_SUM_LOG:
            case ENERGY_MODE_SUM_SQRT:
                en = etSum;
                break;

            case ENERGY_MODE_MAX_LIN:
            case ENERGY_MODE_MAX_LOG:
            case ENERGY_MODE_MAX_SQRT:
                en = et;
                break;
        }

        for (int i = 0; i < numDraw; i++)
        {
            int j = listdl[i];
            if (first)
            {
                min = en[j];
                max = en[j];
                first = false;
            }

            if (en[j] > max)
                max = en[j];
            if (en[j] < min)
                min = en[j];
        }

        for (int i = 0; i < numDraw; i++)
        {
            int j = listdl[i];
            if (AColorMap.getColorMap() == AColorMap.COLOR_MAP_BW)
            {
                // Black and white colormap.
                color[j] = AColorMap.BK;
            }
            else
            {
                int colorIndex = 0;

                // We have numColors colors available.
                switch (parameterStore.get(CALORIMETER_NAME, "EnergyMode").getI())
                {
                    case ENERGY_MODE_MAX_LIN:
                    case ENERGY_MODE_SUM_LIN:
                        colorIndex = (int) (numColors * (en[j] - min) / (max - min));
                        break;
                    case ENERGY_MODE_MAX_LOG:
                    case ENERGY_MODE_SUM_LOG:
                        colorIndex = (int) (numColors * (Math.log(en[j]) - Math.log(min)) / (Math.log(max) - Math.log(min)));
                        break;
                    case ENERGY_MODE_MAX_SQRT:
                    case ENERGY_MODE_SUM_SQRT:
                        colorIndex = (int) (numColors * Math.sqrt((en[j] - min) / (max - min)));
                        break;
                }

                if (colorIndex >= numColors)
                    colorIndex = (byte) (numColors - 1);

                color[j] = (byte) colorIndex;
            }
        }
    }

    /**
     * Colors cells by type (ECAL/HCAL).
     */
    protected void colorByType()
    {

        for (int i = 0; i < numDraw; i++)
        {
            int j = listdl[i];
            String cellType = getCalorimeterType(j);
            //Use detector color but add on 12 to get different shade of same color
            color[j] = (byte) (parameterStore.get("Det", cellType + "Fill").getI() + 12);
        }
    }
    
    //Gets the measured time for a calorimeter cell.
    //It's 0 by default, but can and should be overriden by subclasses.
    protected double getTime(int hit)
    {
        return 0.0;
    }

    //Color cells by their measured time, on a blue-red scale.
    //Unmeasured cells are grey.
    protected void colorByTime()
    {
        double min = 0.0, max = 0.0;
        boolean first = true;
        for (int i = 0; i < numDraw; i++)
        {
            int j = listdl[i];
            double t=getTime(j);
            if (t==0.0) continue;//ignore unmeasured hits
            if (first)
            {
                min = t; max = t;
                first = false;
            }

            if (t > max) max = t;
            if (t < min) min = t;
        }

        int numColors = 127;
        double scale = (max - min);
        if (scale<5) scale=5;//smallest scale is 5 ns
        for (int i = 0; i < numDraw; i++)
        {
            int j = listdl[i];
            double t = getTime(j);

            int colorIndex = (int) (numColors * (t-min) / scale);
            if (colorIndex >= numColors) {
                colorIndex = numColors;//red
            }
            if (colorIndex < 1) {
                colorIndex = 1;//blue
            }

            if (t==0.0) colorIndex=0;//gets grey color
            color[j] = (byte) colorIndex;

        }
    }

    /**
     * Applies cuts to the data.
     */
    protected void applyCuts()
    {
        cutIndex();
        cut("CutsCalo", "Layer", "Layer", sampling);
        cut("CutsCalo", "CellQuality", "CellQuality", BadCell);
        cutPhi(phi, dphi);
        cutEtaDEta(eta, deta);
    }

    /**
     * Performs the local density calibration. This function determines the
     * highest and lowest energy density values ans stores them.
     *
     * @param index int[] hits to be used in calibration
     * @param area double[] cell areas
     */
    public void calibrateLocalDensity(int[] index, double[] area)
    {
        float[] en = null;
        double density;
        boolean firstECAL = true;
        boolean firstHCAL = true;

        AParameter par = parameterStore.get(CALORIMETER_NAME, "Area/Energy");
        if (par.getStatus() && par.getD() > 0)
        {
            minDensityECAL = 0.0;
            minDensityHCAL = 0.0;
            maxDensityECAL = 1. / par.getD();
            maxDensityHCAL = 1. / par.getD();
        }
        else
        {
            // Otherwise we need to do calculate it ourselves.
            switch (parameterStore.get(CALORIMETER_NAME, "EnergyMode").getI())
            {
                case ENERGY_MODE_SUM_LIN:
                case ENERGY_MODE_SUM_LOG:
                case ENERGY_MODE_SUM_SQRT:
                    en = etSum;
                    break;

                case ENERGY_MODE_MAX_LIN:
                case ENERGY_MODE_MAX_LOG:
                case ENERGY_MODE_MAX_SQRT:
                    en = et;
                    break;
            }

            // The default for an empty list of cells or only zero area cells is
            // 0.0.
            minDensityECAL = 0.0;
            maxDensityECAL = 0.0;
            minDensityHCAL = 0.0;
            maxDensityHCAL = 0.0;

            // Calculate density and determine the highest and lowest values.
            for (int i = 0; i < index.length; i++)
            {
                if (area[i] > 0)
                {
                    density = (en[index[i]] / area[i]);

                    if (density > 0)
                    {
                        if (getCalorimeterType(index[i]).equals("ECAL"))
                        {
                            if (firstECAL)
                            {
                                // First cell with a sensible density, take this
                                // as initial value.
                                minDensityECAL = density;
                                maxDensityECAL = density;
                                firstECAL = false;
                            }

                            if (density < minDensityECAL)
                                minDensityECAL = density;
                            if (density > maxDensityECAL)
                                maxDensityECAL = density;

                        }
                        else if (getCalorimeterType(index[i]).equals("HCAL"))
                        {
                            if (firstHCAL)
                            {
                                // First cell with a sensible density, take this
                                // as initial value.
                                minDensityHCAL = density;
                                maxDensityHCAL = density;
                                firstHCAL = false;
                            }

                            if (density < minDensityHCAL)
                                minDensityHCAL = density;
                            if (density > maxDensityHCAL)
                                maxDensityHCAL = density;
                        }
                    }
                }
            }
        }
    }

    /**
     * 
     */
    public double[] getScaleFactors(int[] dl, double[] area) {
        double[] factors = new double[area.length];
        
        float[] en = null;
        int energyMode = parameterStore.get(CALORIMETER_NAME, "EnergyMode").getI();

        switch (energyMode)
        {
            case ENERGY_MODE_SUM_LIN:
            case ENERGY_MODE_SUM_LOG:
            case ENERGY_MODE_SUM_SQRT:
                en = etSum;
                break;

            case ENERGY_MODE_MAX_LIN:
            case ENERGY_MODE_MAX_LOG:
            case ENERGY_MODE_MAX_SQRT:
                en = et;
                break;
        }

        for (int i = 0; i < area.length; i++)
        {
            factors[i] = 1.0f;
            
            if (area[i] == 0)
                continue;

            if (energyScaling())
            {
                // The user has (implicitly) selected energy scaling.
                double density = en[dl[i]] / area[i];
                double minDensity = getMinDensity(dl[i]);
                double maxDensity = getMaxDensity(dl[i]);

                switch (energyMode)
                {
                    case ENERGY_MODE_MAX_LIN:
                    case ENERGY_MODE_SUM_LIN:
                        factors[i] = Math.sqrt(density / maxDensity);
                        break;

                    case ENERGY_MODE_MAX_LOG:
                    case ENERGY_MODE_SUM_LOG:
                        double magnitude = Math.floor(Math.log10(Math.sqrt(maxDensity / minDensity)) + 1.0);
                        factors[i] = (Math.log10(Math.sqrt(density / maxDensity)) + magnitude) / magnitude;
                        break;

                    case ENERGY_MODE_MAX_SQRT:
                    case ENERGY_MODE_SUM_SQRT:
                        factors[i] = Math.sqrt(Math.sqrt(density / maxDensity));
                        break;

                    default:
                        factors[i] = NON_SCALING_FACTOR;
                }
            }
            else
            {
                // No energy scaling, scale all cells by the same factor.
                factors[i] = NON_SCALING_FACTOR;
            }
            
            if (factors[i] > 1.0) {
                factors[i] = 1.0;
            }
        }
        
        return factors;
    }
    
    /**
     * Scales a series of polygons according to the user settings.
     *
     * @param coord ACoord polygons to be scaled
     * @param area double[] surface area of these polygons
     * @return ACoord scaled polygons
     */
    protected ACoord scalePolygons(ACoord coord, double[] area)
    {
        double[] factors = getScaleFactors(coord.index, area);
        
        for (int i = 0; i < area.length; i++)
        {
            if (area[i] == 0)
                continue;

            APolygon.scale(coord.hv[0][i], coord.hv[1][i], factors[i]);
        }
        return coord;
    }

    private String getCellString(int index, String idstr) {
        StringBuffer msg = new StringBuffer();
        msg.append(CALORIMETER_NAME + " cell");
        msg.append(" (id: " + idstr + " index: " + index + ")");
        return msg.toString();
    }

    private String getHitString(int index) {
        StringBuffer msg = new StringBuffer();
        
        msg.append("\n E Max = ");
        msg.append(String.format("%.2f",energy[index]));
        msg.append(" GeV\n ET Max = ");
        msg.append(String.format("%.2f",et[index]));
        msg.append(" GeV\n ET Sum = ");
        msg.append(String.format("%.2f",etSum[index]));
        msg.append(" GeV\n ");
        msg.append(AMath.ETA);
        msg.append(" = ");
        msg.append(String.format("%.4f",eta[index]));
        msg.append(" ");
        msg.append(AMath.PLUSMINUS);
        msg.append(" ");
        // deta is step (i.e. size of a cell), with +- we want to see half of it
        msg.append(String.format("%.3f",deta[index] / 2.0));
        msg.append("\n ");
        msg.append(AMath.PHI);
        msg.append(" = ");
        msg.append(String.format("%.1f",Math.toDegrees(phi[index])));
        msg.append(" " + AMath.PLUSMINUS + " ");
        // dphi is step (i.e. size of a cell), with +- we want to see half of it
        msg.append(String.format("%.1f",Math.toDegrees(dphi[index] / 2.0)));
        msg.append(AMath.DEGREES);
        msg.append("  (" + String.format("%.1f",phi[index]) + "  ");
        msg.append(AMath.PLUSMINUS + " " + String.format("%.1f",dphi[index] / 2.0) + " rad)");
        
        if(feedThrough != null && slot != null && channel != null)
        {
            String s = "\n feed through/slot/channel = " +
                       feedThrough[index] + "/" + slot[index] + "/" +
                       channel[index];
            msg.append(s);
        }
        if(BadCell != null){
        	if(BadCell[index] == 0)
        		msg.append("\n Cell is good");
        	if(BadCell[index] == 1)
        			msg.append("\n Cell is bad");
        	if(BadCell[index] == -1)
        			msg.append("\n Cell is below 50 MeV");
        }
        
        return msg.toString();
    }
    
    /**
     * Getting title string for pulse shape windows
     * 
     * Relies on decodedId[1] being some useful high level string describing
     * the calorimeter part. This isn't the case for ATILEData where this function
     * is overridden to do something different.
     * 
     * Maybe this is a slightly unreasonable plan... but then the first thing
     * you decode out of the id should usually be the "biggest" sort of division...
     * - Adam
     */
    protected String getPulseTitleString(int index) {
        
        String[] decodedId = AIdHelper.getFullIdentifier(id[index]);
        
        String title = null;
        
        if (feedThrough == null || slot == null || channel == null) {
            // Old style title
            logger.warn("Calo FT/slot/channel info not available, using old style title");
            title = decodedId[1] + " cell: " + decodedId[0];
        } else {
            // New title as requested by Andre N. Aug 09
            title = decodedId[1] + " cell: " + 
                    this.feedThrough[index] + "/" + this.slot[index] +
                    "/" + this.channel[index];
        }
        
        return title;
    }

    // Some calo stuff wants to decode the id it's own way...
    public String getHitInfoNoDecode(int index) {
        
        String[] decodedId = AIdHelper.getFullIdentifier(id[index]);
        return getCellString(index, decodedId[0]) + getHitString(index);
        
    }
    
    /**
     * Returns information about a certain hit for display in the information
     * box.
     *
     * @param index int hit index
     * @return String hit information
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
    	
        String[] decodedId = AIdHelper.getFullIdentifier(id[index]);
        StringBuffer msg = new StringBuffer();
        msg.append(getCellString(index, decodedId[0]));
        // iterate over decodedId to get further details (if available),
        // first item [0] was already printed out above, thus start from 1
        for(int i = 1; i < decodedId.length; i++)
        {
            msg.append("\n " + decodedId[i]);
        }

        msg.append(getHitString(index));
        // by zdenek 2009-01-20
        // layer is already present as sampling in the decodedId
        // sub[] should gradually be removed, don't print it for now
        // (see mess in AHashMap - !name.equals("sub")) grrrr
        /*
        msg.append("\n ");
        msg.append("layer = " + sampling[index]);
        msg.append("\n ");
        msg.append("subdetector = " + sub[index]);
        */

        return msg.toString();
    }

    /**
     * Returns the total transverse energy for this data type.
     *
     * @return double total transverse energy
     */
    public double getTotalTransverseEnergy()
    {
        double sumEt = 0.;
        for (int i = 0; i < numData; i++)
        {
            sumEt += et[i];
        }
        return sumEt;
    }

    /**
     * Returns information about the drawn hits, used for the v-plot.
     *
     * @return String information
     */
    public String getVPHitInfo()
    {
        applyCuts();
        if (numDraw == 0)
            return "";
        double sumE = 0.;
        double sumEt = 0.;

        for (int i = 0; i < numDraw; i++)
        {
            int j = listdl[i];
            sumEt += et[j];
            sumE += energy[j];
        }

        String msg = numDraw + " " + getNameScreenName();
        msg += "  sum(ET) = " + String.format("%.1f",sumEt) + "  sum(E) = " + String.format("%.1f",sumE);
        return msg;
    }

    /**
     * Returns the specified hits in the form of a four vector. Used by the list
     * manager.
     *
     * @param num int number of hits
     * @param list int[] indices of the hits
     * @return A4Vector four vector
     */
    public A4Vector get4Vector(int num, int[] list)
    {
        A4Vector sum = new A4Vector();

        for (int i = 0; i < num; ++i)
        {
            int j = list[i];
            double lambda = AMath.lambda(eta[j]);

            double pt = energy[j] * Math.cos(lambda);
            double pz = energy[j] * Math.sin(lambda);

            double px = pt * Math.cos(phi[j]);
            double py = pt * Math.sin(phi[j]);

            sum.add(px, py, pz, 0.);
        }
        return sum;
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
        makeDrawList();
        AProjection projection = ACanvas.getCanvas().getCurrentWindow().getProjection();

        for (int i = 0; i < numDraw; i++)
        {
            int j = listdl[i];

            if (drawn[j])
                continue;

            int detIndex = getDetectorIndex(j);
            int hitIndex = getHitIndex(j, projection);

            if (detIndex >= 0 && hitIndex >= 0) {
                if (cellTable != null && detIndex < cellTable.length
                        && hitIndex < cellTable[detIndex].length) {

                    int mappedTo = cellTable[detIndex][hitIndex];

                    if (mappedTo >= 0 && drawn[mappedTo] == true) {
                        drawn[j] = true;
                    }
                }
            }
        }

        return drawn;
    }

    /**
     * Rebuilds the hit to geometry mapping for all calorimeters. Used when the
     * user has loaded a different geometry file.
     */
    public static void remakeHitToGeometryMapping()
    {
      //Get current event
      AEvent event = eventManager.getCurrentEvent();
      if (event != null){
        ACalorimeterData[] calorimeters = new ACalorimeterData[event.getCalorimeters().size()];
        event.getCalorimeters().toArray(calorimeters);
        for (int i = 0; i < calorimeters.length; i++)
        {
            calorimeters[i].makeHitToGeometryMapping();
        }
      }
    }

    /**
     * Draws the calorimeters. This method is called from AProjection2D.paint().
     *
     * @param window AWindow window to use
     * @param ag AGraphics graphics object to draw onto
     * @param projection AProjection2D current projection
     * @param event AEvent drawn event reference
     */
    public static void drawCalorimeters(AWindow window, AGraphics ag, AProjection2D projection, AEvent event)
    {
        int mode = parameterStore.get(projection.getName(), "Mode").getI();

        if(projection instanceof AProjectionYX || projection instanceof AProjectionFR)
        {
            //only grey out the GUI if relevant for the current window
            boolean changeGUI = false;
            if(window.equals(ACanvas.getCanvas().getCurrentWindow())) 
                changeGUI = true;
            
            //grey out draw FCAL option if not on right layer of HEC
            AParameter drawFCAL = parameterStore.get("YX", "DrawFCAL");
            if(changeGUI)
            {
                if(!drawFCAL.isInitialized())
                    drawFCAL.initialize();
                if(mode < AProjectionYX.MODE_HEC_2 || mode > AProjectionYX.MODE_HEC_4)
                    drawFCAL.getNameComponent().setEnabled(false);
                else
                    drawFCAL.getNameComponent().setEnabled(true);
            }

            
            //grey out split ETA, LAr bin and HEC bin options if not on summed endcaps option
            AParameter splitEta = parameterStore.get("YX", "SplitEta");
            AParameter LArBin = parameterStore.get("YX", "LArBin");
            AParameter HECBin = parameterStore.get("YX", "HECBin");
            if(changeGUI)
            {
                if(!HECBin.isInitialized())
                    HECBin.initialize();
                if(!LArBin.isInitialized())
                    LArBin.initialize();
                if(!splitEta.isInitialized())
                    splitEta.initialize();
            }
            //Summed Endcaps has a different draw method
            if(mode == AProjectionYX.MODE_LAR_ENDCAP_SUMMED || mode == AProjectionYX.MODE_HEC_SUMMED)
            {
                if(changeGUI)
                {
                    LArBin.getValueComponent().setEnabled(true);
                    HECBin.getValueComponent().setEnabled(true);
                    LArBin.getNameComponent().setEnabled(true);
                    HECBin.getNameComponent().setEnabled(true);
                    if(LArBin.getI()==3 || HECBin.getI()==3)
                    {
                        splitEta.getValueComponent().setEnabled(true);
                        splitEta.getNameComponent().setEnabled(true);
                    }
                    else
                    {
                        splitEta.getValueComponent().setEnabled(false);
                        splitEta.getNameComponent().setEnabled(false);
                    }
                }
                ACalorimeterData[] calorimeters = new ACalorimeterData[event.getCalorimeters().size()];
                event.getCalorimeters().toArray(calorimeters);
                for (int i = 0; i < calorimeters.length; i++)
                {
                    //check if in right calorimeter
                    if ((mode == AProjectionYX.MODE_LAR_ENDCAP_SUMMED && !calorimeters[i].getName().equals("LAr")) ||
                            (mode == AProjectionYX.MODE_HEC_SUMMED && !calorimeters[i].getName().equals("HEC")))
                        continue;
                    ADrawCalorimeterSummedEndcaps.drawCalorimeterSummedEndcaps(window,
                            ag, projection, calorimeters[i], calorimeters[i].etaIndex,
                            calorimeters[i].phiIndex, calorimeters[i].side);
                }
                return;
            }
            else
            {
                if(changeGUI)
                {
                    LArBin.getValueComponent().setEnabled(false);
                    HECBin.getValueComponent().setEnabled(false);
                    splitEta.getValueComponent().setEnabled(false);
                    LArBin.getNameComponent().setEnabled(false);
                    HECBin.getNameComponent().setEnabled(false);
                    splitEta.getNameComponent().setEnabled(false);
                }
                if (ALegendWindow.exists())
                    ALegendWindow.getInstance().nothingToDisplay(window);
            }
        }
        else if (ALegendWindow.exists())
            ALegendWindow.getInstance().nothingToDisplay(window);

        if (projection instanceof AProjectionVP && mode == 0)
        {
            // The standard mode of the v-plot uses a different function.
            drawCalorimetersVPlot(window, ag, projection, event);
            return;
        }

        ACalorimeterData[] calorimeters = new ACalorimeterData[event.getCalorimeters().size()];
        event.getCalorimeters().toArray(calorimeters);
        ACoord[] data = new ACoord[calorimeters.length];
        double[][] area = new double[calorimeters.length][];

        for (int i = 0; i < calorimeters.length; i++)
        {
            // Retrieve the cell coordinates.
            data[i] = calorimeters[i].getUserNoTransform(projection);

            // Calculate the cell areas.
            area[i] = new double[data[i].index.length];
            for (int j = 0; j < data[i].index.length; j++)
            {
                area[i][j] = APolygon.getArea(data[i].hv[0][j], data[i].hv[1][j]);
            }

            // Let the calorimeter determine its minimum and maximum energy
            // density.
            calorimeters[i].calibrateLocalDensity(data[i].index, area[i]);
        }

        if (window.getProjection() instanceof AProjectionVP)
        {
            // In case of the v-plot we force the overall energy density
            // calibration.
            calibrateDensity(calorimeters, ENERGY_CALIB_OVERALL);
        }
        else
        {
            // Calibrate the energy density according to the user settings.
            calibrateDensity(calorimeters, ENERGY_CALIB_UNKNOWN);
        }

        for (int i = 0; i < calorimeters.length; i++)
        {
            // Apply the non linear transforms and convert to screen
            // coordinates.
            data[i] = window.calculateDisplay(projection.nonLinearTransform(data[i]));

            // Draw the cell frames.
            if (parameterStore.get(calorimeters[i].getName(), "Cells").getStatus())
            {
                drawFrames(calorimeters[i], data[i], window, ag);
            }
        }

        for (int i = 0; i < calorimeters.length; i++)
        {
            // Draw the cell geometry.
            if (parameterStore.get(calorimeters[i].getName(), "Cells").getStatus())
            {
                drawGeometry(calorimeters[i], data[i], window, ag);
            }
        }

        String[] histograms = { "TILE", "HEC", "LAr", "FCAL", "MBTS" };
        for (int j = 0; j < histograms.length; j++)
        {
            for (int i = 0; i < calorimeters.length; i++)
            {
                if (calorimeters[i].getCalorimeterName().equals(histograms[j]))
                {
                    // Draw the histograms.
                    drawHistogram(calorimeters[i], window, ag, projection);
                }
            }
        }
        for (int j = 0; j < histograms.length; j++)
        {
            for (int i = 0; i < calorimeters.length; i++)
            {
                if (calorimeters[i].getCalorimeterName().equals(histograms[j]))
                {
                    // Draw the frames for the histograms.
                    frameHistogram(calorimeters[i], window, ag, projection);
                }
            }
        }

        for (int i = 0; i < calorimeters.length; i++)
        {
            if (parameterStore.get(calorimeters[i].getName(), "Cells").getStatus())
            {
                // Scale the cell polygons.
                data[i] = calorimeters[i].scalePolygons(data[i], area[i]);
                // And draw them.
                drawHits(calorimeters[i], data[i], window, ag);
            }
        }

    }

    /**
     * Draws the calorimeters in the standard mode v-plot. This method is called
     * from drawCalorimeters and should not be used directly.
     *
     * @param window AWindow window to use
     * @param ag AGraphics graphics object to draw onto
     * @param projection AProjection2D current projection
     * @param event AEvent drawn event reference
     */
    private static void drawCalorimetersVPlot(AWindow window, AGraphics ag, AProjection2D projection, AEvent event)
    {
        ACalorimeterData[] calorimeters = new ACalorimeterData[event.getCalorimeters().size()];
        event.getCalorimeters().toArray(calorimeters);

        float[][] energies = new float[calorimeters.length][];
        int[] sizes = new int[calorimeters.length];
        ACoord[] points = new ACoord[calorimeters.length];
        Color[] colorMap = AColorMap.getColors();
        //if color by energy use different colormap
        for (int i = 0; i < calorimeters.length; i++)
        {
            if(calorimeters[i].CALORIMETER_NAME.equals("MBTS")) continue;
            if (parameterStore.get(calorimeters[i].getParameterGroup(), "ColorFunction").getI()
                    == ACalorimeterData.COLOR_FUNC_ENERGY) {
                switch (AColorMap.getColorMap()) {
                    case AColorMap.COLOR_MAP_DEFAULT1:
                    case AColorMap.COLOR_MAP_DEFAULT2:
                    case AColorMap.COLOR_MAP_M4M5:
                    case AColorMap.COLOR_MAP_GRAYDET:
                    case AColorMap.COLOR_MAP_ORIGINAL:
                        // Use colors.
                        colorMap = AColorMap.getColors(AColorMap.COLOR_MAP_HITCOL);
                        break;
                    case AColorMap.COLOR_MAP_GRAY:
                    case AColorMap.COLOR_MAP_BW:
                        // Use grayscale.
                        colorMap = AColorMap.getColors(AColorMap.COLOR_MAP_GRAY_HITCOL);
                        break;
                }
            } else if (parameterStore.get(calorimeters[i].getParameterGroup(), "ColorFunction").getI() == COLOR_FUNC_TIME) {
                colorMap = AColorMap.getShades(128);
            }

        points[i] = window.calculateDisplay(calorimeters[i].getVPPoints());
        energies[i] = calorimeters[i].getET(points[i].index);
            sizes[i] = energies[i].length;
            calorimeters[i].color();
        }

        for (int i = 0; i < calorimeters.length; i++)
        {
            for (int j = 0; j < sizes[i]; j++)
            {
                int size = (int) Math.sqrt(energies[i][j] * SCALE_FACTOR_VPLOT);
                if (size == 0)
                    size++;

                int cellColor = calorimeters[i].color[points[i].index[j]];
                ag.setColor(colorMap[cellColor]);
                ag.drawPoint(calorimeters[i], points[i].index[j], points[i].hv[0][0][j],
                        points[i].hv[1][0][j], size, size);
            }
        }

    }

    /**
     * Calibrating involves finding the minimum and maximum energy density
     * values. This function takes the local (per calorimeter) values and
     * calculates the overall values (if needed).
     *
     * @param calorimeters ACalorimeterData[] calorimeter data objects
     * @param mode int calibration mode, use APar.EN_CALIB_UNKNOWN to retrieve
     *        the user setting
     */
    protected static void calibrateDensity(ACalorimeterData[] calorimeters, int mode)
    {
        if (mode == ENERGY_CALIB_UNKNOWN)
        {
            mode = parameterStore.get("LAr", "EnergyCalibration").getI();
        }

        // This array contains the calorimeter "types" to combine in the
        // calibration.
        String types[];
        switch (mode)
        {
            case ENERGY_CALIB_OVERALL:
                types = new String[] { "ALL" };
                break;

            case ENERGY_CALIB_BY_TYPE:
                types = new String[] { "ECAL", "HCAL" };
                break;

            case ENERGY_CALIB_BY_CALO:
            default:
                // Nothing to be done here.
                types = new String[0];
                break;
        }

        for (int i = 0; i < types.length; i++)
        {
            boolean first = true;
            double min = 0.0;
            double max = 0.0;
            for (int j = 0; j < calorimeters.length; j++)
            {
                if (calorimeters[j].getMaxDensity(types[i]) > 0)
                {
                    if (first)
                    {
                        min = calorimeters[j].getMinDensity(types[i]);
                        max = calorimeters[j].getMaxDensity(types[i]);
                        first = false;
                    }

                    if (calorimeters[j].getMinDensity(types[i]) < min)
                    {
                        min = calorimeters[j].getMinDensity(types[i]);
                    }
                    if (calorimeters[j].getMaxDensity(types[i]) > max)
                    {
                        max = calorimeters[j].getMaxDensity(types[i]);
                    }
                }
            }
            for (int j = 0; j < calorimeters.length; j++)
            {
                calorimeters[j].setMinDensity(types[i], min);
                calorimeters[j].setMaxDensity(types[i], max);
            }
        }
    }

    /**
     * Draws the frame for the cell geometry of a calorimeter. All frames are
     * drawn before drawing the cells themselves. This way a frame can never be
     * put on top of a cell from another calorimeter.
     *
     * @param calorimeter ACalorimeterData data collection to which this
     *        function applies
     * @param display ACoord cell coordinates to be drawn
     * @param window AWindow window to use
     * @param ag AGraphics graphics object to draw onto
     */
    protected static void drawFrames(ACalorimeterData calorimeter, ACoord display, AWindow window, AGraphics ag)
    {

        String name = calorimeter.getCalorimeterName();
        Color[] colorMap = AColorMap.getColors();
        int[] index = display.index;
        int[] type = calorimeter.getType(index);

        boolean drawCellGeometry = parameterStore.get(name, "CellGeometry").getStatus();
        boolean drawFrame = parameterStore.get(name, "Frame").getStatus();
        int frameColor = parameterStore.get(name, "Frame").getI();
        //only draw frames for Grey/BW color maps if is selected to draw frames
        if(drawFrame && AColorMap.drawFrames())
            drawFrame=true;
        else
            drawFrame=false;

        // For the v-plot we override these settings.
        if (window.getProjection() instanceof AProjectionVP)
        {
            drawCellGeometry = parameterStore.get("VP", "VPlotIsland").getStatus();
            drawFrame = true;
            frameColor = AColorMap.WH;
        }

        // Draw frames for the filled cells.
        if (drawFrame && drawCellGeometry)
        {
            for (int i = 0; i < index.length; i++)
            {
                ag.updateDrawParameters(calorimeter.getDrawParameters(0, type[i]));
                ag.setColor(colorMap[frameColor]);
                ag.drawPolygon(display.hv[0][i], display.hv[1][i], display.hv[0][i].length);
            }
        }
    }

    /**
     * Draws the frame for the cell geometry and outline of a calorimeter. All
     * frames are drawn before drawing the cells themselves. This way a frame
     * can never be put on top of a cell from another calorimeter.
     *
     * @param calorimeter ACalorimeterData data collection to which this
     *        function applies
     * @param display ACoord cell coordinates to be drawn
     * @param window AWindow window to use
     * @param ag AGraphics graphics object to draw onto
     */
    protected static void drawGeometry(ACalorimeterData calorimeter, ACoord display, AWindow window, AGraphics ag)
    {
        String name = calorimeter.getCalorimeterName();
        Color[] colorMap = AColorMap.getColors();
        int[] index = display.index;
        int[] type = calorimeter.getType(index);

        boolean drawCellGeometry = parameterStore.get(name, "CellGeometry").getStatus();
        boolean drawCellOutline = parameterStore.get(name, "CellOutline").getStatus();
        boolean cutByLayer = false;
        int cellGeometryColor = parameterStore.get(name, "CellGeometry").getI();
        int cellOutlineColor = parameterStore.get(name, "CellOutline").getI();
        int cutLayer = 0;
       
        // For the v-plot we override some of these settings.
        if (window.getProjection() instanceof AProjectionVP)
        {
            drawCellGeometry = parameterStore.get("VP", "VPlotIsland").getStatus();
            cellGeometryColor = parameterStore.get("VP", "VPlotIsland").getI();

            int mode = parameterStore.get("VP", "Mode").getI();

            if (mode >= AProjectionVP.MODE_HCAL_LAYER_0)
            {
                cutByLayer = true;
                cutLayer = mode - AProjectionVP.MODE_HCAL_LAYER_0;
            }
            else if (mode >= AProjectionVP.MODE_ECAL_LAYER_0)
            {
                cutByLayer = true;
                cutLayer = mode - AProjectionVP.MODE_ECAL_LAYER_0;
            }
        }

        // Draw filled cells.
        if (drawCellGeometry)
        {
            for (int i = 0; i < index.length; i++)
            {
                ag.updateDrawParameters(calorimeter.getDrawParameters(1, type[i]));
                ag.setColor(colorMap[cellGeometryColor]);
                ag.fillPolygon(display.hv[0][i], display.hv[1][i], display.hv[0][i].length);
            }
        }

        // Draw cell outlines.
        if (drawCellOutline)
        {
            ag.updateDrawParameters(new ADrawParameters(true, cellOutlineColor, 0, 1, 0, 0, false, 1, 0));
            for (int i = 0; i < index.length; ++i)
            {
                if (!cutByLayer || calorimeter.getSampling(index[i]) == cutLayer)
                {
                    ag.drawPolygon(display.hv[0][i], display.hv[1][i], display.hv[0][i].length);
                }
            }
        }
    }

    /**
     * Draws the calorimeter hits.
     *
     * @param calorimeter ACalorimeterData calorimeter object the hits belong to
     * @param display ACoord cell coordinates to be drawn
     * @param window AWindow window to use
     * @param ag AGraphics graphics object to draw onto
     */
    protected static void drawHits(ACalorimeterData calorimeter, ACoord display, AWindow window, AGraphics ag)
    {
        String name = calorimeter.getCalorimeterName();
        int[] index = display.index;
        int[] color = calorimeter.getColor(index);
        int[] type = calorimeter.getType(index);
        int[] sampling = calorimeter.getSampling(index);

        // This cut is performed just before drawing the hits, the geometry has
        // been drawn already.
        // This results in the irregular v-plot islands that help the user
        // locate a certain position in different views.
        boolean cutByLayer = false;
        int cutLayer = 0;

        if (window.getProjection() instanceof AProjectionVP)
        {
            int mode = parameterStore.get("VP", "Mode").getI();

            if (mode >= AProjectionVP.MODE_HCAL_LAYER_0)
            {
                cutByLayer = true;
                cutLayer = mode - AProjectionVP.MODE_HCAL_LAYER_0;
            }
            else if (mode >= AProjectionVP.MODE_ECAL_LAYER_0)
            {
                cutByLayer = true;
                cutLayer = mode - AProjectionVP.MODE_ECAL_LAYER_0;
            }
        }

        AParameter cellGeometry = parameterStore.get(name, "CellGeometry");
        AParameter colorFunction = parameterStore.get(name, "ColorFunction");
        boolean drawFrame = parameterStore.get(name, "Frame").getStatus();
        int frameColor = parameterStore.get(name, "Frame").getI();
        //only draw frames for Grey/BW color maps if is selected to draw frames
        if(drawFrame && AColorMap.drawFrames())
            drawFrame=true;
        else
            drawFrame=false;
        
        Color[] colorMap = AColorMap.getColors();
        if (colorFunction.getI() == COLOR_FUNC_ENERGY)
        {
            switch (AColorMap.getColorMap())
            {
                case AColorMap.COLOR_MAP_DEFAULT1:
                case AColorMap.COLOR_MAP_DEFAULT2:
                case AColorMap.COLOR_MAP_M4M5:
                case AColorMap.COLOR_MAP_GRAYDET:
                case AColorMap.COLOR_MAP_ORIGINAL:
                    // Use colors.
                    colorMap = AColorMap.getColors(AColorMap.COLOR_MAP_HITCOL);
                    break;
                case AColorMap.COLOR_MAP_GRAY:
                case AColorMap.COLOR_MAP_BW:
                    // Use grayscale.
                    colorMap = AColorMap.getColors(AColorMap.COLOR_MAP_GRAY_HITCOL);
                    break;
            }
        }
        else if (colorFunction.getI() == COLOR_FUNC_TIME)
        {
            colorMap = AColorMap.getShades(128);
        }

        if (drawFrame && !cellGeometry.getStatus())
        {
            for (int i = 0; i < index.length; i++)
            {
                if (!cutByLayer || sampling[i] == cutLayer)
                {
                    ag.updateDrawParameters(calorimeter.getDrawParameters(0, type[i]));
                    ag.setColor(colorMap[frameColor]);
                    ag.drawPolygon(display.hv[0][i], display.hv[1][i], display.hv[0][i].length);
                }
            }
        }

        for (int i = 0; i < index.length; i++)
        {
            if (!cutByLayer || sampling[i] == cutLayer)
            {
                ag.updateDrawParameters(calorimeter.getDrawParameters(1, type[i]));
                ag.setColor(colorMap[color[i]]);
                ag.fillPolygon(calorimeter, index[i], display.hv[0][i], display.hv[1][i], display.hv[0][i].length);
            }
        }
    }

    /**
     * Draws the calorimeter histograms.
     *
     * @param calorimeter ACalorimeterData calorimeter object the histograms
     *        belong to
     * @param window AWindow window to use
     * @param ag AGraphics graphics object to draw onto
     * @param projection AProjection2D current projection
     */
    protected static void drawHistogram(ACalorimeterData calorimeter, AWindow window, AGraphics ag, AProjection2D projection)
    {
        String name = calorimeter.getCalorimeterName();
        if (!parameterStore.get(name, "Histogram").getStatus())
            return;

        ACoord[] histograms = calorimeter.getUserHistograms(projection);
        for (int i = 0; i < histograms.length; i++)
        {
            ACoord display = window.calculateDisplay(histograms[i]);
            AHistogram hist = (AHistogram) histograms[i].source;
            hist.completeTowers(display);

            // for rubberband "new list" or "summarize" feature
            if (ag.getGraphics2D() instanceof ADrawnGraphics2D)
                ((ADrawnGraphics2D) ag.getGraphics2D()).addHistogramData(hist);

            boolean onTop;
            if (parameterStore.getUnknown(name, "OnTopHistoFrame") != null)
            {
                onTop = parameterStore.get(name, "OnTopHistoFrame").getStatus();
            }
            else
            {
                onTop = false;
            }

            // Draw frame for the histogram.
            if (parameterStore.get(name, "HistoFrame").getStatus() && !onTop)
            {
                int frameColor = parameterStore.get(name, "HistoFrame").getI();

                ag.updateDrawParameters(new ADrawParameters(true, frameColor, 0, 0, 1, 0));
                for (int j = 0; j < display.hv[0].length; j++)
                {
                    ag.drawPolygon(display.hv[0][j], display.hv[1][j], display.hv[0][j].length);
                }
            }

            // Draw histogram towers.
            int fillColor = parameterStore.get(name, "Histogram").getI();
            ag.updateDrawParameters(new ADrawParameters(true, fillColor, 0, 1, 0, 0));
            for (int j = 0; j < display.hv[0].length; j++)
            {
                ag.fillPolygon(hist, display.index[j], display.hv[0][j], display.hv[1][j], display.hv[0][j].length);
            }
        }
    }

    /**
     * Draws the calorimeter histogram frames.
     *
     * @param calorimeter ACalorimeterData calorimeter object the histograms
     *        belong to
     * @param window AWindow window to use
     * @param ag AGraphics graphics object to draw onto
     * @param projection AProjection2D current projection
     */
    protected static void frameHistogram(ACalorimeterData calorimeter, AWindow window, AGraphics ag, AProjection2D projection)
    {
        String name = calorimeter.getCalorimeterName();
        if (!parameterStore.get(name, "Histogram").getStatus())
            return;

        ACoord[] histograms = calorimeter.getUserHistograms(projection);
        for (int i = 0; i < histograms.length; i++)
        {
            ACoord display = window.calculateDisplay(histograms[i]);
            AHistogram hist = (AHistogram) display.source;

            hist.completeTowers(display);

            boolean onTop = false;
            if (parameterStore.getUnknown(name, "OnTopHistoFrame") != null)
            {
                onTop = parameterStore.get(name, "OnTopHistoFrame").getStatus();
            }

            // Draw frame for the histogram.
            if (parameterStore.get(name, "HistoFrame").getStatus() && onTop)
            {
                int frameColor = parameterStore.get(name, "HistoFrame").getI();

                ag.updateDrawParameters(new ADrawParameters(true, frameColor, 0, 1, 0, 0));
                for (int j = 0; j < display.hv[0].length; j++)
                {
                    ag.drawPolygon(display.hv[0][j], display.hv[1][j], display.hv[0][j].length);
                }
            }
        }
    }

    public static void drawEndcapHistogram(ACalorimeterData calorimeter, AWindow window, AGraphics ag, AProjection2D projection)
    {
        //filter the draw list for the histograms
        calorimeter.filterDrawList(projection);
        //Draw the histograms.
        drawHistogram(calorimeter, window, ag, projection);
        // Draw the frames for the histograms.
        frameHistogram(calorimeter, window, ag, projection);
    }



    /**
     * getLookupTableArray() parses String[][] s with comma separated values
     * (CSV) and converts each number to float lookup table value which is used
     * for calorimeter real pulse shapes plots
     * @param s String[][]
     * @param numLookupTableValues int
     * @return float[]
     * @throws AAtlantisException
     */
    protected static float[] getLookupTableArray(String[][] s, int numLookupTableValues)
                                         throws AAtlantisException
    {
        float[] r = new float[numLookupTableValues];
        int i = 0;
        for(int k = 0; k < s.length; k++)
        {
            for(int l = 0; l < s[k].length; l++)
            {
                if("".equals(s[k][l]))
                {
                    // command and new line results into empty value - ignore
                    continue;
                }
                try
                {
                    // there shouldn't be more than NUMBER_OF_LOOKUP_VALUES
                    // i.e. numLookupTableValues
                    if(i >= numLookupTableValues)
                    {
                        String m = "more lookup values than NUMBER_OF_LOOKUP_VALUES: " +
                                   i + " >= " + numLookupTableValues;
                        throw new AAtlantisException(m);
                    }
                    r[i] = Float.valueOf(s[k][l]).floatValue();
                    i++;
                }
                catch(NumberFormatException ex)
                {
                    throw new AAtlantisException("number format exception: " + s[k][l]);
                }
            }
        }

        // now the i counter shall be equal to numLookupTableValues
        if(i != numLookupTableValues)
        {
            throw new AAtlantisException("incorrect number to items (" + i + ") " +
                      "in the lookup table, should be " + numLookupTableValues);
        }

        // all right, return the result float array
        return r;

    } // getLookupTableArray() ----------------------------------------------



    /**
     * Calculate real pulse shapes values based on the values in the
     * lookup tables (parameters amplitude and time, etc)

     * @param xTime double
     * @param cellTime double
     * @param cellPedestal double
     * @param energy double
     * @param amplitude float[] lookup table
     * @param time float[] lookup table
     * @param numerOfLookupValues int
     * @return double
     * @throws AAtlantisException
     */
    protected double getPhysicsPulseShape(double localTime,
                                        double cellPedestal, double energy,
                                        float[] amplitude, float[] time,
                                        int numerOfLookupValues)
                                        throws AAtlantisException
    {
        double tdiv = 0.0;
        int lookup = 0;
        double xpulse = 0.0;

        tdiv = time[1] - time[0];
        lookup = (int) ((localTime - time[0]) / tdiv);

        if(lookup < 0)
        {
            lookup = 0;
        }
        if(lookup >= numerOfLookupValues - 1)
        {
            lookup = numerOfLookupValues - 2; // -1 was off by 1
        }

        try
        {
            if(lookup == 0 || lookup == numerOfLookupValues - 2)
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
            String m = CALORIMETER_NAME + " getPhysicsPulseShape():\n" +
                       "  lookup index out of bound: lookup = " + lookup;
            throw new AAtlantisException(m);
        }

        return (xpulse * energy) + cellPedestal;

    } // getPhysicsPulseShape() ---------------------------------------------
    
    

    /**
     *
     * Events from DPD don't contain all necessary data
     * if all adc counts are 0, then print info message and
     * no pulse shape plot window as it may lead to confusion.
     * This method is aimed to check availability of adc counts digits.
     * 
     * @param adcCountsLocal
     * @return true if any ADC counts are non-zero
     */
    protected static boolean checkADCCountsAvailability(int[][] adcCountsLocal)
    {
        boolean adcCountsAvailable = false;
        
        for(int x = 0; x < adcCountsLocal.length; x++)
        {
            for(int y = 0; y < adcCountsLocal[0].length; y++)
            {
                if(adcCountsLocal[x][y] != 0)
                {
                    adcCountsAvailable = true;
                    break;
                }
            }
            if(adcCountsAvailable)
            {
                break;
            }
        }
        
        return adcCountsAvailable;
        
    } // checkADCCountsAvailability() ---------------------------------------
    

}
