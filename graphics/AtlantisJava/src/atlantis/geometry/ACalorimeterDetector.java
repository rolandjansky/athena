package atlantis.geometry;

import atlantis.canvas.AWindow;
import atlantis.graphics.ACoord;
import atlantis.graphics.AClipper;
import atlantis.graphics.AGraphics;
import atlantis.graphics.colormap.AColorMap;
import atlantis.utils.AAtlantisException;
import atlantis.utils.AIdHelper;
import atlantis.utils.AMath;
import atlantis.parameters.APar;
import atlantis.parameters.AParameter;
import atlantis.projection.AProjection;
import atlantis.projection.AProjection2D;
import atlantis.projection.AProjectionFR;
import atlantis.projection.AProjectionFZ;
import atlantis.projection.AProjectionRZ;
import atlantis.projection.AProjectionVP;
import atlantis.projection.AProjectionYX;
import java.awt.Color;
import java.util.List;
import java.util.ArrayList;

/**
 * Abstract class representing a region of a certain sampling of a ATLAS
 * calorimeter. Based on the parameters given to the constructor, the
 * derived classes should be able to calculate the geometrical properties
 * of the cells they contain. This behavior is required for drawing the data.
 *
 * @author Eric Jansen
 */
abstract public class ACalorimeterDetector extends ADetector {

    // Keep a static list of instances for easy access
    protected static List detectors = null;

    // These arrays are used for fast cell to detector mapping.
    protected static int[][][] lar = null;
    protected static int[][] hec = null;
    protected static int[] tile = null;
    protected static int[] tile_ext = null;
    protected static int[][] tile_gap = null;
    protected static int[] mbts = null;

    protected int sampling;
    protected int region;
    protected double rMin;
    protected double rMax;
    protected double zMin;
    protected double zMax;
    protected double phi0;
    protected int numPhi;
    protected double deltaPhi;
    protected double eta0;
    protected double deltaEta;
    protected int numEta;
    protected int minEta;

    protected double detEtaMin;
    protected double detEtaMax;
    protected AClipper clipper;

    /**
     * Constructs a new calorimeter object.
     * @param name String name of the object
     * @param color String name of the color used to draw this object
     */
    public ACalorimeterDetector(String name, String color) {
        super(name, " ", color);

        if (detectors == null) {
            detectors = new ArrayList(40);

            // These numbers correspond to the maximum number of values that can
            // be encoded in the identifier.
            lar = new int[4][4][8];
            hec = new int[4][2];
            tile = new int[16];
            tile_ext = new int[16];
            tile_gap = new int[16][64];
            mbts = new int[2];

            // Initialize them to -1, which means no detector element known.
            for (int i=0; i<lar.length; i++)
                for (int j=0; j<lar[i].length; j++)
                    for (int k=0; k<lar[i][j].length; k++)
                        lar[i][j][k] = -1;

            for (int i=0; i<hec.length; i++)
                for (int j=0; j<hec[i].length; j++)
                    hec[i][j] = -1;

            for (int i=0; i<tile.length; i++)
                tile[i] = -1;

            for (int i=0; i<tile_ext.length; i++)
                tile_ext[i] = -1;

            for (int i=0; i<tile_gap.length; i++)
                for (int j=0; j<tile_gap[i].length; j++)
                    tile_gap[i][j] = -1;

            for (int i=0; i<mbts.length; i++)
                mbts[i] = -1;
        }

        detectors.add(this);
    }

    /**
     * Returns one of the instances of this class.
     * @param index int index of instance
     * @return ACalorimeterDetector instance at the requested index
     */
    public static ACalorimeterDetector get(int index) {
        return (ACalorimeterDetector) detectors.get(index);
    }

    /**
     * Shows the number of instances of this class that have been created.
     * @return int number of instances
     */
    public static int count() {
        if (detectors != null) {
            return detectors.size();
        } else {
            return 0;
        }
    }

