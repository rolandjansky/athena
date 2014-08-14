package atlantis.geometry;

import atlantis.canvas.AWindow;
import atlantis.data.AS3DData;
import atlantis.graphics.ACoord;
import atlantis.graphics.AGraphics;
import atlantis.graphics.colormap.AColorMap;
import atlantis.parameters.AParameter;
import atlantis.projection.AProjection2D;
import atlantis.projection.AProjectionFR;
import atlantis.projection.AProjectionYX;
import java.awt.Color;
import java.util.ArrayList;
import java.util.List;

/**
 * Class representing a pixel/SCT endcap detector part.
 * Also implements methods to draw more detailed structure of the detector.
 *
 * @author Eric Jansen
 */
public class AEndcapSiliconDetector extends ADetector {

    private static List detectors = null;

    protected int layer;
    protected double length;
    protected double width;
    protected double thickness;
    protected int numZ;
    protected int numPhi;
    protected double phi0;
    protected double zMin;
    protected double zMax;

    protected double deltaPhi;
    protected double deltaZ;
    protected double rMin;
    protected double rMax;

    public AEndcapSiliconDetector(String name, String color, int layer, double length, double width, double thickness,
                                  int numZ, int numPhi, double rMin, double rMax, double phi0, double zMin, double zMax) {
        super(name, " ", color);

        if (detectors == null) {
            detectors = new ArrayList();
        }
        detectors.add(this);

        this.layer = layer;
        this.length = length;
        this.width = width;
        this.thickness = thickness;
        this.numZ = numZ;
        this.numPhi = numPhi;
        this.rMin = rMin;
        this.rMax = rMax;
        this.phi0 = Math.toRadians(phi0);
        this.zMin = zMin;
        this.zMax = zMax;

        this.deltaPhi = 2.*Math.PI / this.numPhi;
        this.deltaZ = (this.zMax - this.zMin) / this.numZ;
    }

    /**
     * Empties the list of instances of this class.
     */
    public static void clear() {
        if (detectors != null) {
            detectors.clear();
        }
    }
    
    /**
     * This function draws the wafer/stave structure of the silicon detectors. It is called
     * from ADetectorSystem.draw().
     * @param window AWindow window to use
     * @param ag AGraphics graphics object to draw onto
     * @param projection AProjection2D current projection
     */
    public static void drawAdditionalItems(AWindow window, AGraphics ag, AProjection2D projection) {
        AParameter inDetDetail = parameterStore.getUnknown("Det", "SiliconDetail");
        if (inDetDetail != null && detectors != null && inDetDetail.getStatus()) {
            for (int i=0; i<detectors.size(); i++) {
                ACoord coord = ACoord.NO_DATA;
                AEndcapSiliconDetector detector = (AEndcapSiliconDetector) detectors.get(i);
                if (projection instanceof AProjectionYX) {
                    coord = detector.getYXDetail();
                } else if (projection instanceof AProjectionFR) {
                    coord = detector.getYXDetail().convertYXToFR().includePhiWrapAround("FR");
                }

                if (coord != ACoord.NO_DATA) {
                    Color[] colorMap = AColorMap.getColors();
                    ag.setColor(colorMap[parameterStore.get("Det", "SiliconDetail").getI()]);

                    ACoord display = window.calculateDisplay(projection.nonLinearTransform(coord));
                    for (int j=0; j<display.hv[0].length; j++) {
                        ag.drawPolygon(display.hv[0][j], display.hv[1][j], display.hv[0][j].length);
                    }
                }
            }
        }
    }

    /**
     * Determines if this element should be visible or not.
     * @return visibility
     */
    protected boolean isVisible() {
        int cut = parameterStore.get("CutsInDet", "SCT").getI();

        if (cut == AS3DData.CUT_INDET_SCT_ALL || cut == AS3DData.CUT_INDET_SCT_EC_NEG 
                || cut == AS3DData.CUT_INDET_SCT_EC_POS) {
            return true;
        } else {
            return false;
        }
    }
    
