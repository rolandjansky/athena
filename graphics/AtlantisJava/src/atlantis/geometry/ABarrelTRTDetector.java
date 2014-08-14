package atlantis.geometry;

import atlantis.canvas.AWindow;
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
 * Class representing a TRT barrel detector part.
 * Also implements methods to draw more detailed structure of the detector.
 *
 * @author Eric Jansen
 */
public class ABarrelTRTDetector extends ADetector {

    private static List detectors = null;

    protected int layer;
    protected int numPhi;
    protected double rMin;
    protected double rMax;
    protected double phiIn;
    protected double phiOut;
    protected double zMin;
    protected double zMax;
    protected double deltaPhi;

    public ABarrelTRTDetector(String name, String color, int layer, int numPhi, double rMin, double rMax, 
                              double phiIn, double phiOut, double zMin, double zMax) {
        super(name, " ", color);

        if (detectors == null) {
            detectors = new ArrayList();
        }
        detectors.add(this);

        this.layer = layer;
        this.numPhi = numPhi;
        this.rMin = rMin;
        this.rMax = rMax;
        this.phiIn = Math.toRadians(phiIn);
        this.phiOut = Math.toRadians(phiOut);
        this.zMin = zMin;
        this.zMax = zMax;
        
        this.deltaPhi = 2.*Math.PI / numPhi;
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
     * This function draws the module structure of the TRT detector. It is called
     * from ADetectorSystem.draw().
     * @param window AWindow window to use
     * @param ag AGraphics graphics object to draw onto
     * @param projection AProjection2D current projection
     */
    public static void drawAdditionalItems(AWindow window, AGraphics ag, AProjection2D projection) {
        AParameter inDetDetail = parameterStore.getUnknown("Det", "TRTDetail");
        if (inDetDetail != null && detectors != null && inDetDetail.getStatus()) {
            for (int i=0; i<detectors.size(); i++) {
                ACoord coord = ACoord.NO_DATA;
                ABarrelTRTDetector detector = (ABarrelTRTDetector) detectors.get(i);
                if (projection instanceof AProjectionYX) {
                    coord = detector.getYXDetail();
                } else if (projection instanceof AProjectionFR) {
                    coord = detector.getYXDetail().convertYXToFR().includePhiWrapAround("FR");
                }

                if (coord != ACoord.NO_DATA) {
                    Color[] colorMap = AColorMap.getColors();
                    ag.setColor(colorMap[parameterStore.get("Det", "TRTDetail").getI()]);

                    ACoord display = window.calculateDisplay(projection.nonLinearTransform(coord));
                    for (int j=0; j<display.hv[0].length; j++) {
                        ag.drawPolygon(display.hv[0][j], display.hv[1][j], display.hv[0][j].length);
                    }
                }
            }
        }
    }

    /**
     * Returns the detector outline in the y-x projection.
     * @return ACoord polygon representing the detector
     */
    protected ACoord getYXUser() {
        if (parameterStore.get("YX", "Mode").getI() != AProjectionYX.MODE_STANDARD) return ACoord.NO_DATA;

        if (!globals.getUseOpenGL()) {
            int numPoints = 2 * (numPhi + 1);
            double[][] hv = new double[2][numPoints];

            for (int i = 0; i < numPhi + 1; i++) {
                hv[0][i] = rMin * Math.cos(phiIn + i * deltaPhi);
                hv[1][i] = rMin * Math.sin(phiIn + i * deltaPhi);

                hv[0][numPhi + 1 + i] = rMax * Math.cos(phiOut - i * deltaPhi);
                hv[1][numPhi + 1 + i] = rMax * Math.sin(phiOut - i * deltaPhi);
            }
            return new ACoord(hv);
        } else {
            return new ACoord(generateAnnulusTris(numPhi, phiIn, rMin, rMax));
        }
    }

    /**
     * Returns the detector outline in the rho-z projection.
     * @return ACoord polygon representing the detector
     */
    protected ACoord getRZUser() {
        double[][][] hv = new double[2][2][];

        hv[0][0] = new double[] { zMax,  zMin,  zMin,  zMax};
        hv[1][0] = new double[] { rMax,  rMax,  rMin,  rMin};
        hv[0][1] = new double[] { zMax,  zMin,  zMin,  zMax};
        hv[1][1] = new double[] {-rMin, -rMin, -rMax, -rMax};

        return new ACoord(hv);
    }

    /**
     * Returns the detector outline in the phi-z projection.
     * @return ACoord polygon representing the detector
     */
    protected ACoord getFZUser() {
        return ACoord.NO_DATA;
    }

    /**
     * Returns the detector outline in the phi-rho projection.
     * @return ACoord polygon representing the detector
     */
    protected ACoord getFRUser() {
        double[][] hv = new double[2][];

        hv[0] = new double[] {rMin, rMax, rMax, rMin};
        hv[1] = new double[] {360., 360., 0., 0.};

        return new ACoord(hv);
    }

    /**
     * Returns the module structure in the y-x projection.
     * @return ACoord polygons representing the modules
     */
    protected ACoord getYXDetail() {
        if (parameterStore.get("YX", "Mode").getI() != AProjectionYX.MODE_STANDARD) return ACoord.NO_DATA;

        double[][][] hv = new double[2][numPhi][];
        for (int i=0; i<numPhi; i++) {
            double phiMin = i*deltaPhi;
            double phiMax = (i+1)*deltaPhi;

            double x1 = rMax*Math.cos(phiMin+phiOut);
            double x2 = rMax*Math.cos(phiMax+phiOut);
            double x3 = rMin*Math.cos(phiMax+phiIn);
            double x4 = rMin*Math.cos(phiMin+phiIn);

            double y1 = rMax*Math.sin(phiMin+phiOut);
            double y2 = rMax*Math.sin(phiMax+phiOut);
            double y3 = rMin*Math.sin(phiMax+phiIn);
            double y4 = rMin*Math.sin(phiMin+phiIn);

            hv[0][i] = new double[] {x1, x2, x3, x4};
            hv[1][i] = new double[] {y1, y2, y3, y4};
        }
        return new ACoord(hv);
    }
}