    /**
     * This functions maps the calorimeter cells to a geometry object. In principle we could
     * compare every cell to every geometry object, but that would be too slow. Therefore we
     * organize the geometry in an array structure.
     * @param id int identifier of a cell
     * @return int index of the geometry object in the detector array
     */
    public static int getDetectorIndex(int id) {
        if (detectors == null) return -1;
        
         try {
            switch (AIdHelper.subDetector(id)) {
                case 4:
                    switch (AIdHelper.larPart(id)) {
                        case 1:
                            // LAr EM
                            return lar[Math.abs(AIdHelper.larBarrelEndcap(id))]
                                    [AIdHelper.larSampling(id)][AIdHelper.larRegion(id)];
                        case 2:
                            // HEC
                            return hec[AIdHelper.larSampling(id)][AIdHelper.larRegion(id)];
                        case 3:
                            // FCAL
                            return -1;
                        default:
                            return -1;
                    }
                case 5:
                    switch (AIdHelper.tileSection(id)) {
                        case 1:
                            // TILE barrel
                            return tile[AIdHelper.tileSampling(id)];
                        case 2:
                            // TILE extended barrel
                            return tile_ext[AIdHelper.tileSampling(id)];
                        case 3:
                            // ITC gap/crack
                            return tile_gap[AIdHelper.tileSampling(id)][AIdHelper.tileTower(id)];
                        case 4:
                            // MBTS
                            return -1;
                        default:
                            return -1;
                    }
                default:
                    return -1;
            }
        } catch (AAtlantisException e) {
            return -1;
        }
    }

    /**
     * Clear the list of instances. This is called before reading another geometry file.
     */
    public static void clear() {
        if (detectors != null) {
            detectors.clear();
        }
        detectors = null;
        lar = null;
        hec = null;
        tile = null;
        tile_ext = null;
        tile_gap = null;
        mbts = null;
    }

    /**
     * Get the outline of a certain cell in a certain projection.
     * @param projection AProjection projection
     * @param eta int eta index of the cell
     * @param phi int phi index of the cell
     * @param side int side sign of the z coordinate of the cell
     * @return ACoord cell outline
     */
    public ACoord getCell(AProjection projection, int eta, int phi, int side) {
        if (projection instanceof AProjectionYX) {
            return getYXCell(eta, phi);
        } else if (projection instanceof AProjectionRZ) {
            return getRZCell(eta, phi, side);
        } else if (projection instanceof AProjectionFR) {
            return getFRCell(eta, phi);
        } else if (projection instanceof AProjectionFZ) {
            return getFZCell(eta, phi, side);
        } else if (projection instanceof AProjectionVP) {
            return getVPCell(eta, phi, side);
        } else {
            return ACoord.NO_DATA;
        }
    }

    /**
     * Returns the y-x geometry of a specific cell in this calorimeter part.
     * @param eta int eta index of cell
     * @param phi int phi index of cell
     * @return ACoord polygon representing cell geometry
     */
    public ACoord getYXCell(int eta, int phi) {
        return ACoord.NO_DATA;
    }

    /**
     * Returns the rho-z geometry of a specific cell in this calorimeter part.
     * @param eta int eta index of cell
     * @param phi int phi index of cell
     * @param side int sign of the z coordinate of cell
     * @return ACoord polygon representing cell geometry
     */
    public ACoord getRZCell(int eta, int phi, int side) {
        return ACoord.NO_DATA;
    }

    /**
     * Returns the phi-rho geometry of a specific cell in this calorimeter part.
     * @param eta int eta index of cell
     * @param phi int phi index of cell
     * @return ACoord polygon representing cell geometry
     */
    public ACoord getFRCell(int eta, int phi) {
        return convertYXToFR(getYXCell(eta, phi));
    }

    /**
     * Returns the phi-z geometry of a specific cell in this calorimeter part.
     * @param eta int eta index of cell
     * @param phi int phi index of cell
     * @param side int sign of the z coordinate of cell
     * @return ACoord
     */
    public ACoord getFZCell(int eta, int phi, int side) {
        return ACoord.NO_DATA;
    }

    /**
     * Returns the eta-phi geometry of a specific cell in this calorimeter part.
     * @param eta int eta index of cell
     * @param phi int phi index of cell
     * @param side int sign of the z coordinate of cell
     * @return ACoord polygon representing cell geometry
     */
    public ACoord getVPCell(int eta, int phi, int side) {
        return ACoord.NO_DATA;
    }

    /**
     * Returns the eta value of the center (in eta) of a cell.
     * @param eta int eta index of the cell
     * @param side int sign of the z coordinate of the cell
     * @return double eta value
     */
    public double getEta(int eta, int side) {
        if (side < 0) {
            return -getEta(eta, -side);
        } else {
            return eta0 + (eta - minEta + 0.5) * deltaEta;
        }
    }

