package atlantis.geometry;

import atlantis.canvas.AWindow;
import atlantis.graphics.ACoord;
import atlantis.graphics.AGraphics;
import atlantis.graphics.colormap.AColorMap;
import atlantis.parameters.APar;
import atlantis.parameters.AParameter;
import atlantis.projection.AProjection2D;
import atlantis.projection.AProjectionFZ;
import atlantis.projection.AProjectionRZ;
import java.awt.Color;
import java.util.ArrayList;
import java.util.List;

/**
 * Class representing a TRT endcap detector part.
 * Also implements methods to draw more detailed structure of the detector.
 *
 * @author Eric Jansen
 */
public class AEndcapTRTDetector extends ADetector {

    private static List detectors = null;

    protected int layer;
    protected int numPhi;
    protected double rMin;
    protected double rMax;
    protected double phi0;
    protected double zMin;
    protected double zMax;
    protected double deltaPhi;

    public AEndcapTRTDetector(String name, String color, int layer, int numPhi, double rMin, double rMax, 
                              double phi0, double zMin, double zMax) {
        super(name, " ", color);

        if (detectors == null) {
            detectors = new ArrayList();
        }
        detectors.add(this);

        this.layer = layer;
        this.numPhi = numPhi;
        this.rMin = rMin;
        this.rMax = rMax;
        this.phi0 = Math.toRadians(phi0);
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
                AEndcapTRTDetector detector = (AEndcapTRTDetector) detectors.get(i);
                if (projection instanceof AProjectionRZ) {
                    coord = detector.getRZDetail();
                } else if (projection instanceof AProjectionFZ) {
                    coord = detector.getFZDetail();
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
        return ACoord.NO_DATA;
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
        return ACoord.NO_DATA;
    }
    
    /**
     * Returns the module structure in the rho-z projection.
     * @return ACoord polygons representing the modules
     */
    protected ACoord getRZDetail() {
        return ACoord.NO_DATA;
    }

    /**
     * Returns the module structure in the phi-z projection.
     * @return ACoord polygons representing the modules
     */
    protected ACoord getFZDetail() {
        double[][][] hv = new double[2][2*numPhi][2];
        for (int i=0; i<numPhi; i++) {
            hv[0][i][0] = zMin;
            hv[1][i][0] = Math.toDegrees(phi0 + i*deltaPhi);
            hv[0][i][1] = zMax;
            hv[1][i][1] = Math.toDegrees(phi0 + i*deltaPhi);
            hv[0][numPhi+i][0] = -zMax;
            hv[1][numPhi+i][0] = Math.toDegrees(phi0 + i*deltaPhi);
            hv[0][numPhi+i][1] = -zMin;
            hv[1][numPhi+i][1] = Math.toDegrees(phi0 + i*deltaPhi);
        }

        return new ACoord(hv).includePhiWrapAround("FZ");
    }
}