    /**
     * Returns the detector outline in the y-x projection.
     * @return ACoord polygon representing the detector
     */
    protected ACoord getYXUser() {
        if (parameterStore.get("YX", "Mode").getI() != AProjectionYX.MODE_STANDARD) return ACoord.NO_DATA;
        if (!isVisible()) return ACoord.NO_DATA;
        
        if (!globals.getUseOpenGL()) {
            int numPoints = 2 * (numPhi + 1);
            double[][] hv = new double[2][numPoints];

            for (int i = 0; i < numPhi + 1; i++) {
                hv[0][i] = rMin * Math.cos(phi0 + (i + 0.5) * deltaPhi);
                hv[1][i] = rMin * Math.sin(phi0 + (i + 0.5) * deltaPhi);

                hv[0][numPhi + 1 + i] = rMax * Math.cos(phi0 - (i + 0.5) * deltaPhi);
                hv[1][numPhi + 1 + i] = rMax * Math.sin(phi0 - (i + 0.5) * deltaPhi);
            }
            return new ACoord(hv);
        } else {
            return new ACoord(generateAnnulusTris(numPhi, phi0, rMin, rMax));
        }
    }

    /**
     * Returns the detector outline in the rho-z projection.
     * @return ACoord polygon representing the detector
     */
    protected ACoord getRZUser() {
        double[][][] hv = new double[2][4][];

        hv[0][0] = new double[] { zMax,  zMin,  zMin,  zMax};
        hv[1][0] = new double[] { rMax,  rMax,  rMin,  rMin};
        hv[0][1] = new double[] { zMax,  zMin,  zMin,  zMax};
        hv[1][1] = new double[] {-rMin, -rMin, -rMax, -rMax};

        hv[0][2] = new double[] {-zMax, -zMin, -zMin, -zMax};
        hv[1][2] = new double[] { rMax,  rMax,  rMin,  rMin};
        hv[0][3] = new double[] {-zMax, -zMin, -zMin, -zMax};
        hv[1][3] = new double[] {-rMin, -rMin, -rMax, -rMax};
        
        return new ACoord(hv);
    }

    /**
     * Returns the detector outline in the phi-z projection.
     * @return ACoord polygon representing the detector
     */
    protected ACoord getFZUser() {
        if (!isVisible()) return ACoord.NO_DATA;

        double[][][] hv = new double[2][2][];

        hv[0][0] = new double[] {zMax, zMin, zMin, zMax};
        hv[1][0] = new double[] {360., 360., 0., 0.};
        hv[0][1] = new double[] {-zMax, -zMin, -zMin, -zMax};
        hv[1][1] = new double[] {360., 360., 0., 0.};

        return new ACoord(hv);
    }

    /**
     * Returns the detector outline in the phi-rho projection.
     * @return ACoord polygon representing the detector
     */
    protected ACoord getFRUser() {
        if (!isVisible()) return ACoord.NO_DATA;

        return getYXUser().convertYXToFR();
    }

   /**
     * Returns the wafer/stave structure in the y-x projection.
     * @return ACoord polygons representing the wafers/staves
     */
    protected ACoord getYXDetail() {
        if (parameterStore.get("YX", "Mode").getI() != AProjectionYX.MODE_STANDARD) return ACoord.NO_DATA;
        if (!isVisible()) return ACoord.NO_DATA;

        double[][][] hv = new double[2][numPhi][];
        for (int i=0; i<numPhi; i++) {            
            hv[0][i] = new double[] {
                rMax*Math.sin(phi0+(i-0.5)*deltaPhi),
                rMax*Math.sin(phi0+(i+0.5)*deltaPhi),
                rMin*Math.sin(phi0+(i+0.5)*deltaPhi),
                rMin*Math.sin(phi0+(i-0.5)*deltaPhi)
            };
            hv[1][i] = new double[] {
                rMax*Math.cos(phi0+(i-0.5)*deltaPhi),
                rMax*Math.cos(phi0+(i+0.5)*deltaPhi),
                rMin*Math.cos(phi0+(i+0.5)*deltaPhi),
                rMin*Math.cos(phi0+(i-0.5)*deltaPhi)               
            };
        }

        return new ACoord(hv);
    }
}