    /**
     * Returns the lowest eta value of a cell.
     * @param eta int eta index of the cell
     * @param side int sign of the z coordinate of the cell
     * @return double lowest eta value
     */
    public double getEtaMin(int eta, int side) {
        if (side < 0) {
            return -getEtaMax(eta, -side);
        } else {
            return eta0 + (eta - minEta) * deltaEta;
        }
    }

    /**
     * Returns the highest eta value of a cell.
     * @param eta int eta index of the cell
     * @param side int sign of the z coordinate of the cell
     * @return double highest eta value
     */
    public double getEtaMax(int eta, int side) {
        if (side < 0) {
            return -getEtaMin(eta, -side);
        } else {
            return eta0 + (eta - minEta + 1) * deltaEta;
        }
    }

    /**
     * Returns the phi value of the center of a cell.
     * @param phi int phi index of the cell
     * @return double phi value
     */
    public double getPhi(int phi) {
        return phi0 + (phi + 0.5) * deltaPhi;
    }

    /**
     * Returns the lowest phi value of a cell.
     * @param phi int phi index of the cell
     * @return double lowest phi value
     */
    public double getPhiMin(int phi) {
        return phi0 + phi * deltaPhi;
    }

    /**
     * Returns the highest phi value of a cell.
     * @param phi int phi index of the cell
     * @return double highest phi value
     */
    public double getPhiMax(int phi) {
        return phi0 + (phi + 1) * deltaPhi;
    }

    /**
     * Determines on which side a certain cell should be drawn in the RZ projection,
     * based on the phi angle the user has set in the menu.
     * @param phi int phi index of the cell
     * @return int 1 or -1, meaning top or bottom repectively
     */
    public int getRSign(int phi) {
        double cellPhi = phi0 + phi*deltaPhi + deltaPhi/2.;
        double phiMid = Math.toRadians(parameterStore.get("RZ", "Phi").getD());
        double phiDiff = Math.abs(cellPhi-phiMid);
        if (phiDiff <= Math.PI/2.|| phiDiff > 3*Math.PI/2.) {
            return 1;
        } else {
            return -1;
        }
    }

    /**
     * Returns the number of cells in phi for this calorimeter part.
     * @return int number of cells in phi
     */
    public int getNumPhi() {
        return numPhi;
    }

    /**
     * Returns the size of a cell in phi for this calorimeter part (this is the same for all cells).
     * @return double size of a cell in radians
     */
    public double getDeltaPhi() {
        return deltaPhi;
    }

    /**
     * Returns the number of cells in eta for this calorimeter part.
     * @return double number of cells in eta
     */
    public int getNumEta() {
        return numEta;
    }

    /**
     * Returns the size of a cell in eta for this calorimeter part (this is the same for all cells).
     * @return double size of a cell in eta
     */
    public double getDeltaEta() {
        return deltaEta;
    }

    /**
     * Returns the center radius of the calorimeter part.
     * @return double center radius
     */
    public double getR() {
        return (rMin+rMax)/2.;
    }

    /**
     * Returns the inner radius of the calorimter part.
     * @return double inner radius
     */
    public double getRMin() {
        return rMin;
    }

    /**
     * Returns the outer radius of the calorimeter part.
     * @return double outer radius
     */
    public double getRMax() {
        return rMax;
    }

    /**
     * Returns the center z of the calorimeter part.
     * @return double center z
     */
    public double getZ() {
        return (zMin+zMax)/2.f;
    }

    /**
     * Returns the inner z of the calorimeter part.
     * @return double inner z
     */
    public double getZMin() {
        return zMin;
    }

    /**
     * Returns the outer z of the calorimter part.
     * @return double outer z
     */
    public double getZMax() {
        return zMax;
    }

    /**
     * Returns the inner eta of the calorimeter part.
     * @return double inner eta
     */
    public double getEtaMin() {
        return eta0;
    }

    /**
     * Returns the outer eta of the calorimeter part.
     * @return double outer eta
     */
    public double getEtaMax() {
        return eta0 + numEta*deltaEta;
    }

    /**
     * Returns the sampling of the calorimeter part. This corresponds to the layers of the calorimeter.
     * @return int sampling
     */
    public int getSampling() {
        return sampling;
    }

    /**
     * Returns the region of this calorimeter part. Within a layer a new region is started when the
     * size of the cells changes.
     * @return int region
     */
    public int getRegion() {
        return region;
    }
    
    /**
     * Returns the eta index of the first cell in eta for this calorimeter part.
     * @return int eta
     */
    public int getFirstEta() {
        return minEta;
    }

    /**
     * Returns the eta index of the last cell in eta for this calorimeter part.
     * @return int eta
     */
    public int getLastEta() {
        if (sampling == 2 && name.indexOf("TILE") >= 0)
            return minEta + 2*numEta;
        else
            return minEta + numEta-1;
    }
    
    /**
     * Returns the phi index of the first cell in phi for this calorimeter part.
     * @return int phi
     */
    public int getFirstPhi() {
        // This is always 0, still we provide this method for consistency.
        return 0;
    }

    /**
     * Returns the phi index of the last cell in phi for this calorimeter part.
     * @return int phi
     */
    public int getLastPhi() {
        return numPhi-1;
    }
        
    /**
     * This function draws the module structure of the calorimeter. It is called
     * from ADetectorSystem.draw().
     * @param window AWindow window to use
     * @param ag AGraphics graphics object to draw onto
     * @param projection AProjection2D current projection
     */
    public static void drawAdditionalItems(AWindow window, AGraphics ag, AProjection2D projection) {
        AParameter caloDetail = parameterStore.getUnknown("Det", "CaloDetail");
        if (caloDetail != null && detectors != null && caloDetail.getStatus()) 
        {
            for (int i=0; i<detectors.size(); i++) 
            {
                ACalorimeterDetector detector = (ACalorimeterDetector) detectors.get(i);

                // Only draw region 0 in YX, not the few different cells at the ends
                if ((projection instanceof AProjectionYX && detector.getRegion() == 0) || projection instanceof AProjectionFR)
                {
                    int start;
                    int mode = parameterStore.get("YX", "Mode").getI();
                    if(mode==AProjectionYX.MODE_STANDARD)
                        //only needs one eta for standard view
                        start = detector.getFirstEta();
                    else if(mode>=AProjectionYX.MODE_LAR_ENDCAP_PRESAMPLER && mode <=AProjectionYX.MODE_HEC_4)
                        //need to loop over eta for endcaps
                        start = detector.getLastEta();
                    else
                        start=0;
                    if(mode==AProjectionYX.MODE_STANDARD || (mode>=AProjectionYX.MODE_LAR_ENDCAP_PRESAMPLER && mode <=AProjectionYX.MODE_HEC_4) || mode==AProjectionYX.MODE_MBTS)
                    {
                        for (int eta=start; eta>=detector.getFirstEta(); eta--) 
                        {
                            for (int phi=detector.getFirstPhi(); phi<=detector.getLastPhi(); phi++) 
                            {
                                ACoord coord = detector.getCell(projection, eta, phi, 1);
                                if (coord != ACoord.NO_DATA) {
                                    ACoord display;
                                    if (projection instanceof AProjectionYX) 
                                        display = window.calculateDisplay(projection.nonLinearTransform(coord));
                                    else
                                        display = window.calculateDisplay(projection.nonLinearTransform(
                                                coord.includePhiWrapAround("FR"))
                                        );
        
                                    Color[] colorMap = AColorMap.getColors();
                                    ag.setColor(colorMap[parameterStore.get("Det", "CaloDetail").getI()]);
                                    
                                    for (int j=0; j<display.hv[0].length; j++)
                                       ag.drawPolygon(display.hv[0][j], display.hv[1][j], display.hv[0][j].length);
                                }
                            }
                        }
                    }
                    else if((mode==AProjectionYX.MODE_LAR_ENDCAP_SUMMED && (detector.getName().equals("LAr Endcap Presampler")||detector.getName().equals("LAr_EC_Presampler")))  
                            || (mode==AProjectionYX.MODE_HEC_SUMMED && detector.getName().equals("HEC") && detector.getSampling()==0))
                    {
                        //For summed endcaps only need to do this once
                        //The cells match the bining used but are not specific to the detector
                        //hence they do not all lie inside the detector
                        double etaBin=0.1,phiBin=(2*Math.PI/64);
                        int binvalue;
                        if(mode==AProjectionYX.MODE_HEC_SUMMED)
                        {
                            binvalue = parameterStore.get("YX", "HECBin").getI();
                        }
                        else
                        {
                            binvalue = parameterStore.get("YX", "LArBin").getI();
                        }
                        if(binvalue==3)
                        {
                            etaBin=0.1;
                            phiBin=(2*Math.PI/64);
                        }
                        else
                        {
                            etaBin*=binvalue;
                            phiBin*=binvalue;
                        }
                        Color[] colorMap = AColorMap.getColors();
                        ag.setColor(colorMap[parameterStore.get("Det", "CaloDetail").getI()]);
                        double firstZMin=detector.getZMin();
                        //check etaSplit is set correct
                        if(binvalue==3)
                        {
                            setEtaSplit();
                        }
                        //loop between values greater and less than the actual size
                        for(double Eta=1.2; Eta<3.3; Eta+=etaBin)
                        {
                            if(binvalue==3 && Eta>=parameterStore.get("YX", "SplitEta").getD())
                            {
                                etaBin=0.2;
                                phiBin=(2*Math.PI/32);
                            }
                            double RMax=firstZMin/Math.sinh(Eta);
                            double RMin=firstZMin/Math.sinh(Eta+etaBin);
                            for(double Phi=0.0; Phi<(2*Math.PI); Phi+=phiBin)
                            {
                                double[][] hv = new double[2][];
                                hv[0] = new double[] {RMax*Math.cos(Phi), RMax*Math.cos(Phi+phiBin),
                                        RMin*Math.cos(Phi+phiBin), RMin*Math.cos(Phi)};
                                hv[1] = new double[] {RMax*Math.sin(Phi), RMax*Math.sin(Phi+phiBin),
                                        RMin*Math.sin(Phi+phiBin), RMin*Math.sin(Phi)};
 
                                ACoord coord = new ACoord(hv);
                                ACoord display;
                                
                                if (projection instanceof AProjectionYX) 
                                    display = window.calculateDisplay(projection.nonLinearTransform(coord));
                                else
                                    display = window.calculateDisplay(projection.nonLinearTransform(
                                            coord.convertYXToFR().includePhiWrapAround("FR"))
                                    );
                                for(int j=0; j<display.hv[0].length; j++)
                                    ag.drawPolygon(display.hv[0][j], display.hv[1][j], display.hv[0][j].length);
                            }
                        }
                    }
                    
                } 
                else if (projection instanceof AProjectionRZ) 
                {
                    for (int eta=detector.getFirstEta(); eta<=detector.getLastEta(); eta++) 
                    {
                        
                        for (int phi=detector.getFirstPhi();
                                phi<=detector.getFirstPhi()+detector.getNumPhi()/2;
                                phi+=detector.getNumPhi()/2) 
                        {
                            
                            for (int side=-1; side<=1; side+=2) {
                                ACoord coord = detector.getCell(projection, eta, phi, side);
                        
                                if (coord != ACoord.NO_DATA) 
                                {
                                    ACoord display = window.calculateDisplay(projection.nonLinearTransform(coord));
                                    Color[] colorMap = AColorMap.getColors();
                                    ag.setColor(colorMap[parameterStore.get("Det", "CaloDetail").getI()]);
                                    ag.drawPolygon(display.hv[0][0], display.hv[1][0], display.hv[0][0].length);
                                }
                            }
                        }
                    }                    
                } 
                else if (projection instanceof AProjectionFZ) 
                {
                    int eta = detector.getFirstEta();

                    for (int phi=detector.getFirstPhi(); phi<=detector.getLastPhi(); phi++) 
                    {                        
                        for (int side=-1; side<=1; side+=2) 
                        {
                            ACoord coord = detector.getCell(projection, eta, phi, side).includePhiWrapAround("FZ");
                       
                            if (coord != ACoord.NO_DATA) 
                            {
                                ACoord display = window.calculateDisplay(projection.nonLinearTransform(coord));
                                Color[] colorMap = AColorMap.getColors();
                                ag.setColor(colorMap[parameterStore.get("Det", "CaloDetail").getI()]);
                                
                                for (int j=0; j<display.hv[0].length; j++) 
                                {
                                   ag.drawPolygon(display.hv[0][j], display.hv[1][j], display.hv[0][j].length);
                                }
                            }
                        }
                    }                    
                }
            }
        }
    }
    
    /**
     * This function sets the value of EtaSplit to a minimum of 1.2 and 
     * to an accuracy of 1 decimal place
     * 1.2 is minimum as is minimum of where cells outline is drawn
     */
    public static void setEtaSplit() {
        if(parameterStore.get("YX", "SplitEta").getD()<1.2)
            parameterStore.get("YX", "SplitEta").setD(1.2);
        else
        {
            int temp=(int) Math.floor(parameterStore.get("YX", "SplitEta").getD()*10.0);
            parameterStore.get("YX", "SplitEta").setD(temp/10.0);
        }
    }
    
}
